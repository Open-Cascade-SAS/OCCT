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

#include <BSplCLib.hxx>
#include <BSplSLib.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Geometry.hxx>
#include "Geom_RevolutionUtils.pxx"
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_UndefinedDerivative.hxx>
#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir.hxx>
#include <gp_GTrsf2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_SurfaceOfRevolution, Geom_SweptSurface)

#define POLES (poles->Array2())
#define WEIGHTS (weights->Array2())
#define UKNOTS (uknots->Array1())
#define VKNOTS (vknots->Array1())
#define UFKNOTS (ufknots->Array1())
#define VFKNOTS (vfknots->Array1())
#define FMULTS (BSplCLib::NoMults())

typedef Geom_SurfaceOfRevolution SurfaceOfRevolution;
typedef Geom_Curve               Curve;
typedef gp_Ax1                   Ax1;
typedef gp_Ax2                   Ax2;
typedef gp_Dir                   Dir;
typedef gp_Lin                   Lin;
typedef gp_Pnt                   Pnt;
typedef gp_Trsf                  Trsf;
typedef gp_Vec                   Vec;
typedef gp_XYZ                   XYZ;

//=================================================================================================

occ::handle<Geom_Geometry> Geom_SurfaceOfRevolution::Copy() const
{

  return new Geom_SurfaceOfRevolution(basisCurve, Axis());
}

//=================================================================================================

Geom_SurfaceOfRevolution::Geom_SurfaceOfRevolution(const occ::handle<Geom_Curve>& C, const Ax1& A1)
    : loc(A1.Location())
{
  direction = A1.Direction();
  SetBasisCurve(C);
}

//=================================================================================================

void Geom_SurfaceOfRevolution::UReverse()
{
  direction.Reverse();
}

//=================================================================================================

double Geom_SurfaceOfRevolution::UReversedParameter(const double U) const
{

  return (2. * M_PI - U);
}

//=================================================================================================

void Geom_SurfaceOfRevolution::VReverse()
{

  basisCurve->Reverse();
}

//=================================================================================================

double Geom_SurfaceOfRevolution::VReversedParameter(const double V) const
{

  return basisCurve->ReversedParameter(V);
}

//=================================================================================================

const gp_Pnt& Geom_SurfaceOfRevolution::Location() const
{

  return loc;
}

//=================================================================================================

bool Geom_SurfaceOfRevolution::IsUPeriodic() const
{

  return true;
}

//=================================================================================================

bool Geom_SurfaceOfRevolution::IsCNu(const int) const
{

  return true;
}

//=================================================================================================

Ax1 Geom_SurfaceOfRevolution::Axis() const
{

  return Ax1(loc, direction);
}

//=================================================================================================

bool Geom_SurfaceOfRevolution::IsCNv(const int N) const
{

  Standard_RangeError_Raise_if(N < 0, " ");
  return basisCurve->IsCN(N);
}

//=================================================================================================

bool Geom_SurfaceOfRevolution::IsUClosed() const
{

  return true;
}

//=================================================================================================

bool Geom_SurfaceOfRevolution::IsVClosed() const
{
  return basisCurve->IsClosed();
}

//=================================================================================================

bool Geom_SurfaceOfRevolution::IsVPeriodic() const
{

  return basisCurve->IsPeriodic();
}

//=================================================================================================

void Geom_SurfaceOfRevolution::SetAxis(const Ax1& A1)
{
  direction = A1.Direction();
  loc       = A1.Location();
}

//=================================================================================================

void Geom_SurfaceOfRevolution::SetDirection(const Dir& V)
{
  direction = V;
}

//=================================================================================================

void Geom_SurfaceOfRevolution::SetBasisCurve(const occ::handle<Geom_Curve>& C)
{
  basisCurve = occ::down_cast<Geom_Curve>(C->Copy());
  smooth     = C->Continuity();
}

//=================================================================================================

void Geom_SurfaceOfRevolution::SetLocation(const Pnt& P)
{
  loc = P;
}

//=================================================================================================

void Geom_SurfaceOfRevolution::Bounds(double& U1, double& U2, double& V1, double& V2) const
{

  U1 = 0.0;
  U2 = 2.0 * M_PI;
  V1 = basisCurve->FirstParameter();
  V2 = basisCurve->LastParameter();
}

//=================================================================================================

std::optional<gp_Pnt> Geom_SurfaceOfRevolution::EvalD0(const double U, const double V) const
{
  std::optional<gp_Pnt> aBasisD0 = basisCurve->EvalD0(V);
  if (!aBasisD0)
    return std::nullopt;
  gp_Pnt aP;
  Geom_RevolutionUtils::CalculateD0(*aBasisD0, U, gp_Ax1(loc, direction), aP);
  return aP;
}

//=================================================================================================

std::optional<Geom_Surface::ResD1> Geom_SurfaceOfRevolution::EvalD1(const double U,
                                                                    const double V) const
{
  std::optional<Geom_Curve::ResD1> aBasisD1 = basisCurve->EvalD1(V);
  if (!aBasisD1)
    return std::nullopt;
  std::optional<Geom_Surface::ResD1> aResult{std::in_place};
  Geom_RevolutionUtils::CalculateD1(aBasisD1->Point,
                                    aBasisD1->D1,
                                    U,
                                    gp_Ax1(loc, direction),
                                    aResult->Point,
                                    aResult->D1U,
                                    aResult->D1V);
  return aResult;
}

