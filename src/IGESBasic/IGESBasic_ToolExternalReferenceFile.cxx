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

#include <IGESBasic_ExternalReferenceFile.hxx>
#include <IGESBasic_ToolExternalReferenceFile.hxx>
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

IGESBasic_ToolExternalReferenceFile::IGESBasic_ToolExternalReferenceFile () { }


void  IGESBasic_ToolExternalReferenceFile::ReadOwnParams
  (const Handle(IGESBasic_ExternalReferenceFile)& ent,
   const Handle(IGESData_IGESReaderData)& /* IR */, IGESData_ParamReader& PR) const
{
  //Standard_Boolean st; //szv#4:S4163:12Mar99 moved down
  Standard_Integer num;
  Handle(Interface_HArray1OfHAsciiString) tempNames;
  Standard_Boolean st = PR.ReadInteger(PR.Current(), "Number of list entries", num);
  if (st && num > 0) tempNames = new Interface_HArray1OfHAsciiString(1, num);
  else  PR.AddFail("Number of list entries: Not Positive");
  if (!tempNames.IsNull())
    PR.ReadTexts(PR.CurrentList(num), "External Reference Entity", tempNames); //szv#4:S4163:12Mar99 `st=` not needed

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(tempNames);
}

void  IGESBasic_ToolExternalReferenceFile::WriteOwnParams
  (const Handle(IGESBasic_ExternalReferenceFile)& ent, IGESData_IGESWriter& IW) const
{
  Standard_Integer i, num;
  IW.Send(ent->NbListEntries());
  for ( num = ent->NbListEntries(), i = 1; i <= num; i++ )
    IW.Send(ent->Name(i));
}

void  IGESBasic_ToolExternalReferenceFile::OwnShared
  (const Handle(IGESBasic_ExternalReferenceFile)& /* ent */, Interface_EntityIterator& /* iter */) const
{
}

void  IGESBasic_ToolExternalReferenceFile::OwnCopy
  (const Handle(IGESBasic_ExternalReferenceFile)& another,
   const Handle(IGESBasic_ExternalReferenceFile)& ent, Interface_CopyTool& /* TC */) const
{
  Standard_Integer num = another->NbListEntries();
  Handle(Interface_HArray1OfHAsciiString) tempNames =
    new Interface_HArray1OfHAsciiString(1, num);
  for ( Standard_Integer i = 1; i <= num; i++ )
    tempNames->SetValue(i, new TCollection_HAsciiString
			(another->Name(i)));
  ent->Init(tempNames);
}

IGESData_DirChecker  IGESBasic_ToolExternalReferenceFile::DirChecker
  (const Handle(IGESBasic_ExternalReferenceFile)& /* ent */ ) const
{
  IGESData_DirChecker DC(406, 12);
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

void  IGESBasic_ToolExternalReferenceFile::OwnCheck
  (const Handle(IGESBasic_ExternalReferenceFile)& /* ent */,
   const Interface_ShareTool& , Handle(Interface_Check)& /* ach */) const
{
}

void  IGESBasic_ToolExternalReferenceFile::OwnDump
  (const Handle(IGESBasic_ExternalReferenceFile)& ent, const IGESData_IGESDumper& /* dumper */,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  S << "IGESBasic_ExternalReferenceFile" << endl;
  S << "External Reference Names : ";
  IGESData_DumpStrings(S ,level,1, ent->NbListEntries(),ent->Name);
  S << endl;
}
