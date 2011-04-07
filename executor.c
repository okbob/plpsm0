#include "psm.h"

#include "postgres.h"
#include "funcapi.h"

#include "access/xact.h"
#include "catalog/pg_type.h"
#include "commands/prepare.h"
#include "executor/spi.h"
#include "nodes/bitmapset.h"
#include "parser/parse_coerce.h"
#include "utils/builtins.h"
#include "utils/datum.h"
#include "utils/lsyscache.h"
#include "utils/typcache.h"
#include "utils/memutils.h"
#include "utils/resowner.h"

typedef struct
{
	int			level;
	int64	row_count;
	int	sqlstate;
	char		*message_text;
	char		*detail_text;
	char		*hint_text;
	char		*condition_identifier;
} DiagnosticsInfoData;

typedef DiagnosticsInfoData *DiagnosticsInfo;

typedef struct
{
	int nargs;
	Oid	*argtypes;
	Datum	*values;
	char	*nulls;
	bool	*typbyval;
	int16	*typlen;
} Params;

typedef struct
{
	Plpsm_module *module;
	Datum		*values;
	char		*nulls;
	int		*PC;
	char		**src;
	bool		is_signal;
	FmgrInfo		*out_funcs;
} DebugInfoData;

typedef DebugInfoData *DebugInfo;

static void plpsm_exec_error_callback(void *arg);

/*
 * Returns a addr of relevant handler or release all nested transaction and
 * returns zero.
 */
static int
search_handler(Plpsm_module *mod, char *condition_name, int sqlstate, ResourceOwner *ROstack, int *ROP,
								    int htnum, Plpsm_handler_type *htyp,
								    DiagnosticsInfoData *DInfoStack, DiagnosticsInfo first_area, int *DID)
{
	int handler_addr = 0;

	if (htnum > 0 && sqlstate != ERRCODE_QUERY_CANCELED)
	{
		int	ht_addr = htnum;
		Plpsm_pcode *ht_item = &mod->ht_table->code[ht_addr];

		Assert(ht_item->typ == PCODE_HT);
		while (ht_item->HT_field.typ != PLPSM_HT_STOP)
		{
			Plpsm_ht_type typ = ht_item->HT_field.typ;

			if (typ == PLPSM_HT_CONDITION_NAME && condition_name != NULL)
			{
				if (strcmp(ht_item->HT_field.condition_name, condition_name) == 0)
				{
					handler_addr = ht_item->HT_field.addr;
					break;
				}
			}
			else if (typ == PLPSM_HT_SQLCODE)
			{
				if (ht_item->HT_field.sqlcode == sqlstate)
				{
					handler_addr = ht_item->HT_field.addr;
					break;
				}
			}
			else if (typ == PLPSM_HT_SQLCLASS)
			{
				if (ht_item->HT_field.sqlclass == ERRCODE_TO_CATEGORY(sqlstate))
				{
					handler_addr = ht_item->HT_field.addr;
					break;
				}
			}
			else if (typ == PLPSM_HT_SQLEXCEPTION)
			{
				int	eclass = ERRCODE_TO_CATEGORY(sqlstate);

				if (eclass != MAKE_SQLSTATE('0','2','0','0','0') &&
					eclass != MAKE_SQLSTATE('0','1','0','0','0'))
				{
					handler_addr = ht_item->HT_field.addr;
					break;
				}
			}
			else if (typ == PLPSM_HT_SQLWARNING)
			{
				int	eclass = ERRCODE_TO_CATEGORY(sqlstate);

				if (eclass == MAKE_SQLSTATE('0','2','0','0','0') ||
					eclass == MAKE_SQLSTATE('0','1','0','0','0'))
				{
					handler_addr = ht_item->HT_field.addr;
					break;
				}
			}
			else if (typ == PLPSM_HT_PARENT)
			{
				ht_addr = ht_item->HT_field.parent_HT_addr;
				ht_item = &mod->ht_table->code[--ht_addr];
				continue;
			}
			else if (typ == PLPSM_HT_RELEASE_SUBTRANSACTION)
			{
				RollbackAndReleaseCurrentSubTransaction();
				CurrentResourceOwner = ROstack[(*ROP)--];
				SPI_restore_connection();
			}
			else if (typ == PLPSM_HT_DIAGNOSTICS_POP)
			{
					if (*DID == -1)
						elog(ERROR, "runtime error, diagnostics stack is empty");

				first_area->level = DInfoStack[*DID].level;
				first_area->sqlstate = DInfoStack[*DID].sqlstate;
				first_area->message_text = DInfoStack[*DID].message_text;
				first_area->detail_text = DInfoStack[*DID].detail_text;
				first_area->hint_text = DInfoStack[*DID].hint_text;
				first_area->condition_identifier = DInfoStack[*DID].condition_identifier;

				(*DID)--;
			}

			ht_item = &mod->ht_table->code[--ht_addr];
		}

		if (handler_addr > 0)
		{
			if (ht_item->HT_field.htyp == PLPSM_HANDLER_UNDO)
			{
				RollbackAndReleaseCurrentSubTransaction();
				CurrentResourceOwner = ROstack[(*ROP)--];
				SPI_restore_connection();
			}

			if (htyp != NULL)
				*htyp = ht_item->HT_field.htyp;

			return handler_addr;
		}
	}

	/*
	 * We didn't find a handler, so now rollback all handlers
	 */
	while (*ROP >= 0)
	{
		/* rollback all nested transaction */
		RollbackAndReleaseCurrentSubTransaction();
		CurrentResourceOwner = ROstack[(*ROP)--];
		SPI_restore_connection();
	}
	return 0;

}

