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
#include <IGESGraph_IntercharacterSpacing.hxx>
#include <IGESGraph_ToolIntercharacterSpacing.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>

IGESGraph_ToolIntercharacterSpacing::IGESGraph_ToolIntercharacterSpacing ()
      {  }


void IGESGraph_ToolIntercharacterSpacing::ReadOwnParams
  (const Handle(IGESGraph_IntercharacterSpacing)& ent,
   const Handle(IGESData_IGESReaderData)& /*IR*/, IGESData_ParamReader& PR) const
{ 
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  Standard_Integer nbPropertyValues;
  Standard_Real    iSpace; 

  // Reading nbPropertyValues(Integer)
  PR.ReadInteger(PR.Current(), "No. of property values", nbPropertyValues); //szv#4:S4163:12Mar99 `st=` not needed
  if (nbPropertyValues != 1)
    PR.AddFail("No. of Property values : Value is not 1");

  // Reading iSpace(Real)
  PR.ReadReal(PR.Current(), "Intercharacter space in % of text height", iSpace); //szv#4:S4163:12Mar99 `st=` not needed

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(nbPropertyValues, iSpace);
}

void IGESGraph_ToolIntercharacterSpacing::WriteOwnParams
  (const Handle(IGESGraph_IntercharacterSpacing)& ent, IGESData_IGESWriter& IW)  const
{ 
  IW.Send( ent->NbPropertyValues() );
  IW.Send( ent->ISpace() );
}

void  IGESGraph_ToolIntercharacterSpacing::OwnShared
  (const Handle(IGESGraph_IntercharacterSpacing)& /*ent*/, Interface_EntityIterator& /*iter*/) const
{
}

void IGESGraph_ToolIntercharacterSpacing::OwnCopy
  (const Handle(IGESGraph_IntercharacterSpacing)& another,
   const Handle(IGESGraph_IntercharacterSpacing)& ent, Interface_CopyTool& /*TC*/) const
{
  ent->Init(1,another->ISpace());
}

Standard_Boolean  IGESGraph_ToolIntercharacterSpacing::OwnCorrect
  (const Handle(IGESGraph_IntercharacterSpacing)& ent) const
{
  Standard_Boolean res = (ent->NbPropertyValues() != 1);
  if (res) ent->Init(1,ent->ISpace());    // nbpropertyvalues=1
  return res;
}

IGESData_DirChecker IGESGraph_ToolIntercharacterSpacing::DirChecker
  (const Handle(IGESGraph_IntercharacterSpacing)& /*ent*/)  const
{ 
  IGESData_DirChecker DC (406, 18);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESGraph_ToolIntercharacterSpacing::OwnCheck
  (const Handle(IGESGraph_IntercharacterSpacing)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach)  const
{
  if ((ent->ISpace() < 0.0) || (ent->ISpace() > 100.0))
    ach->AddFail("Intercharacter Space : Value not in the range [0-100]");
  if (ent->NbPropertyValues() != 1)
    ach->AddFail("No. of Property values : Value != 1");
}

void IGESGraph_ToolIntercharacterSpacing::OwnDump
  (const Handle(IGESGraph_IntercharacterSpacing)& ent, const IGESData_IGESDumper& /*dumper*/,
   const Handle(Message_Messenger)& S, const Standard_Integer /*level*/)  const
{
  S << "IGESGraph_IntercharacterSpacing" << endl;

  S << "No. of property values : " << ent->NbPropertyValues() << endl;
  S << "Intercharacter space in % of text height : " << ent->ISpace() << endl;
  S << endl;
}
