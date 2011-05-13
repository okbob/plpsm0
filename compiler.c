/*
 * Copyright (c) 2011 CZ.NIC, z. s. p. o.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the CZ.NIC, z. s. p. o.. The name of the CZ.NIC, z. s. p. o.
 * organization may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 */

#include "psm.h"

#include "funcapi.h"
#include "access/tupdesc.h"
#include "catalog/namespace.h"
#include "catalog/pg_proc.h"
#include "catalog/pg_proc_fn.h"
#include "catalog/pg_type.h"
#include "commands/trigger.h"
#include "nodes/bitmapset.h"
#include "tcop/tcopprot.h"
#include "utils/builtins.h"
#include "utils/plancache.h"
#include "utils/syscache.h"
#include "utils/lsyscache.h"
#include "utils/memutils.h"
#include "utils/typcache.h"

#include "parser/parse_node.h"

/* ----------
 * Lookup table for EXCEPTION condition names
 * ----------
 */
typedef struct
{
	const char *label;
	int			sqlerrstate;
} ExceptionLabelMap;

static const ExceptionLabelMap exception_label_map[] = {
#include "plerrcodes.h"
	{NULL, 0}
};

Plpsm_stmt *plpsm_parser_tree;
bool	plpsm_debug_compiler = false;
bool	plpsm_debug_info = true;

/* a context for short-term alloc during compilation */
MemoryContext plpsm_compile_tmp_cxt;

/*----
 * Hash table for compiled functions
 *----
 */
static HTAB *plpsm_HashTable = NULL;

typedef struct 
{
	Plpsm_module_hashkey hashkey;
	Plpsm_module *module;
} Plpsm_HashEnt;

#define FUNCS_PER_USER		128	/* initial table size */

#define parser_errposition(pos)		plpsm_scanner_errposition(pos)

const char *plpsm_error_funcname;

ParserState pstate = NULL;		/* parser state */

typedef struct
{							/* it's used for storing data from SQL parser */
	int	location;
	const char *name1;
	const char *name2;
	int16 offset;
} SQLHostVar;

typedef struct PreparedStatement
{							/* prepared statements has own scope inside function */
	const char *name;
	int	data;
	struct PreparedStatement *next;
} PreparedStatement;

typedef struct
{
	Plpsm_object *top_scope;
	Plpsm_object *current_scope;			/* pointer to outer compound statement */
	Plpsm_pcode_module *module;
	Plpsm_ht_table *ht_table;			/* pointer to HT table if exists */
	struct
	{
		int16 ndatums;				/* number of datums in current scope */
		struct
		{
			int size;			/* size of preallocated data for datums oids vector */
			Oid	*data;
		}			oids;
		bool	has_sqlstate;			/* true, when sqlstate variable is declared */
		bool	has_sqlcode;			/* true, when sqlcode variable is declared */
		bool	has_notfound_continue_handler;	/* true, when in current scope not found continue handler exists */
		int	ndata;				/* number of data address global		*/
		int	ht_entry;			/* entry to TH table */
		bool inside_handler;				/* true, when we are in handler */
	}			stack;
	struct						/* used as data for parsing a SQL expression */
	{
		SQLHostVar   *vars;				/* list of found host variables */
		int	nvars;				/* number of variables */
		int	maxvars;			/* size of declared array */
		bool	has_external_params;			/* true, when expr need a external parameters - detected host var. or param */
		bool	is_expression;				/* show a message about missing variables instead missing columns */
	} pdata;
	struct
	{
		int	addr1;				/* address for trigger variable initialisation */
		int	addr2;				/* address for trigger variable initialisation */
		Plpsm_object *var_new;			/* ref on NEW variable's object */
		Plpsm_object *var_old;			/* ref on OLD variable's object */
	} trigger;
	struct
	{
		int	nargs;
		char *name;
		char	*source;
		struct
		{
			struct
			{
				Oid	typoid;
				int16	typmod;
				bool	typbyval;
				int16	typlen;
			} datum;
		} result;
		char		*return_expr;		/* generated result expression for function with OUT params */
		TupleDesc	result_desc;
	} finfo;
	PreparedStatement *prepared;
	bool	use_stacked_diagnostics;		/* true when stacked diagnostics is used */
	bool	allow_return_query;			/* true, when function returns setof tuples */
} CompileStateData;

#define CURRENT_SCOPE	(cstate->current_scope)

typedef CompileStateData *CompileState;

static void _compile(CompileState cstate, Plpsm_stmt *stmt, Plpsm_object *parent);
static Node *resolve_column_ref(CompileState cstate, ColumnRef *cref);
void plpsm_parser_setup(struct ParseState *pstate, CompileState cstate);
static Plpsm_object *lookup_var(Plpsm_object *scope, const char *name);
static void compile_expr(CompileState cstate, Plpsm_ESQL *esql, const char *expr, Oid targetoid, int16 typmod, bool refresh_state_vars);

static Plpsm_module *compile(FunctionCallInfo fcinfo, HeapTuple procTup, Plpsm_module *module, 
								Plpsm_module_hashkey *hashkey, bool forValidator);
static void plpsm_compile_error_callback(void *arg);

static void compute_module_hashkey(FunctionCallInfo fcinfo,
						    Form_pg_proc procStruct,
						    Plpsm_module_hashkey *key,
						    bool forValidator);
static void delete_module(Plpsm_module *mod);
void plpsm_HashTableInit(void);
static Plpsm_module *plpsm_HashTableLookup(Plpsm_module_hashkey *func_key);
static void plpsm_HashTableInsert(Plpsm_module *module,
						Plpsm_module_hashkey *func_key);
static void plpsm_HashTableDelete(Plpsm_module *module);

static int compile_ht(CompileState cstate, Plpsm_stmt *compound, int parent_addr);

static void compile_refresh_basic_diagnostic(CompileState cstate);



/*
 * fill a array with output functions' flinfo structures
 * Array must be declared before,
 */
void 
init_out_funcs(Plpsm_object *scope, FmgrInfo *out_flinfos)
{
	Plpsm_object *iterator;

	if (scope == NULL)
		return;

	iterator = scope->inner;

	while (iterator != NULL)
	{
		if (iterator->typ == PLPSM_STMT_COMPOUND_STATEMENT || scope->typ == PLPSM_STMT_SCHEMA)
			init_out_funcs(iterator, out_flinfos);
		if (iterator->typ == PLPSM_STMT_DECLARE_VARIABLE)
		{
			Oid typoid = iterator->stmt->datum.typoid;
			int16	offset = iterator->offset;
			Oid		output_func;
			bool		is_varlena;

			getTypeOutputInfo(typoid, &output_func, &is_varlena);
			fmgr_info(output_func, &out_flinfos[offset]);
		}

		iterator = iterator->next;
	}
}

/*
 * Returns a offset of prepared statement, reserves a new offset
 * when prepered statement with the name isn't registered yet.
 */
static int
fetchPrepared(CompileState cstate, const char *name)
{
	PreparedStatement *iter = cstate->prepared;
	PreparedStatement *new;

	while (iter != NULL)
	{
		if (strcmp(iter->name, name) == 0)
			return iter->data;

		if (iter->next == NULL)
		{
			new = palloc0(sizeof(PreparedStatement));
			new->name = name;
			new->data = cstate->stack.ndata++;
			iter->next = new;
			return new->data;
		}
		else
			iter = iter->next;
	};

	new = palloc0(sizeof(PreparedStatement));
	new->name = name;
	new->data = cstate->stack.ndata++;
	cstate->prepared = new;
	return new->data;
}

/*
 * It registers a psm object to nested objects. 
 */
static void
append_object(Plpsm_object *parent, Plpsm_object *child)
{
	Assert(parent != NULL);
	Assert(child != NULL);

	child->last = child;
	child->inner = NULL;
	child->next = NULL;

	child->outer = parent;
	if (parent->inner != NULL)
	{
		Assert (parent->inner->last->next == NULL);

		parent->inner->last->next = child;
		parent->inner->last = child;
	}
	else
		parent->inner = child;
}

/*
 * Create a compiler object for any stmt statement
 * and append it to nested compiler objects.
 */
static Plpsm_object *
new_object_for(Plpsm_stmt *stmt, Plpsm_object *outer)
{
	Plpsm_object *new = palloc0(sizeof(Plpsm_object));

	if (outer != NULL)
		append_object(outer, new);

	if (stmt)
	{
		new->name = stmt->name;
		new->typ = stmt->typ;
		new->stmt = stmt;
	}

	return new;
}

/*
 * Search a label in current scope
 */
static Plpsm_object *
lookup_object_with_label_in_scope(Plpsm_object *scope, char *name)
{
	Plpsm_object *iterator;

	if (scope == NULL)
		return NULL;

	/*
	 * control flow cannot to leave a handler declaration when
	 * we assemble a condition's handler body.
	 */
	if (scope->typ == PLPSM_STMT_DECLARE_HANDLER)
		return NULL;

	iterator = scope->inner;

	while (iterator != NULL)
	{
		switch (iterator->typ)
		{
			case PLPSM_STMT_COMPOUND_STATEMENT:
			case PLPSM_STMT_LOOP:
			case PLPSM_STMT_WHILE:
			case PLPSM_STMT_REPEAT_UNTIL:
			case PLPSM_STMT_FOR:
				if (iterator->name && strcmp(iterator->stmt->name, name) == 0)
					return iterator;
				break;
			default:
				/* be compiler quite */;
		}
		iterator = iterator->next;
	}

	/* try to find label in outer scope */
	return lookup_object_with_label_in_scope(scope->outer, name);
}

static int
_lookup_condition(Plpsm_object *scope, Plpsm_condition_value *condition, bool *found)
{
	if (scope == NULL)
		return 0;

	/* only compound statement has a DECLARE CONDITIONs stmts */
	if (scope->typ == PLPSM_STMT_COMPOUND_STATEMENT)
	{
		Plpsm_object *iterator = scope->inner;

		while (iterator != NULL)
		{
			if (iterator->typ == PLPSM_STMT_DECLARE_CONDITION &&
				strcmp(iterator->name, condition->condition_name) == 0)
			{
				*found = true;
				return iterator->sqlstate;
			}
		
			iterator = iterator->next;
		}
	}

	/* go to outer scope */
	return _lookup_condition(scope->outer, condition, found);
}

/*
 * Try to search a custom condion available in scope, when it doesn't find
 * then raise a syntax error.
 */
static int
lookup_condition(Plpsm_object *scope, Plpsm_condition_value *condition)
{
	bool	found = false;
	int	sqlstate;
	int		i;

	Assert(condition->typ == PLPSM_CONDITION_NAME);
	sqlstate = _lookup_condition(scope, condition, &found);

	/* cannot to overwrite buildin conditions */
	for (i = 0; exception_label_map[i].label != NULL; i++)
	{
		if (strcmp(condition->condition_name, exception_label_map[i].label) == 0)
		{
			if (found)
				ereport(ERROR,
					(errcode(ERRCODE_SYNTAX_ERROR),
						 errmsg("condition \"%s\" is defined yet (buildin condition)", condition->condition_name),
							    parser_errposition(condition->location)));

			return exception_label_map[i].sqlerrstate;
		}
	}

	if (!found)
		ereport(ERROR,
				(errcode(ERRCODE_SYNTAX_ERROR),
				 errmsg("condition \"%s\" is not known", condition->condition_name),
					parser_errposition(condition->location)));
	return sqlstate;
}

static void
verify_condition_list(CompileState cstate, Plpsm_condition_value *condition, bool *isnotfound, Plpsm_handler_type handler_typ)
{
	while (condition != NULL)
	{
		Plpsm_condition_value *iterator = condition->next;
		Plpsm_condition_value_type typ = condition->typ;
		int sqlstate = 0;

		/* when condtion is specified by name, then we should to get derivated sqlstate */
		if (typ == PLPSM_CONDITION_NAME)
			sqlstate = condition->derivated_sqlstate;
		else if (typ == PLPSM_SQLSTATE)
			sqlstate = condition->sqlstate;

		if (*isnotfound == false && (typ == PLPSM_SQLWARNING || (typ == PLPSM_SQLSTATE && 
				ERRCODE_TO_CATEGORY(sqlstate) == MAKE_SQLSTATE('0','2','0','0','0'))))
				*isnotfound = true;

		while (iterator != NULL)
		{
			if (typ == iterator->typ && (typ == PLPSM_SQLEXCEPTION || typ == PLPSM_SQLWARNING))
				ereport(ERROR,
						(errcode(ERRCODE_SYNTAX_ERROR),
						 errmsg("condition handling is ambigonuous"),
						 parser_errposition(condition->location)));
			if (iterator->typ == typ && typ == PLPSM_SQLSTATE && sqlstate == iterator->sqlstate && sqlstate != 0)
				ereport(ERROR,
						(errcode(ERRCODE_SYNTAX_ERROR),
						 errmsg("condition handling is ambigonuous"),
						 parser_errposition(condition->location)));
			iterator = iterator->next;
		}

		/* we enable only exit or continue handlers for warnings and undo handlers for errors */
		if (!(typ == PLPSM_SQLWARNING || 
			((typ == PLPSM_SQLSTATE || PLPSM_CONDITION_NAME) && 
				(ERRCODE_TO_CATEGORY(sqlstate) == MAKE_SQLSTATE('0','2','0','0','0') ||
				 ERRCODE_TO_CATEGORY(sqlstate) == MAKE_SQLSTATE('0','1','0','0','0')))))
		{
			if (handler_typ != PLPSM_HANDLER_UNDO)
				ereport(ERROR,
						(errcode(ERRCODE_SYNTAX_ERROR),
						 errmsg("SQLEXCEPTION should be processed only UNDO handlers only"),
						 parser_errposition(condition->location)));
		}

		condition = condition->next;
	}
}

/*
 * Ensure only one sqlstate handler in compound statement
 */
static void
verify_condition_value(Plpsm_object *scope, Plpsm_condition_value *condition)
{
	Plpsm_object *iterator = scope->inner;

	while (iterator != NULL)
	{
		if (iterator->typ == PLPSM_STMT_DECLARE_HANDLER)
		{
			Plpsm_condition_value *prev_condition = (Plpsm_condition_value *) iterator->stmt->data;
			Plpsm_condition_value *c = condition;

			while (c != NULL)
			{
				Plpsm_condition_value *p = prev_condition;
				while (p != NULL)
				{

					if (p->typ == c->typ && (c->typ == PLPSM_SQLEXCEPTION || c->typ == PLPSM_SQLWARNING))
						ereport(ERROR,
								(errcode(ERRCODE_SYNTAX_ERROR),
								 errmsg("condition handling is ambigonuous"),
								 parser_errposition(c->location)));
					if (p->typ == c->typ && c->typ == PLPSM_SQLSTATE && c->sqlstate == p->sqlstate)
						ereport(ERROR,
								(errcode(ERRCODE_SYNTAX_ERROR),
								 errmsg("condition handling is ambigonuous"),
								 parser_errposition(c->location)));
					p = p->next;
				}
				c = c->next;
			}
		}

		iterator = iterator->next;
	}

	/* search outer compound statement */
	if (scope->typ != PLPSM_STMT_COMPOUND_STATEMENT)
		verify_condition_value(scope->outer, condition);
}

static Plpsm_object *
create_handler_for(Plpsm_stmt *handler_def, CompileState cstate, int handler_addr, bool *isnotfound)
{
	Plpsm_object *handler;
	Plpsm_condition_value *condition = (Plpsm_condition_value *) handler_def->data;

	verify_condition_list(cstate, condition, isnotfound, (Plpsm_handler_type) handler_def->option);
	verify_condition_value(cstate->current_scope, condition);

	handler = new_object_for(handler_def, cstate->current_scope);
	handler->calls.entry_addr = handler_addr;
	cstate->current_scope = handler;

	return handler;
}

/*
 * new condition
 */
static Plpsm_object *
create_condition_for(Plpsm_stmt *decl_stmt, CompileState cstate)
{
	Plpsm_object *iterator = cstate->current_scope->inner;
	Plpsm_condition_value *condition = (Plpsm_condition_value *) decl_stmt->data;
	char *name;
	Plpsm_object *obj;
	int		location;

	Assert(decl_stmt->target != NULL);
	name = (char *)linitial(decl_stmt->target->qualId);
	location = decl_stmt->target->location;

	Assert(cstate->current_scope->typ == PLPSM_STMT_COMPOUND_STATEMENT);
	Assert(decl_stmt->typ == PLPSM_STMT_DECLARE_CONDITION);

	while (iterator != NULL)
	{
		if (iterator->typ == PLPSM_STMT_DECLARE_CONDITION)
		{
			/* condition name should be unique */
			if (strcmp(iterator->name, name) == 0)
				ereport(ERROR,
						(errcode(ERRCODE_SYNTAX_ERROR),
						 errmsg("condition \"%s\" is defined yet", name),
						 parser_errposition(location)));

			/* when sqlstate is defined, then sould be unique */
			if (condition != NULL)
			{
				Assert(condition->typ == PLPSM_SQLSTATE);
				if (condition->sqlstate == iterator->sqlstate)
					ereport(ERROR,
							(errcode(ERRCODE_SYNTAX_ERROR),
							 errmsg("condition \"%s\" has not unique SQLSTATE", name),
							 parser_errposition(decl_stmt->location)));
			}
		}
		iterator = iterator->next;
	}

	obj = new_object_for(decl_stmt, cstate->current_scope);
	obj->name = name;
	obj->sqlstate = condition != NULL ? condition->sqlstate : 0;

	return obj;
}

/*
 * new variable
 *
 * SQL/PSM doesn't allow a sharing space between independent scopes. The reason is 
 * a concept of continue handlers, where you can go to independent scope, working
 * there with variables and then return back. Probably there should be some rules 
 * like divide a frame on handler part and routine part, but the most simple solution
 * is using a non overlaped frames. 
 */
static Plpsm_object *
create_variable_for(Plpsm_stmt *decl_stmt, CompileState cstate, Plpsm_positioned_qualid *qualid,
						char *name, Plpsm_usage_variable_type typ)
{
	Plpsm_object *iterator = cstate->current_scope->inner;
	Plpsm_object *var;
	int	location = -1;

	Assert(qualid != NULL || name != NULL);

	if (qualid != NULL)
	{
		name = (char *)linitial(qualid->qualId);
		location = qualid->location;
	}

	Assert(cstate->current_scope->typ == PLPSM_STMT_COMPOUND_STATEMENT ||
	       cstate->current_scope->typ == PLPSM_STMT_SCHEMA);
	Assert(decl_stmt->typ == PLPSM_STMT_DECLARE_VARIABLE || 
		decl_stmt->typ == PLPSM_STMT_DECLARE_CURSOR);

	while (iterator != NULL)
	{
		switch (iterator->typ)
		{
			case PLPSM_STMT_DECLARE_VARIABLE:
			case PLPSM_STMT_DECLARE_CURSOR:
				if (strcmp(iterator->name, name) == 0)
					ereport(ERROR,
							(errcode(ERRCODE_SYNTAX_ERROR),
							 errmsg("identifier \"%s\" is used yet", name),
							 parser_errposition(location)));
				break;
			default:
				/* be compiler quite */;
		}
		iterator = iterator->next;
	}
	var = new_object_for(decl_stmt, cstate->current_scope);
	/* variable uses a target, not name */
	var->name = name;

	if (typ == PLPSM_VARIABLE)
	{
		int offset = cstate->stack.ndatums++;

		if (offset >= cstate->stack.oids.size)
		{
			cstate->stack.oids.size += 128;
			cstate->stack.oids.data = repalloc(cstate->stack.oids.data, cstate->stack.oids.size * sizeof(Oid));
		}

		/* cursor has not typoid */
		if  (decl_stmt->typ == PLPSM_STMT_DECLARE_VARIABLE)
			cstate->stack.oids.data[offset] = decl_stmt->datum.typoid;
		else
		{
			cstate->stack.oids.data[offset] = InvalidOid;
			cstate->current_scope->calls.has_release_call = true;
		}

		if (decl_stmt->typ == PLPSM_STMT_DECLARE_VARIABLE)
			var->offset = offset;
		else
			var->cursor.offset = offset;
	}

	if (strcmp(name, "sqlstate") == 0)
	{
		if (cstate->stack.has_sqlstate)
			ereport(ERROR,
					(errcode(ERRCODE_SYNTAX_ERROR),
					 errmsg("SQLSTATE variable is already defined"),
					 parser_errposition(location)));
		cstate->stack.has_sqlstate = true;
		switch (decl_stmt->datum.typoid)
		{
			case TEXTOID:
			case BPCHAROID:
			case VARCHAROID:
				if (decl_stmt->datum.typmod < 9 && decl_stmt->datum.typmod != -1)
					ereport(ERROR,
							(errcode(ERRCODE_SYNTAX_ERROR),
							 errmsg("too short datatype for SQLSTATE"),
							 parser_errposition(location)));
				break;
			default:
					ereport(ERROR,
							(errcode(ERRCODE_SYNTAX_ERROR),
							 errmsg("SQLSTATE variable should be text, char or varchar"),
							 parser_errposition(location)));
		}
	}

	if (strcmp(name, "sqlcode") == 0)
	{
		if (cstate->stack.has_sqlcode)
			ereport(ERROR,
					(errcode(ERRCODE_SYNTAX_ERROR),
					 errmsg("SQLOCODE variable is already defined"),
						 parser_errposition(location)));
		cstate->stack.has_sqlcode = true;
		if (decl_stmt->datum.typoid != INT4OID)
			ereport(ERROR,
					(errcode(ERRCODE_SYNTAX_ERROR),
					 errmsg("SQLCODE variable should be integer"),
						 parser_errposition(location)));
	}

	return var;
}

