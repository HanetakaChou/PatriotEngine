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
    YYTOKEN_SCALAR = 292,          /* YYTOKEN_SCALAR  */
    YYTOKEN_VEC2 = 293,            /* YYTOKEN_VEC2  */
    YYTOKEN_VEC3 = 294,            /* YYTOKEN_VEC3  */
    YYTOKEN_VEC4 = 295,            /* YYTOKEN_VEC4  */
    YYTOKEN_MAT2 = 296,            /* YYTOKEN_MAT2  */
    YYTOKEN_MAT3 = 297,            /* YYTOKEN_MAT3  */
    YYTOKEN_MAT4 = 298,            /* YYTOKEN_MAT4  */
    YYTOKEN_MAX = 299,             /* YYTOKEN_MAX  */
    YYTOKEN_MIN = 300,             /* YYTOKEN_MIN  */
    YYTOKEN_SPARSE = 301,          /* YYTOKEN_SPARSE  */
    YYTOKEN_TRUE = 302,            /* YYTOKEN_TRUE  */
    YYTOKEN_FALSE = 303,           /* YYTOKEN_FALSE  */
    YYTOKEN_NULL = 304,            /* YYTOKEN_NULL  */
    YYTOKEN_LEFTBRACE = 305,       /* YYTOKEN_LEFTBRACE  */
    YYTOKEN_RIGHTBRACE = 306,      /* YYTOKEN_RIGHTBRACE  */
    YYTOKEN_COLON = 307,           /* YYTOKEN_COLON  */
    YYTOKEN_LEFTBRACKET = 308,     /* YYTOKEN_LEFTBRACKET  */
    YYTOKEN_RIGHTBRACKET = 309,    /* YYTOKEN_RIGHTBRACKET  */
    YYTOKEN_COMMA = 310,           /* YYTOKEN_COMMA  */
    YYTOKEN_STRING = 311,          /* YYTOKEN_STRING  */
    YYTOKEN_NUMBER_INT = 312,      /* YYTOKEN_NUMBER_INT  */
    YYTOKEN_NUMBER_FLOAT = 313,    /* YYTOKEN_NUMBER_FLOAT  */
    PSEUDO_LEX_ERROR = 314         /* PSEUDO_LEX_ERROR  */
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
  YYSYMBOL_YYTOKEN_SCALAR = 37,            /* YYTOKEN_SCALAR  */
  YYSYMBOL_YYTOKEN_VEC2 = 38,              /* YYTOKEN_VEC2  */
  YYSYMBOL_YYTOKEN_VEC3 = 39,              /* YYTOKEN_VEC3  */
  YYSYMBOL_YYTOKEN_VEC4 = 40,              /* YYTOKEN_VEC4  */
  YYSYMBOL_YYTOKEN_MAT2 = 41,              /* YYTOKEN_MAT2  */
  YYSYMBOL_YYTOKEN_MAT3 = 42,              /* YYTOKEN_MAT3  */
  YYSYMBOL_YYTOKEN_MAT4 = 43,              /* YYTOKEN_MAT4  */
  YYSYMBOL_YYTOKEN_MAX = 44,               /* YYTOKEN_MAX  */
  YYSYMBOL_YYTOKEN_MIN = 45,               /* YYTOKEN_MIN  */
  YYSYMBOL_YYTOKEN_SPARSE = 46,            /* YYTOKEN_SPARSE  */
  YYSYMBOL_YYTOKEN_TRUE = 47,              /* YYTOKEN_TRUE  */
  YYSYMBOL_YYTOKEN_FALSE = 48,             /* YYTOKEN_FALSE  */
  YYSYMBOL_YYTOKEN_NULL = 49,              /* YYTOKEN_NULL  */
  YYSYMBOL_YYTOKEN_LEFTBRACE = 50,         /* YYTOKEN_LEFTBRACE  */
  YYSYMBOL_YYTOKEN_RIGHTBRACE = 51,        /* YYTOKEN_RIGHTBRACE  */
  YYSYMBOL_YYTOKEN_COLON = 52,             /* YYTOKEN_COLON  */
  YYSYMBOL_YYTOKEN_LEFTBRACKET = 53,       /* YYTOKEN_LEFTBRACKET  */
  YYSYMBOL_YYTOKEN_RIGHTBRACKET = 54,      /* YYTOKEN_RIGHTBRACKET  */
  YYSYMBOL_YYTOKEN_COMMA = 55,             /* YYTOKEN_COMMA  */
  YYSYMBOL_YYTOKEN_STRING = 56,            /* YYTOKEN_STRING  */
  YYSYMBOL_YYTOKEN_NUMBER_INT = 57,        /* YYTOKEN_NUMBER_INT  */
  YYSYMBOL_YYTOKEN_NUMBER_FLOAT = 58,      /* YYTOKEN_NUMBER_FLOAT  */
  YYSYMBOL_PSEUDO_LEX_ERROR = 59,          /* PSEUDO_LEX_ERROR  */
  YYSYMBOL_YYACCEPT = 60,                  /* $accept  */
  YYSYMBOL_gltf_object = 61,               /* gltf_object  */
  YYSYMBOL_gltf_members = 62,              /* gltf_members  */
  YYSYMBOL_gltf_member = 63,               /* gltf_member  */
  YYSYMBOL_asset_object = 64,              /* asset_object  */
  YYSYMBOL_asset_properties = 65,          /* asset_properties  */
  YYSYMBOL_scenes_array = 66,              /* scenes_array  */
  YYSYMBOL_scene_objects = 67,             /* scene_objects  */
  YYSYMBOL_scene_object = 68,              /* scene_object  */
  YYSYMBOL_scene_properties = 69,          /* scene_properties  */
  YYSYMBOL_scene_property_nodes = 70,      /* scene_property_nodes  */
  YYSYMBOL_scene_property_nodes_elements = 71, /* scene_property_nodes_elements  */
  YYSYMBOL_nodes_array = 72,               /* nodes_array  */
  YYSYMBOL_node_objects = 73,              /* node_objects  */
  YYSYMBOL_node_object = 74,               /* node_object  */
  YYSYMBOL_node_properties = 75,           /* node_properties  */
  YYSYMBOL_node_property_children = 76,    /* node_property_children  */
  YYSYMBOL_node_property_children_elements = 77, /* node_property_children_elements  */
  YYSYMBOL_node_property_matrix = 78,      /* node_property_matrix  */
  YYSYMBOL_node_property_matrix_element = 79, /* node_property_matrix_element  */
  YYSYMBOL_node_property_rotation = 80,    /* node_property_rotation  */
  YYSYMBOL_node_property_rotation_element = 81, /* node_property_rotation_element  */
  YYSYMBOL_node_property_scale = 82,       /* node_property_scale  */
  YYSYMBOL_node_property_scale_element = 83, /* node_property_scale_element  */
  YYSYMBOL_node_property_translation = 84, /* node_property_translation  */
  YYSYMBOL_node_property_translation_element = 85, /* node_property_translation_element  */
  YYSYMBOL_node_property_weights = 86,     /* node_property_weights  */
  YYSYMBOL_node_property_weights_elements = 87, /* node_property_weights_elements  */
  YYSYMBOL_node_property_weights_element = 88, /* node_property_weights_element  */
  YYSYMBOL_buffers_array = 89,             /* buffers_array  */
  YYSYMBOL_buffer_objects = 90,            /* buffer_objects  */
  YYSYMBOL_buffer_object = 91,             /* buffer_object  */
  YYSYMBOL_buffer_properties = 92,         /* buffer_properties  */
  YYSYMBOL_bufferviews_array = 93,         /* bufferviews_array  */
  YYSYMBOL_bufferview_objects = 94,        /* bufferview_objects  */
  YYSYMBOL_bufferview_object = 95,         /* bufferview_object  */
  YYSYMBOL_bufferview_properties = 96,     /* bufferview_properties  */
  YYSYMBOL_accessors_array = 97,           /* accessors_array  */
  YYSYMBOL_accessor_objects = 98,          /* accessor_objects  */
  YYSYMBOL_accessor_object = 99,           /* accessor_object  */
  YYSYMBOL_accessor_properties = 100,      /* accessor_properties  */
  YYSYMBOL_accessor_property_type = 101,   /* accessor_property_type  */
  YYSYMBOL_accessor_property_max = 102,    /* accessor_property_max  */
  YYSYMBOL_accessor_property_max_element = 103, /* accessor_property_max_element  */
  YYSYMBOL_accessor_property_min = 104,    /* accessor_property_min  */
  YYSYMBOL_accessor_property_min_element = 105, /* accessor_property_min_element  */
  YYSYMBOL_json_string = 106,              /* json_string  */
  YYSYMBOL_json_boolean = 107,             /* json_boolean  */
  YYSYMBOL_json_value = 108,               /* json_value  */
  YYSYMBOL_json_object = 109,              /* json_object  */
  YYSYMBOL_json_members = 110,             /* json_members  */
  YYSYMBOL_json_member = 111,              /* json_member  */
  YYSYMBOL_json_array = 112,               /* json_array  */
  YYSYMBOL_json_elements = 113,            /* json_elements  */
  YYSYMBOL_json_element = 114              /* json_element  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 13 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"

    int yylex(union YYSTYPE *lvalp, YYLTYPE *llocp, void *user_defined, void *yyscanner);
    void yyerror(YYLTYPE *llocp, void *user_defined, void *yyscanner, const char *msg);

#line 332 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"

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
#define YYLAST   571

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  60
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  55
/* YYNRULES -- Number of rules.  */
#define YYNRULES  203
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  526

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   314


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
      55,    56,    57,    58,    59
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   143,   143,   147,   154,   162,   166,   170,   174,   178,
     182,   186,   190,   194,   198,   202,   206,   210,   214,   218,
     222,   226,   230,   234,   238,   242,   246,   250,   254,   258,
     263,   268,   272,   278,   284,   288,   292,   296,   300,   305,
     309,   313,   317,   321,   325,   329,   333,   338,   342,   346,
     350,   354,   358,   362,   367,   372,   377,   383,   388,   393,
     398,   403,   408,   413,   419,   425,   429,   433,   437,   442,
     461,   465,   469,   476,   480,   484,   490,   494,   498,   504,
     508,   512,   516,   520,   524,   530,   534,   539,   543,   547,
     551,   555,   559,   563,   567,   573,   578,   584,   588,   592,
     596,   600,   604,   608,   612,   616,   620,   624,   628,   633,
     638,   643,   648,   653,   657,   661,   665,   669,   673,   677,
     681,   685,   689,   693,   697,   701,   705,   709,   713,   718,
     723,   728,   733,   738,   743,   748,   753,   758,   762,   768,
     772,   776,   780,   784,   788,   792,   796,   815,   834,   853,
     872,   891,   910,   914,   918,   937,   956,   975,   994,  1013,
    1032,  1036,  1040,  1044,  1048,  1052,  1056,  1060,  1064,  1068,
    1072,  1076,  1080,  1084,  1088,  1092,  1096,  1100,  1104,  1108,
    1112,  1116,  1120,  1124,  1128,  1132,  1136,  1140,  1144,  1148,
    1152,  1156,  1160,  1164,  1168,  1172,  1176,  1180,  1184,  1188,
    1192,  1196,  1200,  1204
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
  "YYTOKEN_TYPE", "YYTOKEN_SCALAR", "YYTOKEN_VEC2", "YYTOKEN_VEC3",
  "YYTOKEN_VEC4", "YYTOKEN_MAT2", "YYTOKEN_MAT3", "YYTOKEN_MAT4",
  "YYTOKEN_MAX", "YYTOKEN_MIN", "YYTOKEN_SPARSE", "YYTOKEN_TRUE",
  "YYTOKEN_FALSE", "YYTOKEN_NULL", "YYTOKEN_LEFTBRACE",
  "YYTOKEN_RIGHTBRACE", "YYTOKEN_COLON", "YYTOKEN_LEFTBRACKET",
  "YYTOKEN_RIGHTBRACKET", "YYTOKEN_COMMA", "YYTOKEN_STRING",
  "YYTOKEN_NUMBER_INT", "YYTOKEN_NUMBER_FLOAT", "PSEUDO_LEX_ERROR",
  "$accept", "gltf_object", "gltf_members", "gltf_member", "asset_object",
  "asset_properties", "scenes_array", "scene_objects", "scene_object",
  "scene_properties", "scene_property_nodes",
  "scene_property_nodes_elements", "nodes_array", "node_objects",
  "node_object", "node_properties", "node_property_children",
  "node_property_children_elements", "node_property_matrix",
  "node_property_matrix_element", "node_property_rotation",
  "node_property_rotation_element", "node_property_scale",
  "node_property_scale_element", "node_property_translation",
  "node_property_translation_element", "node_property_weights",
  "node_property_weights_elements", "node_property_weights_element",
  "buffers_array", "buffer_objects", "buffer_object", "buffer_properties",
  "bufferviews_array", "bufferview_objects", "bufferview_object",
  "bufferview_properties", "accessors_array", "accessor_objects",
  "accessor_object", "accessor_properties", "accessor_property_type",
  "accessor_property_max", "accessor_property_max_element",
  "accessor_property_min", "accessor_property_min_element", "json_string",
  "json_boolean", "json_value", "json_object", "json_members",
  "json_member", "json_array", "json_elements", "json_element", YY_NULLPTR
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
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314
};
#endif

