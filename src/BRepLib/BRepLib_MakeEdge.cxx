// Created on: 1993-07-23
// Created by: Joelle CHAUVET
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

// Modified:	Wed Oct 23 09:17:47 1996
//		check ponctuallity (PRO4896)

#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <ElCLib.hxx>
#include <Extrema_ExtPC.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <gp.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <StdFail_NotDone.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

//=======================================================================
//function : Project
//purpose  : project a vertex on a curve
//=======================================================================
static Standard_Boolean Project(const Handle(Geom_Curve)& C,
				const TopoDS_Vertex& V,
				Standard_Real& p)
{
  Standard_Real Eps2 = BRep_Tool::Tolerance(V);
  Eps2 *= Eps2;
  
  gp_Pnt P = BRep_Tool::Pnt(V);
  GeomAdaptor_Curve GAC(C);
  
  // Afin de faire les extremas, on verifie les distances en bout
  Standard_Real D1,D2;
  gp_Pnt P1,P2;
  P1 = GAC.Value(GAC.FirstParameter());
  P2 = GAC.Value(GAC.LastParameter());
  D1 = P1.SquareDistance(P);
  D2 = P2.SquareDistance(P);
  if ( (D1 < D2) && (D1 <= Eps2) ) {
    p = GAC.FirstParameter();
    return Standard_True;
  }
  else if ( (D2 < D1) && (D2 <= Eps2) ) {
    p = GAC.LastParameter();
    return Standard_True;
  }

  // Sinon, on calcule les extremas.

  Extrema_ExtPC extrema(P,GAC);
  if (extrema.IsDone()) {
    Standard_Integer i, index = 0, n = extrema.NbExt();
    Standard_Real Dist2 = RealLast(), dist2min;

    for (i = 1; i <= n; i++) {
      dist2min = extrema.SquareDistance(i);
      if (dist2min < Dist2) {
	index = i;
	Dist2 = dist2min;
      }
    }

    if (index != 0) {
      if (Dist2 <= Eps2) {
	p = (extrema.Point(index)).Parameter();
	return Standard_True;
      }
    }
  }
  return Standard_False;
}


//=======================================================================
//function : Project
//purpose  : project a vertex on a curve on surface
//=======================================================================

static Standard_Boolean Project(const Handle(Geom2d_Curve)& C,
				const Handle(Geom_Surface)& S,
				const TopoDS_Vertex& V,
				Standard_Real& p)
{
  gp_Pnt P = BRep_Tool::Pnt(V);
  Standard_Real Eps2 = BRep_Tool::Tolerance(V);
  Eps2 *= Eps2;
  
  Handle(Geom2dAdaptor_HCurve) HG2AHC = new Geom2dAdaptor_HCurve();
  HG2AHC->Set(C);
  Handle(GeomAdaptor_HSurface) HGAHS = new GeomAdaptor_HSurface();
  HGAHS->Set(S);
  Adaptor3d_CurveOnSurface ACOS(HG2AHC,HGAHS);

  Standard_Real D1,D2;
  gp_Pnt P1,P2;
  P1 = ACOS.Value(ACOS.FirstParameter());
  P2 = ACOS.Value(ACOS.LastParameter());
  D1 = P1.SquareDistance(P);
  D2 = P2.SquareDistance(P);
  if ( (D1 < D2) && (D1 <= Eps2) ) {
    p = ACOS.FirstParameter();
    return Standard_True;
  }
  else if ( (D2 < D1) && (D2 <= Eps2) ) {
    p = ACOS.LastParameter();
    return Standard_True;
  }
  
  
  Extrema_ExtPC extrema(P,ACOS);
  
  if (extrema.IsDone()) {
    Standard_Integer i, index = 0, n = extrema.NbExt();
    Standard_Real Dist2 = RealLast(), dist2min;
    
    for (i = 1; i <= n; i++) {
      dist2min = extrema.SquareDistance(i);
      if (dist2min < Dist2) {
	index = i;
	Dist2 = dist2min;
      }
    }
    
    if (index != 0) {
      Extrema_POnCurv POC = extrema.Point(index);
      if (P.SquareDistance(POC.Value()) <= Precision::SquareConfusion()) {
	p = POC.Parameter();
	return Standard_True;
      }
    }
  }
  return Standard_False;
}

