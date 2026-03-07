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

#include <GeomProp.hxx>
#include <GeomProp_Surface.hxx>
#include <Geom_Surface.hxx>
#include <LProp_SLPropsCompat.pxx>
#include <LProp_NotDefined.hxx>
#include <LProp_Status.hxx>
#include <Standard_OutOfRange.hxx>

namespace
{
const GeomAdaptor_Surface* surfaceAdaptor(const std::shared_ptr<GeomProp_Surface>& theSurfaceProp)
{
  return theSurfaceProp ? theSurfaceProp->Adaptor() : nullptr;
}

gp_Pnt surfaceValue(const occ::handle<Geom_Surface>&         theSurface,
                    const std::shared_ptr<GeomProp_Surface>& theSurfaceProp,
                    const double                             theU,
                    const double                             theV)
{
  if (const GeomAdaptor_Surface* anAdaptor = surfaceAdaptor(theSurfaceProp))
  {
    return anAdaptor->Value(theU, theV);
  }

  gp_Pnt aPoint;
  theSurface->D0(theU, theV, aPoint);
  return aPoint;
}

void surfaceD1(const occ::handle<Geom_Surface>&         theSurface,
               const std::shared_ptr<GeomProp_Surface>& theSurfaceProp,
               const double                             theU,
               const double                             theV,
               gp_Pnt&                                  thePoint,
               gp_Vec&                                  theD1U,
               gp_Vec&                                  theD1V)
{
  if (const GeomAdaptor_Surface* anAdaptor = surfaceAdaptor(theSurfaceProp))
  {
    anAdaptor->D1(theU, theV, thePoint, theD1U, theD1V);
    return;
  }

  theSurface->D1(theU, theV, thePoint, theD1U, theD1V);
}

void surfaceD2(const occ::handle<Geom_Surface>&         theSurface,
               const std::shared_ptr<GeomProp_Surface>& theSurfaceProp,
               const double                             theU,
               const double                             theV,
               gp_Pnt&                                  thePoint,
               gp_Vec&                                  theD1U,
               gp_Vec&                                  theD1V,
               gp_Vec&                                  theD2U,
               gp_Vec&                                  theD2V,
               gp_Vec&                                  theDUV)
{
  if (const GeomAdaptor_Surface* anAdaptor = surfaceAdaptor(theSurfaceProp))
  {
    anAdaptor->D2(theU, theV, thePoint, theD1U, theD1V, theD2U, theD2V, theDUV);
    return;
  }

  theSurface->D2(theU, theV, thePoint, theD1U, theD1V, theD2U, theD2V, theDUV);
}

void surfaceBounds(const occ::handle<Geom_Surface>&         theSurface,
                   const std::shared_ptr<GeomProp_Surface>& theSurfaceProp,
                   double&                                  theU1,
                   double&                                  theU2,
                   double&                                  theV1,
                   double&                                  theV2)
{
  if (const GeomAdaptor_Surface* anAdaptor = surfaceAdaptor(theSurfaceProp))
  {
    theU1 = anAdaptor->FirstUParameter();
    theU2 = anAdaptor->LastUParameter();
    theV1 = anAdaptor->FirstVParameter();
    theV2 = anAdaptor->LastVParameter();
    return;
  }

  theSurface->Bounds(theU1, theU2, theV1, theV2);
}
} // namespace

//==================================================================================================

GeomLProp_SLProps::GeomLProp_SLProps(const occ::handle<Geom_Surface>& S,
                                     const double                     U,
                                     const double                     V,
                                     const int                        N,
                                     const double                     Resolution)
    : mySurf(S),
      mySurfaceProp(S.IsNull() ? nullptr : std::make_shared<GeomProp_Surface>(S)),
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
      mySurfaceProp(S.IsNull() ? nullptr : std::make_shared<GeomProp_Surface>(S)),
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
  Standard_OutOfRange_Raise_if(N < 0 || N > 2, "GeomLProp_SLProps::GeomLProp_SLProps()");
}

//==================================================================================================

GeomLProp_SLProps::~GeomLProp_SLProps() = default;

//==================================================================================================

