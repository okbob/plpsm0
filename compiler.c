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
bool	plpsm_debug_compiler = false;

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
		int	maxvars;			/* size of declared array */
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

static void compile(CompileState cstate, Plpsm_stmt *stmt);
static Node *resolve_column_ref(CompileState cstate, ColumnRef *cref);
void plpsm_parser_setup(struct ParseState *pstate, CompileState cstate);
static Plpsm_object *lookup_var(Plpsm_object *scope, const char *name);
static void compile_expr(CompileState cstate, char *expr, Oid targetoid, int16 typmod);

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


/*
 * Assign info about Object reference to state 
 */
static void
appendHostVar(CompileState cstate, int location, const char *name1, const char *name2, int offset)
{
	if (cstate->pdata.nvars == cstate->pdata.maxvars)
	{
		cstate->pdata.maxvars += 128;
		cstate->pdata.vars = repalloc(cstate->pdata.vars, cstate->pdata.maxvars * sizeof(SQLHostVar));
	}

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
 * Create a extensible module
 */
static Plpsm_pcode_module *
init_module(void)
{
	Plpsm_pcode_module *m = palloc0(1024 * sizeof(Plpsm_pcode) + offsetof(Plpsm_pcode_module, code));
	m->mlength = 1024;
	m->length = 0;
	return m;
}

#define VALUE(x)	m->code[pc].x

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
		if (VALUE(typ) != PCODE_DEBUG)
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
				appendStringInfo(&ds, "call %d", VALUE(addr));
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
			case PCODE_DEBUG:
				appendStringInfo(&ds, "/* %s */", VALUE(str));
				break;
			case PCODE_NOOP:
				appendStringInfo(&ds, "Noop");
				break;
			case PCODE_DONE:
				appendStringInfoString(&ds, "Done.");
				break;
			case PCODE_EXECUTE:
				appendStringInfo(&ds, "execute %s()", VALUE(str));
				break;
			case PCODE_IF_NOTEXIST_PREPARE:
				appendStringInfo(&ds, "prepere_ifnexist %s as %s", VALUE(prep.name), VALUE(prep.expr));
				break;
			case PCODE_SET_NULL:
				appendStringInfo(&ds, "SetNull @%d", VALUE(target.offset));
				break;
			case PCODE_SAVETO:
				appendStringInfo(&ds, "SaveTo @%d, size:%d, byval:%s", VALUE(target.offset), VALUE(target.typlen),
											VALUE(target.typbyval) ? "BYVAL" : "BYREF");
				break;
			case PCODE_SAVETO_FIELD:
				appendStringInfo(&ds, "SaveToField #%d @%d, data[%d] size:%d, byval:%s oid:%d typmod %d", VALUE(saveto_field.fnumber), VALUE(saveto_field.offset),
											VALUE(saveto_field.data),
											VALUE(saveto_field.typlen),
											VALUE(saveto_field.typbyval) ? "BYVAL" : "BYREF",
											VALUE(saveto_field.typoid), 
											VALUE(saveto_field.typmod));
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

#define PC(m)				m->length

#define SET_OPVAL(n, v)			m->code[m->length].n = v
#define EMIT_OPCODE(t)			do { \
						m->code[m->length++].typ = t; \
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
#define EMIT_JMP(a)			do { \
						SET_OPVAL(addr, a); \
						EMIT_OPCODE(PCODE_JMP); \
					} while (0)
#define EMIT_CALL(a)			do { \
						SET_OPVAL(addr, a); \
						EMIT_OPCODE(PCODE_CALL); \
					} while (0)


