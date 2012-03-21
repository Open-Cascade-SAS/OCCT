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

#include <IGESGraph_ToolPick.ixx>
#include <IGESData_ParamCursor.hxx>


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
