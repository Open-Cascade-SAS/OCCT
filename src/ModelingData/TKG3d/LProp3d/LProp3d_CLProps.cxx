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
#include <LProp3d_LegacyCLProps.hxx>
#include <LProp_CLPropsCompat.pxx>
#include <LProp_CompareDebug.pxx>
#include <LProp_NotDefined.hxx>
#include <LProp_Status.hxx>
#include <LProp_WrapperTools.pxx>
#include <Precision.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_OutOfRange.hxx>

namespace
{
std::shared_ptr<GeomProp_Curve> makeCurveProp(const occ::handle<Adaptor3d_Curve>& theCurve)
{
  return std::make_shared<GeomProp_Curve>(*theCurve);
}

std::string curveGeometry(const occ::handle<Adaptor3d_Curve>& theCurve)
{
  return theCurve.IsNull() ? "null" : std::to_string((int)GeomProp_Curve(*theCurve).GetType());
}

std::string curveParameters(const double theU)
{
  return "U=" + LProp_CompareDebug::ToString(theU);
}
} // namespace

//==================================================================================================

LProp3d_CLProps::LProp3d_CLProps(const occ::handle<Adaptor3d_Curve>& C,
                                 const double                        U,
                                 const int                           N,
                                 const double                        Resolution)
    : myCurve(C),
      myCurveProp(makeCurveProp(C)),
      myLegacyProps(C.IsNull() ? nullptr
                               : std::make_shared<LProp3d_LegacyCLProps>(C, N, Resolution)),
      myDerOrder(N),
      myCN(4),
      myLinTol(Resolution),
      myCurvature(0.0),
      myTangentStatus(LProp_Undecided),
      mySignificantFirstDerivativeOrder(0),
      myHasTangent(false),
      myHasCurvature(false),
      myHasNormal(false),
      myHasCentre(false)
{
  Standard_NullObject_Raise_if(C.IsNull(), "LProp3d_CLProps::LProp3d_CLProps()");
  Standard_OutOfRange_Raise_if(N < 0 || N > 3, "LProp3d_CLProps::LProp3d_CLProps()");
  SetParameter(U);
}

//==================================================================================================

LProp3d_CLProps::LProp3d_CLProps(const occ::handle<Adaptor3d_Curve>& C,
                                 const int                           N,
                                 const double                        Resolution)
    : myCurve(C),
      myCurveProp(makeCurveProp(C)),
      myLegacyProps(C.IsNull() ? nullptr
                               : std::make_shared<LProp3d_LegacyCLProps>(C, N, Resolution)),
      myU(RealLast()),
      myDerOrder(N),
      myCN(4),
      myLinTol(Resolution),
      myCurvature(0.0),
      myTangentStatus(LProp_Undecided),
      mySignificantFirstDerivativeOrder(0),
      myHasTangent(false),
      myHasCurvature(false),
      myHasNormal(false),
      myHasCentre(false)
{
  Standard_NullObject_Raise_if(C.IsNull(), "LProp3d_CLProps::LProp3d_CLProps()");
  Standard_OutOfRange_Raise_if(N < 0 || N > 3, "LProp3d_CLProps::LProp3d_CLProps()");
}

//==================================================================================================

LProp3d_CLProps::~LProp3d_CLProps() = default;

//==================================================================================================

LProp3d_CLProps::LProp3d_CLProps(const LProp3d_CLProps& theOther)
    : myCurve(theOther.myCurve),
      myCurveProp(makeCurveProp(theOther.myCurve)),
      myLegacyProps(LProp_WrapperTools::CloneShared(theOther.myLegacyProps)),
      myU(theOther.myU),
      myDerOrder(theOther.myDerOrder),
      myCN(theOther.myCN),
      myLinTol(theOther.myLinTol),
      myPnt(theOther.myPnt),
      myTangent(theOther.myTangent),
      myNormal(theOther.myNormal),
      myCentre(theOther.myCentre),
      myCurvature(theOther.myCurvature),
      myTangentStatus(theOther.myTangentStatus),
      mySignificantFirstDerivativeOrder(theOther.mySignificantFirstDerivativeOrder),
      myHasTangent(theOther.myHasTangent),
      myHasCurvature(theOther.myHasCurvature),
      myHasNormal(theOther.myHasNormal),
      myHasCentre(theOther.myHasCentre)
{
  myDerivArr[0] = theOther.myDerivArr[0];
  myDerivArr[1] = theOther.myDerivArr[1];
  myDerivArr[2] = theOther.myDerivArr[2];
}

//==================================================================================================

