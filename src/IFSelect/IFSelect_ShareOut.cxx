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


#include <IFSelect_Dispatch.hxx>
#include <IFSelect_GeneralModifier.hxx>
#include <IFSelect_Modifier.hxx>
#include <IFSelect_ShareOut.hxx>
#include <Interface_InterfaceError.hxx>
#include <Interface_Macros.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(IFSelect_ShareOut,Standard_Transient)

IFSelect_ShareOut::IFSelect_ShareOut ()
{
  thedefrt  = new TCollection_HAsciiString ("Default");
  thenbdefs = thelastrun = 0;
}


    void  IFSelect_ShareOut::Clear (const Standard_Boolean onlydisp)
{
  thedisps.Clear();
  ClearResult(!onlydisp);
  if (onlydisp) return;
  themodelmodifiers.Clear();
  thefilemodifiers.Clear();
}

    Standard_Boolean  IFSelect_ShareOut::RemoveItem
  (const Handle(Standard_Transient)& item)
{
  DeclareAndCast(IFSelect_GeneralModifier,modifier,item);
  if (!modifier.IsNull()) {
    Standard_Boolean formodel = modifier->IsKind(STANDARD_TYPE(IFSelect_Modifier));
    Standard_Integer atnum = ModifierRank(modifier);
    return RemoveModifier (formodel,atnum);
  }
  DeclareAndCast(IFSelect_Dispatch,disp,item);
  if (!disp.IsNull()) {
    Standard_Integer atnum = DispatchRank(disp);
    return RemoveDispatch(atnum);
  }
  return Standard_False;
}


    void  IFSelect_ShareOut::ClearResult (const Standard_Boolean alsoname)
{
  thelastrun = 0;
  if (alsoname) thenbdefs = 0;
}

    Standard_Integer  IFSelect_ShareOut::LastRun () const
      {  return thelastrun;  }

    void  IFSelect_ShareOut::SetLastRun (const Standard_Integer lastrun)
      {  thelastrun = lastrun;  }

//  #######################################################################
//  ####                DISPATCHES (ENVOI DES FICHIERS)                ####

    Standard_Integer  IFSelect_ShareOut::NbDispatches () const 
      {  return thedisps.Length();  }

    Standard_Integer  IFSelect_ShareOut::DispatchRank
  (const Handle(IFSelect_Dispatch)& disp) const 
{
  if (disp.IsNull()) return 0;
  for (Standard_Integer i = thedisps.Length(); i >= 1; i --)
    if (disp == thedisps.Value(i)) return i;
  return 0;
}

    const Handle(IFSelect_Dispatch)&  IFSelect_ShareOut::Dispatch
  (const Standard_Integer num) const 
{
  return thedisps.Value(num);
}

    void  IFSelect_ShareOut::AddDispatch
  (const Handle(IFSelect_Dispatch)& disp)
{
  if (disp.IsNull()) return;
  thedisps.Append(disp);
}


    Standard_Boolean  IFSelect_ShareOut::RemoveDispatch
  (const Standard_Integer rank)
{
  if (rank <= thelastrun || rank > thedisps.Length()) return Standard_False;
  thedisps.Remove(rank);
  return Standard_True;
}

