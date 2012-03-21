/*
 Copyright (c) 1999-2012 OPEN CASCADE SAS

 The content of this file is subject to the Open CASCADE Technology Public
 License Version 6.5 (the "License"). You may not use the content of this file
 except in compliance with the License. Please obtain a copy of the License
 at http://www.opencascade.org and read it completely before using this file.

 The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
 main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.

 The Original Code and all software distributed under the License is
 distributed on an "AS IS" basis, without warranty of any kind, and the
 Initial Developer hereby disclaims all such warranties, including without
 limitation, any warranties of merchantability, fitness for a particular
 purpose or non-infringement. Please see the License for the specific terms
 and conditions governing the rights and limitations under the License.

*/

/*  A Bison parser, made from j:/kas/c40/ros/src/exprintrp/exprintrp.yacc
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse ExprIntrpparse
#define yylex ExprIntrplex
#define yyerror ExprIntrperror
#define yylval ExprIntrplval
#define yychar ExprIntrpchar
#define yydebug ExprIntrpdebug
#define yynerrs ExprIntrpnerrs
#define	SUMOP	258
#define	MINUSOP	259
#define	DIVIDEOP	260
#define	EXPOP	261
#define	MULTOP	262
#define	PARENTHESIS	263
#define	BRACKET	264
#define	ENDPARENTHESIS	265
#define	ENDBRACKET	266
#define	VALUE	267
#define	IDENTIFIER	268
#define	COMMA	269
#define	DIFFERENTIAL	270
#define	DERIVATE	271
#define	DERIVKEY	272
#define	ASSIGNOP	273
#define	DEASSIGNKEY	274
#define	EQUALOP	275
#define	RELSEPARATOR	276
#define	CONSTKEY	277
#define	SUMKEY	278
#define	PRODKEY	279

#line 1 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"

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
#line 52 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"

#ifndef YYSTYPE
#define YYSTYPE int
#endif
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		147
#define	YYFLAG		-32768
#define	YYNTBASE	26

#define YYTRANSLATE(x) ((unsigned)(x) <= 279 ? yytranslate[x] : 67)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    25,
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
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     4,     6,     8,    10,    11,    16,    17,    23,
    27,    31,    35,    39,    43,    47,    51,    54,    56,    58,
    60,    62,    64,    66,    68,    70,    72,    77,    82,    83,
    89,    91,    92,    97,    99,   100,   105,   106,   110,   116,
   117,   118,   128,   130,   132,   133,   139,   141,   142,   147,
   149,   150,   158,   159,   160,   171,   172,   173,   182,   183,
   184,   199,   200,   201,   216,   218,   222,   226
};

static const short yyrhs[] = {    31,
     0,    27,     0,    29,     0,    39,     0,    65,     0,     0,
    13,    28,    18,    31,     0,     0,    19,     9,    13,    30,
    11,     0,    31,     3,    31,     0,    31,     4,    31,     0,
    31,     7,    31,     0,    31,     5,    31,     0,    31,     6,
    31,     0,     8,    31,    10,     0,     9,    31,    11,     0,
     4,    31,     0,    32,     0,    52,     0,    56,     0,    59,
     0,    62,     0,    33,     0,    34,     0,    13,     0,    12,
     0,    38,     8,    36,    10,     0,    41,     8,    36,    10,
     0,     0,    43,    35,     8,    36,    10,     0,    31,     0,
     0,    31,    14,    37,    36,     0,    13,     0,     0,    47,
    40,    18,    31,     0,     0,    13,    42,    16,     0,    15,
    46,     5,    15,    13,     0,     0,     0,    15,    12,    44,
    46,     5,    15,    12,    45,    13,     0,    13,     0,    43,
     0,     0,    13,    48,     9,    49,    11,     0,    51,     0,
     0,    51,    14,    50,    49,     0,    13,     0,     0,    17,
     9,    31,    14,    13,    53,    11,     0,     0,     0,    17,
     9,    31,    14,    13,    54,    14,    12,    55,    11,     0,
     0,     0,    22,     9,    13,    57,    14,    12,    58,    11,
     0,     0,     0,    23,     9,    31,    14,    13,    60,    14,
    31,    14,    31,    14,    12,    61,    11,     0,     0,     0,
    24,     9,    31,    14,    13,    63,    14,    31,    14,    31,
    14,    12,    64,    11,     0,    66,     0,    66,    21,    65,
     0,    66,    25,    65,     0,    31,    20,    31,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    56,    57,    58,    59,    60,    63,    63,    66,    67,    69,
    70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
    80,    81,    84,    85,    89,    90,    93,    94,    95,    95,
    98,    99,   100,   102,   105,   105,   108,   108,   111,   112,
   112,   112,   115,   116,   119,   119,   122,   123,   124,   126,
   129,   129,   130,   130,   130,   133,   133,   134,   136,   136,
   136,   139,   139,   139,   142,   143,   144,   147
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","SUMOP",
"MINUSOP","DIVIDEOP","EXPOP","MULTOP","PARENTHESIS","BRACKET","ENDPARENTHESIS",
"ENDBRACKET","VALUE","IDENTIFIER","COMMA","DIFFERENTIAL","DERIVATE","DERIVKEY",
"ASSIGNOP","DEASSIGNKEY","EQUALOP","RELSEPARATOR","CONSTKEY","SUMKEY","PRODKEY",
"'\\n'","exprentry","Assignment","@1","Deassignment","@2","GenExpr","SingleExpr",
"Single","Function","@3","ListGenExpr","@4","funcident","FunctionDefinition",
"@5","DerFunctionId","@6","DiffFuncId","@7","@8","DiffId","FunctionDef","@9",
"ListArg","@10","unarg","Derivation","@11","@12","@13","ConstantDefinition",
"@14","@15","Sumator","@16","@17","Productor","@18","@19","RelationList","SingleRelation", NULL
};
#endif

static const short yyr1[] = {     0,
    26,    26,    26,    26,    26,    28,    27,    30,    29,    31,
    31,    31,    31,    31,    31,    31,    31,    31,    31,    31,
    31,    31,    32,    32,    33,    33,    34,    34,    35,    34,
    36,    37,    36,    38,    40,    39,    42,    41,    43,    44,
    45,    43,    46,    46,    48,    47,    49,    50,    49,    51,
    53,    52,    54,    55,    52,    57,    58,    56,    60,    61,
    59,    63,    64,    62,    65,    65,    65,    66
};

static const short yyr2[] = {     0,
     1,     1,     1,     1,     1,     0,     4,     0,     5,     3,
     3,     3,     3,     3,     3,     3,     2,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     4,     4,     0,     5,
     1,     0,     4,     1,     0,     4,     0,     3,     5,     0,
     0,     9,     1,     1,     0,     5,     1,     0,     4,     1,
     0,     7,     0,     0,    10,     0,     0,     8,     0,     0,
    14,     0,     0,    14,     1,     3,     3,     3
};

static const short yydefact[] = {     0,
     0,     0,     0,    26,    25,     0,     0,     0,     0,     0,
     0,     2,     3,     1,    18,    23,    24,     0,     4,     0,
    29,    35,    19,    20,    21,    22,     5,    65,    25,    17,
     0,     0,     0,     0,     0,    40,    43,    44,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    15,    16,     0,    38,
     0,     0,     0,     0,     8,    56,     0,     0,    10,    11,
    13,    14,    12,    68,    31,     0,     0,     0,     0,     0,
    66,    67,     7,    50,     0,    47,     0,     0,     0,     0,
     0,     0,     0,    32,    27,    28,     0,    36,    46,    48,
     0,    39,    51,     9,     0,    59,    62,     0,    30,     0,
     0,     0,     0,    57,     0,     0,    33,    49,    41,    52,
     0,     0,     0,     0,     0,    54,    58,     0,     0,    42,
     0,     0,     0,    55,     0,     0,     0,     0,    60,    63,
     0,     0,    61,    64,     0,     0,     0
};

static const short yydefgoto[] = {   145,
    12,    33,    13,    90,    75,    15,    16,    17,    53,    76,
   108,    18,    19,    54,    20,    34,    21,    62,   125,    39,
    22,    35,    85,   110,    86,    23,   112,   113,   131,    24,
    91,   122,    25,   115,   141,    26,   116,   142,    27,    28
};

static const short yypact[] = {     2,
    54,    54,    54,-32768,     4,    60,     9,    18,    20,    24,
    30,-32768,-32768,    31,-32768,-32768,-32768,     8,-32768,    49,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,     7,    15,    47,
   161,   152,   -10,    48,    56,-32768,-32768,-32768,    63,    54,
    57,    61,    54,    54,    54,    54,    54,    54,    54,    54,
    54,    54,    84,    72,    54,    54,-32768,-32768,    54,-32768,
    80,    -6,    79,    77,-32768,-32768,    82,    94,    47,    47,
    89,-32768,    89,   169,    99,    97,   100,    54,    54,    31,
-32768,-32768,   169,-32768,    98,   105,   106,   107,   108,   101,
   117,   109,   121,-32768,-32768,-32768,   125,   169,-32768,-32768,
   133,-32768,   122,-32768,   138,-32768,-32768,    54,-32768,    80,
   139,   141,   146,-32768,   147,   148,-32768,-32768,-32768,-32768,
   157,   142,    54,    54,   164,-32768,-32768,   111,   123,-32768,
   159,    54,    54,-32768,   135,   140,   166,   167,-32768,-32768,
   170,   171,-32768,-32768,   180,   183,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,-32768,     0,-32768,-32768,-32768,-32768,   -48,
-32768,-32768,-32768,-32768,-32768,-32768,    -1,-32768,-32768,   124,
-32768,-32768,    74,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   -14,-32768
};


#define	YYLAST		186


static const short yytable[] = {    14,
    30,    31,    32,    77,    38,     1,    37,    59,     6,     2,
     3,   -34,   -45,     4,     5,    51,     6,    40,     7,   -37,
     8,    -6,   -34,     9,    10,    11,    41,    55,    42,    97,
   -37,    56,    43,    45,    46,    47,    48,    49,    44,    64,
    81,    82,    67,    68,    69,    70,    71,    72,    73,    74,
    50,    47,    48,    49,    80,    80,    52,     1,    83,   117,
    38,     2,     3,    60,    61,     4,    29,    63,     6,    65,
     7,    36,    37,    66,     6,     9,    10,    11,    98,    45,
    46,    47,    48,    49,    45,    46,    47,    48,    49,    79,
    89,    78,    84,    88,    48,    92,    45,    46,    47,    48,
    49,    45,    46,    47,    48,    49,    95,    93,    99,    96,
   101,   104,    94,    45,    46,    47,    48,    49,   100,   102,
   103,   106,   128,   129,   132,    45,    46,    47,    48,    49,
   105,   135,   136,   107,   109,   -53,   133,    45,    46,    47,
    48,    49,    45,    46,    47,    48,    49,   111,   137,   114,
   119,   120,   127,   138,    45,    46,    47,    48,    49,   121,
   123,   124,    58,    45,    46,    47,    48,    49,   126,   134,
    57,    45,    46,    47,    48,    49,   130,   139,   140,   146,
   143,   144,   147,   118,     0,    87
};

static const short yycheck[] = {     0,
     1,     2,     3,    52,     6,     4,    13,    18,    15,     8,
     9,     8,     9,    12,    13,     8,    15,     9,    17,    16,
    19,    18,     8,    22,    23,    24,     9,    21,     9,    78,
    16,    25,     9,     3,     4,     5,     6,     7,     9,    40,
    55,    56,    43,    44,    45,    46,    47,    48,    49,    50,
    20,     5,     6,     7,    55,    56,     8,     4,    59,   108,
    62,     8,     9,    16,     9,    12,    13,     5,    15,    13,
    17,    12,    13,    13,    15,    22,    23,    24,    79,     3,
     4,     5,     6,     7,     3,     4,     5,     6,     7,    18,
    14,     8,    13,    15,     6,    14,     3,     4,     5,     6,
     7,     3,     4,     5,     6,     7,    10,    14,    11,    10,
     5,    11,    14,     3,     4,     5,     6,     7,    14,    13,
    13,    13,   123,   124,    14,     3,     4,     5,     6,     7,
    14,   132,   133,    13,    10,    14,    14,     3,     4,     5,
     6,     7,     3,     4,     5,     6,     7,    15,    14,    12,
    12,    11,    11,    14,     3,     4,     5,     6,     7,    14,
    14,    14,    11,     3,     4,     5,     6,     7,    12,    11,
    10,     3,     4,     5,     6,     7,    13,    12,    12,     0,
    11,    11,     0,   110,    -1,    62
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Bob Corbett and Richard Stallman

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */


