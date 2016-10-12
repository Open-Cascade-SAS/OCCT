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


#include <IFSelect_BasicDumper.hxx>
#include <IFSelect_Dispatch.hxx>
#include <IFSelect_GeneralModifier.hxx>
#include <IFSelect_IntParam.hxx>
#include <IFSelect_Modifier.hxx>
#include <IFSelect_SelectAnyList.hxx>
#include <IFSelect_SelectAnyType.hxx>
#include <IFSelect_SelectCombine.hxx>
#include <IFSelect_SelectControl.hxx>
#include <IFSelect_SelectDeduct.hxx>
#include <IFSelect_SelectExtract.hxx>
#include <IFSelect_Selection.hxx>
#include <IFSelect_SessionDumper.hxx>
#include <IFSelect_SessionFile.hxx>
#include <IFSelect_ShareOut.hxx>
#include <IFSelect_Transformer.hxx>
#include <IFSelect_WorkSession.hxx>
#include <Interface_Macros.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <OSD_OpenFile.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HSequenceOfInteger.hxx>

#include <stdio.h>
static int deja = 0;


    IFSelect_SessionFile::IFSelect_SessionFile
  (const Handle(IFSelect_WorkSession)& WS)
{
  ClearLines();
  themode = Standard_False;
  if (!deja) {    // au moins celui-la :
    Handle(IFSelect_BasicDumper) basedumper = new IFSelect_BasicDumper;
    deja = 1;
  }
  thedone = Standard_False;
  thelastgen = 0;
  thesess = WS;
}

    IFSelect_SessionFile::IFSelect_SessionFile
  (const Handle(IFSelect_WorkSession)& WS, const Standard_CString filename)
{
  ClearLines();
  themode = Standard_True;
  if (!deja) {    // au moins celui-la :
    Handle(IFSelect_BasicDumper) basedumper = new IFSelect_BasicDumper;
    deja = 1;
  }
  thedone = Standard_False;
  theownflag = Standard_False;
  thelastgen = 0;
  thesess = WS;
  thedone = (Write (filename) == 0);
//  Close fait par Write (selon les cas)
}


    void  IFSelect_SessionFile::ClearLines ()
      {  thelist.Clear();  thenl = 0;  }

    Standard_Integer  IFSelect_SessionFile::NbLines () const
      {  return thelist.Length();  }


    const TCollection_AsciiString&  IFSelect_SessionFile::Line
  (const Standard_Integer num) const
      {  return thelist.Value(num);  }


    void  IFSelect_SessionFile::AddLine (const Standard_CString line)
      {  thelist.Append (TCollection_AsciiString(line) );  }

    void  IFSelect_SessionFile::RemoveLastLine ()
      {  if (thelist.Length() > 1) thelist.Remove(thelist.Length());  }


    Standard_Boolean  IFSelect_SessionFile::WriteFile
  (const Standard_CString filename)
{
  FILE* lefic = OSD_OpenFile(filename,"w");
  Standard_Integer nbl = thelist.Length();
  for (Standard_Integer i = 1; i <= nbl; i ++)
    fprintf (lefic,"%s\n",thelist.Value(i).ToCString());
  fclose ( lefic );
  ClearLines();
  return Standard_True;
}

    Standard_Boolean  IFSelect_SessionFile::ReadFile
  (const Standard_CString filename)
{
  char ligne[201];
  FILE* lefic = OSD_OpenFile(filename,"r");
  if (!lefic) return Standard_False;
  ClearLines();
//  read mode : lire les lignes
//  On charge le fichier dans "thelist"
  Standard_Boolean header = Standard_False;
  for(;;) {
    ligne[0] = '\0';
    if (fgets(ligne,200,lefic) == NULL
     || feof(lefic) != 0)
    {
      break;
    }
    if (ligne[0] == '\0') continue;
//  D abord ligne initiale ?
    if (!header)
      {  if (!RecognizeFile(ligne)) break;  header = Standard_True;  }
    ligne[200] = '\0';  // fin forcee ...
    TCollection_AsciiString onemore(ligne);
    thelist.Append(onemore);
  }
  fclose ( lefic );
  return header;
}

    Standard_Boolean  IFSelect_SessionFile::RecognizeFile
  (const Standard_CString headerline)
{
  Handle(Message_Messenger) sout = Message::DefaultMessenger();

  SplitLine (headerline);
  if (theline.Length() != 4) { sout<<"File Form Incorrect"<<endl; return Standard_False; }
  Handle(Standard_Type) sesstype = thesess->DynamicType();
  if (!theline.Value(1).IsEqual("!XSTEP")  ||
      !theline.Value(2).IsEqual("SESSION") ||
      !theline.Value(4).IsEqual(sesstype->Name()) )
    { sout<<"Lineno."<<thenl<<" : File Header Description Incorrect"<<endl; return Standard_False; }
//   Value(3) definit la VERSION du format de fichier
  return Standard_True;
}


    Standard_Integer  IFSelect_SessionFile::Write
  (const Standard_CString filename)
{
  thenewnum = 0;
  Standard_Integer stat = WriteSession();
  if (stat != 0) return stat;
  stat = WriteEnd();
  if (stat != 0) return stat;
  return (WriteFile(filename) ? 0 : -1);
}

    Standard_Integer  IFSelect_SessionFile::Read
  (const Standard_CString filename)
{
  if (!ReadFile(filename)) return -1;
  thenewnum = 0;
  Standard_Integer stat = ReadSession();
  if (stat != 0) return stat;
  stat = ReadEnd();
  return stat;
}