//  ##########################################################################
//  ####                            MODIFIERS                             ####

    void  IFSelect_ShareOut::AddModifier
  (const Handle(IFSelect_GeneralModifier)& modifier,
   const Standard_Integer atnum)
{
  Standard_Boolean formodel = modifier->IsKind(STANDARD_TYPE(IFSelect_Modifier));
  if (ModifierRank(modifier) == 0)  AddModif (modifier,formodel,atnum);
  Handle(IFSelect_Dispatch) nuldisp;
  modifier->SetDispatch(nuldisp);
}

    void  IFSelect_ShareOut::AddModifier
  (const Handle(IFSelect_GeneralModifier)& modifier,
   const Standard_Integer dispnum, const Standard_Integer atnum)
{
  Standard_Boolean formodel = modifier->IsKind(STANDARD_TYPE(IFSelect_Modifier));
  if (ModifierRank(modifier) == 0)  AddModif (modifier,formodel,atnum);
  Handle(IFSelect_Dispatch) disp = Dispatch(dispnum);
  modifier->SetDispatch(disp);
}


    void  IFSelect_ShareOut::AddModif
  (const Handle(IFSelect_GeneralModifier)& modifier,
   const Standard_Boolean formodel, const Standard_Integer atnum)
{
  if (formodel) {
    if (atnum > 0 && atnum <= themodelmodifiers.Length())
      themodelmodifiers.InsertBefore(atnum,modifier);
    else themodelmodifiers.Append(modifier);
  } else {
    if (atnum > 0 && atnum <= thefilemodifiers.Length())
      thefilemodifiers.InsertBefore(atnum,modifier);
    else thefilemodifiers.Append(modifier);
  }
}

    Standard_Integer  IFSelect_ShareOut::NbModifiers
  (const Standard_Boolean formodel) const 
{
  if (formodel) return themodelmodifiers.Length();
  else          return thefilemodifiers.Length();
}

    Handle(IFSelect_GeneralModifier)  IFSelect_ShareOut::GeneralModifier
  (const Standard_Boolean formodel, const Standard_Integer atnum) const
{
  if (formodel) return themodelmodifiers.Value(atnum);
  else          return thefilemodifiers.Value(atnum);
}

    Handle(IFSelect_Modifier)  IFSelect_ShareOut::ModelModifier
  (const Standard_Integer num) const 
{  return Handle(IFSelect_Modifier)::DownCast(themodelmodifiers.Value(num));  }

    Standard_Integer  IFSelect_ShareOut::ModifierRank
  (const Handle(IFSelect_GeneralModifier)& modifier) const 
{
  Standard_Integer i;
  Standard_Boolean formodel = modifier->IsKind(STANDARD_TYPE(IFSelect_Modifier));
  if (formodel) {
    for (i = themodelmodifiers.Length(); i >= 1; i --)
      if (modifier == themodelmodifiers.Value(i)) return i;
  } else {
    for (i = thefilemodifiers.Length(); i >= 1; i --)
      if (modifier == thefilemodifiers.Value(i)) return i;
  }
  return 0;
}


    Standard_Boolean  IFSelect_ShareOut::RemoveModifier
  (const Standard_Boolean formodel, const Standard_Integer atnum)
{
  if (atnum <= 0) return Standard_False;
  if (formodel) {
    if (atnum > themodelmodifiers.Length()) return Standard_False;
    themodelmodifiers.Remove(atnum);
  } else {
    if (atnum > thefilemodifiers.Length()) return Standard_False;
    thefilemodifiers.Remove(atnum);
  }
  return Standard_True;
}


//    ChangeModifierRank revient a une permutation circulaire :
//    before est mis en after, ceux qui sont entre tournent
    Standard_Boolean    IFSelect_ShareOut::ChangeModifierRank
  (const Standard_Boolean formodel,
   const Standard_Integer before,   const Standard_Integer after)
{
  Standard_Integer nb;
  if (before <= 0 || after <= 0) return Standard_False;
  if (before == after) return Standard_True;
  if (formodel) {
    nb = themodelmodifiers.Length();
    if (before > nb || after > nb) return Standard_False;
    Handle(IFSelect_GeneralModifier) bef = themodelmodifiers.Value(before);
    themodelmodifiers.Remove(before);
    if (after == nb) themodelmodifiers.Append(bef);
    else             themodelmodifiers.InsertBefore(after,bef);
  } else {
    nb = thefilemodifiers.Length();
    if (before > nb || after > nb) return Standard_False;
    Handle(IFSelect_GeneralModifier) bef = thefilemodifiers.Value(before);
    thefilemodifiers.Remove(before);
    if (after == nb) thefilemodifiers.Append(bef);
    else             thefilemodifiers.InsertBefore(after,bef);
  }
  return Standard_True;
}

