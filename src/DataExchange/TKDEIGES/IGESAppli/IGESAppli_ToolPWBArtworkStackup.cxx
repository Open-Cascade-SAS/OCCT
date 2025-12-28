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

#include <IGESAppli_PWBArtworkStackup.hxx>
#include <IGESAppli_ToolPWBArtworkStackup.hxx>
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
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

IGESAppli_ToolPWBArtworkStackup::IGESAppli_ToolPWBArtworkStackup() {}

void IGESAppli_ToolPWBArtworkStackup::ReadOwnParams(const occ::handle<IGESAppli_PWBArtworkStackup>& ent,
                                                    const occ::handle<IGESData_IGESReaderData>& /* IR */,
                                                    IGESData_ParamReader& PR) const
{
  // bool st; //szv#4:S4163:12Mar99 not needed
  int                 num;
  int                 tempNbPropertyValues;
  occ::handle<TCollection_HAsciiString> tempArtworkStackupIdent;
  occ::handle<NCollection_HArray1<int>> tempLevelNumbers;
  // szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadInteger(PR.Current(), "Number of property values", tempNbPropertyValues);
  PR.ReadText(PR.Current(), "Artwork Stackup Identification", tempArtworkStackupIdent);
  if (!PR.ReadInteger(PR.Current(), "Number of level numbers", num))
    num = 0;
  if (num > 0)
    tempLevelNumbers = new NCollection_HArray1<int>(1, num);
  else
    PR.AddFail("Number of level numbers: Not Positive");
  if (!tempLevelNumbers.IsNull())
    PR.ReadInts(PR.CurrentList(num), "Level Numbers", tempLevelNumbers);

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(tempNbPropertyValues, tempArtworkStackupIdent, tempLevelNumbers);
}

void IGESAppli_ToolPWBArtworkStackup::WriteOwnParams(const occ::handle<IGESAppli_PWBArtworkStackup>& ent,
                                                     IGESData_IGESWriter& IW) const
{
  int i, num;
  IW.Send(ent->NbPropertyValues());
  IW.Send(ent->Identification());
  IW.Send(ent->NbLevelNumbers());
  for (num = ent->NbLevelNumbers(), i = 1; i <= num; i++)
    IW.Send(ent->LevelNumber(i));
}

void IGESAppli_ToolPWBArtworkStackup::OwnShared(
  const occ::handle<IGESAppli_PWBArtworkStackup>& /* ent */,
  Interface_EntityIterator& /* iter */) const
{
}

void IGESAppli_ToolPWBArtworkStackup::OwnCopy(const occ::handle<IGESAppli_PWBArtworkStackup>& another,
                                              const occ::handle<IGESAppli_PWBArtworkStackup>& ent,
                                              Interface_CopyTool& /* TC */) const
{
  int                 num                  = another->NbLevelNumbers();
  int                 tempNbPropertyValues = another->NbPropertyValues();
  occ::handle<TCollection_HAsciiString> tempArtworkStackupIdent =
    new TCollection_HAsciiString(another->Identification());
  occ::handle<NCollection_HArray1<int>> tempLevelNumbers = new NCollection_HArray1<int>(1, num);
  for (int i = 1; i <= num; i++)
    tempLevelNumbers->SetValue(i, another->LevelNumber(i));
  ent->Init(tempNbPropertyValues, tempArtworkStackupIdent, tempLevelNumbers);
}

IGESData_DirChecker IGESAppli_ToolPWBArtworkStackup::DirChecker(
  const occ::handle<IGESAppli_PWBArtworkStackup>& /* ent */) const
{
  IGESData_DirChecker DC(406, 25);
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

void IGESAppli_ToolPWBArtworkStackup::OwnCheck(const occ::handle<IGESAppli_PWBArtworkStackup>& /* ent */,
                                               const Interface_ShareTool&,
                                               occ::handle<Interface_Check>& /* ach */) const
{
}

void IGESAppli_ToolPWBArtworkStackup::OwnDump(const occ::handle<IGESAppli_PWBArtworkStackup>& ent,
                                              const IGESData_IGESDumper& /* dumper */,
                                              Standard_OStream&      S,
                                              const int level) const
{
  S << "IGESAppli_PWBArtworkStackup\n";
  S << "Number of property values : " << ent->NbPropertyValues() << "\n";
  S << "Artwork Stackup Identification : ";
  IGESData_DumpString(S, ent->Identification());
  S << "\n";
  S << "Level Numbers : ";
  IGESData_DumpVals(S, level, 1, ent->NbLevelNumbers(), ent->LevelNumber);
  S << std::endl;
}
