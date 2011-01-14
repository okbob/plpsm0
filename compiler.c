#include "psm.h"

#include "funcapi.h"
#include "catalog/namespace.h"
#include "catalog/pg_proc.h"
#include "catalog/pg_type.h"
#include "nodes/bitmapset.h"
#include "tcop/tcopprot.h"
#include "utils/builtins.h"
#include "utils/syscache.h"
#include "utils/lsyscache.h"
#include "utils/typcache.h"

#include "parser/parse_node.h"

Plpsm_stmt *plpsm_parser_tree;

typedef struct
{							/* it's used for storing data from SQL parser */
	int	location;
	const char *name1;
	const char *name2;
	int16 offset;
} SQLHostVar;

typedef struct
{
	Plpsm_object *top_scope;
	Plpsm_object *current_scope;			/* pointer to outer compound statement */
	Plpsm_pcode_module *module;
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
		int	ndata;				/* number of data address global		*/
	}			stack;
	struct						/* used as data for parsing a SQL expression */
	{
		SQLHostVar   *vars;				/* list of found host variables */
		int	nvars;				/* number of variables */
		bool	has_external_params;			/* true, when expr need a external parameters - detected host var. or param */
		bool	is_expression;				/* show a message about missing variables instead missing columns */
	} pdata;
	struct
	{
		int	nargs;
		char *name;
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
	} finfo;
} CompileStateData;

typedef CompileStateData *CompileState;

static void compile(CompileState cstate, Plpsm_stmt *stmt, Plpsm_pcode_module *m);

static Node *resolve_column_ref(CompileState cstate, ColumnRef *cref);

void plpsm_parser_setup(struct ParseState *pstate, CompileState cstate);

static Plpsm_object *lookup_var(Plpsm_object *scope, const char *name);

static void store_debug_info(Plpsm_pcode_module *m, char *str);

bool	plpsm_debug_compiler = false;

