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

#include <LProp3d_SLProps.hxx>

#include <GeomProp.hxx>
#include <GeomProp_Surface.hxx>
#include <LProp3d_LegacySLProps.hxx>
#include <LProp_SLPropsCompat.pxx>
#include <LProp_CompareDebug.pxx>
#include <LProp_NotDefined.hxx>
#include <LProp_Status.hxx>
#include <Precision.hxx>
#include <Standard_OutOfRange.hxx>

namespace
{
bool hasGeomPropEvaluator(const std::shared_ptr<GeomProp_Surface>& theSurfaceProp)
{
  return theSurfaceProp && theSurfaceProp->Adaptor() != nullptr;
}

std::string surfaceGeometry(const std::shared_ptr<GeomProp_Surface>& theSurfaceProp)
{
  return theSurfaceProp != nullptr ? std::to_string((int)theSurfaceProp->GetType()) : "null";
}

std::string surfaceParameters(const double theU, const double theV)
{
  return "U=" + LProp_CompareDebug::ToString(theU) + ",V=" + LProp_CompareDebug::ToString(theV);
}
} // namespace

//==================================================================================================

LProp3d_SLProps::LProp3d_SLProps(const occ::handle<Adaptor3d_Surface>& S,
                                 const double                          U,
                                 const double                          V,
                                 const int                             N,
                                 const double                          Resolution)
    : mySurf(S),
      mySurfaceProp(S.IsNull() ? nullptr : std::make_shared<GeomProp_Surface>(*S)),
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
  Standard_OutOfRange_Raise_if(N < 0 || N > 2, "LProp3d_SLProps::LProp3d_SLProps()");
  SetParameters(U, V);
}

//==================================================================================================

LProp3d_SLProps::LProp3d_SLProps(const occ::handle<Adaptor3d_Surface>& S,
                                 const int                             N,
                                 const double                          Resolution)
    : mySurf(S),
      mySurfaceProp(S.IsNull() ? nullptr : std::make_shared<GeomProp_Surface>(*S)),
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
  Standard_OutOfRange_Raise_if(N < 0 || N > 2, "LProp3d_SLProps::LProp3d_SLProps()");
}

//==================================================================================================

LProp3d_SLProps::LProp3d_SLProps(const int N, const double Resolution)
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
  Standard_OutOfRange_Raise_if(N < 0 || N > 2, "LProp3d_SLProps::LProp3d_SLProps()");
}

//==================================================================================================

LProp3d_SLProps::~LProp3d_SLProps() = default;

//==================================================================================================

void LProp3d_SLProps::SetSurface(const occ::handle<Adaptor3d_Surface>& S)
{
  mySurf        = S;
  mySurfaceProp = S.IsNull() ? nullptr : std::make_shared<GeomProp_Surface>(*S);
  myCN          = 4;
  mySignificantFirstDerivativeOrderU = 0;
  mySignificantFirstDerivativeOrderV = 0;
  myUTangentStatus                   = LProp_Undecided;
  myVTangentStatus                   = LProp_Undecided;
  myNormalStatus                     = LProp_Undecided;
  myCurvatureStatus                  = LProp_Undecided;
}

//==================================================================================================