/*
 * create a new psm object for psm statement
 */
static Plpsm_object *
new_psm_object_for(Plpsm_stmt *stmt, CompileState cstate, int entry_addr)
{
	Plpsm_object *new;

	switch (stmt->typ)
	{
		case PLPSM_STMT_COMPOUND_STATEMENT:
		case PLPSM_STMT_SCHEMA:
		case PLPSM_STMT_LOOP:
		case PLPSM_STMT_WHILE:
		case PLPSM_STMT_REPEAT_UNTIL:
		case PLPSM_STMT_FOR:
			if (stmt->name && lookup_object_with_label_in_scope(cstate->current_scope, stmt->name) != NULL)
				ereport(ERROR,
						(errcode(ERRCODE_SYNTAX_ERROR),
						 errmsg("label \"%s\" is defined in current scope", stmt->name),
							parser_errposition(stmt->location)));
			new = new_object_for(stmt, cstate->current_scope);
			new->calls.entry_addr = entry_addr;
			cstate->current_scope = new;
			break;
		default:
			new = new_object_for(stmt, cstate->current_scope);
			break;
	}
	
	return new;
}

/*
 * plpsm_post_column_ref - parser callback after parsing a CoumnRef.
 * In this moment we doen't allow a ambigonous identifiers.
 */
static Node *
plpsm_post_column_ref(ParseState *pstate, ColumnRef *cref, Node *var)
{
	CompileState cstate = (CompileState) pstate->p_ref_hook_state;
	Node *myvar;

	myvar = resolve_column_ref(cstate, cref);

	if (myvar != NULL && var != NULL)
	{
		/*
		 * We could leave it to the core parser to throw this error, but we
		 * can add a more useful detail message than the core could.
		 */
		ereport(ERROR,
				(errcode(ERRCODE_AMBIGUOUS_COLUMN),
				 errmsg("column reference \"%s\" is ambiguous",
							    NameListToString(cref->fields)),
				 errdetail("It could refer to either a PL/pgSQL variable or a table column."),
				 errposition(cref->location + 1)));
	}

	if (myvar == NULL && var == NULL && cstate->pdata.is_expression)
	{
		ereport(ERROR,
			(errcode(ERRCODE_INVALID_NAME),
			 errmsg("variable \"%s\" isn't available in current scope", NameListToString(cref->fields)),
					errposition(cref->location + 1)));
	}

	if (myvar != NULL)
		cstate->pdata.has_external_params = true;

	return myvar;
}

/*
 * Allow only args variables accessable via $n syntax
 */
static Node *
plpsm_paramref_hook(ParseState *pstate, ParamRef *pref)
{
	CompileState cstate = (CompileState) pstate->p_ref_hook_state;
	int	paramno = pref->number;
	Param	*param;

	if (paramno <= 0 || paramno > cstate->finfo.nargs)
		ereport(ERROR,
				(errcode(ERRCODE_UNDEFINED_PARAMETER),
				 errmsg("there is no parameter $%d", paramno),
				 errposition(pref->location + 1)));

	param = makeNode(Param);
	param->paramkind = PARAM_EXTERN;
	param->paramid = paramno;
	param->paramtype = cstate->stack.oids.data[paramno - 1];
	param->paramtypmod = -1;
	param->location = pref->location;

	cstate->pdata.has_external_params = true;

	return (Node *) param;
}

/*
 * generate a fake node for analyser
 */
static Node *
make_param(Plpsm_object *var, ColumnRef *cref)
{
	Param *param = makeNode(Param);

	param->paramkind = PARAM_EXTERN;
	param->paramid = var->offset + 1;
	param->paramtype = var->stmt->datum.typoid;
	param->paramtypmod = var->stmt->datum.typmod;
	param->location = cref->location;

	return (Node *) param;
}

/*
 * generate fake node for analyser
 */
static Node *
make_fieldSelect(Plpsm_object *var, ColumnRef *cref, const char *fieldname)
{
	TupleDesc tupdesc;
	Param *param = (Param *) make_param(var, cref);
	int16	typmod = var->stmt->datum.typmod;
	Oid	typoid = var->stmt->datum.typoid;
	int i;

	tupdesc = lookup_rowtype_tupdesc(typoid, typmod);

	for (i = 0; i < tupdesc->natts; i++)
	{
		Form_pg_attribute att = tupdesc->attrs[i];

		if (strcmp(fieldname, NameStr(att->attname)) == 0 &&
			!att->attisdropped)
		{
			/* Success, so generate a FieldSelect expression */
			FieldSelect *fselect = makeNode(FieldSelect);

			fselect->arg = (Expr *) param;
			fselect->fieldnum = i + 1;
			fselect->resulttype = att->atttypid;
			fselect->resulttypmod = att->atttypmod;
			
			ReleaseTupleDesc(tupdesc);
			return (Node *) fselect;
		}
	}

	ereport(ERROR,
			(errcode(ERRCODE_INVALID_COLUMN_REFERENCE),
			 errmsg("there are no field \"%s\" in type \"%s\"", fieldname,
									format_type_with_typemod(typoid, typmod)),
					errposition(cref->location + 1)));
	return NULL; 		/* be compiler quiet */
}

/*
 * lookup in objects
 */
static Plpsm_object *
lookup_qualified_var(Plpsm_object *scope, const char *label, const char *name)
{
	Plpsm_object *iterator;

	if (scope == NULL)
		return NULL;

	/* fast path, we are in current scope */
	if ((scope->typ == PLPSM_STMT_COMPOUND_STATEMENT || scope->typ == PLPSM_STMT_SCHEMA) && scope->name && 
				    strcmp(scope->name, label) == 0)
	{
		/* search a variable in this scope */
		iterator = scope->inner;
		while (iterator != NULL)
		{
    			if ((iterator->typ == PLPSM_STMT_DECLARE_VARIABLE ||
				iterator->typ == PLPSM_STMT_DECLARE_CURSOR) && 
				strcmp(iterator->name, name) == 0)
				return iterator;
			iterator = iterator->next;
		}
		return NULL;
	}

	return lookup_qualified_var(scope->outer, label, name);
}

/*
 * lookup variable
 */
static Plpsm_object *
lookup_var(Plpsm_object *scope, const char *name)
{
	Plpsm_object *iterator;

	if (scope == NULL)
		return NULL;

	iterator = scope->inner;

	while (iterator != NULL)
	{
		if ((iterator->typ == PLPSM_STMT_DECLARE_VARIABLE || iterator->typ == PLPSM_STMT_DECLARE_CURSOR) && strcmp(iterator->name, name) == 0)
			return iterator;
		iterator = iterator->next;
	}

	/* try to find label in outer scope */
	return lookup_var(scope->outer, name);
}


static Plpsm_object *
lookup_handler(Plpsm_object *scope, Plpsm_condition_value *condition)
{
	Plpsm_object *iterator;
	Plpsm_condition_value *c;

	Assert(condition->typ == PLPSM_SQLSTATE || condition->typ == PLPSM_CONDITION_NAME);

	if (scope == NULL)
		return NULL;

	/* if current scope is handler, go up two outer scopes */
	if (scope->typ == PLPSM_STMT_DECLARE_HANDLER)
		return lookup_handler(scope->outer->outer, condition);

	if (scope->typ == PLPSM_STMT_COMPOUND_STATEMENT)
	{
		iterator = scope->inner;
		while (iterator != NULL)
		{
			if (iterator->typ == PLPSM_STMT_DECLARE_HANDLER)
			{
				c = (Plpsm_condition_value *) iterator->stmt->data;

				/* search a exact match */
				while (c != NULL)
				{
					if (condition->typ == PLPSM_SQLSTATE)
					{
						if (c->typ == PLPSM_SQLSTATE && condition->sqlstate == c->sqlstate)
							return iterator;
						if (c->typ == PLPSM_CONDITION_NAME && condition->sqlstate == c->derivated_sqlstate)
							return iterator;
					}
					else
					{
						Assert(condition->typ == PLPSM_CONDITION_NAME);
						if (c->typ == PLPSM_CONDITION_NAME &&
							strcmp(c->condition_name, condition->condition_name) == 0)
							return iterator;
					}

					c = c->next;
				}
			}
			iterator = iterator->next;
		}

		iterator = scope->inner;
		while (iterator != NULL)
		{
			if (iterator->typ == PLPSM_STMT_DECLARE_HANDLER)
			{
				/* search a category match */
				c = (Plpsm_condition_value *) iterator->stmt->data;
				while (c != NULL)
				{
					if (c->typ == PLPSM_SQLSTATE && ERRCODE_IS_CATEGORY(c->sqlstate) &&
						ERRCODE_TO_CATEGORY(condition->sqlstate) == c->sqlstate)
						return iterator;
					if (c->typ == PLPSM_CONDITION_NAME && c->derivated_sqlstate != 0 &&
						ERRCODE_IS_CATEGORY(c->derivated_sqlstate) &&
						ERRCODE_TO_CATEGORY(c->derivated_sqlstate) == c->sqlstate)
						return iterator;
					c = c->next;
				}
			}
			iterator = iterator->next;
		}

		iterator = scope->inner;
		while (iterator != NULL)
		{
			if (iterator->typ == PLPSM_STMT_DECLARE_HANDLER)
			{
				/* search a generic match */
				c = (Plpsm_condition_value *) iterator->stmt->data;
				while (c != NULL)
				{
					if (c->typ == PLPSM_SQLWARNING && 
						(ERRCODE_TO_CATEGORY(condition->sqlstate) == MAKE_SQLSTATE('0','2','0','0','0') ||
						 ERRCODE_TO_CATEGORY(condition->sqlstate) == MAKE_SQLSTATE('0','1','0','0','0')))
						 return iterator;

					if (c->typ == PLPSM_SQLEXCEPTION &&
						ERRCODE_TO_CATEGORY(condition->sqlstate) != MAKE_SQLSTATE('0','2','0','0','0') &&
						ERRCODE_TO_CATEGORY(condition->sqlstate) != MAKE_SQLSTATE('0','1','0','0','0') &&
						ERRCODE_TO_CATEGORY(condition->sqlstate) != MAKE_SQLSTATE('0','0','0','0','0'))
						return iterator;
					c = c->next;
				}
			}
			iterator = iterator->next;
		}
	}

	return lookup_handler(scope->outer, condition);
}

/*
 * lookup a not found continue handler
 */
static Plpsm_object *
lookup_notfound_continue_handler(Plpsm_object *scope)
{
	Plpsm_condition_value condition;

	if (scope == NULL)
		return NULL;

	condition.typ = PLPSM_SQLSTATE;
	condition.sqlstate = MAKE_SQLSTATE('0','2','0','0','0');

	return lookup_handler(scope, &condition);
}

/*
 * Assign info about Object reference to state 
 */
static void
appendHostVar(CompileState cstate, int location, const char *name1, const char *name2, int offset)
{
	SQLHostVar *var;

	if (cstate->pdata.nvars == cstate->pdata.maxvars)
	{
		cstate->pdata.maxvars += 128;
		cstate->pdata.vars = repalloc(cstate->pdata.vars, cstate->pdata.maxvars * sizeof(SQLHostVar));
	}

	var = &cstate->pdata.vars[cstate->pdata.nvars++];
	var->location = location;
	var->name1 = name1;
	var->name2 = name2;
	var->offset = offset;
} 

/*
 * Try to find a variable related to ColumnRef names
 *
 * Possible combinations:
 *
 * A          Variable
 * A.B        Qualified variable, field reference
 * A.B.C      Qualified record reference
 */
static Node *
resolve_column_ref(CompileState cstate, ColumnRef *cref)
{
	const char *name1;
	const char *name2 = NULL;
	const char *name3 = NULL;
	Plpsm_object *var;

	switch (list_length(cref->fields))
	{
		case 1:
			{
				Node	*field1 = (Node *) linitial(cref->fields);
				Assert(IsA(field1, String));
				name1 = strVal(field1);
				var = lookup_var(cstate->current_scope, name1);
				if (var != NULL)
				{
					appendHostVar(cstate, cref->location, name1, NULL, var->offset + 1);
					return make_param(var, cref);
				}
				break;
			}
		case 2:
			{
				/* 
				 * if we found a label A with variable B and variable A,
				 * then we have to raise a error, because there are not
				 * clean what situation is: label.var or record.field
				 */
				Node	*field1 = (Node *) linitial(cref->fields);
				Node	*field2 = (Node *) lsecond(cref->fields);
				Assert(IsA(field1, String));
				name1 = strVal(field1);
				Assert(IsA(field2, String));
				name2 = strVal(field2);

				var = lookup_qualified_var(cstate->current_scope, name1, name2);
				if (var != NULL)
				{
					if (lookup_var(cstate->current_scope, name1))
						ereport(ERROR,
								(errcode(ERRCODE_AMBIGUOUS_COLUMN),
								 errmsg("there is conflict between compound statement label and composite variable"),
								 parser_errposition(cref->location)));

					appendHostVar(cstate, cref->location, name1, name2, var->offset + 1);
					return make_param(var, cref);
				}
				else
				{
					var = lookup_var(cstate->current_scope, name1);
					if (var != NULL)
					{
						appendHostVar(cstate, cref->location, name1, NULL, var->offset + 1);
						return make_fieldSelect(var, cref, name2);
					}
				}
			}
		case 3:
			{
				Node	*field1 = (Node *) linitial(cref->fields);
				Node	*field2 = (Node *) lsecond(cref->fields);
				Node	*field3 = (Node *) lsecond(cref->fields);


				Assert(IsA(field1, String));
				name1 = strVal(field1);
				Assert(IsA(field2, String));
				name2 = strVal(field2);
				Assert(IsA(field3, String));
				name3 = strVal(field3);

				var = lookup_qualified_var(cstate->current_scope, name1, name2);
				if (var != NULL)
				{
					appendHostVar(cstate, cref->location, name1, name2, var->offset + 1);
					return make_fieldSelect(var, cref, name3);
				}
			}
	}

	return NULL;
}

/*
 * returns a target objects for assign statement. When target is composite type with 
 * specified field, then fieldname is filled.
 */
static Plpsm_object *
resolve_target(CompileState cstate, Plpsm_positioned_qualid *target, const char **fieldname, int target_type)
{
	const char *name1;
	const char *name2 = NULL;
	Plpsm_object *var;
	const char *objectclass;

	*fieldname = NULL;

	switch (target_type)
	{
		case PLPSM_STMT_DECLARE_CURSOR:
			objectclass= "cursor";
			break;
		case PLPSM_STMT_DECLARE_VARIABLE:
			objectclass = "target variable";
			break;
		default:
			objectclass = "identifier";
			break;
	}

	switch (list_length(target->qualId))
	{
		case 1:
			{
				name1 = (const char *) linitial(target->qualId);
				var = lookup_var(cstate->current_scope, name1);
				if (var == NULL)
					ereport(ERROR,
							(errcode(ERRCODE_SYNTAX_ERROR),
							 errmsg("a %s \"%s\" is undefined", objectclass, name1),
							 parser_errposition(target->location)));
				return var;
			}
		case 2:
			{
				/* 
				 * if we found a label A with variable B and variable A,
				 * then we have to raise a error, because there are not
				 * clean what situation is: label.var or record.field
				 */
				name1 = (const char *) linitial(target->qualId);
				name2 = (const char *) lsecond(target->qualId);

				var = lookup_qualified_var(cstate->current_scope, name1, name2);
				if (var != NULL)
				{
					if (lookup_var(cstate->current_scope, name1))
						ereport(ERROR,
							(errcode(ERRCODE_SYNTAX_ERROR),
							 errmsg("there is conflict between compound statement label and composite variable"),
							 parser_errposition(target->location)));
					return var;
				}
				else
				{
					var = lookup_var(cstate->current_scope, name1);
					if (var == NULL)
						ereport(ERROR,
							(errcode(ERRCODE_SYNTAX_ERROR),
							 errmsg("%s \"%s\" is undefined", objectclass, name1),
							 parser_errposition(target->location)));
					*fieldname = name2;
					return var;
				}
			}
		case 3:
			{
				const char *name3;

				name1 = (const char *) linitial(target->qualId); 
				name2 = (const char *) lsecond(target->qualId);
				name3 = (const char *) lthird(target->qualId);

				var = lookup_qualified_var(cstate->current_scope, name1, name2);
				if (var == NULL)
					ereport(ERROR,
						(errcode(ERRCODE_SYNTAX_ERROR),
						 errmsg("%s \"%s\" with label \"%s\" is undefined", objectclass, name2, name1),
						 parser_errposition(target->location)));
				*fieldname = name3;
			}
	}

	return NULL;
}

/*
 * Create a extensible module
 */
static Plpsm_pcode_module *
init_module(void)
{
	Plpsm_pcode_module *m = palloc0(1024 * sizeof(Plpsm_pcode) + offsetof(Plpsm_pcode_module, code));
	m->mlength = 1024;
	m->length = 1;
	return m;
}

#define VALUE(x)	m->code[pc].x

static void
list_ht_table(Plpsm_ht_table *m)
{
	StringInfoData ds;
	int pc;

	initStringInfo(&ds);

	if (m == NULL)
		return;

	for (pc = 1; pc < m->length; pc++)
	{
		appendStringInfo(&ds, "%5d", pc);
		appendStringInfoChar(&ds, '\t');

		Assert(VALUE(typ) == PCODE_HT);

		appendStringInfoString(&ds, "HT ");
		if (VALUE(HT_field.typ) != PLPSM_HT_PARENT && 
				VALUE(HT_field.typ != PLPSM_HT_STOP) && VALUE(HT_field.typ != PLPSM_HT_RELEASE_SUBTRANSACTION) &&
				VALUE(HT_field.typ != PLPSM_HT_DIAGNOSTICS_POP))
		{
			switch (VALUE(HT_field.htyp))
			{
				case PLPSM_HANDLER_CONTINUE:
					appendStringInfoString(&ds, "CONTINUE ");
					break;
				case PLPSM_HANDLER_EXIT:
					appendStringInfoString(&ds, "EXIT ");
					break;
				case PLPSM_HANDLER_UNDO:
					appendStringInfoString(&ds, "UNDO ");
					break;
			}
		}

		switch (VALUE(HT_field.typ))
		{
			case PLPSM_HT_SQLCODE:
				appendStringInfo(&ds, "SQLCODE: %s, addr:%d", 
							unpack_sql_state(VALUE(HT_field.sqlcode)),
							VALUE(HT_field.addr));
				break;
			case PLPSM_HT_CONDITION_NAME:
				appendStringInfo(&ds, "CONDITION_NAME: %s, addr:%d",
							VALUE(HT_field.condition_name),
							VALUE(HT_field.addr));
				break;
			case PLPSM_HT_SQLCLASS:
				appendStringInfo(&ds, "SQLCLASS: %s, addr:%d", 
							unpack_sql_state(VALUE(HT_field.sqlclass)),
							VALUE(HT_field.addr));
				break;						
			case PLPSM_HT_SQLWARNING:
				appendStringInfo(&ds, "SQLWARNING, addr:%d", 
							VALUE(HT_field.addr));
				break;						
			case PLPSM_HT_SQLEXCEPTION:
				appendStringInfo(&ds, "SQLEXCEPTION, addr:%d", 
							VALUE(HT_field.addr));
				break;						
			case PLPSM_HT_PARENT:
				appendStringInfo(&ds, "Parent tab: %d", 
							VALUE(HT_field.parent_HT_addr));
				break;
			case PLPSM_HT_RELEASE_SUBTRANSACTION:
				appendStringInfo(&ds, "Release Subtransaction");
				break;
			case PLPSM_HT_DIAGNOSTICS_POP:
				appendStringInfo(&ds, "Diagnostics Pop");
				break;
			case PLPSM_HT_STOP:
				appendStringInfo(&ds, "STOP");
				break;
		}
		
		appendStringInfoChar(&ds, '\n');
	}

	elog(NOTICE, "Handler table:\n\n%s", ds.data);
	pfree(ds.data);
}