static void store_jmp(Plpsm_pcode_module *m, int addr);
static int store_jmp_unknown(Plpsm_pcode_module *m);
static int store_call_unknown(Plpsm_pcode_module *m);


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
	new->typ = stmt->typ;
	new->stmt = stmt;
	if (outer != NULL)
		append_object(outer, new);
	if (stmt)
		new->name = stmt->name;
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
create_variable_for(Plpsm_stmt *decl_stmt, CompileState cstate, 
						char *name, Plpsm_usage_variable_type typ)
{
	Plpsm_object *iterator = cstate->current_scope->inner;
	Plpsm_object *var;

	Assert(cstate->current_scope->typ == PLPSM_STMT_COMPOUND_STATEMENT);
	Assert(decl_stmt->typ == PLPSM_STMT_DECLARE_VARIABLE || 
		decl_stmt->typ == PLPSM_STMT_DECLARE_CURSOR);

	while (iterator != NULL)
	{
		switch (iterator->typ)
		{
			case PLPSM_STMT_DECLARE_VARIABLE:
			case PLPSM_STMT_DECLARE_CURSOR:
				if (strcmp(iterator->name, name) == 0)
					elog(ERROR, "identifier \"%s\" is used yet", name);
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
			elog(ERROR, "SQLSTATE variable is already defined");
		cstate->stack.has_sqlstate = true;
		switch (decl_stmt->datum.typoid)
		{
			case TEXTOID:
			case BPCHAROID:
			case VARCHAROID:
				if (decl_stmt->datum.typmod < 9 && decl_stmt->datum.typmod != -1)
					elog(ERROR, "too short datatype for SQLSTATE");
				break;
			default:
				elog(ERROR, "SQLSTATE variable should be text, char or varchar");
		}
	}

	if (strcmp(name, "sqlcode") == 0)
	{
		if (cstate->stack.has_sqlcode)
			elog(ERROR, "SQLOCODE variable is already defined");
		cstate->stack.has_sqlcode = true;
		if (decl_stmt->datum.typoid != INT4OID)
			elog(ERROR, "SQLCODE variable should be integer");
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
		case PLPSM_STMT_LOOP:
		case PLPSM_STMT_WHILE:
		case PLPSM_STMT_REPEAT_UNTIL:
		case PLPSM_STMT_FOR:
			if (stmt->name && lookup_object_with_label_in_scope(cstate->current_scope, stmt->name) != NULL)
				elog(ERROR, "label \"%s\" is defined in current scope", stmt->name);
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
				 parser_errposition(pstate, cref->location)));
	}

	if (myvar == NULL && var == NULL && cstate->pdata.is_expression)
		elog(ERROR, "variable \"%s\" isn't available in current scope", NameListToString(cref->fields));

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
				 parser_errposition(pstate, pref->location)));

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
 * generate a fake node
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
 * generate fake node
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

	elog(ERROR, "there are no field \"%s\" in type \"%s\"", fieldname,
									format_type_with_typemod(typoid, typmod));
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
	if (scope->typ == PLPSM_STMT_COMPOUND_STATEMENT && scope->name && 
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


/*
 * Assign info about Object reference to state 
 */
static void
appendHostVar(CompileState cstate, int location, const char *name1, const char *name2, int offset)
{
	SQLHostVar *var = &cstate->pdata.vars[cstate->pdata.nvars++];
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
						elog(ERROR, "there is conflict between compound statement label and composite variable");
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

void
plpsm_parser_setup(struct ParseState *pstate, CompileState cstate)
{
	pstate->p_post_columnref_hook = plpsm_post_column_ref;
	pstate->p_paramref_hook = plpsm_paramref_hook;
	pstate->p_ref_hook_state = (void *) cstate;
}

/*
 * Compile a expression - replace a variables by placeholders,
 *
 * This is really hack. Better way is using a possible query deparsing,
 * but necessary methods are not accesable now.
 */
static char *
compile_expr_simple_target(CompileState cstate, char *expr, Oid typoid, int16 typmod, bool *no_params)
{

	List       *raw_parsetree_list;
	StringInfoData ds;
	StringInfoData		cds;
	int i = 0;
	int				loc = 0;
	int		j;
	SQLHostVar	*vars;
	int	nvars;
	bool	not_sorted;
	char *ptr;
	ListCell *li;

	initStringInfo(&ds);
	appendStringInfo(&ds, "SELECT (%s)::%s", expr, format_type_with_typemod(typoid, typmod));

	cstate->pdata.vars = (SQLHostVar *) palloc(cstate->stack.ndatums * sizeof(SQLHostVar));
	cstate->pdata.nvars = 0;
	cstate->pdata.has_external_params = false;
	cstate->pdata.is_expression = true;

	raw_parsetree_list = pg_parse_query(ds.data);

	foreach(li, raw_parsetree_list)
	{
		 Node       *parsetree = (Node *) lfirst(li);

		pg_analyze_and_rewrite_params((Node *) parsetree, ds.data, (ParserSetupHook) plpsm_parser_setup, (void *) cstate);
	}

	/* fast path, there are no params */
	if (cstate->pdata.nvars == 0)
	{
		*no_params = !cstate->pdata.has_external_params;
		return ds.data;
	}

	*no_params = false;

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

	initStringInfo(&cds);
	ptr = ds.data; j = 0;

	while (*ptr)
	{
		char *str;

		/* copy content to position of replaced object reference */
		while (loc < vars[j].location)
		{
			appendStringInfoChar(&cds, *ptr++);
			loc++;
		}

		/* replace ref. object by placeholder */
		appendStringInfo(&cds,"$%d", (int) vars[j].offset);

		/* find and skip a referenced object */
		str = strstr(ptr, vars[j].name1);
		Assert(str != NULL);
		str += strlen(vars[j].name1);
		loc += str - ptr; ptr = str;

		/* when object was referenced with qualified name, then skip second identif */
		if (vars[j].name2 != NULL)
		{
			str = strstr(ptr, vars[j].name2);
			Assert(str != NULL);
			str += strlen(vars[j].name2);
			loc += str - ptr; ptr = str;
		}

		if (++j >= nvars)
		{
			/* copy to end and leave */
			while (*ptr)
				appendStringInfoChar(&cds, *ptr++);
			break;
		}
	}

	pfree(ds.data);
	pfree(vars);

	cstate->pdata.vars = NULL;

	return cds.data;
}

/*
 * compile a declared cursor
 */
static char *
compile_query(CompileState cstate, char *query, bool *no_params, TupleDesc *tupdesc)
{
	List       *raw_parsetree_list;
	StringInfoData		cds;
	int i = 0;
	int				loc = 0;
	int		j;
	SQLHostVar	*vars;
	int	nvars;
	bool	not_sorted;
	char *ptr;
	ListCell *li;

	cstate->pdata.vars = (SQLHostVar *) palloc(cstate->stack.ndatums * sizeof(SQLHostVar));
	cstate->pdata.nvars = 0;
	cstate->pdata.has_external_params = false;
	cstate->pdata.is_expression = true;

	raw_parsetree_list = pg_parse_query(query);

	foreach(li, raw_parsetree_list)
	{
		 Node       *parsetree = (Node *) lfirst(li);

		pg_analyze_and_rewrite_params((Node *) parsetree, query, (ParserSetupHook) plpsm_parser_setup, (void *) cstate);
	}
	

	/* fast path, there are no params */
	if (cstate->pdata.nvars == 0)
	{
		*no_params = !cstate->pdata.has_external_params;
		return query;
	}

	*no_params = false;

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

	initStringInfo(&cds);
	ptr = query; j = 0;

	while (*ptr)
	{
		char *str;

		/* copy content to position of replaced object reference */
		while (loc < vars[j].location)
		{
			appendStringInfoChar(&cds, *ptr++);
			loc++;
		}

		/* replace ref. object by placeholder */
		appendStringInfo(&cds,"$%d", (int) vars[j].offset);

		/* find and skip a referenced object */
		str = strstr(ptr, vars[j].name1);
		Assert(str != NULL);
		str += strlen(vars[j].name1);
		loc += str - ptr; ptr = str;

		/* when object was referenced with qualified name, then skip second identif */
		if (vars[j].name2 != NULL)
		{
			str = strstr(ptr, vars[j].name2);
			Assert(str != NULL);
			str += strlen(vars[j].name2);
			loc += str - ptr; ptr = str;
		}

		if (++j >= nvars)
		{
			/* copy to end and leave */
			while (*ptr)
				appendStringInfoChar(&cds, *ptr++);
			break;
		}
	}

	pfree(vars);

	cstate->pdata.vars = NULL;

	return cds.data;


}


/*
 * returns a target objects for assign statement. When target is composite type with 
 * specified field, then fieldname is filled.
 */
static Plpsm_object *
resolve_target(CompileState cstate, List *target, const char **fieldname, int location)
{
	const char *name1;
	const char *name2 = NULL;
	Plpsm_object *var;

	switch (list_length(target))
	{
		case 1:
			{
				Node	*field1 = (Node *) linitial(target);
				Assert(IsA(field1, String));
				name1 = strVal(field1);
				var = lookup_var(cstate->current_scope, name1);
				if (var == NULL)
					elog(ERROR, "a missing target variable \"%s\"", name1);
				return var;
			}
		case 2:
			{
				/* 
				 * if we found a label A with variable B and variable A,
				 * then we have to raise a error, because there are not
				 * clean what situation is: label.var or record.field
				 */
				Node	*field1 = (Node *) linitial(target);
				Node	*field2 = (Node *) lsecond(target);
				Assert(IsA(field1, String));
				name1 = strVal(field1);
				Assert(IsA(field2, String));
				name2 = strVal(field2);

				var = lookup_qualified_var(cstate->current_scope, name1, name2);
				if (var != NULL)
				{
					if (lookup_var(cstate->current_scope, name1))
						elog(ERROR, "there is conflict between compound statement label and composite variable");
					return var;
				}
				else
				{
					var = lookup_var(cstate->current_scope, name1);
					if (var == NULL)
						elog(ERROR, "a missing target variable \"%s\"", name1);
					*fieldname = name2;
					return var;
				}
			}
		case 3:
			{
				Node	*field1 = (Node *) linitial(target);
				Node	*field2 = (Node *) lsecond(target);
				Node	*field3 = (Node *) lthird(target);
				const char *name3;

				Assert(IsA(field1, String));
				name1 = strVal(field1);
				Assert(IsA(field2, String));
				name2 = strVal(field2);
				Assert(IsA(field3, String));
				name3 = strVal(field3);

				var = lookup_qualified_var(cstate->current_scope, name1, name2);
				if (var == NULL)
					elog(ERROR, "a missing target variable \"%s\" with label \"%s\"", name2, name1);
				*fieldname = name3;
			}
	}

	return NULL;
}

/*
 * helps with LEAVE, ITERATE statement compilation
 *
 */
static void
compile_leave_iterate(CompileState cstate, 
						Plpsm_object *scope, Plpsm_stmt *stmt)
{
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
							elog(ERROR, "label of iterate statement is related to compound statement");
						store_jmp(cstate->module, scope->calls.entry_addr);
					}
					else
					{
						int	addr;
						if (scope->calls.has_release_call)
						{
							addr = store_call_unknown(cstate->module);
							scope->calls.release_calls = lappend(scope->calls.release_calls, 
														makeInteger(addr));
						}
						addr = store_jmp_unknown(cstate->module);
						scope->calls.leave_jmps = lappend(scope->calls.leave_jmps,
													    makeInteger(addr));
					}
				}
				else
				{
					if (scope->calls.has_release_call)
					{
						int addr = store_call_unknown(cstate->module);
						scope->calls.release_calls = lappend(scope->calls.release_calls,
													    makeInteger(addr));
					}
				}
				compile_leave_iterate(cstate, scope->outer, stmt);
			}
			break;
		default:
			/* do nothing */;
	}
}



