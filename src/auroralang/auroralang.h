#ifndef AURORALANG_H
#define AURORALANG_H

#include "../kernel/kernel.h"

/* ═══════════════════════════════════════════════════════════════════
   AURORALANG  –  Token types
   ═══════════════════════════════════════════════════════════════════ */

typedef enum {
    /* Literals */
    TOK_INT, TOK_FLOAT, TOK_STRING, TOK_BOOL, TOK_NULL,
    /* Identifiers / keywords */
    TOK_IDENT,
    TOK_LET, TOK_CONST, TOK_FUNCTION, TOK_RETURN,
    TOK_IF, TOK_ELSE, TOK_LOOP, TOK_FROM, TOK_TO, TOK_WHILE,
    TOK_BREAK, TOK_CONTINUE,
    TOK_PRINT, TOK_INPUT,
    TOK_TRY, TOK_CATCH, TOK_THROW,
    TOK_PARALLEL, TOK_TASK,
    TOK_USE,
    TOK_SYSTEM,
    TOK_UI, TOK_WINDOW, TOK_BUTTON, TOK_LABEL,
    TOK_SERVER, TOK_ROUTE, TOK_START,
    TOK_CREATE, TOK_WRITE, TOK_READ, TOK_FILE,
    TOK_REWIND, TOK_STEP, TOK_REPLAY,
    TOK_STR, TOK_INT_CAST, TOK_FLOAT_CAST,
    /* Operators */
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH, TOK_PERCENT,
    TOK_EQ, TOK_NEQ, TOK_LT, TOK_GT, TOK_LTE, TOK_GTE,
    TOK_AND, TOK_OR, TOK_NOT,
    TOK_ASSIGN,
    TOK_PLUS_ASSIGN, TOK_MINUS_ASSIGN,
    /* Delimiters */
    TOK_LPAREN, TOK_RPAREN,
    TOK_LBRACE, TOK_RBRACE,
    TOK_LBRACKET, TOK_RBRACKET,
    TOK_COMMA, TOK_DOT, TOK_COLON, TOK_SEMICOLON,
    /* Special */
    TOK_NEWLINE, TOK_EOF, TOK_ERROR
} token_type_t;

typedef struct {
    token_type_t type;
    char         lexeme[128];
    int          line;
    union {
        int    ival;
        double fval;
    };
} token_t;

/* ── Lexer ─────────────────────────────────────────────────────────── */
#define MAX_TOKENS 2048

typedef struct {
    const char *src;
    int         pos;
    int         line;
    token_t     tokens[MAX_TOKENS];
    int         tok_count;
    int         tok_pos;
} lexer_t;

void  lexer_init(lexer_t *l, const char *src);
void  lexer_tokenize(lexer_t *l);
token_t lexer_peek(lexer_t *l);
token_t lexer_next(lexer_t *l);
bool  lexer_match(lexer_t *l, token_type_t t);

/* ── AST ───────────────────────────────────────────────────────────── */
typedef enum {
    NODE_PROGRAM,
    NODE_LET, NODE_CONST_DECL,
    NODE_ASSIGN,
    NODE_FUNCTION, NODE_CALL, NODE_RETURN,
    NODE_IF, NODE_WHILE, NODE_LOOP,
    NODE_BREAK, NODE_CONTINUE,
    NODE_PRINT, NODE_INPUT,
    NODE_BINOP, NODE_UNOP,
    NODE_INT_LIT, NODE_FLOAT_LIT, NODE_STR_LIT, NODE_BOOL_LIT, NODE_NULL_LIT,
    NODE_IDENT,
    NODE_BLOCK,
    NODE_TRY_CATCH,
    NODE_PARALLEL,
    NODE_USE,
    NODE_SYSTEM_CALL,
    NODE_UI_WINDOW, NODE_UI_BUTTON, NODE_UI_LABEL,
    NODE_SERVER, NODE_ROUTE,
    NODE_FILE_CREATE, NODE_FILE_WRITE, NODE_FILE_READ,
    NODE_REWIND, NODE_STEP,
    NODE_CAST,
    NODE_INDEX,
} node_type_t;

#define MAX_CHILDREN 16
#define MAX_NODES    1024

typedef struct ast_node {
    node_type_t type;
    char        sval[128];
    int         ival;
    double      fval;
    int         children[MAX_CHILDREN];
    int         child_count;
    int         line;
} ast_node_t;

typedef struct {
    ast_node_t nodes[MAX_NODES];
    int        count;
} ast_t;

int  ast_alloc(ast_t *a, node_type_t t);
void ast_add_child(ast_t *a, int parent, int child);

/* ── Parser ────────────────────────────────────────────────────────── */
typedef struct {
    lexer_t *lex;
    ast_t   *ast;
    char     error[256];
    bool     had_error;
} parser_t;

int  parse_program(parser_t *p);

/* ── Runtime value ─────────────────────────────────────────────────── */
typedef enum { VAL_INT, VAL_FLOAT, VAL_STRING, VAL_BOOL, VAL_NULL, VAL_ERROR } val_type_t;

typedef struct {
    val_type_t type;
    int        ival;
    double     fval;
    char       sval[256];
    bool       bval;
} value_t;

/* ── Environment (variable scope) ─────────────────────────────────── */
#define ENV_MAX_VARS  128
#define ENV_MAX_DEPTH 32

typedef struct {
    char    name[64];
    value_t val;
    bool    is_const;
} env_var_t;

typedef struct env_frame {
    env_var_t vars[ENV_MAX_VARS];
    int       var_count;
} env_frame_t;

typedef struct {
    env_frame_t frames[ENV_MAX_DEPTH];
    int         depth;
} env_t;

void  env_push(env_t *e);
void  env_pop(env_t *e);
bool  env_set(env_t *e, const char *name, value_t val, bool is_const);
bool  env_get(env_t *e, const char *name, value_t *out);
bool  env_assign(env_t *e, const char *name, value_t val);

/* ── Function table ────────────────────────────────────────────────── */
#define MAX_FUNCS 64

typedef struct {
    char name[64];
    int  params[8];   /* AST node indices of param idents */
    int  param_count;
    int  body;        /* AST node index of body block */
    bool used;
} func_def_t;

/* ── Interpreter ───────────────────────────────────────────────────── */
typedef struct {
    ast_t      *ast;
    env_t       env;
    func_def_t  funcs[MAX_FUNCS];
    int         func_count;
    bool        returning;
    value_t     return_val;
    bool        breaking;
    bool        continuing;
    /* Time-travel debug */
    char        history[64][256];
    int         hist_count;
    int         hist_cursor;
    bool        tracing;
} interp_t;

value_t interp_exec(interp_t *it, int node_idx);
void    interp_init(interp_t *it, ast_t *ast);

/* ── Public API ────────────────────────────────────────────────────── */
void aurora_runtime_init(void);
int  aurora_run_string(const char *code);
int  aurora_run_file(const char *path);
void aurora_repl(void);

#endif /* AURORALANG_H */