static void 
list(Plpsm_pcode_module *m)
{
	StringInfoData ds;
	int pc;

	initStringInfo(&ds);

	appendStringInfo(&ds, "\n   Datums: %d variable(s) \n", m->ndatums);
	appendStringInfo(&ds, "   Local data size: %d pointers\n", m->ndata);
	appendStringInfo(&ds, "   Size: %d instruction(s)\n", m->length - 1);

	for (pc = 1; pc < m->length; pc++)
	{
		appendStringInfo(&ds, "%5d", pc);
		appendStringInfoChar(&ds, '\t');

		switch (VALUE(typ))
		{
			case PCODE_JMP_FALSE_UNKNOWN:
				appendStringInfo(&ds, "Jmp_false_unknown %d", VALUE(addr));
				break;
			case PCODE_JMP:
				appendStringInfo(&ds, "Jmp %d", VALUE(addr));
				break;
			case PCODE_JMP_NOT_FOUND:
				appendStringInfo(&ds, "Jmp_not_found %d", VALUE(addr));
				break;
			case PCODE_CALL:
				appendStringInfo(&ds, "Call %d", VALUE(addr));
				break;
			case PCODE_CALL_NOT_FOUND:
				appendStringInfo(&ds, "Call_not_found %d", VALUE(addr));
				break;
			case PCODE_RETURN:
				appendStringInfo(&ds, "Return size:%d, byval:%s", VALUE(target.typlen),
											VALUE(target.typbyval) ? "BYVAL" : "BYREF");
				break;
			case PCODE_EXEC_EXPR:
				{
					StringInfoData ds2;
					int	i;
					initStringInfo(&ds2);
					for (i = 0; i < VALUE(expr.nparams); i++)
					{
						if (i > 0)
							appendStringInfoChar(&ds2, ',');
						appendStringInfo(&ds2,"%d", VALUE(expr.typoids[i]));
					}
					
					appendStringInfo(&ds, "ExecExpr \"%s\",{%s}, data[%d]", VALUE(expr.expr),ds2.data,
											VALUE(expr.data));
					pfree(ds2.data);
				}
				break;
			case PCODE_RETURN_QUERY:
				{
					StringInfoData ds2;
					int	i;
					initStringInfo(&ds2);
					for (i = 0; i < VALUE(expr.nparams); i++)
					{
						if (i > 0)
							appendStringInfoChar(&ds2, ',');
						appendStringInfo(&ds2,"%d", VALUE(expr.typoids[i]));
					}
					
					appendStringInfo(&ds, "Return Query \"%s\",{%s}, data[%d]", VALUE(expr.expr),ds2.data,
											VALUE(expr.data));
					pfree(ds2.data);
				}
				break;
			case PCODE_EXEC_QUERY:
				{
					StringInfoData ds2;
					int	i;
					initStringInfo(&ds2);
					for (i = 0; i < VALUE(expr.nparams); i++)
					{
						if (i > 0)
							appendStringInfoChar(&ds2, ',');
						appendStringInfo(&ds2,"%d", VALUE(expr.typoids[i]));
					}
					
					appendStringInfo(&ds, "ExecQuery \"%s\",{%s}, data[%d]", VALUE(expr.expr),ds2.data,
											VALUE(expr.data));
					pfree(ds2.data);
				}
				break;
			case PCODE_DATA_QUERY:
				{
					StringInfoData ds2;
					int	i;
					initStringInfo(&ds2);
					for (i = 0; i < VALUE(expr.nparams); i++)
					{
						if (i > 0)
							appendStringInfoChar(&ds2, ',');
						appendStringInfo(&ds2,"%d", VALUE(expr.typoids[i]));
					}
					
					appendStringInfo(&ds, "Data \"%s\",{%s}, data[%d]", VALUE(expr.expr),ds2.data,
											VALUE(expr.data));
					pfree(ds2.data);
				}
				break;
			case PCODE_PRINT:
				appendStringInfo(&ds, "Print");
				break;
			case PCODE_DONE:
				appendStringInfoString(&ds, "Done.");
				break;
			case PCODE_SET_NULL:
				appendStringInfo(&ds, "SetNull @%d", VALUE(target.offset));
				break;
			case PCODE_SAVETO:
				appendStringInfo(&ds, "SaveTo @%d, size:%d, byval:%s", VALUE(target.offset), VALUE(target.typlen),
											VALUE(target.typbyval) ? "BYVAL" : "BYREF");
				break;
			case PCODE_INIT_TRIGGER_VAR:
				appendStringInfo(&ds, "InitTriggerVar %s @%d, size:%d, byval:%s oid:%d typmod %d", 
																VALUE(trigger_var.typ) == PLPSM_TRIGGER_VARIABLE_NEW ? "NEW" : "OLD",
											VALUE(trigger_var.offset),
											VALUE(trigger_var.typlen),
											VALUE(trigger_var.typbyval) ? "BYVAL" : "BYREF",
											VALUE(trigger_var.typoid), 
											VALUE(trigger_var.typmod));
				break;
			case PCODE_SAVETO_FIELD:
				appendStringInfo(&ds, "SaveToField #%d @%d, data[%d] size:%d, byval:%s oid:%d typmod %d", VALUE(saveto_field.fnumber), VALUE(saveto_field.offset),
											VALUE(saveto_field.data),
											VALUE(saveto_field.typlen),
											VALUE(saveto_field.typbyval) ? "BYVAL" : "BYREF",
											VALUE(saveto_field.typoid), 
											VALUE(saveto_field.typmod));
				break;
			case PCODE_UPDATE_FIELD:
				appendStringInfo(&ds, "UpdateToField #%d @%d.%d, oid:%d typmod %d", VALUE(update_field.fnumber), VALUE(update_field.offset), VALUE(update_field.fno),
											VALUE(update_field.typoid), 
											VALUE(update_field.typmod));
				break;
			case PCODE_ARRAY_UPDATE:
				appendStringInfo(&ds, "ArrayUpdate @%d, len:%d, elemlen: %d, elembyval:%d, elemalign:%c. elemoid:%d",
											VALUE(array_update.offset),
											VALUE(array_update.arraytyplen),
											VALUE(array_update.elemtyplen),
											VALUE(array_update.elemtypbyval),
											VALUE(array_update.elemtypalign),
											VALUE(array_update.arrayelemtypid));
				break;
			case PCODE_COPY_PARAM:
				appendStringInfo(&ds, "CopyParam %d, @%d, size:%d, byval:%s", 
											VALUE(copyto.src),
											VALUE(copyto.dest),
											VALUE(copyto.typlen),
											VALUE(copyto.typbyval) ? "BYVAL" : "BYREF");
				break;
			case PCODE_CURSOR_OPEN:
				appendStringInfo(&ds, "OpenCursor @%d, ^%d name:%s", VALUE(cursor.offset),
											VALUE(cursor.addr),
											VALUE(cursor.name));
				break;
			case PCODE_CURSOR_OPEN_DYNAMIC:
				appendStringInfo(&ds, "OpenCursor @%d, ^%d name:%s", VALUE(cursor.offset),
											VALUE(cursor.addr),
											VALUE(cursor.name));
				break;
			case PCODE_CURSOR_CLOSE:
				appendStringInfo(&ds, "CloseCursor @%d, ^%d name:%s", VALUE(cursor.offset), 
											VALUE(cursor.addr),
											VALUE(cursor.name));
				break;
			case PCODE_CURSOR_RELEASE:
				appendStringInfo(&ds, "ReleaseCursor @%d, ^%d name:%s", VALUE(cursor.offset), 
											VALUE(cursor.addr),
											VALUE(cursor.name));
				break;
			case PCODE_CURSOR_FETCH:
				appendStringInfo(&ds, "Fetch @%d, count:%d name: %s", VALUE(fetch.offset), 
												VALUE(fetch.count),
												VALUE(fetch.name));
				break;
			case PCODE_RETURN_NULL:
				appendStringInfo(&ds, "Return NULL");
				break;
			case PCODE_RET_SUBR:
				appendStringInfo(&ds, "RetSubr");
				break;
			case PCODE_RETURN_VOID:
				appendStringInfo(&ds, "Return 0");
				break;
			case PCODE_SIGNAL_NODATA:
				appendStringInfo(&ds, "Signal NODATA, \"%s\"", VALUE(str));
				break;
			case PCODE_SQLCODE_REFRESH:
				appendStringInfo(&ds, "RefreshSQLCODE @%d", VALUE(target.offset));
				break;
			case PCODE_SQLSTATE_REFRESH:
				appendStringInfo(&ds, "RefreshSQLSTATE @%d", VALUE(target.offset));
				break;
			case PCODE_STRBUILDER:
				appendStringInfo(&ds, "StringBuilder data[%d] op:%d", VALUE(strbuilder.data),
											VALUE(strbuilder.op));
				break;
			case PCODE_CHECK_DATA:
				appendStringInfo(&ds, "CheckData ncolums:%d", VALUE(ncolumns));
				break;
			case PCODE_EXECUTE_IMMEDIATE:
				appendStringInfo(&ds, "ExecImmediate");
				break;
			case PCODE_PREPARE:
				appendStringInfo(&ds, "Prepare name:%s data[%d]", VALUE(prepare.name), VALUE(prepare.data));
				break;
			case PCODE_PARAMBUILDER:
				appendStringInfo(&ds, "Parambuilder data[%d] op:%d", VALUE(parambuilder.data), 
											VALUE(parambuilder.op));
				break;
			case PCODE_EXECUTE:
				appendStringInfo(&ds, "Execute sqlstr[%d] params[%d]", VALUE(execute.sqlstr), 
											VALUE(execute.params));
				break;
			case PCODE_DEBUG_SOURCE_CODE:
				appendStringInfo(&ds, "Source code attached");
				break;
			case PCODE_STORE_SP:
				appendStringInfo(&ds, "StoreSP @%d", VALUE(target.offset));
				break;
			case PCODE_LOAD_SP:
				appendStringInfo(&ds, "LoadSP @%d", VALUE(target.offset));
				break;
			case PCODE_BEGIN_SUBTRANSACTION:
				appendStringInfo(&ds, "BeginSubtransaction");
				break;
			case PCODE_RELEASE_SUBTRANSACTION:
				appendStringInfo(&ds, "ReleaseSubtransaction");
				break;
			case PCODE_SIGNAL_JMP:
				appendStringInfo(&ds, "Signal Jmp %d", VALUE(signal_params.addr));
				break;
			case PCODE_SIGNAL_CALL:
				appendStringInfo(&ds, "Signal Call %d", VALUE(signal_params.addr));
				break;
			case PCODE_RESIGNAL_JMP:
				appendStringInfo(&ds, "Resignal Jmp %d", VALUE(signal_params.addr));
				break;
			case PCODE_RESIGNAL_CALL:
				appendStringInfo(&ds, "Resignal Call %d", VALUE(signal_params.addr));
				break;
			case PCODE_SET_SQLSTATE:
				appendStringInfo(&ds, "Set SQLSTATE '%s'", unpack_sql_state(VALUE(sqlstate)));
				break;
			case PCODE_DIAGNOSTICS_INIT:
				appendStringInfo(&ds, "Diagnostics Init stacked:%d", VALUE(use_stacked_diagnostics));
				break;
			case PCODE_DIAGNOSTICS_PUSH:
				appendStringInfo(&ds, "Diagnostics Push");
				break;
			case PCODE_DIAGNOSTICS_POP:
				appendStringInfo(&ds, "Diagnostics Pop");
				break;
			case PCODE_SUBSCRIPTS_RESET:
				appendStringInfo(&ds, "Subscripts Reset");
				break;
			case PCODE_SUBSCRIPTS_APPEND:
				appendStringInfo(&ds, "Subscripts Append");
				break;
			case PCODE_INIT_TUPLESTORE:
				appendStringInfo(&ds, "Tuplestore Init");
				break;
			case PCODE_SIGNAL_PROPERTY:
				{
					appendStringInfoString(&ds, "Signal Property ");
					switch (VALUE(signal_property.typ))
					{
						case PLPSM_SIGNAL_PROPERTY_RESET:
							appendStringInfoString(&ds, "Reset");
							break;
						case PLPSM_SIGNAL_PROPERTY_LOAD_STACKED:
							appendStringInfoString(&ds, "Load Stacked");
							break;
						case PLPSM_SIGNAL_PROPERTY_SET_INT:
						case PLPSM_SIGNAL_PROPERTY_SET_CSTRING:
							switch (VALUE(signal_property.gdtyp))
							{
								case PLPSM_GDINFO_MESSAGE:
									appendStringInfo(&ds, "MESSAGE_TEXT = %s", 
										VALUE(signal_property.cstr));
									break;
								case PLPSM_GDINFO_DETAIL:
								appendStringInfo(&ds, "DETAIL_TEXT = %s", 
										VALUE(signal_property.cstr));
									break;
								case PLPSM_GDINFO_HINT:
									appendStringInfo(&ds, "HINT_TEXT = %s", 
										VALUE(signal_property.cstr));
									break;
								case PLPSM_GDINFO_SQLCODE:
									appendStringInfo(&ds, "SQLCODE = %d", 
										VALUE(signal_property.ival));
									break;
								case PLPSM_GDINFO_LEVEL:
									appendStringInfo(&ds, "LEVEL = %d", 
										VALUE(signal_property.ival));
									break;
								case PLPSM_GDINFO_CONDITION_IDENTIFIER:
									appendStringInfo(&ds, "CONDITION_IDENTIFIER = %s", 
										VALUE(signal_property.cstr));
								default:
									/* be compiler quite */;
							}
							break;
						case PLPSM_SIGNAL_PROPERTY_COPY_TEXT_VAR:
							switch (VALUE(signal_property.gdtyp))
							{
								case PLPSM_GDINFO_MESSAGE:
									appendStringInfo(&ds, "MESSAGE_TEXT = @%d", 
										VALUE(signal_property.offset));
									break;
								case PLPSM_GDINFO_DETAIL:
									appendStringInfo(&ds, "DETAIL_TEXT = @%d", 
										VALUE(signal_property.offset));
									break;
								case PLPSM_GDINFO_HINT:
									appendStringInfo(&ds, "HINT_TEXT = @%d", 
										VALUE(signal_property.offset));
									break;
								default:
									/* be compiler quite */;
							}
							break;
					}
					break;
				}
			case PCODE_GET_DIAGNOSTICS:
				appendStringInfo(&ds, "Get %s Diagnostics @%d oid:%d byval:%s = ",
							VALUE(get_diagnostics.which_area) == PLPSM_GDAREA_CURRENT ? "CURRENT" : "STACKED",
							VALUE(get_diagnostics.offset),
							VALUE(get_diagnostics.target_type),
							VALUE(get_diagnostics.byval) ? "BYVAL" : "BYREF");
				switch (VALUE(get_diagnostics.typ))
				{
					case PLPSM_GDINFO_DETAIL:
						appendStringInfoString(&ds, "DETAIL_TEXT");
						break;
					case PLPSM_GDINFO_HINT:
						appendStringInfoString(&ds, "HINT_TEXT");
						break;
					case PLPSM_GDINFO_MESSAGE:
						appendStringInfoString(&ds, "MESSAGE_TEXT");
						break;
					case PLPSM_GDINFO_SQLSTATE:
						appendStringInfoString(&ds, "RETURNED_SQLSTATE");
						break;
					case PLPSM_GDINFO_SQLCODE:
						appendStringInfoString(&ds, "RETURNED_SQLCODE");
						break;
					case PLPSM_GDINFO_ROW_COUNT:
						appendStringInfoString(&ds, "ROW_COUNT");
						break;
					case PLPSM_GDINFO_CONDITION_IDENTIFIER:
						appendStringInfoString(&ds, "CONDITION_IDENTIFIER");
						break;
					default:
						/* be compiler quite */;
				}
				break;
		}

		if (VALUE(htnum) != 0)
		{
			switch (VALUE(typ))
			{
				case PCODE_EXEC_EXPR:
				case PCODE_EXEC_QUERY:
				case PCODE_EXECUTE_IMMEDIATE:
				case PCODE_PREPARE:
				case PCODE_PARAMBUILDER:
				case PCODE_EXECUTE:
				case PCODE_SIGNAL_JMP:
				case PCODE_SIGNAL_CALL:
				case PCODE_RESIGNAL_JMP:
				case PCODE_RESIGNAL_CALL:
					appendStringInfo(&ds, ", HT[%d]", VALUE(htnum));
			}
		}
		
		appendStringInfoChar(&ds, '\n');
	}
	elog(NOTICE, "\n%s", ds.data);
	pfree(ds.data);
}

static Plpsm_pcode_module *
check_module_size(Plpsm_pcode_module *m)
{
	if (m->length == m->mlength)
	{
		Plpsm_pcode_module *new;
		int length = m->mlength + 1024;
		
		new = repalloc(m, length * sizeof(Plpsm_pcode) + offsetof(Plpsm_pcode_module, code));
		new->length = length;
		return new;
	}
	return m;
}

static void 
check_ht_table_size(Plpsm_ht_table *m)
{
	if (m->length == 129)
		elog(ERROR, "handler's table is full");
}

#define PC(m)				m->length

#define SET_OPVAL(n, v)			m->code[m->length].n = v
#define EMIT_OPCODE(t, lno)		do { \
						m->code[m->length].lineno = lno; \
						m->code[m->length].cframe = plpsm_debug_info ? CURRENT_SCOPE : NULL; \
						m->code[m->length].htnum = cstate->stack.ht_entry; \
						m->code[m->length++].typ = PCODE_ ## t; \
						m = check_module_size(m); \
					} while (0)
#define SET_OPVAL_ADDR(a,n,v)		m->code[a].n = v
#define SET_OPVALS_DATUM_INFO(mi,var)	do { \
						SET_OPVAL(mi.offset, var->offset); \
						SET_OPVAL(mi.typlen, var->stmt->datum.typlen); \
						SET_OPVAL(mi.typbyval, var->stmt->datum.typbyval); \
						SET_OPVAL(mi.typoid, var->stmt->datum.typoid); \
						SET_OPVAL(mi.typmod, var->stmt->datum.typmod); \
					} while (0)
#define SET_OPVALS_DATUM_COPY(mi,var)	do { \
						SET_OPVAL(mi.offset, var->offset); \
						SET_OPVAL(mi.typlen, var->stmt->datum.typlen); \
						SET_OPVAL(mi.typbyval, var->stmt->datum.typbyval); \
					} while (0)
#define EMIT_JMP(a, lno)	do { \
						SET_OPVAL(addr, a); \
						EMIT_OPCODE(JMP, lno); \
					} while (0)
#define EMIT_CALL(a, lno)	do { \
						SET_OPVAL(addr, a); \
						EMIT_OPCODE(CALL, lno); \
					} while (0)
#define PARAMBUILDER(o,d)		do { \
						SET_OPVAL(parambuilder.data, d); \
						SET_OPVAL(parambuilder.op, PLPSM_PARAMBUILDER_ ## o); \
						EMIT_OPCODE(PARAMBUILDER, -1); \
					} while (0)
#define PARAMBUILDER1(o,d, p,v)		do { \
						SET_OPVAL(parambuilder.data, d); \
						SET_OPVAL(parambuilder.op, PLPSM_PARAMBUILDER_ ## o); \
						SET_OPVAL(parambuilder.p, v); \
						EMIT_OPCODE(PARAMBUILDER, -1); \
					} while (0)
#define STRBUILDER(o,d)		do { \
						SET_OPVAL(strbuilder.data, d); \
						SET_OPVAL(strbuilder.op, PLPSM_STRBUILDER_ ## o); \
						EMIT_OPCODE(STRBUILDER, -1); \
					} while (0)
#define STRBUILDER1(o,d, p,v)		do { \
						SET_OPVAL(strbuilder.data, d); \
						SET_OPVAL(strbuilder.op, PLPSM_STRBUILDER_ ## o); \
						SET_OPVAL(strbuilder.p, v); \
						EMIT_OPCODE(STRBUILDER, -1); \
					} while (0)
