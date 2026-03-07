// Copyright (c) 1994-2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <BRepLProp_SLProps.hxx>

#include <GeomProp.hxx>
#include <LProp_SLPropsCompat.pxx>
#include <LProp_NotDefined.hxx>
#include <LProp_Status.hxx>
#include <Standard_OutOfRange.hxx>

namespace
{
static int surfaceContinuity(const BRepAdaptor_Surface& theSurface)
{
  const GeomAbs_Shape aShape = std::min(theSurface.UContinuity(), theSurface.VContinuity());
  switch (aShape)
  {
    case GeomAbs_C0:
      return 0;
    case GeomAbs_C1:
      return 1;
    case GeomAbs_C2:
      return 2;
    case GeomAbs_C3:
      return 3;
    case GeomAbs_CN:
      return 3;
    default:
      return 0;
  }
}
} // namespace

//==================================================================================================

BRepLProp_SLProps::BRepLProp_SLProps(const BRepAdaptor_Surface& S,
                                     const double               U,
                                     const double               V,
                                     const int                  N,
                                     const double               Resolution)
    : mySurf(S),
      myDerOrder(N),
      myCN(surfaceContinuity(S)),
      myLinTol(Resolution),
      myMinCurv(0.0),
      myMaxCurv(0.0),
      myMeanCurv(0.0),
      myGausCurv(0.0),
      mySignificantFirstDerivativeOrderU(0),
      mySignificantFirstDerivativeOrderV(0),
      myUTangentStatus(LProp_Undecided),
      myVTangentStatus(LProp_Undecided),
      myNormalStatus(LProp_Undecided),
      myCurvatureStatus(LProp_Undecided)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 2, "BRepLProp_SLProps::BRepLProp_SLProps()");
  SetParameters(U, V);
}

//==================================================================================================

BRepLProp_SLProps::BRepLProp_SLProps(const BRepAdaptor_Surface& S,
                                     const int                  N,
                                     const double               Resolution)
    : mySurf(S),
      myU(RealLast()),
      myV(RealLast()),
      myDerOrder(N),
      myCN(surfaceContinuity(S)),
      myLinTol(Resolution),
      myMinCurv(0.0),
      myMaxCurv(0.0),
      myMeanCurv(0.0),
      myGausCurv(0.0),
      mySignificantFirstDerivativeOrderU(0),
      mySignificantFirstDerivativeOrderV(0),
      myUTangentStatus(LProp_Undecided),
      myVTangentStatus(LProp_Undecided),
      myNormalStatus(LProp_Undecided),
      myCurvatureStatus(LProp_Undecided)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 2, "BRepLProp_SLProps::BRepLProp_SLProps()");
}

//==================================================================================================

BRepLProp_SLProps::BRepLProp_SLProps(const int N, const double Resolution)
    : myU(RealLast()),
      myV(RealLast()),
      myDerOrder(N),
      myCN(0),
      myLinTol(Resolution),
      myMinCurv(0.0),
      myMaxCurv(0.0),
      myMeanCurv(0.0),
      myGausCurv(0.0),
      mySignificantFirstDerivativeOrderU(0),
      mySignificantFirstDerivativeOrderV(0),
      myUTangentStatus(LProp_Undecided),
      myVTangentStatus(LProp_Undecided),
      myNormalStatus(LProp_Undecided),
      myCurvatureStatus(LProp_Undecided)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 2, "BRepLProp_SLProps::BRepLProp_SLProps()");
}

//==================================================================================================

void BRepLProp_SLProps::SetSurface(const BRepAdaptor_Surface& S)
{
  mySurf = S;
  myCN   = surfaceContinuity(S);
  mySignificantFirstDerivativeOrderU = 0;
  mySignificantFirstDerivativeOrderV = 0;
  myUTangentStatus                   = LProp_Undecided;
  myVTangentStatus                   = LProp_Undecided;
  myNormalStatus                     = LProp_Undecided;
  myCurvatureStatus                  = LProp_Undecided;
}