//=================================================================================================

std::optional<Geom_Surface::ResD2> Geom_SurfaceOfRevolution::EvalD2(const double U,
                                                                    const double V) const
{
  std::optional<Geom_Curve::ResD2> aBasisD2 = basisCurve->EvalD2(V);
  if (!aBasisD2)
    return std::nullopt;
  std::optional<Geom_Surface::ResD2> aResult{std::in_place};
  Geom_RevolutionUtils::CalculateD2(aBasisD2->Point,
                                    aBasisD2->D1,
                                    aBasisD2->D2,
                                    U,
                                    gp_Ax1(loc, direction),
                                    aResult->Point,
                                    aResult->D1U,
                                    aResult->D1V,
                                    aResult->D2U,
                                    aResult->D2V,
                                    aResult->D2UV);
  return aResult;
}

//=================================================================================================

std::optional<Geom_Surface::ResD3> Geom_SurfaceOfRevolution::EvalD3(const double U,
                                                                    const double V) const
{
  std::optional<Geom_Curve::ResD3> aBasisD3 = basisCurve->EvalD3(V);
  if (!aBasisD3)
    return std::nullopt;
  std::optional<Geom_Surface::ResD3> aResult{std::in_place};
  Geom_RevolutionUtils::CalculateD3(aBasisD3->Point,
                                    aBasisD3->D1,
                                    aBasisD3->D2,
                                    aBasisD3->D3,
                                    U,
                                    gp_Ax1(loc, direction),
                                    aResult->Point,
                                    aResult->D1U,
                                    aResult->D1V,
                                    aResult->D2U,
                                    aResult->D2V,
                                    aResult->D2UV,
                                    aResult->D3U,
                                    aResult->D3V,
                                    aResult->D3UUV,
                                    aResult->D3UVV);
  return aResult;
}

//=================================================================================================

std::optional<gp_Vec> Geom_SurfaceOfRevolution::EvalDN(const double U,
                                                       const double V,
                                                       const int    Nu,
                                                       const int    Nv) const
{
  if (Nu + Nv < 1 || Nu < 0 || Nv < 0)
    return std::nullopt;
  gp_Vec aCurvePtOrDN;
  if (Nu == 0)
  {
    std::optional<gp_Vec> aDN = basisCurve->EvalDN(V, Nv);
    if (!aDN)
      return std::nullopt;
    aCurvePtOrDN = *aDN;
  }
  else if (Nv == 0)
  {
    std::optional<gp_Pnt> aD0 = basisCurve->EvalD0(V);
    if (!aD0)
      return std::nullopt;
    aCurvePtOrDN = gp_Vec(aD0->XYZ() - loc.XYZ());
  }
  else
  {
    std::optional<gp_Vec> aDN = basisCurve->EvalDN(V, Nv);
    if (!aDN)
      return std::nullopt;
    aCurvePtOrDN = *aDN;
  }
  return Geom_RevolutionUtils::CalculateDN(aCurvePtOrDN, U, gp_Ax1(loc, direction), Nu, Nv);
}

//=================================================================================================

Ax2 Geom_SurfaceOfRevolution::ReferencePlane() const
{

  throw Standard_NotImplemented();
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_SurfaceOfRevolution::UIso(const double U) const
{

  occ::handle<Geom_Curve> C       = occ::down_cast<Geom_Curve>(basisCurve->Copy());
  Ax1                     RotAxis = Ax1(loc, direction);
  C->Rotate(RotAxis, U);
  return C;
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_SurfaceOfRevolution::VIso(const double V) const
{

  occ::handle<Geom_Circle> Circ;
  Pnt                      Pc = basisCurve->Value(V);
  gp_Lin                   L1(loc, direction);
  double                   Rad = L1.Distance(Pc);

  Ax2 Rep;
  if (Rad > gp::Resolution())
  {
    XYZ P = Pc.XYZ();
    XYZ C;
    C.SetLinearForm((P - loc.XYZ()).Dot(direction.XYZ()), direction.XYZ(), loc.XYZ());
    P = P - C;
    if (P.Modulus() > gp::Resolution())
    {
      gp_Dir D = P.Normalized();
      Rep      = gp_Ax2(C, direction, D);
    }
    else
      Rep = gp_Ax2(C, direction);
  }
  else
    Rep = gp_Ax2(Pc, direction);

  Circ = new Geom_Circle(Rep, Rad);
  return Circ;
}

//=================================================================================================

void Geom_SurfaceOfRevolution::Transform(const Trsf& T)
{
  loc.Transform(T);
  direction.Transform(T);
  basisCurve->Transform(T);
  if (T.ScaleFactor() * T.HVectorialPart().Determinant() < 0.)
    UReverse();
}

//=================================================================================================

void Geom_SurfaceOfRevolution::TransformParameters(double&, double& V, const gp_Trsf& T) const
{
  V = basisCurve->TransformedParameter(V, T);
}

//=================================================================================================

gp_GTrsf2d Geom_SurfaceOfRevolution::ParametricTransformation(const gp_Trsf& T) const
{
  gp_GTrsf2d T2;
  gp_Ax2d    Axis(gp::Origin2d(), gp::DX2d());
  T2.SetAffinity(Axis, basisCurve->ParametricTransformation(T));
  return T2;
}

//=================================================================================================

void Geom_SurfaceOfRevolution::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_SweptSurface)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &loc)
}
