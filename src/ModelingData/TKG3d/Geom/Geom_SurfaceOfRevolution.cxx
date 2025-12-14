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

#include <cmath>

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

namespace
{
//! Shifts a point by offset along axis.
inline void shiftPoint(const XYZ& theLoc, const XYZ& theAxis, const XYZ& thePoint, XYZ& theShift)
{
  theShift = thePoint - theLoc;
  double aScalar = theShift.Dot(theAxis);
  theShift.SetLinearForm(-aScalar, theAxis, theShift);
}

//! Rotates point around axis.
inline void rotatePoint(const double theSin,
                        const double theCos,
                        const XYZ&   theLoc,
                        const XYZ&   theAxis,
                        const XYZ&   thePoint,
                        XYZ&         theResult)
{
  XYZ aShift;
  shiftPoint(theLoc, theAxis, thePoint, aShift);
  XYZ aCross = theAxis.Crossed(aShift);
  theResult.SetLinearForm(theCos - 1.0, aShift, theSin, aCross, thePoint);
}

} // namespace

//=================================================================================================

Handle(Geom_Geometry) Geom_SurfaceOfRevolution::Copy() const
{

  return new Geom_SurfaceOfRevolution(basisCurve, Axis());
}

//=================================================================================================

Geom_SurfaceOfRevolution::Geom_SurfaceOfRevolution(const Handle(Geom_Curve)& C, const Ax1& A1)
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

Standard_Real Geom_SurfaceOfRevolution::UReversedParameter(const Standard_Real U) const
{

  return (2. * M_PI - U);
}

//=================================================================================================

void Geom_SurfaceOfRevolution::VReverse()
{

  basisCurve->Reverse();
}

//=================================================================================================

Standard_Real Geom_SurfaceOfRevolution::VReversedParameter(const Standard_Real V) const
{

  return basisCurve->ReversedParameter(V);
}

//=================================================================================================

const gp_Pnt& Geom_SurfaceOfRevolution::Location() const
{

  return loc;
}

//=================================================================================================

Standard_Boolean Geom_SurfaceOfRevolution::IsUPeriodic() const
{

  return Standard_True;
}

//=================================================================================================

Standard_Boolean Geom_SurfaceOfRevolution::IsCNu(const Standard_Integer) const
{

  return Standard_True;
}

//=================================================================================================

Ax1 Geom_SurfaceOfRevolution::Axis() const
{

  return Ax1(loc, direction);
}

//=================================================================================================

Standard_Boolean Geom_SurfaceOfRevolution::IsCNv(const Standard_Integer N) const
{

  Standard_RangeError_Raise_if(N < 0, " ");
  return basisCurve->IsCN(N);
}

//=================================================================================================

Standard_Boolean Geom_SurfaceOfRevolution::IsUClosed() const
{

  return Standard_True;
}

//=================================================================================================

Standard_Boolean Geom_SurfaceOfRevolution::IsVClosed() const
{
  return basisCurve->IsClosed();
}

//=================================================================================================

Standard_Boolean Geom_SurfaceOfRevolution::IsVPeriodic() const
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

void Geom_SurfaceOfRevolution::SetBasisCurve(const Handle(Geom_Curve)& C)
{
  basisCurve = Handle(Geom_Curve)::DownCast(C->Copy());
  smooth     = C->Continuity();
}

//=================================================================================================

void Geom_SurfaceOfRevolution::SetLocation(const Pnt& P)
{
  loc = P;
}

//=================================================================================================

void Geom_SurfaceOfRevolution::Bounds(Standard_Real& U1,
                                      Standard_Real& U2,
                                      Standard_Real& V1,
                                      Standard_Real& V2) const
{

  U1 = 0.0;
  U2 = 2.0 * M_PI;
  V1 = basisCurve->FirstParameter();
  V2 = basisCurve->LastParameter();
}

//=================================================================================================

void Geom_SurfaceOfRevolution::D0(const Standard_Real U, const Standard_Real V, Pnt& P) const
{
  const double aSin = std::sin(U);
  const double aCos = std::cos(U);
  basisCurve->D0(V, P);
  XYZ aRes;
  rotatePoint(aSin, aCos, loc.XYZ(), direction.XYZ(), P.XYZ(), aRes);
  P.SetXYZ(aRes);
}

//=================================================================================================

void Geom_SurfaceOfRevolution::D1(const Standard_Real U,
                                  const Standard_Real V,
                                  Pnt&                P,
                                  Vec&                D1U,
                                  Vec&                D1V) const
{
  const double aSin = std::sin(U);
  const double aCos = std::cos(U);
  Vec          aD1V;
  basisCurve->D1(V, P, aD1V);

  const XYZ& aLoc  = loc.XYZ();
  const XYZ& aAxis = direction.XYZ();
  const XYZ& aPnt  = P.XYZ();

  // Rotate point
  XYZ aRes;
  rotatePoint(aSin, aCos, aLoc, aAxis, aPnt, aRes);
  P.SetXYZ(aRes);

  // D1U: derivative of rotation with respect to U
  XYZ aShift;
  shiftPoint(aLoc, aAxis, aPnt, aShift);
  XYZ aCross = aAxis.Crossed(aShift);
  XYZ aD1Ures;
  aD1Ures.SetLinearForm(-aSin, aShift, aCos, aCross);
  D1U.SetXYZ(aD1Ures);

  // D1V: rotate the curve derivative
  XYZ aD1Vres;
  rotatePoint(aSin, aCos, XYZ(0, 0, 0), aAxis, aD1V.XYZ(), aD1Vres);
  D1V.SetXYZ(aD1Vres);
}

