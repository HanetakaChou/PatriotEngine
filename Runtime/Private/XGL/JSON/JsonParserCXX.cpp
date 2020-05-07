#include "JsonFrontend.h"
#include "JsonScannerParser.h"
#include <exception>
extern "C" int json_lllex(struct llscan_t *scanner, union YYSTYPE *lvalp, struct YYLTYPE *llocp);

extern "C" int json_yylex(union YYSTYPE *lvalp, struct YYLTYPE *llocp, JsonFrontend *pUserData, struct llscan_t *pScanner)
{
    try
    {
        int yytoken = json_lllex(pScanner, lvalp, llocp);
        return yytoken;
    }
    catch (const std::exception &)
    {
        return YYTOKEN_PSEUDO_LEX_ERROR;
    }
}

//https://www.gnu.org/software/bison/manual/bison.html#Location-Type
struct YYLTYPE
{
    int first_line;
    int first_column;
    int last_line;
    int last_column;
};
typedef struct YYLTYPE YYLTYPE;

extern "C" void json_yyerror(YYLTYPE *llocp, JsonFrontend *pUserData, struct llscan_t *, const char *s)
{
    pUserData->Callback_Error(llocp->first_line, llocp->first_column, s);
}

extern "C" void *json_yyalloc(size_t size, JsonFrontend *pUserData)
{
    return pUserData->Callback_Malloc(size);
}

extern "C" void *json_yyrealloc(void *ptr, size_t size, JsonFrontend *pUserData)
{
    return pUserData->Callback_Realloc(ptr, size);
}

extern "C" void json_yyfree(void *ptr, JsonFrontend *pUserData)
{
    return pUserData->Callback_Free(ptr);
}