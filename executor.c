#include "psm.h"
#include "commands/prepare.h"
#include "executor/spi.h"
#include "nodes/bitmapset.h"
#include "parser/parse_coerce.h"
#include "utils/builtins.h"
#include "utils/datum.h"
#include "utils/lsyscache.h"
#include "utils/memutils.h"

typedef struct
{
	int nargs;
	Oid	*argtypes;
	Datum	*values;
	char	*nulls;
	bool	*typbyval;
	int16	*typlen;
} Params;

Datum
plpsm_func_execute(Plpsm_pcode_module *module, FunctionCallInfo fcinfo)
{
	Datum	*values = NULL;
	char	*nulls = NULL;
	void	**DataPtrs = NULL;
	int		PC = 1;
	int		SP = 0;
	bool			clean_result = false;
	Datum		result = (Datum) 0;
	bool		isnull;
	int				CallStack[1024];
	int	sqlstate = 0;
	Bitmapset	*acursors = NULL;		/* active cursors */
	int	offset;
	int	rc;

	MemoryContext	exec_ctx;
	MemoryContext	oldctx;

	/*
	 * The temp context is a child of current context
	 */
	exec_ctx = AllocSetContextCreate(CurrentMemoryContext,
										   "plpsm execution context",
										   ALLOCSET_DEFAULT_MINSIZE,
										   ALLOCSET_DEFAULT_INITSIZE,
										   ALLOCSET_DEFAULT_MAXSIZE);

	if (module->ndatums)
	{
		values = palloc(module->ndatums * sizeof(Datum));
		nulls = palloc(module->ndatums * sizeof(char));
	}
	if (module->ndata >= 1)
	{
		DataPtrs = palloc0((module->ndata + 1) * sizeof(void*) );
	}
	
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
				if (SPI_processed == 0)
				{
					PC = pcode->addr;
					goto next_op;
				}
				break;
			case PCODE_CALL:
				if (SP == 1024)
					elog(ERROR, "runtime error, stack is full");
				CallStack[SP++] = PC + 1;
				PC = pcode->addr;
				goto next_op;

			case PCODE_CALL_NOT_FOUND:
				{
					if (SPI_processed == 0)
					{
						if (SP == 1024)
							elog(ERROR, "runtime error, stack is full");
						CallStack[SP++] = PC + 1;
						PC = pcode->addr;
						goto next_op;
					}
				}
				break;

			case PCODE_RET_SUBR:
				if (SP < 1)
					elog(ERROR, "broken stack");
				PC = CallStack[--SP];
				if (PC == 0)
					elog(ERROR, "broken stack");
				goto next_op;
				
			case PCODE_RETURN:
				{
					/*
					 * If the function's return type isn't by value, copy the value
					 * into upper executor memory context.
					 */
					if (!isnull && !pcode->target.typbyval)
					{

		    				Size		len;
						void	   *tmp;

						len = datumGetSize(result, false, pcode->target.typlen);
						tmp = SPI_palloc(len);
						memcpy(tmp, DatumGetPointer(result), len);
						result =  PointerGetDatum(tmp);
					}

					/* release a opened cursors */
					while ((offset = bms_first_member(acursors)) >= 0)
						SPI_cursor_close((Portal) DatumGetPointer(values[offset]));
					
					fcinfo->isnull = isnull;
					return (Datum) result;
				}
			case PCODE_EXEC_EXPR:
				{
					int rc;
					SPIPlanPtr plan = DataPtrs[pcode->expr.data];

					if (clean_result)
					{
						SPI_freetuptable(SPI_tuptable);
						MemoryContextReset(exec_ctx);
						clean_result = false;
					}

					if (plan == NULL)
					{
						plan =  SPI_prepare(pcode->expr.expr, pcode->expr.nparams, pcode->expr.typoids);
						DataPtrs[pcode->expr.data] = plan;
						if (plan == NULL)
							elog(ERROR, "query \"%s\" cannot be prepared", pcode->expr.expr);
					}

					oldctx = MemoryContextSwitchTo(exec_ctx);
					rc = SPI_execute_plan(plan, values, nulls, false, 2);
					MemoryContextSwitchTo(oldctx);


					if (rc != SPI_OK_SELECT)
						elog(ERROR, "SPI_execute failed executing query \"%s\" : %s",
									pcode->expr.expr, SPI_result_code_string(rc));
					clean_result = true;

					/* check if returned expression is only one value */
					if (SPI_tuptable->tupdesc->natts != 1 && !pcode->expr.is_multicol)
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

			case PCODE_EXEC_QUERY:
				{
					int rc;
					SPIPlanPtr plan = DataPtrs[pcode->expr.data];

					if (clean_result)
					{
						SPI_freetuptable(SPI_tuptable);
						MemoryContextReset(exec_ctx);
					}

					if (plan == NULL)
					{
						plan =  SPI_prepare(pcode->expr.expr, pcode->expr.nparams, pcode->expr.typoids);
						DataPtrs[pcode->expr.data] = plan;
						if (plan == NULL)
							elog(ERROR, "query \"%s\" cannot be prepared", pcode->expr.expr);
					}

					oldctx = MemoryContextSwitchTo(exec_ctx);
					rc = SPI_execute_plan(plan, values, nulls, false, 2);
					MemoryContextSwitchTo(oldctx);

					if (rc < 0)
						elog(ERROR, "SPI_execute failed executing query \"%s\" : %s",
									pcode->expr.expr, SPI_result_code_string(rc));

					clean_result = true;

					switch (rc)
					{
						case SPI_OK_SELECT:
						case SPI_OK_INSERT_RETURNING:
						case SPI_OK_DELETE_RETURNING:
						case SPI_OK_UPDATE_RETURNING:
							elog(ERROR, "query \"%s\" returns data",
												pcode->expr.expr);
					}

					sqlstate = SPI_processed > 0 ? ERRCODE_SUCCESSFUL_COMPLETION : ERRCODE_NO_DATA;
				}
				break;

			case PCODE_EXECUTE_IMMEDIATE:
				{
					char *sqlstr;
					int		rc;

					sqlstr = text_to_cstring(DatumGetTextP(result));
					SPI_freetuptable(SPI_tuptable);
					MemoryContextReset(exec_ctx);

					rc = SPI_execute(sqlstr, false, 0);

					if (rc < 0)
						elog(ERROR, "SPI_execute failed executing query \"%s\" : %s",
									sqlstr, SPI_result_code_string(rc));
					switch (rc)
					{
						case SPI_OK_SELECT:
						case SPI_OK_INSERT_RETURNING:
						case SPI_OK_DELETE_RETURNING:
						case SPI_OK_UPDATE_RETURNING:
							elog(ERROR, "query \"%s\" returns data",
												sqlstr);
					}
					pfree(sqlstr);
					SPI_freetuptable(SPI_tuptable);
					clean_result = false;
				}
				break;
			case PCODE_PREPARE:
				{
					/*
					 * There is different concept between ANSI SQL PREPARE statement and
					 * PostgreSQL PREPARE statement. More, there are not way to prepare
					 * query with params without known paramtypes via SPI. And there are
					 * not possible to execute prepared plan by PREPARE statement via
					 * parametrized SPI statements. Then PSM prepare statement, doesn't
					 * store a plan, just store a query string. All wark will be done
					 * in EXECUTE statement.
					 */
					if (DataPtrs[pcode->prepare.data] != NULL)
						pfree(DataPtrs[pcode->prepare.data]);
					DataPtrs[pcode->prepare.data] = text_to_cstring(DatumGetTextP(result));
				}
				break;
			case PCODE_PRINT:
				if (isnull)
					elog(NOTICE, "NULL");
				else
				{
					char *str = text_to_cstring(DatumGetTextP(result));
					
					elog(NOTICE, "%s", str);
					pfree(str);
				}
				break;
			case PCODE_DEBUG:
			case PCODE_NOOP:
			case PCODE_DONE:
				goto leave_process;
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
			case PCODE_SQLSTATE_REFRESH:
				{
					char *unpacked_sqlstate;

					/* release a memory */
					if (nulls[pcode->target.offset] != 'n')
						pfree(DatumGetPointer(values[pcode->target.offset]));

					if (sqlstate != 0)
						unpacked_sqlstate = unpack_sql_state(sqlstate);
					else
						unpacked_sqlstate = "00000";

					values[pcode->target.offset] = CStringGetTextDatum(unpacked_sqlstate);
					nulls[pcode->target.offset] = ' ';
				}
				break;
			case PCODE_SQLCODE_REFRESH:
				{
					values[pcode->target.offset] = Int32GetDatum(sqlstate);
					nulls[pcode->target.offset] = ' ';
				}
				break;
			case PCODE_CURSOR_OPEN:
				{
					Portal portal;
					Plpsm_pcode *data;

					data = &module->code[pcode->cursor.addr];
					portal = SPI_cursor_open_with_args(NULL, 
										data->expr.expr,
										data->expr.nparams, data->expr.typoids,
												values, nulls,
														false, 0);
					values[pcode->cursor.offset] = PointerGetDatum(portal);
					nulls[pcode->cursor.offset] = ' ';
					acursors = bms_add_member(acursors, pcode->cursor.offset);
				}
				break;

			case PCODE_CURSOR_OPEN_DYNAMIC:
				{
					Portal portal;

					char *sqlstr = DataPtrs[pcode->cursor.addr];

					if (sqlstr == NULL)
						elog(ERROR, "missing a prepared statement \"%s\"", pcode->execute.name);

					if (pcode->cursor.params != -1)
					{
						Params *params = DataPtrs[pcode->cursor.params];

						portal = SPI_cursor_open_with_args(NULL,
											sqlstr,
												 params->nargs, params->argtypes,
															    params->values, 
															    params->nulls,
															    false, 0);
					}
					else
						portal = SPI_cursor_open_with_args(NULL,
											sqlstr,
												 0, NULL,
															    NULL, NULL,
															    false, 0);
					values[pcode->cursor.offset] = PointerGetDatum(portal);
					nulls[pcode->cursor.offset] = ' ';
					acursors = bms_add_member(acursors, pcode->cursor.offset);
				}
				break;

			case PCODE_CURSOR_FETCH:
				{
					if (nulls[pcode->fetch.offset] != ' ')
						elog(ERROR, "cursor \"%s\" isn't open", pcode->fetch.name);

					if (clean_result)
					{
						SPI_freetuptable(SPI_tuptable);
						MemoryContextReset(exec_ctx);
						clean_result = false;
					}

					oldctx = MemoryContextSwitchTo(exec_ctx);
					SPI_cursor_fetch((Portal) DatumGetPointer(values[pcode->fetch.offset]), true, pcode->fetch.count);
					MemoryContextSwitchTo(oldctx);

					if (SPI_tuptable->tupdesc->natts != pcode->fetch.nvars)
						elog(ERROR, "too few or too much variables");
					clean_result = true;

					sqlstate = SPI_processed > 0 ? ERRCODE_SUCCESSFUL_COMPLETION : ERRCODE_NO_DATA;
				}
				break;

			case PCODE_SAVETO_FIELD:
				{
					/* release a memory */
					if (nulls[pcode->saveto_field.offset] != 'n' && !pcode->saveto_field.typbyval)
					{
						pfree(DatumGetPointer(values[pcode->saveto_field.offset]));
					}

					if (SPI_processed > 0)
					{
						/* ensure a correct casting */
						HeapTuple tuple = SPI_tuptable->vals[0];
						bool	isnull;
						Datum	val;

						val = SPI_getbinval(tuple, SPI_tuptable->tupdesc, pcode->saveto_field.fnumber, &isnull);
						if (!isnull)
						{
							Form_pg_attribute attr = SPI_tuptable->tupdesc->attrs[pcode->saveto_field.fnumber - 1];

							if (pcode->saveto_field.typoid != attr->atttypid || pcode->saveto_field.typmod != attr->atttypmod)
							{
								Oid			typoutput;
								bool		typIsVarlena;
								char *str;

								Oid			typinput;
								Oid			typioparam;
								FmgrInfo	finfo_input;

								getTypeOutputInfo(attr->atttypid, &typoutput, &typIsVarlena);
								str =  OidOutputFunctionCall(typoutput, val);
								getTypeInputInfo(pcode->saveto_field.typoid, &typinput, &typioparam);
								fmgr_info(typinput, &finfo_input);
								val = InputFunctionCall(&finfo_input, str, typioparam, pcode->saveto_field.typmod);
								pfree(str);
							}
							values[pcode->saveto_field.offset] = datumCopy(val, pcode->saveto_field.typbyval, pcode->saveto_field.typlen);
							nulls[pcode->saveto_field.offset] = ' ';
						}
						else
							nulls[pcode->saveto_field.offset] = 'n';
					}
					else
						nulls[pcode->saveto_field.offset] = 'n';
				}
				break;

			case PCODE_CURSOR_CLOSE:
					if (nulls[pcode->cursor.offset] != ' ')
						elog(ERROR, "cursor \"%s\" is closed", pcode->cursor.name);
					SPI_cursor_close((Portal) DatumGetPointer(values[pcode->cursor.offset]));
					nulls[pcode->cursor.offset] = 'n';
					acursors = bms_del_member(acursors, pcode->cursor.offset);
				break;

			case PCODE_CURSOR_RELEASE:
					if (nulls[pcode->cursor.offset] != 'n')
					{
						SPI_cursor_close((Portal) DatumGetPointer(values[pcode->cursor.offset]));
						nulls[pcode->cursor.offset] = 'n';
						acursors = bms_del_member(acursors, pcode->cursor.offset);
					}
				break;

			case PCODE_SIGNAL_NODATA:
				{
					if (pcode->str != NULL)
						ereport(ERROR, 
								(errcode(ERRCODE_NO_DATA_FOUND),
								 errmsg("%s", pcode->str)));
					else
						ereport(ERROR, 
								(errcode(ERRCODE_NO_DATA_FOUND),
								 errmsg("no data")));
				}
				break;
			case PCODE_SIGNAL_INVALID_CALL:
				{
					elog(ERROR, "Call or Jump on addr 0");
				}
				break;

			case PCODE_STRBUILDER:
				{
					StringInfo ds;

					switch (pcode->strbuilder.op)
					{
						case PLPSM_STRBUILDER_APPEND_CHAR:
							ds = DataPtrs[pcode->strbuilder.data];
							appendStringInfoChar(ds, pcode->strbuilder.chr);
							break;
						case PLPSM_STRBUILDER_APPEND_RESULT:
							ds = DataPtrs[pcode->strbuilder.data];
							if (isnull)
								appendStringInfoString(ds,"<NULL>");
							else
								appendStringInfo(ds, "%s", text_to_cstring(DatumGetTextP(result)));
							break;
						case PLPSM_STRBUILDER_APPEND_FIELD:
							ds = DataPtrs[pcode->strbuilder.data];
							if (SPI_processed > 0)
							{
								/* ensure a correct casting */
								HeapTuple tuple = SPI_tuptable->vals[0];
								bool	isnull;
								Datum	val;

								val = SPI_getbinval(tuple, SPI_tuptable->tupdesc, pcode->strbuilder.fnumber, &isnull);
								if (!isnull)
									appendStringInfo(ds, "%s", text_to_cstring(DatumGetTextP(val)));
								else
									appendStringInfo(ds, "<NULL>");
							}
							else
								appendStringInfo(ds, "<NULL>");
							break;
						case PLPSM_STRBUILDER_PRINT_FREE:
							ds = DataPtrs[pcode->strbuilder.data];
							ereport(NOTICE, (0, errmsg_internal("%s", ds->data)));
							pfree(ds->data);
							break;
						case PLPSM_STRBUILDER_FREE:
							ds = DataPtrs[pcode->strbuilder.data];
							pfree(ds->data);
							pfree(ds);
							break;
						case PLPSM_STRBUILDER_INIT:
							DataPtrs[pcode->strbuilder.data] = makeStringInfo();
						break;
					}
				}
				break;
			case PCODE_PARAMBUILDER:
				{
					Params *params;

					switch (pcode->parambuilder.op)
					{
						case PLPSM_PARAMBUILDER_INIT:
							params = palloc(sizeof(Params));
							params->nargs = pcode->parambuilder.nargs;
							params->argtypes = palloc(params->nargs * sizeof(Oid));
							params->values = palloc(params->nargs * sizeof(Datum));
							params->nulls = palloc(params->nargs * sizeof(char));
							params->typbyval = palloc(params->nargs * sizeof(bool));
							params->typlen = palloc(params->nargs * sizeof(int16));
							DataPtrs[pcode->parambuilder.data] = params;
							break;
						case PLPSM_PARAMBUILDER_FREE:
							{
								int i;
								params = DataPtrs[pcode->parambuilder.data];
								for (i = 0; i < params->nargs; i++)
								{
									if (params->nulls[i] != 'n' && !params->typbyval[i])
										pfree(DatumGetPointer(params->values[i]));
								}
								pfree(params->argtypes);
								pfree(params->values);
								pfree(params->nulls);
								pfree(params->typbyval);
								pfree(params->typlen);
								pfree(params);
							}
							break;
						case PLPSM_PARAMBUILDER_APPEND:
							{
								Datum	val;
								bool	isnull;
								HeapTuple tuple = SPI_tuptable->vals[0];
								int16		fnumber = pcode->parambuilder.fnumber - 1;
								Form_pg_attribute attr = SPI_tuptable->tupdesc->attrs[fnumber];

								params = DataPtrs[pcode->parambuilder.data];
								params->argtypes[fnumber] = attr->atttypid;
								val = SPI_getbinval(tuple, SPI_tuptable->tupdesc, fnumber + 1, &isnull);
								if (!isnull)
								{
									int16	typlen;
									bool	typbyval;

									get_typlenbyval(attr->atttypid, &typlen, &typbyval);
									params->typlen[fnumber] = typlen;
									params->typbyval[fnumber] = typbyval;
									params->values[fnumber] = datumCopy(val, typbyval, typlen);
									params->nulls[fnumber] = ' ';
								}
								else
									params->nulls[fnumber] = 'n';
							}
							break;
					}
				}
				break;
			case PCODE_EXECUTE:
				{
					char *sqlstr = DataPtrs[pcode->execute.sqlstr];
					
					if (sqlstr == NULL)
						elog(ERROR, "missing a prepared statement \"%s\"", pcode->execute.name);

					if (pcode->execute.params != -1)
					{
						Params *params = DataPtrs[pcode->execute.params];
						rc = SPI_execute_with_args(sqlstr, params->nargs, params->argtypes,
														    params->values, 
														    params->nulls,
														    false, 0);
					}
					else
						rc = SPI_execute(sqlstr, false, 0);

					clean_result = true;
				}
				break;
			case PCODE_CHECK_DATA:
				{
					if (SPI_processed == 0)
						elog(ERROR, "Query doesn't return data");
				}
				break;
			case PCODE_RETURN_VOID:
			case PCODE_RETURN_NULL:
				goto leave_process;
			case PCODE_DATA_QUERY:
				/* do nothing */
				break;
			default:
				elog(ERROR, "unknown pcode %d %d", pcode->typ, PC);
		}

		PC += 1;
	}

	if (PC >= module->length)
		elog(ERROR, "Segmentation fault");

leave_process:

	fcinfo->isnull = true;
	
	/* release a opened cursors */
	while ((offset = bms_first_member(acursors)) >= 0)
		SPI_cursor_close((Portal) DatumGetPointer(values[offset]));
	bms_free(acursors);

	return (Datum) 0;
}
