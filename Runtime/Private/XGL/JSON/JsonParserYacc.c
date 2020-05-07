#include <stddef.h>

#include "JsonParser.h"

extern void *json_yyalloc(size_t size, void *pUserData);
extern void *json_yyrealloc(void *ptr, size_t size, void *pUserData);
extern void json_yyfree(void *ptr, void *pUserData);

#define YYMALLOC(size) (json_yyalloc((size), pUserData))
#define YYREALLOC(ptr, size) (json_yyrealloc((ptr), (size), pUserData))
#define YYFREE(ptr) (json_yyfree((ptr), pUserData))

//https://www.gnu.org/software/bison/manual/html_node/Table-of-Symbols.html
//#define YYSTACK_USE_ALLOCA 1
#include "JsonParserYacc.inl"

//https://www.gnu.org/software/bison/manual/html_node/Union-Decl.html#Union-Decl
//https://www.gnu.org/software/bison/manual/html_node/Location-Type.html

int YYTOKEN_STRING = STRING;
int YYTOKEN_LEFTBRACE = LEFTBRACE;
int YYTOKEN_RIGHTBRACE = RIGHTBRACE;
int YYTOKEN_COLON = COLON;
int YYTOKEN_LEFTBRACKET = LEFTBRACKET;
int YYTOKEN_RIGHTBRACKET = RIGHTBRACKET;
int YYTOKEN_COMMA = COMMA;
int YYTOKEN_NUMBER = NUMBER;
int YYTOKEN_TRUE = TRUE;
int YYTOKEN_FALSE = FALSE;
int YYTOKEN_NULL = NULL;
int YYTOKEN_PSEUDO_LEX_ERROR = PSEUDO_LEX_ERROR;
int YYTOKEN_EOF = YYEOF;

static void _static_assert_json_yy_parser_(void)
{
    char _static_assert_yyeof_[((YYEOF == 0) ? 1 : -1)];
    ((void)_static_assert_yyeof_);
}