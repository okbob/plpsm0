#ifndef PSM_H
#define PSM_H

#include "postgres.h"
#include "nodes/pg_list.h"

#include "fmgr.h"
#include "lib/stringinfo.h"

/* Struct types used during parsing */

typedef struct
{
	char	*ident;				/* palloc'd converted identifier */
	bool		quoted;			/* Was it double quoted? */
} PLword;

typedef struct
{
	List	*idents;			/* composite identifiers (list of Strings) */
} PLcword;

typedef enum
{
	PLPSM_HANDLER_CONTINUE,
	PLPSM_HANDLER_EXIT,
	PLPSM_HANDLER_UNDO
} Plpsm_handler_type;

typedef enum
{
	PLPSM_CURSOR_NOSCROLL = 0,
	PLPSM_CURSOR_SCROLL = 1
} Plpsm_cursor_type;

typedef enum
{
	PLPSM_VARIABLE = 0,
	PLPSM_REFERENCE = 1
} Plpsm_usage_variable_type;

typedef enum
{
	PLPSM_NONE,
	PLPSM_STMT_COMPOUND_STATEMENT,
	PLPSM_STMT_SCHEMA,				/* it's used as explicit schema for implicit variables of FOR statement */
	PLPSM_STMT_DECLARE_VARIABLE,
	PLPSM_STMT_DECLARE_CONDITION,
	PLPSM_STMT_DECLARE_CURSOR,
	PLPSM_STMT_DECLARE_HANDLER,
	PLPSM_STMT_EXECUTE,
	PLPSM_STMT_EXECUTE_IMMEDIATE,
	PLPSM_STMT_SET,
	PLPSM_STMT_PREPARE,
	PLPSM_STMT_PRINT,
	PLPSM_STMT_LOOP,
	PLPSM_STMT_WHILE,
	PLPSM_STMT_REPEAT_UNTIL,
	PLPSM_STMT_ITERATE,
	PLPSM_STMT_LEAVE,
	PLPSM_STMT_RETURN,
	PLPSM_STMT_UNKNOWN,
	PLPSM_STMT_OPEN,
	PLPSM_STMT_CLOSE,
	PLPSM_STMT_FETCH,
	PLPSM_STMT_FOR,
	PLPSM_STMT_IF,
	PLPSM_STMT_CASE,
	PLPSM_STMT_SQL,
	PLPSM_STMT_SELECT_INTO
} Plpsm_stmt_type;

typedef enum
{
	PLPSM_ESQL_EXPR,
	PLPSM_ESQL_QUERY,
	PLPSM_ESQL_DATATYPE
} Plpsm_esql_type;

typedef struct 
{							/* used for embeded SQL expressions and SQL queries */
	Plpsm_esql_type		typ;
	int		lineno;
	int		location;
	char	*sqlstr;
} Plpsm_ESQL;

typedef struct
{							/* used for variable identifiers */
	int		lineno;
	int		loc;
	List	*qualId;
} Plpsm_positioned_qualid;

typedef struct
{							/* Holds a basic info about Datum value */
	Oid		typoid;
	int32		typmod;
	char		*typname;
	bool	typbyval;
	int16	typlen;
} Plpsm_type_descriptor;

typedef struct Plpsm_stmt
{
	Plpsm_stmt_type		typ;
	int		location;
	int		lineno;
	char *name;
	List *target;
	List 	*compound_target;
	union
	{
		void *data;
		struct
		{
			Oid	typoid;
			int16	typmod;
			char	*typname;
			bool	typbyval;
			int16	typlen;
		} datum;
		struct
		{
			char *loopvar_name;
			char *cursor_name;
		} stmtfor;
		Plpsm_ESQL *from_clause;
	};
	void			*data;
	int	option;
	
	Plpsm_ESQL	*esql;				/* used when we working with single expr or query */
	List		*esql_list;			/* used when we working with list of expressions */
	
	//char	*query;
	//char	*expr;
	union
	{
		//List		*expr_list;
		List		*var_list;
	};
	struct Plpsm_stmt *next;
	struct Plpsm_stmt *last;
	struct Plpsm_stmt *inner_left;
	struct Plpsm_stmt *inner_right;
} Plpsm_stmt;

typedef struct Plpsm_object
{
	Plpsm_stmt_type		typ;
	char	*name;
	Plpsm_stmt	*stmt;
	union
	{
		struct
		{
			int	entry_addr;
			bool	has_release_call;
			List	*release_calls;
			List	*leave_jmps;
		} calls;
		struct
		{
			int	data_addr;
			int16	offset;
			bool	is_dynamic;
			bool	is_for_stmt_cursor;
			char *prepname;
		} cursor;
		int16		offset;
	};
	struct Plpsm_object *next;
	struct Plpsm_object *last;
	struct Plpsm_object *inner;
	struct Plpsm_object *outer;
} Plpsm_object;