#define YYPACT_NINF (-391)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -33,   249,    48,     4,    12,    29,    31,    38,    47,    50,
      46,  -391,  -391,    21,    51,    52,    71,    76,    96,   106,
    -391,   249,   221,  -391,  -391,    61,  -391,    63,  -391,    64,
    -391,    80,  -391,    85,  -391,  -391,   109,   121,   127,   140,
     148,    72,    24,  -391,    19,  -391,    25,  -391,    86,  -391,
      26,  -391,    93,  -391,   194,  -391,   117,  -391,   132,  -391,
     177,  -391,   351,   351,   351,   351,   295,  -391,   324,   167,
     189,   199,  -391,    91,  -391,   203,   217,   240,   244,   253,
     280,   286,   289,   309,   347,   348,  -391,   107,  -391,   352,
     349,   353,  -391,   119,  -391,   354,   356,   357,   358,   359,
     360,  -391,   129,  -391,   363,   362,   364,   365,   366,   367,
     368,   369,   370,   371,  -391,   131,  -391,   374,  -391,  -391,
    -391,  -391,  -391,  -391,  -391,  -391,  -391,  -391,  -391,  -391,
    -391,  -391,  -391,  -391,  -391,  -391,  -391,  -391,  -391,  -391,
    -391,  -391,  -391,  -391,  -391,  -391,  -391,  -391,     7,   190,
    -391,  -391,  -391,  -391,  -391,  -391,  -391,   373,   375,   376,
     377,   378,   295,   164,   351,  -391,    73,  -391,   351,   214,
     350,   379,   380,   381,   382,   384,   386,   387,  -391,   269,
    -391,   385,   351,  -391,   209,  -391,   388,   389,   390,   391,
     392,  -391,   161,  -391,   351,   393,   394,    13,   395,   284,
     400,   401,   295,  -391,   222,  -391,  -391,   403,   165,  -391,
    -391,  -391,   208,  -391,   351,   351,   351,   351,   295,  -391,
       0,  -391,  -391,   404,   405,   406,  -391,  -391,     8,  -391,
    -391,   207,  -391,  -391,   216,  -391,   219,  -391,   237,  -391,
      35,  -391,   407,   408,   409,   410,   411,   412,   413,   414,
     415,   416,  -391,  -391,   417,   418,  -391,  -391,  -391,  -391,
    -391,   419,   420,   421,   422,   423,  -391,  -391,  -391,  -391,
    -391,  -391,  -391,  -391,  -391,  -391,  -391,  -391,  -391,  -391,
     277,  -391,   279,  -391,  -391,   424,   425,   426,   427,   428,
     429,   430,   431,   432,   433,   295,  -391,   351,  -391,   295,
    -391,  -391,  -391,  -391,  -391,  -391,  -391,   224,   295,   164,
     351,  -391,  -391,   285,  -391,  -391,   434,  -391,  -391,   435,
    -391,  -391,   436,  -391,  -391,   437,  -391,  -391,  -391,   292,
    -391,   351,   438,   350,   439,   380,   440,   382,   384,   386,
     387,   441,   351,   442,   443,   444,   445,   446,  -391,  -391,
     323,  -391,  -391,   325,   351,   447,   448,   449,   302,   450,
     284,   400,   401,   295,  -391,  -391,  -391,  -391,   451,  -391,
    -391,  -391,  -391,   452,   207,   216,   219,   237,  -391,   326,
    -391,  -391,  -391,  -391,  -391,  -391,  -391,  -391,  -391,  -391,
    -391,  -391,  -391,  -391,  -391,  -391,  -391,  -391,   277,  -391,
     279,  -391,  -391,  -391,  -391,  -391,  -391,  -391,  -391,  -391,
    -391,  -391,  -391,   455,   456,   457,   458,  -391,   327,   331,
     207,   216,   219,   237,  -391,   277,  -391,   279,   459,   460,
     361,   372,   333,   335,   207,   216,  -391,  -391,  -391,   277,
    -391,   279,   461,   463,   337,   339,   207,  -391,  -391,   277,
    -391,   279,   464,   465,   466,   207,   277,   279,   467,   468,
     469,   207,   277,   279,   470,   471,   472,   207,   277,   279,
     473,   474,   475,   207,   277,   279,   476,   341,   343,   207,
    -391,   277,  -391,   279,   477,   478,   479,   207,   277,   279,
     480,   481,   482,   207,   277,   279,   483,   484,   485,   207,
     277,   279,   486,   487,   488,   207,   277,   279,   489,   490,
     491,   207,   277,   279,   492,   493,   494,   207,   277,   279,
     496,   497,   498,  -391,  -391,  -391
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
       0,   102,     0,    97,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   118,     0,   113,     0,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     162,    18,    19,    20,    21,   185,   186,   193,     0,     0,
     190,   191,   189,   192,    22,   187,   188,     0,     0,     0,
       0,     0,     0,     0,     0,    27,     0,    25,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    43,     0,
      41,     0,     0,    91,     0,    89,     0,     0,     0,     0,
       0,   101,     0,    99,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   117,     0,   115,   195,     0,     0,   197,
     200,   203,     0,   202,     0,     0,     0,     0,     0,    34,
       0,    32,    33,     0,     0,     0,    64,    55,     0,    56,
      57,     0,    58,    59,     0,    60,     0,    61,     0,    62,
       0,    63,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    95,    96,     0,     0,   110,   108,   109,   111,
     112,     0,     0,     0,     0,     0,   138,   130,   129,   131,
     132,   133,   139,   140,   141,   142,   143,   144,   145,   134,
       0,   135,     0,   136,   137,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   194,     0,   199,     0,
      13,    14,    15,    16,    17,    36,    38,     0,     0,     0,
       0,    66,    68,     0,    71,    70,     0,    74,    73,     0,
      77,    76,     0,    80,    79,     0,    82,    86,    85,     0,
      84,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   153,   152,
       0,   161,   160,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   198,   196,   201,    35,     0,    31,
      29,    30,    65,     0,     0,     0,     0,     0,    81,     0,
      54,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      93,    94,   105,   103,   104,   106,   107,   151,     0,   159,
       0,   128,   120,   119,   121,   122,   123,   124,   125,   126,
     127,    37,    67,     0,     0,     0,     0,    83,     0,     0,
       0,     0,     0,     0,   150,     0,   158,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    75,    78,   149,     0,
     157,     0,     0,     0,     0,     0,     0,    72,   148,     0,
     156,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     147,     0,   155,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    69,   146,   154
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -391,  -391,  -391,   532,  -391,  -391,  -391,  -391,   495,  -391,
      97,  -391,  -391,  -391,   342,  -391,    99,  -391,   108,  -367,
     104,  -366,   149,  -370,    95,  -343,   146,  -391,    65,  -391,
    -391,   398,  -391,  -391,  -391,   453,  -391,  -391,  -391,   454,
    -391,   128,   133,  -390,   156,  -332,   -61,  -192,   -66,  -391,
    -391,   257,  -391,  -391,   256
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     2,    10,    11,    23,    41,    26,    44,    45,    73,
     221,   307,    28,    48,    49,    87,   229,   313,   232,   316,
     235,   319,   237,   322,   239,   325,   241,   329,   330,    30,
      52,    53,    93,    32,    56,    57,   102,    34,    60,    61,
     115,   279,   281,   350,   283,   353,   152,   153,   211,   155,
     208,   209,   156,   212,   213
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     154,   141,   142,   143,   144,   270,   415,   413,   418,   414,
     118,   119,   120,   121,   122,   123,   124,     1,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,    69,   416,   432,    70,    71,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    12,   444,
      90,    91,   430,   428,   305,   429,    13,   306,   206,   453,
     145,   146,   311,   140,    14,   312,   459,   442,   419,   443,
     269,    22,   465,    74,    75,    72,    86,    92,   471,   452,
     431,    15,   223,    16,   477,   224,   225,   207,   458,   326,
      17,   485,   327,   328,   464,   433,   219,    20,   491,    18,
     470,    21,    19,   222,   497,    25,   476,   226,    24,   445,
     503,    42,   484,    46,    50,    43,   509,    47,    51,   454,
     490,   253,   515,    67,    27,   460,   496,    68,   521,    29,
      54,   466,   502,   266,    55,    58,   284,   472,   508,    59,
      88,    89,   165,   478,   514,   105,   166,    94,    95,    31,
     520,   486,   304,   300,   301,   302,   303,   492,   178,    33,
     106,    62,   179,   498,   107,   108,   405,   109,   110,   504,
     183,   103,   104,    63,   184,   510,   111,   112,   113,    64,
     191,   516,   203,   114,   192,   261,   204,   522,   262,   263,
     264,   265,    65,   118,   119,   120,   121,   122,   123,   124,
      66,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   296,   220,    96,   162,
     297,    97,    98,    99,   100,    36,    37,    38,    39,   364,
      40,   116,   117,   254,   255,   285,   207,   145,   146,   147,
     148,   163,   369,   149,   210,   101,   140,   150,   151,   371,
     286,   164,     3,    42,   287,   288,   289,   290,   291,     4,
       5,     6,   298,   299,   314,   315,   292,   293,   294,   168,
     380,   227,     7,   317,   318,     8,   320,   321,   367,   368,
       9,   391,   242,   243,   244,   245,   246,   247,   248,   249,
     250,   251,   169,   401,   323,   324,   170,   410,   118,   119,
     120,   121,   122,   123,   124,   171,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   272,   273,   274,   275,   276,   277,   278,   157,   158,
     159,   160,   172,   161,   348,   349,   351,   352,   173,   372,
     373,   174,   145,   146,   147,   148,   378,   379,   149,   145,
     146,   140,   150,   151,   118,   119,   120,   121,   122,   123,
     124,   175,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   139,   397,   398,   399,
     400,   424,   425,   327,   328,   426,   427,   438,   439,   440,
     441,   448,   449,   450,   451,   480,   481,   482,   483,   176,
     177,   181,    46,   228,    50,   182,   370,   140,   186,   187,
     188,   189,   190,    54,   194,   436,   195,   196,   197,   198,
     199,   200,   201,   202,    58,   214,   437,   215,   216,   217,
     218,   180,   382,   231,   388,   234,   230,   236,   233,   238,
     240,   386,   252,   384,   417,   256,   257,   258,   259,   260,
     267,   268,   271,   280,   282,   295,   308,   309,   310,   331,
     332,   333,   334,   335,   336,   337,   338,   339,   340,   341,
     342,   343,   344,   345,   346,   347,   354,   355,   356,   357,
     358,   359,   360,   361,   362,   363,   389,   387,   407,   374,
     375,   376,   377,   185,   408,   381,   383,   385,   390,   392,
     393,   394,   395,   396,   402,   403,   404,   406,   411,   412,
     420,   421,   422,   423,   434,   435,   446,   447,   409,   455,
     456,   457,   461,   462,   463,   467,   468,   469,   473,   474,
     475,   479,   487,   488,   489,   493,   494,   495,   499,   500,
     501,   505,   506,   507,   511,   512,   513,   517,   518,   519,
     523,   524,   525,    35,   365,   366,     0,   193,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     167,   205
};

