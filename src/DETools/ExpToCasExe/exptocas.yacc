/* 
 Copyright (c) 1999-2022 OPEN CASCADE SAS

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
  
  This YACC parser implements parsing algorithm for EXPRESS -> CASCADE/XSTEP
  classes generator
  
  Input in the form of tokens is obtained from lexical analyser. Then, data 
  structure representing schema is created on the basis of grammar analysis.
  
  NOTE: The grammar currently implemented is not full. 
  FUNCTION, RULE and CONSTANT items, WHERE, INVERSE and DERIVE clauses 
  of TYPE and ENTITY items are not considered (ignored on the level of lexical 
  scanner). 
  SUPERTYPE and UNIQUE clauses of ENTITY item are recognized but ignored.
  Also, complex constructs such as using call to function in dimensions of 
  complex time or redefinition of inherited field are ignored.
  
  *****************************************************************************/
}

%language "C++"
%require "3.2"
%parse-param  {exptocas::scanner* scanner}

%code requires {
#include <OSD_OpenFile.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <Express_HSequenceOfField.hxx>
#include <Express_HSequenceOfItem.hxx>
#include <Express_Field.hxx>
#include <Express_Item.hxx>
#include <Express_Enum.hxx>
#include <Express_Alias.hxx>
#include <Express_Select.hxx>
#include <Express_Entity.hxx>
#include <Express_Type.hxx>
#include <Express_NamedType.hxx>
#include <Express_PredefinedType.hxx>
#include <Express_Number.hxx>
#include <Express_Integer.hxx>
#include <Express_Boolean.hxx>
#include <Express_Logical.hxx>
#include <Express_Real.hxx>
#include <Express_String.hxx>
#include <Express_ComplexType.hxx>
#include <Express_Array.hxx>
#include <Express_List.hxx>
#include <Express_Set.hxx>
#include <Express_Bag.hxx>
#include <Express_Schema.hxx>
#include <Express_Reference.hxx>
#include <Express.hxx>
  
namespace exptocas {
  class scanner;
};
#ifdef _MSC_VER
// disable MSVC warning C4522: 'exptocas::parser::stack_symbol_type': multiple assignment operators
#pragma warning(disable: 4522)
#endif
}

%code {
#undef yylex
#define yylex scanner->lex
// disable MSVC warnings in bison code
#ifdef _MSC_VER
#pragma warning(disable:4244 4800)
#endif

/************************************************/
/* ERROR MESSAGE FUNCTION                       */

/* external functions (from exptocas.l) */
int ec_error ( const std::string& s, const std::string& text );
int ec_curline ( void );

//int yyerror ( char *s )
//{
//  printf ( "\nParse error at line %d: %s\n", ec_curline(), s );
//  return 0;
//}

/************************************************/
/* FUNCTIONS FOR CREATING SCHEMA REPRESENTATION */

static Express_Schema *mkschema ( char *name, Express_HSequenceOfItem *ilist );
static Express_HSequenceOfItem *mkilist ( Express_Item *item, Express_HSequenceOfItem *seq );
static Express_Item *mkenum ( char *name, TColStd_HSequenceOfHAsciiString *tlist );
static Express_Item *mkselect ( char *name, TColStd_HSequenceOfHAsciiString *tlist );
static Express_Item *mkalias ( char *name, Express_Type *type );
static Express_Item *mkentity ( char *name, TColStd_HSequenceOfHAsciiString *inherit,
                                Express_HSequenceOfField *field, int isabstract );
static Express_Reference *mkrefs ( char *name, TColStd_HSequenceOfHAsciiString *items);
static TColStd_HSequenceOfHAsciiString *mktlist ( char *name, TColStd_HSequenceOfHAsciiString *tlist );
static TColStd_HSequenceOfHAsciiString *mktlists ( TColStd_HSequenceOfHAsciiString *tlist1, TColStd_HSequenceOfHAsciiString *tlist2 );
static Express_Type *mktstd ( int keyword );
static Express_Type *mktname ( char *name );
static Express_Type *mktset ( int keyword, int ilow, int ihigh, Express_Type *of );
static Express_Field *mkfield ( char *name, Express_Type *type, int optional );
static Express_HSequenceOfField *mkflist ( Express_Field *field, Express_HSequenceOfField *seq );
}

