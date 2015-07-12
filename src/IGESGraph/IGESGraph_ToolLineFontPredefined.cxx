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

#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESGraph_LineFontPredefined.hxx>
#include <IGESGraph_ToolLineFontPredefined.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>

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