//  ##################################################################
//  ########        WriteSession : Ecriture du contenu        ########

    Standard_Integer  IFSelect_SessionFile::WriteSession ()
{
  char laligne[200];
  thedone = Standard_True;
//  ...  Preparation Specifique
  thenames.Clear();
  Standard_Integer nbidents = thesess->MaxIdent();
  thenums = new TColStd_HArray1OfInteger (0,nbidents); thenums->Init(0);
  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for ( i = 1; i <= nbidents; i ++) {
    Handle(Standard_Transient) item = thesess->Item(i);
    if (!item.IsNull()) thenums->SetValue(i,-1);
  }

//  ...  ECRITURE
  sprintf (laligne,"!XSTEP SESSION V1 %s",thesess->DynamicType()->Name());
  WriteLine(laligne,'\n');
  sprintf (laligne,"!GENERALS");
  WriteLine(laligne,'\n');
  sprintf (laligne,"ErrorHandle %d", (thesess->ErrorHandle() ? 1 : 0));
  WriteLine(laligne,'\n');
  Handle(TColStd_HSequenceOfInteger) idents;
  Standard_Integer nb;
  Handle(TCollection_HAsciiString) name;

  idents = thesess->ItemIdents(STANDARD_TYPE(IFSelect_IntParam));
  nb = idents->Length();
  if (nb > 0) WriteLine ("!INTEGERS",'\n');
  Standard_Integer j; // svv Jan11 2000 : porting on DEC
  for (j = 1; j <= nb; j ++) {
    i = idents->Value(j);
    Handle(IFSelect_IntParam) P = thesess->IntParam(i);
    name = thesess->Name(P);
    if (name.IsNull()) {
      thenewnum ++;  idents->SetValue(i,thenewnum);
      sprintf(laligne," #%d %d",thenewnum,P->Value());
    }
    else  sprintf(laligne," %s %d",name->ToCString(),P->Value());
    WriteLine(laligne,'\n');
  }

  idents = thesess->ItemIdents(STANDARD_TYPE(TCollection_HAsciiString));
  nb = idents->Length();
  if (nb > 0) WriteLine ("!TEXTS",'\n');
  for (j = 1; j <= nb; j ++) {
    i = idents->Value(j);
    Handle(TCollection_HAsciiString) P = thesess->TextParam(i);
    name = thesess->Name(P);
    if (name.IsNull()) {
      thenewnum ++;  thenums->SetValue(i,thenewnum);
      sprintf(laligne," #%d %s",thenewnum,P->ToCString());
    }
    else  sprintf(laligne," %s %s",name->ToCString(),P->ToCString());
    WriteLine(laligne,'\n');
  }

  idents = thesess->ItemIdents(STANDARD_TYPE(IFSelect_Selection));
  nb = idents->Length();
  if (nb > 0) WriteLine ("!SELECTIONS",'\n');
  for (j = 1; j <= nb; j ++) {
    i = idents->Value(j);
    Handle(IFSelect_Selection) P = thesess->Selection(i);
    NewItem (i,P);
//  ..  Ecritures particulieres
//  ->  Traiter les principaux sous-types : Extract,AnyList,AnyType
    DeclareAndCast(IFSelect_SelectExtract,sxt,P);
    if (!sxt.IsNull()) {
      sprintf(laligne," %c", (sxt->IsDirect() ? 'D' : 'R'));
      WriteLine(laligne);
    }
    DeclareAndCast(IFSelect_SelectAnyList,sli,P);
    if (!sli.IsNull()) {
      SetOwn(Standard_False);
      WriteLine(" LIST");
      SendItem(sli->Lower());
      SendItem(sli->Upper());
      SetOwn(Standard_True);
    }
//  ..  Ecritures specifiques selon dumpers
    WriteOwn(P);
    WriteLine("",'\n');
  }

  SetOwn(Standard_False);
  if (nb > 0) WriteLine ("!SOURCES",'\n');
  for (j = 1; j <= nb; j ++) {
    i = idents->Value(j);
    Handle(IFSelect_Selection) P = thesess->Selection(i);
    Standard_Integer nbs = thesess->NbSources(P);
    if (nbs == 0) continue;
    name = thesess->Name(P);
    if (name.IsNull())  sprintf(laligne," #%d %d",thenums->Value(i),nbs);
    else                sprintf(laligne," %s %d",name->ToCString(),nbs);
    WriteLine(laligne);
    for (Standard_Integer k = 1; k <= nbs; k ++)
      SendItem (thesess->Source(P,k));
    WriteLine("",'\n');
  }

  idents = thesess->ItemIdents(STANDARD_TYPE(IFSelect_GeneralModifier));
  nb = idents->Length();
  if (nb > 0) WriteLine ("!MODIFIERS",'\n');
  for (j = 1; j <= nb; j ++) {
//  Description de base des Modifiers, donc sans Selection ni Dispatch-Rank
    i = idents->Value(j);
    Handle(IFSelect_GeneralModifier) P = thesess->GeneralModifier(i);
    NewItem (i,P);
    SetOwn(Standard_True);
//  ..  Ecritures specifiques selon dumpers
    WriteOwn(P);
    WriteLine("",'\n');
  }

  idents = thesess->ItemIdents(STANDARD_TYPE(IFSelect_Transformer));
  nb = idents->Length();
  if (nb > 0) WriteLine ("!TRANSFORMERS",'\n');
  for (j = 1; j <= nb; j ++) {
//  Description des Transformers
    i = idents->Value(j);
    Handle(IFSelect_Transformer) P = thesess->Transformer(i);
    NewItem (i,P);
    SetOwn(Standard_True);
//  ..  Ecritures specifiques selon dumpers
    WriteOwn(P);
    WriteLine("",'\n');
  }

  SetOwn(Standard_False);
  idents = thesess->ItemIdents(STANDARD_TYPE(IFSelect_Dispatch));
  nb = idents->Length();
  if (nb > 0) WriteLine ("!DISPATCHES",'\n');
  for (j = 1; j <= nb; j ++) {
    i = idents->Value(j);
    Handle(IFSelect_Dispatch) P = thesess->Dispatch(i);
    NewItem (i,P);
//  ..  Final Selection
    SetOwn(Standard_False);
    SendItem(P->FinalSelection());
    SetOwn(Standard_True);
//  ..  Ecritures specifiques selon dumpers
    WriteOwn(P);
    WriteLine("",'\n');
  }

  WriteLine ("!FILENAMING");
  SetOwn(Standard_False);
  Handle(TCollection_HAsciiString) namingpart = thesess->FilePrefix();
  if (namingpart->IsEmpty()) namingpart.Nullify();
  if (namingpart.IsNull()) SendVoid();
  else SendText(namingpart->ToCString());
  namingpart = thesess->DefaultFileRoot();
  if (namingpart->IsEmpty()) namingpart.Nullify();
  if (namingpart.IsNull()) SendVoid();
  else SendText(namingpart->ToCString());
  namingpart = thesess->FileExtension();
  if (namingpart->IsEmpty()) namingpart.Nullify();
  if (namingpart.IsNull()) SendVoid();
  else SendText(namingpart->ToCString());
  WriteLine("",'\n');

  for (j = 1; j <= nb; j ++) {
    i = idents->Value(j);
    Handle(IFSelect_Dispatch) P = thesess->Dispatch(i);
    if (!P->HasRootName()) continue;
    namingpart = P->RootName();
    SetOwn(Standard_False);
    SendItem(P);
    sprintf(laligne," %s",namingpart->ToCString());
    WriteLine(laligne,' ');
    WriteLine("",'\n');
  }

//   Pour les Modifiers, ATTENTION car il faut respecter l ORDRE effectif
//   Or il y a deux listes : Model Modifiers; File Modifiers
//   Les Modifiers eux-memes ont deja ete ecrits
//   Ici, on ecrit simplement leur utilisation dans l envoi final
  for (Standard_Integer formod = 1; formod >= 0; formod --) {
    idents = thesess->FinalModifierIdents((formod > 0));  // donnes dans l ordre d application
    nb = idents->Length();
    if (nb == 0) continue;
    if (formod > 0) WriteLine ("!MODELMODIFIERS",'\n');
    else            WriteLine ("!FILEMODIFIERS",'\n');
    for (j = 1; j <= nb; j ++) {
      i = idents->Value(j);
      Handle(IFSelect_GeneralModifier) P = thesess->GeneralModifier(i);
      SetOwn(Standard_False);
      SendItem(P);
      //  ..  Parametres Generaux (les specifiques ont deja ete envoyes)
      SendItem(P->Selection());
      SendItem(P->Dispatch());
      WriteLine("",'\n');
    }
  }

//  ...  Conclusion
  theline.Clear();
  return 0;
}

    Standard_Integer  IFSelect_SessionFile::WriteEnd ()
{
  WriteLine("!XSTEP END",'\n');   // sinon, cf sous-types de SessionFile ...
  return 0;
}


    void  IFSelect_SessionFile::WriteLine
  (const Standard_CString line, const Standard_Character follow)
{
  if (line[0] != '\0') thebuff.AssignCat (line);
  if (follow == '\0') return;
  if (follow != '\n') thebuff.AssignCat(follow);
  else {
    thelist.Append(thebuff);
    thebuff.Clear();
    thenl ++;
  }
}

    Standard_Boolean  IFSelect_SessionFile::WriteOwn
  (const Handle(Standard_Transient)& item)
{
  if (item.IsNull()) return Standard_False;
  SetOwn(Standard_True);
  Handle(IFSelect_SessionDumper) dumper = IFSelect_SessionDumper::First();
  while (!dumper.IsNull()) {
    if (dumper->WriteOwn(*this,item)) break;
    dumper = dumper->Next();
  }
  SetOwn(Standard_False);
  return (!dumper.IsNull());    // IsNull -> echec
}



