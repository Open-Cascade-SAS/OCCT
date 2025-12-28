// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDraw_ToolViewsVisible.hxx>
#include <IGESDraw_ViewsVisible.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_ShareTool.hxx>

#include <stdio.h>

IGESDraw_ToolViewsVisible::IGESDraw_ToolViewsVisible() {}

void IGESDraw_ToolViewsVisible::ReadOwnParams(const occ::handle<IGESDraw_ViewsVisible>&   ent,
                                              const occ::handle<IGESData_IGESReaderData>& IR,
                                              IGESData_ParamReader&                  PR) const
{
  // bool st; //szv#4:S4163:12Mar99 not needed

  int                         tempNbViewsVisible, tempNbDisplayedEntities;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>> tempViewEntities;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>     tempDisplayEntity;

  // st = PR.ReadInteger(PR.Current(), "Number Of Views Visible",
  // tempNbViewsVisible); //szv#4:S4163:12Mar99 moved in if
  if (PR.ReadInteger(PR.Current(), "Number Of Views Visible", tempNbViewsVisible))
  {
    // Initialise HArray1 only if there is no error reading its Length
    if (tempNbViewsVisible <= 0)
      PR.AddFail("Number Of Views Visible : Not Positive");
    else
      tempViewEntities = new NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>(1, tempNbViewsVisible);
  }

  if (PR.DefinedElseSkip())
    PR.ReadInteger(PR.Current(),
                   "Number of Entities Displayed",
                   tempNbDisplayedEntities); // szv#4:S4163:12Mar99 `st=` not needed
  else
  {
    tempNbDisplayedEntities = 0;
    PR.AddWarning("Number of Entities Displayed : undefined, set to Zero");
  }
  // Initialise HArray1 only if there is no error reading its Length
  if (tempNbDisplayedEntities < 0)
    PR.AddFail("Number Of Entities Displayed : Less than Zero");
  //  else if (tempNbDisplayedEntities > 0) {

  if (!tempViewEntities.IsNull())
  {
    // Read the HArray1 only if its Length was read without any Error
    int I;
    for (I = 1; I <= tempNbViewsVisible; I++)
    {
      occ::handle<IGESData_ViewKindEntity> tempViewEntity1;
      // st = PR.ReadEntity(IR, PR.Current(), "View Entity",
      // STANDARD_TYPE(IGESData_ViewKindEntity), tempViewEntity1); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadEntity(IR,
                        PR.Current(),
                        "View Entity",
                        STANDARD_TYPE(IGESData_ViewKindEntity),
                        tempViewEntity1))
        tempViewEntities->SetValue(I, tempViewEntity1);
    }
  }

  // Read the HArray1 only if its Length was read without any Error
  if (tempNbDisplayedEntities > 0)
  {
    PR.ReadEnts(IR,
                PR.CurrentList(tempNbDisplayedEntities),
                "Displayed Entities",
                tempDisplayEntity); // szv#4:S4163:12Mar99 `st=` not needed
    /*
        tempDisplayEntity =
          new NCollection_HArray1<occ::handle<IGESData_IGESEntity>> (1, tempNbDisplayedEntities);

        occ::handle<IGESData_IGESEntity> tempEntity2;
        int I;
        for (I = 1; I <= tempNbDisplayedEntities; I++) {
          st = PR.ReadEntity(IR, PR.Current(), "Displayed Entity",
                 tempEntity2);
          if (st) tempDisplayEntity->SetValue(I, tempEntity2);
        }
    */
  }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(tempViewEntities, tempDisplayEntity);
}

void IGESDraw_ToolViewsVisible::WriteOwnParams(const occ::handle<IGESDraw_ViewsVisible>& ent,
                                               IGESData_IGESWriter&                 IW) const
{
  IW.Send(ent->NbViews());
  IW.Send(ent->NbDisplayedEntities());

  int I;
  int up = ent->NbViews();
  for (I = 1; I <= up; I++)
    IW.Send(ent->ViewItem(I));
  up = ent->NbDisplayedEntities();
  for (I = 1; I <= up; I++)
    IW.Send(ent->DisplayedEntity(I));
}

void IGESDraw_ToolViewsVisible::OwnShared(const occ::handle<IGESDraw_ViewsVisible>& ent,
                                          Interface_EntityIterator&            iter) const
{
  int I, up;
  up = ent->NbViews();
  for (I = 1; I <= up; I++)
    iter.GetOneItem(ent->ViewItem(I));
  //  Displayed -> Implied
}

void IGESDraw_ToolViewsVisible::OwnImplied(const occ::handle<IGESDraw_ViewsVisible>& ent,
                                           Interface_EntityIterator&            iter) const
{
  int I, up;
  up = ent->NbDisplayedEntities();
  for (I = 1; I <= up; I++)
    iter.GetOneItem(ent->DisplayedEntity(I));
}

