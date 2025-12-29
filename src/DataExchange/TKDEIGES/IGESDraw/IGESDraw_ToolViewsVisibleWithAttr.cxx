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

#include <IGESData_LineFontEntity.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDraw_ToolViewsVisibleWithAttr.hxx>
#include <IGESDraw_ViewsVisibleWithAttr.hxx>
#include <IGESGraph_Color.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Integer.hxx>

#include <cstdio>

IGESDraw_ToolViewsVisibleWithAttr::IGESDraw_ToolViewsVisibleWithAttr() = default;

void IGESDraw_ToolViewsVisibleWithAttr::ReadOwnParams(
  const occ::handle<IGESDraw_ViewsVisibleWithAttr>& ent,
  const occ::handle<IGESData_IGESReaderData>&       IR,
  IGESData_ParamReader&                             PR) const
{
  // bool st; //szv#4:S4163:12Mar99 not needed

  int                                                                    tempNbBlocks, tempNbEntity;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>> tempViewEntities;
  occ::handle<NCollection_HArray1<occ::handle<IGESGraph_Color>>>         tempColorDefinitions;
  occ::handle<NCollection_HArray1<int>>                                  tempLineFonts;
  occ::handle<NCollection_HArray1<int>>                                  tempColorValues;
  occ::handle<NCollection_HArray1<int>>                                  tempLineWeights;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>     tempDisplayEntities;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_LineFontEntity>>> tempLineDefinitions;

  // st = PR.ReadInteger(PR.Current(), "Number Of Blocks", tempNbBlocks); //szv#4:S4163:12Mar99
  // moved in if
  if (PR.ReadInteger(PR.Current(), "Number Of Blocks", tempNbBlocks))
  {
    // Initialise HArray1 only if there is no error reading its Length
    if (tempNbBlocks <= 0)
      PR.AddFail("Number Of Blocks : Not Positive");
    else
    {
      tempViewEntities =
        new NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>(1, tempNbBlocks);
      tempLineFonts = new NCollection_HArray1<int>(1, tempNbBlocks);
      tempLineDefinitions =
        new NCollection_HArray1<occ::handle<IGESData_LineFontEntity>>(1, tempNbBlocks);
      tempColorValues      = new NCollection_HArray1<int>(1, tempNbBlocks);
      tempColorDefinitions = new NCollection_HArray1<occ::handle<IGESGraph_Color>>(1, tempNbBlocks);
      tempLineWeights      = new NCollection_HArray1<int>(1, tempNbBlocks);
    }
  }

  if (PR.DefinedElseSkip())
    PR.ReadInteger(PR.Current(),
                   "Number of Entities Displayed",
                   tempNbEntity); // szv#4:S4163:12Mar99 `st=` not needed
  else
  {
    tempNbEntity = 0;
    PR.AddWarning("Number of Entities Displayed : undefined, set to Zero");
  }
  // Initialise HArray1 only if there is no error reading its Length
  if (tempNbEntity < 0)
    PR.AddFail("Number Of Entities Displayed : Less than Zero");

  // Read the HArray1 only if its Length was read without any Error
  if (!(tempViewEntities.IsNull()))
  {
    // Assumption : When tempViewEntities != NULL, all other parallel
    //              arrays are also non-NULL
    int I;
    for (I = 1; I <= tempNbBlocks; I++)
    {
      occ::handle<IGESData_ViewKindEntity> tempView;
      int                                  tempLineFont;
      occ::handle<IGESData_LineFontEntity> tempEntity1;
      int                                  tempColorValue;
      occ::handle<IGESGraph_Color>         tempColorDef;
      int                                  tempLineWeightValue;

      // st = PR.ReadEntity(IR, PR.Current(), "View Entity",
      // STANDARD_TYPE(IGESData_ViewKindEntity), tempView); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadEntity(IR,
                        PR.Current(),
                        "View Entity",
                        STANDARD_TYPE(IGESData_ViewKindEntity),
                        tempView))
        tempViewEntities->SetValue(I, tempView);

      // st = PR.ReadInteger(PR.Current(), "Line Font Value", tempLineFont); //szv#4:S4163:12Mar99
      // moved in if
      if (PR.ReadInteger(PR.Current(), "Line Font Value", tempLineFont))
        tempLineFonts->SetValue(I, tempLineFont);

      // st = PR.ReadEntity(IR, PR.Current(), "Line Font Definition",
      // STANDARD_TYPE(IGESData_LineFontEntity),
      // tempEntity1, true); //szv#4:S4163:12Mar99 moved in if
      if (tempLineFont == 0
          && PR.ReadEntity(IR,
                           PR.Current(),
                           "Line Font Definition",
                           STANDARD_TYPE(IGESData_LineFontEntity),
                           tempEntity1,
                           true))
        tempLineDefinitions->SetValue(I, tempEntity1);

      int curnum = PR.CurrentNumber();
      //  Reading Color : Value (>0) or Definition (<0 = D.E. Pointer)
      if (PR.DefinedElseSkip())
        // clang-format off
	PR.ReadInteger( PR.Current(), "Color Value", tempColorValue); //szv#4:S4163:12Mar99 `st=` not needed
      // clang-format on
      else
      {
        tempColorValue = 0;
        PR.AddWarning("Color Value : undefined, set to Zero");
      }
      if (tempColorValue < 0)
      {
        tempColorValues->SetValue(I, -1);
        tempColorDef = GetCasted(IGESGraph_Color, PR.ParamEntity(IR, curnum));
        if (tempColorDef.IsNull())
          PR.AddFail("A Color Definition Entity is incorrect");
        else
          tempColorDefinitions->SetValue(I, tempColorDef);
      }
      else
        tempColorValues->SetValue(I, tempColorValue);

      // st = PR.ReadInteger(PR.Current(), "Line Weight Value",
      // tempLineWeightValue); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadInteger(PR.Current(), "Line Weight Value", tempLineWeightValue))
        tempLineWeights->SetValue(I, tempLineWeightValue);
    }
  }

  // Read the HArray1 only if its Length was read without any Error
  if (tempNbEntity > 0)
  {
    PR.ReadEnts(IR,
                PR.CurrentList(tempNbEntity),
                "Displayed Entities",
                tempDisplayEntities); // szv#4:S4163:12Mar99 `st=` not needed
    /*
        tempDisplayEntities = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>> (1,
       tempNbEntity); int I; for (I = 1; I <= tempNbEntity; I++) { occ::handle<IGESData_IGESEntity>
       tempEntity3; st = PR.ReadEntity(IR, PR.Current(), "Entity", tempEntity3); if (st)
       tempDisplayEntities->SetValue(I, tempEntity3);
        }
    */
  }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(tempViewEntities,
            tempLineFonts,
            tempLineDefinitions,
            tempColorValues,
            tempColorDefinitions,
            tempLineWeights,
            tempDisplayEntities);
}

