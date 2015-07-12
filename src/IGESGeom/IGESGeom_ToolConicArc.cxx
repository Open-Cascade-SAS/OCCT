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

#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_XY.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESGeom_ConicArc.hxx>
#include <IGESGeom_ToolConicArc.hxx>
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
//=======================================================================
//function : IGESGeom_ToolConicArc
//purpose  : 
//=======================================================================
IGESGeom_ToolConicArc::IGESGeom_ToolConicArc ()
{
}


//=======================================================================
//function : ReadOwnParams
//purpose  : 
//=======================================================================

void IGESGeom_ToolConicArc::ReadOwnParams(const Handle(IGESGeom_ConicArc)& ent,
                                          const Handle(IGESData_IGESReaderData)& /* IR */,
                                          IGESData_ParamReader& PR) const
{
  // MGE 28/07/98
  // Building of messages
  //======================================
  Message_Msg Msg83("XSTEP_83");
  Message_Msg Msg84("XSTEP_84");
  //======================================

  //Standard_Boolean st; //szv#4:S4163:12Mar99 not needed
  Standard_Real A, B = 0., C = 0., D = 0., E = 0., F = 0., ZT;
  gp_XY tempStart, tempEnd;

 /* PR.ReadReal(PR.Current(), Msg81, A); //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadReal(PR.Current(), Msg81, B); //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadReal(PR.Current(), Msg81, C); //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadReal(PR.Current(), Msg81, D); //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadReal(PR.Current(), Msg81, E); //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadReal(PR.Current(), Msg81, F); //szv#4:S4163:12Mar99 `st=` not needed
  */
  if ((!PR.ReadReal(PR.Current(),A)) || (!PR.ReadReal(PR.Current(),B)) ||
      (!PR.ReadReal(PR.Current(),C)) || (!PR.ReadReal(PR.Current(),D)) ||
      (!PR.ReadReal(PR.Current(),E)) || (!PR.ReadReal(PR.Current(),F))){
    Message_Msg Msg81("XSTEP_81");
    PR.SendFail(Msg81);
  }

  if (!PR.ReadReal(PR.Current(), ZT)){ //szv#4:S4163:12Mar99 `st=` not needed
    Message_Msg Msg82("XSTEP_82");
    PR.SendFail(Msg82);
  }
  PR.ReadXY(PR.CurrentList(1, 2),Msg83, tempStart); //szv#4:S4163:12Mar99 `st=` not needed
  PR.ReadXY(PR.CurrentList(1, 2), Msg84, tempEnd); //szv#4:S4163:12Mar99 `st=` not needed

/*
  st = PR.ReadReal(PR.Current(), "Conic Coefficient A", A);
  st = PR.ReadReal(PR.Current(), "Conic Coefficient B", B);
  st = PR.ReadReal(PR.Current(), "Conic Coefficient C", C);
  st = PR.ReadReal(PR.Current(), "Conic Coefficient D", D);
  st = PR.ReadReal(PR.Current(), "Conic Coefficient E", E);
  st = PR.ReadReal(PR.Current(), "Conic Coefficient F", F);
  st = PR.ReadReal(PR.Current(), "Z-plane shift", ZT);
  st = PR.ReadXY(PR.CurrentList(1, 2), "Starting Point Of Arc", tempStart);
  st = PR.ReadXY(PR.CurrentList(1, 2), "End Point Of Arc", tempEnd);
*/
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(A, B, C, D, E, F, ZT, tempStart, tempEnd);
}


//=======================================================================
//function : WriteOwnParams
//purpose  : 
//=======================================================================

void IGESGeom_ToolConicArc::WriteOwnParams(const Handle(IGESGeom_ConicArc)& ent,
                                           IGESData_IGESWriter& IW)  const
{
  Standard_Real A,B,C,D,E,F;
  ent->Equation(A,B,C,D,E,F);
  IW.Send(A);
  IW.Send(B);
  IW.Send(C);
  IW.Send(D);
  IW.Send(E);
  IW.Send(F);
  IW.Send(ent->ZPlane());
  IW.Send(ent->StartPoint().X());
  IW.Send(ent->StartPoint().Y());
  IW.Send(ent->EndPoint().X());
  IW.Send(ent->EndPoint().Y());
}


