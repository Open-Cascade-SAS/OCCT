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

#include <Geom2dLProp_LegacyCLProps2d.hxx>
#include <Geom2dProp.hxx>
#include <Geom2dProp_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <LProp_CLPropsCompat.pxx>
#include <LProp_CompareDebug.pxx>
#include <LProp_NotDefined.hxx>
#include <LProp_Status.hxx>
#include <Precision.hxx>
#include <Standard_OutOfRange.hxx>

namespace
{
std::string curveGeometry(const occ::handle<Geom2d_Curve>& theCurve)
{
  return theCurve.IsNull() ? "null" : std::to_string((int)Geom2dProp_Curve(theCurve).GetType());
}

std::string curveParameters(const double theU)
{
  return "U=" + LProp_CompareDebug::ToString(theU);
}

bool sameCurvatureClass(const double theNewValue, const double theOldValue, const double theTol)
{
  if ((theNewValue == RealLast()) != (theOldValue == RealLast()))
  {
    return false;
  }
  if (theNewValue == RealLast())
  {
    return true;
  }

  const bool isNewZero = std::abs(theNewValue) <= theTol;
  const bool isOldZero = std::abs(theOldValue) <= theTol;
  if (isNewZero != isOldZero)
  {
    return false;
  }

  const int aNewSign = (theNewValue > theTol) - (theNewValue < -theTol);
  const int anOldSign = (theOldValue > theTol) - (theOldValue < -theTol);
  return aNewSign == anOldSign;
}
}

//==================================================================================================

Geom2dLProp_CLProps2d::Geom2dLProp_CLProps2d(const occ::handle<Geom2d_Curve>& C,
                                             const double                     U,
                                             const int                        N,
                                             const double                     Resolution)
    : myCurve(C),
      myLegacyProps(C.IsNull() ? nullptr
                               : std::make_shared<Geom2dLProp_LegacyCLProps2d>(C, N, Resolution)),
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
      myLegacyProps(C.IsNull() ? nullptr
                               : std::make_shared<Geom2dLProp_LegacyCLProps2d>(C, N, Resolution)),
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
    : myLegacyProps(std::make_shared<Geom2dLProp_LegacyCLProps2d>(N, Resolution)),
      myU(RealLast()),
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

Geom2dLProp_CLProps2d::Geom2dLProp_CLProps2d(const Geom2dLProp_CLProps2d& theOther)
    : myCurve(theOther.myCurve),
      myLegacyProps(theOther.myLegacyProps != nullptr
                      ? std::make_shared<Geom2dLProp_LegacyCLProps2d>(*theOther.myLegacyProps)
                      : nullptr),
      myU(theOther.myU),
      myDerOrder(theOther.myDerOrder),
      myCN(theOther.myCN),
      myLinTol(theOther.myLinTol),
      myPnt(theOther.myPnt),
      myTangent(theOther.myTangent),
      myCurvature(theOther.myCurvature),
      myTangentStatus(theOther.myTangentStatus),
      mySignificantFirstDerivativeOrder(theOther.mySignificantFirstDerivativeOrder)
{
  myDerivArr[0] = theOther.myDerivArr[0];
  myDerivArr[1] = theOther.myDerivArr[1];
  myDerivArr[2] = theOther.myDerivArr[2];
}

//==================================================================================================

Geom2dLProp_CLProps2d& Geom2dLProp_CLProps2d::operator=(const Geom2dLProp_CLProps2d& theOther)
{
  if (this == &theOther)
  {
    return *this;
  }

  myCurve    = theOther.myCurve;
  myLegacyProps = theOther.myLegacyProps != nullptr
                    ? std::make_shared<Geom2dLProp_LegacyCLProps2d>(*theOther.myLegacyProps)
                    : nullptr;
  myU                                  = theOther.myU;
  myDerOrder                           = theOther.myDerOrder;
  myCN                                 = theOther.myCN;
  myLinTol                             = theOther.myLinTol;
  myPnt                                = theOther.myPnt;
  myDerivArr[0]                        = theOther.myDerivArr[0];
  myDerivArr[1]                        = theOther.myDerivArr[1];
  myDerivArr[2]                        = theOther.myDerivArr[2];
  myTangent                            = theOther.myTangent;
  myCurvature                          = theOther.myCurvature;
  myTangentStatus                      = theOther.myTangentStatus;
  mySignificantFirstDerivativeOrder    = theOther.mySignificantFirstDerivativeOrder;
  return *this;
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
  if (myLegacyProps != nullptr)
  {
    myLegacyProps->SetParameter(U);
  }
}

//==================================================================================================

void Geom2dLProp_CLProps2d::SetCurve(const occ::handle<Geom2d_Curve>& C)
{
  myCurve                           = C;
  if (myLegacyProps == nullptr)
  {
    myLegacyProps = std::make_shared<Geom2dLProp_LegacyCLProps2d>(myDerOrder, myLinTol);
  }
  myLegacyProps->SetCurve(C);
  myCN                              = 4;
  myCurvature                       = 0.0;
  myTangentStatus                   = LProp_Undecided;
  mySignificantFirstDerivativeOrder = 0;
}