void IGESDraw_ToolViewsVisible::OwnCopy(const occ::handle<IGESDraw_ViewsVisible>& another,
                                        const occ::handle<IGESDraw_ViewsVisible>& ent,
                                        Interface_CopyTool&                  TC) const
{
  occ::handle<NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>> tempViewEntities =
    new NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>(1, another->NbViews());
  int I, up;
  up = another->NbViews();
  for (I = 1; I <= up; I++)
  {
    DeclareAndCast(IGESData_ViewKindEntity, tempView, TC.Transferred(another->ViewItem(I)));
    tempViewEntities->SetValue(I, tempView);
  }
  //  Displayed -> Implied : set an empty list by default
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> tempDisplayEntities;
  ent->Init(tempViewEntities, tempDisplayEntities);
}

void IGESDraw_ToolViewsVisible::OwnRenew(const occ::handle<IGESDraw_ViewsVisible>& another,
                                         const occ::handle<IGESDraw_ViewsVisible>& ent,
                                         const Interface_CopyTool&            TC) const
{
  Interface_EntityIterator newdisp;
  int         I, up;
  up = another->NbDisplayedEntities();
  if (up == 0)
    return;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> tempDisplayEntities;
  occ::handle<Standard_Transient>           anew;
  for (I = 1; I <= up; I++)
  {
    if (TC.Search(another->DisplayedEntity(I), anew))
      newdisp.GetOneItem(anew);
  }

  up = newdisp.NbEntities();
  I  = 0;
  if (up > 0)
    tempDisplayEntities = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, up);
  for (newdisp.Start(); newdisp.More(); newdisp.Next())
  {
    I++;
    DeclareAndCast(IGESData_IGESEntity, tempEntity, newdisp.Value());
    tempDisplayEntities->SetValue(I, tempEntity);
  }
  ent->InitImplied(tempDisplayEntities);
}

IGESData_DirChecker IGESDraw_ToolViewsVisible::DirChecker(
  const occ::handle<IGESDraw_ViewsVisible>& /*ent*/) const
{
  IGESData_DirChecker DC(402, 3);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.SubordinateStatusRequired(0);
  DC.UseFlagRequired(1);
  DC.HierarchyStatusIgnored();

  return DC;
}

void IGESDraw_ToolViewsVisible::OwnCheck(const occ::handle<IGESDraw_ViewsVisible>& ent,
                                         const Interface_ShareTool&,
                                         occ::handle<Interface_Check>& ach) const
{
  int                       res     = 0;
  int                       nb      = ent->NbDisplayedEntities();
  const occ::handle<IGESData_ViewKindEntity>& entcomp = ent;
  for (int i = 1; i <= nb; i++)
  {
    occ::handle<IGESData_IGESEntity> displayed = ent->DisplayedEntity(i);
    if (entcomp != displayed->View())
      res++;
  }
  if (!res)
    return;
  char mess[80];
  Sprintf(mess, "Mismatch for %d Entities displayed", res);
  ach->AddFail(mess, "Mismatch for %d Entities displayed");
}

void IGESDraw_ToolViewsVisible::OwnWhenDelete(const occ::handle<IGESDraw_ViewsVisible>& ent) const
{
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> tempDisplayEntities;
  ent->InitImplied(tempDisplayEntities);
}

void IGESDraw_ToolViewsVisible::OwnDump(const occ::handle<IGESDraw_ViewsVisible>& ent,
                                        const IGESData_IGESDumper&           dumper,
                                        Standard_OStream&                    S,
                                        const int               level) const
{
  S << "IGESDraw_ViewsVisible\n"
    << "Views Visible : ";
  IGESData_DumpEntities(S, dumper, level, 1, ent->NbViews(), ent->ViewItem);
  S << "\n"
    << "Entities Displayed : ";
  IGESData_DumpEntities(S, dumper, level, 1, ent->NbDisplayedEntities(), ent->DisplayedEntity);
  S << std::endl;
}

bool IGESDraw_ToolViewsVisible::OwnCorrect(
  const occ::handle<IGESDraw_ViewsVisible>& ent) const
{
  //  The displayed entities must reference <ent>. They have priority.
  bool                       res     = false;
  int                       nb      = ent->NbDisplayedEntities();
  const occ::handle<IGESData_ViewKindEntity>& entcomp = ent;
  for (int i = 1; i <= nb; i++)
  {
    occ::handle<IGESData_IGESEntity> displayed = ent->DisplayedEntity(i);
    if (entcomp != displayed->View())
      res = true;
  }
  if (!res)
    return res;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> nulDisplayEntities;
  ent->InitImplied(nulDisplayEntities);
  return res;
}
