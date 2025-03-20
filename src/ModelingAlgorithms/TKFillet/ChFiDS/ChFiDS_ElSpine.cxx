// Created on: 1995-05-04
// Created by: Laurent BOURESCHE
// Copyright (c) 1995-1999 Matra Datavision
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

#include <Adaptor3d_Curve.hxx>
#include <ChFiDS_ElSpine.hxx>
#include <ChFiDS_SurfData.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <gp_Ax1.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ChFiDS_ElSpine, Adaptor3d_Curve)

//=================================================================================================

ChFiDS_ElSpine::ChFiDS_ElSpine()
    : pfirst(0.0),
      plast(0.0),
      period(0.0),
      periodic(Standard_False),
      pfirstsav(Precision::Infinite()),
      plastsav(Precision::Infinite())
{
}

//=================================================================================================

Handle(Adaptor3d_Curve) ChFiDS_ElSpine::ShallowCopy() const
{
  Handle(ChFiDS_ElSpine) aCopy = new ChFiDS_ElSpine();

  const Handle(Adaptor3d_Curve) aCurve     = curve.ShallowCopy();
  const GeomAdaptor_Curve&      aGeomCurve = *(Handle(GeomAdaptor_Curve)::DownCast(aCurve));
  aCopy->curve                             = aGeomCurve;

  aCopy->ptfirst              = ptfirst;
  aCopy->ptlast               = ptlast;
  aCopy->tgfirst              = tgfirst;
  aCopy->tglast               = tglast;
  aCopy->VerticesWithTangents = VerticesWithTangents;
  aCopy->previous             = previous;
  aCopy->next                 = next;
  aCopy->pfirst               = pfirst;
  aCopy->plast                = plast;
  aCopy->period               = period;
  aCopy->periodic             = periodic;
  aCopy->pfirstsav            = pfirstsav;
  aCopy->plastsav             = plastsav;

  return aCopy;
}

//=================================================================================================

Standard_Real ChFiDS_ElSpine::FirstParameter() const
{
  return pfirst;
}

//=================================================================================================

Standard_Real ChFiDS_ElSpine::LastParameter() const
{
  return plast;
}

//=================================================================================================

Standard_Real ChFiDS_ElSpine::GetSavedFirstParameter() const
{
  return pfirstsav;
}

//=================================================================================================

Standard_Real ChFiDS_ElSpine::GetSavedLastParameter() const
{
  return plastsav;
}

//=================================================================================================

GeomAbs_Shape ChFiDS_ElSpine::Continuity() const
{
  return curve.Continuity();
}

//=================================================================================================

Standard_Integer ChFiDS_ElSpine::NbIntervals(const GeomAbs_Shape S) const
{
  return curve.NbIntervals(S);
}

//=================================================================================================

void ChFiDS_ElSpine::Intervals(TColStd_Array1OfReal& T, const GeomAbs_Shape S) const
{
  curve.Intervals(T, S);
}

//=================================================================================================

Handle(Adaptor3d_Curve) ChFiDS_ElSpine::Trim(const Standard_Real First,
                                             const Standard_Real Last,
                                             const Standard_Real Tol) const
{
  return curve.Trim(First, Last, Tol);
}

//=================================================================================================

Standard_Real ChFiDS_ElSpine::Resolution(const Standard_Real R3d) const
{
  return curve.Resolution(R3d);
}

//=================================================================================================

GeomAbs_CurveType ChFiDS_ElSpine::GetType() const
{
  return curve.GetType();
}

//=================================================================================================

Standard_Boolean ChFiDS_ElSpine::IsPeriodic() const
{
  return periodic;
}

//=================================================================================================

void ChFiDS_ElSpine::SetPeriodic(const Standard_Boolean I)
{
  periodic = I;
  period   = plast - pfirst;
}

//=================================================================================================

Standard_Real ChFiDS_ElSpine::Period() const
{
  if (!periodic)
    throw Standard_Failure("ElSpine non periodique");
  return period;
}

//=================================================================================================

gp_Pnt ChFiDS_ElSpine::Value(const Standard_Real AbsC) const
{
  return curve.Value(AbsC);
}

//=================================================================================================

void ChFiDS_ElSpine::D0(const Standard_Real AbsC, gp_Pnt& P) const
{
  curve.D0(AbsC, P);
}

//=================================================================================================

void ChFiDS_ElSpine::D1(const Standard_Real AbsC, gp_Pnt& P, gp_Vec& V1) const
{
  curve.D1(AbsC, P, V1);
}

