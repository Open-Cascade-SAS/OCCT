%{

// Created:	Thu Oct 28 12:21:16 1999
// Author:	Andrey BETENEV
// Copyright (c) 1999-2020 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

/*****************************************************************************

This LEX scanner is performs lexical analysis of EXPRESS schema file   
for EXPRESS -> CASCADE/XSTEP classes generator                             

On the stage of lexical scanner comments (single- and multi-line),         
definitions of CONSTANT, FUNCTION, RULE items and clauses WHERE, INVERSE
and DERIVE of TYPE amd ENTITY items are dismissed (ignored)         

Then, keywords such as ENTITY, SUPERTYPE, SET etc., names of items         
and special symbols are identified and returned to parser (yacc) as tokens 

Also, error processing and current line number tracking functions are defined

*****************************************************************************/

/************************************/
/* Section 1                        */
/* definitions                      */

#include <stdlib.h>
#include <string.h>

#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <Express_HSequenceOfField.hxx>
#include <Express_HSequenceOfItem.hxx>
#include <Express_Field.hxx>
#include <Express_Item.hxx>
#include <Express_Type.hxx>
#include <Express_Schema.hxx>
#include <Express_Reference.hxx>

#include "exptocas.tab.hxx"       /* define tokens */

/* Auxiliary functions */

static int fun_level=0;
static int ec_linenum=1;
static int ec_state = 0;

int yywrap(void) { return 1; }

int ec_curline ( void )
{
  return ec_linenum;
}

int ec_error ( const std::string& s, const std::string& text )
{
  printf ( "\nError at line %d: %s \"%s\"\n", ec_curline(), s.c_str(), text.c_str() );
  return 0;
}

%}

/*
    c++                 generate C++ parser class
*/
%option c++
%option noyywrap
%option yyclass="exptocas::scanner"
%option nounistd

%s TYP ENT
%x COMM SKP RULE FUN

%a 4000
%o 6000

%top{
// Pre-include stdlib.h to avoid redefinition of integer type macros (INT8_MIN and similar in generated code)
#if !defined(_MSC_VER) || (_MSC_VER >= 1600) // Visual Studio 2010+
#include "stdint.h"
#endif
}

%{

// Tell flex which function to define
#ifdef  YY_DECL
# undef YY_DECL
#endif
#define YY_DECL int exptocas::scanner::lex (exptocas::parser::semantic_type* yylval)

typedef exptocas::parser::token token;

/************************************/
/* Section 2                        */
/* parsing rules                    */
%}

%%

"--".*               { /* Eat line comments */ }
"(*"                 { ec_state = YYSTATE; BEGIN(COMM); }
<COMM>.              { /* Eat multiline comments */ }
<COMM>"*)"           { BEGIN(ec_state); }

"SCHEMA"             { return yylval->num = token::KSCHEM; }
"END_SCHEMA"         { return yylval->num = token::KENDS; }

"TYPE"               { BEGIN(TYP); return yylval->num = token::KTYP; }
<TYP,SKP>"END_TYPE"  { BEGIN(0);   return yylval->num = token::KENDT; }

"ENTITY"             { BEGIN(ENT); return yylval->num = token::KENT; }
<ENT,SKP>"END_ENTITY" { BEGIN(0);  return yylval->num = token::KENDE; }

<ENT>"INVERSE"       |
<ENT>"DERIVE"        |
<ENT,TYP>"WHERE"     { BEGIN(SKP); }
<SKP>.               { /* eat contents of WHERE and DERIVE subclauses of ENTITY and TYPE */ }

"SELECT"             { return yylval->num = token::KSEL; }
"ENUMERATION"        { return yylval->num = token::KENUM; }
"LIST"               { return yylval->num = token::KLIST; }
"ARRAY"              { return yylval->num = token::KARR; }
"SET"                { return yylval->num = token::KSET; }
"BAG"                { return yylval->num = token::KBAG; }
"OF"                 { return yylval->num = token::KOF; }

"NUMBER"             { return yylval->num = token::KNUM; }
"INTEGER"            { return yylval->num = token::KINT; }
"REAL"               { return yylval->num = token::KDBL; }
"STRING"             { return yylval->num = token::KSTR; }
"LOGICAL"            { return yylval->num = token::KLOG; }
"BOOLEAN"            { return yylval->num = token::KBOOL; }

"OPTIONAL"           { return yylval->num = token::KOPT; }
"UNIQUE"             { return yylval->num = token::KUNIQ; }
"SELF"               { return yylval->num = token::KSELF; }

"ABSTRACT"           { return yylval->num = token::KABSTR; }
"SUBTYPE"            { return yylval->num = token::KSUBT; }
"SUPERTYPE"          { return yylval->num = token::KSPRT; }
"ANDOR"              { return yylval->num = token::KANDOR; }
"ONEOF"              { return yylval->num = token::K1OF; }
"AND"                { return yylval->num = token::KAND; }

"UR"[0-9]+           { yylval->str = strdup ( yytext ); return token::NAME; }

[a-z_][a-z_0-9]*     { yylval->str = strdup ( yytext ); return token::NAME; }

[0-9]+               { yylval->num = atoi ( yytext ); return token::NUMBER; }
[,=();?:.\\]|"["|"]" { return yylval->num = yytext[0]; }

<INITIAL,FUN>"FUNCTION"  { BEGIN(FUN); fun_level++; }
<FUN>"(*"            { ec_state = YYSTATE; BEGIN(COMM); /* eat comments in functions */ }
<FUN>"--".*          { /* Eat line comments in functions */ }
<FUN>[A-Za-z_0-9]*   { /* eat FUNCTIONs - skip IDs explicitly */ }
<FUN>\'[^\']*\'      { /* eat FUNCTIONs - skip strings explicitly */ }
<FUN>.               { /* eat FUNCTIONs - skip all other symbols in functions */ }
<FUN>"END_FUNCTION;" { fun_level--; if ( ! fun_level ) BEGIN(0); }

"RULE"               { BEGIN(RULE); /* eat RULEs */ }
<RULE>.              { /* eat RULEs */ }
<RULE>"END_RULE;"    { BEGIN(0); }

"CONSTANT"[ \t\na-z_0-9:=;'()|-]+"END_CONSTANT;" { /* eat CONSTANTs */
                       char *s = yytext; /* but don't forget to count lines.. */
                       while ( *s ) if ( *(s++) == '\n' ) ec_linenum++;
                     }

[ \t]+               { /* eat spaces */ }
[A-Za-z0-9_]+        { ec_error ( "unknown keyword ", yytext ); /* put unrecognized keywords to cerr */ }
.                    { ec_error ( "unknown symbol ", yytext ); /* put unrecognized data to cerr */ }

<INITIAL,COMM,SKP,RULE,ENT,TYP,FUN>\n  { ec_linenum++; /* count lines */ }

%%

/************************************/
/* Section 3                        */
/* auxiliary procedures             */

exptocas::scanner::scanner(std::istream* in, std::ostream* out)
    : exptocasFlexLexer(in, out)
{
}

/*
int main ( void )
{
  yylex();
}
*/

