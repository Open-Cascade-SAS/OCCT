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
#include <IGESGraph_Pick.hxx>
#include <IGESGraph_ToolPick.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>

IGESGraph_ToolPick::IGESGraph_ToolPick ()    {  }


void IGESGraph_ToolPick::ReadOwnParams
  (const Handle(IGESGraph_Pick)& ent,
   const Handle(IGESData_IGESReaderData)& /*IR*/, IGESData_ParamReader& PR) const
{ 
  Standard_Integer nbPropertyValues;
  Standard_Integer pickStatus; 

  // Reading nbPropertyValues(Integer)
  PR.ReadInteger(PR.Current(), "No. of property values", nbPropertyValues);
  if (nbPropertyValues != 1)
    PR.AddFail("No. of Property values : Value is not 1");

  if (PR.DefinedElseSkip())
    // Reading pickStatus(Integer)
    PR.ReadInteger( PR.Current(), "Pick Flag", pickStatus);
  else
    pickStatus = 0; // Default Value

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(nbPropertyValues, pickStatus);
}

void IGESGraph_ToolPick::WriteOwnParams
  (const Handle(IGESGraph_Pick)& ent, IGESData_IGESWriter& IW)  const
{ 
  IW.Send( ent->NbPropertyValues() );
  IW.Send( ent->PickFlag() );
}

void  IGESGraph_ToolPick::OwnShared
  (const Handle(IGESGraph_Pick)& /*ent*/, Interface_EntityIterator& /*iter*/) const
{
}

void IGESGraph_ToolPick::OwnCopy
  (const Handle(IGESGraph_Pick)& another,
   const Handle(IGESGraph_Pick)& ent, Interface_CopyTool& /*TC*/) const
{
  ent->Init(1,another->PickFlag());
}

Standard_Boolean  IGESGraph_ToolPick::OwnCorrect
  (const Handle(IGESGraph_Pick)& ent) const
{
  Standard_Boolean res = (ent->NbPropertyValues() != 1);
  if (res) ent->Init(1,ent->PickFlag());    // nbpropertyvalues=1
  return res;
}

IGESData_DirChecker IGESGraph_ToolPick::DirChecker
  (const Handle(IGESGraph_Pick)& /*ent*/)  const
{ 
  IGESData_DirChecker DC (406, 21);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESGraph_ToolPick::OwnCheck
  (const Handle(IGESGraph_Pick)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach)  const
{
  if (ent->NbPropertyValues() != 1)
    ach->AddFail("No. of Property values : Value != 1");
  if ( (ent->PickFlag() != 0) && (ent->PickFlag() != 1) )
    ach->AddFail("Pick Flag : Value != 0/1");
}

void IGESGraph_ToolPick::OwnDump
  (const Handle(IGESGraph_Pick)& ent, const IGESData_IGESDumper& /*dumper*/,
   const Handle(Message_Messenger)& S, const Standard_Integer /*level*/)  const
{
  S << "IGESGraph_Pick" << endl;

  S << "No. of property values : " << ent->NbPropertyValues() << endl;
  S << "Pick flag : " << ent->PickFlag();
  S << (ent->PickFlag() == 0 ? " NO" : " YES" );
  S << endl;
}