%code provides {
#if !defined(yyFlexLexer) && !defined(FlexLexerOnce)
#define yyFlexLexer exptocasFlexLexer
#include "FlexLexer.h"
#endif
namespace exptocas {
    // To feed data back to bison, the yylex method needs yylval and
    // yylloc parameters. Since the exptocasFlexLexer class is defined in the
    // system header <FlexLexer.h> the signature of its yylex() method
    // can not be changed anymore. This makes it necessary to derive a
    // scanner class that provides a method with the desired signature:
    class scanner : public exptocasFlexLexer
    {
    public:
      explicit scanner(std::istream* in = 0, std::ostream* out = 0);

      int lex(exptocas::parser::semantic_type* yylval);
    };
};
}

/* Definition of possible types of expressions */
%union {
  int num;
  char *str;
  TColStd_HSequenceOfHAsciiString *tlist;
  Express_HSequenceOfField *flist;
  Express_HSequenceOfItem *ilist;
  Express_Field *field;
  Express_Item *item;
  Express_Type *type;
  Express_Schema *schema;
  Express_Reference *ref;
}

/* Definition of keywords */

%token <num> KSCHEM  /* SCHEMA keyword */
%token <num> KENDS   /* END_SCHEMA keyword */
%token <num> KTYP    /* TYPE keyword */
%token <num> KENDT   /* END_TYPE keyword */
%token <num> KENT    /* ENTITY keyword */
%token <num> KENDE   /* END_ENTITY keyword */
%token <num> KREF    /* REFERENCE keyword */
%token <num> KFROM   /* FROM keyword */

%token <num> KSEL    /* SELECT keyword */
%token <num> KENUM   /* ENUMERATION keyword */
%token <num> KLIST   /* LIST keyword */
%token <num> KARR    /* ARRAY keyword */
%token <num> KBAG    /* BAG keyword */
%token <num> KSET    /* SET keyword */
%token <num> KOF     /* OF keyword */

%token <num> KNUM    /* NUMBER keyword */
%token <num> KINT    /* INTEGER keyword */
%token <num> KDBL    /* REAL keyword */
%token <num> KSTR    /* STRING keyword */
%token <num> KLOG    /* LOGICAL keyword */
%token <num> KBOOL   /* BOOLEAN keyword */

%token <num> KOPT    /* OPTIONAL keyword */
%token <num> KUNIQ   /* UNIQUE keyword */
%token <num> KSELF   /* SELF keyword */

%token <num> KABSTR  /* ABSTRACT keyword */
%token <num> KSUBT   /* SUBTYPE keyword */
%token <num> KSPRT   /* SUPERTYPE keyword */
%left  <num> KANDOR  /* ANDOR keyword (%left is for eliminating shift/reduce conflict on SUPLST) */
%left  <num> K1OF    /* ONEOF keyword */
%token <num> KAND    /* AND keyword */

%token <num> NUMBER  /* integer value */
%token <str> NAME    /* name of type or entity */

%left ','            /* to eliminate shift/reduce conflict in SUPERTYPE */

  /* Definition of expressions and their types */

%type <num>    INDEX OPTNL OPTUNI SUPERT SUPLST REDEF SPCLST
%type <tlist>  TLIST TLIST1 UNIQIT UNIQLS UNIQUE SUBT SPECIF
%type <type>   TYPE TSTD TNAME TSET 
%type <item>   ENUM SELECT ALIAS ENTITY ITEM
%type <ilist>  ILIST
%type <field>  FIELD
%type <flist>  FLIST FLIST1
%type <schema> SCHEMA
%type <ref>    REFERENCE

%%

  /************************************************/
  /*  Definition of parsing rules (expressions)   */
  /************************************************/

SCHEMA: KSCHEM NAME ';' ILIST KENDS ';' { $$ = mkschema ( $2, $4 ); /* Root: EXPRESS schema */ }
      ;
