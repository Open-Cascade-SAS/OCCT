// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

// List of changes:
//skl 29.01.2003 - deleted one space symbol at the begining
//                 of strings from Header Section

#include <Interface_Check.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_FloatWriter.hxx>
#include <Interface_InterfaceMismatch.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ReportEntity.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_Transient.hxx>
#include <StepData_ESDescr.hxx>
#include <StepData_Field.hxx>
#include <StepData_FieldList.hxx>
#include <StepData_PDescr.hxx>
#include <StepData_Protocol.hxx>
#include <StepData_ReadWriteModule.hxx>
#include <StepData_SelectArrReal.hxx>
#include <StepData_SelectMember.hxx>
#include <StepData_StepModel.hxx>
#include <StepData_StepWriter.hxx>
#include <StepData_UndefinedEntity.hxx>
#include <StepData_WriterLib.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

#include <stdio.h>
#define StepLong 72
// StepLong : longueur maxi d une ligne de fichier Step


//  Constantes litterales (interessantes, pour les performances ET LA MEMOIRE)

static TCollection_AsciiString  textscope    (" &SCOPE");
static TCollection_AsciiString  textendscope ("        ENDSCOPE");
static TCollection_AsciiString  textcomm     ("  /*  ");
static TCollection_AsciiString  textendcomm  ("  */");
static TCollection_AsciiString  textlist     ("(");
static TCollection_AsciiString  textendlist  (")");
static TCollection_AsciiString  textendent   (");");
static TCollection_AsciiString  textparam    (",");
static TCollection_AsciiString  textundef    ("$");
static TCollection_AsciiString  textderived  ("*");
static TCollection_AsciiString  texttrue     (".T.");
static TCollection_AsciiString  textfalse    (".F.");
static TCollection_AsciiString  textunknown  (".U.");



//=======================================================================
//function : StepData_StepWriter
//purpose  : 
//=======================================================================

StepData_StepWriter::StepData_StepWriter(const Handle(StepData_StepModel)& amodel)
    : thecurr (StepLong) , thefloatw (12)
{
  themodel = amodel;  thelabmode = thetypmode = 0;
  thefile  = new TColStd_HSequenceOfHAsciiString();
  thesect  = Standard_False;  thefirst = Standard_True;
  themult  = Standard_False;  thecomm  = Standard_False;
  thelevel = theindval = 0;   theindent = Standard_False;
//  Format flottant : reporte dans le FloatWriter
}

//  ....                Controle d Envoi des Flottants                ....

//=======================================================================
//function : FloatWriter
//purpose  : 
//=======================================================================

Interface_FloatWriter& StepData_StepWriter::FloatWriter ()
{  return thefloatw;  }    // s y reporter


//=======================================================================
//function : LabelMode
//purpose  : 
//=======================================================================

Standard_Integer&  StepData_StepWriter::LabelMode ()
{  return thelabmode;  }


//=======================================================================
//function : TypeMode
//purpose  : 
//=======================================================================

Standard_Integer&  StepData_StepWriter::TypeMode  ()
{  return thetypmode;  }

//  ....                Description des Scopes (AVANT Envoi)               ....


//=======================================================================
//function : SetScope
//purpose  : 
//=======================================================================

void StepData_StepWriter::SetScope (const Standard_Integer numscope,
                                    const Standard_Integer numin)
{
  Standard_Integer nb = themodel->NbEntities();
  if (numscope <= 0 || numscope > nb || numin <= 0 || numin > nb)
    throw Interface_InterfaceMismatch("StepWriter : SetScope, out of range");
  if (thescopenext.IsNull()) {
    thescopebeg  = new TColStd_HArray1OfInteger (1,nb); thescopebeg->Init(0);
    thescopeend  = new TColStd_HArray1OfInteger (1,nb); thescopeend->Init(0);
    thescopenext = new TColStd_HArray1OfInteger (1,nb); thescopenext->Init(0);
  }
  else if (thescopenext->Value(numin) != 0) {
#ifdef OCCT_DEBUG
    std::cout << "StepWriter : SetScope (scope : " << numscope << " entity : "
      << numin << "), Entity already in a Scope"<<std::endl;
#endif
    throw Interface_InterfaceMismatch("StepWriter : SetScope, already set");
  }
  thescopenext->SetValue(numin,-1);  // nouvelle fin de scope
  if (thescopebeg->Value(numscope) == 0) thescopebeg->SetValue(numscope,numin);
  Standard_Integer lastin = thescopeend->Value(numscope);
  if (lastin > 0) thescopenext->SetValue(lastin,numin);
  thescopeend->SetValue(numscope,numin);
}


