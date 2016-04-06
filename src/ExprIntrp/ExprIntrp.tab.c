/* A Bison parser, made by GNU Bison 3.7.1.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
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

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.7.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         ExprIntrpparse
#define yylex           ExprIntrplex
#define yyerror         ExprIntrperror
#define yydebug         ExprIntrpdebug
#define yynerrs         ExprIntrpnerrs
#define yylval          ExprIntrplval
#define yychar          ExprIntrpchar

/* First part of user prologue.  */

#include <ExprIntrp_yaccintrf.hxx>

extern void ExprIntrp_EndOfFuncDef();
extern void ExprIntrp_EndOfRelation();
extern void ExprIntrp_AssignVariable();
extern void ExprIntrp_EndOfAssign();
extern void ExprIntrp_Deassign();
extern void ExprIntrp_SumOperator();
extern void ExprIntrp_MinusOperator();
extern void ExprIntrp_ProductOperator();
extern void ExprIntrp_DivideOperator();
extern void ExprIntrp_ExpOperator();
extern void ExprIntrp_UnaryMinusOperator();
extern void ExprIntrp_UnaryPlusOperator();
extern void ExprIntrp_VariableIdentifier();
extern void ExprIntrp_NumValue();
extern void ExprIntrp_EndFunction();
extern void ExprIntrp_EndDerFunction();
extern void ExprIntrp_EndDifferential();
extern void ExprIntrp_EndDiffFunction();
extern void ExprIntrp_EndFuncArg();
extern void ExprIntrp_NextFuncArg();
extern void ExprIntrp_StartFunction();
extern void ExprIntrp_DefineFunction();
extern void ExprIntrp_StartDerivate();
extern void ExprIntrp_EndDerivate();
extern void ExprIntrp_DiffVar();
extern void ExprIntrp_DiffDegree();
extern void ExprIntrp_VerDiffDegree();
extern void ExprIntrp_DiffDegreeVar();
extern void ExprIntrp_StartDifferential();
extern void ExprIntrp_StartFunction();
extern void ExprIntrp_EndFuncArg();
extern void ExprIntrp_NextFuncArg();
extern void ExprIntrp_VariableIdentifier();
extern void ExprIntrp_Derivation();
extern void ExprIntrp_EndDerivation();
extern void ExprIntrp_DerivationValue();
extern void ExprIntrp_ConstantIdentifier();
extern void ExprIntrp_ConstantDefinition();
extern void ExprIntrp_VariableIdentifier();
extern void ExprIntrp_NumValue();
extern void ExprIntrp_Sumator();
extern void ExprIntrp_VariableIdentifier();
extern void ExprIntrp_Productor();
extern void ExprIntrp_EndOfEqual();

// disable MSVC warnings in bison code
#ifdef _MSC_VER
#pragma warning(disable:4131 4244 4127 4702)
#endif




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

