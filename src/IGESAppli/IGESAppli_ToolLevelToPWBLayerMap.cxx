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
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfInteger.hxx>

IGESAppli_ToolLevelToPWBLayerMap::IGESAppli_ToolLevelToPWBLayerMap ()    {  }


void  IGESAppli_ToolLevelToPWBLayerMap::ReadOwnParams
  (const Handle(IGESAppli_LevelToPWBLayerMap)& ent,
   const Handle(IGESData_IGESReaderData)& /* IR */, IGESData_ParamReader& PR) const
{
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed
  Standard_Integer num, i;
  Standard_Integer tempNbPropertyValues;
  Handle(TColStd_HArray1OfInteger) tempExchangeFileLevelNumber;
  Handle(Interface_HArray1OfHAsciiString) tempNativeLevel;
  Handle(TColStd_HArray1OfInteger) tempPhysicalLayerNumber;
  Handle(Interface_HArray1OfHAsciiString) tempExchangeFileLevelIdent;
  //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadInteger(PR.Current(), "Number of property values", tempNbPropertyValues);
  if (!PR.ReadInteger(PR.Current(), "Number of definitions", num)) num = 0;
  if (num > 0) {
    tempExchangeFileLevelNumber =
      new TColStd_HArray1OfInteger(1, num);
    tempNativeLevel = new Interface_HArray1OfHAsciiString(1, num);
    tempPhysicalLayerNumber = new TColStd_HArray1OfInteger(1, num);
    tempExchangeFileLevelIdent = new Interface_HArray1OfHAsciiString(1, num);
  }
  else PR.AddFail("Number of definitions: Not Positive");

  if (!tempExchangeFileLevelNumber.IsNull() &&
      !tempNativeLevel.IsNull() &&
      !tempPhysicalLayerNumber.IsNull() &&
      !tempExchangeFileLevelIdent.IsNull() )
    for ( i = 1; i <= num; i++ )
      {
	Standard_Integer tempEFLN;
	//szv#4:S4163:12Mar99 moved in if
	if (PR.ReadInteger(PR.Current(), "Exchange File Level Number", tempEFLN))
	  tempExchangeFileLevelNumber->SetValue(i, tempEFLN);
	Handle(TCollection_HAsciiString) tempNL;
	if (PR.ReadText(PR.Current(), "Native Level Identification", tempNL))
	  tempNativeLevel->SetValue(i, tempNL);
	Standard_Integer tempPLN;
	if (PR.ReadInteger(PR.Current(), "Physical Layer Number", tempPLN))
	  tempPhysicalLayerNumber->SetValue(i, tempPLN);
	Handle(TCollection_HAsciiString) tempEFLI;
	if (PR.ReadText(PR.Current(), "Exchange File Level Identification", tempEFLI))
	  tempExchangeFileLevelIdent->SetValue(i, tempEFLI);
      }
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(tempNbPropertyValues, tempExchangeFileLevelNumber,
	    tempNativeLevel, tempPhysicalLayerNumber, tempExchangeFileLevelIdent);
}

void  IGESAppli_ToolLevelToPWBLayerMap::WriteOwnParams
  (const Handle(IGESAppli_LevelToPWBLayerMap)& ent, IGESData_IGESWriter& IW) const
{
  Standard_Integer i, num;
  IW.Send(ent->NbPropertyValues());
  IW.Send(ent->NbLevelToLayerDefs());
  for ( num = ent->NbLevelToLayerDefs(), i = 1; i <= num; i++ )
    {
      IW.Send(ent->ExchangeFileLevelNumber(i));
      IW.Send(ent->NativeLevel(i));
      IW.Send(ent->PhysicalLayerNumber(i));
      IW.Send(ent->ExchangeFileLevelIdent(i));
    }
}

void  IGESAppli_ToolLevelToPWBLayerMap::OwnShared
  (const Handle(IGESAppli_LevelToPWBLayerMap)& /* ent */, Interface_EntityIterator& /* iter */) const
{
}

void  IGESAppli_ToolLevelToPWBLayerMap::OwnCopy
  (const Handle(IGESAppli_LevelToPWBLayerMap)& another,
   const Handle(IGESAppli_LevelToPWBLayerMap)& ent, Interface_CopyTool& /* TC */) const
{
  Standard_Integer tempNbPropertyValues = another->NbPropertyValues();
  Standard_Integer num = another->NbLevelToLayerDefs();
  Handle(TColStd_HArray1OfInteger) tempExchangeFileLevelNumber =
    new TColStd_HArray1OfInteger(1, num);
  Handle(Interface_HArray1OfHAsciiString) tempNativeLevel =
    new Interface_HArray1OfHAsciiString(1, num);
  Handle(TColStd_HArray1OfInteger) tempPhysicalLayerNumber =
    new TColStd_HArray1OfInteger(1, num);
  Handle(Interface_HArray1OfHAsciiString) tempExchangeFileLevelIdent =
    new Interface_HArray1OfHAsciiString(1, num);
  for ( Standard_Integer i = 1; i <= num; i++ )
    {
      tempExchangeFileLevelNumber->SetValue(i,another->ExchangeFileLevelNumber(i));
      tempNativeLevel->SetValue
	(i, new TCollection_HAsciiString(another->NativeLevel(i)));
      tempPhysicalLayerNumber->SetValue(i, another->PhysicalLayerNumber(i));
      tempExchangeFileLevelIdent->SetValue
	(i,new TCollection_HAsciiString
	 (another->ExchangeFileLevelIdent(i)));
    }
  ent->Init (tempNbPropertyValues, tempExchangeFileLevelNumber, tempNativeLevel,
	     tempPhysicalLayerNumber, tempExchangeFileLevelIdent);
}

IGESData_DirChecker  IGESAppli_ToolLevelToPWBLayerMap::DirChecker
  (const Handle(IGESAppli_LevelToPWBLayerMap)& /* ent */ ) const
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

void  IGESAppli_ToolLevelToPWBLayerMap::OwnCheck
  (const Handle(IGESAppli_LevelToPWBLayerMap)& /* ent */,
   const Interface_ShareTool& , Handle(Interface_Check)& /* ach */) const
{
}

void  IGESAppli_ToolLevelToPWBLayerMap::OwnDump
  (const Handle(IGESAppli_LevelToPWBLayerMap)& ent, const IGESData_IGESDumper& /* dumper */,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  Standard_Integer i, num;
  S << "IGESAppli_LevelToPWBLayerMap" << endl;
  S << "Number of property values : " << ent->NbPropertyValues() << endl;
  S << "Exchange File Level Number : " << endl;
  S << "Native Level Identification : " << endl;
  S << "Physical Layer Number : " << endl;
  S << "Exchange File Level Identification : ";
  IGESData_DumpStrings
    (S,-level,1, ent->NbLevelToLayerDefs(),ent->ExchangeFileLevelIdent);
  S << endl;
  if (level > 4)
    for ( num = ent->NbLevelToLayerDefs(), i = 1; i <= num; i++ )
      {
	S << "[" << i << "]: " << endl;
	S << "Exchange File Level Number : "
	  << ent->ExchangeFileLevelNumber(i) << endl;
	S << "Native Level Identification : ";
	IGESData_DumpString(S,ent->NativeLevel(i));
	S << endl;
	S << "Physical Layer Number : " << ent->PhysicalLayerNumber(i) << endl;
	S << "Exchange File Level Identification : ";
	IGESData_DumpString(S,ent->ExchangeFileLevelIdent(i));
	S << endl;
      }
}
