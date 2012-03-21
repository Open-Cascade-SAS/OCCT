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

#include <IGESBasic_ToolName.ixx>
#include <IGESData_ParamCursor.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Interface_Macros.hxx>
#include <IGESData_Dump.hxx>


IGESBasic_ToolName::IGESBasic_ToolName ()    {  }


void  IGESBasic_ToolName::ReadOwnParams
  (const Handle(IGESBasic_Name)& ent,
   const Handle(IGESData_IGESReaderData)& /* IR */, IGESData_ParamReader& PR) const
{
  Standard_Integer tempNbPropertyValues;
  Handle(TCollection_HAsciiString) tempName;
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  PR.ReadInteger(PR.Current(),"Number of property values",tempNbPropertyValues); //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadText(PR.Current(),"Name",tempName); //szv#4:S4163:12Mar99 `st=` not needed

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(tempNbPropertyValues,tempName);
}

void  IGESBasic_ToolName::WriteOwnParams
  (const Handle(IGESBasic_Name)& ent, IGESData_IGESWriter& IW) const
{
  IW.Send(ent->NbPropertyValues());
  IW.Send(ent->Value());
}

void  IGESBasic_ToolName::OwnShared
  (const Handle(IGESBasic_Name)& /* ent */, Interface_EntityIterator& /* iter */) const
{
}

void  IGESBasic_ToolName::OwnCopy
  (const Handle(IGESBasic_Name)& another,
   const Handle(IGESBasic_Name)& ent, Interface_CopyTool& /* TC */) const
{
  Standard_Integer aNbPropertyValues;
  Handle(TCollection_HAsciiString) aName;
  aName = new TCollection_HAsciiString(another->Value());
  aNbPropertyValues = another->NbPropertyValues();
  ent->Init(aNbPropertyValues,aName);
}

Standard_Boolean  IGESBasic_ToolName::OwnCorrect
  (const Handle(IGESBasic_Name)& ent) const
{
  Standard_Boolean res = (ent->NbPropertyValues() != 1);
  if (res) ent->Init(1,ent->Value());
  return res;    // nbpropertyvalues = 1
}

IGESData_DirChecker  IGESBasic_ToolName::DirChecker
  (const Handle(IGESBasic_Name)& /*ent*/ ) const
{
  IGESData_DirChecker DC(406,15);  //Form no = 15 & Type = 406
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.BlankStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void  IGESBasic_ToolName::OwnCheck
  (const Handle(IGESBasic_Name)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const
{
  if (ent->NbPropertyValues() != 1)
    ach->AddFail("Number of Property Values != 1");
}

void  IGESBasic_ToolName::OwnDump
  (const Handle(IGESBasic_Name)& ent, const IGESData_IGESDumper& /* dumper */,
   const Handle(Message_Messenger)& S, const Standard_Integer /* level */) const
{
  S << "IGESBasic_Name" << endl;

  S << "Number of property values : " << ent->NbPropertyValues() << endl;
  S << "Name : ";
  IGESData_DumpString(S,ent->Value());
  S << endl;
}
