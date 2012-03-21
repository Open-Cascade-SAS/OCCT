// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <Interface_Macros.hxx>
#include <IGESGraph_ToolDefinitionLevel.ixx>
#include <IGESData_ParamCursor.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <IGESData_Dump.hxx>


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