//  ##################################################################
//  ########        ReadSession  :  Lecture du contenu        ########

    Standard_Integer  IFSelect_SessionFile::ReadSession ()
{
  Handle(Message_Messenger) sout = Message::DefaultMessenger();

  thedone = Standard_True;
//  ...  Preparation Specifique
  thenums.Nullify();
  thenames.Clear();
//  ..  Donnees generales, controle
  if (!ReadLine()) return 1;
  if (theline.Length() != 4) { sout<<"File Form Incorrect"<<endl; return 1; }
  Handle(Standard_Type) sesstype = thesess->DynamicType();
  if (!theline.Value(1).IsEqual("!XSTEP")  ||
      !theline.Value(2).IsEqual("SESSION") ||
      !theline.Value(4).IsEqual(sesstype->Name()) )
    { sout<<"Lineno."<<thenl<<" : File Header Description Incorrect"<<endl; return 1; }
//   Value(3) definit la VERSION du format de fichier
  if (!ReadLine()) return 1;

//  ..  Parametres Generaux
  Standard_Integer rubr =
    (theline.Length() == 1 && theline.Value(1).IsEqual("!GENERALS"));
  while (rubr) {
    if (!ReadLine()) return 1;
    if (theline.Length() == 0) continue;
    const TCollection_AsciiString& ungen = theline.Value(1);
    if (ungen.Value(1) == '!') break;  // fin des generaux
    if (ungen.IsEqual("ErrorHandle")) {
      if (theline.Length() != 2)
	{ sout<<"Lineno."<<thenl<<" : ErrorHandle Description Incorrect"<<endl; continue; }
      if      (theline.Value(2).IsEqual("0"))
	thesess->SetErrorHandle(Standard_False);
      else if (theline.Value(2).IsEqual("1"))
	thesess->SetErrorHandle(Standard_True);
      else { sout<<"Lineno."<<thenl<<" : ErrorHandle Incorrect : "<<theline.Value(2)<<endl; continue; }
      continue;
    }
    else sout<<"Lineno."<<thenl<<" : Unknown General Parameter : "<<ungen<<" , ignored"<<endl;
  }

//  ..  IntParams
//   deja fait  if (!ReadLine()) return 1;
  rubr = (theline.Length() == 1 && theline.Value(1).IsEqual("!INTEGERS"));
  while (rubr) {
    if (!ReadLine()) return 1;
    if (theline.Value(1).Value(1) == '!') break;    // liste suivante
    if (theline.Length() != 2)
      { sout<<"Lineno."<<thenl<<" : An Integer Parameter is badly defined"<<endl; continue; }
    Handle(IFSelect_IntParam) par = new IFSelect_IntParam;
    par->SetValue ( atoi(theline.Value(2).ToCString()) );
    AddItem (par);
  }

//  .. TextParams (ligne de garde deja lue)
  rubr = (theline.Length() == 1 && theline.Value(1).IsEqual("!TEXTS"));
  while (rubr) {
    if (!ReadLine()) return 1;
    if (theline.Value(1).Value(1) == '!') break;    // liste suivante
    if (theline.Length() != 2)
      { sout<<"Lineno."<<thenl<<" : A Text Parameter is badly defined"<<endl; continue; }
//    Attention, un texte peut contenir des blancs ...  repartir de line(thenl)
    TCollection_AsciiString oneline = thelist.Value(thenl);
    Standard_Integer iw = 0, inc = 0;
    for (Standard_Integer ic = 1; ic <= oneline.Length(); ic ++) {
      char unc = oneline.Value(1);
      inc = ic;
      if (unc == ' ') iw = 1;
      else if (iw > 0) break;
    }
    oneline.Remove (1,inc);
    AddItem ( new TCollection_HAsciiString (oneline.ToCString()) );
  }

//  .. Selections (ligne de garde deja lue)
  rubr = (theline.Length() == 1 && theline.Value(1).IsEqual("!SELECTIONS"));
  while (rubr) {
    if (!ReadLine()) return 1;
    if (theline.Value(1).Value(1) == '!') break;    // liste suivante
    if (theline.Length() < 2)
      { sout<<"Lineno."<<thenl<<" : A Selection is badly defined"<<endl; continue; }
//  ..  Analyse de certains cas generaux
    Handle(IFSelect_IntParam) low,up;
    Standard_Integer firstown = 3;
    Standard_Integer direct   = 0;
    Standard_Integer numlist  = 0;
    if (theline.Length() > 2) {
      if      (theline.Value(3).IsEqual("D")) direct = 1;
      else if (theline.Value(3).IsEqual("R")) direct = -1;
      if (direct != 0) firstown ++;
      if (firstown+2 <= theline.Length()) {
	if (theline.Value(firstown).IsEqual("LIST"))  {
	  numlist = firstown;  firstown += 3;
	  low = GetCasted(IFSelect_IntParam,ItemValue(numlist+1));
	  up  = GetCasted(IFSelect_IntParam,ItemValue(numlist+2));
	}
      }
      SetLastGeneral (firstown-1);
    }
    Handle(Standard_Transient) item;   // a fournir ...
    ReadOwn(item);
    if (item.IsNull()) continue;
    DeclareAndCast(IFSelect_SelectExtract,sxt,item);
    if (!sxt.IsNull()) {
      if (direct == 0) sout<<"Lineno."<<thenl<<" : A SelectExtract is badly defined"<<endl;
      else  sxt->SetDirect( (direct > 0) );
    }
    DeclareAndCast(IFSelect_SelectAnyList,sli,item);
    if (!sli.IsNull()) {
      if (numlist == 0) sout<<"Lineno."<<thenl<<" : A SelectAnyList is badly defined"<<endl;
      else sli->SetRange(low,up);
    }
    AddItem(item);
  }

//  .. Sources
  rubr = (theline.Length() == 1 && theline.Value(1).IsEqual("!SOURCES"));
  while (rubr) {
    if (!ReadLine()) return 1;
    if (theline.Value(1).Value(1) == '!') break;    // liste suivante
    if (theline.Length() < 3)
      { sout<<"Lineno."<<thenl<<" : A Selection Source List is badly defined"<<endl; continue; }
    DeclareAndCast(IFSelect_Selection,sel,ItemValue(1));
    if (sel.IsNull())
      { sout<<"Lineno."<<thenl<<" : A Source List is not for a Selection"<<endl; continue; }
    Standard_Integer nbs = atoi(theline.Value(2).ToCString());
//  .. Differents cas reconnus
    DeclareAndCast(IFSelect_SelectExtract,sxt,sel);
    if (!sxt.IsNull()) {
      if (nbs > 1)
	sout<<"Lineno."<<thenl<<" : SelectExtract, more than one source, followings ignored"<<endl;
      DeclareAndCast(IFSelect_Selection,source,ItemValue(3));
      sxt->SetInput(source);
    }
    DeclareAndCast(IFSelect_SelectDeduct,sdt,sel);
    if (!sdt.IsNull()) {
      if (nbs > 1)
	sout<<"Lineno."<<thenl<<" : SelectDeduct, more than one source, followings ignored"<<endl;
      sdt->SetInput(GetCasted(IFSelect_Selection,ItemValue(3)));
    }
    DeclareAndCast(IFSelect_SelectControl,sct,sel);
    if (!sct.IsNull()) {
      if (nbs != 2)
	sout<<"Lineno."<<thenl<<" : SelectControl, not two sources, followings ignored"<<endl;
      sct->SetMainInput   (GetCasted(IFSelect_Selection,ItemValue(3)));
      sct->SetSecondInput (GetCasted(IFSelect_Selection,ItemValue(4)));
    }
    DeclareAndCast(IFSelect_SelectCombine,sco,sel);
    if (!sco.IsNull()) {
      for (Standard_Integer j = 1; j <= nbs; j ++)
	sco->Add(GetCasted(IFSelect_Selection,ItemValue(j+2)));
    }
  }

//  ... Modifiers en tout genre
  rubr = (theline.Length() == 1 && theline.Value(1).IsEqual("!MODIFIERS"));
  while (rubr) {
    if (!ReadLine()) return 1;
    if (theline.Value(1).Value(1) == '!') break;    // liste suivante
    if (theline.Length() < 2)
      { sout<<"Lineno."<<thenl<<" : A Modifier is badly defined"<<endl; continue; }
    Handle(Standard_Transient) item;   // a fournir ...
    ReadOwn(item);
    if (item.IsNull()) continue;
    DeclareAndCast(IFSelect_GeneralModifier,modif,item);
    if (modif.IsNull())
      { sout<<"Lineno."<<thenl<<" : A Modifier has not been Recognized"<<endl; continue; }
    AddItem(modif,Standard_False);  // active plus tard
  }

//  ... Transformers
  rubr = (theline.Length() == 1 && theline.Value(1).IsEqual("!TRANSFORMERS"));
  while (rubr) {
    if (!ReadLine()) return 1;
    if (theline.Value(1).Value(1) == '!') break;    // liste suivante
    if (theline.Length() < 2)
      { sout<<"Lineno."<<thenl<<" : A Transformer is badly defined"<<endl; continue; }
    Handle(Standard_Transient) item;   // a fournir ...
    ReadOwn(item);
    if (item.IsNull()) continue;
    DeclareAndCast(IFSelect_Transformer,trf,item);
    if (trf.IsNull())
      { sout<<"Lineno."<<thenl<<" : A Transformer has not been Recognized"<<endl; continue; }
    AddItem(trf,Standard_False);  // active plus tard
  }

//  ... Dispatches (ligne de garde deja lue)
  rubr = (theline.Length() == 1 && theline.Value(1).IsEqual("!DISPATCHES"));
  while (rubr) {
    if (!ReadLine()) return 1;
    if (theline.Value(1).Value(1) == '!') break;    // liste suivante
    if (theline.Length() < 3)
      { sout<<"Lineno."<<thenl<<" : A Dispatch is badly defined"<<endl; continue; }
    DeclareAndCast(IFSelect_Selection,input,ItemValue(3));
    SetLastGeneral(3);
    Handle(Standard_Transient) item;   // a fournir ...
    ReadOwn(item);
    if (item.IsNull()) continue;
    DeclareAndCast(IFSelect_Dispatch,disp,item);
    if (disp.IsNull())
      { sout<<"Lineno."<<thenl<<" : A Dispatch has not been Recognized"<<endl; continue; }
    AddItem(disp);
    thesess->SetItemSelection(disp,input);
  }

//  ... FileNaming (ligne de garde deja lue)
//  ..  Modifiers deja lus et charges
  rubr = (theline.Length() == 4 && theline.Value(1).IsEqual("!FILENAMING"));
  if (rubr) {
    if (!IsVoid(2)) thesess->SetFilePrefix      (TextValue(2).ToCString());
    if (!IsVoid(3)) thesess->SetDefaultFileRoot (TextValue(3).ToCString());
    if (!IsVoid(4)) thesess->SetFileExtension   (TextValue(4).ToCString());
  }
  while (rubr) {
    if (!ReadLine()) return 1;
    if (theline.Value(1).Value(1) == '!') break;    // liste suivante
    if (theline.Length() != 2)
      { sout<<"Lineno."<<thenl<<" : A File Root is badly defined"<<endl; continue; }
    DeclareAndCast(IFSelect_Dispatch,disp,ItemValue(1));
    thesess->SetFileRoot (disp,theline.Value(2).ToCString());
  }

//  ... Modifiers (ligne de garde deja lue)
//  ... Attention, deux listes (MODELMODIFIERS et FILEMODIFIERS)
  for (Standard_Integer formod = 1; formod >= 0; formod --) {
    rubr = (theline.Length() == 1 &&
	    ( (formod == 1 && theline.Value(1).IsEqual("!MODELMODIFIERS")) ||
	      (formod == 0 && theline.Value(1).IsEqual("!FILEMODIFIERS"))  ) );
//    if ( formod == 1 && ( theline.Length() != 1 ||
//			 !theline.Value(1).IsEqual("!MODELMODIFIERS")) )
//      { sout<<"Lineno."<<thenl<<" : Model Modifier List Incorrect"<<endl;  return 1; }
//    if ( formod == 0 && ( theline.Length() != 1 ||
//			 !theline.Value(1).IsEqual("!FILEMODIFIERS")) )
//      { sout<<"Lineno."<<thenl<<" : File Modifier List Incorrect"<<endl;  return 1; }
    while (rubr) {
      if (!ReadLine()) return 1;
      if (theline.Value(1).Value(1) == '!') break;    // liste suivante
      if (theline.Length() < 3)
	{ sout<<"Lineno."<<thenl<<" : A General Modifier is badly defined"<<endl; continue; }
      DeclareAndCast(IFSelect_GeneralModifier,modif,ItemValue(1));
      DeclareAndCast(IFSelect_Selection,input,ItemValue(2));
      DeclareAndCast(IFSelect_Dispatch,disp,ItemValue(3));
      if (modif.IsNull())
	{ sout<<"Lineno."<<thenl<<" : A General Modifier has not been Recognized"<<endl; continue; }
      thesess->SetItemSelection   (modif,input);
      if (!disp.IsNull()) thesess->SetAppliedModifier (modif,disp);
      else                thesess->SetAppliedModifier (modif,thesess->ShareOut());
    }
  }

//  ...  Conclusion : voir ReadEnd (separe)
  return 0;
}

    Standard_Integer  IFSelect_SessionFile::ReadEnd ()
{
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if ( theline.Length() != 2 ||
      !theline.Value(1).IsEqual("!XSTEP") ||
      !theline.Value(2).IsEqual("END"))
    { sout<<"End of File Incorrect, lineno"<<thenl<<endl;  return 1; }
  return 0;
}


    Standard_Boolean  IFSelect_SessionFile::ReadLine ()
{
  if (thenl >= thelist.Length()) return Standard_False;
  thenl ++;
  Standard_CString ligne = thelist.Value(thenl).ToCString();
//   Lignes vides ?
  if (ligne[0] == '\0') return ReadLine();
  SplitLine (ligne);
  return Standard_True;
}


    void  IFSelect_SessionFile::SplitLine (const Standard_CString line)
{
  char mot[80];
  theline.Clear();
  Standard_Integer nbc  = 0;
  Standard_Boolean word = (line[0] > ' ');
  for (Standard_Integer i = 0; line[i] != '\0'; i ++) {
    if (line[i] > ' ') {
      if (!word) {  nbc = 0; word = Standard_True;  }
      mot[nbc] = line[i];  nbc ++;
    } else {
      if (word) {
	word = Standard_False;
	mot[nbc] = '\0';
	theline.Append (TCollection_AsciiString(mot));
      }
      if (line[i] == '\0' || line[i] == '\n') break;
    }
  }
  thelastgen = 0;
}

    Standard_Boolean  IFSelect_SessionFile::ReadOwn
  (Handle(Standard_Transient)& item)
{
  Handle(Message_Messenger) sout = Message::DefaultMessenger();

  if (theline.Length() < 2) return Standard_False;
  const TCollection_AsciiString& type = theline.Value(2);
  if (thelastgen < 2) thelastgen = 2;        // mini : ident+type d abord
//  thelastgen = theline.Length();
//  for (Standard_Integer i = theline.Length(); i > 0; i --) {
//    if (theline.Value(i).Value(1) == ':') thelastgen = i - 1;
//  }
  Handle(IFSelect_SessionDumper) dumper = IFSelect_SessionDumper::First();
  while (!dumper.IsNull()) {
    if (dumper->ReadOwn(*this,type,item)) break;
    dumper = dumper->Next();
  }
  if (dumper.IsNull()) sout<<" -- Lineno."<<thenl<<" : an Item could not be read"<<endl;
  return (!dumper.IsNull());    // IsNull -> echec
}


    void  IFSelect_SessionFile::AddItem
  (const Handle(Standard_Transient)& item, const Standard_Boolean active)
{
  Handle(Message_Messenger) sout = Message::DefaultMessenger();

  const TCollection_AsciiString& name = theline.Value(1);
  Standard_Integer id = 0;
  if (!item.IsNull()) {
    if (name.Value(1) == '#') id = thesess->AddItem(item,active);
    else if (!thesess->NamedItem(name.ToCString()).IsNull()) id =
      thesess->AddItem(item,active);
    else id = thesess->AddNamedItem(name.ToCString(),item,active);
  }
  else sout<<"Lineno."<<thenl<<" -- Name : "<<name
    <<" : Item could not be defined" << endl;
  thenames.Bind(name,id);
}

    Standard_Boolean  IFSelect_SessionFile::IsDone () const
      {  return thedone;  }

    Handle(IFSelect_WorkSession)  IFSelect_SessionFile::WorkSession () const
      {  return thesess;  }