#define SIGNAL_PROPERTY(t, gdt, f, v)	do { \
						SET_OPVAL(signal_property.typ, PLPSM_SIGNAL_PROPERTY_ ## t); \
						SET_OPVAL(signal_property.gdtyp, PLPSM_GDINFO_ ## gdt); \
						SET_OPVAL(signal_property.f, v); \
						EMIT_OPCODE(SIGNAL_PROPERTY, -1); \
					} while (0);

#define EMIT_HT_FIELD(t, lno)		do { \
						SET_OPVAL(HT_field.typ, PLPSM_HT_ ## t); \
						m->code[m->length].lineno = lno; \
						m->code[m->length++].typ = PCODE_HT; \
						check_ht_table_size(m); \
					} while (0)


static void
compile_release_cursors(CompileState cstate)
{
	Plpsm_pcode_module *m = cstate->module;
	Plpsm_object *scope = cstate->current_scope;
	Plpsm_object *iterator;

	/* search all cursors from current compound statement */
	if (scope->typ != PLPSM_STMT_COMPOUND_STATEMENT &&
		scope->typ != PLPSM_STMT_SCHEMA)
		return;

	iterator = scope->inner;
	while (iterator != NULL)
	{
		if (iterator->typ == PLPSM_STMT_DECLARE_CURSOR)
		{
			SET_OPVAL(cursor.name, iterator->name);
			SET_OPVAL(cursor.addr, iterator->cursor.data_addr);
			SET_OPVAL(cursor.offset, iterator->cursor.offset);
			EMIT_OPCODE(CURSOR_RELEASE, -1);
		}
		iterator = iterator->next;
	}
}

/*
 * Helps with exit or undo handler calls 
 */
static void
compile_leave_target_block(CompileState cstate, Plpsm_object *scope, Plpsm_object *target_scope)
{
	Plpsm_pcode_module *m = cstate->module;

	if (scope == NULL)
		return;

	if (scope != target_scope)
	{
		if (scope->calls.has_release_call)
		{
			scope->calls.release_calls = lappend(scope->calls.release_calls,
										    makeInteger(PC(m)));
			EMIT_OPCODE(CALL, -1);
		}

		compile_leave_target_block(cstate, scope->outer, target_scope);
	}
}

/*
 * helps with LEAVE, ITERATE statement compilation
 *
 */
static void
compile_leave_iterate(CompileState cstate, 
						Plpsm_object *scope, Plpsm_stmt *stmt)
{
	Plpsm_pcode_module *m = cstate->module;

	Assert(stmt->typ == PLPSM_STMT_LEAVE || stmt->typ == PLPSM_STMT_ITERATE);

	if (scope == NULL)
		return;

	switch (scope->typ)
	{
		case PLPSM_STMT_COMPOUND_STATEMENT:
		case PLPSM_STMT_LOOP:
		case PLPSM_STMT_WHILE:
		case PLPSM_STMT_REPEAT_UNTIL:
		case PLPSM_STMT_FOR:
			{
				if (scope->name && strcmp(scope->name, stmt->name) == 0)
				{
					if (stmt->typ == PLPSM_STMT_ITERATE)
					{
						if (scope->typ == PLPSM_STMT_COMPOUND_STATEMENT)
							ereport(ERROR,
									(errcode(ERRCODE_SYNTAX_ERROR),
									 errmsg("label of iterate statement is related to compound statement"),
											parser_errposition(stmt->location)));
						EMIT_JMP(scope->calls.entry_addr, stmt->lineno);
					}
					else
					{
						if (scope->calls.has_release_call)
						{
							scope->calls.release_jmps = lappend(scope->calls.release_jmps, 
														makeInteger(PC(m)));
							EMIT_OPCODE(JMP, stmt->lineno);
						}
						
						scope->calls.leave_jmps = lappend(scope->calls.leave_jmps,
													    makeInteger(PC(m)));
						EMIT_OPCODE(JMP, stmt->lineno);
					}
				}
				else
				{
					if (scope->calls.has_release_call)
					{
						scope->calls.release_calls = lappend(scope->calls.release_calls,
													    makeInteger(PC(m)));
						EMIT_OPCODE(CALL, stmt->lineno);
					}
				}
				compile_leave_iterate(cstate, scope->outer, stmt);
			}
			break;
		case PLPSM_STMT_SCHEMA:
			{
				/* 
				 * isn't possible directly leave or iterate schema,
				 * but schema has still release block and has a parents,
				 */
				if (scope->calls.has_release_call)
				{
					scope->calls.release_calls = lappend(scope->calls.release_calls, 
													makeInteger(PC(m)));
					EMIT_OPCODE(CALL, stmt->lineno);
				}
				compile_leave_iterate(cstate, scope->outer, stmt);
			}
			break;
		default:
			/* do nothing */;
	}
}

/*
 * finalize a compilation - append implicit RETURN statement
 */
static void
compile_done(CompileState cstate)
{
	 Plpsm_pcode_module *m = cstate->module;

	/* do nothing when previous command is RETURN */
	if (m->code[m->length - 1].typ == PCODE_RETURN)
		return;

	if (cstate->finfo.return_expr != NULL && cstate->finfo.result.datum.typoid != VOIDOID)
	{
		compile_expr(cstate, NULL, cstate->finfo.return_expr,
						cstate->finfo.result.datum.typoid, -1, false);
		SET_OPVAL(target.typlen, cstate->finfo.result.datum.typlen);
		SET_OPVAL(target.typbyval, cstate->finfo.result.datum.typbyval);
		EMIT_OPCODE(RETURN, -1);
	}
	else if (cstate->finfo.result.datum.typoid != VOIDOID)
	{
		SET_OPVAL(str,"function doesn't return data");
		EMIT_OPCODE(SIGNAL_NODATA, -1);
	}
	else
	{
		EMIT_OPCODE(RETURN_VOID, -1);
	}
}

/*
 * Ensure correct settings of release calls and leave jmps
 *
 * Every compound statement has a release block (active only when execution
 * is leaving from this block). Before leaving block, a release call should
 * be done. Next instruction is jump to target address. 
 *
 */
static Plpsm_object *
release_psm_object(CompileState cstate, Plpsm_object *obj, int release_call_entry, int release_jmp_entry, int leave_entry)
{
	Plpsm_pcode_module *m = cstate->module;
	ListCell	*l;

	if (obj->calls.release_calls)
	{
		Assert(release_call_entry != 0);
		foreach(l, obj->calls.release_calls)
		{
			SET_OPVAL_ADDR(intVal(lfirst(l)), addr, release_call_entry);
		}
	}

	if (obj->calls.release_jmps)
	{
		foreach(l, obj->calls.release_jmps)
		{
			SET_OPVAL_ADDR(intVal(lfirst(l)), addr, release_jmp_entry);
		}
	}

	if (obj->calls.leave_jmps)
	{
		Assert(leave_entry != 0);
		foreach(l, obj->calls.leave_jmps)
		{
			SET_OPVAL_ADDR(intVal(lfirst(l)), addr, leave_entry);
		}
	}

	return obj->outer;
}

void
plpsm_parser_setup(struct ParseState *pstate, CompileState cstate)
{
	pstate->p_post_columnref_hook = plpsm_post_column_ref;
	pstate->p_paramref_hook = plpsm_paramref_hook;
	pstate->p_ref_hook_state = (void *) cstate;
}

/*
 * Transform a query string - replace vars's identifiers by placeholders
 */
static char *
replace_vars(CompileState cstate, char *sqlstr, Oid **argtypes, int *nargs, TupleDesc *tupdesc, int location)
{
	Oid	*newargtypes;
	ListCell *l;
	List       *raw_parsetree_list;
	MemoryContext oldctx, tmpctx;
	ErrorContextCallback  			syntax_errcontext;
	Plpsm_sql_error_callback_arg 		cbarg;

	cstate->pdata.maxvars = 128;
	cstate->pdata.vars = (SQLHostVar *) palloc(cstate->pdata.maxvars * sizeof(SQLHostVar));
	cstate->pdata.nvars = 0;
	cstate->pdata.has_external_params = false;
	cstate->pdata.is_expression = true;

	/*
	 * The temp context is a child of current context
	 */
	tmpctx = AllocSetContextCreate(CurrentMemoryContext,
										   "compilation context",
										   ALLOCSET_DEFAULT_MINSIZE,
										   ALLOCSET_DEFAULT_INITSIZE,
										   ALLOCSET_DEFAULT_MAXSIZE);

	oldctx = MemoryContextSwitchTo(tmpctx);

	if (location != -1)
	{
		cbarg.location = location;
		cbarg.leaderlen = strlen(sqlstr);

		syntax_errcontext.callback = plpsm_sql_error_callback;
		syntax_errcontext.arg = &cbarg;
		syntax_errcontext.previous = error_context_stack;
		error_context_stack = &syntax_errcontext;
	}

	raw_parsetree_list = pg_parse_query(sqlstr);

	foreach(l, raw_parsetree_list)
	{
		Node       *parsetree = (Node *) lfirst(l);
		List   *stmt_list;

		stmt_list = pg_analyze_and_rewrite_params((Node *) parsetree, sqlstr, 
								(ParserSetupHook) plpsm_parser_setup, 
												(void *) cstate);
		if (tupdesc)
		{
			stmt_list = pg_plan_queries(stmt_list, 0, NULL);
			MemoryContextSwitchTo(oldctx);
			*tupdesc = CreateTupleDescCopy(PlanCacheComputeResultDesc(stmt_list));
			break;
		}
	}

	MemoryContextSwitchTo(oldctx);

	if (location != -1)
	{
		/* Restore former ereport callback */
		error_context_stack = syntax_errcontext.previous;
	}

	if (!cstate->pdata.has_external_params)
	{
		*argtypes = NULL;
		*nargs = 0;
		return sqlstr;
	}
	else
	{
		newargtypes = palloc(cstate->stack.ndatums * sizeof(Oid));
		memcpy(newargtypes, cstate->stack.oids.data, cstate->stack.ndatums * sizeof(Oid));

		*argtypes = newargtypes;
		*nargs = cstate->stack.ndatums;
	}

	if (cstate->pdata.nvars > 0)
	{
		int		i;
		SQLHostVar	*vars;
		int	nvars;
		bool	not_sorted;
		char *ptr;
		StringInfoData		cds;
		int	loc = 0;

		initStringInfo(&cds);

		/* now, pdata.params and pdata.cref_list contains a info about placeholders */
		vars = cstate->pdata.vars;
		nvars = cstate->pdata.nvars;

		/* simple bublesort */
		not_sorted = true;
		while (not_sorted)
		{
			not_sorted = false;
			for (i = 0; i < nvars - 1; i++)
			{
				SQLHostVar	var;

				if (vars[i].location > vars[i+1].location)
				{
					not_sorted = true;
					var = vars[i];
					vars[i] = vars[i+1]; vars[i+1] = var;
				}
			}
		}

		ptr = sqlstr; i = 0;

		while (*ptr)
		{
			char *str;

			/* copy content to position of replaced object reference */
			while (loc < vars[i].location)
			{
				appendStringInfoChar(&cds, *ptr++);
				loc++;
			}

			/* replace ref. object by placeholder */
			appendStringInfo(&cds,"$%d", (int) vars[i].offset);

			/* find and skip a referenced object */
			str = strstr(ptr, vars[i].name1);
			Assert(str != NULL);
			str += strlen(vars[i].name1);
			loc += str - ptr; ptr = str;

			/* when object was referenced with qualified name, then skip second identif */
			if (vars[i].name2 != NULL)
			{
				str = strstr(ptr, vars[i].name2);
				Assert(str != NULL);
				str += strlen(vars[i].name2);
				loc += str - ptr; ptr = str;
			}

			if (++i >= nvars)
			{
				/* copy to end and leave */
				while (*ptr)
					appendStringInfoChar(&cds, *ptr++);
				break;
			}
		}

		pfree(sqlstr);
		MemoryContextDelete(tmpctx);
		return cds.data;
	}
	else
	{
		MemoryContextDelete(tmpctx);
		return sqlstr;
	}
}

/*
 * returns necessary informations about field of composite type
 */
static int
resolve_composite_field(Oid rowoid, int16 rowtypmod, const char *fieldname, 
								Oid *typoid, int16 *typmod, 
											    int location)
{
	TupleDesc tupdesc = lookup_rowtype_tupdesc(rowoid, rowtypmod);
	int		i;

	Assert(fieldname != NULL);

	for (i = 0; i < tupdesc->natts; i++)
	{
		Form_pg_attribute att = tupdesc->attrs[i];

		if (strcmp(fieldname, NameStr(att->attname)) == 0 && !att->attisdropped)
		{
			*typoid = att->atttypid;
			*typmod = att->atttypmod;
			ReleaseTupleDesc(tupdesc);
			return i + 1;
		}
	}

	ereport(ERROR,
		    (errcode(ERRCODE_SYNTAX_ERROR),
		     errmsg("there are no field \"%s\" in type \"%s\"", fieldname,
						    format_type_with_typemod(rowoid, rowtypmod)),
					parser_errposition(location)));
	return -1;		/* be compiler quiete */
}

/*
 * Compile a SET statement in (var,var,..) = (expr, expr, ..) form
 */
static void
compile_multiset_stmt(CompileState cstate, Plpsm_stmt *stmt, Plpsm_ESQL *from_clause)
{
	Plpsm_pcode_module *m = cstate->module;
	int	addr;
	StringInfoData	ds;
	ListCell	*l1, *l2;
	int	i = 1;
	Oid	*locargtypes;
	int	nargs;
	bool		isfirst = true;

	initStringInfo(&ds);

	/* 
	 * now we doesn't a know a properties of generated query, so only emit
	 * opcode now. The property will be reasigned later.
	 */
	addr = PC(m);
	SET_OPVAL(expr.data, cstate->stack.ndata++);
	SET_OPVAL(expr.is_multicol, true);
	EMIT_OPCODE(EXEC_EXPR, stmt->lineno);

	/*
	 * Check a variant, where esql_list has one item and it is a embeded query
	 * type.
	 */
	if (list_length(stmt->esql_list) == 1 && (((Plpsm_ESQL *)(linitial(stmt->esql_list)))->typ == PLPSM_ESQL_QUERY))
	{
		const char *fieldname;
		Plpsm_object *var;
		Plpsm_ESQL *esql;
		int		param_number = 1;
		int		ta_number = cstate->stack.ndata - 1;

		StringInfoData target_list;
		StringInfoData alias_list;

		initStringInfo(&target_list);
		initStringInfo(&alias_list);

		/*
		 * We have to inject query with explicit casting to taget types. It can be done
		 * with derivated table - and alias type descriptions
		 */
		foreach (l1, stmt->compound_target)
		{
			Plpsm_positioned_qualid *qualid = (Plpsm_positioned_qualid *) lfirst(l1);

			if (!isfirst)
			{
				appendStringInfoChar(&target_list, ',');
				appendStringInfoChar(&alias_list, ',');
			}
			else
				isfirst = false;

			var = resolve_target(cstate, qualid, &fieldname, PLPSM_STMT_DECLARE_VARIABLE);
			esql = (Plpsm_ESQL *) lfirst(l2);

			if (fieldname != NULL)
			{
				int16	typmod = -1;
				Oid	typoid = InvalidOid;
				int	fno;

				fno = resolve_composite_field(var->stmt->datum.typoid, var->stmt->datum.typmod, fieldname,
															    &typoid,
															    &typmod,
																qualid->location);

				appendStringInfo(&target_list, "___ta_%d.___%d::%s", ta_number, param_number,
											    format_type_with_typemod(typoid, typmod));
				appendStringInfo(&alias_list, "___%d", param_number++);

				SET_OPVAL(update_field.fno, fno);
				SET_OPVAL(update_field.typoid, var->stmt->datum.typoid);
				SET_OPVAL(update_field.typmod, var->stmt->datum.typmod);
				SET_OPVAL(update_field.offset, var->offset);
				SET_OPVAL(update_field.fnumber, i++);
				EMIT_OPCODE(UPDATE_FIELD, stmt->lineno);
			}
			else
			{

				appendStringInfo(&target_list, "___ta_%d.___%d::%s", ta_number, param_number,
											    format_type_with_typemod(var->stmt->datum.typoid, var->stmt->datum.typmod));
				appendStringInfo(&alias_list, "___%d", param_number++);

				SET_OPVALS_DATUM_INFO(saveto_field, var);
				SET_OPVAL(saveto_field.fnumber, i++);
				EMIT_OPCODE(SAVETO_FIELD, stmt->lineno);
			}
		}
		appendStringInfo(&ds, "SELECT %s FROM (%s) ___ta_%d(%s)", 
											target_list.data,
											((Plpsm_ESQL *)(linitial(stmt->esql_list)))->sqlstr,
											ta_number,
											alias_list.data);
		pfree(alias_list.data);
		pfree(target_list.data);

		compile_refresh_basic_diagnostic(cstate);

	}
	else
	{
		if (list_length(stmt->compound_target) != list_length(stmt->esql_list))
			elog(ERROR, "there are different number of target variables and expressions in list");

		appendStringInfoString(&ds, "SELECT ");

		forboth (l1, stmt->compound_target, l2, stmt->esql_list)
		{
			const char *fieldname;
			Plpsm_object *var;
			Plpsm_ESQL *esql;
			Plpsm_positioned_qualid *qualid = (Plpsm_positioned_qualid *) lfirst(l1);

			var = resolve_target(cstate, qualid, &fieldname, PLPSM_STMT_DECLARE_VARIABLE);
			esql = (Plpsm_ESQL *) lfirst(l2);

			if (!isfirst)
				appendStringInfoChar(&ds, ',');
			else
				isfirst = false;

			if (fieldname != NULL)
			{
				int16	typmod = -1;
				Oid	typoid = InvalidOid;
				int	fno;

				fno = resolve_composite_field(var->stmt->datum.typoid, var->stmt->datum.typmod, fieldname,
															    &typoid,
															    &typmod,
																qualid->location);
				appendStringInfo(&ds, "(%s)::%s", esql->sqlstr, format_type_with_typemod(typoid, typmod));
				SET_OPVAL(update_field.fno, fno);
				SET_OPVAL(update_field.typoid, var->stmt->datum.typoid);
				SET_OPVAL(update_field.typmod, var->stmt->datum.typmod);
				SET_OPVAL(update_field.offset, var->offset);
				SET_OPVAL(update_field.fnumber, i++);
				EMIT_OPCODE(UPDATE_FIELD, stmt->lineno);
			}
			else
			{
				appendStringInfo(&ds, "(%s)::%s", esql->sqlstr, 
								format_type_with_typemod(var->stmt->datum.typoid, var->stmt->datum.typmod));
				SET_OPVALS_DATUM_INFO(saveto_field, var);
				SET_OPVAL(saveto_field.fnumber, i++);
				EMIT_OPCODE(SAVETO_FIELD, stmt->lineno);
			}
		}

		if (from_clause != NULL)
			appendStringInfo(&ds, " %s", from_clause->sqlstr);
	}

	SET_OPVAL_ADDR(addr, expr.expr, replace_vars(cstate, ds.data, &locargtypes, &nargs, NULL, stmt->location));
	SET_OPVAL_ADDR(addr, expr.nparams, nargs);
	SET_OPVAL_ADDR(addr, expr.typoids, locargtypes);
}

/*
 * emit instruction to execute a expression with specified targetoid and typmod
 */
static void 
compile_expr(CompileState cstate, Plpsm_ESQL *esql, const char *expr, Oid targetoid, int16 typmod, bool refresh_state_vars)
{
	StringInfoData	ds;
	Plpsm_pcode_module *m = cstate->module;
	int	nargs;
	Oid	*argtypes;
	Plpsm_sql_error_callback_arg 		cbarg;
	ErrorContextCallback  			syntax_errcontext;
	int	lineno = -1;

	if (esql != NULL)
	{
		expr = esql->sqlstr;
		lineno = esql->lineno;

		cbarg.location = esql->location;
		cbarg.leaderlen = strlen("SELECT (");

		syntax_errcontext.callback = plpsm_sql_error_callback;
		syntax_errcontext.arg = &cbarg;
		syntax_errcontext.previous = error_context_stack;
		error_context_stack = &syntax_errcontext;
	}

	SET_OPVAL(expr.without_diagnostics, !refresh_state_vars);

	initStringInfo(&ds);
	appendStringInfo(&ds, "SELECT (%s)::%s", expr, format_type_with_typemod(targetoid, typmod));
	SET_OPVAL(expr.expr, replace_vars(cstate, ds.data, &argtypes, &nargs, NULL, -1));
	SET_OPVAL(expr.nparams, nargs);
	SET_OPVAL(expr.typoids, argtypes);
	SET_OPVAL(expr.data, cstate->stack.ndata++);
	SET_OPVAL(expr.is_multicol, false);
	EMIT_OPCODE(EXEC_EXPR, lineno);

	if (esql != NULL)
	{
		/* Restore former ereport callback */
		error_context_stack = syntax_errcontext.previous;
	}

	/*
	 * refresh state variables
	 */
	if (refresh_state_vars)
	{
		if (cstate->stack.has_sqlstate)
		{
			Plpsm_object *var = lookup_var(cstate->current_scope, "sqlstate");
			Assert(var != NULL);
			SET_OPVALS_DATUM_COPY(target, var);
			EMIT_OPCODE(SQLSTATE_REFRESH, -1);
		}

		if (cstate->stack.has_sqlcode)
		{
			Plpsm_object *var = lookup_var(cstate->current_scope, "sqlcode");
			Assert(var != NULL);
			SET_OPVALS_DATUM_COPY(target, var);
			EMIT_OPCODE(SQLCODE_REFRESH, -1);
		}
	}
}

/*
 * emit simple expression
 */
static void 
compile_cast_var(CompileState cstate, int offset, Oid targetoid, int16 typmod, 
										bool is_array, 
										bool refresh_state_vars,
										int lineno)
{
	StringInfoData	ds;
	Plpsm_pcode_module *m = cstate->module;
	Oid	*argtypes;

	initStringInfo(&ds);
	appendStringInfo(&ds, "SELECT ($%d)::%s%s", offset,
								format_type_with_typemod(targetoid, typmod),
								is_array ? "[]" : "");

	SET_OPVAL(expr.without_diagnostics, !refresh_state_vars);

	argtypes = palloc(cstate->stack.ndatums * sizeof(Oid));
	memcpy(argtypes, cstate->stack.oids.data, cstate->stack.ndatums * sizeof(Oid));

	SET_OPVAL(expr.nparams, cstate->stack.ndatums);
	SET_OPVAL(expr.typoids, argtypes);
	SET_OPVAL(expr.data, cstate->stack.ndata++);
	SET_OPVAL(expr.is_multicol, false);
	EMIT_OPCODE(EXEC_EXPR, lineno);

	/*
	 * refresh state variables
	 */
	if (refresh_state_vars)
	{
		if (cstate->stack.has_sqlstate)
		{
			Plpsm_object *var = lookup_var(cstate->current_scope, "sqlstate");
			Assert(var != NULL);
			SET_OPVALS_DATUM_COPY(target, var);
			EMIT_OPCODE(SQLSTATE_REFRESH, -1);
		}

		if (cstate->stack.has_sqlcode)
		{
			Plpsm_object *var = lookup_var(cstate->current_scope, "sqlcode");
			Assert(var != NULL);
			SET_OPVALS_DATUM_COPY(target, var);
			EMIT_OPCODE(SQLCODE_REFRESH, -1);
		}
	}
}

/*
 * emit code necessary for USAGE clause
 */
static void
compile_usage_clause(CompileState cstate, Plpsm_stmt *stmt, int *params)
{
	StringInfoData ds;
	Plpsm_pcode_module *m = cstate->module;
	int dataidx = cstate->stack.ndata++;
	int	addr1;
	ListCell	*l;
	int	i = 1;
	Oid	*argtypes;
	int	nargs;
	bool		isFirst = true;

	initStringInfo(&ds);

	PARAMBUILDER1(INIT, dataidx, nargs, list_length(stmt->variables));

	addr1 = PC(m);
	SET_OPVAL(expr.data, cstate->stack.ndata++);
	SET_OPVAL(expr.is_multicol, true);
	EMIT_OPCODE(EXEC_EXPR, stmt->lineno);
	appendStringInfoString(&ds, "SELECT ");

	foreach (l, stmt->variables)
	{
		Plpsm_object *var;
		const char *fieldname;

		if (!isFirst)
			appendStringInfoChar(&ds, ',');
		else
			isFirst = false;

		var = resolve_target(cstate, (Plpsm_positioned_qualid *) lfirst(l), &fieldname, PLPSM_STMT_DECLARE_VARIABLE);
		if (fieldname == NULL)
			appendStringInfo(&ds, "$%d", var->offset + 1);
		else
			appendStringInfo(&ds, "$%d.%s", var->offset + 1, fieldname);

		PARAMBUILDER1(APPEND, dataidx, fnumber, i++);
	}

	nargs = cstate->stack.ndatums;
	argtypes = palloc(nargs * sizeof(Oid));
	memcpy(argtypes, cstate->stack.oids.data, nargs * sizeof(Oid));

	SET_OPVAL_ADDR(addr1, expr.expr, ds.data);
	SET_OPVAL_ADDR(addr1, expr.nparams, nargs);
	SET_OPVAL_ADDR(addr1, expr.typoids, argtypes);
	*params = dataidx;
}

/*
 * emits a code for refreshing a basic diagnostics
 * sqlstate, sqlcode and possible not found warning.
 */
static void
compile_refresh_basic_diagnostic(CompileState cstate)
{
	Plpsm_pcode_module *m = cstate->module;

	if (cstate->stack.has_sqlstate)
	{
		Plpsm_object *var = lookup_var(cstate->current_scope, "sqlstate");
		Assert(var != NULL);
		SET_OPVALS_DATUM_COPY(target, var);
		EMIT_OPCODE(SQLSTATE_REFRESH, -1);
	}

	if (cstate->stack.has_sqlcode)
	{
		Plpsm_object *var = lookup_var(cstate->current_scope, "sqlcode");
		Assert(var != NULL);
		SET_OPVALS_DATUM_COPY(target, var);
		EMIT_OPCODE(SQLCODE_REFRESH, -1);
	}

	if (cstate->stack.has_notfound_continue_handler)
	{
		/* recheck not found handler */
		Plpsm_object *notfound_handler = lookup_notfound_continue_handler(cstate->current_scope);
		if (notfound_handler != NULL)
		{
			if (notfound_handler->stmt->option == PLPSM_HANDLER_CONTINUE)
			{
				SET_OPVAL(addr, notfound_handler->calls.entry_addr);
				EMIT_OPCODE(CALL_NOT_FOUND, -1);
			}
			else
			{
				/* call a handler and leave to outers' handler block */
				int	addr1 = PC(m);
				int	addr2;

				EMIT_OPCODE(JMP_NOT_FOUND, -1);
				addr2 = PC(m);
				EMIT_OPCODE(JMP, -1);
				SET_OPVAL_ADDR(addr1, addr, PC(m));
				SET_OPVAL(addr, notfound_handler->calls.entry_addr);
				EMIT_OPCODE(CALL, -1);
				/* leave a current compound statement */
				compile_leave_target_block(cstate, cstate->current_scope, notfound_handler->outer);
				SET_OPVAL_ADDR(addr2, addr, PC(m));
			}
		}
	}
}

/*
 * ensure a emmiting a source's releasing
 */
static void
finalize_block(CompileState cstate, Plpsm_object *obj)
{
	int	release_call_entry = 0;
	int	release_jmp_entry = 0;
	
	Plpsm_pcode_module *m = cstate->module;

	if (obj->calls.has_release_call)
	{
		if (obj->calls.release_calls != NULL)
		{
			int addr1 = PC(m);

			EMIT_OPCODE(JMP, -1);

			release_call_entry = PC(m);
			compile_release_cursors(cstate);
			if ((bool) obj->stmt->option && obj->typ == PLPSM_STMT_COMPOUND_STATEMENT)
			{
				/* release a savepoint */
				EMIT_OPCODE(RELEASE_SUBTRANSACTION, -1);
			}

			if (obj->typ == PLPSM_STMT_DECLARE_HANDLER && cstate->use_stacked_diagnostics)
			{
				/* pop a stacked diagnostics info to first_area */
				EMIT_OPCODE(DIAGNOSTICS_POP, -1);
			}

			EMIT_OPCODE(RET_SUBR, -1);
			SET_OPVAL_ADDR(addr1, addr, PC(m));
		}

		release_jmp_entry = PC(m);
		compile_release_cursors(cstate);
		if ((bool) obj->stmt->option && obj->typ == PLPSM_STMT_COMPOUND_STATEMENT)
			/* release a savepoint */
			EMIT_OPCODE(RELEASE_SUBTRANSACTION, -1);

		if (obj->typ == PLPSM_STMT_DECLARE_HANDLER && cstate->use_stacked_diagnostics)
		{
			/* pop a stacked diagnostics info to first_area */
			EMIT_OPCODE(DIAGNOSTICS_POP, -1);
		}
	}
	else
		release_jmp_entry = PC(m);

	cstate->current_scope = release_psm_object(cstate, obj, release_call_entry, release_jmp_entry, PC(m));
}

/*
 * compile a signal statement
 */
static void
compile_signal(CompileState cstate, Plpsm_stmt *stmt, int addr, Plpsm_pcode_type typ,
			 bool is_undo_handler, bool is_resignal, char *condition_name, int derivated_sqlstate)
{
	Plpsm_signal_info *sinfo = (Plpsm_signal_info *) stmt->data;
	Plpsm_pcode_module *m = cstate->module;
	int	eclass;
	int	level;
	int sqlstate;

	if (condition_name != NULL)
	{
		SIGNAL_PROPERTY(SET_CSTRING, CONDITION_IDENTIFIER, cstr, pstrdup(condition_name));
		sqlstate = derivated_sqlstate != 0 ? derivated_sqlstate : MAKE_SQLSTATE('4','5','0','0','0');
	}
	else
		sqlstate = stmt->option;

	SET_OPVAL(signal_params.addr, addr);
	SET_OPVAL(signal_params.is_undo_handler, is_undo_handler);

	/* 
	 * SQL/PSM doesn't know a levels in PL/pgSQL semantic. We must to deduce
	 * level from sql state.
	 */
	eclass = ERRCODE_TO_CATEGORY(sqlstate);
	if (eclass == MAKE_SQLSTATE('0','0','0','0','0'))
		level = NOTICE;
	else if (eclass == MAKE_SQLSTATE('0','2','0','0','0') || eclass == MAKE_SQLSTATE('0','1','0','0','0'))
		level = WARNING;
	else
		level = ERROR;

	SIGNAL_PROPERTY(SET_INT, SQLCODE, ival, sqlstate);
	SIGNAL_PROPERTY(SET_INT, LEVEL, ival, level);

	while (sinfo != NULL)
	{
		Plpsm_object *var;

		if (sinfo->var != NULL)
		{
			const char *fieldname;

			var = resolve_target(cstate, sinfo->var, &fieldname, PLPSM_STMT_DECLARE_VARIABLE);
			if (fieldname != NULL)
				elog(ERROR, "variable used in SIGNAL or RESIGNAL statement must not be a composite type");

			switch (var->stmt->datum.typoid)
			{
				case TEXTOID:
				case BPCHAROID:
				case VARCHAROID:
					break;
				default:
					elog(ERROR, "variable used in SIGNAL or RESIGNAL statement should be only text type");
			}

			switch (sinfo->typ)
			{
				case PLPSM_SINFO_DETAIL:
					SIGNAL_PROPERTY(COPY_TEXT_VAR, DETAIL, offset, var->offset);
					break;
				case PLPSM_SINFO_HINT:
					SIGNAL_PROPERTY(COPY_TEXT_VAR, HINT, offset, var->offset);
					break;
				case PLPSM_SINFO_MESSAGE:
					SIGNAL_PROPERTY(COPY_TEXT_VAR, MESSAGE, offset, var->offset);
					break;
			}
		}
		else
		{
			switch (sinfo->typ)
			{
				case PLPSM_SINFO_DETAIL:
					SIGNAL_PROPERTY(SET_CSTRING, DETAIL, cstr, pstrdup(sinfo->value));
					break;
				case PLPSM_SINFO_HINT:
					SIGNAL_PROPERTY(SET_CSTRING, HINT, cstr, pstrdup(sinfo->value));
					break;
				case PLPSM_SINFO_MESSAGE:
					SIGNAL_PROPERTY(SET_CSTRING, MESSAGE, cstr, pstrdup(sinfo->value));
					break;
			}
		}
		sinfo = sinfo->next;
	}

	if (cstate->stack.has_sqlstate)
	{
		Plpsm_object *var = lookup_var(cstate->current_scope, "sqlstate");
		Assert(var != NULL);
		SET_OPVALS_DATUM_COPY(target, var);
		EMIT_OPCODE(SQLSTATE_REFRESH, -1);
	}

	if (cstate->stack.has_sqlcode)
	{
		Plpsm_object *var = lookup_var(cstate->current_scope, "sqlcode");
		Assert(var != NULL);
		SET_OPVALS_DATUM_COPY(target, var);
		EMIT_OPCODE(SQLCODE_REFRESH, -1);
	}

	SET_OPVAL(addr, addr);

	if (!is_resignal)
	{
		if (typ == PCODE_SIGNAL_JMP)
			EMIT_OPCODE(SIGNAL_JMP, stmt->lineno);
		else 
		{
			Assert(typ == PCODE_SIGNAL_CALL);
			EMIT_OPCODE(SIGNAL_CALL, stmt->lineno);
		}
	}
	else
	{
		if (typ == PCODE_SIGNAL_JMP)
			EMIT_OPCODE(RESIGNAL_JMP, stmt->lineno);
		else 
		{
			Assert(typ == PCODE_SIGNAL_CALL);
			EMIT_OPCODE(RESIGNAL_CALL, stmt->lineno);
		}
	}
}

/*
 * diff from plpgsql 
 *
 * SQL/PSM is compilable language - so it checking a all SQL and expression
 * before first start of procedure. All SQL object must exists. When a access
 * to dynamic object is necessary, then dynamic SQL must be used. ???
 */
static void 
_compile(CompileState cstate, Plpsm_stmt *stmt, Plpsm_object *parent)
{
	int	addr1;
	int	addr2;
	Plpsm_object *obj, *var;
	Plpsm_pcode_module *m = cstate->module;
	bool		should_insert_subtransaction;		/* true, when start of subtransaction will be inserted after declare statements */
	bool		handlers_table_refreshed = false;

	int old_th_addr = cstate->stack.ht_entry;
	bool	inside_handler = cstate->stack.inside_handler;

	should_insert_subtransaction = parent != NULL && parent->typ == PLPSM_STMT_COMPOUND_STATEMENT && parent->is_atomic;

	while (stmt != NULL)
	{

		if (should_insert_subtransaction && 
			stmt->typ != PLPSM_STMT_DECLARE_VARIABLE && 
			stmt->typ != PLPSM_STMT_DECLARE_CURSOR &&
			stmt->typ != PLPSM_STMT_DECLARE_HANDLER)
		{
			EMIT_OPCODE(BEGIN_SUBTRANSACTION, stmt->lineno);

			should_insert_subtransaction = false;
		}

		/* 
		 * we have to generate HT_tab after last DECLARE CONDTION statement and 
		 * before first DECLARE HANDLER statement.
		 */
		if (stmt->typ == PLPSM_STMT_DECLARE_HANDLER && !handlers_table_refreshed)
		{
			Assert(parent != NULL && parent->typ == PLPSM_STMT_COMPOUND_STATEMENT);
			handlers_table_refreshed = true;
			cstate->stack.ht_entry = compile_ht(cstate, parent->stmt, cstate->stack.ht_entry);
		}

		switch (stmt->typ)
		{
			case PLPSM_STMT_LOOP:
				{
					obj = new_psm_object_for(stmt, cstate, PC(m));
					addr1 = PC(m);
					_compile(cstate, stmt->inner_left, NULL);
					EMIT_JMP(addr1, stmt->lineno);
					cstate->current_scope = release_psm_object(cstate, obj, 0, 0, PC(m));
				}
				break;

			case PLPSM_STMT_WHILE:
				{
					obj = new_psm_object_for(stmt, cstate, PC(m));
					addr1 = PC(m);
					compile_expr(cstate, stmt->esql, NULL, BOOLOID, -1, true);
					addr2 = PC(m);
					EMIT_OPCODE(JMP_FALSE_UNKNOWN, stmt->lineno);
					_compile(cstate, stmt->inner_left, NULL);
					EMIT_JMP(addr1, stmt->lineno);
					SET_OPVAL_ADDR(addr2, addr, PC(m));
					cstate->current_scope = release_psm_object(cstate, obj, 0, 0, PC(m));
				}
				break;

			case PLPSM_STMT_REPEAT_UNTIL:
				{
					obj = new_psm_object_for(stmt, cstate, PC(m));
					addr1 = PC(m);
					_compile(cstate, stmt->inner_left, NULL);
					compile_expr(cstate, stmt->esql, NULL, BOOLOID, -1, true);
					SET_OPVAL(addr, addr1);
					EMIT_OPCODE(JMP_FALSE_UNKNOWN, stmt->lineno);
					cstate->current_scope = release_psm_object(cstate, obj, 0, 0, PC(m));
				}
				break;

			case PLPSM_STMT_FOR:
				{
					bool	has_sqlstate = cstate->stack.has_sqlstate;
					bool	has_sqlcode = cstate->stack.has_sqlcode;
					bool	has_notfound_continue_handler = cstate->stack.has_notfound_continue_handler;

					Plpsm_stmt *loopvar_stmt = palloc0(sizeof(Plpsm_stmt));
					Plpsm_stmt *decl_cur = palloc0(sizeof(Plpsm_stmt));
					Plpsm_object *loopvar_obj;
					Plpsm_object *for_obj;
					Plpsm_object	*cursor;
					Plpsm_object		**vars;
					char *cursor_name;
					Oid				*argtypes;
					int				nargs;
					TupleDesc	tupdesc;
					int i;

					/* generate outer loopvar */
					loopvar_stmt->typ = PLPSM_STMT_SCHEMA;
					loopvar_stmt->location = stmt->location;
					loopvar_stmt->name = stmt->stmtfor.loopvar_name;
					loopvar_obj = new_psm_object_for(loopvar_stmt, cstate, PC(m));

					decl_cur->typ = PLPSM_STMT_DECLARE_CURSOR;
					cursor_name = stmt->stmtfor.cursor_name ? stmt->stmtfor.cursor_name : "";
					cursor = create_variable_for(decl_cur, cstate, NULL, cursor_name, PLPSM_VARIABLE);

					cursor->cursor.data_addr = PC(m);
					cursor->cursor.is_dynamic = false;

					SET_OPVAL(expr.expr, replace_vars(cstate, pstrdup(stmt->esql->sqlstr), &argtypes, &nargs, &tupdesc, stmt->esql->location));
					SET_OPVAL(expr.nparams, nargs);
					SET_OPVAL(expr.typoids, argtypes);
					SET_OPVAL(expr.data, cstate->stack.ndata++);
					SET_OPVAL(expr.is_multicol, true);

					EMIT_OPCODE(DATA_QUERY, stmt->lineno);

					vars = (Plpsm_object **) palloc(tupdesc->natts * sizeof(Plpsm_object *));

					for (i = 0; i < tupdesc->natts; i++)
					{
						Form_pg_attribute att = tupdesc->attrs[i];
						Plpsm_stmt *decl_stmt = palloc0(sizeof(Plpsm_stmt));
						int16 typlen;
						Oid	typoid;
						bool	typbyval;
						Plpsm_object *var;

						decl_stmt->typ = PLPSM_STMT_DECLARE_VARIABLE;
						decl_stmt->target = new_qualid(list_make1(pstrdup(NameStr(att->attname))), -1);
						typoid = att->atttypid;
						get_typlenbyval(typoid, &typlen, &typbyval);

						decl_stmt->datum.typoid = typoid;
						decl_stmt->datum.typmod = att->atttypmod;
						decl_stmt->datum.typname = NULL;
						decl_stmt->datum.typlen = typlen;
						decl_stmt->datum.typbyval = typbyval;

						/* append implicit name to scope */
						var = create_variable_for(decl_stmt, cstate, NULL, pstrdup(NameStr(att->attname)), PLPSM_VARIABLE);
						vars[i] = var;
					}

					SET_OPVAL(cursor.addr, cursor->cursor.data_addr);
					SET_OPVAL(cursor.offset, cursor->cursor.offset);
					SET_OPVAL(cursor.name, cursor->name);
					EMIT_OPCODE(CURSOR_OPEN, stmt->lineno);

					/* generate innerate loop */
					for_obj = new_psm_object_for(stmt, cstate, PC(m));

					addr2 = PC(m);

					/* fill implicit variables */
					SET_OPVAL(fetch.offset, cursor->cursor.offset);
					SET_OPVAL(fetch.name, cursor->name);
					SET_OPVAL(fetch.nvars, 1);
					SET_OPVAL(fetch.count, 1);
					EMIT_OPCODE(CURSOR_FETCH, stmt->lineno);

					addr1 = PC(m);
					EMIT_OPCODE(JMP_NOT_FOUND, stmt->lineno);

					for (i = 0; i < tupdesc->natts; i++)
					{
						/* we are sure so datum doesn't need a cast */
						SET_OPVALS_DATUM_INFO(saveto_field, vars[i]);
						SET_OPVAL(saveto_field.fnumber, i + 1);
						EMIT_OPCODE(SAVETO_FIELD, stmt->lineno);
					}
					pfree(vars);

					_compile(cstate, stmt->inner_left, NULL);
					EMIT_JMP(addr2, stmt->lineno);
					SET_OPVAL_ADDR(addr1, addr, PC(m));

					/* release inner compose loop stmt */
					finalize_block(cstate, for_obj);

					/* 
					 * cursor is closed inside release block,
					 * not necessary to explicit close cursor.
					 */

					/* generate release block */
					finalize_block(cstate, loopvar_obj);

					cstate->stack.has_sqlstate = has_sqlstate;
					cstate->stack.has_sqlcode = has_sqlcode;
					cstate->stack.has_notfound_continue_handler = has_notfound_continue_handler;

					FreeTupleDesc(tupdesc);
				}
				break;

			case PLPSM_STMT_COMPOUND_STATEMENT:
				{
					bool	has_sqlstate = cstate->stack.has_sqlstate;
					bool	has_sqlcode = cstate->stack.has_sqlcode;
					bool	has_notfound_continue_handler = cstate->stack.has_notfound_continue_handler;
					bool	has_undo_handler = false;
					bool	has_exit_handler = false;
					Plpsm_stmt *inner_stmt;
					int16	offset = -1;		/* be compiler quite */

					/* 
					 * Store PC when compound statement contains a exit or undo handler.
					 * Stored value will be used after end of block.
					 */
					inner_stmt = stmt->inner_left;
					while (inner_stmt != NULL)
					{
						if (inner_stmt->typ == PLPSM_STMT_DECLARE_HANDLER)
						{
							if (inner_stmt->option == PLPSM_HANDLER_EXIT)
								has_exit_handler = true;
							if (inner_stmt->option == PLPSM_HANDLER_UNDO)
								has_undo_handler = true;
						}
						inner_stmt = inner_stmt->next;
					}

					if (has_undo_handler || has_exit_handler)
					{
						/* 
						 * Allocate a space for storing stack counter
						 */
						offset = cstate->stack.ndatums++;
						if (offset >= cstate->stack.oids.size)
						{
							cstate->stack.oids.size += 128;
							cstate->stack.oids.data = repalloc(cstate->stack.oids.data, cstate->stack.oids.size * sizeof(Oid));
						}

						cstate->stack.oids.data[offset] = InvalidOid;

						SET_OPVAL(target.offset, offset);
						EMIT_OPCODE(STORE_SP, stmt->lineno);
					}

					if (has_undo_handler && !((bool) stmt->option))
						ereport(ERROR,
								(errcode(ERRCODE_SYNTAX_ERROR),
								 errmsg("a UNDO handler is allowed only ATOMIC compound statement"),
									parser_errposition(stmt->location)));

					obj = new_psm_object_for(stmt, cstate, PC(m));
					obj->is_atomic = (bool) stmt->option;

					/*
					 * When compound statement is atomic 
					 */
					if ((bool) stmt->option)
					{
						obj->calls.has_release_call = true;
					}

					_compile(cstate, stmt->inner_left, obj);

					cstate->stack.has_sqlstate = has_sqlstate;
					cstate->stack.has_sqlcode = has_sqlcode;
					cstate->stack.has_notfound_continue_handler = has_notfound_continue_handler;

					/* generate release block */
					finalize_block(cstate, obj);

					if (has_undo_handler || has_exit_handler)
					{
						SET_OPVAL(target.offset, offset);
						EMIT_OPCODE(LOAD_SP, stmt->lineno);
					}
				}
				break;

			case PLPSM_STMT_DECLARE_HANDLER:
				{
					bool	isnotfound = false;
					int	curr_ht_entry = cstate->stack.ht_entry;

					addr1 = PC(m);
					EMIT_OPCODE(JMP, stmt->lineno);
					addr2 = PC(m);

					obj = create_handler_for(stmt, cstate, PC(m), &isnotfound);

					/* inside handler use a parent HT entry */
					cstate->stack.ht_entry = old_th_addr;

					/* 
					 * handler object is used as barier against to lookup
					 * labels outside a handler body.
					 * has_notfound_continue_handler can be mistaken, there
					 * is necessary do recheck.
					 */
					cstate->stack.has_notfound_continue_handler = isnotfound;

					/* in inner code, we can use a RESIGNAL statement */
					cstate->stack.inside_handler = true;

					if (cstate->use_stacked_diagnostics)
					{
						EMIT_OPCODE(DIAGNOSTICS_PUSH, -1);
						obj->calls.has_release_call = true;
					}
					
					/* 
					 * UNDO handler does ROLLBACK on entry and jumps
					 * to first next statement after current compound 
					 * statement.
					 */
					if (stmt->option == PLPSM_HANDLER_CONTINUE)
					{
						_compile(cstate, stmt->inner_left, NULL);
						finalize_block(cstate, obj);
						EMIT_OPCODE(RET_SUBR, stmt->lineno);
					}
					else if (stmt->option == PLPSM_HANDLER_EXIT)
					{
						/*
						 * when parent is ATOMIC, then we have to release a
						 * current block. We can do it via jump to last
						 * statement of current compound statement, that is
						 * RELEASE stmt.
						 */
						_compile(cstate, stmt->inner_left, NULL);
						finalize_block(cstate, obj);
						parent->calls.release_jmps = lappend(parent->calls.release_jmps,
														makeInteger(PC(m)));
						EMIT_OPCODE(JMP, stmt->lineno);
					}
					else
					{
						Assert(stmt->option == PLPSM_HANDLER_UNDO);

						/* 
						 * a rollbac is done before we call a handler
						 */
						_compile(cstate, stmt->inner_left, NULL);
						finalize_block(cstate, obj);
						/* jmp out of parent */
						parent->calls.leave_jmps = lappend(parent->calls.leave_jmps,
														makeInteger(PC(m)));
						EMIT_OPCODE(JMP, stmt->lineno);
					}

					SET_OPVAL_ADDR(addr1, addr, PC(m));

					Assert(cstate->ht_table != NULL);
					Assert(stmt->ht_info.addr1 != 0);

					cstate->ht_table->code[stmt->ht_info.addr1].HT_field.addr = addr2;
					if (stmt->ht_info.addr2 != 0)
						cstate->ht_table->code[stmt->ht_info.addr2].HT_field.addr = addr2;

					cstate->stack.ht_entry = curr_ht_entry;
					cstate->current_scope = obj->outer;
				}
				break;

			case PLPSM_STMT_DECLARE_VARIABLE:
				{
					ListCell *l;

					if (stmt->esql != NULL)
						compile_expr(cstate, stmt->esql, NULL, stmt->datum.typoid, stmt->datum.typmod, false);

					foreach(l, stmt->compound_target)
					{
						Plpsm_positioned_qualid *qualid = (Plpsm_positioned_qualid *) lfirst(l);

						var = create_variable_for(stmt, cstate, qualid, NULL, PLPSM_VARIABLE);

						if (stmt->option == PLPSM_LOCAL_VARIABLE)
						{
							/* 
							 * every variable must be initialised to NULL, because
							 * save_to will to try free a not null value.
							 */
							SET_OPVAL(target.offset, var->offset);
							EMIT_OPCODE(SET_NULL, stmt->lineno);

							if (stmt->esql != NULL)
							{
								SET_OPVALS_DATUM_COPY(target, var);
								EMIT_OPCODE(SAVETO, stmt->lineno);
							}
						}
						else
						{
							Assert(stmt->option == PLPSM_TRIGGER_VARIABLE_OLD || stmt->option == PLPSM_TRIGGER_VARIABLE_NEW);

							/*
							 * Can be used only in trigger function
							 */
							if (cstate->finfo.result.datum.typoid != TRIGGEROID)
								ereport(ERROR,
										(errcode(ERRCODE_SYNTAX_ERROR),
										 errmsg("cannot use a trigger variables outside trigger function"),
												parser_errposition(stmt->location)));

							if (stmt->option == PLPSM_TRIGGER_VARIABLE_OLD)
							{
								if (cstate->trigger.var_old != NULL)
									ereport(ERROR,
											(errcode(ERRCODE_SYNTAX_ERROR),
											 errmsg("trigger variable should be declared only one times in function"),
													parser_errposition(stmt->location)));

								if (cstate->trigger.var_new != NULL)
								{
									if (var->stmt->datum.typoid != cstate->trigger.var_new->stmt->datum.typoid ||
										var->stmt->datum.typmod != cstate->trigger.var_new->stmt->datum.typmod)
										ereport(ERROR,
											(errcode(ERRCODE_SYNTAX_ERROR),
											 errmsg("OLD and NEW trigger variables should have same type"),
												parser_errposition(stmt->location)));
								}

								cstate->trigger.var_old = var;
								Assert(cstate->trigger.addr2 != 0);

								SET_OPVAL_ADDR(cstate->trigger.addr2, trigger_var.typ, stmt->option);
								SET_OPVAL_ADDR(cstate->trigger.addr2, trigger_var.offset, var->offset); 
								SET_OPVAL_ADDR(cstate->trigger.addr2, trigger_var.typlen, var->stmt->datum.typlen); 
								SET_OPVAL_ADDR(cstate->trigger.addr2, trigger_var.typbyval, var->stmt->datum.typbyval); 
								SET_OPVAL_ADDR(cstate->trigger.addr2, trigger_var.typoid, var->stmt->datum.typoid); 
								SET_OPVAL_ADDR(cstate->trigger.addr2, trigger_var.typmod, var->stmt->datum.typmod); 
							}
							else
							{
								if (cstate->trigger.var_new != NULL)
									ereport(ERROR,
											(errcode(ERRCODE_SYNTAX_ERROR),
											 errmsg("trigger variable should be declared only one times in function"),
												parser_errposition(stmt->location)));

								if (cstate->trigger.var_old != NULL)
								{
									if (var->stmt->datum.typoid != cstate->trigger.var_old->stmt->datum.typoid ||
										var->stmt->datum.typmod != cstate->trigger.var_old->stmt->datum.typmod)
										ereport(ERROR,
											(errcode(ERRCODE_SYNTAX_ERROR),
											 errmsg("OLD and NEW trigger variables should have same type"),
												parser_errposition(stmt->location)));
								}

								cstate->trigger.var_new = var;
								Assert(cstate->trigger.addr1 != 0);

								SET_OPVAL_ADDR(cstate->trigger.addr1, trigger_var.typ, stmt->option);
								SET_OPVAL_ADDR(cstate->trigger.addr1, trigger_var.offset, var->offset); 
								SET_OPVAL_ADDR(cstate->trigger.addr1, trigger_var.typlen, var->stmt->datum.typlen); 
								SET_OPVAL_ADDR(cstate->trigger.addr1, trigger_var.typbyval, var->stmt->datum.typbyval); 
								SET_OPVAL_ADDR(cstate->trigger.addr1, trigger_var.typoid, var->stmt->datum.typoid); 
								SET_OPVAL_ADDR(cstate->trigger.addr1, trigger_var.typmod, var->stmt->datum.typmod); 
							}
						}
					}
				}
				break;

			case PLPSM_STMT_DECLARE_CONDITION:
				{
					/*
					 * Rules:
					 *  Condition name must be unique in compound statement,
					 *  When SQLSTATE attribute is used, then should be unique in compound statement
					 */
					obj = create_condition_for(stmt, cstate);
				}
				break;

			case PLPSM_STMT_DECLARE_CURSOR:
				{
					var = create_variable_for(stmt, cstate, stmt->target, NULL, PLPSM_VARIABLE);
					if (stmt->esql != NULL)
					{
						Oid	*argtypes;
						int	nargs;

						var->cursor.data_addr = PC(m);
						var->cursor.is_dynamic = false;

						SET_OPVAL(expr.expr, replace_vars(cstate, pstrdup(stmt->esql->sqlstr), &argtypes, &nargs, NULL, stmt->esql->location));
						SET_OPVAL(expr.nparams, nargs);
						SET_OPVAL(expr.typoids, argtypes);
						SET_OPVAL(expr.data, cstate->stack.ndata++);
						SET_OPVAL(expr.is_multicol, true);

						EMIT_OPCODE(DATA_QUERY, stmt->lineno);
					}
					else
					{
						Assert(stmt->name != NULL);

						var->cursor.data_addr = fetchPrepared(cstate, stmt->name);
						var->cursor.is_dynamic = true;
						var->cursor.prepname = stmt->name;
					}

					/* initialise a cursor variable */
					SET_OPVAL(target.offset, var->cursor.offset);
					EMIT_OPCODE(SET_NULL, stmt->lineno);

				}
				break;

			case PLPSM_STMT_OPEN:
				{
					const char *fieldname;
					int	params = -1;

					var = resolve_target(cstate, stmt->target, &fieldname, PLPSM_STMT_DECLARE_CURSOR);
					if (var->stmt->typ != PLPSM_STMT_DECLARE_CURSOR)
						ereport(ERROR,
								(errcode(ERRCODE_SYNTAX_ERROR),
								 errmsg("variable \"%s\" isn't cursor", var->name),
									parser_errposition(stmt->target->location)));
					if (var->cursor.is_for_stmt_cursor)
						ereport(ERROR,
								(errcode(ERRCODE_SYNTAX_ERROR),
								 errmsg("cannot to open a cursor used in FOR statement"),
									parser_errposition(stmt->location)));

					if (stmt->variables != NIL)
					{
						if (!var->cursor.is_dynamic)
						ereport(ERROR,
								(errcode(ERRCODE_SYNTAX_ERROR),
								 errmsg("using a USAGE clause in static cursor \"%s\"", 
														var->name),
									parser_errposition(stmt->location)));
						compile_usage_clause(cstate, stmt, &params);
					}

					if (!var->cursor.is_dynamic)
					{
						SET_OPVAL(cursor.addr, var->cursor.data_addr);
						SET_OPVAL(cursor.offset, var->cursor.offset);
						SET_OPVAL(cursor.name, var->name);
						EMIT_OPCODE(CURSOR_OPEN, stmt->lineno);
					}
					else
					{
						SET_OPVAL(cursor.addr, var->cursor.data_addr);
						SET_OPVAL(cursor.offset, var->cursor.offset);
						SET_OPVAL(cursor.name, var->name);
						SET_OPVAL(cursor.params, params);
						SET_OPVAL(cursor.prepname, var->cursor.prepname);
						EMIT_OPCODE(CURSOR_OPEN_DYNAMIC, stmt->lineno);
						if (params != -1)
							PARAMBUILDER(FREE, params);
					}
				}
				break;

			case PLPSM_STMT_FETCH:
				{
					const char *fieldname;
					ListCell *l;
					int	i = 1;

					obj = resolve_target(cstate, stmt->target,  &fieldname, PLPSM_STMT_DECLARE_CURSOR);
					if (obj->stmt->typ != PLPSM_STMT_DECLARE_CURSOR)
						ereport(ERROR,
								(errcode(ERRCODE_SYNTAX_ERROR),
								 errmsg("variable \"%s\" isn't cursor", obj->name),
									parser_errposition(stmt->target->location)));
					if (obj->cursor.is_for_stmt_cursor)
						ereport(ERROR,
								(errcode(ERRCODE_SYNTAX_ERROR),
								 errmsg("cannot to fetch from cursor related to FOR statement"),
									parser_errposition(stmt->location)));

					SET_OPVAL(fetch.offset, obj->cursor.offset);
					SET_OPVAL(fetch.name, obj->name);
					SET_OPVAL(fetch.nvars, list_length(stmt->compound_target));
					SET_OPVAL(fetch.count, 1);
					EMIT_OPCODE(CURSOR_FETCH, stmt->lineno);

					foreach (l, stmt->compound_target)
					{
						Plpsm_positioned_qualid *qualid = (Plpsm_positioned_qualid *) lfirst(l);
						Plpsm_object	*var;

						var = resolve_target(cstate, qualid,
													    &fieldname,
														    PLPSM_STMT_DECLARE_VARIABLE);

						if (fieldname != NULL)
						{
							int16	typmod = -1;
							Oid	typoid = InvalidOid;
				    			int	fno;

							fno = resolve_composite_field(var->stmt->datum.typoid, var->stmt->datum.typmod, fieldname,
																		    &typoid,
																		    &typmod,
																			    qualid->location);

							SET_OPVAL(update_field.fno, fno);
							SET_OPVAL(update_field.typoid, var->stmt->datum.typoid);
							SET_OPVAL(update_field.typmod, var->stmt->datum.typmod);
							SET_OPVAL(update_field.offset, var->offset);
							SET_OPVAL(update_field.fnumber, i++);
							EMIT_OPCODE(UPDATE_FIELD, stmt->lineno);
						}
						else
						{
							SET_OPVALS_DATUM_INFO(saveto_field, var);
							SET_OPVAL(saveto_field.data, cstate->stack.ndata++);
							SET_OPVAL(saveto_field.fnumber, i++);
							EMIT_OPCODE(SAVETO_FIELD, stmt->lineno);
						}
					}

					compile_refresh_basic_diagnostic(cstate);
				}
				break;

			case PLPSM_STMT_SELECT_INTO:
				{
					compile_multiset_stmt(cstate, stmt, stmt->from_clause);
					compile_refresh_basic_diagnostic(cstate);
				}
				break;

			case PLPSM_STMT_CLOSE:
				{
					const char *fieldname;

					obj = resolve_target(cstate, stmt->target,  &fieldname, PLPSM_STMT_DECLARE_VARIABLE);
					if (obj->stmt->typ != PLPSM_STMT_DECLARE_CURSOR)
						ereport(ERROR,
								(errcode(ERRCODE_SYNTAX_ERROR),
								 errmsg("variable \"%s\" isn't cursor", obj->name),
									parser_errposition(stmt->target->location)));
					if (obj->cursor.is_for_stmt_cursor)
						ereport(ERROR,
								(errcode(ERRCODE_SYNTAX_ERROR),
								 errmsg("cannot to open a cursor used in FOR statement"),
									parser_errposition(stmt->location)));

					SET_OPVAL(cursor.addr, obj->cursor.data_addr);
					SET_OPVAL(cursor.offset, obj->cursor.offset);
					SET_OPVAL(cursor.name, obj->name);
					EMIT_OPCODE(CURSOR_CLOSE, stmt->lineno);
				}
				break;

			case PLPSM_STMT_IF:
				{
					compile_expr(cstate, stmt->esql, NULL, BOOLOID, -1, true);
					addr1 = PC(m);
					EMIT_OPCODE(JMP_FALSE_UNKNOWN, stmt->lineno);
					_compile(cstate, stmt->inner_left, NULL);
					if (stmt->inner_right)
					{
						addr2 = PC(m);
						EMIT_OPCODE(JMP, stmt->lineno);
						SET_OPVAL_ADDR(addr1, addr, PC(m));
						_compile(cstate, stmt->inner_right, NULL);
						SET_OPVAL_ADDR(addr2, addr, PC(m));
					}
					else
						SET_OPVAL_ADDR(addr1, addr, PC(m));
				}
				break;

			case PLPSM_STMT_CASE:
				{
					Plpsm_stmt *outer_case = stmt;
					Plpsm_stmt *cond = stmt->inner_left;
					List	*final_jmps = NIL;
					ListCell	*l;

					while (cond != NULL)
					{
						char *expr;

						if (outer_case->esql != NULL)
						{
							StringInfoData	ds;

							initStringInfo(&ds);
							appendStringInfo(&ds, "%s IN (%s)", outer_case->esql->sqlstr, cond->esql->sqlstr);
							expr = ds.data;
						}
						else
							expr = pstrdup(cond->esql->sqlstr);

						compile_expr(cstate, NULL, expr, BOOLOID, -1, true);
						addr1 = PC(m);
						EMIT_OPCODE(JMP_FALSE_UNKNOWN, stmt->lineno);
						_compile(cstate, cond->inner_left, NULL);
						final_jmps = lappend(final_jmps, makeInteger(PC(m)));
						EMIT_OPCODE(JMP, stmt->lineno);
						SET_OPVAL_ADDR(addr1, addr, PC(m));
						cond = cond->next;
					}

					/* when there are not ELSE path */
					if (outer_case->inner_right == NULL)
					{
						SET_OPVAL(str, "case doesn't match any value");
						EMIT_OPCODE(SIGNAL_NODATA, stmt->lineno);
					}
					else
						_compile(cstate, outer_case->inner_right, NULL);

					/* complete leave jumps */
					foreach (l, final_jmps)
					{
						addr1 = intVal(lfirst(l));
						SET_OPVAL_ADDR(addr1, addr, PC(m));
					}
				}
				break;

			case PLPSM_STMT_PRINT:
				{
					if (list_length(stmt->esql_list) > 1)
					{
						ListCell *l;
						bool	isfirst = true;
						StringInfoData ds;
						int dataidx = cstate->stack.ndata++;
						int i = 1;
						Oid	*argtypes;
						int	nargs;

						initStringInfo(&ds);

						STRBUILDER(INIT, dataidx);

						addr1 = PC(m);
						SET_OPVAL(expr.data, cstate->stack.ndata++);
						SET_OPVAL(expr.is_multicol, true);
						SET_OPVAL(expr.without_diagnostics, true);
						EMIT_OPCODE(EXEC_EXPR, stmt->lineno);
						appendStringInfoString(&ds, "SELECT ");

						foreach (l, stmt->esql_list)
						{
							if (!isfirst)
							{
								appendStringInfoChar(&ds, ',');
								STRBUILDER1(APPEND_CHAR, dataidx, chr, ' ');
							}
							else
								isfirst = false;

							STRBUILDER1(APPEND_FIELD, dataidx, fnumber, i++);

							appendStringInfo(&ds, "(%s)::text", ((Plpsm_ESQL *)(lfirst(l)))->sqlstr);
						}

						SET_OPVAL_ADDR(addr1, expr.expr, replace_vars(cstate, ds.data, &argtypes, &nargs, NULL, stmt->location));
						SET_OPVAL_ADDR(addr1, expr.nparams, nargs);
						SET_OPVAL_ADDR(addr1, expr.typoids, argtypes);

						STRBUILDER(PRINT_FREE, dataidx);
					}
					else
					{
						compile_expr(cstate, linitial(stmt->esql_list), NULL, TEXTOID, -1, false);
						EMIT_OPCODE(PRINT, stmt->lineno);
					}
				}
				break;

			case PLPSM_STMT_SET:
				{
					const char *fieldname;

					if (stmt->target != NULL)
					{
						var = resolve_target(cstate, stmt->target,  &fieldname, PLPSM_STMT_DECLARE_VARIABLE);

						if (fieldname != NULL)
						{
							int16	typmod = -1;
							Oid	typoid = InvalidOid;
				    			int	fno;

							fno = resolve_composite_field(var->stmt->datum.typoid, var->stmt->datum.typmod, fieldname,
																		    &typoid,
																		    &typmod,
																			    stmt->target->location);

							compile_expr(cstate, stmt->esql, NULL, typoid, typmod, true);
							SET_OPVAL(update_field.fno, fno);
							SET_OPVAL(update_field.typoid, var->stmt->datum.typoid);
							SET_OPVAL(update_field.typmod, var->stmt->datum.typmod);
							SET_OPVAL(update_field.offset, var->offset);
							SET_OPVAL(update_field.fnumber, 1);
							EMIT_OPCODE(UPDATE_FIELD, stmt->lineno);
						}
						else
						{
							if (stmt->subscripts != NULL)
							{
								int	i = 0;
								Oid	arraytypeid = var->stmt->datum.typoid;
								int32	arraytypmod = var->stmt->datum.typmod;
								Oid		arrayelemtypid;
								int16		arraytyplen,
												elemtyplen;
								bool		elemtypbyval;
								char		elemtypalign;

								/* If target is domain over array, reduce to base type */
								arraytypeid = getBaseTypeAndTypmod(arraytypeid, &arraytypmod);

								/* ... and identify the element type */
								arrayelemtypid = get_element_type(arraytypeid);
								if (!OidIsValid(arrayelemtypid))
									ereport(ERROR,
											(errcode(ERRCODE_DATATYPE_MISMATCH),
											 errmsg("subscripted object is not an array"),
												parser_errposition(stmt->target->location)));

								get_typlenbyvalalign(arrayelemtypid,
													 &elemtyplen,
													 &elemtypbyval,
													 &elemtypalign);
								arraytyplen = get_typlen(arraytypeid);

								EMIT_OPCODE(SUBSCRIPTS_RESET, -1);
								while (i < MAXDIM)
								{
									if (stmt->subscripts[i] == NULL)
										break;
									compile_expr(cstate, stmt->subscripts[i++], NULL, INT4OID, -1, true);
									EMIT_OPCODE(SUBSCRIPTS_APPEND, -1);
								}

								/* target type should be a element of array */
								compile_expr(cstate, stmt->esql, NULL, arrayelemtypid, arraytypmod, true);

								SET_OPVAL(array_update.offset, var->offset);
								
								SET_OPVAL(array_update.arrayelemtypid, arrayelemtypid);
								SET_OPVAL(array_update.elemtyplen, elemtyplen);
								SET_OPVAL(array_update.elemtypbyval, elemtypbyval);
								SET_OPVAL(array_update.elemtypalign, elemtypalign);
								SET_OPVAL(array_update.arraytyplen, arraytyplen);

								EMIT_OPCODE(ARRAY_UPDATE, -1);

								/* cast to domains when it is necessary */
								if (arraytypeid != var->stmt->datum.typoid || arraytypmod != var->stmt->datum.typmod)
								{
									compile_cast_var(cstate, var->offset, var->stmt->datum.typoid, var->stmt->datum.typmod,
																	    true, true, stmt->lineno);
									SET_OPVALS_DATUM_COPY(target, var);
									EMIT_OPCODE(SAVETO, stmt->lineno);
								}
							}
							else
							{
								compile_expr(cstate, stmt->esql, NULL, var->stmt->datum.typoid, var->stmt->datum.typmod, true);
								SET_OPVALS_DATUM_COPY(target, var);
								EMIT_OPCODE(SAVETO, stmt->lineno);
							}
						}
					}
					else
					{
						Assert(stmt->compound_target != NIL && stmt->esql_list != NIL);
						compile_multiset_stmt(cstate, stmt, NULL);
					}
				}
				break;

			case PLPSM_STMT_ITERATE:
			case PLPSM_STMT_LEAVE:
				{
					if (strcmp(cstate->finfo.name, stmt->name) == 0)
						ereport(ERROR,
								(errcode(ERRCODE_SYNTAX_ERROR),
								 errmsg("cannot leave function by LEAVE statement"),
									parser_errposition(stmt->location)));
					compile_leave_iterate(cstate, cstate->current_scope, stmt);
					break;
				}

			case PLPSM_STMT_RETURN:
				{
					/* leave all compound statements too */
					if (cstate->finfo.result.datum.typoid != VOIDOID)
					{
						if (cstate->finfo.return_expr != NULL && 
							stmt->esql != NULL)
							ereport(ERROR,
								(errcode(ERRCODE_SYNTAX_ERROR),
								 errmsg("using RETURN expr in function with OUT arguments"),
									parser_errposition(stmt->location)));

						if (stmt->option == PLPSM_RETURN_EXPR)
						{
							Oid	typoid = cstate->finfo.result.datum.typoid;
							int16	typmod = -1;
							int16	typlen = cstate->finfo.result.datum.typlen;
							bool	typbyval = cstate->finfo.result.datum.typbyval;

							if (cstate->finfo.result.datum.typoid == TRIGGEROID)
							{
								/* old or new trigger var should be declared */
								if (cstate->trigger.var_new != NULL)
								{
									typoid = cstate->trigger.var_new->stmt->datum.typoid; 
									typmod = cstate->trigger.var_new->stmt->datum.typmod; 
									typlen = -1;
									typbyval = false;
								}
								else if (cstate->trigger.var_old != NULL)
								{
									typoid = cstate->trigger.var_old->stmt->datum.typoid; 
									typmod = cstate->trigger.var_old->stmt->datum.typmod; 
									typlen = -1;
									typbyval = false;
								}
								else
									ereport(ERROR,
											(errcode(ERRCODE_SYNTAX_ERROR),
											 errmsg("target type of this trigger function isn't known yet"),
											 errhint("Declare OLD or NEW trigger variables."),
													parser_errposition(stmt->location)));
							}
						
							if (stmt->esql != NULL)
								compile_expr(cstate, stmt->esql, NULL, typoid, typmod, false);
							else
								compile_expr(cstate, NULL, cstate->finfo.return_expr,
												typoid, typmod, false);

							SET_OPVAL(target.typlen, typlen);
							SET_OPVAL(target.typbyval, typbyval);
							EMIT_OPCODE(RETURN, stmt->lineno);
						}
						else
						{
							int	nargs;
							Oid	*argtypes;
							StringInfoData ds;
							StringInfoData		aliases;
							StringInfoData 		targets;
							int	i;
							bool		isfirst = true;

							if (!cstate->allow_return_query)
								ereport(ERROR,
									(errcode(ERRCODE_SYNTAX_ERROR),
									 errmsg("RETURN SELECT is allowed only for SET returning functions"),
											parser_errposition(stmt->location)));

							Assert(cstate->finfo.result_desc != NULL);

							initStringInfo(&ds);
							initStringInfo(&aliases);
							initStringInfo(&targets);

							for(i = 0; i < cstate->finfo.result_desc->natts; i++)
							{
								Form_pg_attribute att;

								if (!isfirst)
								{
									appendStringInfoChar(&aliases, ',');
									appendStringInfoChar(&targets, ',');
								}
								else
									isfirst = false;

								/*
								 * initialize the attribute fields
								 */
								att = cstate->finfo.result_desc->attrs[i];
								appendStringInfo(&targets, "___rt_%d.___%d::%s", cstate->stack.ndata, i, 
																format_type_with_typemod(att->atttypid,
																			 att->atttypmod));
								appendStringInfo(&aliases, "___%d", i);
							}

							appendStringInfo(&ds, "SELECT %s FROM (%s) ___rt_%d (%s)",
														targets.data,
														stmt->esql->sqlstr, 
														cstate->stack.ndata,
														aliases.data);

							/* I don't convert query, because tuple convertor is used */
							SET_OPVAL(expr.expr, replace_vars(cstate, ds.data, &argtypes, &nargs, NULL, -1));
							SET_OPVAL(expr.nparams, nargs);
							SET_OPVAL(expr.typoids, argtypes);
							SET_OPVAL(expr.data, cstate->stack.ndata++);
							SET_OPVAL(expr.is_multicol, true);

							pfree(aliases.data);
							pfree(targets.data);

							/*
							 * refresh state variables
							 */
							if (cstate->stack.has_sqlstate)
							{
								Plpsm_object *var = lookup_var(cstate->current_scope, "sqlstate");
								Assert(var != NULL);
								SET_OPVALS_DATUM_COPY(target, var);
								EMIT_OPCODE(SQLSTATE_REFRESH, -1);
							}

							if (cstate->stack.has_sqlcode)
							{
								Plpsm_object *var = lookup_var(cstate->current_scope, "sqlcode");
								Assert(var != NULL);
								SET_OPVALS_DATUM_COPY(target, var);
								EMIT_OPCODE(SQLCODE_REFRESH, -1);
							}

							EMIT_OPCODE(RETURN_QUERY, stmt->lineno);
						}
					}
					else
					{
						Assert(cstate->finfo.return_expr == NULL);
						if (stmt->esql != NULL)
							elog(ERROR, "returned a value in VOID function");
						EMIT_OPCODE(RETURN_VOID, stmt->lineno);
					}
				}
				break;

			case PLPSM_STMT_EXECUTE_IMMEDIATE:
				{
					compile_expr(cstate, stmt->esql, NULL, TEXTOID, -1, true);
					EMIT_OPCODE(EXECUTE_IMMEDIATE, stmt->lineno);
					compile_refresh_basic_diagnostic(cstate);
				}
				break;

			case PLPSM_STMT_SQL:
				{
					Oid	*argtypes;
					int	nargs;
				
					SET_OPVAL(expr.expr, replace_vars(cstate, stmt->esql->sqlstr, &argtypes, &nargs, NULL, stmt->esql->location));
					SET_OPVAL(expr.nparams, nargs);
					SET_OPVAL(expr.typoids, argtypes);
					SET_OPVAL(expr.data, cstate->stack.ndata++);
					EMIT_OPCODE(EXEC_QUERY, stmt->lineno);
					compile_refresh_basic_diagnostic(cstate);
				}
				break;

			case PLPSM_STMT_EXECUTE:
				{
					int dataidx = -1;
				
					if (stmt->variables != NIL)
					{
						compile_usage_clause(cstate, stmt, &dataidx);
						SET_OPVAL(execute.params, dataidx);
					}
					else
						SET_OPVAL(execute.params, -1);

					SET_OPVAL(execute.name, stmt->name);
					SET_OPVAL(execute.sqlstr, fetchPrepared(cstate, stmt->name));
					EMIT_OPCODE(EXECUTE, stmt->lineno);

					if (dataidx != -1)
						PARAMBUILDER(FREE, dataidx);

					if (stmt->compound_target != NIL)
					{
						const char *fieldname;
						int	i = 1;
						ListCell *l;

						EMIT_OPCODE(CHECK_DATA, stmt->lineno);
						foreach (l, stmt->compound_target)
						{
							Plpsm_object	*var = resolve_target(cstate, (Plpsm_positioned_qualid *) lfirst(l),
														    &fieldname,
																    PLPSM_STMT_DECLARE_VARIABLE);
							SET_OPVALS_DATUM_INFO(saveto_field, var);
							SET_OPVAL(saveto_field.data, cstate->stack.ndata++);
							SET_OPVAL(saveto_field.fnumber, i++);
							EMIT_OPCODE(SAVETO_FIELD, stmt->lineno);
						}
					}
					compile_refresh_basic_diagnostic(cstate);
				}
				break;

			case PLPSM_STMT_PREPARE:
				{
					int	prepnum = fetchPrepared(cstate, stmt->name);
					compile_expr(cstate, stmt->esql, NULL, TEXTOID, -1, true);
					SET_OPVAL(prepare.name, stmt->name);
					SET_OPVAL(prepare.data, prepnum);
					EMIT_OPCODE(PREPARE, stmt->lineno);
				}
				break;

			case PLPSM_STMT_SIGNAL:
			case PLPSM_STMT_RESIGNAL:
				{
					bool	is_resignal = stmt->typ == PLPSM_STMT_RESIGNAL;

					if (!cstate->stack.inside_handler && is_resignal)
						ereport(ERROR,
								(errcode(ERRCODE_SYNTAX_ERROR),
								 errmsg("cannot use a RESIGNAL statement outside condition handler"),
									parser_errposition(stmt->location)));

					if (is_resignal)
					{
						SET_OPVAL(signal_property.typ, PLPSM_SIGNAL_PROPERTY_LOAD_STACKED);
						EMIT_OPCODE(SIGNAL_PROPERTY, -1);
					}
					else
					{
						SET_OPVAL(signal_property.typ, PLPSM_SIGNAL_PROPERTY_RESET);
						EMIT_OPCODE(SIGNAL_PROPERTY, -1);
					}

					if (!is_resignal || (is_resignal && (stmt->option != 0 || stmt->name != NULL)))
					{
						Plpsm_object *handler;
						Plpsm_condition_value condition;
						char *condition_name = NULL;
						int	derivated_sqlstate = 0;

						if (stmt->name != NULL)
						{
							condition.typ = PLPSM_CONDITION_NAME;
							condition.location = stmt->location;
							condition.condition_name = stmt->name;
							condition.derivated_sqlstate = lookup_condition(cstate->current_scope, &condition);

							condition_name = condition.condition_name;
							derivated_sqlstate = condition.derivated_sqlstate;
						}
						else
						{
							Assert(stmt->option != 0);
							condition.typ = PLPSM_SQLSTATE;
							condition.sqlstate = stmt->option;
							condition.next = NULL;
						}

						handler = lookup_handler(cstate->current_scope, &condition);
						if (handler != NULL)
						{
							/* when handler is undo or exit, generate a leave steps */
							if (handler->stmt->option != PLPSM_HANDLER_CONTINUE)
							{
								compile_leave_target_block(cstate, cstate->current_scope, handler->outer);
								compile_signal(cstate, stmt, handler->calls.entry_addr, PCODE_SIGNAL_JMP,
											handler->stmt->option == PLPSM_HANDLER_UNDO, is_resignal,
												condition_name, derivated_sqlstate);
							}
							else
							{
								compile_signal(cstate, stmt, handler->calls.entry_addr, PCODE_SIGNAL_CALL,
											handler->stmt->option == PLPSM_HANDLER_UNDO, is_resignal,
												condition_name, derivated_sqlstate);
							}
						}
						else
						{
							/* there are no local handler */
							compile_signal(cstate, stmt, 0, PCODE_SIGNAL_JMP, false, is_resignal,
											condition_name, derivated_sqlstate);
						}
					}
					else
					{
						Plpsm_signal_info *sinfo = (Plpsm_signal_info *) stmt->data;

						/*
						 * resignal without known sqlstate, we have to put REFRESH routines
						 * after statement.
						 */
						while (sinfo != NULL)
						{
							switch (sinfo->typ)
							{
								case PLPSM_SINFO_DETAIL:
									SIGNAL_PROPERTY(SET_CSTRING, DETAIL, cstr, pstrdup(sinfo->value));
									break;
								case PLPSM_SINFO_HINT:
									SIGNAL_PROPERTY(SET_CSTRING, HINT, cstr, pstrdup(sinfo->value));
									break;
								case PLPSM_SINFO_MESSAGE:
									SIGNAL_PROPERTY(SET_CSTRING, MESSAGE, cstr, pstrdup(sinfo->value));
									break;
							}
							sinfo = sinfo->next;
						}

						if (cstate->stack.has_sqlstate)
						{
							Plpsm_object *var = lookup_var(cstate->current_scope, "sqlstate");
							Assert(var != NULL);
							SET_OPVALS_DATUM_COPY(target, var);
							EMIT_OPCODE(SQLSTATE_REFRESH, -1);
						}

						if (cstate->stack.has_sqlcode)
						{
							Plpsm_object *var = lookup_var(cstate->current_scope, "sqlcode");
							Assert(var != NULL);
							SET_OPVALS_DATUM_COPY(target, var);
							EMIT_OPCODE(SQLCODE_REFRESH, -1);
						}

						EMIT_OPCODE(RESIGNAL_JMP, stmt->lineno);
					}
				}
				break;

			case PLPSM_STMT_GET_DIAGNOSTICS:
				{
					Plpsm_diagnostics_area which_type = (Plpsm_diagnostics_area) stmt->option;
					Plpsm_gd_info *iterator = (Plpsm_gd_info *) stmt->data;
					const char	*fieldname;

					while (iterator != NULL)
					{
						Plpsm_object *var = resolve_target(cstate, iterator->target,  &fieldname, PLPSM_STMT_DECLARE_VARIABLE);

						if (fieldname != NULL)
							ereport(ERROR,
									(errcode(ERRCODE_SYNTAX_ERROR),
									 errmsg("cannot use a composite variables inside GET DIAGNOSTICS statement"),
										parser_errposition(stmt->location)));

						SET_OPVAL(get_diagnostics.which_area, which_type);
						SET_OPVAL(get_diagnostics.typ, iterator->typ);
						SET_OPVAL(get_diagnostics.offset, var->offset);
						SET_OPVAL(get_diagnostics.byval, var->stmt->datum.typbyval);
						SET_OPVAL(get_diagnostics.target_type, var->stmt->datum.typoid);

						switch (iterator->typ)
						{
							case PLPSM_GDINFO_DETAIL:
							case PLPSM_GDINFO_HINT:
							case PLPSM_GDINFO_MESSAGE:
							case PLPSM_GDINFO_SQLSTATE:
							case PLPSM_GDINFO_CONDITION_IDENTIFIER:
								if (var->stmt->datum.typoid != TEXTOID)
									ereport(ERROR,
											(errcode(ERRCODE_SYNTAX_ERROR),
											 errmsg("target of MESSAGE_TEXT, DETAIL_TEXT, HINT_TEXT or SQLSTATE should be text type"),
												parser_errposition(stmt->location)));
								break;
							case PLPSM_GDINFO_SQLCODE:
							case PLPSM_GDINFO_ROW_COUNT:
								if (var->stmt->datum.typoid != INT4OID && var->stmt->datum.typoid != INT8OID)
									ereport(ERROR,
											(errcode(ERRCODE_SYNTAX_ERROR),
											 errmsg("target of SQLCODE or ROW_COUNT should be text int or big int"),
												parser_errposition(stmt->location)));
								break;
							case PLPSM_GDINFO_LEVEL:
								/* do nothing */;
						}

						EMIT_OPCODE(GET_DIAGNOSTICS, stmt->lineno);
						iterator = iterator->next;
					}
				}
				break;

			default:
				elog(ERROR, "unknown command typeid");
		}
		stmt = stmt->next;
	}

	/* returns back TH pointer */
	cstate->stack.ht_entry = old_th_addr;

	/* return back info about handler's compilation */
	cstate->stack.inside_handler = inside_handler;
}

/*
 * Prepare Handler's table fields for compound statement
 */
static int
compile_ht(CompileState cstate, Plpsm_stmt *compound, int parent_addr)
{
	Plpsm_ht_table *m = cstate->ht_table;	
	Plpsm_stmt *iterator;
	bool	handler_found = false;
			Plpsm_condition_value *c;

	Assert(compound->typ == PLPSM_STMT_COMPOUND_STATEMENT);

	/* we have to detect, if there are handlers */
	iterator = compound->inner_left;
	while (iterator != NULL)
	{
		if (iterator->typ == PLPSM_STMT_DECLARE_VARIABLE || iterator->typ == PLPSM_STMT_DECLARE_CURSOR || 
				iterator->typ == PLPSM_STMT_DECLARE_CONDITION)
		{
			iterator = iterator->next;
			continue;
		}
		else if (iterator->typ == PLPSM_STMT_DECLARE_HANDLER)
		{
			Plpsm_condition_value *condition = (Plpsm_condition_value *) iterator->data;
			handler_found = true;

			/* search conditions */
			while (condition != NULL)
			{
				/* when condtion is specified by name, then we should to get derivated sqlstate */
				if (condition->typ == PLPSM_CONDITION_NAME)
					condition->derivated_sqlstate = lookup_condition(cstate->current_scope, condition);
				condition = condition->next;
			}

			iterator->ht_info.addr1 = 0;
			iterator->ht_info.addr2 = 0;

			iterator = iterator->next;
		}
		else
			break;
	}

	if (!handler_found)
		return parent_addr;

	/* there is some handlers */
	if (m == NULL)
	{
		m = palloc0(128 * sizeof(Plpsm_pcode) + offsetof(Plpsm_ht_table, code));
		m->mlength = 128;
		m->length = 1;
		cstate->ht_table = m;

		EMIT_HT_FIELD(STOP, -1);
	}
	else
	{
		SET_OPVAL(HT_field.parent_HT_addr, parent_addr);
		EMIT_HT_FIELD(PARENT, -1);
	}

	/* 
	 * We know, so we doesn't use any handler from compound statement, 
	 * so we have to POP stacked diagnostics info.
	 */
	if (cstate->use_stacked_diagnostics)
	{
		EMIT_HT_FIELD(DIAGNOSTICS_POP, -1);
	}

	/* release a subtransaction when leave a ATOMIC compound statement */
	if ((bool) compound->option)
	{
		EMIT_HT_FIELD(RELEASE_SUBTRANSACTION, -1);
	}

	/*
	 * there should be two independent itereation, because there are
	 * two different orders. First we iterate over handler in this
	 * statement in order from less general to general conditions.
	 * Second, we should to iterate over nested Plpsm_objects.
	 *
	 * Seraching a generic handlers
	 */
	iterator = compound->inner_left;
	while (iterator != NULL)
	{
		if (iterator->typ == PLPSM_STMT_DECLARE_VARIABLE || iterator->typ == PLPSM_STMT_DECLARE_CURSOR ||
			iterator->typ == PLPSM_STMT_DECLARE_CONDITION)
		{
			iterator = iterator->next;
			continue;
		}
		else if (iterator->typ == PLPSM_STMT_DECLARE_HANDLER)
		{
			c = (Plpsm_condition_value *) iterator->data;
			while (c != NULL)
			{
				if (c->typ == PLPSM_SQLWARNING)
				{
					SET_OPVAL(HT_field.htyp, iterator->option);
					iterator->ht_info.addr1 = PC(m);
					EMIT_HT_FIELD(SQLWARNING, iterator->lineno);
				}
				else if (c->typ == PLPSM_SQLEXCEPTION)
				{
					SET_OPVAL(HT_field.htyp, iterator->option);
					iterator->ht_info.addr1 = PC(m);
					EMIT_HT_FIELD(SQLEXCEPTION, iterator->lineno);
				}
				c = c->next;
			}
			iterator = iterator->next;
		}
		else
			break;
	}

	/* searching a class handlers */
	iterator = compound->inner_left;
	while (iterator != NULL)
	{
		if (iterator->typ == PLPSM_STMT_DECLARE_VARIABLE || iterator->typ == PLPSM_STMT_DECLARE_CURSOR ||
			iterator->typ == PLPSM_STMT_DECLARE_CONDITION)
		{
			iterator = iterator->next;
			continue;
		}
		else if (iterator->typ == PLPSM_STMT_DECLARE_HANDLER)
		{
			c = (Plpsm_condition_value *) iterator->data;
			while (c != NULL)
			{
				if (c->typ == PLPSM_SQLSTATE && ERRCODE_IS_CATEGORY(c->sqlstate))
				{
					SET_OPVAL(HT_field.htyp, iterator->option);
					SET_OPVAL(HT_field.sqlclass, c->sqlstate);
					iterator->ht_info.addr1 = PC(m);
					EMIT_HT_FIELD(SQLCLASS, iterator->lineno);
				}
				else if (c->typ == PLPSM_CONDITION_NAME && c->derivated_sqlstate != 0 &&
					ERRCODE_IS_CATEGORY(c->derivated_sqlstate))
				{
					SET_OPVAL(HT_field.htyp, iterator->option);
					SET_OPVAL(HT_field.sqlclass, c->derivated_sqlstate);
					iterator->ht_info.addr1 = PC(m);
					EMIT_HT_FIELD(SQLCLASS, iterator->lineno);
				}
				c = c->next;
			}
			iterator = iterator->next;
		}
		else
			break;
	}

	/* searching a sqlstate handlers */
	iterator = compound->inner_left;
	while (iterator != NULL)
	{
		if (iterator->typ == PLPSM_STMT_DECLARE_VARIABLE || iterator->typ == PLPSM_STMT_DECLARE_CURSOR ||
			iterator->typ == PLPSM_STMT_DECLARE_CONDITION)
		{
			iterator = iterator->next;
			continue;
		}
		else if (iterator->typ == PLPSM_STMT_DECLARE_HANDLER)
		{
			c = (Plpsm_condition_value *) iterator->data;
			while (c != NULL)
			{
				if (c->typ == PLPSM_SQLSTATE && !ERRCODE_IS_CATEGORY(c->sqlstate))
				{
					SET_OPVAL(HT_field.htyp, iterator->option);
					SET_OPVAL(HT_field.sqlcode, c->sqlstate);
					iterator->ht_info.addr1 = PC(m);
					EMIT_HT_FIELD(SQLCODE, iterator->lineno);
				}
				else if (c->typ == PLPSM_CONDITION_NAME && c->derivated_sqlstate != 0
					&& !ERRCODE_IS_CATEGORY(c->derivated_sqlstate))
				{
					SET_OPVAL(HT_field.htyp, iterator->option);
					SET_OPVAL(HT_field.sqlcode, c->derivated_sqlstate);
					iterator->ht_info.addr1 = PC(m);
					EMIT_HT_FIELD(SQLCODE, iterator->lineno);
				}

				/* special entry, when handler is described by condition name */
				if (c->typ == PLPSM_CONDITION_NAME)
				{
					SET_OPVAL(HT_field.htyp, iterator->option);
					SET_OPVAL(HT_field.condition_name, pstrdup(c->condition_name));
					iterator->ht_info.addr2 = PC(m);
					EMIT_HT_FIELD(CONDITION_NAME, iterator->lineno);
				}
				c = c->next;
			}

			iterator = iterator->next;
		}
		else
			break;
	}

	return m->length - 1;
}

Plpsm_module *
plpsm_compile(FunctionCallInfo fcinfo, bool forValidator)
{
	Oid			funcOid = fcinfo->flinfo->fn_oid;
	HeapTuple	procTup;
	Form_pg_proc procStruct;
	Plpsm_module *module;
	Plpsm_module_hashkey hashkey;
	bool		module_valid = false;
	bool		hashkey_valid = false;


	procTup = SearchSysCache1(PROCOID, ObjectIdGetDatum(funcOid));
	if (!HeapTupleIsValid(procTup))
		elog(ERROR, "cache lookup failed for function %u", funcOid);
	procStruct = (Form_pg_proc) GETSTRUCT(procTup);

	/*
	 * See if there's already a cache entry for the current FmgrInfo. If not,
	 * try to find one in the hash table.
	 */
	module = (Plpsm_module *) fcinfo->flinfo->fn_extra;

recheck:
	if (!module)
	{
		/* Compute hashkey using function signature and actual arg types */
		compute_module_hashkey(fcinfo, procStruct, &hashkey, forValidator);
		hashkey_valid = true;

		/* And do the lookup */
		module = plpsm_HashTableLookup(&hashkey);
	}

	if (module)
	{
		if (module->xmin == HeapTupleHeaderGetXmin(procTup->t_data) &&
			ItemPointerEquals(&module->tid, &procTup->t_self))
			module_valid = true;
		else
		{
			delete_module(module);
			if (module->use_count != 0)
			{
				module = NULL;
				if (!hashkey_valid)
					goto recheck;
			}
		}
	}

	/*
	 * if module isn't valid still, compile it 
	 */
	if (!module_valid)
	{
		if (!hashkey_valid)
			compute_module_hashkey(fcinfo, procStruct, &hashkey,
									forValidator);
		module = compile(fcinfo, procTup, module,
							    &hashkey, forValidator);
	}

	ReleaseSysCache(procTup);

	/*
	 * Save pointer in FmgrInfo to avoid search on subsequent calls 
	 */
	fcinfo->flinfo->fn_extra = (void *) module;

	return module;
}

static Plpsm_module *
compile(FunctionCallInfo fcinfo, HeapTuple procTup, Plpsm_module *module, Plpsm_module_hashkey *hashkey, bool forValidator)
{
	int parse_rc;
	Form_pg_proc procStruct;
	char	*proc_source;
	Datum	prosrcdatum;
	bool		isnull;
	CompileStateData cstated;
	CompileState	cstate;
	Plpsm_object *outer_scope;
	int			numargs;
	Oid		   *argtypes;
	char	  **argnames;
	char	   *argmodes;
	int	i;
	int16	typlen;
	bool	typbyval;
	Bitmapset	*outargs = NULL;
	ErrorContextCallback	plerrcontext;
	MemoryContext	func_cxt;
	Plpsm_pcode_module *m;
	Oid rettypeid;
	int		out_nargs = 0;
	TupleDesc	out_tupdesc = NULL;

	ParserStateData parser_state_var;

	procStruct = (Form_pg_proc) GETSTRUCT(procTup);

	prosrcdatum = SysCacheGetAttr(PROCOID, procTup,
							Anum_pg_proc_prosrc, &isnull);
	if (isnull)
		elog(ERROR, "null prosrc");

	if (module == NULL)
	{
		module = (Plpsm_module *)
			MemoryContextAllocZero(TopMemoryContext, sizeof(Plpsm_module));

	}
	else
	{
		memset(module, 0, sizeof(Plpsm_module));
	}

	func_cxt = AllocSetContextCreate(TopMemoryContext,
							    "PLPSM function context",
							    ALLOCSET_DEFAULT_MINSIZE,
							    ALLOCSET_DEFAULT_INITSIZE,
							    ALLOCSET_DEFAULT_MAXSIZE);
	plpsm_compile_tmp_cxt = MemoryContextSwitchTo(func_cxt);

	outer_scope = palloc0(sizeof(Plpsm_object));

	module->oid = fcinfo->flinfo->fn_oid;
	module->xmin = HeapTupleHeaderGetXmin(procTup->t_data);
	module->tid = procTup->t_self;
	module->cxt = func_cxt;
	module->with_cframe_debug = plpsm_debug_info;
	module->ht_table = NULL;

	m = init_module();
	cstated.module = m;
	m->name = pstrdup(NameStr(procStruct->proname));
	m->is_read_only = (procStruct->provolatile != PROVOLATILE_VOLATILE);

	proc_source = TextDatumGetCString(prosrcdatum);

	parser_state_var.has_get_diagnostics_stmt = false;
	parser_state_var.has_get_stacked_diagnostics_stmt = false;
	parser_state_var.has_resignal_stmt = false;
	parser_state_var.has_trigger_variable_new = false;
	parser_state_var.has_trigger_variable_old = false;
	pstate = &parser_state_var;

	plerrcontext.callback = plpsm_compile_error_callback;
	plerrcontext.arg = forValidator ? proc_source : NULL;
	plerrcontext.previous = error_context_stack;
	error_context_stack = &plerrcontext;

	plpsm_error_funcname = NameStr(procStruct->proname);

	plpsm_scanner_init(proc_source);
	parse_rc = plpsm_yyparse();
	if (parse_rc != 0)
		elog(ERROR, "plpsm parser returned %d", parse_rc);

	outer_scope->typ = PLPSM_STMT_COMPOUND_STATEMENT;
	outer_scope->name = pstrdup(NameStr(procStruct->proname));

	cstate = &cstated;

	cstated.top_scope = outer_scope;
	cstated.top_scope->name = outer_scope->name;
	cstated.current_scope = cstated.top_scope;

	cstated.stack.ndata = 0;
	cstated.stack.ht_entry = 0;
	cstated.stack.ndatums = 0;
	cstated.stack.oids.size = 128;
	cstated.stack.oids.data = (Oid *) palloc(cstated.stack.oids.size * sizeof(Oid));
	cstated.stack.has_sqlstate = false;
	cstated.stack.has_sqlcode = false;
	cstated.stack.has_notfound_continue_handler = false;
	cstated.stack.inside_handler = false;

	cstated.trigger.addr1 = 0;
	cstated.trigger.addr2 = 0;
	cstated.trigger.var_new = NULL;
	cstated.trigger.var_old = NULL;

	cstated.prepared = NULL;
	cstated.ht_table = NULL;
	cstated.use_stacked_diagnostics = false;

	cstated.finfo.result.datum.typoid = procStruct->prorettype;
	get_typlenbyval(cstated.finfo.result.datum.typoid, &cstated.finfo.result.datum.typlen, &cstated.finfo.result.datum.typbyval);

	if (plpsm_debug_info)
	{
		SET_OPVAL(str, pstrdup(proc_source));
		EMIT_OPCODE(DEBUG_SOURCE_CODE, -1);
	}

	/*
	 * initialise diagnostics when is used
	 */
	if (pstate->has_resignal_stmt || pstate->has_get_stacked_diagnostics_stmt ||
		 pstate->has_get_diagnostics_stmt)
	{
		if (pstate->has_resignal_stmt || pstate->has_get_stacked_diagnostics_stmt)
			cstated.use_stacked_diagnostics = true;

		SET_OPVAL(use_stacked_diagnostics, cstated.use_stacked_diagnostics);
		EMIT_OPCODE(DIAGNOSTICS_INIT, -1);
	}

	/* initialise a output tuple store */
	if (procStruct->proretset)
	{
		cstated.allow_return_query = true;
		EMIT_OPCODE(INIT_TUPLESTORE, -1);
	}

	/* 
	 * append to scope a variables for parameters, and store 
	 * instruction for copy from fcinfo
	 */
	numargs = get_func_arg_info(procTup,
						&argtypes, &argnames, &argmodes);
	cstated.finfo.nargs = numargs;
	cstated.finfo.name = m->name;
	cstated.finfo.source = proc_source;

	rettypeid = procStruct->prorettype;

	if (parser_state_var.has_trigger_variable_new)
	{
		cstated.trigger.addr1 = PC(m);
		EMIT_OPCODE(INIT_TRIGGER_VAR, -1);
	}

	if (parser_state_var.has_trigger_variable_old)
	{
		cstated.trigger.addr2 = PC(m);
		EMIT_OPCODE(INIT_TRIGGER_VAR, -1);
	}

	for (i = 0; i < numargs; i++)
	{
		Oid			argtypid = argtypes[i];
		char	argmode = argmodes ? argmodes[i] : PROARGMODE_IN;
		Plpsm_stmt *decl_stmt;
		char		buf[32];
		Plpsm_object *var;
		Plpsm_object *alias;

		if (argmode == PROARGMODE_TABLE)
		{
			Assert(argnames && argnames[i][0] != '\0');

			if (out_tupdesc == NULL)
				out_tupdesc = CreateTemplateTupleDesc(numargs, false);

			TupleDescInitEntry(out_tupdesc, 1 + out_nargs++, argnames[i],
										argtypid, 
											-1, 0);
			continue;
		}

		get_typlenbyval(argtypid, &typlen, &typbyval);

		/* Create $n name for variable */
		snprintf(buf, sizeof(buf), "$%d", i + 1);

		/* append a fake statements for parameter variable */
		decl_stmt = plpsm_new_stmt(PLPSM_STMT_DECLARE_VARIABLE, -1);
		decl_stmt->target = new_qualid(list_make1(pstrdup(buf)), -1);

		decl_stmt->datum.typoid = argtypid;
		decl_stmt->datum.typmod = -1;
		decl_stmt->datum.typname = NULL;
		decl_stmt->datum.typlen = typlen;
		decl_stmt->datum.typbyval = typbyval;

		/* append implicit name to scope */
		var = create_variable_for(decl_stmt, &cstated, decl_stmt->target, NULL, PLPSM_VARIABLE);
		/* append explicit name to scope */
		if (argnames && argnames[i][0] != '\0')
		{
			alias = create_variable_for(decl_stmt, &cstated, NULL, pstrdup(argnames[i]), PLPSM_REFERENCE);
			alias->offset = var->offset; 
		}

		if (argmode == PROARGMODE_OUT ||
			argmode == PROARGMODE_INOUT)
			outargs = bms_add_member(outargs, i);

		/* append a initialization instruction */
		if (argmode == PROARGMODE_IN || 
			argmode == PROARGMODE_INOUT ||
			argmode == PROARGMODE_VARIADIC)
		{
			SET_OPVAL(copyto.src, i);
			SET_OPVAL(copyto.dest, var->offset);
			SET_OPVAL(copyto.typlen, var->stmt->datum.typlen);
			SET_OPVAL(copyto.typbyval, var->stmt->datum.typbyval);
			EMIT_OPCODE(COPY_PARAM, -1);
		}
		else
		{
			/* initialize OUT variables to NULL */
			SET_OPVAL(target.offset, var->offset);
			EMIT_OPCODE(SET_NULL, -1);
		}
	}

	if (out_nargs > 0)
	{
		Assert(out_tupdesc != 0);
		out_tupdesc->natts = out_nargs;
		cstated.finfo.result_desc = out_tupdesc;
	}

	if (outargs != NULL)
	{
		StringInfoData ds;

		initStringInfo(&ds);
		if (bms_num_members(outargs) > 1)
		{
			bool first = true;
			int	paramno;

			while ((paramno = bms_first_member(outargs)) >= 0)
			{
				if (first)
				{
					first = false;
					appendStringInfo(&ds, "($%d", paramno + 1);
				}
				else
					appendStringInfo(&ds, ",$%d", paramno + 1);
			}
			appendStringInfoChar(&ds, ')');
		}
		else
			appendStringInfo(&ds, "$%d", bms_singleton_member(outargs) + 1);
		cstated.finfo.return_expr = ds.data;

		bms_free(outargs);
	}
	else
		cstated.finfo.return_expr = NULL;

	_compile(&cstated, plpsm_parser_tree, NULL);
	compile_done(&cstated);

	m->ndatums = cstated.stack.ndatums;
	m->ndata = cstated.stack.ndata;

	module->code = m;
	module->ht_table = cstated.ht_table;

	/* prepare a persistent memory */
	module->DataPtrs = palloc0((m->ndata + 1) * sizeof(void*) );

	if (plpsm_debug_compiler)
	{
		list(m);
		if (module->ht_table != NULL)
			list_ht_table(module->ht_table);
	}

	plpsm_HashTableInsert(module, hashkey);

	error_context_stack = plerrcontext.previous;
	plpsm_error_funcname = NULL;

	plpsm_scanner_finish();
	pfree(proc_source);

	MemoryContextSwitchTo(plpsm_compile_tmp_cxt);
	plpsm_compile_tmp_cxt = NULL;

	return module;
}

/*
 * error context callback
 */
static void
plpsm_compile_error_callback(void *arg)
{
	if (arg)
	{
		if (function_parse_error_transpose((const char *) arg))
			return;
	}

	errcontext("compilation of PLPSM function \"%s\" near line %d",
				plpsm_error_funcname, plpsm_latest_lineno());
}

/*
 * Compute a module's hashkey
 */
static void
compute_module_hashkey(FunctionCallInfo fcinfo,
						    Form_pg_proc procStruct,
						    Plpsm_module_hashkey *key,
						    bool forValidator)
{
	/* Make sure any unused bytes of the struct are zero */
	MemSet(key, 0, sizeof(Plpsm_module_hashkey));

	/* get function OID */
	key->oid = fcinfo->flinfo->fn_oid;

	/* get call context */
	key->isTrigger = CALLED_AS_TRIGGER(fcinfo);

	/*
	 * if trigger, get relation OID.  In validation mode we do not know what
	 * relation is intended to be used, so we leave trigrelOid zero; the hash
	 * entry built in this case will never really be used.
	 */
	if (key->isTrigger && !forValidator)
	{
		TriggerData *trigdata = (TriggerData *) fcinfo->context;

		key->trigrelOid = RelationGetRelid(trigdata->tg_relation);
	}
}

static void
delete_module(Plpsm_module *mod)
{
	plpsm_HashTableDelete(mod);

	if (mod->use_count == 0 && mod->cxt)
	{
		MemoryContextDelete(mod->cxt);
		mod->cxt = NULL;
	}
}

/* exported so we can call it from plpsm_init() */
void
plpsm_HashTableInit(void)
{
	HASHCTL		ctl;

	/* don't allow double-initialization */
	Assert(plpsm_HashTable == NULL);

	memset(&ctl, 0, sizeof(ctl));
	ctl.keysize = sizeof(Plpsm_module_hashkey);
	ctl.entrysize = sizeof(Plpsm_HashEnt);
	ctl.hash = tag_hash;
	plpsm_HashTable = hash_create("PLPSM function cache",
									FUNCS_PER_USER,
									&ctl,
									HASH_ELEM | HASH_FUNCTION);
}

static Plpsm_module *
plpsm_HashTableLookup(Plpsm_module_hashkey *func_key)
{
	Plpsm_HashEnt *hentry;

	hentry = (Plpsm_HashEnt *) hash_search(plpsm_HashTable,
											 (void *) func_key,
											 HASH_FIND,
											 NULL);
	if (hentry)
		return hentry->module;
	else
		return NULL;
}

static void
plpsm_HashTableInsert(Plpsm_module *module,
						Plpsm_module_hashkey *func_key)
{
	Plpsm_HashEnt *hentry;
	bool		found;

	hentry = (Plpsm_HashEnt *) hash_search(plpsm_HashTable,
											 (void *) func_key,
											 HASH_ENTER,
											 &found);
	if (found)
		elog(WARNING, "trying to insert a function that already exists");

	hentry->module = module;
	/* prepare back link from function to hashtable key */
	module->hashkey = &hentry->hashkey;
}

static void
plpsm_HashTableDelete(Plpsm_module *module)
{
	Plpsm_HashEnt *hentry;

	/* do nothing if not in table */
	if (module->hashkey == NULL)
		return;

	hentry = (Plpsm_HashEnt *) hash_search(plpsm_HashTable,
											 (void *) module->hashkey,
											 HASH_REMOVE,
											 NULL);
	if (hentry == NULL)
		elog(WARNING, "trying to delete function that does not exist");

	/* remove back link, which no longer points to allocated storage */
	module->hashkey = NULL;
}
