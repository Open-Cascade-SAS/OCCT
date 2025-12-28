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

#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDimen_SectionedArea.hxx>
#include <IGESDimen_ToolSectionedArea.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Standard_DomainError.hxx>

IGESDimen_ToolSectionedArea::IGESDimen_ToolSectionedArea() = default;

void IGESDimen_ToolSectionedArea::ReadOwnParams(const occ::handle<IGESDimen_SectionedArea>& ent,
                                                const occ::handle<IGESData_IGESReaderData>& IR,
                                                IGESData_ParamReader& PR) const
{
  occ::handle<IGESData_IGESEntity>                                   extCurve;
  int                                                                tempPattern, nbislands;
  gp_XYZ                                                             passPnt;
  double                                                             tempDistance, tempAngle;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> tempIslands;
  occ::handle<IGESData_IGESEntity>                                   anent;
  // bool st; //szv#4:S4163:12Mar99 moved down

  // szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadEntity(IR, PR.Current(), "Exterior curve", extCurve, (ent->FormNumber() == 1));
  PR.ReadInteger(PR.Current(), "Fill pattern", tempPattern);
  PR.ReadXYZ(PR.CurrentList(1, 3), "Passing point", passPnt);
  PR.ReadReal(PR.Current(), "Distance between lines", tempDistance);

  if (PR.DefinedElseSkip())
    // clang-format off
    PR.ReadReal(PR.Current(), "Angle between line and X axis", tempAngle); //szv#4:S4163:12Mar99 `st=` not needed
  else
    tempAngle = M_PI / 4.0;

  bool st = PR.ReadInteger(PR.Current(), "Number of island curves", nbislands);
  if (st && nbislands > 0)
    PR.ReadEnts (IR,PR.CurrentList(nbislands),"Island curves",tempIslands); //szv#4:S4163:12Mar99 `st=` not needed
  // clang-format on
  /*
      {
        tempIslands = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, nbislands);
        for (int i=1; i<=nbislands; i++)
      {
            st = PR.ReadEntity(IR, PR.Current(), "Island curves", anent);
        if (st) tempIslands->SetValue(i, anent);
      }
      }
  */

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(extCurve, tempPattern, passPnt, tempDistance, tempAngle, tempIslands);
}

void IGESDimen_ToolSectionedArea::WriteOwnParams(const occ::handle<IGESDimen_SectionedArea>& ent,
                                                 IGESData_IGESWriter& IW) const
{
  int i, length = ent->NbIslands();
  IW.Send(ent->ExteriorCurve());
  IW.Send(ent->Pattern());
  IW.Send(ent->PassingPoint().X());
  IW.Send(ent->PassingPoint().Y());
  IW.Send(ent->PassingPoint().Z());
  IW.Send(ent->Distance());
  IW.Send(ent->Angle());
  IW.Send(length);
  for (i = 1; i <= length; i++)
    IW.Send(ent->IslandCurve(i));
}

void IGESDimen_ToolSectionedArea::OwnShared(const occ::handle<IGESDimen_SectionedArea>& ent,
                                            Interface_EntityIterator&                   iter) const
{
  int i, length = ent->NbIslands();
  iter.GetOneItem(ent->ExteriorCurve());
  for (i = 1; i <= length; i++)
    iter.GetOneItem(ent->IslandCurve(i));
}

void IGESDimen_ToolSectionedArea::OwnCopy(const occ::handle<IGESDimen_SectionedArea>& another,
                                          const occ::handle<IGESDimen_SectionedArea>& ent,
                                          Interface_CopyTool&                         TC) const
{
  DeclareAndCast(IGESData_IGESEntity, extCurve, TC.Transferred(another->ExteriorCurve()));
  int    tempPattern  = another->Pattern();
  gp_XYZ passPnt      = (another->PassingPoint()).XYZ();
  double tempDistance = another->Distance();
  double tempAngle    = another->Angle();
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> tempIslands;
  int nbislands = another->NbIslands();
  if (nbislands > 0)
  {
    tempIslands = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, nbislands);
    for (int i = 1; i <= nbislands; i++)
    {
      DeclareAndCast(IGESData_IGESEntity, anent, TC.Transferred(another->IslandCurve(i)));
      tempIslands->SetValue(i, anent);
    }
  }
  ent->Init(extCurve, tempPattern, passPnt, tempDistance, tempAngle, tempIslands);
  ent->SetInverted(another->IsInverted());
}

IGESData_DirChecker IGESDimen_ToolSectionedArea::DirChecker(
  const occ::handle<IGESDimen_SectionedArea>& /* ent */) const
{
  IGESData_DirChecker DC(230, 0, 1);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);

  DC.UseFlagRequired(1);

  return DC;
}

void IGESDimen_ToolSectionedArea::OwnCheck(const occ::handle<IGESDimen_SectionedArea>& /* ent */,
                                           const Interface_ShareTool&,
                                           occ::handle<Interface_Check>& /* ach */) const
{
}

void IGESDimen_ToolSectionedArea::OwnDump(const occ::handle<IGESDimen_SectionedArea>& ent,
                                          const IGESData_IGESDumper&                  dumper,
                                          Standard_OStream&                           S,
                                          const int                                   level) const
{
  int sublevel = (level <= 4) ? 0 : 1;

  S << "IGESDimen_SectionedArea\n"
    << (ent->IsInverted() ? "Inverted Cross Hatches" : "Standard Cross Hatches")
    << " - Exterior curve : ";
  dumper.Dump(ent->ExteriorCurve(), S, sublevel);
  S << "\n"
    << "Fill pattern code : " << ent->Pattern() << "\n"
    << "Passing point : ";
  IGESData_DumpXYZL(S, level, ent->PassingPoint(), ent->Location());
  S << "\n";
  S << "Distance between lines : " << ent->Distance() << "\n"
    << "Angle between lines and X axis : " << ent->Angle() << "\n"
    << "Island Curve : ";
  IGESData_DumpEntities(S, dumper, level, 1, ent->NbIslands(), ent->IslandCurve);
  S << "\n";
}