//=================================================================================================

void Geom_SurfaceOfRevolution::D2(const Standard_Real U,
                                  const Standard_Real V,
                                  Pnt&                P,
                                  Vec&                D1U,
                                  Vec&                D1V,
                                  Vec&                D2U,
                                  Vec&                D2V,
                                  Vec&                D2UV) const
{
  const double aSin = std::sin(U);
  const double aCos = std::cos(U);
  Vec          aD1V, aD2V;
  basisCurve->D2(V, P, aD1V, aD2V);

  const XYZ& aLoc  = loc.XYZ();
  const XYZ& aAxis = direction.XYZ();
  const XYZ& aPnt  = P.XYZ();

  // Rotate point
  XYZ aRes;
  rotatePoint(aSin, aCos, aLoc, aAxis, aPnt, aRes);
  P.SetXYZ(aRes);

  // Compute shift and cross for derivatives
  XYZ aShift;
  shiftPoint(aLoc, aAxis, aPnt, aShift);
  XYZ aCross = aAxis.Crossed(aShift);

  // D1U: -sin*shift + cos*cross
  XYZ aD1Ures;
  aD1Ures.SetLinearForm(-aSin, aShift, aCos, aCross);
  D1U.SetXYZ(aD1Ures);

  // D2U: -cos*shift - sin*cross
  XYZ aD2Ures;
  aD2Ures.SetLinearForm(-aCos, aShift, -aSin, aCross);
  D2U.SetXYZ(aD2Ures);

  // D1V: rotate the curve derivative
  XYZ aD1Vres;
  rotatePoint(aSin, aCos, XYZ(0, 0, 0), aAxis, aD1V.XYZ(), aD1Vres);
  D1V.SetXYZ(aD1Vres);

  // D2V: rotate the second curve derivative
  XYZ aD2Vres;
  rotatePoint(aSin, aCos, XYZ(0, 0, 0), aAxis, aD2V.XYZ(), aD2Vres);
  D2V.SetXYZ(aD2Vres);

  // D2UV: derivative of D1V with respect to U
  XYZ aD1VShift;
  shiftPoint(XYZ(0, 0, 0), aAxis, aD1V.XYZ(), aD1VShift);
  XYZ aD1VCross = aAxis.Crossed(aD1VShift);
  XYZ aD2UVres;
  aD2UVres.SetLinearForm(-aSin, aD1VShift, aCos, aD1VCross);
  D2UV.SetXYZ(aD2UVres);
}

//=================================================================================================

void Geom_SurfaceOfRevolution::D3(const Standard_Real U,
                                  const Standard_Real V,
                                  Pnt&                P,
                                  Vec&                D1U,
                                  Vec&                D1V,
                                  Vec&                D2U,
                                  Vec&                D2V,
                                  Vec&                D2UV,
                                  Vec&                D3U,
                                  Vec&                D3V,
                                  Vec&                D3UUV,
                                  Vec&                D3UVV) const
{
  const double aSin = std::sin(U);
  const double aCos = std::cos(U);
  Vec          aD1V, aD2V, aD3V;
  basisCurve->D3(V, P, aD1V, aD2V, aD3V);

  const XYZ& aLoc  = loc.XYZ();
  const XYZ& aAxis = direction.XYZ();
  const XYZ& aPnt  = P.XYZ();

  // Rotate point
  XYZ aRes;
  rotatePoint(aSin, aCos, aLoc, aAxis, aPnt, aRes);
  P.SetXYZ(aRes);

  // Compute shift and cross for point derivatives
  XYZ aShift;
  shiftPoint(aLoc, aAxis, aPnt, aShift);
  XYZ aCross = aAxis.Crossed(aShift);

  // D1U: -sin*shift + cos*cross
  XYZ aD1Ures;
  aD1Ures.SetLinearForm(-aSin, aShift, aCos, aCross);
  D1U.SetXYZ(aD1Ures);

  // D2U: -cos*shift - sin*cross
  XYZ aD2Ures;
  aD2Ures.SetLinearForm(-aCos, aShift, -aSin, aCross);
  D2U.SetXYZ(aD2Ures);

  // D3U: sin*shift - cos*cross
  XYZ aD3Ures;
  aD3Ures.SetLinearForm(aSin, aShift, -aCos, aCross);
  D3U.SetXYZ(aD3Ures);

  // D1V: rotate the curve derivative
  XYZ aD1Vres;
  rotatePoint(aSin, aCos, XYZ(0, 0, 0), aAxis, aD1V.XYZ(), aD1Vres);
  D1V.SetXYZ(aD1Vres);

  // D2V: rotate the second curve derivative
  XYZ aD2Vres;
  rotatePoint(aSin, aCos, XYZ(0, 0, 0), aAxis, aD2V.XYZ(), aD2Vres);
  D2V.SetXYZ(aD2Vres);

  // D3V: rotate the third curve derivative
  XYZ aD3Vres;
  rotatePoint(aSin, aCos, XYZ(0, 0, 0), aAxis, aD3V.XYZ(), aD3Vres);
  D3V.SetXYZ(aD3Vres);

  // D2UV: derivative of D1V with respect to U
  XYZ aD1VShift;
  shiftPoint(XYZ(0, 0, 0), aAxis, aD1V.XYZ(), aD1VShift);
  XYZ aD1VCross = aAxis.Crossed(aD1VShift);
  XYZ aD2UVres;
  aD2UVres.SetLinearForm(-aSin, aD1VShift, aCos, aD1VCross);
  D2UV.SetXYZ(aD2UVres);

  // D3UUV: second derivative of D1V with respect to U
  XYZ aD3UUVres;
  aD3UUVres.SetLinearForm(-aCos, aD1VShift, -aSin, aD1VCross);
  D3UUV.SetXYZ(aD3UUVres);

  // D3UVV: derivative of D2V with respect to U
  XYZ aD2VShift;
  shiftPoint(XYZ(0, 0, 0), aAxis, aD2V.XYZ(), aD2VShift);
  XYZ aD2VCross = aAxis.Crossed(aD2VShift);
  XYZ aD3UVVres;
  aD3UVVres.SetLinearForm(-aSin, aD2VShift, aCos, aD2VCross);
  D3UVV.SetXYZ(aD3UVVres);
}