LProp3d_CLProps& LProp3d_CLProps::operator=(const LProp3d_CLProps& theOther)
{
  if (this == &theOther)
  {
    return *this;
  }

  myCurve = theOther.myCurve;
  myCurveProp = makeCurveProp(theOther.myCurve);
  myLegacyProps = LProp_WrapperTools::CloneShared(theOther.myLegacyProps);
  myU                               = theOther.myU;
  myDerOrder                        = theOther.myDerOrder;
  myCN                              = theOther.myCN;
  myLinTol                          = theOther.myLinTol;
  myPnt                             = theOther.myPnt;
  myDerivArr[0]                     = theOther.myDerivArr[0];
  myDerivArr[1]                     = theOther.myDerivArr[1];
  myDerivArr[2]                     = theOther.myDerivArr[2];
  myTangent                         = theOther.myTangent;
  myNormal                          = theOther.myNormal;
  myCentre                          = theOther.myCentre;
  myCurvature                       = theOther.myCurvature;
  myTangentStatus                   = theOther.myTangentStatus;
  mySignificantFirstDerivativeOrder = theOther.mySignificantFirstDerivativeOrder;
  myHasTangent                      = theOther.myHasTangent;
  myHasCurvature                    = theOther.myHasCurvature;
  myHasNormal                       = theOther.myHasNormal;
  myHasCentre                       = theOther.myHasCentre;
  return *this;
}

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

  LProp_WrapperTools::ResetCurveState(myCurvature,
                                      myTangentStatus,
                                      mySignificantFirstDerivativeOrder);
  LProp_WrapperTools::ResetCurvePropertyCache(myHasTangent,
                                              myHasCurvature,
                                              myHasNormal,
                                              myHasCentre);
  if (myLegacyProps != nullptr)
  {
    myLegacyProps->SetParameter(U);
  }
}

//==================================================================================================

void LProp3d_CLProps::SetCurve(const occ::handle<Adaptor3d_Curve>& C)
{
  Standard_NullObject_Raise_if(C.IsNull(), "LProp3d_CLProps::SetCurve()");
  myCurve                           = C;
  myCurveProp                       = makeCurveProp(C);
  myLegacyProps->SetCurve(C);
  myCN                              = 4;
  LProp_WrapperTools::ResetCurveState(myCurvature,
                                      myTangentStatus,
                                      mySignificantFirstDerivativeOrder);
  LProp_WrapperTools::ResetCurvePropertyCache(myHasTangent,
                                              myHasCurvature,
                                              myHasNormal,
                                              myHasCentre);
}

//==================================================================================================