static void
set_state_variable(Plpsm_module *mod, int sqlstate, int PC, Datum *values, char *nulls)
{
	Plpsm_pcode *pcode;
	Plpsm_pcode_module *module = mod->code;

	if (PC == 0)
		elog(ERROR, "invalid memory reference");

	pcode = &module->code[PC];
	switch (pcode->typ)
	{
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
			/* do nothing */
	}
}

static void 
set_text(char **ptr, char *str)
{
	if (*ptr != NULL)
		pfree(*ptr);
	if (str != NULL)
		*ptr = pstrdup(str);
	else
		*ptr = NULL;
}

static void
set_diagnostics(DiagnosticsInfo dginfo, ErrorData *edata)
{
	dginfo->level = edata->elevel;
	dginfo->row_count = 0;
	dginfo->sqlstate = edata->sqlerrcode;

	set_text(&dginfo->message_text, edata->message);
	set_text(&dginfo->detail_text, edata->detail);
	set_text(&dginfo->hint_text, edata->hint);
	set_text(&dginfo->condition_identifier, NULL);
}

static void
diagnostics_sqlstate(DiagnosticsInfo dginfo, int sqlstate, int level, int64 row_count)
{
	dginfo->level = level;
	dginfo->row_count = row_count;
	dginfo->sqlstate = sqlstate;
	set_text(&dginfo->message_text, NULL);
	set_text(&dginfo->detail_text, NULL);
	set_text(&dginfo->hint_text, NULL);
	set_text(&dginfo->condition_identifier, NULL);
}

static void
diagnostics_info_move(DiagnosticsInfo target, DiagnosticsInfo src)
{
	target->level = src->level;
	target->row_count = src->row_count;
	target->sqlstate = src->sqlstate;

	if (target->message_text != NULL)
		pfree(target->message_text);
	if (target->detail_text != NULL)
		pfree(target->detail_text);
	if (target->hint_text != NULL)
		pfree(target->hint_text);
	if (target->condition_identifier != NULL)
		pfree(target->condition_identifier);

	target->message_text = src->message_text;
	target->detail_text = src->detail_text;
	target->hint_text = src->hint_text;
	target->condition_identifier = src->condition_identifier;

	src->message_text = NULL;
	src->detail_text = NULL;
	src->hint_text = NULL;
	src->condition_identifier = NULL;
}

