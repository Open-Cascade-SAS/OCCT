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


#include <IFSelect_Selection.hxx>
#include <IFSelect_Signature.hxx>
#include <IFSelect_SignCounter.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_SignCounter,IFSelect_SignatureList)

IFSelect_SignCounter::IFSelect_SignCounter
  (const Standard_Boolean withmap, const Standard_Boolean withlist)
    : IFSelect_SignatureList (withlist)
{
  themapstat = withmap;  thenbcomp1 = thenbcomp2 = theselmode = 0;
}

    IFSelect_SignCounter::IFSelect_SignCounter
  (const Handle(IFSelect_Signature)& matcher,
   const Standard_Boolean withmap, const Standard_Boolean withlist)
    : IFSelect_SignatureList (withlist) , thematcher (matcher)
{
  themapstat = withmap;  thenbcomp1 = thenbcomp2 = theselmode = 0;
  TCollection_AsciiString sign = thematcher->Name();
  SetName (sign.ToCString());
}

    Handle(IFSelect_Signature)  IFSelect_SignCounter::Signature () const
      {  return thematcher;  }

    void  IFSelect_SignCounter::SetMap  (const Standard_Boolean withmap)
      {  themapstat = withmap;  }

    Standard_Boolean  IFSelect_SignCounter::AddEntity
  (const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& model)
{
  if (themapstat && !ent.IsNull()) {
    if (themap.Contains(ent)) return Standard_False;
    themap.Add(ent);
  }
  AddSign (ent,model);
  return Standard_True;
}

    void  IFSelect_SignCounter::AddSign
  (const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& model)
{
  char nulsign[2];
  nulsign[0] = '\0';
  if (ent.IsNull() || thematcher.IsNull())
    Add (ent, nulsign);  // pour compter les Nuls
  else              Add (ent, thematcher->Value(ent,model) );
}


    void  IFSelect_SignCounter::AddList
  (const Handle(TColStd_HSequenceOfTransient)& list,
   const Handle(Interface_InterfaceModel)& model)
{
  if (list.IsNull()) return;
  Standard_Integer nb = list->Length();
  for (Standard_Integer i = 1; i <= nb; i ++) AddEntity(list->Value(i),model);
}

    void  IFSelect_SignCounter::AddWithGraph
  (const Handle(TColStd_HSequenceOfTransient)& list,
   const Interface_Graph& graph)
      {  AddList (list,graph.Model());  }


    void  IFSelect_SignCounter::AddModel
  (const Handle(Interface_InterfaceModel)& model)
{
  if (model.IsNull()) return;
  Standard_Integer nb = model->NbEntities();
//  Si on part de vide, on sait que chque entite est unique dans le modele
  Standard_Boolean mapstat = themapstat;
  if (themap.Extent() == 0) themapstat = Standard_False;
  for (Standard_Integer i = 1; i <= nb; i ++) AddEntity(model->Value(i),model);
  themapstat = mapstat;
}

    void  IFSelect_SignCounter::AddFromSelection
  (const Handle(IFSelect_Selection)& sel, const Interface_Graph& G)
{
  Interface_EntityIterator iter = sel->RootResult(G);
  AddWithGraph (iter.Content(),G);
}

//  #############    SELECTION    ##############

    void  IFSelect_SignCounter::SetSelection
  (const Handle(IFSelect_Selection)& sel)
{  theselect = sel;  SetSelMode(-1);  SetSelMode (sel.IsNull() ? 0 : 2);  }

    Handle(IFSelect_Selection)  IFSelect_SignCounter::Selection () const
      {  return theselect;  }

    void  IFSelect_SignCounter::SetSelMode (const Standard_Integer selmode)
{
  if (selmode < 0) thenbcomp1 = thenbcomp2 = 0;
  else theselmode = selmode;
  if (selmode == 0) theselect.Nullify();
}

    Standard_Integer  IFSelect_SignCounter::SelMode () const
      {  return theselmode;  }

    Standard_Boolean  IFSelect_SignCounter::ComputeSelected
  (const Interface_Graph& G, const Standard_Boolean forced)
{
  if (theselmode < 2 || theselect.IsNull()) return Standard_False;
  Standard_Boolean afaire = forced;
  Interface_EntityIterator iter = theselect->RootResult(G);
  Standard_Integer nb1 = G.Size();
  Standard_Integer nb2 = iter.NbEntities();
  if (!afaire) afaire = (nb1 != thenbcomp1 || nb2 != thenbcomp2);
  thenbcomp1 = nb1;  thenbcomp2 = nb2;
  if (afaire) AddWithGraph (iter.Content(),G);
  return Standard_True;
}


    Handle(TCollection_HAsciiString)  IFSelect_SignCounter::Sign
  (const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& model) const
{
  Handle(TCollection_HAsciiString) res;
  if (ent.IsNull() || thematcher.IsNull()) return res;
  res = new TCollection_HAsciiString (thematcher->Value(ent,model));
  return res;
}

    Standard_CString  IFSelect_SignCounter::ComputedSign
  (const Handle(Standard_Transient)& ent,
   const Interface_Graph& G)
{
  Handle(TColStd_HSequenceOfTransient) list = new TColStd_HSequenceOfTransient();
  list->Append (ent);
  ModeSignOnly() = Standard_True;
  AddWithGraph (list,G);
  Standard_CString val = LastValue();
  ModeSignOnly() = Standard_False;
  return val;
}
