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


#include <IFSelect_PacketList.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_ViewKindEntity.hxx>
#include <IGESSelect_ViewSorter.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSelect_ViewSorter,Standard_Transient)

#define PourDrawing 404


IGESSelect_ViewSorter::IGESSelect_ViewSorter  ()    {  }

    void  IGESSelect_ViewSorter::SetModel
  (const Handle(IGESData_IGESModel)& model)    {  themodel = model;  }


    void  IGESSelect_ViewSorter::Clear ()
{
  Standard_Integer nb = themodel->NbEntities();
  if (nb < 100) nb = 100;
  themap.Clear();      themap.ReSize (nb);
  theitems.Clear();    theitems.ReSize (nb);
  thefinals.Clear();   thefinals.ReSize (nb);
  theinditem.Clear();  theindfin.Clear();  // seq//
}


    Standard_Boolean  IGESSelect_ViewSorter::Add
  (const Handle(Standard_Transient)& ent)
{
  DeclareAndCast(IGESData_IGESEntity,igesent,ent);
  if (!igesent.IsNull()) return AddEntity (igesent);
  DeclareAndCast(TColStd_HSequenceOfTransient,list,ent);
  if (!list.IsNull())  {  AddList  (list);   return Standard_True;  }
  DeclareAndCast(Interface_InterfaceModel,model,ent);
  if (!model.IsNull()) {  AddModel (model);  return Standard_True;  }
  return Standard_False;
}

    Standard_Boolean  IGESSelect_ViewSorter::AddEntity
  (const Handle(IGESData_IGESEntity)& igesent)
{
//  Reception, controle de type et de map
  if (igesent.IsNull()) return Standard_False;
  if (themap.FindIndex(igesent)) return Standard_False;
  themap.Add(igesent);
//  Recuperation de la vue (attention au cas du Drawing)
  Handle(IGESData_IGESEntity) view;
  if (igesent->TypeNumber() == PourDrawing) view = igesent;  // DRAWING
  else {
    if (igesent->IsKind(STANDARD_TYPE(IGESData_ViewKindEntity))) view = igesent; // VIEW
    else view = igesent->View();
/*
    DeclareAndCast(IGESData_ViewKindEntity,trueview,view);
    if (!trueview.IsNull())
      if (trueview->IsSingle()) view.Nullify();  // Multiple -> Nulle
*/
  }
//  On enregistre
  Standard_Integer viewindex = 0;  // 0 sera pour remain
  if (!view.IsNull()) {
    viewindex = theitems.FindIndex(view);
    if (viewindex <= 0) viewindex = theitems.Add(view);
  }
  theinditem.Append(viewindex);
  theindfin.Append(0);
  return Standard_True;
}


    void  IGESSelect_ViewSorter::AddList
  (const Handle(TColStd_HSequenceOfTransient)& list)
{
  Standard_Integer nb = list->Length();
  for (Standard_Integer i = 1; i <= nb; i ++) Add (list->Value(i));
}

    void  IGESSelect_ViewSorter::AddModel
  (const Handle(Interface_InterfaceModel)& model)
{
  DeclareAndCast(IGESData_IGESModel,igesmod,model);
  if (igesmod.IsNull()) return;
  Standard_Integer nb = igesmod->NbEntities();
  for (Standard_Integer i = 1; i <= nb; i ++) AddEntity (igesmod->Entity(i));
}

    Standard_Integer  IGESSelect_ViewSorter::NbEntities () const
      {  return themap.Extent();  }