void IGESDraw_ToolViewsVisibleWithAttr::WriteOwnParams(
  const occ::handle<IGESDraw_ViewsVisibleWithAttr>& ent,
  IGESData_IGESWriter&                              IW) const
{
  int up = ent->NbViews();
  IW.Send(up);
  IW.Send(ent->NbDisplayedEntities());

  int I;
  for (I = 1; I <= up; I++)
  {
    IW.Send(ent->ViewItem(I));
    IW.Send(ent->LineFontValue(I));
    IW.Send(ent->FontDefinition(I)); // controlled by LineFontValue, both sent
    if (ent->IsColorDefinition(I))
      IW.Send(ent->ColorDefinition(I), true); // negative
    else
      IW.Send(ent->ColorValue(I));
    IW.Send(ent->LineWeightItem(I));
  }
  up = ent->NbDisplayedEntities();
  for (I = 1; I <= up; I++)
    IW.Send(ent->DisplayedEntity(I));
}

void IGESDraw_ToolViewsVisibleWithAttr::OwnShared(
  const occ::handle<IGESDraw_ViewsVisibleWithAttr>& ent,
  Interface_EntityIterator&                         iter) const
{
  int up = ent->NbViews();

  int I;
  for (I = 1; I <= up; I++)
  {
    iter.GetOneItem(ent->ViewItem(I));
    iter.GetOneItem(ent->FontDefinition(I));
    if (ent->IsColorDefinition(I))
      iter.GetOneItem(ent->ColorDefinition(I));
  }
  //  Displayed -> Implied
}

