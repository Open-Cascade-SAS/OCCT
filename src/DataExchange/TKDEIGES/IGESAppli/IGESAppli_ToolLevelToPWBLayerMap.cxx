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

#include <IGESAppli_LevelToPWBLayerMap.hxx>
#include <IGESAppli_ToolLevelToPWBLayerMap.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamReader.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Integer.hxx>

IGESAppli_ToolLevelToPWBLayerMap::IGESAppli_ToolLevelToPWBLayerMap() {}

void IGESAppli_ToolLevelToPWBLayerMap::ReadOwnParams(
  const occ::handle<IGESAppli_LevelToPWBLayerMap>& ent,
  const occ::handle<IGESData_IGESReaderData>& /* IR */,
  IGESData_ParamReader& PR) const
{
  // bool st; //szv#4:S4163:12Mar99 not needed
  int                                   num, i;
  int                                   tempNbPropertyValues;
  occ::handle<NCollection_HArray1<int>> tempExchangeFileLevelNumber;
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> tempNativeLevel;
  occ::handle<NCollection_HArray1<int>>                                   tempPhysicalLayerNumber;
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>
    tempExchangeFileLevelIdent;
  // szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadInteger(PR.Current(), "Number of property values", tempNbPropertyValues);
  if (!PR.ReadInteger(PR.Current(), "Number of definitions", num))
    num = 0;
  if (num > 0)
  {
    tempExchangeFileLevelNumber = new NCollection_HArray1<int>(1, num);
    tempNativeLevel = new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, num);
    tempPhysicalLayerNumber = new NCollection_HArray1<int>(1, num);
    tempExchangeFileLevelIdent =
      new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, num);
  }
  else
    PR.AddFail("Number of definitions: Not Positive");

  if (!tempExchangeFileLevelNumber.IsNull() && !tempNativeLevel.IsNull()
      && !tempPhysicalLayerNumber.IsNull() && !tempExchangeFileLevelIdent.IsNull())
    for (i = 1; i <= num; i++)
    {
      int tempEFLN;
      // szv#4:S4163:12Mar99 moved in if
      if (PR.ReadInteger(PR.Current(), "Exchange File Level Number", tempEFLN))
        tempExchangeFileLevelNumber->SetValue(i, tempEFLN);
      occ::handle<TCollection_HAsciiString> tempNL;
      if (PR.ReadText(PR.Current(), "Native Level Identification", tempNL))
        tempNativeLevel->SetValue(i, tempNL);
      int tempPLN;
      if (PR.ReadInteger(PR.Current(), "Physical Layer Number", tempPLN))
        tempPhysicalLayerNumber->SetValue(i, tempPLN);
      occ::handle<TCollection_HAsciiString> tempEFLI;
      if (PR.ReadText(PR.Current(), "Exchange File Level Identification", tempEFLI))
        tempExchangeFileLevelIdent->SetValue(i, tempEFLI);
    }
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(tempNbPropertyValues,
            tempExchangeFileLevelNumber,
            tempNativeLevel,
            tempPhysicalLayerNumber,
            tempExchangeFileLevelIdent);
}

void IGESAppli_ToolLevelToPWBLayerMap::WriteOwnParams(
  const occ::handle<IGESAppli_LevelToPWBLayerMap>& ent,
  IGESData_IGESWriter&                             IW) const
{
  int i, num;
  IW.Send(ent->NbPropertyValues());
  IW.Send(ent->NbLevelToLayerDefs());
  for (num = ent->NbLevelToLayerDefs(), i = 1; i <= num; i++)
  {
    IW.Send(ent->ExchangeFileLevelNumber(i));
    IW.Send(ent->NativeLevel(i));
    IW.Send(ent->PhysicalLayerNumber(i));
    IW.Send(ent->ExchangeFileLevelIdent(i));
  }
}

void IGESAppli_ToolLevelToPWBLayerMap::OwnShared(
  const occ::handle<IGESAppli_LevelToPWBLayerMap>& /* ent */,
  Interface_EntityIterator& /* iter */) const
{
}

void IGESAppli_ToolLevelToPWBLayerMap::OwnCopy(
  const occ::handle<IGESAppli_LevelToPWBLayerMap>& another,
  const occ::handle<IGESAppli_LevelToPWBLayerMap>& ent,
  Interface_CopyTool& /* TC */) const
{
  int                                   tempNbPropertyValues = another->NbPropertyValues();
  int                                   num                  = another->NbLevelToLayerDefs();
  occ::handle<NCollection_HArray1<int>> tempExchangeFileLevelNumber =
    new NCollection_HArray1<int>(1, num);
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> tempNativeLevel =
    new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, num);
  occ::handle<NCollection_HArray1<int>> tempPhysicalLayerNumber =
    new NCollection_HArray1<int>(1, num);
  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>
    tempExchangeFileLevelIdent =
      new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, num);
  for (int i = 1; i <= num; i++)
  {
    tempExchangeFileLevelNumber->SetValue(i, another->ExchangeFileLevelNumber(i));
    tempNativeLevel->SetValue(i, new TCollection_HAsciiString(another->NativeLevel(i)));
    tempPhysicalLayerNumber->SetValue(i, another->PhysicalLayerNumber(i));
    tempExchangeFileLevelIdent->SetValue(
      i,
      new TCollection_HAsciiString(another->ExchangeFileLevelIdent(i)));
  }
  ent->Init(tempNbPropertyValues,
            tempExchangeFileLevelNumber,
            tempNativeLevel,
            tempPhysicalLayerNumber,
            tempExchangeFileLevelIdent);
}

IGESData_DirChecker IGESAppli_ToolLevelToPWBLayerMap::DirChecker(
  const occ::handle<IGESAppli_LevelToPWBLayerMap>& /* ent */) const
{
  IGESData_DirChecker DC(406, 24);
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

void IGESAppli_ToolLevelToPWBLayerMap::OwnCheck(
  const occ::handle<IGESAppli_LevelToPWBLayerMap>& /* ent */,
  const Interface_ShareTool&,
  occ::handle<Interface_Check>& /* ach */) const
{
}

void IGESAppli_ToolLevelToPWBLayerMap::OwnDump(const occ::handle<IGESAppli_LevelToPWBLayerMap>& ent,
                                               const IGESData_IGESDumper& /* dumper */,
                                               Standard_OStream& S,
                                               const int         level) const
{
  int i, num;
  S << "IGESAppli_LevelToPWBLayerMap\n";
  S << "Number of property values : " << ent->NbPropertyValues() << "\n";
  S << "Exchange File Level Number :\n";
  S << "Native Level Identification :\n";
  S << "Physical Layer Number :\n";
  S << "Exchange File Level Identification : ";
  IGESData_DumpStrings(S, -level, 1, ent->NbLevelToLayerDefs(), ent->ExchangeFileLevelIdent);
  S << "\n";
  if (level > 4)
    for (num = ent->NbLevelToLayerDefs(), i = 1; i <= num; i++)
    {
      S << "[" << i << "]:\n";
      S << "Exchange File Level Number : " << ent->ExchangeFileLevelNumber(i) << "\n";
      S << "Native Level Identification : ";
      IGESData_DumpString(S, ent->NativeLevel(i));
      S << "\n";
      S << "Physical Layer Number : " << ent->PhysicalLayerNumber(i) << "\n";
      S << "Exchange File Level Identification : ";
      IGESData_DumpString(S, ent->ExchangeFileLevelIdent(i));
      S << "\n";
    }
}
