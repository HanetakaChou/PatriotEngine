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
    YYTOKEN_TRUE = 278,            /* YYTOKEN_TRUE  */
    YYTOKEN_FALSE = 279,           /* YYTOKEN_FALSE  */
    YYTOKEN_NULL = 280,            /* YYTOKEN_NULL  */
    YYTOKEN_LEFTBRACE = 281,       /* YYTOKEN_LEFTBRACE  */
    YYTOKEN_RIGHTBRACE = 282,      /* YYTOKEN_RIGHTBRACE  */
    YYTOKEN_COLON = 283,           /* YYTOKEN_COLON  */
    YYTOKEN_LEFTBRACKET = 284,     /* YYTOKEN_LEFTBRACKET  */
    YYTOKEN_RIGHTBRACKET = 285,    /* YYTOKEN_RIGHTBRACKET  */
    YYTOKEN_COMMA = 286,           /* YYTOKEN_COMMA  */
    YYTOKEN_STRING = 287,          /* YYTOKEN_STRING  */
    YYTOKEN_NUMBER_INT = 288,      /* YYTOKEN_NUMBER_INT  */
    YYTOKEN_NUMBER_FLOAT = 289,    /* YYTOKEN_NUMBER_FLOAT  */
    PSEUDO_LEX_ERROR = 290         /* PSEUDO_LEX_ERROR  */
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
  YYSYMBOL_YYTOKEN_TRUE = 23,              /* YYTOKEN_TRUE  */
  YYSYMBOL_YYTOKEN_FALSE = 24,             /* YYTOKEN_FALSE  */
  YYSYMBOL_YYTOKEN_NULL = 25,              /* YYTOKEN_NULL  */
  YYSYMBOL_YYTOKEN_LEFTBRACE = 26,         /* YYTOKEN_LEFTBRACE  */
  YYSYMBOL_YYTOKEN_RIGHTBRACE = 27,        /* YYTOKEN_RIGHTBRACE  */
  YYSYMBOL_YYTOKEN_COLON = 28,             /* YYTOKEN_COLON  */
  YYSYMBOL_YYTOKEN_LEFTBRACKET = 29,       /* YYTOKEN_LEFTBRACKET  */
  YYSYMBOL_YYTOKEN_RIGHTBRACKET = 30,      /* YYTOKEN_RIGHTBRACKET  */
  YYSYMBOL_YYTOKEN_COMMA = 31,             /* YYTOKEN_COMMA  */
  YYSYMBOL_YYTOKEN_STRING = 32,            /* YYTOKEN_STRING  */
  YYSYMBOL_YYTOKEN_NUMBER_INT = 33,        /* YYTOKEN_NUMBER_INT  */
  YYSYMBOL_YYTOKEN_NUMBER_FLOAT = 34,      /* YYTOKEN_NUMBER_FLOAT  */
  YYSYMBOL_PSEUDO_LEX_ERROR = 35,          /* PSEUDO_LEX_ERROR  */
  YYSYMBOL_YYACCEPT = 36,                  /* $accept  */
  YYSYMBOL_gltf_object = 37,               /* gltf_object  */
  YYSYMBOL_gltf_members = 38,              /* gltf_members  */
  YYSYMBOL_gltf_member = 39,               /* gltf_member  */
  YYSYMBOL_asset_object = 40,              /* asset_object  */
  YYSYMBOL_asset_members = 41,             /* asset_members  */
  YYSYMBOL_asset_member = 42,              /* asset_member  */
  YYSYMBOL_scenes_array = 43,              /* scenes_array  */
  YYSYMBOL_scene_objects = 44,             /* scene_objects  */
  YYSYMBOL_scene_object = 45,              /* scene_object  */
  YYSYMBOL_scene_properties = 46,          /* scene_properties  */
  YYSYMBOL_scene_property_nodes = 47,      /* scene_property_nodes  */
  YYSYMBOL_scene_property_nodes_elements = 48, /* scene_property_nodes_elements  */
  YYSYMBOL_nodes_array = 49,               /* nodes_array  */
  YYSYMBOL_node_objects = 50,              /* node_objects  */
  YYSYMBOL_node_object = 51,               /* node_object  */
  YYSYMBOL_node_properties = 52,           /* node_properties  */
  YYSYMBOL_node_property_children = 53,    /* node_property_children  */
  YYSYMBOL_node_property_children_elements = 54, /* node_property_children_elements  */
  YYSYMBOL_node_property_matrix = 55,      /* node_property_matrix  */
  YYSYMBOL_node_property_matrix_element = 56, /* node_property_matrix_element  */
  YYSYMBOL_node_property_rotation = 57,    /* node_property_rotation  */
  YYSYMBOL_node_property_rotation_element = 58, /* node_property_rotation_element  */
  YYSYMBOL_node_property_scale = 59,       /* node_property_scale  */
  YYSYMBOL_node_property_scale_element = 60, /* node_property_scale_element  */
  YYSYMBOL_node_property_translation = 61, /* node_property_translation  */
  YYSYMBOL_node_property_translation_element = 62, /* node_property_translation_element  */
  YYSYMBOL_node_property_weights = 63,     /* node_property_weights  */
  YYSYMBOL_node_property_weights_elements = 64, /* node_property_weights_elements  */
  YYSYMBOL_json_string = 65,               /* json_string  */
  YYSYMBOL_json_value = 66,                /* json_value  */
  YYSYMBOL_json_object = 67,               /* json_object  */
  YYSYMBOL_json_members = 68,              /* json_members  */
  YYSYMBOL_json_member = 69,               /* json_member  */
  YYSYMBOL_json_array = 70,                /* json_array  */
  YYSYMBOL_json_elements = 71,             /* json_elements  */
  YYSYMBOL_json_element = 72               /* json_element  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 13 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"

    int yylex(union YYSTYPE *lvalp, YYLTYPE *llocp, void *user_defined, void *yyscanner);
    void yyerror(YYLTYPE *llocp, void *user_defined, void *yyscanner, const char *msg);

#line 266 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"

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
#define YYFINAL  9
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   267

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  36
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  37
/* YYNRULES -- Number of rules.  */
#define YYNRULES  107
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  266

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   290


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
      35
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    98,    98,   102,   109,   117,   121,   125,   129,   133,
     137,   144,   152,   156,   160,   164,   168,   172,   176,   180,
     184,   188,   192,   196,   200,   204,   208,   213,   218,   222,
     230,   234,   239,   244,   248,   252,   256,   260,   264,   268,
     272,   276,   280,   284,   288,   292,   296,   300,   304,   308,
     313,   318,   323,   328,   333,   338,   343,   348,   353,   358,
     366,   370,   375,   380,   399,   403,   407,   414,   418,   422,
     428,   432,   436,   442,   446,   450,   458,   462,   467,   472,
     476,   480,   484,   488,   492,   496,   500,   504,   508,   512,
     516,   519,   522,   525,   528,   531,   534,   537,   540,   543,
     546,   549,   552,   555,   558,   561,   564,   567
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
  "YYTOKEN_TRANSLATION", "YYTOKEN_WEIGHTS", "YYTOKEN_TRUE",
  "YYTOKEN_FALSE", "YYTOKEN_NULL", "YYTOKEN_LEFTBRACE",
  "YYTOKEN_RIGHTBRACE", "YYTOKEN_COLON", "YYTOKEN_LEFTBRACKET",
  "YYTOKEN_RIGHTBRACKET", "YYTOKEN_COMMA", "YYTOKEN_STRING",
  "YYTOKEN_NUMBER_INT", "YYTOKEN_NUMBER_FLOAT", "PSEUDO_LEX_ERROR",
  "$accept", "gltf_object", "gltf_members", "gltf_member", "asset_object",
  "asset_members", "asset_member", "scenes_array", "scene_objects",
  "scene_object", "scene_properties", "scene_property_nodes",
  "scene_property_nodes_elements", "nodes_array", "node_objects",
  "node_object", "node_properties", "node_property_children",
  "node_property_children_elements", "node_property_matrix",
  "node_property_matrix_element", "node_property_rotation",
  "node_property_rotation_element", "node_property_scale",
  "node_property_scale_element", "node_property_translation",
  "node_property_translation_element", "node_property_weights",
  "node_property_weights_elements", "json_string", "json_value",
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
     285,   286,   287,   288,   289,   290
};
#endif