//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge()
{}

//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const TopoDS_Vertex& V1, 
				   const TopoDS_Vertex& V2)
{
  gp_Pnt P1 = BRep_Tool::Pnt(V1);
  gp_Pnt P2 = BRep_Tool::Pnt(V2);
  Standard_Real l = P1.Distance(P2);
  if (l <= gp::Resolution()) {
    myError = BRepLib_LineThroughIdenticPoints;
    return;
  }
  gp_Lin L(P1,gp_Vec(P1,P2));
  Handle(Geom_Line) GL = new Geom_Line(L);
  Init(GL,V1,V2,0,l);
}


//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Pnt& P1, 
				   const gp_Pnt& P2)
{
  Standard_Real l = P1.Distance(P2);
  if (l <= gp::Resolution()) {
    myError = BRepLib_LineThroughIdenticPoints;
    return;
  }
  gp_Lin L(P1,gp_Vec(P1,P2));
  Handle(Geom_Line) GL = new Geom_Line(L);
  Init(GL,P1,P2,0,l);
}


//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Lin& L)
{
  Handle(Geom_Line) GL = new Geom_Line(L);
  Init(GL);
}


//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Lin& L, 
				   const Standard_Real p1, 
				   const Standard_Real p2)
{
  Handle(Geom_Line) GL = new Geom_Line(L);
  Init(GL,p1,p2);
}


//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Lin& L, 
				   const gp_Pnt& P1, 
				   const gp_Pnt& P2)
{
  Handle(Geom_Line) GL = new Geom_Line(L);
  Init(GL,P1,P2);
}


//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Lin& L, 
				   const TopoDS_Vertex& V1, 
				   const TopoDS_Vertex& V2)
{
  Handle(Geom_Line) GL = new Geom_Line(L);
  Init(GL,V1,V2);
}


//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Circ& C) 
{
  Handle(Geom_Circle) GC = new Geom_Circle(C);
  Init(GC);
}


//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Circ& C,
				   const Standard_Real p1,
				   const Standard_Real p2)
{
  Handle(Geom_Circle) GC = new Geom_Circle(C);
  Init(GC,p1,p2);
}


//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Circ& C,
				   const gp_Pnt& P1,
				   const gp_Pnt& P2)
{
  Handle(Geom_Circle) GC = new Geom_Circle(C);
  Init(GC,P1,P2);
}


//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Circ& C,
				   const TopoDS_Vertex& V1,
				   const TopoDS_Vertex& V2)
{
  Handle(Geom_Circle) GC = new Geom_Circle(C);
  Init(GC,V1,V2);
}


//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Elips& E) 
{
  Handle(Geom_Ellipse) GE = new Geom_Ellipse(E);
  Init(GE);
}


//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Elips& E,
				   const Standard_Real p1,
				   const Standard_Real p2)
{
  Handle(Geom_Ellipse) GE = new Geom_Ellipse(E);
  Init(GE,p1,p2);
}


//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Elips& E,
				   const gp_Pnt& P1,
				   const gp_Pnt& P2)
{
  Handle(Geom_Ellipse) GE = new Geom_Ellipse(E);
  Init(GE,P1,P2);
}


//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Elips& E,
				   const TopoDS_Vertex& V1,
				   const TopoDS_Vertex& V2)
{
  Handle(Geom_Ellipse) GE = new Geom_Ellipse(E);
  Init(GE,V1,V2);
}


//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Hypr& H)
{
  Handle(Geom_Hyperbola) GH = new Geom_Hyperbola(H);
  Init(GH);
}


