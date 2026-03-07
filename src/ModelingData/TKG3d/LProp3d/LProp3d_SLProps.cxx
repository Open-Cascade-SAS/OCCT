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
#include <LProp_NotDefined.hxx>
#include <LProp_Status.hxx>
#include <Standard_OutOfRange.hxx>

#include <algorithm>
#include <cmath>

namespace
{
constexpr double THE_MIN_STEP = 1.0e-7;

bool hasGeomPropEvaluator(const std::shared_ptr<GeomProp_Surface>& theSurfaceProp)
{
  return theSurfaceProp && theSurfaceProp->Adaptor() != nullptr;
}

static bool isTangentDefined(LProp3d_SLProps& theProps,
                             const int        theContinuity,
                             const double     theLinTol,
                             const int        theDerivative,
                             int&             theOrder,
                             LProp_Status&    theStatus)
{
  const double aTol2 = theLinTol * theLinTol;
  gp_Vec       aDerivatives[2];
  theOrder = 0;

  while (theOrder < 3)
  {
    ++theOrder;
    if (theContinuity < theOrder)
    {
      theStatus = LProp_Undefined;
      return false;
    }

    switch (theOrder)
    {
      case 1:
        aDerivatives[0] = theProps.D1U();
        aDerivatives[1] = theProps.D1V();
        break;
      case 2:
        aDerivatives[0] = theProps.D2U();
        aDerivatives[1] = theProps.D2V();
        break;
      default:
        break;
    }

    if (aDerivatives[theDerivative].SquareMagnitude() > aTol2)
    {
      theStatus = LProp_Defined;
      return true;
    }
  }

  theStatus = LProp_Undefined;
  return false;
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

  return myDuv;
}

//==================================================================================================

bool LProp3d_SLProps::IsTangentUDefined()
{
  if (myUTangentStatus == LProp_Undefined)
  {
    return false;
  }
  if (myUTangentStatus >= LProp_Defined)
  {
    return true;
  }

  return isTangentDefined(*this,
                          myCN,
                          myLinTol,
                          0,
                          mySignificantFirstDerivativeOrderU,
                          myUTangentStatus);
}

//==================================================================================================

void LProp3d_SLProps::TangentU(gp_Dir& D)
{
  LProp_NotDefined_Raise_if(!IsTangentUDefined(), "LProp3d_SLProps::TangentU()");

  if (mySignificantFirstDerivativeOrderU == 1)
  {
    D = gp_Dir(myD1u);
    return;
  }

  const double aU1    = mySurf->FirstUParameter();
  const double aU2    = mySurf->LastUParameter();
  const double aRange = (aU2 >= RealLast() || aU1 <= RealFirst()) ? 0.0 : aU2 - aU1;
  const double aDelta = std::max(aRange * 1.0e-3, THE_MIN_STEP);
  const double aOther = (myU - aU1 < aDelta) ? myU + aDelta : myU - aDelta;

  gp_Pnt aPntBefore;
  gp_Pnt aPntAfter;
  aPntBefore = mySurf->Value(std::min(myU, aOther), myV);
  aPntAfter  = mySurf->Value(std::max(myU, aOther), myV);

  const GeomProp::TangentResult aResult =
    GeomProp::ComputeTangent(myD1u, myD2u, gp_Vec(0.0, 0.0, 0.0), myLinTol, aPntBefore, aPntAfter);
  LProp_NotDefined_Raise_if(!aResult.IsDefined, "LProp3d_SLProps::TangentU()");
  D = aResult.Direction;
}

//==================================================================================================

bool LProp3d_SLProps::IsTangentVDefined()
{
  if (myVTangentStatus == LProp_Undefined)
  {
    return false;
  }
  if (myVTangentStatus >= LProp_Defined)
  {
    return true;
  }

  return isTangentDefined(*this,
                          myCN,
                          myLinTol,
                          1,
                          mySignificantFirstDerivativeOrderV,
                          myVTangentStatus);
}

//==================================================================================================

void LProp3d_SLProps::TangentV(gp_Dir& D)
{
  LProp_NotDefined_Raise_if(!IsTangentVDefined(), "LProp3d_SLProps::TangentV()");

  if (mySignificantFirstDerivativeOrderV == 1)
  {
    D = gp_Dir(myD1v);
    return;
  }

  const double aV1    = mySurf->FirstVParameter();
  const double aV2    = mySurf->LastVParameter();
  const double aRange = (aV2 >= RealLast() || aV1 <= RealFirst()) ? 0.0 : aV2 - aV1;
  const double aDelta = std::max(aRange * 1.0e-3, THE_MIN_STEP);
  const double aOther = (myV - aV1 < aDelta) ? myV + aDelta : myV - aDelta;

  gp_Pnt aPntBefore;
  gp_Pnt aPntAfter;
  aPntBefore = mySurf->Value(myU, std::min(myV, aOther));
  aPntAfter  = mySurf->Value(myU, std::max(myV, aOther));

  const GeomProp::TangentResult aResult =
    GeomProp::ComputeTangent(myD1v, myD2v, gp_Vec(0.0, 0.0, 0.0), myLinTol, aPntBefore, aPntAfter);
  LProp_NotDefined_Raise_if(!aResult.IsDefined, "LProp3d_SLProps::TangentV()");
  D = aResult.Direction;
}

//==================================================================================================

bool LProp3d_SLProps::IsNormalDefined()
{
  if (myNormalStatus == LProp_Undefined)
  {
    return false;
  }
  if (myNormalStatus >= LProp_Defined)
  {
    return true;
  }

  const GeomProp::SurfaceNormalResult aResult =
    hasGeomPropEvaluator(mySurfaceProp) ? mySurfaceProp->Normal(myU, myV, myLinTol)
                                        : GeomProp::ComputeSurfaceNormal(D1U(), D1V(), myLinTol);
  if (!aResult.IsDefined)
  {
    myNormalStatus = LProp_Undefined;
    return false;
  }

  myNormal       = aResult.Direction;
  myNormalStatus = LProp_Computed;
  return true;
}

//==================================================================================================

const gp_Dir& LProp3d_SLProps::Normal()
{
  if (!IsNormalDefined())
  {
    throw LProp_NotDefined();
  }

  return myNormal;
}

//==================================================================================================

bool LProp3d_SLProps::IsCurvatureDefined()
{
  if (myCurvatureStatus == LProp_Undefined)
  {
    return false;
  }
  if (myCurvatureStatus >= LProp_Defined)
  {
    return true;
  }
  if (myCN < 2 || !IsNormalDefined() || !IsTangentUDefined() || !IsTangentVDefined())
  {
    myCurvatureStatus = LProp_Undefined;
    return false;
  }

  const GeomProp::SurfaceCurvatureResult aCurvatures =
    hasGeomPropEvaluator(mySurfaceProp)
      ? mySurfaceProp->Curvatures(myU, myV, myLinTol)
      : GeomProp::ComputeSurfaceCurvatures(D1U(), D1V(), D2U(), D2V(), DUV(), myLinTol);
  const GeomProp::MeanGaussianResult aMeanGaussian =
    hasGeomPropEvaluator(mySurfaceProp)
      ? mySurfaceProp->MeanGaussian(myU, myV, myLinTol)
      : GeomProp::ComputeMeanGaussian(D1U(), D1V(), D2U(), D2V(), DUV(), myLinTol);
  if (!aCurvatures.IsDefined || !aMeanGaussian.IsDefined)
  {
    myCurvatureStatus = LProp_Undefined;
    return false;
  }

  myMinCurv         = aCurvatures.MinCurvature;
  myMaxCurv         = aCurvatures.MaxCurvature;
  myDirMinCurv      = aCurvatures.MinDirection;
  myDirMaxCurv      = aCurvatures.MaxDirection;
  myMeanCurv        = aMeanGaussian.MeanCurvature;
  myGausCurv        = aMeanGaussian.GaussianCurvature;
  myCurvatureStatus = LProp_Computed;
  return true;
}

//==================================================================================================

bool LProp3d_SLProps::IsUmbilic()
{
  if (!IsCurvatureDefined())
  {
    throw LProp_NotDefined();
  }

  return std::abs(myMaxCurv - myMinCurv) < std::abs(Epsilon(myMaxCurv));
}

//==================================================================================================

double LProp3d_SLProps::MaxCurvature()
{
  if (!IsCurvatureDefined())
  {
    throw LProp_NotDefined();
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
}

//==================================================================================================

double LProp3d_SLProps::MeanCurvature()
{
  if (!IsCurvatureDefined())
  {
    throw LProp_NotDefined();
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

  return myGausCurv;
}
