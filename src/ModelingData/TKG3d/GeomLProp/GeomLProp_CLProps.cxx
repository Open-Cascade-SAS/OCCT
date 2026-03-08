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

#include <GeomLProp_LegacyCLProps.hxx>
#include <GeomProp.hxx>
#include <GeomProp_Curve.hxx>
#include <Geom_Curve.hxx>
#include <LProp_CLPropsCompat.pxx>
#include <LProp_CompareDebug.pxx>
#include <LProp_NotDefined.hxx>
#include <LProp_Status.hxx>
#include <Precision.hxx>
#include <Standard_OutOfRange.hxx>

namespace
{
gp_Pnt curveValue(const occ::handle<Geom_Curve>& theCurve, const double theParam)
{
  gp_Pnt aPoint;
  theCurve->D0(theParam, aPoint);
  return aPoint;
}

void curveD1(const occ::handle<Geom_Curve>& theCurve,
             const double                   theParam,
             gp_Pnt&                        thePoint,
             gp_Vec&                        theD1)
{
  theCurve->D1(theParam, thePoint, theD1);
}

void curveD2(const occ::handle<Geom_Curve>& theCurve,
             const double                   theParam,
             gp_Pnt&                        thePoint,
             gp_Vec&                        theD1,
             gp_Vec&                        theD2)
{
  theCurve->D2(theParam, thePoint, theD1, theD2);
}

void curveD3(const occ::handle<Geom_Curve>& theCurve,
             const double                   theParam,
             gp_Pnt&                        thePoint,
             gp_Vec&                        theD1,
             gp_Vec&                        theD2,
             gp_Vec&                        theD3)
{
  theCurve->D3(theParam, thePoint, theD1, theD2, theD3);
}

double curveFirstParameter(const occ::handle<Geom_Curve>& theCurve)
{
  return theCurve->FirstParameter();
}

double curveLastParameter(const occ::handle<Geom_Curve>& theCurve)
{
  return theCurve->LastParameter();
}

std::string curveGeometry(const std::shared_ptr<GeomProp_Curve>& theCurveProp)
{
  return theCurveProp != nullptr ? std::to_string((int)theCurveProp->GetType()) : "null";
}

std::string curveParameters(const double theU)
{
  return "U=" + LProp_CompareDebug::ToString(theU);
}
} // namespace

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

GeomLProp_CLProps::GeomLProp_CLProps(const GeomLProp_CLProps& theOther)
    : myCurve(theOther.myCurve),
      myCurveProp(theOther.myCurve.IsNull() ? nullptr
                                            : std::make_shared<GeomProp_Curve>(theOther.myCurve)),
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

GeomLProp_CLProps& GeomLProp_CLProps::operator=(const GeomLProp_CLProps& theOther)
{
  if (this == &theOther)
  {
    return *this;
  }

  myCurve    = theOther.myCurve;
  myCurveProp = theOther.myCurve.IsNull() ? nullptr
                                          : std::make_shared<GeomProp_Curve>(theOther.myCurve);
  myU                               = theOther.myU;
  myDerOrder                        = theOther.myDerOrder;
  myCN                              = theOther.myCN;
  myLinTol                          = theOther.myLinTol;
  myPnt                             = theOther.myPnt;
  myDerivArr[0]                     = theOther.myDerivArr[0];
  myDerivArr[1]                     = theOther.myDerivArr[1];
  myDerivArr[2]                     = theOther.myDerivArr[2];
  myTangent                         = theOther.myTangent;
  myCurvature                       = theOther.myCurvature;
  myTangentStatus                   = theOther.myTangentStatus;
  mySignificantFirstDerivativeOrder = theOther.mySignificantFirstDerivativeOrder;
  return *this;
}

//==================================================================================================