//=======================================================================
//function : IsInScope
//purpose  : 
//=======================================================================

Standard_Boolean StepData_StepWriter::IsInScope(const Standard_Integer num) const
{
  if (thescopenext.IsNull()) return Standard_False;
  return (thescopenext->Value(num) != 0);
}

//  ###########################################################################
//  ##    ##    ##    ##        ENVOI DES  SECTIONS        ##    ##    ##    ##

//  ....                      Envoi du Modele Complet                      ....


//=======================================================================
//function : SendModel
//purpose  : 
//=======================================================================

void StepData_StepWriter::SendModel(const Handle(StepData_Protocol)& protocol,
                                    const Standard_Boolean headeronly)
{
  StepData_WriterLib lib(protocol);

  if (!headeronly)
    thefile->Append (new TCollection_HAsciiString("ISO-10303-21;"));
  SendHeader();

//  ....                Header : suite d entites sans Ident                ....

  Interface_EntityIterator header = themodel->Header();
  thenum = 0;
  for (header.Start(); header.More(); header.Next()) {
    Handle(Standard_Transient) anent = header.Value();

//   Write Entity via Lib  (similaire a SendEntity)
    Handle(StepData_ReadWriteModule) module;  Standard_Integer CN;
    if (lib.Select(anent,module,CN)) {
      if (module->IsComplex(CN))   StartComplex();
      else {
	TCollection_AsciiString styp;
	if (thetypmode > 0) styp = module->ShortType(CN);
	if (styp.Length() == 0) styp = module->StepType(CN);
	StartEntity (styp);
      }
      module->WriteStep(CN,*this,anent);
      if (module->IsComplex(CN))   EndComplex();
    } else {
//    Pas trouve ci-dessus ... tenter UndefinedEntity
      DeclareAndCast(StepData_UndefinedEntity,und,anent);
      if (und.IsNull()) continue;
      if (und->IsComplex())   StartComplex();
      und->WriteParams(*this); 
      if (und->IsComplex())   EndComplex();
   }
    EndEntity ();
  }
  EndSec();
  if (headeronly) return;

//  Data : Comme Header mais avec des Idents ... sinon le code est le meme
  SendData();

// ....                    Erreurs Globales (silya)                    ....

  Handle(Interface_Check) achglob = themodel->GlobalCheck();
  Standard_Integer nbfails = achglob->NbFails();
  if (nbfails > 0) {
    Comment(Standard_True);
    SendComment("GLOBAL FAIL MESSAGES,  recorded at Read time :");
    for (Standard_Integer ifail = 1; ifail <= nbfails; ifail ++) {
      SendComment (achglob->Fail(ifail));
    }
    Comment(Standard_False);
    NewLine(Standard_False);
  }

//  ....                Sortie des Entites une par une                ....

  Standard_Integer nb = themodel->NbEntities();
  for (Standard_Integer i = 1 ; i <= nb; i ++) {
//    Liste principale : on n envoie pas les Entites dans un Scope
//    Elles le seront par l intermediaire du Scope qui les contient
    if (!thescopebeg.IsNull()) {  if (thescopenext->Value(i) != 0) continue;  }
    SendEntity (i,lib);
  }

  EndSec();
  EndFile();
}


//  ....                DECOUPAGE DU FICHIER EN SECTIONS                ....


//=======================================================================
//function : SendHeader
//purpose  : 
//=======================================================================

void StepData_StepWriter::SendHeader ()
{
  NewLine(Standard_False);
  thefile->Append (new TCollection_HAsciiString("HEADER;"));
  thesect = Standard_True;
}


//=======================================================================
//function : SendData
//purpose  : 
//=======================================================================

void StepData_StepWriter::SendData ()
{
  if (thesect) throw Interface_InterfaceMismatch("StepWriter : Data section");
  NewLine(Standard_False);
  thefile->Append (new TCollection_HAsciiString("DATA;"));
  thesect = Standard_True;
}


//=======================================================================
//function : EndSec
//purpose  : 
//=======================================================================

void StepData_StepWriter::EndSec ()
{
  thefile->Append (new TCollection_HAsciiString("ENDSEC;"));
  thesect = Standard_False;
}


//=======================================================================
//function : EndFile
//purpose  : 
//=======================================================================

