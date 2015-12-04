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


#include <IFSelect_ContextModif.hxx>
#include <IFSelect_Modifier.hxx>
#include <IFSelect_Selection.hxx>
#include <IFSelect_TransformStandard.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_CopyControl.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Protocol.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_HArray1OfInteger.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(IFSelect_TransformStandard,IFSelect_Transformer)

IFSelect_TransformStandard::IFSelect_TransformStandard ()
      {  thecopy = Standard_True;  }

    void  IFSelect_TransformStandard::SetCopyOption
  (const Standard_Boolean option)
      {  thecopy = option;  }

    Standard_Boolean  IFSelect_TransformStandard::CopyOption () const
      {  return thecopy;  }

    void  IFSelect_TransformStandard::SetSelection
  (const Handle(IFSelect_Selection)& sel)
      {  thesel = sel;  }

    Handle(IFSelect_Selection)  IFSelect_TransformStandard::Selection () const
      {  return thesel;  }

    Standard_Integer IFSelect_TransformStandard::NbModifiers () const
      {  return themodifs.Length();  }

    Handle(IFSelect_Modifier)  IFSelect_TransformStandard::Modifier
  (const Standard_Integer num) const
      {  return GetCasted(IFSelect_Modifier,themodifs.Value(num));  }

   Standard_Integer  IFSelect_TransformStandard::ModifierRank
  (const Handle(IFSelect_Modifier)& modif) const
{
  for (Standard_Integer i = themodifs.Length(); i >= 1; i --)
    if (modif == themodifs.Value(i)) return i;
  return 0;
}

    Standard_Boolean  IFSelect_TransformStandard::AddModifier
  (const Handle(IFSelect_Modifier)& modif,
   const Standard_Integer atnum)
{
  if (atnum < 0 || atnum > themodifs.Length()) return Standard_False;
  if (atnum == 0) themodifs.Append(modif);
  else  themodifs.InsertBefore(atnum,modif);
  return Standard_True;
}

    Standard_Boolean  IFSelect_TransformStandard::RemoveModifier
  (const Handle(IFSelect_Modifier)& modif)
{
  Standard_Integer num = ModifierRank(modif);
  return RemoveModifier(num);
}


    Standard_Boolean  IFSelect_TransformStandard::RemoveModifier
  (const Standard_Integer num)
{
  if (num <= 0 || num > themodifs.Length()) return Standard_False;
  themodifs.Remove(num);
  return Standard_True;
}

//  #################################################################
//  ########                     ACTION                      ########

    Standard_Boolean IFSelect_TransformStandard::Perform
  (const Interface_Graph& G, const Handle(Interface_Protocol)& protocol,
   Interface_CheckIterator& checks,
   Handle(Interface_InterfaceModel)& newmod)
{
  Interface_CopyTool TC(G.Model(),protocol);
  themap = TC.Control();
  Copy (G,TC,newmod);
  return ApplyModifiers (G,protocol,TC,checks,newmod);
}

    void  IFSelect_TransformStandard::Copy
  (const Interface_Graph& G, Interface_CopyTool& TC,
   Handle(Interface_InterfaceModel)& newmod) const
{
  if (CopyOption()) StandardCopy (G,TC,newmod);
  else              OnTheSpot    (G,TC,newmod);
}

    void  IFSelect_TransformStandard::StandardCopy
  (const Interface_Graph& G, Interface_CopyTool& TC,
   Handle(Interface_InterfaceModel)& newmod) const
{
  Handle(Interface_InterfaceModel) original = G.Model();
  newmod  = original->NewEmptyModel();
  TC.Clear();
  Standard_Integer nb = G.Size();
  Handle(TColStd_HArray1OfInteger) remain =
    new TColStd_HArray1OfInteger(0,nb+1);  remain->Init(0);
  for (Standard_Integer i = 1; i <= nb; i ++) {
//    if (G.Status(i) == 0) TC.TransferEntity (original->Value(i));
    TC.TransferEntity (original->Value(i));
  }
  TC.FillModel(newmod);
}

    void  IFSelect_TransformStandard::OnTheSpot
  (const Interface_Graph& G, Interface_CopyTool& TC,
   Handle(Interface_InterfaceModel)& newmod) const
{
  Standard_Integer nb = G.Size();
  for (Standard_Integer i = 1; i <= nb; i ++) TC.Bind(G.Entity(i),G.Entity(i));
  newmod = G.Model();
}


    Standard_Boolean  IFSelect_TransformStandard::ApplyModifiers
  (const Interface_Graph& G,  const Handle(Interface_Protocol)& protocol,
   Interface_CopyTool& TC,    Interface_CheckIterator& checks,
   Handle(Interface_InterfaceModel)& newmod) const
{
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  Standard_Boolean res = Standard_True;
  Standard_Boolean chg = Standard_False;
  Standard_Integer nb = NbModifiers();
  Handle(Interface_InterfaceModel) original = G.Model();

  for (Standard_Integer i = 1; i <= nb; i ++) {
    Handle(IFSelect_Modifier) unmod = Modifier(i);
    if (unmod->MayChangeGraph()) chg = Standard_True;

//    Appliquer ce Modifier (nb : le Dispatch, on s en moque)
//    D abord, la Selection
    IFSelect_ContextModif ctx (G,TC);
//    Ensuite, la Selection
//    S il y en a une ici, elle a priorite. Sinon, chaque Modifier a la sienne

    Handle(IFSelect_Selection) sel = thesel;
    if ( sel.IsNull())  sel = unmod->Selection();
    if (!sel.IsNull()) {
      Interface_EntityIterator entiter = sel->UniqueResult(G);
      ctx.Select (entiter);
    }
    if (ctx.IsForNone()) continue;
    unmod->Perform (ctx,newmod,protocol,TC);

//    Report des Erreurs
//    Faut-il les enregistrer dans newmod ? bonne question
    Interface_CheckIterator checklist = ctx.CheckList();
    if (!checklist.IsEmpty(Standard_False)) {
      checks.Merge(checklist);
      sout<<"IFSelect_TransformStandard :  Messages from Modifier n0 "<<i<<" of "<<nb<<endl;
      checklist.Print(sout,newmod,Standard_False);
    }
    if (!checklist.IsEmpty(Standard_True)) {
      sout<<" --  Abandon TransformStandard  --"<<endl;
      res = Standard_False;  break;
    }
  }

//   Modele pas modifie et Graphe pas modifie : le dire
  if (newmod == original && !chg) newmod.Nullify();
  return res;
}

    Standard_Boolean  IFSelect_TransformStandard::Updated
  (const Handle(Standard_Transient)& entfrom,
   Handle(Standard_Transient)& entto) const
{
  if (themap.IsNull()) return Standard_False;
  return themap->Search(entfrom,entto);
}


    TCollection_AsciiString  IFSelect_TransformStandard::Label () const
{
  char lab[30];
  TCollection_AsciiString labl("");
  if (CopyOption()) labl.AssignCat("Standard Copy");
  else              labl.AssignCat("On the spot Edition");
  Standard_Integer nb = NbModifiers();
  if (nb == 0) sprintf(lab," (no Modifier)");
  if (nb == 1) sprintf(lab," - %s",Modifier(1)->Label().ToCString());
  if (nb >  1) sprintf(lab," - %d Modifiers",nb);
  labl.AssignCat(lab);
  return labl;
}
