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

#include <IGESGeom_ToolCircularArc.ixx>
#include <IGESData_ParamCursor.hxx>
#include <gp_XY.hxx>
#include <gp_Pnt2d.hxx>
#include <IGESData_Dump.hxx>
#include <Interface_MSG.hxx>
#include <Interface_Macros.hxx>
#include <stdio.h>

// MGE 28/07/98
#include <Message_Msg.hxx>

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