void StepData_StepWriter::EndFile ()
{
  if (thesect) throw Interface_InterfaceMismatch("StepWriter : EndFile");
  NewLine(Standard_False);
  thefile->Append (new TCollection_HAsciiString("END-ISO-10303-21;"));
  thesect = Standard_False;
}

//  ....                        ENVOI D UNE ENTITE                        ....


//=======================================================================
//function : SendEntity

//purpose  : 
//=======================================================================

void StepData_StepWriter::SendEntity(const Standard_Integer num,
                                     const StepData_WriterLib& lib)
{
  char lident[20];
  Handle(Standard_Transient) anent = themodel->Entity(num);
  Standard_Integer idnum = num , idtrue = 0;

    //   themodel->Number(anent) et-ou IdentLabel(anent)
  if (thelabmode > 0) idtrue = themodel->IdentLabel(anent);
  if (thelabmode == 1) idnum = idtrue;
  if (idnum == 0) idnum = num;
  if (thelabmode < 2 || idnum == idtrue) sprintf(lident,"#%d = ",idnum); //skl 29.01.2003
  else sprintf(lident,"%d:#%d = ",idnum,idtrue); //skl 29.01.2003

//  SendIdent repris , lident vient d etre calcule
  thecurr.Clear();
  thecurr.Add (lident);
  themult = Standard_False;

//  ....        Traitement du Scope Eventuel
  if (!thescopebeg.IsNull()) {
    Standard_Integer numin = thescopebeg->Value(num);
    if (numin != 0) {
      SendScope();
      for (Standard_Integer nument = numin; numin > 0; nument = numin) {
	SendEntity(nument,lib);
	numin = thescopenext->Value(nument);
      }
      SendEndscope();
    }
  }

//  ....        Envoi de l Entite proprement dite

//   Write Entity via Lib
  thenum = num;
  Handle(StepData_ReadWriteModule) module;  Standard_Integer CN;
  if (themodel->IsRedefinedContent(num)) {
//    Entite Erreur : Ecrire le Contenu + les Erreurs en Commentaires
    Handle(Interface_ReportEntity) rep = themodel->ReportEntity(num);
    DeclareAndCast(StepData_UndefinedEntity,und,rep->Content());
    if (und.IsNull()) {
      thechecks.CCheck(num)->AddFail("Erroneous Entity, Content lost");
      StartEntity(TCollection_AsciiString("!?LOST_DATA"));
    } else {
      thechecks.CCheck(num)->AddWarning("Erroneous Entity, equivalent content");
      if (und->IsComplex())   AddString(" (",2);
      und->WriteParams(*this);
      if (und->IsComplex()) { AddString(") ",2); }  //thelevel --; }
    }
    EndEntity ();        // AVANT les Commentaires
    NewLine(Standard_False);
    Comment(Standard_True);
    if (und.IsNull()) SendComment("   ERRONEOUS ENTITY, DATA LOST");
    SendComment("On Entity above, Fail Messages recorded at Read time :");
    Handle(Interface_Check) ach = rep->Check();
    Standard_Integer nbfails = ach->NbFails();
    for (Standard_Integer ifail = 1; ifail <= nbfails; ifail ++) {
      SendComment (ach->Fail(ifail));
    }
    Comment(Standard_False);
    NewLine(Standard_False);

//    Cas normal
  }
  else if (lib.Select(anent,module,CN)) {
    if (module->IsComplex(CN))   StartComplex();
    else {
      TCollection_AsciiString styp;
      if (thetypmode > 0) styp = module->ShortType(CN);
      if (styp.Length() == 0) styp = module->StepType(CN);
      StartEntity (styp);
    }
    module->WriteStep(CN,*this,anent);
    if (module->IsComplex(CN))   EndComplex();
    EndEntity ();
  }
  else {
    //    Pas trouve ci-dessus ... tenter UndefinedEntity
    DeclareAndCast(StepData_UndefinedEntity,und,anent);
    if (und.IsNull()) return;
    if (und->IsComplex())   StartComplex();
    und->WriteParams(*this);
    if (und->IsComplex())   EndComplex();
    EndEntity ();
  }
}

//  ###########################################################################
//  ##    ##    ##        CONSTITUTION DU TEXTE A ENVOYER        ##    ##    ##

//  Passer a la ligne. Ligne vide pas comptee sauf si evenempty == Standard_True


//=======================================================================
//function : NewLine
//purpose  : 
//=======================================================================

