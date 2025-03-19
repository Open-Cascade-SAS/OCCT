// Created on: 1997-01-10
// Created by: Bruno DUMORTIER
// Copyright (c) 1997-1999 Matra Datavision
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

#include <BiTgte_CurveOnEdge.hxx>

#include <Adaptor3d_Curve.hxx>
#include <BRep_Tool.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <TopoDS_Edge.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BiTgte_CurveOnEdge, Adaptor3d_Curve)

//=================================================================================================

BiTgte_CurveOnEdge::BiTgte_CurveOnEdge()
    : myType(GeomAbs_OtherCurve)
{
}

//=================================================================================================

BiTgte_CurveOnEdge::BiTgte_CurveOnEdge(const TopoDS_Edge& theEonF, const TopoDS_Edge& theEdge)
    : myEdge(theEdge),
      myEonF(theEonF),
      myType(GeomAbs_OtherCurve)
{
  Init(theEonF, theEdge);
}

//=================================================================================================

Handle(Adaptor3d_Curve) BiTgte_CurveOnEdge::ShallowCopy() const
{
  Handle(BiTgte_CurveOnEdge) aCopy = new BiTgte_CurveOnEdge();

  aCopy->myEdge = myEdge;
  aCopy->myEonF = myEonF;
  aCopy->myCurv = myCurv;
  aCopy->myConF = myConF;
  aCopy->myType = myType;
  aCopy->myCirc = myCirc;

  return aCopy;
}

//=================================================================================================

void BiTgte_CurveOnEdge::Init(const TopoDS_Edge& EonF, const TopoDS_Edge& Edge)
{
  Standard_Real f, l;

  myEdge = Edge;
  myCurv = BRep_Tool::Curve(myEdge, f, l);
  myCurv = new Geom_TrimmedCurve(myCurv, f, l);

  myEonF = EonF;
  myConF = BRep_Tool::Curve(myEonF, f, l);
  myConF = new Geom_TrimmedCurve(myConF, f, l);

  // peut on generer un cercle de rayon nul
  GeomAdaptor_Curve Curv(myCurv);
  GeomAdaptor_Curve ConF(myConF);

  myType = GeomAbs_OtherCurve;
  if (Curv.GetType() == GeomAbs_Line && ConF.GetType() == GeomAbs_Circle)
  {
    gp_Ax1 a1 = Curv.Line().Position();
    gp_Ax1 a2 = ConF.Circle().Axis();
    if (a1.IsCoaxial(a2, Precision::Angular(), Precision::Confusion()))
    {
      myType = GeomAbs_Circle;
      myCirc = gp_Circ(ConF.Circle().Position(), 0.);
    }
  }
}

//=================================================================================================

Standard_Real BiTgte_CurveOnEdge::FirstParameter() const
{
  return myConF->FirstParameter();
}

//=================================================================================================

Standard_Real BiTgte_CurveOnEdge::LastParameter() const
{
  return myConF->LastParameter();
}

//=================================================================================================

GeomAbs_Shape BiTgte_CurveOnEdge::Continuity() const
{
  throw Standard_NotImplemented("BiTgte_CurveOnEdge");
}

//=================================================================================================

Standard_Integer BiTgte_CurveOnEdge::NbIntervals(const GeomAbs_Shape) const
{
  throw Standard_NotImplemented("BiTgte_CurveOnEdge");
}

//=================================================================================================

void BiTgte_CurveOnEdge::Intervals(TColStd_Array1OfReal&, const GeomAbs_Shape) const
{
  throw Standard_NotImplemented("BiTgte_CurveOnEdge");
}

//=================================================================================================

Handle(Adaptor3d_Curve) BiTgte_CurveOnEdge::Trim(const Standard_Real,
                                                 const Standard_Real,
                                                 const Standard_Real) const
{
  throw Standard_NotImplemented("BiTgte_CurveOnEdge");
}

//=================================================================================================

Standard_Boolean BiTgte_CurveOnEdge::IsClosed() const
{
  throw Standard_NotImplemented("BiTgte_CurveOnEdge");
}

//=================================================================================================

