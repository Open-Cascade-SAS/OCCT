/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
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

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.7"

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
#define yylval          ExprIntrplval
#define yychar          ExprIntrpchar
#define yydebug         ExprIntrpdebug
#define yynerrs         ExprIntrpnerrs

/* Copy the first part of user declarations.  */
/* Line 371 of yacc.c  */
#line 17 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"

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

/* Line 371 of yacc.c  */
#line 77 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"


/* Line 371 of yacc.c  */
#line 133 "ExprIntrp.tab.c"

# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "ExprIntrp.tab.h".  */
#ifndef YY_EXPRINTRP_EXPRINTRP_TAB_H_INCLUDED
# define YY_EXPRINTRP_EXPRINTRP_TAB_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int ExprIntrpdebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     SUMOP = 258,
     MINUSOP = 259,
     DIVIDEOP = 260,
     EXPOP = 261,
     MULTOP = 262,
     PARENTHESIS = 263,
     BRACKET = 264,
     ENDPARENTHESIS = 265,
     ENDBRACKET = 266,
     VALUE = 267,
     IDENTIFIER = 268,
     COMMA = 269,
     DIFFERENTIAL = 270,
     DERIVATE = 271,
     DERIVKEY = 272,
     ASSIGNOP = 273,
     DEASSIGNKEY = 274,
     EQUALOP = 275,
     RELSEPARATOR = 276,
     CONSTKEY = 277,
     SUMKEY = 278,
     PRODKEY = 279
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE ExprIntrplval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int ExprIntrpparse (void *YYPARSE_PARAM);
#else
int ExprIntrpparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int ExprIntrpparse (void);
#else
int ExprIntrpparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_EXPRINTRP_EXPRINTRP_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

