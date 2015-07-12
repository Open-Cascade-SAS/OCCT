// Created on: 1998-06-16
// Created by: Philippe NOUAILLE
// Copyright (c) 1998-1999 Matra Datavision
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


#include <Adaptor3d_HSurface.hxx>
#include <ChFiDS_Spine.hxx>
#include <ChFiDS_SurfData.hxx>
#include <ChFiKPart_ComputeData.hxx>
#include <ChFiKPart_ComputeData_Fcts.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <gp.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <IntAna_QuadQuadGeo.hxx>
#include <Precision.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>

//=======================================================================
//function : MakeChAsym
//Purpose  : Compute the chamfer in the particular case plane/plane.
//           Compute the SurfData <Data> of the chamfer on the <Spine> 
//           between the plane <Pl1> and the plane <Pl2>, with distances
//           <Dis> and Angle on <Pl1> .
//           <First> is the parameter of the start point on the <Spine>
//           <Or1> and <Or2> are the orientations of the plane <Pl1> and
//           <Pl2>, and <Of1> the orientation of the face build on the
//           plane <Pl1>.
//Out      : True if the chamfer has been computed
//           False else
//=======================================================================
Standard_Boolean ChFiKPart_MakeChAsym(TopOpeBRepDS_DataStructure& DStr,
				      const Handle(ChFiDS_SurfData)& Data, 
				      const gp_Pln& Pl1, 
				      const gp_Pln& Pl2, 
				      const TopAbs_Orientation Or1,
				      const TopAbs_Orientation Or2,
				      const Standard_Real Dis, 
				      const Standard_Real Angle, 
				      const gp_Lin& Spine, 
				      const Standard_Real First, 
				      const TopAbs_Orientation Of1,
                                      const Standard_Boolean DisOnP1)
{

  // Creation of the plane which carry the chamfer

  // compute the normals to the planes Pl1 and Pl2
  gp_Ax3 Pos1 = Pl1.Position();
  gp_Dir D1   = Pos1.XDirection().Crossed(Pos1.YDirection());
  if (Or1 == TopAbs_REVERSED) { D1.Reverse(); }

  gp_Ax3 Pos2 = Pl2.Position();
  gp_Dir D2   = Pos2.XDirection().Crossed(Pos2.YDirection());
  if (Or2 == TopAbs_REVERSED) { D2.Reverse(); }

  // compute the intersection line of Pl1 and Pl2
  IntAna_QuadQuadGeo LInt (Pl1, Pl2, Precision::Angular(),
			   Precision::Confusion());
  
  gp_Pnt P;
  Standard_Real Fint;
  if (LInt.IsDone()) {
    Fint = ElCLib::Parameter(LInt.Line(1), ElCLib::Value(First, Spine));
    P = ElCLib::Value(Fint, LInt.Line(1));
  }
  else { return Standard_False; }

  gp_Dir LinAx1     = Spine.Direction();
  gp_Dir VecTransl1 = LinAx1.Crossed(D1);
  if ( VecTransl1.Dot(D2) < 0. )
    VecTransl1.Reverse();

  gp_Dir VecTransl2 = LinAx1.Crossed(D2);
  if ( VecTransl2.Dot(D1) < 0. )
    VecTransl2.Reverse();

  Standard_Real cosP, sinP, dis1, dis2;
  cosP = VecTransl1.Dot(VecTransl2);
  sinP = sqrt(1. - cosP * cosP);
   
  if (DisOnP1) {
    dis1 = Dis;
    dis2 = Dis / (cosP + sinP / Tan(Angle));    
  }  
  else {
    dis1 = Dis / (cosP + sinP / Tan(Angle));
    dis2 = Dis;
  }
  // Compute a point on the plane Pl1 and on the chamfer
  gp_Pnt P1( P.X() + dis1 * VecTransl1.X(),
	     P.Y() + dis1 * VecTransl1.Y(),
	     P.Z() + dis1 * VecTransl1.Z());

  // Point on the plane Pl2 and on the chamfer
  gp_Pnt P2( P.X() + dis2 * VecTransl2.X(),
	     P.Y() + dis2 * VecTransl2.Y(),
	     P.Z() + dis2 * VecTransl2.Z());

  //the middle point of P1 P2 is the origin of the chamfer
  gp_Pnt Po ( (P1.X() + P2.X()) / 2., (P1.Y() + P2.Y()) / 2., (P1.Z() + P2.Z()) / 2.); 
  
  // compute a second point on the plane Pl2 
  gp_Pnt Pp = ElCLib::Value(Fint + 10., LInt.Line(1));
  gp_Pnt P22(Pp.X() + dis2 * VecTransl2.X(),
	     Pp.Y() + dis2 * VecTransl2.Y(),
	     Pp.Z() + dis2 * VecTransl2.Z()); 
  
  // Compute the normal vector <AxisPlan> to the chamfer's plane
  gp_Dir V1 ( P2.X() - P1.X(), P2.Y() - P1.Y(), P2.Z() - P1.Z());
  gp_Dir V2 ( P22.X() - P1.X(), P22.Y() - P1.Y(), P22.Z() - P1.Z());
  gp_Dir AxisPlan = V1.Crossed(V2);

  gp_Dir xdir = LinAx1; // u axis
  gp_Ax3 PlanAx3 (Po, AxisPlan, xdir);
  if (PlanAx3.YDirection().Dot(D2)>=0.)  PlanAx3.YReverse();

  Handle(Geom_Plane) gpl= new Geom_Plane(PlanAx3);
  Data->ChangeSurf(ChFiKPart_IndexSurfaceInDS(gpl, DStr));

  // About the orientation of the chamfer plane
  // Compute the normal to the face 1
  gp_Dir norpl    = Pos1.XDirection().Crossed(Pos1.YDirection());
  gp_Dir norface1 = norpl;
  if (Of1 == TopAbs_REVERSED ) { norface1.Reverse(); }

  // Compute the orientation of the chamfer plane
  gp_Dir norplch = gpl->Pln().Position().XDirection().Crossed (
				gpl->Pln().Position().YDirection());

  gp_Dir DirCh12(gp_Vec(P1, P2));
  Standard_Boolean toreverse = ( norplch.Dot(norface1) <= 0. );
  if (VecTransl1.Dot(DirCh12) > 0)  toreverse = !toreverse;
  
  if (toreverse)
    Data->ChangeOrientation() = TopAbs_REVERSED; 
  else
    Data->ChangeOrientation() = TopAbs_FORWARD;

  // Loading of the FaceInterferences with pcurves & 3d curves.
  
  // case face 1 
  gp_Lin linPln(P1, xdir);
  Handle(Geom_Line) GLinPln1 = new Geom_Line(linPln);

  Standard_Real u, v;
  ElSLib::PlaneParameters(Pos1, P1, u, v);
  gp_Pnt2d p2dPln(u, v);
  gp_Dir2d dir2dPln( xdir.Dot(Pos1.XDirection()),
		     xdir.Dot(Pos1.YDirection()));
  gp_Lin2d lin2dPln(p2dPln, dir2dPln);
  Handle(Geom2d_Line) GLin2dPln1 = new Geom2d_Line(lin2dPln);

  ElSLib::PlaneParameters(PlanAx3, P1, u, v);
  p2dPln.SetCoord(u, v);
  lin2dPln.SetLocation(p2dPln);
  lin2dPln.SetDirection(gp::DX2d());
  Handle(Geom2d_Line) GLin2dPlnCh1 = new Geom2d_Line(lin2dPln);

  TopAbs_Orientation trans; 
  toreverse = ( norplch.Dot(norpl) <= 0. );
  if (VecTransl1.Dot(DirCh12) > 0)  toreverse = !toreverse;
  if (toreverse)
    trans = TopAbs_FORWARD;
  else 
    trans = TopAbs_REVERSED; 

  Data->ChangeInterferenceOnS1().
    SetInterference(ChFiKPart_IndexCurveInDS(GLinPln1, DStr),
		    trans, GLin2dPln1, GLin2dPlnCh1);


  // case face 2

  linPln.SetLocation(P2);
  Handle(Geom_Line) GLinPln2 = new Geom_Line(linPln);

  ElSLib::PlaneParameters(Pos2, P2, u, v);
  p2dPln.SetCoord(u, v);
  dir2dPln.SetCoord( xdir.Dot(Pos2.XDirection()),
		     xdir.Dot(Pos2.YDirection()));
  lin2dPln.SetLocation(p2dPln);
  lin2dPln.SetDirection(dir2dPln);
  Handle(Geom2d_Line) GLin2dPln2 = new Geom2d_Line(lin2dPln);

  ElSLib::PlaneParameters(PlanAx3, P2, u, v);
  p2dPln.SetCoord(u, v);
  lin2dPln.SetLocation(p2dPln);
  lin2dPln.SetDirection(gp::DX2d());
  Handle(Geom2d_Line) GLin2dPlnCh2 = new Geom2d_Line(lin2dPln);

  norpl = Pos2.XDirection().Crossed(Pos2.YDirection());
  toreverse = ( norplch.Dot(norpl) <= 0. );
  if (VecTransl2.Dot(DirCh12) < 0)  toreverse = !toreverse;
  if (toreverse)
    trans = TopAbs_REVERSED; 
  else
    trans = TopAbs_FORWARD; 

  Data->ChangeInterferenceOnS2().
    SetInterference(ChFiKPart_IndexCurveInDS(GLinPln2,DStr),
		    trans, GLin2dPln2, GLin2dPlnCh2);

  return Standard_True;

}