static const yytype_int16 yycheck[] =
{
      66,    62,    63,    64,    65,   197,   376,   374,   398,   375,
       3,     4,     5,     6,     7,     8,     9,    50,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,     9,   377,   425,    12,    13,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,     0,   439,
      24,    25,   422,   420,    54,   421,    52,    57,    51,   449,
      47,    48,    54,    56,    52,    57,   456,   434,   400,   435,
      57,    50,   462,    54,    55,    51,    51,    51,   468,   446,
     423,    52,     9,    52,   474,    12,    13,   148,   455,    54,
      52,   481,    57,    58,   461,   427,   162,    51,   488,    52,
     467,    55,    52,   164,   494,    53,   473,   168,    57,   441,
     500,    50,   479,    50,    50,    54,   506,    54,    54,   451,
     487,   182,   512,    51,    53,   457,   493,    55,   518,    53,
      50,   463,   499,   194,    54,    50,   202,   469,   505,    54,
      54,    55,    51,   475,   511,    13,    55,    54,    55,    53,
     517,   483,   218,   214,   215,   216,   217,   489,    51,    53,
      28,    52,    55,   495,    32,    33,   358,    35,    36,   501,
      51,    54,    55,    52,    55,   507,    44,    45,    46,    52,
      51,   513,    51,    51,    55,    24,    55,   519,    27,    28,
      29,    30,    52,     3,     4,     5,     6,     7,     8,     9,
      52,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    51,    53,    24,    52,
      55,    27,    28,    29,    30,     4,     5,     6,     7,   295,
       9,    54,    55,    24,    25,    13,   297,    47,    48,    49,
      50,    52,   308,    53,    54,    51,    56,    57,    58,   310,
      28,    52,     3,    50,    32,    33,    34,    35,    36,    10,
      11,    12,    54,    55,    57,    58,    44,    45,    46,    52,
     331,    57,    23,    57,    58,    26,    57,    58,    54,    55,
      31,   342,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    52,   354,    57,    58,    52,   363,     3,     4,
       5,     6,     7,     8,     9,    52,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    37,    38,    39,    40,    41,    42,    43,     4,     5,
       6,     7,    52,     9,    57,    58,    57,    58,    52,    54,
      55,    52,    47,    48,    49,    50,    54,    55,    53,    47,
      48,    56,    57,    58,     3,     4,     5,     6,     7,     8,
       9,    52,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    54,    55,    54,
      55,    54,    55,    57,    58,    54,    55,    54,    55,    54,
      55,    54,    55,    54,    55,    54,    55,    54,    55,    52,
      52,    52,    50,    53,    50,    52,   309,    56,    52,    52,
      52,    52,    52,    50,    52,    54,    52,    52,    52,    52,
      52,    52,    52,    52,    50,    52,    54,    52,    52,    52,
      52,    89,   333,    53,   339,    53,    57,    53,    57,    53,
      53,   337,    57,   335,   379,    57,    57,    57,    57,    57,
      57,    57,    57,    53,    53,    52,    52,    52,    52,    52,
      52,    52,    52,    52,    52,    52,    52,    52,    52,    52,
      52,    52,    52,    52,    52,    52,    52,    52,    52,    52,
      52,    52,    52,    52,    52,    52,   340,   338,   360,    55,
      55,    55,    55,    95,   361,    57,    57,    57,    57,    57,
      57,    57,    57,    57,    57,    57,    57,    57,    57,    57,
      55,    55,    55,    55,    55,    55,    55,    54,   362,    55,
      55,    55,    55,    55,    55,    55,    55,    55,    55,    55,
      55,    55,    55,    55,    55,    55,    55,    55,    55,    55,
      55,    55,    55,    55,    55,    55,    55,    55,    55,    55,
      54,    54,    54,    21,   297,   299,    -1,   104,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      75,   117
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    50,    61,     3,    10,    11,    12,    23,    26,    31,
      62,    63,     0,    52,    52,    52,    52,    52,    52,    52,
      51,    55,    50,    64,    57,    53,    66,    53,    72,    53,
      89,    53,    93,    53,    97,    63,     4,     5,     6,     7,
       9,    65,    50,    54,    67,    68,    50,    54,    73,    74,
      50,    54,    90,    91,    50,    54,    94,    95,    50,    54,
      98,    99,    52,    52,    52,    52,    52,    51,    55,     9,
      12,    13,    51,    69,    54,    55,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    51,    75,    54,    55,
      24,    25,    51,    92,    54,    55,    24,    27,    28,    29,
      30,    51,    96,    54,    55,    13,    28,    32,    33,    35,
      36,    44,    45,    46,    51,   100,    54,    55,     3,     4,
       5,     6,     7,     8,     9,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      56,   106,   106,   106,   106,    47,    48,    49,    50,    53,
      57,    58,   106,   107,   108,   109,   112,     4,     5,     6,
       7,     9,    52,    52,    52,    51,    55,    68,    52,    52,
      52,    52,    52,    52,    52,    52,    52,    52,    51,    55,
      74,    52,    52,    51,    55,    91,    52,    52,    52,    52,
      52,    51,    55,    95,    52,    52,    52,    52,    52,    52,
      52,    52,    52,    51,    55,    99,    51,   106,   110,   111,
      54,   108,   113,   114,    52,    52,    52,    52,    52,   108,
      53,    70,   106,     9,    12,    13,   106,    57,    53,    76,
      57,    53,    78,    57,    53,    80,    53,    82,    53,    84,
      53,    86,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    57,   106,    24,    25,    57,    57,    57,    57,
      57,    24,    27,    28,    29,    30,   106,    57,    57,    57,
     107,    57,    37,    38,    39,    40,    41,    42,    43,   101,
      53,   102,    53,   104,   108,    13,    28,    32,    33,    34,
      35,    36,    44,    45,    46,    52,    51,    55,    54,    55,
     106,   106,   106,   106,   108,    54,    57,    71,    52,    52,
      52,    54,    57,    77,    57,    58,    79,    57,    58,    81,
      57,    58,    83,    57,    58,    85,    54,    57,    58,    87,
      88,    52,    52,    52,    52,    52,    52,    52,    52,    52,
      52,    52,    52,    52,    52,    52,    52,    52,    57,    58,
     103,    57,    58,   105,    52,    52,    52,    52,    52,    52,
      52,    52,    52,    52,   108,   111,   114,    54,    55,   108,
      70,   106,    54,    55,    55,    55,    55,    55,    54,    55,
     106,    57,    76,    57,    78,    57,    80,    82,    84,    86,
      57,   106,    57,    57,    57,    57,    57,    54,    55,    54,
      55,   106,    57,    57,    57,   107,    57,   101,   102,   104,
     108,    57,    57,    79,    81,    83,    85,    88,   103,   105,
      55,    55,    55,    55,    54,    55,    54,    55,    79,    81,
      83,    85,   103,   105,    55,    55,    54,    54,    54,    55,
      54,    55,    79,    81,   103,   105,    55,    54,    54,    55,
      54,    55,    79,   103,   105,    55,    55,    55,    79,   103,
     105,    55,    55,    55,    79,   103,   105,    55,    55,    55,
      79,   103,   105,    55,    55,    55,    79,   103,   105,    55,
      54,    55,    54,    55,    79,   103,   105,    55,    55,    55,
      79,   103,   105,    55,    55,    55,    79,   103,   105,    55,
      55,    55,    79,   103,   105,    55,    55,    55,    79,   103,
     105,    55,    55,    55,    79,   103,   105,    55,    55,    55,
      79,   103,   105,    54,    54,    54
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    60,    61,    62,    62,    63,    63,    63,    63,    63,
      63,    63,    64,    65,    65,    65,    65,    65,    65,    65,
      65,    65,    65,    66,    66,    67,    67,    68,    68,    69,
      69,    69,    69,    69,    69,    70,    70,    71,    71,    72,
      72,    73,    73,    74,    74,    75,    75,    75,    75,    75,
      75,    75,    75,    75,    75,    75,    75,    75,    75,    75,
      75,    75,    75,    75,    75,    76,    76,    77,    77,    78,
      79,    79,    80,    81,    81,    82,    83,    83,    84,    85,
      85,    86,    86,    87,    87,    88,    88,    89,    89,    90,
      90,    91,    91,    92,    92,    92,    92,    93,    93,    94,
      94,    95,    95,    96,    96,    96,    96,    96,    96,    96,
      96,    96,    96,    97,    97,    98,    98,    99,    99,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   101,
     101,   101,   101,   101,   101,   101,   102,   102,   102,   102,
     102,   102,   103,   103,   104,   104,   104,   104,   104,   104,
     105,   105,   106,   106,   106,   106,   106,   106,   106,   106,
     106,   106,   106,   106,   106,   106,   106,   106,   106,   106,
     106,   106,   106,   106,   106,   107,   107,   108,   108,   108,
     108,   108,   108,   108,   109,   109,   110,   110,   111,   112,
     112,   113,   113,   114
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
       5,     5,     5,     5,     5,     5,     5,     5,     5,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     1,
       1,     1,     1,     1,     1,     1,    33,    19,     9,     7,
       5,     3,     1,     1,    33,    19,     9,     7,     5,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     2,     3,     1,     3,     3,
       2,     3,     1,     1
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
#line 143 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                { 
	//$$ = $2;
}
#line 2048 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 3: /* gltf_members: gltf_members YYTOKEN_COMMA gltf_member  */
#line 147 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                     { 
	//bool _res_addmember = JsonParser_Json_Object_AddMember(pUserData, $1, $3._stdstring, $3._jsonvalue); 
	//JsonParser_Std_String_Dispose(pUserData, $3._stdstring);
	//if(!_res_addmember) { yyerror(&yylloc, pUserData, pScanner, "Duplicate object key"); }
	//$$ = $1;
}
#line 2059 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 4: /* gltf_members: gltf_member  */
#line 154 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                          { 
    //void *_jsonobject = JsonParser_Json_Object_Create(pUserData);
	//bool _res_addmember = JsonParser_Json_Object_AddMember(pUserData, _jsonobject, $1._stdstring, $1._jsonvalue);
	//JsonParser_Std_String_Dispose(pUserData, $1._stdstring);
	//if(!_res_addmember) { yyerror(&yylloc, pUserData, pScanner, "Duplicate object key"); }
	//$$ = _jsonobject;
}
#line 2071 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 5: /* gltf_member: YYTOKEN_ASSET YYTOKEN_COLON asset_object  */
#line 162 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                      {
	//$$ = $3;
}
#line 2079 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 6: /* gltf_member: YYTOKEN_SCENE YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 166 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                            {
	gltf_yacc_gltf_set_scene_index_callback((yyvsp[0].m_token_numberint), user_defined);
}
#line 2087 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 7: /* gltf_member: YYTOKEN_SCENES YYTOKEN_COLON scenes_array  */
#line 170 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                       {

}
#line 2095 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 8: /* gltf_member: YYTOKEN_NODES YYTOKEN_COLON nodes_array  */
#line 174 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                     {

}
#line 2103 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 9: /* gltf_member: YYTOKEN_BUFFERS YYTOKEN_COLON buffers_array  */
#line 178 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                         {

}
#line 2111 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 10: /* gltf_member: YYTOKEN_BUFFERVIEWS YYTOKEN_COLON bufferviews_array  */
#line 182 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                 {

}
#line 2119 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 11: /* gltf_member: YYTOKEN_ACCESSORS YYTOKEN_COLON accessors_array  */
#line 186 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                             {

}
#line 2127 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 12: /* asset_object: YYTOKEN_LEFTBRACE asset_properties YYTOKEN_RIGHTBRACE  */
#line 190 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                     { 
	
}
#line 2135 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 13: /* asset_properties: asset_properties YYTOKEN_COMMA YYTOKEN_COPYRIGHT YYTOKEN_COLON json_string  */
#line 194 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                             { 
	gltf_lex_yacc_temp_string_destroy_callback((yyvsp[0].m_temp_string_version), user_defined);
}
#line 2143 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 14: /* asset_properties: asset_properties YYTOKEN_COMMA YYTOKEN_GENERATOR YYTOKEN_COLON json_string  */
#line 198 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                             { 
	gltf_lex_yacc_temp_string_destroy_callback((yyvsp[0].m_temp_string_version), user_defined);
}
#line 2151 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 15: /* asset_properties: asset_properties YYTOKEN_COMMA YYTOKEN_VERSION YYTOKEN_COLON json_string  */
#line 202 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                           { 
	gltf_lex_yacc_temp_string_destroy_callback((yyvsp[0].m_temp_string_version), user_defined);
}
#line 2159 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 16: /* asset_properties: asset_properties YYTOKEN_COMMA YYTOKEN_MINVERSION YYTOKEN_COLON json_string  */
#line 206 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                              { 
	gltf_lex_yacc_temp_string_destroy_callback((yyvsp[0].m_temp_string_version), user_defined);
}
#line 2167 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 17: /* asset_properties: asset_properties YYTOKEN_COMMA YYTOKEN_EXTRAS YYTOKEN_COLON json_value  */
#line 210 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                         { 

}
#line 2175 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 18: /* asset_properties: YYTOKEN_COPYRIGHT YYTOKEN_COLON json_string  */
#line 214 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                              { 
	gltf_lex_yacc_temp_string_destroy_callback((yyvsp[0].m_temp_string_version), user_defined);
}
#line 2183 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 19: /* asset_properties: YYTOKEN_GENERATOR YYTOKEN_COLON json_string  */
#line 218 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                              { 
	gltf_lex_yacc_temp_string_destroy_callback((yyvsp[0].m_temp_string_version), user_defined);
}
#line 2191 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 20: /* asset_properties: YYTOKEN_VERSION YYTOKEN_COLON json_string  */
#line 222 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                            { 
	gltf_lex_yacc_temp_string_destroy_callback((yyvsp[0].m_temp_string_version), user_defined);
}
#line 2199 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 21: /* asset_properties: YYTOKEN_MINVERSION YYTOKEN_COLON json_string  */
#line 226 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                               { 
	gltf_lex_yacc_temp_string_destroy_callback((yyvsp[0].m_temp_string_version), user_defined);
}
#line 2207 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 22: /* asset_properties: YYTOKEN_EXTRAS YYTOKEN_COLON json_value  */
#line 230 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                          { 

}
#line 2215 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 23: /* scenes_array: YYTOKEN_LEFTBRACKET scene_objects YYTOKEN_RIGHTBRACKET  */
#line 234 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                     { 

}
#line 2223 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 24: /* scenes_array: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 238 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                       {

}
#line 2231 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 25: /* scene_objects: scene_objects YYTOKEN_COMMA scene_object  */
#line 242 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                        { 
	assert(-1 == (yyvsp[0].m_scene_index) || gltf_yacc_scene_size_callback(user_defined) == ((yyvsp[0].m_scene_index) + 1));
}
#line 2239 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 26: /* scene_objects: scene_object  */
#line 246 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                            {
	assert(-1 == (yyvsp[0].m_scene_index) || gltf_yacc_scene_size_callback(user_defined) == ((yyvsp[0].m_scene_index) + 1));
}
#line 2247 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 27: /* scene_object: YYTOKEN_LEFTBRACE scene_properties YYTOKEN_RIGHTBRACE  */
#line 250 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                    {
	(yyval.m_scene_index) = (yyvsp[-1].m_scene_index);
}
#line 2255 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 28: /* scene_object: YYTOKEN_LEFTBRACE YYTOKEN_RIGHTBRACE  */
#line 254 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                   { 
	(yyval.m_scene_index) = -1;
}
#line 2263 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 29: /* scene_properties: scene_properties YYTOKEN_COMMA YYTOKEN_NODES YYTOKEN_COLON scene_property_nodes  */
#line 258 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                  {
	gltf_yacc_scene_set_nodes_callback((yyval.m_scene_index), (yyvsp[0].m_temp_int_array_version), user_defined);
	gltf_yacc_temp_int_array_destroy_callback((yyvsp[0].m_temp_int_array_version), user_defined);
}
#line 2272 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 30: /* scene_properties: scene_properties YYTOKEN_COMMA YYTOKEN_NAME YYTOKEN_COLON json_string  */
#line 263 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                        {
	gltf_yacc_scene_set_name_callback((yyval.m_scene_index), (yyvsp[0].m_temp_string_version), user_defined);
	gltf_lex_yacc_temp_string_destroy_callback((yyvsp[0].m_temp_string_version), user_defined);
}
#line 2281 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 31: /* scene_properties: scene_properties YYTOKEN_COMMA YYTOKEN_EXTRAS YYTOKEN_COLON json_value  */
#line 268 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                         { 
	// ignore
}
#line 2289 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 32: /* scene_properties: YYTOKEN_NODES YYTOKEN_COLON scene_property_nodes  */
#line 272 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                   { 
	(yyval.m_scene_index) = gltf_yacc_scene_push_callback(user_defined);
	gltf_yacc_scene_set_nodes_callback((yyval.m_scene_index), (yyvsp[0].m_temp_int_array_version), user_defined);
	gltf_yacc_temp_int_array_destroy_callback((yyvsp[0].m_temp_int_array_version), user_defined);
}
#line 2299 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 33: /* scene_properties: YYTOKEN_NAME YYTOKEN_COLON json_string  */
#line 278 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                         {
	(yyval.m_scene_index) = gltf_yacc_scene_push_callback(user_defined);
	gltf_yacc_scene_set_name_callback((yyval.m_scene_index), (yyvsp[0].m_temp_string_version), user_defined);
	gltf_lex_yacc_temp_string_destroy_callback((yyvsp[0].m_temp_string_version), user_defined);
}
#line 2309 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 34: /* scene_properties: YYTOKEN_EXTRAS YYTOKEN_COLON json_value  */
#line 284 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                          { 

}
#line 2317 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 35: /* scene_property_nodes: YYTOKEN_LEFTBRACKET scene_property_nodes_elements YYTOKEN_RIGHTBRACKET  */
#line 288 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                             {
	(yyval.m_temp_int_array_version) = (yyvsp[-1].m_temp_int_array_version);
}
#line 2325 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 36: /* scene_property_nodes: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 292 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                               { 
	(yyval.m_temp_int_array_version) = gltf_yacc_temp_int_array_init_callback(user_defined);
}
#line 2333 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 37: /* scene_property_nodes_elements: scene_property_nodes_elements YYTOKEN_COMMA YYTOKEN_NUMBER_INT  */
#line 296 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                              {
	gltf_yacc_temp_int_array_push_callback((yyval.m_temp_int_array_version), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2341 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 38: /* scene_property_nodes_elements: YYTOKEN_NUMBER_INT  */
#line 300 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                   {
	(yyval.m_temp_int_array_version) = gltf_yacc_temp_int_array_init_callback(user_defined);
	gltf_yacc_temp_int_array_push_callback((yyval.m_temp_int_array_version), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2350 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 39: /* nodes_array: YYTOKEN_LEFTBRACKET node_objects YYTOKEN_RIGHTBRACKET  */
#line 305 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                   {

}
#line 2358 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 40: /* nodes_array: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 309 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                      {

}
#line 2366 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 41: /* node_objects: node_objects YYTOKEN_COMMA node_object  */
#line 313 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                     { 
   	assert(-1 == (yyvsp[0].m_node_index) || gltf_yacc_node_size_callback(user_defined) == ((yyvsp[0].m_node_index) + 1));
}
#line 2374 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 42: /* node_objects: node_object  */
#line 317 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                          { 
	assert(-1 == (yyvsp[0].m_node_index) || gltf_yacc_node_size_callback(user_defined) == ((yyvsp[0].m_node_index) + 1));
}
#line 2382 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 43: /* node_object: YYTOKEN_LEFTBRACE node_properties YYTOKEN_RIGHTBRACE  */
#line 321 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                  {
	(yyval.m_node_index) = (yyvsp[-1].m_node_index);
}
#line 2390 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 44: /* node_object: YYTOKEN_LEFTBRACE YYTOKEN_RIGHTBRACE  */
#line 325 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                  { 
	(yyval.m_node_index) = -1;
}
#line 2398 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 45: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_CAMERA YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 329 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                               {
	gltf_yacc_node_set_camera_callback((yyval.m_node_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2406 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 46: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_CHILDREN YYTOKEN_COLON node_property_children  */
#line 333 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                     {
	gltf_yacc_node_set_children_callback((yyval.m_node_index), (yyvsp[0].m_temp_int_array_version), user_defined);
	gltf_yacc_temp_int_array_destroy_callback((yyvsp[0].m_temp_int_array_version), user_defined);
}
#line 2415 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 47: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_SKIN YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 338 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                             {
	gltf_yacc_node_set_skin_callback((yyval.m_node_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2423 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 48: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_MATRIX YYTOKEN_COLON node_property_matrix  */
#line 342 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                 {
	gltf_yacc_node_set_matrix_callback((yyval.m_node_index), (yyvsp[0].m_mat4x4), user_defined);
}
#line 2431 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 49: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_MESH YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 346 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                             {
	gltf_yacc_node_set_mesh_callback((yyval.m_node_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2439 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 50: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_ROTATION YYTOKEN_COLON node_property_rotation  */
#line 350 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                     {
	gltf_yacc_node_set_rotation_callback((yyval.m_node_index), (yyvsp[0].m_vec4), user_defined);
}
#line 2447 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 51: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_SCALE YYTOKEN_COLON node_property_scale  */
#line 354 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                               {
	gltf_yacc_node_set_scale_callback((yyval.m_node_index), (yyvsp[0].m_vec3), user_defined);
}
#line 2455 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 52: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_TRANSLATION YYTOKEN_COLON node_property_translation  */
#line 358 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                           {
	gltf_yacc_node_set_translation_callback((yyval.m_node_index), (yyvsp[0].m_vec3), user_defined);
}
#line 2463 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 53: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_WEIGHTS YYTOKEN_COLON node_property_weights  */
#line 362 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                   {
	gltf_yacc_node_set_weights_callback((yyval.m_node_index), (yyvsp[0].m_temp_float_array_version), user_defined);
	gltf_yacc_temp_float_array_destroy_callback((yyvsp[0].m_temp_float_array_version), user_defined);
}
#line 2472 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 54: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_NAME YYTOKEN_COLON json_string  */
#line 367 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                      {
	gltf_yacc_node_set_name_callback((yyval.m_node_index), (yyvsp[0].m_temp_string_version), user_defined);
	gltf_lex_yacc_temp_string_destroy_callback((yyvsp[0].m_temp_string_version), user_defined);
}
#line 2481 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 55: /* node_properties: YYTOKEN_CAMERA YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 372 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                 {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_camera_callback((yyval.m_node_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2490 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 56: /* node_properties: YYTOKEN_CHILDREN YYTOKEN_COLON node_property_children  */
#line 377 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                       {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_children_callback((yyval.m_node_index), (yyvsp[0].m_temp_int_array_version), user_defined);
	gltf_yacc_temp_int_array_destroy_callback((yyvsp[0].m_temp_int_array_version), user_defined);
}
#line 2500 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 57: /* node_properties: YYTOKEN_SKIN YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 383 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                               {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_skin_callback((yyval.m_node_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2509 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 58: /* node_properties: YYTOKEN_MATRIX YYTOKEN_COLON node_property_matrix  */
#line 388 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                   {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_matrix_callback((yyval.m_node_index), (yyvsp[0].m_mat4x4), user_defined);
}
#line 2518 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 59: /* node_properties: YYTOKEN_MESH YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 393 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                               {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_mesh_callback((yyval.m_node_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2527 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 60: /* node_properties: YYTOKEN_ROTATION YYTOKEN_COLON node_property_rotation  */
#line 398 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                       {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_rotation_callback((yyval.m_node_index), (yyvsp[0].m_vec4), user_defined);
}
#line 2536 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 61: /* node_properties: YYTOKEN_SCALE YYTOKEN_COLON node_property_scale  */
#line 403 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                 {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_scale_callback((yyval.m_node_index), (yyvsp[0].m_vec3), user_defined);
}
#line 2545 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 62: /* node_properties: YYTOKEN_TRANSLATION YYTOKEN_COLON node_property_translation  */
#line 408 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                             {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_translation_callback((yyval.m_node_index), (yyvsp[0].m_vec3), user_defined);
}
#line 2554 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 63: /* node_properties: YYTOKEN_WEIGHTS YYTOKEN_COLON node_property_weights  */
#line 413 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                     {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_weights_callback((yyval.m_node_index), (yyvsp[0].m_temp_float_array_version), user_defined);
	gltf_yacc_temp_float_array_destroy_callback((yyvsp[0].m_temp_float_array_version), user_defined);
}
#line 2564 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 64: /* node_properties: YYTOKEN_NAME YYTOKEN_COLON json_string  */
#line 419 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                        {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_name_callback((yyval.m_node_index), (yyvsp[0].m_temp_string_version), user_defined);
	gltf_lex_yacc_temp_string_destroy_callback((yyvsp[0].m_temp_string_version), user_defined);
}
#line 2574 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 65: /* node_property_children: YYTOKEN_LEFTBRACKET node_property_children_elements YYTOKEN_RIGHTBRACKET  */
#line 425 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                 {
	(yyval.m_temp_int_array_version) = (yyvsp[-1].m_temp_int_array_version);
}
#line 2582 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 66: /* node_property_children: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 429 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                 { 
    (yyval.m_temp_int_array_version) = gltf_yacc_temp_int_array_init_callback(user_defined);
}
#line 2590 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 67: /* node_property_children_elements: node_property_children_elements YYTOKEN_COMMA YYTOKEN_NUMBER_INT  */
#line 433 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                  {
	gltf_yacc_temp_int_array_push_callback((yyval.m_temp_int_array_version), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2598 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 68: /* node_property_children_elements: YYTOKEN_NUMBER_INT  */
#line 437 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                     {
	(yyval.m_temp_int_array_version) = gltf_yacc_temp_int_array_init_callback(user_defined);
	gltf_yacc_temp_int_array_push_callback((yyval.m_temp_int_array_version), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2607 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 69: /* node_property_matrix: YYTOKEN_LEFTBRACKET node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_RIGHTBRACKET  */
#line 442 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
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
#line 2630 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 70: /* node_property_matrix_element: YYTOKEN_NUMBER_FLOAT  */
#line 461 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                   {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberfloat);
}
#line 2638 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 71: /* node_property_matrix_element: YYTOKEN_NUMBER_INT  */
#line 465 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                 {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberint);
}
#line 2646 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 72: /* node_property_rotation: YYTOKEN_LEFTBRACKET node_property_rotation_element YYTOKEN_COMMA node_property_rotation_element YYTOKEN_COMMA node_property_rotation_element YYTOKEN_COMMA node_property_rotation_element YYTOKEN_RIGHTBRACKET  */
#line 469 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                                                                                                                                                       {
	(yyval.m_vec4)[0] = (yyvsp[-7].m_token_numberfloat);
	(yyval.m_vec4)[1] = (yyvsp[-5].m_token_numberfloat);
	(yyval.m_vec4)[2] = (yyvsp[-3].m_token_numberfloat);
	(yyval.m_vec4)[3] = (yyvsp[-1].m_token_numberfloat);
}
#line 2657 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 73: /* node_property_rotation_element: YYTOKEN_NUMBER_FLOAT  */
#line 476 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                     {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberfloat);
}
#line 2665 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 74: /* node_property_rotation_element: YYTOKEN_NUMBER_INT  */
#line 480 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                   {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberint);
}
#line 2673 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 75: /* node_property_scale: YYTOKEN_LEFTBRACKET node_property_scale_element YYTOKEN_COMMA node_property_scale_element YYTOKEN_COMMA node_property_scale_element YYTOKEN_RIGHTBRACKET  */
#line 484 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                                                                                              {
	(yyval.m_vec3)[0] = (yyvsp[-5].m_token_numberfloat);
	(yyval.m_vec3)[1] = (yyvsp[-3].m_token_numberfloat);
	(yyval.m_vec3)[2] = (yyvsp[-1].m_token_numberfloat);
}
#line 2683 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 76: /* node_property_scale_element: YYTOKEN_NUMBER_FLOAT  */
#line 490 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                  {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberfloat);
}
#line 2691 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 77: /* node_property_scale_element: YYTOKEN_NUMBER_INT  */
#line 494 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberint);
}
#line 2699 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 78: /* node_property_translation: YYTOKEN_LEFTBRACKET node_property_translation_element YYTOKEN_COMMA node_property_translation_element YYTOKEN_COMMA node_property_translation_element YYTOKEN_RIGHTBRACKET  */
#line 498 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                                                                                                                      {
	(yyval.m_vec3)[0] = (yyvsp[-5].m_token_numberfloat);
	(yyval.m_vec3)[1] = (yyvsp[-3].m_token_numberfloat);
	(yyval.m_vec3)[2] = (yyvsp[-1].m_token_numberfloat);
}
#line 2709 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 79: /* node_property_translation_element: YYTOKEN_NUMBER_FLOAT  */
#line 504 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                        {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberfloat);
}
#line 2717 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 80: /* node_property_translation_element: YYTOKEN_NUMBER_INT  */
#line 508 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                      {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberint);
}
#line 2725 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 81: /* node_property_weights: YYTOKEN_LEFTBRACKET node_property_weights_elements YYTOKEN_RIGHTBRACKET  */
#line 512 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                               {
	(yyval.m_temp_float_array_version) = (yyvsp[-1].m_temp_float_array_version);
}
#line 2733 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 82: /* node_property_weights: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 516 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                { 
    (yyval.m_temp_float_array_version) = gltf_yacc_temp_float_array_init_callback(user_defined);
}
#line 2741 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 83: /* node_property_weights_elements: node_property_weights_elements YYTOKEN_COMMA node_property_weights_element  */
#line 520 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                           {
	gltf_yacc_temp_float_array_push_callback((yyval.m_temp_float_array_version), (yyvsp[0].m_token_numberfloat), user_defined);
}
#line 2749 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 84: /* node_property_weights_elements: node_property_weights_element  */
#line 524 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                               {
	(yyval.m_temp_float_array_version) = gltf_yacc_temp_float_array_init_callback(user_defined);
	gltf_yacc_temp_float_array_push_callback((yyval.m_temp_float_array_version), (yyvsp[0].m_token_numberfloat), user_defined);
}
#line 2758 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 85: /* node_property_weights_element: YYTOKEN_NUMBER_FLOAT  */
#line 530 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                    {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberfloat);
}
#line 2766 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 86: /* node_property_weights_element: YYTOKEN_NUMBER_INT  */
#line 534 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                  {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberint);
}
#line 2774 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 87: /* buffers_array: YYTOKEN_LEFTBRACKET buffer_objects YYTOKEN_RIGHTBRACKET  */
#line 539 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                       {

}
#line 2782 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 88: /* buffers_array: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 543 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                        {

}
#line 2790 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 89: /* buffer_objects: buffer_objects YYTOKEN_COMMA buffer_object  */
#line 547 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                           { 
   	assert(-1 == (yyvsp[0].m_buffer_index) || gltf_yacc_buffer_size_callback(user_defined) == ((yyvsp[0].m_buffer_index) + 1));
}
#line 2798 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 90: /* buffer_objects: buffer_object  */
#line 551 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                              { 
	assert(-1 == (yyvsp[0].m_buffer_index) || gltf_yacc_buffer_size_callback(user_defined) == ((yyvsp[0].m_buffer_index) + 1));
}
#line 2806 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 91: /* buffer_object: YYTOKEN_LEFTBRACE buffer_properties YYTOKEN_RIGHTBRACE  */
#line 555 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                      {
	(yyval.m_buffer_index) = (yyvsp[-1].m_buffer_index);
}
#line 2814 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 92: /* buffer_object: YYTOKEN_LEFTBRACE YYTOKEN_RIGHTBRACE  */
#line 559 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                    { 
	(yyval.m_buffer_index) = -1;
}
#line 2822 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 93: /* buffer_properties: buffer_properties YYTOKEN_COMMA YYTOKEN_BYTELENGTH YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 563 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                       {
	gltf_yacc_buffer_set_bytelength_callback((yyval.m_buffer_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2830 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 94: /* buffer_properties: buffer_properties YYTOKEN_COMMA YYTOKEN_URI YYTOKEN_COLON json_string  */
#line 567 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                         {
	gltf_yacc_buffer_set_url_callback((yyval.m_buffer_index), (yyvsp[0].m_temp_string_version), user_defined);
	gltf_lex_yacc_temp_string_destroy_callback((yyvsp[0].m_temp_string_version), user_defined);
}
#line 2839 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 95: /* buffer_properties: YYTOKEN_BYTELENGTH YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 573 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                       {
	(yyval.m_buffer_index) = gltf_yacc_buffer_push_callback(user_defined);
	gltf_yacc_buffer_set_bytelength_callback((yyval.m_buffer_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2848 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 96: /* buffer_properties: YYTOKEN_URI YYTOKEN_COLON json_string  */
#line 578 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                         {
	(yyval.m_buffer_index) = gltf_yacc_buffer_push_callback(user_defined);
	gltf_yacc_buffer_set_url_callback((yyval.m_buffer_index), (yyvsp[0].m_temp_string_version), user_defined);
	gltf_lex_yacc_temp_string_destroy_callback((yyvsp[0].m_temp_string_version), user_defined);
}
#line 2858 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 97: /* bufferviews_array: YYTOKEN_LEFTBRACKET bufferview_objects YYTOKEN_RIGHTBRACKET  */
#line 584 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                               {

}
#line 2866 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 98: /* bufferviews_array: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 588 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                            {

}
#line 2874 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 99: /* bufferview_objects: bufferview_objects YYTOKEN_COMMA bufferview_object  */
#line 592 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                       { 
   	assert(-1 == (yyvsp[0].m_bufferview_index) || gltf_yacc_bufferview_size_callback(user_defined) == ((yyvsp[0].m_bufferview_index) + 1));
}
#line 2882 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 100: /* bufferview_objects: bufferview_object  */
#line 596 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                      { 
	assert(-1 == (yyvsp[0].m_bufferview_index) || gltf_yacc_bufferview_size_callback(user_defined) == ((yyvsp[0].m_bufferview_index) + 1));
}
#line 2890 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 101: /* bufferview_object: YYTOKEN_LEFTBRACE bufferview_properties YYTOKEN_RIGHTBRACE  */
#line 600 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                              {
	(yyval.m_bufferview_index) = (yyvsp[-1].m_bufferview_index);
}
#line 2898 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 102: /* bufferview_object: YYTOKEN_LEFTBRACE YYTOKEN_RIGHTBRACE  */
#line 604 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                        { 
	(yyval.m_bufferview_index) = -1;
}
#line 2906 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 103: /* bufferview_properties: bufferview_properties YYTOKEN_COMMA YYTOKEN_BUFFER YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 608 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                           {
	gltf_yacc_bufferview_set_buffer_callback((yyval.m_bufferview_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2914 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 104: /* bufferview_properties: bufferview_properties YYTOKEN_COMMA YYTOKEN_BYTEOFFSET YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 612 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                               {
	gltf_yacc_bufferview_set_byteoffset_callback((yyval.m_bufferview_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2922 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 105: /* bufferview_properties: bufferview_properties YYTOKEN_COMMA YYTOKEN_BYTELENGTH YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 616 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                               {
	gltf_yacc_bufferview_set_bytelength_callback((yyval.m_bufferview_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2930 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 106: /* bufferview_properties: bufferview_properties YYTOKEN_COMMA YYTOKEN_BYTESTRIDE YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 620 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                               {
	gltf_yacc_bufferview_set_bytestride_callback((yyval.m_bufferview_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2938 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 107: /* bufferview_properties: bufferview_properties YYTOKEN_COMMA YYTOKEN_TARGET YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 624 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                           {
	gltf_yacc_bufferview_set_target_callback((yyval.m_bufferview_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2946 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 108: /* bufferview_properties: YYTOKEN_BUFFER YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 628 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                       {
	(yyval.m_bufferview_index) = gltf_yacc_bufferview_push_callback(user_defined);
	gltf_yacc_bufferview_set_buffer_callback((yyval.m_bufferview_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2955 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 109: /* bufferview_properties: YYTOKEN_BYTEOFFSET YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 633 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                           {
	(yyval.m_bufferview_index) = gltf_yacc_bufferview_push_callback(user_defined);
	gltf_yacc_bufferview_set_byteoffset_callback((yyval.m_bufferview_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2964 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 110: /* bufferview_properties: YYTOKEN_BYTELENGTH YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 638 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                           {
	(yyval.m_bufferview_index) = gltf_yacc_bufferview_push_callback(user_defined);
	gltf_yacc_bufferview_set_bytelength_callback((yyval.m_bufferview_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2973 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 111: /* bufferview_properties: YYTOKEN_BYTESTRIDE YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 643 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                           {
	(yyval.m_bufferview_index) = gltf_yacc_bufferview_push_callback(user_defined);
	gltf_yacc_bufferview_set_bytestride_callback((yyval.m_bufferview_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2982 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 112: /* bufferview_properties: YYTOKEN_TARGET YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 648 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                       {
	(yyval.m_bufferview_index) = gltf_yacc_bufferview_push_callback(user_defined);
	gltf_yacc_bufferview_set_target_callback((yyval.m_bufferview_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2991 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 113: /* accessors_array: YYTOKEN_LEFTBRACKET accessor_objects YYTOKEN_RIGHTBRACKET  */
#line 653 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                           {

}
#line 2999 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 114: /* accessors_array: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 657 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                          {

}
#line 3007 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 115: /* accessor_objects: accessor_objects YYTOKEN_COMMA accessor_object  */
#line 661 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                 { 
   	assert(-1 == (yyvsp[0].m_accessor_index) || gltf_yacc_accessor_size_callback(user_defined) == ((yyvsp[0].m_accessor_index) + 1));
}
#line 3015 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 116: /* accessor_objects: accessor_object  */
#line 665 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                  { 
	assert(-1 == (yyvsp[0].m_accessor_index) || gltf_yacc_accessor_size_callback(user_defined) == ((yyvsp[0].m_accessor_index) + 1));
}
#line 3023 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 117: /* accessor_object: YYTOKEN_LEFTBRACE accessor_properties YYTOKEN_RIGHTBRACE  */
#line 669 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                          {
	(yyval.m_accessor_index) = (yyvsp[-1].m_accessor_index);
}
#line 3031 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 118: /* accessor_object: YYTOKEN_LEFTBRACE YYTOKEN_RIGHTBRACE  */
#line 673 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                      { 
	(yyval.m_accessor_index) = -1;
}
#line 3039 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 119: /* accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_BUFFERVIEW YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 677 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                           {
	gltf_yacc_accessor_set_bufferview_callback((yyval.m_accessor_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 3047 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 120: /* accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_BYTEOFFSET YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 681 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                           {
	gltf_yacc_accessor_set_byteoffset_callback((yyval.m_accessor_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 3055 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 121: /* accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_COMPONENTTYPE YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 685 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                              {
	gltf_yacc_accessor_set_componenttype_callback((yyval.m_accessor_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 3063 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 122: /* accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_NORMALIZED YYTOKEN_COLON json_boolean  */
#line 689 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                     {
	gltf_yacc_accessor_set_normalized_callback((yyval.m_accessor_index), (yyvsp[0].m_boolean), user_defined);
}
#line 3071 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 123: /* accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 693 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                      {
	gltf_yacc_accessor_set_count_callback((yyval.m_accessor_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 3079 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 124: /* accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_TYPE YYTOKEN_COLON accessor_property_type  */
#line 697 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                         {
	gltf_yacc_accessor_set_type_callback((yyval.m_accessor_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 3087 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 125: /* accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_MAX YYTOKEN_COLON accessor_property_max  */
#line 701 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                       {
	gltf_yacc_accessor_set_max_callback((yyval.m_accessor_index), (yyvsp[0].m_mat4x4), user_defined);
}
#line 3095 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 126: /* accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_MIN YYTOKEN_COLON accessor_property_min  */
#line 705 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                       {
	gltf_yacc_accessor_set_min_callback((yyval.m_accessor_index), (yyvsp[0].m_mat4x4), user_defined);
}
#line 3103 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 127: /* accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_SPARSE YYTOKEN_COLON json_value  */
#line 709 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                               {
	//ignore
}
#line 3111 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 128: /* accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_NAME YYTOKEN_COLON json_string  */
#line 713 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                              {
	gltf_yacc_accessor_set_name_callback((yyval.m_accessor_index), (yyvsp[0].m_temp_string_version), user_defined);
	gltf_lex_yacc_temp_string_destroy_callback((yyvsp[0].m_temp_string_version), user_defined);
}
#line 3120 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 129: /* accessor_properties: YYTOKEN_BUFFERVIEW YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 718 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                         {
	(yyval.m_accessor_index) = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_bufferview_callback((yyval.m_accessor_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 3129 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 130: /* accessor_properties: YYTOKEN_BYTEOFFSET YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 723 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                         {
	(yyval.m_accessor_index) = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_byteoffset_callback((yyval.m_accessor_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 3138 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 131: /* accessor_properties: YYTOKEN_COMPONENTTYPE YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 728 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                            {
	(yyval.m_accessor_index) = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_componenttype_callback((yyval.m_accessor_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 3147 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 132: /* accessor_properties: YYTOKEN_COMPONENTTYPE YYTOKEN_COLON json_boolean  */
#line 733 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                      {
	(yyval.m_accessor_index) = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_normalized_callback((yyval.m_accessor_index), (yyvsp[0].m_boolean), user_defined);
}
#line 3156 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 133: /* accessor_properties: YYTOKEN_COUNT YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 738 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                    {
	(yyval.m_accessor_index) = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_count_callback((yyval.m_accessor_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 3165 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 134: /* accessor_properties: YYTOKEN_TYPE YYTOKEN_COLON accessor_property_type  */
#line 743 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                       {
	(yyval.m_accessor_index) = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_type_callback((yyval.m_accessor_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 3174 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 135: /* accessor_properties: YYTOKEN_MAX YYTOKEN_COLON accessor_property_max  */
#line 748 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                     {
	(yyval.m_accessor_index) = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_max_callback((yyval.m_accessor_index), (yyvsp[0].m_mat4x4), user_defined);
}
#line 3183 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 136: /* accessor_properties: YYTOKEN_MIN YYTOKEN_COLON accessor_property_min  */
#line 753 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                     {
	(yyval.m_accessor_index) = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_min_callback((yyval.m_accessor_index), (yyvsp[0].m_mat4x4), user_defined);
}
#line 3192 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 137: /* accessor_properties: YYTOKEN_SPARSE YYTOKEN_COLON json_value  */
#line 758 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                             {
	//ignore
}
#line 3200 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 138: /* accessor_properties: YYTOKEN_NAME YYTOKEN_COLON json_string  */
#line 762 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                            {
	(yyval.m_accessor_index) = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_name_callback((yyval.m_accessor_index), (yyvsp[0].m_temp_string_version), user_defined);
	gltf_lex_yacc_temp_string_destroy_callback((yyvsp[0].m_temp_string_version), user_defined);
}
#line 3210 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 139: /* accessor_property_type: YYTOKEN_SCALAR  */
#line 768 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                        {
	(yyval.m_token_numberint) = 1;
}
#line 3218 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 140: /* accessor_property_type: YYTOKEN_VEC2  */
#line 772 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                      {
	(yyval.m_token_numberint) = 2;
}
#line 3226 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 141: /* accessor_property_type: YYTOKEN_VEC3  */
#line 776 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                      {
	(yyval.m_token_numberint) = 3;
}
#line 3234 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 142: /* accessor_property_type: YYTOKEN_VEC4  */
#line 780 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                      {
	(yyval.m_token_numberint) = 4;
}
#line 3242 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 143: /* accessor_property_type: YYTOKEN_MAT2  */
#line 784 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                      {
	(yyval.m_token_numberint) = 5;
}
#line 3250 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 144: /* accessor_property_type: YYTOKEN_MAT3  */
#line 788 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                      {
	(yyval.m_token_numberint) = 9;
}
#line 3258 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 145: /* accessor_property_type: YYTOKEN_MAT4  */
#line 792 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                      {
	(yyval.m_token_numberint) = 16;
}
#line 3266 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 146: /* accessor_property_max: YYTOKEN_LEFTBRACKET accessor_property_max_element YYTOKEN_COMMA accessor_property_max_element YYTOKEN_COMMA accessor_property_max_element YYTOKEN_COMMA accessor_property_max_element YYTOKEN_COMMA accessor_property_max_element YYTOKEN_COMMA accessor_property_max_element YYTOKEN_COMMA accessor_property_max_element YYTOKEN_COMMA accessor_property_max_element YYTOKEN_COMMA accessor_property_max_element YYTOKEN_COMMA accessor_property_max_element YYTOKEN_COMMA accessor_property_max_element YYTOKEN_COMMA accessor_property_max_element YYTOKEN_COMMA accessor_property_max_element YYTOKEN_COMMA accessor_property_max_element YYTOKEN_COMMA accessor_property_max_element YYTOKEN_COMMA accessor_property_max_element YYTOKEN_RIGHTBRACKET  */
#line 796 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
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
#line 3289 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 147: /* accessor_property_max: YYTOKEN_LEFTBRACKET accessor_property_max_element YYTOKEN_COMMA accessor_property_max_element YYTOKEN_COMMA accessor_property_max_element YYTOKEN_COMMA accessor_property_max_element YYTOKEN_COMMA accessor_property_max_element YYTOKEN_COMMA accessor_property_max_element YYTOKEN_COMMA accessor_property_max_element YYTOKEN_COMMA accessor_property_max_element YYTOKEN_COMMA accessor_property_max_element YYTOKEN_RIGHTBRACKET  */
#line 815 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                                                                                                                                                                                                                                                                                                                                                                              {
	(yyval.m_mat4x4)[0] = (yyvsp[-17].m_token_numberfloat);
	(yyval.m_mat4x4)[1] = (yyvsp[-15].m_token_numberfloat);
	(yyval.m_mat4x4)[2] = (yyvsp[-13].m_token_numberfloat);
	(yyval.m_mat4x4)[3] = (yyvsp[-11].m_token_numberfloat);
	(yyval.m_mat4x4)[4] = (yyvsp[-9].m_token_numberfloat);
	(yyval.m_mat4x4)[5] = (yyvsp[-7].m_token_numberfloat);
	(yyval.m_mat4x4)[6] = (yyvsp[-5].m_token_numberfloat);
	(yyval.m_mat4x4)[7] = (yyvsp[-3].m_token_numberfloat);
	(yyval.m_mat4x4)[8] = (yyvsp[-1].m_token_numberfloat);
	(yyval.m_mat4x4)[9] = 0.0f;
	(yyval.m_mat4x4)[10] = 0.0f;
	(yyval.m_mat4x4)[11] = 0.0f;
	(yyval.m_mat4x4)[12] = 0.0f;
	(yyval.m_mat4x4)[13] = 0.0f;
	(yyval.m_mat4x4)[14] = 0.0f;
	(yyval.m_mat4x4)[15] = 0.0f;
}
#line 3312 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 148: /* accessor_property_max: YYTOKEN_LEFTBRACKET accessor_property_max_element YYTOKEN_COMMA accessor_property_max_element YYTOKEN_COMMA accessor_property_max_element YYTOKEN_COMMA accessor_property_max_element YYTOKEN_RIGHTBRACKET  */
#line 834 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                                                                                                                                                  {
	(yyval.m_mat4x4)[0] = (yyvsp[-7].m_token_numberfloat);
	(yyval.m_mat4x4)[1] = (yyvsp[-5].m_token_numberfloat);
	(yyval.m_mat4x4)[2] = (yyvsp[-3].m_token_numberfloat);
	(yyval.m_mat4x4)[3] = (yyvsp[-1].m_token_numberfloat);
	(yyval.m_mat4x4)[4] = 0.0f;
	(yyval.m_mat4x4)[5] = 0.0f;
	(yyval.m_mat4x4)[6] = 0.0f;
	(yyval.m_mat4x4)[7] = 0.0f;
	(yyval.m_mat4x4)[8] = 0.0f;
	(yyval.m_mat4x4)[9] = 0.0f;
	(yyval.m_mat4x4)[10] = 0.0f;
	(yyval.m_mat4x4)[11] = 0.0f;
	(yyval.m_mat4x4)[12] = 0.0f;
	(yyval.m_mat4x4)[13] = 0.0f;
	(yyval.m_mat4x4)[14] = 0.0f;
	(yyval.m_mat4x4)[15] = 0.0f;
}
#line 3335 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 149: /* accessor_property_max: YYTOKEN_LEFTBRACKET accessor_property_max_element YYTOKEN_COMMA accessor_property_max_element YYTOKEN_COMMA accessor_property_max_element YYTOKEN_RIGHTBRACKET  */
#line 853 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                                                                                                      {
	(yyval.m_mat4x4)[0] = (yyvsp[-5].m_token_numberfloat);
	(yyval.m_mat4x4)[1] = (yyvsp[-3].m_token_numberfloat);
	(yyval.m_mat4x4)[2] = (yyvsp[-1].m_token_numberfloat);
	(yyval.m_mat4x4)[3] = 0.0f;
	(yyval.m_mat4x4)[4] = 0.0f;
	(yyval.m_mat4x4)[5] = 0.0f;
	(yyval.m_mat4x4)[6] = 0.0f;
	(yyval.m_mat4x4)[7] = 0.0f;
	(yyval.m_mat4x4)[8] = 0.0f;
	(yyval.m_mat4x4)[9] = 0.0f;
	(yyval.m_mat4x4)[10] = 0.0f;
	(yyval.m_mat4x4)[11] = 0.0f;
	(yyval.m_mat4x4)[12] = 0.0f;
	(yyval.m_mat4x4)[13] = 0.0f;
	(yyval.m_mat4x4)[14] = 0.0f;
	(yyval.m_mat4x4)[15] = 0.0f;
}
#line 3358 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 150: /* accessor_property_max: YYTOKEN_LEFTBRACKET accessor_property_max_element YYTOKEN_COMMA accessor_property_max_element YYTOKEN_RIGHTBRACKET  */
#line 872 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                                                          {
	(yyval.m_mat4x4)[0] = (yyvsp[-3].m_token_numberfloat);
	(yyval.m_mat4x4)[1] = (yyvsp[-1].m_token_numberfloat);
	(yyval.m_mat4x4)[2] = 0.0f;
	(yyval.m_mat4x4)[3] = 0.0f;
	(yyval.m_mat4x4)[4] = 0.0f;
	(yyval.m_mat4x4)[5] = 0.0f;
	(yyval.m_mat4x4)[6] = 0.0f;
	(yyval.m_mat4x4)[7] = 0.0f;
	(yyval.m_mat4x4)[8] = 0.0f;
	(yyval.m_mat4x4)[9] = 0.0f;
	(yyval.m_mat4x4)[10] = 0.0f;
	(yyval.m_mat4x4)[11] = 0.0f;
	(yyval.m_mat4x4)[12] = 0.0f;
	(yyval.m_mat4x4)[13] = 0.0f;
	(yyval.m_mat4x4)[14] = 0.0f;
	(yyval.m_mat4x4)[15] = 0.0f;
}
#line 3381 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 151: /* accessor_property_max: YYTOKEN_LEFTBRACKET accessor_property_max_element YYTOKEN_RIGHTBRACKET  */
#line 891 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                              {
	(yyval.m_mat4x4)[0] = (yyvsp[-1].m_token_numberfloat);
	(yyval.m_mat4x4)[1] = 0.0f;
	(yyval.m_mat4x4)[2] = 0.0f;
	(yyval.m_mat4x4)[3] = 0.0f;
	(yyval.m_mat4x4)[4] = 0.0f;
	(yyval.m_mat4x4)[5] = 0.0f;
	(yyval.m_mat4x4)[6] = 0.0f;
	(yyval.m_mat4x4)[7] = 0.0f;
	(yyval.m_mat4x4)[8] = 0.0f;
	(yyval.m_mat4x4)[9] = 0.0f;
	(yyval.m_mat4x4)[10] = 0.0f;
	(yyval.m_mat4x4)[11] = 0.0f;
	(yyval.m_mat4x4)[12] = 0.0f;
	(yyval.m_mat4x4)[13] = 0.0f;
	(yyval.m_mat4x4)[14] = 0.0f;
	(yyval.m_mat4x4)[15] = 0.0f;
}
#line 3404 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 152: /* accessor_property_max_element: YYTOKEN_NUMBER_FLOAT  */
#line 910 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                    {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberfloat);
}
#line 3412 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 153: /* accessor_property_max_element: YYTOKEN_NUMBER_INT  */
#line 914 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                  {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberint);
}
#line 3420 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 154: /* accessor_property_min: YYTOKEN_LEFTBRACKET accessor_property_min_element YYTOKEN_COMMA accessor_property_min_element YYTOKEN_COMMA accessor_property_min_element YYTOKEN_COMMA accessor_property_min_element YYTOKEN_COMMA accessor_property_min_element YYTOKEN_COMMA accessor_property_min_element YYTOKEN_COMMA accessor_property_min_element YYTOKEN_COMMA accessor_property_min_element YYTOKEN_COMMA accessor_property_min_element YYTOKEN_COMMA accessor_property_min_element YYTOKEN_COMMA accessor_property_min_element YYTOKEN_COMMA accessor_property_min_element YYTOKEN_COMMA accessor_property_min_element YYTOKEN_COMMA accessor_property_min_element YYTOKEN_COMMA accessor_property_min_element YYTOKEN_COMMA accessor_property_min_element YYTOKEN_RIGHTBRACKET  */
#line 918 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
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
#line 3443 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 155: /* accessor_property_min: YYTOKEN_LEFTBRACKET accessor_property_min_element YYTOKEN_COMMA accessor_property_min_element YYTOKEN_COMMA accessor_property_min_element YYTOKEN_COMMA accessor_property_min_element YYTOKEN_COMMA accessor_property_min_element YYTOKEN_COMMA accessor_property_min_element YYTOKEN_COMMA accessor_property_min_element YYTOKEN_COMMA accessor_property_min_element YYTOKEN_COMMA accessor_property_min_element YYTOKEN_RIGHTBRACKET  */
#line 937 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                                                                                                                                                                                                                                                                                                                                                                              {
	(yyval.m_mat4x4)[0] = (yyvsp[-17].m_token_numberfloat);
	(yyval.m_mat4x4)[1] = (yyvsp[-15].m_token_numberfloat);
	(yyval.m_mat4x4)[2] = (yyvsp[-13].m_token_numberfloat);
	(yyval.m_mat4x4)[3] = (yyvsp[-11].m_token_numberfloat);
	(yyval.m_mat4x4)[4] = (yyvsp[-9].m_token_numberfloat);
	(yyval.m_mat4x4)[5] = (yyvsp[-7].m_token_numberfloat);
	(yyval.m_mat4x4)[6] = (yyvsp[-5].m_token_numberfloat);
	(yyval.m_mat4x4)[7] = (yyvsp[-3].m_token_numberfloat);
	(yyval.m_mat4x4)[8] = (yyvsp[-1].m_token_numberfloat);
	(yyval.m_mat4x4)[9] = 0.0f;
	(yyval.m_mat4x4)[10] = 0.0f;
	(yyval.m_mat4x4)[11] = 0.0f;
	(yyval.m_mat4x4)[12] = 0.0f;
	(yyval.m_mat4x4)[13] = 0.0f;
	(yyval.m_mat4x4)[14] = 0.0f;
	(yyval.m_mat4x4)[15] = 0.0f;
}
#line 3466 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 156: /* accessor_property_min: YYTOKEN_LEFTBRACKET accessor_property_min_element YYTOKEN_COMMA accessor_property_min_element YYTOKEN_COMMA accessor_property_min_element YYTOKEN_COMMA accessor_property_min_element YYTOKEN_RIGHTBRACKET  */
#line 956 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                                                                                                                                                  {
	(yyval.m_mat4x4)[0] = (yyvsp[-7].m_token_numberfloat);
	(yyval.m_mat4x4)[1] = (yyvsp[-5].m_token_numberfloat);
	(yyval.m_mat4x4)[2] = (yyvsp[-3].m_token_numberfloat);
	(yyval.m_mat4x4)[3] = (yyvsp[-1].m_token_numberfloat);
	(yyval.m_mat4x4)[4] = 0.0f;
	(yyval.m_mat4x4)[5] = 0.0f;
	(yyval.m_mat4x4)[6] = 0.0f;
	(yyval.m_mat4x4)[7] = 0.0f;
	(yyval.m_mat4x4)[8] = 0.0f;
	(yyval.m_mat4x4)[9] = 0.0f;
	(yyval.m_mat4x4)[10] = 0.0f;
	(yyval.m_mat4x4)[11] = 0.0f;
	(yyval.m_mat4x4)[12] = 0.0f;
	(yyval.m_mat4x4)[13] = 0.0f;
	(yyval.m_mat4x4)[14] = 0.0f;
	(yyval.m_mat4x4)[15] = 0.0f;
}
#line 3489 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 157: /* accessor_property_min: YYTOKEN_LEFTBRACKET accessor_property_min_element YYTOKEN_COMMA accessor_property_min_element YYTOKEN_COMMA accessor_property_min_element YYTOKEN_RIGHTBRACKET  */
#line 975 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                                                                                                      {
	(yyval.m_mat4x4)[0] = (yyvsp[-5].m_token_numberfloat);
	(yyval.m_mat4x4)[1] = (yyvsp[-3].m_token_numberfloat);
	(yyval.m_mat4x4)[2] = (yyvsp[-1].m_token_numberfloat);
	(yyval.m_mat4x4)[3] = 0.0f;
	(yyval.m_mat4x4)[4] = 0.0f;
	(yyval.m_mat4x4)[5] = 0.0f;
	(yyval.m_mat4x4)[6] = 0.0f;
	(yyval.m_mat4x4)[7] = 0.0f;
	(yyval.m_mat4x4)[8] = 0.0f;
	(yyval.m_mat4x4)[9] = 0.0f;
	(yyval.m_mat4x4)[10] = 0.0f;
	(yyval.m_mat4x4)[11] = 0.0f;
	(yyval.m_mat4x4)[12] = 0.0f;
	(yyval.m_mat4x4)[13] = 0.0f;
	(yyval.m_mat4x4)[14] = 0.0f;
	(yyval.m_mat4x4)[15] = 0.0f;
}
#line 3512 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 158: /* accessor_property_min: YYTOKEN_LEFTBRACKET accessor_property_min_element YYTOKEN_COMMA accessor_property_min_element YYTOKEN_RIGHTBRACKET  */
#line 994 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                                                          {
	(yyval.m_mat4x4)[0] = (yyvsp[-3].m_token_numberfloat);
	(yyval.m_mat4x4)[1] = (yyvsp[-1].m_token_numberfloat);
	(yyval.m_mat4x4)[2] = 0.0f;
	(yyval.m_mat4x4)[3] = 0.0f;
	(yyval.m_mat4x4)[4] = 0.0f;
	(yyval.m_mat4x4)[5] = 0.0f;
	(yyval.m_mat4x4)[6] = 0.0f;
	(yyval.m_mat4x4)[7] = 0.0f;
	(yyval.m_mat4x4)[8] = 0.0f;
	(yyval.m_mat4x4)[9] = 0.0f;
	(yyval.m_mat4x4)[10] = 0.0f;
	(yyval.m_mat4x4)[11] = 0.0f;
	(yyval.m_mat4x4)[12] = 0.0f;
	(yyval.m_mat4x4)[13] = 0.0f;
	(yyval.m_mat4x4)[14] = 0.0f;
	(yyval.m_mat4x4)[15] = 0.0f;
}
#line 3535 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 159: /* accessor_property_min: YYTOKEN_LEFTBRACKET accessor_property_min_element YYTOKEN_RIGHTBRACKET  */
#line 1013 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                              {
	(yyval.m_mat4x4)[0] = (yyvsp[-1].m_token_numberfloat);
	(yyval.m_mat4x4)[1] = 0.0f;
	(yyval.m_mat4x4)[2] = 0.0f;
	(yyval.m_mat4x4)[3] = 0.0f;
	(yyval.m_mat4x4)[4] = 0.0f;
	(yyval.m_mat4x4)[5] = 0.0f;
	(yyval.m_mat4x4)[6] = 0.0f;
	(yyval.m_mat4x4)[7] = 0.0f;
	(yyval.m_mat4x4)[8] = 0.0f;
	(yyval.m_mat4x4)[9] = 0.0f;
	(yyval.m_mat4x4)[10] = 0.0f;
	(yyval.m_mat4x4)[11] = 0.0f;
	(yyval.m_mat4x4)[12] = 0.0f;
	(yyval.m_mat4x4)[13] = 0.0f;
	(yyval.m_mat4x4)[14] = 0.0f;
	(yyval.m_mat4x4)[15] = 0.0f;
}
#line 3558 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 160: /* accessor_property_min_element: YYTOKEN_NUMBER_FLOAT  */
#line 1032 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                    {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberfloat);
}
#line 3566 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 161: /* accessor_property_min_element: YYTOKEN_NUMBER_INT  */
#line 1036 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                  {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberint);
}
#line 3574 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 162: /* json_string: YYTOKEN_STRING  */
#line 1040 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                            {
	(yyval.m_temp_string_version) = (yyvsp[0].m_temp_string_version);
}
#line 3582 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 163: /* json_string: YYTOKEN_ASSET  */
#line 1044 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                           {

}
#line 3590 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 164: /* json_string: YYTOKEN_COPYRIGHT  */
#line 1048 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                               {

}
#line 3598 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 165: /* json_string: YYTOKEN_GENERATOR  */
#line 1052 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                               {

}
#line 3606 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 166: /* json_string: YYTOKEN_VERSION  */
#line 1056 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                             {

}
#line 3614 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 167: /* json_string: YYTOKEN_MINVERSION  */
#line 1060 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                {

}
#line 3622 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 168: /* json_string: YYTOKEN_EXTENSIONS  */
#line 1064 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                {

}
#line 3630 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 169: /* json_string: YYTOKEN_EXTRAS  */
#line 1068 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                            {

}
#line 3638 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 170: /* json_string: YYTOKEN_SCENES  */
#line 1072 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                            {

}
#line 3646 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 171: /* json_string: YYTOKEN_NODES  */
#line 1076 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                           {

}
#line 3654 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 172: /* json_string: YYTOKEN_NAME  */
#line 1080 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                          {

}
#line 3662 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 173: /* json_string: YYTOKEN_CAMERA  */
#line 1084 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                            {

}
#line 3670 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 174: /* json_string: YYTOKEN_CHILDREN  */
#line 1088 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                              {

}
#line 3678 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 175: /* json_string: YYTOKEN_SKIN  */
#line 1092 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                          {

}
#line 3686 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 176: /* json_string: YYTOKEN_MATRIX  */
#line 1096 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                            {

}
#line 3694 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 177: /* json_string: YYTOKEN_MESH  */
#line 1100 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                          {

}
#line 3702 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 178: /* json_string: YYTOKEN_ROTATION  */
#line 1104 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                              {

}
#line 3710 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 179: /* json_string: YYTOKEN_SCALE  */
#line 1108 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                           {

}
#line 3718 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 180: /* json_string: YYTOKEN_TRANSLATION  */
#line 1112 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                 {

}
#line 3726 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 181: /* json_string: YYTOKEN_WEIGHTS  */
#line 1116 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                             {

}
#line 3734 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 182: /* json_string: YYTOKEN_BUFFERS  */
#line 1120 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                             {

}
#line 3742 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 183: /* json_string: YYTOKEN_BYTELENGTH  */
#line 1124 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                {

}
#line 3750 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 184: /* json_string: YYTOKEN_URI  */
#line 1128 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                         {

}
#line 3758 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 185: /* json_boolean: YYTOKEN_TRUE  */
#line 1132 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                           {
	(yyval.m_boolean) = true;
}
#line 3766 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 186: /* json_boolean: YYTOKEN_FALSE  */
#line 1136 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                            {
	(yyval.m_boolean) = false;
}
#line 3774 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 187: /* json_value: json_object  */
#line 1140 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                        { 

}
#line 3782 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 188: /* json_value: json_array  */
#line 1144 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                       { 

}
#line 3790 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 189: /* json_value: json_string  */
#line 1148 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                        { 
	gltf_lex_yacc_temp_string_destroy_callback((yyvsp[0].m_temp_string_version), user_defined);
}
#line 3798 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 190: /* json_value: YYTOKEN_NUMBER_INT  */
#line 1152 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                               { 

}
#line 3806 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 191: /* json_value: YYTOKEN_NUMBER_FLOAT  */
#line 1156 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                 { 

}
#line 3814 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 192: /* json_value: json_boolean  */
#line 1160 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                         { 

}
#line 3822 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 193: /* json_value: YYTOKEN_NULL  */
#line 1164 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                         { 

}
#line 3830 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 194: /* json_object: YYTOKEN_LEFTBRACE json_members YYTOKEN_RIGHTBRACE  */
#line 1168 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                               { 

}
#line 3838 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 195: /* json_object: YYTOKEN_LEFTBRACE YYTOKEN_RIGHTBRACE  */
#line 1172 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                  { 

}
#line 3846 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 196: /* json_members: json_members YYTOKEN_COMMA json_member  */
#line 1176 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                     { 

}
#line 3854 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 197: /* json_members: json_member  */
#line 1180 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                          { 

}
#line 3862 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 198: /* json_member: json_string YYTOKEN_COLON json_value  */
#line 1184 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                  { 
	gltf_lex_yacc_temp_string_destroy_callback((yyvsp[-2].m_temp_string_version), user_defined);
}
#line 3870 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 199: /* json_array: YYTOKEN_LEFTBRACKET json_elements YYTOKEN_RIGHTBRACKET  */
#line 1188 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                   { 

}
#line 3878 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 200: /* json_array: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 1192 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                     { 

}
#line 3886 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 201: /* json_elements: json_elements YYTOKEN_COMMA json_element  */
#line 1196 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                        { 

}
#line 3894 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 202: /* json_elements: json_element  */
#line 1200 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                            { 

}
#line 3902 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 203: /* json_element: json_value  */
#line 1204 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                         {
	
}
#line 3910 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;


#line 3914 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"

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

#line 1208 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"

