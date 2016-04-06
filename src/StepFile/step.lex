/* 
 Copyright (c) 1999-2014 OPEN CASCADE SAS

 This file is part of Open CASCADE Technology software library.

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License version 2.1 as published
 by the Free Software Foundation, with special exception defined in the file
 OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
 distribution for complete text of the license and disclaimer of any warranty.

 Alternatively, this file may be used under the terms of Open CASCADE
 commercial license or contractual agreement.
*/ 

/*
    c++                 generate C++ parser class
    8bit                don't fail on 8-bit input characters
    warn                warn about inconsistencies
    nodefault           don't create default echo-all rule
    noyywrap            don't use yywrap() function
    yyclass             define name of the scanner class
*/
%option c++
%option 8bit warn nodefault
%option noyywrap
%option yyclass="step::scanner"

%top{
// This file is part of Open CASCADE Technology software library.
// This file is generated, do not modify it directly; edit source file step.lex instead.

// Pre-include stdlib.h to avoid redefinition of integer type macros (INT8_MIN and similar in generated code)
#if !defined(_MSC_VER) || (_MSC_VER >= 1600) // Visual Studio 2010+
#include "stdint.h"
#endif
}

%{
#include "step.tab.hxx"
#include "recfile.ph"
#include "stdio.h"

// Tell flex which function to define
#ifdef  YY_DECL
# undef YY_DECL
#endif
#define YY_DECL int step::scanner::lex (step::parser::semantic_type* /*yylval*/, step::parser::location_type* /*yylloc*/)

typedef step::parser::token token;

/* skl 31.01.2002 for OCC133(OCC96,97) - uncorrect
long string in files Henri.stp and 401.stp*/
#include <Standard_Failure.hxx>
#define YY_FATAL_ERROR(msg) Standard_Failure::Raise(msg);

/* abv 07.06.02: force inclusion of stdlib.h on WNT to avoid warnings */
#ifdef _MSC_VER
// add includes for flex 2.91 (Linux version)
#include <stdlib.h>
#include <io.h>

// Avoid includion of unistd.h if parser is generated on Linux (flex 2.5.35)
#ifndef YY_NO_UNISTD_H
#define YY_NO_UNISTD_H
#endif

// disable MSVC warnings in flex 2.89 and 2.5.35 code
// Note that Intel compiler also defines _MSC_VER but has different warning ids
#if defined(__INTEL_COMPILER)
#pragma warning(disable:177 1786 1736)
#elif defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Winconsistent-dllimport"
#pragma GCC diagnostic ignored "-Wunneeded-internal-declaration"
#else
#pragma warning(disable:4131 4244 4273 4127 4267)
#endif

#endif /* MSC_VER */

void rec_restext(const char *constnewtext, int lentext);
void rec_typarg(int argtype);

// disable GCC warnings in flex code
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wunused-function"
#endif
%}
%x Com End Text
%%
"/*"               { BEGIN(Com); }     /* start of comment - put the scanner in the "Com" state */
<Com>[^*\n]*       {;}                 /* in comment, skip any characters except asterisk (and newline, handled by its own rule) */
<Com>[*]+[^*/\n]*  {;}                 /* in comment, skip any sequence of asterisks followed by other symbols (except slash or newline) */
<Com>[*]+[/]       { BEGIN(INITIAL); } /* end of comment - reset the scanner to initial state */

[']                { BEGIN(Text); yymore(); }   /* start of quoted text string - put the scanner in the "Text" state, but keep ' as part of yytext */
<Text>[\n]         { yymore(); yylineno ++; }   /* newline in text string - increment line counter and keep collecting yytext */
<Text>[']          { yymore(); }                /* single ' inside text string - keep collecting yytext*/
<Text>[^\n']+      { yymore(); }                /* a sequence of any characters except ' and \n - keep collecting yytext */
<Text>[']/[" "\n\r]*[\)\,]    { BEGIN(INITIAL); rec_restext(YYText(),YYLeng()); rec_typarg(rec_argText); return(token::QUID); } /* end of string (apostrophe followed by comma or closing parenthesis) - reset the scanner to initial state, record the value of all yytext collected */

"	"	{;}
" "		{;}
<*>[\n]		{ yylineno ++; } /* count lines (one rule for all start conditions) */
[\r]    	{;} /* abv 30.06.00: for reading DOS files */
[\0]+		{;} /* fix from C21. for test load e3i file with line 15 with null symbols */

#[0-9]+/=		{ rec_restext(YYText(),YYLeng()); return(token::ENTITY); }
#[0-9]+/[ 	]*=	{ rec_restext(YYText(),YYLeng()); return(token::ENTITY); }
#[0-9]+		{ rec_restext(YYText(),YYLeng()); return(token::IDENT); }
[-+0-9][0-9]*	{ rec_restext(YYText(),YYLeng()); rec_typarg(rec_argInteger); return(token::QUID); }
[-+\.0-9][\.0-9]+	{ rec_restext(YYText(),YYLeng()); rec_typarg(rec_argFloat); return(token::QUID); }
[-+\.0-9][\.0-9]+E[-+0-9][0-9]*	{ rec_restext(YYText(),YYLeng()); rec_typarg(rec_argFloat); return(token::QUID); }
["][0-9A-F]+["] 	{ rec_restext(YYText(),YYLeng()); rec_typarg(rec_argHexa); return(token::QUID); }
[.][A-Z0-9_]+[.]	{ rec_restext(YYText(),YYLeng()); rec_typarg(rec_argEnum); return(token::QUID); }
[(]		{ return ('('); }
[)]		{ return (')'); }
[,]		{ return (','); }
[$]		{ rec_restext(YYText(),YYLeng()); rec_typarg(rec_argNondef); return(token::QUID); }
[=]		{ return ('='); }
[;]		{ return (';'); }

STEP;		{ return(token::STEP); }
HEADER;		{ return(token::HEADER); }
ENDSEC;		{ return(token::ENDSEC); }
DATA;		{ return(token::DATA); }
ENDSTEP;	{ return(token::ENDSTEP);}
"ENDSTEP;".*	 { return(token::ENDSTEP);}
END-ISO[0-9\-]*; { BEGIN(End); return(token::ENDSTEP); } /* at the end of the STEP data, enter dedicated start condition "End" to skip everything that follows */
ISO[0-9\-]*;	 { return(token::STEP); }

[/]		{ return ('/'); }
&SCOPE		{ return(token::SCOPE); }
ENDSCOPE	{ return(token::ENDSCOPE); }
[a-zA-Z0-9_]+	{ rec_restext(YYText(),YYLeng()); return(token::TYPE); }
![a-zA-Z0-9_]+	{ rec_restext(YYText(),YYLeng()); return(token::TYPE); }
[^)]		{ rec_restext(YYText(),YYLeng()); rec_typarg(rec_argMisc); return(token::QUID); }

<End>[^\n]      {;} /* skip any characters (except newlines) */

%%

step::scanner::scanner(std::istream* in, std::ostream* out)
    : stepFlexLexer(in, out)
{
}
