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

#include <IGESAppli_ToolLevelFunction.ixx>
#include <IGESData_ParamCursor.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Interface_Macros.hxx>
#include <IGESData_Dump.hxx>


IGESAppli_ToolLevelFunction::IGESAppli_ToolLevelFunction ()    {  }


void  IGESAppli_ToolLevelFunction::ReadOwnParams
  (const Handle(IGESAppli_LevelFunction)& ent,
   const Handle(IGESData_IGESReaderData)& /* IR */, IGESData_ParamReader& PR) const
{
  Standard_Integer tempNbPropertyValues;
  Standard_Integer tempFuncDescripCode;
  Handle(TCollection_HAsciiString) tempFuncDescrip;
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadInteger(PR.Current(),"No. of Property values", tempNbPropertyValues);

  if (PR.DefinedElseSkip())
    PR.ReadInteger(PR.Current(),"Function description code",tempFuncDescripCode);
  else
    tempFuncDescripCode = 0;

  if (PR.DefinedElseSkip())
    PR.ReadText(PR.Current(),"Function description",tempFuncDescrip);

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init (tempNbPropertyValues,tempFuncDescripCode,tempFuncDescrip);
}

void  IGESAppli_ToolLevelFunction::WriteOwnParams
  (const Handle(IGESAppli_LevelFunction)& ent, IGESData_IGESWriter& IW) const
{
  IW.Send(ent->NbPropertyValues());
  IW.Send(ent->FuncDescriptionCode());
  if (ent->FuncDescription().IsNull()) IW.SendVoid();
  else IW.Send(ent->FuncDescription());
}

void  IGESAppli_ToolLevelFunction::OwnShared
  (const Handle(IGESAppli_LevelFunction)& /* ent */, Interface_EntityIterator& /* iter */) const
{
}

void  IGESAppli_ToolLevelFunction::OwnCopy
  (const Handle(IGESAppli_LevelFunction)& another,
   const Handle(IGESAppli_LevelFunction)& ent, Interface_CopyTool& /* TC */) const
{
  Standard_Integer aNbPropertyValues, code;
  Handle(TCollection_HAsciiString) descrip;
  if (!another->FuncDescription().IsNull()) descrip =
    new TCollection_HAsciiString(another->FuncDescription());
  code = another->FuncDescriptionCode();
  aNbPropertyValues = another->NbPropertyValues();

  ent->Init(aNbPropertyValues,code,descrip);
}

Standard_Boolean  IGESAppli_ToolLevelFunction::OwnCorrect
  (const Handle(IGESAppli_LevelFunction)& ent) const
{
  Standard_Boolean res = (ent->NbPropertyValues() != 2);
  if (res) ent->Init(2,ent->FuncDescriptionCode(),ent->FuncDescription());
  return res;    // nbpropertyvalues = 2
}

IGESData_DirChecker  IGESAppli_ToolLevelFunction::DirChecker
  (const Handle(IGESAppli_LevelFunction)& /* ent */ ) const
{
  IGESData_DirChecker DC(406,3);  //Form no = 3 & Type = 406
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.BlankStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();
  DC.SubordinateStatusRequired(00);
  return DC;
}

void  IGESAppli_ToolLevelFunction::OwnCheck
  (const Handle(IGESAppli_LevelFunction)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const
{
  if (ent->NbPropertyValues() != 2)
    ach->AddFail("Number of Property Values != 2");
}

void  IGESAppli_ToolLevelFunction::OwnDump
  (const Handle(IGESAppli_LevelFunction)& ent, const IGESData_IGESDumper& /* dumper */,
   const Handle(Message_Messenger)& S, const Standard_Integer /* level */) const
{
  S << "IGESAppli_LevelFunction" << endl;

  S << "Number of property values : " << ent->NbPropertyValues() << endl;
  S << "Function Description code : " << ent->FuncDescriptionCode() << endl;
  S << "Function Description : ";
  IGESData_DumpString(S,ent->FuncDescription());
  S << endl;
}
