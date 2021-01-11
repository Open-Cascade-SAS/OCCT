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

%define parse.error verbose

%token STEP HEADER ENDSEC DATA ENDSTEP SCOPE ENDSCOPE ENTITY TYPE INTEGER FLOAT IDENT TEXT NONDEF ENUM HEXA QUID
%start stepf

%code requires {
// This file is part of Open CASCADE Technology software library.
// This file is generated, do not modify it directly; edit source file step.yacc instead.

#include <StepFile_ReadData.hxx>
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
#undef yylex
#define yylex scanner->lex
#define StepData scanner->myDataModel

// disable MSVC warnings in bison code
#ifdef _MSC_VER
#pragma warning(disable:4065 4244 4131 4127 4702)
#define YYMALLOC malloc
#define YYFREE free
#endif
void StepFile_Interrupt (Standard_CString theErrorMessage, const Standard_Boolean theIsFail);
}

%code provides {
// Define stepFlexLexer class by inclusion of FlexLexer.h,
// but only if this has not been done yet, to avoid redefinition
#if !defined(yyFlexLexer) && !defined(FlexLexerOnce)
#define yyFlexLexer stepFlexLexer
#include "FlexLexer.h"
#endif

namespace step {

    // To feed data back to bison, the yylex method needs yylval and
    // yylloc parameters. Since the stepFlexLexer class is defined in the
    // system header <FlexLexer.h> the signature of its yylex() method
    // can not be changed anymore. This makes it necessary to derive a
    // scanner class that provides a method with the desired signature:

    class scanner : public stepFlexLexer
    {
    public:
      explicit scanner(StepFile_ReadData* theDataModel, std::istream* in = 0, std::ostream* out = 0);

      int lex(step::parser::semantic_type* yylval);

      StepFile_ReadData* myDataModel;
    };

};
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
		{  return(0);  /*  fini pour celui-la  */  }
	;
headl	: headent
	| headl headent
	;
headent : enttype listarg ';'
	| error  			/*  Erreur sur Entite : la sauter  */
	;
endhead : DATA
	{  StepData->FinalOfHead();  }
	;
unarg	: IDENT		{  StepData->SetTypeArg(Interface_ParamIdent);     StepData->CreateNewArg();  }
	| QUID		{  /* deja fait par lex*/ 	 StepData->CreateNewArg();  }
	| listarg	/*  rec_newent lors du ')' */ {  StepData->CreateNewArg();  }
	| listype listarg  /*  liste typee  */        {  StepData->CreateNewArg();  }
	| error		{  StepData->CreateErrorArg();  }
/*  Erreur sur Parametre : tacher de le noter sans jeter l'Entite  */
	;
listype	: TYPE
	{  StepData->RecordTypeText();  }
	;
deblist	: '('
	{  StepData->RecordListStart();  }
	;
finlist	: ')'
	{  if (StepData->GetModePrint() > 0)
		{  printf("Record no : %d -- ", StepData->GetNbRecord()+1);  StepData->PrintCurrentRecord();  }
	   StepData->RecordNewEntity ();  yyerrstatus_ = 0; }
	;
listarg	: deblist finlist		/* liste vide (peut y en avoir) */
	| deblist arglist finlist	/* liste normale, non vide */
	;
arglist	: unarg
	| arglist ',' unarg
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
	{  StepData->AddNewScope();  }
	;
unid	: IDENT
	{  StepData->SetTypeArg(Interface_ParamIdent);    StepData->CreateNewArg();  }
	;
export	: unid
	| export ',' unid
	;
debexp	: '/'
	{  StepData->RecordListStart();  }
	;
finscop	: ENDSCOPE
	{  StepData->FinalOfScope();  }
	| ENDSCOPE debexp export '/'
	{  printf("***  Warning : Export List not yet processed\n");
	   StepData->RecordNewEntity();  StepData->FinalOfScope() ; }
		/*  La liste Export est prise comme ARGUMENT du EndScope  */
	;
entlab	: ENTITY
	{  StepData->RecordIdent();  }
	;
enttype	: TYPE
	{  StepData->RecordType ();  }
	;
%%
void step::parser::error(const std::string& m)
{
  char newmess[120];
  Standard_Boolean isSyntax = (Standard_Boolean)strncmp(m.c_str(), "syntax error", 13);
  if (isSyntax && strlen(m.c_str()) > 13)
    sprintf(newmess, "Undefined Parsing: Line %d: %s: %s", scanner->lineno() + 1, "Incorrect syntax", m.c_str() + 14);
  else if (isSyntax)
    sprintf(newmess, "Undefined Parsing: Line %d: Incorrect syntax", scanner->lineno() + 1);
  else
    sprintf(newmess, "Undefined Parsing: Line %d: %s", scanner->lineno() + 1, m.c_str());

  StepFile_Interrupt(newmess, Standard_False);

  StepData->AddError(newmess);
}