//==================================================================================================

void BRepLProp_SLProps::SetParameters(const double U, const double V)
{
  myU = U;
  myV = V;
  switch (myDerOrder)
  {
    case 0:
      myPnt = mySurf.Value(myU, myV);
      break;
    case 1:
      mySurf.D1(myU, myV, myPnt, myD1u, myD1v);
      break;
    case 2:
      mySurf.D2(myU, myV, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv);
      break;
  }

  myMinCurv                          = 0.0;
  myMaxCurv                          = 0.0;
  myMeanCurv                         = 0.0;
  myGausCurv                         = 0.0;
  mySignificantFirstDerivativeOrderU = 0;
  mySignificantFirstDerivativeOrderV = 0;
  myUTangentStatus                   = LProp_Undecided;
  myVTangentStatus                   = LProp_Undecided;
  myNormalStatus                     = LProp_Undecided;
  myCurvatureStatus                  = LProp_Undecided;
}

//==================================================================================================

const gp_Pnt& BRepLProp_SLProps::Value() const
{
  return myPnt;
}

//==================================================================================================

const gp_Vec& BRepLProp_SLProps::D1U()
{
  if (myDerOrder < 1)
  {
    myDerOrder = 1;
    mySurf.D1(myU, myV, myPnt, myD1u, myD1v);
  }

  return myD1u;
}

//==================================================================================================

const gp_Vec& BRepLProp_SLProps::D1V()
{
  if (myDerOrder < 1)
  {
    myDerOrder = 1;
    mySurf.D1(myU, myV, myPnt, myD1u, myD1v);
  }

  return myD1v;
}

//==================================================================================================

const gp_Vec& BRepLProp_SLProps::D2U()
{
  if (myDerOrder < 2)
  {
    myDerOrder = 2;
    mySurf.D2(myU, myV, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv);
  }

  return myD2u;
}

//==================================================================================================

const gp_Vec& BRepLProp_SLProps::D2V()
{
  if (myDerOrder < 2)
  {
    myDerOrder = 2;
    mySurf.D2(myU, myV, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv);
  }

  return myD2v;
}

//==================================================================================================

const gp_Vec& BRepLProp_SLProps::DUV()
{
  if (myDerOrder < 2)
  {
    myDerOrder = 2;
    mySurf.D2(myU, myV, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv);
  }

  return myDuv;
}

//==================================================================================================

bool BRepLProp_SLProps::IsTangentUDefined()
{
  return LProp_SLPropsCompat::IsTangentDefined(myCN,
                                               myLinTol,
                                               D1U(),
                                               D1V(),
                                               D2U(),
                                               D2V(),
                                               0,
                                               mySignificantFirstDerivativeOrderU,
                                               myUTangentStatus);
}

//==================================================================================================

void BRepLProp_SLProps::TangentU(gp_Dir& D)
{
  LProp_NotDefined_Raise_if(!IsTangentUDefined(), "BRepLProp_SLProps::TangentU()");
  LProp_SLPropsCompat::Tangent(
    mySignificantFirstDerivativeOrderU,
    myLinTol,
    myU,
    mySurf.FirstUParameter(),
    mySurf.LastUParameter(),
    [&]() { return gp_Dir(D1U()); },
    [&](const gp_Pnt& thePntBefore, const gp_Pnt& thePntAfter) {
      return GeomProp::ComputeTangent(D1U(), D2U(), gp_Vec(0.0, 0.0, 0.0), myLinTol, thePntBefore, thePntAfter);
    },
    [&](const double theParam) { return mySurf.Value(theParam, myV); },
    D,
    "BRepLProp_SLProps::TangentU()");
}

//==================================================================================================

bool BRepLProp_SLProps::IsTangentVDefined()
{
  return LProp_SLPropsCompat::IsTangentDefined(myCN,
                                               myLinTol,
                                               D1U(),
                                               D1V(),
                                               D2U(),
                                               D2V(),
                                               1,
                                               mySignificantFirstDerivativeOrderV,
                                               myVTangentStatus);
}