#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if (  defined( MSDOS ) || defined( WNT )  ) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#include <malloc.h>
#define alloca malloc
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int yyparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#if defined( WNT )
#define __yy_bcopy(FROM,TO,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#endif
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
#if defined( WNT )
__yy_bcopy (__from, __to, __count)
     char *__from;
     char *__to;
     int __count;
#else
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     int count;
#endif
{
#ifdef WNT 
  register char *f = __from;
  register char *t = __to;
  register int i = __count;
#else
  register char *f = from;
  register char *t = to;
  register int i = count;
#endif
  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
#if defined(WNT)
__yy_bcopy (char *__from, char *__to, int __count)
#else
__yy_memcpy (char *to, char *from, int count)
#endif
{
#if defined(WNT)
  register char *f = __from;
  register char *t = __to;
  register int i = __count;
#else
  register char *f = from;
  register char *t = to;
  register int i = count;
#endif

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 196 "/PRODUCTS/flexbis-253-125/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

int
#if defined(WNT)
yyparse()
#else
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
#endif
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
#if defined(WNT)
      __yy_bcopy ((char *)yyss1, (char *)yyss, size * sizeof (*yyssp));
#else
      __yy_memcpy ((char *)yyss, (char *)yyss1, size * sizeof (*yyssp));
#endif
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
#if defined(WNT)
      __yy_bcopy ((char *)yyvs1, (char *)yyvs, size * sizeof (*yyvsp));
#else
      __yy_memcpy ((char *)yyvs, (char *)yyvs1, size * sizeof (*yyvsp));
#endif
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
#if defined(WNT)
      __yy_bcopy ((char *)yyls1, (char *)yyls, size * sizeof (*yylsp));
#else
      __yy_memcpy ((char *)yyls, (char *)yyls1, size * sizeof (*yylsp));
#endif
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 4:
#line 59 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_EndOfFuncDef();;
    break;}
case 5:
#line 60 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_EndOfRelation();;
    break;}
