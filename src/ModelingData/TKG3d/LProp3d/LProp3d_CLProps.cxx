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

#include <LProp3d_CLProps.hxx>

#include <Adaptor3d_Curve.hxx>
#include <LProp_NotDefined.hxx>
#include <Standard_OutOfRange.hxx>

#include <LProp_CurveUtils.pxx>

using Access = LProp_CurveUtils::DirectAccess;

//=================================================================================================

LProp3d_CLProps::LProp3d_CLProps(const occ::handle<Adaptor3d_Curve>& C,
                                 const double                        U,
                                 const int                           N,
                                 const double                        Resolution)
    : myCurve(C),
      myDerOrder(N),
      myCN(4),
      myLinTol(Resolution),
      myTangentStatus(LProp_Undecided)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 3, "LProp3d_CLProps::LProp3d_CLProps()");
  SetParameter(U);
}

//=================================================================================================

LProp3d_CLProps::LProp3d_CLProps(const occ::handle<Adaptor3d_Curve>& C,
                                 const int                           N,
                                 const double                        Resolution)
    : myCurve(C),
      myU(RealLast()),
      myDerOrder(N),
      myCN(4),
      myLinTol(Resolution),
      myTangentStatus(LProp_Undecided)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 3, "LProp3d_CLProps::LProp3d_CLProps()");
}

//=================================================================================================

LProp3d_CLProps::LProp3d_CLProps(const int N, const double Resolution)
    : myU(RealLast()),
      myDerOrder(N),
      myCN(0),
      myLinTol(Resolution),
      myTangentStatus(LProp_Undecided)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 3, "LProp3d_CLProps() - invalid input");
}

//=================================================================================================

void LProp3d_CLProps::SetParameter(const double U)
{
  LProp_CurveUtils::SetParameter<Access>(myCurve,
                                         U,
                                         myU,
                                         myDerOrder,
                                         myPnt,
                                         myDerivArr,
                                         myTangentStatus);
}

//=================================================================================================

void LProp3d_CLProps::SetCurve(const occ::handle<Adaptor3d_Curve>& C)
{
  myCurve = C;
  myCN    = 4;
}

//=================================================================================================

const gp_Pnt& LProp3d_CLProps::Value() const
{
  return myPnt;
}

//=================================================================================================

const gp_Vec& LProp3d_CLProps::D1()
{
  return LProp_CurveUtils::EnsureDeriv<Access>(myCurve, myU, myDerOrder, 1, myPnt, myDerivArr);
}

//=================================================================================================

const gp_Vec& LProp3d_CLProps::D2()
{
  return LProp_CurveUtils::EnsureDeriv<Access>(myCurve, myU, myDerOrder, 2, myPnt, myDerivArr);
}

//=================================================================================================

const gp_Vec& LProp3d_CLProps::D3()
{
  return LProp_CurveUtils::EnsureDeriv<Access>(myCurve, myU, myDerOrder, 3, myPnt, myDerivArr);
}

//=================================================================================================

bool LProp3d_CLProps::IsTangentDefined()
{
  return LProp_CurveUtils::IsTangentDefined<gp_Vec>(*this,
                                                    myCN,
                                                    myLinTol,
                                                    mySignificantFirstDerivativeOrder,
                                                    myTangentStatus);
}

//=================================================================================================

void LProp3d_CLProps::Tangent(gp_Dir& D)
{
  LProp_CurveUtils::Tangent<Access>(*this,
                                    myCurve,
                                    myU,
                                    myDerivArr,
                                    myPnt,
                                    mySignificantFirstDerivativeOrder,
                                    D);
}

//=================================================================================================

double LProp3d_CLProps::Curvature()
{
  return LProp_CurveUtils::Curvature(*this,
                                     myDerivArr[0],
                                     myDerivArr[1],
                                     myLinTol,
                                     mySignificantFirstDerivativeOrder,
                                     myCurvature);
}

//=================================================================================================

void LProp3d_CLProps::Normal(gp_Dir& N)
{
  LProp_CurveUtils::Normal(*this, myDerivArr[0], myDerivArr[1], myLinTol, N);
}

//=================================================================================================

void LProp3d_CLProps::CentreOfCurvature(gp_Pnt& P)
{
  LProp_CurveUtils::CentreOfCurvature(*this,
                                      myPnt,
                                      myDerivArr[0],
                                      myDerivArr[1],
                                      myLinTol,
                                      myCurvature,
                                      P);
}