//==================================================================================================

void BRepLProp_SLProps::TangentV(gp_Dir& D)
{
  LProp_NotDefined_Raise_if(!IsTangentVDefined(), "BRepLProp_SLProps::TangentV()");
  LProp_SLPropsCompat::Tangent(
    mySignificantFirstDerivativeOrderV,
    myLinTol,
    myV,
    mySurf.FirstVParameter(),
    mySurf.LastVParameter(),
    [&]() { return gp_Dir(D1V()); },
    [&](const gp_Pnt& thePntBefore, const gp_Pnt& thePntAfter) {
      return GeomProp::ComputeTangent(D1V(), D2V(), gp_Vec(0.0, 0.0, 0.0), myLinTol, thePntBefore, thePntAfter);
    },
    [&](const double theParam) { return mySurf.Value(myU, theParam); },
    D,
    "BRepLProp_SLProps::TangentV()");
}

//==================================================================================================

bool BRepLProp_SLProps::IsNormalDefined()
{
  return LProp_SLPropsCompat::IsNormalDefined<GeomProp::SurfaceNormalResult>(
    [&]() { return GeomProp::ComputeSurfaceNormal(D1U(), D1V(), myLinTol); },
    myNormal,
    myNormalStatus);
}

//==================================================================================================

const gp_Dir& BRepLProp_SLProps::Normal()
{
  if (!IsNormalDefined())
  {
    throw LProp_NotDefined();
  }

  return myNormal;
}

//==================================================================================================

bool BRepLProp_SLProps::IsCurvatureDefined()
{
  return LProp_SLPropsCompat::IsCurvatureDefined<GeomProp::SurfaceCurvatureResult,
                                                 GeomProp::MeanGaussianResult>(
    myCN,
    IsNormalDefined(),
    IsTangentUDefined(),
    IsTangentVDefined(),
    [&]() { return GeomProp::ComputeSurfaceCurvatures(D1U(), D1V(), D2U(), D2V(), DUV(), myLinTol); },
    [&]() { return GeomProp::ComputeMeanGaussian(D1U(), D1V(), D2U(), D2V(), DUV(), myLinTol); },
    myMinCurv,
    myMaxCurv,
    myDirMinCurv,
    myDirMaxCurv,
    myMeanCurv,
    myGausCurv,
    myCurvatureStatus);
}

//==================================================================================================

bool BRepLProp_SLProps::IsUmbilic()
{
  if (!IsCurvatureDefined())
  {
    throw LProp_NotDefined();
  }

  return std::abs(myMaxCurv - myMinCurv) < std::abs(Epsilon(myMaxCurv));
}

//==================================================================================================

double BRepLProp_SLProps::MaxCurvature()
{
  if (!IsCurvatureDefined())
  {
    throw LProp_NotDefined();
  }

  return myMaxCurv;
}

//==================================================================================================

double BRepLProp_SLProps::MinCurvature()
{
  if (!IsCurvatureDefined())
  {
    throw LProp_NotDefined();
  }

  return myMinCurv;
}

//==================================================================================================

void BRepLProp_SLProps::CurvatureDirections(gp_Dir& MaxD, gp_Dir& MinD)
{
  if (!IsCurvatureDefined())
  {
    throw LProp_NotDefined();
  }

  MaxD = myDirMaxCurv;
  MinD = myDirMinCurv;
}

//==================================================================================================

double BRepLProp_SLProps::MeanCurvature()
{
  if (!IsCurvatureDefined())
  {
    throw LProp_NotDefined();
  }

  return myMeanCurv;
}

//==================================================================================================

double BRepLProp_SLProps::GaussianCurvature()
{
  if (!IsCurvatureDefined())
  {
    throw LProp_NotDefined();
  }

  return myGausCurv;
}
