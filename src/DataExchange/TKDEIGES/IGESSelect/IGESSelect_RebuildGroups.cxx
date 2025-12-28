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
#include <IGESBasic_GroupWithoutBackP.hxx>
#include <IGESBasic_OrderedGroup.hxx>
#include <IGESBasic_OrderedGroupWithoutBackP.hxx>
#include <IGESData_IGESEntity.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESSelect_RebuildGroups.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <MoniTool_Macros.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSelect_RebuildGroups, IGESSelect_ModelModifier)

IGESSelect_RebuildGroups::IGESSelect_RebuildGroups()
    : IGESSelect_ModelModifier(true)
{
}

void IGESSelect_RebuildGroups::Performing(IFSelect_ContextModif&            ctx,
                                          const occ::handle<IGESData_IGESModel>& target,
                                          Interface_CopyTool&               TC) const
{
  //  We rebuild the groups that can be
  //  For each group of the original, we look at the transferred components
  //   (possibly filtered by <ctx>)
  //  Then, if there are more than one, we create a new group
  DeclareAndCast(IGESData_IGESModel, original, ctx.OriginalModel());
  int nbo = original->NbEntities();

  //  Entities to consider for reconstruction
  //  NB : Les groupes deja transferes ne sont bien sur pas reconstruits !
  NCollection_Array1<int> pris(0, nbo);
  pris.Init(0);
  for (ctx.Start(); ctx.More(); ctx.Next())
  {
    pris.SetValue(original->Number(ctx.ValueOriginal()), 1);
  }

  for (int i = 1; i <= nbo; i++)
  {
    occ::handle<IGESData_IGESEntity> ent = original->Entity(i);
    if (ent->TypeNumber() != 402)
      continue;
    int           casenum = 0;
    occ::handle<Standard_Transient> newent;
    Interface_EntityIterator   newlist;
    if (TC.Search(ent, newent))
      continue; // deja passe
    if (ent->IsKind(STANDARD_TYPE(IGESBasic_Group)))
    {
      DeclareAndCast(IGESBasic_Group, g, ent);
      casenum              = 1;
      int nbg = g->NbEntities();
      for (int ig = 1; ig <= nbg; ig++)
      {
        if (TC.Search(g->Value(i), newent))
          newlist.GetOneItem(newent);
      }
    }
    if (ent->IsKind(STANDARD_TYPE(IGESBasic_GroupWithoutBackP)))
    {
      DeclareAndCast(IGESBasic_GroupWithoutBackP, g, ent);
      casenum              = 2;
      int nbg = g->NbEntities();
      for (int ig = 1; ig <= nbg; ig++)
      {
        if (TC.Search(g->Value(i), newent))
          newlist.GetOneItem(newent);
      }
    }
    if (ent->IsKind(STANDARD_TYPE(IGESBasic_OrderedGroup)))
    {
      DeclareAndCast(IGESBasic_OrderedGroup, g, ent);
      casenum              = 3;
      int nbg = g->NbEntities();
      for (int ig = 1; ig <= nbg; ig++)
      {
        if (TC.Search(g->Value(i), newent))
          newlist.GetOneItem(newent);
      }
    }
    if (ent->IsKind(STANDARD_TYPE(IGESBasic_OrderedGroupWithoutBackP)))
    {
      DeclareAndCast(IGESBasic_OrderedGroupWithoutBackP, g, ent);
      casenum              = 4;
      int nbg = g->NbEntities();
      for (int ig = 1; ig <= nbg; ig++)
      {
        if (TC.Search(g->Value(i), newent))
          newlist.GetOneItem(newent);
      }
    }
    //  A present, reconstruire sil le faut
    if (newlist.NbEntities() <= 1)
      continue; // 0 ou 1 : rien a refaire
    occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> tab =
      new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, newlist.NbEntities());
    int ng = 0;
    for (newlist.Start(); newlist.More(); newlist.Next())
    {
      ng++;
      tab->SetValue(ng, GetCasted(IGESData_IGESEntity, newlist.Value()));
    }
    switch (casenum)
    {
      case 1: {
        occ::handle<IGESBasic_Group> g = new IGESBasic_Group;
        g->Init(tab);
        target->AddEntity(g);

        //  Q : faut-il transferer le nom silyena un ?
      }
      break;
      case 2: {
        occ::handle<IGESBasic_GroupWithoutBackP> g = new IGESBasic_GroupWithoutBackP;
        g->Init(tab);
        target->AddEntity(g);
      }
      break;
      case 3: {
        occ::handle<IGESBasic_OrderedGroup> g = new IGESBasic_OrderedGroup;
        g->Init(tab);
        target->AddEntity(g);
      }
      break;
      case 4: {
        occ::handle<IGESBasic_OrderedGroupWithoutBackP> g = new IGESBasic_OrderedGroupWithoutBackP;
        g->Init(tab);
        target->AddEntity(g);
      }
      break;
      default:
        break;
    }
  }
}

TCollection_AsciiString IGESSelect_RebuildGroups::Label() const
{
  return TCollection_AsciiString("Rebuild Groups");
}
