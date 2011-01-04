#include "psm.h"

#include "catalog/pg_proc.h"
#include "catalog/pg_type.h"
#include "utils/builtins.h"
#include "utils/syscache.h"


Plpsm_stmt *plpsm_parser_tree;

typedef struct
{
	Plpsm_object *top_scope;
	Plpsm_object *current_scope;			/* pointer to outer compound statement */
	int16	variables;
	int16		max_variables;			/* max variables in one visible scope */
	int16	current_offset;
} CompilationContextData;

typedef CompilationContextData *CompilationContext;

static void compile(CompilationContext ctxt, Plpsm_stmt *stmt, Plpsm_pcode_module *m);

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
get_object_with_label_in_scope(Plpsm_object *scope, char *name)
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
	return get_object_with_label_in_scope(scope->outer, name);
}

/*
 * new variable
 */
static Plpsm_object *
new_variable(Plpsm_object *scope, char *name, Plpsm_stmt *stmt)
{
	Plpsm_object *iterator = scope->inner;
	Plpsm_object *var;

	Assert(scope->typ == PLPSM_STMT_COMPOUND_STATEMENT);

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
	var = new_object_for(stmt, scope);
	var->name = name;

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
			if (stmt->name && get_object_with_label_in_scope(ctxt->current_scope, stmt->name) != NULL)
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
 * Create a overdimensioned module
 */
static Plpsm_pcode_module *
init_module(void)
{
	Plpsm_pcode_module *m = palloc(10000 * sizeof(Plpsm_pcode) + offsetof(Plpsm_pcode_module, code));
	m->max_length = 10000;
	m->length = 0;
	m->prep_statements_nums = 0;
	return m;
}

static void 
list(Plpsm_pcode_module *m)
{
	StringInfoData ds;
	int pc;

	initStringInfo(&ds);
	for (pc = 0; pc < m->length; pc++)
	{
		appendStringInfo(&ds, "%5d", pc);
		appendStringInfoChar(&ds, '\t');
		if (m->code[pc].typ != PCODE_DEBUG)
			appendStringInfoChar(&ds, '\t');

		switch (m->code[pc].typ)
		{
			case PCODE_JMP_FALSE_UNKNOWN:
				appendStringInfo(&ds, "jmp false_unknown, %d", m->code[pc].addr);
				break;
			case PCODE_JMP:
				appendStringInfo(&ds, "jmp %d", m->code[pc].addr);
				break;
			case PCODE_JMP_NOT_FOUND:
				appendStringInfo(&ds, "jmp not_found, %d", m->code[pc].addr);
				break;
			case PCODE_CALL:
				appendStringInfo(&ds, "call %d", m->code[pc].addr);
				break;
			case PCODE_RETURN:
				appendStringInfo(&ds, "return");
				break;
			case PCODE_EXEC_EXPR:
				appendStringInfo(&ds, "exec \"%s\", %d", m->code[pc].expr.expr,  m->code[pc].expr.oid);
				break;
			case PCODE_PRINT:
				appendStringInfo(&ds, "print");
				break;
			case PCODE_DEBUG:
				appendStringInfo(&ds, "/* %s */", m->code[pc].str);
				break;
			case PCODE_NOOP:
				appendStringInfo(&ds, "noop");
				break;
			case PCODE_DONE:
				appendStringInfoString(&ds, "done");
				break;
			case PCODE_EXECUTE:
				appendStringInfo(&ds, "execute %s()", m->code[pc].str);
				break;
			case PCODE_IF_NOTEXIST_PREPARE:
				appendStringInfo(&ds, "prepere_ifnexist %s as %s", m->code[pc].prep.name, m->code[pc].prep.expr);
				break;
			case PCODE_SET_NULL:
				appendStringInfo(&ds, "set_to_null %d", m->code[pc].offset);
				break;
			case PCODE_PALLOC:
				appendStringInfo(&ds, "palloc %dB", m->code[pc].size);
				break;
			case PCODE_SAVETO:
				appendStringInfo(&ds, "store to %d", m->code[pc].offset);
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
#define SET_EXPR(m, e, o)		m->code[m->length].expr.expr = e; m->code[m->length].expr.oid = o
#define SET_TARGET(a, ta)		m->code[a].addr = ta
#define PC(m)				m->length
#define SET_PREP(m, n, q)		m->code[m->length].prep.name = n; m->code[m->length].prep.expr = q
#define SET_SIZE(m, a, s)		m->code[a].size = s
#define SET_OFFSET(m, o)		m->code[m->length].offset = o

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
store_exec_expr(Plpsm_pcode_module *m, char *expr, Oid targetoid)
{
	char *query;
	StringInfoData ds;

	CHECK_MODULE_SIZE(m);
	initStringInfo(&ds);
	appendStringInfo(&ds, "SELECT (%s)::%d", expr, targetoid);
	query = pstrdup(ds.data);
	resetStringInfo(&ds);
	appendStringInfo(&ds, "___pp_%s_%d", m->fn_name, m->prep_statements_nums++);
	SET_PREP(m, ds.data, query);
	SET_AND_INC_PC(m, PCODE_IF_NOTEXIST_PREPARE);
	CHECK_MODULE_SIZE(m);
	SET_STR(m, ds.data);
	SET_AND_INC_PC(m, PCODE_EXECUTE);
}

static void
store_print(Plpsm_pcode_module *m)
{
	CHECK_MODULE_SIZE(m);
	SET_AND_INC_PC(m, PCODE_PRINT);
}

static void
store_saveto(Plpsm_pcode_module *m, int16 offset)
{
	CHECK_MODULE_SIZE(m);
	SET_OFFSET(m, offset);
	SET_AND_INC_PC(m, PCODE_SAVETO);
}

static void
store_return(Plpsm_pcode_module *m)
{
	CHECK_MODULE_SIZE(m);
	SET_AND_INC_PC(m, PCODE_RETURN);
}

static void
store_done(Plpsm_pcode_module *m)
{
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
		//if (stmt->debug != NULL)
		//	store_debug_info(m, stmt->debug);

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
				store_exec_expr(m, stmt->expr, BOOLOID);
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
				store_exec_expr(m, stmt->expr, BOOLOID);
				addr2 = store_jmp_not_true_unknown(m);
				store_jmp(m, addr1);
				SET_TARGET(addr2, PC(m));
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
			elog(NOTICE, ">>>>%s", name);

				obj = new_variable(ctxt->current_scope, name, stmt);
				obj->offset = ctxt->current_offset++;
				if (stmt->expr != NULL)
				{
					store_exec_expr(m, stmt->expr, stmt->vartype.typoid);
					store_saveto(m, obj->offset);
				}
				else
				{
					CHECK_MODULE_SIZE(m);
					SET_OFFSET(m, obj->offset);
					SET_AND_INC_PC(m, PCODE_SET_NULL);
				}
				ctxt->variables += 1;
				}
				}
				break;

			case PLPSM_STMT_IF:
				store_exec_expr(m, stmt->expr, BOOLOID);
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
				store_exec_expr(m, stmt->expr, TEXTOID);
				store_print(m);
				break;

			case PLPSM_STMT_SET:
				//store_exec_expr(m, stmt->expr, UNKNOWNOID);
				break;

			default:
				elog(ERROR, "unknown command typeid");
		}
		stmt = stmt->next;
	}
}


void
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

	procTup = SearchSysCache1(PROCOID, ObjectIdGetDatum(funcOid));
	if (!HeapTupleIsValid(procTup))
		elog(ERROR, "cache lookup failed for function %u", funcOid);

	procStruct = (Form_pg_proc) GETSTRUCT(procTup);

	prosrcdatum = SysCacheGetAttr(PROCOID, procTup,
							Anum_pg_proc_prosrc, &isnull);
	if (isnull)
		elog(ERROR, "null prosrc");

	proc_source = TextDatumGetCString(prosrcdatum);

	elog(NOTICE, "source code: %s", proc_source);

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

	module = init_module();
	module->fn_name = pstrdup(NameStr(procStruct->proname));
	SET_AND_INC_PC(module, PCODE_PALLOC);
	compile(&ctxt, plpsm_parser_tree, module);
	SET_SIZE(module, 0, ctxt.max_variables  * sizeof(Plpsm_value));
	store_done(module);
	list(module);

	ReleaseSysCache(procTup);
}