static Datum
execute_module(Plpsm_module *mod, FunctionCallInfo fcinfo, DebugInfo dinfo)
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
	ResourceOwner			ResourceOwnerStack[1024];
	DiagnosticsInfoData		DInfoStack[128];
	DiagnosticsInfoData		first_area;
	DiagnosticsInfoData		signal_properties;
	int		ROP = -1;
	int	sqlstate = 0;
	int64		row_count;
	int		DID = -1;
	Bitmapset	*acursors = NULL;		/* active cursors */
	int	rc;
	Plpsm_pcode_module *module = mod->code;
	bool		is_read_only = module->is_read_only;
	char	*src = NULL;
	FmgrInfo	*out_funcs = NULL;

	MemoryContext	exec_ctx;
	MemoryContext	oldctx;
	MemoryContext	func_cxt = mod->cxt;

	bool		rollback_nested_transactions = false;

	bool 	keep_diagnostics_info = false;		/* when is true, then statement actualises first_area */
	bool	use_stacked_diagnostics = false;	/* when is true, then system moves info between fist_area and stacked second area */

	DataPtrs = mod->DataPtrs;

	/* initialise stack of signal properties */
	signal_properties.level = 0;
	signal_properties.row_count = 0;
	signal_properties.sqlstate = 0;
	signal_properties.message_text = NULL;
	signal_properties.detail_text = NULL;
	signal_properties.hint_text = NULL;
	signal_properties.condition_identifier = NULL;

	/* 
	 * Output functions for types used in variables are not searchable in 
	 * exception time - because it needs a disabled access to system tables.
	 * So it's necessary to prepare this table now.
	 */
	if (mod->with_cframe_debug)
		out_funcs = palloc0(module->ndatums * sizeof(FmgrInfo));

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

	if (dinfo != NULL)
	{
		dinfo->module = mod;
		dinfo->values = values;
		dinfo->nulls = nulls;
		dinfo->PC = &PC;
		dinfo->src = &src;
		dinfo->is_signal = false;
		dinfo->out_funcs = out_funcs;
	}

	if (mod->with_cframe_debug)
		init_out_funcs(mod->code->code[1].cframe, out_funcs);

	while (PC < module->length)
	{
		Plpsm_pcode *pcode;
next_op:

/*

 dinfo->is_signal = true;
 elog(NOTICE, "PC %d  ... sqlstate:%d", PC, sqlstate); 
 dinfo->is_signal = false;

*/

		if (PC == 0)
			elog(ERROR, "invalid memory reference");

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
				}
				break;

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
					goto leave_process;
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

					oldctx = CurrentMemoryContext;

					PG_TRY();
					{
						if (plan == NULL)
						{
							plan =  SPI_prepare(pcode->expr.expr, pcode->expr.nparams, pcode->expr.typoids);
							if (plan == NULL)
								elog(ERROR, "query \"%s\" cannot be prepared", pcode->expr.expr);

							oldctx = MemoryContextSwitchTo(func_cxt);
							DataPtrs[pcode->expr.data] = SPI_saveplan(plan);
							MemoryContextSwitchTo(oldctx);
						}

						oldctx = MemoryContextSwitchTo(exec_ctx);
						rc = SPI_execute_plan(plan, values, nulls, is_read_only, 2);
						MemoryContextSwitchTo(oldctx);
					}
					PG_CATCH();
					{
						ErrorData *edata;
						int		handler_addr;
						
						MemoryContextSwitchTo(oldctx);
						edata = CopyErrorData();
						FlushErrorState();

						rollback_nested_transactions = true;

						handler_addr = search_handler(mod, NULL, edata->sqlerrcode, ResourceOwnerStack, &ROP, pcode->htnum, NULL,
												DInfoStack, &first_area, &DID);
						if (handler_addr > 0)
						{
							sqlstate = edata->sqlerrcode;
							/* 
							 * before we jump to handler, we should to set
							 * a state variables. A addresses of these variables
							 * are stored in next two instructions - if are used
							 */
							set_state_variable(mod, sqlstate, PC + 1, values, nulls);
							set_state_variable(mod, sqlstate, PC + 2, values, nulls);

							if (keep_diagnostics_info)
								set_diagnostics(&first_area, edata);

							FreeErrorData(edata);

							/* go to handler */
							rollback_nested_transactions = false;
							PC = handler_addr;
							goto next_op;
						}
						else
						{
							/* 
							 * we should to release a allocated resources. The most important
							 * is correct finishing a nested transactions.
							 */
							while (ROP >= 0)
							{
								if (rollback_nested_transactions)
									RollbackAndReleaseCurrentSubTransaction();
								else
									ReleaseCurrentSubTransaction();

								CurrentResourceOwner = ResourceOwnerStack[ROP--];
							}

							ReThrowError(edata);
						}
					}
					PG_END_TRY();

					MemoryContextSwitchTo(oldctx);

					clean_result = true;

					/* check if returned expression is only one value */
					if (SPI_tuptable->tupdesc->natts != 1 && !pcode->expr.is_multicol)
						elog(ERROR, "EEEEquery returned %d column", 
										SPI_tuptable->tupdesc->natts);

					sqlstate = SPI_processed > 0 ? ERRCODE_SUCCESSFUL_COMPLETION : ERRCODE_NO_DATA;
					row_count = SPI_processed;

					if (keep_diagnostics_info && !pcode->expr.without_diagnostics)
						diagnostics_sqlstate(&first_area, sqlstate, WARNING, row_count);

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

						oldctx = MemoryContextSwitchTo(func_cxt);
						DataPtrs[pcode->expr.data] = SPI_saveplan(plan);
						MemoryContextSwitchTo(oldctx);
					}

					oldctx = MemoryContextSwitchTo(exec_ctx);
					rc = SPI_execute_plan(plan, values, nulls, is_read_only, 2);
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
					row_count = SPI_processed;

					if (keep_diagnostics_info)
						diagnostics_sqlstate(&first_area, sqlstate, WARNING, row_count);
				}
				break;

			case PCODE_EXECUTE_IMMEDIATE:
				{
					char *sqlstr;
					int		rc;

					sqlstr = text_to_cstring(DatumGetTextP(result));
					SPI_freetuptable(SPI_tuptable);
					MemoryContextReset(exec_ctx);

					rc = SPI_execute(sqlstr, is_read_only, 0);

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
					sqlstate = SPI_processed > 0 ? ERRCODE_SUCCESSFUL_COMPLETION : ERRCODE_NO_DATA;
					row_count = SPI_processed;

					if (keep_diagnostics_info)
						diagnostics_sqlstate(&first_area, sqlstate, WARNING, row_count);

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
				{
					if (dinfo != NULL)
						dinfo->is_signal = true;

					if (isnull)
						elog(NOTICE, "NULL");
					else
					{
						char *str = text_to_cstring(DatumGetTextP(result));

						elog(NOTICE, "%s", str);
						pfree(str);
					}

					if (dinfo != NULL)
						dinfo->is_signal = false;
				}
				break;

			case PCODE_DONE:
				goto leave_process;

			case PCODE_SET_NULL:
				{
					/* in this moment, a variable must be empty, so there will not be pfreed */
					nulls[pcode->target.offset] = 'n';
				}
				break;

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
				}
				break;

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

			case PCODE_SET_SQLSTATE:
				{
					sqlstate = pcode->sqlstate;
				}
				break;

			case PCODE_DIAGNOSTICS_INIT:
				{
					first_area.row_count = 0;
					first_area.sqlstate = 0;
					first_area.message_text = NULL;
					first_area.detail_text = NULL;
					first_area.hint_text = NULL;
					first_area.condition_identifier = NULL;

					keep_diagnostics_info = true;
					use_stacked_diagnostics = pcode->use_stacked_diagnostics;
				}
				break;

			case PCODE_DIAGNOSTICS_PUSH:
				{
					if (++DID == 128)
						elog(ERROR, "runtime error, diagnostics stack overflow");

					DInfoStack[DID].level = first_area.level;
					DInfoStack[DID].sqlstate = first_area.sqlstate;
					DInfoStack[DID].message_text = first_area.message_text != NULL ? pstrdup(first_area.message_text) : NULL;
					DInfoStack[DID].detail_text = first_area.detail_text != NULL ? pstrdup(first_area.detail_text) : NULL;
					DInfoStack[DID].hint_text = first_area.hint_text != NULL ? pstrdup(first_area.hint_text) : NULL;
					DInfoStack[DID].condition_identifier = first_area.condition_identifier != NULL ? 
										pstrdup(first_area.condition_identifier) : NULL;
				}
				break;

			case PCODE_DIAGNOSTICS_POP:
				{
					if (DID == -1)
						elog(ERROR, "runtime error, diagnostics stack is empty");

					first_area.level = DInfoStack[DID].level;
					first_area.sqlstate = DInfoStack[DID].sqlstate;
					first_area.message_text = DInfoStack[DID].message_text;
					first_area.detail_text = DInfoStack[DID].detail_text;
					first_area.hint_text = DInfoStack[DID].hint_text;
					first_area.condition_identifier = DInfoStack[DID].condition_identifier;

					DID--;
				}
				break;

			case PCODE_GET_DIAGNOSTICS:
				{
					DiagnosticsInfo darea;
					Datum value = (Datum) 0;

					if (pcode->get_diagnostics.which_area == PLPSM_GDAREA_CURRENT)
						darea = &first_area;
					else
					{
						/* access to a stacked diagnostics info */
						if (DID == -1)
							elog(ERROR, "diagnostics exception — stacked diagnostics accessed without active handler");
						darea = &DInfoStack[DID];
					}

					if (nulls[pcode->get_diagnostics.offset] == ' ' && !pcode->get_diagnostics.byval)
						pfree(DatumGetPointer(values[pcode->get_diagnostics.offset]));

					switch (pcode->get_diagnostics.typ)
					{
						case PLPSM_GDINFO_DETAIL:
							
							if (darea->detail_text != NULL)
								value = CStringGetTextDatum(darea->detail_text);
							else
								value = CStringGetTextDatum("");
							break;

						case PLPSM_GDINFO_HINT:
							if (darea->hint_text != NULL)
								value = CStringGetTextDatum(darea->hint_text);
							else
								value = CStringGetTextDatum("");
							break;

						case PLPSM_GDINFO_MESSAGE:
							if (darea->message_text)
								value = CStringGetTextDatum(darea->message_text);
							else
								value = CStringGetTextDatum("");
							break;

						case PLPSM_GDINFO_CONDITION_IDENTIFIER:
							if (darea->condition_identifier)
								value = CStringGetTextDatum(darea->condition_identifier);
							else
								value = CStringGetTextDatum("");
							break;

						case PLPSM_GDINFO_SQLSTATE:
							value = CStringGetTextDatum(unpack_sql_state(darea->sqlstate));
							break;

						case PLPSM_GDINFO_SQLCODE:
							if (pcode->get_diagnostics.target_type == INT4OID)
								value = Int32GetDatum(darea->sqlstate);
							else
								value = Int64GetDatum((int64) darea->sqlstate);
							break;

						case PLPSM_GDINFO_ROW_COUNT:
							if (pcode->get_diagnostics.target_type == INT4OID)
								values[pcode->get_diagnostics.offset] = 
											DirectFunctionCall1(int84, Int64GetDatum(darea->row_count));
							else
								values[pcode->get_diagnostics.offset] = Int64GetDatum(darea->row_count);
							break;

						default:
							/* be compiler quite */;
					}
					
					values[pcode->get_diagnostics.offset] = value;
					nulls[pcode->get_diagnostics.offset] = ' ';
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
														is_read_only, 0);
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
															    is_read_only, 0);
					}
					else
						portal = SPI_cursor_open_with_args(NULL,
											sqlstr,
												 0, NULL,
															    NULL, NULL,
															    is_read_only, 0);
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
					row_count = SPI_processed;

					if (keep_diagnostics_info)
						diagnostics_sqlstate(&first_area, sqlstate, WARNING, row_count);
				}
				break;

			case PCODE_STORE_SP:
				{
					int offset = pcode->target.offset;
					values[offset] = SP;
					break;
				}

			case PCODE_LOAD_SP:
				{
					int offset = pcode->target.offset;
					SP = values[offset];
					break;
				}

			case PCODE_BEGIN_SUBTRANSACTION:
				{
					MemoryContext	oldcontext = CurrentMemoryContext;

					if (++ROP >= 1024)
						elog(ERROR, "too much nested transactions");

					ResourceOwnerStack[ROP] = CurrentResourceOwner;
					BeginInternalSubTransaction(NULL);

					MemoryContextSwitchTo(oldcontext);
					SPI_restore_connection();
					break;
				}

			case PCODE_RELEASE_SUBTRANSACTION:
				{
					MemoryContext	oldcontext = CurrentMemoryContext;

					if (ROP == -1)
						elog(NOTICE, "there are no active subtransaction");
					if (rollback_nested_transactions)
						RollbackAndReleaseCurrentSubTransaction();
					else
						ReleaseCurrentSubTransaction();

					MemoryContextSwitchTo(oldcontext);
					CurrentResourceOwner = ResourceOwnerStack[ROP--];
					SPI_restore_connection();
					break;
				}

			case PCODE_UPDATE_FIELD:
				{
					int	fno = pcode->update_field.fno;
					int	fnumber = pcode->update_field.fnumber;
					int	offset = pcode->update_field.offset;
					bool	isnull;
					Datum		value = (Datum) 0;

					if (SPI_processed > 0)
						value = SPI_getbinval(SPI_tuptable->vals[0], SPI_tuptable->tupdesc, fnumber, &isnull);
					else
						isnull = true;

					if (nulls[offset] != 'n' || !isnull)
					{
						int	natts;
						Datum		*_values;
						bool		*_nulls;
						bool		*_replaces;
						HeapTupleHeader		td;
						HeapTupleData		tmptup;
						Oid	tupType;
						int16	tupTypmod;
						TupleDesc tupdesc;
						HeapTuple newtup;
						HeapTupleHeader result;


						if (pcode->update_field.typoid != RECORDOID)
						{
							tupdesc = lookup_rowtype_tupdesc(pcode->update_field.typoid, pcode->update_field.typmod);
						}
						else
						{
							td = DatumGetHeapTupleHeader(value);

							tupType = HeapTupleHeaderGetTypeId(td);
							tupTypmod = HeapTupleHeaderGetTypMod(td);
							tupdesc = lookup_rowtype_tupdesc(tupType, tupTypmod);
						}

						natts = tupdesc->natts;

						_values = palloc(sizeof(Datum) * natts);
						_nulls = palloc(sizeof(bool) * natts);
						_replaces = palloc(sizeof(bool) * natts);
						memset(_replaces, false, sizeof(bool) * natts);

						BlessTupleDesc(tupdesc);

						/* check possible IO conversion */
						if (!isnull)
						{
							Form_pg_attribute src_attr = SPI_tuptable->tupdesc->attrs[fnumber - 1];
							Form_pg_attribute dest_attr = tupdesc->attrs[fno - 1];

							if (src_attr->atttypid != dest_attr->atttypid || src_attr->atttypmod != dest_attr->atttypmod)
							{
								Oid			typoutput;
								bool		typIsVarlena;
								char *str;

								Oid			typinput;
								Oid			typioparam;
								FmgrInfo	finfo_input;

								getTypeOutputInfo(src_attr->atttypid, &typoutput, &typIsVarlena);
								str =  OidOutputFunctionCall(typoutput, value);
								getTypeInputInfo(dest_attr->atttypid, &typinput, &typioparam);
								fmgr_info(typinput, &finfo_input);
								value = InputFunctionCall(&finfo_input, str, typioparam, dest_attr->atttypmod);
								pfree(str);
							}
						}

						if (nulls[pcode->update_field.offset] != 'n')
						{
							td = DatumGetHeapTupleHeader(values[offset]);
							_values[fno - 1] = value;
							_replaces[fno - 1] = true;
							_nulls[fno - 1] = isnull;

							tmptup.t_len = HeapTupleHeaderGetDatumLength(td);
							ItemPointerSetInvalid(&(tmptup.t_self));
							tmptup.t_tableOid = InvalidOid;
							tmptup.t_data = td;

							newtup = heap_modify_tuple(&tmptup, tupdesc, _values, _nulls, _replaces);
						}
						else
						{
							_values[fno - 1] = value;
							_nulls[fno - 1] = isnull;

							newtup = heap_form_tuple(tupdesc, _values, _nulls);
						}

						result = (HeapTupleHeader) palloc(newtup->t_len);
						memcpy(result, newtup->t_data, newtup->t_len);
						heap_freetuple(newtup);

						values[offset] = PointerGetDatum(result);

						nulls[offset] = ' ';

						ReleaseTupleDesc(tupdesc);

						pfree(_values);
						pfree(_nulls);
						pfree(_replaces);
					}
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
							if (dinfo)
								dinfo->is_signal = true;
							ereport(NOTICE, (0, errmsg_internal("%s", ds->data)));
							if (dinfo)
								dinfo->is_signal = false;
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
							{
								params = palloc(sizeof(Params));
								params->nargs = pcode->parambuilder.nargs;
								params->argtypes = palloc(params->nargs * sizeof(Oid));
								params->values = palloc(params->nargs * sizeof(Datum));
								params->nulls = palloc(params->nargs * sizeof(char));
								params->typbyval = palloc(params->nargs * sizeof(bool));
								params->typlen = palloc(params->nargs * sizeof(int16));
								DataPtrs[pcode->parambuilder.data] = params;
							}
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
														    is_read_only, 0);
					}
					else
						rc = SPI_execute(sqlstr, is_read_only, 0);

					sqlstate = SPI_processed > 0 ? ERRCODE_SUCCESSFUL_COMPLETION : ERRCODE_NO_DATA;

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
				goto leave_process;

			case PCODE_RETURN_NULL:
				isnull = true;
				goto leave_process;

			case PCODE_DATA_QUERY:
				/* do nothing */
				break;

			case PCODE_DEBUG_SOURCE_CODE:
				{
					src = pcode->str;
				}
				break;

			case PCODE_SIGNAL_CALL:
			case PCODE_SIGNAL_JMP:
				{
					if (pcode->signal_params.addr != 0)
					{
						/* 
						 * Actualise a first_area when it is requested
						 * Handler must to push and pop these first_area on stack when it is requested
						 */
						if (keep_diagnostics_info)
							diagnostics_info_move(&first_area, &signal_properties);

						if (pcode->signal_params.is_undo_handler)
						{
							MemoryContext	oldcontext = CurrentMemoryContext;

							RollbackAndReleaseCurrentSubTransaction();
							MemoryContextSwitchTo(oldcontext);
							CurrentResourceOwner = ResourceOwnerStack[ROP--];
							SPI_restore_connection();
						}

						if (pcode->typ == PCODE_SIGNAL_JMP)
						{
							PC = pcode->addr;
							goto next_op;
						}
						else
						{
							if (SP == 1024)
								elog(ERROR, "runtime error, stack is full");
							CallStack[SP++] = PC + 1;
							PC = pcode->addr;
							goto next_op;
						}
					}
					else
					{
						/* raise a outer exception */
						int oldinfo = dinfo->is_signal;
						char *message_text;

						/* dont append context informations to notices */
						if (signal_properties.level == NOTICE)
							dinfo->is_signal = true;

						/* 
						 * in this moment we don't call a handler, so when level is NOTICE or WARNING
						 * we must to actualise a diagnostics info.
						 */
						if (keep_diagnostics_info && (signal_properties.level == NOTICE || 
								    signal_properties.level == WARNING))
							diagnostics_info_move(&first_area, &signal_properties);

						if (signal_properties.message_text != NULL)
							message_text = signal_properties.message_text;
						else if (signal_properties.condition_identifier != NULL)
							message_text = signal_properties.condition_identifier;
						else
							message_text = "";

						/* 
						 * we should to release a allocated resources. The most important
						 * is correct finishing a nested transactions.
						 */
						while (ROP >= 0)
						{
							if (rollback_nested_transactions)
								RollbackAndReleaseCurrentSubTransaction();
							else
								ReleaseCurrentSubTransaction();

							CurrentResourceOwner = ResourceOwnerStack[ROP--];
						}

						ereport(signal_properties.level,
								( errcode(signal_properties.sqlstate),
								 errmsg_internal("%s", message_text),
								 (signal_properties.detail_text != NULL) ? errdetail("%s", signal_properties.detail_text) : 0,
								 (signal_properties.hint_text != NULL) ? errhint("%s", signal_properties.hint_text) : 0));
						dinfo->is_signal = oldinfo;
					}
				}
				break;

			case PCODE_RESIGNAL_CALL:
			case PCODE_RESIGNAL_JMP:
				{
					/* 
					 * now signal_properties has a mix of stacked info and resignal stmt info,
					 * if we know a target address, we can call there.
					 */
					if (pcode->signal_params.addr != 0)
					{
						/* 
						 * Actualise a first_area when it is requested
						 * Handler must to push and pop these first_area on stack when it is requested
						 */
						if (keep_diagnostics_info)
							diagnostics_info_move(&first_area, &signal_properties);

						if (pcode->signal_params.is_undo_handler)
						{
							MemoryContext	oldcontext = CurrentMemoryContext;

							RollbackAndReleaseCurrentSubTransaction();
							MemoryContextSwitchTo(oldcontext);
							CurrentResourceOwner = ResourceOwnerStack[ROP--];
							SPI_restore_connection();
						}

						if (pcode->typ == PCODE_SIGNAL_JMP)
						{
							PC = pcode->addr;
							goto next_op;
						}
						else
						{
							if (SP == 1024)
								elog(ERROR, "runtime error, stack is full");
							CallStack[SP++] = PC + 1;
							PC = pcode->addr;
							goto next_op;
						}
					}
					else
					{
						int		handler_addr;
						Plpsm_handler_type htyp;

						handler_addr = search_handler(mod, signal_properties.condition_identifier, signal_properties.sqlstate, 
														ResourceOwnerStack, &ROP, pcode->htnum, &htyp,
														DInfoStack, &first_area, &DID);
						if (handler_addr > 0)
						{
							if (keep_diagnostics_info)
								diagnostics_info_move(&first_area, &signal_properties);

							if (htyp == PLPSM_HANDLER_CONTINUE)
							{
								if (SP == 1024)
									elog(ERROR, "runtime error, stack is full");
								CallStack[SP++] = PC + 1;
								PC = handler_addr;
								goto next_op;
							}
							else
							{
								PC = handler_addr;
								goto next_op;
							}
						}
						else
						{
							/* there isn't local handler, so emit outer handler */
							/* raise a outer exception */
							int oldinfo = dinfo->is_signal;
							char *message_text;

							/* dont append context informations to notices */
							if (signal_properties.level == NOTICE)
								dinfo->is_signal = true;

							/* 
							 * in this moment we don't call a handler, so when level is NOTICE or WARNING
							 * we must to actualise a diagnostics info.
							 */
							if (keep_diagnostics_info && (signal_properties.level == NOTICE || 
									    signal_properties.level == WARNING))
								diagnostics_info_move(&first_area, &signal_properties);

							if (signal_properties.message_text != NULL)
								message_text = signal_properties.message_text;
							else if (signal_properties.condition_identifier != NULL)
								message_text = signal_properties.condition_identifier;
							else
								message_text = "";

							/* 
							 * we should to release a allocated resources. The most important
							 * is correct finishing a nested transactions.
							 */
							while (ROP >= 0)
							{
								if (rollback_nested_transactions)
									RollbackAndReleaseCurrentSubTransaction();
								else
									ReleaseCurrentSubTransaction();

								CurrentResourceOwner = ResourceOwnerStack[ROP--];
							}

							ereport(signal_properties.level,
									( errcode(signal_properties.sqlstate),
									 errmsg_internal("%s", message_text),
									 (signal_properties.detail_text != NULL) ? errdetail("%s", signal_properties.detail_text) : 0,
									 (signal_properties.hint_text != NULL) ? errhint("%s", signal_properties.hint_text) : 0));
							dinfo->is_signal = oldinfo;
						}
					}
				}
				break;

			case PCODE_SIGNAL_PROPERTY:
				{
					switch (pcode->signal_property.typ)
					{
						case PLPSM_SIGNAL_PROPERTY_RESET:
							{
								signal_properties.level = 0;
								signal_properties.row_count = 0;
								signal_properties.sqlstate = 0;
								set_text(&signal_properties.message_text, NULL);
								set_text(&signal_properties.detail_text, NULL);
								set_text(&signal_properties.hint_text, NULL);
								set_text(&signal_properties.condition_identifier, NULL);
							}
							break;

						case PLPSM_SIGNAL_PROPERTY_LOAD_STACKED:
							{
								if (DID == -1)
									elog(ERROR, "runtime error, diagnostick stack is empty");

								signal_properties.level = DInfoStack[DID].level;
								signal_properties.row_count = 0;
								signal_properties.sqlstate = DInfoStack[DID].sqlstate;
								sqlstate = signal_properties.sqlstate;
								set_text(&signal_properties.message_text, DInfoStack[DID].message_text);
								set_text(&signal_properties.detail_text, DInfoStack[DID].detail_text);
								set_text(&signal_properties.hint_text, DInfoStack[DID].hint_text);
								set_text(&signal_properties.condition_identifier, DInfoStack[DID].condition_identifier);
							}
							break;

						case PLPSM_SIGNAL_PROPERTY_SET_INT:
							{
								Assert(pcode->signal_property.gdtyp == PLPSM_GDINFO_SQLCODE ||
									pcode->signal_property.gdtyp == PLPSM_GDINFO_LEVEL);

								switch (pcode->signal_property.gdtyp)
								{
									case PLPSM_GDINFO_SQLCODE:
										signal_properties.sqlstate = pcode->signal_property.ival;
										sqlstate = signal_properties.sqlstate;
										break;
									case PLPSM_GDINFO_LEVEL:
										signal_properties.level = pcode->signal_property.ival;
										break;
									default:
										elog(ERROR, "internal error, diagnostics variable isn't of int type");
								}
							}
							break;

						case PLPSM_SIGNAL_PROPERTY_SET_CSTRING:
							{
								Assert(pcode->signal_property.gdtyp == PLPSM_GDINFO_DETAIL ||
									pcode->signal_property.gdtyp == PLPSM_GDINFO_HINT ||
									pcode->signal_property.gdtyp == PLPSM_GDINFO_MESSAGE ||
									pcode->signal_property.gdtyp == PLPSM_GDINFO_CONDITION_IDENTIFIER);

								switch (pcode->signal_property.gdtyp)
								{
									case PLPSM_GDINFO_DETAIL:
										set_text(&signal_properties.detail_text,
												pstrdup(pcode->signal_property.cstr));
										break;
									case PLPSM_GDINFO_HINT:
										set_text(&signal_properties.hint_text,
												pstrdup(pcode->signal_property.cstr));
										break;
									case PLPSM_GDINFO_MESSAGE:
										set_text(&signal_properties.message_text,
												pstrdup(pcode->signal_property.cstr));
										break;
									case PLPSM_GDINFO_CONDITION_IDENTIFIER:
										set_text(&signal_properties.condition_identifier,
												pstrdup(pcode->signal_property.cstr));
										break;
									default:
										elog(ERROR, "internal error, diagnostics variable isn't of text type");
								}
							}
							break;

						case PLPSM_SIGNAL_PROPERTY_COPY_TEXT_VAR:
							{
								char *cstr = NULL;

								Assert(pcode->signal_property.gdtyp == PLPSM_GDINFO_DETAIL ||
									pcode->signal_property.gdtyp == PLPSM_GDINFO_HINT ||
									pcode->signal_property.gdtyp == PLPSM_GDINFO_MESSAGE);

								if (nulls[pcode->signal_property.offset] == 'n')
									elog(ERROR, "a diagnostics value is NULL");
								else
									cstr = text_to_cstring(DatumGetTextP(values[pcode->signal_property.offset]));

								switch (pcode->signal_property.gdtyp)
								{
									case PLPSM_GDINFO_DETAIL:
										set_text(&signal_properties.detail_text, cstr);
										break;
									case PLPSM_GDINFO_HINT:
										set_text(&signal_properties.hint_text, cstr);
										break;
									case PLPSM_GDINFO_MESSAGE:
										set_text(&signal_properties.message_text, cstr);
										break;
									default:
										elog(ERROR, "internal error, diagnostics variable isn't of text type");
								}
							}
							break;
					}
				}
				break;

			default:
				elog(ERROR, "unknown pcode %d %d", pcode->typ, PC);
		}

		PC += 1;
	}

	if (PC >= module->length)
		elog(ERROR, "invalid memory reference");

