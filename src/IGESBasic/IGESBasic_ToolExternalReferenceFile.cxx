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

#include <IGESBasic_ToolExternalReferenceFile.ixx>
#include <IGESData_ParamCursor.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <IGESData_Dump.hxx>
#include <Interface_Macros.hxx>


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