#define YYPACT_NINF (-188)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -22,    40,    13,    -7,    18,    47,    58,    32,  -188,  -188,
      70,     6,    75,   114,  -188,    40,    86,  -188,  -188,    39,
    -188,    41,  -188,  -188,   105,   130,   146,   148,   149,    46,
    -188,    11,  -188,    23,  -188,   132,  -188,    82,  -188,   123,
     123,   123,   123,    76,  -188,    86,   150,   151,   152,  -188,
      80,  -188,    91,   153,   154,   155,   156,   157,   158,   159,
     160,   161,   162,  -188,    87,  -188,   165,  -188,  -188,  -188,
    -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,
    -188,  -188,  -188,  -188,  -188,    -2,     3,  -188,  -188,  -188,
    -188,  -188,  -188,  -188,    76,   163,   123,  -188,    85,  -188,
     123,   164,   166,   167,   169,   168,   170,   173,   174,   175,
    -188,   147,  -188,  -188,   177,    88,  -188,  -188,  -188,    90,
    -188,  -188,     1,  -188,  -188,   178,   179,   180,  -188,  -188,
      14,  -188,  -188,    89,  -188,  -188,   104,  -188,   106,  -188,
     108,  -188,    73,  -188,   181,   182,   183,   184,   185,   186,
     187,   188,   189,   190,    76,  -188,   143,  -188,    76,  -188,
    -188,   126,    76,   163,   123,  -188,  -188,   140,  -188,  -188,
     191,  -188,  -188,   192,  -188,  -188,   193,  -188,  -188,   194,
    -188,  -188,   142,   123,   195,   166,   196,   169,   197,   170,
     173,   174,   175,  -188,  -188,  -188,  -188,   198,  -188,  -188,
    -188,  -188,   199,    89,   104,   106,   108,  -188,   200,  -188,
    -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,  -188,
    -188,   203,   204,   205,   206,  -188,    89,   104,   106,   108,
     207,   208,   210,   211,    89,   104,  -188,  -188,   212,   214,
      89,  -188,   215,    89,   216,    89,   217,    89,   218,    89,
     219,    89,   220,    89,   221,    89,   222,    89,   223,    89,
     224,    89,   225,    89,   227,  -188
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     4,     1,
       0,     0,     0,     0,     2,     0,     0,     5,     6,     0,
       7,     0,     8,     3,     0,     0,     0,     0,     0,     0,
      11,     0,    18,     0,    20,     0,    34,     0,    36,     0,
       0,     0,     0,     0,     9,     0,     0,     0,     0,    22,
       0,    17,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    38,     0,    33,     0,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    79,    12,    13,
      14,    15,    95,    96,    97,     0,     0,    93,    94,    92,
      16,    90,    91,    10,     0,     0,     0,    21,     0,    19,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      37,     0,    35,    99,     0,     0,   101,   104,   107,     0,
     106,    28,     0,    26,    27,     0,     0,     0,    58,    49,
       0,    50,    51,     0,    52,    53,     0,    54,     0,    55,
       0,    56,     0,    57,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    98,     0,   103,     0,    30,
      32,     0,     0,     0,     0,    60,    62,     0,    65,    64,
       0,    68,    67,     0,    71,    70,     0,    74,    73,     0,
      76,    78,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   102,   100,   105,    29,     0,    25,    23,
      24,    59,     0,     0,     0,     0,     0,    75,     0,    48,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    31,
      61,     0,     0,     0,     0,    77,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    69,    72,     0,     0,
       0,    66,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    63
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -188,  -188,  -188,   230,  -188,  -188,   176,  -188,  -188,   141,
    -188,    31,  -188,  -188,  -188,   201,  -188,    34,  -188,     9,
    -185,    37,  -187,    30,  -186,    36,  -184,    50,  -188,   -39,
     -38,  -188,  -188,   102,  -188,  -188,   101
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     2,     7,     8,    17,    29,    30,    20,    33,    34,
      50,   123,   161,    22,    37,    38,    64,   131,   167,   134,
     170,   137,   173,   139,   176,   141,   179,   143,   182,    89,
     118,    91,   115,   116,    92,   119,   120
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      78,    79,    80,    81,     1,    90,    67,    68,    69,    70,
      71,    72,    73,     9,    74,    75,    76,   222,   221,   223,
      46,    10,   224,    47,    48,   113,    82,    83,    84,    85,
     114,   159,    86,   117,   160,    77,    87,    88,    49,    18,
     231,   230,   232,     3,   165,   233,    11,   166,   239,   238,
       4,     5,     6,    51,    52,   242,   121,   124,   244,    14,
     246,   128,   248,    15,   250,    31,   252,    35,   254,    32,
     256,    36,   258,    44,   260,    12,   262,    45,   264,    67,
      68,    69,    70,    71,    72,    73,    13,    74,    75,    76,
      24,    25,    26,    27,   125,    28,    16,   126,   127,    82,
      83,    84,    85,   180,    19,    86,   181,    97,    77,    87,
      88,    98,    65,    66,   110,   155,   193,    31,   111,   156,
     157,   158,   168,   169,   198,   200,    67,    68,    69,    70,
      71,    72,    73,    39,    74,    75,    76,   171,   172,   174,
     175,   177,   178,    21,   209,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    77,   196,   197,    40,    63,
     144,   145,   146,   147,   148,   149,   150,   151,   152,   153,
     201,   202,   207,   208,    41,   114,    42,    43,    94,    95,
      96,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,    35,   122,    99,   199,   130,   213,   129,   133,   136,
     132,   135,   138,   140,   142,   154,   162,   163,   164,   183,
     184,   185,   186,   187,   188,   189,   190,   191,   192,   211,
     216,    93,   203,   204,   205,   206,   215,   217,   210,   212,
     214,   219,   220,   225,   226,   227,   228,   229,   234,   235,
     236,   237,   218,   240,   241,    23,   243,   245,   247,   249,
     251,   253,   255,   257,   259,   261,   263,   265,   194,   195,
       0,     0,     0,     0,     0,     0,     0,   112
};

