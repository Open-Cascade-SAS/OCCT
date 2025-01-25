// Created on: 1993-03-24
// Created by: JCV
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

#include <ElCLib.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Geometry.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_UndefinedDerivative.hxx>
#include <gp.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom2d_TrimmedCurve, Geom2d_BoundedCurve)

typedef Geom2d_TrimmedCurve TrimmedCurve;
typedef gp_Ax2d             Ax2d;
typedef gp_Pnt2d            Pnt2d;
typedef gp_Trsf2d           Trsf2d;
typedef gp_Vec2d            Vec2d;

//=================================================================================================

Handle(Geom2d_Geometry) Geom2d_TrimmedCurve::Copy() const
{
  Handle(Geom2d_TrimmedCurve) Tc;
  Tc = new TrimmedCurve(basisCurve, uTrim1, uTrim2);
  return Tc;
}

//=================================================================================================

Geom2d_TrimmedCurve::Geom2d_TrimmedCurve(const Handle(Geom2d_Curve)& C,
                                         const Standard_Real         U1,
                                         const Standard_Real         U2,
                                         const Standard_Boolean      Sense,
                                         const Standard_Boolean      theAdjustPeriodic)
    : uTrim1(U1),
      uTrim2(U2)
{
  if (C.IsNull())
    throw Standard_ConstructionError("Geom2d_TrimmedCurve:: C is null");
  // kill trimmed basis curves
  Handle(Geom2d_TrimmedCurve) T = Handle(Geom2d_TrimmedCurve)::DownCast(C);
  if (!T.IsNull())
    basisCurve = Handle(Geom2d_Curve)::DownCast(T->BasisCurve()->Copy());
  else
    basisCurve = Handle(Geom2d_Curve)::DownCast(C->Copy());

  SetTrim(U1, U2, Sense, theAdjustPeriodic);
}

//=================================================================================================

void Geom2d_TrimmedCurve::Reverse()
{
  Standard_Real U1 = basisCurve->ReversedParameter(uTrim2);
  Standard_Real U2 = basisCurve->ReversedParameter(uTrim1);
  basisCurve->Reverse();
  SetTrim(U1, U2, Standard_True, Standard_False);
}

//=================================================================================================

Standard_Real Geom2d_TrimmedCurve::ReversedParameter(const Standard_Real U) const
{
  return basisCurve->ReversedParameter(U);
}

//=================================================================================================

void Geom2d_TrimmedCurve::SetTrim(const Standard_Real    U1,
                                  const Standard_Real    U2,
                                  const Standard_Boolean Sense,
                                  const Standard_Boolean theAdjustPeriodic)
{
  Standard_Boolean sameSense = Standard_True;
  if (U1 == U2)
    throw Standard_ConstructionError("Geom2d_TrimmedCurve::U1 == U2");

  Standard_Real Udeb = basisCurve->FirstParameter();
  Standard_Real Ufin = basisCurve->LastParameter();

  if (basisCurve->IsPeriodic())
  {
    sameSense = Sense;

    // set uTrim1 in the range Udeb , Ufin
    // set uTrim2 in the range uTrim1 , uTrim1 + Period()
    uTrim1 = U1;
    uTrim2 = U2;
    if (theAdjustPeriodic)
      ElCLib::AdjustPeriodic(Udeb,
                             Ufin,
                             Min(Abs(uTrim2 - uTrim1) / 2, Precision::PConfusion()),
                             uTrim1,
                             uTrim2);
  }
  else
  {
    if (U1 < U2)
    {
      sameSense = Sense;
      uTrim1    = U1;
      uTrim2    = U2;
    }
    else
    {
      sameSense = !Sense;
      uTrim1    = U2;
      uTrim2    = U1;
    }

    if ((Udeb - uTrim1 > Precision::PConfusion()) || (uTrim2 - Ufin > Precision::PConfusion()))
    {
      throw Standard_ConstructionError("Geom_TrimmedCurve::parameters out of range");
    }
  }

  if (!sameSense)
    Reverse();
}

