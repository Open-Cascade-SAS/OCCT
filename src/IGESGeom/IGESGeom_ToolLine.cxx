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

#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESGeom_Line.hxx>
#include <IGESGeom_ToolLine.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Standard_DomainError.hxx>

// MGE 29/07/98
IGESGeom_ToolLine::IGESGeom_ToolLine ()    {  }


void IGESGeom_ToolLine::ReadOwnParams
  (const Handle(IGESGeom_Line)& ent,
   const Handle(IGESData_IGESReaderData)& /* IR */, IGESData_ParamReader& PR) const
{
  // MGE 29/07/98
  // Building of messages
  //====================================
  Message_Msg Msg89("XSTEP_89");
  Message_Msg Msg90("XSTEP_90");
  //====================================

  gp_XYZ aStart, anEnd;

  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  PR.ReadXYZ(PR.CurrentList(1, 3),Msg89, aStart); //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadXYZ(PR.CurrentList(1, 3), Msg90, anEnd); //szv#4:S4163:12Mar99 `st=` not needed

 /* st = PR.ReadXYZ(PR.CurrentList(1, 3), "Starting Point", aStart);
    st = PR.ReadXYZ(PR.CurrentList(1, 3), "End Point", anEnd);
 */
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(aStart, anEnd);
}

void IGESGeom_ToolLine::WriteOwnParams
  (const Handle(IGESGeom_Line)& ent, IGESData_IGESWriter& IW)  const
{
  IW.Send(ent->StartPoint().X());
  IW.Send(ent->StartPoint().Y());
  IW.Send(ent->StartPoint().Z());
  IW.Send(ent->EndPoint().X());
  IW.Send(ent->EndPoint().Y());
  IW.Send(ent->EndPoint().Z());
}

void  IGESGeom_ToolLine::OwnShared
  (const Handle(IGESGeom_Line)& /* ent */, Interface_EntityIterator& /* iter */) const
{
}

void IGESGeom_ToolLine::OwnCopy 
  (const Handle(IGESGeom_Line)& another,
   const Handle(IGESGeom_Line)& ent, Interface_CopyTool& /* TC */) const
{
  ent->Init(another->StartPoint().XYZ(), another->EndPoint().XYZ());
}

IGESData_DirChecker IGESGeom_ToolLine::DirChecker
  (const Handle(IGESGeom_Line)& /* ent */ )  const
{
  IGESData_DirChecker DC(110, 0,2);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
//  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESGeom_ToolLine::OwnCheck
  (const Handle(IGESGeom_Line)& /* ent */,
   const Interface_ShareTool& , Handle(Interface_Check)& /* ach */)  const
{ 
}

void IGESGeom_ToolLine::OwnDump
  (const Handle(IGESGeom_Line)& ent, const IGESData_IGESDumper& /* dumper */,
   const Handle(Message_Messenger)& S, const Standard_Integer level)  const
{
  Standard_Integer infin = ent->Infinite();
  switch (infin) {
    case 1 : S << "Semi-Infinite Line"<<endl;  break;
    case 2 : S << "Infinite Line"<<endl;  break;
    default : S << "Bounded Line"<<endl; break;
  }

  S << "Line from IGESGeom" << endl;
  S << "Starting Point : ";
  IGESData_DumpXYZL(S,level, ent->StartPoint(), ent->Location());
  S << endl;
  S << "End Point : ";
  IGESData_DumpXYZL(S,level, ent->EndPoint(), ent->Location());
  S << endl;
}
