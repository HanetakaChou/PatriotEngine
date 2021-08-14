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
    YYTOKEN_TRUE = 294,            /* YYTOKEN_TRUE  */
    YYTOKEN_FALSE = 295,           /* YYTOKEN_FALSE  */
    YYTOKEN_NULL = 296,            /* YYTOKEN_NULL  */
    YYTOKEN_LEFTBRACE = 297,       /* YYTOKEN_LEFTBRACE  */
    YYTOKEN_RIGHTBRACE = 298,      /* YYTOKEN_RIGHTBRACE  */
    YYTOKEN_COLON = 299,           /* YYTOKEN_COLON  */
    YYTOKEN_LEFTBRACKET = 300,     /* YYTOKEN_LEFTBRACKET  */
    YYTOKEN_RIGHTBRACKET = 301,    /* YYTOKEN_RIGHTBRACKET  */
    YYTOKEN_COMMA = 302,           /* YYTOKEN_COMMA  */
    YYTOKEN_STRING = 303,          /* YYTOKEN_STRING  */
    YYTOKEN_NUMBER_INT = 304,      /* YYTOKEN_NUMBER_INT  */
    YYTOKEN_NUMBER_FLOAT = 305,    /* YYTOKEN_NUMBER_FLOAT  */
    PSEUDO_LEX_ERROR = 306         /* PSEUDO_LEX_ERROR  */
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
  YYSYMBOL_YYTOKEN_TRUE = 39,              /* YYTOKEN_TRUE  */
  YYSYMBOL_YYTOKEN_FALSE = 40,             /* YYTOKEN_FALSE  */
  YYSYMBOL_YYTOKEN_NULL = 41,              /* YYTOKEN_NULL  */
  YYSYMBOL_YYTOKEN_LEFTBRACE = 42,         /* YYTOKEN_LEFTBRACE  */
  YYSYMBOL_YYTOKEN_RIGHTBRACE = 43,        /* YYTOKEN_RIGHTBRACE  */
  YYSYMBOL_YYTOKEN_COLON = 44,             /* YYTOKEN_COLON  */
  YYSYMBOL_YYTOKEN_LEFTBRACKET = 45,       /* YYTOKEN_LEFTBRACKET  */
  YYSYMBOL_YYTOKEN_RIGHTBRACKET = 46,      /* YYTOKEN_RIGHTBRACKET  */
  YYSYMBOL_YYTOKEN_COMMA = 47,             /* YYTOKEN_COMMA  */
  YYSYMBOL_YYTOKEN_STRING = 48,            /* YYTOKEN_STRING  */
  YYSYMBOL_YYTOKEN_NUMBER_INT = 49,        /* YYTOKEN_NUMBER_INT  */
  YYSYMBOL_YYTOKEN_NUMBER_FLOAT = 50,      /* YYTOKEN_NUMBER_FLOAT  */
  YYSYMBOL_PSEUDO_LEX_ERROR = 51,          /* PSEUDO_LEX_ERROR  */
  YYSYMBOL_YYACCEPT = 52,                  /* $accept  */
  YYSYMBOL_gltf_object = 53,               /* gltf_object  */
  YYSYMBOL_gltf_members = 54,              /* gltf_members  */
  YYSYMBOL_gltf_member = 55,               /* gltf_member  */
  YYSYMBOL_asset_object = 56,              /* asset_object  */
  YYSYMBOL_asset_properties = 57,          /* asset_properties  */
  YYSYMBOL_scenes_array = 58,              /* scenes_array  */
  YYSYMBOL_scene_objects = 59,             /* scene_objects  */
  YYSYMBOL_scene_object = 60,              /* scene_object  */
  YYSYMBOL_scene_properties = 61,          /* scene_properties  */
  YYSYMBOL_scene_property_nodes = 62,      /* scene_property_nodes  */
  YYSYMBOL_scene_property_nodes_elements = 63, /* scene_property_nodes_elements  */
  YYSYMBOL_nodes_array = 64,               /* nodes_array  */
  YYSYMBOL_node_objects = 65,              /* node_objects  */
  YYSYMBOL_node_object = 66,               /* node_object  */
  YYSYMBOL_node_properties = 67,           /* node_properties  */
  YYSYMBOL_node_property_children = 68,    /* node_property_children  */
  YYSYMBOL_node_property_children_elements = 69, /* node_property_children_elements  */
  YYSYMBOL_node_property_matrix = 70,      /* node_property_matrix  */
  YYSYMBOL_node_property_matrix_element = 71, /* node_property_matrix_element  */
  YYSYMBOL_node_property_rotation = 72,    /* node_property_rotation  */
  YYSYMBOL_node_property_rotation_element = 73, /* node_property_rotation_element  */
  YYSYMBOL_node_property_scale = 74,       /* node_property_scale  */
  YYSYMBOL_node_property_scale_element = 75, /* node_property_scale_element  */
  YYSYMBOL_node_property_translation = 76, /* node_property_translation  */
  YYSYMBOL_node_property_translation_element = 77, /* node_property_translation_element  */
  YYSYMBOL_node_property_weights = 78,     /* node_property_weights  */
  YYSYMBOL_node_property_weights_elements = 79, /* node_property_weights_elements  */
  YYSYMBOL_node_property_weights_element = 80, /* node_property_weights_element  */
  YYSYMBOL_buffers_array = 81,             /* buffers_array  */
  YYSYMBOL_buffer_objects = 82,            /* buffer_objects  */
  YYSYMBOL_buffer_object = 83,             /* buffer_object  */
  YYSYMBOL_buffer_properties = 84,         /* buffer_properties  */
  YYSYMBOL_bufferviews_array = 85,         /* bufferviews_array  */
  YYSYMBOL_bufferview_objects = 86,        /* bufferview_objects  */
  YYSYMBOL_bufferview_object = 87,         /* bufferview_object  */
  YYSYMBOL_bufferview_properties = 88,     /* bufferview_properties  */
  YYSYMBOL_accessors_array = 89,           /* accessors_array  */
  YYSYMBOL_accessor_objects = 90,          /* accessor_objects  */
  YYSYMBOL_accessor_object = 91,           /* accessor_object  */
  YYSYMBOL_accessor_properties = 92,       /* accessor_properties  */
  YYSYMBOL_json_string = 93,               /* json_string  */
  YYSYMBOL_json_boolean = 94,              /* json_boolean  */
  YYSYMBOL_json_value = 95,                /* json_value  */
  YYSYMBOL_json_object = 96,               /* json_object  */
  YYSYMBOL_json_members = 97,              /* json_members  */
  YYSYMBOL_json_member = 98,               /* json_member  */
  YYSYMBOL_json_array = 99,                /* json_array  */
  YYSYMBOL_json_elements = 100,            /* json_elements  */
  YYSYMBOL_json_element = 101              /* json_element  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 13 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"

    int yylex(union YYSTYPE *lvalp, YYLTYPE *llocp, void *user_defined, void *yyscanner);
    void yyerror(YYLTYPE *llocp, void *user_defined, void *yyscanner, const char *msg);

#line 311 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"

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
#define YYLAST   463

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  52
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  50
/* YYNRULES -- Number of rules.  */
#define YYNRULES  168
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  403

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   306


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
      45,    46,    47,    48,    49,    50,    51
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   131,   131,   135,   142,   150,   154,   158,   162,   166,
     170,   174,   178,   182,   186,   190,   194,   198,   202,   206,
     210,   214,   218,   222,   226,   230,   234,   238,   242,   246,
     251,   255,   259,   265,   270,   274,   278,   282,   286,   291,
     295,   299,   303,   307,   311,   315,   319,   324,   328,   332,
     336,   340,   344,   348,   353,   357,   362,   368,   373,   378,
     383,   388,   393,   398,   404,   409,   413,   417,   421,   426,
     445,   449,   453,   460,   464,   468,   474,   478,   482,   488,
     492,   496,   500,   504,   508,   514,   518,   523,   527,   531,
     535,   539,   543,   547,   551,   556,   561,   566,   570,   574,
     578,   582,   586,   590,   594,   598,   602,   606,   610,   615,
     620,   625,   630,   635,   639,   643,   647,   651,   655,   659,
     663,   667,   671,   675,   680,   685,   690,   695,   699,   703,
     707,   711,   715,   719,   723,   727,   731,   735,   739,   743,
     747,   751,   755,   759,   763,   767,   771,   775,   779,   783,
     787,   791,   795,   799,   803,   807,   811,   815,   819,   823,
     827,   831,   835,   839,   843,   847,   851,   855,   859
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
  "YYTOKEN_TYPE", "YYTOKEN_MAX", "YYTOKEN_MIN", "YYTOKEN_TRUE",
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
  "accessor_object", "accessor_properties", "json_string", "json_boolean",
  "json_value", "json_object", "json_members", "json_member", "json_array",
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
     305,   306
};
#endif

