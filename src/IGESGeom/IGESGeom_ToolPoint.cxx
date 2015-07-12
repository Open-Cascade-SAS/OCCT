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
#include <IGESBasic_SubfigureDef.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESData_Status.hxx>
#include <IGESGeom_Point.hxx>
#include <IGESGeom_ToolPoint.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Standard_DomainError.hxx>

// MGE 28/07/98
IGESGeom_ToolPoint::IGESGeom_ToolPoint ()    {  }


void IGESGeom_ToolPoint::ReadOwnParams
  (const Handle(IGESGeom_Point)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  // MGE 28/07/98
  // Building of messages
  //==================================
  Message_Msg Msg73("XSTEP_73");
  //==================================

  gp_XYZ aPoint;
  Handle(IGESBasic_SubfigureDef) aSymbol;
  IGESData_Status aStatus;
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  PR.ReadXYZ(PR.CurrentList(1, 3), Msg73, aPoint); //szv#4:S4163:12Mar99 `st=` not needed
  //st = PR.ReadXYZ(PR.CurrentList(1, 3), "Point", aPoint);

  if (PR.DefinedElseSkip()){
    if (!PR.ReadEntity(IR, PR.Current(), aStatus,
		  STANDARD_TYPE(IGESBasic_SubfigureDef), aSymbol, Standard_True)){
      Message_Msg Msg74("XSTEP_74");
      switch(aStatus) {
      case IGESData_ReferenceError: {  
	Message_Msg Msg216 ("IGES_216");
	Msg74.Arg(Msg216.Value());
	PR.SendFail(Msg74);
	break; }
      case IGESData_EntityError: {
	Message_Msg Msg217 ("IGES_217");
	Msg74.Arg(Msg217.Value());
	PR.SendFail(Msg74);
	break; }
      case IGESData_TypeError: {
	Message_Msg Msg218 ("IGES_218");
	Msg74.Arg(Msg218.Value());
	PR.SendFail(Msg74);
	break; }
      default:{
      }
      }
    }
 //szv#4:S4163:12Mar99 `st=` not needed
  }
   // st = PR.ReadEntity(IR, PR.Current(), "Display Symbol",
//		       STANDARD_TYPE(IGESBasic_SubfigureDef), aSymbol, Standard_True);

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(aPoint, aSymbol);
}

void IGESGeom_ToolPoint::WriteOwnParams
  (const Handle(IGESGeom_Point)& ent, IGESData_IGESWriter& IW)  const
{
  IW.Send(ent->Value().X());
  IW.Send(ent->Value().Y());
  IW.Send(ent->Value().Z());
  IW.Send(ent->DisplaySymbol());
}

void  IGESGeom_ToolPoint::OwnShared
  (const Handle(IGESGeom_Point)& ent, Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->DisplaySymbol());
}

void IGESGeom_ToolPoint::OwnCopy
  (const Handle(IGESGeom_Point)& another,
   const Handle(IGESGeom_Point)& ent, Interface_CopyTool& TC) const
{
  gp_XYZ aPoint = (another->Value()).XYZ();

  DeclareAndCast(IGESBasic_SubfigureDef, aSymbol, 
                 TC.Transferred(another->DisplaySymbol()));
  ent->Init(aPoint, aSymbol);
}


IGESData_DirChecker IGESGeom_ToolPoint::DirChecker
  (const Handle(IGESGeom_Point)& ent )   const
{
  IGESData_DirChecker DC(116,0);
  DC.Structure(IGESData_DefVoid);
  if (ent->HasDisplaySymbol())
    {
      DC.LineFont(IGESData_DefAny);
      DC.LineWeight(IGESData_DefValue);
    }
  DC.Color(IGESData_DefAny);
  return DC;
}

void IGESGeom_ToolPoint::OwnCheck
  (const Handle(IGESGeom_Point)& /* ent */,
   const Interface_ShareTool& , Handle(Interface_Check)& /* ach */)  const
{
}

void IGESGeom_ToolPoint::OwnDump
  (const Handle(IGESGeom_Point)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level)  const
{
  S << "IGESGeom_Point" << endl;

  S << " Value         : ";
  IGESData_DumpXYZL(S,level, ent->Value(), ent->Location());
  S << endl;
  S << " Display Symbol : ";
  dumper.Dump(ent->DisplaySymbol(),S, (level <= 4) ? 0 : 1);
  S << endl;
}
