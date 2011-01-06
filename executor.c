#include "psm.h"
#include "executor/spi.h"
#include "utils/builtins.h"
#include "utils/datum.h"

Datum
plpsm_func_execute(Plpsm_pcode_module *module, FunctionCallInfo fcinfo)
{
	Datum	*values;
	char	*nulls;
	int		PC = 0;
	bool			clean_result = false;
	Datum		result = (Datum) 0;
	bool		isnull;

	values = palloc(module->ndatums * sizeof(Datum));
	nulls = palloc(module->ndatums * sizeof(char));

	while (PC < module->length)
	{
		Plpsm_pcode *pcode;
next_op:

		pcode = &module->code[PC];
		switch (pcode->typ)
		{
			case PCODE_JMP_FALSE_UNKNOWN:
				{
					if (isnull || DatumGetBool(result) == false)
					{
						PC = pcode->addr;
						goto next_op;
					}
					break;
				}

			case PCODE_JMP:
				PC = pcode->addr;
				goto next_op;

			case PCODE_JMP_NOT_FOUND:
			case PCODE_CALL:
				break;
			case PCODE_RETURN:
				{
					if (!isnull)
					{
						result = datumCopy(result, pcode->target.typbyval, pcode->target.typlen);
						fcinfo->isnull = false;
						pfree(values); pfree(nulls);
						return result;
					}
					fcinfo->isnull = true;
					return (Datum) 0;
				}
			case PCODE_EXEC_EXPR:
				{
					int rc;

					if (clean_result)
						SPI_freetuptable(SPI_tuptable);

					rc = SPI_execute_with_args(pcode->expr.expr, 
								    pcode->expr.nparams, pcode->expr.typoids,
									    values, nulls,
											    false, 2);
					if (rc != SPI_OK_SELECT)
						elog(ERROR, "SPI_execute failed executing query \"%s\" : %s",
									pcode->expr.expr, SPI_result_code_string(rc));
					clean_result = true;

					/* check if returned expression is only one value */
					if (SPI_tuptable->tupdesc->natts != 1)
						elog(ERROR, "query returned %d column", 
										SPI_tuptable->tupdesc->natts);

					if (SPI_processed == 0)
					{
						isnull = true;
						result = (Datum) 0;
					}
					else if (SPI_processed != 1)
					{
						elog(ERROR, "query \"%s\" returned more than one row",
												pcode->expr.expr);
					}
					else
					{
						result = SPI_getbinval(SPI_tuptable->vals[0],
									    SPI_tuptable->tupdesc, 1, &isnull);
					}
				}
				break;
			
			case PCODE_PRINT:
				if (isnull)
					elog(NOTICE, "NULL");
				else
					elog(NOTICE, "%s", text_to_cstring(DatumGetTextP(result)));
				break;
			case PCODE_DEBUG:
			case PCODE_NOOP:
			case PCODE_DONE:
				goto leave_process;
			case PCODE_EXECUTE:
			case PCODE_IF_NOTEXIST_PREPARE:
			case PCODE_SET_NULL:
				{
					/* in this moment, a variable must be empty, so there will not be pfreed */
					nulls[pcode->target.offset] = 'n';
					break;
				}
			case PCODE_SAVETO:
				{
					/* release a memory */
					if (nulls[pcode->target.offset] != 'n' && !pcode->target.typbyval)
					{
						pfree(DatumGetPointer(values[pcode->target.offset]));
					}
					if (!isnull)
					{
						values[pcode->target.offset] = datumCopy(result, pcode->target.typbyval, pcode->target.typlen);
						nulls[pcode->target.offset] = ' ';
					}
					else
					{
						nulls[pcode->target.offset] = 'n';
					}
					break;
				}
			case PCODE_COPY_PARAM:
				{
					/* in this moment a variables must be empty */
					int paramno = pcode->copyto.src;
					if (!fcinfo->argnull[paramno])
					{
						values[pcode->copyto.dest] = datumCopy(fcinfo->arg[paramno],
												    pcode->copyto.typbyval,
												    pcode->copyto.typlen);
						nulls[pcode->copyto.dest] = ' ';
					}
					else
						nulls[pcode->copyto.dest] = 'n';
					break;
				}
			case PCODE_RETURN_VOID:
				goto leave_process;
			default:
				elog(ERROR, "unknown pcode");
		}
	
		PC += 1;
	}

	if (PC >= module->length)
		elog(ERROR, "Segmentation fault");

leave_process:

	fcinfo->isnull = true;
	pfree(values); pfree(nulls);
	return (Datum) 0;
}
