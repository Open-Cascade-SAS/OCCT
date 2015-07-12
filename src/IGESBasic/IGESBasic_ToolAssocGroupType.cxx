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

#include <IGESBasic_AssocGroupType.hxx>
#include <IGESBasic_ToolAssocGroupType.hxx>
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

IGESBasic_ToolAssocGroupType::IGESBasic_ToolAssocGroupType ()    {  }


void  IGESBasic_ToolAssocGroupType::ReadOwnParams
  (const Handle(IGESBasic_AssocGroupType)& ent,
   const Handle(IGESData_IGESReaderData)& /* IR */, IGESData_ParamReader& PR) const
{
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed
  Standard_Integer tempNbData;
  Standard_Integer tempType;
  Handle(TCollection_HAsciiString) tempName;
  if (PR.DefinedElseSkip())
    PR.ReadInteger(PR.Current(), "Number of data fields", tempNbData); //szv#4:S4163:12Mar99 `st=` not needed
  else
    tempNbData = 2;
  PR.ReadInteger(PR.Current(), "Type of attached associativity",tempType); //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadText(PR.Current(), "Name of attached associativity", tempName); //szv#4:S4163:12Mar99 `st=` not needed
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(tempNbData, tempType, tempName);
}

void  IGESBasic_ToolAssocGroupType::WriteOwnParams
  (const Handle(IGESBasic_AssocGroupType)& ent, IGESData_IGESWriter& IW) const
{
  IW.Send(ent->NbData());
  IW.Send(ent->AssocType());
  IW.Send(ent->Name());
}

void  IGESBasic_ToolAssocGroupType::OwnShared
  (const Handle(IGESBasic_AssocGroupType)& /* ent */, Interface_EntityIterator& /* iter */) const
{
}

void  IGESBasic_ToolAssocGroupType::OwnCopy
  (const Handle(IGESBasic_AssocGroupType)& another,
   const Handle(IGESBasic_AssocGroupType)& ent, Interface_CopyTool& /* TC */) const
{
  Standard_Integer tempNbData = another->NbData();
  Standard_Integer tempType = another->AssocType();
  Handle(TCollection_HAsciiString) tempName =
    new TCollection_HAsciiString(another->Name());
  ent->Init(tempNbData, tempType, tempName);
}

Standard_Boolean  IGESBasic_ToolAssocGroupType::OwnCorrect
  (const Handle(IGESBasic_AssocGroupType)& ent) const
{
  Standard_Boolean res = (ent->NbData() != 2);
  if (res) ent->Init(2,ent->AssocType(),ent->Name());
  return res;    // nbdata=2
}

IGESData_DirChecker  IGESBasic_ToolAssocGroupType::DirChecker
  (const Handle(IGESBasic_AssocGroupType)& /* ent */ ) const
{
  IGESData_DirChecker DC(406, 23);
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

void  IGESBasic_ToolAssocGroupType::OwnCheck
  (const Handle(IGESBasic_AssocGroupType)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach) const
{
  if (ent->NbData() != 2)
    ach->AddFail("Number of data fields != 2");
}

void  IGESBasic_ToolAssocGroupType::OwnDump
  (const Handle(IGESBasic_AssocGroupType)& ent, const IGESData_IGESDumper& /* dumper */,
   const Handle(Message_Messenger)& S, const Standard_Integer /* level */) const
{
  S << "IGESBasic_AssocGroupType" << endl;
  S << "Number of data fields : " << ent->NbData() << endl;
  S << "Type of attached associativity : " << ent->AssocType() << endl;
  S << "Name of attached associativity : ";
  IGESData_DumpString(S,ent->Name());
  S << endl;
}
