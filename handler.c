#include "psm.h"

#include "funcapi.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(psm0_call_handler);
PG_FUNCTION_INFO_V1(psm0_inline_handler);
PG_FUNCTION_INFO_V1(psm0_validator);

Datum
psm0_call_handler(PG_FUNCTION_ARGS)
{

	PG_RETURN_NULL();
}

Datum
psm0_inline_handler(PG_FUNCTION_ARGS)
{
	PG_RETURN_NULL();
}

Datum
psm0_validator(PG_FUNCTION_ARGS)
{
	Oid			funcOid = PG_GETARG_OID(0);

	plpsm_compile(funcOid, true);

	PG_RETURN_VOID();
}
