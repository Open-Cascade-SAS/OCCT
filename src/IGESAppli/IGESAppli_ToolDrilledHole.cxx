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

#include <IGESAppli_DrilledHole.hxx>
#include <IGESAppli_ToolDrilledHole.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_LevelListEntity.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>

IGESAppli_ToolDrilledHole::IGESAppli_ToolDrilledHole ()    {  }


void  IGESAppli_ToolDrilledHole::ReadOwnParams
  (const Handle(IGESAppli_DrilledHole)& ent,
   const Handle(IGESData_IGESReaderData)& /*IR*/, IGESData_ParamReader& PR) const
{
  Standard_Integer tempNbPropertyValues;
  Standard_Real    tempDrillDiaSize;
  Standard_Real    tempFinishDiaSize;
  Standard_Integer tempPlatingFlag;
  Standard_Integer tempNbLowerLayer;
  Standard_Integer tempNbHigherLayer;
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadInteger(PR.Current(),"No. of Property values",tempNbPropertyValues);
  PR.ReadReal(PR.Current(),"Drill diameter size",tempDrillDiaSize);
  PR.ReadReal(PR.Current(),"Finish diameter size",tempFinishDiaSize);
  PR.ReadInteger(PR.Current(),"Plating Flag",tempPlatingFlag);
  PR.ReadInteger(PR.Current(),"Lower numbered layer", tempNbLowerLayer);
  PR.ReadInteger(PR.Current(),"Higher numbered layer", tempNbHigherLayer);

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(tempNbPropertyValues,tempDrillDiaSize,tempFinishDiaSize,
	    tempPlatingFlag, tempNbLowerLayer,tempNbHigherLayer);
}

void  IGESAppli_ToolDrilledHole::WriteOwnParams
  (const Handle(IGESAppli_DrilledHole)& ent, IGESData_IGESWriter& IW) const
{
  IW.Send(ent->NbPropertyValues());
  IW.Send(ent->DrillDiaSize());
  IW.Send(ent->FinishDiaSize());
  IW.SendBoolean(ent->IsPlating());
  IW.Send(ent->NbLowerLayer());
  IW.Send(ent->NbHigherLayer());
}

void  IGESAppli_ToolDrilledHole::OwnShared
  (const Handle(IGESAppli_DrilledHole)& /*ent*/, Interface_EntityIterator& /*iter*/) const
{
}

void  IGESAppli_ToolDrilledHole::OwnCopy
  (const Handle(IGESAppli_DrilledHole)& another,
   const Handle(IGESAppli_DrilledHole)& ent, Interface_CopyTool& /*TC*/) const
{
  ent->Init
    (5,another->DrillDiaSize(),another->FinishDiaSize(),
     (another->IsPlating() ? 1 : 0),
     another->NbLowerLayer(),another->NbHigherLayer());
}


Standard_Boolean  IGESAppli_ToolDrilledHole::OwnCorrect
  (const Handle(IGESAppli_DrilledHole)& ent) const
{
  Standard_Boolean res = (ent->NbPropertyValues() != 5);
  if (res) ent->Init
    (5,ent->DrillDiaSize(),ent->FinishDiaSize(),(ent->IsPlating() ? 1 : 0),
     ent->NbLowerLayer(),ent->NbHigherLayer());
  if (ent->SubordinateStatus() != 0) {
    Handle(IGESData_LevelListEntity) nulevel;
    ent->InitLevel(nulevel,0);
    res = Standard_True;
  }   // NbPropertyvalues = 5 + RAZ level selon subordinate
  return res;
}

IGESData_DirChecker  IGESAppli_ToolDrilledHole::DirChecker
  (const Handle(IGESAppli_DrilledHole)& /*ent*/) const
{
  IGESData_DirChecker DC(406,6);  //Form no = 6 & Type = 406
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.BlankStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void  IGESAppli_ToolDrilledHole::OwnCheck
  (const Handle(IGESAppli_DrilledHole)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const
{
  if (ent->SubordinateStatus() != 0)
    if (ent->DefLevel() != IGESData_DefOne &&
	ent->DefLevel() != IGESData_DefSeveral)
      ach->AddFail("Level type : Not value/reference");
  if (ent->NbPropertyValues() != 5)
    ach->AddFail("Number of Property Values != 5");
}

void  IGESAppli_ToolDrilledHole::OwnDump
  (const Handle(IGESAppli_DrilledHole)& ent, const IGESData_IGESDumper& /*dumper*/,
   const Handle(Message_Messenger)& S, const Standard_Integer /*level*/) const
{
  S << "IGESAppli_DrilledHole" << endl;

  S << "Number of property values : " << ent->NbPropertyValues() << endl;
  S << "Drill  diameter size :" << ent->DrillDiaSize() << "  ";
  S << "Finish diameter size : "  << ent->FinishDiaSize() << endl;
  S << "Plating indication flag : ";
  if (!ent->IsPlating())   S << "NO"  << "  -  ";
  else                     S << "YES  -  ";
  S << "Lower Numbered Layer  : " << ent->NbLowerLayer() << "  ";
  S << "Higher Numbered Layer : " << ent->NbHigherLayer() << endl;
}