static const yytype_int16 yycheck[] =
{
      39,    40,    41,    42,    26,    43,     3,     4,     5,     6,
       7,     8,     9,     0,    11,    12,    13,   204,   203,   205,
       9,    28,   206,    12,    13,    27,    23,    24,    25,    26,
      32,    30,    29,    30,    33,    32,    33,    34,    27,    33,
     227,   226,   228,     3,    30,   229,    28,    33,   235,   234,
      10,    11,    12,    30,    31,   240,    94,    96,   243,    27,
     245,   100,   247,    31,   249,    26,   251,    26,   253,    30,
     255,    30,   257,    27,   259,    28,   261,    31,   263,     3,
       4,     5,     6,     7,     8,     9,    28,    11,    12,    13,
       4,     5,     6,     7,     9,     9,    26,    12,    13,    23,
      24,    25,    26,    30,    29,    29,    33,    27,    32,    33,
      34,    31,    30,    31,    27,    27,   154,    26,    31,    31,
      30,    31,    33,    34,   162,   164,     3,     4,     5,     6,
       7,     8,     9,    28,    11,    12,    13,    33,    34,    33,
      34,    33,    34,    29,   183,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    32,    30,    31,    28,    27,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      30,    31,    30,    31,    28,    32,    28,    28,    28,    28,
      28,    28,    28,    28,    28,    28,    28,    28,    28,    28,
      28,    26,    29,    52,   163,    29,   187,    33,    29,    29,
      33,    33,    29,    29,    29,    28,    28,    28,    28,    28,
      28,    28,    28,    28,    28,    28,    28,    28,    28,   185,
     190,    45,    31,    31,    31,    31,   189,   191,    33,    33,
      33,    33,    33,    33,    31,    31,    31,    31,    31,    31,
      30,    30,   192,    31,    30,    15,    31,    31,    31,    31,
      31,    31,    31,    31,    31,    31,    31,    30,   156,   158,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    66
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    26,    37,     3,    10,    11,    12,    38,    39,     0,
      28,    28,    28,    28,    27,    31,    26,    40,    33,    29,
      43,    29,    49,    39,     4,     5,     6,     7,     9,    41,
      42,    26,    30,    44,    45,    26,    30,    50,    51,    28,
      28,    28,    28,    28,    27,    31,     9,    12,    13,    27,
      46,    30,    31,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    27,    52,    30,    31,     3,     4,     5,
       6,     7,     8,     9,    11,    12,    13,    32,    65,    65,
      65,    65,    23,    24,    25,    26,    29,    33,    34,    65,
      66,    67,    70,    42,    28,    28,    28,    27,    31,    45,
      28,    28,    28,    28,    28,    28,    28,    28,    28,    28,
      27,    31,    51,    27,    32,    68,    69,    30,    66,    71,
      72,    66,    29,    47,    65,     9,    12,    13,    65,    33,
      29,    53,    33,    29,    55,    33,    29,    57,    29,    59,
      29,    61,    29,    63,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    28,    27,    31,    30,    31,    30,
      33,    48,    28,    28,    28,    30,    33,    54,    33,    34,
      56,    33,    34,    58,    33,    34,    60,    33,    34,    62,
      30,    33,    64,    28,    28,    28,    28,    28,    28,    28,
      28,    28,    28,    66,    69,    72,    30,    31,    66,    47,
      65,    30,    31,    31,    31,    31,    31,    30,    31,    65,
      33,    53,    33,    55,    33,    57,    59,    61,    63,    33,
      33,    56,    58,    60,    62,    33,    31,    31,    31,    31,
      56,    58,    60,    62,    31,    31,    30,    30,    56,    58,
      31,    30,    56,    31,    56,    31,    56,    31,    56,    31,
      56,    31,    56,    31,    56,    31,    56,    31,    56,    31,
      56,    31,    56,    31,    56,    30
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    36,    37,    38,    38,    39,    39,    39,    39,    40,
      41,    41,    42,    42,    42,    42,    42,    43,    43,    44,
      44,    45,    45,    46,    46,    46,    46,    46,    46,    47,
      47,    48,    48,    49,    49,    50,    50,    51,    51,    52,
      52,    52,    52,    52,    52,    52,    52,    52,    52,    52,
      52,    52,    52,    52,    52,    52,    52,    52,    52,    53,
      53,    54,    54,    55,    56,    56,    57,    58,    58,    59,
      60,    60,    61,    62,    62,    63,    63,    64,    64,    65,
      65,    65,    65,    65,    65,    65,    65,    65,    65,    65,
      66,    66,    66,    66,    66,    66,    66,    66,    67,    67,
      68,    68,    69,    70,    70,    71,    71,    72
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     3,     3,     1,     3,     3,     3,     3,     3,
       3,     1,     3,     3,     3,     3,     3,     3,     2,     3,
       1,     3,     2,     5,     5,     5,     3,     3,     3,     3,
       2,     3,     1,     3,     2,     3,     1,     3,     2,     5,
       5,     5,     5,     5,     5,     5,     5,     5,     5,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       2,     3,     1,    33,     1,     1,     9,     1,     1,     7,
       1,     1,     7,     1,     1,     3,     2,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     2,
       3,     1,     3,     3,     2,     3,     1,     1
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
#line 98 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                { 
	//$$ = $2;
}
#line 1791 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 3: /* gltf_members: gltf_members YYTOKEN_COMMA gltf_member  */
#line 102 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                     { 
	//bool _res_addmember = JsonParser_Json_Object_AddMember(pUserData, $1, $3._stdstring, $3._jsonvalue); 
	//JsonParser_Std_String_Dispose(pUserData, $3._stdstring);
	//if(!_res_addmember) { yyerror(&yylloc, pUserData, pScanner, "Duplicate object key"); }
	//$$ = $1;
}
#line 1802 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 4: /* gltf_members: gltf_member  */
#line 109 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                          { 
    //void *_jsonobject = JsonParser_Json_Object_Create(pUserData);
	//bool _res_addmember = JsonParser_Json_Object_AddMember(pUserData, _jsonobject, $1._stdstring, $1._jsonvalue);
	//JsonParser_Std_String_Dispose(pUserData, $1._stdstring);
	//if(!_res_addmember) { yyerror(&yylloc, pUserData, pScanner, "Duplicate object key"); }
	//$$ = _jsonobject;
    }
