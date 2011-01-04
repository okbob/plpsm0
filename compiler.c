#include "psm.h"

#include "catalog/pg_proc.h"
#include "catalog/pg_type.h"
#include "utils/builtins.h"
#include "utils/syscache.h"


Plpsm_stmt *plpsm_parser_result;

static void compile(Plpsm_stmt *stmt, Plpsm_pcode_module *m);

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

static void
push_label(char *name, int addr)
{
}

static void
pop_label(char *name, int addr)
{
}

/*
 * diff from plpgsql 
 *
 * SQL/PSM is compilable language - so it checking a all SQL and expression
 * before first start of procedure. All SQL object must exists. When a access
 * to dynamic object is necessary, then dynamic SQL must be used. ???
 */
static void 
compile(Plpsm_stmt *stmt, Plpsm_pcode_module *m)
{
	int	addr1;
	int	addr2;

	while (stmt != NULL)
	{
		if (stmt->debug != NULL)
			store_debug_info(m, stmt->debug);

		switch (stmt->typ)
		{
			case PLPSM_STMT_IF:
				store_exec_expr(m, stmt->expr, BOOLOID);
				addr1 = store_jmp_not_true_unknown(m);
				compile(stmt->inner_left, m);
				if (stmt->inner_right)
				{
					addr2 = store_jmp_unknown(m);
					SET_TARGET(addr1, PC(m));
					compile(stmt->inner_right, m);
					SET_TARGET(addr2, PC(m));
				}
				else
					SET_TARGET(addr1, PC(m));
				break;

			case PLPSM_STMT_PRINT:
				store_exec_expr(m, stmt->expr, TEXTOID);
				store_print(m);
				break;

			case PLPSM_STMT_LOOP:
				if (stmt->name != NULL)
					push_label(stmt->name, PC(m));
				addr1 = PC(m);
				compile(stmt->inner_left, m);
				store_jmp(m, addr1);
				if (stmt->name != NULL)
					pop_label(stmt->name, PC(m));		/* set all leave statements */
				break;

			case PLPSM_STMT_WHILE:
				if (stmt->name != NULL)
					push_label(stmt->name, PC(m));
				addr1 = PC(m);
				store_exec_expr(m, stmt->expr, BOOLOID);
				addr2 = store_jmp_not_true_unknown(m);
				compile(stmt->inner_left, m);
				store_jmp(m, addr1);
				if (stmt->name != NULL)
					pop_label(stmt->name, PC(m));
				SET_TARGET(addr2, PC(m));
				break;

			case PLPSM_STMT_REPEAT_UNTIL:
				if (stmt->name != NULL)
					push_label(stmt->name, PC(m));
				addr1 = PC(m);
				compile(stmt->inner_left, m);
				store_exec_expr(m, stmt->expr, BOOLOID);
				addr2 = store_jmp_not_true_unknown(m);
				store_jmp(m, addr1);
				if (stmt->name != NULL)
					pop_label(stmt->name, PC(m));
				SET_TARGET(addr2, PC(m));
				break;

			case PLPSM_STMT_COMPOUND_STATEMENT:
				//push_frame(PC, stmt);
				compile(stmt->inner_left, m);
				addr1 = store_call_unknown(m);
				addr2 = store_jmp_unknown(m);
				SET_TARGET(addr1, PC(m));
				//pop_frame(PC);
				store_return(m);
				SET_TARGET(addr2, PC(m));
				break;

			case PLPSM_STMT_SET:
				store_exec_expr(m, stmt->expr, UNKNOWNOID);
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

	module = init_module();
	module->fn_name = pstrdup(NameStr(procStruct->proname));
	compile(plpsm_parser_result, module);
	store_done(module);
	list(module);

	ReleaseSysCache(procTup);
}