void StepData_StepWriter::NewLine (const Standard_Boolean evenempty)
{
  if (evenempty || thecurr.Length() > 0) {
    thefile->Append(thecurr.Moved());
  }
  Standard_Integer indst = thelevel * 2; if (theindent) indst += theindval;
  thecurr.SetInitial(indst);  thecurr.Clear();
}


//  Regrouper ligne en cours avec precedente; reste en cours sauf si newline
//  == Standard_True, auquel cas on commence une nouvelle ligne
//  Ne fait rien si : total correspondant > StepLong ou debut ou fin d`entite


//=======================================================================
//function : JoinLast
//purpose  : 
//=======================================================================

void StepData_StepWriter::JoinLast (const Standard_Boolean)
{
  thecurr.SetKeep();
}


//=======================================================================
//function : Indent
//purpose  : 
//=======================================================================

void StepData_StepWriter::Indent (const Standard_Boolean onent)
{  theindent = onent;  }


//=======================================================================
//function : SendIdent
//purpose  : 
//=======================================================================

void StepData_StepWriter::SendIdent(const Standard_Integer ident)
{
  char lident[12];
  sprintf(lident,"#%d =",ident);
  thecurr.Clear();
  thecurr.Add (lident);
  themult = Standard_False;
}


//=======================================================================
//function : SendScope
//purpose  : 
//=======================================================================

void StepData_StepWriter::SendScope ()
{  AddString(textscope);  }


//=======================================================================
//function : SendEndscope
//purpose  : 
//=======================================================================

void StepData_StepWriter::SendEndscope ()
{
  NewLine(Standard_False);
  thefile->Append(new TCollection_HAsciiString(textendscope));
}


//=======================================================================
//function : Comment
//purpose  : 
//=======================================================================

void StepData_StepWriter::Comment (const Standard_Boolean mode)
{
  if (mode && !thecomm) AddString(textcomm,20);
  if (!mode && thecomm) AddString(textendcomm);
  thecomm = mode;
}


//=======================================================================
//function : SendComment
//purpose  : 
//=======================================================================

void StepData_StepWriter::SendComment(const Handle(TCollection_HAsciiString)& text)
{
  if (!thecomm) throw Interface_InterfaceMismatch("StepWriter : Comment");
  AddString(text->ToCString(),text->Length());
}


//=======================================================================
//function : SendComment
//purpose  : 
//=======================================================================

void StepData_StepWriter::SendComment (const Standard_CString text)
{
  if (!thecomm) throw Interface_InterfaceMismatch("StepWriter : Comment");
  AddString(text,(Standard_Integer) strlen(text));
}


//=======================================================================
//function : StartEntity
//purpose  : 
//=======================================================================

void StepData_StepWriter::StartEntity(const TCollection_AsciiString& atype)
{
  if (atype.Length() == 0) return;
  if (themult) {
    if (thelevel != 1) throw Interface_InterfaceMismatch("StepWriter : StartEntity");   // decompte de parentheses mauvais ...
    AddString(textendlist);
    AddString(" ",1); //skl 29.01.2003
  }
  themult = Standard_True;
  //AddString(" ",1);  //skl 29.01.2003
  AddString(atype);
  thelevel  = 0;
  theindval = thecurr.Length();
  thecurr.SetInitial(0);
  thefirst  = Standard_True;
  OpenSub();
}


//=======================================================================
//function : StartComplex
//purpose  : 
//=======================================================================

void  StepData_StepWriter::StartComplex ()
{
  AddString("( ",2); //skl 29.01.2003
}    // thelevel unchanged


//=======================================================================
//function : EndComplex
//purpose  : 
//=======================================================================

void  StepData_StepWriter::EndComplex ()
{  AddString(") ",2);  }    // thelevel unchanged


//  ....                SendField et ce qui va avec


//=======================================================================
//function : SendField
//purpose  : 
//=======================================================================