#line 1814 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 5: /* gltf_member: YYTOKEN_ASSET YYTOKEN_COLON asset_object  */
#line 117 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                      {
	//$$ = $3;
	}
#line 1822 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 6: /* gltf_member: YYTOKEN_SCENE YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 121 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                            {
	gltf_yacc_set_default_scene_index_callback((yyvsp[0].m_token_numberint), user_defined);
}
#line 1830 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 7: /* gltf_member: YYTOKEN_SCENES YYTOKEN_COLON scenes_array  */
#line 125 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                       {
	//$$ = $3;
	}
#line 1838 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 8: /* gltf_member: YYTOKEN_NODES YYTOKEN_COLON nodes_array  */
#line 129 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                     {
	//$$ = $3;
	}
#line 1846 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 9: /* asset_object: YYTOKEN_LEFTBRACE asset_members YYTOKEN_RIGHTBRACE  */
#line 133 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                  { 
	//$$ = $2;
	}
#line 1854 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 10: /* asset_members: asset_members YYTOKEN_COMMA asset_member  */
#line 137 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                        { 
	//bool _res_addmember = JsonParser_Json_Object_AddMember(pUserData, $1, $3._stdstring, $3._jsonvalue); 
	//JsonParser_Std_String_Dispose(pUserData, $3._stdstring);
	//if(!_res_addmember) { yyerror(&yylloc, pUserData, pScanner, "Duplicate object key"); }
	//$$ = $1;
    }