//  #######################################################################
//  ####                    NOMINATION DES FICHIERS                    ####
//  Rq : thenbdefs s applique tant que l on ne change pas les termes principaux

    Standard_Boolean  IFSelect_ShareOut::SetRootName
  (const Standard_Integer num, const Handle(TCollection_HAsciiString)& name)
{
  if (num < 1 || num > thedisps.Length()) return Standard_False;
  if (RootNumber(name) != 0) return Standard_False;
  Dispatch(num)->SetRootName (name);
  return Standard_True;
}

    Standard_Boolean  IFSelect_ShareOut::HasRootName
  (const Standard_Integer num) const
{
  if (num < 1 || num > thedisps.Length()) return Standard_False;
  return Dispatch(num)->HasRootName();
}

    Handle(TCollection_HAsciiString)  IFSelect_ShareOut::RootName
  (const Standard_Integer num) const
{
  Handle(TCollection_HAsciiString) nulname;
  if (num < 1 || num > thedisps.Length()) return nulname;
  return Dispatch(num)->RootName();
}

    Standard_Integer  IFSelect_ShareOut::RootNumber
  (const Handle(TCollection_HAsciiString)& name) const
{
  if (name.IsNull()) return 0;
  if (!thedefrt.IsNull()) {
    if (thedefrt->IsSameString(name)) return -1;
  }
  for (Standard_Integer i = 1; i <= thedisps.Length(); i ++) {
    Handle(TCollection_HAsciiString) root = thedisps.Value(i)->RootName();
    if (root.IsNull()) continue;
    if (root->IsSameString(name)) return i;
  }
  return 0;
}


    void  IFSelect_ShareOut::SetPrefix
  (const Handle(TCollection_HAsciiString)& pref)
      {  thepref = pref;  thenbdefs = 0;  }

    Standard_Boolean  IFSelect_ShareOut::SetDefaultRootName
  (const Handle(TCollection_HAsciiString)& defrt)
{
  if (RootNumber(defrt) != 0) return Standard_False;
  if (thedefrt.IsNull() || !thedefrt->IsSameString(defrt)) thenbdefs = 0;
  thedefrt = defrt;
  return Standard_True;
}

    void  IFSelect_ShareOut::SetExtension
  (const Handle(TCollection_HAsciiString)& ext)
      {  theext = ext;  thenbdefs = 0;  }

    Handle(TCollection_HAsciiString)  IFSelect_ShareOut::Prefix () const 
{
  if (thepref.IsNull()) return new TCollection_HAsciiString("");
  return thepref;
}

    Handle(TCollection_HAsciiString)  IFSelect_ShareOut::DefaultRootName () const
{
  if (thedefrt.IsNull()) return new TCollection_HAsciiString("");
  return thedefrt;
}

    Handle(TCollection_HAsciiString)  IFSelect_ShareOut::Extension () const 
{
  if (theext.IsNull()) return new TCollection_HAsciiString("");
  return theext;
}


    TCollection_AsciiString  IFSelect_ShareOut::FileName
  (const Standard_Integer dnum, const Standard_Integer pnum,
   const Standard_Integer nbpack)
{
  Handle(TCollection_HAsciiString) rot = RootName(dnum);
  Standard_Integer num  = pnum;
  Standard_Integer npac = nbpack;
  Standard_Boolean sufnum = (npac > 1 || num > 1);
  if (rot.IsNull()) {
    rot = thedefrt;
    thenbdefs ++;  num = thenbdefs;
    npac = 0;
    sufnum = Standard_True; // numeroter sur noms par defaut, des le 1er sans 0
  }

  TCollection_AsciiString res;
  if (!thepref.IsNull()) res.AssignCat (thepref->ToCString());
  if (!rot.IsNull())     res.AssignCat (rot->ToCString());

//  Suffixe numerique
  if (sufnum) {    // sinon, pas de suffixe numerique
//  Nom du PacketSuffix : _ suivi du numero <num>
//  Si nbpack non nul, alors on a un majorant et on peut preceder de zeros
//  Ex.: nbpack = 50 (donc 2 chiffres), num = 3, cela donnera _03
//  idnum pas utilise : cette methode peut etre redefinie et utiliser idnum ...
//  Si nbpack = 0 ou 1, num = 1 pas de suffixe, sinon suffixe "_num" tel quel
//  MODIF du 3-NOV-1995 -> pour eviter toute confusion, num = 1 donne aussi _1
    Standard_Integer nbch = 0;
    char format[10],suffixe[30];  format[1] = ' ';
    if (npac >= num) {
      Standard_Integer nbpa = 1;
      while (nbpa <= npac)  {  nbpa *= 10; nbch ++;  }
    }
    if (nbch > 1) {
      sprintf(format,"_ %d.%dd",nbch,nbch);
      format[1] = '%';
    } else if (npac >= num || num >= 1) {
      sprintf(format,"_ d");
      format[1] = '%';
    }
    if (format[1] == '%') {
      sprintf (suffixe,format,num);
      res.AssignCat (suffixe);
    }
  }

  if (!theext.IsNull())  res.AssignCat (theext->ToCString());
  return res;
}
