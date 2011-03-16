%{
#include "step.tab.h"
#include "recfile.ph"
#include "stdio.h"
#include <StepFile_CallFailure.hxx>

/* skl 31.01.2002 for OCC133(OCC96,97) - uncorrect
long string in files Henri.stp and 401.stp*/
#define YY_FATAL_ERROR(msg) StepFile_CallFailure( msg )

/* abv 07.06.02: force inclusion of stdlib.h on WNT to avoid warnings */
#include <stdlib.h>

/*
void steperror ( FILE *input_file );
void steprestart ( FILE *input_file );
*/
void rec_restext(char *newtext, int lentext);
void rec_typarg(int argtype);
 
  int  steplineno;      /* Comptage de ligne (ben oui, fait tout faire)  */

  int  modcom = 0;      /* Commentaires type C */
  int  modend = 0;      /* Flag for finishing of the STEP file */
  void resultat ()           /* Resultat alloue dynamiquement, "jete" une fois lu */
      { if (modcom == 0) rec_restext(yytext,yyleng); }

%}
%%
"	"	{;}
" "		{;}
[\n]		{ steplineno ++; }
[\r]            {;} /* abv 30.06.00: for reading DOS files */
[\0]+		{;} /* fix from C21. for test load e3i file with line 15 with null symbols */

#[0-9]+/=		{ resultat();  if (modcom == 0) return(ENTITY); }
#[0-9]+/[ 	]*=	{ resultat();  if (modcom == 0) return(ENTITY); }
#[0-9]+		{ resultat();  if (modcom == 0) return(IDENT); }
[-+0-9][0-9]*	{ resultat();  if (modcom == 0) { rec_typarg(rec_argInteger); return(QUID); } }
[-+\.0-9][\.0-9]+	{ resultat();  if (modcom == 0) { rec_typarg(rec_argFloat); return(QUID); } }
[-+\.0-9][\.0-9]+E[-+0-9][0-9]*	{ resultat(); if (modcom == 0) { rec_typarg(rec_argFloat); return(QUID); } }
[\']([\n]|[\000\011-\046\050-\176\201-\237\240-\777]|[\047][\047])*[\']	{ resultat(); if (modcom == 0) { rec_typarg(rec_argText); return(QUID); } }
["][0-9A-F]+["] 	{ resultat();  if (modcom == 0) { rec_typarg(rec_argHexa); return(QUID); } }
[.][A-Z0-9_]+[.]	{ resultat();  if (modcom == 0) { rec_typarg(rec_argEnum); return(QUID); } }
[(]		{ if (modcom == 0) return ('('); }
[)]		{ if (modcom == 0) return (')'); }
[,]		{ if (modcom == 0) return (','); }
[$]		{ resultat();  if (modcom == 0) { rec_typarg(rec_argNondef); return(QUID); } }
[=]		{ if (modcom == 0) return ('='); }
[;]		{ if (modcom == 0) return (';'); }
"/*"		{ modcom = 1;  }
"*/"		{ if (modend == 0) modcom = 0;  }

STEP;		{ if (modcom == 0) return(STEP); }
HEADER;		{ if (modcom == 0) return(HEADER); }
ENDSEC;		{ if (modcom == 0) return(ENDSEC); }
DATA;		{ if (modcom == 0) return(DATA); }
ENDSTEP;	{ if (modend == 0) {modcom = 0;  return(ENDSTEP);} }
"ENDSTEP;".*	{ if (modend == 0) {modcom = 0;  return(ENDSTEP);} }
END-ISO[0-9\-]*; { modcom = 1; modend = 1; return(ENDSTEP); }
ISO[0-9\-]*;	{ if (modend == 0) {modcom = 0;  return(STEP); } }

[/]		{ if (modcom == 0) return ('/'); }
&SCOPE		{ if (modcom == 0) return(SCOPE); }
ENDSCOPE	{ if (modcom == 0) return(ENDSCOPE); }
[a-zA-Z0-9_]+	{ resultat();  if (modcom == 0) return(TYPE); }
![a-zA-Z0-9_]+	{ resultat();  if (modcom == 0) return(TYPE); }
[^)]		{ resultat();  if (modcom == 0) { rec_typarg(rec_argMisc); return(QUID); } }
