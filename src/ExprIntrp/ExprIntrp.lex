/*

 Copyright (c) 1997-1999 Matra Datavision
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

%option yywrap
%{
#include <ExprIntrp.tab.h>

#include <ExprIntrp_yaccintrf.hxx>

#define YY_SKIP_YYWRAP

static YY_BUFFER_STATE ExprIntrp_bufstring;

void ExprIntrp_start_string(const char* str)
{
  // depending on configuration and generator, yyconst may be defined as const or empty
  ExprIntrp_bufstring = ExprIntrp_scan_string((yyconst char*)str);
}

void ExprIntrp_stop_string()
{
  ExprIntrp_delete_buffer(ExprIntrp_bufstring);
  ExprIntrp_bufstring = (YY_BUFFER_STATE) 0;
}

static int yywrap()
{
  return 1;
}

// provide safe error handler (exception instead of exit())
#define YY_FATAL_ERROR(msg) ExprIntrperror(msg)

#ifdef _MSC_VER
// add includes for flex 2.91 (Linux version)
#include <stdlib.h>
#include <io.h>

// Avoid includion of unistd.h if parser is generated on Linux (flex 2.5.35)
#define YY_NO_UNISTD_H

// disable MSVC warnings in flex 2.89 and 2.5.35 code
// Note that Intel compiler also defines _MSC_VER but has different warning ids
#if defined(__INTEL_COMPILER)
#pragma warning(disable:177 1786 1736)
#else
#pragma warning(disable:4131 4244 4273 4127 4267)
#endif

#endif

#ifdef __GNUC__
// add includes for flex 2.91 (Linux version)
#include <unistd.h>

// disable GCC warnings in flex 2.91 code
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

%}
%%
" "		{;}
"+"		{return(SUMOP) ;}
"-"		{return(MINUSOP) ;}
"/"		{return(DIVIDEOP) ;}
"^"		{return(EXPOP) ;}
"**"		{return(EXPOP) ;}
"*"		{return(MULTOP) ;}
"("		{return(PARENTHESIS);}
"["		{return(BRACKET);}
")"		{return(ENDPARENTHESIS);}
"]"		{return(ENDBRACKET);}
","		{return(COMMA);}
"@"		{return(DIFFERENTIAL);}
"<-"		{return(ASSIGNOP);}
"="		{return(EQUALOP);}
"Deassign"      {return(DEASSIGNKEY);}
"Deriv"         {return(DERIVKEY);}
"Const"         {return(CONSTKEY);}
"Sum"           {return(SUMKEY);}
"Prod"          {return(PRODKEY);}
[0-9.]+e[+|-]?[0-9]+ {ExprIntrp_SetResult(); return(VALUE);}
[0-9.]+		{ExprIntrp_SetResult(); return(VALUE);}
[a-zA-Z][a-zA-Z0-9_]*	{ExprIntrp_SetResult(); return(IDENTIFIER);}
";"		{return(RELSEPARATOR);}
"'"+            {ExprIntrp_SetDegree();return(DERIVATE);}
