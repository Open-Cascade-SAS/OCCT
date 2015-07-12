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

#include <IGESAppli_FlowLineSpec.hxx>
#include <IGESAppli_ToolFlowLineSpec.hxx>
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

IGESAppli_ToolFlowLineSpec::IGESAppli_ToolFlowLineSpec ()    {  }


void  IGESAppli_ToolFlowLineSpec::ReadOwnParams
  (const Handle(IGESAppli_FlowLineSpec)& ent,
   const Handle(IGESData_IGESReaderData)& /* IR */, IGESData_ParamReader& PR) const
{
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed
  Standard_Integer num;
  Handle(Interface_HArray1OfHAsciiString) tempNameAndModifiers;
  if (!PR.ReadInteger(PR.Current(), "Number of property values", num)) num = 0;
  if (num > 0) tempNameAndModifiers = new Interface_HArray1OfHAsciiString(1, num);
  else PR.AddFail("Number of property values: Not Positive");
  //szv#4:S4163:12Mar99 `st=` not needed
  if (!tempNameAndModifiers.IsNull())
    PR.ReadTexts(PR.CurrentList(num), "Name and Modifiers", tempNameAndModifiers);

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(tempNameAndModifiers);
}

void  IGESAppli_ToolFlowLineSpec::WriteOwnParams
  (const Handle(IGESAppli_FlowLineSpec)& ent, IGESData_IGESWriter& IW) const
{
  Standard_Integer i, num;
  IW.Send(ent->NbPropertyValues());
  for ( num = ent->NbPropertyValues(), i = 1; i <= num; i++ )
    IW.Send(ent->Modifier(i));
}

void  IGESAppli_ToolFlowLineSpec::OwnShared
  (const Handle(IGESAppli_FlowLineSpec)& /* ent */, Interface_EntityIterator& /* iter */) const
{
}

void  IGESAppli_ToolFlowLineSpec::OwnCopy
  (const Handle(IGESAppli_FlowLineSpec)& another,
   const Handle(IGESAppli_FlowLineSpec)& ent, Interface_CopyTool& /* TC */) const
{
  Standard_Integer num = another->NbPropertyValues();
  Handle(Interface_HArray1OfHAsciiString) tempNameAndModifiers =
    new Interface_HArray1OfHAsciiString(1, num);
  for ( Standard_Integer i = 1; i <= num; i++ )
    tempNameAndModifiers->SetValue
      (i, new TCollection_HAsciiString(another->Modifier(i)));
  ent->Init(tempNameAndModifiers);
}

IGESData_DirChecker  IGESAppli_ToolFlowLineSpec::DirChecker
  (const Handle(IGESAppli_FlowLineSpec)& /* ent */ ) const
{
  IGESData_DirChecker DC(406, 14);
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

void  IGESAppli_ToolFlowLineSpec::OwnCheck
  (const Handle(IGESAppli_FlowLineSpec)& /* ent */,
   const Interface_ShareTool& , Handle(Interface_Check)& /* ach */) const
{
}

void  IGESAppli_ToolFlowLineSpec::OwnDump
  (const Handle(IGESAppli_FlowLineSpec)& ent, const IGESData_IGESDumper& /* dumper */,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  S << "IGESAppli_FlowLineSpec" << endl;
  S << "Name and Modifiers : ";
  IGESData_DumpStrings(S ,level,1, ent->NbPropertyValues(),ent->Modifier);
  S << endl;
}