#include "ExprIntrp.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_SUMOP = 3,                      /* SUMOP  */
  YYSYMBOL_MINUSOP = 4,                    /* MINUSOP  */
  YYSYMBOL_DIVIDEOP = 5,                   /* DIVIDEOP  */
  YYSYMBOL_EXPOP = 6,                      /* EXPOP  */
  YYSYMBOL_MULTOP = 7,                     /* MULTOP  */
  YYSYMBOL_PARENTHESIS = 8,                /* PARENTHESIS  */
  YYSYMBOL_BRACKET = 9,                    /* BRACKET  */
  YYSYMBOL_ENDPARENTHESIS = 10,            /* ENDPARENTHESIS  */
  YYSYMBOL_ENDBRACKET = 11,                /* ENDBRACKET  */
  YYSYMBOL_VALUE = 12,                     /* VALUE  */
  YYSYMBOL_IDENTIFIER = 13,                /* IDENTIFIER  */
  YYSYMBOL_COMMA = 14,                     /* COMMA  */
  YYSYMBOL_DIFFERENTIAL = 15,              /* DIFFERENTIAL  */
  YYSYMBOL_DERIVATE = 16,                  /* DERIVATE  */
  YYSYMBOL_DERIVKEY = 17,                  /* DERIVKEY  */
  YYSYMBOL_ASSIGNOP = 18,                  /* ASSIGNOP  */
  YYSYMBOL_DEASSIGNKEY = 19,               /* DEASSIGNKEY  */
  YYSYMBOL_EQUALOP = 20,                   /* EQUALOP  */
  YYSYMBOL_RELSEPARATOR = 21,              /* RELSEPARATOR  */
  YYSYMBOL_CONSTKEY = 22,                  /* CONSTKEY  */
  YYSYMBOL_SUMKEY = 23,                    /* SUMKEY  */
  YYSYMBOL_PRODKEY = 24,                   /* PRODKEY  */
  YYSYMBOL_25_n_ = 25,                     /* '\n'  */
  YYSYMBOL_YYACCEPT = 26,                  /* $accept  */
  YYSYMBOL_exprentry = 27,                 /* exprentry  */
  YYSYMBOL_Assignment = 28,                /* Assignment  */
  YYSYMBOL_29_1 = 29,                      /* $@1  */
  YYSYMBOL_Deassignment = 30,              /* Deassignment  */
  YYSYMBOL_31_2 = 31,                      /* $@2  */
  YYSYMBOL_GenExpr = 32,                   /* GenExpr  */
  YYSYMBOL_SingleExpr = 33,                /* SingleExpr  */
  YYSYMBOL_Single = 34,                    /* Single  */
  YYSYMBOL_Function = 35,                  /* Function  */
  YYSYMBOL_36_3 = 36,                      /* $@3  */
  YYSYMBOL_ListGenExpr = 37,               /* ListGenExpr  */
  YYSYMBOL_38_4 = 38,                      /* $@4  */
  YYSYMBOL_funcident = 39,                 /* funcident  */
  YYSYMBOL_FunctionDefinition = 40,        /* FunctionDefinition  */
  YYSYMBOL_41_5 = 41,                      /* $@5  */
  YYSYMBOL_DerFunctionId = 42,             /* DerFunctionId  */
  YYSYMBOL_43_6 = 43,                      /* $@6  */
  YYSYMBOL_DiffFuncId = 44,                /* DiffFuncId  */
  YYSYMBOL_45_7 = 45,                      /* $@7  */
  YYSYMBOL_46_8 = 46,                      /* $@8  */
  YYSYMBOL_DiffId = 47,                    /* DiffId  */
  YYSYMBOL_FunctionDef = 48,               /* FunctionDef  */
  YYSYMBOL_49_9 = 49,                      /* $@9  */
  YYSYMBOL_ListArg = 50,                   /* ListArg  */
  YYSYMBOL_51_10 = 51,                     /* $@10  */
  YYSYMBOL_unarg = 52,                     /* unarg  */
  YYSYMBOL_Derivation = 53,                /* Derivation  */
  YYSYMBOL_54_11 = 54,                     /* $@11  */
  YYSYMBOL_55_12 = 55,                     /* $@12  */
  YYSYMBOL_56_13 = 56,                     /* $@13  */
  YYSYMBOL_ConstantDefinition = 57,        /* ConstantDefinition  */
  YYSYMBOL_58_14 = 58,                     /* $@14  */
  YYSYMBOL_59_15 = 59,                     /* $@15  */
  YYSYMBOL_Sumator = 60,                   /* Sumator  */
  YYSYMBOL_61_16 = 61,                     /* $@16  */
  YYSYMBOL_62_17 = 62,                     /* $@17  */
  YYSYMBOL_Productor = 63,                 /* Productor  */
  YYSYMBOL_64_18 = 64,                     /* $@18  */
  YYSYMBOL_65_19 = 65,                     /* $@19  */
  YYSYMBOL_RelationList = 66,              /* RelationList  */
  YYSYMBOL_SingleRelation = 67             /* SingleRelation  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




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
typedef yytype_uint8 yy_state_t;

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
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
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

#if !defined yyoverflow

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
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

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
#define YYFINAL  48
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   189

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  26
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  42
/* YYNRULES -- Number of rules.  */
#define YYNRULES  70
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  149

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   279


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
      25,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    80,    80,    81,    82,    83,    84,    87,    87,    90,
      90,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   109,   110,   114,   115,   118,
     119,   120,   120,   123,   124,   124,   127,   130,   130,   133,
     133,   136,   137,   137,   137,   140,   141,   144,   144,   147,
     148,   148,   151,   154,   154,   155,   155,   155,   158,   158,
     158,   161,   161,   161,   164,   164,   164,   167,   168,   169,
     172
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "SUMOP", "MINUSOP",
  "DIVIDEOP", "EXPOP", "MULTOP", "PARENTHESIS", "BRACKET",
  "ENDPARENTHESIS", "ENDBRACKET", "VALUE", "IDENTIFIER", "COMMA",
  "DIFFERENTIAL", "DERIVATE", "DERIVKEY", "ASSIGNOP", "DEASSIGNKEY",
  "EQUALOP", "RELSEPARATOR", "CONSTKEY", "SUMKEY", "PRODKEY", "'\\n'",
  "$accept", "exprentry", "Assignment", "$@1", "Deassignment", "$@2",
  "GenExpr", "SingleExpr", "Single", "Function", "$@3", "ListGenExpr",
  "$@4", "funcident", "FunctionDefinition", "$@5", "DerFunctionId", "$@6",
  "DiffFuncId", "$@7", "$@8", "DiffId", "FunctionDef", "$@9", "ListArg",
  "$@10", "unarg", "Derivation", "$@11", "$@12", "$@13",
  "ConstantDefinition", "$@14", "$@15", "Sumator", "$@16", "$@17",
  "Productor", "$@18", "$@19", "RelationList", "SingleRelation", YY_NULLPTR
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
     275,   276,   277,   278,   279,    10
};
#endif

