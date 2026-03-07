// Copyright (c) 1992-2026 OPEN CASCADE SAS
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

#include <HLRBRep_CLProps.hxx>

#include <Geom2dProp.hxx>
#include <HLRBRep_CLPropsATool.hxx>
#include <LProp_CLPropsCompat.pxx>
#include <LProp_NotDefined.hxx>
#include <LProp_Status.hxx>
#include <Standard_OutOfRange.hxx>

//==================================================================================================

HLRBRep_CLProps::HLRBRep_CLProps(const HLRBRep_Curve*& C,
                                 const double          U,
                                 const int             N,
                                 const double          Resolution)
    : myCurve(C),
      myDerOrder(N),
      myCN(HLRBRep_CLPropsATool::Continuity(C)),
      myLinTol(Resolution),
      myCurvature(0.0),
      myTangentStatus(LProp_Undecided),
      mySignificantFirstDerivativeOrder(0)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 3, "HLRBRep_CLProps::HLRBRep_CLProps()");
  SetParameter(U);
}

//==================================================================================================

HLRBRep_CLProps::HLRBRep_CLProps(const HLRBRep_Curve*& C,
                                 const int             N,
                                 const double          Resolution)
    : myCurve(C),
      myU(RealLast()),
      myDerOrder(N),
      myCN(HLRBRep_CLPropsATool::Continuity(C)),
      myLinTol(Resolution),
      myCurvature(0.0),
      myTangentStatus(LProp_Undecided),
      mySignificantFirstDerivativeOrder(0)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 3, "HLRBRep_CLProps::HLRBRep_CLProps()");
}

//==================================================================================================

HLRBRep_CLProps::HLRBRep_CLProps(const int N, const double Resolution)
    : myCurve(nullptr),
      myU(RealLast()),
      myDerOrder(N),
      myCN(0),
      myLinTol(Resolution),
      myCurvature(0.0),
      myTangentStatus(LProp_Undecided),
      mySignificantFirstDerivativeOrder(0)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 3, "HLRBRep_CLProps::HLRBRep_CLProps()");
}

//==================================================================================================

void HLRBRep_CLProps::SetParameter(const double U)
{
  myU = U;
  switch (myDerOrder)
  {
    case 0:
      HLRBRep_CLPropsATool::Value(myCurve, myU, myPnt);
      break;
    case 1:
      HLRBRep_CLPropsATool::D1(myCurve, myU, myPnt, myDerivArr[0]);
      break;
    case 2:
      HLRBRep_CLPropsATool::D2(myCurve, myU, myPnt, myDerivArr[0], myDerivArr[1]);
      break;
    case 3:
      HLRBRep_CLPropsATool::D3(myCurve, myU, myPnt, myDerivArr[0], myDerivArr[1], myDerivArr[2]);
      break;
  }

  myCurvature                       = 0.0;
  myTangentStatus                   = LProp_Undecided;
  mySignificantFirstDerivativeOrder = 0;
}

//==================================================================================================

void HLRBRep_CLProps::SetCurve(const HLRBRep_Curve*& C)
{
  myCurve                           = C;
  myCN                              = HLRBRep_CLPropsATool::Continuity(C);
  myCurvature                       = 0.0;
  myTangentStatus                   = LProp_Undecided;
  mySignificantFirstDerivativeOrder = 0;
}

//==================================================================================================

const gp_Pnt2d& HLRBRep_CLProps::Value() const
{
  return myPnt;
}

//==================================================================================================

const gp_Vec2d& HLRBRep_CLProps::D1()
{
  if (myDerOrder < 1)
  {
    myDerOrder = 1;
    HLRBRep_CLPropsATool::D1(myCurve, myU, myPnt, myDerivArr[0]);
  }

  return myDerivArr[0];
}

//==================================================================================================

const gp_Vec2d& HLRBRep_CLProps::D2()
{
  if (myDerOrder < 2)
  {
    myDerOrder = 2;
    HLRBRep_CLPropsATool::D2(myCurve, myU, myPnt, myDerivArr[0], myDerivArr[1]);
  }

  return myDerivArr[1];
}

//==================================================================================================

const gp_Vec2d& HLRBRep_CLProps::D3()
{
  if (myDerOrder < 3)
  {
    myDerOrder = 3;
    HLRBRep_CLPropsATool::D3(myCurve, myU, myPnt, myDerivArr[0], myDerivArr[1], myDerivArr[2]);
  }

  return myDerivArr[2];
}

//==================================================================================================

bool HLRBRep_CLProps::IsTangentDefined()
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

void HLRBRep_CLProps::Tangent(gp_Dir2d& D)
{
  LProp_NotDefined_Raise_if(!IsTangentDefined(), "HLRBRep_CLProps::Tangent()");
  LProp_CLPropsCompat::Tangent<Geom2dProp::TangentResult, gp_Dir2d, gp_Pnt2d>(
    mySignificantFirstDerivativeOrder,
    myLinTol,
    myU,
    HLRBRep_CLPropsATool::FirstParameter(myCurve),
    HLRBRep_CLPropsATool::LastParameter(myCurve),
    [&]() { return Geom2dProp::ComputeTangent(D1(), D2(), D3(), myLinTol); },
    [&](const gp_Pnt2d& thePntBefore, const gp_Pnt2d& thePntAfter) {
      return Geom2dProp::ComputeTangent(D1(), D2(), D3(), myLinTol, thePntBefore, thePntAfter);
    },
    [&](const double theParam) { return myCurve->Value(theParam); },
    D,
    "HLRBRep_CLProps::Tangent()");
}

//==================================================================================================

double HLRBRep_CLProps::Curvature()
{
  const bool isDefined = IsTangentDefined();
  (void)isDefined;
  LProp_NotDefined_Raise_if(!isDefined, "HLRBRep_CLProps::Curvature()");

  myCurvature = LProp_CLPropsCompat::Curvature<Geom2dProp::CurvatureResult>(
    mySignificantFirstDerivativeOrder,
    myLinTol,
    [&]() { return Geom2dProp::ComputeCurvature(D1(), D2(), myLinTol); },
    "HLRBRep_CLProps::Curvature()");
  return myCurvature;
}

//==================================================================================================

void HLRBRep_CLProps::Normal(gp_Dir2d& N)
{
  LProp_CLPropsCompat::Normal<Geom2dProp::NormalResult>(
    Curvature(),
    myLinTol,
    "HLRBRep_CLProps::Normal(): Curvature is null or infinity",
    [&]() { return Geom2dProp::ComputeNormal(D1(), D2(), myLinTol); },
    N,
    "HLRBRep_CLProps::Normal()");
}

//==================================================================================================

void HLRBRep_CLProps::CentreOfCurvature(gp_Pnt2d& P)
{
  LProp_CLPropsCompat::CentreOfCurvature<Geom2dProp::CentreResult>(
    Curvature(),
    myLinTol,
    [&]() { return Geom2dProp::ComputeCentreOfCurvature(myPnt, D1(), D2(), myLinTol); },
    P,
    "HLRBRep_CLProps::CentreOfCurvature()");
}