#line 1865 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 11: /* asset_members: asset_member  */
#line 144 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                            { 
    //void *_jsonobject = JsonParser_Json_Object_Create(pUserData);
	//bool _res_addmember = JsonParser_Json_Object_AddMember(pUserData, _jsonobject, $1._stdstring, $1._jsonvalue);
	//JsonParser_Std_String_Dispose(pUserData, $1._stdstring);
	//if(!_res_addmember) { yyerror(&yylloc, pUserData, pScanner, "Duplicate object key"); }
	//$$ = _jsonobject;
    }
#line 1877 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 12: /* asset_member: YYTOKEN_COPYRIGHT YYTOKEN_COLON json_string  */
#line 152 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                          { 
	//$$._jsonvalue = $3;
    }
#line 1885 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 13: /* asset_member: YYTOKEN_GENERATOR YYTOKEN_COLON json_string  */
#line 156 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                          { 
	//$$._jsonvalue = $3;
    }
#line 1893 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 14: /* asset_member: YYTOKEN_VERSION YYTOKEN_COLON json_string  */
#line 160 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                        { 
	//$$._jsonvalue = $3;
    }
#line 1901 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 15: /* asset_member: YYTOKEN_MINVERSION YYTOKEN_COLON json_string  */
#line 164 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                           { 
	//$$._jsonvalue = $3;
    }
#line 1909 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 16: /* asset_member: YYTOKEN_EXTRAS YYTOKEN_COLON json_value  */
#line 168 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                      { 
	//$$._jsonvalue = $3;
    }
