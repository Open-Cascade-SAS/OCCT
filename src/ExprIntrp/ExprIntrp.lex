%option yywrap
%{
#include <ExprIntrp.tab.h>
#define YY_SKIP_YYWRAP

static YY_BUFFER_STATE ExprIntrp_bufstring;

void ExprIntrp_SetResult();
void ExprIntrp_SetDegree();

void ExprIntrp_start_string(char* str)
{
  ExprIntrp_bufstring = ExprIntrp_scan_string(str);
}

void ExprIntrp_stop_string()
{
  ExprIntrp_delete_buffer(ExprIntrp_bufstring);
}

int yywrap()
{
  return 1;
}

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
