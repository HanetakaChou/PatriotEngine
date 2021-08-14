/* A Bison parser, made by GNU Bison 3.7.5.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30705

/* Bison version string.  */
#define YYBISON_VERSION "3.7.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         gltf_yyparse
#define yylex           gltf_yylex
#define yyerror         gltf_yyerror
#define yydebug         gltf_yydebug
#define yynerrs         gltf_yynerrs


# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif


/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int gltf_yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    YYTOKEN_ASSET = 258,           /* YYTOKEN_ASSET  */
    YYTOKEN_COPYRIGHT = 259,       /* YYTOKEN_COPYRIGHT  */
    YYTOKEN_GENERATOR = 260,       /* YYTOKEN_GENERATOR  */
    YYTOKEN_VERSION = 261,         /* YYTOKEN_VERSION  */
    YYTOKEN_MINVERSION = 262,      /* YYTOKEN_MINVERSION  */
    YYTOKEN_EXTENSIONS = 263,      /* YYTOKEN_EXTENSIONS  */
    YYTOKEN_EXTRAS = 264,          /* YYTOKEN_EXTRAS  */
    YYTOKEN_SCENE = 265,           /* YYTOKEN_SCENE  */
    YYTOKEN_SCENES = 266,          /* YYTOKEN_SCENES  */
    YYTOKEN_NODES = 267,           /* YYTOKEN_NODES  */
    YYTOKEN_NAME = 268,            /* YYTOKEN_NAME  */
    YYTOKEN_CAMERA = 269,          /* YYTOKEN_CAMERA  */
    YYTOKEN_CHILDREN = 270,        /* YYTOKEN_CHILDREN  */
    YYTOKEN_SKIN = 271,            /* YYTOKEN_SKIN  */
    YYTOKEN_MATRIX = 272,          /* YYTOKEN_MATRIX  */
    YYTOKEN_MESH = 273,            /* YYTOKEN_MESH  */
    YYTOKEN_ROTATION = 274,        /* YYTOKEN_ROTATION  */
    YYTOKEN_SCALE = 275,           /* YYTOKEN_SCALE  */
    YYTOKEN_TRANSLATION = 276,     /* YYTOKEN_TRANSLATION  */
    YYTOKEN_WEIGHTS = 277,         /* YYTOKEN_WEIGHTS  */
    YYTOKEN_BUFFERS = 278,         /* YYTOKEN_BUFFERS  */
    YYTOKEN_BYTELENGTH = 279,      /* YYTOKEN_BYTELENGTH  */
    YYTOKEN_URI = 280,             /* YYTOKEN_URI  */
    YYTOKEN_BUFFERVIEWS = 281,     /* YYTOKEN_BUFFERVIEWS  */
    YYTOKEN_BUFFER = 282,          /* YYTOKEN_BUFFER  */
    YYTOKEN_BYTEOFFSET = 283,      /* YYTOKEN_BYTEOFFSET  */
    YYTOKEN_BYTESTRIDE = 284,      /* YYTOKEN_BYTESTRIDE  */
    YYTOKEN_TARGET = 285,          /* YYTOKEN_TARGET  */
    YYTOKEN_ACCESSORS = 286,       /* YYTOKEN_ACCESSORS  */
    YYTOKEN_BUFFERVIEW = 287,      /* YYTOKEN_BUFFERVIEW  */
    YYTOKEN_COMPONENTTYPE = 288,   /* YYTOKEN_COMPONENTTYPE  */
    YYTOKEN_NORMALIZED = 289,      /* YYTOKEN_NORMALIZED  */
    YYTOKEN_COUNT = 290,           /* YYTOKEN_COUNT  */
    YYTOKEN_TYPE = 291,            /* YYTOKEN_TYPE  */
    YYTOKEN_MAX = 292,             /* YYTOKEN_MAX  */
    YYTOKEN_MIN = 293,             /* YYTOKEN_MIN  */
    YYTOKEN_SCALAR = 294,          /* YYTOKEN_SCALAR  */
    YYTOKEN_VEC2 = 295,            /* YYTOKEN_VEC2  */
    YYTOKEN_VEC3 = 296,            /* YYTOKEN_VEC3  */
    YYTOKEN_VEC4 = 297,            /* YYTOKEN_VEC4  */
    YYTOKEN_MAT2 = 298,            /* YYTOKEN_MAT2  */
    YYTOKEN_MAT3 = 299,            /* YYTOKEN_MAT3  */
    YYTOKEN_MAT4 = 300,            /* YYTOKEN_MAT4  */
    YYTOKEN_TRUE = 301,            /* YYTOKEN_TRUE  */
    YYTOKEN_FALSE = 302,           /* YYTOKEN_FALSE  */
    YYTOKEN_NULL = 303,            /* YYTOKEN_NULL  */
    YYTOKEN_LEFTBRACE = 304,       /* YYTOKEN_LEFTBRACE  */
    YYTOKEN_RIGHTBRACE = 305,      /* YYTOKEN_RIGHTBRACE  */
    YYTOKEN_COLON = 306,           /* YYTOKEN_COLON  */
    YYTOKEN_LEFTBRACKET = 307,     /* YYTOKEN_LEFTBRACKET  */
    YYTOKEN_RIGHTBRACKET = 308,    /* YYTOKEN_RIGHTBRACKET  */
    YYTOKEN_COMMA = 309,           /* YYTOKEN_COMMA  */
    YYTOKEN_STRING = 310,          /* YYTOKEN_STRING  */
    YYTOKEN_NUMBER_INT = 311,      /* YYTOKEN_NUMBER_INT  */
    YYTOKEN_NUMBER_FLOAT = 312,    /* YYTOKEN_NUMBER_FLOAT  */
    PSEUDO_LEX_ERROR = 313         /* PSEUDO_LEX_ERROR  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif



int gltf_yyparse (void *user_defined, void *yyscanner);


/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_YYTOKEN_ASSET = 3,              /* YYTOKEN_ASSET  */
  YYSYMBOL_YYTOKEN_COPYRIGHT = 4,          /* YYTOKEN_COPYRIGHT  */
  YYSYMBOL_YYTOKEN_GENERATOR = 5,          /* YYTOKEN_GENERATOR  */
  YYSYMBOL_YYTOKEN_VERSION = 6,            /* YYTOKEN_VERSION  */
  YYSYMBOL_YYTOKEN_MINVERSION = 7,         /* YYTOKEN_MINVERSION  */
  YYSYMBOL_YYTOKEN_EXTENSIONS = 8,         /* YYTOKEN_EXTENSIONS  */
  YYSYMBOL_YYTOKEN_EXTRAS = 9,             /* YYTOKEN_EXTRAS  */
  YYSYMBOL_YYTOKEN_SCENE = 10,             /* YYTOKEN_SCENE  */
  YYSYMBOL_YYTOKEN_SCENES = 11,            /* YYTOKEN_SCENES  */
  YYSYMBOL_YYTOKEN_NODES = 12,             /* YYTOKEN_NODES  */
  YYSYMBOL_YYTOKEN_NAME = 13,              /* YYTOKEN_NAME  */
  YYSYMBOL_YYTOKEN_CAMERA = 14,            /* YYTOKEN_CAMERA  */
  YYSYMBOL_YYTOKEN_CHILDREN = 15,          /* YYTOKEN_CHILDREN  */
  YYSYMBOL_YYTOKEN_SKIN = 16,              /* YYTOKEN_SKIN  */
  YYSYMBOL_YYTOKEN_MATRIX = 17,            /* YYTOKEN_MATRIX  */
  YYSYMBOL_YYTOKEN_MESH = 18,              /* YYTOKEN_MESH  */
  YYSYMBOL_YYTOKEN_ROTATION = 19,          /* YYTOKEN_ROTATION  */
  YYSYMBOL_YYTOKEN_SCALE = 20,             /* YYTOKEN_SCALE  */
  YYSYMBOL_YYTOKEN_TRANSLATION = 21,       /* YYTOKEN_TRANSLATION  */
  YYSYMBOL_YYTOKEN_WEIGHTS = 22,           /* YYTOKEN_WEIGHTS  */
  YYSYMBOL_YYTOKEN_BUFFERS = 23,           /* YYTOKEN_BUFFERS  */
  YYSYMBOL_YYTOKEN_BYTELENGTH = 24,        /* YYTOKEN_BYTELENGTH  */
  YYSYMBOL_YYTOKEN_URI = 25,               /* YYTOKEN_URI  */
  YYSYMBOL_YYTOKEN_BUFFERVIEWS = 26,       /* YYTOKEN_BUFFERVIEWS  */
  YYSYMBOL_YYTOKEN_BUFFER = 27,            /* YYTOKEN_BUFFER  */
  YYSYMBOL_YYTOKEN_BYTEOFFSET = 28,        /* YYTOKEN_BYTEOFFSET  */
  YYSYMBOL_YYTOKEN_BYTESTRIDE = 29,        /* YYTOKEN_BYTESTRIDE  */
  YYSYMBOL_YYTOKEN_TARGET = 30,            /* YYTOKEN_TARGET  */
  YYSYMBOL_YYTOKEN_ACCESSORS = 31,         /* YYTOKEN_ACCESSORS  */
  YYSYMBOL_YYTOKEN_BUFFERVIEW = 32,        /* YYTOKEN_BUFFERVIEW  */
  YYSYMBOL_YYTOKEN_COMPONENTTYPE = 33,     /* YYTOKEN_COMPONENTTYPE  */
  YYSYMBOL_YYTOKEN_NORMALIZED = 34,        /* YYTOKEN_NORMALIZED  */
  YYSYMBOL_YYTOKEN_COUNT = 35,             /* YYTOKEN_COUNT  */
  YYSYMBOL_YYTOKEN_TYPE = 36,              /* YYTOKEN_TYPE  */
  YYSYMBOL_YYTOKEN_MAX = 37,               /* YYTOKEN_MAX  */
  YYSYMBOL_YYTOKEN_MIN = 38,               /* YYTOKEN_MIN  */
  YYSYMBOL_YYTOKEN_SCALAR = 39,            /* YYTOKEN_SCALAR  */
  YYSYMBOL_YYTOKEN_VEC2 = 40,              /* YYTOKEN_VEC2  */
  YYSYMBOL_YYTOKEN_VEC3 = 41,              /* YYTOKEN_VEC3  */
  YYSYMBOL_YYTOKEN_VEC4 = 42,              /* YYTOKEN_VEC4  */
  YYSYMBOL_YYTOKEN_MAT2 = 43,              /* YYTOKEN_MAT2  */
  YYSYMBOL_YYTOKEN_MAT3 = 44,              /* YYTOKEN_MAT3  */
  YYSYMBOL_YYTOKEN_MAT4 = 45,              /* YYTOKEN_MAT4  */
  YYSYMBOL_YYTOKEN_TRUE = 46,              /* YYTOKEN_TRUE  */
  YYSYMBOL_YYTOKEN_FALSE = 47,             /* YYTOKEN_FALSE  */
  YYSYMBOL_YYTOKEN_NULL = 48,              /* YYTOKEN_NULL  */
  YYSYMBOL_YYTOKEN_LEFTBRACE = 49,         /* YYTOKEN_LEFTBRACE  */
  YYSYMBOL_YYTOKEN_RIGHTBRACE = 50,        /* YYTOKEN_RIGHTBRACE  */
  YYSYMBOL_YYTOKEN_COLON = 51,             /* YYTOKEN_COLON  */
  YYSYMBOL_YYTOKEN_LEFTBRACKET = 52,       /* YYTOKEN_LEFTBRACKET  */
  YYSYMBOL_YYTOKEN_RIGHTBRACKET = 53,      /* YYTOKEN_RIGHTBRACKET  */
  YYSYMBOL_YYTOKEN_COMMA = 54,             /* YYTOKEN_COMMA  */
  YYSYMBOL_YYTOKEN_STRING = 55,            /* YYTOKEN_STRING  */
  YYSYMBOL_YYTOKEN_NUMBER_INT = 56,        /* YYTOKEN_NUMBER_INT  */
  YYSYMBOL_YYTOKEN_NUMBER_FLOAT = 57,      /* YYTOKEN_NUMBER_FLOAT  */
  YYSYMBOL_PSEUDO_LEX_ERROR = 58,          /* PSEUDO_LEX_ERROR  */
  YYSYMBOL_YYACCEPT = 59,                  /* $accept  */
  YYSYMBOL_gltf_object = 60,               /* gltf_object  */
  YYSYMBOL_gltf_members = 61,              /* gltf_members  */
  YYSYMBOL_gltf_member = 62,               /* gltf_member  */
  YYSYMBOL_asset_object = 63,              /* asset_object  */
  YYSYMBOL_asset_properties = 64,          /* asset_properties  */
  YYSYMBOL_scenes_array = 65,              /* scenes_array  */
  YYSYMBOL_scene_objects = 66,             /* scene_objects  */
  YYSYMBOL_scene_object = 67,              /* scene_object  */
  YYSYMBOL_scene_properties = 68,          /* scene_properties  */
  YYSYMBOL_scene_property_nodes = 69,      /* scene_property_nodes  */
  YYSYMBOL_scene_property_nodes_elements = 70, /* scene_property_nodes_elements  */
  YYSYMBOL_nodes_array = 71,               /* nodes_array  */
  YYSYMBOL_node_objects = 72,              /* node_objects  */
  YYSYMBOL_node_object = 73,               /* node_object  */
  YYSYMBOL_node_properties = 74,           /* node_properties  */
  YYSYMBOL_node_property_children = 75,    /* node_property_children  */
  YYSYMBOL_node_property_children_elements = 76, /* node_property_children_elements  */
  YYSYMBOL_node_property_matrix = 77,      /* node_property_matrix  */
  YYSYMBOL_node_property_matrix_element = 78, /* node_property_matrix_element  */
  YYSYMBOL_node_property_rotation = 79,    /* node_property_rotation  */
  YYSYMBOL_node_property_rotation_element = 80, /* node_property_rotation_element  */
  YYSYMBOL_node_property_scale = 81,       /* node_property_scale  */
  YYSYMBOL_node_property_scale_element = 82, /* node_property_scale_element  */
  YYSYMBOL_node_property_translation = 83, /* node_property_translation  */
  YYSYMBOL_node_property_translation_element = 84, /* node_property_translation_element  */
  YYSYMBOL_node_property_weights = 85,     /* node_property_weights  */
  YYSYMBOL_node_property_weights_elements = 86, /* node_property_weights_elements  */
  YYSYMBOL_node_property_weights_element = 87, /* node_property_weights_element  */
  YYSYMBOL_buffers_array = 88,             /* buffers_array  */
  YYSYMBOL_buffer_objects = 89,            /* buffer_objects  */
  YYSYMBOL_buffer_object = 90,             /* buffer_object  */
  YYSYMBOL_buffer_properties = 91,         /* buffer_properties  */
  YYSYMBOL_bufferviews_array = 92,         /* bufferviews_array  */
  YYSYMBOL_bufferview_objects = 93,        /* bufferview_objects  */
  YYSYMBOL_bufferview_object = 94,         /* bufferview_object  */
  YYSYMBOL_bufferview_properties = 95,     /* bufferview_properties  */
  YYSYMBOL_accessors_array = 96,           /* accessors_array  */
  YYSYMBOL_accessor_objects = 97,          /* accessor_objects  */
  YYSYMBOL_accessor_object = 98,           /* accessor_object  */
  YYSYMBOL_accessor_properties = 99,       /* accessor_properties  */
  YYSYMBOL_json_string = 100,              /* json_string  */
  YYSYMBOL_json_boolean = 101,             /* json_boolean  */
  YYSYMBOL_json_value = 102,               /* json_value  */
  YYSYMBOL_json_object = 103,              /* json_object  */
  YYSYMBOL_json_members = 104,             /* json_members  */
  YYSYMBOL_json_member = 105,              /* json_member  */
  YYSYMBOL_json_array = 106,               /* json_array  */
  YYSYMBOL_json_elements = 107,            /* json_elements  */
  YYSYMBOL_json_element = 108              /* json_element  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 13 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"

    int yylex(union YYSTYPE *lvalp, YYLTYPE *llocp, void *user_defined, void *yyscanner);
    void yyerror(YYLTYPE *llocp, void *user_defined, void *yyscanner, const char *msg);

#line 325 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"

#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  12
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   471

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  59
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  50
/* YYNRULES -- Number of rules.  */
#define YYNRULES  184
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  423

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   313


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   138,   138,   142,   149,   157,   161,   165,   169,   173,
     177,   181,   185,   189,   193,   197,   201,   205,   209,   213,
     217,   221,   225,   229,   233,   237,   241,   245,   249,   253,
     258,   262,   266,   272,   277,   281,   285,   289,   293,   298,
     302,   306,   310,   314,   318,   322,   326,   331,   335,   339,
     343,   347,   351,   355,   360,   364,   369,   375,   380,   385,
     390,   395,   400,   405,   411,   416,   420,   424,   428,   433,
     452,   456,   460,   467,   471,   475,   481,   485,   489,   495,
     499,   503,   507,   511,   515,   521,   525,   530,   534,   538,
     542,   546,   550,   554,   558,   563,   568,   573,   577,   581,
     585,   589,   593,   597,   601,   605,   609,   613,   617,   622,
     627,   632,   637,   642,   646,   650,   654,   658,   662,   666,
     670,   674,   678,   682,   686,   690,   694,   698,   702,   706,
     710,   714,   719,   724,   729,   734,   739,   744,   749,   754,
     759,   764,   769,   774,   778,   782,   786,   790,   794,   798,
     802,   806,   810,   814,   818,   822,   826,   830,   834,   838,
     842,   846,   850,   854,   858,   862,   866,   870,   874,   878,
     882,   886,   890,   894,   898,   902,   906,   910,   914,   918,
     922,   926,   930,   934,   938
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "YYTOKEN_ASSET",
  "YYTOKEN_COPYRIGHT", "YYTOKEN_GENERATOR", "YYTOKEN_VERSION",
  "YYTOKEN_MINVERSION", "YYTOKEN_EXTENSIONS", "YYTOKEN_EXTRAS",
  "YYTOKEN_SCENE", "YYTOKEN_SCENES", "YYTOKEN_NODES", "YYTOKEN_NAME",
  "YYTOKEN_CAMERA", "YYTOKEN_CHILDREN", "YYTOKEN_SKIN", "YYTOKEN_MATRIX",
  "YYTOKEN_MESH", "YYTOKEN_ROTATION", "YYTOKEN_SCALE",
  "YYTOKEN_TRANSLATION", "YYTOKEN_WEIGHTS", "YYTOKEN_BUFFERS",
  "YYTOKEN_BYTELENGTH", "YYTOKEN_URI", "YYTOKEN_BUFFERVIEWS",
  "YYTOKEN_BUFFER", "YYTOKEN_BYTEOFFSET", "YYTOKEN_BYTESTRIDE",
  "YYTOKEN_TARGET", "YYTOKEN_ACCESSORS", "YYTOKEN_BUFFERVIEW",
  "YYTOKEN_COMPONENTTYPE", "YYTOKEN_NORMALIZED", "YYTOKEN_COUNT",
  "YYTOKEN_TYPE", "YYTOKEN_MAX", "YYTOKEN_MIN", "YYTOKEN_SCALAR",
  "YYTOKEN_VEC2", "YYTOKEN_VEC3", "YYTOKEN_VEC4", "YYTOKEN_MAT2",
  "YYTOKEN_MAT3", "YYTOKEN_MAT4", "YYTOKEN_TRUE", "YYTOKEN_FALSE",
  "YYTOKEN_NULL", "YYTOKEN_LEFTBRACE", "YYTOKEN_RIGHTBRACE",
  "YYTOKEN_COLON", "YYTOKEN_LEFTBRACKET", "YYTOKEN_RIGHTBRACKET",
  "YYTOKEN_COMMA", "YYTOKEN_STRING", "YYTOKEN_NUMBER_INT",
  "YYTOKEN_NUMBER_FLOAT", "PSEUDO_LEX_ERROR", "$accept", "gltf_object",
  "gltf_members", "gltf_member", "asset_object", "asset_properties",
  "scenes_array", "scene_objects", "scene_object", "scene_properties",
  "scene_property_nodes", "scene_property_nodes_elements", "nodes_array",
  "node_objects", "node_object", "node_properties",
  "node_property_children", "node_property_children_elements",
  "node_property_matrix", "node_property_matrix_element",
  "node_property_rotation", "node_property_rotation_element",
  "node_property_scale", "node_property_scale_element",
  "node_property_translation", "node_property_translation_element",
  "node_property_weights", "node_property_weights_elements",
  "node_property_weights_element", "buffers_array", "buffer_objects",
  "buffer_object", "buffer_properties", "bufferviews_array",
  "bufferview_objects", "bufferview_object", "bufferview_properties",
  "accessors_array", "accessor_objects", "accessor_object",
  "accessor_properties", "json_string", "json_boolean", "json_value",
  "json_object", "json_members", "json_member", "json_array",
  "json_elements", "json_element", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313
};
#endif

