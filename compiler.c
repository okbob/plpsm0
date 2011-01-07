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

#include "parser/parse_node.h"

Plpsm_stmt *plpsm_parser_tree;

typedef struct
{							/* it's used for storing data from SQL parser */
	int	location;
	const char *name1;
	const char *name2;
	int16 offset;
} SQLObjRef;

typedef struct
{
	Plpsm_object *top_scope;
	Plpsm_object *current_scope;			/* pointer to outer compound statement */
	int16	variables;
	int16		max_variables;			/* max variables in one visible scope */
	int16	current_offset;
	int16	ntypoids;				/* number of vector with oids */
	Oid 	*typoids;
	struct						/* used as data for parsing a SQL expression */
	{
		SQLObjRef   *objects;				/* list of substituted parameters */
		int	nobjects;				/* list of generated placeholders */
		bool	has_params;
		bool	is_expression;				/* show a message about missing variables instead missing columns */
	} pdata;
	struct
	{
		Oid	typoid;
		int16	typmod;
		bool	typbyval;
		int16	typlen;
	} resulttyp;
	struct
	{
		int	nargs;
		char		*default_return_expr;
		char *fn_name;
	} finfo;

} CompilationContextData;

typedef CompilationContextData *CompilationContext;

static void compile(CompilationContext ctxt, Plpsm_stmt *stmt, Plpsm_pcode_module *m);

static Node *resolve_column_ref(CompilationContext ctxt, ColumnRef *cref);

void plpsm_parser_setup(struct ParseState *pstate, CompilationContext ctxt);

static Plpsm_object *lookup_var(Plpsm_object *scope, const char *name);

static void store_debug_info(Plpsm_pcode_module *m, char *str);

bool	plpsm_debug_compiler = false;


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
 */
static Plpsm_object *
create_variable_for(Plpsm_stmt *decl_stmt, CompilationContext ctxt, 
						char *name, bool increment_offset)
{
	Plpsm_object *iterator = ctxt->current_scope->inner;
	Plpsm_object *var;

	Assert(ctxt->current_scope->typ == PLPSM_STMT_COMPOUND_STATEMENT);
	Assert(decl_stmt->typ == PLPSM_STMT_DECLARE_VARIABLE);

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
	var = new_object_for(decl_stmt, ctxt->current_scope);
	/* variable uses a target, not name */
	var->name = name;

	if (increment_offset)
	{
		var->offset = ctxt->current_offset++;
		if (var->offset > ctxt->ntypoids)
		{
			ctxt->ntypoids *= 2;
			ctxt->typoids = repalloc(ctxt->typoids, ctxt->ntypoids * sizeof (Oid));
		}
		ctxt->typoids[var->offset] = decl_stmt->vartype.typoid;
		ctxt->variables += 1;
		if (ctxt->variables > ctxt->max_variables)
			ctxt->max_variables = ctxt->variables;
	}

	return var;
}

/*
 * create a new psm object for psm statement
 */
