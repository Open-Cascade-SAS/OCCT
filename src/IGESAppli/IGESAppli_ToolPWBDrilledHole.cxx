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

#include <IGESAppli_PWBDrilledHole.hxx>
#include <IGESAppli_ToolPWBDrilledHole.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>

IGESAppli_ToolPWBDrilledHole::IGESAppli_ToolPWBDrilledHole ()    {  }


void  IGESAppli_ToolPWBDrilledHole::ReadOwnParams
  (const Handle(IGESAppli_PWBDrilledHole)& ent,
   const Handle(IGESData_IGESReaderData)& /*IR*/, IGESData_ParamReader& PR)  const
{
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed
  Standard_Integer tempNbPropertyValues;
  Standard_Real tempDrillDiameter;
  Standard_Real tempFinishDiameter;
  Standard_Integer tempFunctionCode;

  //szv#4:S4163:12Mar99 `st=` not needed
  if (PR.DefinedElseSkip())
    PR.ReadInteger(PR.Current(), "Number of property values", tempNbPropertyValues);
  else
    tempNbPropertyValues = 3;

  PR.ReadReal(PR.Current(), "Drill Diameter Size", tempDrillDiameter);
  PR.ReadReal(PR.Current(), "Finish Diameter Size", tempFinishDiameter);
  PR.ReadInteger(PR.Current(), "Drilled Hole Function Code", tempFunctionCode);
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(tempNbPropertyValues, tempDrillDiameter, tempFinishDiameter, tempFunctionCode);
}

void  IGESAppli_ToolPWBDrilledHole::WriteOwnParams
  (const Handle(IGESAppli_PWBDrilledHole)& ent, IGESData_IGESWriter& IW) const
{
  IW.Send(ent->NbPropertyValues());
  IW.Send(ent->DrillDiameterSize());
  IW.Send(ent->FinishDiameterSize());
  IW.Send(ent->FunctionCode());
}

void  IGESAppli_ToolPWBDrilledHole::OwnShared
  (const Handle(IGESAppli_PWBDrilledHole)& /*ent*/, Interface_EntityIterator& /*iter*/) const
{
}

void  IGESAppli_ToolPWBDrilledHole::OwnCopy
  (const Handle(IGESAppli_PWBDrilledHole)& another,
   const Handle(IGESAppli_PWBDrilledHole)& ent, Interface_CopyTool& /*TC*/) const
{
  ent->Init (3,another->DrillDiameterSize(),another->FinishDiameterSize(),
	     another->FunctionCode());
}

Standard_Boolean  IGESAppli_ToolPWBDrilledHole::OwnCorrect
  (const Handle(IGESAppli_PWBDrilledHole)& ent) const
{
  Standard_Boolean res = (ent->NbPropertyValues() != 3);
  if (res) ent->Init
    (3,ent->DrillDiameterSize(),ent->FinishDiameterSize(),ent->FunctionCode());
//     nbpropertyvalues=3
  return res;
}

IGESData_DirChecker  IGESAppli_ToolPWBDrilledHole::DirChecker
  (const Handle(IGESAppli_PWBDrilledHole)& /*ent*/) const
{
  IGESData_DirChecker DC(406, 26);
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

void  IGESAppli_ToolPWBDrilledHole::OwnCheck
  (const Handle(IGESAppli_PWBDrilledHole)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const
{
  if (ent->NbPropertyValues() != 3)
    ach->AddFail("Number of property values != 3");
  if ( (ent->FunctionCode() < 1) ||
      ((ent->FunctionCode() > 5) && (ent->FunctionCode() < 5001)) ||
      (ent->FunctionCode() > 9999))
    ach->AddFail("Drilled Hole Function Code != 1-5,5001-9999");
}

void  IGESAppli_ToolPWBDrilledHole::OwnDump
  (const Handle(IGESAppli_PWBDrilledHole)& ent, const IGESData_IGESDumper& /*dumper*/,
   const Handle(Message_Messenger)& S, const Standard_Integer /*level*/) const
{
  S << "IGESAppli_PWBDrilledHole" << endl;
  S << "Number of property values : " << ent->NbPropertyValues() << endl;
  S << "Drill Diameter Size  : " << ent->DrillDiameterSize() << endl;
  S << "Finish Diameter Size : " << ent->FinishDiameterSize() << endl;
  S << "Drilled Hole Function Code : " << ent->FunctionCode() << endl;
}