#define YYPACT_NINF (-324)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -28,    87,    31,   -11,    73,   109,   128,   136,   167,   195,
      -4,  -324,  -324,     9,    40,    85,    99,   142,   190,   226,
    -324,    87,   100,  -324,  -324,    16,  -324,    28,  -324,   144,
    -324,   153,  -324,   154,  -324,  -324,   229,   230,   231,   236,
     239,     6,    23,  -324,    92,  -324,    65,  -324,   124,  -324,
      13,  -324,   171,  -324,    17,  -324,   173,  -324,    60,  -324,
     175,  -324,     4,     4,     4,     4,   237,  -324,   172,   240,
     244,   245,  -324,    86,  -324,   248,   247,   249,   250,   251,
     252,   253,   254,   255,   256,   257,  -324,   159,  -324,   260,
     259,   261,  -324,   160,  -324,   262,   263,   264,   265,   266,
     267,  -324,   161,  -324,   270,   269,   271,   272,  -324,   162,
    -324,   275,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,
    -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,
    -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,
    -324,  -324,     7,   143,  -324,  -324,  -324,  -324,  -324,  -324,
    -324,   274,   276,   277,   278,   279,   237,   227,     4,  -324,
     103,  -324,     4,   232,   280,   243,   281,   282,   283,   284,
     285,   287,  -324,   106,  -324,   286,     4,  -324,   199,  -324,
     288,   289,   290,   291,   292,  -324,   107,  -324,   293,   294,
      62,  -324,   141,  -324,  -324,   300,   169,  -324,  -324,  -324,
     179,  -324,     4,     4,     4,     4,   237,  -324,    45,  -324,
    -324,   301,   302,   303,  -324,  -324,   164,  -324,  -324,   178,
    -324,  -324,   180,  -324,   182,  -324,   184,  -324,   148,  -324,
     304,   305,   306,   307,   308,   309,   310,   311,   312,   313,
    -324,  -324,   314,   315,  -324,  -324,  -324,  -324,  -324,   316,
     317,   318,   319,   320,  -324,  -324,  -324,  -324,   321,   322,
     323,   324,   237,  -324,   258,  -324,   237,  -324,  -324,  -324,
    -324,  -324,  -324,  -324,   191,   237,   227,     4,  -324,  -324,
     217,  -324,  -324,   325,  -324,  -324,   326,  -324,  -324,   327,
    -324,  -324,   328,  -324,  -324,  -324,   219,  -324,     4,   329,
     280,   330,   281,   331,   283,   284,   285,   287,   332,     4,
     333,   334,   335,   336,   337,   338,   339,   340,   228,  -324,
    -324,  -324,  -324,   341,  -324,  -324,  -324,  -324,   342,   178,
     180,   182,   184,  -324,   220,  -324,  -324,  -324,  -324,  -324,
    -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,
    -324,  -324,  -324,  -324,  -324,  -324,  -324,  -324,   345,   346,
     347,   348,  -324,   178,   180,   182,   184,   349,   350,   268,
     273,   178,   180,  -324,  -324,   351,   353,   178,  -324,   354,
     178,   355,   178,   356,   178,   357,   178,   358,   178,   359,
     178,   360,   178,   361,   178,   362,   178,   363,   178,   364,
     178,   366,  -324
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
       0,   102,     0,    97,     0,     0,     0,     0,   118,     0,
     113,     0,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   127,    18,    19,    20,    21,   150,
     151,   158,     0,     0,   155,   156,   154,   157,    22,   152,
     153,     0,     0,     0,     0,     0,     0,     0,     0,    27,
       0,    25,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    43,     0,    41,     0,     0,    91,     0,    89,
       0,     0,     0,     0,     0,   101,     0,    99,     0,     0,
       0,   117,     0,   115,   160,     0,     0,   162,   165,   168,
       0,   167,     0,     0,     0,     0,     0,    34,     0,    32,
      33,     0,     0,     0,    64,    55,     0,    56,    57,     0,
      58,    59,     0,    60,     0,    61,     0,    62,     0,    63,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      95,    96,     0,     0,   110,   108,   109,   111,   112,     0,
       0,     0,     0,     0,   124,   123,   125,   126,     0,     0,
       0,     0,     0,   159,     0,   164,     0,    13,    14,    15,
      16,    17,    36,    38,     0,     0,     0,     0,    66,    68,
       0,    71,    70,     0,    74,    73,     0,    77,    76,     0,
      80,    79,     0,    82,    86,    85,     0,    84,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   163,
     161,   166,    35,     0,    31,    29,    30,    65,     0,     0,
       0,     0,     0,    81,     0,    54,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    93,    94,   105,   103,   104,
     106,   107,   120,   119,   121,   122,    37,    67,     0,     0,
       0,     0,    83,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    75,    78,     0,     0,     0,    72,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    69
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -324,  -324,  -324,   379,  -324,  -324,  -324,  -324,   295,  -324,
      48,  -324,  -324,  -324,   238,  -324,    33,  -324,    32,  -323,
      67,  -300,    64,  -297,    30,  -290,    69,  -324,    43,  -324,
    -324,   343,  -324,  -324,  -324,   344,  -324,  -324,  -324,   352,
    -324,   -62,  -186,   -61,  -324,  -324,   149,  -324,  -324,   150
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     2,    10,    11,    23,    41,    26,    44,    45,    73,
     209,   274,    28,    48,    49,    87,   217,   280,   220,   283,
     223,   286,   225,   289,   227,   292,   229,   296,   297,    30,
      52,    53,    93,    32,    56,    57,   102,    34,    60,    61,
     109,   146,   147,   199,   149,   196,   197,   150,   200,   201
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     135,   136,   137,   138,   257,   148,   358,   112,   113,   114,
     115,   116,   117,   118,     1,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     359,    12,    69,    13,   360,    70,    71,    90,    91,    20,
     367,    96,   361,    21,    97,    98,    99,   100,   375,    67,
     194,    22,   134,    68,   379,   195,    92,   381,    42,   383,
     101,   385,    43,   387,   368,   389,    72,   391,   369,   393,
      46,   395,   376,   397,    47,   399,   370,   401,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,   105,    24,
       3,   272,   106,   107,   273,   207,   210,     4,     5,     6,
     214,   139,   140,   108,    36,    37,    38,    39,    86,    40,
       7,   256,   211,     8,   241,   212,   213,    14,     9,   230,
     231,   232,   233,   234,   235,   236,   237,   238,   239,   159,
      25,   249,   355,   160,   250,   251,   252,   253,    74,    75,
     267,   268,   269,   270,    27,   271,   112,   113,   114,   115,
     116,   117,   118,    15,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   258,
      88,    89,    16,   259,   260,   261,   151,   152,   153,   154,
      17,   155,   139,   140,   141,   142,    50,    29,   143,   198,
      51,   134,   144,   145,   293,    54,    58,   294,   295,    55,
      59,   319,   172,   177,   185,   191,   173,   178,   186,   192,
     278,    18,   263,   279,   324,   326,   264,    94,    95,   103,
     104,   110,   111,   242,   243,   265,   266,   281,   282,   284,
     285,   287,   288,   290,   291,    31,   335,   322,   323,    19,
     112,   113,   114,   115,   116,   117,   118,   346,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   327,   328,   333,   334,   139,   140,   294,
     295,    33,   208,    62,    63,    64,   139,   140,   141,   142,
      65,   215,   143,    66,   156,   134,   144,   145,   157,   158,
      42,   162,   218,   163,   164,   165,   166,   167,   168,   169,
     170,   171,    46,   175,    50,   176,   195,   180,   181,   182,
     183,   184,    54,   188,   373,   189,   190,    58,   202,   374,
     203,   204,   205,   206,   325,   216,   219,   174,   222,   224,
     226,   221,   228,   337,   339,   240,   343,   244,   245,   246,
     247,   248,   254,   255,   262,   275,   276,   277,   298,   299,
     300,   301,   302,   303,   304,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   342,
     161,   341,   329,   330,   331,   332,   344,   362,   336,   338,
     340,   345,   347,   348,   349,   350,   351,   352,   353,   354,
     356,   357,   363,   364,   365,   366,   371,   372,   377,   378,
      35,   380,   382,   384,   386,   388,   390,   392,   394,   396,
     398,   400,   402,   320,     0,     0,   321,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   179,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   187,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   193
};

