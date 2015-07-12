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

#include <IGESBasic_ExternalRefName.hxx>
#include <IGESBasic_ToolExternalRefName.hxx>
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
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <TCollection_HAsciiString.hxx>

IGESBasic_ToolExternalRefName::IGESBasic_ToolExternalRefName ()    {  }


void  IGESBasic_ToolExternalRefName::ReadOwnParams
  (const Handle(IGESBasic_ExternalRefName)& ent,
   const Handle(IGESData_IGESReaderData)& /* IR */, IGESData_ParamReader& PR) const
{
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed
  Handle(TCollection_HAsciiString) tempExtRefEntitySymbName;
  PR.ReadText(PR.Current(), "External Reference Symbolic Name",
	      tempExtRefEntitySymbName); //szv#4:S4163:12Mar99 `st=` not needed

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(tempExtRefEntitySymbName);
}

void  IGESBasic_ToolExternalRefName::WriteOwnParams
  (const Handle(IGESBasic_ExternalRefName)& ent, IGESData_IGESWriter& IW) const
{
  IW.Send(ent->ReferenceName());
}

void  IGESBasic_ToolExternalRefName::OwnShared
  (const Handle(IGESBasic_ExternalRefName)& /* ent */, Interface_EntityIterator& /* iter */) const
{
}

void  IGESBasic_ToolExternalRefName::OwnCopy
  (const Handle(IGESBasic_ExternalRefName)& another,
   const Handle(IGESBasic_ExternalRefName)& ent, Interface_CopyTool& /* TC */) const
{
  Handle(TCollection_HAsciiString) tempRefName =
    new TCollection_HAsciiString(another->ReferenceName());
  ent->Init(tempRefName);
}

IGESData_DirChecker  IGESBasic_ToolExternalRefName::DirChecker
  (const Handle(IGESBasic_ExternalRefName)& /* ent */ ) const
{
  IGESData_DirChecker DC(416, 3);
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusRequired(0);
  DC.HierarchyStatusRequired(0);
  return DC;
}

void  IGESBasic_ToolExternalRefName::OwnCheck
  (const Handle(IGESBasic_ExternalRefName)& /* ent */,
   const Interface_ShareTool& , Handle(Interface_Check)& /* ach */) const
{
}

void  IGESBasic_ToolExternalRefName::OwnDump
  (const Handle(IGESBasic_ExternalRefName)& ent, const IGESData_IGESDumper& /* dumper */,
   const Handle(Message_Messenger)& S, const Standard_Integer /* level */) const
{
  S << "IGESBasic_ExternalRefName" << endl;
  S << "External Reference Symbolic Name : ";
  IGESData_DumpString(S,ent->ReferenceName());
  S << endl;
}