/* Line 390 of yacc.c  */
#line 223 "ExprIntrp.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

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

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(N) (N)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
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
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
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
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (YYID (0))
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
/* YYNRULES -- Number of states.  */
#define YYNSTATES  149

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   279

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
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
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    11,    13,    14,    19,
      20,    26,    30,    34,    38,    42,    46,    50,    54,    57,
      60,    62,    64,    66,    68,    70,    72,    74,    76,    78,
      83,    88,    89,    95,    97,    98,   103,   105,   106,   111,
     112,   116,   122,   123,   124,   134,   136,   138,   139,   145,
     147,   148,   153,   155,   156,   164,   165,   166,   177,   178,
     179,   188,   189,   190,   205,   206,   207,   222,   224,   228,
     232
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      27,     0,    -1,    32,    -1,    28,    -1,    30,    -1,    40,
      -1,    66,    -1,    -1,    13,    29,    18,    32,    -1,    -1,
      19,     9,    13,    31,    11,    -1,    32,     3,    32,    -1,
      32,     4,    32,    -1,    32,     7,    32,    -1,    32,     5,
      32,    -1,    32,     6,    32,    -1,     8,    32,    10,    -1,
       9,    32,    11,    -1,     4,    32,    -1,     3,    32,    -1,
      33,    -1,    53,    -1,    57,    -1,    60,    -1,    63,    -1,
      34,    -1,    35,    -1,    13,    -1,    12,    -1,    39,     8,
      37,    10,    -1,    42,     8,    37,    10,    -1,    -1,    44,
      36,     8,    37,    10,    -1,    32,    -1,    -1,    32,    14,
      38,    37,    -1,    13,    -1,    -1,    48,    41,    18,    32,
      -1,    -1,    13,    43,    16,    -1,    15,    47,     5,    15,
      13,    -1,    -1,    -1,    15,    12,    45,    47,     5,    15,
      12,    46,    13,    -1,    13,    -1,    44,    -1,    -1,    13,
      49,     9,    50,    11,    -1,    52,    -1,    -1,    52,    14,
      51,    50,    -1,    13,    -1,    -1,    17,     9,    32,    14,
      13,    54,    11,    -1,    -1,    -1,    17,     9,    32,    14,
      13,    55,    14,    12,    56,    11,    -1,    -1,    -1,    22,
       9,    13,    58,    14,    12,    59,    11,    -1,    -1,    -1,
      23,     9,    32,    14,    13,    61,    14,    32,    14,    32,
      14,    12,    62,    11,    -1,    -1,    -1,    24,     9,    32,
      14,    13,    64,    14,    32,    14,    32,    14,    12,    65,
      11,    -1,    67,    -1,    67,    21,    66,    -1,    67,    25,
      66,    -1,    32,    20,    32,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    81,    81,    82,    83,    84,    85,    88,    88,    91,
      91,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   110,   111,   115,   116,   119,
     120,   121,   121,   124,   125,   125,   128,   131,   131,   134,
     134,   137,   138,   138,   138,   141,   142,   145,   145,   148,
     149,   149,   152,   155,   155,   156,   156,   156,   159,   159,
     159,   162,   162,   162,   165,   165,   165,   168,   169,   170,
     173
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "SUMOP", "MINUSOP", "DIVIDEOP", "EXPOP",
  "MULTOP", "PARENTHESIS", "BRACKET", "ENDPARENTHESIS", "ENDBRACKET",
  "VALUE", "IDENTIFIER", "COMMA", "DIFFERENTIAL", "DERIVATE", "DERIVKEY",
  "ASSIGNOP", "DEASSIGNKEY", "EQUALOP", "RELSEPARATOR", "CONSTKEY",
  "SUMKEY", "PRODKEY", "'\\n'", "$accept", "exprentry", "Assignment",
  "$@1", "Deassignment", "$@2", "GenExpr", "SingleExpr", "Single",
  "Function", "$@3", "ListGenExpr", "$@4", "funcident",
  "FunctionDefinition", "$@5", "DerFunctionId", "$@6", "DiffFuncId", "$@7",
  "$@8", "DiffId", "FunctionDef", "$@9", "ListArg", "$@10", "unarg",
  "Derivation", "$@11", "$@12", "$@13", "ConstantDefinition", "$@14",
  "$@15", "Sumator", "$@16", "$@17", "Productor", "$@18", "$@19",
  "RelationList", "SingleRelation", YY_NULL
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,    10
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
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

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
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

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
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

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    13,    14,    36,    15,    94,    79,    17,    18,    19,
      57,    80,   112,    20,    21,    58,    22,    37,    23,    66,
     129,    42,    24,    38,    89,   114,    90,    25,   116,   117,
     135,    26,    95,   126,    27,   119,   145,    28,   120,   146,
      29,    30
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -51
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

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -51,   -51,   -51,   -51,   -51,   -51,     0,   -51,   -51,   -51,
     -51,   -50,   -51,   -51,   -51,   -51,   -51,   -51,    -2,   -51,
     -51,   123,   -51,   -51,    74,   -51,   -51,   -51,   -51,   -51,
     -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,
     -35,   -51
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -56
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

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-51)))

#define yytable_value_is_error(Yytable_value) \
  YYID (0)

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
static const yytype_uint8 yystos[] =
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

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
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
      YYERROR;							\
    }								\
while (YYID (0))

/* Error token number */
#define YYTERROR	1
#define YYERRCODE	256


/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */
#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
        break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
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


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
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
	    /* Fall through.  */
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

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULL, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULL;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
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
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
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
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULL, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
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
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
        break;
    }
}




/* The lookahead symbol.  */
int yychar;


#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval YY_INITIAL_VALUE(yyval_default);

/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

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

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
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

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

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
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 5:
/* Line 1792 of yacc.c  */
#line 84 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_EndOfFuncDef();}
    break;

  case 6:
/* Line 1792 of yacc.c  */
#line 85 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_EndOfRelation();}
    break;

  case 7:
/* Line 1792 of yacc.c  */
#line 88 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_AssignVariable();}
    break;

  case 8:
/* Line 1792 of yacc.c  */
#line 88 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_EndOfAssign();}
    break;

  case 9:
/* Line 1792 of yacc.c  */
#line 91 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_Deassign();}
    break;

  case 11:
/* Line 1792 of yacc.c  */
#line 94 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_SumOperator();}
    break;

  case 12:
/* Line 1792 of yacc.c  */
#line 95 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_MinusOperator();}
    break;

  case 13:
/* Line 1792 of yacc.c  */
#line 96 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_ProductOperator();}
    break;

  case 14:
/* Line 1792 of yacc.c  */
#line 97 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_DivideOperator();}
    break;

  case 15:
/* Line 1792 of yacc.c  */
#line 98 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_ExpOperator();}
    break;

  case 18:
