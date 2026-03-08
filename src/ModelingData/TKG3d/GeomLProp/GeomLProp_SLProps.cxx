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

#include <GeomLProp_SLProps.hxx>

#include <GeomLProp_LegacySLProps.hxx>
#include <GeomProp.hxx>
#include <GeomProp_Surface.hxx>
#include <Geom_Surface.hxx>
#include <LProp_SLPropsCompat.pxx>
#include <LProp_CompareDebug.pxx>
#include <LProp_NotDefined.hxx>
#include <LProp_Status.hxx>
#include <Precision.hxx>
#include <Standard_OutOfRange.hxx>

namespace
{
gp_Pnt surfaceValue(const occ::handle<Geom_Surface>& theSurface, const double theU, const double theV)
{
  gp_Pnt aPoint;
  theSurface->D0(theU, theV, aPoint);
  return aPoint;
}

void surfaceD1(const occ::handle<Geom_Surface>& theSurface,
               const double                     theU,
               const double                     theV,
               gp_Pnt&                          thePoint,
               gp_Vec&                          theD1U,
               gp_Vec&                          theD1V)
{
  theSurface->D1(theU, theV, thePoint, theD1U, theD1V);
}

void surfaceD2(const occ::handle<Geom_Surface>& theSurface,
               const double                     theU,
               const double                     theV,
               gp_Pnt&                          thePoint,
               gp_Vec&                          theD1U,
               gp_Vec&                          theD1V,
               gp_Vec&                          theD2U,
               gp_Vec&                          theD2V,
               gp_Vec&                          theDUV)
{
  theSurface->D2(theU, theV, thePoint, theD1U, theD1V, theD2U, theD2V, theDUV);
}

void surfaceBounds(const occ::handle<Geom_Surface>& theSurface,
                   double&                          theU1,
                   double&                          theU2,
                   double&                          theV1,
                   double&                          theV2)
{
  theSurface->Bounds(theU1, theU2, theV1, theV2);
}

std::string surfaceGeometry(const occ::handle<Geom_Surface>& theSurface)
{
  return theSurface.IsNull() ? "null" : std::to_string((int)GeomProp_Surface(theSurface).GetType());
}

std::string surfaceParameters(const double theU, const double theV)
{
  return "U=" + LProp_CompareDebug::ToString(theU) + ",V=" + LProp_CompareDebug::ToString(theV);
}
} // namespace

//==================================================================================================

GeomLProp_SLProps::GeomLProp_SLProps(const occ::handle<Geom_Surface>& S,
                                     const double                     U,
                                     const double                     V,
                                     const int                        N,
                                     const double                     Resolution)
    : mySurf(S),
      myLegacyProps(S.IsNull() ? nullptr
                               : std::make_shared<GeomLProp_LegacySLProps>(S, N, Resolution)),
      myDerOrder(N),
      myCN(4),
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
  Standard_OutOfRange_Raise_if(N < 0 || N > 2, "GeomLProp_SLProps::GeomLProp_SLProps()");
  SetParameters(U, V);
}

//==================================================================================================

GeomLProp_SLProps::GeomLProp_SLProps(const occ::handle<Geom_Surface>& S,
                                     const int                        N,
                                     const double                     Resolution)
    : mySurf(S),
      myLegacyProps(S.IsNull() ? nullptr
                               : std::make_shared<GeomLProp_LegacySLProps>(S, N, Resolution)),
      myU(RealLast()),
      myV(RealLast()),
      myDerOrder(N),
      myCN(4),
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
  Standard_OutOfRange_Raise_if(N < 0 || N > 2, "GeomLProp_SLProps::GeomLProp_SLProps()");
}

//==================================================================================================

GeomLProp_SLProps::GeomLProp_SLProps(const int N, const double Resolution)
    : myLegacyProps(std::make_shared<GeomLProp_LegacySLProps>(N, Resolution)),
      myU(RealLast()),
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
  Standard_OutOfRange_Raise_if(N < 0 || N > 2, "GeomLProp_SLProps::GeomLProp_SLProps()");
}