ILIST : ITEM             { $$ = mkilist ( $1, 0 ); /* list of items for schema */ }
      | ITEM ILIST       { $$ = mkilist ( $1, $2 ); }
      ;

ITEM  : ENUM
      | SELECT
      | ALIAS
      | REFERENCE
      | ENTITY           { $$ = $1; /* item of schema (type definition) */ }
	  ;
ENUM  : KTYP NAME '=' KENUM KOF TLIST1 ';' KENDT ';' { $$ = mkenum ( $2, $6 ); /* TYPE ENUMERATION definition */ }
      ;
SELECT: KTYP NAME '=' KSEL TLIST1 ';' KENDT ';' { $$ = mkselect ( $2, $5 ); /* TYPE SELECT definition */ }
      ;
ALIAS : KTYP NAME '=' TYPE ';' KENDT ';' { $$ = mkalias ( $2, $4 ); /* TYPE '=' definition (alias) */ }
      ;
ENTITY: KENT NAME        SUPERT SUBT ';' FLIST1 UNIQUE KENDE ';' { $$ = mkentity ( $2, $4, $6, 0 ); /* ENTITY definition */ }
      | KENT NAME KABSTR SUPERT SUBT ';' FLIST1 UNIQUE KENDE ';' { $$ = mkentity ( $2, $5, $7, 1 ); /* ENTITY definition */ }
      ;

REFERENCE: KREF KFROM NAME TLIST1 ';' { $$ = mkrefs ( $3, $4 ); /* REFERENCE FROM definition */ }
      ;

TLIST : NAME             { $$ = mktlist ( $1, 0 ); /* list of (type) names */ }
      | NAME ',' TLIST   { $$ = mktlist ( $1, $3 ); }
      ;
TLIST1: '(' TLIST ')'    { $$ = $2; /* TLIST in brackets */ }
      ;

TYPE  : TSTD
      | TNAME
      | TSET             { $$ = $1; /* type, simple or complex */ }
      ;
TSTD  : KINT             { $$ = mktstd ( $1 ); /* predefined type: INTEGER */ }
      | KNUM             { $$ = mktstd ( $1 ); /* predefined type: NUMBER */ }
      | KDBL             { $$ = mktstd ( $1 ); /* predefined type: REAL */ }
      | KSTR             { $$ = mktstd ( $1 ); /* predefined type: STRING */ }
      | KLOG             { $$ = mktstd ( $1 ); /* predefined type: LOGICAL */ }
      | KBOOL            { $$ = mktstd ( $1 ); /* predefined type: BOOLEAN */ }
      ;                  
TNAME : NAME             { $$ = mktname ( $1 ); /* named type */ }
      ;
TSET  : KLIST '[' INDEX ':' INDEX ']' KOF OPTUNI TYPE { $$ = mktset ( $1, $3, $5, $9 ); /* complex type: LIST */ }
      | KARR  '[' INDEX ':' INDEX ']' KOF OPTUNI TYPE { $$ = mktset ( $1, $3, $5, $9 ); /* complex type: ARRAY */ }
      | KSET  '[' INDEX ':' INDEX ']' KOF OPTUNI TYPE { $$ = mktset ( $1, $3, $5, $9 ); /* complex type: SET */ }
      | KBAG  '[' INDEX ':' INDEX ']' KOF OPTUNI TYPE { $$ = mktset ( $1, $3, $5, $9 ); /* complex type: BAG */ }
      ;
INDEX : NUMBER           { $$ = $1; /* index for array, set, bag, list range */ }
      | '?'              { $$ = -1; /* undefined */ }
      | NAME '(' NAME ')' {$$ = -1; printf ( "Warning at line %d: index function %s(%s) ignored\n", ec_curline(), $1, $3 ); /* some function.. */ }
      ;
OPTUNI: /* empty */      { $$ = 0; /* UNIQUE keyword for complex type definition */ }
      | KUNIQ            { $$ = 1; }
      ;

SUBT  : /* empty */      { $$ = NULL; /* no subtype clause */ }
      | KSUBT KOF TLIST1 { $$ = $3;   /* subtype clause */ }
      ;
