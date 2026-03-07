// Copyright (c) 1994-2026 OPEN CASCADE SAS
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

#include <BRepLProp_CLProps.hxx>

#include <GeomProp.hxx>
#include <LProp_CLPropsCompat.pxx>
#include <LProp_NotDefined.hxx>
#include <LProp_Status.hxx>
#include <Standard_OutOfRange.hxx>

namespace
{
static int curveContinuity(const BRepAdaptor_Curve& theCurve)
{
  const GeomAbs_Shape aShape = theCurve.Continuity();
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

BRepLProp_CLProps::BRepLProp_CLProps(const BRepAdaptor_Curve& C,
                                     const double             U,
                                     const int                N,
                                     const double             Resolution)
    : myCurve(C),
      myDerOrder(N),
      myCN(curveContinuity(C)),
      myLinTol(Resolution),
      myCurvature(0.0),
      myTangentStatus(LProp_Undecided),
      mySignificantFirstDerivativeOrder(0)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 3, "BRepLProp_CLProps::BRepLProp_CLProps()");
  SetParameter(U);
}

//==================================================================================================

BRepLProp_CLProps::BRepLProp_CLProps(const BRepAdaptor_Curve& C,
                                     const int                N,
                                     const double             Resolution)
    : myCurve(C),
      myU(RealLast()),
      myDerOrder(N),
      myCN(curveContinuity(C)),
      myLinTol(Resolution),
      myCurvature(0.0),
      myTangentStatus(LProp_Undecided),
      mySignificantFirstDerivativeOrder(0)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 3, "BRepLProp_CLProps::BRepLProp_CLProps()");
}

//==================================================================================================

BRepLProp_CLProps::BRepLProp_CLProps(const int N, const double Resolution)
    : myU(RealLast()),
      myDerOrder(N),
      myCN(0),
      myLinTol(Resolution),
      myCurvature(0.0),
      myTangentStatus(LProp_Undecided),
      mySignificantFirstDerivativeOrder(0)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 3, "BRepLProp_CLProps::BRepLProp_CLProps()");
}

//==================================================================================================

void BRepLProp_CLProps::SetParameter(const double U)
{
  myU = U;
  switch (myDerOrder)
  {
    case 0:
      myPnt = myCurve.Value(myU);
      break;
    case 1:
      myCurve.D1(myU, myPnt, myDerivArr[0]);
      break;
    case 2:
      myCurve.D2(myU, myPnt, myDerivArr[0], myDerivArr[1]);
      break;
    case 3:
      myCurve.D3(myU, myPnt, myDerivArr[0], myDerivArr[1], myDerivArr[2]);
      break;
  }

  myCurvature                       = 0.0;
  myTangentStatus                   = LProp_Undecided;
  mySignificantFirstDerivativeOrder = 0;
}

//==================================================================================================

void BRepLProp_CLProps::SetCurve(const BRepAdaptor_Curve& C)
{
  myCurve                           = C;
  myCN                              = curveContinuity(C);
  myCurvature                       = 0.0;
  myTangentStatus                   = LProp_Undecided;
  mySignificantFirstDerivativeOrder = 0;
}

//==================================================================================================

const gp_Pnt& BRepLProp_CLProps::Value() const
{
  return myPnt;
}

//==================================================================================================

const gp_Vec& BRepLProp_CLProps::D1()
{
  if (myDerOrder < 1)
  {
    myDerOrder = 1;
    myCurve.D1(myU, myPnt, myDerivArr[0]);
  }

  return myDerivArr[0];
}

//==================================================================================================

const gp_Vec& BRepLProp_CLProps::D2()
{
  if (myDerOrder < 2)
  {
    myDerOrder = 2;
    myCurve.D2(myU, myPnt, myDerivArr[0], myDerivArr[1]);
  }

  return myDerivArr[1];
}

//==================================================================================================

const gp_Vec& BRepLProp_CLProps::D3()
{
  if (myDerOrder < 3)
  {
    myDerOrder = 3;
    myCurve.D3(myU, myPnt, myDerivArr[0], myDerivArr[1], myDerivArr[2]);
  }

  return myDerivArr[2];
}

//==================================================================================================

bool BRepLProp_CLProps::IsTangentDefined()
{
  return LProp_CLPropsCompat::IsTangentDefined(myCN,
                                               myLinTol,
                                               D1(),
                                               D2(),
                                               D3(),
                                               mySignificantFirstDerivativeOrder,
                                               myTangentStatus);
}

//==================================================================================================

void BRepLProp_CLProps::Tangent(gp_Dir& D)
{
  LProp_NotDefined_Raise_if(!IsTangentDefined(), "BRepLProp_CLProps::Tangent()");
  LProp_CLPropsCompat::Tangent<GeomProp::TangentResult, gp_Dir, gp_Pnt>(
    mySignificantFirstDerivativeOrder,
    myLinTol,
    myU,
    myCurve.FirstParameter(),
    myCurve.LastParameter(),
    [&]() { return GeomProp::ComputeTangent(D1(), D2(), D3(), myLinTol); },
    [&](const gp_Pnt& thePntBefore, const gp_Pnt& thePntAfter) {
      return GeomProp::ComputeTangent(D1(), D2(), D3(), myLinTol, thePntBefore, thePntAfter);
    },
    [&](const double theParam) { return myCurve.Value(theParam); },
    D,
    "BRepLProp_CLProps::Tangent()");
}

//==================================================================================================

double BRepLProp_CLProps::Curvature()
{
  const bool isDefined = IsTangentDefined();
  (void)isDefined;
  LProp_NotDefined_Raise_if(!isDefined, "BRepLProp_CLProps::Curvature()");

  myCurvature = LProp_CLPropsCompat::Curvature<GeomProp::CurvatureResult>(
    mySignificantFirstDerivativeOrder,
    myLinTol,
    [&]() { return GeomProp::ComputeCurvature(D1(), D2(), myLinTol); },
    "BRepLProp_CLProps::Curvature()");
  return myCurvature;
}

//==================================================================================================

void BRepLProp_CLProps::Normal(gp_Dir& N)
{
  LProp_CLPropsCompat::Normal<GeomProp::NormalResult>(
    Curvature(),
    myLinTol,
    "BRepLProp_CLProps::Normal(): Curvature is null or infinity",
    [&]() { return GeomProp::ComputeNormal(D1(), D2(), myLinTol); },
    N,
    "BRepLProp_CLProps::Normal()");
}

//==================================================================================================

void BRepLProp_CLProps::CentreOfCurvature(gp_Pnt& P)
{
  LProp_CLPropsCompat::CentreOfCurvature<GeomProp::CentreResult>(
    Curvature(),
    myLinTol,
    [&]() { return GeomProp::ComputeCentreOfCurvature(myPnt, D1(), D2(), myLinTol); },
    P,
    "BRepLProp_CLProps::CentreOfCurvature()");
}