//==================================================================================================

GeomLProp_SLProps::~GeomLProp_SLProps() = default;

//==================================================================================================

GeomLProp_SLProps::GeomLProp_SLProps(const GeomLProp_SLProps& theOther)
    : mySurf(theOther.mySurf),
      myLegacyProps(theOther.myLegacyProps != nullptr
                      ? std::make_shared<GeomLProp_LegacySLProps>(*theOther.myLegacyProps)
                      : nullptr),
      myU(theOther.myU),
      myV(theOther.myV),
      myDerOrder(theOther.myDerOrder),
      myCN(theOther.myCN),
      myLinTol(theOther.myLinTol),
      myPnt(theOther.myPnt),
      myD1u(theOther.myD1u),
      myD1v(theOther.myD1v),
      myD2u(theOther.myD2u),
      myD2v(theOther.myD2v),
      myDuv(theOther.myDuv),
      myNormal(theOther.myNormal),
      myMinCurv(theOther.myMinCurv),
      myMaxCurv(theOther.myMaxCurv),
      myDirMinCurv(theOther.myDirMinCurv),
      myDirMaxCurv(theOther.myDirMaxCurv),
      myMeanCurv(theOther.myMeanCurv),
      myGausCurv(theOther.myGausCurv),
      mySignificantFirstDerivativeOrderU(theOther.mySignificantFirstDerivativeOrderU),
      mySignificantFirstDerivativeOrderV(theOther.mySignificantFirstDerivativeOrderV),
      myUTangentStatus(theOther.myUTangentStatus),
      myVTangentStatus(theOther.myVTangentStatus),
      myNormalStatus(theOther.myNormalStatus),
      myCurvatureStatus(theOther.myCurvatureStatus)
{
}

//==================================================================================================

GeomLProp_SLProps& GeomLProp_SLProps::operator=(const GeomLProp_SLProps& theOther)
{
  if (this == &theOther)
  {
    return *this;
  }

  mySurf = theOther.mySurf;
  myLegacyProps = theOther.myLegacyProps != nullptr
                    ? std::make_shared<GeomLProp_LegacySLProps>(*theOther.myLegacyProps)
                    : nullptr;
  myU                                  = theOther.myU;
  myV                                  = theOther.myV;
  myDerOrder                           = theOther.myDerOrder;
  myCN                                 = theOther.myCN;
  myLinTol                             = theOther.myLinTol;
  myPnt                                = theOther.myPnt;
  myD1u                                = theOther.myD1u;
  myD1v                                = theOther.myD1v;
  myD2u                                = theOther.myD2u;
  myD2v                                = theOther.myD2v;
  myDuv                                = theOther.myDuv;
  myNormal                             = theOther.myNormal;
  myMinCurv                            = theOther.myMinCurv;
  myMaxCurv                            = theOther.myMaxCurv;
  myDirMinCurv                         = theOther.myDirMinCurv;
  myDirMaxCurv                         = theOther.myDirMaxCurv;
  myMeanCurv                           = theOther.myMeanCurv;
  myGausCurv                           = theOther.myGausCurv;
  mySignificantFirstDerivativeOrderU   = theOther.mySignificantFirstDerivativeOrderU;
  mySignificantFirstDerivativeOrderV   = theOther.mySignificantFirstDerivativeOrderV;
  myUTangentStatus                     = theOther.myUTangentStatus;
  myVTangentStatus                     = theOther.myVTangentStatus;
  myNormalStatus                       = theOther.myNormalStatus;
  myCurvatureStatus                    = theOther.myCurvatureStatus;
  return *this;
}

//==================================================================================================