#line 1917 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 17: /* scenes_array: YYTOKEN_LEFTBRACKET scene_objects YYTOKEN_RIGHTBRACKET  */
#line 172 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                     { 

}
#line 1925 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 18: /* scenes_array: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 176 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                       {

}
#line 1933 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 19: /* scene_objects: scene_objects YYTOKEN_COMMA scene_object  */
#line 180 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                        { 
	assert(-1 == (yyvsp[0].m_scene_index) || gltf_yacc_scene_size_callback(user_defined) == ((yyvsp[0].m_scene_index) + 1));
}
#line 1941 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 20: /* scene_objects: scene_object  */
#line 184 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                            {
	assert(-1 == (yyvsp[0].m_scene_index) || gltf_yacc_scene_size_callback(user_defined) == ((yyvsp[0].m_scene_index) + 1));
}
#line 1949 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 21: /* scene_object: YYTOKEN_LEFTBRACE scene_properties YYTOKEN_RIGHTBRACE  */
#line 188 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                    {
	(yyval.m_scene_index) = (yyvsp[-1].m_scene_index);
}
#line 1957 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 22: /* scene_object: YYTOKEN_LEFTBRACE YYTOKEN_RIGHTBRACE  */
#line 192 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                   { 
	(yyval.m_scene_index) = -1;
}
#line 1965 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 23: /* scene_properties: scene_properties YYTOKEN_COMMA YYTOKEN_NODES YYTOKEN_COLON scene_property_nodes  */
#line 196 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                  {
	gltf_yacc_scene_set_nodes_callback((yyval.m_scene_index), (yyvsp[0].m_int_array).m_data, (yyvsp[0].m_int_array).m_size, user_defined);
}
#line 1973 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 24: /* scene_properties: scene_properties YYTOKEN_COMMA YYTOKEN_NAME YYTOKEN_COLON json_string  */
#line 200 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                        {
	gltf_yacc_scene_set_name_callback((yyval.m_scene_index), (yyvsp[0].m_token_string).m_data, (yyvsp[0].m_token_string).m_size, user_defined);
}
#line 1981 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 25: /* scene_properties: scene_properties YYTOKEN_COMMA YYTOKEN_EXTRAS YYTOKEN_COLON json_value  */
#line 204 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                         { 
	// ignore
}
#line 1989 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 26: /* scene_properties: YYTOKEN_NODES YYTOKEN_COLON scene_property_nodes  */
#line 208 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                   { 
	(yyval.m_scene_index) = gltf_yacc_scene_push_callback(user_defined);
	gltf_yacc_scene_set_nodes_callback((yyval.m_scene_index), (yyvsp[0].m_int_array).m_data, (yyvsp[0].m_int_array).m_size, user_defined);
}
#line 1998 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 27: /* scene_properties: YYTOKEN_NAME YYTOKEN_COLON json_string  */
#line 213 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                         {
	(yyval.m_scene_index) = gltf_yacc_scene_push_callback(user_defined);
	gltf_yacc_scene_set_name_callback((yyval.m_scene_index), (yyvsp[0].m_token_string).m_data, (yyvsp[0].m_token_string).m_size, user_defined);
}
#line 2007 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 28: /* scene_properties: YYTOKEN_EXTRAS YYTOKEN_COLON json_value  */
#line 218 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                          { 
	// ignore
}
#line 2015 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 29: /* scene_property_nodes: YYTOKEN_LEFTBRACKET scene_property_nodes_elements YYTOKEN_RIGHTBRACKET  */
#line 222 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                             {
	for (int i = 0; i < (yyvsp[-1].m_int_array).m_size; ++i)
	{
		(yyval.m_int_array).m_data[i] = (yyvsp[-1].m_int_array).m_data[i];
	}
	(yyval.m_int_array).m_size = (yyvsp[-1].m_int_array).m_size;
}
#line 2027 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 30: /* scene_property_nodes: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 230 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                               { 
    (yyval.m_int_array).m_size = 0;
}
#line 2035 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 31: /* scene_property_nodes_elements: scene_property_nodes_elements YYTOKEN_COMMA YYTOKEN_NUMBER_INT  */
#line 234 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                              {
	(yyval.m_int_array).m_data[(yyval.m_int_array).m_size] = (yyvsp[0].m_token_numberint);
	++(yyval.m_int_array).m_size;
}
#line 2044 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 32: /* scene_property_nodes_elements: YYTOKEN_NUMBER_INT  */
#line 239 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                   {
	(yyval.m_int_array).m_data[0] = (yyvsp[0].m_token_numberint);
	(yyval.m_int_array).m_size = 1;
}
#line 2053 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 33: /* nodes_array: YYTOKEN_LEFTBRACKET node_objects YYTOKEN_RIGHTBRACKET  */
#line 244 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                   {

}
#line 2061 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 34: /* nodes_array: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 248 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                      {

}
#line 2069 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 35: /* node_objects: node_objects YYTOKEN_COMMA node_object  */
#line 252 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                     { 
   	assert(-1 == (yyvsp[0].m_node_index) || gltf_yacc_node_size_callback(user_defined) == ((yyvsp[0].m_node_index) + 1));
}
#line 2077 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 36: /* node_objects: node_object  */
#line 256 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                          { 
	assert(-1 == (yyvsp[0].m_node_index) || gltf_yacc_node_size_callback(user_defined) == ((yyvsp[0].m_node_index) + 1));
}
#line 2085 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 37: /* node_object: YYTOKEN_LEFTBRACE node_properties YYTOKEN_RIGHTBRACE  */
#line 260 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                  {
	(yyval.m_node_index) = (yyvsp[-1].m_node_index);
}
#line 2093 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 38: /* node_object: YYTOKEN_LEFTBRACE YYTOKEN_RIGHTBRACE  */
#line 264 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                  { 
	(yyval.m_node_index) = -1;
}
#line 2101 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 39: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_CAMERA YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 268 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                               {
	gltf_yacc_node_set_camera_callback((yyval.m_node_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2109 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 40: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_CHILDREN YYTOKEN_COLON node_property_children  */
#line 272 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                     {
	gltf_yacc_node_set_children_callback((yyval.m_node_index), (yyvsp[0].m_int_array).m_data, (yyvsp[0].m_int_array).m_size, user_defined);
}
#line 2117 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 41: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_SKIN YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 276 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                             {
	gltf_yacc_node_set_skin_callback((yyval.m_node_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2125 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 42: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_MATRIX YYTOKEN_COLON node_property_matrix  */
#line 280 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                 {
	gltf_yacc_node_set_matrix_callback((yyval.m_node_index), (yyvsp[0].m_mat4x4), user_defined);
}
#line 2133 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 43: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_MESH YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 284 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                             {
	gltf_yacc_node_set_mesh_callback((yyval.m_node_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2141 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 44: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_ROTATION YYTOKEN_COLON node_property_rotation  */
#line 288 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                     {
	gltf_yacc_node_set_rotation_callback((yyval.m_node_index), (yyvsp[0].m_vec4), user_defined);
}
#line 2149 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 45: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_SCALE YYTOKEN_COLON node_property_scale  */
#line 292 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                               {
	gltf_yacc_node_set_scale_callback((yyval.m_node_index), (yyvsp[0].m_vec3), user_defined);
}
#line 2157 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 46: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_TRANSLATION YYTOKEN_COLON node_property_translation  */
#line 296 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                           {
	gltf_yacc_node_set_translation_callback((yyval.m_node_index), (yyvsp[0].m_vec3), user_defined);
}
#line 2165 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 47: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_WEIGHTS YYTOKEN_COLON node_property_weights  */
#line 300 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                   {
	gltf_yacc_node_set_weights_callback((yyval.m_node_index), (yyvsp[0].m_float_array).m_data, (yyvsp[0].m_float_array).m_size, user_defined);
}
#line 2173 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 48: /* node_properties: node_properties YYTOKEN_COMMA YYTOKEN_NAME YYTOKEN_COLON json_string  */
#line 304 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                      {
	gltf_yacc_node_set_name_callback((yyval.m_node_index), (yyvsp[0].m_token_string).m_data, (yyvsp[0].m_token_string).m_size, user_defined);
}
#line 2181 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 49: /* node_properties: YYTOKEN_CAMERA YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 308 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                 {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_camera_callback((yyval.m_node_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2190 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 50: /* node_properties: YYTOKEN_CHILDREN YYTOKEN_COLON node_property_children  */
#line 313 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                       {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_children_callback((yyval.m_node_index), (yyvsp[0].m_int_array).m_data, (yyvsp[0].m_int_array).m_size, user_defined);
}
#line 2199 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 51: /* node_properties: YYTOKEN_SKIN YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 318 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                               {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_skin_callback((yyval.m_node_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2208 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 52: /* node_properties: YYTOKEN_MATRIX YYTOKEN_COLON node_property_matrix  */
#line 323 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                   {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_matrix_callback((yyval.m_node_index), (yyvsp[0].m_mat4x4), user_defined);
}
#line 2217 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 53: /* node_properties: YYTOKEN_MESH YYTOKEN_COLON YYTOKEN_NUMBER_INT  */
#line 328 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                               {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_mesh_callback((yyval.m_node_index), (yyvsp[0].m_token_numberint), user_defined);
}
#line 2226 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 54: /* node_properties: YYTOKEN_ROTATION YYTOKEN_COLON node_property_rotation  */
#line 333 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                       {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_rotation_callback((yyval.m_node_index), (yyvsp[0].m_vec4), user_defined);
}
#line 2235 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 55: /* node_properties: YYTOKEN_SCALE YYTOKEN_COLON node_property_scale  */
#line 338 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                 {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_scale_callback((yyval.m_node_index), (yyvsp[0].m_vec3), user_defined);
}
#line 2244 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 56: /* node_properties: YYTOKEN_TRANSLATION YYTOKEN_COLON node_property_translation  */
#line 343 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                             {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_translation_callback((yyval.m_node_index), (yyvsp[0].m_vec3), user_defined);
}
#line 2253 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 57: /* node_properties: YYTOKEN_WEIGHTS YYTOKEN_COLON node_property_weights  */
#line 348 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                     {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_weights_callback((yyval.m_node_index), (yyvsp[0].m_float_array).m_data, (yyvsp[0].m_float_array).m_size, user_defined);
}
#line 2262 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 58: /* node_properties: YYTOKEN_NAME YYTOKEN_COLON json_string  */
#line 353 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                        {
	(yyval.m_node_index) = gltf_yacc_node_push_callback(user_defined);
	gltf_yacc_node_set_name_callback((yyval.m_node_index), (yyvsp[0].m_token_string).m_data, (yyvsp[0].m_token_string).m_size, user_defined);
}
#line 2271 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 59: /* node_property_children: YYTOKEN_LEFTBRACKET node_property_children_elements YYTOKEN_RIGHTBRACKET  */
#line 358 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                 {
	for (int i = 0; i < (yyvsp[-1].m_int_array).m_size; ++i)
	{
		(yyval.m_int_array).m_data[i] = (yyvsp[-1].m_int_array).m_data[i];
	}
	(yyval.m_int_array).m_size = (yyvsp[-1].m_int_array).m_size;
}
#line 2283 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 60: /* node_property_children: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 366 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                 { 
    (yyval.m_int_array).m_size = 0;
}
#line 2291 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 61: /* node_property_children_elements: node_property_children_elements YYTOKEN_COMMA YYTOKEN_NUMBER_INT  */
#line 370 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                  {
	(yyval.m_int_array).m_data[(yyval.m_int_array).m_size] = (yyvsp[0].m_token_numberint);
	++(yyval.m_int_array).m_size;
}
#line 2300 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 62: /* node_property_children_elements: YYTOKEN_NUMBER_INT  */
#line 375 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                     {
	(yyval.m_int_array).m_data[0] = (yyvsp[0].m_token_numberint);
	(yyval.m_int_array).m_size = 1;
}
#line 2309 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 63: /* node_property_matrix: YYTOKEN_LEFTBRACKET node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_COMMA node_property_matrix_element YYTOKEN_RIGHTBRACKET  */
#line 380 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
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
#line 2332 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 64: /* node_property_matrix_element: YYTOKEN_NUMBER_FLOAT  */
#line 399 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                   {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberfloat);
}
#line 2340 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 65: /* node_property_matrix_element: YYTOKEN_NUMBER_INT  */
#line 403 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                 {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberint);
}
#line 2348 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 66: /* node_property_rotation: YYTOKEN_LEFTBRACKET node_property_rotation_element YYTOKEN_COMMA node_property_rotation_element YYTOKEN_COMMA node_property_rotation_element YYTOKEN_COMMA node_property_rotation_element YYTOKEN_RIGHTBRACKET  */
#line 407 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                                                                                                                                                       {
	(yyval.m_vec4)[0] = (yyvsp[-7].m_token_numberfloat);
	(yyval.m_vec4)[1] = (yyvsp[-5].m_token_numberfloat);
	(yyval.m_vec4)[2] = (yyvsp[-3].m_token_numberfloat);
	(yyval.m_vec4)[3] = (yyvsp[-1].m_token_numberfloat);
}
#line 2359 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 67: /* node_property_rotation_element: YYTOKEN_NUMBER_FLOAT  */
#line 414 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                     {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberfloat);
}
#line 2367 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 68: /* node_property_rotation_element: YYTOKEN_NUMBER_INT  */
#line 418 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                   {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberint);
}
#line 2375 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 69: /* node_property_scale: YYTOKEN_LEFTBRACKET node_property_scale_element YYTOKEN_COMMA node_property_scale_element YYTOKEN_COMMA node_property_scale_element YYTOKEN_RIGHTBRACKET  */
#line 422 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                                                                                              {
	(yyval.m_vec3)[0] = (yyvsp[-5].m_token_numberfloat);
	(yyval.m_vec3)[1] = (yyvsp[-3].m_token_numberfloat);
	(yyval.m_vec3)[2] = (yyvsp[-1].m_token_numberfloat);
}
#line 2385 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 70: /* node_property_scale_element: YYTOKEN_NUMBER_FLOAT  */
#line 428 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                  {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberfloat);
}
#line 2393 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 71: /* node_property_scale_element: YYTOKEN_NUMBER_INT  */
#line 432 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberint);
}
#line 2401 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 72: /* node_property_translation: YYTOKEN_LEFTBRACKET node_property_translation_element YYTOKEN_COMMA node_property_translation_element YYTOKEN_COMMA node_property_translation_element YYTOKEN_RIGHTBRACKET  */
#line 436 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                                                                                                                      {
	(yyval.m_vec3)[0] = (yyvsp[-5].m_token_numberfloat);
	(yyval.m_vec3)[1] = (yyvsp[-3].m_token_numberfloat);
	(yyval.m_vec3)[2] = (yyvsp[-1].m_token_numberfloat);
}
#line 2411 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 73: /* node_property_translation_element: YYTOKEN_NUMBER_FLOAT  */
#line 442 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                        {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberfloat);
}
#line 2419 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 74: /* node_property_translation_element: YYTOKEN_NUMBER_INT  */
#line 446 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                      {
	(yyval.m_token_numberfloat) = (yyvsp[0].m_token_numberint);
}
#line 2427 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 75: /* node_property_weights: YYTOKEN_LEFTBRACKET node_property_weights_elements YYTOKEN_RIGHTBRACKET  */
#line 450 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                               {
	for (int i = 0; i < (yyvsp[-1].m_float_array).m_size; ++i)
	{
		(yyval.m_float_array).m_data[i] = (yyvsp[-1].m_float_array).m_data[i];
	}
	(yyval.m_float_array).m_size = (yyvsp[-1].m_float_array).m_size;
}
#line 2439 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 76: /* node_property_weights: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 458 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                { 
    (yyval.m_float_array).m_size = 0;
}
#line 2447 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 77: /* node_property_weights_elements: node_property_weights_elements YYTOKEN_COMMA YYTOKEN_NUMBER_INT  */
#line 462 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                                                {
	(yyval.m_float_array).m_data[(yyval.m_float_array).m_size] = (yyvsp[0].m_token_numberint);
	++(yyval.m_float_array).m_size;
}
#line 2456 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 78: /* node_property_weights_elements: YYTOKEN_NUMBER_INT  */
#line 467 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                    {
	(yyval.m_float_array).m_data[0] = (yyvsp[0].m_token_numberint);
	(yyval.m_float_array).m_size = 1;
}
#line 2465 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 79: /* json_string: YYTOKEN_STRING  */
#line 472 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                            {

}
#line 2473 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 80: /* json_string: YYTOKEN_ASSET  */
#line 476 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                           {

}
#line 2481 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 81: /* json_string: YYTOKEN_COPYRIGHT  */
#line 480 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                               {

}
#line 2489 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 82: /* json_string: YYTOKEN_GENERATOR  */
#line 484 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                               {

}
#line 2497 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 83: /* json_string: YYTOKEN_VERSION  */
#line 488 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                             {

}
#line 2505 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 84: /* json_string: YYTOKEN_MINVERSION  */
#line 492 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                {

}
#line 2513 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 85: /* json_string: YYTOKEN_EXTENSIONS  */
#line 496 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                {

}
#line 2521 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 86: /* json_string: YYTOKEN_EXTRAS  */
#line 500 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                            {

}
#line 2529 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 87: /* json_string: YYTOKEN_SCENES  */
#line 504 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                            {

}
#line 2537 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 88: /* json_string: YYTOKEN_NODES  */
#line 508 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                           {

}
#line 2545 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 89: /* json_string: YYTOKEN_NAME  */
#line 512 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                          {

}
#line 2553 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 90: /* json_value: json_object  */
#line 516 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                        { 
}
#line 2560 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 91: /* json_value: json_array  */
#line 519 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                       { 
}
#line 2567 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 92: /* json_value: json_string  */
#line 522 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                        { 
}
#line 2574 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 93: /* json_value: YYTOKEN_NUMBER_INT  */
#line 525 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                               { 
}
#line 2581 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 94: /* json_value: YYTOKEN_NUMBER_FLOAT  */
#line 528 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                 { 
}
#line 2588 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 95: /* json_value: YYTOKEN_TRUE  */
#line 531 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                         { 
}
#line 2595 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 96: /* json_value: YYTOKEN_FALSE  */
#line 534 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                          { 
}
#line 2602 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 97: /* json_value: YYTOKEN_NULL  */
#line 537 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                         { 
}
#line 2609 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 98: /* json_object: YYTOKEN_LEFTBRACE json_members YYTOKEN_RIGHTBRACE  */
#line 540 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                               { 
}
#line 2616 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 99: /* json_object: YYTOKEN_LEFTBRACE YYTOKEN_RIGHTBRACE  */
#line 543 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                  { 
}
#line 2623 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 100: /* json_members: json_members YYTOKEN_COMMA json_member  */
#line 546 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                     { 
}
#line 2630 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 101: /* json_members: json_member  */
#line 549 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                          { 
}
#line 2637 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 102: /* json_member: YYTOKEN_STRING YYTOKEN_COLON json_value  */
#line 552 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                     { 
}
#line 2644 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 103: /* json_array: YYTOKEN_LEFTBRACKET json_elements YYTOKEN_RIGHTBRACKET  */
#line 555 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                                   { 
}
#line 2651 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 104: /* json_array: YYTOKEN_LEFTBRACKET YYTOKEN_RIGHTBRACKET  */
#line 558 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                     { 
}
#line 2658 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 105: /* json_elements: json_elements YYTOKEN_COMMA json_element  */
#line 561 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                                                        { 
}
#line 2665 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 106: /* json_elements: json_element  */
#line 564 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                            { 
}
#line 2672 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;

  case 107: /* json_element: json_value  */
#line 567 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"
                         {
	
}
#line 2680 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"
    break;


#line 2684 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.inl"

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

#line 571 "/home/HanetakaYuminaga/Documents/PatriotEngine/bldsys/posix_linux_x11/../../src/pt_gfx_mesh_base_gltf_yacc.y"