#define YYPACT_NINF (-338)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -42,   211,    43,    21,    42,    49,    56,    81,    99,   112,
      14,  -338,  -338,     8,   115,   145,   194,   227,   230,   247,
    -338,   211,   234,  -338,  -338,    17,  -338,    29,  -338,    31,
    -338,    45,  -338,   214,  -338,  -338,   160,   232,   250,   251,
     252,   215,    26,  -338,   -17,  -338,   174,  -338,    87,  -338,
       9,  -338,    95,  -338,   109,  -338,   102,  -338,    12,  -338,
     152,  -338,     6,     6,     6,     6,   161,  -338,   255,   253,
     254,   256,  -338,   216,  -338,   257,   258,   259,   260,   261,
     262,   263,   264,   265,   266,   267,  -338,   218,  -338,   270,
     269,   271,  -338,   221,  -338,   272,   273,   274,   275,   276,
     277,  -338,   223,  -338,   280,   279,   281,   282,   283,  -338,
     224,  -338,   286,  -338,  -338,  -338,  -338,  -338,  -338,  -338,
    -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,
    -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,
    -338,  -338,  -338,     0,   105,  -338,  -338,  -338,  -338,  -338,
    -338,  -338,   285,   287,   288,   289,   290,   161,   248,     6,
    -338,    40,  -338,     6,   291,   292,   293,   294,   295,   296,
     298,   300,   301,  -338,   235,  -338,   299,     6,  -338,   195,
    -338,   302,   303,   304,   305,   306,  -338,   203,  -338,   307,
     308,    -5,    46,  -338,    71,  -338,  -338,   314,   226,  -338,
    -338,  -338,   175,  -338,     6,     6,     6,     6,   161,  -338,
      39,  -338,  -338,   315,   316,   317,  -338,  -338,   228,  -338,
    -338,   179,  -338,  -338,   188,  -338,   229,  -338,   231,  -338,
      78,  -338,   318,   319,   320,   321,   322,   323,   324,   325,
     326,   327,  -338,  -338,   328,   329,  -338,  -338,  -338,  -338,
    -338,   330,   331,   332,   333,   334,  -338,  -338,  -338,  -338,
    -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,   335,   336,
     337,   338,   339,   161,  -338,   268,  -338,   161,  -338,  -338,
    -338,  -338,  -338,  -338,  -338,   236,   161,   248,     6,  -338,
    -338,   238,  -338,  -338,   340,  -338,  -338,   341,  -338,  -338,
     342,  -338,  -338,   343,  -338,  -338,  -338,   240,  -338,     6,
     344,   292,   345,   294,   346,   296,   298,   300,   301,   347,
       6,   348,   349,   350,   351,   352,   353,   354,   355,   249,
     159,  -338,  -338,  -338,  -338,   356,  -338,  -338,  -338,  -338,
     357,   179,   188,   229,   231,  -338,   241,  -338,  -338,  -338,
    -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,
    -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,
    -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,   360,   361,
     362,   363,  -338,   179,   188,   229,   231,   364,   365,   278,
     284,   179,   188,  -338,  -338,   366,   368,   179,  -338,   369,
     179,   370,   179,   371,   179,   372,   179,   373,   179,   374,
     179,   375,   179,   376,   179,   377,   179,   378,   179,   379,
     179,   381,  -338
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     4,     1,     0,     0,     0,     0,     0,     0,     0,
       2,     0,     0,     5,     6,     0,     7,     0,     8,     0,
       9,     0,    10,     0,    11,     3,     0,     0,     0,     0,
       0,     0,     0,    24,     0,    26,     0,    40,     0,    42,
       0,    88,     0,    90,     0,    98,     0,   100,     0,   114,
       0,   116,     0,     0,     0,     0,     0,    12,     0,     0,
       0,     0,    28,     0,    23,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    44,     0,    39,     0,
       0,     0,    92,     0,    87,     0,     0,     0,     0,     0,
       0,   102,     0,    97,     0,     0,     0,     0,     0,   118,
       0,   113,     0,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   143,    18,    19,    20,    21,
     166,   167,   174,     0,     0,   171,   172,   170,   173,    22,
     168,   169,     0,     0,     0,     0,     0,     0,     0,     0,
      27,     0,    25,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    43,     0,    41,     0,     0,    91,     0,
      89,     0,     0,     0,     0,     0,   101,     0,    99,     0,
       0,     0,     0,   117,     0,   115,   176,     0,     0,   178,
     181,   184,     0,   183,     0,     0,     0,     0,     0,    34,
       0,    32,    33,     0,     0,     0,    64,    55,     0,    56,
      57,     0,    58,    59,     0,    60,     0,    61,     0,    62,
       0,    63,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    95,    96,     0,     0,   110,   108,   109,   111,
     112,     0,     0,     0,     0,     0,   132,   131,   133,   134,
     136,   137,   138,   139,   140,   141,   142,   135,     0,     0,
       0,     0,     0,     0,   175,     0,   180,     0,    13,    14,
      15,    16,    17,    36,    38,     0,     0,     0,     0,    66,
      68,     0,    71,    70,     0,    74,    73,     0,    77,    76,
       0,    80,    79,     0,    82,    86,    85,     0,    84,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   179,   177,   182,    35,     0,    31,    29,    30,    65,
       0,     0,     0,     0,     0,    81,     0,    54,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    93,    94,   105,
     103,   104,   106,   107,   120,   119,   121,   122,   124,   125,
     126,   127,   128,   129,   130,   123,    37,    67,     0,     0,
       0,     0,    83,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    75,    78,     0,     0,     0,    72,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    69
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -338,  -338,  -338,   401,  -338,  -338,  -338,  -338,   233,  -338,
      55,  -338,  -338,  -338,   309,  -338,    32,  -338,    41,  -337,
      30,  -336,    75,  -327,    76,  -312,    38,  -338,    11,  -338,
    -338,   297,  -338,  -338,  -338,   358,  -338,  -338,  -338,   359,
    -338,   -62,  -183,   -61,  -338,  -338,   124,  -338,  -338,   158
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     2,    10,    11,    23,    41,    26,    44,    45,    73,
     211,   285,    28,    48,    49,    87,   219,   291,   222,   294,
     225,   297,   227,   300,   229,   303,   231,   307,   308,    30,
      52,    53,    93,    32,    56,    57,   102,    34,    60,    61,
     110,   147,   148,   201,   150,   198,   199,   151,   202,   203
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     136,   137,   138,   139,   378,   149,   379,     1,   259,   113,
     114,   115,   116,   117,   118,   119,   380,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   381,    90,    91,    69,    74,    75,    70,    71,
     105,   140,   141,    12,   106,   107,   387,   108,   388,   213,
     196,   258,   214,   215,   395,   197,   396,    22,   389,    92,
     399,   135,   109,   401,    20,   403,    42,   405,    21,   407,
      43,   409,    13,   411,   390,   413,    72,   415,    46,   417,
      50,   419,    47,   421,    51,   260,   261,   262,   263,   264,
     265,   266,   283,    14,    54,   284,   209,   212,    55,   268,
      15,   216,   267,   269,   270,   271,   272,    16,   113,   114,
     115,   116,   117,   118,   119,   243,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   304,    17,    96,   305,   306,    97,    98,    99,   100,
      88,    89,   278,   279,   280,   281,   367,   282,    94,    95,
      18,   140,   141,   142,   143,   103,   104,   144,   200,   101,
     135,   145,   146,    19,   113,   114,   115,   116,   117,   118,
     119,    24,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    25,   368,   369,
     370,   371,   372,   373,   374,   111,   112,   140,   141,   142,
     143,    62,   331,   144,     3,   375,   135,   145,   146,   244,
     245,     4,     5,     6,    86,   336,   338,   251,   276,   277,
     252,   253,   254,   255,     7,   292,   293,     8,    36,    37,
      38,    39,     9,    40,   295,   296,    27,   347,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   358,   152,
     153,   154,   155,    58,   156,    67,   160,    59,   173,    68,
     161,   178,   174,   186,   193,   179,   274,   187,   194,    29,
     275,   289,    31,    63,   290,   298,   299,   301,   302,   334,
     335,   339,   340,   345,   346,   140,   141,   305,   306,    33,
     210,    64,    65,    66,   157,   158,    42,   159,   162,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,    46,
     176,    50,   177,   197,   181,   182,   183,   184,   185,    54,
     189,   393,   190,   191,   192,    58,   204,   394,   205,   206,
     207,   208,   337,   349,   218,   353,   221,   217,   224,   220,
     226,   223,   228,   230,   351,   242,   356,   382,   246,   247,
     248,   249,   250,   256,   257,   273,   286,   287,   288,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   354,   180,   355,   341,   342,   343,   344,   175,   332,
     348,   350,   352,   357,   359,   360,   361,   362,   363,   364,
     365,   366,   376,   377,   383,   384,   385,   386,   391,   392,
     397,   398,    35,   400,   402,   404,   406,   408,   410,   412,
     414,   416,   418,   420,   422,   333,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   188,     0,     0,     0,     0,     0,     0,     0,
       0,   195
};

