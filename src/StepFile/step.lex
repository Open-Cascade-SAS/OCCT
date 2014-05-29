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

// MSVC specifics
#ifdef _MSC_VER

// disable MSVC warnings in flex code
// Note that Intel compiler also defines _MSC_VER but has different warning ids
#if defined(__INTEL_COMPILER)
#pragma warning(disable:177 1786 1736)
#else
#pragma warning(disable:4131 4244 4273 4267 4127)
#endif

// Avoid includion of unistd.h if parser is generated on Linux (flex 2.5.35)
#define YY_NO_UNISTD_H

#endif

// disable GCC warnings in flex code
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

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
