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

#include <Geom2dLProp_CLProps2d.hxx>

#include <Geom2dProp.hxx>
#include <Geom2dProp_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <LProp_NotDefined.hxx>
#include <LProp_Status.hxx>
#include <Standard_OutOfRange.hxx>

#include <algorithm>
#include <cmath>

namespace
{
constexpr double THE_MIN_STEP = 1.0e-7;
}

//==================================================================================================

Geom2dLProp_CLProps2d::Geom2dLProp_CLProps2d(const occ::handle<Geom2d_Curve>& C,
                                             const double                     U,
                                             const int                        N,
                                             const double                     Resolution)
    : myCurve(C),
      myCurveProp(C.IsNull() ? nullptr : std::make_shared<Geom2dProp_Curve>(C)),
      myDerOrder(N),
      myCN(4),
      myLinTol(Resolution),
      myCurvature(0.0),
      myTangentStatus(LProp_Undecided),
      mySignificantFirstDerivativeOrder(0)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 3, "Geom2dLProp_CLProps2d::Geom2dLProp_CLProps2d()");
  SetParameter(U);
}

//==================================================================================================

Geom2dLProp_CLProps2d::Geom2dLProp_CLProps2d(const occ::handle<Geom2d_Curve>& C,
                                             const int                        N,
                                             const double                     Resolution)
    : myCurve(C),
      myCurveProp(C.IsNull() ? nullptr : std::make_shared<Geom2dProp_Curve>(C)),
      myU(RealLast()),
      myDerOrder(N),
      myCN(4),
      myLinTol(Resolution),
      myCurvature(0.0),
      myTangentStatus(LProp_Undecided),
      mySignificantFirstDerivativeOrder(0)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 3, "Geom2dLProp_CLProps2d::Geom2dLProp_CLProps2d()");
}

//==================================================================================================

Geom2dLProp_CLProps2d::Geom2dLProp_CLProps2d(const int N, const double Resolution)
    : myU(RealLast()),
      myDerOrder(N),
      myCN(0),
      myLinTol(Resolution),
      myCurvature(0.0),
      myTangentStatus(LProp_Undecided),
      mySignificantFirstDerivativeOrder(0)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 3, "Geom2dLProp_CLProps2d::Geom2dLProp_CLProps2d()");
}

//==================================================================================================

void Geom2dLProp_CLProps2d::SetParameter(const double U)
{
  myU = U;
  switch (myDerOrder)
  {
    case 0:
      myPnt = myCurve->Value(myU);
      break;
    case 1:
      myCurve->D1(myU, myPnt, myDerivArr[0]);
      break;
    case 2:
      myCurve->D2(myU, myPnt, myDerivArr[0], myDerivArr[1]);
      break;
    case 3:
      myCurve->D3(myU, myPnt, myDerivArr[0], myDerivArr[1], myDerivArr[2]);
      break;
  }

  myCurvature                       = 0.0;
  myTangentStatus                   = LProp_Undecided;
  mySignificantFirstDerivativeOrder = 0;
}

//==================================================================================================

void Geom2dLProp_CLProps2d::SetCurve(const occ::handle<Geom2d_Curve>& C)
{
  myCurve                           = C;
  myCurveProp                       = C.IsNull() ? nullptr : std::make_shared<Geom2dProp_Curve>(C);
  myCN                              = 4;
  myCurvature                       = 0.0;
  myTangentStatus                   = LProp_Undecided;
  mySignificantFirstDerivativeOrder = 0;
}

//==================================================================================================

const gp_Pnt2d& Geom2dLProp_CLProps2d::Value() const
{
  return myPnt;
}

//==================================================================================================

const gp_Vec2d& Geom2dLProp_CLProps2d::D1()
{
  if (myDerOrder < 1)
  {
    myDerOrder = 1;
    myCurve->D1(myU, myPnt, myDerivArr[0]);
  }

  return myDerivArr[0];
}

//==================================================================================================

const gp_Vec2d& Geom2dLProp_CLProps2d::D2()
{
  if (myDerOrder < 2)
  {
    myDerOrder = 2;
    myCurve->D2(myU, myPnt, myDerivArr[0], myDerivArr[1]);
  }

  return myDerivArr[1];
}

//==================================================================================================

const gp_Vec2d& Geom2dLProp_CLProps2d::D3()
{
  if (myDerOrder < 3)
  {
    myDerOrder = 3;
    myCurve->D3(myU, myPnt, myDerivArr[0], myDerivArr[1], myDerivArr[2]);
  }

  return myDerivArr[2];
}

//==================================================================================================

