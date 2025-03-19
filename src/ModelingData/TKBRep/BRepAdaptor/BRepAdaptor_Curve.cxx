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

#include <BRepAdaptor_Curve.hxx>

#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRep_Tool.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NullObject.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <Geom_OffsetCurve.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepAdaptor_Curve, Adaptor3d_Curve)

//=================================================================================================

BRepAdaptor_Curve::BRepAdaptor_Curve() {}

//=================================================================================================

BRepAdaptor_Curve::BRepAdaptor_Curve(const TopoDS_Edge& E)
{
  Initialize(E);
}

//=================================================================================================

BRepAdaptor_Curve::BRepAdaptor_Curve(const TopoDS_Edge& E, const TopoDS_Face& F)
{
  Initialize(E, F);
}

//=================================================================================================

Handle(Adaptor3d_Curve) BRepAdaptor_Curve::ShallowCopy() const
{
  Handle(BRepAdaptor_Curve) aCopy = new BRepAdaptor_Curve();

  aCopy->myTrsf = myTrsf;

  const Handle(Adaptor3d_Curve) aCurve     = myCurve.ShallowCopy();
  const GeomAdaptor_Curve&      aGeomCurve = *(Handle(GeomAdaptor_Curve)::DownCast(aCurve));
  aCopy->myCurve                           = aGeomCurve;

  if (!myConSurf.IsNull())
  {
    aCopy->myConSurf = Handle(Adaptor3d_CurveOnSurface)::DownCast(myConSurf->ShallowCopy());
  }
  aCopy->myEdge = myEdge;

  return aCopy;
}

//=================================================================================================

void BRepAdaptor_Curve::Reset()
{
  myCurve.Reset();
  myConSurf.Nullify();
  myEdge.Nullify();
  myTrsf = gp_Trsf();
}

//=================================================================================================

void BRepAdaptor_Curve::Initialize(const TopoDS_Edge& E)
{
  myConSurf.Nullify();
  myEdge = E;
  Standard_Real pf, pl;

  TopLoc_Location    L;
  Handle(Geom_Curve) C = BRep_Tool::Curve(E, L, pf, pl);

  if (!C.IsNull())
  {
    myCurve.Load(C, pf, pl);
  }
  else
  {
    Handle(Geom2d_Curve) PC;
    Handle(Geom_Surface) S;
    BRep_Tool::CurveOnSurface(E, PC, S, L, pf, pl);
    if (!PC.IsNull())
    {
      Handle(GeomAdaptor_Surface) HS = new GeomAdaptor_Surface();
      HS->Load(S);
      Handle(Geom2dAdaptor_Curve) HC = new Geom2dAdaptor_Curve();
      HC->Load(PC, pf, pl);
      myConSurf = new Adaptor3d_CurveOnSurface();
      myConSurf->Load(HC, HS);
    }
    else
    {
      throw Standard_NullObject("BRepAdaptor_Curve::No geometry");
    }
  }
  myTrsf = L.Transformation();
}

//=================================================================================================

void BRepAdaptor_Curve::Initialize(const TopoDS_Edge& E, const TopoDS_Face& F)
{
  myConSurf.Nullify();

  myEdge = E;
  TopLoc_Location      L;
  Standard_Real        pf, pl;
  Handle(Geom_Surface) S  = BRep_Tool::Surface(F, L);
  Handle(Geom2d_Curve) PC = BRep_Tool::CurveOnSurface(E, F, pf, pl);

  Handle(GeomAdaptor_Surface) HS = new GeomAdaptor_Surface();
  HS->Load(S);
  Handle(Geom2dAdaptor_Curve) HC = new Geom2dAdaptor_Curve();
  HC->Load(PC, pf, pl);
  myConSurf = new Adaptor3d_CurveOnSurface();
  myConSurf->Load(HC, HS);

  myTrsf = L.Transformation();
}

//=================================================================================================

const gp_Trsf& BRepAdaptor_Curve::Trsf() const
{
  return myTrsf;
}

//=================================================================================================

Standard_Boolean BRepAdaptor_Curve::Is3DCurve() const
{
  return myConSurf.IsNull();
}

//=================================================================================================

Standard_Boolean BRepAdaptor_Curve::IsCurveOnSurface() const
{
  return !myConSurf.IsNull();
}

//=================================================================================================

const GeomAdaptor_Curve& BRepAdaptor_Curve::Curve() const
{
  return myCurve;
}

