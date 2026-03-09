// Created on: 1992-10-14
// Created by: Christophe MARION
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

#include <HLRBRep_CLProps.hxx>

#include <HLRBRep_CLPropsATool.hxx>
#include <HLRBRep_Curve.hxx>
#include <LProp_NotDefined.hxx>
#include <Standard_OutOfRange.hxx>

#include <LProp_CurveUtils.pxx>

using Access = LProp_CurveUtils::ToolAccess<HLRBRep_CLPropsATool>;

//=================================================================================================

HLRBRep_CLProps::HLRBRep_CLProps(const HLRBRep_Curve*& C,
                                  const double          U,
                                  const int             N,
                                  const double          Resolution)
    : myCurve(C),
      myDerOrder(N),
      myCN(4),
      myLinTol(Resolution),
      myTangentStatus(LProp_Undecided)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 3, "HLRBRep_CLProps::HLRBRep_CLProps()");
  SetParameter(U);
}

//=================================================================================================

HLRBRep_CLProps::HLRBRep_CLProps(const HLRBRep_Curve*& C,
                                  const int             N,
                                  const double          Resolution)
    : myCurve(C),
      myU(RealLast()),
      myDerOrder(N),
      myCN(4),
      myLinTol(Resolution),
      myTangentStatus(LProp_Undecided)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 3, "HLRBRep_CLProps::HLRBRep_CLProps()");
}

//=================================================================================================

HLRBRep_CLProps::HLRBRep_CLProps(const int N, const double Resolution)
    : myCurve(nullptr),
      myU(RealLast()),
      myDerOrder(N),
      myCN(0),
      myLinTol(Resolution),
      myTangentStatus(LProp_Undecided)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 3, "HLRBRep_CLProps() - invalid input");
}

//=================================================================================================

void HLRBRep_CLProps::SetParameter(const double U)
{
  LProp_CurveUtils::SetParameter<Access>(myCurve, U, myU, myDerOrder, myPnt, myDerivArr, myTangentStatus);
}

//=================================================================================================

void HLRBRep_CLProps::SetCurve(const HLRBRep_Curve*& C)
{
  myCurve = C;
  myCN    = 4;
}

//=================================================================================================

const gp_Pnt2d& HLRBRep_CLProps::Value() const
{
  return myPnt;
}

//=================================================================================================

const gp_Vec2d& HLRBRep_CLProps::D1()
{
  return LProp_CurveUtils::EnsureDeriv<Access>(myCurve, myU, myDerOrder, 1, myPnt, myDerivArr);
}

//=================================================================================================

const gp_Vec2d& HLRBRep_CLProps::D2()
{
  return LProp_CurveUtils::EnsureDeriv<Access>(myCurve, myU, myDerOrder, 2, myPnt, myDerivArr);
}

//=================================================================================================

const gp_Vec2d& HLRBRep_CLProps::D3()
{
  return LProp_CurveUtils::EnsureDeriv<Access>(myCurve, myU, myDerOrder, 3, myPnt, myDerivArr);
}

//=================================================================================================

bool HLRBRep_CLProps::IsTangentDefined()
{
  return LProp_CurveUtils::IsTangentDefined<gp_Vec2d>(*this, myCN, myLinTol, mySignificantFirstDerivativeOrder, myTangentStatus);
}

//=================================================================================================

void HLRBRep_CLProps::Tangent(gp_Dir2d& D)
{
  LProp_CurveUtils::Tangent<Access>(*this, myCurve, myU, myDerivArr, myPnt, mySignificantFirstDerivativeOrder, D);
}

//=================================================================================================

double HLRBRep_CLProps::Curvature()
{
  return LProp_CurveUtils::Curvature(*this, myDerivArr[0], myDerivArr[1], myLinTol, mySignificantFirstDerivativeOrder, myCurvature);
}

//=================================================================================================

void HLRBRep_CLProps::Normal(gp_Dir2d& N)
{
  LProp_CurveUtils::Normal(*this, myDerivArr[0], myDerivArr[1], myLinTol, N);
}

//=================================================================================================

void HLRBRep_CLProps::CentreOfCurvature(gp_Pnt2d& P)
{
  LProp_CurveUtils::CentreOfCurvature(*this, myPnt, myDerivArr[0], myDerivArr[1], myLinTol, myCurvature, P);
}