/* Line 1792 of yacc.c  */
#line 101 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_UnaryMinusOperator();}
    break;

  case 19:
/* Line 1792 of yacc.c  */
#line 102 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_UnaryPlusOperator();}
    break;

  case 27:
/* Line 1792 of yacc.c  */
#line 115 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_VariableIdentifier();}
    break;

  case 28:
/* Line 1792 of yacc.c  */
#line 116 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_NumValue();}
    break;

  case 29:
/* Line 1792 of yacc.c  */
#line 119 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_EndFunction();}
    break;

  case 30:
/* Line 1792 of yacc.c  */
#line 120 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_EndDerFunction();}
    break;

  case 31:
/* Line 1792 of yacc.c  */
#line 121 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_EndDifferential();}
    break;

  case 32:
/* Line 1792 of yacc.c  */
#line 121 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_EndDiffFunction();}
    break;

  case 33:
/* Line 1792 of yacc.c  */
#line 124 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_EndFuncArg();}
    break;

  case 34:
/* Line 1792 of yacc.c  */
#line 125 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_NextFuncArg();}
    break;

  case 36:
/* Line 1792 of yacc.c  */
#line 128 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_StartFunction();}
    break;

  case 37:
/* Line 1792 of yacc.c  */
#line 131 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_DefineFunction();}
    break;

  case 39:
/* Line 1792 of yacc.c  */
#line 134 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_StartDerivate();}
    break;

  case 40:
/* Line 1792 of yacc.c  */
#line 134 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_EndDerivate();}
    break;

  case 41:
/* Line 1792 of yacc.c  */
#line 137 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_DiffVar();}
    break;

  case 42:
/* Line 1792 of yacc.c  */
#line 138 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_DiffDegree();}
    break;

  case 43:
/* Line 1792 of yacc.c  */
#line 138 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_VerDiffDegree();}
    break;

  case 44:
/* Line 1792 of yacc.c  */
#line 138 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_DiffDegreeVar();}
    break;

  case 45:
/* Line 1792 of yacc.c  */
#line 141 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_StartDifferential();}
    break;

  case 47:
/* Line 1792 of yacc.c  */
#line 145 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_StartFunction();}
    break;

  case 49:
/* Line 1792 of yacc.c  */
#line 148 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_EndFuncArg();}
    break;

  case 50:
/* Line 1792 of yacc.c  */
#line 149 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_NextFuncArg();}
    break;

  case 52:
/* Line 1792 of yacc.c  */
#line 152 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_VariableIdentifier();}
    break;

  case 53:
/* Line 1792 of yacc.c  */
#line 155 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_Derivation();}
    break;

  case 54:
/* Line 1792 of yacc.c  */
#line 155 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_EndDerivation();}
    break;

  case 55:
/* Line 1792 of yacc.c  */
#line 156 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_Derivation();}
    break;

  case 56:
/* Line 1792 of yacc.c  */
#line 156 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_DerivationValue();}
    break;

  case 57:
/* Line 1792 of yacc.c  */
#line 156 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_EndDerivation();}
    break;

  case 58:
/* Line 1792 of yacc.c  */
#line 159 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_ConstantIdentifier();}
    break;

  case 59:
/* Line 1792 of yacc.c  */
#line 159 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_ConstantDefinition();}
    break;

  case 61:
/* Line 1792 of yacc.c  */
#line 162 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_VariableIdentifier();}
    break;

  case 62:
/* Line 1792 of yacc.c  */
#line 162 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_NumValue();}
    break;

  case 63:
/* Line 1792 of yacc.c  */
#line 162 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_Sumator();}
    break;

  case 64:
/* Line 1792 of yacc.c  */
#line 165 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_VariableIdentifier();}
    break;

  case 65:
/* Line 1792 of yacc.c  */
#line 165 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_NumValue();}
    break;

  case 66:
/* Line 1792 of yacc.c  */
#line 165 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_Productor();}
    break;

  case 70:
/* Line 1792 of yacc.c  */
#line 173 "D:/ABV/OCCT/occt7/src/ExprIntrp/ExprIntrp.yacc"
    {ExprIntrp_EndOfEqual();}
    break;


/* Line 1792 of yacc.c  */
#line 1831 "ExprIntrp.tab.c"
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
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
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

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
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
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
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
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

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

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