void IGESDraw_ToolViewsVisibleWithAttr::OwnImplied(
  const occ::handle<IGESDraw_ViewsVisibleWithAttr>& ent,
  Interface_EntityIterator&                         iter) const
{
  int I, up;
  up = ent->NbDisplayedEntities();
  for (I = 1; I <= up; I++)
    iter.GetOneItem(ent->DisplayedEntity(I));
}

void IGESDraw_ToolViewsVisibleWithAttr::OwnCopy(
  const occ::handle<IGESDraw_ViewsVisibleWithAttr>& another,
  const occ::handle<IGESDraw_ViewsVisibleWithAttr>& ent,
  Interface_CopyTool&                               TC) const
{
  int                                                                    I;
  int                                                                    up = another->NbViews();
  occ::handle<NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>> tempViewEntities =
    new NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>(1, up);
  occ::handle<NCollection_HArray1<int>> tempLineFonts = new NCollection_HArray1<int>(1, up);
  occ::handle<NCollection_HArray1<occ::handle<IGESData_LineFontEntity>>> tempLineDefinitions =
    new NCollection_HArray1<occ::handle<IGESData_LineFontEntity>>(1, up);
  occ::handle<NCollection_HArray1<int>> tempColorValues = new NCollection_HArray1<int>(1, up);
  occ::handle<NCollection_HArray1<occ::handle<IGESGraph_Color>>> tempColorDefinitions =
    new NCollection_HArray1<occ::handle<IGESGraph_Color>>(1, up);
  occ::handle<NCollection_HArray1<int>> tempLineWeights = new NCollection_HArray1<int>(1, up);

  for (I = 1; I <= up; I++)
  {
    DeclareAndCast(IGESData_ViewKindEntity, tempView, TC.Transferred(another->ViewItem(I)));
    tempViewEntities->SetValue(I, tempView);
    int tempLineFont = another->LineFontValue(I);
    tempLineFonts->SetValue(I, tempLineFont);
    if (another->IsFontDefinition(I))
    {
      DeclareAndCast(IGESData_LineFontEntity,
                     tempEntity1,
                     TC.Transferred(another->FontDefinition(I)));
      tempLineDefinitions->SetValue(I, tempEntity1);
    }
    if (another->IsColorDefinition(I))
    {
      DeclareAndCast(IGESGraph_Color, tempEntity2, TC.Transferred(another->ColorDefinition(I)));
      tempColorDefinitions->SetValue(I, tempEntity2);
    }
    else
    {
      int tempColorValue = another->ColorValue(I);
      tempColorValues->SetValue(I, tempColorValue);
    }
    int tempLineWeight = another->LineWeightItem(I);
    tempLineWeights->SetValue(I, tempLineWeight);
  }
  //  Displayed -> Implied : set an empty list by default
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> tempDisplayEntities;
  ent->Init(tempViewEntities,
            tempLineFonts,
            tempLineDefinitions,
            tempColorValues,
            tempColorDefinitions,
            tempLineWeights,
            tempDisplayEntities);
}

