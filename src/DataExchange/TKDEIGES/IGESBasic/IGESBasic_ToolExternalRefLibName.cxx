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

#include <IGESBasic_ExternalRefLibName.hxx>
#include <IGESBasic_ToolExternalRefLibName.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamReader.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_ShareTool.hxx>
#include <TCollection_HAsciiString.hxx>

IGESBasic_ToolExternalRefLibName::IGESBasic_ToolExternalRefLibName() = default;

void IGESBasic_ToolExternalRefLibName::ReadOwnParams(
  const occ::handle<IGESBasic_ExternalRefLibName>& ent,
  const occ::handle<IGESData_IGESReaderData>& /* IR */,
  IGESData_ParamReader& PR) const
{
  // bool st; //szv#4:S4163:12Mar99 not needed
  occ::handle<TCollection_HAsciiString> tempLibName;
  occ::handle<TCollection_HAsciiString> tempExtRefEntitySymbName;
  // clang-format off
  PR.ReadText(PR.Current(), "Name of Library", tempLibName); //szv#4:S4163:12Mar99 `st=` not needed
  // clang-format on
  PR.ReadText(PR.Current(),
              "External Reference Symbolic Name",
              tempExtRefEntitySymbName); // szv#4:S4163:12Mar99 `st=` not needed

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(tempLibName, tempExtRefEntitySymbName);
}

void IGESBasic_ToolExternalRefLibName::WriteOwnParams(
  const occ::handle<IGESBasic_ExternalRefLibName>& ent,
  IGESData_IGESWriter&                             IW) const
{
  IW.Send(ent->LibraryName());
  IW.Send(ent->ReferenceName());
}

void IGESBasic_ToolExternalRefLibName::OwnShared(
  const occ::handle<IGESBasic_ExternalRefLibName>& /* ent */,
  Interface_EntityIterator& /* iter */) const
{
}

void IGESBasic_ToolExternalRefLibName::OwnCopy(
  const occ::handle<IGESBasic_ExternalRefLibName>& another,
  const occ::handle<IGESBasic_ExternalRefLibName>& ent,
  Interface_CopyTool& /* TC */) const
{
  occ::handle<TCollection_HAsciiString> tempLibName =
    new TCollection_HAsciiString(another->LibraryName());
  occ::handle<TCollection_HAsciiString> tempRefName =
    new TCollection_HAsciiString(another->ReferenceName());
  ent->Init(tempLibName, tempRefName);
}

IGESData_DirChecker IGESBasic_ToolExternalRefLibName::DirChecker(
  const occ::handle<IGESBasic_ExternalRefLibName>& /* ent */) const
{
  IGESData_DirChecker DC(416, 4);
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusRequired(0);
  DC.HierarchyStatusRequired(0);
  return DC;
}

void IGESBasic_ToolExternalRefLibName::OwnCheck(
  const occ::handle<IGESBasic_ExternalRefLibName>& /* ent */,
  const Interface_ShareTool&,
  occ::handle<Interface_Check>& /* ach */) const
{
}

void IGESBasic_ToolExternalRefLibName::OwnDump(const occ::handle<IGESBasic_ExternalRefLibName>& ent,
                                               const IGESData_IGESDumper& /* dumper */,
                                               Standard_OStream& S,
                                               const int /* level */) const
{
  S << "IGESBasic_ExternalRefLibName\n"
    << "Name of Library : ";
  IGESData_DumpString(S, ent->LibraryName());
  S << "\n"
    << "External Reference Symbolic Name : ";
  IGESData_DumpString(S, ent->ReferenceName());
  S << std::endl;
}
