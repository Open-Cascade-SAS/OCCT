// Created on: 1992-11-19
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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


#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepClass_Edge.hxx>
#include <BRepClass_Intersector.hxx>
#include <ElCLib.hxx>
#include <Extrema_ExtPC2d.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom2dLProp_CLProps2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <IntRes2d_Domain.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntRes2d_Transition.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopoDS_Vertex.hxx>

static 
void RefineTolerance(const TopoDS_Face& aF,
                     const BRepAdaptor_Curve2d& aC,
                     const Standard_Real aT,
                     Standard_Real& aTolZ);

//=======================================================================
//function : BRepClass_Intersector
//purpose  : 
//=======================================================================

BRepClass_Intersector::BRepClass_Intersector()
{
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void  BRepClass_Intersector::Perform(const gp_Lin2d& L, 
                                     const Standard_Real P, 
                                     const Standard_Real Tol, 
                                     const BRepClass_Edge& E)
{
  Standard_Real deb = 0.0, fin = 0.0, aTolZ = Tol;
  Handle(Geom2d_Curve) aC2D;
  //
  const TopoDS_Edge& EE = E.Edge();
  const TopoDS_Face& F = E.Face();

  //
  aC2D=BRep_Tool::CurveOnSurface(EE, F, deb, fin);
  if (aC2D.IsNull()) {
    done = Standard_False; // !IsDone()
    return;
  }
  //
  BRepAdaptor_Curve2d C(EE, F);
  //
  deb = C.FirstParameter();
  fin = C.LastParameter();
  //
  // Case of "ON": direct check of belonging to edge
  // taking into account the tolerance
  Extrema_ExtPC2d anExtPC2d(L.Location(), C);
  Standard_Real MinDist = RealLast(), aDist;
  Standard_Integer MinInd = 0, i;
  if (anExtPC2d.IsDone())
  {
    const Standard_Integer aNbPnts = anExtPC2d.NbExt();
    for (i = 1; i <= aNbPnts; ++i)
    {
      aDist = anExtPC2d.SquareDistance(i);

      if (aDist < MinDist)
      {
        MinDist = aDist;
        MinInd = i;
      }
    }
  }

  if (MinInd) {
    MinDist = sqrt(MinDist);
  }
  if (MinDist <= aTolZ) {
    gp_Pnt2d pnt_exact = (anExtPC2d.Point(MinInd)).Value();
    Standard_Real par = (anExtPC2d.Point(MinInd)).Parameter();
    //
    RefineTolerance(F, C, par, aTolZ);
    //
    if (MinDist <= aTolZ) {
      IntRes2d_Transition tr_on_lin(IntRes2d_Head);
      IntRes2d_Position pos_on_curve = IntRes2d_Middle;
      if (Abs(par - deb) <= Precision::Confusion()) {
        pos_on_curve = IntRes2d_Head;
      }
      else if (Abs(par - fin) <= Precision::Confusion()) {
        pos_on_curve = IntRes2d_End;
      }
      //
      IntRes2d_Transition tr_on_curve(pos_on_curve);
      IntRes2d_IntersectionPoint pnt_inter(pnt_exact, 0., par,
        tr_on_lin, tr_on_curve, 
        Standard_False);
      //
      Append(pnt_inter);
      done = Standard_True;
      return;
    }
  }
  //  
  gp_Pnt2d pdeb,pfin;
  C.D0(deb,pdeb);
  C.D0(fin,pfin);
  Standard_Real toldeb = 1.e-5, tolfin = 1.e-5;

  IntRes2d_Domain DL;
  //
  if(P!=RealLast()) {
    DL.SetValues(L.Location(),0.,aTolZ,ElCLib::Value(P,L),P,aTolZ);
  }
  else { 
    DL.SetValues(L.Location(),0.,aTolZ,Standard_True);
  }

  IntRes2d_Domain DE(pdeb,deb,toldeb,pfin,fin,tolfin);
  // temporary periodic domain
  if (C.Curve()->IsPeriodic()) {
    DE.SetEquivalentParameters(C.FirstParameter(),
      C.FirstParameter() + 
      C.Curve()->LastParameter() -
      C.Curve()->FirstParameter());
  }

  Handle(Geom2d_Line) GL= new Geom2d_Line(L);
  Geom2dAdaptor_Curve CGA(GL);
  Geom2dInt_GInter Inter(CGA,DL,C,DE,
    Precision::PConfusion(),
    Precision::PIntersection());
  //
  SetValues(Inter);
}

//=======================================================================
//function : LocalGeometry
//purpose  : 
//=======================================================================
void  BRepClass_Intersector::LocalGeometry(const BRepClass_Edge& E, 
                                           const Standard_Real U, 
                                           gp_Dir2d& Tang, 
                                           gp_Dir2d& Norm, 
                                           Standard_Real& C) const 
{
  Standard_Real f,l;
  Geom2dLProp_CLProps2d Prop(BRep_Tool::CurveOnSurface(E.Edge(),E.Face(),f,l),
    U,2,Precision::PConfusion());
  Prop.Tangent(Tang);
  C = Prop.Curvature();
  if (C > Precision::PConfusion())
    Prop.Normal(Norm);
  else
    Norm.SetCoord(Tang.Y(),-Tang.X());
}

//=======================================================================
//function : RefineTolerance
//purpose  : 
//=======================================================================
void RefineTolerance(const TopoDS_Face& aF,
                     const BRepAdaptor_Curve2d& aC,
                     const Standard_Real aT,
                     Standard_Real& aTolZ)
{
  GeomAbs_SurfaceType aTypeS;
  //
  BRepAdaptor_Surface aBAS(aF, Standard_False);
  //
  aTypeS=aBAS.GetType();
  if (aTypeS==GeomAbs_Cylinder) {
    Standard_Real aURes, aVRes, aTolX;
    gp_Pnt2d aP2D;
    gp_Vec2d aV2D;
    //
    aURes=aBAS.UResolution(aTolZ);
    aVRes=aBAS.VResolution(aTolZ);
    //
    aC.D1(aT, aP2D, aV2D);
    gp_Dir2d aD2D(aV2D);
    //
    aTolX=aURes*aD2D.Y()+aVRes*aD2D.X();
    if (aTolX<0.) {
      aTolX=-aTolX;
    }
    //
    if (aTolX < Precision::Confusion()) {
      aTolX = Precision::Confusion();
    }
    //
    if (aTolX<aTolZ) {
      aTolZ=aTolX;
    }
  }
}