void LProp3d_SLProps::SetParameters(const double U, const double V)
{
  myU = U;
  myV = V;
  switch (myDerOrder)
  {
    case 0:
      myPnt = mySurf->Value(myU, myV);
      break;
    case 1:
      mySurf->D1(myU, myV, myPnt, myD1u, myD1v);
      break;
    case 2:
      mySurf->D2(myU, myV, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv);
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

const gp_Pnt& LProp3d_SLProps::Value() const
{
  LProp3d_LegacySLProps aLegacy(mySurf, myU, myV, myDerOrder, myLinTol);
  if (!myPnt.IsEqual(aLegacy.Value(), myLinTol))
  {
    LProp_CompareDebug::LogValueMismatch("LProp3d_SLProps::Value",
                                         surfaceGeometry(mySurfaceProp),
                                         surfaceParameters(myU, myV),
                                         myDerOrder,
                                         myLinTol,
                                         "point",
                                         myPnt,
                                         aLegacy.Value());
  }
  return myPnt;
}

//==================================================================================================

const gp_Vec& LProp3d_SLProps::D1U()
{
  if (myDerOrder < 1)
  {
    myDerOrder = 1;
    mySurf->D1(myU, myV, myPnt, myD1u, myD1v);
  }

  LProp3d_LegacySLProps aLegacy(mySurf, myU, myV, myDerOrder, myLinTol);
  if (!myD1u.IsEqual(aLegacy.D1U(), myLinTol, myLinTol))
  {
    LProp_CompareDebug::LogValueMismatch("LProp3d_SLProps::D1U",
                                         surfaceGeometry(mySurfaceProp),
                                         surfaceParameters(myU, myV),
                                         myDerOrder,
                                         myLinTol,
                                         "vec",
                                         myD1u,
                                         aLegacy.D1U());
  }

  return myD1u;
}

//==================================================================================================

const gp_Vec& LProp3d_SLProps::D1V()
{
  if (myDerOrder < 1)
  {
    myDerOrder = 1;
    mySurf->D1(myU, myV, myPnt, myD1u, myD1v);
  }

  LProp3d_LegacySLProps aLegacy(mySurf, myU, myV, myDerOrder, myLinTol);
  if (!myD1v.IsEqual(aLegacy.D1V(), myLinTol, myLinTol))
  {
    LProp_CompareDebug::LogValueMismatch("LProp3d_SLProps::D1V",
                                         surfaceGeometry(mySurfaceProp),
                                         surfaceParameters(myU, myV),
                                         myDerOrder,
                                         myLinTol,
                                         "vec",
                                         myD1v,
                                         aLegacy.D1V());
  }

  return myD1v;
}

//==================================================================================================

const gp_Vec& LProp3d_SLProps::D2U()
{
  if (myDerOrder < 2)
  {
    myDerOrder = 2;
    mySurf->D2(myU, myV, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv);
  }

  LProp3d_LegacySLProps aLegacy(mySurf, myU, myV, myDerOrder, myLinTol);
  if (!myD2u.IsEqual(aLegacy.D2U(), myLinTol, myLinTol))
  {
    LProp_CompareDebug::LogValueMismatch("LProp3d_SLProps::D2U",
                                         surfaceGeometry(mySurfaceProp),
                                         surfaceParameters(myU, myV),
                                         myDerOrder,
                                         myLinTol,
                                         "vec",
                                         myD2u,
                                         aLegacy.D2U());
  }

  return myD2u;
}

//==================================================================================================

const gp_Vec& LProp3d_SLProps::D2V()
{
  if (myDerOrder < 2)
  {
    myDerOrder = 2;
    mySurf->D2(myU, myV, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv);
  }

  LProp3d_LegacySLProps aLegacy(mySurf, myU, myV, myDerOrder, myLinTol);
  if (!myD2v.IsEqual(aLegacy.D2V(), myLinTol, myLinTol))
  {
    LProp_CompareDebug::LogValueMismatch("LProp3d_SLProps::D2V",
                                         surfaceGeometry(mySurfaceProp),
                                         surfaceParameters(myU, myV),
                                         myDerOrder,
                                         myLinTol,
                                         "vec",
                                         myD2v,
                                         aLegacy.D2V());
  }

  return myD2v;
}

//==================================================================================================

const gp_Vec& LProp3d_SLProps::DUV()
{
  if (myDerOrder < 2)
  {
    myDerOrder = 2;
    mySurf->D2(myU, myV, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv);
  }

  LProp3d_LegacySLProps aLegacy(mySurf, myU, myV, myDerOrder, myLinTol);
  if (!myDuv.IsEqual(aLegacy.DUV(), myLinTol, myLinTol))
  {
    LProp_CompareDebug::LogValueMismatch("LProp3d_SLProps::DUV",
                                         surfaceGeometry(mySurfaceProp),
                                         surfaceParameters(myU, myV),
                                         myDerOrder,
                                         myLinTol,
                                         "vec",
                                         myDuv,
                                         aLegacy.DUV());
  }

  return myDuv;
}

//==================================================================================================

bool LProp3d_SLProps::IsTangentUDefined()
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
  LProp3d_LegacySLProps aLegacy(mySurf, myU, myV, myDerOrder, myLinTol);
  const bool            anOldDefined = aLegacy.IsTangentUDefined();
  if (isDefined != anOldDefined || myUTangentStatus != aLegacy.UTangentStatus())
  {
    LProp_CompareDebug::LogMismatch("LProp3d_SLProps::IsTangentUDefined",
                                    surfaceGeometry(mySurfaceProp),
                                    surfaceParameters(myU, myV),
                                    myDerOrder,
                                    myLinTol,
                                    std::string("new=") + LProp_CompareDebug::ToString(isDefined)
                                      + " old=" + LProp_CompareDebug::ToString(anOldDefined)
                                      + " newStatus=" + LProp_CompareDebug::ToString(myUTangentStatus)
                                      + " oldStatus="
                                      + LProp_CompareDebug::ToString(aLegacy.UTangentStatus()));
  }
  return isDefined;
}

//==================================================================================================

void LProp3d_SLProps::TangentU(gp_Dir& D)
{
  LProp_NotDefined_Raise_if(!IsTangentUDefined(), "LProp3d_SLProps::TangentU()");
  LProp_SLPropsCompat::Tangent(
    mySignificantFirstDerivativeOrderU,
    myLinTol,
    myU,
    mySurf->FirstUParameter(),
    mySurf->LastUParameter(),
    [&]() { return gp_Dir(D1U()); },
    [&](const gp_Pnt& thePntBefore, const gp_Pnt& thePntAfter) {
      return GeomProp::ComputeTangent(D1U(),
                                      D2U(),
                                      gp_Vec(0.0, 0.0, 0.0),
                                      myLinTol,
                                      thePntBefore,
                                      thePntAfter);
    },
    [&](const double theParam) { return mySurf->Value(theParam, myV); },
    D,
    "LProp3d_SLProps::TangentU()");
  LProp3d_LegacySLProps aLegacy(mySurf, myU, myV, myDerOrder, myLinTol);
  gp_Dir                anOldDir;
  try
  {
    aLegacy.TangentU(anOldDir);
    if (!D.IsEqual(anOldDir, Precision::Angular()))
    {
      LProp_CompareDebug::LogValueMismatch("LProp3d_SLProps::TangentU",
                                           surfaceGeometry(mySurfaceProp),
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
    LProp_CompareDebug::LogExceptionMismatch("LProp3d_SLProps::TangentU",
                                             surfaceGeometry(mySurfaceProp),
                                             surfaceParameters(myU, myV),
                                             myDerOrder,
                                             myLinTol,
                                             "value",
                                             LProp_CompareDebug::ExceptionSummary(theFailure));
  }
}

//==================================================================================================

bool LProp3d_SLProps::IsTangentVDefined()
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
  LProp3d_LegacySLProps aLegacy(mySurf, myU, myV, myDerOrder, myLinTol);
  const bool            anOldDefined = aLegacy.IsTangentVDefined();
  if (isDefined != anOldDefined || myVTangentStatus != aLegacy.VTangentStatus())
  {
    LProp_CompareDebug::LogMismatch("LProp3d_SLProps::IsTangentVDefined",
                                    surfaceGeometry(mySurfaceProp),
                                    surfaceParameters(myU, myV),
                                    myDerOrder,
                                    myLinTol,
                                    std::string("new=") + LProp_CompareDebug::ToString(isDefined)
                                      + " old=" + LProp_CompareDebug::ToString(anOldDefined)
                                      + " newStatus=" + LProp_CompareDebug::ToString(myVTangentStatus)
                                      + " oldStatus="
                                      + LProp_CompareDebug::ToString(aLegacy.VTangentStatus()));
  }
  return isDefined;
}

//==================================================================================================

void LProp3d_SLProps::TangentV(gp_Dir& D)
{
  LProp_NotDefined_Raise_if(!IsTangentVDefined(), "LProp3d_SLProps::TangentV()");
  LProp_SLPropsCompat::Tangent(
    mySignificantFirstDerivativeOrderV,
    myLinTol,
    myV,
    mySurf->FirstVParameter(),
    mySurf->LastVParameter(),
    [&]() { return gp_Dir(D1V()); },
    [&](const gp_Pnt& thePntBefore, const gp_Pnt& thePntAfter) {
      return GeomProp::ComputeTangent(D1V(),
                                      D2V(),
                                      gp_Vec(0.0, 0.0, 0.0),
                                      myLinTol,
                                      thePntBefore,
                                      thePntAfter);
    },
    [&](const double theParam) { return mySurf->Value(myU, theParam); },
    D,
    "LProp3d_SLProps::TangentV()");
  LProp3d_LegacySLProps aLegacy(mySurf, myU, myV, myDerOrder, myLinTol);
  gp_Dir                anOldDir;
  try
  {
    aLegacy.TangentV(anOldDir);
    if (!D.IsEqual(anOldDir, Precision::Angular()))
    {
      LProp_CompareDebug::LogValueMismatch("LProp3d_SLProps::TangentV",
                                           surfaceGeometry(mySurfaceProp),
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
    LProp_CompareDebug::LogExceptionMismatch("LProp3d_SLProps::TangentV",
                                             surfaceGeometry(mySurfaceProp),
                                             surfaceParameters(myU, myV),
                                             myDerOrder,
                                             myLinTol,
                                             "value",
                                             LProp_CompareDebug::ExceptionSummary(theFailure));
  }
}

//==================================================================================================

bool LProp3d_SLProps::IsNormalDefined()
{
  const bool isDefined = LProp_SLPropsCompat::IsNormalDefined<GeomProp::SurfaceNormalResult>(
    [&]() {
      return hasGeomPropEvaluator(mySurfaceProp)
               ? mySurfaceProp->Normal(myU, myV, myLinTol)
               : GeomProp::ComputeSurfaceNormal(D1U(), D1V(), myLinTol);
    },
    myNormal,
    myNormalStatus);
  LProp3d_LegacySLProps aLegacy(mySurf, myU, myV, myDerOrder, myLinTol);
  const bool            anOldDefined = aLegacy.IsNormalDefined();
  if (isDefined != anOldDefined || myNormalStatus != aLegacy.NormalStatus())
  {
    LProp_CompareDebug::LogMismatch("LProp3d_SLProps::IsNormalDefined",
                                    surfaceGeometry(mySurfaceProp),
                                    surfaceParameters(myU, myV),
                                    myDerOrder,
                                    myLinTol,
                                    std::string("new=") + LProp_CompareDebug::ToString(isDefined)
                                      + " old=" + LProp_CompareDebug::ToString(anOldDefined)
                                      + " newStatus=" + LProp_CompareDebug::ToString(myNormalStatus)
                                      + " oldStatus="
                                      + LProp_CompareDebug::ToString(aLegacy.NormalStatus()));
  }
  return isDefined;
}

//==================================================================================================

const gp_Dir& LProp3d_SLProps::Normal()
{
  if (!IsNormalDefined())
  {
    throw LProp_NotDefined();
  }

  LProp3d_LegacySLProps aLegacy(mySurf, myU, myV, myDerOrder, myLinTol);
  try
  {
    const gp_Dir& anOldNormal = aLegacy.Normal();
    if (!myNormal.IsEqual(anOldNormal, Precision::Angular()))
    {
      LProp_CompareDebug::LogValueMismatch("LProp3d_SLProps::Normal",
                                           surfaceGeometry(mySurfaceProp),
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
    LProp_CompareDebug::LogExceptionMismatch("LProp3d_SLProps::Normal",
                                             surfaceGeometry(mySurfaceProp),
                                             surfaceParameters(myU, myV),
                                             myDerOrder,
                                             myLinTol,
                                             "value",
                                             LProp_CompareDebug::ExceptionSummary(theFailure));
  }

  return myNormal;
}

//==================================================================================================

bool LProp3d_SLProps::IsCurvatureDefined()
{
  const bool isDefined = LProp_SLPropsCompat::IsCurvatureDefined<GeomProp::SurfaceCurvatureResult,
                                                                 GeomProp::MeanGaussianResult>(
    myCN,
    IsNormalDefined(),
    IsTangentUDefined(),
    IsTangentVDefined(),
    [&]() {
      return hasGeomPropEvaluator(mySurfaceProp)
               ? mySurfaceProp->Curvatures(myU, myV, myLinTol)
               : GeomProp::ComputeSurfaceCurvatures(D1U(), D1V(), D2U(), D2V(), DUV(), myLinTol);
    },
    [&]() {
      return hasGeomPropEvaluator(mySurfaceProp)
               ? mySurfaceProp->MeanGaussian(myU, myV, myLinTol)
               : GeomProp::ComputeMeanGaussian(D1U(), D1V(), D2U(), D2V(), DUV(), myLinTol);
    },
    myMinCurv,
    myMaxCurv,
    myDirMinCurv,
    myDirMaxCurv,
    myMeanCurv,
    myGausCurv,
    myCurvatureStatus);
  LProp3d_LegacySLProps aLegacy(mySurf, myU, myV, myDerOrder, myLinTol);
  const bool            anOldDefined = aLegacy.IsCurvatureDefined();
  if (isDefined != anOldDefined || myCurvatureStatus != aLegacy.CurvatureStatus())
  {
    LProp_CompareDebug::LogMismatch("LProp3d_SLProps::IsCurvatureDefined",
                                    surfaceGeometry(mySurfaceProp),
                                    surfaceParameters(myU, myV),
                                    myDerOrder,
                                    myLinTol,
                                    std::string("new=") + LProp_CompareDebug::ToString(isDefined)
                                      + " old=" + LProp_CompareDebug::ToString(anOldDefined)
                                      + " newStatus=" + LProp_CompareDebug::ToString(myCurvatureStatus)
                                      + " oldStatus="
                                      + LProp_CompareDebug::ToString(aLegacy.CurvatureStatus()));
  }
  return isDefined;
}

//==================================================================================================

bool LProp3d_SLProps::IsUmbilic()
{
  if (!IsCurvatureDefined())
  {
    throw LProp_NotDefined();
  }
  const bool isUmbilic = std::abs(myMaxCurv - myMinCurv) < std::abs(Epsilon(myMaxCurv));
  LProp3d_LegacySLProps aLegacy(mySurf, myU, myV, myDerOrder, myLinTol);
  try
  {
    const bool anOldUmbilic = aLegacy.IsUmbilic();
    if (isUmbilic != anOldUmbilic)
    {
      LProp_CompareDebug::LogValueMismatch("LProp3d_SLProps::IsUmbilic",
                                           surfaceGeometry(mySurfaceProp),
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
    LProp_CompareDebug::LogExceptionMismatch("LProp3d_SLProps::IsUmbilic",
                                             surfaceGeometry(mySurfaceProp),
                                             surfaceParameters(myU, myV),
                                             myDerOrder,
                                             myLinTol,
                                             "value",
                                             LProp_CompareDebug::ExceptionSummary(theFailure));
  }
  return isUmbilic;
}

//==================================================================================================

double LProp3d_SLProps::MaxCurvature()
{
  if (!IsCurvatureDefined())
  {
    throw LProp_NotDefined();
  }
  LProp3d_LegacySLProps aLegacy(mySurf, myU, myV, myDerOrder, myLinTol);
  try
  {
    const double anOldValue = aLegacy.MaxCurvature();
    if (std::abs(myMaxCurv - anOldValue) > myLinTol)
    {
      LProp_CompareDebug::LogValueMismatch("LProp3d_SLProps::MaxCurvature",
                                           surfaceGeometry(mySurfaceProp),
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
    LProp_CompareDebug::LogExceptionMismatch("LProp3d_SLProps::MaxCurvature",
                                             surfaceGeometry(mySurfaceProp),
                                             surfaceParameters(myU, myV),
                                             myDerOrder,
                                             myLinTol,
                                             "value",
                                             LProp_CompareDebug::ExceptionSummary(theFailure));
  }
  return myMaxCurv;
}

//==================================================================================================

double LProp3d_SLProps::MinCurvature()
{
  if (!IsCurvatureDefined())
  {
    throw LProp_NotDefined();
  }
  LProp3d_LegacySLProps aLegacy(mySurf, myU, myV, myDerOrder, myLinTol);
  try
  {
    const double anOldValue = aLegacy.MinCurvature();
    if (std::abs(myMinCurv - anOldValue) > myLinTol)
    {
      LProp_CompareDebug::LogValueMismatch("LProp3d_SLProps::MinCurvature",
                                           surfaceGeometry(mySurfaceProp),
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
    LProp_CompareDebug::LogExceptionMismatch("LProp3d_SLProps::MinCurvature",
                                             surfaceGeometry(mySurfaceProp),
                                             surfaceParameters(myU, myV),
                                             myDerOrder,
                                             myLinTol,
                                             "value",
                                             LProp_CompareDebug::ExceptionSummary(theFailure));
  }
  return myMinCurv;
}

//==================================================================================================

void LProp3d_SLProps::CurvatureDirections(gp_Dir& MaxD, gp_Dir& MinD)
{
  if (!IsCurvatureDefined())
  {
    throw LProp_NotDefined();
  }

  MaxD = myDirMaxCurv;
  MinD = myDirMinCurv;
  LProp3d_LegacySLProps aLegacy(mySurf, myU, myV, myDerOrder, myLinTol);
  gp_Dir                anOldMax;
  gp_Dir                anOldMin;
  try
  {
    aLegacy.CurvatureDirections(anOldMax, anOldMin);
    if (!MaxD.IsEqual(anOldMax, Precision::Angular()))
    {
      LProp_CompareDebug::LogValueMismatch("LProp3d_SLProps::CurvatureDirections",
                                           surfaceGeometry(mySurfaceProp),
                                           surfaceParameters(myU, myV),
                                           myDerOrder,
                                           myLinTol,
                                           "maxDir",
                                           MaxD,
                                           anOldMax);
    }
    if (!MinD.IsEqual(anOldMin, Precision::Angular()))
    {
      LProp_CompareDebug::LogValueMismatch("LProp3d_SLProps::CurvatureDirections",
                                           surfaceGeometry(mySurfaceProp),
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
    LProp_CompareDebug::LogExceptionMismatch("LProp3d_SLProps::CurvatureDirections",
                                             surfaceGeometry(mySurfaceProp),
                                             surfaceParameters(myU, myV),
                                             myDerOrder,
                                             myLinTol,
                                             "value",
                                             LProp_CompareDebug::ExceptionSummary(theFailure));
  }
}

//==================================================================================================

double LProp3d_SLProps::MeanCurvature()
{
  if (!IsCurvatureDefined())
  {
    throw LProp_NotDefined();
  }
  LProp3d_LegacySLProps aLegacy(mySurf, myU, myV, myDerOrder, myLinTol);
  try
  {
    const double anOldValue = aLegacy.MeanCurvature();
    if (std::abs(myMeanCurv - anOldValue) > myLinTol)
    {
      LProp_CompareDebug::LogValueMismatch("LProp3d_SLProps::MeanCurvature",
                                           surfaceGeometry(mySurfaceProp),
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
    LProp_CompareDebug::LogExceptionMismatch("LProp3d_SLProps::MeanCurvature",
                                             surfaceGeometry(mySurfaceProp),
                                             surfaceParameters(myU, myV),
                                             myDerOrder,
                                             myLinTol,
                                             "value",
                                             LProp_CompareDebug::ExceptionSummary(theFailure));
  }
  return myMeanCurv;
}

//==================================================================================================

double LProp3d_SLProps::GaussianCurvature()
{
  if (!IsCurvatureDefined())
  {
    throw LProp_NotDefined();
  }
  LProp3d_LegacySLProps aLegacy(mySurf, myU, myV, myDerOrder, myLinTol);
  try
  {
    const double anOldValue = aLegacy.GaussianCurvature();
    if (std::abs(myGausCurv - anOldValue) > myLinTol)
    {
      LProp_CompareDebug::LogValueMismatch("LProp3d_SLProps::GaussianCurvature",
                                           surfaceGeometry(mySurfaceProp),
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
    LProp_CompareDebug::LogExceptionMismatch("LProp3d_SLProps::GaussianCurvature",
                                             surfaceGeometry(mySurfaceProp),
                                             surfaceParameters(myU, myV),
                                             myDerOrder,
                                             myLinTol,
                                             "value",
                                             LProp_CompareDebug::ExceptionSummary(theFailure));
  }
  return myGausCurv;
}