leave_process:

	/* 
	 * we should to release a allocated resources. The most important
	 * is correct finishing a nested transactions.
	 */
	while (ROP >= 0)
	{
		if (rollback_nested_transactions)
			RollbackAndReleaseCurrentSubTransaction();
		else
			ReleaseCurrentSubTransaction();

		CurrentResourceOwner = ResourceOwnerStack[ROP--];
	}

	fcinfo->isnull = isnull;
	MemoryContextDelete(exec_ctx);

	return (Datum) result;
}

/*
 * Set a ErrorCallback and exeute a module
 */
Datum
plpsm_func_execute(Plpsm_module *mod, FunctionCallInfo fcinfo)
{
	ErrorContextCallback plerrcontext;
	DebugInfoData	dinfo;
	Datum	result;

	plerrcontext.callback = plpsm_exec_error_callback;
	plerrcontext.arg = &dinfo;
	plerrcontext.previous = error_context_stack;
	error_context_stack = &plerrcontext;

	result = execute_module(mod, fcinfo, &dinfo);

	error_context_stack = plerrcontext.previous;

	return result;
}

/*
 * collect a variables info
 */
static void
collect_vars_info(StringInfo ds, Plpsm_object *scope, FmgrInfo *flinfo, Datum *values, char *nulls)
{
	Plpsm_object *iterator;
	StringInfoData		lds;
	bool append = false;

	if (scope == NULL)
		return;

	initStringInfo(&lds);

	iterator = scope->inner;

	appendStringInfo(&lds, "\n  ==== %s: frame ====\n", scope->name != NULL ? scope->name : "unnamed");

	while (iterator != NULL)
	{
		if (iterator->typ == PLPSM_STMT_DECLARE_VARIABLE)
		{
			char *value;

			if (nulls[iterator->offset] != 'n')
				value = OutputFunctionCall(&flinfo[iterator->offset], values[iterator->offset]);
			else
				value = "NULL";

			if (iterator->typ == PLPSM_STMT_DECLARE_VARIABLE)
				appendStringInfo(&lds, "  %3d\t%s = %s\n", iterator->offset, 
												iterator->name,
												value);
			append = true;
		}
		else if (iterator->typ == PLPSM_STMT_DECLARE_CURSOR)
		{
				appendStringInfo(&lds, "  %3d\t%s cursor\n", iterator->offset, 
												iterator->name);
				append = true;
		}
		iterator = iterator->next;
	}

	if (append)
		appendStringInfoString(ds, lds.data);

	collect_vars_info(ds, scope->outer, flinfo, values, nulls);
}

