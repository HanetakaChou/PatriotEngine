/*
 * Copyright (C) YuqiaoZhang(HanetakaYuminaga)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

// Shift/Reduce Conflicts
// https://www.gnu.org/software/bison/manual/html_node/Shift_002fReduce.html

// How to debug

// example
//
// %token NUMBER
// %token PLUS
// %type expession
// %start expession
// %%
// expession: expession PLUS expession
// expession: NUMBER
// %%

// bison
// add "-r all" option and generate the "*.output" file

// when there is a "%type"(nonterminal) right after the "."
// we should consider all the results which can be deduced
// and we should focus whether the shift and reduce can be distinguished by the first "%token"(the LA(1))
// the shift and reduce can exist at the same time as long as they can be distinguished by the first "%token"(the LA(1))

// search "State * conflicts: * shift/reduce" in "*.output" file
// in this case, we have "State 5 conflicts: 1 shift/reduce"

// search "State 5", we have "State 4" --expession--> "State 5"
// search "State 4", we have "State 2" --PLUS--> "State 4"
// ...
// finally, we have the chain "State 2" --PLUS--> "State 4" --expession--> "State 5"

// It should be noted that the "%type"(nonterminal) is right after "." in "State 2" and thus the deduction should be applied
//
// State 4

//    1 expession: . expession PLUS expession // which means expession PLUS (. expession PLUS expession) // deduction appiled
//    1          | expession PLUS . expession
//    2          | . NUMBER

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "pt_gfx_mesh_base_gltf_lex_yacc.h"

// For YACC
union YYSTYPE
{
    int m_token_numberint;
    int m_token_numberfloat;

    int m_scene_index;
    int m_node_index;
    int m_buffer_index;
    int m_bufferview_index;
    int m_accessor_index;

    struct temp_string_version_t *m_temp_string_version;
    struct temp_int_array_version_t *m_temp_int_array_version;
    struct temp_float_array_version_t *m_temp_float_array_version;

    float m_mat4x4[16];
    float m_vec4[4];
    float m_vec3[3];
    bool m_boolean;
};

#define YYMALLOC(size) (gltf_lex_yacc_alloc_callback((size), (user_defined)))
#define YYREALLOC(ptr, size) (gltf_lex_yacc_realloc_callback((ptr), (size), (user_defined)))
#define YYFREE(ptr) (gltf_lex_yacc_free_callback((ptr), (user_defined)))

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
    return gltf_lex_yacc_alloc_callback(size, (yyget_extra(yyscanner)));
}

void *gltf_lex_yyrealloc(void *ptr, size_t size, yyscan_t yyscanner)
{
    return gltf_lex_yacc_realloc_callback(ptr, size, (yyget_extra(yyscanner)));
}

void gltf_lex_yyfree(void *ptr, yyscan_t yyscanner)
{
    return gltf_lex_yacc_free_callback(ptr, (yyget_extra(yyscanner)));
}

//https://westes.github.io/flex/manual/Generated-Scanner.html#Generated-Scanner
int yywrap(yyscan_t yyscanner)
{
    return 1;
}