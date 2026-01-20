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

#include <gp_Pnt2d.hxx>
#include <gp_XY.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDimen_DiameterDimension.hxx>
#include <IGESDimen_GeneralNote.hxx>
#include <IGESDimen_LeaderArrow.hxx>
#include <IGESDimen_ToolDiameterDimension.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>

IGESDimen_ToolDiameterDimension::IGESDimen_ToolDiameterDimension() {}

void IGESDimen_ToolDiameterDimension::ReadOwnParams(const occ::handle<IGESDimen_DiameterDimension>& ent,
                                                    const occ::handle<IGESData_IGESReaderData>&     IR,
                                                    IGESData_ParamReader& PR) const
{
  // bool st; //szv#4:S4163:12Mar99 not needed

  occ::handle<IGESDimen_GeneralNote> note;
  occ::handle<IGESDimen_LeaderArrow> firstLeader;
  occ::handle<IGESDimen_LeaderArrow> secondLeader;
  gp_XY                         center;

  PR.ReadEntity(IR,
                PR.Current(),
                "General Note Entity",
                STANDARD_TYPE(IGESDimen_GeneralNote),
                note); // szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadEntity(IR,
                PR.Current(),
                "First Leader Entity",
                STANDARD_TYPE(IGESDimen_LeaderArrow),
                firstLeader); // szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadEntity(IR,
                PR.Current(),
                "Second Leader Entity",
                // clang-format off
		 STANDARD_TYPE(IGESDimen_LeaderArrow), secondLeader, true); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadXY(PR.CurrentList(1, 2), "Arc Center Co-ords", center); //szv#4:S4163:12Mar99 `st=` not needed
  // clang-format on

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(note, firstLeader, secondLeader, center);
}

void IGESDimen_ToolDiameterDimension::WriteOwnParams(const occ::handle<IGESDimen_DiameterDimension>& ent,
                                                     IGESData_IGESWriter& IW) const
{
  IW.Send(ent->Note());
  IW.Send(ent->FirstLeader());
  IW.Send(ent->SecondLeader());
  IW.Send(ent->Center().X());
  IW.Send(ent->Center().Y());
}

void IGESDimen_ToolDiameterDimension::OwnShared(const occ::handle<IGESDimen_DiameterDimension>& ent,
                                                Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->Note());
  iter.GetOneItem(ent->FirstLeader());
  iter.GetOneItem(ent->SecondLeader());
}

void IGESDimen_ToolDiameterDimension::OwnCopy(const occ::handle<IGESDimen_DiameterDimension>& another,
                                              const occ::handle<IGESDimen_DiameterDimension>& ent,
                                              Interface_CopyTool&                        TC) const
{
  DeclareAndCast(IGESDimen_GeneralNote, note, TC.Transferred(another->Note()));
  DeclareAndCast(IGESDimen_LeaderArrow, firstLeader, TC.Transferred(another->FirstLeader()));
  DeclareAndCast(IGESDimen_LeaderArrow, secondLeader, TC.Transferred(another->SecondLeader()));
  gp_XY center = (another->Center()).XY();

  ent->Init(note, firstLeader, secondLeader, center);
}

IGESData_DirChecker IGESDimen_ToolDiameterDimension::DirChecker(
  const occ::handle<IGESDimen_DiameterDimension>& /* ent */) const
{
  IGESData_DirChecker DC(206, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.UseFlagRequired(1);
  return DC;
}

void IGESDimen_ToolDiameterDimension::OwnCheck(const occ::handle<IGESDimen_DiameterDimension>& /* ent */,
                                               const Interface_ShareTool&,
                                               occ::handle<Interface_Check>& /* ach */) const
{
}

void IGESDimen_ToolDiameterDimension::OwnDump(const occ::handle<IGESDimen_DiameterDimension>& ent,
                                              const IGESData_IGESDumper&                 dumper,
                                              Standard_OStream&                          S,
                                              const int level) const
{
  int sublevel = (level > 4) ? 1 : 0;

  S << "IGESDimen_DiameterDimension\n"
    << "General Note Entity : ";
  dumper.Dump(ent->Note(), S, sublevel);
  S << "\n"
    << "First  Leader Entity : ";
  dumper.Dump(ent->FirstLeader(), S, sublevel);
  S << "\n"
    << "Second Leader Entity : ";
  dumper.Dump(ent->SecondLeader(), S, sublevel);
  S << "\n"
    << "Center Point : ";
  IGESData_DumpXYL(S, level, ent->Center(), ent->Location());
  S << std::endl;
}