void GeomLProp_CLProps::SetParameter(const double U)
{
  myU = U;
  switch (myDerOrder)
  {
    case 0:
      myPnt = curveValue(myCurve, myU);
      break;
    case 1:
      curveD1(myCurve, myU, myPnt, myDerivArr[0]);
      break;
    case 2:
      curveD2(myCurve, myU, myPnt, myDerivArr[0], myDerivArr[1]);
      break;
    case 3:
      curveD3(myCurve, myU, myPnt, myDerivArr[0], myDerivArr[1], myDerivArr[2]);
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
  GeomLProp_LegacyCLProps aLegacy(myCurve, myU, myDerOrder, myLinTol);
  if (!myPnt.IsEqual(aLegacy.Value(), myLinTol))
  {
    LProp_CompareDebug::LogValueMismatch("GeomLProp_CLProps::Value",
                                         curveGeometry(myCurveProp),
                                         curveParameters(myU),
                                         myDerOrder,
                                         myLinTol,
                                         "point",
                                         myPnt,
                                         aLegacy.Value());
  }
  return myPnt;
}

//==================================================================================================

const gp_Vec& GeomLProp_CLProps::D1()
{
  if (myDerOrder < 1)
  {
    myDerOrder = 1;
    curveD1(myCurve, myU, myPnt, myDerivArr[0]);
  }
  GeomLProp_LegacyCLProps aLegacy(myCurve, myU, myDerOrder, myLinTol);
  if (!myDerivArr[0].IsEqual(aLegacy.D1(), myLinTol, myLinTol))
  {
    LProp_CompareDebug::LogValueMismatch("GeomLProp_CLProps::D1",
                                         curveGeometry(myCurveProp),
                                         curveParameters(myU),
                                         myDerOrder,
                                         myLinTol,
                                         "vec",
                                         myDerivArr[0],
                                         aLegacy.D1());
  }

  return myDerivArr[0];
}

//==================================================================================================

const gp_Vec& GeomLProp_CLProps::D2()
{
  if (myDerOrder < 2)
  {
    myDerOrder = 2;
    curveD2(myCurve, myU, myPnt, myDerivArr[0], myDerivArr[1]);
  }
  GeomLProp_LegacyCLProps aLegacy(myCurve, myU, myDerOrder, myLinTol);
  if (!myDerivArr[1].IsEqual(aLegacy.D2(), myLinTol, myLinTol))
  {
    LProp_CompareDebug::LogValueMismatch("GeomLProp_CLProps::D2",
                                         curveGeometry(myCurveProp),
                                         curveParameters(myU),
                                         myDerOrder,
                                         myLinTol,
                                         "vec",
                                         myDerivArr[1],
                                         aLegacy.D2());
  }

  return myDerivArr[1];
}

//==================================================================================================

const gp_Vec& GeomLProp_CLProps::D3()
{
  if (myDerOrder < 3)
  {
    myDerOrder = 3;
    curveD3(myCurve, myU, myPnt, myDerivArr[0], myDerivArr[1], myDerivArr[2]);
  }
  GeomLProp_LegacyCLProps aLegacy(myCurve, myU, myDerOrder, myLinTol);
  if (!myDerivArr[2].IsEqual(aLegacy.D3(), myLinTol, myLinTol))
  {
    LProp_CompareDebug::LogValueMismatch("GeomLProp_CLProps::D3",
                                         curveGeometry(myCurveProp),
                                         curveParameters(myU),
                                         myDerOrder,
                                         myLinTol,
                                         "vec",
                                         myDerivArr[2],
                                         aLegacy.D3());
  }

  return myDerivArr[2];
}

//==================================================================================================

bool GeomLProp_CLProps::IsTangentDefined()
{
  const bool isDefined = LProp_CLPropsCompat::IsTangentDefined(myCN,
                                                               myLinTol,
                                                               D1(),
                                                               D2(),
                                                               D3(),
                                                               mySignificantFirstDerivativeOrder,
                                                               myTangentStatus);
  GeomLProp_LegacyCLProps aLegacy(myCurve, myU, myDerOrder, myLinTol);
  const bool              anOldDefined = aLegacy.IsTangentDefined();
  if (isDefined != anOldDefined || myTangentStatus != aLegacy.TangentStatus())
  {
    LProp_CompareDebug::LogMismatch("GeomLProp_CLProps::IsTangentDefined",
                                    curveGeometry(myCurveProp),
                                    curveParameters(myU),
                                    myDerOrder,
                                    myLinTol,
                                    std::string("new=") + LProp_CompareDebug::ToString(isDefined)
                                      + " old=" + LProp_CompareDebug::ToString(anOldDefined)
                                      + " newStatus=" + LProp_CompareDebug::ToString(myTangentStatus)
                                      + " oldStatus=" + LProp_CompareDebug::ToString(aLegacy.TangentStatus()));
  }
  return isDefined;
}

//==================================================================================================

void GeomLProp_CLProps::Tangent(gp_Dir& D)
{
  LProp_NotDefined_Raise_if(!IsTangentDefined(), "GeomLProp_CLProps::Tangent()");
  LProp_CLPropsCompat::Tangent<GeomProp::TangentResult, gp_Dir, gp_Pnt>(
    mySignificantFirstDerivativeOrder,
    myLinTol,
    myU,
    curveFirstParameter(myCurve),
    curveLastParameter(myCurve),
    [&]() {
      return myCurveProp ? myCurveProp->Tangent(myU, myLinTol)
                         : GeomProp::ComputeTangent(D1(), D2(), D3(), myLinTol);
    },
    [&](const gp_Pnt& thePntBefore, const gp_Pnt& thePntAfter) {
      return GeomProp::ComputeTangent(D1(), D2(), D3(), myLinTol, thePntBefore, thePntAfter);
    },
    [&](const double theParam) { return curveValue(myCurve, theParam); },
    D,
    "GeomLProp_CLProps::Tangent()");
  GeomLProp_LegacyCLProps aLegacy(myCurve, myU, myDerOrder, myLinTol);
  try
  {
    gp_Dir anOldDir;
    aLegacy.Tangent(anOldDir);
    if (!D.IsEqual(anOldDir, Precision::Angular()))
    {
      LProp_CompareDebug::LogValueMismatch("GeomLProp_CLProps::Tangent",
                                           curveGeometry(myCurveProp),
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
    LProp_CompareDebug::LogExceptionMismatch("GeomLProp_CLProps::Tangent",
                                             curveGeometry(myCurveProp),
                                             curveParameters(myU),
                                             myDerOrder,
                                             myLinTol,
                                             "value",
                                             LProp_CompareDebug::ExceptionSummary(theFailure));
  }
}

//==================================================================================================

double GeomLProp_CLProps::Curvature()
{
  const bool isDefined = IsTangentDefined();
  (void)isDefined;
  LProp_NotDefined_Raise_if(!isDefined, "GeomLProp_CLProps::Curvature()");
  myCurvature = LProp_CLPropsCompat::Curvature<GeomProp::CurvatureResult>(
    mySignificantFirstDerivativeOrder,
    myLinTol,
    [&]() {
      return myCurveProp ? myCurveProp->Curvature(myU, myLinTol)
                         : GeomProp::ComputeCurvature(D1(), D2(), myLinTol);
    },
    "GeomLProp_CLProps::Curvature()");
  GeomLProp_LegacyCLProps aLegacy(myCurve, myU, myDerOrder, myLinTol);
  try
  {
    const double anOldCurv = aLegacy.Curvature();
    if ((myCurvature == RealLast()) != (anOldCurv == RealLast())
        || (myCurvature != RealLast() && std::abs(myCurvature - anOldCurv) > myLinTol))
    {
      LProp_CompareDebug::LogValueMismatch("GeomLProp_CLProps::Curvature",
                                           curveGeometry(myCurveProp),
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
    LProp_CompareDebug::LogExceptionMismatch("GeomLProp_CLProps::Curvature",
                                             curveGeometry(myCurveProp),
                                             curveParameters(myU),
                                             myDerOrder,
                                             myLinTol,
                                             "value",
                                             LProp_CompareDebug::ExceptionSummary(theFailure));
  }
  return myCurvature;
}

//==================================================================================================

void GeomLProp_CLProps::Normal(gp_Dir& D)
{
  LProp_CLPropsCompat::Normal<GeomProp::NormalResult>(
    Curvature(),
    myLinTol,
    "GeomLProp_CLProps::Normal(): Curvature is null or infinity",
    [&]() {
      return myCurveProp ? myCurveProp->Normal(myU, myLinTol)
                         : GeomProp::ComputeNormal(D1(), D2(), myLinTol);
    },
    D,
    "GeomLProp_CLProps::Normal()");
  GeomLProp_LegacyCLProps aLegacy(myCurve, myU, myDerOrder, myLinTol);
  try
  {
    gp_Dir anOldDir;
    aLegacy.Normal(anOldDir);
    if (!D.IsEqual(anOldDir, Precision::Angular()))
    {
      LProp_CompareDebug::LogValueMismatch("GeomLProp_CLProps::Normal",
                                           curveGeometry(myCurveProp),
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
    LProp_CompareDebug::LogExceptionMismatch("GeomLProp_CLProps::Normal",
                                             curveGeometry(myCurveProp),
                                             curveParameters(myU),
                                             myDerOrder,
                                             myLinTol,
                                             "value",
                                             LProp_CompareDebug::ExceptionSummary(theFailure));
  }
}

//==================================================================================================

void GeomLProp_CLProps::CentreOfCurvature(gp_Pnt& P)
{
  LProp_CLPropsCompat::CentreOfCurvature<GeomProp::CentreResult>(
    Curvature(),
    myLinTol,
    [&]() {
      return myCurveProp ? myCurveProp->CentreOfCurvature(myU, myLinTol)
                         : GeomProp::ComputeCentreOfCurvature(myPnt, D1(), D2(), myLinTol);
    },
    P,
    "GeomLProp_CLProps::CentreOfCurvature()");
  GeomLProp_LegacyCLProps aLegacy(myCurve, myU, myDerOrder, myLinTol);
  try
  {
    gp_Pnt anOldPoint;
    aLegacy.CentreOfCurvature(anOldPoint);
    if (!P.IsEqual(anOldPoint, myLinTol))
    {
      LProp_CompareDebug::LogValueMismatch("GeomLProp_CLProps::CentreOfCurvature",
                                           curveGeometry(myCurveProp),
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
    LProp_CompareDebug::LogExceptionMismatch("GeomLProp_CLProps::CentreOfCurvature",
                                             curveGeometry(myCurveProp),
                                             curveParameters(myU),
                                             myDerOrder,
                                             myLinTol,
                                             "value",
                                             LProp_CompareDebug::ExceptionSummary(theFailure));
  }
}