static Plpsm_object *
new_psm_object_for(Plpsm_stmt *stmt, CompilationContext ctxt, int iterate_addr)
{
	Plpsm_object *new;

	switch (stmt->typ)
	{
		case PLPSM_STMT_COMPOUND_STATEMENT:
		case PLPSM_STMT_LOOP:
		case PLPSM_STMT_WHILE:
		case PLPSM_STMT_REPEAT_UNTIL:
		case PLPSM_STMT_FOR:
			if (stmt->name && lookup_object_with_label_in_scope(ctxt->current_scope, stmt->name) != NULL)
				elog(ERROR, "label \"%s\" is defined in current scope", stmt->name);
			new = new_object_for(stmt, ctxt->current_scope);
			new->iterate_addr = iterate_addr;
			ctxt->current_scope = new;
			break;
		default:
			new = new_object_for(stmt, ctxt->current_scope);
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
	CompilationContext ctxt = (CompilationContext) pstate->p_ref_hook_state;
	Node *myvar;

	myvar = resolve_column_ref(ctxt, cref);

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

	if (myvar == NULL && ctxt->pdata.is_expression)
		elog(ERROR, "variable \"%s\" isn't available in current scope", NameListToString(cref->fields));

	ctxt->pdata.has_params = true;

	return myvar;
}

/*
 * Allow only args variables accessable via $n syntax
 */
static Node *
plpsm_paramref_hook(ParseState *pstate, ParamRef *pref)
{
	CompilationContext ctxt = (CompilationContext) pstate->p_ref_hook_state;
	int	paramno = pref->number;
	Param	*param;

	if (paramno <= 0 || paramno > ctxt->finfo.nargs)
		ereport(ERROR,
				(errcode(ERRCODE_UNDEFINED_PARAMETER),
				 errmsg("there is no parameter $%d", paramno),
				 parser_errposition(pstate, pref->location)));

	param = makeNode(Param);
	param->paramkind = PARAM_EXTERN;
	param->paramid = paramno;
	param->paramtype = ctxt->typoids[paramno - 1];
	param->paramtypmod = -1;
	param->location = pref->location;

	ctxt->pdata.has_params = true;

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
	param->paramtype = var->stmt->vartype.typoid;
	param->paramtypmod = var->stmt->vartype.typmod;
	param->location = cref->location;

	return (Node *) param;
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
			if (iterator->typ == PLPSM_STMT_DECLARE_VARIABLE && strcmp(iterator->name, name) == 0)
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
		if (iterator->typ == PLPSM_STMT_DECLARE_VARIABLE && strcmp(iterator->name, name) == 0)
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
appendObjRef(CompilationContext ctxt, int location, const char *name1, const char *name2, int offset)
{
	SQLObjRef *ro = &ctxt->pdata.objects[ctxt->pdata.nobjects++];
	ro->location = location;
	ro->name1 = name1;
	ro->name2 = name2;
	ro->offset = offset;
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
resolve_column_ref(CompilationContext ctxt, ColumnRef *cref)
{
	const char *name1;
	const char *name2 = NULL;
	Plpsm_object *var;

	switch (list_length(cref->fields))
	{
		case 1:
			{
				Node	*field1 = (Node *) linitial(cref->fields);
				Assert(IsA(field1, String));
				name1 = strVal(field1);
				var = lookup_var(ctxt->current_scope, name1);
				if (var != NULL)
				{
					appendObjRef(ctxt, cref->location, name1, NULL, var->offset + 1);
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

				var = lookup_qualified_var(ctxt->current_scope, name1, name2);
				if (var != NULL)
				{
					if (lookup_var(ctxt->current_scope, name1))
						elog(ERROR, "there is conflict between compound statement label and composite variable");
					appendObjRef(ctxt, cref->location, name1, name2, var->offset + 1);
					return make_param(var, cref);
				}
				else
				{
					var = lookup_var(ctxt->current_scope, name1);
					if (var != NULL)
					{
						appendObjRef(ctxt, cref->location, name1, NULL, var->offset + 1);
						return make_param(var, cref);
					}
				}
			}
		case 3:
			{
				Node	*field1 = (Node *) linitial(cref->fields);
				Node	*field2 = (Node *) lsecond(cref->fields);

				Assert(IsA(field1, String));
				name1 = strVal(field1);
				Assert(IsA(field2, String));
				name2 = strVal(field2);

				var = lookup_qualified_var(ctxt->current_scope, name1, name2);
				if (var != NULL)
				{
					appendObjRef(ctxt, cref->location, name1, name2, var->offset + 1);
					return make_param(var, cref);
				}
			}
	}

	return NULL;
}

void
plpsm_parser_setup(struct ParseState *pstate, CompilationContext ctxt)
{
	pstate->p_post_columnref_hook = plpsm_post_column_ref;
	pstate->p_paramref_hook = plpsm_paramref_hook;
	pstate->p_ref_hook_state = (void *) ctxt;
}


/*
 * Compile a expression - replace a variables by placeholders,
 *
 * This is really hack. Better way is using a possible query deparsing,
 * but necessary methods are not accesable now.
 */
static char *
compile_expr_simple_target(CompilationContext ctxt, char *expr, Oid typoid, int16 typmod, bool *no_params)
{

	List       *raw_parsetree_list;
	StringInfoData ds;
	StringInfoData		cds;
	int i = 0;
	int				loc = 0;
	int		j;
	SQLObjRef *objects;
	SQLObjRef	auxObjRef;
	int	nobjects;
	bool	not_sorted;
	char *ptr;
	ListCell *li;

	initStringInfo(&ds);
	appendStringInfo(&ds, "SELECT (%s)::%s", expr, format_type_with_typemod(typoid, typmod));

	ctxt->pdata.objects = (SQLObjRef *) palloc(ctxt->variables * sizeof(SQLObjRef));
	ctxt->pdata.nobjects = 0;
	ctxt->pdata.has_params = false;
	ctxt->pdata.is_expression = true;

	raw_parsetree_list = pg_parse_query(ds.data);

	foreach(li, raw_parsetree_list)
	{
		 Node       *parsetree = (Node *) lfirst(li);

		pg_analyze_and_rewrite_params((Node *) parsetree, ds.data, (ParserSetupHook) plpsm_parser_setup, (void *) ctxt);
	}

	/* fast path, there are no params */
	if (ctxt->pdata.nobjects == 0)
	{
		*no_params = !ctxt->pdata.has_params;
		return ds.data;
	}

	*no_params = false;

	/* now, pdata.params and pdata.cref_list contains a info about placeholders */
	objects = ctxt->pdata.objects;
	nobjects = ctxt->pdata.nobjects;

	/* simple bublesort */
	not_sorted = true;
	while (not_sorted)
	{
		not_sorted = false;
		for (i = 0; i < nobjects - 1; i++)
		{
			if (objects[i].location > objects[i+1].location)
			{
				not_sorted = true;
				auxObjRef = objects[i];
				objects[i] = objects[i+1]; objects[i+1] = auxObjRef;
			}
		}
	}

	initStringInfo(&cds);
	ptr = ds.data; j = 0;

	while (*ptr)
	{
		char *str;

		/* copy content to position of replaced object reference */
		while (loc < objects[j].location)
		{
			appendStringInfoChar(&cds, *ptr++);
			loc++;
		}

		/* replace ref. object by placeholder */
		appendStringInfo(&cds,"$%d", (int) objects[j].offset);

		/* find and skip a referenced object */
		str = strstr(ptr, objects[j].name1);
		Assert(str != NULL);
		str += strlen(objects[j].name1);
		loc += str - ptr; ptr = str;

		/* when object was referenced with qualified name, then skip second identif */
		if (objects[j].name2 != NULL)
		{
			str = strstr(ptr, objects[j].name2);
			Assert(str != NULL);
			str += strlen(objects[j].name2);
			loc += str - ptr; ptr = str;
		}

		if (++j >= nobjects)
		{
			/* copy to end and leave */
			while (*ptr)
				appendStringInfoChar(&cds, *ptr++);
			break;
		}
	}

	pfree(ds.data);
	pfree(objects);

	return cds.data;
}

/*
 * returns a target objects for assign statement. When target is composite type with 
 * specified field, then fieldname is filled.
 */
static Plpsm_object *
resolve_target(CompilationContext ctxt, List *target, const char **fieldname, int location)
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
				var = lookup_var(ctxt->current_scope, name1);
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

				var = lookup_qualified_var(ctxt->current_scope, name1, name2);
				if (var != NULL)
				{
					if (lookup_var(ctxt->current_scope, name1))
						elog(ERROR, "there is conflict between compound statement label and composite variable");
					return var;
				}
				else
				{
					var = lookup_var(ctxt->current_scope, name1);
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

				var = lookup_qualified_var(ctxt->current_scope, name1, name2);
				if (var == NULL)
					elog(ERROR, "a missing target variable \"%s\" with label \"%s\"", name2, name1);
				*fieldname = name3;
			}
	}

	return NULL;
}


/*
 * Create a overdimensioned module
 */
static Plpsm_pcode_module *
init_module(void)
{
	Plpsm_pcode_module *m = palloc(10000 * sizeof(Plpsm_pcode) + offsetof(Plpsm_pcode_module, code));
	m->max_length = 10000;
	m->length = 0;
	return m;
}

static void 
list(Plpsm_pcode_module *m)
{
	StringInfoData ds;
	int pc;

	initStringInfo(&ds);

	appendStringInfo(&ds, "   Datums: %d variables \n", m->ndatums);
	appendStringInfo(&ds, "   Size: %d instruction\n\n", m->length);

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
					
					appendStringInfo(&ds, "ExecExpr \"%s\",{%s}", m->code[pc].expr.expr,ds2.data);
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
				appendStringInfo(&ds, "SetNull %d", m->code[pc].target.offset);
				break;
			case PCODE_SAVETO:
				appendStringInfo(&ds, "SaveTo %d, size:%d, byval:%s", m->code[pc].target.offset, m->code[pc].target.typlen,
										m->code[pc].target.typbyval ? "BYVAL" : "BYREF");
				break;
			case PCODE_COPY_PARAM:
				appendStringInfo(&ds, "CopyParam %d, %d, size:%d, byval:%s", 
										m->code[pc].copyto.src,
										m->code[pc].copyto.dest,
										m->code[pc].copyto.typlen,
										m->code[pc].copyto.typbyval ? "BYVAL" : "BYREF");
				break;
			case PCODE_RETURN_VOID:
				appendStringInfo(&ds, "Return 0");
				break;
		}
		appendStringInfoChar(&ds, '\n');
	}
	elog(NOTICE, "\n%s", ds.data);
	pfree(ds.data);
}