/*
 * set a context to let us supply a call-stack traceback
 */
static void
plpsm_exec_error_callback(void *arg)
{
	DebugInfo dinfo = (DebugInfo) arg;
	Plpsm_pcode *pcode;
	StringInfoData	ds;

	if (dinfo->is_signal)
		return;

	initStringInfo(&ds);

	pcode = &dinfo->module->code->code[*(dinfo->PC)];

	if (pcode->lineno != -1 && *dinfo->src != NULL)
	{
		int	lineno = pcode->lineno;
		char	*src = *dinfo->src;
		int	curline = 0;
		
		int maxl = 0;

		appendStringInfo(&ds, "PLPSM function \"%s\"  Oid %d line %d\n\n", dinfo->module->code->name, 
											    dinfo->module->oid,
											    pcode->lineno); 

		while (*src != '\0')
		{
			if (maxl++ == 100)
				break;
			if (curline < lineno - 3)
			{
				/* skip line */
				while (*src != '\0')
					if (*src++ == '\n')
						break;
			}
			else if (curline < lineno + 2)
			{
				appendStringInfo(&ds, "%4d\t", curline + 1);
				while (*src != '\0')
				{
					appendStringInfoChar(&ds, *src);
					if (*src++ == '\n')
						break;
				}
			}
			else
				break;
			curline++;
		}
	}
	else if (pcode->lineno != -1)
	{
		appendStringInfo(&ds, "PLPSM function \"%s\"  Oid %d line %d", dinfo->module->code->name, 
										    dinfo->module->oid,
										    pcode->lineno); 
	}
	else
		appendStringInfo(&ds, "PLPSM function \"%s\"  Oid %d", dinfo->module->code->name, dinfo->module->oid); 

	if (pcode->cframe != NULL)
	{
		collect_vars_info(&ds, pcode->cframe, dinfo->out_funcs, dinfo->values, dinfo->nulls);
	}

	errcontext("%s", ds.data);
	pfree(ds.data);
}
