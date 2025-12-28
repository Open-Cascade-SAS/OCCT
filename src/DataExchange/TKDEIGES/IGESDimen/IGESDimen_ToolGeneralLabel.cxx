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
#include <IGESDimen_GeneralLabel.hxx>
#include <IGESDimen_GeneralNote.hxx>
#include <IGESDimen_LeaderArrow.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESDimen_ToolGeneralLabel.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>

IGESDimen_ToolGeneralLabel::IGESDimen_ToolGeneralLabel() {}

void IGESDimen_ToolGeneralLabel::ReadOwnParams(const occ::handle<IGESDimen_GeneralLabel>&  ent,
                                               const occ::handle<IGESData_IGESReaderData>& IR,
                                               IGESData_ParamReader&                       PR) const
{
  // bool st; //szv#4:S4163:12Mar99 moved down

  occ::handle<IGESDimen_GeneralNote>                                   note;
  int                                                                  nbval;
  occ::handle<NCollection_HArray1<occ::handle<IGESDimen_LeaderArrow>>> leaders;

  PR.ReadEntity(IR,
                PR.Current(),
                "General Note Entity",
                STANDARD_TYPE(IGESDimen_GeneralNote),
                note); // szv#4:S4163:12Mar99 `st=` not needed

  bool st = PR.ReadInteger(PR.Current(), "Number of Leaders", nbval);
  if (st && nbval > 0)
    leaders = new NCollection_HArray1<occ::handle<IGESDimen_LeaderArrow>>(1, nbval);
  else
    PR.AddFail("Number of Leaders: Not Positive");

  if (!leaders.IsNull())
    for (int i = 1; i <= nbval; i++)
    {
      occ::handle<IGESDimen_LeaderArrow> anentity;
      // st = PR.ReadEntity(IR, PR.Current(), "Leaders",
      // STANDARD_TYPE(IGESDimen_LeaderArrow), anentity); //szv#4:S4163:12Mar99 moved in if
      if (PR.ReadEntity(IR,
                        PR.Current(),
                        "Leaders",
                        STANDARD_TYPE(IGESDimen_LeaderArrow),
                        anentity))
        leaders->SetValue(i, anentity);
    }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(note, leaders);
}

void IGESDimen_ToolGeneralLabel::WriteOwnParams(const occ::handle<IGESDimen_GeneralLabel>& ent,
                                                IGESData_IGESWriter&                       IW) const
{
  int upper = ent->NbLeaders();
  IW.Send(ent->Note());
  IW.Send(upper);
  for (int i = 1; i <= upper; i++)
    IW.Send(ent->Leader(i));
}

void IGESDimen_ToolGeneralLabel::OwnShared(const occ::handle<IGESDimen_GeneralLabel>& ent,
                                           Interface_EntityIterator&                  iter) const
{
  int upper = ent->NbLeaders();
  iter.GetOneItem(ent->Note());
  for (int i = 1; i <= upper; i++)
    iter.GetOneItem(ent->Leader(i));
}

void IGESDimen_ToolGeneralLabel::OwnCopy(const occ::handle<IGESDimen_GeneralLabel>& another,
                                         const occ::handle<IGESDimen_GeneralLabel>& ent,
                                         Interface_CopyTool&                        TC) const
{
  DeclareAndCast(IGESDimen_GeneralNote, note, TC.Transferred(another->Note()));
  int nbval = another->NbLeaders();

  occ::handle<NCollection_HArray1<occ::handle<IGESDimen_LeaderArrow>>> leaders =
    new NCollection_HArray1<occ::handle<IGESDimen_LeaderArrow>>(1, nbval);
  for (int i = 1; i <= nbval; i++)
  {
    DeclareAndCast(IGESDimen_LeaderArrow, new_ent, TC.Transferred(another->Leader(i)));
    leaders->SetValue(i, new_ent);
  }
  ent->Init(note, leaders);
}

IGESData_DirChecker IGESDimen_ToolGeneralLabel::DirChecker(
  const occ::handle<IGESDimen_GeneralLabel>& /* ent */) const
{
  IGESData_DirChecker DC(210, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.UseFlagRequired(1);
  return DC;
}

void IGESDimen_ToolGeneralLabel::OwnCheck(const occ::handle<IGESDimen_GeneralLabel>& /* ent */,
                                          const Interface_ShareTool&,
                                          occ::handle<Interface_Check>& /* ach */) const
{
}

void IGESDimen_ToolGeneralLabel::OwnDump(const occ::handle<IGESDimen_GeneralLabel>& ent,
                                         const IGESData_IGESDumper&                 dumper,
                                         Standard_OStream&                          S,
                                         const int                                  level) const
{
  int sublevel = (level > 4) ? 1 : 0;

  S << "IGESDimen_GeneralLabel\n"
    << "General Note Entity : ";
  dumper.Dump(ent->Note(), S, sublevel);
  S << "\n"
    << "Number of Leaders : " << ent->NbLeaders() << "\n"
    << "Leaders : ";
  IGESData_DumpEntities(S, dumper, level, 1, ent->NbLeaders(), ent->Leader);
  S << std::endl;
}