//=======================================================================
//function : OwnShared
//purpose  : 
//=======================================================================

void IGESGeom_ToolConicArc::OwnShared(const Handle(IGESGeom_ConicArc)& /* ent */,
                                      Interface_EntityIterator& /* iter */) const
{
}


//=======================================================================
//function : OwnCopy
//purpose  : 
//=======================================================================

void IGESGeom_ToolConicArc::OwnCopy(const Handle(IGESGeom_ConicArc)& another,
                                    const Handle(IGESGeom_ConicArc)& ent,
                                    Interface_CopyTool& /* TC */) const
{
  Standard_Real A,B,C,D,E,F;
  another->Equation(A,B,C,D,E,F);
  ent->Init(A, B, C, D, E, F, another->ZPlane(),
	    another->StartPoint().XY(), another->EndPoint().XY());
}


//=======================================================================
//function : OwnCorrect
//purpose  : 
//=======================================================================

Standard_Boolean IGESGeom_ToolConicArc::OwnCorrect
  (const Handle(IGESGeom_ConicArc)& ent) const
{
  return ent->OwnCorrect();    //  form selon coefs. 1 Ellipse, 2 Hyper, 3 Para
}


//=======================================================================
//function : DirChecker
//purpose  : 
//=======================================================================

IGESData_DirChecker IGESGeom_ToolConicArc::DirChecker
  (const Handle(IGESGeom_ConicArc)& /* ent */ )  const
{
  IGESData_DirChecker DC(104, 0, 3);
  DC.Structure(IGESData_DefVoid);
  DC.GraphicsIgnored();
  DC.LineFont(IGESData_DefAny);
//  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.HierarchyStatusIgnored();
  return DC;
}


//=======================================================================
//function : OwnCheck
//purpose  : 
//=======================================================================

void IGESGeom_ToolConicArc::OwnCheck(const Handle(IGESGeom_ConicArc)& ent,
                                     const Interface_ShareTool&,
                                     Handle(Interface_Check)& ach)  const
{
  // MGE 28/07/98
  // Building of messages
  //=====================================
  //Message_Msg Msg71("XSTEP_71"); 
  //=====================================

  //char mess[80]; //szv#4:S4163:12Mar99 not needed
  Standard_Integer cfn = ent->ComputedFormNumber();
  Standard_Integer  fn = ent->FormNumber();
  if (cfn == 0) {}
    // ach.AddFail("Coefficients do not define correctly a Conic");
  else if (fn != 0 && fn != cfn) {
    Message_Msg Msg71("XSTEP_71"); 
    ach->SendFail(Msg71);
  }
  
  //szv#4:S4163:12Mar99 not needed
  //if (ach.HasFailed()) return;    // les tests suivant deviennent sans objet
  //Standard_Real eps = 1.E-04;     // Tolerance des Tests ??
  //Standard_Real A,B,C,D,E,F;
  //ent->Equation(A,B,C,D,E,F);
  //Standard_Real x = ent->StartPoint().X();
  //Standard_Real y = ent->StartPoint().Y();
  //Standard_Real eq = (A*x*x + B*x*y + C*y*y + D*x + E*y + F);
  // These messages are transfered in the translation procedure
/*  if (eq < -eps || eq > eps) {
    Sprintf(mess,"Start point does not satisfy conic equation, gap over %f",
	    Interface_MSG::Intervalled(eq));
    ach.AddFail(mess,"Start point does not satisfy conic equation, gap over %f");

  }
*/
  //szv#4:S4163:12Mar99 not needed
  //x = ent->EndPoint().X();
  //y = ent->EndPoint().Y();
  //eq = (A*x*x + B*x*y + C*y*y + D*x + E*y + F);
/*  if (eq < -eps || eq > eps) {
    Sprintf(mess,"End point does not satisfy conic equation, gap over %f",
	    Interface_MSG::Intervalled(eq));
    ach.AddFail(mess,"End point does not satisfy conic equation, gap over %f");
  }
*/
/*   Les tests qui suivant ont-ils un sens ??
  if (ent->FormNumber() == 2)    // Hyperbola
    {
      Standard_Real xc = -D / (2 * A);
      Standard_Real yc = -E / (2 * C);
      gp_Dir2d d0(1, 0);
      gp_Dir2d d1(ent->StartPoint().X() - xc, ent->StartPoint().Y() - yc);
      gp_Dir2d d2(ent->EndPoint().X()   - xc, ent->EndPoint().Y()   - yc);
      Standard_Real t1 = d0.Angle(d1);
      Standard_Real t2 = d0.Angle(d2);
      t1 += (t1  >  0 ? 0 : 2*M_PI);
      t2 += (t2  >  0 ? 0 : 2*M_PI);
      t2 += (t1 <= t2 ? 0 : 2*M_PI);
      if ( !(0 <= t1 && t1 <= 2*M_PI) || !(0 <= t2-t1 && t2-t1 <= 2*M_PI) )
	ach.AddFail("Parameter Error for Hyperbola");
    }
  else if (ent->FormNumber() == 3)
    {
      Standard_Real xc = -D / (2 * A);
      Standard_Real yc = -E / (2 * C);
      gp_Dir2d d0(1, 0);
      gp_Dir2d d1(ent->StartPoint().X() - xc, ent->StartPoint().Y() - yc);
      gp_Dir2d d2(ent->EndPoint().X()   - xc, ent->EndPoint().Y()   - yc);
      Standard_Real t1 = d0.Angle(d1);
      Standard_Real t2 = d0.Angle(d2);
      if ( !(-M_PI/2 < t1 && t1 < M_PI/2) || !(-M_PI/2 < t2 && t2 < M_PI/2) )
	ach.AddFail("Parameter Error for Parabola");
    }
*/
}


