#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#if defined(_MSC_VER)
extern __declspec(noreturn) void json_ll_fatal_error(int line, int column, char const *msg, void *pUserData);
#elif defined(__GNUC__)
extern __attribute__((__noreturn__)) void json_ll_fatal_error(int line, int column, char const *msg, void *pUserData);
#else
#error Unknown Compiler //未知的编译器
#endif

#define YY_FATAL_ERROR(msg)                                           \
    {                                                                 \
        struct yyguts_t *yyg = (struct yyguts_t *)yyscanner;          \
        (json_ll_fatal_error((yylineno), (yycolumn), msg, (yyextra))); \
    }

extern ptrdiff_t json_ll_stream_read(void *pUserData, void *pUserStream, void *buf, size_t size);

#define YY_INPUT(buf, result, max_size)                                              \
    {                                                                                \
        (*(&result)) = json_ll_stream_read((yyextra), ((void *)yyin), buf, max_size); \
        if (result == -1)                                                            \
        {                                                                            \
            YY_FATAL_ERROR("input in flex scanner failed");                          \
        }                                                                            \
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

#define YY_DECL int json_lllex(yyscan_t yyscanner, union YYSTYPE *lvalp, struct YYLTYPE *llocp)

#define YY_USER_ACTION                                \
    {                                                 \
        llocp->first_line = (yylineno);               \
        llocp->last_line = (yylineno);                \
        llocp->first_column = (yycolumn);             \
        llocp->last_column = ((yycolumn) + (yyleng)); \
        (yycolumn) += (yyleng);                       \
    }

#include "JsonParser.h"

#include "JsonScannerLex.inl"

static void _static_assert_json_ll_lexer_(void)
{
    char _static_assert_yy_size_t_[((sizeof(yy_size_t) == sizeof(size_t)) ? 1 : -1)];
    char _static_assert_yyscan_t_[((sizeof(yyscan_t) == sizeof(void *)) ? 1 : -1)];
    char _static_assert_YY_NULL_[((YY_NULL == 0) ? 1 : -1)];
    ((void)_static_assert_yy_size_t_);
    ((void)_static_assert_yyscan_t_);
    ((void)_static_assert_YY_NULL_);
}