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

%token STEP HEADER ENDSEC DATA ENDSTEP SCOPE ENDSCOPE ENTITY TYPE INTEGER FLOAT IDENT TEXT NONDEF ENUM HEXA QUID
%start stepf
%{
#include "recfile.ph"		/* definitions des types d'arguments */
#include "recfile.pc"		/* la-dedans, tout y est */
/*
#define stepparse STEPparse
#define steplex STEPlex
#define stepwrap STEPwrap
#define steprestart STEPrestart
#define steplex STEPlex
#define steplval STEPlval
#define stepval STEPval
#define stepchar STEPchar
#define stepdebug STEPdebug
#define stepnerrs STEPnerrs
#define steperror STEPerror
*/
#define stepclearin yychar = -1
#define steperrok yyerrflag = 0

/*
#define stepin STEPin
#define yyerrflag STEPerrflag
#define yyerrstatus STEPerrflag
*/

/* ABV 19.12.00: merging porting modifications by POP (for WNT, AIX) */
#if defined(WNT) && !defined(MSDOS)
#define MSDOS WNT
#endif
#if defined(_AIX)
#include <malloc.h>
#define alloca malloc
#endif


// disable MSVC warnings in bison code
#ifdef _MSC_VER
#pragma warning(disable:4244 4131 4127 4702)
#define YYMALLOC malloc
#define YYFREE free
#endif

%}
%%
/*  N.B. : les commentaires sont filtres par LEX  */
/*  La fin vide (selon systeme emetteur) est filtree ici  */
finvide	: ' '
	| finvide ' ' ;
finstep	: ENDSTEP
	| ENDSTEP finvide ;
stepf1	: STEP HEADER headl ENDSEC endhead model ENDSEC finstep ;
stepf2	: STEP HEADER ENDSEC endhead model ENDSEC ENDSTEP ;
stepf3	: STEP HEADER ENDSEC endhead model error ;
stepf	: stepf1 | stepf2 | stepf3
		{  rec_finfile();  return(0);  /*  fini pour celui-la  */  }
	;
headl	: headent
	| headl headent
	;
headent : enttype listarg ';'
	| error  			/*  Erreur sur Entite : la sauter  */
	;
endhead : DATA
	{  rec_finhead();  }
	;
unarg	: IDENT		{  rec_typarg(rec_argIdent);     rec_newarg();  }
	| QUID		{  /* deja fait par lex*/ 	 rec_newarg();  }
	| listarg	/*  rec_newent lors du ')' */ {  rec_newarg();  }
	| listype listarg  /*  liste typee  */        {  rec_newarg();  }
	| error		{  rec_typarg(rec_argMisc);      rec_newarg();
			   yyerrstatus = 1; yyclearin;  }
/*  Erreur sur Parametre : tacher de le noter sans jeter l'Entite  */
	;
listype	: TYPE
	{  rec_listype();  }
	;
deblist	: '('
	{  rec_deblist();  }
	;
finlist	: ')'
	{  if (modeprint > 0)
		{  printf("Record no : %d -- ",nbrec+1);  rec_print(currec);  }
	   rec_newent ();  yyerrstatus = 0; }
	;
listarg	: deblist finlist		/* liste vide (peut y en avoir) */
	| deblist arglist finlist	/* liste normale, non vide */
	| deblist error
	;
arglist	: unarg
	| arglist ',' unarg
	| arglist error
	;
model	: bloc
	| model bloc
	;
bloc	: entlab '=' unent ';'
	| entlab '=' debscop model finscop unent ';'
	| entlab '=' debscop finscop unent ';'
	| error  			/*  Erreur sur Entite : la sauter  */
	;
plex	: enttype listarg
	| plex enttype listarg          /*    sert a ce qui suit :     */
	;
unent   : enttype listarg               /*    Entite de Type Simple    */
	| '(' plex ')'                  /*    Entite de Type Complexe  */
	;
debscop	: SCOPE
	{  scope_debut();  }
	;
unid	: IDENT
	{  rec_typarg(rec_argIdent);    rec_newarg();  }
	;
export	: unid
	| export ',' unid
	;
debexp	: '/'
	{  rec_deblist();  }
	;
finscop	: ENDSCOPE
	{  scope_fin();  }
	| ENDSCOPE debexp export '/'
	{  printf("***  Warning : Export List not yet processed\n");
	   rec_newent();  scope_fin() ; }
		/*  La liste Export est prise comme ARGUMENT du EndScope  */
	;
entlab	: ENTITY
	{  rec_ident();  }
	;
enttype	: TYPE
	{  rec_type ();  }
	;