//=======================================================================
//function : OwnDump
//purpose  : 
//=======================================================================

void IGESGeom_ToolConicArc::OwnDump(const Handle(IGESGeom_ConicArc)& ent,
                                    const IGESData_IGESDumper& /* dumper */,
                                    const Handle(Message_Messenger)& S,
                                    const Standard_Integer level)  const
{
  Standard_Real A,B,C,D,E,F;
  ent->Equation(A,B,C,D,E,F);
  S << "IGESGeom_ConicArc" << endl;
  Standard_Integer cf = ent->FormNumber();
  if (cf == 0) cf = ent->ComputedFormNumber();
  if (cf == 1)      S << " --     Ellipse     --" << endl;
  else if (cf == 2) S << " --    Hyperbola    --" << endl;
  else if (cf == 3) S << " --    Parabola    --"  << endl;
  else              S << " --    (Undetermined type of Conic)    --" << endl;
  S << "Conic Coefficient A : " << A << endl;
  S << "Conic Coefficient B : " << B << endl;
  S << "Conic Coefficient C : " << C << endl;
  S << "Conic Coefficient D : " << D << endl;
  S << "Conic Coefficient E : " << E << endl;
  S << "Conic Coefficient F : " << F << endl;
  S << "Z-Plane shift : " << ent->ZPlane() << endl;
  S << "Start Point : ";
  IGESData_DumpXYLZ(S,level, ent->StartPoint(), ent->Location(),ent->ZPlane());
  S << endl;
  S << "End   Point : ";
  IGESData_DumpXYLZ(S,level, ent->EndPoint(), ent->Location(), ent->ZPlane());
  S << endl;
  if (level <= 4) S<<" -- Computed Definition : ask level > 4" << endl;
  else {
    gp_Pnt Cen;  gp_Dir Ax;  Standard_Real Rmin,Rmax;
    ent->Definition (Cen,Ax,Rmin,Rmax);
    S << " -- Computed Definition (and Transformed if level > 5)" << endl;

    if (cf != 3) {
      S<<" Center        : "; IGESData_DumpXYZL(S,level,Cen,ent->Location());
      S<<endl;
    }
    S << " Main Axis   : "; IGESData_DumpXYZL(S,level,Ax,ent->VectorLocation());
    S<<endl;
    if (cf == 3) S << " Focal : " << Rmin << endl;
    else if (Rmin == Rmax) S << " Radius (Major = Minor) : " << Rmin << endl;
    else S << " Major Radius : " << Rmax << "  Minor Radius : " << Rmin <<endl;
    S<< "  Normal Axis : ";  IGESData_DumpXYZL(S,level,ent->Axis(),ent->VectorLocation());
    S << endl;
  }
}
