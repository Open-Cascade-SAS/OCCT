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

#include <IGESAppli_LevelFunction.hxx>
#include <IGESAppli_ToolLevelFunction.hxx>
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