typedef enum
{
	PCODE_JMP_FALSE_UNKNOWN,
	PCODE_JMP,
	PCODE_JMP_NOT_FOUND,
	PCODE_CALL,
	PCODE_CALL_NOT_FOUND,
	PCODE_RET_SUBR,
	PCODE_RETURN,
	PCODE_RETURN_VOID,
	PCODE_RETURN_NULL,
	PCODE_EXEC_EXPR,
	PCODE_EXEC_QUERY,
	PCODE_PRINT,
	PCODE_DONE,
	PCODE_EXECUTE,
	PCODE_SET_NULL,
	PCODE_SAVETO,
	PCODE_SAVETO_FIELD,
	PCODE_CURSOR_FETCH,
	PCODE_UPDATE_FIELD,
	PCODE_COPY_PARAM,
	PCODE_SIGNAL_NODATA,
	PCODE_DATA_QUERY,
	PCODE_CURSOR_OPEN,
	PCODE_CURSOR_OPEN_DYNAMIC,
	PCODE_CURSOR_CLOSE,
	PCODE_CURSOR_RELEASE,
	PCODE_SQLSTATE_REFRESH,
	PCODE_SQLCODE_REFRESH,
	PCODE_STRBUILDER,
	PCODE_CHECK_DATA,
	PCODE_EXECUTE_IMMEDIATE,
	PCODE_PREPARE,
	PCODE_PARAMBUILDER
} Plpsm_pcode_type;

typedef enum
{
	PLPSM_STRBUILDER_INIT,
	PLPSM_STRBUILDER_APPEND_CHAR,
	PLPSM_STRBUILDER_APPEND_RESULT,
	PLPSM_STRBUILDER_APPEND_FIELD,
	PLPSM_STRBUILDER_PRINT_FREE,
	PLPSM_STRBUILDER_FREE
} Plpsm_strbuilder_op_type;

typedef enum
{
	PLPSM_PARAMBUILDER_INIT,
	PLPSM_PARAMBUILDER_APPEND,
	PLPSM_PARAMBUILDER_FREE
} Plpsm_parambuilder_op_type;

typedef struct
{
	char		typ;
	union
	{
		int addr;
		char *str;
		struct 
		{
			char *expr;
			int	nparams;
			Oid	*typoids;
			int	data;
			bool	is_multicol;
		} expr;
		struct
		{
			char *name;
			int	data;
		} prepare;
		struct
		{
			int16	typlen;
			bool	typbyval;
			int offset;
		} target;
		struct
		{
			int16	typlen;
			bool	typbyval;
			int	src;
			int	dest;
		} copyto;
		struct
		{
			int	addr;
			int	offset;
			int	params;
			char *name;
			char *prepname;
		} cursor;
		struct 
		{
			Oid	typoid;
			int16	typmod;
			int offset;
			int	fno;				/* number of attrib in composite */
			int	fnumber;			/* number of field in result */
		} update_field;
		struct
		{
			int16	typlen;
			bool	typbyval;
			Oid	typoid;
			int16	typmod;
			int	offset;
			int	fnumber;
			int	data;
		} saveto_field;
		struct
		{
			int	offset;
			int	count;
			int	nvars;
			char *name;
		} fetch;
		struct
		{
			int	data;
			Plpsm_strbuilder_op_type op;
			union
			{
				char	chr;
				int16	fnumber;
				char	*str;
			};
		} strbuilder;
		struct
		{
			int	params;
			int	sqlstr;
			char *name;
		} execute;
		struct
		{
			int	data;
			Plpsm_parambuilder_op_type op;
			union
			{
				int	nargs;
				int	fnumber;
			};
		} parambuilder;
		int	size;
		int16	ncolumns;
	};
} Plpsm_pcode;

typedef struct
{
	int mlength;
	int	length;
	int		ndatums;		/* max number of used Datums */
	int		ndata;			/* number of data address used for module's instance */
	char *name;
	Plpsm_pcode code[1];
} Plpsm_pcode_module;

typedef struct
{
	int		location;
	int		leaderlen;
} Plpsm_sql_error_callback_arg;

extern Plpsm_stmt *plpsm_parser_tree;
extern Plpsm_object *plpsm_parser_objects;

extern bool plpsm_debug_parser;
extern bool plpsm_debug_compiler;

extern Datum psm0_call_handler(PG_FUNCTION_ARGS);
extern Datum psm0_inline_handler(PG_FUNCTION_ARGS);
extern Datum psm0_validator(PG_FUNCTION_ARGS);

extern int		plpsm_yyparse(void);
extern void plpsm_yyerror(const char *message);
extern int plpsm_yylex(void);

extern int plpsm_latest_lineno(void);
extern int plpsm_location_to_lineno(int location);
extern int plpsm_scanner_errposition(int location);

extern void plpsm_scanner_init(const char *str);
extern void plpsm_scanner_finish(void); 
extern void plpsm_push_back_token(int token);
extern void plpsm_append_source_text(StringInfo buf, int startlocation, int endlocation);

extern Plpsm_pcode_module *plpsm_compile(Oid funcOid, bool forValidator);
extern Datum plpsm_func_execute(Plpsm_pcode_module *module, FunctionCallInfo fcinfo);

extern Plpsm_stmt *plpsm_new_stmt(Plpsm_stmt_type typ, int location);

extern void plpsm_sql_error_callback(void *arg);

#endif