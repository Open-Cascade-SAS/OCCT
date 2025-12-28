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

#include <IGESAppli_PinNumber.hxx>
#include <IGESAppli_ToolPinNumber.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_LevelListEntity.hxx>
#include <IGESData_ParamReader.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_ShareTool.hxx>
#include <TCollection_HAsciiString.hxx>

IGESAppli_ToolPinNumber::IGESAppli_ToolPinNumber() {}

void IGESAppli_ToolPinNumber::ReadOwnParams(const occ::handle<IGESAppli_PinNumber>& ent,
                                            const occ::handle<IGESData_IGESReaderData>& /* IR */,
                                            IGESData_ParamReader& PR) const
{
  int                                   tempNbPropertyValues;
  occ::handle<TCollection_HAsciiString> tempPinNumber;
  // bool st; //szv#4:S4163:12Mar99 not needed

  // szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadInteger(PR.Current(), "Number of property values", tempNbPropertyValues);
  PR.ReadText(PR.Current(), "PinNumber", tempPinNumber);

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(tempNbPropertyValues, tempPinNumber);
}

void IGESAppli_ToolPinNumber::WriteOwnParams(const occ::handle<IGESAppli_PinNumber>& ent,
                                             IGESData_IGESWriter&                    IW) const
{
  IW.Send(ent->NbPropertyValues());
  IW.Send(ent->PinNumberVal());
}

void IGESAppli_ToolPinNumber::OwnShared(const occ::handle<IGESAppli_PinNumber>& /* ent */,
                                        Interface_EntityIterator& /* iter */) const
{
}

void IGESAppli_ToolPinNumber::OwnCopy(const occ::handle<IGESAppli_PinNumber>& another,
                                      const occ::handle<IGESAppli_PinNumber>& ent,
                                      Interface_CopyTool& /* TC */) const
{
  int                                   aNbPropertyValues;
  occ::handle<TCollection_HAsciiString> aPinNumber =
    new TCollection_HAsciiString(another->PinNumberVal());
  aNbPropertyValues = another->NbPropertyValues();
  ent->Init(aNbPropertyValues, aPinNumber);
}

bool IGESAppli_ToolPinNumber::OwnCorrect(const occ::handle<IGESAppli_PinNumber>& ent) const
{
  bool res = (ent->SubordinateStatus() != 0);
  if (res)
  {
    occ::handle<IGESData_LevelListEntity> nulevel;
    ent->InitLevel(nulevel, 0);
  }
  return res; // RAZ level according to subordinate
}

IGESData_DirChecker IGESAppli_ToolPinNumber::DirChecker(
  const occ::handle<IGESAppli_PinNumber>& /* ent */) const
{
  IGESData_DirChecker DC(406, 8); // Form no = 8 & Type = 406
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.BlankStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESAppli_ToolPinNumber::OwnCheck(const occ::handle<IGESAppli_PinNumber>& ent,
                                       const Interface_ShareTool&,
                                       occ::handle<Interface_Check>& ach) const
{
  if (ent->SubordinateStatus() != 0)
    if (ent->DefLevel() != IGESData_DefOne && ent->DefLevel() != IGESData_DefSeveral)
      ach->AddFail("Level type: Incorrect");
  if (ent->NbPropertyValues() != 1)
    ach->AddFail("Number of Property Values != 1");
  // UNFINISHED
  // Level to be ignored if the property is subordinate -- queried
}

void IGESAppli_ToolPinNumber::OwnDump(const occ::handle<IGESAppli_PinNumber>& ent,
                                      const IGESData_IGESDumper& /* dumper */,
                                      Standard_OStream& S,
                                      const int /* level */) const
{
  S << "IGESAppli_PinNumber\n";
  S << "Number of Property Values : " << ent->NbPropertyValues() << "\n";
  S << "PinNumber : ";
  IGESData_DumpString(S, ent->PinNumberVal());
  S << std::endl;
}
