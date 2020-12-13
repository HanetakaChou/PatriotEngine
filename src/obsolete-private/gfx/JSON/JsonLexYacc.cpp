#include "../PT_XGL_StreamReader.h"

#include "JsonLexYacc.h"

#include <exception>

extern "C" int json_lllex(struct llscan_t *scanner, union YYSTYPE *lvalp, struct YYLTYPE *llocp);

extern "C" int json_yylex(union YYSTYPE *lvalp, struct YYLTYPE *llocp, class JsonParser *pUserData, struct llscan_t *pScanner)
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

#if defined(_MSC_VER)
static inline __declspec(noreturn) void json_ll_fatal_error_cxx(int line, int column, char const *msg, class JsonParser *pUserData);
extern "C" void json_ll_fatal_error(int line, int column, char const *msg, class JsonParser *pUserData)
{
    return json_ll_fatal_error_cxx(line, column, msg, pUserData);
}
static inline __declspec(noreturn) void json_ll_fatal_error_cxx(int line, int column, char const *msg, class JsonParser *pUserData)
#elif defined(__GNUC__)
extern "C" __attribute__((__noreturn__)) void json_ll_fatal_error(int line, int column, char const *msg, class JsonParser *pUserData)
#else
#error Unknown Compiler //未知的编译器
#endif
{
    JsonParser_Error(pUserData, line, column, msg);

    //RaiseException
    throw std::exception();
}
