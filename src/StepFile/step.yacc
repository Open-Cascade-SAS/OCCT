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

%code top {
// This file is part of Open CASCADE Technology software library.
// This file is generated, do not modify it directly; edit source file step.yacc instead.
}

%language "C++"
%require "3.2"

/* C++ parser interface */
%skeleton "lalr1.cc"

%parse-param  {step::scanner* scanner}

%locations

%token STEP HEADER ENDSEC DATA ENDSTEP SCOPE ENDSCOPE ENTITY TYPE INTEGER FLOAT IDENT TEXT NONDEF ENUM HEXA QUID
%start stepf

%code requires {
// This file is part of Open CASCADE Technology software library.
// This file is generated, do not modify it directly; edit source file step.yacc instead.

namespace step {
  class scanner;
};

#ifdef _MSC_VER
// disable MSVC warning C4522: 'step::parser::stack_symbol_type': multiple assignment operators
#pragma warning(disable: 4522)
// disable MSVC warning C4512: 'step::parser::stack::slice' : assignment operator could not be generated
#pragma warning(disable: 4512)
#endif

}

%code {
#include "recfile.ph"		/* definitions des types d'arguments */
#include "recfile.pc"		/* la-dedans, tout y est */

#undef yylex
#define yylex scanner->lex

#define stepclearin yychar = -1
#define steperrok yyerrflag = 0

// disable MSVC warnings in bison code
#ifdef _MSC_VER
#pragma warning(disable:4065 4244 4131 4127 4702)
#define YYMALLOC malloc
#define YYFREE free
#endif
void StepFile_Interrupt (char* nomfic); /* rln 13.09.00 port on HP*/
}

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
			   yyerrstatus_ = 1; yyclearin;  }
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
	   rec_newent ();  yyerrstatus_ = 0; }
	;
listarg	: deblist finlist		/* liste vide (peut y en avoir) */
	| deblist arglist finlist	/* liste normale, non vide */
	| deblist arglist ',' finlist	/* broken list with missing last parameter, see #31756 */
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
%%

void step::parser::error(const location_type& /*loc*/, const std::string& m)
{
  char newmess[80];
  sprintf(newmess, "At line %d : %s", scanner->lineno() + 1, m.c_str());
  StepFile_Interrupt(newmess);
}
