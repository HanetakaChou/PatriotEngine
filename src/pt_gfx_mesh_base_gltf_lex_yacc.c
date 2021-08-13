#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "pt_gfx_mesh_base_gltf_lex_yacc.h"

// For YACC
#define YY_TOKEN_STRING_BUFFER_SIZE 4096

union YYSTYPE
{
    char m_token_string[YY_TOKEN_STRING_BUFFER_SIZE];

    void *_stdstring;

    long _valueint;

    float _valuefloat;

    void *_jsonvalue;

    struct
    {
        void *_stdstring;
        void *_jsonvalue;
    } _jsonmember;
};

#define YYMALLOC(size) (gltf_alloc_callback((size), (user_defined)))
#define YYREALLOC(ptr, size) (gltf_realloc_callback((ptr), (size), (user_defined)))
#define YYFREE(ptr) (gltf_free_callback((ptr), (user_defined)))

//https://www.gnu.org/software/bison/manual/html_node/Table-of-Symbols.html
//#define YYSTACK_USE_ALLOCA 1

// provide "enum yytokentype" for Lex
// provide "struct YYLTYPE" for Lex
#include "pt_gfx_mesh_base_gltf_yacc.inl"

//https://www.gnu.org/software/bison/manual/html_node/Union-Decl.html#Union-Decl
//https://www.gnu.org/software/bison/manual/html_node/Location-Type.html
void yyerror(YYLTYPE *llocp, void *user_defined, void *yyscanner, const char *msg)
{
    return gltf_yacc_error_callback(llocp->first_line, llocp->first_column, msg, user_defined);
}

// For Lex
#define yyalloc gltf_lex_yyalloc
#define yyrealloc gltf_lex_yyrealloc
#define yyfree gltf_lex_yyfree

#define YY_FATAL_ERROR(msg)                                                    \
    {                                                                          \
        struct yyguts_t *yyg = (struct yyguts_t *)yyscanner;                   \
        gltf_lex_fatal_error_callback((yylineno), (yycolumn), msg, (yyextra)); \
    }

#define YY_INPUT(buf, result, max_size)                                                   \
    {                                                                                     \
        (*(&result)) = gltf_lex_input_callback(((void *)yyin), buf, max_size, (yyextra)); \
        if (result == -1)                                                                 \
        {                                                                                 \
            YY_FATAL_ERROR("input in flex scanner failed");                               \
        }                                                                                 \
    }

#define YY_DECL int yylex(union YYSTYPE *lvalp, YYLTYPE *llocp, void *user_defined, void *yyscanner)

//https://www.gnu.org/software/bison/manual/bison.html#Location-Type
#define YY_USER_ACTION                                \
    {                                                 \
        llocp->first_line = (yylineno);               \
        llocp->last_line = (yylineno);                \
        llocp->first_column = (yycolumn);             \
        llocp->last_column = ((yycolumn) + (yyleng)); \
        (yycolumn) += (yyleng);                       \
    }

#include "pt_gfx_mesh_base_gltf_lex.inl"

void *gltf_lex_yyalloc(size_t size, yyscan_t yyscanner)
{
    return gltf_alloc_callback(size, (yyget_extra(yyscanner)));
}

void *gltf_lex_yyrealloc(void *ptr, size_t size, yyscan_t yyscanner)
{
    return gltf_realloc_callback(ptr, size, (yyget_extra(yyscanner)));
}

void gltf_lex_yyfree(void *ptr, yyscan_t yyscanner)
{
    return gltf_free_callback(ptr, (yyget_extra(yyscanner)));
}

//https://westes.github.io/flex/manual/Generated-Scanner.html#Generated-Scanner
int yywrap(yyscan_t yyscanner)
{
    return 1;
}