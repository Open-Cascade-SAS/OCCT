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

#include <GeomLProp_CLProps.hxx>

#include <GeomProp.hxx>
#include <GeomProp_Curve.hxx>
#include <Geom_Curve.hxx>
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

GeomLProp_CLProps::GeomLProp_CLProps(const occ::handle<Geom_Curve>& C,
                                     const double                   U,
                                     const int                      N,
                                     const double                   Resolution)
    : myCurve(C),
      myCurveProp(C.IsNull() ? nullptr : std::make_shared<GeomProp_Curve>(C)),
      myDerOrder(N),
      myCN(4),
      myLinTol(Resolution),
      myCurvature(0.0),
      myTangentStatus(LProp_Undecided),
      mySignificantFirstDerivativeOrder(0)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 3, "GeomLProp_CLProps::GeomLProp_CLProps()");
  SetParameter(U);
}

//==================================================================================================

GeomLProp_CLProps::GeomLProp_CLProps(const occ::handle<Geom_Curve>& C,
                                     const int                      N,
                                     const double                   Resolution)
    : myCurve(C),
      myCurveProp(C.IsNull() ? nullptr : std::make_shared<GeomProp_Curve>(C)),
      myU(RealLast()),
      myDerOrder(N),
      myCN(4),
      myLinTol(Resolution),
      myCurvature(0.0),
      myTangentStatus(LProp_Undecided),
      mySignificantFirstDerivativeOrder(0)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 3, "GeomLProp_CLProps::GeomLProp_CLProps()");
}

//==================================================================================================

GeomLProp_CLProps::GeomLProp_CLProps(const int N, const double Resolution)
    : myU(RealLast()),
      myDerOrder(N),
      myCN(0),
      myLinTol(Resolution),
      myCurvature(0.0),
      myTangentStatus(LProp_Undecided),
      mySignificantFirstDerivativeOrder(0)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 3, "GeomLProp_CLProps::GeomLProp_CLProps()");
}

//==================================================================================================

GeomLProp_CLProps::~GeomLProp_CLProps() = default;

//==================================================================================================

void GeomLProp_CLProps::SetParameter(const double U)
{
  myU = U;
  switch (myDerOrder)
  {
    case 0:
      myCurve->D0(myU, myPnt);
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

void GeomLProp_CLProps::SetCurve(const occ::handle<Geom_Curve>& C)
{
  myCurve                           = C;
  myCurveProp                       = C.IsNull() ? nullptr : std::make_shared<GeomProp_Curve>(C);
  myCN                              = 4;
  myCurvature                       = 0.0;
  myTangentStatus                   = LProp_Undecided;
  mySignificantFirstDerivativeOrder = 0;
}

//==================================================================================================

const gp_Pnt& GeomLProp_CLProps::Value() const
{
  return myPnt;
}

//==================================================================================================

const gp_Vec& GeomLProp_CLProps::D1()
{
  if (myDerOrder < 1)
  {
    myDerOrder = 1;
    myCurve->D1(myU, myPnt, myDerivArr[0]);
  }

  return myDerivArr[0];
}

//==================================================================================================

const gp_Vec& GeomLProp_CLProps::D2()
{
  if (myDerOrder < 2)
  {
    myDerOrder = 2;
    myCurve->D2(myU, myPnt, myDerivArr[0], myDerivArr[1]);
  }

  return myDerivArr[1];
}

//==================================================================================================

const gp_Vec& GeomLProp_CLProps::D3()
{
  if (myDerOrder < 3)
  {
    myDerOrder = 3;
    myCurve->D3(myU, myPnt, myDerivArr[0], myDerivArr[1], myDerivArr[2]);
  }

  return myDerivArr[2];
}

//==================================================================================================

bool GeomLProp_CLProps::IsTangentDefined()
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

void GeomLProp_CLProps::Tangent(gp_Dir& D)
{
  LProp_NotDefined_Raise_if(!IsTangentDefined(), "GeomLProp_CLProps::Tangent()");

  if (mySignificantFirstDerivativeOrder == 1)
  {
    const GeomProp::TangentResult aResult =
      myCurveProp ? myCurveProp->Tangent(myU, myLinTol)
                  : GeomProp::ComputeTangent(D1(), D2(), D3(), myLinTol);
    LProp_NotDefined_Raise_if(!aResult.IsDefined, "GeomLProp_CLProps::Tangent()");
    D = aResult.Direction;
    return;
  }

  const double aFirst = myCurve->FirstParameter();
  const double aLast  = myCurve->LastParameter();
  const double aRange = (aLast >= RealLast() || aFirst <= RealFirst()) ? 0.0 : aLast - aFirst;
  const double aDelta = std::max(aRange * 1.0e-3, THE_MIN_STEP);
  const double aOther = (myU - aFirst < aDelta) ? myU + aDelta : myU - aDelta;

  gp_Pnt aPntBefore;
  gp_Pnt aPntAfter;
  myCurve->D0(std::min(myU, aOther), aPntBefore);
  myCurve->D0(std::max(myU, aOther), aPntAfter);

  const GeomProp::TangentResult aResult =
    GeomProp::ComputeTangent(D1(), D2(), D3(), myLinTol, aPntBefore, aPntAfter);
  LProp_NotDefined_Raise_if(!aResult.IsDefined, "GeomLProp_CLProps::Tangent()");
  D = aResult.Direction;
}

//==================================================================================================

double GeomLProp_CLProps::Curvature()
{
  const bool isDefined = IsTangentDefined();
  (void)isDefined;
  LProp_NotDefined_Raise_if(!isDefined, "GeomLProp_CLProps::Curvature()");

  if (mySignificantFirstDerivativeOrder > 1)
  {
    myCurvature = RealLast();
    return myCurvature;
  }

  const GeomProp::CurvatureResult aResult = myCurveProp
                                              ? myCurveProp->Curvature(myU, myLinTol)
                                              : GeomProp::ComputeCurvature(D1(), D2(), myLinTol);
  LProp_NotDefined_Raise_if(!aResult.IsDefined, "GeomLProp_CLProps::Curvature()");

  myCurvature = aResult.IsInfinite ? RealLast() : aResult.Value;
  return myCurvature;
}

//==================================================================================================

void GeomLProp_CLProps::Normal(gp_Dir& D)
{
  const double aCurvature = Curvature();
  if (aCurvature == RealLast() || std::abs(aCurvature) <= myLinTol)
  {
    throw LProp_NotDefined("GeomLProp_CLProps::Normal(): Curvature is null or infinity");
  }

  const GeomProp::NormalResult aResult = myCurveProp
                                           ? myCurveProp->Normal(myU, myLinTol)
                                           : GeomProp::ComputeNormal(D1(), D2(), myLinTol);
  LProp_NotDefined_Raise_if(!aResult.IsDefined, "GeomLProp_CLProps::Normal()");
  D = aResult.Direction;
}

//==================================================================================================

void GeomLProp_CLProps::CentreOfCurvature(gp_Pnt& P)
{
  if (std::abs(Curvature()) <= myLinTol)
  {
    throw LProp_NotDefined();
  }

  const GeomProp::CentreResult aResult =
    myCurveProp ? myCurveProp->CentreOfCurvature(myU, myLinTol)
                : GeomProp::ComputeCentreOfCurvature(myPnt, D1(), D2(), myLinTol);
  LProp_NotDefined_Raise_if(!aResult.IsDefined, "GeomLProp_CLProps::CentreOfCurvature()");
  P = aResult.Centre;
}