SUPERT: /* empty */      { $$ = 0;  /* no supertype clause */ }
      | KSPRT            { $$ = 0;  /* supertype clause (ignored) */ }
      | KSPRT KOF SUPLST { $$ = 0;  /* supertype clause (ignored) */ }
      ;
SUPLST: NAME             { $$ = 0; /* simple list of supertypes */ }
      | '(' SUPLST ')'   { $$ = 0; /* allow bracketing */ }
      | NAME ',' SUPLST  { $$ = 0; /* ... */ }
      | K1OF SUPLST      { $$ = 0; /* ONEOF construct */ }
      | SUPLST KANDOR SUPLST { $$ = 0; /* ANDOR construct */ }
      ;

FLIST : FIELD            { $$ = mkflist ( $1, 0 ); /* list of fields of ENTITY item */ }
      | FIELD FLIST      { $$ = mkflist ( $1, $2 ); }
      | REDEF            { $$ = 0;  /* redefinition of inherited field, just skip */ }
      | REDEF FLIST      { $$ = $2; /* ignore redefinition of inherited field, take trailing list */ }
      ;
FLIST1: /* empty */      { $$ = NULL; /* empty list of fields */ }
      | FLIST            { $$ = $1;   /* or not empty.. just to fix reduce/reduce conflict */ }
      ;
FIELD : NAME ':' OPTNL TYPE ';' { $$ = mkfield ( $1, $4, $3 ); }
      ;
REDEF : KSELF '\\' SPECIF ':' TYPE ';' { $$ = 0; printf ( "Warning at line %d: field redefinition ignored\n", ec_curline() ); /* redefinition of inherited field */ }
      ;
SPECIF: NAME             { $$ = mktlist ( $1, 0 ); /* inherited field specification */ } 
      | NAME '.' SPECIF  { $$ = mktlist ( $1, $3 ); }
      ;

OPTNL : /* empty */      { $$ = 0; }
      | KOPT             { $$ = 1; }
      ;

UNIQIT: NAME ':' TLIST ';'  { $$ = $3;   /* UNIQUE statement */ }
      | NAME ':' SPCLST ';' { $$ = NULL; /* UNIQUE statement */ }
      ;
UNIQLS: UNIQIT           { $$ = NULL;    /* list of 1 UNIQUE statements */ }
      | UNIQIT UNIQLS    { $$ = mktlists ( $1, $2 );/* list of UNIQUE statements */ }
      ;
UNIQUE: /* empty */      { $$ = NULL;    /* no UNIQUE clause in ENTITY */ }
      | KUNIQ UNIQLS     { $$ = $2;      /* UNIQUE clause in ENTITY */ }
      ;
SPCLST: KSELF '\\' SPECIF      { $$ = 0; /* list of specifications */ }
      | KSELF '\\' SPECIF ',' SPCLST { $$ = 0; }
      | NAME ',' SPCLST        { $$ = 0; }
      ;

%%

/************************************************/
/* FUNCTIONS FOR CREATING SCHEMA REPRESENTATION */

static Express_Schema *mkschema ( char *name, Express_HSequenceOfItem *ilist )
{
  Express_Schema *sch = new Express_Schema ( name, ilist );
  Express::Schema() = sch;
  return sch;
}

static Express_HSequenceOfItem *mkilist ( Express_Item *item, Express_HSequenceOfItem *seq )
{
  if ( ! seq ) { 
    seq = new Express_HSequenceOfItem;
    seq->Append ( item );
  }
  else seq->Prepend ( item );
  return seq;
}

static Express_Item *mkenum ( char *name, TColStd_HSequenceOfHAsciiString *tlist )
{
  return new Express_Enum ( name, tlist );
}

static Express_Item *mkselect ( char *name, TColStd_HSequenceOfHAsciiString *tlist )
{
  return new Express_Select ( name, tlist );
}

static Express_Item *mkalias ( char *name, Express_Type *type )
{
  return new Express_Alias ( name, type );
}