//=================================================================================================

const Adaptor3d_CurveOnSurface& BRepAdaptor_Curve::CurveOnSurface() const
{
  return *myConSurf;
}

//=================================================================================================

const TopoDS_Edge& BRepAdaptor_Curve::Edge() const
{
  return myEdge;
}

//=================================================================================================

Standard_Real BRepAdaptor_Curve::Tolerance() const
{
  return BRep_Tool::Tolerance(myEdge);
}

//=================================================================================================

Standard_Real BRepAdaptor_Curve::FirstParameter() const
{
  if (myConSurf.IsNull())
  {
    return myCurve.FirstParameter();
  }
  else
  {
    return myConSurf->FirstParameter();
  }
}

//=================================================================================================

Standard_Real BRepAdaptor_Curve::LastParameter() const
{
  if (myConSurf.IsNull())
  {
    return myCurve.LastParameter();
  }
  else
  {
    return myConSurf->LastParameter();
  }
}

//=================================================================================================

GeomAbs_Shape BRepAdaptor_Curve::Continuity() const
{
  if (myConSurf.IsNull())
  {
    return myCurve.Continuity();
  }
  else
  {
    return myConSurf->Continuity();
  }
}

//=================================================================================================

Standard_Integer BRepAdaptor_Curve::NbIntervals(const GeomAbs_Shape S) const
{
  if (myConSurf.IsNull())
  {
    return myCurve.NbIntervals(S);
  }
  else
  {
    return myConSurf->NbIntervals(S);
  }
}

//=================================================================================================

void BRepAdaptor_Curve::Intervals(TColStd_Array1OfReal& T, const GeomAbs_Shape S) const
{
  if (myConSurf.IsNull())
  {
    myCurve.Intervals(T, S);
  }
  else
  {
    myConSurf->Intervals(T, S);
  }
}

//=================================================================================================

Handle(Adaptor3d_Curve) BRepAdaptor_Curve::Trim(const Standard_Real First,
                                                const Standard_Real Last,
                                                const Standard_Real Tol) const
{
  // On fait une copie de this pour garder la trsf.
  Handle(BRepAdaptor_Curve) res;
  if (myConSurf.IsNull())
  {
    Standard_Real      pf = FirstParameter(), pl = LastParameter();
    Handle(Geom_Curve) C = myCurve.Curve();
    const_cast<GeomAdaptor_Curve*>(&myCurve)->Load(C, First, Last);
    res = new BRepAdaptor_Curve(*this);
    const_cast<GeomAdaptor_Curve*>(&myCurve)->Load(C, pf, pl);
  }
  else
  {
    Handle(Adaptor3d_CurveOnSurface) sav = myConSurf;
    const_cast<Handle(Adaptor3d_CurveOnSurface)&>(myConSurf) =
      Handle(Adaptor3d_CurveOnSurface)::DownCast(myConSurf->Trim(First, Last, Tol));
    res                                                      = new BRepAdaptor_Curve(*this);
    const_cast<Handle(Adaptor3d_CurveOnSurface)&>(myConSurf) = sav;
  }
  return res;
}

//=================================================================================================

Standard_Boolean BRepAdaptor_Curve::IsClosed() const
{
  if (myConSurf.IsNull())
  {
    return myCurve.IsClosed();
  }
  else
  {
    return myConSurf->IsClosed();
  }
}

//=================================================================================================

Standard_Boolean BRepAdaptor_Curve::IsPeriodic() const
{
  if (myConSurf.IsNull())
  {
    return myCurve.IsPeriodic();
  }
  else
  {
    return myConSurf->IsPeriodic();
  }
}

//=================================================================================================

Standard_Real BRepAdaptor_Curve::Period() const
{
  if (myConSurf.IsNull())
  {
    return myCurve.Period();
  }
  else
  {
    return myConSurf->Period();
  }
}

//=================================================================================================

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

//=================================================================================================

void BRepAdaptor_Curve::D0(const Standard_Real U, gp_Pnt& P) const
{
  if (myConSurf.IsNull())
    myCurve.D0(U, P);
  else
    myConSurf->D0(U, P);
  P.Transform(myTrsf);
}

//=================================================================================================

void BRepAdaptor_Curve::D1(const Standard_Real U, gp_Pnt& P, gp_Vec& V) const
{
  if (myConSurf.IsNull())
    myCurve.D1(U, P, V);
  else
    myConSurf->D1(U, P, V);
  P.Transform(myTrsf);
  V.Transform(myTrsf);
}