//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Hypr& H,
				   const Standard_Real p1,
				   const Standard_Real p2)
{
  Handle(Geom_Hyperbola) GH = new Geom_Hyperbola(H);
  Init(GH,p1,p2);
}


//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Hypr& H,
				   const gp_Pnt& P1,
				   const gp_Pnt& P2)
{
  Handle(Geom_Hyperbola) GH = new Geom_Hyperbola(H);
  Init(GH,P1,P2);
}


//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Hypr& H,
				   const TopoDS_Vertex& V1,
				   const TopoDS_Vertex& V2)
{
  Handle(Geom_Hyperbola) GH = new Geom_Hyperbola(H);
  Init(GH,V1,V2);
}


//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Parab& P)
{
  Handle(Geom_Parabola) GP = new Geom_Parabola(P);
  Init(GP);
}


//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Parab& P,
				   const Standard_Real p1,
				   const Standard_Real p2)
{
  Handle(Geom_Parabola) GP = new Geom_Parabola(P);
  Init(GP,p1,p2);
}


//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Parab& P,
				   const gp_Pnt& P1,
				   const gp_Pnt& P2)
{
  Handle(Geom_Parabola) GP = new Geom_Parabola(P);
  Init(GP,P1,P2);
}


//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const gp_Parab& P,
				   const TopoDS_Vertex& V1,
				   const TopoDS_Vertex& V2)
{
  Handle(Geom_Parabola) GP = new Geom_Parabola(P);
  Init(GP,V1,V2);
}


//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const Handle(Geom_Curve)& L)
{
  Init(L);
}


//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const Handle(Geom_Curve)& L,
				   const Standard_Real p1,
				   const Standard_Real p2)
{
  Init(L,p1,p2);
}


//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const Handle(Geom_Curve)& L,
				   const gp_Pnt& P1,
				   const gp_Pnt& P2)
{
  Init(L,P1,P2);
}

//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const Handle(Geom_Curve)& L,
				   const TopoDS_Vertex& V1,
				   const TopoDS_Vertex& V2)
{
  Init(L,V1,V2);
}


//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const Handle(Geom_Curve)& L,
				   const gp_Pnt& P1,
				   const gp_Pnt& P2,
				   const Standard_Real p1,
				   const Standard_Real p2)
{
  Init(L,P1,P2,p1,p2);
}


//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const Handle(Geom_Curve)& L,
				   const TopoDS_Vertex& V1,
				   const TopoDS_Vertex& V2,
				   const Standard_Real p1,
				   const Standard_Real p2)
{
  Init(L,V1,V2,p1,p2);
}



//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const Handle(Geom2d_Curve)& L,
				   const Handle(Geom_Surface)& S)
{
  Init(L,S);
}


//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const Handle(Geom2d_Curve)& L,
				   const Handle(Geom_Surface)& S,
				   const Standard_Real p1,
				   const Standard_Real p2)
{
  Init(L,S,p1,p2);
}


//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const Handle(Geom2d_Curve)& L,
				   const Handle(Geom_Surface)& S,
				   const gp_Pnt& P1,
				   const gp_Pnt& P2)
{
  Init(L,S,P1,P2);
}

//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const Handle(Geom2d_Curve)& L,
				   const Handle(Geom_Surface)& S,
				   const TopoDS_Vertex& V1,
				   const TopoDS_Vertex& V2)
{
  Init(L,S,V1,V2);
}


//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const Handle(Geom2d_Curve)& L,
				   const Handle(Geom_Surface)& S,
				   const gp_Pnt& P1,
				   const gp_Pnt& P2,
				   const Standard_Real p1,
				   const Standard_Real p2)
{
  Init(L,S,P1,P2,p1,p2);
}


