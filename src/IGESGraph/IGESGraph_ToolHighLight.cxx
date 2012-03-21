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

#include <IGESGraph_ToolHighLight.ixx>
#include <IGESData_ParamCursor.hxx>


IGESGraph_ToolHighLight::IGESGraph_ToolHighLight ()    {  }


void IGESGraph_ToolHighLight::ReadOwnParams
  (const Handle(IGESGraph_HighLight)& ent,
   const Handle(IGESData_IGESReaderData)& /*IR*/, IGESData_ParamReader& PR) const
{ 
  Standard_Integer nbPropertyValues;
  Standard_Integer highLightStatus;

  // Reading nbPropertyValues(Integer)
  PR.ReadInteger(PR.Current(), "No. of property values", nbPropertyValues);
  if (nbPropertyValues != 1)
    PR.AddFail("No. of Property values : Value is not 1");

  if (PR.DefinedElseSkip())
    // Reading highLightStatus(Integer)
    PR.ReadInteger (PR.Current(), "Highlight flag", highLightStatus);
  else
    highLightStatus = 0; // Default Value

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(nbPropertyValues, highLightStatus);
}

void IGESGraph_ToolHighLight::WriteOwnParams
  (const Handle(IGESGraph_HighLight)& ent, IGESData_IGESWriter& IW)  const
{ 
  IW.Send( ent->NbPropertyValues() );
  IW.Send( ent->HighLightStatus() );
}

void  IGESGraph_ToolHighLight::OwnShared
  (const Handle(IGESGraph_HighLight)& /*ent*/, Interface_EntityIterator& /*iter*/) const
{
}

void IGESGraph_ToolHighLight::OwnCopy
  (const Handle(IGESGraph_HighLight)& another,
   const Handle(IGESGraph_HighLight)& ent, Interface_CopyTool& /*TC*/) const
{
  ent->Init(1,another->HighLightStatus());
}

Standard_Boolean  IGESGraph_ToolHighLight::OwnCorrect
  (const Handle(IGESGraph_HighLight)& ent) const
{
  Standard_Boolean res = (ent->NbPropertyValues() != 1);
  if (res) ent->Init(1,ent->HighLightStatus());    // nbpropertyvalues=1
  return res;
}

IGESData_DirChecker IGESGraph_ToolHighLight::DirChecker
  (const Handle(IGESGraph_HighLight)& /*ent*/)  const
{ 
  IGESData_DirChecker DC (406, 20);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESGraph_ToolHighLight::OwnCheck
  (const Handle(IGESGraph_HighLight)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach)  const
{
  if (ent->NbPropertyValues() != 1)
    ach->AddFail("No. of Property values : Value != 1");
}

void IGESGraph_ToolHighLight::OwnDump
  (const Handle(IGESGraph_HighLight)& ent, const IGESData_IGESDumper& /*dumper*/,
   const Handle(Message_Messenger)& S, const Standard_Integer /*level*/)  const
{
  S << "IGESGraph_HighLight" << endl;

  S << "No. of property values : " << ent->NbPropertyValues() << endl;
  S << "Highlight Status : " << ent->HighLightStatus() << endl;
  S << endl;
}