//  ########                Actions Unitaires d ECRITURE               ########

    void  IFSelect_SessionFile::NewItem
  (const Standard_Integer ident, const Handle(Standard_Transient)& par)
{
  char laligne[100];
  if (!thesess->HasName(par)) {
    thenewnum ++;  thenums->SetValue(ident,thenewnum);
    sprintf(laligne," #%d %s",thenewnum,par->DynamicType()->Name());
  }
  else  sprintf(laligne," %s %s",thesess->Name(par)->ToCString(),
		par->DynamicType()->Name());
  WriteLine(laligne);
}

    void  IFSelect_SessionFile::SetOwn (const Standard_Boolean mode)
      {  theownflag = mode;  }

    void  IFSelect_SessionFile::SendVoid ()
{
////  if (theownflag) WriteLine(" :$");
  WriteLine(" $");
}

    void  IFSelect_SessionFile::SendItem (const Handle(Standard_Transient)& par)
{
  Handle(Message_Messenger) sout = Message::DefaultMessenger();

  char laligne[100];
  Standard_Integer filenum = 0;
  Standard_Integer id = thesess->ItemIdent(par);
  if (id != 0) filenum = thenums->Value(id);
  if (filenum == 0) {
    if (!par.IsNull()) sout << "Lineno " << thenl << " --  Unknown Item : "
       << " Type:" << par->DynamicType()->Name() << endl;  //sout<<Handle par
    SendVoid();
    thedone = Standard_False;
    return;
  }
////  if (theownflag) WriteLine(" :");
////  else            WriteLine(" ");
  if (filenum < 0) sprintf(laligne," :%s",thesess->Name(par)->ToCString());
  else sprintf(laligne," #%d",filenum);
  WriteLine(laligne);
}

    void  IFSelect_SessionFile::SendText (const Standard_CString text)
{
  char laligne[100];
////  if (theownflag) sprintf(laligne," :%s",text);
  sprintf(laligne," %s",text);
  WriteLine(laligne);
}