void StepData_StepWriter::SendField(const StepData_Field& fild,
                                    const Handle(StepData_PDescr)& descr)
{
  Standard_Boolean done = Standard_True;
  Standard_Integer kind = fild.Kind (Standard_False);  // valeur interne

  if (kind == 16) {
    DeclareAndCast(StepData_SelectMember,sm,fild.Transient());
    SendSelect (sm,descr);
    return;
  }
  switch (kind) {
//   ici les cas simples; ensuite on caste et on voit
    case 0 : SendUndef(); break;
    case 1 : Send        (fild.Integer  ()); break;
    case 2 : SendBoolean (fild.Boolean  ()); break;
    case 3 : SendLogical (fild.Logical  ()); break;
    case 4 : SendEnum    (fild.EnumText ()); break; // enum : descr ?
    case 5 : Send        (fild.Real     ()); break;
    case 6 : Send        (fild.String   ()); break;
    case 7 : Send        (fild.Entity   ()); break;
    case 8 : done = Standard_False; break;
    case 9 : SendDerived (); break;
    default: done = Standard_False; break;
  }
  if (done) return;

//  Que reste-t-il : les tableaux ...
  Standard_Integer arity = fild.Arity();
  if (arity == 0) {  SendUndef();  return;  }    // PAS NORMAL
  if (arity == 1) {
    OpenSub();
    Standard_Integer i,low = fild.Lower(), up = low + fild.Length() - 1;
    for (i = low; i <= up; i ++) {
      kind = fild.ItemKind(i);
      done = Standard_True;
      switch (kind) {
        case 0 : SendUndef();  break;
	case 1 : Send        (fild.Integer  (i)); break;
	case 2 : SendBoolean (fild.Boolean  (i)); break;
	case 3 : SendLogical (fild.Logical  (i)); break;
	case 4 : SendEnum    (fild.EnumText (i)); break;
	case 5 : Send        (fild.Real     (i)); break;
	case 6 : Send        (fild.String   (i)); break;
	case 7 : Send        (fild.Entity   (i)); break;
	default: SendUndef();  done = Standard_False; break;  // ANORMAL
      }
    }
    CloseSub();
    return;
  }
  if (arity == 2) {
    OpenSub();
    Standard_Integer   j,low1 = fild.Lower(1), up1 = low1 + fild.Length(1) - 1;
    for (j = low1; j <= up1; j ++) {
      Standard_Integer i=0,low2 = fild.Lower(2), up2 = low2 + fild.Length(2) - 1;
      OpenSub();
      for (i = low2; i <= up2; i ++) {
	kind = fild.ItemKind(i,j);
	done = Standard_True;
	switch (kind) {
        case 0 : SendUndef();  break;
	case 1 : Send        (fild.Integer  (i,j)); break;
	case 2 : SendBoolean (fild.Boolean  (i,j)); break;
	case 3 : SendLogical (fild.Logical  (i,j)); break;
	case 4 : SendEnum    (fild.EnumText (i,j)); break;
	case 5 : Send        (fild.Real     (i,j)); break;
	case 6 : Send        (fild.String   (i,j)); break;
	case 7 : Send        (fild.Entity   (i,j)); break;
	default: SendUndef();  done = Standard_False; break;  // ANORMAL
        }
      }
      CloseSub();
    }
    CloseSub();
    return;
  }
}


//=======================================================================
//function : SendSelect
//purpose  : 
//=======================================================================

void StepData_StepWriter::SendSelect(const Handle(StepData_SelectMember)& sm,
                                     const Handle(StepData_PDescr)& /*descr*/)
{
  //    Cas du SelectMember. Traiter le Select puis la valeur
  //    NB : traitement actuel non recursif (pas de SELNAME(SELNAME(..)) )
  Standard_Boolean selname = Standard_False;
  if (sm.IsNull()) return;  // ??
  if (sm->HasName()) {
    selname = Standard_True;
    //    SendString (sm->Name());
    //    AddString(textlist);     // SANS AJOUT DE PARAMETRE !!
    OpenTypedSub (sm->Name());
  }
  Standard_Integer kind = sm->Kind();
  switch (kind) {
    case 0 : SendUndef(); break;
    case 1 : Send        (sm->Integer  ()); break;
    case 2 : SendBoolean (sm->Boolean  ()); break;
    case 3 : SendLogical (sm->Logical  ()); break;
    case 4 : SendEnum    (sm->EnumText ()); break; // enum : descr ?
    case 5 : Send        (sm->Real     ()); break;
    case 6 : Send        (sm->String   ()); break;
    case 8 : SendArrReal (Handle(StepData_SelectArrReal)::DownCast(sm)->ArrReal()); break;
    default: break;    // ??
  }
  if (selname) CloseSub();
}


//=======================================================================
//function : SendList
//purpose  : 
//=======================================================================

void  StepData_StepWriter::SendList(const StepData_FieldList& list,
                                    const Handle(StepData_ESDescr)& descr)
{
// start entity  ?
  Standard_Integer i, nb = list.NbFields();
  for (i = 1; i <= nb; i ++) {
    Handle(StepData_PDescr) pde;
    if (!descr.IsNull()) pde  = descr->Field(i);
    const StepData_Field fild = list.Field(i);
    SendField (fild,pde);
  }
// end entity  ?
}

