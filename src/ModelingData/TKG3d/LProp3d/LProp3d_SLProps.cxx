// Created on: 2002-08-02
// Created by: Alexander KARTOMIN  (akm)
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#include <Adaptor3d_Surface.hxx>
#include <LProp_NotDefined.hxx>
#include <Standard_OutOfRange.hxx>

#include <LProp_SurfaceUtils.pxx>

using Access = LProp_SurfaceUtils::DirectAccess;

//=================================================================================================

LProp3d_SLProps::LProp3d_SLProps(const occ::handle<Adaptor3d_Surface>& S,
                                 const double                          U,
                                 const double                          V,
                                 const int                             N,
                                 const double                          Resolution)
    : mySurf(S),
      myDerOrder(N),
      myCN(4),
      myLinTol(Resolution)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 2, "LProp3d_SLProps::LProp3d_SLProps()");
  SetParameters(U, V);
}

//=================================================================================================

LProp3d_SLProps::LProp3d_SLProps(const occ::handle<Adaptor3d_Surface>& S,
                                 const int                             N,
                                 const double                          Resolution)
    : mySurf(S),
      myU(RealLast()),
      myV(RealLast()),
      myDerOrder(N),
      myCN(4),
      myLinTol(Resolution),
      myUTangentStatus(LProp_Undecided),
      myVTangentStatus(LProp_Undecided),
      myNormalStatus(LProp_Undecided),
      myCurvatureStatus(LProp_Undecided)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 2, "LProp3d_SLProps::LProp3d_SLProps()");
}

//=================================================================================================

LProp3d_SLProps::LProp3d_SLProps(const int N, const double Resolution)
    : myU(RealLast()),
      myV(RealLast()),
      myDerOrder(N),
      myCN(0),
      myLinTol(Resolution),
      myUTangentStatus(LProp_Undecided),
      myVTangentStatus(LProp_Undecided),
      myNormalStatus(LProp_Undecided),
      myCurvatureStatus(LProp_Undecided)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 2, "LProp3d_SLProps::LProp3d_SLProps() bad level");
}

//=================================================================================================

void LProp3d_SLProps::SetSurface(const occ::handle<Adaptor3d_Surface>& S)
{
  mySurf = S;
  myCN   = 4;
}

//=================================================================================================

void LProp3d_SLProps::SetParameters(const double U, const double V)
{
  LProp_SurfaceUtils::SetParameters<Access>(mySurf,
                                            U,
                                            V,
                                            myU,
                                            myV,
                                            myDerOrder,
                                            myPnt,
                                            myD1u,
                                            myD1v,
                                            myD2u,
                                            myD2v,
                                            myDuv,
                                            myUTangentStatus,
                                            myVTangentStatus,
                                            myNormalStatus,
                                            myCurvatureStatus);
}

//=================================================================================================

const gp_Pnt& LProp3d_SLProps::Value() const
{
  return myPnt;
}

//=================================================================================================

const gp_Vec& LProp3d_SLProps::D1U()
{
  return LProp_SurfaceUtils::EnsureSurfDeriv<
    Access>(mySurf, myU, myV, myDerOrder, 1, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv, myD1u);
}

//=================================================================================================

const gp_Vec& LProp3d_SLProps::D1V()
{
  return LProp_SurfaceUtils::EnsureSurfDeriv<
    Access>(mySurf, myU, myV, myDerOrder, 1, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv, myD1v);
}

//=================================================================================================

const gp_Vec& LProp3d_SLProps::D2U()
{
  return LProp_SurfaceUtils::EnsureSurfDeriv<
    Access>(mySurf, myU, myV, myDerOrder, 2, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv, myD2u);
}

//=================================================================================================

const gp_Vec& LProp3d_SLProps::D2V()
{
  return LProp_SurfaceUtils::EnsureSurfDeriv<
    Access>(mySurf, myU, myV, myDerOrder, 2, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv, myD2v);
}

//=================================================================================================

const gp_Vec& LProp3d_SLProps::DUV()
{
  return LProp_SurfaceUtils::EnsureSurfDeriv<
    Access>(mySurf, myU, myV, myDerOrder, 2, myPnt, myD1u, myD1v, myD2u, myD2v, myDuv, myDuv);
}

//=================================================================================================

bool LProp3d_SLProps::IsTangentUDefined()
{
  return LProp_SurfaceUtils::IsTangentUDefined(*this,
                                               myCN,
                                               myLinTol,
                                               mySignificantFirstDerivativeOrderU,
                                               myUTangentStatus);
}

//=================================================================================================

void LProp3d_SLProps::TangentU(gp_Dir& D)
{
  LProp_SurfaceUtils::TangentU<Access>(*this,
                                       mySurf,
                                       myU,
                                       myV,
                                       myD1u,
                                       myD2u,
                                       mySignificantFirstDerivativeOrderU,
                                       D);
}

//=================================================================================================

bool LProp3d_SLProps::IsTangentVDefined()
{
  return LProp_SurfaceUtils::IsTangentVDefined(*this,
                                               myCN,
                                               myLinTol,
                                               mySignificantFirstDerivativeOrderV,
                                               myVTangentStatus);
}

//=================================================================================================

void LProp3d_SLProps::TangentV(gp_Dir& D)
{
  LProp_SurfaceUtils::TangentV<Access>(*this,
                                       mySurf,
                                       myU,
                                       myV,
                                       myD1v,
                                       myD2v,
                                       mySignificantFirstDerivativeOrderV,
                                       D);
}

//=================================================================================================

bool LProp3d_SLProps::IsNormalDefined()
{
  return LProp_SurfaceUtils::IsNormalDefined(myD1u, myD1v, myLinTol, myNormal, myNormalStatus);
}

//=================================================================================================

const gp_Dir& LProp3d_SLProps::Normal()
{
  return LProp_SurfaceUtils::Normal(*this, myNormal);
}

//=================================================================================================

bool LProp3d_SLProps::IsCurvatureDefined()
{
  return LProp_SurfaceUtils::IsCurvatureDefined(*this,
                                                myCN,
                                                myDerOrder,
                                                myD1u,
                                                myD1v,
                                                myD2u,
                                                myD2v,
                                                myDuv,
                                                myNormal,
                                                myMinCurv,
                                                myMaxCurv,
                                                myDirMinCurv,
                                                myDirMaxCurv,
                                                myMeanCurv,
                                                myGausCurv,
                                                myCurvatureStatus);
}

//=================================================================================================

bool LProp3d_SLProps::IsUmbilic()
{
  return LProp_SurfaceUtils::IsUmbilic(*this, myMaxCurv, myMinCurv);
}

//=================================================================================================

double LProp3d_SLProps::MaxCurvature()
{
  return LProp_SurfaceUtils::RequireCurvature(*this, myMaxCurv);
}

//=================================================================================================

double LProp3d_SLProps::MinCurvature()
{
  return LProp_SurfaceUtils::RequireCurvature(*this, myMinCurv);
}

//=================================================================================================

void LProp3d_SLProps::CurvatureDirections(gp_Dir& Max, gp_Dir& Min)
{
  LProp_SurfaceUtils::CurvatureDirections(*this, myDirMaxCurv, myDirMinCurv, Max, Min);
}

//=================================================================================================

double LProp3d_SLProps::MeanCurvature()
{
  return LProp_SurfaceUtils::RequireCurvature(*this, myMeanCurv);
}

//=================================================================================================

double LProp3d_SLProps::GaussianCurvature()
{
  return LProp_SurfaceUtils::RequireCurvature(*this, myGausCurv);
}