static void
compile_release_cursors(CompileState cstate)
{
	Plpsm_pcode_module *m = cstate->module;
	Plpsm_object *scope = cstate->current_scope;
	Plpsm_object *iterator;

	/* search all cursors from current compound statement */
	Assert(scope->typ == PLPSM_STMT_COMPOUND_STATEMENT);

	iterator = scope->inner;
	while (iterator != NULL)
	{
		if (iterator->typ == PLPSM_STMT_DECLARE_CURSOR)
		{
			SET_OPVAL(cursor.name, iterator->name);
			SET_OPVAL(cursor.addr, iterator->cursor.data_addr);
			SET_OPVAL(cursor.offset, iterator->cursor.offset);
			EMIT_OPCODE(PCODE_CURSOR_RELEASE);
		}
		iterator = iterator->next;
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
							elog(ERROR, "label of iterate statement is related to compound statement");
						EMIT_JMP(scope->calls.entry_addr);
					}
					else
					{
						if (scope->calls.has_release_call)
						{
							scope->calls.release_calls = lappend(scope->calls.release_calls, 
														makeInteger(PC(m)));
							EMIT_OPCODE(PCODE_CALL);
						}
						
						scope->calls.leave_jmps = lappend(scope->calls.leave_jmps,
													    makeInteger(PC(m)));
						EMIT_OPCODE(PCODE_JMP);
					}
				}
				else
				{
					if (scope->calls.has_release_call)
					{
						scope->calls.release_calls = lappend(scope->calls.release_calls,
													    makeInteger(PC(m)));
						EMIT_OPCODE(PCODE_CALL);
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
 * finalize a compilation
 */
static void
compile_done(CompileState cstate)
{
	 Plpsm_pcode_module *m = cstate->module;

	if (cstate->finfo.return_expr != NULL && cstate->finfo.result.datum.typoid != VOIDOID)
	{
		compile_expr(cstate,cstate->finfo.return_expr,
						cstate->finfo.result.datum.typoid, -1);
		SET_OPVAL(target.typlen, cstate->finfo.result.datum.typlen);
		SET_OPVAL(target.typbyval, cstate->finfo.result.datum.typbyval);
		EMIT_OPCODE(PCODE_RETURN);
	}
	else if (cstate->finfo.result.datum.typoid != VOIDOID)
	{
		SET_OPVAL(str,"function doesn't return data");
		EMIT_OPCODE(PCODE_SIGNAL_NODATA);
	}
	else
	{
		EMIT_OPCODE(PCODE_RETURN_VOID);
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
			SET_OPVAL_ADDR(intVal(lfirst(l)), addr, release_entry);
		}
	}

	if (obj->calls.leave_jmps)
	{
		ListCell	*l;

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

static char *
replace_vars(CompileState cstate, char *sqlstr, Oid **argtypes, int *nargs)
{
	Oid	*newargtypes;
	ListCell *l;

	List       *raw_parsetree_list;

	cstate->pdata.maxvars = 128;
	cstate->pdata.vars = (SQLHostVar *) palloc(cstate->pdata.maxvars * sizeof(SQLHostVar));
	cstate->pdata.nvars = 0;
	cstate->pdata.has_external_params = false;
	cstate->pdata.is_expression = true;

	raw_parsetree_list = pg_parse_query(sqlstr);

	foreach(l, raw_parsetree_list)
	{
		 Node       *parsetree = (Node *) lfirst(l);

		pg_analyze_and_rewrite_params((Node *) parsetree, sqlstr, 
								(ParserSetupHook) plpsm_parser_setup, 
												(void *) cstate);
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

		return cds.data;
	}
	else
		return sqlstr;
}

/*
 * Compile a SET statement in (var,var,..) = (expr, expr, ..) form
 */
static void
compile_multiset_stmt(CompileState cstate, Plpsm_stmt *stmt)
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

	if (list_length(stmt->compound_target) != list_length(stmt->expr_list))
		elog(ERROR, "there are different number of target variables and expressions in list");

	/* 
	 * now we doesn't a know a properties of generated query, so only emit
	 * opcode now. The property will be reasigned later.
	 */
	addr = PC(m);
	SET_OPVAL(expr.data, cstate->stack.ndata++);
	SET_OPVAL(expr.is_multicol, true);
	EMIT_OPCODE(PCODE_EXEC_EXPR);
	appendStringInfoString(&ds, "SELECT ");

	forboth (l1, stmt->compound_target, l2, stmt->expr_list)
	{
		const char *fieldname;
		Plpsm_object *var;
		char	*expr;

		var = resolve_target(cstate, (List *) lfirst(l1), &fieldname, stmt->location);
		expr = strVal(lfirst(l2));

		if (!isfirst)
			appendStringInfoChar(&ds, ',');
		else
			isfirst = false;

		appendStringInfo(&ds, "(%s)::%s", expr, format_type_with_typemod(var->stmt->datum.typoid, var->stmt->datum.typmod));
		SET_OPVALS_DATUM_INFO(saveto_field, var);
		SET_OPVAL(saveto_field.fnumber, i++);
		EMIT_OPCODE(PCODE_SAVETO_FIELD);
	}

	SET_OPVAL_ADDR(addr, expr.expr, replace_vars(cstate, ds.data, &locargtypes, &nargs));
	SET_OPVAL_ADDR(addr, expr.nparams, nargs);
	SET_OPVAL_ADDR(addr, expr.typoids, locargtypes);
}

/*
 * emit instruction to execute a expression with specified targetoid and typmod
 */
static void 
compile_expr(CompileState cstate, char *expr, Oid targetoid, int16 typmod)
{
	StringInfoData	ds;
	Plpsm_pcode_module *m = cstate->module;
	int	nargs;
	Oid	*argtypes;

	initStringInfo(&ds);
	appendStringInfo(&ds, "SELECT (%s)::%s", expr, format_type_with_typemod(targetoid, typmod));
	SET_OPVAL(expr.expr, replace_vars(cstate, ds.data, &argtypes, &nargs));
	SET_OPVAL(expr.nparams, nargs);
	SET_OPVAL(expr.typoids, argtypes);
	SET_OPVAL(expr.data, cstate->stack.ndata++);
	SET_OPVAL(expr.is_multicol, false);
	EMIT_OPCODE(PCODE_EXEC_EXPR);
}

/*
 * diff from plpgsql 
 *
 * SQL/PSM is compilable language - so it checking a all SQL and expression
 * before first start of procedure. All SQL object must exists. When a access
 * to dynamic object is necessary, then dynamic SQL must be used. ???
 */
static void 
compile(CompileState cstate, Plpsm_stmt *stmt)
{
	int	addr1;
	int	addr2;
	Plpsm_object *obj, *var;
	Plpsm_pcode_module *m = cstate->module;

	while (stmt != NULL)
	{
		switch (stmt->typ)
		{
			case PLPSM_STMT_LOOP:
				{
					obj = new_psm_object_for(stmt, cstate, PC(m));
					addr1 = PC(m);
					compile(cstate, stmt->inner_left);
					EMIT_JMP(addr1);
					cstate->current_scope = release_psm_object(cstate, obj, 0, PC(m));\
				}
				break;

			case PLPSM_STMT_WHILE:
				{
					obj = new_psm_object_for(stmt, cstate, PC(m));
					addr1 = PC(m);
					compile_expr(cstate, stmt->expr, BOOLOID, -1);
					addr2 = PC(m);
					EMIT_OPCODE(PCODE_JMP_FALSE_UNKNOWN);
					compile(cstate, stmt->inner_left);
					EMIT_JMP(addr1);
					SET_OPVAL_ADDR(addr2, addr, PC(m));
					cstate->current_scope = release_psm_object(cstate, obj, 0, PC(m));
				}
				break;

			case PLPSM_STMT_REPEAT_UNTIL:
				{
					obj = new_psm_object_for(stmt, cstate, PC(m));
					addr1 = PC(m);
					compile(cstate, stmt->inner_left);
					compile_expr(cstate, stmt->expr, BOOLOID, -1);
					SET_OPVAL(addr, addr1);
					EMIT_OPCODE(PCODE_JMP_FALSE_UNKNOWN);
					cstate->current_scope = release_psm_object(cstate, obj, 0, PC(m));
				}
				break;

			case PLPSM_STMT_COMPOUND_STATEMENT:
				{
					bool	has_sqlstate = cstate->stack.has_sqlstate;
					bool	has_sqlcode = cstate->stack.has_sqlcode;

					obj = new_psm_object_for(stmt, cstate, PC(m));
					compile(cstate, stmt->inner_left);

					cstate->stack.has_sqlstate = has_sqlstate;
					cstate->stack.has_sqlcode = has_sqlcode;

					/* generate release block */
					if (obj->calls.has_release_call)
					{
						if (obj->calls.release_calls != NULL)
						{
							int release_addr;
							/* generate a release block as subrotine */
							EMIT_CALL(PC(m) + 2);
							addr1 = PC(m);
							EMIT_OPCODE(PCODE_JMP);
							release_addr = PC(m);
							compile_release_cursors(cstate);
							EMIT_OPCODE(PCODE_RET_SUBR);
							SET_OPVAL_ADDR(addr1, addr, PC(m));
							cstate->current_scope = release_psm_object(cstate, obj, release_addr, PC(m));
						}
						else
						{
							compile_release_cursors(cstate);
							cstate->current_scope = release_psm_object(cstate, obj, 0, PC(m));
						}
					}
					else
						cstate->current_scope = release_psm_object(cstate, obj, 0, PC(m));
				}
				break;

			case PLPSM_STMT_DECLARE_VARIABLE:
				{
					ListCell *l;

					if (stmt->expr != NULL)
						compile_expr(cstate, stmt->expr, stmt->datum.typoid, stmt->datum.typmod);

					foreach(l, stmt->compound_target)
					{
						char *name = strVal(lfirst(l));
						var = create_variable_for(stmt, cstate, name, PLPSM_VARIABLE);

						if (stmt->expr != NULL)
						{
							SET_OPVALS_DATUM_COPY(target, var);
							EMIT_OPCODE(PCODE_SAVETO);
						}
						else
						{
							SET_OPVAL(target.offset, var->offset);
							EMIT_OPCODE(PCODE_SET_NULL);
						}
					}
				}
				break;

			case PLPSM_STMT_DECLARE_CURSOR:
				{
					char *name = strVal(linitial(stmt->target)) ;

					var = create_variable_for(stmt, cstate, name, PLPSM_VARIABLE);
					if (stmt->query != NULL)
					{
						Oid	*argtypes;
						int	nargs;

						var->cursor.data_addr = PC(m);

						SET_OPVAL(expr.expr, replace_vars(cstate, pstrdup(stmt->query), &argtypes, &nargs));
						SET_OPVAL(expr.nparams, nargs);
						SET_OPVAL(expr.typoids, argtypes);
						SET_OPVAL(expr.data, cstate->stack.ndata++);
						SET_OPVAL(expr.is_multicol, true);

						EMIT_OPCODE(PCODE_DATA_QUERY);
					}
				}
				break;

			case PLPSM_STMT_OPEN:
				{
					const char *fieldname;

					obj = resolve_target(cstate, stmt->target,  &fieldname, stmt->location);
					if (obj->stmt->typ != PLPSM_STMT_DECLARE_CURSOR)
						elog(ERROR, "variable \"%s\" isn't cursor", obj->name);
					SET_OPVAL(cursor.addr, obj->cursor.data_addr);
					SET_OPVAL(cursor.offset, obj->cursor.offset);
					SET_OPVAL(cursor.name, obj->name);
					EMIT_OPCODE(PCODE_CURSOR_OPEN);
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
						SET_OPVALS_DATUM_COPY(target, var);
						EMIT_OPCODE(PCODE_SQLSTATE_REFRESH);
					}

					if (cstate->stack.has_sqlcode)
					{
						Plpsm_object *var = lookup_var(cstate->current_scope, "sqlcode");
						Assert(var != NULL);
						SET_OPVALS_DATUM_COPY(target, var);
						EMIT_OPCODE(PCODE_SQLCODE_REFRESH);
					}

					foreach (l, stmt->compound_target)
					{
						Plpsm_object	*var = resolve_target(cstate, (List *) lfirst(l),
													    &fieldname,
														stmt->location);
						SET_OPVALS_DATUM_INFO(saveto_field, var);
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
					SET_OPVAL(cursor.addr, obj->cursor.data_addr);
					SET_OPVAL(cursor.offset, obj->cursor.offset);
					SET_OPVAL(cursor.name, obj->name);
					EMIT_OPCODE(PCODE_CURSOR_CLOSE);
				}
				break;

			case PLPSM_STMT_IF:
				{
					compile_expr(cstate, stmt->expr, BOOLOID, -1);
					addr1 = PC(m);
					EMIT_OPCODE(PCODE_JMP_FALSE_UNKNOWN);
					compile(cstate, stmt->inner_left);
					if (stmt->inner_right)
					{
						addr2 = PC(m);
						EMIT_OPCODE(PCODE_JMP);
						SET_OPVAL_ADDR(addr1, addr, PC(m));
						compile(cstate, stmt->inner_right);
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

						if (outer_case->expr != NULL)
						{
							StringInfoData	ds;

							initStringInfo(&ds);
							appendStringInfo(&ds, "%s IN (%s)", outer_case->expr, cond->expr);
							expr = ds.data;
						}
						else
							expr = pstrdup(cond->expr);

						compile_expr(cstate, expr, BOOLOID, -1);
						addr1 = PC(m);
						EMIT_OPCODE(PCODE_JMP_FALSE_UNKNOWN);
						compile(cstate, cond->inner_left);
						final_jmps = lappend(final_jmps, makeInteger(PC(m)));
						EMIT_OPCODE(PCODE_JMP);
						SET_OPVAL_ADDR(addr1, addr, PC(m));
						cond = cond->next;
					}

					/* when there are not ELSE path */
					if (outer_case->inner_right == NULL)
					{
						SET_OPVAL(str, "case doesn't match any value");
						EMIT_OPCODE(PCODE_SIGNAL_NODATA);
					}
					else
						compile(cstate, outer_case->inner_right);

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
					if (list_length(stmt->compound_target) > 1)
					{
						ListCell *l;
						bool	isfirst = true;
						StringInfoData ds;
						int dataidx = cstate->stack.ndata++;
						int i = 1;
						Oid	*argtypes;
						int	nargs;

						initStringInfo(&ds);

						SET_OPVAL(strbuilder.data, dataidx);
						SET_OPVAL(strbuilder.op, PLPSM_STRBUILDER_INIT);
						EMIT_OPCODE(PCODE_STRBUILDER);

						addr1 = PC(m);
						SET_OPVAL(expr.data, cstate->stack.ndata++);
						SET_OPVAL(expr.is_multicol, true);
						EMIT_OPCODE(PCODE_EXEC_EXPR);
						appendStringInfoString(&ds, "SELECT ");

						foreach (l, stmt->compound_target)
						{
							if (!isfirst)
							{
								appendStringInfoChar(&ds, ',');
								SET_OPVAL(strbuilder.data, dataidx);
								SET_OPVAL(strbuilder.op, PLPSM_STRBUILDER_APPEND_CHAR);
								SET_OPVAL(strbuilder.chr, ' ');
								EMIT_OPCODE(PCODE_STRBUILDER);
							}
							else
								isfirst = false;

							SET_OPVAL(strbuilder.data, dataidx);
							SET_OPVAL(strbuilder.op, PLPSM_STRBUILDER_APPEND_FIELD);
							SET_OPVAL(strbuilder.fnumber, i++);
							EMIT_OPCODE(PCODE_STRBUILDER);

							appendStringInfo(&ds, "(%s)::text", strVal(lfirst(l)));
						}

						SET_OPVAL_ADDR(addr1, expr.expr, replace_vars(cstate, ds.data, &argtypes, &nargs));
						SET_OPVAL_ADDR(addr1, expr.nparams, nargs);
						SET_OPVAL_ADDR(addr1, expr.typoids, argtypes);
					
						SET_OPVAL(strbuilder.data, dataidx);
						SET_OPVAL(strbuilder.op, PLPSM_STRBUILDER_PRINT_FREE);
						EMIT_OPCODE(PCODE_STRBUILDER);
					}
					else
					{
						compile_expr(cstate, strVal(linitial(stmt->compound_target)), TEXTOID, -1);
						EMIT_OPCODE(PCODE_PRINT);
					}
				}
				break;

			case PLPSM_STMT_SET:
				{
					const char *fieldname;

					if (stmt->target != NIL)
					{
						obj = resolve_target(cstate, stmt->target,  &fieldname, stmt->location);
						compile_expr(cstate, stmt->expr, obj->stmt->datum.typoid, obj->stmt->datum.typmod);
						SET_OPVALS_DATUM_COPY(target, obj);
						EMIT_OPCODE(PCODE_SAVETO);
					}
					else
					{
						Assert(stmt->compound_target != NIL && stmt->expr_list != NIL);
						compile_multiset_stmt(cstate, stmt);
					}
				}
				break;

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
							compile_expr(cstate, stmt->expr, cstate->finfo.result.datum.typoid, -1);
						else
							compile_expr(cstate, cstate->finfo.return_expr,
											cstate->finfo.result.datum.typoid, -1);

						SET_OPVAL(target.typlen, cstate->finfo.result.datum.typlen);
						SET_OPVAL(target.typbyval, cstate->finfo.result.datum.typbyval);
						EMIT_OPCODE(PCODE_RETURN);
					}
					else
					{
						Assert(cstate->finfo.return_expr == NULL);
						if (stmt->expr != NULL)
							elog(ERROR, "returned a value in VOID function");
						EMIT_OPCODE(PCODE_RETURN_VOID);
					}
				}
				break;

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
	Plpsm_pcode_module *m;
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

	m = init_module();
	cstate.module = m;

	/* 
	 * append to scope a variables for parameters, and store 
	 * instruction for copy from fcinfo
	 */
	numargs = get_func_arg_info(procTup,
						&argtypes, &argnames, &argmodes);
	cstate.finfo.nargs = numargs;
	cstate.finfo.name = pstrdup(NameStr(procStruct->proname));
	m->name = cstate.finfo.name;

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
			SET_OPVAL(copyto.src, i);
			SET_OPVAL(copyto.dest, var->offset);
			SET_OPVAL(copyto.typlen, var->stmt->datum.typlen);
			SET_OPVAL(copyto.typbyval, var->stmt->datum.typbyval);
			EMIT_OPCODE(PCODE_COPY_PARAM);
		}
		else
		{
			/* initialize OUT variables to NULL */
			SET_OPVAL(target.offset, var->offset);
			EMIT_OPCODE(PCODE_SET_NULL);
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

	compile(&cstate, plpsm_parser_tree);
	compile_done(&cstate);

	m->ndatums = cstate.stack.ndatums;
	m->ndata = cstate.stack.ndata;

	if (plpsm_debug_compiler)
		list(m);

	ReleaseSysCache(procTup);

	return m;
}