//  ....                Send* de base


//=======================================================================
//function : OpenSub
//purpose  : 
//=======================================================================

void StepData_StepWriter::OpenSub ()
{
  AddParam();
  AddString(textlist);
  thefirst = Standard_True;
  thelevel ++;
}


//=======================================================================
//function : OpenTypedSub
//purpose  : 
//=======================================================================

void StepData_StepWriter::OpenTypedSub (const Standard_CString subtype)
{
  AddParam();
  if (subtype[0] != '\0') AddString (subtype,(Standard_Integer) strlen(subtype));
  AddString(textlist);
  thefirst = Standard_True;
  thelevel ++;
}


//=======================================================================
//function : CloseSub
//purpose  : 
//=======================================================================

void StepData_StepWriter::CloseSub ()
{
  AddString(textendlist);
  thefirst = Standard_False;  // le parametre suivant une sous-liste n est donc pas 1er
  thelevel --;
}


//=======================================================================
//function : AddParam
//purpose  : 
//=======================================================================

void StepData_StepWriter::AddParam ()
{
  if (!thefirst) AddString(textparam);
  thefirst = Standard_False;
}


//=======================================================================
//function : Send
//purpose  : 
//=======================================================================

void StepData_StepWriter::Send (const Standard_Integer val)
{
  char lval[12];
  AddParam();
  sprintf(lval,"%d",val);
  AddString(lval,(Standard_Integer) strlen(lval));
}


//=======================================================================
//function : Send
//purpose  : 
//=======================================================================

void StepData_StepWriter::Send (const Standard_Real val)
{
//    Valeur flottante, expurgee de "0000" qui trainent et de "E+00"
  char lval[24];
  Standard_Integer lng = thefloatw.Write(val,lval);
  AddParam();
  AddString(lval,lng);    // gere le format specifique : si besoin est
}

//  Send(String) : attention, on envoie un Texte ... donc entre '  '

//=======================================================================
//function : Send
//purpose  : 
//=======================================================================

void StepData_StepWriter::Send (const TCollection_AsciiString& val)
{
  AddParam();
  TCollection_AsciiString aval(val);  // on duplique pour trafiquer si besoin
  Standard_Integer nb = aval.Length();  Standard_Integer nn = nb;
  aval.AssignCat('\'');    // comme cela, Insert(i+1) est OK

//    Conversion des Caracteres speciaux
  for (Standard_Integer i = nb; i > 0; i --) {
    char uncar = aval.Value(i);
    if (uncar == '\'') {  aval.Insert(i+1,'\'');  nn ++;    continue;  }
    if (uncar == '\\') {  aval.Insert(i+1,'\\');  nn ++;    continue;  }
    if (uncar == '\n') {  aval.SetValue(i,'\\');  aval.Insert(i+1,'\\');
			  aval.Insert(i+1,'N' );  nn += 2;  continue;  }
    if (uncar == '\t') {  aval.SetValue(i,'\\');  aval.Insert(i+1,'\\');
			  aval.Insert(i+1,'T' );  nn += 2;  continue;  }
  }
  //:i2 abv 31 Aug 98: ProSTEP TR9: avoid wrapping text or do it at spaces
  aval.Insert(1,'\'');
  nn += 2;

//:i2  AddString ("\'",1); nn ++;

//    Attention au depassement des 72 caracteres
  if (thecurr.CanGet(nn)) AddString(aval,0);
  //:i2
  else {
    thefile->Append(thecurr.Moved());
    Standard_Integer indst = thelevel * 2; if (theindent) indst += theindval;
    if ( indst+nn <= StepLong ) thecurr.SetInitial(indst);
    else thecurr.SetInitial(0);
    if ( thecurr.CanGet(nn) ) AddString(aval,0);
    else {
      while ( nn >0 ) {
	if (nn <= StepLong) {
	  thecurr.Add (aval);  // Ca yet, on a tout epuise
	  thecurr.FreezeInitial();
	  break;
	}
	Standard_Integer stop = StepLong; // position of last separator
	for ( ; stop > 0 && aval.Value(stop) != ' '; stop-- );
	if ( ! stop ) {
	  stop = StepLong;
	  for ( ; stop > 0 && aval.Value(stop) != '\\'; stop-- );
	  if ( ! stop ) {
	    stop = StepLong;
	    for ( ; stop > 0 && aval.Value(stop) != '_'; stop-- );
	    if ( ! stop ) stop = StepLong;
	  }
	}
	TCollection_AsciiString bval = aval.Split(stop);
	thefile->Append(new TCollection_HAsciiString(aval));
	aval = bval;
	nn -= stop;
      }
    }
  }
/* //:i2
  else {
    //    Il faut tronconner ...  lignes limitees a 72 caracteres (StepLong)
    Standard_Integer ncurr = thecurr.Length();
    Standard_Integer nbuff = StepLong - ncurr;
    thecurr.Add (aval.ToCString(),nbuff);
    thefile->Append(thecurr.Moved());
    aval.Remove(1,nbuff);
    nn -= nbuff;
    while (nn > 0) {
      if (nn <= StepLong) {
	thecurr.Add (aval);  // Ca yet, on a tout epuise
	thecurr.FreezeInitial();
	break;
      }
      TCollection_AsciiString bval = aval.Split(StepLong);
      thefile->Append(new TCollection_HAsciiString(bval));
      nn -= StepLong;
    }
  }
//:i2 */  
//  thecurr.Add('\'');   deja mis dans aval au debut
}