#define YYPACT_NINF (-51)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-56)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
       4,    62,    62,    62,    62,   -51,     2,    87,    39,    58,
      59,    60,    63,     9,   -51,   -51,    35,   -51,   -51,   -51,
       7,   -51,    37,   -51,   -51,   -51,   -51,   -51,   -51,   -51,
      36,     6,    24,    24,   161,   152,    55,    64,    67,   -51,
     -51,   -51,    73,    62,    68,    69,    62,    62,   -51,    62,
      62,    62,    62,    62,    62,    62,    62,    93,    79,    62,
      62,   -51,   -51,    62,   -51,   103,    43,    99,    30,   -51,
     -51,    84,    89,    24,    24,   111,   -51,   111,   169,   101,
     108,   109,    62,    62,    35,   -51,   -51,   169,   -51,   115,
     116,   124,   120,   121,   137,   117,   122,   138,   -51,   -51,
     -51,   142,   169,   -51,   -51,   145,   -51,   136,   -51,   141,
     -51,   -51,    62,   -51,   103,   149,   151,   155,   -51,   156,
     163,   -51,   -51,   -51,   -51,   166,   168,    62,    62,   167,
     -51,   -51,   106,   118,   -51,   170,    62,    62,   -51,   135,
     140,   171,   172,   -51,   -51,   174,   175,   -51,   -51
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     0,    28,    27,     0,     0,     0,
       0,     0,     0,     0,     3,     4,     2,    20,    25,    26,
       0,     5,     0,    31,    37,    21,    22,    23,    24,     6,
      67,    27,    19,    18,     0,     0,     0,     0,     0,    42,
      45,    46,     0,     0,     0,     0,     0,     0,     1,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    16,    17,     0,    40,     0,     0,     0,     0,     9,
      58,     0,     0,    11,    12,    14,    15,    13,    70,    33,
       0,     0,     0,     0,     0,    68,    69,     8,    52,     0,
      49,     0,     0,     0,     0,     0,     0,     0,    34,    29,
      30,     0,    38,    48,    50,     0,    41,    53,    10,     0,
      61,    64,     0,    32,     0,     0,     0,     0,    59,     0,
       0,    35,    51,    43,    54,     0,     0,     0,     0,     0,
      56,    60,     0,     0,    44,     0,     0,     0,    57,     0,
       0,     0,     0,    62,    65,     0,     0,    63,    66
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -51,   -51,   -51,   -51,   -51,   -51,     0,   -51,   -51,   -51,
     -51,   -50,   -51,   -51,   -51,   -51,   -51,   -51,    -2,   -51,
     -51,   123,   -51,   -51,    74,   -51,   -51,   -51,   -51,   -51,
     -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,
     -35,   -51
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    13,    14,    36,    15,    94,    79,    17,    18,    19,
      57,    80,   112,    20,    21,    58,    22,    37,    23,    66,
     129,    42,    24,    38,    89,   114,    90,    25,   116,   117,
     135,    26,    95,   126,    27,   119,   145,    28,   120,   146,
      29,    30
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      16,    32,    33,    34,    35,    41,    81,     1,     2,    48,
     -36,   -47,     3,     4,   -36,    55,     5,     6,   -39,     7,
      -7,     8,   -39,     9,    85,    86,    10,    11,    12,    51,
      52,    53,   101,    49,    50,    51,    52,    53,    49,    50,
      51,    52,    53,    68,    93,    56,    71,    72,    43,    73,
      74,    75,    76,    77,    78,    54,    40,    59,     7,    84,
      84,    60,   121,    87,    41,     1,     2,    44,    45,    46,
       3,     4,    47,    63,     5,    31,    65,     7,    67,     8,
      64,    69,    70,   102,    10,    11,    12,    49,    50,    51,
      52,    53,    49,    50,    51,    52,    53,    83,    96,    39,
      40,    82,     7,    97,    49,    50,    51,    52,    53,    49,
      50,    51,    52,    53,    92,    98,    88,    52,    99,   100,
     136,    49,    50,    51,    52,    53,   103,   132,   133,   105,
     104,   109,   137,   106,   107,   110,   139,   140,    49,    50,
      51,    52,    53,    49,    50,    51,    52,    53,   108,   141,
     -55,   111,   113,   118,   142,    49,    50,    51,    52,    53,
     115,   123,   124,    62,    49,    50,    51,    52,    53,   125,
     127,    61,    49,    50,    51,    52,    53,   128,   130,   131,
     134,   138,     0,   143,   144,   147,   148,     0,   122,    91
};