//=================================================================================================

void BRepAdaptor_Curve::D2(const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const
{
  if (myConSurf.IsNull())
    myCurve.D2(U, P, V1, V2);
  else
    myConSurf->D2(U, P, V1, V2);
  P.Transform(myTrsf);
  V1.Transform(myTrsf);
  V2.Transform(myTrsf);
}

//=================================================================================================

void BRepAdaptor_Curve::D3(const Standard_Real U,
                           gp_Pnt&             P,
                           gp_Vec&             V1,
                           gp_Vec&             V2,
                           gp_Vec&             V3) const
{
  if (myConSurf.IsNull())
    myCurve.D3(U, P, V1, V2, V3);
  else
    myConSurf->D3(U, P, V1, V2, V3);
  P.Transform(myTrsf);
  V1.Transform(myTrsf);
  V2.Transform(myTrsf);
  V3.Transform(myTrsf);
}

//=================================================================================================

gp_Vec BRepAdaptor_Curve::DN(const Standard_Real U, const Standard_Integer N) const
{
  gp_Vec V;
  if (myConSurf.IsNull())
    V = myCurve.DN(U, N);
  else
    V = myConSurf->DN(U, N);
  V.Transform(myTrsf);
  return V;
}

//=================================================================================================

Standard_Real BRepAdaptor_Curve::Resolution(const Standard_Real R) const
{
  if (myConSurf.IsNull())
  {
    return myCurve.Resolution(R);
  }
  else
  {
    return myConSurf->Resolution(R);
  }
}

//=================================================================================================

GeomAbs_CurveType BRepAdaptor_Curve::GetType() const
{
  if (myConSurf.IsNull())
  {
    return myCurve.GetType();
  }
  else
  {
    return myConSurf->GetType();
  }
}

//=================================================================================================

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

//=================================================================================================

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

//=================================================================================================

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

//=================================================================================================

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

//=================================================================================================

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

//=================================================================================================

Standard_Integer BRepAdaptor_Curve::Degree() const
{
  if (myConSurf.IsNull())
    return myCurve.Degree();
  else
    return myConSurf->Degree();
}

//=================================================================================================

Standard_Boolean BRepAdaptor_Curve::IsRational() const
{
  if (myConSurf.IsNull())
    return myCurve.IsRational();
  else
    return myConSurf->IsRational();
}

//=================================================================================================

Standard_Integer BRepAdaptor_Curve::NbPoles() const
{
  if (myConSurf.IsNull())
    return myCurve.NbPoles();
  else
    return myConSurf->NbPoles();
}

//=================================================================================================

Standard_Integer BRepAdaptor_Curve::NbKnots() const
{
  if (myConSurf.IsNull())
    return myCurve.NbKnots();
  else
    return myConSurf->NbKnots();
}

//=================================================================================================

Handle(Geom_BezierCurve) BRepAdaptor_Curve::Bezier() const
{
  Handle(Geom_BezierCurve) BC;
  if (myConSurf.IsNull())
  {
    BC = myCurve.Bezier();
  }
  else
  {
    BC = myConSurf->Bezier();
  }
  return myTrsf.Form() == gp_Identity ? BC
                                      : Handle(Geom_BezierCurve)::DownCast(BC->Transformed(myTrsf));
}

//=================================================================================================

Handle(Geom_BSplineCurve) BRepAdaptor_Curve::BSpline() const
{
  Handle(Geom_BSplineCurve) BS;
  if (myConSurf.IsNull())
  {
    BS = myCurve.BSpline();
  }
  else
  {
    BS = myConSurf->BSpline();
  }
  return myTrsf.Form() == gp_Identity
           ? BS
           : Handle(Geom_BSplineCurve)::DownCast(BS->Transformed(myTrsf));
}

//=================================================================================================

Handle(Geom_OffsetCurve) BRepAdaptor_Curve::OffsetCurve() const
{
  if (!Is3DCurve() || myCurve.GetType() != GeomAbs_OffsetCurve)
    throw Standard_NoSuchObject("BRepAdaptor_Curve::OffsetCurve");

  Handle(Geom_OffsetCurve) anOffC = myCurve.OffsetCurve();
  return myTrsf.Form() == gp_Identity
           ? anOffC
           : Handle(Geom_OffsetCurve)::DownCast(anOffC->Transformed(myTrsf));
}