//=======================================================================
//function : Send
//purpose  : 
//=======================================================================

void StepData_StepWriter::Send (const Handle(Standard_Transient)& val)
{
  char lident[20];
//  Undefined ?
  if (val.IsNull()) {
//   throw Interface_InterfaceMismatch("StepWriter : Sending Null Reference");
    thechecks.CCheck(thenum)->AddFail("Null Reference");
    SendUndef();
    Comment(Standard_True);
    SendComment(" NUL REF ");
    Comment(Standard_False);
    return;
  }
  Standard_Integer num = themodel->Number(val);
//  String ? (si non repertoriee dans le Modele)
  if (num == 0) {
    if (val->IsKind(STANDARD_TYPE(TCollection_HAsciiString))) {
      DeclareAndCast(TCollection_HAsciiString,strval,val);
      Send (TCollection_AsciiString(strval->ToCString()));
      return;
    }
//  SelectMember ? (toujours, si non repertoriee)
//  mais attention, pas de description attachee
    else if (val->IsKind(STANDARD_TYPE(StepData_SelectMember))) {
      DeclareAndCast(StepData_SelectMember,sm,val);
      Handle(StepData_PDescr) descr;  // null
      SendSelect (sm,descr);
    }
//  Sinon, PAS NORMAL !
    else {
      thechecks.CCheck(thenum)->AddFail("UnknownReference");
      SendUndef();
      Comment(Standard_True);
      SendComment(" UNKNOWN REF ");
      Comment(Standard_False);
//      throw Interface_InterfaceMismatch("StepWriter : Sending Unknown Reference");
    }
  }
//  Cas normal : une bonne Entite, on envoie son Ident.
  else {
    Standard_Integer idnum = num, idtrue = 0;
    if (thelabmode > 0) idtrue = themodel->IdentLabel(val);
    if (thelabmode == 1) idnum = idtrue;
    if (idnum == 0) idnum = num;
    if (thelabmode < 2 || idnum == idtrue) sprintf(lident,"#%d",idnum);
    else sprintf(lident,"%d:#%d",idnum,idtrue);
    AddParam();
    AddString(lident,(Standard_Integer) strlen(lident));
  }
}


//=======================================================================
//function : SendBoolean
//purpose  : 
//=======================================================================

void StepData_StepWriter::SendBoolean (const Standard_Boolean val)
{
  if (val) SendString(texttrue);
  else     SendString(textfalse);
}


//=======================================================================
//function : SendLogical
//purpose  : 
//=======================================================================

void StepData_StepWriter::SendLogical (const StepData_Logical val)
{
  if      (val == StepData_LTrue)   SendString(texttrue);
  else if (val == StepData_LFalse)  SendString(textfalse);
  else                              SendString(textunknown);
}


//  SendString : attention, on donne l'intitule exact

//=======================================================================
//function : SendString
//purpose  : 
//=======================================================================

void StepData_StepWriter::SendString (const TCollection_AsciiString& val)
{
  AddParam();
  AddString(val);
}

//  SendString : attention, on donne l'intitule exact

//=======================================================================
//function : SendString
//purpose  : 
//=======================================================================

void StepData_StepWriter::SendString (const Standard_CString val)
{
  AddParam();
  AddString(val,(Standard_Integer) strlen(val));
}

//  SendEnum : attention, on envoie un intitule d'Enum ... donc entre .  .

//=======================================================================
//function : SendEnum
//purpose  : 
//=======================================================================