//=================================================================================================

Handle(Geom2d_Curve) Geom2d_TrimmedCurve::BasisCurve() const
{
  return basisCurve;
}

//=================================================================================================

GeomAbs_Shape Geom2d_TrimmedCurve::Continuity() const
{
  return basisCurve->Continuity();
}

//=================================================================================================

Standard_Boolean Geom2d_TrimmedCurve::IsCN(const Standard_Integer N) const
{
  Standard_RangeError_Raise_if(N < 0, " ");
  return basisCurve->IsCN(N);
}

//=================================================================================================

Pnt2d Geom2d_TrimmedCurve::EndPoint() const
{
  return basisCurve->Value(uTrim2);
}

//=================================================================================================

Standard_Real Geom2d_TrimmedCurve::FirstParameter() const
{
  return uTrim1;
}

//=================================================================================================

Standard_Boolean Geom2d_TrimmedCurve::IsClosed() const
{
  Standard_Real Dist = Value(FirstParameter()).Distance(Value(LastParameter()));
  return (Dist <= gp::Resolution());
}

//=================================================================================================

Standard_Boolean Geom2d_TrimmedCurve::IsPeriodic() const
{
  // return basisCurve->IsPeriodic();
  return Standard_False;
}

//=================================================================================================

Standard_Real Geom2d_TrimmedCurve::Period() const
{
  return basisCurve->Period();
}

//=================================================================================================

Standard_Real Geom2d_TrimmedCurve::LastParameter() const
{
  return uTrim2;
}

//=================================================================================================

Pnt2d Geom2d_TrimmedCurve::StartPoint() const
{
  gp_Pnt2d P;
  P = basisCurve->Value(uTrim1);
  return P;
}

//=================================================================================================

void Geom2d_TrimmedCurve::D0(const Standard_Real U, Pnt2d& P) const
{
  basisCurve->D0(U, P);
}

//=================================================================================================

void Geom2d_TrimmedCurve::D1(const Standard_Real U, Pnt2d& P, Vec2d& V1) const
{
  basisCurve->D1(U, P, V1);
}

//=================================================================================================

void Geom2d_TrimmedCurve::D2(const Standard_Real U, Pnt2d& P, Vec2d& V1, Vec2d& V2) const
{
  basisCurve->D2(U, P, V1, V2);
}

//=================================================================================================

void Geom2d_TrimmedCurve::D3(const Standard_Real U, Pnt2d& P, Vec2d& V1, Vec2d& V2, Vec2d& V3) const
{
  basisCurve->D3(U, P, V1, V2, V3);
}

//=================================================================================================

Vec2d Geom2d_TrimmedCurve::DN(const Standard_Real U, const Standard_Integer N) const
{
  return basisCurve->DN(U, N);
}

//=================================================================================================

void Geom2d_TrimmedCurve::Transform(const Trsf2d& T)
{
  basisCurve->Transform(T);
  Standard_Real U1 = basisCurve->TransformedParameter(uTrim1, T);
  Standard_Real U2 = basisCurve->TransformedParameter(uTrim2, T);
  SetTrim(U1, U2, Standard_True, Standard_False);
}

//=================================================================================================

Standard_Real Geom2d_TrimmedCurve::TransformedParameter(const Standard_Real U,
                                                        const gp_Trsf2d&    T) const
{
  return basisCurve->TransformedParameter(U, T);
}

//=================================================================================================

Standard_Real Geom2d_TrimmedCurve::ParametricTransformation(const gp_Trsf2d& T) const
{
  return basisCurve->ParametricTransformation(T);
}

//=================================================================================================

void Geom2d_TrimmedCurve::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom2d_BoundedCurve)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, basisCurve.get())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, uTrim1)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, uTrim2)
}
