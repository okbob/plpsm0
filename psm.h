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
	PLPSM_STMT_COMPOUND_STATEMENT,
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
	PLPSM_STMT_IF
} Plpsm_stmt_type;

typedef struct Plpsm_stmt
{
	Plpsm_stmt_type		typ;
	int		location;
	int		lno;
	char *name;
	List *target;
	List 	*compound_target;
	void			*frame;
	void			*data;
	int	option;
	char	*query;
	char	*expr;
	char			*debug;
	struct Plpsm_stmt *next;
	struct Plpsm_stmt *last;
	struct Plpsm_stmt *inner_left;
	struct Plpsm_stmt *inner_right;
} Plpsm_stmt;

typedef enum
{
	PCODE_NOOP = 0,
	PCODE_JMP_FALSE_UNKNOWN,
	PCODE_JMP,
	PCODE_JMP_NOT_FOUND,
	PCODE_CALL,
	PCODE_RETURN,
	PCODE_EXEC_EXPR,
	PCODE_PRINT,
	PCODE_DEBUG,
	PCODE_DONE,
	PCODE_IF_NOTEXIST_PREPARE,
	PCODE_EXECUTE
} Plpsm_pcode_type;

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
			Oid	oid;
		} expr;
		struct
		{
			char *expr;
			char *name;
		} prep;
	};
} Plpsm_pcode;

typedef struct
{
	char *fn_name;
	int max_length;
	int	length;
	int		prep_statements_nums;	/* number of prepared_statements */
	Plpsm_pcode code[1];
} Plpsm_pcode_module;

extern Plpsm_stmt *plpsm_parser_result;

extern Datum psm0_call_handler(PG_FUNCTION_ARGS);
extern Datum psm0_inline_handler(PG_FUNCTION_ARGS);
extern Datum psm0_validator(PG_FUNCTION_ARGS);

extern int		plpsm_yyparse(void);
extern void plpsm_yyerror(const char *message);
extern int plpsm_yylex(void);
extern void plpsm_scanner_init(const char *str);
extern void plpsm_scanner_finish(void); 
extern void plpsm_push_back_token(int token);
extern void plpsm_append_source_text(StringInfo buf, int startlocation, int endlocation);

extern void plpsm_compile(Oid funcOid, bool forValidator);

#endif