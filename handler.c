#include "psm.h"

#include "funcapi.h"
#include "executor/spi.h"
#include "utils/guc.h"

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

	inited = true;
}


Datum
psm0_validator(PG_FUNCTION_ARGS)
{
	Oid			funcOid = PG_GETARG_OID(0);

	plpsm_compile(funcOid, true);

	PG_RETURN_VOID();
}


Datum
psm0_call_handler(PG_FUNCTION_ARGS)
{
	Plpsm_pcode_module *module;

	Oid			funcOid = fcinfo->flinfo->fn_oid;
	Datum		retval;
	int			rc;

	/*
	 * Connect to SPI manager
	 */
	if ((rc = SPI_connect()) != SPI_OK_CONNECT)
		elog(ERROR, "SPI_connect failed: %s", SPI_result_code_string(rc));

	/* Find or compile the function */
	module = plpsm_compile(funcOid, false);
	retval = plpsm_func_execute(module, fcinfo);

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

