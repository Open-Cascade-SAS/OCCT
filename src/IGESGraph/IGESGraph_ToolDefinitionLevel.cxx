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
#include <IGESGraph_DefinitionLevel.hxx>
#include <IGESGraph_ToolDefinitionLevel.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <TColStd_HArray1OfInteger.hxx>

IGESGraph_ToolDefinitionLevel::IGESGraph_ToolDefinitionLevel ()    {  }


void IGESGraph_ToolDefinitionLevel::ReadOwnParams
  (const Handle(IGESGraph_DefinitionLevel)& ent,
   const Handle(IGESData_IGESReaderData)& /*IR*/, IGESData_ParamReader& PR) const
{ 
  //Standard_Boolean st; //szv#4:S4163:12Mar99 moved down
  Standard_Integer nbval;

  Handle(TColStd_HArray1OfInteger) levelNumbers;

  // Reading nbval(No. of Property Values)
  Standard_Boolean st = PR.ReadInteger(PR.Current(), "No. of Property Values", nbval);
  if (st && nbval > 0)
    {
      // Reading levelNumbers(HArray1OfInteger)
//      levelNumbers = new TColStd_HArray1OfInteger(1, nbval);   done by :
      PR.ReadInts(PR.CurrentList(nbval), "array levelNumbers", levelNumbers, 1); //szv#4:S4163:12Mar99 `st=` not needed
    }
  else  PR.AddFail("No. of Property Values : Not Positive");

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(levelNumbers);
}

void IGESGraph_ToolDefinitionLevel::WriteOwnParams
  (const Handle(IGESGraph_DefinitionLevel)& ent, IGESData_IGESWriter& IW)  const
{ 
  Standard_Integer Up  = ent->NbPropertyValues();
  IW.Send( Up );
  for ( Standard_Integer i = 1; i <= Up; i++)
    IW.Send( ent->LevelNumber(i) );
}

void  IGESGraph_ToolDefinitionLevel::OwnShared
  (const Handle(IGESGraph_DefinitionLevel)& /*ent*/, Interface_EntityIterator& /*iter*/) const
{
}

void IGESGraph_ToolDefinitionLevel::OwnCopy
  (const Handle(IGESGraph_DefinitionLevel)& another,
   const Handle(IGESGraph_DefinitionLevel)& ent, Interface_CopyTool& /*TC*/) const
{ 
  Standard_Integer nbval;
  Handle(TColStd_HArray1OfInteger) levelNumbers; 
 
  nbval = another->NbPropertyValues();
 
  levelNumbers = new TColStd_HArray1OfInteger(1, nbval);
  for (Standard_Integer i = 1; i <= nbval; i++)
    levelNumbers->SetValue( i, another->LevelNumber(i) );

  ent->Init(levelNumbers);
}

IGESData_DirChecker IGESGraph_ToolDefinitionLevel::DirChecker
  (const Handle(IGESGraph_DefinitionLevel)& /*ent*/)  const
{ 
  IGESData_DirChecker DC (406, 1);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESGraph_ToolDefinitionLevel::OwnCheck
  (const Handle(IGESGraph_DefinitionLevel)& /*ent*/,
   const Interface_ShareTool& , Handle(Interface_Check)& /*ach*/)  const
{
}

void IGESGraph_ToolDefinitionLevel::OwnDump
  (const Handle(IGESGraph_DefinitionLevel)& ent, const IGESData_IGESDumper& /*dumper*/,
   const Handle(Message_Messenger)& S, const Standard_Integer level)  const
{
  S << "IGESGraph_DefinitionLevel" << endl;

  S << "Level Numbers : ";
  IGESData_DumpVals(S ,level,1, ent->NbPropertyValues(),ent->LevelNumber);
  S << endl;
}

