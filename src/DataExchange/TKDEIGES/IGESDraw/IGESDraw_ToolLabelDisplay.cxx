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

#include <gp_XYZ.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDraw_LabelDisplay.hxx>
#include <IGESDraw_PerspectiveView.hxx>
#include <IGESDraw_ToolLabelDisplay.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>

IGESDraw_ToolLabelDisplay::IGESDraw_ToolLabelDisplay() = default;

void IGESDraw_ToolLabelDisplay::ReadOwnParams(const occ::handle<IGESDraw_LabelDisplay>&   ent,
                                              const occ::handle<IGESData_IGESReaderData>& IR,
                                              IGESData_ParamReader&                       PR) const
{
  // bool st; //szv#4:S4163:12Mar99 moved down
  int nbval;

  occ::handle<NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>> views;
  occ::handle<NCollection_HArray1<gp_XYZ>>                               textLocations;
  occ::handle<NCollection_HArray1<occ::handle<IGESDimen_LeaderArrow>>>   leaderEntities;
  occ::handle<NCollection_HArray1<int>>                                  labelLevels;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>     displayedEntities;

  // Reading nbval(No. of Label placements)
  bool st = PR.ReadInteger(PR.Current(), "No. of Label placements", nbval);
  if (st && nbval > 0)
  {
    views             = new NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>(1, nbval);
    textLocations     = new NCollection_HArray1<gp_XYZ>(1, nbval);
    leaderEntities    = new NCollection_HArray1<occ::handle<IGESDimen_LeaderArrow>>(1, nbval);
    labelLevels       = new NCollection_HArray1<int>(1, nbval);
    displayedEntities = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, nbval);

    occ::handle<IGESData_ViewKindEntity> tempView;
    gp_XYZ                               tempXYZ;
    occ::handle<IGESDimen_LeaderArrow>   tempLeaderArrow;
    int                                  tempLabel;
    occ::handle<IGESData_IGESEntity>     tempDisplayedEntity;

    for (int i = 1; i <= nbval; i++)
    {
      // Reading views(HArray1OfView)
      // st = PR.ReadEntity (IR, PR.Current(), "Instance of views",
      // STANDARD_TYPE(IGESData_ViewKindEntity), tempView); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadEntity(IR,
                        PR.Current(),
                        "Instance of views",
                        STANDARD_TYPE(IGESData_ViewKindEntity),
                        tempView))
        views->SetValue(i, tempView);

      // Reading textLocations(HArray1OfXYZ)
      // st = PR.ReadXYZ(PR.CurrentList(1, 3), "array textLocations", tempXYZ);
      // //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadXYZ(PR.CurrentList(1, 3), "array textLocations", tempXYZ))
        textLocations->SetValue(i, tempXYZ);

      // Reading leaderEntities(HArray1OfLeaderArrow)
      // st = PR.ReadEntity (IR, PR.Current(), "Instance of LeaderArrow",
      // STANDARD_TYPE(IGESDimen_LeaderArrow), tempLeaderArrow); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadEntity(IR,
                        PR.Current(),
                        "Instance of LeaderArrow",
                        STANDARD_TYPE(IGESDimen_LeaderArrow),
                        tempLeaderArrow))
        leaderEntities->SetValue(i, tempLeaderArrow);

      // Reading labelLevels(HArray1OfInteger)
      // st = PR.ReadInteger(PR.Current(), "array labelLevels", tempLabel); //szv#4:S4163:12Mar99
      // moved in if
      if (PR.ReadInteger(PR.Current(), "array labelLevels", tempLabel))
        labelLevels->SetValue(i, tempLabel);

      // Reading displayedEntities(HArray1OfIGESEntity)
      // st = PR.ReadEntity (IR, PR.Current(), "displayedEntities entity",
      // tempDisplayedEntity); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadEntity(IR, PR.Current(), "displayedEntities entity", tempDisplayedEntity))
        displayedEntities->SetValue(i, tempDisplayedEntity);
    }
  }
  else
    PR.AddFail("No. of Label placements : Not Positive");

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(views, textLocations, leaderEntities, labelLevels, displayedEntities);
}

void IGESDraw_ToolLabelDisplay::WriteOwnParams(const occ::handle<IGESDraw_LabelDisplay>& ent,
                                               IGESData_IGESWriter&                      IW) const
{
  int Up = ent->NbLabels();
  IW.Send(Up);
  for (int i = 1; i <= Up; i++)
  {
    IW.Send(ent->ViewItem(i));
    IW.Send((ent->TextLocation(i)).X());
    IW.Send((ent->TextLocation(i)).Y());
    IW.Send((ent->TextLocation(i)).Z());
    IW.Send(ent->LeaderEntity(i));
    IW.Send(ent->LabelLevel(i));
    IW.Send(ent->DisplayedEntity(i));
  }
}