//  .....    Attention    .....

    void  IGESSelect_ViewSorter::SortSingleViews
  (const Standard_Boolean alsoframes)
{
// Du tas initial, on ecarte : les vues nulles, et selon alsoframe les drawings
// Vues nulles : cf theremain (remain initial reconduit)

//  Remarque : le filtre IsSingle a ete applique par Add
  thefinals.Clear();
  Standard_Integer nb = theinditem.Length();
  //Standard_Integer numit = 0; //szv#4:S4163:12Mar99 not needed
  for (Standard_Integer i = 1; i <= nb; i ++) {
    Standard_Integer numitem = theinditem.Value(i);
    Standard_Integer finalindex = 0;  // 0 sera pour remain
    if (numitem > 0) {
      //numit = numitem; //szv#4:S4163:12Mar99 not needed
      DeclareAndCast(IGESData_IGESEntity,item,theitems.FindKey(numitem));
      Standard_Boolean ok = Standard_False;
      if (alsoframes)  ok = (item->TypeNumber() == PourDrawing);
      if (!ok) {
	DeclareAndCast(IGESData_ViewKindEntity,view,item);
	if (!view.IsNull()) ok = view->IsSingle();
      }
      if (ok) {
	finalindex = thefinals.FindIndex(item);
	if (finalindex <= 0) finalindex = thefinals.Add(item);
      }
    }
    theindfin.SetValue(i,finalindex);
  }
}


    void  IGESSelect_ViewSorter::SortDrawings (const Interface_Graph& G)
{
// Pour chaque item (vue ou drawing), drawing contenant, silya (sinon tant pis)

  thefinals.Clear();
  Standard_Integer nb = theinditem.Length();
  //Standard_Integer numit = 0; //szv#4:S4163:12Mar99 not needed
  for (Standard_Integer i = 1; i <= nb; i ++) {
    Standard_Integer numitem = theinditem.Value(i);
    Standard_Integer finalindex = 0;  // 0 sera pour remain
    if (numitem > 0) {
      //numit = numitem; //szv#4:S4163:12Mar99 not needed
      DeclareAndCast(IGESData_IGESEntity,item,theitems.FindKey(numitem));
      if (item.IsNull()) continue;
//  Si cest un Drawing, il definit le Set. Sinon, chercher Drawing contenant
      Handle(Standard_Transient) drawing;
      if (item->TypeNumber() == PourDrawing) drawing = item;
      else {
	Interface_EntityIterator list = G.Sharings(item);
	for (list.Start(); list.More(); list.Next()) {
	  DeclareAndCast(IGESData_IGESEntity,draw,list.Value());
	  if (draw.IsNull()) continue;
	  if (draw->TypeNumber() == PourDrawing) drawing = draw;
	}
      }
      if (!drawing.IsNull()) {
	finalindex = thefinals.FindIndex(drawing);
	if (finalindex <= 0) finalindex = thefinals.Add(drawing);
      }
    }
    theindfin.SetValue(i,finalindex);
  }
}

//  ....    Queries    ....

    Standard_Integer  IGESSelect_ViewSorter::NbSets
  (const Standard_Boolean final) const
{
  if (final) return thefinals.Extent();
  else       return theitems.Extent();
}

    Handle(IGESData_IGESEntity)  IGESSelect_ViewSorter::SetItem
  (const Standard_Integer num, const Standard_Boolean final) const
{
  if (final) return GetCasted(IGESData_IGESEntity,thefinals.FindKey(num));
  else       return GetCasted(IGESData_IGESEntity,theitems.FindKey(num));
}

    Handle(IFSelect_PacketList)  IGESSelect_ViewSorter::Sets
  (const Standard_Boolean final) const
{
  Handle(IFSelect_PacketList) list = new IFSelect_PacketList(themodel);
  Standard_Integer i, nb;
  nb = (final ? theindfin.Length() : theinditem.Length());
  Standard_Integer nbs = NbSets(final);
  for (Standard_Integer num = 1; num <= nbs; num ++) {
    list->AddPacket();
    if (final) {
//    Attention a l unicite
      for (i = 1; i <= nb; i ++) {
	if (theindfin.Value(i) != num) continue;
	list->Add (themap.FindKey(i));
      }
    } else {
      for (i = 1; i <= nb; i ++) {
	if (theinditem.Value(i) != num) continue;
	list->Add (themap.FindKey(i));
      }
    }
  }
  return list;
}
