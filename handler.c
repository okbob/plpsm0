#include "psm.h"

#include "funcapi.h"
#include "executor/spi.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(psm0_call_handler);
PG_FUNCTION_INFO_V1(psm0_inline_handler);
PG_FUNCTION_INFO_V1(psm0_validator);

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