void GeomLProp_SLProps::SetSurface(const occ::handle<Geom_Surface>& S)
{
  mySurf = S;
  if (myLegacyProps == nullptr)
  {
    myLegacyProps = std::make_shared<GeomLProp_LegacySLProps>(myDerOrder, myLinTol);
  }
  myLegacyProps->SetSurface(S);
  myCN                               = 4;
  mySignificantFirstDerivativeOrderU = 0;
  mySignificantFirstDerivativeOrderV = 0;
  myUTangentStatus                   = LProp_Undecided;
  myVTangentStatus                   = LProp_Undecided;
  myNormalStatus                     = LProp_Undecided;
  myCurvatureStatus                  = LProp_Undecided;
}

//==================================================================================================

void GeomLProp_SLProps::SetParameters(const double U, const double V)
{
  myU = U;
  myV = V;
  switch (myDerOrder)
  {
    case 0:
      myPnt = surfaceValue(mySurf, myU, myV);
      break;
    case 1:
      surfaceD1(mySurf, myU, myV, myPnt, myD1u, myD1v);
      break;
    case 2:
      surfaceD2(mySurf, myU, myV, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv);
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
  if (myLegacyProps != nullptr)
  {
    myLegacyProps->SetParameters(U, V);
  }
}

//==================================================================================================

const gp_Pnt& GeomLProp_SLProps::Value() const
{
  if (myLegacyProps != nullptr && !myPnt.IsEqual(myLegacyProps->Value(), myLinTol))
  {
    LProp_CompareDebug::LogValueMismatch("GeomLProp_SLProps::Value",
                                         surfaceGeometry(mySurf),
                                         surfaceParameters(myU, myV),
                                         myDerOrder,
                                         myLinTol,
                                         "point",
                                         myPnt,
                                         myLegacyProps->Value());
  }
  return myPnt;
}

//==================================================================================================

const gp_Vec& GeomLProp_SLProps::D1U()
{
  if (myDerOrder < 1)
  {
    myDerOrder = 1;
    surfaceD1(mySurf, myU, myV, myPnt, myD1u, myD1v);
  }

  if (myLegacyProps != nullptr && !myD1u.IsEqual(myLegacyProps->D1U(), myLinTol, myLinTol))
  {
    LProp_CompareDebug::LogValueMismatch("GeomLProp_SLProps::D1U",
                                         surfaceGeometry(mySurf),
                                         surfaceParameters(myU, myV),
                                         myDerOrder,
                                         myLinTol,
                                         "vec",
                                         myD1u,
                                         myLegacyProps->D1U());
  }

  return myD1u;
}

//==================================================================================================

const gp_Vec& GeomLProp_SLProps::D1V()
{
  if (myDerOrder < 1)
  {
    myDerOrder = 1;
    surfaceD1(mySurf, myU, myV, myPnt, myD1u, myD1v);
  }

  if (myLegacyProps != nullptr && !myD1v.IsEqual(myLegacyProps->D1V(), myLinTol, myLinTol))
  {
    LProp_CompareDebug::LogValueMismatch("GeomLProp_SLProps::D1V",
                                         surfaceGeometry(mySurf),
                                         surfaceParameters(myU, myV),
                                         myDerOrder,
                                         myLinTol,
                                         "vec",
                                         myD1v,
                                         myLegacyProps->D1V());
  }

  return myD1v;
}

//==================================================================================================

const gp_Vec& GeomLProp_SLProps::D2U()
{
  if (myDerOrder < 2)
  {
    myDerOrder = 2;
    surfaceD2(mySurf, myU, myV, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv);
  }

  if (myLegacyProps != nullptr && !myD2u.IsEqual(myLegacyProps->D2U(), myLinTol, myLinTol))
  {
    LProp_CompareDebug::LogValueMismatch("GeomLProp_SLProps::D2U",
                                         surfaceGeometry(mySurf),
                                         surfaceParameters(myU, myV),
                                         myDerOrder,
                                         myLinTol,
                                         "vec",
                                         myD2u,
                                         myLegacyProps->D2U());
  }

  return myD2u;
}

//==================================================================================================

const gp_Vec& GeomLProp_SLProps::D2V()
{
  if (myDerOrder < 2)
  {
    myDerOrder = 2;
    surfaceD2(mySurf, myU, myV, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv);
  }

  if (myLegacyProps != nullptr && !myD2v.IsEqual(myLegacyProps->D2V(), myLinTol, myLinTol))
  {
    LProp_CompareDebug::LogValueMismatch("GeomLProp_SLProps::D2V",
                                         surfaceGeometry(mySurf),
                                         surfaceParameters(myU, myV),
                                         myDerOrder,
                                         myLinTol,
                                         "vec",
                                         myD2v,
                                         myLegacyProps->D2V());
  }

  return myD2v;
}

//==================================================================================================

const gp_Vec& GeomLProp_SLProps::DUV()
{
  if (myDerOrder < 2)
  {
    myDerOrder = 2;
    surfaceD2(mySurf, myU, myV, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv);
  }

  if (myLegacyProps != nullptr && !myDuv.IsEqual(myLegacyProps->DUV(), myLinTol, myLinTol))
  {
    LProp_CompareDebug::LogValueMismatch("GeomLProp_SLProps::DUV",
                                         surfaceGeometry(mySurf),
                                         surfaceParameters(myU, myV),
                                         myDerOrder,
                                         myLinTol,
                                         "vec",
                                         myDuv,
                                         myLegacyProps->DUV());
  }

  return myDuv;
}

//==================================================================================================

bool GeomLProp_SLProps::IsTangentUDefined()
{
  const bool isDefined = LProp_SLPropsCompat::IsTangentDefined(myCN,
                                                               myLinTol,
                                                               D1U(),
                                                               D1V(),
                                                               D2U(),
                                                               D2V(),
                                                               0,
                                                               mySignificantFirstDerivativeOrderU,
                                                               myUTangentStatus);
  const bool anOldDefined = myLegacyProps != nullptr ? myLegacyProps->IsTangentUDefined() : isDefined;
  if (myLegacyProps != nullptr
      && (isDefined != anOldDefined || myUTangentStatus != myLegacyProps->UTangentStatus()))
  {
    LProp_CompareDebug::LogMismatch("GeomLProp_SLProps::IsTangentUDefined",
                                    surfaceGeometry(mySurf),
                                    surfaceParameters(myU, myV),
                                    myDerOrder,
                                    myLinTol,
                                    std::string("new=") + LProp_CompareDebug::ToString(isDefined)
                                      + " old=" + LProp_CompareDebug::ToString(anOldDefined)
                                      + " newStatus=" + LProp_CompareDebug::ToString(myUTangentStatus)
                                      + " oldStatus="
                                      + LProp_CompareDebug::ToString(myLegacyProps->UTangentStatus()));
  }
  return isDefined;
}

//==================================================================================================

void GeomLProp_SLProps::TangentU(gp_Dir& D)
{
  LProp_NotDefined_Raise_if(!IsTangentUDefined(), "GeomLProp_SLProps::TangentU()");
  double aU1, aV1, aU2, aV2;
  surfaceBounds(mySurf, aU1, aU2, aV1, aV2);
  LProp_SLPropsCompat::Tangent(
    mySignificantFirstDerivativeOrderU,
    myLinTol,
    myU,
    aU1,
    aU2,
    [&]() { return gp_Dir(D1U()); },
    [&](const gp_Pnt& thePntBefore, const gp_Pnt& thePntAfter) {
      return GeomProp::ComputeTangent(D1U(),
                                      D2U(),
                                      gp_Vec(0.0, 0.0, 0.0),
                                      myLinTol,
                                      thePntBefore,
                                      thePntAfter);
    },
    [&](const double theParam) { return surfaceValue(mySurf, theParam, myV); },
    D,
    "GeomLProp_SLProps::TangentU()");
  if (myLegacyProps != nullptr)
  {
    gp_Dir anOldDir;
    try
    {
      myLegacyProps->TangentU(anOldDir);
      if (!D.IsEqual(anOldDir, Precision::Angular()))
      {
        LProp_CompareDebug::LogValueMismatch("GeomLProp_SLProps::TangentU",
                                             surfaceGeometry(mySurf),
                                             surfaceParameters(myU, myV),
                                             myDerOrder,
                                             myLinTol,
                                             "dir",
                                             D,
                                             anOldDir);
      }
    }
    catch (const Standard_Failure& theFailure)
    {
      LProp_CompareDebug::LogExceptionMismatch("GeomLProp_SLProps::TangentU",
                                               surfaceGeometry(mySurf),
                                               surfaceParameters(myU, myV),
                                               myDerOrder,
                                               myLinTol,
                                               "value",
                                               LProp_CompareDebug::ExceptionSummary(theFailure));
    }
  }
}

//==================================================================================================

bool GeomLProp_SLProps::IsTangentVDefined()
{
  const bool isDefined = LProp_SLPropsCompat::IsTangentDefined(myCN,
                                                               myLinTol,
                                                               D1U(),
                                                               D1V(),
                                                               D2U(),
                                                               D2V(),
                                                               1,
                                                               mySignificantFirstDerivativeOrderV,
                                                               myVTangentStatus);
  const bool anOldDefined = myLegacyProps != nullptr ? myLegacyProps->IsTangentVDefined() : isDefined;
  if (myLegacyProps != nullptr
      && (isDefined != anOldDefined || myVTangentStatus != myLegacyProps->VTangentStatus()))
  {
    LProp_CompareDebug::LogMismatch("GeomLProp_SLProps::IsTangentVDefined",
                                    surfaceGeometry(mySurf),
                                    surfaceParameters(myU, myV),
                                    myDerOrder,
                                    myLinTol,
                                    std::string("new=") + LProp_CompareDebug::ToString(isDefined)
                                      + " old=" + LProp_CompareDebug::ToString(anOldDefined)
                                      + " newStatus=" + LProp_CompareDebug::ToString(myVTangentStatus)
                                      + " oldStatus="
                                      + LProp_CompareDebug::ToString(myLegacyProps->VTangentStatus()));
  }
  return isDefined;
}

//==================================================================================================

void GeomLProp_SLProps::TangentV(gp_Dir& D)
{
  LProp_NotDefined_Raise_if(!IsTangentVDefined(), "GeomLProp_SLProps::TangentV()");
  double aU1, aV1, aU2, aV2;
  surfaceBounds(mySurf, aU1, aU2, aV1, aV2);
  LProp_SLPropsCompat::Tangent(
    mySignificantFirstDerivativeOrderV,
    myLinTol,
    myV,
    aV1,
    aV2,
    [&]() { return gp_Dir(D1V()); },
    [&](const gp_Pnt& thePntBefore, const gp_Pnt& thePntAfter) {
      return GeomProp::ComputeTangent(D1V(),
                                      D2V(),
                                      gp_Vec(0.0, 0.0, 0.0),
                                      myLinTol,
                                      thePntBefore,
                                      thePntAfter);
    },
    [&](const double theParam) { return surfaceValue(mySurf, myU, theParam); },
    D,
    "GeomLProp_SLProps::TangentV()");
  if (myLegacyProps != nullptr)
  {
    gp_Dir anOldDir;
    try
    {
      myLegacyProps->TangentV(anOldDir);
      if (!D.IsEqual(anOldDir, Precision::Angular()))
      {
        LProp_CompareDebug::LogValueMismatch("GeomLProp_SLProps::TangentV",
                                             surfaceGeometry(mySurf),
                                             surfaceParameters(myU, myV),
                                             myDerOrder,
                                             myLinTol,
                                             "dir",
                                             D,
                                             anOldDir);
      }
    }
    catch (const Standard_Failure& theFailure)
    {
      LProp_CompareDebug::LogExceptionMismatch("GeomLProp_SLProps::TangentV",
                                               surfaceGeometry(mySurf),
                                               surfaceParameters(myU, myV),
                                               myDerOrder,
                                               myLinTol,
                                               "value",
                                               LProp_CompareDebug::ExceptionSummary(theFailure));
    }
  }
}

//==================================================================================================

bool GeomLProp_SLProps::IsNormalDefined()
{
  const bool isDefined = LProp_SLPropsCompat::IsNormalDefined<GeomProp::SurfaceNormalResult>(
    [&]() { return GeomProp::ComputeSurfaceNormal(D1U(), D1V(), myLinTol); },
    myNormal,
    myNormalStatus);
  const bool anOldDefined = myLegacyProps != nullptr ? myLegacyProps->IsNormalDefined() : isDefined;
  if (myLegacyProps != nullptr
      && (isDefined != anOldDefined || myNormalStatus != myLegacyProps->NormalStatus()))
  {
    LProp_CompareDebug::LogMismatch("GeomLProp_SLProps::IsNormalDefined",
                                    surfaceGeometry(mySurf),
                                    surfaceParameters(myU, myV),
                                    myDerOrder,
                                    myLinTol,
                                    std::string("new=") + LProp_CompareDebug::ToString(isDefined)
                                      + " old=" + LProp_CompareDebug::ToString(anOldDefined)
                                      + " newStatus=" + LProp_CompareDebug::ToString(myNormalStatus)
                                      + " oldStatus="
                                      + LProp_CompareDebug::ToString(myLegacyProps->NormalStatus()));
  }
  return isDefined;
}

//==================================================================================================

const gp_Dir& GeomLProp_SLProps::Normal()
{
  if (!IsNormalDefined())
  {
    throw LProp_NotDefined();
  }

  if (myLegacyProps != nullptr)
  {
    try
    {
      const gp_Dir& anOldNormal = myLegacyProps->Normal();
      if (!myNormal.IsEqual(anOldNormal, Precision::Angular()))
      {
        LProp_CompareDebug::LogValueMismatch("GeomLProp_SLProps::Normal",
                                             surfaceGeometry(mySurf),
                                             surfaceParameters(myU, myV),
                                             myDerOrder,
                                             myLinTol,
                                             "dir",
                                             myNormal,
                                             anOldNormal);
      }
    }
    catch (const Standard_Failure& theFailure)
    {
      LProp_CompareDebug::LogExceptionMismatch("GeomLProp_SLProps::Normal",
                                               surfaceGeometry(mySurf),
                                               surfaceParameters(myU, myV),
                                               myDerOrder,
                                               myLinTol,
                                               "value",
                                               LProp_CompareDebug::ExceptionSummary(theFailure));
    }
  }

  return myNormal;
}

//==================================================================================================

bool GeomLProp_SLProps::IsCurvatureDefined()
{
  const bool isDefined = LProp_SLPropsCompat::IsCurvatureDefined<GeomProp::SurfaceCurvatureResult,
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
  const bool anOldDefined = myLegacyProps != nullptr ? myLegacyProps->IsCurvatureDefined() : isDefined;
  if (myLegacyProps != nullptr
      && (isDefined != anOldDefined || myCurvatureStatus != myLegacyProps->CurvatureStatus()))
  {
    LProp_CompareDebug::LogMismatch("GeomLProp_SLProps::IsCurvatureDefined",
                                    surfaceGeometry(mySurf),
                                    surfaceParameters(myU, myV),
                                    myDerOrder,
                                    myLinTol,
                                    std::string("new=") + LProp_CompareDebug::ToString(isDefined)
                                      + " old=" + LProp_CompareDebug::ToString(anOldDefined)
                                      + " newStatus=" + LProp_CompareDebug::ToString(myCurvatureStatus)
                                      + " oldStatus="
                                      + LProp_CompareDebug::ToString(myLegacyProps->CurvatureStatus()));
  }
  return isDefined;
}

//==================================================================================================

bool GeomLProp_SLProps::IsUmbilic()
{
  if (!IsCurvatureDefined())
  {
    throw LProp_NotDefined();
  }
  const bool isUmbilic = std::abs(myMaxCurv - myMinCurv) < std::abs(Epsilon(myMaxCurv));
  if (myLegacyProps != nullptr)
  {
    try
    {
      const bool anOldUmbilic = myLegacyProps->IsUmbilic();
      if (isUmbilic != anOldUmbilic)
      {
        LProp_CompareDebug::LogValueMismatch("GeomLProp_SLProps::IsUmbilic",
                                             surfaceGeometry(mySurf),
                                             surfaceParameters(myU, myV),
                                             myDerOrder,
                                             myLinTol,
                                             "value",
                                             isUmbilic,
                                             anOldUmbilic);
      }
    }
    catch (const Standard_Failure& theFailure)
    {
      LProp_CompareDebug::LogExceptionMismatch("GeomLProp_SLProps::IsUmbilic",
                                               surfaceGeometry(mySurf),
                                               surfaceParameters(myU, myV),
                                               myDerOrder,
                                               myLinTol,
                                               "value",
                                               LProp_CompareDebug::ExceptionSummary(theFailure));
    }
  }
  return isUmbilic;
}

//==================================================================================================

double GeomLProp_SLProps::MaxCurvature()
{
  if (!IsCurvatureDefined())
  {
    throw LProp_NotDefined();
  }
  if (myLegacyProps != nullptr)
  {
    try
    {
      const double anOldValue = myLegacyProps->MaxCurvature();
      if (std::abs(myMaxCurv - anOldValue) > myLinTol)
      {
        LProp_CompareDebug::LogValueMismatch("GeomLProp_SLProps::MaxCurvature",
                                             surfaceGeometry(mySurf),
                                             surfaceParameters(myU, myV),
                                             myDerOrder,
                                             myLinTol,
                                             "value",
                                             myMaxCurv,
                                             anOldValue);
      }
    }
    catch (const Standard_Failure& theFailure)
    {
      LProp_CompareDebug::LogExceptionMismatch("GeomLProp_SLProps::MaxCurvature",
                                               surfaceGeometry(mySurf),
                                               surfaceParameters(myU, myV),
                                               myDerOrder,
                                               myLinTol,
                                               "value",
                                               LProp_CompareDebug::ExceptionSummary(theFailure));
    }
  }
  return myMaxCurv;
}

//==================================================================================================

double GeomLProp_SLProps::MinCurvature()
{
  if (!IsCurvatureDefined())
  {
    throw LProp_NotDefined();
  }
  if (myLegacyProps != nullptr)
  {
    try
    {
      const double anOldValue = myLegacyProps->MinCurvature();
      if (std::abs(myMinCurv - anOldValue) > myLinTol)
      {
        LProp_CompareDebug::LogValueMismatch("GeomLProp_SLProps::MinCurvature",
                                             surfaceGeometry(mySurf),
                                             surfaceParameters(myU, myV),
                                             myDerOrder,
                                             myLinTol,
                                             "value",
                                             myMinCurv,
                                             anOldValue);
      }
    }
    catch (const Standard_Failure& theFailure)
    {
      LProp_CompareDebug::LogExceptionMismatch("GeomLProp_SLProps::MinCurvature",
                                               surfaceGeometry(mySurf),
                                               surfaceParameters(myU, myV),
                                               myDerOrder,
                                               myLinTol,
                                               "value",
                                               LProp_CompareDebug::ExceptionSummary(theFailure));
    }
  }
  return myMinCurv;
}

//==================================================================================================

void GeomLProp_SLProps::CurvatureDirections(gp_Dir& MaxD, gp_Dir& MinD)
{
  if (!IsCurvatureDefined())
  {
    throw LProp_NotDefined();
  }

  MaxD = myDirMaxCurv;
  MinD = myDirMinCurv;
  if (myLegacyProps != nullptr)
  {
    gp_Dir anOldMax;
    gp_Dir anOldMin;
    try
    {
      myLegacyProps->CurvatureDirections(anOldMax, anOldMin);
      if (!MaxD.IsEqual(anOldMax, Precision::Angular()))
      {
        LProp_CompareDebug::LogValueMismatch("GeomLProp_SLProps::CurvatureDirections",
                                             surfaceGeometry(mySurf),
                                             surfaceParameters(myU, myV),
                                             myDerOrder,
                                             myLinTol,
                                             "maxDir",
                                             MaxD,
                                             anOldMax);
      }
      if (!MinD.IsEqual(anOldMin, Precision::Angular()))
      {
        LProp_CompareDebug::LogValueMismatch("GeomLProp_SLProps::CurvatureDirections",
                                             surfaceGeometry(mySurf),
                                             surfaceParameters(myU, myV),
                                             myDerOrder,
                                             myLinTol,
                                             "minDir",
                                             MinD,
                                             anOldMin);
      }
    }
    catch (const Standard_Failure& theFailure)
    {
      LProp_CompareDebug::LogExceptionMismatch("GeomLProp_SLProps::CurvatureDirections",
                                               surfaceGeometry(mySurf),
                                               surfaceParameters(myU, myV),
                                               myDerOrder,
                                               myLinTol,
                                               "value",
                                               LProp_CompareDebug::ExceptionSummary(theFailure));
    }
  }
}

//==================================================================================================

double GeomLProp_SLProps::MeanCurvature()
{
  if (!IsCurvatureDefined())
  {
    throw LProp_NotDefined();
  }
  if (myLegacyProps != nullptr)
  {
    try
    {
      const double anOldValue = myLegacyProps->MeanCurvature();
      if (std::abs(myMeanCurv - anOldValue) > myLinTol)
      {
        LProp_CompareDebug::LogValueMismatch("GeomLProp_SLProps::MeanCurvature",
                                             surfaceGeometry(mySurf),
                                             surfaceParameters(myU, myV),
                                             myDerOrder,
                                             myLinTol,
                                             "value",
                                             myMeanCurv,
                                             anOldValue);
      }
    }
    catch (const Standard_Failure& theFailure)
    {
      LProp_CompareDebug::LogExceptionMismatch("GeomLProp_SLProps::MeanCurvature",
                                               surfaceGeometry(mySurf),
                                               surfaceParameters(myU, myV),
                                               myDerOrder,
                                               myLinTol,
                                               "value",
                                               LProp_CompareDebug::ExceptionSummary(theFailure));
    }
  }
  return myMeanCurv;
}

//==================================================================================================

double GeomLProp_SLProps::GaussianCurvature()
{
  if (!IsCurvatureDefined())
  {
    throw LProp_NotDefined();
  }
  if (myLegacyProps != nullptr)
  {
    try
    {
      const double anOldValue = myLegacyProps->GaussianCurvature();
      if (std::abs(myGausCurv - anOldValue) > myLinTol)
      {
        LProp_CompareDebug::LogValueMismatch("GeomLProp_SLProps::GaussianCurvature",
                                             surfaceGeometry(mySurf),
                                             surfaceParameters(myU, myV),
                                             myDerOrder,
                                             myLinTol,
                                             "value",
                                             myGausCurv,
                                             anOldValue);
      }
    }
    catch (const Standard_Failure& theFailure)
    {
      LProp_CompareDebug::LogExceptionMismatch("GeomLProp_SLProps::GaussianCurvature",
                                               surfaceGeometry(mySurf),
                                               surfaceParameters(myU, myV),
                                               myDerOrder,
                                               myLinTol,
                                               "value",
                                               LProp_CompareDebug::ExceptionSummary(theFailure));
    }
  }
  return myGausCurv;
}
