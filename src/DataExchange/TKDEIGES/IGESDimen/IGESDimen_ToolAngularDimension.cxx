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
#include <IGESDimen_AngularDimension.hxx>
#include <IGESDimen_GeneralNote.hxx>
#include <IGESDimen_LeaderArrow.hxx>
#include <IGESDimen_ToolAngularDimension.hxx>
#include <IGESDimen_WitnessLine.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>

IGESDimen_ToolAngularDimension::IGESDimen_ToolAngularDimension() = default;

void IGESDimen_ToolAngularDimension::ReadOwnParams(
  const occ::handle<IGESDimen_AngularDimension>& ent,
  const occ::handle<IGESData_IGESReaderData>&    IR,
  IGESData_ParamReader&                          PR) const
{
  // bool st; //szv#4:S4163:12Mar99 not needed

  occ::handle<IGESDimen_GeneralNote> note;
  occ::handle<IGESDimen_WitnessLine> firstWitness;
  occ::handle<IGESDimen_WitnessLine> secondWitness;
  gp_XY                              vertex;
  double                             radius;
  occ::handle<IGESDimen_LeaderArrow> firstLeader;
  occ::handle<IGESDimen_LeaderArrow> secondLeader;

  PR.ReadEntity(IR,
                PR.Current(),
                "General Note Entity",
                STANDARD_TYPE(IGESDimen_GeneralNote),
                note); // szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadEntity(IR,
                PR.Current(),
                "First Witness Entity",
                // clang-format off
		 STANDARD_TYPE(IGESDimen_WitnessLine), firstWitness, true); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadEntity (IR,PR.Current(),"Second Witness Entity",
		 STANDARD_TYPE(IGESDimen_WitnessLine), secondWitness, true); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadXY(PR.CurrentList(1, 2), "Vertex Point Co-ords", vertex); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadReal(PR.Current(), "Radius of Leader arcs", radius); //szv#4:S4163:12Mar99 `st=` not needed
  // clang-format on

  PR.ReadEntity(IR,
                PR.Current(),
                "First Leader Entity",
                STANDARD_TYPE(IGESDimen_LeaderArrow),
                firstLeader); // szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadEntity(IR,
                PR.Current(),
                "Second Leader Entity",
                STANDARD_TYPE(IGESDimen_LeaderArrow),
                secondLeader); // szv#4:S4163:12Mar99 `st=` not needed

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(note, firstWitness, secondWitness, vertex, radius, firstLeader, secondLeader);
}

void IGESDimen_ToolAngularDimension::WriteOwnParams(
  const occ::handle<IGESDimen_AngularDimension>& ent,
  IGESData_IGESWriter&                           IW) const
{
  IW.Send(ent->Note());
  IW.Send(ent->FirstWitnessLine());
  IW.Send(ent->SecondWitnessLine());
  IW.Send(ent->Vertex().X());
  IW.Send(ent->Vertex().Y());
  IW.Send(ent->Radius());
  IW.Send(ent->FirstLeader());
  IW.Send(ent->SecondLeader());
}

void IGESDimen_ToolAngularDimension::OwnShared(const occ::handle<IGESDimen_AngularDimension>& ent,
                                               Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->Note());
  iter.GetOneItem(ent->FirstWitnessLine());
  iter.GetOneItem(ent->SecondWitnessLine());
  iter.GetOneItem(ent->FirstLeader());
  iter.GetOneItem(ent->SecondLeader());
}

void IGESDimen_ToolAngularDimension::OwnCopy(const occ::handle<IGESDimen_AngularDimension>& another,
                                             const occ::handle<IGESDimen_AngularDimension>& ent,
                                             Interface_CopyTool& TC) const
{
  DeclareAndCast(IGESDimen_GeneralNote, note, TC.Transferred(another->Note()));
  DeclareAndCast(IGESDimen_WitnessLine, firstWitness, TC.Transferred(another->FirstWitnessLine()));
  DeclareAndCast(IGESDimen_WitnessLine,
                 secondWitness,
                 TC.Transferred(another->SecondWitnessLine()));
  gp_XY  vertex = (another->Vertex()).XY();
  double radius = another->Radius();
  DeclareAndCast(IGESDimen_LeaderArrow, firstLeader, TC.Transferred(another->FirstLeader()));
  DeclareAndCast(IGESDimen_LeaderArrow, secondLeader, TC.Transferred(another->SecondLeader()));

  ent->Init(note, firstWitness, secondWitness, vertex, radius, firstLeader, secondLeader);
}

IGESData_DirChecker IGESDimen_ToolAngularDimension::DirChecker(
  const occ::handle<IGESDimen_AngularDimension>& /* ent */) const
{
  IGESData_DirChecker DC(202, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.UseFlagRequired(1);
  return DC;
}

void IGESDimen_ToolAngularDimension::OwnCheck(
  const occ::handle<IGESDimen_AngularDimension>& /* ent */,
  const Interface_ShareTool&,
  occ::handle<Interface_Check>& /* ach */) const
{
}

void IGESDimen_ToolAngularDimension::OwnDump(const occ::handle<IGESDimen_AngularDimension>& ent,
                                             const IGESData_IGESDumper&                     dumper,
                                             Standard_OStream&                              S,
                                             const int level) const
{
  int sublevel = (level > 4) ? 1 : 0;

  S << "IGESDimen_AngularDimension\n"
    << "General Note Entity   : ";
  dumper.Dump(ent->Note(), S, sublevel);
  S << "\n"
    << "First  Witness Entity : ";
  dumper.Dump(ent->FirstWitnessLine(), S, sublevel);
  S << "\n"
    << "Second Witness Entity : ";
  dumper.Dump(ent->SecondWitnessLine(), S, sublevel);
  S << "\n"
    << "Vertex Point Co-ords  : ";
  IGESData_DumpXYL(S, level, ent->Vertex(), ent->Location());
  S << "\n";
  S << "Radius of Leader arcs : " << ent->Radius() << "\n"
    << "First  Leader Entity  : ";
  dumper.Dump(ent->FirstLeader(), S, sublevel);
  S << "\n"
    << "Second Leader Entity  : ";
  dumper.Dump(ent->SecondLeader(), S, sublevel);
  S << std::endl;
}
