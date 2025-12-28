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

#include <IGESBasic_ExternalRefFileIndex.hxx>
#include <IGESBasic_ToolExternalRefFileIndex.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESEntity.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamReader.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <TCollection_HAsciiString.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>

IGESBasic_ToolExternalRefFileIndex::IGESBasic_ToolExternalRefFileIndex() {}

void IGESBasic_ToolExternalRefFileIndex::ReadOwnParams(
  const occ::handle<IGESBasic_ExternalRefFileIndex>& ent,
  const occ::handle<IGESData_IGESReaderData>&        IR,
  IGESData_ParamReader&                              PR) const
{
  // bool st; //szv#4:S4163:12Mar99 moved down
  int                                                                     num, i;
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> tempNames;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>      tempEntities;
  bool st = PR.ReadInteger(PR.Current(), "Number of index entries", num);
  if (st && num > 0)
  {
    tempNames    = new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, num);
    tempEntities = new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, num);
  }
  else
    PR.AddFail("Number of index entries: Not Positive");
  if (!tempNames.IsNull() && !tempEntities.IsNull())
    for (i = 1; i <= num; i++)
    {
      occ::handle<TCollection_HAsciiString> tempNam;
      if (PR.ReadText(PR.Current(),
                      "External Reference Entity",
                      tempNam)) // szv#4:S4163:12Mar99 `st=` not needed
        tempNames->SetValue(i, tempNam);
      occ::handle<IGESData_IGESEntity> tempEnt;
      if (PR.ReadEntity(IR,
                        PR.Current(),
                        "Internal Entity",
                        tempEnt)) // szv#4:S4163:12Mar99 `st=` not needed
        tempEntities->SetValue(i, tempEnt);
    }
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(tempNames, tempEntities);
}

void IGESBasic_ToolExternalRefFileIndex::WriteOwnParams(
  const occ::handle<IGESBasic_ExternalRefFileIndex>& ent,
  IGESData_IGESWriter&                               IW) const
{
  int i, num;
  IW.Send(ent->NbEntries());
  for (num = ent->NbEntries(), i = 1; i <= num; i++)
  {
    IW.Send(ent->Name(i));
    IW.Send(ent->Entity(i));
  }
}

void IGESBasic_ToolExternalRefFileIndex::OwnShared(
  const occ::handle<IGESBasic_ExternalRefFileIndex>& ent,
  Interface_EntityIterator&                          iter) const
{
  int i, num;
  for (num = ent->NbEntries(), i = 1; i <= num; i++)
    iter.GetOneItem(ent->Entity(i));
}

void IGESBasic_ToolExternalRefFileIndex::OwnCopy(
  const occ::handle<IGESBasic_ExternalRefFileIndex>& another,
  const occ::handle<IGESBasic_ExternalRefFileIndex>& ent,
  Interface_CopyTool&                                TC) const
{
  int num = another->NbEntries();
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> tempNames =
    new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, num);
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> tempEntities =
    new NCollection_HArray1<occ::handle<IGESData_IGESEntity>>(1, num);
  for (int i = 1; i <= num; i++)
  {
    tempNames->SetValue(i, new TCollection_HAsciiString(another->Name(i)));
    DeclareAndCast(IGESData_IGESEntity, new_item, TC.Transferred(another->Entity(i)));
    tempEntities->SetValue(i, new_item);
  }
  ent->Init(tempNames, tempEntities);
}

IGESData_DirChecker IGESBasic_ToolExternalRefFileIndex::DirChecker(
  const occ::handle<IGESBasic_ExternalRefFileIndex>& /* ent */) const
{
  IGESData_DirChecker DC(402, 12);
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESBasic_ToolExternalRefFileIndex::OwnCheck(
  const occ::handle<IGESBasic_ExternalRefFileIndex>& /* ent */,
  const Interface_ShareTool&,
  occ::handle<Interface_Check>& /* ach */) const
{
}

void IGESBasic_ToolExternalRefFileIndex::OwnDump(
  const occ::handle<IGESBasic_ExternalRefFileIndex>& ent,
  const IGESData_IGESDumper&                         dumper,
  Standard_OStream&                                  S,
  const int                                          level) const
{
  int i, num;
  S << "IGESBasic_ExternalRefFileIndex\n"
    << "External Reference Names :\n"
    << "Internal Entities : ";
  IGESData_DumpEntities(S, dumper, -level, 1, ent->NbEntries(), ent->Entity);
  S << "\n";
  if (level > 4)
    for (num = ent->NbEntries(), i = 1; i <= num; i++)
    {
      S << "[" << i << "]: "
        << "External Reference Name : ";
      IGESData_DumpString(S, ent->Name(i));
      S << "  Internal Entity : ";
      dumper.Dump(ent->Entity(i), S, 1);
      S << "\n";
    }
  S << std::endl;
}
