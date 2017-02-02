// Created on: 1993-02-19
// Created by: Remi LEQUETTE
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


#include <Adaptor3d_CurveOnSurface.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HCurveOnSurface.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_HCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <Geom_OffsetCurve.hxx>
#include <GeomAdaptor_HCurve.hxx>

//=======================================================================
//function : BRepAdaptor_Curve
//purpose  : 
//=======================================================================
BRepAdaptor_Curve::BRepAdaptor_Curve()
{}

//=======================================================================
//function : BRepAdaptor_Curve
//purpose  : 
//=======================================================================

BRepAdaptor_Curve::BRepAdaptor_Curve(const TopoDS_Edge& E)
{
  Initialize(E);
}

//=======================================================================
//function : BRepAdaptor_Curve
//purpose  : 
//=======================================================================

BRepAdaptor_Curve::BRepAdaptor_Curve(const TopoDS_Edge& E,
				     const TopoDS_Face& F)
{
  Initialize(E,F);
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void BRepAdaptor_Curve::Initialize(const TopoDS_Edge& E)
{
  myConSurf.Nullify();
  myEdge = E;
  Standard_Real pf,pl;

  TopLoc_Location L;
  Handle(Geom_Curve) C = BRep_Tool::Curve(E,L,pf,pl);

  if (!C.IsNull()) {
    myCurve.Load(C,pf,pl);
  }
  else {
    Handle(Geom2d_Curve) PC;
    Handle(Geom_Surface) S;
    BRep_Tool::CurveOnSurface(E,PC,S,L,pf,pl);
    if (!PC.IsNull()) {
      Handle(GeomAdaptor_HSurface) HS = new GeomAdaptor_HSurface();
      HS->ChangeSurface().Load(S);
      Handle(Geom2dAdaptor_HCurve) HC = new Geom2dAdaptor_HCurve();
      HC->ChangeCurve2d().Load(PC,pf,pl);
      myConSurf = new Adaptor3d_HCurveOnSurface();
      myConSurf->ChangeCurve().Load(HC, HS);
    }
    else {
      throw Standard_NullObject("BRepAdaptor_Curve::No geometry");
    }
  }
  myTrsf = L.Transformation();
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void BRepAdaptor_Curve::Initialize(const TopoDS_Edge& E,
				   const TopoDS_Face& F)
{
  myConSurf.Nullify();

  myEdge = E;
  TopLoc_Location L;
  Standard_Real pf,pl;
  Handle(Geom_Surface) S = BRep_Tool::Surface(F,L);
  Handle(Geom2d_Curve) PC = BRep_Tool::CurveOnSurface(E,F,pf,pl); 

  Handle(GeomAdaptor_HSurface) HS = new GeomAdaptor_HSurface();
  HS->ChangeSurface().Load(S);
  Handle(Geom2dAdaptor_HCurve) HC = new Geom2dAdaptor_HCurve();
  HC->ChangeCurve2d().Load(PC,pf,pl);
  myConSurf = new Adaptor3d_HCurveOnSurface();
  myConSurf->ChangeCurve().Load(HC, HS);
  
  myTrsf = L.Transformation();
}

//=======================================================================
//function : Trsf
//purpose  : 
//=======================================================================

const gp_Trsf& BRepAdaptor_Curve::Trsf() const
{
  return myTrsf;
}

//=======================================================================
//function : Is3DCurve
//purpose  : 
//=======================================================================

Standard_Boolean BRepAdaptor_Curve::Is3DCurve() const
{
  return myConSurf.IsNull();
}

//=======================================================================
//function : IsCurveOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean BRepAdaptor_Curve::IsCurveOnSurface() const
{
  return !myConSurf.IsNull();
}

//=======================================================================
//function : Curve
//purpose  : 
//=======================================================================

const GeomAdaptor_Curve& BRepAdaptor_Curve::Curve() const
{
  return myCurve;
}

//=======================================================================
//function : CurveOnSurface
//purpose  : 
//=======================================================================

const Adaptor3d_CurveOnSurface& BRepAdaptor_Curve::CurveOnSurface() const
{
  return *((Adaptor3d_CurveOnSurface*)&(myConSurf->Curve()));
}

//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================

const TopoDS_Edge& BRepAdaptor_Curve::Edge() const
{
  return myEdge;
}

//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

Standard_Real BRepAdaptor_Curve::Tolerance() const
{
  return BRep_Tool::Tolerance(myEdge);
}

//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================

Standard_Real BRepAdaptor_Curve::FirstParameter() const 
{
  if (myConSurf.IsNull()) {
    return myCurve.FirstParameter();
  }
  else {
    return myConSurf->FirstParameter();
  }
}

//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================

Standard_Real BRepAdaptor_Curve::LastParameter() const 
{
  if (myConSurf.IsNull()) {
    return myCurve.LastParameter();
  }
  else {
    return myConSurf->LastParameter();
  }
}

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape BRepAdaptor_Curve::Continuity() const 
{
  if (myConSurf.IsNull()) {
    return myCurve.Continuity();
  }
  else {
    return myConSurf->Continuity();
  }
}

//=======================================================================
//function : NbIntervals
//purpose  : 
//=======================================================================

Standard_Integer BRepAdaptor_Curve::NbIntervals(const GeomAbs_Shape S) const
{
  if (myConSurf.IsNull()) {
    return myCurve.NbIntervals(S);
  }
  else {
    return myConSurf->NbIntervals(S);
  }
}

//=======================================================================
//function : Intervals
//purpose  : 
//=======================================================================

void BRepAdaptor_Curve::Intervals(TColStd_Array1OfReal& T, 
                                  const GeomAbs_Shape S) const
{
  if (myConSurf.IsNull()) {
    myCurve.Intervals(T, S);
  }
  else {
    myConSurf->Intervals(T, S);
  }
}


//=======================================================================
//function : Trim
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HCurve) BRepAdaptor_Curve::Trim(const Standard_Real First, 
					       const Standard_Real Last, 
					       const Standard_Real Tol) const 
{
  // On fait une copie de this pour garder la trsf.
  Handle(BRepAdaptor_HCurve) res = new BRepAdaptor_HCurve();
  if (myConSurf.IsNull()){
    Standard_Real pf = FirstParameter(), pl = LastParameter();
    Handle(Geom_Curve) C = myCurve.Curve();
    ((GeomAdaptor_Curve*) (void*) &myCurve)->Load(C,First,Last);
    res->ChangeCurve() = *this;
    ((GeomAdaptor_Curve*) (void*) &myCurve)->Load(C,pf,pl);
  }
  else {
    Handle(Adaptor3d_HCurveOnSurface) sav = myConSurf;
    *((Handle(Adaptor3d_HCurveOnSurface)*) (void*) &myConSurf) = 
      Handle(Adaptor3d_HCurveOnSurface)::DownCast(myConSurf->Trim(First,Last,Tol));
    res->ChangeCurve() = *this;
    *((Handle(Adaptor3d_HCurveOnSurface)*) (void*) &myConSurf) = sav;
  }
  return res;
}


//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================

Standard_Boolean BRepAdaptor_Curve::IsClosed() const 
{
  if (myConSurf.IsNull()) {
    return myCurve.IsClosed();
  }
  else {
    return myConSurf->IsClosed();
  }
}

//=======================================================================
//function : IsPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean BRepAdaptor_Curve::IsPeriodic() const 
{
  if (myConSurf.IsNull()) {
    return myCurve.IsPeriodic();
  }
  else {
    return myConSurf->IsPeriodic();
  }
}

//=======================================================================
//function : Period
//purpose  : 
//=======================================================================

Standard_Real BRepAdaptor_Curve::Period() const 
{
  if (myConSurf.IsNull()) {
    return myCurve.Period();
  }
  else {
    return myConSurf->Period();
  }
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

gp_Pnt BRepAdaptor_Curve::Value(const Standard_Real U) const
{
  gp_Pnt P;
  if (myConSurf.IsNull())
    P = myCurve.Value(U);
  else
    P = myConSurf->Value(U);
  P.Transform(myTrsf);
  return P;
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void BRepAdaptor_Curve::D0(const Standard_Real U, gp_Pnt& P) const
{
  if (myConSurf.IsNull())
    myCurve.D0(U,P);
  else
    myConSurf->D0(U,P);
  P.Transform(myTrsf);
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void BRepAdaptor_Curve::D1(const Standard_Real U,
			   gp_Pnt& P, gp_Vec& V) const
{
  if (myConSurf.IsNull())
    myCurve.D1(U,P,V);
  else
    myConSurf->D1(U,P,V);
  P.Transform(myTrsf);
  V.Transform(myTrsf);
}

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void BRepAdaptor_Curve::D2(const Standard_Real U,
			   gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const
{
  if (myConSurf.IsNull())
    myCurve.D2(U,P,V1,V2);
  else
    myConSurf->D2(U,P,V1,V2);
  P.Transform(myTrsf);
  V1.Transform(myTrsf);
  V2.Transform(myTrsf);
}

//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void BRepAdaptor_Curve::D3(const Standard_Real U,
			   gp_Pnt& P, 
			   gp_Vec& V1, gp_Vec& V2, gp_Vec& V3) const
{
  if (myConSurf.IsNull())
    myCurve.D3(U,P,V1,V2,V3);
  else
    myConSurf->D3(U,P,V1,V2,V3);
  P.Transform(myTrsf);
  V1.Transform(myTrsf);
  V2.Transform(myTrsf);
  V3.Transform(myTrsf);
}

//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

gp_Vec BRepAdaptor_Curve::DN(const Standard_Real U,
			     const Standard_Integer N) const
{
  gp_Vec V;
  if (myConSurf.IsNull())
    V = myCurve.DN(U,N);
  else
    V = myConSurf->DN(U,N);
  V.Transform(myTrsf);
  return V;
}

//=======================================================================
//function : Resolution
//purpose  : 
//=======================================================================

Standard_Real BRepAdaptor_Curve::Resolution(const Standard_Real R) const 
{
  if (myConSurf.IsNull()) {
    return myCurve.Resolution(R);
  }
  else {
    return myConSurf->Resolution(R);
  }
}

//=======================================================================
//function : GetTYpe
//purpose  : 
//=======================================================================

GeomAbs_CurveType BRepAdaptor_Curve::GetType() const 
{
  if (myConSurf.IsNull()) {
    return myCurve.GetType();
  }
  else {
    return myConSurf->GetType();
  }
}

//=======================================================================
//function : Line
//purpose  : 
//=======================================================================

gp_Lin BRepAdaptor_Curve::Line() const
{
  gp_Lin L;
  if (myConSurf.IsNull())
    L = myCurve.Line();
  else
    L = myConSurf->Line();
  L.Transform(myTrsf);
  return L;
}

//=======================================================================
//function : Circle
//purpose  : 
//=======================================================================

gp_Circ BRepAdaptor_Curve::Circle() const
{
  gp_Circ C;
  if (myConSurf.IsNull())
    C = myCurve.Circle();
  else
    C = myConSurf->Circle();
  C.Transform(myTrsf);
  return C;
}

//=======================================================================
//function : Ellipse
//purpose  : 
//=======================================================================

gp_Elips BRepAdaptor_Curve::Ellipse() const
{
  gp_Elips E;
  if (myConSurf.IsNull())
    E = myCurve.Ellipse();
  else
    E = myConSurf->Ellipse();
  E.Transform(myTrsf);
  return E;
}

//=======================================================================
//function : Hyperbola
//purpose  : 
//=======================================================================

gp_Hypr BRepAdaptor_Curve::Hyperbola() const
{
  gp_Hypr H;
  if (myConSurf.IsNull())
    H = myCurve.Hyperbola();
  else
    H = myConSurf->Hyperbola();
  H.Transform(myTrsf);
  return H;
}

//=======================================================================
//function : Parabola
//purpose  : 
//=======================================================================

gp_Parab BRepAdaptor_Curve::Parabola() const
{
  gp_Parab P;
  if (myConSurf.IsNull())
    P = myCurve.Parabola();
  else
    P = myConSurf->Parabola();
  P.Transform(myTrsf);
  return P;
}

//=======================================================================
//function : Degree
//purpose  : 
//=======================================================================

Standard_Integer BRepAdaptor_Curve::Degree() const
{
  if (myConSurf.IsNull())
    return myCurve.Degree();
  else
    return myConSurf->Degree();
}

//=======================================================================
//function : IsRational
//purpose  : 
//=======================================================================

Standard_Boolean BRepAdaptor_Curve::IsRational() const
{
  if (myConSurf.IsNull())
    return myCurve.IsRational();
  else
    return myConSurf->IsRational();
}
//=======================================================================
//function : NbPoles
//purpose  : 
//=======================================================================

Standard_Integer BRepAdaptor_Curve::NbPoles() const
{
  if (myConSurf.IsNull()) 
    return myCurve.NbPoles();
  else
    return myConSurf->NbPoles();
}
//=======================================================================
//function : NbKnots
//purpose  : 
//=======================================================================

Standard_Integer BRepAdaptor_Curve::NbKnots() const
{
  if (myConSurf.IsNull()) 
    return myCurve.NbKnots();
  else
    return myConSurf->NbKnots();
}

//=======================================================================
//function : Bezier
//purpose  : 
//=======================================================================

Handle(Geom_BezierCurve) BRepAdaptor_Curve::Bezier() const 
{
  Handle(Geom_BezierCurve) BC;
  if (myConSurf.IsNull()) {
    BC = myCurve.Bezier();
  }
  else {
    BC = myConSurf->Bezier();
  }
  return myTrsf.Form() == gp_Identity
    ? BC : Handle(Geom_BezierCurve)::DownCast(BC->Transformed(myTrsf));
}


//=======================================================================
//function : BSpline
//purpose  : 
//=======================================================================

Handle(Geom_BSplineCurve) BRepAdaptor_Curve::BSpline() const 
{
  Handle(Geom_BSplineCurve) BS;
  if (myConSurf.IsNull()) {
    BS = myCurve.BSpline();
  }
  else {
    BS = myConSurf->BSpline();
  }
  return myTrsf.Form() == gp_Identity
    ? BS : Handle(Geom_BSplineCurve)::DownCast(BS->Transformed(myTrsf));
}

//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

Handle(Geom_OffsetCurve) BRepAdaptor_Curve::OffsetCurve() const
{
  if ( !Is3DCurve() || myCurve.GetType() != GeomAbs_OffsetCurve)
    throw Standard_NoSuchObject("BRepAdaptor_Curve::OffsetCurve");

  Handle(Geom_OffsetCurve) anOffC = myCurve.OffsetCurve();
  return myTrsf.Form() == gp_Identity
    ? anOffC : Handle(Geom_OffsetCurve)::DownCast(anOffC->Transformed(myTrsf));
}
