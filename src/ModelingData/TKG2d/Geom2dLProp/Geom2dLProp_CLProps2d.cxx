// Created on: 1992-03-26
// Created by: Herve LEGRAND
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <Geom2d_Curve.hxx>
#include <LProp_NotDefined.hxx>
#include <Standard_OutOfRange.hxx>

#include <LProp_CurveUtils.pxx>

using Access = LProp_CurveUtils::DirectAccess;

//=================================================================================================

Geom2dLProp_CLProps2d::Geom2dLProp_CLProps2d(const occ::handle<Geom2d_Curve>& C,
                                             const double                     U,
                                             const int                        N,
                                             const double                     Resolution)
    : myCurve(C),
      myDerOrder(N),
      myCN(4),
      myLinTol(Resolution),
      myTangentStatus(LProp_Undecided)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 3, "Geom2dLProp_CLProps2d::Geom2dLProp_CLProps2d()");
  SetParameter(U);
}

//=================================================================================================

Geom2dLProp_CLProps2d::Geom2dLProp_CLProps2d(const occ::handle<Geom2d_Curve>& C,
                                             const int                        N,
                                             const double                     Resolution)
    : myCurve(C),
      myU(RealLast()),
      myDerOrder(N),
      myCN(4),
      myLinTol(Resolution),
      myTangentStatus(LProp_Undecided)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 3, "Geom2dLProp_CLProps2d::Geom2dLProp_CLProps2d()");
}

//=================================================================================================

Geom2dLProp_CLProps2d::Geom2dLProp_CLProps2d(const int N, const double Resolution)
    : myU(RealLast()),
      myDerOrder(N),
      myCN(0),
      myLinTol(Resolution),
      myTangentStatus(LProp_Undecided)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 3, "Geom2dLProp_CLProps2d() - invalid input");
}

//=================================================================================================

void Geom2dLProp_CLProps2d::SetParameter(const double U)
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

void Geom2dLProp_CLProps2d::SetCurve(const occ::handle<Geom2d_Curve>& C)
{
  myCurve = C;
  myCN    = 4;
}

//=================================================================================================

const gp_Pnt2d& Geom2dLProp_CLProps2d::Value() const
{
  return myPnt;
}

//=================================================================================================

const gp_Vec2d& Geom2dLProp_CLProps2d::D1()
{
  return LProp_CurveUtils::EnsureDeriv<Access>(myCurve, myU, myDerOrder, 1, myPnt, myDerivArr);
}

//=================================================================================================

const gp_Vec2d& Geom2dLProp_CLProps2d::D2()
{
  return LProp_CurveUtils::EnsureDeriv<Access>(myCurve, myU, myDerOrder, 2, myPnt, myDerivArr);
}

//=================================================================================================

const gp_Vec2d& Geom2dLProp_CLProps2d::D3()
{
  return LProp_CurveUtils::EnsureDeriv<Access>(myCurve, myU, myDerOrder, 3, myPnt, myDerivArr);
}

//=================================================================================================

bool Geom2dLProp_CLProps2d::IsTangentDefined()
{
  return LProp_CurveUtils::IsTangentDefined<gp_Vec2d>(*this,
                                                      myCN,
                                                      myLinTol,
                                                      mySignificantFirstDerivativeOrder,
                                                      myTangentStatus);
}

//=================================================================================================

void Geom2dLProp_CLProps2d::Tangent(gp_Dir2d& D)
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

double Geom2dLProp_CLProps2d::Curvature()
{
  return LProp_CurveUtils::Curvature(*this,
                                     myDerivArr[0],
                                     myDerivArr[1],
                                     myLinTol,
                                     mySignificantFirstDerivativeOrder,
                                     myCurvature);
}

//=================================================================================================

void Geom2dLProp_CLProps2d::Normal(gp_Dir2d& N)
{
  LProp_CurveUtils::Normal(*this, myDerivArr[0], myDerivArr[1], myLinTol, N);
}

//=================================================================================================

void Geom2dLProp_CLProps2d::CentreOfCurvature(gp_Pnt2d& P)
{
  LProp_CurveUtils::CentreOfCurvature(*this,
                                      myPnt,
                                      myDerivArr[0],
                                      myDerivArr[1],
                                      myLinTol,
                                      myCurvature,
                                      P);
}
