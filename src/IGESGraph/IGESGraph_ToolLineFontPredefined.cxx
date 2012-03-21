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

#include <IGESGraph_ToolLineFontPredefined.ixx>
#include <IGESData_ParamCursor.hxx>


IGESGraph_ToolLineFontPredefined::IGESGraph_ToolLineFontPredefined ()    {  }


void IGESGraph_ToolLineFontPredefined::ReadOwnParams
  (const Handle(IGESGraph_LineFontPredefined)& ent,
   const Handle(IGESData_IGESReaderData)& /*IR*/, IGESData_ParamReader& PR) const
{ 
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  Standard_Integer nbPropertyValues;
  Standard_Integer lineFontPatternCode;

  // Reading nbPropertyValues(Integer)
  PR.ReadInteger(PR.Current(), "No. of property values", nbPropertyValues); //szv#4:S4163:12Mar99 `st=` not needed
  if (nbPropertyValues != 1)
    PR.AddFail("No. of Property values : Value is not 1");

  // Reading lineFontPatternCode(Integer)
  PR.ReadInteger(PR.Current(), "Line Font Pattern Code", lineFontPatternCode); //szv#4:S4163:12Mar99 `st=` not needed

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(nbPropertyValues, lineFontPatternCode);
}

void IGESGraph_ToolLineFontPredefined::WriteOwnParams
  (const Handle(IGESGraph_LineFontPredefined)& ent, IGESData_IGESWriter& IW)  const
{ 
  IW.Send( ent->NbPropertyValues() );
  IW.Send( ent->LineFontPatternCode() );
}

void  IGESGraph_ToolLineFontPredefined::OwnShared
  (const Handle(IGESGraph_LineFontPredefined)& /*ent*/, Interface_EntityIterator& /*iter*/) const
{
}

void IGESGraph_ToolLineFontPredefined::OwnCopy
  (const Handle(IGESGraph_LineFontPredefined)& another,
   const Handle(IGESGraph_LineFontPredefined)& ent, Interface_CopyTool& /*TC*/) const
{
  ent->Init(1,another->LineFontPatternCode());
}

Standard_Boolean  IGESGraph_ToolLineFontPredefined::OwnCorrect
  (const Handle(IGESGraph_LineFontPredefined)& ent) const
{
  Standard_Boolean res = (ent->NbPropertyValues() != 1);
  if (res) ent->Init(1,ent->LineFontPatternCode());    // nbpropertyvalues=1
  return res;
}

IGESData_DirChecker IGESGraph_ToolLineFontPredefined::DirChecker
  (const Handle(IGESGraph_LineFontPredefined)& /*ent*/)  const
{ 
  IGESData_DirChecker DC (406, 19);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESGraph_ToolLineFontPredefined::OwnCheck
  (const Handle(IGESGraph_LineFontPredefined)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach)  const
{
  if (ent->NbPropertyValues() != 1)
    ach->AddFail("No. of Property values : Value != 1");
}

void IGESGraph_ToolLineFontPredefined::OwnDump
  (const Handle(IGESGraph_LineFontPredefined)& ent, const IGESData_IGESDumper& /*dumper*/,
   const Handle(Message_Messenger)& S, const Standard_Integer /*level*/)  const
{
  S << "IGESGraph_LineFontPredefined" << endl;

  S << "No. of property values : " << ent->NbPropertyValues() << endl;
  S << "Line font pattern code : " << ent->LineFontPatternCode() << endl;
  S << endl;
}
