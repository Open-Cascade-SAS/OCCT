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
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDimen_DimensionTolerance.hxx>
#include <IGESDimen_ToolDimensionTolerance.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>

IGESDimen_ToolDimensionTolerance::IGESDimen_ToolDimensionTolerance ()    {  }


void  IGESDimen_ToolDimensionTolerance::ReadOwnParams
  (const Handle(IGESDimen_DimensionTolerance)& ent,
   const Handle(IGESData_IGESReaderData)& /*IR*/, IGESData_ParamReader& PR) const
{
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed
  Standard_Integer tempNbProps;
  Standard_Integer tempSecondTolFlag;
  Standard_Integer tempTolTyp;
  Standard_Integer tempTolPlaceFlag;
  Standard_Real    tempUpperTol;
  Standard_Real    tempLowerTol;
  Standard_Boolean tempSignSupFlag;
  Standard_Integer tempFracFlag;
  Standard_Integer tempPrecision;

  if (PR.DefinedElseSkip())
    PR.ReadInteger(PR.Current(), "Number of properties", tempNbProps); //szv#4:S4163:12Mar99 `st=` not needed
  else
    tempNbProps = 8;

  PR.ReadInteger(PR.Current(), "Secondary Tolerance Flag",
		 tempSecondTolFlag); //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadInteger(PR.Current(), "Tolerance Type", tempTolTyp); //szv#4:S4163:12Mar99 `st=` not needed
  if (PR.DefinedElseSkip())
    PR.ReadInteger(PR.Current(), "Tolerance Placement Flag",
		   tempTolPlaceFlag); //szv#4:S4163:12Mar99 `st=` not needed
  else
    tempTolPlaceFlag = 2;

  PR.ReadReal(PR.Current(), "Upper Tolerance", tempUpperTol); //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadReal(PR.Current(), "Lower Tolerance", tempLowerTol); //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadBoolean(PR.Current(), "Sign Suppression Flag",
		 tempSignSupFlag); //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadInteger(PR.Current(), "Fraction Flag", tempFracFlag); //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadInteger(PR.Current(), "Precision", tempPrecision); //szv#4:S4163:12Mar99 `st=` not needed

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init
    (tempNbProps, tempSecondTolFlag, tempTolTyp, tempTolPlaceFlag,
     tempUpperTol, tempLowerTol, tempSignSupFlag, tempFracFlag, tempPrecision);
}

void  IGESDimen_ToolDimensionTolerance::WriteOwnParams
  (const Handle(IGESDimen_DimensionTolerance)& ent, IGESData_IGESWriter& IW) const
{
  IW.Send(ent->NbPropertyValues());
  IW.Send(ent->SecondaryToleranceFlag());
  IW.Send(ent->ToleranceType());
  IW.Send(ent->TolerancePlacementFlag());
  IW.Send(ent->UpperTolerance());
  IW.Send(ent->LowerTolerance());
  IW.SendBoolean(ent->SignSuppressionFlag());
  IW.Send(ent->FractionFlag());
  IW.Send(ent->Precision());
}

void  IGESDimen_ToolDimensionTolerance::OwnShared
  (const Handle(IGESDimen_DimensionTolerance)& /*ent*/, Interface_EntityIterator& /*iter*/) const
{
}

void  IGESDimen_ToolDimensionTolerance::OwnCopy
  (const Handle(IGESDimen_DimensionTolerance)& another,
   const Handle(IGESDimen_DimensionTolerance)& ent, Interface_CopyTool& /*TC*/) const
{
  ent->Init
    (8,another->SecondaryToleranceFlag(),another->ToleranceType(),
     another->TolerancePlacementFlag(),
     another->UpperTolerance(),another->LowerTolerance(),
     (another->SignSuppressionFlag() ? 1 : 0),
     another->FractionFlag(),another->Precision());
}

Standard_Boolean  IGESDimen_ToolDimensionTolerance::OwnCorrect
  (const Handle(IGESDimen_DimensionTolerance)& ent) const
{
  Standard_Boolean res = (ent->NbPropertyValues() != 8);
  if (res) ent->Init
    (8,ent->SecondaryToleranceFlag(),ent->ToleranceType(),
     ent->TolerancePlacementFlag(),ent->UpperTolerance(),ent->LowerTolerance(),
     (ent->SignSuppressionFlag() ? 1 : 0),
     ent->FractionFlag(),ent->Precision());    // nbpropertyvalues=8
  return res;
}

IGESData_DirChecker  IGESDimen_ToolDimensionTolerance::DirChecker
  (const Handle(IGESDimen_DimensionTolerance)& /*ent*/) const
{
  IGESData_DirChecker DC(406, 29);
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

void  IGESDimen_ToolDimensionTolerance::OwnCheck
  (const Handle(IGESDimen_DimensionTolerance)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const
{
  if (ent->NbPropertyValues() != 8)
    ach->AddFail("Number of properties != 8");
  if ((ent->SecondaryToleranceFlag() < 0) || (ent->SecondaryToleranceFlag() > 2))
    ach->AddFail("Secondary Tolerance Flag != 0-2");
  if ((ent->ToleranceType() < 1) || (ent->ToleranceType() > 10))
    ach->AddFail("Tolerance Type != 1-10");
  if ((ent->TolerancePlacementFlag() < 1) || (ent->TolerancePlacementFlag() > 4))
    ach->AddFail("Tolerance Placement Flag != 1-4");
  if ((ent->FractionFlag() < 0) || (ent->FractionFlag() > 2))
    ach->AddFail("Fraction Flag != 0-2");
}

void  IGESDimen_ToolDimensionTolerance::OwnDump
  (const Handle(IGESDimen_DimensionTolerance)& ent, const IGESData_IGESDumper& /*dumper*/,
   const Handle(Message_Messenger)& S, const Standard_Integer /*level*/) const
{
  S << "IGESDimen_DimensionTolerance" << endl;
  S << "Number of property values : " << ent->NbPropertyValues() << endl;
  S << "Secondary Tolerance Flag : " << ent->SecondaryToleranceFlag() << endl;
  S << "Tolerance Type           : " << ent->ToleranceType() << endl;
  S << "Tolerance Placement Flag : " << ent->TolerancePlacementFlag() << endl;
  S << "Upper Tolerance          : " << ent->UpperTolerance() << endl;
  S << "Lower Tolerance          : " << ent->LowerTolerance() << endl;
  S << "Sign Suppression Flag    : " << ( ent->SignSuppressionFlag() ? "True" : "False" ) << endl;
  S << "Fraction Flag            : " << ent->FractionFlag() << endl;
  S << "Precision                : " << ent->Precision() << endl;
}