/*
 * Create a extensible module
 */
static Plpsm_pcode_module *
init_module(void)
{
	Plpsm_pcode_module *m = palloc(1024 * sizeof(Plpsm_pcode) + offsetof(Plpsm_pcode_module, code));
	m->mlength = 1024;
	m->length = 0;
	return m;
}

static void 
list(Plpsm_pcode_module *m)
{
	StringInfoData ds;
	int pc;

	initStringInfo(&ds);

	appendStringInfo(&ds, "\n   Datums: %d variable(s) \n", m->ndatums);
	appendStringInfo(&ds, "   Local data size: %d pointers\n", m->ndata);
	appendStringInfo(&ds, "   Size: %d instruction(s)\n\n", m->length);

	for (pc = 0; pc < m->length; pc++)
	{
		appendStringInfo(&ds, "%5d", pc);
		appendStringInfoChar(&ds, '\t');
		if (m->code[pc].typ != PCODE_DEBUG)
			appendStringInfoChar(&ds, '\t');

		switch (m->code[pc].typ)
		{
			case PCODE_JMP_FALSE_UNKNOWN:
				appendStringInfo(&ds, "Jmp_false_unknown %d", m->code[pc].addr);
				break;
			case PCODE_JMP:
				appendStringInfo(&ds, "Jmp %d", m->code[pc].addr);
				break;
			case PCODE_JMP_NOT_FOUND:
				appendStringInfo(&ds, "Jmp_not_found %d", m->code[pc].addr);
				break;
			case PCODE_CALL:
				appendStringInfo(&ds, "call %d", m->code[pc].addr);
				break;
			case PCODE_RETURN:
				appendStringInfo(&ds, "Return size:%d, byval:%s", m->code[pc].target.typlen,
										m->code[pc].target.typbyval ? "BYVAL" : "BYREF");
				break;
			case PCODE_EXEC_EXPR:
				{
					StringInfoData ds2;
					int	i;
					initStringInfo(&ds2);
					for (i = 0; i < m->code[pc].expr.nparams; i++)
					{
						if (i > 0)
							appendStringInfoChar(&ds2, ',');
						appendStringInfo(&ds2,"%d", m->code[pc].expr.typoids[i]);
					}
					
					appendStringInfo(&ds, "ExecExpr \"%s\",{%s}, data[%d]", m->code[pc].expr.expr,ds2.data,
													m->code[pc].expr.data);
					pfree(ds2.data);
				}
				break;
			case PCODE_DATA_QUERY:
				{
					StringInfoData ds2;
					int	i;
					initStringInfo(&ds2);
					for (i = 0; i < m->code[pc].expr.nparams; i++)
					{
						if (i > 0)
							appendStringInfoChar(&ds2, ',');
						appendStringInfo(&ds2,"%d", m->code[pc].expr.typoids[i]);
					}
					
					appendStringInfo(&ds, "Data \"%s\",{%s}, data[%d]", m->code[pc].expr.expr,ds2.data,
													m->code[pc].expr.data);
					pfree(ds2.data);
				}
				break;

			case PCODE_PRINT:
				appendStringInfo(&ds, "Print");
				break;
			case PCODE_DEBUG:
				appendStringInfo(&ds, "/* %s */", m->code[pc].str);
				break;
			case PCODE_NOOP:
				appendStringInfo(&ds, "Noop");
				break;
			case PCODE_DONE:
				appendStringInfoString(&ds, "Done.");
				break;
			case PCODE_EXECUTE:
				appendStringInfo(&ds, "execute %s()", m->code[pc].str);
				break;
			case PCODE_IF_NOTEXIST_PREPARE:
				appendStringInfo(&ds, "prepere_ifnexist %s as %s", m->code[pc].prep.name, m->code[pc].prep.expr);
				break;
			case PCODE_SET_NULL:
				appendStringInfo(&ds, "SetNull @%d", m->code[pc].target.offset);
				break;
			case PCODE_SAVETO:
				appendStringInfo(&ds, "SaveTo @%d, size:%d, byval:%s", m->code[pc].target.offset, m->code[pc].target.typlen,
										m->code[pc].target.typbyval ? "BYVAL" : "BYREF");
				break;
			case PCODE_SAVETO_FIELD:
				appendStringInfo(&ds, "SaveToField #%d @%d, data[%d] size:%d, byval:%s oid:%d typmod %d", m->code[pc].saveto_field.fnumber, m->code[pc].saveto_field.offset,
										m->code[pc].saveto_field.data,
										m->code[pc].saveto_field.typlen,
										m->code[pc].saveto_field.typbyval ? "BYVAL" : "BYREF",
										m->code[pc].saveto_field.typoid, 
										m->code[pc].saveto_field.typmod);
				break;
			case PCODE_COPY_PARAM:
				appendStringInfo(&ds, "CopyParam @%d, @%d, size:%d, byval:%s", 
										m->code[pc].copyto.src,
										m->code[pc].copyto.dest,
										m->code[pc].copyto.typlen,
										m->code[pc].copyto.typbyval ? "BYVAL" : "BYREF");
				break;
			case PCODE_CURSOR_OPEN:
				appendStringInfo(&ds, "OpenCursor @%d, ^%d name:%s", m->code[pc].cursor.offset,
											m->code[pc].cursor.addr,
											m->code[pc].cursor.name);
				break;
			case PCODE_CURSOR_CLOSE:
				appendStringInfo(&ds, "CloseCursor @%d, ^%d name:%s", m->code[pc].cursor.offset, 
											m->code[pc].cursor.addr,
											m->code[pc].cursor.name);
				break;
			case PCODE_CURSOR_RELEASE:
				appendStringInfo(&ds, "ReleaseCursor @%d, ^%d name:%s", m->code[pc].cursor.offset, 
											m->code[pc].cursor.addr,
											m->code[pc].cursor.name);
				break;
			case PCODE_CURSOR_FETCH:
				appendStringInfo(&ds, "Fetch @%d, count:%d name: %s", m->code[pc].fetch.offset, 
												m->code[pc].fetch.count,
												m->code[pc].fetch.name);
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
				appendStringInfo(&ds, "Signal NODATA, \"%s\"", m->code[pc].str);
				break;
			case PCODE_SQLCODE_REFRESH:
				appendStringInfo(&ds, "RefreshSQLCODE @%d", m->code[pc].target.offset);
				break;
			case PCODE_SQLSTATE_REFRESH:
				appendStringInfo(&ds, "RefreshSQLSTATE @%d", m->code[pc].target.offset);
				break;
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


#define CHECK_MODULE_SIZE(m)		if (m->length == m->mlength) elog(ERROR, "module is too long")
#define SET_AND_INC_PC(m, t)	m->code[m->length++].typ = t
#define SET_ADDR(m, a)			m->code[m->length].addr = a
#define SET_STR(m, s)			m->code[m->length].str = s
#define SET_RETURN_ADDR_INC_PC(m,t)	m->code[m->length].typ = t; return m->length++
#define SET_EXPR(m, e, n, o)		m->code[m->length].expr.expr = e; m->code[m->length].expr.nparams = n; m->code[m->length].expr.typoids = o
#define SET_TARGET(a, ta)		m->code[a].addr = ta
#define PC(m)				m->length
#define SET_PREP(m, n, q)		m->code[m->length].prep.name = n; m->code[m->length].prep.expr = q
#define SET_SIZE(m, a, s)		m->code[a].size = s
#define SET_OFFSET(m, o)		m->code[m->length].target.offset = o
#define SET_DATUM_PROP(m, tl, bv)	m->code[m->length].target.typlen = tl; m->code[m->length].target.typbyval = bv
#define SET_CURSOR(m, a, o)		m->code[m->length].cursor.addr = a; m->code[m->length].cursor.offset = o
#define SET_CURSOR_NAME(m, n)		m->code[m->length].cursor.name = n
#define SET_EXPR_DATA(m, d)		m->code[m->length].expr.data = d

#define SET_OPVAL(n, v)			m->code[m->length].n = v
#define EMIT_OPCODE(t)			do { \
						m->code[m->length++].typ = t; \
						m = check_module_size(m); \
					} while (0)

static void
store_debug_info(Plpsm_pcode_module *m, char *str)
{
	CHECK_MODULE_SIZE(m);
	SET_STR(m, str);
	SET_AND_INC_PC(m, PCODE_DEBUG);
}

static int
store_jmp_not_true_unknown(Plpsm_pcode_module *m)
{
	CHECK_MODULE_SIZE(m);
	SET_RETURN_ADDR_INC_PC(m, PCODE_JMP_FALSE_UNKNOWN);
}

static void
store_jmp_not_true(Plpsm_pcode_module *m, int addr)
{
	CHECK_MODULE_SIZE(m);
	SET_ADDR(m, addr);
	SET_AND_INC_PC(m, PCODE_JMP_FALSE_UNKNOWN);
}


static void
store_jmp(Plpsm_pcode_module *m, int addr)
{
	CHECK_MODULE_SIZE(m);
	SET_ADDR(m, addr);
	SET_AND_INC_PC(m, PCODE_JMP);
}

static int
store_jmp_unknown(Plpsm_pcode_module *m)
{
	CHECK_MODULE_SIZE(m);
	SET_RETURN_ADDR_INC_PC(m, PCODE_JMP);
}

static int
store_call_unknown(Plpsm_pcode_module *m)
{
	CHECK_MODULE_SIZE(m);
	SET_RETURN_ADDR_INC_PC(m, PCODE_CALL);
}

static void
store_release_cursors(CompileState cstate, Plpsm_pcode_module *m)
{
	Plpsm_object *scope = cstate->current_scope;
	Plpsm_object *iterator;
	/* search all cursors from current compound statement */
	
	Assert(scope->typ == PLPSM_STMT_COMPOUND_STATEMENT);
	iterator = scope->inner;
	while (iterator != NULL)
	{
		if (iterator->typ == PLPSM_STMT_DECLARE_CURSOR)
		{
			CHECK_MODULE_SIZE(m);
			SET_CURSOR_NAME(m, iterator->name);
			SET_CURSOR(m, iterator->cursor.data_addr, iterator->cursor.offset);
			/* close only opened cursor */
			SET_AND_INC_PC(m, PCODE_CURSOR_RELEASE);
		}
		iterator = iterator->next;
	}
}

static void
store_exec_expr(CompileState cstate, Plpsm_pcode_module *m, char *expr, Oid targetoid, int16 typmod)
{
	char *cexpr;
	Oid	*typoids;
	bool	no_params;

	CHECK_MODULE_SIZE(m);

	cexpr = compile_expr_simple_target(cstate, expr, targetoid, typmod, &no_params);
	if (!no_params)
	{
		/* copy a actual typeoid vector to private typoid vector */
		typoids = palloc(cstate->stack.ndatums * sizeof(Oid));
		memcpy(typoids, cstate->stack.oids.data, cstate->stack.ndatums * sizeof(Oid));
		SET_EXPR(m, cexpr, cstate->stack.ndatums, cstate->stack.oids.data);
	}
	else
	{
		SET_EXPR(m, cexpr, 0, NULL);
	}

	SET_EXPR_DATA(m, cstate->stack.ndata++);

	SET_AND_INC_PC(m, PCODE_EXEC_EXPR);
}

static void
store_print(Plpsm_pcode_module *m)
{
	CHECK_MODULE_SIZE(m);
	SET_AND_INC_PC(m, PCODE_PRINT);
}

static void
store_saveto(Plpsm_pcode_module *m, int16 offset, int16 typlen, bool typbyval)
{
	CHECK_MODULE_SIZE(m);
	SET_OFFSET(m, offset);
	SET_DATUM_PROP(m, typlen, typbyval);
	SET_AND_INC_PC(m, PCODE_SAVETO);
}

static void
store_copy_parameter(Plpsm_pcode_module *m, Plpsm_object *var, int paramid)
{
	CHECK_MODULE_SIZE(m);
	m->code[m->length].copyto.src = paramid;
	m->code[m->length].copyto.dest = var->offset;
	m->code[m->length].copyto.typlen = var->stmt->datum.typlen; 
	m->code[m->length].copyto.typbyval = var->stmt->datum.typbyval;
	SET_AND_INC_PC(m, PCODE_COPY_PARAM);
}

static void
store_return(Plpsm_pcode_module *m)
{
	CHECK_MODULE_SIZE(m);
	SET_AND_INC_PC(m, PCODE_RETURN);
}

static void
store_done(CompileState cstate, Plpsm_pcode_module *m)
{
	if (cstate->finfo.return_expr != NULL && cstate->finfo.result.datum.typoid != VOIDOID)
	{
		store_exec_expr(cstate, m, cstate->finfo.return_expr,
						cstate->finfo.result.datum.typoid, -1);
		SET_DATUM_PROP(m, cstate->finfo.result.datum.typlen, cstate->finfo.result.datum.typbyval);
		SET_AND_INC_PC(m, PCODE_RETURN);
	}
	else if (cstate->finfo.result.datum.typoid != VOIDOID)
	{
		CHECK_MODULE_SIZE(m);
		SET_STR(m,"function doesn't return data");
		SET_AND_INC_PC(m, PCODE_SIGNAL_NODATA);
	}
	else
	{
		CHECK_MODULE_SIZE(m);
		SET_AND_INC_PC(m, PCODE_RETURN_VOID);
	}
}

static Plpsm_object *
release_psm_object(CompileState cstate, Plpsm_object *obj, int release_entry, int leave_entry)
{
	Plpsm_pcode_module *m = cstate->module;

	if (obj->calls.release_calls)
	{
		ListCell	*l;

		Assert(release_entry != 0);
		foreach(l, obj->calls.release_calls)
		{
			int	addr = intVal(lfirst(l));
			SET_TARGET(addr, release_entry);
		}
	}

	if (obj->calls.leave_jmps)
	{
		ListCell	*l;

		foreach(l, obj->calls.leave_jmps)
		{

			int	addr = intVal(lfirst(l));
			SET_TARGET(addr, leave_entry);
		}
	}

	return obj->outer;
}

/*
 * diff from plpgsql 
 *
 * SQL/PSM is compilable language - so it checking a all SQL and expression
 * before first start of procedure. All SQL object must exists. When a access
 * to dynamic object is necessary, then dynamic SQL must be used. ???
 */
static void 
compile(CompileState cstate, Plpsm_stmt *stmt, Plpsm_pcode_module *m)
{
	int	addr1;
	int	addr2;
	Plpsm_object *obj;

	while (stmt != NULL)
	{
		if (stmt->debug != NULL && 0)
			store_debug_info(m, stmt->debug);

		switch (stmt->typ)
		{
			case PLPSM_STMT_LOOP:
				obj = new_psm_object_for(stmt, cstate, PC(m));
				addr1 = PC(m);
				compile(cstate, stmt->inner_left, m);
				store_jmp(m, addr1);
				cstate->current_scope = release_psm_object(cstate, obj, 0, PC(m));
				break;

			case PLPSM_STMT_WHILE:
				obj = new_psm_object_for(stmt, cstate, PC(m));
				addr1 = PC(m);
				store_exec_expr(cstate, m, stmt->expr, BOOLOID, -1);
				addr2 = store_jmp_not_true_unknown(m);
				compile(cstate, stmt->inner_left, m);
				store_jmp(m, addr1);
				SET_TARGET(addr2, PC(m));
				cstate->current_scope = release_psm_object(cstate, obj, 0, PC(m));
				break;

			case PLPSM_STMT_REPEAT_UNTIL:
				obj = new_psm_object_for(stmt, cstate, PC(m));
				addr1 = PC(m);
				compile(cstate, stmt->inner_left, m);
				store_exec_expr(cstate, m, stmt->expr, BOOLOID, -1);
				store_jmp_not_true(m, addr1);
				cstate->current_scope = release_psm_object(cstate, obj, 0, PC(m));
				break;

			case PLPSM_STMT_COMPOUND_STATEMENT:
				{
					bool	has_sqlstate = cstate->stack.has_sqlstate;
					bool	has_sqlcode = cstate->stack.has_sqlcode;

					obj = new_psm_object_for(stmt, cstate, PC(m));
					compile(cstate, stmt->inner_left, m);

					cstate->stack.has_sqlstate = has_sqlstate;
					cstate->stack.has_sqlcode = has_sqlcode;

					/* generate release block */
					if (obj->calls.has_release_call)
					{
						if (obj->calls.release_calls != NULL)
						{
							int release_addr;
							/* generate a release block as subrotine */
							addr1 = store_call_unknown(m);
							addr2 = store_jmp_unknown(m);
							SET_TARGET(addr1, PC(m));
							release_addr = PC(m);
							store_release_cursors(cstate, m);
							SET_AND_INC_PC(m, PCODE_RET_SUBR);
							SET_TARGET(addr2, PC(m));
							cstate->current_scope = release_psm_object(cstate, obj, release_addr, PC(m));
						}
						else
						{
							store_release_cursors(cstate, m);
							cstate->current_scope = release_psm_object(cstate, obj, 0, PC(m));
						}
					}
					else
						cstate->current_scope = release_psm_object(cstate, obj, 0, PC(m));
					break;
				}

			case PLPSM_STMT_DECLARE_VARIABLE:
				{
					ListCell *l;
					foreach(l, stmt->compound_target)
					{
						char *name = strVal(lfirst(l));
						obj = create_variable_for(stmt, cstate, name, PLPSM_VARIABLE);

						if (stmt->expr != NULL)
						{
							store_exec_expr(cstate, m, stmt->expr, stmt->datum.typoid, stmt->datum.typmod);
							store_saveto(m, obj->offset, stmt->datum.typlen, stmt->datum.typbyval);
						}
						else
						{
							CHECK_MODULE_SIZE(m);
							SET_OFFSET(m, obj->offset);
							SET_AND_INC_PC(m, PCODE_SET_NULL);
						}
					}
				}
				break;

			case PLPSM_STMT_DECLARE_CURSOR:
				{
					char *name = strVal(linitial(stmt->target)) ;
					obj = create_variable_for(stmt, cstate, name, PLPSM_VARIABLE);
					if (stmt->query != NULL)
					{
						bool	noparams;
					
						char *cquery = compile_query(cstate, stmt->query, &noparams, NULL);
						CHECK_MODULE_SIZE(m);
						if (!noparams)
						{
							Oid *typoids;

							/* copy a current typeoid vector to private typeoid vector */
							typoids = palloc(cstate->stack.ndatums * sizeof(Oid));
							memcpy(typoids, cstate->stack.oids.data, cstate->stack.ndatums * sizeof(Oid));
							SET_EXPR(m, cquery, cstate->stack.ndatums, cstate->stack.oids.data);
						}
						else
						{
							SET_EXPR(m, cquery, 0, NULL);
						}
						obj->cursor.data_addr = PC(m);
						SET_EXPR_DATA(m, cstate->stack.ndata++);
						SET_AND_INC_PC(m, PCODE_DATA_QUERY);
					}
				}
				break;

			case PLPSM_STMT_OPEN:
				{
					const char *fieldname;

					obj = resolve_target(cstate, stmt->target,  &fieldname, stmt->location);
					if (obj->stmt->typ != PLPSM_STMT_DECLARE_CURSOR)
						elog(ERROR, "variable \"%s\" isn't cursor", obj->name);
					SET_CURSOR(m, obj->cursor.data_addr, obj->cursor.offset);
					SET_CURSOR_NAME(m, obj->name);
					SET_AND_INC_PC(m, PCODE_CURSOR_OPEN);
				}
				break;

			case PLPSM_STMT_FETCH:
				{
					const char *fieldname;
					ListCell *l;
					int	i = 1;

					obj = resolve_target(cstate, stmt->target,  &fieldname, stmt->location);
					if (obj->stmt->typ != PLPSM_STMT_DECLARE_CURSOR)
						elog(ERROR, "variable \"%s\" isn't cursor", obj->name);
					SET_OPVAL(fetch.offset, obj->cursor.offset);
					SET_OPVAL(fetch.name, obj->name);
					SET_OPVAL(fetch.nvars, list_length(stmt->compound_target));
					SET_OPVAL(fetch.count, 1);
					EMIT_OPCODE(PCODE_CURSOR_FETCH);

					if (cstate->stack.has_sqlstate)
					{
						Plpsm_object *var = lookup_var(cstate->current_scope, "sqlstate");
						Assert(var != NULL);
						SET_OPVAL(target.offset, var->offset);
						SET_OPVAL(target.typlen, var->stmt->datum.typlen);
						SET_OPVAL(target.typbyval, var->stmt->datum.typbyval);
						EMIT_OPCODE(PCODE_SQLSTATE_REFRESH);
					}

					if (cstate->stack.has_sqlcode)
					{
						Plpsm_object *var = lookup_var(cstate->current_scope, "sqlcode");
						Assert(var != NULL);
						SET_OPVAL(target.offset, var->offset);
						SET_OPVAL(target.typlen, var->stmt->datum.typlen);
						SET_OPVAL(target.typbyval, var->stmt->datum.typbyval);
						EMIT_OPCODE(PCODE_SQLCODE_REFRESH);
					}

					foreach (l, stmt->compound_target)
					{
						Plpsm_object	*var = resolve_target(cstate, (List *) lfirst(l),
													    &fieldname,
														stmt->location);
						SET_OPVAL(saveto_field.offset, var->offset);
						SET_OPVAL(saveto_field.typlen, var->stmt->datum.typlen);
						SET_OPVAL(saveto_field.typbyval, var->stmt->datum.typbyval);
						SET_OPVAL(saveto_field.typoid, var->stmt->datum.typoid);
						SET_OPVAL(saveto_field.typmod, var->stmt->datum.typmod);
						SET_OPVAL(saveto_field.data, cstate->stack.ndata++);
						SET_OPVAL(saveto_field.fnumber, i++);
						EMIT_OPCODE(PCODE_SAVETO_FIELD);
					}
				}
				break;

			case PLPSM_STMT_CLOSE:
				{
					const char *fieldname;

					obj = resolve_target(cstate, stmt->target,  &fieldname, stmt->location);
					if (obj->stmt->typ != PLPSM_STMT_DECLARE_CURSOR)
						elog(ERROR, "variable \"%s\" isn't cursor", obj->name);
					SET_CURSOR(m, obj->cursor.data_addr, obj->cursor.offset);
					SET_CURSOR_NAME(m, obj->name);
					SET_AND_INC_PC(m, PCODE_CURSOR_CLOSE);
				}
				break;

			case PLPSM_STMT_IF:
				store_exec_expr(cstate, m, stmt->expr, BOOLOID, -1);
				addr1 = store_jmp_not_true_unknown(m);
				compile(cstate, stmt->inner_left, m);
				if (stmt->inner_right)
				{
					addr2 = store_jmp_unknown(m);
					SET_TARGET(addr1, PC(m));
					compile(cstate, stmt->inner_right, m);
					SET_TARGET(addr2, PC(m));
				}
				else
					SET_TARGET(addr1, PC(m));
				break;

			case PLPSM_STMT_CASE:
				{
					Plpsm_stmt *outer_case = stmt;
					Plpsm_stmt *condition = stmt->inner_left;
					List	*final_jmps = NIL;
					ListCell	*l;
					while (condition != NULL)
					{
						char *expr;
						if (outer_case->expr != NULL)
						{
							StringInfoData	ds;
							initStringInfo(&ds);
							appendStringInfo(&ds, "%s IN (%s)", outer_case->expr,
												condition->expr);
							expr = ds.data;
						}
						else
							expr = pstrdup(condition->expr);
						store_exec_expr(cstate, m, expr, BOOLOID, -1);
						addr1 = store_jmp_not_true_unknown(m);
						compile(cstate, condition->inner_left, m);
						addr2 = store_jmp_unknown(m);
						SET_TARGET(addr1, PC(m));
						final_jmps = lappend(final_jmps, makeInteger(addr2));
						condition = condition->next;
					}
					if (outer_case->inner_right == NULL)
					{
						CHECK_MODULE_SIZE(m);
						SET_STR(m,"case doesn't match any value");
						SET_AND_INC_PC(m, PCODE_SIGNAL_NODATA);
					}
					else
						compile(cstate, outer_case->inner_right, m);
					foreach (l, final_jmps)
					{
						addr1 = intVal(lfirst(l));
						SET_TARGET(addr1, PC(m));
					}
				}
				break;

			case PLPSM_STMT_PRINT:
				store_exec_expr(cstate, m, stmt->expr, TEXTOID, -1);
				store_print(m);
				break;

			case PLPSM_STMT_SET:
				{
					const char *fieldname;

					obj = resolve_target(cstate, stmt->target,  &fieldname, stmt->location);
					store_exec_expr(cstate, m, stmt->expr, obj->stmt->datum.typoid, obj->stmt->datum.typmod);
					store_saveto(m, obj->offset, obj->stmt->datum.typlen, obj->stmt->datum.typbyval);
					break;
				}

			case PLPSM_STMT_ITERATE:
			case PLPSM_STMT_LEAVE:
				{
					if (strcmp(cstate->finfo.name, stmt->name) == 0)
						elog(ERROR, "cannot leave function by LEAVE statement");
					compile_leave_iterate(cstate, cstate->current_scope, stmt);
					break;
				}

			case PLPSM_STMT_RETURN:
				{
					/* leave all compound statements too */
					if (cstate->finfo.result.datum.typoid != VOIDOID)
					{
						if (cstate->finfo.return_expr != NULL && 
							stmt->expr != NULL)
							elog(ERROR, "using RETURN expr in function with OUT arguments");

						if (stmt->expr != NULL)
							store_exec_expr(cstate, m, stmt->expr, cstate->finfo.result.datum.typoid, -1);
						else
							store_exec_expr(cstate, m, cstate->finfo.return_expr,
											cstate->finfo.result.datum.typoid, -1);
						SET_DATUM_PROP(m, cstate->finfo.result.datum.typlen, cstate->finfo.result.datum.typbyval);
						SET_AND_INC_PC(m, PCODE_RETURN);
					}
					else
					{
						Assert(cstate->finfo.return_expr == NULL);
						if (stmt->expr != NULL)
							elog(ERROR, "returned a value in VOID function");
						SET_AND_INC_PC(m, PCODE_RETURN_VOID);
					}
					break;
				}
				

			default:
				elog(ERROR, "unknown command typeid");
		}
		stmt = stmt->next;
	}
}


Plpsm_pcode_module *
plpsm_compile(Oid funcOid, bool forValidator)
{
	HeapTuple	procTup;
	int parse_rc;
	Form_pg_proc procStruct;
	char	*proc_source;
	Datum	prosrcdatum;
	bool		isnull;
	Plpsm_pcode_module *module;
	CompileStateData cstate;
	Plpsm_object outer_scope;
	int			numargs;
	Oid		   *argtypes;
	char	  **argnames;
	char	   *argmodes;
	int	i;
	int16	typlen;
	bool	typbyval;
	Bitmapset	*outargs = NULL;

	procTup = SearchSysCache1(PROCOID, ObjectIdGetDatum(funcOid));
	if (!HeapTupleIsValid(procTup))
		elog(ERROR, "cache lookup failed for function %u", funcOid);

	procStruct = (Form_pg_proc) GETSTRUCT(procTup);

	prosrcdatum = SysCacheGetAttr(PROCOID, procTup,
							Anum_pg_proc_prosrc, &isnull);
	if (isnull)
		elog(ERROR, "null prosrc");

	proc_source = TextDatumGetCString(prosrcdatum);

	plpsm_scanner_init(proc_source);
	parse_rc = plpsm_yyparse();
	if (parse_rc != 0)
		elog(ERROR, "plpsm parser returned %d", parse_rc);

	plpsm_scanner_finish();

	memset(&outer_scope, 0, sizeof(Plpsm_object));
	outer_scope.typ = PLPSM_STMT_COMPOUND_STATEMENT;
	outer_scope.name = pstrdup(NameStr(procStruct->proname));

	cstate.top_scope = &outer_scope;
	cstate.top_scope->name = outer_scope.name;
	cstate.current_scope = cstate.top_scope;
	cstate.stack.ndata = 0;
	cstate.stack.ndatums = 0;
	cstate.stack.oids.size = 128;
	cstate.stack.oids.data = (Oid *) palloc(cstate.stack.oids.size * sizeof(Oid));
	cstate.stack.has_sqlstate = false;
	cstate.stack.has_sqlcode = false;

	cstate.finfo.result.datum.typoid = procStruct->prorettype;
	get_typlenbyval(cstate.finfo.result.datum.typoid, &cstate.finfo.result.datum.typlen, &cstate.finfo.result.datum.typbyval);

	module = init_module();
	cstate.module = module;

	/* 
	 * append to scope a variables for parameters, and store 
	 * instruction for copy from fcinfo
	 */
	numargs = get_func_arg_info(procTup,
						&argtypes, &argnames, &argmodes);
	cstate.finfo.nargs = numargs;
	cstate.finfo.name = pstrdup(NameStr(procStruct->proname));
	module->name = cstate.finfo.name;

	for (i = 0; i < numargs; i++)
	{
		Oid			argtypid = argtypes[i];
		char	argmode = argmodes ? argmodes[i] : PROARGMODE_IN;
		Plpsm_stmt *decl_stmt;
		char		buf[32];
		Plpsm_object *var;
		Plpsm_object *alias;

		/* Create $n name for variable */
		snprintf(buf, sizeof(buf), "$%d", i + 1);

		/* append a fake statements for parameter variable */
		decl_stmt = plpsm_new_stmt(PLPSM_STMT_DECLARE_VARIABLE, -1);
		decl_stmt->target = list_make1(makeString(pstrdup(buf)));
		get_typlenbyval(argtypid, &typlen, &typbyval);

		decl_stmt->datum.typoid = argtypid;
		decl_stmt->datum.typmod = -1;
		decl_stmt->datum.typname = NULL;
		decl_stmt->datum.typlen = typlen;
		decl_stmt->datum.typbyval = typbyval;

		/* append implicit name to scope */
		var = create_variable_for(decl_stmt, &cstate, pstrdup(buf), PLPSM_VARIABLE);
		/* append explicit name to scope */
		if (argnames && argnames[i][0] != '\0')
		{
			alias = create_variable_for(decl_stmt, &cstate, pstrdup(argnames[i]), PLPSM_REFERENCE);
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
			store_copy_parameter(module, var, i);
		}
		else
		{
			/* initialize OUT variables to NULL */
			CHECK_MODULE_SIZE(module);
			SET_OFFSET(module, var->offset);
			SET_AND_INC_PC(module, PCODE_SET_NULL);
		}
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
		cstate.finfo.return_expr = ds.data;

		bms_free(outargs);
	}
	else
		cstate.finfo.return_expr = NULL;

	compile(&cstate, plpsm_parser_tree, module);

	store_done(&cstate, module);

	module->ndatums = cstate.stack.ndatums;
	module->ndata = cstate.stack.ndata;

	if (plpsm_debug_compiler)
		list(module);

	ReleaseSysCache(procTup);

	return module;
}