Standard_Boolean BiTgte_CurveOnEdge::IsPeriodic() const
{
  throw Standard_NotImplemented("BiTgte_CurveOnEdge");
}

//=================================================================================================

Standard_Real BiTgte_CurveOnEdge::Period() const
{
  throw Standard_NotImplemented("BiTgte_CurveOnEdge");
}

//=================================================================================================

gp_Pnt BiTgte_CurveOnEdge::Value(const Standard_Real U) const
{
  gp_Pnt P;
  D0(U, P);
  return P;
}

//=================================================================================================

void BiTgte_CurveOnEdge::D0(const Standard_Real U, gp_Pnt& P) const
{
  GeomAPI_ProjectPointOnCurve Projector;
  P = myConF->Value(U);
  Projector.Init(P, myCurv);
  P = Projector.NearestPoint();
}

//=================================================================================================

void BiTgte_CurveOnEdge::D1(const Standard_Real, gp_Pnt&, gp_Vec&) const
{
  throw Standard_NotImplemented("BiTgte_CurveOnEdge");
}

//=================================================================================================

void BiTgte_CurveOnEdge::D2(const Standard_Real, gp_Pnt&, gp_Vec&, gp_Vec&) const
{
  throw Standard_NotImplemented("BiTgte_CurveOnEdge");
}

//=================================================================================================

void BiTgte_CurveOnEdge::D3(const Standard_Real, gp_Pnt&, gp_Vec&, gp_Vec&, gp_Vec&) const
{
  throw Standard_NotImplemented("BiTgte_CurveOnEdge");
}

//=================================================================================================

gp_Vec BiTgte_CurveOnEdge::DN(const Standard_Real, const Standard_Integer) const
{
  throw Standard_NotImplemented("BiTgte_CurveOnEdge");
}

//=================================================================================================

Standard_Real BiTgte_CurveOnEdge::Resolution(const Standard_Real) const
{
  throw Standard_NotImplemented("BiTgte_CurveOnEdge");
}

//=================================================================================================

GeomAbs_CurveType BiTgte_CurveOnEdge::GetType() const
{
  return myType;
}

//=================================================================================================

gp_Lin BiTgte_CurveOnEdge::Line() const
{
  throw Standard_NotImplemented("BiTgte_CurveOnEdge");
}

//=================================================================================================

gp_Circ BiTgte_CurveOnEdge::Circle() const
{
  if (myType != GeomAbs_Circle)
  {
    throw Standard_NoSuchObject("BiTgte_CurveOnEdge::Circle");
  }

  return myCirc;
}

//=================================================================================================

gp_Elips BiTgte_CurveOnEdge::Ellipse() const
{
  throw Standard_NotImplemented("BiTgte_CurveOnEdge");
}

//=================================================================================================

gp_Hypr BiTgte_CurveOnEdge::Hyperbola() const
{
  throw Standard_NotImplemented("BiTgte_CurveOnEdge");
}

//=================================================================================================

gp_Parab BiTgte_CurveOnEdge::Parabola() const
{
  throw Standard_NotImplemented("BiTgte_CurveOnEdge");
}

//=================================================================================================

Standard_Integer BiTgte_CurveOnEdge::Degree() const
{
  throw Standard_NotImplemented("BiTgte_CurveOnEdge");
}

//=================================================================================================

Standard_Boolean BiTgte_CurveOnEdge::IsRational() const
{
  throw Standard_NotImplemented("BiTgte_CurveOnEdge");
}

//=================================================================================================

Standard_Integer BiTgte_CurveOnEdge::NbPoles() const
{
  throw Standard_NotImplemented("BiTgte_CurveOnEdge");
}

//=================================================================================================

Standard_Integer BiTgte_CurveOnEdge::NbKnots() const
{
  throw Standard_NotImplemented("BiTgte_CurveOnEdge");
}

//=================================================================================================

Handle(Geom_BezierCurve) BiTgte_CurveOnEdge::Bezier() const
{
  throw Standard_NotImplemented("BiTgte_CurveOnEdge");
}

//=================================================================================================

Handle(Geom_BSplineCurve) BiTgte_CurveOnEdge::BSpline() const
{
  throw Standard_NotImplemented("BiTgte_CurveOnEdge");
}