bool Geom2dLProp_CLProps2d::IsTangentDefined()
{
  if (myTangentStatus == LProp_Undefined)
  {
    return false;
  }
  if (myTangentStatus >= LProp_Defined)
  {
    return true;
  }

  const double aTol2                = myLinTol * myLinTol;
  mySignificantFirstDerivativeOrder = 0;

  if (myCN >= 1 && D1().SquareMagnitude() > aTol2)
  {
    mySignificantFirstDerivativeOrder = 1;
  }
  else if (myCN >= 2 && D2().SquareMagnitude() > aTol2)
  {
    mySignificantFirstDerivativeOrder = 2;
  }
  else if (myCN >= 3 && D3().SquareMagnitude() > aTol2)
  {
    mySignificantFirstDerivativeOrder = 3;
  }

  if (mySignificantFirstDerivativeOrder == 0)
  {
    myTangentStatus = LProp_Undefined;
    return false;
  }

  myTangentStatus = LProp_Defined;
  return true;
}

//==================================================================================================

void Geom2dLProp_CLProps2d::Tangent(gp_Dir2d& D)
{
  LProp_NotDefined_Raise_if(!IsTangentDefined(), "Geom2dLProp_CLProps2d::Tangent()");

  if (mySignificantFirstDerivativeOrder == 1)
  {
    const Geom2dProp::TangentResult aResult =
      myCurveProp ? myCurveProp->Tangent(myU, myLinTol)
                  : Geom2dProp::ComputeTangent(D1(), D2(), D3(), myLinTol);
    LProp_NotDefined_Raise_if(!aResult.IsDefined, "Geom2dLProp_CLProps2d::Tangent()");
    D = aResult.Direction;
    return;
  }

  const double aFirst = myCurve->FirstParameter();
  const double aLast  = myCurve->LastParameter();
  const double aRange = (aLast >= RealLast() || aFirst <= RealFirst()) ? 0.0 : aLast - aFirst;
  const double aDelta = std::max(aRange * 1.0e-3, THE_MIN_STEP);
  const double aOther = (myU - aFirst < aDelta) ? myU + aDelta : myU - aDelta;

  const gp_Pnt2d aPntBefore = myCurve->Value(std::min(myU, aOther));
  const gp_Pnt2d aPntAfter  = myCurve->Value(std::max(myU, aOther));

  const Geom2dProp::TangentResult aResult =
    Geom2dProp::ComputeTangent(D1(), D2(), D3(), myLinTol, aPntBefore, aPntAfter);
  LProp_NotDefined_Raise_if(!aResult.IsDefined, "Geom2dLProp_CLProps2d::Tangent()");
  D = aResult.Direction;
}

//==================================================================================================

double Geom2dLProp_CLProps2d::Curvature()
{
  const bool isDefined = IsTangentDefined();
  (void)isDefined;
  LProp_NotDefined_Raise_if(!isDefined, "Geom2dLProp_CLProps2d::Curvature()");

  if (mySignificantFirstDerivativeOrder > 1)
  {
    myCurvature = RealLast();
    return myCurvature;
  }

  const Geom2dProp::CurvatureResult aResult =
    myCurveProp ? myCurveProp->Curvature(myU, myLinTol)
                : Geom2dProp::ComputeCurvature(D1(), D2(), myLinTol);
  LProp_NotDefined_Raise_if(!aResult.IsDefined, "Geom2dLProp_CLProps2d::Curvature()");

  myCurvature = aResult.IsInfinite ? RealLast() : aResult.Value;
  return myCurvature;
}

//==================================================================================================

void Geom2dLProp_CLProps2d::Normal(gp_Dir2d& D)
{
  const double aCurvature = Curvature();
  if (aCurvature == RealLast() || std::abs(aCurvature) <= myLinTol)
  {
    throw LProp_NotDefined("Geom2dLProp_CLProps2d::Normal(): Curvature is null or infinity");
  }

  const Geom2dProp::NormalResult aResult = myCurveProp
                                             ? myCurveProp->Normal(myU, myLinTol)
                                             : Geom2dProp::ComputeNormal(D1(), D2(), myLinTol);
  LProp_NotDefined_Raise_if(!aResult.IsDefined, "Geom2dLProp_CLProps2d::Normal()");
  D = aResult.Direction;
}

//==================================================================================================

void Geom2dLProp_CLProps2d::CentreOfCurvature(gp_Pnt2d& P)
{
  if (std::abs(Curvature()) <= myLinTol)
  {
    throw LProp_NotDefined();
  }

  const Geom2dProp::CentreResult aResult =
    myCurveProp ? myCurveProp->CentreOfCurvature(myU, myLinTol)
                : Geom2dProp::ComputeCentreOfCurvature(myPnt, D1(), D2(), myLinTol);
  LProp_NotDefined_Raise_if(!aResult.IsDefined, "Geom2dLProp_CLProps2d::CentreOfCurvature()");
  P = aResult.Centre;
}