static const yytype_int16 yycheck[] =
{
      62,    63,    64,    65,   341,    66,   342,    49,   191,     3,
       4,     5,     6,     7,     8,     9,   343,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,   344,    24,    25,     9,    53,    54,    12,    13,
      28,    46,    47,     0,    32,    33,   383,    35,   384,     9,
      50,    56,    12,    13,   391,    55,   392,    49,   385,    50,
     397,    55,    50,   400,    50,   402,    49,   404,    54,   406,
      53,   408,    51,   410,   386,   412,    50,   414,    49,   416,
      49,   418,    53,   420,    53,    39,    40,    41,    42,    43,
      44,    45,    53,    51,    49,    56,   157,   159,    53,    28,
      51,   163,    56,    32,    33,    34,    35,    51,     3,     4,
       5,     6,     7,     8,     9,   177,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    53,    51,    24,    56,    57,    27,    28,    29,    30,
      53,    54,   204,   205,   206,   207,   329,   208,    53,    54,
      51,    46,    47,    48,    49,    53,    54,    52,    53,    50,
      55,    56,    57,    51,     3,     4,     5,     6,     7,     8,
       9,    56,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    52,    39,    40,
      41,    42,    43,    44,    45,    53,    54,    46,    47,    48,
      49,    51,   273,    52,     3,    56,    55,    56,    57,    24,
      25,    10,    11,    12,    50,   286,   288,    24,    53,    54,
      27,    28,    29,    30,    23,    56,    57,    26,     4,     5,
       6,     7,    31,     9,    56,    57,    52,   309,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,   320,     4,
       5,     6,     7,    49,     9,    50,    50,    53,    50,    54,
      54,    50,    54,    50,    50,    54,    50,    54,    54,    52,
      54,    53,    52,    51,    56,    56,    57,    56,    57,    53,
      54,    53,    54,    53,    54,    46,    47,    56,    57,    52,
      52,    51,    51,    51,    51,    51,    49,    51,    75,    51,
      51,    51,    51,    51,    51,    51,    51,    51,    51,    49,
      51,    49,    51,    55,    51,    51,    51,    51,    51,    49,
      51,    53,    51,    51,    51,    49,    51,    53,    51,    51,
      51,    51,   287,   311,    52,   315,    52,    56,    52,    56,
      52,    56,    52,    52,   313,    56,   318,   346,    56,    56,
      56,    56,    56,    56,    56,    51,    51,    51,    51,    51,
      51,    51,    51,    51,    51,    51,    51,    51,    51,    51,
      51,    51,    51,    51,    51,    51,    51,    51,    51,    51,
      51,   316,    95,   317,    54,    54,    54,    54,    89,   275,
      56,    56,    56,    56,    56,    56,    56,    56,    56,    56,
      56,    56,    56,    56,    54,    54,    54,    54,    54,    54,
      54,    53,    21,    54,    54,    54,    54,    54,    54,    54,
      54,    54,    54,    54,    53,   277,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   104,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   112
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    49,    60,     3,    10,    11,    12,    23,    26,    31,
      61,    62,     0,    51,    51,    51,    51,    51,    51,    51,
      50,    54,    49,    63,    56,    52,    65,    52,    71,    52,
      88,    52,    92,    52,    96,    62,     4,     5,     6,     7,
       9,    64,    49,    53,    66,    67,    49,    53,    72,    73,
      49,    53,    89,    90,    49,    53,    93,    94,    49,    53,
      97,    98,    51,    51,    51,    51,    51,    50,    54,     9,
      12,    13,    50,    68,    53,    54,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    50,    74,    53,    54,
      24,    25,    50,    91,    53,    54,    24,    27,    28,    29,
      30,    50,    95,    53,    54,    28,    32,    33,    35,    50,
      99,    53,    54,     3,     4,     5,     6,     7,     8,     9,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    55,   100,   100,   100,   100,
      46,    47,    48,    49,    52,    56,    57,   100,   101,   102,
     103,   106,     4,     5,     6,     7,     9,    51,    51,    51,
      50,    54,    67,    51,    51,    51,    51,    51,    51,    51,
      51,    51,    51,    50,    54,    73,    51,    51,    50,    54,
      90,    51,    51,    51,    51,    51,    50,    54,    94,    51,
      51,    51,    51,    50,    54,    98,    50,    55,   104,   105,
      53,   102,   107,   108,    51,    51,    51,    51,    51,   102,
      52,    69,   100,     9,    12,    13,   100,    56,    52,    75,
      56,    52,    77,    56,    52,    79,    52,    81,    52,    83,
      52,    85,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    56,   100,    24,    25,    56,    56,    56,    56,
      56,    24,    27,    28,    29,    30,    56,    56,    56,   101,
      39,    40,    41,    42,    43,    44,    45,    56,    28,    32,
      33,    34,    35,    51,    50,    54,    53,    54,   100,   100,
     100,   100,   102,    53,    56,    70,    51,    51,    51,    53,
      56,    76,    56,    57,    78,    56,    57,    80,    56,    57,
      82,    56,    57,    84,    53,    56,    57,    86,    87,    51,
      51,    51,    51,    51,    51,    51,    51,    51,    51,    51,
      51,    51,    51,    51,    51,    51,    51,    51,    51,    51,
      51,   102,   105,   108,    53,    54,   102,    69,   100,    53,
      54,    54,    54,    54,    54,    53,    54,   100,    56,    75,
      56,    77,    56,    79,    81,    83,    85,    56,   100,    56,
      56,    56,    56,    56,    56,    56,    56,   101,    39,    40,
      41,    42,    43,    44,    45,    56,    56,    56,    78,    80,
      82,    84,    87,    54,    54,    54,    54,    78,    80,    82,
      84,    54,    54,    53,    53,    78,    80,    54,    53,    78,
      54,    78,    54,    78,    54,    78,    54,    78,    54,    78,
      54,    78,    54,    78,    54,    78,    54,    78,    54,    78,
      54,    78,    53
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    59,    60,    61,    61,    62,    62,    62,    62,    62,
      62,    62,    63,    64,    64,    64,    64,    64,    64,    64,
      64,    64,    64,    65,    65,    66,    66,    67,    67,    68,
      68,    68,    68,    68,    68,    69,    69,    70,    70,    71,
      71,    72,    72,    73,    73,    74,    74,    74,    74,    74,
      74,    74,    74,    74,    74,    74,    74,    74,    74,    74,
      74,    74,    74,    74,    74,    75,    75,    76,    76,    77,
      78,    78,    79,    80,    80,    81,    82,    82,    83,    84,
      84,    85,    85,    86,    86,    87,    87,    88,    88,    89,
      89,    90,    90,    91,    91,    91,    91,    92,    92,    93,
      93,    94,    94,    95,    95,    95,    95,    95,    95,    95,
      95,    95,    95,    96,    96,    97,    97,    98,    98,    99,
      99,    99,    99,    99,    99,    99,    99,    99,    99,    99,
      99,    99,    99,    99,    99,    99,    99,    99,    99,    99,
      99,    99,    99,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   101,   101,   102,   102,
     102,   102,   102,   102,   102,   103,   103,   104,   104,   105,
     106,   106,   107,   107,   108
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     3,     3,     1,     3,     3,     3,     3,     3,
       3,     3,     3,     5,     5,     5,     5,     5,     3,     3,
       3,     3,     3,     3,     2,     3,     1,     3,     2,     5,
       5,     5,     3,     3,     3,     3,     2,     3,     1,     3,
       2,     3,     1,     3,     2,     5,     5,     5,     5,     5,
       5,     5,     5,     5,     5,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     2,     3,     1,    33,
       1,     1,     9,     1,     1,     7,     1,     1,     7,     1,
       1,     3,     2,     3,     1,     1,     1,     3,     2,     3,
       1,     3,     2,     5,     5,     3,     3,     3,     2,     3,
       1,     3,     2,     5,     5,     5,     5,     5,     3,     3,
       3,     3,     3,     3,     2,     3,     1,     3,     2,     5,
       5,     5,     5,     5,     5,     5,     5,     5,     5,     5,
       5,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     2,     3,     1,     3,
       3,     2,     3,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (&yylloc, user_defined, yyscanner, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YY_LOCATION_PRINT
#  if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
 }

#   define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

#  else
#   define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#  endif
# endif /* !defined YY_LOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location, user_defined, yyscanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, void *user_defined, void *yyscanner)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  YY_USE (user_defined);
  YY_USE (yyscanner);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yykind < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yykind], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, void *user_defined, void *yyscanner)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YY_LOCATION_PRINT (yyo, *yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp, user_defined, yyscanner);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule, void *user_defined, void *yyscanner)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]), user_defined, yyscanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, user_defined, yyscanner); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
  YYLTYPE *yylloc;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;
      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, void *user_defined, void *yyscanner)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  YY_USE (user_defined);
  YY_USE (yyscanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (void *user_defined, void *yyscanner)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */
  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, &yylloc, user_defined, yyscanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* gltf_object: YYTOKEN_LEFTBRACE gltf_members YYTOKEN_RIGHTBRACE  */
#line 138 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                { 
	//$$ = $2;
}
#line 1981 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 3: /* gltf_members: gltf_members YYTOKEN_COMMA gltf_member  */
#line 142 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                     { 
	//bool _res_addmember = JsonParser_Json_Object_AddMember(pUserData, $1, $3._stdstring, $3._jsonvalue); 
	//JsonParser_Std_String_Dispose(pUserData, $3._stdstring);
	//if(!_res_addmember) { yyerror(&yylloc, pUserData, pScanner, "Duplicate object key"); }
	//$$ = $1;
}
#line 1992 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 4: /* gltf_members: gltf_member  */
#line 149 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                          { 
    //void *_jsonobject = JsonParser_Json_Object_Create(pUserData);
	//bool _res_addmember = JsonParser_Json_Object_AddMember(pUserData, _jsonobject, $1._stdstring, $1._jsonvalue);
	//JsonParser_Std_String_Dispose(pUserData, $1._stdstring);
	//if(!_res_addmember) { yyerror(&yylloc, pUserData, pScanner, "Duplicate object key"); }
	//$$ = _jsonobject;
}
#line 2004 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 5: /* gltf_member: YYTOKEN_ASSET YYTOKEN_COLON asset_object  */
#line 157 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                      {
	//$$ = $3;
}
#line 2012 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 6: /* gltf_member: YYTOKEN_SCENE YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 161 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                            {
	gltf_yacc_set_default_scene_index_callback((yyvsp[0].m_token_numberint), user_defined);
}
#line 2020 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 7: /* gltf_member: YYTOKEN_SCENES YYTOKEN_COLON scenes_array  */
#line 165 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                       {

}
#line 2028 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 8: /* gltf_member: YYTOKEN_NODES YYTOKEN_COLON nodes_array  */
#line 169 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                     {

}
#line 2036 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 9: /* gltf_member: YYTOKEN_BUFFERS YYTOKEN_COLON buffers_array  */
#line 173 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                         {

}
#line 2044 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 10: /* gltf_member: YYTOKEN_BUFFERVIEWS YYTOKEN_COLON bufferviews_array  */
#line 177 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                 {

}
#line 2052 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 11: /* gltf_member: YYTOKEN_ACCESSORS YYTOKEN_COLON accessors_array  */
#line 181 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                             {

}
#line 2060 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 12: /* asset_object: YYTOKEN_LEFTBRACE asset_properties YYTOKEN_RIGHTBRACE  */
#line 185 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                     { 
	
}
#line 2068 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 13: /* asset_properties: asset_properties YYTOKEN_COMMA YYTOKEN_COPYRIGHT YYTOKEN_COLON json_string  */
#line 189 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                             { 

}
#line 2076 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 14: /* asset_properties: asset_properties YYTOKEN_COMMA YYTOKEN_GENERATOR YYTOKEN_COLON json_string  */
#line 193 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                             { 

}
#line 2084 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 15: /* asset_properties: asset_properties YYTOKEN_COMMA YYTOKEN_VERSION YYTOKEN_COLON json_string  */
#line 197 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                           { 

}
#line 2092 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 16: /* asset_properties: asset_properties YYTOKEN_COMMA YYTOKEN_MINVERSION YYTOKEN_COLON json_string  */
#line 201 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                              { 

}
#line 2100 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 17: /* asset_properties: asset_properties YYTOKEN_COMMA YYTOKEN_EXTRAS YYTOKEN_COLON json_value  */
#line 205 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                         { 

}
#line 2108 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 18: /* asset_properties: YYTOKEN_COPYRIGHT YYTOKEN_COLON json_string  */
#line 209 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                              { 

}
#line 2116 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 19: /* asset_properties: YYTOKEN_GENERATOR YYTOKEN_COLON json_string  */
#line 213 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                              { 

}
#line 2124 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 20: /* asset_properties: YYTOKEN_VERSION YYTOKEN_COLON json_string  */
#line 217 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                            { 

}
#line 2132 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 21: /* asset_properties: YYTOKEN_MINVERSION YYTOKEN_COLON json_string  */
#line 221 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                               { 

}
#line 2140 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 22: /* asset_properties: YYTOKEN_EXTRAS YYTOKEN_COLON json_value  */
#line 225 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                          { 

}
#line 2148 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 23: /* scenes_array: YYTOKEN_LEFTBRACKET scene_objects YYTOKEN_RIGHTBRACKET  */
#line 229 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                     { 

}
#line 2156 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 24: /* scenes_array: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 233 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                       {

}
#line 2164 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 25: /* scene_objects: scene_objects YYTOKEN_COMMA scene_object  */
#line 237 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                        { 
	assert(-1 == (yyvsp[0].m_scene_index) || gltf_yacc_scene_size_callback(user_defined) == ((yyvsp[0].m_scene_index) + 1));
}
#line 2172 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 26: /* scene_objects: scene_object  */
#line 241 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                            {
	assert(-1 == (yyvsp[0].m_scene_index) || gltf_yacc_scene_size_callback(user_defined) == ((yyvsp[0].m_scene_index) + 1));
}
#line 2180 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 27: /* scene_object: YYTOKEN_LEFTBRACE scene_properties YYTOKEN_RIGHTBRACE  */
#line 245 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                    {
	(yyval.m_scene_index) = (yyvsp[-1].m_scene_index);
}
#line 2188 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 28: /* scene_object: YYTOKEN_LEFTBRACE YYTOKEN_RIGHTBRACE  */
#line 249 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                   { 
	(yyval.m_scene_index) = -1;
}
#line 2196 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 29: /* scene_properties: scene_properties YYTOKEN_COMMA YYTOKEN_NODES YYTOKEN_COLON scene_property_nodes  */
#line 253 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                  {
	gltf_yacc_scene_set_nodes_callback((yyval.m_scene_index), (yyvsp[0].m_temp_int_array_version), user_defined);
	gltf_yacc_temp_int_array_destroy_callback((yyvsp[0].m_temp_int_array_version), user_defined);
}
#line 2205 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 30: /* scene_properties: scene_properties YYTOKEN_COMMA YYTOKEN_NAME YYTOKEN_COLON json_string  */
#line 258 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                        {
	gltf_yacc_scene_set_name_callback((yyval.m_scene_index), (yyvsp[0].m_token_string).m_data, (yyvsp[0].m_token_string).m_size, user_defined);
}
#line 2213 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 31: /* scene_properties: scene_properties YYTOKEN_COMMA YYTOKEN_EXTRAS YYTOKEN_COLON json_value  */
#line 262 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                         { 
	// ignore
}
#line 2221 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 32: /* scene_properties: YYTOKEN_NODES YYTOKEN_COLON scene_property_nodes  */
#line 266 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                   { 
	(yyval.m_scene_index) = gltf_yacc_scene_push_callback(user_defined);
	gltf_yacc_scene_set_nodes_callback((yyval.m_scene_index), (yyvsp[0].m_temp_int_array_version), user_defined);
	gltf_yacc_temp_int_array_destroy_callback((yyvsp[0].m_temp_int_array_version), user_defined);
}
#line 2231 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 33: /* scene_properties: YYTOKEN_NAME YYTOKEN_COLON json_string  */
#line 272 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                         {
	(yyval.m_scene_index) = gltf_yacc_scene_push_callback(user_defined);
	gltf_yacc_scene_set_name_callback((yyval.m_scene_index), (yyvsp[0].m_token_string).m_data, (yyvsp[0].m_token_string).m_size, user_defined);
}
#line 2240 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 34: /* scene_properties: YYTOKEN_EXTRAS YYTOKEN_COLON json_value  */
#line 277 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                          { 

}
#line 2248 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 35: /* scene_property_nodes: YYTOKEN_LEFTBRACKET scene_property_nodes_elements YYTOKEN_RIGHTBRACKET  */
#line 281 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                             {
	(yyval.m_temp_int_array_version) = (yyvsp[-1].m_temp_int_array_version);
}
#line 2256 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 36: /* scene_property_nodes: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 285 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                               { 
	(yyval.m_temp_int_array_version) = gltf_yacc_temp_int_array_init_callback(user_defined);
}
#line 2264 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 37: /* scene_property_nodes_elements: scene_property_nodes_elements YYTOKEN_COMMA YYTOKEN_NUMBER_INT  */
#line 289 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                              {
	gltf_yacc_temp_int_array_push_callback((yyval.m_temp_int_array_version), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2272 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 38: /* scene_property_nodes_elements: YYTOKEN_NUMBER_INT  */
#line 293 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                   {
	(yyval.m_temp_int_array_version) = gltf_yacc_temp_int_array_init_callback(user_defined);
	gltf_yacc_temp_int_array_push_callback((yyval.m_temp_int_array_version), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2281 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 39: /* nodes_array: YYTOKEN_LEFTBRACKET node_objects YYTOKEN_RIGHTBRACKET  */
#line 298 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                   {

}
#line 2289 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 40: /* nodes_array: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 302 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                      {

}
#line 2297 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 41: /* node_objects: node_objects YYTOKEN_COMMA node_object  */
#line 306 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                     { 
   	assert(-1 == (yyvsp[0].m_node_index) || gltf_yacc_node_size_callback(user_defined) == ((yyvsp[0].m_node_index) + 1));
}
#line 2305 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 42: /* node_objects: node_object  */
#line 310 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                          { 
	assert(-1 == (yyvsp[0].m_node_index) || gltf_yacc_node_size_callback(user_defined) == ((yyvsp[0].m_node_index) + 1));
}
#line 2313 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 43: /* node_object: YYTOKEN_LEFTBRACE node_properties YYTOKEN_RIGHTBRACE  */
#line 314 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                  {
	(yyval.m_node_index) = (yyvsp[-1].m_node_index);
}
#line 2321 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 44: /* node_object: YYTOKEN_LEFTBRACE YYTOKEN_RIGHTBRACE  */
#line 318 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                  { 
	(yyval.m_node_index) = -1;
}
#line 2329 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 45: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_CAMERA YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 322 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                               {
	gltf_yacc_node_set_camera_callback((yyval.m_node_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2337 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 46: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_CHILDREN YYTOKEN_COLON node_property_children  */
#line 326 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                     {
	gltf_yacc_node_set_children_callback((yyval.m_node_index), (yyvsp[0].m_temp_int_array_version), user_defined);
	gltf_yacc_temp_int_array_destroy_callback((yyvsp[0].m_temp_int_array_version), user_defined);
}
#line 2346 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 47: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_SKIN YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 331 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                             {
	gltf_yacc_node_set_skin_callback((yyval.m_node_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2354 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 48: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_MATRIX YYTOKEN_COLON node_property_matrix  */
#line 335 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                 {
	gltf_yacc_node_set_matrix_callback((yyval.m_node_index), (yyvsp[0].m_mat4x4), user_defined);
}
#line 2362 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 49: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_MESH YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 339 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                             {
	gltf_yacc_node_set_mesh_callback((yyval.m_node_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2370 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 50: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_ROTATION YYTOKEN_COLON node_property_rotation  */
#line 343 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                     {
	gltf_yacc_node_set_rotation_callback((yyval.m_node_index), (yyvsp[0].m_vec4), user_defined);
}
#line 2378 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 51: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_SCALE YYTOKEN_COLON node_property_scale  */
#line 347 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                               {
	gltf_yacc_node_set_scale_callback((yyval.m_node_index), (yyvsp[0].m_vec3), user_defined);
}
#line 2386 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 52: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_TRANSLATION YYTOKEN_COLON node_property_translation  */
#line 351 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                           {
	gltf_yacc_node_set_translation_callback((yyval.m_node_index), (yyvsp[0].m_vec3), user_defined);
}
#line 2394 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 53: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_WEIGHTS YYTOKEN_COLON node_property_weights  */
#line 355 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                   {
	gltf_yacc_node_set_weights_callback((yyval.m_node_index), (yyvsp[0].m_temp_float_array_version), user_defined);
	gltf_yacc_temp_float_array_destroy_callback((yyvsp[0].m_temp_float_array_version), user_defined);
}
#line 2403 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 54: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_NAME YYTOKEN_COLON json_string  */
#line 360 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                      {
	gltf_yacc_node_set_name_callback((yyval.m_node_index), (yyvsp[0].m_token_string).m_data, (yyvsp[0].m_token_string).m_size, user_defined);
}
#line 2411 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 55: /* node_properties: YYTOKEN_CAMERA YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 364 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                 {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_camera_callback((yyval.m_node_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2420 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 56: /* node_properties: YYTOKEN_CHILDREN YYTOKEN_COLON node_property_children  */
#line 369 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                       {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_children_callback((yyval.m_node_index), (yyvsp[0].m_temp_int_array_version), user_defined);
	gltf_yacc_temp_int_array_destroy_callback((yyvsp[0].m_temp_int_array_version), user_defined);
}
#line 2430 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 57: /* node_properties: YYTOKEN_SKIN YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 375 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                               {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_skin_callback((yyval.m_node_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2439 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 58: /* node_properties: YYTOKEN_MATRIX YYTOKEN_COLON node_property_matrix  */
#line 380 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                   {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_matrix_callback((yyval.m_node_index), (yyvsp[0].m_mat4x4), user_defined);
}
#line 2448 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 59: /* node_properties: YYTOKEN_MESH YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 385 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                               {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_mesh_callback((yyval.m_node_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2457 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 60: /* node_properties: YYTOKEN_ROTATION YYTOKEN_COLON node_property_rotation  */
#line 390 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                       {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_rotation_callback((yyval.m_node_index), (yyvsp[0].m_vec4), user_defined);
}
#line 2466 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 61: /* node_properties: YYTOKEN_SCALE YYTOKEN_COLON node_property_scale  */
#line 395 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                 {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_scale_callback((yyval.m_node_index), (yyvsp[0].m_vec3), user_defined);
}
#line 2475 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 62: /* node_properties: YYTOKEN_TRANSLATION YYTOKEN_COLON node_property_translation  */
#line 400 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                             {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_translation_callback((yyval.m_node_index), (yyvsp[0].m_vec3), user_defined);
}
#line 2484 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 63: /* node_properties: YYTOKEN_WEIGHTS YYTOKEN_COLON node_property_weights  */
#line 405 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                     {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_weights_callback((yyval.m_node_index), (yyvsp[0].m_temp_float_array_version), user_defined);
	gltf_yacc_temp_float_array_destroy_callback((yyvsp[0].m_temp_float_array_version), user_defined);
}
#line 2494 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 64: /* node_properties: YYTOKEN_NAME YYTOKEN_COLON json_string  */
#line 411 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                        {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_name_callback((yyval.m_node_index), (yyvsp[0].m_token_string).m_data, (yyvsp[0].m_token_string).m_size, user_defined);
}
#line 2503 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 65: /* node_property_children: YYTOKEN_LEFTBRACKET node_property_children_elements YYTOKEN_RIGHTBRACKET  */
#line 416 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                 {
	(yyval.m_temp_int_array_version) = (yyvsp[-1].m_temp_int_array_version);
}
#line 2511 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 66: /* node_property_children: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 420 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                 { 
    (yyval.m_temp_int_array_version) = gltf_yacc_temp_int_array_init_callback(user_defined);
}
#line 2519 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 67: /* node_property_children_elements: node_property_children_elements YYTOKEN_COMMA YYTOKEN_NUMBER_INT  */
#line 424 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                  {
	gltf_yacc_temp_int_array_push_callback((yyval.m_temp_int_array_version), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2527 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 68: /* node_property_children_elements: YYTOKEN_NUMBER_INT  */
#line 428 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                     {
	(yyval.m_temp_int_array_version) = gltf_yacc_temp_int_array_init_callback(user_defined);
	gltf_yacc_temp_int_array_push_callback((yyval.m_temp_int_array_version), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2536 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 69: /* node_property_matrix: YYTOKEN_LEFTBRACKET node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_RIGHTBRACKET  */
#line 433 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 {
	(yyval.m_mat4x4)[0] = (yyvsp[-31].m_token_numberfloat);
	(yyval.m_mat4x4)[1] = (yyvsp[-29].m_token_numberfloat);
	(yyval.m_mat4x4)[2] = (yyvsp[-27].m_token_numberfloat);
	(yyval.m_mat4x4)[3] = (yyvsp[-25].m_token_numberfloat);
	(yyval.m_mat4x4)[4] = (yyvsp[-23].m_token_numberfloat);
	(yyval.m_mat4x4)[5] = (yyvsp[-21].m_token_numberfloat);
	(yyval.m_mat4x4)[6] = (yyvsp[-19].m_token_numberfloat);
	(yyval.m_mat4x4)[7] = (yyvsp[-17].m_token_numberfloat);
	(yyval.m_mat4x4)[8] = (yyvsp[-15].m_token_numberfloat);
	(yyval.m_mat4x4)[9] = (yyvsp[-13].m_token_numberfloat);
	(yyval.m_mat4x4)[10] = (yyvsp[-11].m_token_numberfloat);
	(yyval.m_mat4x4)[11] = (yyvsp[-9].m_token_numberfloat);
	(yyval.m_mat4x4)[12] = (yyvsp[-7].m_token_numberfloat);
	(yyval.m_mat4x4)[13] = (yyvsp[-5].m_token_numberfloat);
	(yyval.m_mat4x4)[14] = (yyvsp[-3].m_token_numberfloat);
	(yyval.m_mat4x4)[15] = (yyvsp[-1].m_token_numberfloat);
}
#line 2559 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 70: /* node_property_matrix_element: YYTOKEN_NUMBER_FLOAT  */
#line 452 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                   {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberfloat);
}
#line 2567 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 71: /* node_property_matrix_element: YYTOKEN_NUMBER_INT  */
#line 456 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                 {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberint);
}
#line 2575 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 72: /* node_property_rotation: YYTOKEN_LEFTBRACKET node_property_rotation_element YYTOKEN_COMMA node_property_rotation_element YYTOKEN_COMMA node_property_rotation_element YYTOKEN_COMMA node_property_rotation_element YYTOKEN_RIGHTBRACKET  */
#line 460 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                                                                                                                                                       {
	(yyval.m_vec4)[0] = (yyvsp[-7].m_token_numberfloat);
	(yyval.m_vec4)[1] = (yyvsp[-5].m_token_numberfloat);
	(yyval.m_vec4)[2] = (yyvsp[-3].m_token_numberfloat);
	(yyval.m_vec4)[3] = (yyvsp[-1].m_token_numberfloat);
}
#line 2586 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 73: /* node_property_rotation_element: YYTOKEN_NUMBER_FLOAT  */
#line 467 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                     {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberfloat);
}
#line 2594 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 74: /* node_property_rotation_element: YYTOKEN_NUMBER_INT  */
#line 471 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                   {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberint);
}
#line 2602 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 75: /* node_property_scale: YYTOKEN_LEFTBRACKET node_property_scale_element YYTOKEN_COMMA node_property_scale_element YYTOKEN_COMMA node_property_scale_element YYTOKEN_RIGHTBRACKET  */
#line 475 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                                                                                              {
	(yyval.m_vec3)[0] = (yyvsp[-5].m_token_numberfloat);
	(yyval.m_vec3)[1] = (yyvsp[-3].m_token_numberfloat);
	(yyval.m_vec3)[2] = (yyvsp[-1].m_token_numberfloat);
}
#line 2612 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 76: /* node_property_scale_element: YYTOKEN_NUMBER_FLOAT  */
#line 481 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                  {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberfloat);
}
#line 2620 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 77: /* node_property_scale_element: YYTOKEN_NUMBER_INT  */
#line 485 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberint);
}
#line 2628 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 78: /* node_property_translation: YYTOKEN_LEFTBRACKET node_property_translation_element YYTOKEN_COMMA node_property_translation_element YYTOKEN_COMMA node_property_translation_element YYTOKEN_RIGHTBRACKET  */
#line 489 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                                                                                                                      {
	(yyval.m_vec3)[0] = (yyvsp[-5].m_token_numberfloat);
	(yyval.m_vec3)[1] = (yyvsp[-3].m_token_numberfloat);
	(yyval.m_vec3)[2] = (yyvsp[-1].m_token_numberfloat);
}
#line 2638 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 79: /* node_property_translation_element: YYTOKEN_NUMBER_FLOAT  */
#line 495 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                        {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberfloat);
}
#line 2646 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 80: /* node_property_translation_element: YYTOKEN_NUMBER_INT  */
#line 499 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                      {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberint);
}
#line 2654 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 81: /* node_property_weights: YYTOKEN_LEFTBRACKET node_property_weights_elements YYTOKEN_RIGHTBRACKET  */
#line 503 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                               {
	(yyval.m_temp_float_array_version) = (yyvsp[-1].m_temp_float_array_version);
}
#line 2662 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 82: /* node_property_weights: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 507 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                { 
    (yyval.m_temp_float_array_version) = gltf_yacc_temp_float_array_init_callback(user_defined);
}
#line 2670 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 83: /* node_property_weights_elements: node_property_weights_elements YYTOKEN_COMMA node_property_weights_element  */
#line 511 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                           {
	gltf_yacc_temp_float_array_push_callback((yyval.m_temp_float_array_version), (yyvsp[0].m_token_numberfloat), user_defined);
}
#line 2678 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 84: /* node_property_weights_elements: node_property_weights_element  */
#line 515 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                               {
	(yyval.m_temp_float_array_version) = gltf_yacc_temp_float_array_init_callback(user_defined);
	gltf_yacc_temp_float_array_push_callback((yyval.m_temp_float_array_version), (yyvsp[0].m_token_numberfloat), user_defined);
}
#line 2687 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 85: /* node_property_weights_element: YYTOKEN_NUMBER_FLOAT  */
#line 521 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                    {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberfloat);
}
#line 2695 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 86: /* node_property_weights_element: YYTOKEN_NUMBER_INT  */
#line 525 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                  {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberint);
}
#line 2703 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 87: /* buffers_array: YYTOKEN_LEFTBRACKET buffer_objects YYTOKEN_RIGHTBRACKET  */
#line 530 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                       {

}
#line 2711 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 88: /* buffers_array: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 534 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                        {

}
#line 2719 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 89: /* buffer_objects: buffer_objects YYTOKEN_COMMA buffer_object  */
#line 538 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                           { 
   	assert(-1 == (yyvsp[0].m_buffer_index) || gltf_yacc_buffer_size_callback(user_defined) == ((yyvsp[0].m_buffer_index) + 1));
}
#line 2727 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 90: /* buffer_objects: buffer_object  */
#line 542 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                              { 
	assert(-1 == (yyvsp[0].m_buffer_index) || gltf_yacc_buffer_size_callback(user_defined) == ((yyvsp[0].m_buffer_index) + 1));
}
#line 2735 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 91: /* buffer_object: YYTOKEN_LEFTBRACE buffer_properties YYTOKEN_RIGHTBRACE  */
#line 546 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                      {
	(yyval.m_buffer_index) = (yyvsp[-1].m_buffer_index);
}
#line 2743 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 92: /* buffer_object: YYTOKEN_LEFTBRACE YYTOKEN_RIGHTBRACE  */
#line 550 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                    { 
	(yyval.m_buffer_index) = -1;
}
#line 2751 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 93: /* buffer_properties: buffer_properties YYTOKEN_COMMA YYTOKEN_BYTELENGTH YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 554 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                       {
	gltf_yacc_buffer_set_bytelength_callback((yyval.m_buffer_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2759 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 94: /* buffer_properties: buffer_properties YYTOKEN_COMMA YYTOKEN_URI YYTOKEN_COLON json_string  */
#line 558 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                         {
	gltf_yacc_buffer_set_url_callback((yyval.m_buffer_index), (yyvsp[0].m_token_string).m_data, (yyvsp[0].m_token_string).m_size, user_defined);
}
#line 2767 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 95: /* buffer_properties: YYTOKEN_BYTELENGTH YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 563 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                       {
	(yyval.m_buffer_index) = gltf_yacc_buffer_push_callback(user_defined);
	gltf_yacc_buffer_set_bytelength_callback((yyval.m_buffer_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2776 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 96: /* buffer_properties: YYTOKEN_URI YYTOKEN_COLON json_string  */
#line 568 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                         {
	(yyval.m_buffer_index) = gltf_yacc_buffer_push_callback(user_defined);
	gltf_yacc_buffer_set_url_callback((yyval.m_buffer_index), (yyvsp[0].m_token_string).m_data, (yyvsp[0].m_token_string).m_size, user_defined);
}
#line 2785 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 97: /* bufferviews_array: YYTOKEN_LEFTBRACKET bufferview_objects YYTOKEN_RIGHTBRACKET  */
#line 573 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                               {

}
#line 2793 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 98: /* bufferviews_array: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 577 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                            {

}
#line 2801 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 99: /* bufferview_objects: bufferview_objects YYTOKEN_COMMA bufferview_object  */
#line 581 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                       { 
   	assert(-1 == (yyvsp[0].m_bufferview_index) || gltf_yacc_bufferview_size_callback(user_defined) == ((yyvsp[0].m_bufferview_index) + 1));
}
#line 2809 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 100: /* bufferview_objects: bufferview_object  */
#line 585 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                      { 
	assert(-1 == (yyvsp[0].m_bufferview_index) || gltf_yacc_bufferview_size_callback(user_defined) == ((yyvsp[0].m_bufferview_index) + 1));
}
#line 2817 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 101: /* bufferview_object: YYTOKEN_LEFTBRACE bufferview_properties YYTOKEN_RIGHTBRACE  */
#line 589 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                              {
	(yyval.m_bufferview_index) = (yyvsp[-1].m_bufferview_index);
}
#line 2825 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 102: /* bufferview_object: YYTOKEN_LEFTBRACE YYTOKEN_RIGHTBRACE  */
#line 593 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                        { 
	(yyval.m_bufferview_index) = -1;
}
#line 2833 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 103: /* bufferview_properties: bufferview_properties YYTOKEN_COMMA YYTOKEN_BUFFER YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 597 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                           {
	gltf_yacc_bufferview_set_buffer_callback((yyval.m_bufferview_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2841 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 104: /* bufferview_properties: bufferview_properties YYTOKEN_COMMA YYTOKEN_BYTEOFFSET YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 601 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                               {
	gltf_yacc_bufferview_set_byteoffset_callback((yyval.m_bufferview_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2849 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 105: /* bufferview_properties: bufferview_properties YYTOKEN_COMMA YYTOKEN_BYTELENGTH YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 605 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                               {
	gltf_yacc_bufferview_set_bytelength_callback((yyval.m_bufferview_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2857 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 106: /* bufferview_properties: bufferview_properties YYTOKEN_COMMA YYTOKEN_BYTESTRIDE YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 609 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                               {
	gltf_yacc_bufferview_set_bytestride_callback((yyval.m_bufferview_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2865 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 107: /* bufferview_properties: bufferview_properties YYTOKEN_COMMA YYTOKEN_TARGET YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 613 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                           {
	gltf_yacc_bufferview_set_target_callback((yyval.m_bufferview_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2873 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 108: /* bufferview_properties: YYTOKEN_BUFFER YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 617 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                       {
	(yyval.m_bufferview_index) = gltf_yacc_bufferview_push_callback(user_defined);
	gltf_yacc_bufferview_set_buffer_callback((yyval.m_bufferview_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2882 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 109: /* bufferview_properties: YYTOKEN_BYTEOFFSET YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 622 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                           {
	(yyval.m_bufferview_index) = gltf_yacc_bufferview_push_callback(user_defined);
	gltf_yacc_bufferview_set_byteoffset_callback((yyval.m_bufferview_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2891 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 110: /* bufferview_properties: YYTOKEN_BYTELENGTH YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 627 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                           {
	(yyval.m_bufferview_index) = gltf_yacc_bufferview_push_callback(user_defined);
	gltf_yacc_bufferview_set_bytelength_callback((yyval.m_bufferview_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2900 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 111: /* bufferview_properties: YYTOKEN_BYTESTRIDE YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 632 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                           {
	(yyval.m_bufferview_index) = gltf_yacc_bufferview_push_callback(user_defined);
	gltf_yacc_bufferview_set_bytestride_callback((yyval.m_bufferview_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2909 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 112: /* bufferview_properties: YYTOKEN_TARGET YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 637 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                       {
	(yyval.m_bufferview_index) = gltf_yacc_bufferview_push_callback(user_defined);
	gltf_yacc_bufferview_set_target_callback((yyval.m_bufferview_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2918 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 113: /* accessors_array: YYTOKEN_LEFTBRACKET accessor_objects YYTOKEN_RIGHTBRACKET  */
#line 642 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                           {

}
#line 2926 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 114: /* accessors_array: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 646 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                          {

}
#line 2934 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 115: /* accessor_objects: accessor_objects YYTOKEN_COMMA accessor_object  */
#line 650 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                 { 
   	assert(-1 == (yyvsp[0].m_accessor_index) || gltf_yacc_accessor_size_callback(user_defined) == ((yyvsp[0].m_accessor_index) + 1));
}
#line 2942 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 116: /* accessor_objects: accessor_object  */
#line 654 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                  { 
	assert(-1 == (yyvsp[0].m_accessor_index) || gltf_yacc_accessor_size_callback(user_defined) == ((yyvsp[0].m_accessor_index) + 1));
}
#line 2950 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 117: /* accessor_object: YYTOKEN_LEFTBRACE accessor_properties YYTOKEN_RIGHTBRACE  */
#line 658 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                          {
	(yyval.m_accessor_index) = (yyvsp[-1].m_accessor_index);
}
#line 2958 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 118: /* accessor_object: YYTOKEN_LEFTBRACE YYTOKEN_RIGHTBRACE  */
#line 662 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                      { 
	(yyval.m_accessor_index) = -1;
}
#line 2966 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 119: /* accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_BUFFERVIEW YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 666 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                           {
	gltf_yacc_accessor_set_bufferview_callback((yyval.m_accessor_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2974 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 120: /* accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_BYTEOFFSET YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 670 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                           {
	gltf_yacc_accessor_set_byteoffset_callback((yyval.m_accessor_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2982 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 121: /* accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_COMPONENTTYPE YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 674 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                              {
	gltf_yacc_accessor_set_componenttype_callback((yyval.m_accessor_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2990 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 122: /* accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_NORMALIZED YYTOKEN_COLON json_boolean  */
#line 678 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                     {
	gltf_yacc_accessor_set_normalized_callback((yyval.m_accessor_index), (yyvsp[0].m_type_boolean), user_defined);
}
#line 2998 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 123: /* accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 682 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                      {
	gltf_yacc_accessor_set_count_callback((yyval.m_accessor_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 3006 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 124: /* accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_SCALAR  */
#line 686 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                  {
	gltf_yacc_accessor_set_type_callback((yyval.m_accessor_index), 1, user_defined);
}
#line 3014 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 125: /* accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_VEC2  */
#line 690 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                {
	gltf_yacc_accessor_set_type_callback((yyval.m_accessor_index), 2, user_defined);
}
#line 3022 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 126: /* accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_VEC3  */
#line 694 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                {
	gltf_yacc_accessor_set_type_callback((yyval.m_accessor_index), 3, user_defined);
}
#line 3030 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 127: /* accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_VEC4  */
#line 698 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                {
	gltf_yacc_accessor_set_type_callback((yyval.m_accessor_index), 4, user_defined);
}
#line 3038 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 128: /* accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_MAT2  */
#line 702 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                {
	gltf_yacc_accessor_set_type_callback((yyval.m_accessor_index), 5, user_defined);
}
#line 3046 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 129: /* accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_MAT3  */
#line 706 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                {
	gltf_yacc_accessor_set_type_callback((yyval.m_accessor_index), 9, user_defined);
}
#line 3054 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 130: /* accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_MAT4  */
#line 710 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                {
	gltf_yacc_accessor_set_type_callback((yyval.m_accessor_index), 16, user_defined);
}
#line 3062 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 131: /* accessor_properties: YYTOKEN_BUFFERVIEW YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 714 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                         {
	(yyval.m_accessor_index) = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_bufferview_callback((yyval.m_accessor_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 3071 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 132: /* accessor_properties: YYTOKEN_BYTEOFFSET YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 719 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                         {
	(yyval.m_accessor_index) = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_byteoffset_callback((yyval.m_accessor_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 3080 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 133: /* accessor_properties: YYTOKEN_COMPONENTTYPE YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 724 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                            {
	(yyval.m_accessor_index) = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_componenttype_callback((yyval.m_accessor_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 3089 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 134: /* accessor_properties: YYTOKEN_COMPONENTTYPE YYTOKEN_COLON json_boolean  */
#line 729 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                      {
	(yyval.m_accessor_index) = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_normalized_callback((yyval.m_accessor_index), (yyvsp[0].m_type_boolean), user_defined);
}
#line 3098 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 135: /* accessor_properties: YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 734 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                    {
	(yyval.m_accessor_index) = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_count_callback((yyval.m_accessor_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 3107 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 136: /* accessor_properties: YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_SCALAR  */
#line 739 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                {
	(yyval.m_accessor_index) = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_type_callback((yyval.m_accessor_index), 1, user_defined);
}
#line 3116 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 137: /* accessor_properties: YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_VEC2  */
#line 744 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                              {
	(yyval.m_accessor_index) = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_type_callback((yyval.m_accessor_index), 2, user_defined);
}
#line 3125 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 138: /* accessor_properties: YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_VEC3  */
#line 749 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                              {
	(yyval.m_accessor_index) = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_type_callback((yyval.m_accessor_index), 3, user_defined);
}
#line 3134 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 139: /* accessor_properties: YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_VEC4  */
#line 754 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                              {
	(yyval.m_accessor_index) = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_type_callback((yyval.m_accessor_index), 4, user_defined);
}
#line 3143 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 140: /* accessor_properties: YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_MAT2  */
#line 759 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                              {
	(yyval.m_accessor_index) = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_type_callback((yyval.m_accessor_index), 5, user_defined);
}
#line 3152 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 141: /* accessor_properties: YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_MAT3  */
#line 764 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                              {
	(yyval.m_accessor_index) = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_type_callback((yyval.m_accessor_index), 9, user_defined);
}
#line 3161 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 142: /* accessor_properties: YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_MAT4  */
#line 769 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                              {
	(yyval.m_accessor_index) = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_type_callback((yyval.m_accessor_index), 16, user_defined);
}
#line 3170 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 143: /* json_string: YYTOKEN_STRING  */
#line 774 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                            {

}
#line 3178 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 144: /* json_string: YYTOKEN_ASSET  */
#line 778 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                           {

}
#line 3186 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 145: /* json_string: YYTOKEN_COPYRIGHT  */
#line 782 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                               {

}
#line 3194 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 146: /* json_string: YYTOKEN_GENERATOR  */
#line 786 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                               {

}
#line 3202 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 147: /* json_string: YYTOKEN_VERSION  */
#line 790 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                             {

}
#line 3210 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 148: /* json_string: YYTOKEN_MINVERSION  */
#line 794 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                {

}
#line 3218 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 149: /* json_string: YYTOKEN_EXTENSIONS  */
#line 798 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                {

}
#line 3226 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 150: /* json_string: YYTOKEN_EXTRAS  */
#line 802 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                            {

}
#line 3234 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 151: /* json_string: YYTOKEN_SCENES  */
#line 806 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                            {

}
#line 3242 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 152: /* json_string: YYTOKEN_NODES  */
#line 810 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                           {

}
#line 3250 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 153: /* json_string: YYTOKEN_NAME  */
#line 814 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                          {

}
#line 3258 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 154: /* json_string: YYTOKEN_CAMERA  */
#line 818 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                            {

}
#line 3266 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 155: /* json_string: YYTOKEN_CHILDREN  */
#line 822 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                              {

}
#line 3274 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 156: /* json_string: YYTOKEN_SKIN  */
#line 826 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                          {

}
#line 3282 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 157: /* json_string: YYTOKEN_MATRIX  */
#line 830 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                            {

}
#line 3290 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 158: /* json_string: YYTOKEN_MESH  */
#line 834 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                          {

}
#line 3298 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 159: /* json_string: YYTOKEN_ROTATION  */
#line 838 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                              {

}
#line 3306 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 160: /* json_string: YYTOKEN_SCALE  */
#line 842 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                           {

}
#line 3314 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 161: /* json_string: YYTOKEN_TRANSLATION  */
#line 846 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                 {

}
#line 3322 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 162: /* json_string: YYTOKEN_WEIGHTS  */
#line 850 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                             {

}
#line 3330 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 163: /* json_string: YYTOKEN_BUFFERS  */
#line 854 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                             {

}
#line 3338 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 164: /* json_string: YYTOKEN_BYTELENGTH  */
#line 858 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                {

}
#line 3346 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 165: /* json_string: YYTOKEN_URI  */
#line 862 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                         {

}
#line 3354 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 166: /* json_boolean: YYTOKEN_TRUE  */
#line 866 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                           {
	(yyval.m_type_boolean) = true;
}
#line 3362 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 167: /* json_boolean: YYTOKEN_FALSE  */
#line 870 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                            {
	(yyval.m_type_boolean) = false;
}
#line 3370 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 168: /* json_value: json_object  */
#line 874 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                        { 

}
#line 3378 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 169: /* json_value: json_array  */
#line 878 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                       { 

}
#line 3386 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 170: /* json_value: json_string  */
#line 882 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                        { 

}
#line 3394 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 171: /* json_value: YYTOKEN_NUMBER_INT  */
#line 886 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                               { 

}
#line 3402 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 172: /* json_value: YYTOKEN_NUMBER_FLOAT  */
#line 890 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                 { 

}
#line 3410 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 173: /* json_value: json_boolean  */
#line 894 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                         { 

}
#line 3418 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 174: /* json_value: YYTOKEN_NULL  */
#line 898 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                         { 

}
#line 3426 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 175: /* json_object: YYTOKEN_LEFTBRACE json_members YYTOKEN_RIGHTBRACE  */
#line 902 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                               { 

}
#line 3434 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 176: /* json_object: YYTOKEN_LEFTBRACE YYTOKEN_RIGHTBRACE  */
#line 906 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                  { 

}
#line 3442 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 177: /* json_members: json_members YYTOKEN_COMMA json_member  */
#line 910 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                     { 

}
#line 3450 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 178: /* json_members: json_member  */
#line 914 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                          { 

}
#line 3458 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 179: /* json_member: YYTOKEN_STRING YYTOKEN_COLON json_value  */
#line 918 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                     { 

}
#line 3466 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 180: /* json_array: YYTOKEN_LEFTBRACKET json_elements YYTOKEN_RIGHTBRACKET  */
#line 922 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                   { 

}
#line 3474 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 181: /* json_array: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 926 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                     { 

}
#line 3482 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 182: /* json_elements: json_elements YYTOKEN_COMMA json_element  */
#line 930 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                        { 

}
#line 3490 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 183: /* json_elements: json_element  */
#line 934 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                            { 

}
#line 3498 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 184: /* json_element: json_value  */
#line 938 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                         {
	
}
#line 3506 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;


#line 3510 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken, &yylloc};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (&yylloc, user_defined, yyscanner, yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          goto yyexhaustedlab;
      }
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc, user_defined, yyscanner);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp, user_defined, yyscanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if 1
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, user_defined, yyscanner, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;
#endif


/*-------------------------------------------------------.
| yyreturn -- parsing is finished, clean up and return.  |
`-------------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, user_defined, yyscanner);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp, user_defined, yyscanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 942 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"

