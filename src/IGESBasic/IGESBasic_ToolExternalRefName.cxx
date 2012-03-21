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

#include <IGESBasic_ToolExternalRefName.ixx>
#include <IGESData_ParamCursor.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Interface_Macros.hxx>
#include <IGESData_Dump.hxx>


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