//=================================================================================================

void ChFiDS_ElSpine::D2(const Standard_Real AbsC, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const
{
  curve.D2(AbsC, P, V1, V2);
}

//=================================================================================================

void ChFiDS_ElSpine::D3(const Standard_Real AbsC,
                        gp_Pnt&             P,
                        gp_Vec&             V1,
                        gp_Vec&             V2,
                        gp_Vec&             V3) const
{
  curve.D3(AbsC, P, V1, V2, V3);
}

//=================================================================================================

void ChFiDS_ElSpine::FirstParameter(const Standard_Real P)
{
  pfirst = P;
}

//=================================================================================================

void ChFiDS_ElSpine::LastParameter(const Standard_Real P)
{
  plast = P;
}

//=================================================================================================

void ChFiDS_ElSpine::SaveFirstParameter()
{
  pfirstsav = pfirst;
}

//=================================================================================================

void ChFiDS_ElSpine::SaveLastParameter()
{
  plastsav = plast;
}

//=================================================================================================

void ChFiDS_ElSpine::SetOrigin(const Standard_Real O)
{
  if (!periodic)
    throw Standard_Failure("Elspine non periodique");
  Handle(Geom_BSplineCurve) bs = Handle(Geom_BSplineCurve)::DownCast(curve.Curve());
  if (!bs.IsNull())
  {
    bs->SetOrigin(O, Precision::PConfusion());
    curve.Load(bs);
  }
}

//=================================================================================================

void ChFiDS_ElSpine::SetFirstPointAndTgt(const gp_Pnt& P, const gp_Vec& T)
{
  ptfirst = P;
  tgfirst = T;
}

//=================================================================================================

void ChFiDS_ElSpine::SetLastPointAndTgt(const gp_Pnt& P, const gp_Vec& T)
{
  ptlast = P;
  tglast = T;
}

//=================================================================================================

void ChFiDS_ElSpine::AddVertexWithTangent(const gp_Ax1& anAx1)
{
  VerticesWithTangents.Append(anAx1);
}

//=================================================================================================

void ChFiDS_ElSpine::FirstPointAndTgt(gp_Pnt& P, gp_Vec& T) const
{
  P = ptfirst;
  T = tgfirst;
}

//=================================================================================================

void ChFiDS_ElSpine::LastPointAndTgt(gp_Pnt& P, gp_Vec& T) const
{
  P = ptlast;
  T = tglast;
}

//=================================================================================================

Standard_Integer ChFiDS_ElSpine::NbVertices() const
{
  return VerticesWithTangents.Length();
}

//=================================================================================================

const gp_Ax1& ChFiDS_ElSpine::VertexWithTangent(const Standard_Integer Index) const
{
  return VerticesWithTangents(Index);
}

//=================================================================================================

void ChFiDS_ElSpine::SetCurve(const Handle(Geom_Curve)& C)
{
  curve.Load(C);
}

//=================================================================================================

const Handle(ChFiDS_SurfData)& ChFiDS_ElSpine::Previous() const
{
  return previous;
}

//=================================================================================================

Handle(ChFiDS_SurfData)& ChFiDS_ElSpine::ChangePrevious()
{
  return previous;
}

//=================================================================================================

const Handle(ChFiDS_SurfData)& ChFiDS_ElSpine::Next() const
{
  return next;
}

//=================================================================================================

Handle(ChFiDS_SurfData)& ChFiDS_ElSpine::ChangeNext()
{
  return next;
}

//    --
//    --     The following methods must  be called when GetType returned
//    --     the corresponding type.
//    --

//=================================================================================================

gp_Lin ChFiDS_ElSpine::Line() const
{
  return curve.Line();
}

//=================================================================================================

gp_Circ ChFiDS_ElSpine::Circle() const
{
  return curve.Circle();
}

//=================================================================================================

gp_Elips ChFiDS_ElSpine::Ellipse() const
{
  return curve.Ellipse();
}

//=================================================================================================

gp_Hypr ChFiDS_ElSpine::Hyperbola() const
{
  return curve.Hyperbola();
}

//=================================================================================================

gp_Parab ChFiDS_ElSpine::Parabola() const
{
  return curve.Parabola();
}

//=================================================================================================

Handle(Geom_BezierCurve) ChFiDS_ElSpine::Bezier() const
{
  return curve.Bezier();
}

//=================================================================================================

Handle(Geom_BSplineCurve) ChFiDS_ElSpine::BSpline() const
{
  return curve.BSpline();
}
