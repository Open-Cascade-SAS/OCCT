// Created on: 1994-02-03
// Created by: Isabelle GRIGNON
// Copyright (c) 1994-1999 Matra Datavision
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
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Line.hxx>
#include <gp.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <IntAna_QuadQuadGeo.hxx>
#include <Precision.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>

//=======================================================================
//function : MakeFillet
//Purpose  : cas plan/plan.
//=======================================================================
Standard_Boolean ChFiKPart_MakeFillet(TopOpeBRepDS_DataStructure& DStr,
				      const Handle(ChFiDS_SurfData)& Data, 
				      const gp_Pln& Pl1, 
				      const gp_Pln& Pl2, 
				      const TopAbs_Orientation Or1,
				      const TopAbs_Orientation Or2,
				      const Standard_Real Radius, 
				      const gp_Lin& Spine, 
				      const Standard_Real First, 
				      const TopAbs_Orientation Of1)
{

//calcul du cylindre
  gp_Ax3 Pos1 = Pl1.Position();
  gp_Dir D1 = Pos1.XDirection().Crossed(Pos1.YDirection());
  if (Or1 == TopAbs_REVERSED) { D1.Reverse(); }
  gp_Ax3 Pos2 = Pl2.Position();
  gp_Dir D2 = Pos2.XDirection().Crossed(Pos2.YDirection());
  if (Or2 == TopAbs_REVERSED) { D2.Reverse(); }
  IntAna_QuadQuadGeo LInt (Pl1,Pl2,Precision::Angular(),
			   Precision::Confusion());
  gp_Pnt Pv;
  if (LInt.IsDone()) {
    //On met l origine du cylindre au point de depart fourni sur la 
    //ligne guide.
    Pv = ElCLib::Value(ElCLib::Parameter(LInt.Line(1),
					 ElCLib::Value(First,Spine)),
		       LInt.Line(1));
  }
  else { return Standard_False; }
  gp_Dir AxisCylinder = Spine.Direction();
  Standard_Real Ang = D1.Angle(D2);
  gp_Vec V = gp_Vec(D1)+gp_Vec(D2);
  gp_Dir S(V);
  gp_Pnt C;
  Standard_Real Fac = Radius/Cos(Ang/2.);
  C.SetCoord(Pv.X()+Fac*S.X(),Pv.Y()+Fac*S.Y(),Pv.Z()+Fac*S.Z());
  gp_Dir xdir = D1.Reversed();
  gp_Ax3 CylAx3(C,AxisCylinder,xdir);
  if (CylAx3.YDirection().Dot(D2) >= 0.){ CylAx3.YReverse(); }
  Handle(Geom_CylindricalSurface) 
    gcyl = new Geom_CylindricalSurface(CylAx3,Radius);
  Data->ChangeSurf(ChFiKPart_IndexSurfaceInDS(gcyl,DStr));

  // On regarde si l orientation du cylindre est la meme que celle 
  // des faces.
  gp_Pnt P;
  gp_Vec deru,derv;
  ElSLib::CylinderD1(0.,0.,CylAx3,Radius,P,deru,derv);
  gp_Dir norcyl(deru.Crossed(derv));
  gp_Dir norpl = Pos1.XDirection().Crossed(Pos1.YDirection());
  gp_Dir norface = norpl;
  if (Of1 == TopAbs_REVERSED ) { norface.Reverse(); }
  Standard_Boolean toreverse = ( norcyl.Dot(norface) <= 0. );
  if (toreverse) { Data->ChangeOrientation() = TopAbs_REVERSED; }
  else { Data->ChangeOrientation() = TopAbs_FORWARD; }

  // On charge les FaceInterferences avec les pcurves et courbes 3d.

  Standard_Real u,v;
  // La face 1.
  ElSLib::PlaneParameters(Pos1,P,u,v);
  gp_Pnt2d p2dPln(u,v);
  gp_Dir2d dir2dPln(AxisCylinder.Dot(Pos1.XDirection()),
		    AxisCylinder.Dot(Pos1.YDirection()));
  gp_Lin2d lin2dPln(p2dPln,dir2dPln);
  Handle(Geom2d_Line) GLin2dPln1 = new Geom2d_Line(lin2dPln);
  gp_Lin linPln(P,AxisCylinder);
  Handle(Geom_Line) GLinPln1 = new Geom_Line(linPln);
  gp_Lin2d lin2dCyl(gp_Pnt2d(0.,0.),gp::DY2d());
  Handle(Geom2d_Line) GLin2dCyl1 = new Geom2d_Line(lin2dCyl);
  TopAbs_Orientation trans; 
  toreverse = ( norcyl.Dot(norpl) <= 0. );
  if (toreverse){ trans = TopAbs_REVERSED; }
  else { trans = TopAbs_FORWARD; }
  Data->ChangeInterferenceOnS1().
    SetInterference(ChFiKPart_IndexCurveInDS(GLinPln1,DStr),
		    trans,GLin2dPln1,GLin2dCyl1);

  // La face 2.
  ElSLib::CylinderD1(Ang,0.,CylAx3,Radius,P,deru,derv);
  norcyl = deru.Crossed(derv);
  norpl = Pos2.XDirection().Crossed(Pos2.YDirection());
  toreverse = ( norcyl.Dot(norpl) <= 0. );
  ElSLib::PlaneParameters(Pos2,P,u,v);
  p2dPln.SetCoord(u,v);
  dir2dPln.SetCoord(AxisCylinder.Dot(Pos2.XDirection()),
		    AxisCylinder.Dot(Pos2.YDirection()));
  lin2dPln.SetLocation(p2dPln);
  lin2dPln.SetDirection(dir2dPln);
  Handle(Geom2d_Line) GLin2dPln2 = new Geom2d_Line(lin2dPln);
  linPln.SetLocation(P);
  linPln.SetDirection(AxisCylinder);
  Handle(Geom_Line) GLinPln2 = new Geom_Line(linPln);
  lin2dCyl.SetLocation(gp_Pnt2d(Ang,0.));
  Handle(Geom2d_Line) GLin2dCyl2 = new Geom2d_Line(lin2dCyl);
  if (toreverse){ trans = TopAbs_FORWARD; }
  else { trans = TopAbs_REVERSED; }
  Data->ChangeInterferenceOnS2().
    SetInterference(ChFiKPart_IndexCurveInDS(GLinPln2,DStr),
		    trans,GLin2dPln2,GLin2dCyl2);
  return Standard_True;
}