void StepData_StepWriter::SendEnum (const TCollection_AsciiString& val)
{
  if (val.Length() == 1 && val.Value(1) == '$')  {  SendUndef();  return;  }
  AddParam();
  TCollection_AsciiString aValue = val;
  if (aValue.Value(1) != '.') aValue.Prepend('.');
  if (aValue.Value(aValue.Length()) != '.') aValue+='.';
  AddString(aValue,2);
  
}

//  SendEnum : attention, on envoie un intitule d'Enum ... donc entre .  .

//=======================================================================
//function : SendEnum
//purpose  : 
//=======================================================================

void StepData_StepWriter::SendEnum (const Standard_CString val)
{
  
  if (val[0] == '$' && val[1] == '\0')  {  SendUndef();  return;  }
  TCollection_AsciiString aValue(val);
  SendEnum(aValue);
}


//=======================================================================
//function : SendArrReal
//purpose  : 
//=======================================================================

void StepData_StepWriter::SendArrReal (const Handle(TColStd_HArray1OfReal) &anArr)
{
  AddString(textlist);
  if(anArr->Length()>0) {
    // add real
    Send(anArr->Value(1));
    for( Standard_Integer i=2; i<=anArr->Length(); i++) {
//      AddString(textparam);
      //add real
      Send(anArr->Value(i));
    }
  }
  AddString(textendlist);
}


//=======================================================================
//function : SendUndef
//purpose  : 
//=======================================================================

void StepData_StepWriter::SendUndef ()
{
  AddParam();
  AddString(textundef);
}


//=======================================================================
//function : SendDerived
//purpose  : 
//=======================================================================

void StepData_StepWriter::SendDerived ()
{
  AddParam();
  AddString(textderived);
}


// EndEntity : s'il faut mettre ; a la ligne, l'aligner sur debut d'entite ...

//=======================================================================
//function : EndEntity
//purpose  : 
//=======================================================================

void StepData_StepWriter::EndEntity ()
{
  if (thelevel != 1) throw Interface_InterfaceMismatch("StepWriter : EndEntity");   // decompte de parentheses mauvais ...
  AddString(textendent);
  thelevel  = 0;        // on garde theindval : sera traite au prochain NewLine
  Standard_Boolean indent = theindent; theindent = Standard_False;
  NewLine(Standard_False); theindent = indent;
  themult = Standard_False;
// pour forcer indentation si necessaire
}


//  gestion de la ligne courante (cf aussi NewLine/JoinLine)

//=======================================================================
//function : AddString
//purpose  : 
//=======================================================================

void StepData_StepWriter::AddString(const TCollection_AsciiString& astr,
                                    const Standard_Integer more)
{
  while (!thecurr.CanGet(astr.Length() + more)) {
    thefile->Append(thecurr.Moved());
    Standard_Integer indst = thelevel * 2; if (theindent) indst += theindval;
    thecurr.SetInitial(indst);
  }
  thecurr.Add(astr);
}


//=======================================================================
//function : AddString
//purpose  : 
//=======================================================================

void StepData_StepWriter::AddString(const Standard_CString astr,
                                    const Standard_Integer lnstr,
                                    const Standard_Integer more)
{
  while (!thecurr.CanGet(lnstr + more)) {
    thefile->Append(thecurr.Moved());
    Standard_Integer indst = thelevel * 2; if (theindent) indst += theindval;
    thecurr.SetInitial(indst);
  }
  thecurr.Add(astr,lnstr);
}


//   ENVOI FINAL


//=======================================================================
//function : CheckList
//purpose  : 
//=======================================================================

Interface_CheckIterator  StepData_StepWriter::CheckList () const
{
  return thechecks;
}


//=======================================================================
//function : NbLines
//purpose  : 
//=======================================================================

Standard_Integer  StepData_StepWriter::NbLines () const
{  return thefile->Length();  }


//=======================================================================
//function : Line
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepData_StepWriter::Line
       (const Standard_Integer num) const
{  return thefile->Value(num);  }


//=======================================================================
//function : Printw
//purpose  : 
//=======================================================================

Standard_Boolean StepData_StepWriter::Print (Standard_OStream& S)
{
  Standard_Boolean isGood = (S.good());
  Standard_Integer nb = thefile->Length();
  for (Standard_Integer i = 1; i <= nb && isGood; i ++) 
    S << thefile->Value(i)->ToCString() << "\n";
  
  S<< std::flush;
  isGood = (S && S.good());
  
  return  isGood;
  
}
