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
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDimen_DimensionUnits.hxx>
#include <IGESDimen_ToolDimensionUnits.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <TCollection_HAsciiString.hxx>

IGESDimen_ToolDimensionUnits::IGESDimen_ToolDimensionUnits ()    {  }


void  IGESDimen_ToolDimensionUnits::ReadOwnParams
  (const Handle(IGESDimen_DimensionUnits)& ent,
   const Handle(IGESData_IGESReaderData)& /* IR */, IGESData_ParamReader& PR) const
{
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed
  Standard_Integer tempNbProps;
  Standard_Integer tempSecondDimenPos;
  Standard_Integer tempUnitsIndic;
  Standard_Integer tempCharSet;
  Standard_Integer tempFracFlag;
  Standard_Integer tempPrecision;
  Handle(TCollection_HAsciiString) tempFormatString;

  if (PR.DefinedElseSkip())
    PR.ReadInteger(PR.Current(), "Number of Properties", tempNbProps); //szv#4:S4163:12Mar99 `st=` not needed
  else
    tempNbProps = 6;

  PR.ReadInteger(PR.Current(), "Secondary Dimension Position",
		 tempSecondDimenPos); //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadInteger(PR.Current(), "Units Indicator", tempUnitsIndic); //szv#4:S4163:12Mar99 `st=` not needed
  if (PR.DefinedElseSkip())
    PR.ReadInteger(PR.Current(), "Character Set", tempCharSet); //szv#4:S4163:12Mar99 `st=` not needed
  else
    tempCharSet = 1;

  PR.ReadText(PR.Current(), "Format String", tempFormatString); //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadInteger(PR.Current(), "Fraction Flag", tempFracFlag); //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadInteger(PR.Current(), "Precision", tempPrecision); //szv#4:S4163:12Mar99 `st=` not needed

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init
    (tempNbProps, tempSecondDimenPos, tempUnitsIndic, tempCharSet,
     tempFormatString, tempFracFlag, tempPrecision);
}

void  IGESDimen_ToolDimensionUnits::WriteOwnParams
  (const Handle(IGESDimen_DimensionUnits)& ent, IGESData_IGESWriter& IW) const
{
  IW.Send(ent->NbPropertyValues());
  IW.Send(ent->SecondaryDimenPosition());
  IW.Send(ent->UnitsIndicator());
  IW.Send(ent->CharacterSet());
  IW.Send(ent->FormatString());
  IW.Send(ent->FractionFlag());
  IW.Send(ent->PrecisionOrDenominator());
}

void  IGESDimen_ToolDimensionUnits::OwnShared
  (const Handle(IGESDimen_DimensionUnits)& /* ent */, Interface_EntityIterator& /* iter */) const
{
}

void  IGESDimen_ToolDimensionUnits::OwnCopy
  (const Handle(IGESDimen_DimensionUnits)& another,
   const Handle(IGESDimen_DimensionUnits)& ent, Interface_CopyTool& /* TC */) const
{
  Standard_Integer tempNbProps        = another->NbPropertyValues();
  Standard_Integer tempSecondDimenPos = another->SecondaryDimenPosition();
  Standard_Integer tempUnitsIndic     = another->UnitsIndicator();
  Standard_Integer tempCharSet        = another->CharacterSet();
  Handle(TCollection_HAsciiString) tempFormatString =
    new TCollection_HAsciiString(another->FormatString());
  Standard_Integer tempFracFlag       = another->FractionFlag();
  Standard_Integer tempPrecision      = another->PrecisionOrDenominator();
  ent->Init (tempNbProps, tempSecondDimenPos, tempUnitsIndic, tempCharSet,
	     tempFormatString, tempFracFlag, tempPrecision);
}

Standard_Boolean  IGESDimen_ToolDimensionUnits::OwnCorrect
  (const Handle(IGESDimen_DimensionUnits)& ent) const
{
  Standard_Boolean res = (ent->NbPropertyValues() != 6);
  if (res) ent->Init
    (6,ent->SecondaryDimenPosition(),ent->UnitsIndicator(),ent->CharacterSet(),
     ent->FormatString(),ent->FractionFlag(),ent->PrecisionOrDenominator());
  return res;    // nbpropertyvalues = 6
}

IGESData_DirChecker  IGESDimen_ToolDimensionUnits::DirChecker
  (const Handle(IGESDimen_DimensionUnits)& /* ent */) const
{
  IGESData_DirChecker DC(406, 28);
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.SubordinateStatusRequired(2);
  DC.UseFlagRequired(2);
  DC.HierarchyStatusIgnored();
  return DC;
}

void  IGESDimen_ToolDimensionUnits::OwnCheck
  (const Handle(IGESDimen_DimensionUnits)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const
{
  if (ent->NbPropertyValues() != 6)
    ach->AddFail("Number of properties != 6");
  if (ent->SecondaryDimenPosition() < 0 || ent->SecondaryDimenPosition() > 4)
    ach->AddFail("Secondary Dimension Position != 0-4");
  if ( (ent->CharacterSet() != 1) &&
      ((ent->CharacterSet() < 1001) || (ent->CharacterSet() > 1003)) )
    ach->AddFail("Character Set != 1,1001-1003");
  if ((ent->FractionFlag() != 0) && (ent->FractionFlag() != 1))
    ach->AddFail("Fraction Flag != 0,1");
}

void  IGESDimen_ToolDimensionUnits::OwnDump
  (const Handle(IGESDimen_DimensionUnits)& ent, const IGESData_IGESDumper& /* dumper */,
   const Handle(Message_Messenger)& S, const Standard_Integer /* level */) const
{
  S << "IGESDimen_DimensionUnits" << endl;
  S << "Number of property values : " << ent->NbPropertyValues() << endl;
  S << "Secondary Dimension Position : " << ent->SecondaryDimenPosition() << endl;
  S << "Units Indicator : " << ent->UnitsIndicator() << endl;
  S << "Character Set   : " << ent->CharacterSet() << endl;
  S << "Format String   : ";
  IGESData_DumpString(S,ent->FormatString());
  S << endl;
  S << "Fraction Flag   : " << ent->FractionFlag();
  if (ent->FractionFlag() == 0) S << " Decimal  , Precision   : ";
  else                          S << " Fraction , Denominator : ";
  S << ent->PrecisionOrDenominator() << endl;
}