//  ########                Actions Unitaires de LECTURE               ########

    void  IFSelect_SessionFile::SetLastGeneral (const Standard_Integer lastgen)
      {  thelastgen = lastgen;  }

    Standard_Integer  IFSelect_SessionFile::NbParams () const
      {  return theline.Length() - thelastgen;  }


    Standard_Boolean  IFSelect_SessionFile::IsVoid
  (const Standard_Integer num) const
{
  Standard_Integer nm = num + thelastgen;
  if (nm <= 0 || nm > theline.Length()) return Standard_True;
  const TCollection_AsciiString& term = theline.Value(nm);
  return (term.IsEqual ("$") || term.IsEqual (":$") );
}

    Standard_Boolean  IFSelect_SessionFile::IsText
  (const Standard_Integer num) const
{
  Standard_Integer nm = num + thelastgen;
  if (nm <= 0 || nm > theline.Length()) return Standard_False;
  const TCollection_AsciiString& term = theline.Value(nm);
  if (term.Value(1) == ':') return Standard_False;
  if (term.Value(1) == '#') return Standard_False;
  if (term.IsEqual("$"))    return Standard_False;
  return Standard_True;
}

    const TCollection_AsciiString&  IFSelect_SessionFile::ParamValue
  (const Standard_Integer num) const
      {  return theline.Value(num+thelastgen);  }


    TCollection_AsciiString  IFSelect_SessionFile::TextValue
  (const Standard_Integer num) const
{
  Standard_Integer nm = num + thelastgen;
  TCollection_AsciiString res;
  if (nm <= 0 || nm > theline.Length()) return res;
  res = theline.Value(nm);
  if (res.Value( res.Length() ) == '"') res.Remove(res.Length());
  if (res.Value(1) == ':') res.Remove(1);
  if (res.Value(1) == '"') res.Remove(1);
  return res;
}

    Handle(Standard_Transient)  IFSelect_SessionFile::ItemValue
  (const Standard_Integer num) const
{
  Handle(Message_Messenger) sout = Message::DefaultMessenger();

  Handle(Standard_Transient) res;
  Standard_Integer nm = num + thelastgen;
  if (nm <= 0 || nm > theline.Length()) return res;
  Standard_Integer id;
  TCollection_AsciiString name = theline.Value(nm);
  if (name.Value(1) == ':') name.Remove(1);
  if (name.IsEqual("$")) return res;    // item non-defini justement
  if (!thenames.Find(name, id)) {
    sout << " -- Item Unknown in File : " << name
      << " lineno " << thenl << " param." << nm << endl;
    id = 0;
  }
  return thesess->Item(id);
}


    void  IFSelect_SessionFile::Destroy ()
{   }  // agit si File non ferme, sinon ne fait rien