void GeomLProp_SLProps::SetSurface(const occ::handle<Geom_Surface>& S)
{
  mySurf                             = S;
  mySurfaceProp                      = S.IsNull() ? nullptr : std::make_shared<GeomProp_Surface>(S);
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
      myPnt = surfaceValue(mySurf, mySurfaceProp, myU, myV);
      break;
    case 1:
      surfaceD1(mySurf, mySurfaceProp, myU, myV, myPnt, myD1u, myD1v);
      break;
    case 2:
      surfaceD2(mySurf, mySurfaceProp, myU, myV, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv);
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

const gp_Pnt& GeomLProp_SLProps::Value() const
{
  return myPnt;
}

//==================================================================================================

const gp_Vec& GeomLProp_SLProps::D1U()
{
  if (myDerOrder < 1)
  {
    myDerOrder = 1;
    surfaceD1(mySurf, mySurfaceProp, myU, myV, myPnt, myD1u, myD1v);
  }

  return myD1u;
}

//==================================================================================================

const gp_Vec& GeomLProp_SLProps::D1V()
{
  if (myDerOrder < 1)
  {
    myDerOrder = 1;
    surfaceD1(mySurf, mySurfaceProp, myU, myV, myPnt, myD1u, myD1v);
  }

  return myD1v;
}

//==================================================================================================

const gp_Vec& GeomLProp_SLProps::D2U()
{
  if (myDerOrder < 2)
  {
    myDerOrder = 2;
    surfaceD2(mySurf, mySurfaceProp, myU, myV, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv);
  }

  return myD2u;
}

//==================================================================================================

const gp_Vec& GeomLProp_SLProps::D2V()
{
  if (myDerOrder < 2)
  {
    myDerOrder = 2;
    surfaceD2(mySurf, mySurfaceProp, myU, myV, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv);
  }

  return myD2v;
}

//==================================================================================================

const gp_Vec& GeomLProp_SLProps::DUV()
{
  if (myDerOrder < 2)
  {
    myDerOrder = 2;
    surfaceD2(mySurf, mySurfaceProp, myU, myV, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv);
  }

  return myDuv;
}

//==================================================================================================

bool GeomLProp_SLProps::IsTangentUDefined()
{
  return LProp_SLPropsCompat::IsTangentDefined(myCN,
                                               myLinTol,
                                               D1U(),
                                               D1V(),
                                               D2U(),
                                               D2V(),
                                               0,
                                               mySignificantFirstDerivativeOrderU,
                                               myUTangentStatus);
}

//==================================================================================================

void GeomLProp_SLProps::TangentU(gp_Dir& D)
{
  LProp_NotDefined_Raise_if(!IsTangentUDefined(), "GeomLProp_SLProps::TangentU()");
  double aU1, aV1, aU2, aV2;
  surfaceBounds(mySurf, mySurfaceProp, aU1, aU2, aV1, aV2);
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
    [&](const double theParam) { return surfaceValue(mySurf, mySurfaceProp, theParam, myV); },
    D,
    "GeomLProp_SLProps::TangentU()");
}

//==================================================================================================

bool GeomLProp_SLProps::IsTangentVDefined()
{
  return LProp_SLPropsCompat::IsTangentDefined(myCN,
                                               myLinTol,
                                               D1U(),
                                               D1V(),
                                               D2U(),
                                               D2V(),
                                               1,
                                               mySignificantFirstDerivativeOrderV,
                                               myVTangentStatus);
}

//==================================================================================================

void GeomLProp_SLProps::TangentV(gp_Dir& D)
{
  LProp_NotDefined_Raise_if(!IsTangentVDefined(), "GeomLProp_SLProps::TangentV()");
  double aU1, aV1, aU2, aV2;
  surfaceBounds(mySurf, mySurfaceProp, aU1, aU2, aV1, aV2);
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
    [&](const double theParam) { return surfaceValue(mySurf, mySurfaceProp, myU, theParam); },
    D,
    "GeomLProp_SLProps::TangentV()");
}

//==================================================================================================

bool GeomLProp_SLProps::IsNormalDefined()
{
  return LProp_SLPropsCompat::IsNormalDefined<GeomProp::SurfaceNormalResult>(
    [&]() {
      return mySurfaceProp ? mySurfaceProp->Normal(myU, myV, myLinTol)
                           : GeomProp::ComputeSurfaceNormal(D1U(), D1V(), myLinTol);
    },
    myNormal,
    myNormalStatus);
}

//==================================================================================================

const gp_Dir& GeomLProp_SLProps::Normal()
{
  if (!IsNormalDefined())
  {
    throw LProp_NotDefined();
  }

  return myNormal;
}

//==================================================================================================

bool GeomLProp_SLProps::IsCurvatureDefined()
{
  return LProp_SLPropsCompat::IsCurvatureDefined<GeomProp::SurfaceCurvatureResult,
                                                 GeomProp::MeanGaussianResult>(
    myCN,
    IsNormalDefined(),
    IsTangentUDefined(),
    IsTangentVDefined(),
    [&]() {
      return mySurfaceProp
               ? mySurfaceProp->Curvatures(myU, myV, myLinTol)
               : GeomProp::ComputeSurfaceCurvatures(D1U(), D1V(), D2U(), D2V(), DUV(), myLinTol);
    },
    [&]() {
      return mySurfaceProp
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
}

//==================================================================================================

bool GeomLProp_SLProps::IsUmbilic()
{
  if (!IsCurvatureDefined())
  {
    throw LProp_NotDefined();
  }

  return std::abs(myMaxCurv - myMinCurv) < std::abs(Epsilon(myMaxCurv));
}

//==================================================================================================

double GeomLProp_SLProps::MaxCurvature()
{
  if (!IsCurvatureDefined())
  {
    throw LProp_NotDefined();
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
}

//==================================================================================================

double GeomLProp_SLProps::MeanCurvature()
{
  if (!IsCurvatureDefined())
  {
    throw LProp_NotDefined();
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

  return myGausCurv;
}