void IGESDraw_ToolViewsVisibleWithAttr::OwnRenew(
  const occ::handle<IGESDraw_ViewsVisibleWithAttr>& another,
  const occ::handle<IGESDraw_ViewsVisibleWithAttr>& ent,
  const Interface_CopyTool&                         TC) const
{
  Interface_EntityIterator newdisp;
  int                      I, up;
  up = another->NbDisplayedEntities();
  if (up == 0)
    return;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> tempDisplayEntities;
  occ::handle<Standard_Transient>                                    anew;
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

IGESData_DirChecker IGESDraw_ToolViewsVisibleWithAttr::DirChecker(
  const occ::handle<IGESDraw_ViewsVisibleWithAttr>& /*ent*/) const
{
  IGESData_DirChecker DC(402, 4);
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

void IGESDraw_ToolViewsVisibleWithAttr::OwnCheck(
  const occ::handle<IGESDraw_ViewsVisibleWithAttr>& ent,
  const Interface_ShareTool&,
  occ::handle<Interface_Check>& ach) const
{
  int nb = ent->NbViews();
  int i; // svv Jan 10 2000 : porting on DEC
  for (i = 1; i <= nb; i++)
  {
    if (ent->LineFontValue(i) != 0 && ent->IsFontDefinition(i))
      ach->AddFail("At least one Line Font Definition Mismatch (both Value and Entity");
  }
  const occ::handle<IGESData_ViewKindEntity>& entcomp = ent;
  int                                         res     = 0;
  nb                                                  = ent->NbDisplayedEntities();
  for (i = 1; i <= nb; i++)
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

void IGESDraw_ToolViewsVisibleWithAttr::OwnWhenDelete(
  const occ::handle<IGESDraw_ViewsVisibleWithAttr>& ent) const
{
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> tempDisplayEntities;
  ent->InitImplied(tempDisplayEntities);
}

void IGESDraw_ToolViewsVisibleWithAttr::OwnDump(
  const occ::handle<IGESDraw_ViewsVisibleWithAttr>& ent,
  const IGESData_IGESDumper&                        dumper,
  Standard_OStream&                                 S,
  const int                                         level) const
{
  int tempSubLevel = (level <= 4) ? 0 : 1;

  S << "IGESDraw_ViewsVisibleWithAttr\n"
    << "View Entities            :\n"
    << "Line Font Values         :\n"
    << "Line Font Definitions    :\n"
    << "Color Number/Definitions :\n"
    << "Line Weights             :\n"
    << "Count of View Blocks : " << ent->NbViews() << "\n";
  if (level > 4)
  { // Level = 4 : nothing to Dump. Level = 5 & 6 : same Dump
    int I;
    int upper = ent->NbViews();
    for (I = 1; I <= upper; I++)
    {
      S << "[" << I << "]:\n"
        << "View Entity : ";
      dumper.Dump(ent->ViewItem(I), S, tempSubLevel);
      S << "\n";

      if (ent->IsFontDefinition(I))
      {
        S << "Line Font Definition  : ";
        dumper.Dump(ent->FontDefinition(I), S, tempSubLevel);
        S << "\n";
      }
      else
        S << "Line Font Value       : " << ent->LineFontValue(I) << "\n";

      if (ent->IsColorDefinition(I))
      {
        S << "Color Definition : ";
        dumper.Dump(ent->ColorDefinition(I), S, tempSubLevel);
        S << std::endl;
      }
      else
        S << "Color Value      : " << ent->ColorValue(I) << "\n";

      S << "Line Weight      : " << ent->LineWeightItem(I) << "\n";
    }
  }
  S << "Displayed Entities : ";
  IGESData_DumpEntities(S, dumper, level, 1, ent->NbDisplayedEntities(), ent->DisplayedEntity);
  S << std::endl;
}

bool IGESDraw_ToolViewsVisibleWithAttr::OwnCorrect(
  const occ::handle<IGESDraw_ViewsVisibleWithAttr>& ent) const
{
  //  The displayed entities must reference <ent>. They have priority.
  bool                                        res     = false;
  int                                         nb      = ent->NbDisplayedEntities();
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