#define CHECK_MODULE_SIZE(m)		if (m->length == m->max_length) elog(ERROR, "module is too long")
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
store_exec_expr(CompilationContext ctxt, Plpsm_pcode_module *m, char *expr, Oid targetoid, int16 typmod)
{
	char *cexpr;
	Oid	*typoids;
	bool	no_params;

	CHECK_MODULE_SIZE(m);

	cexpr = compile_expr_simple_target(ctxt, expr, targetoid, typmod, &no_params);
	if (!no_params)
	{
		/* copy a actual typeoid vector to private typoid vector */
		typoids = palloc(ctxt->variables * sizeof(Oid));
		memcpy(typoids, ctxt->typoids, ctxt->variables);
		SET_EXPR(m, cexpr, ctxt->variables, ctxt->typoids);
	}
	else
	{
		SET_EXPR(m, cexpr, 0, NULL);
	}

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
	m->code[m->length].copyto.typlen = var->stmt->vartype.typlen; 
	m->code[m->length].copyto.typbyval = var->stmt->vartype.typbyval;
	SET_AND_INC_PC(m, PCODE_COPY_PARAM);
}


static void
store_return(Plpsm_pcode_module *m)
{
	CHECK_MODULE_SIZE(m);
	SET_AND_INC_PC(m, PCODE_RETURN);
}

