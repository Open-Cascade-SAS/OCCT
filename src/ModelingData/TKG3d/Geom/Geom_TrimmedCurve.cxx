// Created on: 1993-03-10
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
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_UndefinedDerivative.hxx>
#include <gp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_TrimmedCurve, Geom_BoundedCurve)

typedef Geom_TrimmedCurve TrimmedCurve;
typedef gp_Ax1            Ax1;
typedef gp_Ax2            Ax2;
typedef gp_Pnt            Pnt;
typedef gp_Trsf           Trsf;
typedef gp_Vec            Vec;

//=================================================================================================

occ::handle<Geom_Geometry> Geom_TrimmedCurve::Copy() const
{

  occ::handle<Geom_TrimmedCurve> Tc = new TrimmedCurve(basisCurve, uTrim1, uTrim2);
  return Tc;
}

//=================================================================================================

Geom_TrimmedCurve::Geom_TrimmedCurve(const occ::handle<Geom_Curve>& C,
                                     const double                   U1,
                                     const double                   U2,
                                     const bool                     Sense,
                                     const bool                     theAdjustPeriodic)
    : uTrim1(U1),
      uTrim2(U2)
{
  // kill trimmed basis curves
  occ::handle<Geom_TrimmedCurve> T = occ::down_cast<Geom_TrimmedCurve>(C);
  if (!T.IsNull())
    basisCurve = occ::down_cast<Geom_Curve>(T->BasisCurve()->Copy());
  else
    basisCurve = occ::down_cast<Geom_Curve>(C->Copy());

  SetTrim(U1, U2, Sense, theAdjustPeriodic);
}

//=================================================================================================

void Geom_TrimmedCurve::Reverse()
{
  double U1 = basisCurve->ReversedParameter(uTrim2);
  double U2 = basisCurve->ReversedParameter(uTrim1);
  basisCurve->Reverse();
  SetTrim(U1, U2, true, false);
}

//=================================================================================================

double Geom_TrimmedCurve::ReversedParameter(const double U) const
{
  return basisCurve->ReversedParameter(U);
}

//=================================================================================================

void Geom_TrimmedCurve::SetTrim(const double U1,
                                const double U2,
                                const bool   Sense,
                                const bool   theAdjustPeriodic)
{
  bool sameSense = true;
  if (U1 == U2)
    throw Standard_ConstructionError("Geom_TrimmedCurve::U1 == U2");

  double Udeb = basisCurve->FirstParameter();
  double Ufin = basisCurve->LastParameter();

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
                             std::min(std::abs(uTrim2 - uTrim1) / 2, Precision::PConfusion()),
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
      throw Standard_ConstructionError("Geom_TrimmedCurve::parameters out of range");
  }
  if (!sameSense)
  {
    Reverse();
  }
}

//=================================================================================================

bool Geom_TrimmedCurve::IsClosed() const
{
  return (StartPoint().Distance(EndPoint()) <= gp::Resolution());
}

//=================================================================================================

bool Geom_TrimmedCurve::IsPeriodic() const
{
  // return basisCurve->IsPeriodic();
  return false;
}

//=================================================================================================

double Geom_TrimmedCurve::Period() const
{
  return basisCurve->Period();
}

//=================================================================================================

GeomAbs_Shape Geom_TrimmedCurve::Continuity() const
{

  return basisCurve->Continuity();
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_TrimmedCurve::BasisCurve() const
{

  return basisCurve;
}

//=================================================================================================

std::optional<gp_Pnt> Geom_TrimmedCurve::EvalD0(const double U) const
{
  return basisCurve->EvalD0(U);
}

//=================================================================================================

std::optional<Geom_Curve::ResD1> Geom_TrimmedCurve::EvalD1(const double U) const
{
  return basisCurve->EvalD1(U);
}

//=================================================================================================

std::optional<Geom_Curve::ResD2> Geom_TrimmedCurve::EvalD2(const double U) const
{
  return basisCurve->EvalD2(U);
}

//=================================================================================================

std::optional<Geom_Curve::ResD3> Geom_TrimmedCurve::EvalD3(const double U) const
{
  return basisCurve->EvalD3(U);
}

//=================================================================================================

std::optional<gp_Vec> Geom_TrimmedCurve::EvalDN(const double U, const int N) const
{
  return basisCurve->EvalDN(U, N);
}

//=================================================================================================

Pnt Geom_TrimmedCurve::EndPoint() const
{

  return basisCurve->Value(uTrim2);
}

//=================================================================================================

double Geom_TrimmedCurve::FirstParameter() const
{

  return uTrim1;
}

//=================================================================================================

double Geom_TrimmedCurve::LastParameter() const
{

  return uTrim2;
}

//=================================================================================================

Pnt Geom_TrimmedCurve::StartPoint() const
{

  return basisCurve->Value(uTrim1);
}

//=================================================================================================

bool Geom_TrimmedCurve::IsCN(const int N) const
{

  Standard_RangeError_Raise_if(N < 0, " ");
  return basisCurve->IsCN(N);
}

//=================================================================================================

void Geom_TrimmedCurve::Transform(const Trsf& T)
{
  basisCurve->Transform(T);
  double U1 = basisCurve->TransformedParameter(uTrim1, T);
  double U2 = basisCurve->TransformedParameter(uTrim2, T);
  SetTrim(U1, U2, true, false);
}

//=================================================================================================

double Geom_TrimmedCurve::TransformedParameter(const double U, const gp_Trsf& T) const
{
  return basisCurve->TransformedParameter(U, T);
}

//=================================================================================================

double Geom_TrimmedCurve::ParametricTransformation(const gp_Trsf& T) const
{
  return basisCurve->ParametricTransformation(T);
}

//=================================================================================================

void Geom_TrimmedCurve::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_BoundedCurve)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, basisCurve.get())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, uTrim1)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, uTrim2)
}
