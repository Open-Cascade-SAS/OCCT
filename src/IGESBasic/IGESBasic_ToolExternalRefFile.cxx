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

#include <IGESBasic_ExternalRefFile.hxx>
#include <IGESBasic_ToolExternalRefFile.hxx>
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

IGESBasic_ToolExternalRefFile::IGESBasic_ToolExternalRefFile ()    {  }


void  IGESBasic_ToolExternalRefFile::ReadOwnParams
  (const Handle(IGESBasic_ExternalRefFile)& ent,
   const Handle(IGESData_IGESReaderData)& /* IR */, IGESData_ParamReader& PR) const
{
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed
  Handle(TCollection_HAsciiString) tempExtRefFileIdentifier;
  PR.ReadText(PR.Current(), "External Reference File Identifier",
	      tempExtRefFileIdentifier); //szv#4:S4163:12Mar99 `st=` not needed

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(tempExtRefFileIdentifier);
}

void  IGESBasic_ToolExternalRefFile::WriteOwnParams
  (const Handle(IGESBasic_ExternalRefFile)& ent, IGESData_IGESWriter& IW) const
{
  IW.Send(ent->FileId());
}

void  IGESBasic_ToolExternalRefFile::OwnShared
  (const Handle(IGESBasic_ExternalRefFile)& /* ent */, Interface_EntityIterator& /* iter */) const
{
}

void  IGESBasic_ToolExternalRefFile::OwnCopy
  (const Handle(IGESBasic_ExternalRefFile)& another,
   const Handle(IGESBasic_ExternalRefFile)& ent, Interface_CopyTool& /* TC */) const
{
  Handle(TCollection_HAsciiString) tempFileId =
    new TCollection_HAsciiString(another->FileId());
  ent->Init(tempFileId);
}

IGESData_DirChecker  IGESBasic_ToolExternalRefFile::DirChecker
  (const Handle(IGESBasic_ExternalRefFile)& /* ent */ ) const
{
  IGESData_DirChecker DC(416, 1);
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void  IGESBasic_ToolExternalRefFile::OwnCheck
  (const Handle(IGESBasic_ExternalRefFile)& /* ent */,
   const Interface_ShareTool& , Handle(Interface_Check)& /* ach */) const
{
}

void  IGESBasic_ToolExternalRefFile::OwnDump
  (const Handle(IGESBasic_ExternalRefFile)& ent, const IGESData_IGESDumper& /* dumper */,
   const Handle(Message_Messenger)& S, const Standard_Integer /* level */) const
{
  S << "IGESBasic_ExternalRefFile" << endl;
  S << "External Reference File Identifier : ";
  IGESData_DumpString(S,ent->FileId());
  S << endl;
}
