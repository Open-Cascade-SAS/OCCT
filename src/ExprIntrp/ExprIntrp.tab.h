/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison interface for Yacc-like parsers in C
   
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
