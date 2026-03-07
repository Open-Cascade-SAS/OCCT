// Copyright (c) 2002-2026 OPEN CASCADE SAS
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

#include <LProp3d_CLProps.hxx>

#include <GeomProp.hxx>
#include <GeomProp_Curve.hxx>
#include <LProp_CLPropsCompat.pxx>
#include <LProp_NotDefined.hxx>
#include <LProp_Status.hxx>
#include <Standard_OutOfRange.hxx>

namespace
{
bool hasGeomPropEvaluator(const std::shared_ptr<GeomProp_Curve>& theCurveProp)
{
  return theCurveProp && theCurveProp->Adaptor() != nullptr;
}
} // namespace

//==================================================================================================

LProp3d_CLProps::LProp3d_CLProps(const occ::handle<Adaptor3d_Curve>& C,
                                 const double                        U,
                                 const int                           N,
                                 const double                        Resolution)
    : myCurve(C),
      myCurveProp(C.IsNull() ? nullptr : std::make_shared<GeomProp_Curve>(*C)),
      myDerOrder(N),
      myCN(4),
      myLinTol(Resolution),
      myCurvature(0.0),
      myTangentStatus(LProp_Undecided),
      mySignificantFirstDerivativeOrder(0)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 3, "LProp3d_CLProps::LProp3d_CLProps()");
  SetParameter(U);
}

//==================================================================================================

LProp3d_CLProps::LProp3d_CLProps(const occ::handle<Adaptor3d_Curve>& C,
                                 const int                           N,
                                 const double                        Resolution)
    : myCurve(C),
      myCurveProp(C.IsNull() ? nullptr : std::make_shared<GeomProp_Curve>(*C)),
      myU(RealLast()),
      myDerOrder(N),
      myCN(4),
      myLinTol(Resolution),
      myCurvature(0.0),
      myTangentStatus(LProp_Undecided),
      mySignificantFirstDerivativeOrder(0)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 3, "LProp3d_CLProps::LProp3d_CLProps()");
}

//==================================================================================================

LProp3d_CLProps::LProp3d_CLProps(const int N, const double Resolution)
    : myU(RealLast()),
      myDerOrder(N),
      myCN(0),
      myLinTol(Resolution),
      myCurvature(0.0),
      myTangentStatus(LProp_Undecided),
      mySignificantFirstDerivativeOrder(0)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 3, "LProp3d_CLProps::LProp3d_CLProps()");
}

//==================================================================================================

LProp3d_CLProps::~LProp3d_CLProps() = default;

//==================================================================================================

void LProp3d_CLProps::SetParameter(const double U)
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

void LProp3d_CLProps::SetCurve(const occ::handle<Adaptor3d_Curve>& C)
{
  myCurve                           = C;
  myCurveProp                       = C.IsNull() ? nullptr : std::make_shared<GeomProp_Curve>(*C);
  myCN                              = 4;
  myCurvature                       = 0.0;
  myTangentStatus                   = LProp_Undecided;
  mySignificantFirstDerivativeOrder = 0;
}

//==================================================================================================

const gp_Pnt& LProp3d_CLProps::Value() const
{
  return myPnt;
}

//==================================================================================================

const gp_Vec& LProp3d_CLProps::D1()
{
  if (myDerOrder < 1)
  {
    myDerOrder = 1;
    myCurve->D1(myU, myPnt, myDerivArr[0]);
  }

  return myDerivArr[0];
}

//==================================================================================================

const gp_Vec& LProp3d_CLProps::D2()
{
  if (myDerOrder < 2)
  {
    myDerOrder = 2;
    myCurve->D2(myU, myPnt, myDerivArr[0], myDerivArr[1]);
  }

  return myDerivArr[1];
}

//==================================================================================================

const gp_Vec& LProp3d_CLProps::D3()
{
  if (myDerOrder < 3)
  {
    myDerOrder = 3;
    myCurve->D3(myU, myPnt, myDerivArr[0], myDerivArr[1], myDerivArr[2]);
  }

  return myDerivArr[2];
}

//==================================================================================================

bool LProp3d_CLProps::IsTangentDefined()
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

void LProp3d_CLProps::Tangent(gp_Dir& D)
{
  LProp_NotDefined_Raise_if(!IsTangentDefined(), "LProp3d_CLProps::Tangent()");
  LProp_CLPropsCompat::Tangent<GeomProp::TangentResult, gp_Dir, gp_Pnt>(
    mySignificantFirstDerivativeOrder,
    myLinTol,
    myU,
    myCurve->FirstParameter(),
    myCurve->LastParameter(),
    [&]() {
      return hasGeomPropEvaluator(myCurveProp)
               ? myCurveProp->Tangent(myU, myLinTol)
               : GeomProp::ComputeTangent(D1(), D2(), D3(), myLinTol);
    },
    [&](const gp_Pnt& thePntBefore, const gp_Pnt& thePntAfter) {
      return GeomProp::ComputeTangent(D1(), D2(), D3(), myLinTol, thePntBefore, thePntAfter);
    },
    [&](const double theParam) {
      gp_Pnt aPoint;
      myCurve->D0(theParam, aPoint);
      return aPoint;
    },
    D,
    "LProp3d_CLProps::Tangent()");
}

//==================================================================================================

double LProp3d_CLProps::Curvature()
{
  const bool isDefined = IsTangentDefined();
  (void)isDefined;
  LProp_NotDefined_Raise_if(!isDefined, "LProp3d_CLProps::Curvature()");

  myCurvature = LProp_CLPropsCompat::Curvature<GeomProp::CurvatureResult>(
    mySignificantFirstDerivativeOrder,
    myLinTol,
    [&]() {
      const GeomProp::CurvatureResult aResult = GeomProp::ComputeCurvature(D1(), D2(), myLinTol);
      return hasGeomPropEvaluator(myCurveProp) ? myCurveProp->Curvature(myU, myLinTol) : aResult;
    },
    "LProp3d_CLProps::Curvature()");
  return myCurvature;
}

//==================================================================================================

void LProp3d_CLProps::Normal(gp_Dir& D)
{
  LProp_CLPropsCompat::Normal<GeomProp::NormalResult>(
    Curvature(),
    myLinTol,
    "LProp3d_CLProps::Normal(): Curvature is null or infinity",
    [&]() {
      const GeomProp::NormalResult aResult = GeomProp::ComputeNormal(D1(), D2(), myLinTol);
      return hasGeomPropEvaluator(myCurveProp) ? myCurveProp->Normal(myU, myLinTol) : aResult;
    },
    D,
    "LProp3d_CLProps::Normal()");
}

//==================================================================================================

void LProp3d_CLProps::CentreOfCurvature(gp_Pnt& P)
{
  LProp_CLPropsCompat::CentreOfCurvature<GeomProp::CentreResult>(
    Curvature(),
    myLinTol,
    [&]() {
      const GeomProp::CentreResult aResult =
        GeomProp::ComputeCentreOfCurvature(myPnt, D1(), D2(), myLinTol);
      return hasGeomPropEvaluator(myCurveProp) ? myCurveProp->CentreOfCurvature(myU, myLinTol)
                                               : aResult;
    },
    P,
    "LProp3d_CLProps::CentreOfCurvature()");
}