void IGESDraw_ToolLabelDisplay::OwnShared(const occ::handle<IGESDraw_LabelDisplay>& ent,
                                          Interface_EntityIterator&                 iter) const
{
  int Up = ent->NbLabels();
  for (int i = 1; i <= Up; i++)
  {
    iter.GetOneItem(ent->ViewItem(i));
    iter.GetOneItem(ent->LeaderEntity(i));
    iter.GetOneItem(ent->DisplayedEntity(i));
  }
}

void IGESDraw_ToolLabelDisplay::OwnCopy(const occ::handle<IGESDraw_LabelDisplay>& another,
                                        const occ::handle<IGESDraw_LabelDisplay>& ent,
                                        Interface_CopyTool&                       TC) const
{
  int                                                                    nbval;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>> views;
  occ::handle<NCollection_HArray1<gp_XYZ>>                               textLocations;
  occ::handle<NCollection_HArray1<occ::handle<IGESDimen_LeaderArrow>>>   leaderEntities;
  occ::handle<NCollection_HArray1<int>>                                  labelLevels;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>     displayedEntities;

  nbval             = another->NbLabels();
  views             = new NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>(1, nbval);
  textLocations     = new NCollection_HArray1<gp_XYZ>(1, nbval);
  leaderEntities    = new NCollection_HArray1<occ::handle<IGESDimen_LeaderArrow>>(1, nbval);
  labelLevels       = new NCollection_HArray1<int>(1, nbval);
  displayedEntities = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, nbval);

  for (int i = 1; i <= nbval; i++)
  {
    DeclareAndCast(IGESData_ViewKindEntity, tempView, TC.Transferred(another->ViewItem(i)));
    views->SetValue(i, tempView);

    textLocations->SetValue(i, (another->TextLocation(i)).XYZ());

    DeclareAndCast(IGESDimen_LeaderArrow, tempArrow, TC.Transferred(another->LeaderEntity(i)));
    leaderEntities->SetValue(i, tempArrow);

    labelLevels->SetValue(i, another->LabelLevel(i));

    DeclareAndCast(IGESData_IGESEntity, tempEntity, TC.Transferred(another->DisplayedEntity(i)));
    displayedEntities->SetValue(i, tempEntity);
  }

  ent->Init(views, textLocations, leaderEntities, labelLevels, displayedEntities);
}

IGESData_DirChecker IGESDraw_ToolLabelDisplay::DirChecker(
  const occ::handle<IGESDraw_LabelDisplay>& /*ent*/) const
{
  IGESData_DirChecker DC(402, 5);
  DC.Structure(IGESData_DefVoid);
  DC.HierarchyStatusIgnored();
  DC.BlankStatusIgnored();
  return DC;
}

void IGESDraw_ToolLabelDisplay::OwnCheck(const occ::handle<IGESDraw_LabelDisplay>& /*ent*/,
                                         const Interface_ShareTool&,
                                         occ::handle<Interface_Check>& /*ach*/) const
{
}

void IGESDraw_ToolLabelDisplay::OwnDump(const occ::handle<IGESDraw_LabelDisplay>& ent,
                                        const IGESData_IGESDumper&                dumper,
                                        Standard_OStream&                         S,
                                        const int                                 level) const
{
  int sublevel = (level <= 4) ? 0 : 1;

  S << "IGESDraw_LabelDisplay\n"
    << "View Entities       :\n"
    << "Text Locations      :\n"
    << "Leader Entities     :\n"
    << "Label Level Numbers :\n"
    << "Displayed Entities  : "
    << "Count = " << ent->NbLabels() << "\n";
  if (level > 4) // Level = 4 : no Dump. Level = 5 & 6 : same Dump
  {
    int I;
    int up = ent->NbLabels();
    for (I = 1; I <= up; I++)
    {
      S << "[" << I << "]:\n"
        << "View Entity : ";
      dumper.Dump(ent->ViewItem(I), S, sublevel);
      S << "\n"
        << "Text Location in View : ";
      IGESData_DumpXYZL(S, level, ent->TextLocation(I), ent->Location());
      S << "  Leader Entity in View : ";
      dumper.Dump(ent->LeaderEntity(I), S, sublevel);
      S << "\n"
        << "Entity Label Level Number : " << ent->LabelLevel(I) << "  "
        << "Displayed Entity : ";
      dumper.Dump(ent->DisplayedEntity(I), S, sublevel);
      S << "\n";
    }
  }
  S << std::endl;
}