//==================================================================================================

const gp_Pnt2d& Geom2dLProp_CLProps2d::Value() const
{
  if (myLegacyProps != nullptr && !myPnt.IsEqual(myLegacyProps->Value(), myLinTol))
  {
    LProp_CompareDebug::LogValueMismatch("Geom2dLProp_CLProps2d::Value",
                                         curveGeometry(myCurve),
                                         curveParameters(myU),
                                         myDerOrder,
                                         myLinTol,
                                         "point",
                                         myPnt,
                                         myLegacyProps->Value());
  }
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
  if (myLegacyProps != nullptr && !myDerivArr[0].IsEqual(myLegacyProps->D1(), myLinTol, myLinTol))
  {
    LProp_CompareDebug::LogValueMismatch("Geom2dLProp_CLProps2d::D1",
                                         curveGeometry(myCurve),
                                         curveParameters(myU),
                                         myDerOrder,
                                         myLinTol,
                                         "vec",
                                         myDerivArr[0],
                                         myLegacyProps->D1());
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
  if (myLegacyProps != nullptr && !myDerivArr[1].IsEqual(myLegacyProps->D2(), myLinTol, myLinTol))
  {
    LProp_CompareDebug::LogValueMismatch("Geom2dLProp_CLProps2d::D2",
                                         curveGeometry(myCurve),
                                         curveParameters(myU),
                                         myDerOrder,
                                         myLinTol,
                                         "vec",
                                         myDerivArr[1],
                                         myLegacyProps->D2());
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
  if (myLegacyProps != nullptr && !myDerivArr[2].IsEqual(myLegacyProps->D3(), myLinTol, myLinTol))
  {
    LProp_CompareDebug::LogValueMismatch("Geom2dLProp_CLProps2d::D3",
                                         curveGeometry(myCurve),
                                         curveParameters(myU),
                                         myDerOrder,
                                         myLinTol,
                                         "vec",
                                         myDerivArr[2],
                                         myLegacyProps->D3());
  }

  return myDerivArr[2];
}

//==================================================================================================

bool Geom2dLProp_CLProps2d::IsTangentDefined()
{
  const bool isDefined = LProp_CLPropsCompat::IsTangentDefined(myCN,
                                                               myLinTol,
                                                               D1(),
                                                               D2(),
                                                               D3(),
                                                               mySignificantFirstDerivativeOrder,
                                                               myTangentStatus);
  const bool anOldDefined = myLegacyProps != nullptr ? myLegacyProps->IsTangentDefined() : isDefined;
  if (myLegacyProps != nullptr
      && (isDefined != anOldDefined || myTangentStatus != myLegacyProps->TangentStatus()))
  {
    LProp_CompareDebug::LogMismatch("Geom2dLProp_CLProps2d::IsTangentDefined",
                                    curveGeometry(myCurve),
                                    curveParameters(myU),
                                    myDerOrder,
                                    myLinTol,
                                    std::string("new=") + LProp_CompareDebug::ToString(isDefined)
                                      + " old=" + LProp_CompareDebug::ToString(anOldDefined)
                                      + " newStatus=" + LProp_CompareDebug::ToString(myTangentStatus)
                                      + " oldStatus="
                                      + LProp_CompareDebug::ToString(myLegacyProps->TangentStatus()));
  }
  return isDefined;
}

//==================================================================================================

void Geom2dLProp_CLProps2d::Tangent(gp_Dir2d& D)
{
  LProp_NotDefined_Raise_if(!IsTangentDefined(), "Geom2dLProp_CLProps2d::Tangent()");
  LProp_CLPropsCompat::Tangent<Geom2dProp::TangentResult, gp_Dir2d, gp_Pnt2d>(
    mySignificantFirstDerivativeOrder,
    myLinTol,
    myU,
    myCurve->FirstParameter(),
    myCurve->LastParameter(),
    [&]() { return Geom2dProp::ComputeTangent(D1(), D2(), D3(), myLinTol); },
    [&](const gp_Pnt2d& thePntBefore, const gp_Pnt2d& thePntAfter) {
      return Geom2dProp::ComputeTangent(D1(), D2(), D3(), myLinTol, thePntBefore, thePntAfter);
    },
    [&](const double theParam) { return myCurve->Value(theParam); },
    D,
    "Geom2dLProp_CLProps2d::Tangent()");
  if (myLegacyProps != nullptr) try
  {
    gp_Dir2d anOldDir;
    myLegacyProps->Tangent(anOldDir);
    if (!D.IsEqual(anOldDir, Precision::Angular()))
    {
      LProp_CompareDebug::LogValueMismatch("Geom2dLProp_CLProps2d::Tangent",
                                           curveGeometry(myCurve),
                                           curveParameters(myU),
                                           myDerOrder,
                                           myLinTol,
                                           "dir",
                                           D,
                                           anOldDir);
    }
  }
  catch (Standard_Failure const& theFailure)
  {
    LProp_CompareDebug::LogExceptionMismatch("Geom2dLProp_CLProps2d::Tangent",
                                             curveGeometry(myCurve),
                                             curveParameters(myU),
                                             myDerOrder,
                                             myLinTol,
                                             "value",
                                             LProp_CompareDebug::ExceptionSummary(theFailure));
  }
}

//==================================================================================================

double Geom2dLProp_CLProps2d::Curvature()
{
  const bool isDefined = IsTangentDefined();
  (void)isDefined;
  LProp_NotDefined_Raise_if(!isDefined, "Geom2dLProp_CLProps2d::Curvature()");

  myCurvature = LProp_CLPropsCompat::Curvature<Geom2dProp::CurvatureResult>(
    mySignificantFirstDerivativeOrder,
    myLinTol,
    [&]() { return Geom2dProp::ComputeCurvature(D1(), D2(), myLinTol); },
    "Geom2dLProp_CLProps2d::Curvature()");
  if (myLegacyProps != nullptr) try
  {
    const double anOldCurv = myLegacyProps->Curvature();
    if ((myCurvature == RealLast()) != (anOldCurv == RealLast())
        || !sameCurvatureClass(myCurvature, anOldCurv, myLinTol)
        || (myCurvature != RealLast() && std::abs(myCurvature - anOldCurv) > myLinTol))
    {
      LProp_CompareDebug::LogValueMismatch("Geom2dLProp_CLProps2d::Curvature",
                                           curveGeometry(myCurve),
                                           curveParameters(myU),
                                           myDerOrder,
                                           myLinTol,
                                           "curv",
                                           myCurvature,
                                           anOldCurv);
    }
  }
  catch (Standard_Failure const& theFailure)
  {
    LProp_CompareDebug::LogExceptionMismatch("Geom2dLProp_CLProps2d::Curvature",
                                             curveGeometry(myCurve),
                                             curveParameters(myU),
                                             myDerOrder,
                                             myLinTol,
                                             "value",
                                             LProp_CompareDebug::ExceptionSummary(theFailure));
  }
  return myCurvature;
}

//==================================================================================================

void Geom2dLProp_CLProps2d::Normal(gp_Dir2d& D)
{
  LProp_CLPropsCompat::Normal<Geom2dProp::NormalResult>(
    Curvature(),
    myLinTol,
    "Geom2dLProp_CLProps2d::Normal(): Curvature is null or infinity",
    [&]() { return Geom2dProp::ComputeNormal(D1(), D2(), myLinTol); },
    D,
    "Geom2dLProp_CLProps2d::Normal()");
  if (myLegacyProps != nullptr) try
  {
    gp_Dir2d anOldDir;
    myLegacyProps->Normal(anOldDir);
    if (!D.IsEqual(anOldDir, Precision::Angular()))
    {
      LProp_CompareDebug::LogValueMismatch("Geom2dLProp_CLProps2d::Normal",
                                           curveGeometry(myCurve),
                                           curveParameters(myU),
                                           myDerOrder,
                                           myLinTol,
                                           "dir",
                                           D,
                                           anOldDir);
    }
  }
  catch (Standard_Failure const& theFailure)
  {
    LProp_CompareDebug::LogExceptionMismatch("Geom2dLProp_CLProps2d::Normal",
                                             curveGeometry(myCurve),
                                             curveParameters(myU),
                                             myDerOrder,
                                             myLinTol,
                                             "value",
                                             LProp_CompareDebug::ExceptionSummary(theFailure));
  }
}

//==================================================================================================

void Geom2dLProp_CLProps2d::CentreOfCurvature(gp_Pnt2d& P)
{
  LProp_CLPropsCompat::CentreOfCurvature<Geom2dProp::CentreResult>(
    Curvature(),
    myLinTol,
    [&]() { return Geom2dProp::ComputeCentreOfCurvature(myPnt, D1(), D2(), myLinTol); },
    P,
    "Geom2dLProp_CLProps2d::CentreOfCurvature()");
  if (myLegacyProps != nullptr) try
  {
    gp_Pnt2d anOldPoint;
    myLegacyProps->CentreOfCurvature(anOldPoint);
    if (!P.IsEqual(anOldPoint, myLinTol))
    {
      LProp_CompareDebug::LogValueMismatch("Geom2dLProp_CLProps2d::CentreOfCurvature",
                                           curveGeometry(myCurve),
                                           curveParameters(myU),
                                           myDerOrder,
                                           myLinTol,
                                           "point",
                                           P,
                                           anOldPoint);
    }
  }
  catch (Standard_Failure const& theFailure)
  {
    LProp_CompareDebug::LogExceptionMismatch("Geom2dLProp_CLProps2d::CentreOfCurvature",
                                             curveGeometry(myCurve),
                                             curveParameters(myU),
                                             myDerOrder,
                                             myLinTol,
                                             "value",
                                             LProp_CompareDebug::ExceptionSummary(theFailure));
  }
}