static Express_Item *mkentity ( char *name, TColStd_HSequenceOfHAsciiString *inherit,
				 Express_HSequenceOfField *field, int isabstract )
{
  Express_Entity *ent = new Express_Entity ( name, inherit, field );
  if ( isabstract ) ent->SetAbstractFlag ( Standard_True );
  return ent;
}

static Express_Reference *mkrefs ( char *name, TColStd_HSequenceOfHAsciiString *items)
{
  return new Express_Reference ( name, items );
}

static TColStd_HSequenceOfHAsciiString *mktlist ( char *name, TColStd_HSequenceOfHAsciiString *tlist )
{
  Handle(TCollection_HAsciiString) str = new TCollection_HAsciiString ( name );
  if ( tlist ) tlist->Prepend ( str );
  else {
    tlist = new TColStd_HSequenceOfHAsciiString;
    tlist->Append ( str );
  }
  return tlist;
}

static TColStd_HSequenceOfHAsciiString *mktlists ( TColStd_HSequenceOfHAsciiString *tlist1, 
						   TColStd_HSequenceOfHAsciiString *tlist2 )
{
  if ( ! tlist1 ) return tlist2;
  if ( ! tlist2 ) return tlist1;
  for ( int i=1; i <= tlist2->Length(); i++ )
    tlist1->Append ( tlist2->Value(i) );
  return tlist1;
}

static Express_Type *mktstd ( int keyword )
{
  switch ( keyword ) {
  case exptocas::parser::token::KINT : return new Express_Integer;
  case exptocas::parser::token::KNUM : return new Express_Number;
  case exptocas::parser::token::KDBL : return new Express_Real;
  case exptocas::parser::token::KSTR : return new Express_String;
  case exptocas::parser::token::KBOOL: return new Express_Boolean;
  case exptocas::parser::token::KLOG : return new Express_Logical;
  default   : ec_error ( "Predefined type not treated!", "" );
    return NULL;
  }
}

static Express_Type *mktname ( char *name )
{
  return new Express_NamedType ( name );
}

static Express_Type *mktset ( int keyword, int ilow, int ihigh, Express_Type *of )
{
  switch ( keyword ) {
  case exptocas::parser::token::KLIST: return new Express_List  ( ilow, ihigh, of );
  case exptocas::parser::token::KARR : return new Express_Array ( ilow, ihigh, of );
  case exptocas::parser::token::KBAG : return new Express_Bag   ( ilow, ihigh, of );
  case exptocas::parser::token::KSET : return new Express_Set   ( ilow, ihigh, of );
  default   : ec_error ( "Complex type not treated!", "" );
    return NULL;
  }
}

static Express_Field *mkfield ( char *name, Express_Type *type, int optional )
{
  return new Express_Field ( name, type, optional );
}

static Express_HSequenceOfField *mkflist ( Express_Field *field, Express_HSequenceOfField *seq )
{
  if ( seq ) seq->Prepend ( field );
  else {
    seq = new Express_HSequenceOfField;
    seq->Append ( field );
  }
  return seq;
}

/*******************************************************************/
/* MAIN & co */

/*
void tlistfree ( struct tlist *list )
{
  if ( ! list ) return;
  tlistfree ( list->next );
  list->next = 0;
  free ( list->str );
  list->str = 0;
}
* /

int printtlist ( struct ec_tlist *tl )
{
  int num=0;
  while ( tl ) {
    num++;
    printf ( "%s\n", tl->name );
    tl = tl->next;
  }
  return num;
}

int main ( void )
{
  int num = 0;
  yyparse();

  printf ( "\nFinished\n" );
  if ( schema ) {
    struct ec_item *it;
    it = schema->items;
    printf ( "\nSchema %s", schema->name );
    printf ( "\nItems:" );
    while ( it ) {
      num++;
      printf ( "\n%s", it->name );
      it = it->next;
    }
//    num = printtlist ( res );
    printf ( "\nTotal %d", num );
  }

//  tlistfree ( yylval.tlist );
  return num;
}
*/

void exptocas::parser::error(const std::string& s)
{
  printf("\nParse error at line %d: %s\n", scanner->lineno(), s.c_str());
}

