#include "psm.h"

#include "funcapi.h"
#include "catalog/pg_proc.h"
#include "catalog/pg_type.h"
#include "commands/trigger.h"
#include "executor/spi.h"
#include "utils/builtins.h"
#include "utils/guc.h"
#include "utils/lsyscache.h"
#include "utils/syscache.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(psm0_call_handler);
PG_FUNCTION_INFO_V1(psm0_inline_handler);
PG_FUNCTION_INFO_V1(psm0_validator);

/*
 * _PG_init()			- library load-time initialization
 *
 * DO NOT make this static nor change its name!
 */
void
_PG_init(void)
{
	/* Be sure we do initialization only once (should be redundant now) */
	static bool inited = false;

	if (inited)
		return;

	DefineCustomBoolVariable("plpsm.debug_parser",
						"when is true, then parser output is raised",
						NULL,
						&plpsm_debug_parser,
						false,
						PGC_SUSET, 0,
						NULL, NULL);

	DefineCustomBoolVariable("plpsm.debug_compiler",
						"when is true, then compiler output is raised",
						NULL,
						&plpsm_debug_compiler,
						false,
						PGC_SUSET, 0,
						NULL, NULL);

	plpsm_HashTableInit();

	inited = true;
}


Datum
psm0_validator(PG_FUNCTION_ARGS)
{
	Oid			funcoid = PG_GETARG_OID(0);
	HeapTuple	tuple;
	Form_pg_proc proc;
	char		functyptype;
	int			numargs;
	Oid		   *argtypes;
	char	  **argnames;
	char	   *argmodes;
	bool		istrigger = false;
	int			i;

	/* Get the new function's pg_proc entry */
	tuple = SearchSysCache1(PROCOID, ObjectIdGetDatum(funcoid));
	if (!HeapTupleIsValid(tuple))
		elog(ERROR, "cache lookup failed for function %u", funcoid);
	proc = (Form_pg_proc) GETSTRUCT(tuple);

	functyptype = get_typtype(proc->prorettype);

	/* Disallow pseudotype result */
	/* except for TRIGGER, RECORD, VOID, or polymorphic */
	if (functyptype == TYPTYPE_PSEUDO)
	{
		/* we assume OPAQUE with no arguments means a trigger */
		if (proc->prorettype == TRIGGEROID ||
			(proc->prorettype == OPAQUEOID && proc->pronargs == 0))
			istrigger = true;
		else if (proc->prorettype != VOIDOID && proc->prorettype != RECORDOID)
			ereport(ERROR,
					(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
					 errmsg("PLPSM functions cannot return type %s",
							format_type_be(proc->prorettype))));
	}

	/* Disallow pseudotypes in arguments (either IN or OUT) */
	/* except for polymorphic */
	numargs = get_func_arg_info(tuple,
								&argtypes, &argnames, &argmodes);
	for (i = 0; i < numargs; i++)
	{
		if (get_typtype(argtypes[i]) == TYPTYPE_PSEUDO)
		{
			ereport(ERROR,
					(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
					 errmsg("PLPSM functions cannot accept type %s",
							format_type_be(argtypes[i]))));
		}
	}

	/* Postpone body checks if !check_function_bodies */
	if (check_function_bodies)
	{
		FunctionCallInfoData fake_fcinfo;
		FmgrInfo	flinfo;
		TriggerData trigdata;
		int			rc;

		/*
		 * Connect to SPI manager (is this needed for compilation?)
		 */
		if ((rc = SPI_connect()) != SPI_OK_CONNECT)
			elog(ERROR, "SPI_connect failed: %s", SPI_result_code_string(rc));

		/*
		 * Set up a fake fcinfo with just enough info to satisfy
		 * plpgsql_compile().
		 */
		MemSet(&fake_fcinfo, 0, sizeof(fake_fcinfo));
		MemSet(&flinfo, 0, sizeof(flinfo));
		fake_fcinfo.flinfo = &flinfo;
		flinfo.fn_oid = funcoid;
		flinfo.fn_mcxt = CurrentMemoryContext;
		if (istrigger)
		{
			MemSet(&trigdata, 0, sizeof(trigdata));
			trigdata.type = T_TriggerData;
			fake_fcinfo.context = (Node *) &trigdata;
		}

		/* Test-compile the function */
		plpsm_compile(&fake_fcinfo, true);

		/*
		 * Disconnect from SPI manager
		 */
		if ((rc = SPI_finish()) != SPI_OK_FINISH)
			elog(ERROR, "SPI_finish failed: %s", SPI_result_code_string(rc));
	}

	ReleaseSysCache(tuple);

	PG_RETURN_VOID();
}


Datum
psm0_call_handler(PG_FUNCTION_ARGS)
{
	Plpsm_module *module;
	Datum		retval;
	int			rc;

	/*
	 * Connect to SPI manager
	 */
	if ((rc = SPI_connect()) != SPI_OK_CONNECT)
		elog(ERROR, "SPI_connect failed: %s", SPI_result_code_string(rc));

	/* Find or compile the function */
	module = plpsm_compile(fcinfo, false);

	module->use_count++;
	PG_TRY();
	{
		retval = plpsm_func_execute(module, fcinfo);
	}
	PG_CATCH();
	{
		module->use_count--;
		PG_RE_THROW();
	}
	PG_END_TRY();

	module->use_count--;

	/*
	 * Disconnect from SPI manager
	 */
	if ((rc = SPI_finish()) != SPI_OK_FINISH)
		elog(ERROR, "SPI_finish failed: %s", SPI_result_code_string(rc));

	return retval;
}

Datum
psm0_inline_handler(PG_FUNCTION_ARGS)
{
	PG_RETURN_NULL();
}

