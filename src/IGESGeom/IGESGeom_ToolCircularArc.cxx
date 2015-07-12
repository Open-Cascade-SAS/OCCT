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

#include <gp_Pnt2d.hxx>
#include <gp_XY.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESGeom_CircularArc.hxx>
#include <IGESGeom_ToolCircularArc.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_MSG.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Standard_DomainError.hxx>

#include <stdio.h>
// MGE 28/07/98
IGESGeom_ToolCircularArc::IGESGeom_ToolCircularArc ()    {  }


void IGESGeom_ToolCircularArc::ReadOwnParams
  (const Handle(IGESGeom_CircularArc)& ent,
   const Handle(IGESData_IGESReaderData)& /* IR */, IGESData_ParamReader& PR) const
{
  // MGE 28/07/98
  // Building of messages
  //=====================================
  Message_Msg Msg76("XSTEP_76");
  Message_Msg Msg77("XSTEP_77");
  Message_Msg Msg78("XSTEP_78");
  //=====================================
  
  Standard_Real aZT;
  gp_XY aCenter, aStart, anEnd;
  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed

  // MGE 28/07/98
  if (!PR.ReadReal(PR.Current(), aZT)){ //szv#4:S4163:12Mar99 `st=` not needed
    Message_Msg Msg75("XSTEP_75");
    PR.SendFail(Msg75);
  }
  PR.ReadXY(PR.CurrentList(1, 2), Msg76, aCenter); //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadXY(PR.CurrentList(1, 2), Msg77, aStart); //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadXY(PR.CurrentList(1, 2), Msg78, anEnd); //szv#4:S4163:12Mar99 `st=` not needed
  
/*
  st = PR.ReadReal(PR.Current(), "Shift above z-plane", aZT);
  st = PR.ReadXY(PR.CurrentList(1, 2), "Center Of Arc", aCenter);
  st = PR.ReadXY(PR.CurrentList(1, 2), "Start Point Of Arc", aStart);
  st = PR.ReadXY(PR.CurrentList(1, 2), "End Point Of Arc", anEnd);
*/

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(aZT, aCenter, aStart, anEnd);

}

void IGESGeom_ToolCircularArc::WriteOwnParams
  (const Handle(IGESGeom_CircularArc)& ent, IGESData_IGESWriter& IW)  const
{
  IW.Send(ent->ZPlane());
  IW.Send(ent->Center().X());
  IW.Send(ent->Center().Y());
  IW.Send(ent->StartPoint().X());
  IW.Send(ent->StartPoint().Y());
  IW.Send(ent->EndPoint().X());
  IW.Send(ent->EndPoint().Y());
}

void  IGESGeom_ToolCircularArc::OwnShared
  (const Handle(IGESGeom_CircularArc)& /* ent */, Interface_EntityIterator& /* iter */) const
{
}

void IGESGeom_ToolCircularArc::OwnCopy 
  (const Handle(IGESGeom_CircularArc)& another,
   const Handle(IGESGeom_CircularArc)& ent, Interface_CopyTool& /* TC */) const
{
  ent->Init(another->ZPlane(), another->Center().XY(),
	    another->StartPoint().XY(), another->EndPoint().XY());
}


IGESData_DirChecker IGESGeom_ToolCircularArc::DirChecker
  (const Handle(IGESGeom_CircularArc)& /* ent */ )  const
{
  IGESData_DirChecker DC(100, 0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
//  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESGeom_ToolCircularArc::OwnCheck
  (const Handle(IGESGeom_CircularArc)& /*ent*/,
   const Interface_ShareTool& , Handle(Interface_Check)& /*ach*/)  const
{
/*
  //Standard_Real eps  = 1.E-04;    // Tolerance des tests ?? //szv#4:S4163:12Mar99 not needed

  Standard_Real Rad1 = Sqrt(Square(ent->StartPoint().X() - ent->Center().X()) +
			    Square(ent->StartPoint().Y() - ent->Center().Y()));
  Standard_Real Rad2 = Sqrt(Square(ent->EndPoint().X()   - ent->Center().X()) +
			    Square(ent->EndPoint().Y()   - ent->Center().Y()));

  Standard_Real ratio = Abs(Rad1 - Rad2) / (Rad1+Rad2);
  if (ratio > eps) {
    char mess[80];
    Sprintf(mess,"Radius at Start & End Points, relative gap over %f",
	    Interface_MSG::Intervalled (ratio));
    ach.AddFail(mess,"Radius at Start & End Points, relative gap over %f");
  }
*/
}

void IGESGeom_ToolCircularArc::OwnDump
  (const Handle(IGESGeom_CircularArc)& ent, const IGESData_IGESDumper& /* dumper */,
   const Handle(Message_Messenger)& S, const Standard_Integer level)  const
{
  S << "CircularArc from IGESGeom" << endl;
  S << "Z-Plane Displacement : " << ent->ZPlane() << endl;
  S << "Center      : ";
  IGESData_DumpXYLZ(S,level, ent->Center(), ent->Location(), ent->ZPlane());
  S << endl;
  S << "Start Point : ";
  IGESData_DumpXYLZ(S,level, ent->StartPoint(), ent->Location(),ent->ZPlane());
  S << endl;
  S << "End Point   : ";
  IGESData_DumpXYLZ(S,level, ent->EndPoint(), ent->Location(), ent->ZPlane());
  S << endl;
  if (level <= 5) return;
  S<< "  Normal Axis : ";  IGESData_DumpXYZL(S,level,ent->Axis(),ent->VectorLocation());
  S << endl;
}