static const yytype_int16 yycheck[] =
{
       0,     1,     2,     3,     4,     7,    56,     3,     4,     0,
       8,     9,     8,     9,     8,     8,    12,    13,    16,    15,
      18,    17,    16,    19,    59,    60,    22,    23,    24,     5,
       6,     7,    82,     3,     4,     5,     6,     7,     3,     4,
       5,     6,     7,    43,    14,     8,    46,    47,     9,    49,
      50,    51,    52,    53,    54,    20,    13,    21,    15,    59,
      60,    25,   112,    63,    66,     3,     4,     9,     9,     9,
       8,     9,     9,    18,    12,    13,     9,    15,     5,    17,
      16,    13,    13,    83,    22,    23,    24,     3,     4,     5,
       6,     7,     3,     4,     5,     6,     7,    18,    14,    12,
      13,     8,    15,    14,     3,     4,     5,     6,     7,     3,
       4,     5,     6,     7,    15,    14,    13,     6,    10,    10,
      14,     3,     4,     5,     6,     7,    11,   127,   128,     5,
      14,    14,    14,    13,    13,    13,   136,   137,     3,     4,
       5,     6,     7,     3,     4,     5,     6,     7,    11,    14,
      14,    13,    10,    12,    14,     3,     4,     5,     6,     7,
      15,    12,    11,    11,     3,     4,     5,     6,     7,    14,
      14,    10,     3,     4,     5,     6,     7,    14,    12,    11,
      13,    11,    -1,    12,    12,    11,    11,    -1,   114,    66
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     4,     8,     9,    12,    13,    15,    17,    19,
      22,    23,    24,    27,    28,    30,    32,    33,    34,    35,
      39,    40,    42,    44,    48,    53,    57,    60,    63,    66,
      67,    13,    32,    32,    32,    32,    29,    43,    49,    12,
      13,    44,    47,     9,     9,     9,     9,     9,     0,     3,
       4,     5,     6,     7,    20,     8,     8,    36,    41,    21,
      25,    10,    11,    18,    16,     9,    45,     5,    32,    13,
      13,    32,    32,    32,    32,    32,    32,    32,    32,    32,
      37,    37,     8,    18,    32,    66,    66,    32,    13,    50,
      52,    47,    15,    14,    31,    58,    14,    14,    14,    10,
      10,    37,    32,    11,    14,     5,    13,    13,    11,    14,
      13,    13,    38,    10,    51,    15,    54,    55,    12,    61,
      64,    37,    50,    12,    11,    14,    59,    14,    14,    46,
      12,    11,    32,    32,    13,    56,    14,    14,    11,    32,
      32,    14,    14,    12,    12,    62,    65,    11,    11
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    26,    27,    27,    27,    27,    27,    29,    28,    31,
      30,    32,    32,    32,    32,    32,    32,    32,    32,    32,
      32,    32,    32,    32,    32,    33,    33,    34,    34,    35,
      35,    36,    35,    37,    38,    37,    39,    41,    40,    43,
      42,    44,    45,    46,    44,    47,    47,    49,    48,    50,
      51,    50,    52,    54,    53,    55,    56,    53,    58,    59,
      57,    61,    62,    60,    64,    65,    63,    66,    66,    66,
      67
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     0,     4,     0,
       5,     3,     3,     3,     3,     3,     3,     3,     2,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     4,
       4,     0,     5,     1,     0,     4,     1,     0,     4,     0,
       3,     5,     0,     0,     9,     1,     1,     0,     5,     1,
       0,     4,     1,     0,     7,     0,     0,    10,     0,     0,
       8,     0,     0,    14,     0,     0,    14,     1,     3,     3,
       3
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
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


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

/* This macro is provided for backward compatibility. */
# ifndef YY_LOCATION_PRINT
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yykind < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yykind], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
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
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
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






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
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

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */
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

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
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
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

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
      yychar = yylex ();
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


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 5: /* exprentry: FunctionDefinition  */
                                         {ExprIntrp_EndOfFuncDef();}
    break;

  case 6: /* exprentry: RelationList  */
                                  {ExprIntrp_EndOfRelation();}
    break;

  case 7: /* $@1: %empty  */
                                {ExprIntrp_AssignVariable();}
    break;

  case 8: /* Assignment: IDENTIFIER $@1 ASSIGNOP GenExpr  */
                                                                               {ExprIntrp_EndOfAssign();}
    break;

  case 9: /* $@2: %empty  */
                                                    {ExprIntrp_Deassign();}
    break;

  case 11: /* GenExpr: GenExpr SUMOP GenExpr  */
                                            {ExprIntrp_SumOperator();}
    break;

  case 12: /* GenExpr: GenExpr MINUSOP GenExpr  */
                                             {ExprIntrp_MinusOperator();}
    break;

  case 13: /* GenExpr: GenExpr MULTOP GenExpr  */
                                            {ExprIntrp_ProductOperator();}
    break;

  case 14: /* GenExpr: GenExpr DIVIDEOP GenExpr  */
                                              {ExprIntrp_DivideOperator();}
    break;

  case 15: /* GenExpr: GenExpr EXPOP GenExpr  */
                                           {ExprIntrp_ExpOperator();}
    break;

  case 18: /* GenExpr: MINUSOP GenExpr  */
                                     {ExprIntrp_UnaryMinusOperator();}
    break;

  case 19: /* GenExpr: SUMOP GenExpr  */
                                   {ExprIntrp_UnaryPlusOperator();}
    break;

  case 27: /* Single: IDENTIFIER  */
                                 {ExprIntrp_VariableIdentifier();}
    break;

  case 28: /* Single: VALUE  */
                            {ExprIntrp_NumValue();}
    break;

  case 29: /* Function: funcident PARENTHESIS ListGenExpr ENDPARENTHESIS  */
                                                                      {ExprIntrp_EndFunction();}
    break;

  case 30: /* Function: DerFunctionId PARENTHESIS ListGenExpr ENDPARENTHESIS  */
                                                                          {ExprIntrp_EndDerFunction();}
    break;

  case 31: /* $@3: %empty  */
                                {ExprIntrp_EndDifferential();}
    break;

  case 32: /* Function: DiffFuncId $@3 PARENTHESIS ListGenExpr ENDPARENTHESIS  */
                                                                                                      {ExprIntrp_EndDiffFunction();}
    break;

  case 33: /* ListGenExpr: GenExpr  */
                             {ExprIntrp_EndFuncArg();}
    break;

  case 34: /* $@4: %empty  */
                                   {ExprIntrp_NextFuncArg();}
    break;

  case 36: /* funcident: IDENTIFIER  */
                                 {ExprIntrp_StartFunction();}
    break;

  case 37: /* $@5: %empty  */
                                 {ExprIntrp_DefineFunction();}
    break;

  case 39: /* $@6: %empty  */
                                {ExprIntrp_StartDerivate();}
    break;

  case 40: /* DerFunctionId: IDENTIFIER $@6 DERIVATE  */
                                                                      {ExprIntrp_EndDerivate();}
    break;

  case 41: /* DiffFuncId: DIFFERENTIAL DiffId DIVIDEOP DIFFERENTIAL IDENTIFIER  */
                                                                          {ExprIntrp_DiffVar();}
    break;

  case 42: /* $@7: %empty  */
                                        {ExprIntrp_DiffDegree();}
    break;

  case 43: /* $@8: %empty  */
                                                                                                     {ExprIntrp_VerDiffDegree();}
    break;

  case 44: /* DiffFuncId: DIFFERENTIAL VALUE $@7 DiffId DIVIDEOP DIFFERENTIAL VALUE $@8 IDENTIFIER  */
                                                                                                                                             {ExprIntrp_DiffDegreeVar();}
    break;

  case 45: /* DiffId: IDENTIFIER  */
                                {ExprIntrp_StartDifferential();}
    break;

  case 47: /* $@9: %empty  */
                                {ExprIntrp_StartFunction();}
    break;

  case 49: /* ListArg: unarg  */
                           {ExprIntrp_EndFuncArg();}
    break;

  case 50: /* $@10: %empty  */
                                 {ExprIntrp_NextFuncArg();}
    break;

  case 52: /* unarg: IDENTIFIER  */
                                {ExprIntrp_VariableIdentifier();}
    break;

  case 53: /* $@11: %empty  */
                                                               {ExprIntrp_Derivation();}
    break;

  case 54: /* Derivation: DERIVKEY BRACKET GenExpr COMMA IDENTIFIER $@11 ENDBRACKET  */
                                                                                                    {ExprIntrp_EndDerivation();}
    break;

  case 55: /* $@12: %empty  */
                                                               {ExprIntrp_Derivation();}
    break;

  case 56: /* $@13: %empty  */
                                                                                                     {ExprIntrp_DerivationValue();}
    break;

  case 57: /* Derivation: DERIVKEY BRACKET GenExpr COMMA IDENTIFIER $@12 COMMA VALUE $@13 ENDBRACKET  */
                                                                                                                                               {ExprIntrp_EndDerivation();}
    break;

  case 58: /* $@14: %empty  */
                                                 {ExprIntrp_ConstantIdentifier();}
    break;

  case 59: /* $@15: %empty  */
                                                                                               {ExprIntrp_ConstantDefinition();}
    break;

  case 61: /* $@16: %empty  */
                                                             {ExprIntrp_VariableIdentifier();}
    break;

  case 62: /* $@17: %empty  */
                                                                                                                                       {ExprIntrp_NumValue();}
    break;

  case 63: /* Sumator: SUMKEY BRACKET GenExpr COMMA IDENTIFIER $@16 COMMA GenExpr COMMA GenExpr COMMA VALUE $@17 ENDBRACKET  */
                                                                                                                                                                          {ExprIntrp_Sumator();}
    break;

  case 64: /* $@18: %empty  */
                                                              {ExprIntrp_VariableIdentifier();}
    break;

  case 65: /* $@19: %empty  */
                                                                                                                                        {ExprIntrp_NumValue();}
    break;

  case 66: /* Productor: PRODKEY BRACKET GenExpr COMMA IDENTIFIER $@18 COMMA GenExpr COMMA GenExpr COMMA VALUE $@19 ENDBRACKET  */
                                                                                                                                                                           {ExprIntrp_Productor();}
    break;

  case 70: /* SingleRelation: GenExpr EQUALOP GenExpr  */
                                             {ExprIntrp_EndOfEqual();}
    break;



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
      yyerror (YY_("syntax error"));
    }

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
                      yytoken, &yylval);
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


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


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


#if !defined yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
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
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