//=======================================================================
//function : BRepLib_MakeEdge
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::BRepLib_MakeEdge(const Handle(Geom2d_Curve)& L,
				   const Handle(Geom_Surface)& S,
				   const TopoDS_Vertex& V1,
				   const TopoDS_Vertex& V2,
				   const Standard_Real p1,
				   const Standard_Real p2)
{
  Init(L,S,V1,V2,p1,p2);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepLib_MakeEdge::Init(const Handle(Geom_Curve)& C)
{
  Init(C,C->FirstParameter(),C->LastParameter());
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepLib_MakeEdge::Init(const Handle(Geom_Curve)& C,
			     const Standard_Real p1,
			     const Standard_Real p2)
{
//  BRep_Builder B;

  TopoDS_Vertex V1,V2;
  Init(C,V1,V2,p1,p2);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepLib_MakeEdge::Init(const Handle(Geom_Curve)& C,
			     const gp_Pnt& P1,
			     const gp_Pnt& P2)
{
  Standard_Real Tol = BRepLib::Precision();

  BRep_Builder B;
  TopoDS_Vertex V1,V2;
  B.MakeVertex(V1,P1,Tol);
  if (P1.Distance(P2) < Tol)
    V2 = V1;
  else
    B.MakeVertex(V2,P2,Tol);
  
  Init(C,V1,V2);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepLib_MakeEdge::Init(const Handle(Geom_Curve)& C,
			     const TopoDS_Vertex& V1,
			     const TopoDS_Vertex& V2)
{
  // try projecting the vertices on the curve

  Standard_Real p1,p2;
  
  if (V1.IsNull())
    p1 = C->FirstParameter();
  else
    if (!Project(C,V1,p1)) {
      myError = BRepLib_PointProjectionFailed;
      return;
    }
  if (V2.IsNull())
    p2 = C->LastParameter();
  else
    if (!Project(C,V2,p2))  {
      myError = BRepLib_PointProjectionFailed;
      return;
    }
  
  Init(C,V1,V2,p1,p2);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepLib_MakeEdge::Init(const Handle(Geom_Curve)& C,
			     const gp_Pnt& P1,
			     const gp_Pnt& P2,
			     const Standard_Real p1,
			     const Standard_Real p2)
{
  Standard_Real Tol = BRepLib::Precision();
  BRep_Builder B;

  TopoDS_Vertex V1,V2;
  B.MakeVertex(V1,P1,Tol);
  if (P1.Distance(P2) < Tol)
    V2 = V1;
  else
    B.MakeVertex(V2,P2,Tol);
  
  Init(C,V1,V2,p1,p2);
}


//=======================================================================
//function : Init
//purpose  : this one really makes the job ...
//=======================================================================

void  BRepLib_MakeEdge::Init(const Handle(Geom_Curve)& CC,
			     const TopoDS_Vertex& VV1,
			     const TopoDS_Vertex& VV2,
			     const Standard_Real pp1,
			     const Standard_Real pp2)
{
  // kill trimmed curves
  Handle(Geom_Curve) C = CC;
  Handle(Geom_TrimmedCurve) CT = Handle(Geom_TrimmedCurve)::DownCast(C);
  while (!CT.IsNull()) {
    C = CT->BasisCurve();
    CT = Handle(Geom_TrimmedCurve)::DownCast(C);
  }

  // check parameters
  Standard_Real p1 = pp1;
  Standard_Real p2 = pp2;
  Standard_Real cf = C->FirstParameter();
  Standard_Real cl = C->LastParameter();
  Standard_Real epsilon = Precision::PConfusion();
  Standard_Boolean periodic = C->IsPeriodic();
  GeomAdaptor_Curve aCA(C);

  TopoDS_Vertex V1,V2;
  if (periodic) {
    // adjust in period
    ElCLib::AdjustPeriodic(cf,cl,epsilon,p1,p2);
    V1 = VV1;
    V2 = VV2;
  }
  else {
    // reordonate
    if (p1 < p2) {
      V1 = VV1;
      V2 = VV2;
    }
    else {
      V2 = VV1;
      V1 = VV2;
      Standard_Real x = p1;
      p1 = p2;
      p2 = x;
    }

    // check range
    if ((cf - p1 > epsilon) || (p2 - cl > epsilon)) {
      myError = BRepLib_ParameterOutOfRange;
      return;
    }

    // check ponctuallity
    if ((p2-p1) <= gp::Resolution()) {
      myError = BRepLib_LineThroughIdenticPoints;
      return;
    }
  }
  
  // compute points on the curve
  Standard_Boolean p1inf = Precision::IsNegativeInfinite(p1);
  Standard_Boolean p2inf = Precision::IsPositiveInfinite(p2);
  gp_Pnt P1,P2;
  if (!p1inf) P1 = aCA.Value(p1);
  if (!p2inf) P2 = aCA.Value(p2);

  Standard_Real preci = BRepLib::Precision();
  BRep_Builder B;

  // check for closed curve
  Standard_Boolean closed = Standard_False;
  Standard_Boolean degenerated = Standard_False;
  if (!p1inf && !p2inf)
    closed = (P1.Distance(P2) <= preci);

  // check if the vertices are on the curve
  if (closed) {
    if (V1.IsNull() && V2.IsNull()) {
      B.MakeVertex(V1,P1,preci);
      V2 = V1;
    }
    else if (V1.IsNull())
      V1 = V2;
    else if (V2.IsNull())
      V2 = V1;
    else {
      if (!V1.IsSame(V2)) {
        myError = BRepLib_DifferentPointsOnClosedCurve;
        return;
      }
      else if (P1.Distance(BRep_Tool::Pnt(V1)) > 
        Max(preci,BRep_Tool::Tolerance(V1))) {
        myError = BRepLib_DifferentPointsOnClosedCurve;
        return;
      }
      else
      {
        gp_Pnt PM = aCA.Value((p1+p2)/2);
        if (P1.Distance(PM) < preci)
          degenerated = Standard_True;
      }
    }
  }

  else {    // not closed

    if (p1inf) {
      if (!V1.IsNull()) {
	myError = BRepLib_PointWithInfiniteParameter;
	return;
      }
    }
    else {
      if (V1.IsNull()) {
	B.MakeVertex(V1,P1,preci);
      }
      else if (P1.Distance(BRep_Tool::Pnt(V1)) >
	       Max(preci,BRep_Tool::Tolerance(V1))) {
	myError = BRepLib_DifferentsPointAndParameter;
	return;
      }
    }
    
    if (p2inf) {
      if (!V2.IsNull()) {
	myError = BRepLib_PointWithInfiniteParameter;
	return;
      }
    }
    else {
      if (V2.IsNull()) {
	B.MakeVertex(V2,P2,preci);
      }
      else if (P2.Distance(BRep_Tool::Pnt(V2)) >
	       Max(preci,BRep_Tool::Tolerance(V2))){
	myError = BRepLib_DifferentsPointAndParameter;
	return;
      }
    }
  }

  V1.Orientation(TopAbs_FORWARD);
  V2.Orientation(TopAbs_REVERSED);
  myVertex1 = V1;
  myVertex2 = V2;

  TopoDS_Edge& E = TopoDS::Edge(myShape);
  B.MakeEdge(E,C,preci);
  if (!V1.IsNull()) {
    B.Add(E,V1);
  }
  if (!V2.IsNull()) {
    B.Add(E,V2);
  }
  B.Range(E,p1,p2);
  B.Degenerated(E, degenerated);

  myError = BRepLib_EdgeDone;
  Done();
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepLib_MakeEdge::Init(const Handle(Geom2d_Curve)& C,
			     const Handle(Geom_Surface)& S)
{
  Init(C,S,C->FirstParameter(),C->LastParameter());
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepLib_MakeEdge::Init(const Handle(Geom2d_Curve)& C,
			     const Handle(Geom_Surface)& S,
			     const Standard_Real p1,
			     const Standard_Real p2)
{
//  BRep_Builder B;

  TopoDS_Vertex V1,V2;
  Init(C,S,V1,V2,p1,p2);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepLib_MakeEdge::Init(const Handle(Geom2d_Curve)& C,
			     const Handle(Geom_Surface)& S,
			     const gp_Pnt& P1,
			     const gp_Pnt& P2)
{
  Standard_Real Tol = BRepLib::Precision();
  
  BRep_Builder B;
  TopoDS_Vertex V1,V2;
  B.MakeVertex(V1,P1,Tol);
  if (P1.Distance(P2) < Tol)
    V2 = V1;
  else
    B.MakeVertex(V2,P2,Tol);
  
  Init(C,S,V1,V2);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepLib_MakeEdge::Init(const Handle(Geom2d_Curve)& C,
			     const Handle(Geom_Surface)& S,
			     const TopoDS_Vertex& V1,
			     const TopoDS_Vertex& V2)
{
  // try projecting the vertices on the curve

  Standard_Real p1,p2;
  
  if (V1.IsNull())
    p1 = C->FirstParameter();
  else
    if (!Project(C,S,V1,p1)) {
      myError = BRepLib_PointProjectionFailed;
      return;
    }
  if (V2.IsNull())
    p2 = C->LastParameter();
  else
    if (!Project(C,S,V2,p2))  {
      myError = BRepLib_PointProjectionFailed;
      return;
    }
  
  Init(C,S,V1,V2,p1,p2);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  BRepLib_MakeEdge::Init(const Handle(Geom2d_Curve)& C,
			     const Handle(Geom_Surface)& S,
			     const gp_Pnt& P1,
			     const gp_Pnt& P2,
			     const Standard_Real p1,
			     const Standard_Real p2)
{
  Standard_Real Tol = BRepLib::Precision();
  BRep_Builder B;

  TopoDS_Vertex V1,V2;
  B.MakeVertex(V1,P1,Tol);
  if (P1.Distance(P2) < Tol)
    V2 = V1;
  else
    B.MakeVertex( V2, P2, Tol);
  
  Init(C,S,V1,V2,p1,p2);
}


//=======================================================================
//function : Init
//purpose  : this one really makes the job ...
//=======================================================================

void  BRepLib_MakeEdge::Init(const Handle(Geom2d_Curve)& CC,
			     const Handle(Geom_Surface)& S,
			     const TopoDS_Vertex& VV1,
			     const TopoDS_Vertex& VV2,
			     const Standard_Real pp1,
			     const Standard_Real pp2)
{
  // kill trimmed curves
  Handle(Geom2d_Curve) C = CC;
  Handle(Geom2d_TrimmedCurve) CT = Handle(Geom2d_TrimmedCurve)::DownCast(C);
  while (!CT.IsNull()) {
    C = CT->BasisCurve();
    CT = Handle(Geom2d_TrimmedCurve)::DownCast(C);
  }

  // check parameters
  Standard_Real p1 = pp1;
  Standard_Real p2 = pp2;
  Standard_Real cf = C->FirstParameter();
  Standard_Real cl = C->LastParameter();
  Standard_Real epsilon = Precision::PConfusion();
  Standard_Boolean periodic = C->IsPeriodic();


  TopoDS_Vertex V1,V2;
  Standard_Boolean reverse = Standard_False;

  if (periodic) {
    // adjust in period
    ElCLib::AdjustPeriodic(cf,cl,epsilon,p1,p2);
    V1 = VV1;
    V2 = VV2;
  }
  else {
    // reordonate
    if (p1 < p2) {
      V1 = VV1;
      V2 = VV2;
    }
    else {
      V2 = VV1;
      V1 = VV2;
      Standard_Real x = p1;
      p1 = p2;
      p2 = x;
      reverse = Standard_True;
    }

    // check range
    if ((cf - p1 > epsilon) || (p2 - cl > epsilon)) {
      myError = BRepLib_ParameterOutOfRange;
      return;
    }
  }
  
  // compute points on the curve
  Standard_Boolean p1inf = Precision::IsNegativeInfinite(p1);
  Standard_Boolean p2inf = Precision::IsPositiveInfinite(p2);
  gp_Pnt P1,P2;
  gp_Pnt2d P2d1,P2d2;
  if (!p1inf) {
    P2d1 = C->Value(p1);
    P1   = S->Value(P2d1.X(),P2d1.Y());
  }
  if (!p2inf) {
    P2d2 = C->Value(p2);
    P2   = S->Value(P2d2.X(),P2d2.Y());
  }

  Standard_Real preci = BRepLib::Precision();
  BRep_Builder B;

  // check for closed curve
  Standard_Boolean closed = Standard_False;
  if (!p1inf && !p2inf)
    closed = (P1.Distance(P2) <= preci);

  // check if the vertices are on the curve
  if (closed) {
    if (V1.IsNull() && V2.IsNull()) {
      B.MakeVertex(V1,P1,preci);
      V2 = V1;
    }
    else if (V1.IsNull())
      V1 = V2;
    else if (V2.IsNull())
      V2 = V1;
    else {
      if (!V1.IsSame(V2)) {
	myError = BRepLib_DifferentPointsOnClosedCurve;
	return;
      }
      else if (P1.Distance(BRep_Tool::Pnt(V1)) > 
	       Max(preci,BRep_Tool::Tolerance(V1))) {
	myError = BRepLib_DifferentPointsOnClosedCurve;
	return;
      }
    }
  }

  else {    // not closed

    if (p1inf) {
      if (!V1.IsNull()) {
	myError = BRepLib_PointWithInfiniteParameter;
	return;
      }
    }
    else {
      if (V1.IsNull()) {
	B.MakeVertex(V1,P1,preci);
      }
      else if (P1.Distance(BRep_Tool::Pnt(V1)) >
	       Max(preci,BRep_Tool::Tolerance(V1))) {
	myError = BRepLib_DifferentsPointAndParameter;
	return;
      }
    }
    
    if (p2inf) {
      if (!V2.IsNull()) {
	myError = BRepLib_PointWithInfiniteParameter;
	return;
      }
    }
    else {
      if (V2.IsNull()) {
	B.MakeVertex(V2,P2,preci);
      }
      else if (P2.Distance(BRep_Tool::Pnt(V2)) >
	       Max(preci,BRep_Tool::Tolerance(V2))){
	myError = BRepLib_DifferentsPointAndParameter;
	return;
      }
    }
  }

  V1.Orientation(TopAbs_FORWARD);
  V2.Orientation(TopAbs_REVERSED);
  myVertex1 = V1;
  myVertex2 = V2;

  TopoDS_Edge& E = TopoDS::Edge(myShape);
  B.MakeEdge(E);
  B.UpdateEdge(E,C,S,TopLoc_Location(),preci);

  if (!V1.IsNull()) {
    B.Add(E,V1);
  }
  if (!V2.IsNull()) {
    B.Add(E,V2);
  }
  B.Range(E,p1,p2);

  if (reverse)
    E.Orientation(TopAbs_REVERSED);

  myError = BRepLib_EdgeDone;
  Done();
}

//=======================================================================
//function : Error
//purpose  : 
//=======================================================================

BRepLib_EdgeError BRepLib_MakeEdge::Error() const
{
  return myError;
}

//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================

const TopoDS_Edge&  BRepLib_MakeEdge::Edge()
{
  return TopoDS::Edge(Shape());
}


//=======================================================================
//function : Vertex1
//purpose  : 
//=======================================================================

const TopoDS_Vertex&  BRepLib_MakeEdge::Vertex1()const 
{
  Check();
  return myVertex1;
}


//=======================================================================
//function : Vertex2
//purpose  : 
//=======================================================================

const TopoDS_Vertex&  BRepLib_MakeEdge::Vertex2()const 
{
  Check();
  return myVertex2;
}



//=======================================================================
//function : operator
//purpose  : 
//=======================================================================

BRepLib_MakeEdge::operator TopoDS_Edge()
{
  return Edge();
}
