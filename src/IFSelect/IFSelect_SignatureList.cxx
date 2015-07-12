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


#include <Dico_DictionaryOfInteger.hxx>
#include <Dico_DictionaryOfTransient.hxx>
#include <Dico_IteratorOfDictionaryOfInteger.hxx>
#include <Dico_IteratorOfDictionaryOfTransient.hxx>
#include <IFSelect_SignatureList.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Interface_MSG.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HSequenceOfTransient.hxx>

IFSelect_SignatureList::IFSelect_SignatureList
  (const Standard_Boolean withlist)
{
  thesignonly = Standard_False;
  thelistat = withlist;
  thenbnuls = 0;
  thedicount = new Dico_DictionaryOfInteger;
  thediclist = new Dico_DictionaryOfTransient;
  SetName("...");
}

    void IFSelect_SignatureList::SetList
  (const Standard_Boolean withlist)
      {  thelistat = withlist;  }

    Standard_Boolean&  IFSelect_SignatureList::ModeSignOnly ()
      {  return thesignonly;  }

    void IFSelect_SignatureList::Clear ()
{
  thelastval.Clear();
  thenbnuls = 0;
  thedicount = new Dico_DictionaryOfInteger;
  thediclist = new Dico_DictionaryOfTransient;
}

    void IFSelect_SignatureList::Add
  (const Handle(Standard_Transient)& ent, const Standard_CString sign)
{
  if (thesignonly) {
    thelastval.Clear();
    thelastval.AssignCat (sign);
    return;
  }

  if (sign[0] == '\0') {  thenbnuls ++;  return;  }

  Standard_Boolean deja;
  Standard_Integer& nb = thedicount->NewItem(sign,deja);
  if (!deja) nb = 0;
  nb ++;

  if (thelistat) {
    Handle(Standard_Transient)& anitem = thediclist->NewItem(sign,deja);
    DeclareAndCast(TColStd_HSequenceOfTransient,alist,anitem);
    if (!deja) { alist = new TColStd_HSequenceOfTransient(); anitem = alist;  }
    alist->Append(ent);
  }
}

    Standard_CString  IFSelect_SignatureList::LastValue () const
      {  return thelastval.ToCString();  }

    void IFSelect_SignatureList::Init
  (const Standard_CString name,
   const Handle(Dico_DictionaryOfInteger)&   theCount,
   const Handle(Dico_DictionaryOfTransient)& list,
   const Standard_Integer nbnuls)
{
  thelastval.Clear();
  thename    = new TCollection_HAsciiString (name);
  thedicount = theCount;
  thediclist = list;
  thenbnuls  = nbnuls;
  if (thediclist.IsNull()) thelistat = Standard_False;
}


    Handle(TColStd_HSequenceOfHAsciiString)  IFSelect_SignatureList::List
  (const Standard_CString root) const
{
  Handle(TColStd_HSequenceOfHAsciiString) list =
    new TColStd_HSequenceOfHAsciiString();
  Dico_IteratorOfDictionaryOfInteger iter(thedicount,root);
  for (; iter.More(); iter.Next()) {
    Handle(TCollection_HAsciiString) sign =
      new TCollection_HAsciiString (iter.Name());
    list->Append(sign);
  }
  return list;
}


    Standard_Boolean  IFSelect_SignatureList::HasEntities () const
      {  return thelistat;  }

    Standard_Integer  IFSelect_SignatureList::NbNulls () const
      {  return thenbnuls;  }

    Standard_Integer  IFSelect_SignatureList::NbTimes
  (const Standard_CString sign) const
{
  Standard_Integer nb;
  if (thedicount->GetItem(sign,nb)) return nb;
  else return 0;
}

    Handle(TColStd_HSequenceOfTransient)  IFSelect_SignatureList::Entities
  (const Standard_CString sign) const
{
  Handle(TColStd_HSequenceOfTransient) list;
  if (!thelistat) return list;
  if (thediclist->GetItem(sign,list)) return list;
  list = new TColStd_HSequenceOfTransient();
  return list;
}


    void IFSelect_SignatureList::SetName (const Standard_CString name)
      {  thename    = new TCollection_HAsciiString (name);  }

    Standard_CString  IFSelect_SignatureList::Name () const
      {  return thename->ToCString();  }


    void  IFSelect_SignatureList::PrintCount (const Handle(Message_Messenger)& S) const
{
  Standard_Integer nbtot = 0, nbsign = 0;
  Dico_IteratorOfDictionaryOfInteger iter(thedicount,"");
  S << " Count	"<<thename->ToCString()<<"\n -----	-----------"<<endl;
  for (; iter.More(); iter.Next()) {
    Standard_Integer val = iter.Value();
    S << Interface_MSG::Blanks(val,6) << val <<"	"<<iter.Name()<<endl;
    nbtot += val;
    nbsign ++;
  }
  if (thenbnuls > 0) S << thename->ToCString()<< " Nul : " << thenbnuls<<endl;
  S<<"    Nb Total:"<<nbtot<<"  for "<<nbsign<<" items"<<endl;
}

    void  IFSelect_SignatureList::PrintList
  (const Handle(Message_Messenger)& S, const Handle(Interface_InterfaceModel)& model,
   const IFSelect_PrintCount mod) const
{
  if (mod == IFSelect_ItemsByEntity) return;
  if (mod == IFSelect_CountByItem)   {  PrintCount (S);  return;  }
  if (mod == IFSelect_CountSummary)  {  PrintSum   (S);  return;  }
  if (!HasEntities()) {
    S <<" SignatureList "<<Name()<<" : PrintList, list not available"<<endl;
    PrintCount(S);
    return;
  }
  Standard_Integer nbtot = 0, nbsign = 0;
  Dico_IteratorOfDictionaryOfTransient iter(thediclist,"");
  for (; iter.More(); iter.Next()) {
    DeclareAndCast(TColStd_HSequenceOfTransient,list,iter.Value());
    S<<Name()<<" : "<<iter.Name()<<endl;
    if (list.IsNull())  {  S<<"  - (empty list)"<<endl; continue;  }
    Standard_Integer nb = list->Length();
    S<<"  - Nb: "<<nb<<" : ";
    Standard_Integer nc = nb;  if (nb > 5 && mod == IFSelect_ShortByItem) nc = 5;
    for (Standard_Integer i = 1; i <= nc; i ++) {
      if (list->Value(i).IsNull()) {
	S<<"  0";  if (mod == IFSelect_EntitiesByItem) S<<":(Global)";
	continue;
      }
      Standard_Integer num = model->Number(list->Value(i));
      if (num == IFSelect_ShortByItem)  {  S<<"  ??";  continue;  }
      S<<"  "<<num;
      if (mod == IFSelect_EntitiesByItem)
	{  S<<":";  model->PrintLabel(list->Value(i),S);  }
    }
    if (nc < nb) S<<"  .. etc";
    S<<endl;
    nbtot += nb;
    nbsign ++;
  }
  S<<" Nb Total:"<<nbtot<<"  for "<<nbsign<<" items"<<endl;
}


    void  IFSelect_SignatureList::PrintSum (const Handle(Message_Messenger)& S) const
{
  Dico_IteratorOfDictionaryOfInteger iter(thedicount,"");
  S << " Summary "<<thename->ToCString()<<"\n -----	-----------"<<endl;
  Standard_Integer nbtot = 0, nbsign = 0, maxent = 0, nbval = 0, nbve = 0, minval = 0, maxval = 0, totval = 0;
  for (; iter.More(); iter.Next()) {
    Standard_Integer nbent = iter.Value();
    nbtot += nbent;
    nbsign ++;
    if (nbent > maxent) maxent = nbent;
    TCollection_AsciiString name = iter.Name();
//    if (!name.IsIntegerValue()) continue;  pas bien fiable
    Standard_Integer ic, nc = name.Length();
    Standard_Boolean iaint = Standard_True;
    for (ic = 1; ic <= nc; ic ++) {
      char unc = name.Value(ic);
      if (ic == 1 && (unc == ' ' || unc == '+' || unc == '-')) continue;
      if (unc >= '0' && unc <= '9') continue;
      iaint = Standard_False;    break;
    }
    if (!iaint) continue;
    Standard_Integer val = name.IntegerValue();
    if (nbval == 0) { minval = maxval = val; }
    if (minval > val) minval = val;
    if (maxval < val) maxval = val;
    nbval ++;
    nbve += nbent;
    totval += (val*nbent);
  }
  S << "    Nb Total:"<<nbtot<<"  for "<<nbsign<<" items"<<endl;
  S << "    Highest count of entities : "<<maxent<<" on one item"<<endl;
  if (nbval > 0) {
    S<<"    Summary on Integer Values"<<endl;
    S<<"    Nb Integer Items : "<<nbval<<endl;
    S<<"    For Nb Entities  : "<<nbve<<endl;
    S<<"    Cumulated Values : "<<totval<<endl;
    S<<"    Maximum Value    : "<<maxval<<endl;
    Standard_Integer avg1, avg2;
    avg1 = totval/nbve;
    avg2 = ((totval - (avg1*nbve)) * 10) / nbve;
    S<<"    Average Value    : "<<avg1<<" "<<avg2<<"/10"<<endl;
    S<<"    Minimum Value    : "<<minval<<endl;
  }
}