case 6:
#line 63 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_AssignVariable();;
    break;}
case 7:
#line 63 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_EndOfAssign();;
    break;}
case 8:
#line 66 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_Deassign();;
    break;}
case 10:
#line 69 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_SumOperator();;
    break;}
case 11:
#line 70 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_MinusOperator();;
    break;}
case 12:
#line 71 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_ProductOperator();;
    break;}
case 13:
#line 72 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_DivideOperator();;
    break;}
case 14:
#line 73 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_ExpOperator();;
    break;}
case 17:
#line 76 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_UnaryMinusOperator();;
    break;}
case 25:
#line 89 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_VariableIdentifier();;
    break;}
case 26:
#line 90 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_NumValue();;
    break;}
case 27:
#line 93 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_EndFunction();;
    break;}
case 28:
#line 94 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_EndDerFunction();;
    break;}
case 29:
#line 95 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_EndDifferential();;
    break;}
case 30:
#line 95 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_EndDiffFunction();;
    break;}
case 31:
#line 98 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_EndFuncArg();;
    break;}
case 32:
#line 99 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_NextFuncArg();;
    break;}
case 34:
#line 102 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_StartFunction();;
    break;}
case 35:
#line 105 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_DefineFunction();;
    break;}
case 37:
#line 108 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_StartDerivate();;
    break;}