static const yytype_int16 yycheck[] =
{
      62,    63,    64,    65,   190,    66,   329,     3,     4,     5,
       6,     7,     8,     9,    42,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
     330,     0,     9,    44,   331,    12,    13,    24,    25,    43,
     363,    24,   332,    47,    27,    28,    29,    30,   371,    43,
      43,    42,    48,    47,   377,    48,    43,   380,    42,   382,
      43,   384,    46,   386,   364,   388,    43,   390,   365,   392,
      42,   394,   372,   396,    46,   398,   366,   400,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    28,    49,
       3,    46,    32,    33,    49,   156,   158,    10,    11,    12,
     162,    39,    40,    43,     4,     5,     6,     7,    43,     9,
      23,    49,     9,    26,   176,    12,    13,    44,    31,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    43,
      45,    24,   318,    47,    27,    28,    29,    30,    46,    47,
     202,   203,   204,   205,    45,   206,     3,     4,     5,     6,
       7,     8,     9,    44,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    28,
      46,    47,    44,    32,    33,    34,     4,     5,     6,     7,
      44,     9,    39,    40,    41,    42,    42,    45,    45,    46,
      46,    48,    49,    50,    46,    42,    42,    49,    50,    46,
      46,   262,    43,    43,    43,    43,    47,    47,    47,    47,
      46,    44,    43,    49,   275,   277,    47,    46,    47,    46,
      47,    46,    47,    24,    25,    46,    47,    49,    50,    49,
      50,    49,    50,    49,    50,    45,   298,    46,    47,    44,
       3,     4,     5,     6,     7,     8,     9,   309,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    46,    47,    46,    47,    39,    40,    49,
      50,    45,    45,    44,    44,    44,    39,    40,    41,    42,
      44,    49,    45,    44,    44,    48,    49,    50,    44,    44,
      42,    44,    49,    44,    44,    44,    44,    44,    44,    44,
      44,    44,    42,    44,    42,    44,    48,    44,    44,    44,
      44,    44,    42,    44,    46,    44,    44,    42,    44,    46,
      44,    44,    44,    44,   276,    45,    45,    89,    45,    45,
      45,    49,    45,   300,   302,    49,   306,    49,    49,    49,
      49,    49,    49,    49,    44,    44,    44,    44,    44,    44,
      44,    44,    44,    44,    44,    44,    44,    44,    44,    44,
      44,    44,    44,    44,    44,    44,    44,    44,    44,   305,
      75,   304,    47,    47,    47,    47,   307,   334,    49,    49,
      49,    49,    49,    49,    49,    49,    49,    49,    49,    49,
      49,    49,    47,    47,    47,    47,    47,    47,    47,    46,
      21,    47,    47,    47,    47,    47,    47,    47,    47,    47,
      47,    47,    46,   264,    -1,    -1,   266,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   104,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   111
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    42,    53,     3,    10,    11,    12,    23,    26,    31,
      54,    55,     0,    44,    44,    44,    44,    44,    44,    44,
      43,    47,    42,    56,    49,    45,    58,    45,    64,    45,
      81,    45,    85,    45,    89,    55,     4,     5,     6,     7,
       9,    57,    42,    46,    59,    60,    42,    46,    65,    66,
      42,    46,    82,    83,    42,    46,    86,    87,    42,    46,
      90,    91,    44,    44,    44,    44,    44,    43,    47,     9,
      12,    13,    43,    61,    46,    47,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    43,    67,    46,    47,
      24,    25,    43,    84,    46,    47,    24,    27,    28,    29,
      30,    43,    88,    46,    47,    28,    32,    33,    43,    92,
      46,    47,     3,     4,     5,     6,     7,     8,     9,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    48,    93,    93,    93,    93,    39,
      40,    41,    42,    45,    49,    50,    93,    94,    95,    96,
      99,     4,     5,     6,     7,     9,    44,    44,    44,    43,
      47,    60,    44,    44,    44,    44,    44,    44,    44,    44,
      44,    44,    43,    47,    66,    44,    44,    43,    47,    83,
      44,    44,    44,    44,    44,    43,    47,    87,    44,    44,
      44,    43,    47,    91,    43,    48,    97,    98,    46,    95,
     100,   101,    44,    44,    44,    44,    44,    95,    45,    62,
      93,     9,    12,    13,    93,    49,    45,    68,    49,    45,
      70,    49,    45,    72,    45,    74,    45,    76,    45,    78,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      49,    93,    24,    25,    49,    49,    49,    49,    49,    24,
      27,    28,    29,    30,    49,    49,    49,    94,    28,    32,
      33,    34,    44,    43,    47,    46,    47,    93,    93,    93,
      93,    95,    46,    49,    63,    44,    44,    44,    46,    49,
      69,    49,    50,    71,    49,    50,    73,    49,    50,    75,
      49,    50,    77,    46,    49,    50,    79,    80,    44,    44,
      44,    44,    44,    44,    44,    44,    44,    44,    44,    44,
      44,    44,    44,    44,    44,    44,    44,    44,    44,    95,
      98,   101,    46,    47,    95,    62,    93,    46,    47,    47,
      47,    47,    47,    46,    47,    93,    49,    68,    49,    70,
      49,    72,    74,    76,    78,    49,    93,    49,    49,    49,
      49,    49,    49,    49,    49,    94,    49,    49,    71,    73,
      75,    77,    80,    47,    47,    47,    47,    71,    73,    75,
      77,    47,    47,    46,    46,    71,    73,    47,    46,    71,
      47,    71,    47,    71,    47,    71,    47,    71,    47,    71,
      47,    71,    47,    71,    47,    71,    47,    71,    47,    71,
      47,    71,    46
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    52,    53,    54,    54,    55,    55,    55,    55,    55,
      55,    55,    56,    57,    57,    57,    57,    57,    57,    57,
      57,    57,    57,    58,    58,    59,    59,    60,    60,    61,
      61,    61,    61,    61,    61,    62,    62,    63,    63,    64,
      64,    65,    65,    66,    66,    67,    67,    67,    67,    67,
      67,    67,    67,    67,    67,    67,    67,    67,    67,    67,
      67,    67,    67,    67,    67,    68,    68,    69,    69,    70,
      71,    71,    72,    73,    73,    74,    75,    75,    76,    77,
      77,    78,    78,    79,    79,    80,    80,    81,    81,    82,
      82,    83,    83,    84,    84,    84,    84,    85,    85,    86,
      86,    87,    87,    88,    88,    88,    88,    88,    88,    88,
      88,    88,    88,    89,    89,    90,    90,    91,    91,    92,
      92,    92,    92,    92,    92,    92,    92,    93,    93,    93,
      93,    93,    93,    93,    93,    93,    93,    93,    93,    93,
      93,    93,    93,    93,    93,    93,    93,    93,    93,    93,
      94,    94,    95,    95,    95,    95,    95,    95,    95,    96,
      96,    97,    97,    98,    99,    99,   100,   100,   101
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
       5,     5,     5,     3,     3,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       2,     3,     1,     3,     3,     2,     3,     1,     1
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
#line 131 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                { 
	//$$ = $2;
}
#line 1950 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 3: /* gltf_members: gltf_members YYTOKEN_COMMA gltf_member  */
#line 135 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                     { 
	//bool _res_addmember = JsonParser_Json_Object_AddMember(pUserData, $1, $3._stdstring, $3._jsonvalue); 
	//JsonParser_Std_String_Dispose(pUserData, $3._stdstring);
	//if(!_res_addmember) { yyerror(&yylloc, pUserData, pScanner, "Duplicate object key"); }
	//$$ = $1;
}
#line 1961 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 4: /* gltf_members: gltf_member  */
#line 142 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                          { 
    //void *_jsonobject = JsonParser_Json_Object_Create(pUserData);
	//bool _res_addmember = JsonParser_Json_Object_AddMember(pUserData, _jsonobject, $1._stdstring, $1._jsonvalue);
	//JsonParser_Std_String_Dispose(pUserData, $1._stdstring);
	//if(!_res_addmember) { yyerror(&yylloc, pUserData, pScanner, "Duplicate object key"); }
	//$$ = _jsonobject;
}
#line 1973 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 5: /* gltf_member: YYTOKEN_ASSET YYTOKEN_COLON asset_object  */
#line 150 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                      {
	//$$ = $3;
}
#line 1981 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 6: /* gltf_member: YYTOKEN_SCENE YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 154 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                            {
	gltf_yacc_set_default_scene_index_callback((yyvsp[0].m_token_numberint), user_defined);
}
#line 1989 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 7: /* gltf_member: YYTOKEN_SCENES YYTOKEN_COLON scenes_array  */
#line 158 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                       {

}
#line 1997 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 8: /* gltf_member: YYTOKEN_NODES YYTOKEN_COLON nodes_array  */
#line 162 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                     {

}
#line 2005 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 9: /* gltf_member: YYTOKEN_BUFFERS YYTOKEN_COLON buffers_array  */
#line 166 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                         {

}
#line 2013 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 10: /* gltf_member: YYTOKEN_BUFFERVIEWS YYTOKEN_COLON bufferviews_array  */
#line 170 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                 {

}
#line 2021 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 11: /* gltf_member: YYTOKEN_ACCESSORS YYTOKEN_COLON accessors_array  */
#line 174 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                             {

}
#line 2029 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 12: /* asset_object: YYTOKEN_LEFTBRACE asset_properties YYTOKEN_RIGHTBRACE  */
#line 178 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                     { 
	
}
#line 2037 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 13: /* asset_properties: asset_properties YYTOKEN_COMMA YYTOKEN_COPYRIGHT YYTOKEN_COLON json_string  */
#line 182 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                             { 

}
#line 2045 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 14: /* asset_properties: asset_properties YYTOKEN_COMMA YYTOKEN_GENERATOR YYTOKEN_COLON json_string  */
#line 186 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                             { 

}
#line 2053 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 15: /* asset_properties: asset_properties YYTOKEN_COMMA YYTOKEN_VERSION YYTOKEN_COLON json_string  */
#line 190 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                           { 

}
#line 2061 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 16: /* asset_properties: asset_properties YYTOKEN_COMMA YYTOKEN_MINVERSION YYTOKEN_COLON json_string  */
#line 194 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                              { 

}
#line 2069 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 17: /* asset_properties: asset_properties YYTOKEN_COMMA YYTOKEN_EXTRAS YYTOKEN_COLON json_value  */
#line 198 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                         { 

}
#line 2077 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 18: /* asset_properties: YYTOKEN_COPYRIGHT YYTOKEN_COLON json_string  */
#line 202 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                              { 

}
#line 2085 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 19: /* asset_properties: YYTOKEN_GENERATOR YYTOKEN_COLON json_string  */
#line 206 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                              { 

}
#line 2093 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 20: /* asset_properties: YYTOKEN_VERSION YYTOKEN_COLON json_string  */
#line 210 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                            { 

}
#line 2101 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 21: /* asset_properties: YYTOKEN_MINVERSION YYTOKEN_COLON json_string  */
#line 214 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                               { 

}
#line 2109 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 22: /* asset_properties: YYTOKEN_EXTRAS YYTOKEN_COLON json_value  */
#line 218 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                          { 

}
#line 2117 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 23: /* scenes_array: YYTOKEN_LEFTBRACKET scene_objects YYTOKEN_RIGHTBRACKET  */
#line 222 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                     { 

}
#line 2125 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 24: /* scenes_array: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 226 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                       {

}
#line 2133 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 25: /* scene_objects: scene_objects YYTOKEN_COMMA scene_object  */
#line 230 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                        { 
	assert(-1 == (yyvsp[0].m_scene_index) || gltf_yacc_scene_size_callback(user_defined) == ((yyvsp[0].m_scene_index) + 1));
}
#line 2141 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 26: /* scene_objects: scene_object  */
#line 234 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                            {
	assert(-1 == (yyvsp[0].m_scene_index) || gltf_yacc_scene_size_callback(user_defined) == ((yyvsp[0].m_scene_index) + 1));
}
#line 2149 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 27: /* scene_object: YYTOKEN_LEFTBRACE scene_properties YYTOKEN_RIGHTBRACE  */
#line 238 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                    {
	(yyval.m_scene_index) = (yyvsp[-1].m_scene_index);
}
#line 2157 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 28: /* scene_object: YYTOKEN_LEFTBRACE YYTOKEN_RIGHTBRACE  */
#line 242 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                   { 
	(yyval.m_scene_index) = -1;
}
#line 2165 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 29: /* scene_properties: scene_properties YYTOKEN_COMMA YYTOKEN_NODES YYTOKEN_COLON scene_property_nodes  */
#line 246 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                  {
	gltf_yacc_scene_set_nodes_callback((yyval.m_scene_index), (yyvsp[0].m_temp_int_array_version), user_defined);
	gltf_yacc_temp_int_array_destroy_callback((yyvsp[0].m_temp_int_array_version), user_defined);
}
#line 2174 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 30: /* scene_properties: scene_properties YYTOKEN_COMMA YYTOKEN_NAME YYTOKEN_COLON json_string  */
#line 251 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                        {
	gltf_yacc_scene_set_name_callback((yyval.m_scene_index), (yyvsp[0].m_token_string).m_data, (yyvsp[0].m_token_string).m_size, user_defined);
}
#line 2182 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 31: /* scene_properties: scene_properties YYTOKEN_COMMA YYTOKEN_EXTRAS YYTOKEN_COLON json_value  */
#line 255 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                         { 
	// ignore
}
#line 2190 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 32: /* scene_properties: YYTOKEN_NODES YYTOKEN_COLON scene_property_nodes  */
#line 259 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                   { 
	(yyval.m_scene_index) = gltf_yacc_scene_push_callback(user_defined);
	gltf_yacc_scene_set_nodes_callback((yyval.m_scene_index), (yyvsp[0].m_temp_int_array_version), user_defined);
	gltf_yacc_temp_int_array_destroy_callback((yyvsp[0].m_temp_int_array_version), user_defined);
}
#line 2200 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 33: /* scene_properties: YYTOKEN_NAME YYTOKEN_COLON json_string  */
#line 265 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                         {
	(yyval.m_scene_index) = gltf_yacc_scene_push_callback(user_defined);
	gltf_yacc_scene_set_name_callback((yyval.m_scene_index), (yyvsp[0].m_token_string).m_data, (yyvsp[0].m_token_string).m_size, user_defined);
}
#line 2209 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 34: /* scene_properties: YYTOKEN_EXTRAS YYTOKEN_COLON json_value  */
#line 270 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                          { 

}
#line 2217 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 35: /* scene_property_nodes: YYTOKEN_LEFTBRACKET scene_property_nodes_elements YYTOKEN_RIGHTBRACKET  */
#line 274 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                             {
	(yyval.m_temp_int_array_version) = (yyvsp[-1].m_temp_int_array_version);
}
#line 2225 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 36: /* scene_property_nodes: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 278 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                               { 
	(yyval.m_temp_int_array_version) = gltf_yacc_temp_int_array_init_callback(user_defined);
}
#line 2233 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 37: /* scene_property_nodes_elements: scene_property_nodes_elements YYTOKEN_COMMA YYTOKEN_NUMBER_INT  */
#line 282 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                              {
	gltf_yacc_temp_int_array_push_callback((yyval.m_temp_int_array_version), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2241 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 38: /* scene_property_nodes_elements: YYTOKEN_NUMBER_INT  */
#line 286 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                   {
	(yyval.m_temp_int_array_version) = gltf_yacc_temp_int_array_init_callback(user_defined);
	gltf_yacc_temp_int_array_push_callback((yyval.m_temp_int_array_version), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2250 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 39: /* nodes_array: YYTOKEN_LEFTBRACKET node_objects YYTOKEN_RIGHTBRACKET  */
#line 291 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                   {

}
#line 2258 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 40: /* nodes_array: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 295 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                      {

}
#line 2266 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 41: /* node_objects: node_objects YYTOKEN_COMMA node_object  */
#line 299 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                     { 
   	assert(-1 == (yyvsp[0].m_node_index) || gltf_yacc_node_size_callback(user_defined) == ((yyvsp[0].m_node_index) + 1));
}
#line 2274 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 42: /* node_objects: node_object  */
#line 303 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                          { 
	assert(-1 == (yyvsp[0].m_node_index) || gltf_yacc_node_size_callback(user_defined) == ((yyvsp[0].m_node_index) + 1));
}
#line 2282 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 43: /* node_object: YYTOKEN_LEFTBRACE node_properties YYTOKEN_RIGHTBRACE  */
#line 307 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                  {
	(yyval.m_node_index) = (yyvsp[-1].m_node_index);
}
#line 2290 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 44: /* node_object: YYTOKEN_LEFTBRACE YYTOKEN_RIGHTBRACE  */
#line 311 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                  { 
	(yyval.m_node_index) = -1;
}
#line 2298 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 45: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_CAMERA YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 315 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                               {
	gltf_yacc_node_set_camera_callback((yyval.m_node_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2306 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 46: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_CHILDREN YYTOKEN_COLON node_property_children  */
#line 319 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                     {
	gltf_yacc_node_set_children_callback((yyval.m_node_index), (yyvsp[0].m_temp_int_array_version), user_defined);
	gltf_yacc_temp_int_array_destroy_callback((yyvsp[0].m_temp_int_array_version), user_defined);
}
#line 2315 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 47: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_SKIN YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 324 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                             {
	gltf_yacc_node_set_skin_callback((yyval.m_node_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2323 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 48: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_MATRIX YYTOKEN_COLON node_property_matrix  */
#line 328 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                 {
	gltf_yacc_node_set_matrix_callback((yyval.m_node_index), (yyvsp[0].m_mat4x4), user_defined);
}
#line 2331 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 49: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_MESH YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 332 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                             {
	gltf_yacc_node_set_mesh_callback((yyval.m_node_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2339 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 50: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_ROTATION YYTOKEN_COLON node_property_rotation  */
#line 336 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                     {
	gltf_yacc_node_set_rotation_callback((yyval.m_node_index), (yyvsp[0].m_vec4), user_defined);
}
#line 2347 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 51: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_SCALE YYTOKEN_COLON node_property_scale  */
#line 340 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                               {
	gltf_yacc_node_set_scale_callback((yyval.m_node_index), (yyvsp[0].m_vec3), user_defined);
}
#line 2355 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 52: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_TRANSLATION YYTOKEN_COLON node_property_translation  */
#line 344 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                           {
	gltf_yacc_node_set_translation_callback((yyval.m_node_index), (yyvsp[0].m_vec3), user_defined);
}
#line 2363 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 53: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_WEIGHTS YYTOKEN_COLON node_property_weights  */
#line 348 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                   {
	gltf_yacc_node_set_weights_callback((yyval.m_node_index), (yyvsp[0].m_temp_float_array_version), user_defined);
	gltf_yacc_temp_float_array_destroy_callback((yyvsp[0].m_temp_float_array_version), user_defined);
}
#line 2372 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 54: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_NAME YYTOKEN_COLON json_string  */
#line 353 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                      {
	gltf_yacc_node_set_name_callback((yyval.m_node_index), (yyvsp[0].m_token_string).m_data, (yyvsp[0].m_token_string).m_size, user_defined);
}
#line 2380 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 55: /* node_properties: YYTOKEN_CAMERA YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 357 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                 {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_camera_callback((yyval.m_node_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2389 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 56: /* node_properties: YYTOKEN_CHILDREN YYTOKEN_COLON node_property_children  */
#line 362 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                       {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_children_callback((yyval.m_node_index), (yyvsp[0].m_temp_int_array_version), user_defined);
	gltf_yacc_temp_int_array_destroy_callback((yyvsp[0].m_temp_int_array_version), user_defined);
}
#line 2399 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 57: /* node_properties: YYTOKEN_SKIN YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 368 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                               {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_skin_callback((yyval.m_node_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2408 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 58: /* node_properties: YYTOKEN_MATRIX YYTOKEN_COLON node_property_matrix  */
#line 373 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                   {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_matrix_callback((yyval.m_node_index), (yyvsp[0].m_mat4x4), user_defined);
}
#line 2417 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 59: /* node_properties: YYTOKEN_MESH YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 378 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                               {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_mesh_callback((yyval.m_node_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2426 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 60: /* node_properties: YYTOKEN_ROTATION YYTOKEN_COLON node_property_rotation  */
#line 383 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                       {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_rotation_callback((yyval.m_node_index), (yyvsp[0].m_vec4), user_defined);
}
#line 2435 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 61: /* node_properties: YYTOKEN_SCALE YYTOKEN_COLON node_property_scale  */
#line 388 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                 {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_scale_callback((yyval.m_node_index), (yyvsp[0].m_vec3), user_defined);
}
#line 2444 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 62: /* node_properties: YYTOKEN_TRANSLATION YYTOKEN_COLON node_property_translation  */
#line 393 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                             {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_translation_callback((yyval.m_node_index), (yyvsp[0].m_vec3), user_defined);
}
#line 2453 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 63: /* node_properties: YYTOKEN_WEIGHTS YYTOKEN_COLON node_property_weights  */
#line 398 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                     {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_weights_callback((yyval.m_node_index), (yyvsp[0].m_temp_float_array_version), user_defined);
	gltf_yacc_temp_float_array_destroy_callback((yyvsp[0].m_temp_float_array_version), user_defined);
}
#line 2463 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 64: /* node_properties: YYTOKEN_NAME YYTOKEN_COLON json_string  */
#line 404 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                        {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_name_callback((yyval.m_node_index), (yyvsp[0].m_token_string).m_data, (yyvsp[0].m_token_string).m_size, user_defined);
}
#line 2472 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 65: /* node_property_children: YYTOKEN_LEFTBRACKET node_property_children_elements YYTOKEN_RIGHTBRACKET  */
#line 409 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                 {
	(yyval.m_temp_int_array_version) = (yyvsp[-1].m_temp_int_array_version);
}
#line 2480 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 66: /* node_property_children: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 413 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                 { 
    (yyval.m_temp_int_array_version) = gltf_yacc_temp_int_array_init_callback(user_defined);
}
#line 2488 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 67: /* node_property_children_elements: node_property_children_elements YYTOKEN_COMMA YYTOKEN_NUMBER_INT  */
#line 417 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                  {
	gltf_yacc_temp_int_array_push_callback((yyval.m_temp_int_array_version), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2496 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 68: /* node_property_children_elements: YYTOKEN_NUMBER_INT  */
#line 421 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                     {
	(yyval.m_temp_int_array_version) = gltf_yacc_temp_int_array_init_callback(user_defined);
	gltf_yacc_temp_int_array_push_callback((yyval.m_temp_int_array_version), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2505 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 69: /* node_property_matrix: YYTOKEN_LEFTBRACKET node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_RIGHTBRACKET  */
#line 426 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
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
#line 2528 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 70: /* node_property_matrix_element: YYTOKEN_NUMBER_FLOAT  */
#line 445 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                   {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberfloat);
}
#line 2536 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 71: /* node_property_matrix_element: YYTOKEN_NUMBER_INT  */
#line 449 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                 {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberint);
}
#line 2544 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 72: /* node_property_rotation: YYTOKEN_LEFTBRACKET node_property_rotation_element YYTOKEN_COMMA node_property_rotation_element YYTOKEN_COMMA node_property_rotation_element YYTOKEN_COMMA node_property_rotation_element YYTOKEN_RIGHTBRACKET  */
#line 453 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                                                                                                                                                       {
	(yyval.m_vec4)[0] = (yyvsp[-7].m_token_numberfloat);
	(yyval.m_vec4)[1] = (yyvsp[-5].m_token_numberfloat);
	(yyval.m_vec4)[2] = (yyvsp[-3].m_token_numberfloat);
	(yyval.m_vec4)[3] = (yyvsp[-1].m_token_numberfloat);
}
#line 2555 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 73: /* node_property_rotation_element: YYTOKEN_NUMBER_FLOAT  */
#line 460 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                     {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberfloat);
}
#line 2563 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 74: /* node_property_rotation_element: YYTOKEN_NUMBER_INT  */
#line 464 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                   {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberint);
}
#line 2571 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 75: /* node_property_scale: YYTOKEN_LEFTBRACKET node_property_scale_element YYTOKEN_COMMA node_property_scale_element YYTOKEN_COMMA node_property_scale_element YYTOKEN_RIGHTBRACKET  */
#line 468 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                                                                                              {
	(yyval.m_vec3)[0] = (yyvsp[-5].m_token_numberfloat);
	(yyval.m_vec3)[1] = (yyvsp[-3].m_token_numberfloat);
	(yyval.m_vec3)[2] = (yyvsp[-1].m_token_numberfloat);
}
#line 2581 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 76: /* node_property_scale_element: YYTOKEN_NUMBER_FLOAT  */
#line 474 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                  {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberfloat);
}
#line 2589 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 77: /* node_property_scale_element: YYTOKEN_NUMBER_INT  */
#line 478 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberint);
}
#line 2597 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 78: /* node_property_translation: YYTOKEN_LEFTBRACKET node_property_translation_element YYTOKEN_COMMA node_property_translation_element YYTOKEN_COMMA node_property_translation_element YYTOKEN_RIGHTBRACKET  */
#line 482 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                                                                                                                      {
	(yyval.m_vec3)[0] = (yyvsp[-5].m_token_numberfloat);
	(yyval.m_vec3)[1] = (yyvsp[-3].m_token_numberfloat);
	(yyval.m_vec3)[2] = (yyvsp[-1].m_token_numberfloat);
}
#line 2607 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 79: /* node_property_translation_element: YYTOKEN_NUMBER_FLOAT  */
#line 488 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                        {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberfloat);
}
#line 2615 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 80: /* node_property_translation_element: YYTOKEN_NUMBER_INT  */
#line 492 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                      {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberint);
}
#line 2623 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 81: /* node_property_weights: YYTOKEN_LEFTBRACKET node_property_weights_elements YYTOKEN_RIGHTBRACKET  */
#line 496 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                               {
	(yyval.m_temp_float_array_version) = (yyvsp[-1].m_temp_float_array_version);
}
#line 2631 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 82: /* node_property_weights: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 500 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                { 
    (yyval.m_temp_float_array_version) = gltf_yacc_temp_float_array_init_callback(user_defined);
}
#line 2639 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 83: /* node_property_weights_elements: node_property_weights_elements YYTOKEN_COMMA node_property_weights_element  */
#line 504 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                           {
	gltf_yacc_temp_float_array_push_callback((yyval.m_temp_float_array_version), (yyvsp[0].m_token_numberfloat), user_defined);
}
#line 2647 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 84: /* node_property_weights_elements: node_property_weights_element  */
#line 508 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                               {
	(yyval.m_temp_float_array_version) = gltf_yacc_temp_float_array_init_callback(user_defined);
	gltf_yacc_temp_float_array_push_callback((yyval.m_temp_float_array_version), (yyvsp[0].m_token_numberfloat), user_defined);
}
#line 2656 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 85: /* node_property_weights_element: YYTOKEN_NUMBER_FLOAT  */
#line 514 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                    {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberfloat);
}
#line 2664 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 86: /* node_property_weights_element: YYTOKEN_NUMBER_INT  */
#line 518 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                  {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberint);
}
#line 2672 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 87: /* buffers_array: YYTOKEN_LEFTBRACKET buffer_objects YYTOKEN_RIGHTBRACKET  */
#line 523 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                       {

}
#line 2680 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 88: /* buffers_array: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 527 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                        {

}
#line 2688 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 89: /* buffer_objects: buffer_objects YYTOKEN_COMMA buffer_object  */
#line 531 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                           { 
   	assert(-1 == (yyvsp[0].m_buffer_index) || gltf_yacc_buffer_size_callback(user_defined) == ((yyvsp[0].m_buffer_index) + 1));
}
#line 2696 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 90: /* buffer_objects: buffer_object  */
#line 535 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                              { 
	assert(-1 == (yyvsp[0].m_buffer_index) || gltf_yacc_buffer_size_callback(user_defined) == ((yyvsp[0].m_buffer_index) + 1));
}
#line 2704 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 91: /* buffer_object: YYTOKEN_LEFTBRACE buffer_properties YYTOKEN_RIGHTBRACE  */
#line 539 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                      {
	(yyval.m_buffer_index) = (yyvsp[-1].m_buffer_index);
}
#line 2712 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 92: /* buffer_object: YYTOKEN_LEFTBRACE YYTOKEN_RIGHTBRACE  */
#line 543 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                    { 
	(yyval.m_buffer_index) = -1;
}
#line 2720 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 93: /* buffer_properties: buffer_properties YYTOKEN_COMMA YYTOKEN_BYTELENGTH YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 547 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                       {
	gltf_yacc_buffer_set_bytelength_callback((yyval.m_buffer_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2728 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 94: /* buffer_properties: buffer_properties YYTOKEN_COMMA YYTOKEN_URI YYTOKEN_COLON json_string  */
#line 551 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                         {
	gltf_yacc_buffer_set_url_callback((yyval.m_buffer_index), (yyvsp[0].m_token_string).m_data, (yyvsp[0].m_token_string).m_size, user_defined);
}
#line 2736 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 95: /* buffer_properties: YYTOKEN_BYTELENGTH YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 556 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                       {
	(yyval.m_buffer_index) = gltf_yacc_buffer_push_callback(user_defined);
	gltf_yacc_buffer_set_bytelength_callback((yyval.m_buffer_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2745 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 96: /* buffer_properties: YYTOKEN_URI YYTOKEN_COLON json_string  */
#line 561 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                         {
	(yyval.m_buffer_index) = gltf_yacc_buffer_push_callback(user_defined);
	gltf_yacc_buffer_set_url_callback((yyval.m_buffer_index), (yyvsp[0].m_token_string).m_data, (yyvsp[0].m_token_string).m_size, user_defined);
}
#line 2754 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 97: /* bufferviews_array: YYTOKEN_LEFTBRACKET bufferview_objects YYTOKEN_RIGHTBRACKET  */
#line 566 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                               {

}
#line 2762 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 98: /* bufferviews_array: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 570 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                            {

}
#line 2770 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 99: /* bufferview_objects: bufferview_objects YYTOKEN_COMMA bufferview_object  */
#line 574 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                       { 
   	assert(-1 == (yyvsp[0].m_bufferview_index) || gltf_yacc_bufferview_size_callback(user_defined) == ((yyvsp[0].m_bufferview_index) + 1));
}
#line 2778 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 100: /* bufferview_objects: bufferview_object  */
#line 578 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                      { 
	assert(-1 == (yyvsp[0].m_bufferview_index) || gltf_yacc_bufferview_size_callback(user_defined) == ((yyvsp[0].m_bufferview_index) + 1));
}
#line 2786 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 101: /* bufferview_object: YYTOKEN_LEFTBRACE bufferview_properties YYTOKEN_RIGHTBRACE  */
#line 582 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                              {
	(yyval.m_bufferview_index) = (yyvsp[-1].m_bufferview_index);
}
#line 2794 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 102: /* bufferview_object: YYTOKEN_LEFTBRACE YYTOKEN_RIGHTBRACE  */
#line 586 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                        { 
	(yyval.m_bufferview_index) = -1;
}
#line 2802 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 103: /* bufferview_properties: bufferview_properties YYTOKEN_COMMA YYTOKEN_BUFFER YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 590 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                           {
	gltf_yacc_bufferview_set_buffer_callback((yyval.m_bufferview_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2810 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 104: /* bufferview_properties: bufferview_properties YYTOKEN_COMMA YYTOKEN_BYTEOFFSET YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 594 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                               {
	gltf_yacc_bufferview_set_byteoffset_callback((yyval.m_bufferview_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2818 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 105: /* bufferview_properties: bufferview_properties YYTOKEN_COMMA YYTOKEN_BYTELENGTH YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 598 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                               {
	gltf_yacc_bufferview_set_bytelength_callback((yyval.m_bufferview_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2826 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 106: /* bufferview_properties: bufferview_properties YYTOKEN_COMMA YYTOKEN_BYTESTRIDE YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 602 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                               {
	gltf_yacc_bufferview_set_bytestride_callback((yyval.m_bufferview_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2834 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 107: /* bufferview_properties: bufferview_properties YYTOKEN_COMMA YYTOKEN_TARGET YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 606 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                           {
	gltf_yacc_bufferview_set_target_callback((yyval.m_bufferview_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2842 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 108: /* bufferview_properties: YYTOKEN_BUFFER YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 610 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                       {
	(yyval.m_bufferview_index) = gltf_yacc_bufferview_push_callback(user_defined);
	gltf_yacc_bufferview_set_buffer_callback((yyval.m_bufferview_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2851 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 109: /* bufferview_properties: YYTOKEN_BYTEOFFSET YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 615 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                           {
	(yyval.m_bufferview_index) = gltf_yacc_bufferview_push_callback(user_defined);
	gltf_yacc_bufferview_set_byteoffset_callback((yyval.m_bufferview_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2860 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 110: /* bufferview_properties: YYTOKEN_BYTELENGTH YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 620 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                           {
	(yyval.m_bufferview_index) = gltf_yacc_bufferview_push_callback(user_defined);
	gltf_yacc_bufferview_set_bytelength_callback((yyval.m_bufferview_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2869 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 111: /* bufferview_properties: YYTOKEN_BYTESTRIDE YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 625 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                           {
	(yyval.m_bufferview_index) = gltf_yacc_bufferview_push_callback(user_defined);
	gltf_yacc_bufferview_set_bytestride_callback((yyval.m_bufferview_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2878 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 112: /* bufferview_properties: YYTOKEN_TARGET YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 630 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                       {
	(yyval.m_bufferview_index) = gltf_yacc_bufferview_push_callback(user_defined);
	gltf_yacc_bufferview_set_target_callback((yyval.m_bufferview_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2887 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 113: /* accessors_array: YYTOKEN_LEFTBRACKET accessor_objects YYTOKEN_RIGHTBRACKET  */
#line 635 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                           {

}
#line 2895 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 114: /* accessors_array: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 639 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                          {

}
#line 2903 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 115: /* accessor_objects: accessor_objects YYTOKEN_COMMA accessor_object  */
#line 643 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                 { 
   	assert(-1 == (yyvsp[0].m_accessor_index) || gltf_yacc_accessor_size_callback(user_defined) == ((yyvsp[0].m_accessor_index) + 1));
}
#line 2911 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 116: /* accessor_objects: accessor_object  */
#line 647 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                  { 
	assert(-1 == (yyvsp[0].m_accessor_index) || gltf_yacc_accessor_size_callback(user_defined) == ((yyvsp[0].m_accessor_index) + 1));
}
#line 2919 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 117: /* accessor_object: YYTOKEN_LEFTBRACE accessor_properties YYTOKEN_RIGHTBRACE  */
#line 651 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                          {
	(yyval.m_accessor_index) = (yyvsp[-1].m_accessor_index);
}
#line 2927 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 118: /* accessor_object: YYTOKEN_LEFTBRACE YYTOKEN_RIGHTBRACE  */
#line 655 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                      { 
	(yyval.m_accessor_index) = -1;
}
#line 2935 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 119: /* accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_BUFFERVIEW YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 659 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                           {
	gltf_yacc_accessor_set_bufferview_callback((yyval.m_accessor_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2943 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 120: /* accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_BYTEOFFSET YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 663 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                           {
	gltf_yacc_accessor_set_byteoffset_callback((yyval.m_accessor_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2951 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 121: /* accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_COMPONENTTYPE YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 667 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                              {
	gltf_yacc_accessor_set_componenttype_callback((yyval.m_accessor_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2959 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 122: /* accessor_properties: accessor_properties YYTOKEN_COMMA YYTOKEN_NORMALIZED YYTOKEN_COLON json_boolean  */
#line 671 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                     {
	gltf_yacc_accessor_set_normalized_callback((yyval.m_accessor_index), (yyvsp[0].m_type_boolean), user_defined);
}
#line 2967 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 123: /* accessor_properties: YYTOKEN_BUFFERVIEW YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 675 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                         {
	(yyval.m_accessor_index) = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_bufferview_callback((yyval.m_accessor_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2976 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 124: /* accessor_properties: YYTOKEN_BYTEOFFSET YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 680 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                         {
	(yyval.m_accessor_index) = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_byteoffset_callback((yyval.m_accessor_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2985 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 125: /* accessor_properties: YYTOKEN_COMPONENTTYPE YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 685 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                            {
	(yyval.m_accessor_index) = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_componenttype_callback((yyval.m_accessor_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2994 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 126: /* accessor_properties: YYTOKEN_COMPONENTTYPE YYTOKEN_COLON json_boolean  */
#line 690 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                      {
	(yyval.m_accessor_index) = gltf_yacc_accessor_push_callback(user_defined);
	gltf_yacc_accessor_set_normalized_callback((yyval.m_accessor_index), (yyvsp[0].m_type_boolean), user_defined);
}
#line 3003 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 127: /* json_string: YYTOKEN_STRING  */
#line 695 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                            {

}
#line 3011 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 128: /* json_string: YYTOKEN_ASSET  */
#line 699 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                           {

}
#line 3019 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 129: /* json_string: YYTOKEN_COPYRIGHT  */
#line 703 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                               {

}
#line 3027 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 130: /* json_string: YYTOKEN_GENERATOR  */
#line 707 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                               {

}
#line 3035 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 131: /* json_string: YYTOKEN_VERSION  */
#line 711 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                             {

}
#line 3043 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 132: /* json_string: YYTOKEN_MINVERSION  */
#line 715 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                {

}
#line 3051 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 133: /* json_string: YYTOKEN_EXTENSIONS  */
#line 719 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                {

}
#line 3059 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 134: /* json_string: YYTOKEN_EXTRAS  */
#line 723 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                            {

}
#line 3067 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 135: /* json_string: YYTOKEN_SCENES  */
#line 727 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                            {

}
#line 3075 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 136: /* json_string: YYTOKEN_NODES  */
#line 731 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                           {

}
#line 3083 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 137: /* json_string: YYTOKEN_NAME  */
#line 735 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                          {

}
#line 3091 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 138: /* json_string: YYTOKEN_CAMERA  */
#line 739 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                            {

}
#line 3099 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 139: /* json_string: YYTOKEN_CHILDREN  */
#line 743 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                              {

}
#line 3107 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 140: /* json_string: YYTOKEN_SKIN  */
#line 747 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                          {

}
#line 3115 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 141: /* json_string: YYTOKEN_MATRIX  */
#line 751 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                            {

}
#line 3123 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 142: /* json_string: YYTOKEN_MESH  */
#line 755 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                          {

}
#line 3131 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 143: /* json_string: YYTOKEN_ROTATION  */
#line 759 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                              {

}
#line 3139 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 144: /* json_string: YYTOKEN_SCALE  */
#line 763 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                           {

}
#line 3147 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 145: /* json_string: YYTOKEN_TRANSLATION  */
#line 767 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                 {

}
#line 3155 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 146: /* json_string: YYTOKEN_WEIGHTS  */
#line 771 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                             {

}
#line 3163 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 147: /* json_string: YYTOKEN_BUFFERS  */
#line 775 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                             {

}
#line 3171 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 148: /* json_string: YYTOKEN_BYTELENGTH  */
#line 779 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                {

}
#line 3179 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 149: /* json_string: YYTOKEN_URI  */
#line 783 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                         {

}
#line 3187 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 150: /* json_boolean: YYTOKEN_TRUE  */
#line 787 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                           {
	(yyval.m_type_boolean) = true;
}
#line 3195 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 151: /* json_boolean: YYTOKEN_FALSE  */
#line 791 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                            {
	(yyval.m_type_boolean) = false;
}
#line 3203 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 152: /* json_value: json_object  */
#line 795 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                        { 

}
#line 3211 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 153: /* json_value: json_array  */
#line 799 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                       { 

}
#line 3219 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 154: /* json_value: json_string  */
#line 803 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                        { 

}
#line 3227 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 155: /* json_value: YYTOKEN_NUMBER_INT  */
#line 807 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                               { 

}
#line 3235 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 156: /* json_value: YYTOKEN_NUMBER_FLOAT  */
#line 811 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                 { 

}
#line 3243 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 157: /* json_value: json_boolean  */
#line 815 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                         { 

}
#line 3251 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 158: /* json_value: YYTOKEN_NULL  */
#line 819 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                         { 

}
#line 3259 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 159: /* json_object: YYTOKEN_LEFTBRACE json_members YYTOKEN_RIGHTBRACE  */
#line 823 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                               { 

}
#line 3267 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 160: /* json_object: YYTOKEN_LEFTBRACE YYTOKEN_RIGHTBRACE  */
#line 827 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                  { 

}
#line 3275 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 161: /* json_members: json_members YYTOKEN_COMMA json_member  */
#line 831 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                     { 

}
#line 3283 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 162: /* json_members: json_member  */
#line 835 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                          { 

}
#line 3291 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 163: /* json_member: YYTOKEN_STRING YYTOKEN_COLON json_value  */
#line 839 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                     { 

}
#line 3299 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 164: /* json_array: YYTOKEN_LEFTBRACKET json_elements YYTOKEN_RIGHTBRACKET  */
#line 843 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                   { 

}
#line 3307 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 165: /* json_array: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 847 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                     { 

}
#line 3315 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 166: /* json_elements: json_elements YYTOKEN_COMMA json_element  */
#line 851 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                        { 

}
#line 3323 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 167: /* json_elements: json_element  */
#line 855 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                            { 

}
#line 3331 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 168: /* json_element: json_value  */
#line 859 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                         {
	
}
#line 3339 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;


#line 3343 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"

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

#line 863 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"