const gp_Pnt& LProp3d_CLProps::Value() const
{
  if (myLegacyProps != nullptr && !myPnt.IsEqual(myLegacyProps->Value(), myLinTol))
  {
    LProp_CompareDebug::LogValueMismatch("LProp3d_CLProps::Value",
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

const gp_Vec& LProp3d_CLProps::D1()
{
  if (myDerOrder < 1)
  {
    myDerOrder = 1;
    myCurve->D1(myU, myPnt, myDerivArr[0]);
  }
  if (myLegacyProps != nullptr && !myDerivArr[0].IsEqual(myLegacyProps->D1(), myLinTol, myLinTol))
  {
    LProp_CompareDebug::LogValueMismatch("LProp3d_CLProps::D1",
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

const gp_Vec& LProp3d_CLProps::D2()
{
  if (myDerOrder < 2)
  {
    myDerOrder = 2;
    myCurve->D2(myU, myPnt, myDerivArr[0], myDerivArr[1]);
  }
  if (myLegacyProps != nullptr && !myDerivArr[1].IsEqual(myLegacyProps->D2(), myLinTol, myLinTol))
  {
    LProp_CompareDebug::LogValueMismatch("LProp3d_CLProps::D2",
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

const gp_Vec& LProp3d_CLProps::D3()
{
  if (myDerOrder < 3)
  {
    myDerOrder = 3;
    myCurve->D3(myU, myPnt, myDerivArr[0], myDerivArr[1], myDerivArr[2]);
  }
  if (myLegacyProps != nullptr && !myDerivArr[2].IsEqual(myLegacyProps->D3(), myLinTol, myLinTol))
  {
    LProp_CompareDebug::LogValueMismatch("LProp3d_CLProps::D3",
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

bool LProp3d_CLProps::IsTangentDefined()
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
    LProp_CompareDebug::LogMismatch("LProp3d_CLProps::IsTangentDefined",
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

void LProp3d_CLProps::Tangent(gp_Dir& D)
{
  LProp_NotDefined_Raise_if(!IsTangentDefined(), "LProp3d_CLProps::Tangent()");
  if (myHasTangent)
  {
    D = myTangent;
    return;
  }
  LProp_CLPropsCompat::Tangent<GeomProp::TangentResult, gp_Dir, gp_Pnt>(
    mySignificantFirstDerivativeOrder,
    myLinTol,
    myU,
    myCurve->FirstParameter(),
    myCurve->LastParameter(),
    [&]() { return myCurveProp->Tangent(myU, myLinTol); },
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
  myTangent    = D;
  myHasTangent = true;
  if (myLegacyProps != nullptr)
  {
    try
    {
      gp_Dir anOldDir;
      myLegacyProps->Tangent(anOldDir);
      if (!D.IsEqual(anOldDir, Precision::Angular()))
      {
        LProp_CompareDebug::LogValueMismatch("LProp3d_CLProps::Tangent",
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
      LProp_CompareDebug::LogExceptionMismatch("LProp3d_CLProps::Tangent",
                                               curveGeometry(myCurve),
                                               curveParameters(myU),
                                               myDerOrder,
                                               myLinTol,
                                               "value",
                                               LProp_CompareDebug::ExceptionSummary(theFailure));
    }
  }
}

//==================================================================================================

double LProp3d_CLProps::Curvature()
{
  if (myHasCurvature)
  {
    return myCurvature;
  }
  const bool isDefined = IsTangentDefined();
  (void)isDefined;
  LProp_NotDefined_Raise_if(!isDefined, "LProp3d_CLProps::Curvature()");

  myCurvature = LProp_CLPropsCompat::Curvature<GeomProp::CurvatureResult>(
    mySignificantFirstDerivativeOrder,
    myLinTol,
    [&]() { return myCurveProp->Curvature(myU, myLinTol); },
    "LProp3d_CLProps::Curvature()");
  if (myLegacyProps != nullptr)
  {
    try
    {
      const double anOldCurv = myLegacyProps->Curvature();
      if ((myCurvature == RealLast()) != (anOldCurv == RealLast())
          || (myCurvature != RealLast() && std::abs(myCurvature - anOldCurv) > myLinTol))
      {
        LProp_CompareDebug::LogValueMismatch("LProp3d_CLProps::Curvature",
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
      LProp_CompareDebug::LogExceptionMismatch("LProp3d_CLProps::Curvature",
                                               curveGeometry(myCurve),
                                               curveParameters(myU),
                                               myDerOrder,
                                               myLinTol,
                                               "value",
                                               LProp_CompareDebug::ExceptionSummary(theFailure));
    }
  }
  return myCurvature;
}

//==================================================================================================

void LProp3d_CLProps::Normal(gp_Dir& D)
{
  if (myHasNormal)
  {
    D = myNormal;
    return;
  }
  LProp_CLPropsCompat::Normal<GeomProp::NormalResult>(
    Curvature(),
    myLinTol,
    "LProp3d_CLProps::Normal(): Curvature is null or infinity",
    [&]() { return myCurveProp->Normal(myU, myLinTol); },
    D,
    "LProp3d_CLProps::Normal()");
  myNormal    = D;
  myHasNormal = true;
  if (myLegacyProps != nullptr)
  {
    try
    {
      gp_Dir anOldDir;
      myLegacyProps->Normal(anOldDir);
      if (!D.IsEqual(anOldDir, Precision::Angular()))
      {
        LProp_CompareDebug::LogValueMismatch("LProp3d_CLProps::Normal",
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
      LProp_CompareDebug::LogExceptionMismatch("LProp3d_CLProps::Normal",
                                               curveGeometry(myCurve),
                                               curveParameters(myU),
                                               myDerOrder,
                                               myLinTol,
                                               "value",
                                               LProp_CompareDebug::ExceptionSummary(theFailure));
    }
  }
}

//==================================================================================================

void LProp3d_CLProps::CentreOfCurvature(gp_Pnt& P)
{
  if (myHasCentre)
  {
    P = myCentre;
    return;
  }
  LProp_CLPropsCompat::CentreOfCurvature<GeomProp::CentreResult>(
    Curvature(),
    myLinTol,
    [&]() { return myCurveProp->CentreOfCurvature(myU, myLinTol); },
    P,
    "LProp3d_CLProps::CentreOfCurvature()");
  myCentre    = P;
  myHasCentre = true;
  if (myLegacyProps != nullptr)
  {
    try
    {
      gp_Pnt anOldPoint;
      myLegacyProps->CentreOfCurvature(anOldPoint);
      if (!P.IsEqual(anOldPoint, myLinTol))
      {
        LProp_CompareDebug::LogValueMismatch("LProp3d_CLProps::CentreOfCurvature",
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
      LProp_CompareDebug::LogExceptionMismatch("LProp3d_CLProps::CentreOfCurvature",
                                               curveGeometry(myCurve),
                                               curveParameters(myU),
                                               myDerOrder,
                                               myLinTol,
                                               "value",
                                               LProp_CompareDebug::ExceptionSummary(theFailure));
    }
  }
}