//=================================================================================================

Vec Geom_SurfaceOfRevolution::DN(const Standard_Real    U,
                                 const Standard_Real    V,
                                 const Standard_Integer Nu,
                                 const Standard_Integer Nv) const
{
  Standard_RangeError_Raise_if(Nu + Nv < 1 || Nu < 0 || Nv < 0, " ");

  const double aSin = std::sin(U);
  const double aCos = std::cos(U);
  const XYZ&   aAxis = direction.XYZ();

  // Get curve derivative of order Nv
  Vec aDNv = basisCurve->DN(V, Nv);

  // For pure V derivatives (Nu == 0), just rotate the curve derivative
  if (Nu == 0)
  {
    XYZ aRes;
    rotatePoint(aSin, aCos, XYZ(0, 0, 0), aAxis, aDNv.XYZ(), aRes);
    return Vec(aRes);
  }

  // For mixed derivatives, we need to differentiate the rotation Nu times
  // Shift and cross for V derivative
  XYZ aShift;
  shiftPoint(XYZ(0, 0, 0), aAxis, aDNv.XYZ(), aShift);
  XYZ aCross = aAxis.Crossed(aShift);

  // The pattern of derivatives of rotation with respect to U:
  // d^0/dU^0 = (cos-1)*shift + sin*cross
  // d^1/dU^1 = -sin*shift + cos*cross
  // d^2/dU^2 = -cos*shift - sin*cross
  // d^3/dU^3 = sin*shift - cos*cross
  // d^4/dU^4 = cos*shift + sin*cross = d^0 (cyclic with period 4)
  const int aPhase = Nu % 4;
  XYZ       aRes;
  switch (aPhase)
  {
    case 0:
      aRes.SetLinearForm(aCos - 1.0, aShift, aSin, aCross);
      break;
    case 1:
      aRes.SetLinearForm(-aSin, aShift, aCos, aCross);
      break;
    case 2:
      aRes.SetLinearForm(-aCos, aShift, -aSin, aCross);
      break;
    case 3:
      aRes.SetLinearForm(aSin, aShift, -aCos, aCross);
      break;
  }
  return Vec(aRes);
}

//=================================================================================================

Ax2 Geom_SurfaceOfRevolution::ReferencePlane() const
{

  throw Standard_NotImplemented();
}

//=================================================================================================

Handle(Geom_Curve) Geom_SurfaceOfRevolution::UIso(const Standard_Real U) const
{

  Handle(Geom_Curve) C       = Handle(Geom_Curve)::DownCast(basisCurve->Copy());
  Ax1                RotAxis = Ax1(loc, direction);
  C->Rotate(RotAxis, U);
  return C;
}

//=================================================================================================

Handle(Geom_Curve) Geom_SurfaceOfRevolution::VIso(const Standard_Real V) const
{

  Handle(Geom_Circle) Circ;
  Pnt                 Pc = basisCurve->Value(V);
  gp_Lin              L1(loc, direction);
  Standard_Real       Rad = L1.Distance(Pc);

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

void Geom_SurfaceOfRevolution::TransformParameters(Standard_Real&,
                                                   Standard_Real& V,
                                                   const gp_Trsf& T) const
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

void Geom_SurfaceOfRevolution::DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_SweptSurface)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &loc)
}