static void
store_done(CompilationContext ctxt, Plpsm_pcode_module *m)
{
	if (ctxt->finfo.default_return_expr != NULL && ctxt->resulttyp.typoid != VOIDOID)
	{
		store_exec_expr(ctxt, m, ctxt->finfo.default_return_expr,
						ctxt->resulttyp.typoid, -1);
		SET_DATUM_PROP(m, ctxt->resulttyp.typlen, ctxt->resulttyp.typbyval);
		SET_AND_INC_PC(m, PCODE_RETURN);
	}
	CHECK_MODULE_SIZE(m);
	SET_AND_INC_PC(m, PCODE_DONE);
}

static Plpsm_object *
release_psm_object(Plpsm_object *obj)
{
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
compile(CompilationContext ctxt, Plpsm_stmt *stmt, Plpsm_pcode_module *m)
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
				obj = new_psm_object_for(stmt, ctxt, PC(m));
				addr1 = PC(m);
				compile(ctxt, stmt->inner_left, m);
				store_jmp(m, addr1);
				ctxt->current_scope = release_psm_object(obj);
				break;

			case PLPSM_STMT_WHILE:
				obj = new_psm_object_for(stmt, ctxt, PC(m));
				addr1 = PC(m);
				store_exec_expr(ctxt, m, stmt->expr, BOOLOID, -1);
				addr2 = store_jmp_not_true_unknown(m);
				compile(ctxt, stmt->inner_left, m);
				store_jmp(m, addr1);
				SET_TARGET(addr2, PC(m));
				ctxt->current_scope = release_psm_object(obj);
				break;

			case PLPSM_STMT_REPEAT_UNTIL:
				obj = new_psm_object_for(stmt, ctxt, PC(m));
				addr1 = PC(m);
				compile(ctxt, stmt->inner_left, m);
				store_exec_expr(ctxt, m, stmt->expr, BOOLOID, -1);
				store_jmp_not_true(m, addr1);
				ctxt->current_scope = release_psm_object(obj);
				break;

			case PLPSM_STMT_COMPOUND_STATEMENT:
				{
					int16	offset = ctxt->current_offset;
					int16	variables = ctxt->variables;

					obj = new_psm_object_for(stmt, ctxt, PC(m));
					compile(ctxt, stmt->inner_left, m);
					ctxt->current_offset = offset;
					if (ctxt->variables > ctxt->max_variables)
						ctxt->max_variables = ctxt->variables;
					ctxt->variables = variables;

					/* generate release block */
					if (obj->has_release_block)
					{
						if (obj->release_address_list != NULL)
						{
							/* generate a release block as subrotine */
							addr1 = store_call_unknown(m);
							addr2 = store_jmp_unknown(m);
							SET_TARGET(addr1, PC(m));
							store_return(m);
							SET_TARGET(addr2, PC(m));
						}
						else
						{
						}
					}
					
					ctxt->current_scope = release_psm_object(obj);
					break;
				}

			case PLPSM_STMT_DECLARE_VARIABLE:
				{
					ListCell *l;
					foreach(l, stmt->compound_target)
					{
						char *name = strVal(lfirst(l));
						obj = create_variable_for(stmt, ctxt, name, true);

						if (stmt->expr != NULL)
						{
							store_exec_expr(ctxt, m, stmt->expr, stmt->vartype.typoid, stmt->vartype.typmod);
							store_saveto(m, obj->offset, stmt->vartype.typlen, stmt->vartype.typbyval);
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

			case PLPSM_STMT_IF:
				store_exec_expr(ctxt, m, stmt->expr, BOOLOID, -1);
				addr1 = store_jmp_not_true_unknown(m);
				compile(ctxt, stmt->inner_left, m);
				if (stmt->inner_right)
				{
					addr2 = store_jmp_unknown(m);
					SET_TARGET(addr1, PC(m));
					compile(ctxt, stmt->inner_right, m);
					SET_TARGET(addr2, PC(m));
				}
				else
					SET_TARGET(addr1, PC(m));
				break;

			case PLPSM_STMT_PRINT:
				store_exec_expr(ctxt, m, stmt->expr, TEXTOID, -1);
				store_print(m);
				break;

			case PLPSM_STMT_SET:
				{
					const char *fieldname;

					obj = resolve_target(ctxt, stmt->target,  &fieldname, stmt->location);
					store_exec_expr(ctxt, m, stmt->expr, obj->stmt->vartype.typoid, obj->stmt->vartype.typmod);
					store_saveto(m, obj->offset, obj->stmt->vartype.typlen, obj->stmt->vartype.typbyval);
					break;
				}

			case PLPSM_STMT_RETURN:
				{
					if (ctxt->resulttyp.typoid != VOIDOID)
					{
						if (ctxt->finfo.default_return_expr != NULL && 
							stmt->expr != NULL)
							elog(ERROR, "using RETURN expr in function with OUT arguments");

						if (stmt->expr != NULL)
							store_exec_expr(ctxt, m, stmt->expr, ctxt->resulttyp.typoid, -1);
						else
							store_exec_expr(ctxt, m, ctxt->finfo.default_return_expr,
											ctxt->resulttyp.typoid, -1);
						SET_DATUM_PROP(m, ctxt->resulttyp.typlen, ctxt->resulttyp.typbyval);
						SET_AND_INC_PC(m, PCODE_RETURN);
					}
					else
					{
						Assert(ctxt->finfo.default_return_expr == NULL);
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
	CompilationContextData ctxt;
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

	ctxt.top_scope = &outer_scope;
	ctxt.top_scope->name = outer_scope.name;
	ctxt.current_scope = ctxt.top_scope;
	ctxt.variables = 0;
	ctxt.max_variables = 0;
	ctxt.current_offset = 0;
	ctxt.ntypoids = 256;
	ctxt.typoids = palloc(256 * sizeof(Oid));

	ctxt.resulttyp.typoid = procStruct->prorettype;
	get_typlenbyval(ctxt.resulttyp.typoid, &ctxt.resulttyp.typlen, &ctxt.resulttyp.typbyval);

	module = init_module();

	/* 
	 * append to scope a variables for parameters, and store 
	 * instruction for copy from fcinfo
	 */
	numargs = get_func_arg_info(procTup,
						&argtypes, &argnames, &argmodes);
	ctxt.finfo.nargs = numargs;
	ctxt.finfo.fn_name = pstrdup(NameStr(procStruct->proname));

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

		decl_stmt->vartype.typoid = argtypid;
		decl_stmt->vartype.typmod = -1;
		decl_stmt->vartype.typename = NULL;
		decl_stmt->vartype.typlen = typlen;
		decl_stmt->vartype.typbyval = typbyval;

		/* append implicit name to scope */
		var = create_variable_for(decl_stmt, &ctxt, pstrdup(buf), true);
		/* append explicit name to scope */
		if (argnames && argnames[i][0] != '\0')
		{
			alias = create_variable_for(decl_stmt, &ctxt, pstrdup(argnames[i]), false);
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
		ctxt.finfo.default_return_expr = ds.data;

		bms_free(outargs);
	}
	else
		ctxt.finfo.default_return_expr = NULL;

	compile(&ctxt, plpsm_parser_tree, module);
	module->ndatums = ctxt.max_variables;
	store_done(&ctxt, module);
	if (plpsm_debug_compiler)
		list(module);

	ReleaseSysCache(procTup);

	return module;
}