case 38:
#line 108 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_EndDerivate();;
    break;}
case 39:
#line 111 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_DiffVar();;
    break;}
case 40:
#line 112 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_DiffDegree();;
    break;}
case 41:
#line 112 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_VerDiffDegree();;
    break;}
case 42:
#line 112 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_DiffDegreeVar();;
    break;}
case 43:
#line 115 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_StartDifferential();;
    break;}
case 45:
#line 119 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_StartFunction();;
    break;}
case 47:
#line 122 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_EndFuncArg();;
    break;}
case 48:
#line 123 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_NextFuncArg();;
    break;}
case 50:
#line 126 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_VariableIdentifier();;
    break;}
case 51:
#line 129 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_Derivation();;
    break;}
case 52:
#line 129 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_EndDerivation();;
    break;}
case 53:
#line 130 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_Derivation();;
    break;}
case 54:
#line 130 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_DerivationValue();;
    break;}
case 55:
#line 130 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_EndDerivation();;
    break;}
case 56:
#line 133 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_ConstantIdentifier();;
    break;}
case 57:
#line 133 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_ConstantDefinition();;
    break;}
case 59:
#line 136 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_VariableIdentifier();;
    break;}
case 60:
#line 136 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_NumValue();;
    break;}
case 61:
#line 136 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_Sumator();;
    break;}
case 62:
#line 139 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_VariableIdentifier();;
    break;}
case 63:
#line 139 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_NumValue();;
    break;}
case 64:
#line 139 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_Productor();;
    break;}
case 68:
#line 147 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
{ExprIntrp_EndOfEqual();;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 442 "bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}
#line 150 "j:/kas/c40/ros/src/exprintrp/exprintrp.yacc"
