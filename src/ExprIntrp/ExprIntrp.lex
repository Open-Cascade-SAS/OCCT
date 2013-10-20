/*

 Copyright (c) 1997-1999 Matra Datavision
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

%option yywrap
%{
#include <ExprIntrp.tab.h>

#define YY_SKIP_YYWRAP

static YY_BUFFER_STATE ExprIntrp_bufstring;

void ExprIntrp_SetResult();
void ExprIntrp_SetDegree();

int ExprIntrlex (void);

void ExprIntrp_start_string(char* str)
{
  ExprIntrp_bufstring = ExprIntrp_scan_string(str);
}

void ExprIntrp_stop_string()
{
  ExprIntrp_delete_buffer(ExprIntrp_bufstring);
  ExprIntrp_bufstring = (YY_BUFFER_STATE) 0;
}

int yywrap()
{
  return 1;
}

#ifdef _MSC_VER
// add includes for flex 2.91 (Linux version)
#include <stdlib.h>
#include <io.h>

// disable MSVC warnings in flex 2.89 code
#pragma warning(disable:4131 4244 4273 4127)
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
