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
#include <IGESBasic_Group.hxx>
#include <IGESBasic_GroupWithoutBackP.hxx>
#include <IGESBasic_OrderedGroup.hxx>
#include <IGESBasic_OrderedGroupWithoutBackP.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESSelect_RebuildGroups.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_Array1OfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSelect_RebuildGroups,IGESSelect_ModelModifier)

IGESSelect_RebuildGroups::IGESSelect_RebuildGroups ()
    : IGESSelect_ModelModifier (Standard_True)    {  }

    void  IGESSelect_RebuildGroups::Performing
  (IFSelect_ContextModif& ctx,
   const Handle(IGESData_IGESModel)& target,
   Interface_CopyTool& TC) const
{
//  On reconstruit les groupes qui peuvent l etre
//  Pour chaque groupe de l original, on regarde les composants transferes
//   (evt filtres par <ctx>)
//  Ensuite, silyena plus d une, on refait un nouveau groupe
  DeclareAndCast(IGESData_IGESModel,original,ctx.OriginalModel());
  Standard_Integer nbo = original->NbEntities();

//  Entites a prendre en compte pour la reconstruction
//  NB : Les groupes deja transferes ne sont bien sur pas reconstruits !
  TColStd_Array1OfInteger pris(0,nbo); pris.Init(0);
  for (ctx.Start(); ctx.More(); ctx.Next()) {
    pris.SetValue (original->Number(ctx.ValueOriginal()),1);
  }

  for (Standard_Integer i = 1; i <= nbo; i ++) {
    Handle(IGESData_IGESEntity) ent = original->Entity(i);
    if (ent->TypeNumber() != 402) continue;
    Standard_Integer casenum = 0;
    Handle(Standard_Transient) newent;
    Interface_EntityIterator newlist;
    if (TC.Search(ent,newent)) continue;    // deja passe
    if (ent->IsKind(STANDARD_TYPE(IGESBasic_Group))) {
      DeclareAndCast(IGESBasic_Group,g,ent);
      casenum = 1;
      Standard_Integer nbg = g->NbEntities();
      for (Standard_Integer ig = 1; ig <= nbg; ig ++) {
	if (TC.Search(g->Value(i),newent)) newlist.GetOneItem(newent);
      }
    }
    if (ent->IsKind(STANDARD_TYPE(IGESBasic_GroupWithoutBackP))) {
      DeclareAndCast(IGESBasic_GroupWithoutBackP,g,ent);
      casenum = 2;
      Standard_Integer nbg = g->NbEntities();
      for (Standard_Integer ig = 1; ig <= nbg; ig ++) {
	if (TC.Search(g->Value(i),newent)) newlist.GetOneItem(newent);
      }
    }
    if (ent->IsKind(STANDARD_TYPE(IGESBasic_OrderedGroup))) {
      DeclareAndCast(IGESBasic_OrderedGroup,g,ent);
      casenum = 3;
      Standard_Integer nbg = g->NbEntities();
      for (Standard_Integer ig = 1; ig <= nbg; ig ++) {
	if (TC.Search(g->Value(i),newent)) newlist.GetOneItem(newent);
      }
    }
    if (ent->IsKind(STANDARD_TYPE(IGESBasic_OrderedGroupWithoutBackP))) {
      DeclareAndCast(IGESBasic_OrderedGroupWithoutBackP,g,ent);
      casenum = 4;
      Standard_Integer nbg = g->NbEntities();
      for (Standard_Integer ig = 1; ig <= nbg; ig ++) {
	if (TC.Search(g->Value(i),newent)) newlist.GetOneItem(newent);
      }
    }
//  A present, reconstruire sil le faut
    if (newlist.NbEntities() <= 1) continue;   // 0 ou 1 : rien a refaire
    Handle(IGESData_HArray1OfIGESEntity) tab =
      new IGESData_HArray1OfIGESEntity(1,newlist.NbEntities());
    Standard_Integer ng = 0;
    for (newlist.Start(); newlist.More(); newlist.Next()) {
      ng ++;  tab->SetValue(ng,GetCasted(IGESData_IGESEntity,newlist.Value()));
    }
    switch (casenum) {
      case 1 : {
	Handle(IGESBasic_Group) g = new IGESBasic_Group;
	g->Init(tab);
	target->AddEntity(g);

//  Q : faut-il transferer le nom silyena un ?
      }
	break;
      case 2 : {
	Handle(IGESBasic_GroupWithoutBackP) g = new IGESBasic_GroupWithoutBackP;
	g->Init(tab);
	target->AddEntity(g);
      }
	break;
      case 3 : {
	Handle(IGESBasic_OrderedGroup) g = new IGESBasic_OrderedGroup;
	g->Init(tab);
	target->AddEntity(g);
      }
	break;
      case 4 : {
	Handle(IGESBasic_OrderedGroupWithoutBackP) g =
	  new IGESBasic_OrderedGroupWithoutBackP;
	g->Init(tab);
	target->AddEntity(g);
      }
	break;
      default : break;
    }
  }
}

    TCollection_AsciiString  IGESSelect_RebuildGroups::Label () const
{  return TCollection_AsciiString("Rebuild Groups");  }
