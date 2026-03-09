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

#include <GeomLProp_CLProps.hxx>

#include <Geom_Curve.hxx>
#include <GeomProp.hxx>
#include <LProp_NotDefined.hxx>
#include <LProp_Status.hxx>
#include <Standard_OutOfRange.hxx>

static const double MinStep = 1.0e-7;

//=================================================================================================

GeomLProp_CLProps::GeomLProp_CLProps(const occ::handle<Geom_Curve>& C,
                                     const double                   U,
                                     const int                      N,
                                     const double                   Resolution)
    : myCurve(C),
      myDerOrder(N),
      myCN(4),
      myLinTol(Resolution),
      myTangentStatus(LProp_Undecided)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 3, "GeomLProp_CLProps::GeomLProp_CLProps()");

  SetParameter(U);
}

//=================================================================================================

GeomLProp_CLProps::GeomLProp_CLProps(const occ::handle<Geom_Curve>& C,
                                     const int                      N,
                                     const double                   Resolution)
    : myCurve(C),
      myU(RealLast()),
      myDerOrder(N),
      myCN(4),
      myLinTol(Resolution),
      myTangentStatus(LProp_Undecided)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 3, "GeomLProp_CLProps::GeomLProp_CLProps()");
}

//=================================================================================================

GeomLProp_CLProps::GeomLProp_CLProps(const int N, const double Resolution)
    : myU(RealLast()),
      myDerOrder(N),
      myCN(0),
      myLinTol(Resolution),
      myTangentStatus(LProp_Undecided)
{
  Standard_OutOfRange_Raise_if(N < 0 || N > 3, "GeomLProp_CLProps() - invalid input");
}

//=================================================================================================

void GeomLProp_CLProps::SetParameter(const double U)
{
  myU = U;
  switch (myDerOrder)
  {
    case 0:
      myPnt = myCurve->Value(myU);
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

  myTangentStatus = LProp_Undecided;
}

//=================================================================================================

void GeomLProp_CLProps::SetCurve(const occ::handle<Geom_Curve>& C)
{
  myCurve = C;
  myCN    = 4;
}

//=================================================================================================

const gp_Pnt& GeomLProp_CLProps::Value() const
{
  return myPnt;
}

//=================================================================================================

const gp_Vec& GeomLProp_CLProps::D1()
{
  if (myDerOrder < 1)
  {
    myDerOrder = 1;
    myCurve->D1(myU, myPnt, myDerivArr[0]);
  }

  return myDerivArr[0];
}

//=================================================================================================

const gp_Vec& GeomLProp_CLProps::D2()
{
  if (myDerOrder < 2)
  {
    myDerOrder = 2;
    myCurve->D2(myU, myPnt, myDerivArr[0], myDerivArr[1]);
  }

  return myDerivArr[1];
}

//=================================================================================================

const gp_Vec& GeomLProp_CLProps::D3()
{
  if (myDerOrder < 3)
  {
    myDerOrder = 3;
    myCurve->D3(myU, myPnt, myDerivArr[0], myDerivArr[1], myDerivArr[2]);
  }

  return myDerivArr[2];
}

//=================================================================================================

bool GeomLProp_CLProps::IsTangentDefined()
{
  if (myTangentStatus == LProp_Undefined)
    return false;
  else if (myTangentStatus >= LProp_Defined)
    return true;

  // tangentStatus == Lprop_Undecided
  // we have to calculate the first non null derivative
  const double Tol = myLinTol * myLinTol;

  gp_Vec V;

  int Order = 0;
  while (Order++ < 4)
  {
    if (myCN >= Order)
    {
      switch (Order)
      {
        case 1:
          V = D1();
          break;
        case 2:
          V = D2();
          break;
        case 3:
          V = D3();
          break;
      }

      if (V.SquareMagnitude() > Tol)
      {
        mySignificantFirstDerivativeOrder = Order;
        myTangentStatus                   = LProp_Defined;
        return true;
      }
    }
    else
    {
      myTangentStatus = LProp_Undefined;
      return false;
    }
  }

  return false;
}

//=================================================================================================

void GeomLProp_CLProps::Tangent(gp_Dir& D)
{
  if (!IsTangentDefined())
    throw LProp_NotDefined();

  if (mySignificantFirstDerivativeOrder == 1)
    D = gp_Dir(myDerivArr[0]);
  else if (mySignificantFirstDerivativeOrder > 1)
  {
    const double DivisionFactor = 1.e-3;
    const double anUsupremum = myCurve->LastParameter(), anUinfium = myCurve->FirstParameter();

    double du;
    if ((anUsupremum >= RealLast()) || (anUinfium <= RealFirst()))
      du = 0.0;
    else
      du = anUsupremum - anUinfium;

    const double aDelta = std::max(du * DivisionFactor, MinStep);

    gp_Vec V = myDerivArr[mySignificantFirstDerivativeOrder - 1];

    double u;

    if (myU - anUinfium < aDelta)
      u = myU + aDelta;
    else
      u = myU - aDelta;

    gp_Pnt P1 = myCurve->Value(std::min(myU, u));
    gp_Pnt P2 = myCurve->Value(std::max(myU, u));

    gp_Vec V1(P1, P2);
    double aDirFactor = V.Dot(V1);

    if (aDirFactor < 0.0)
      V = -V;

    D = gp_Dir(V);
  }
}

//=================================================================================================

double GeomLProp_CLProps::Curvature()
{
  bool isDefined = IsTangentDefined();
  (void)isDefined;
  LProp_NotDefined_Raise_if(!isDefined, "GeomLProp_CLProps::CurvatureNotDefined()");

  // if the first derivative is null the curvature is infinite.
  if (mySignificantFirstDerivativeOrder > 1)
    return RealLast();

  GeomProp::CurvatureResult aResult =
    GeomProp::ComputeCurvature(myDerivArr[0], myDerivArr[1], myLinTol);
  myCurvature = aResult.IsDefined ? aResult.Value : 0.0;

  return myCurvature;
}

//=================================================================================================

void GeomLProp_CLProps::Normal(gp_Dir& N)
{
  double c = Curvature();
  if (c == RealLast() || std::abs(c) <= myLinTol)
  {
    throw LProp_NotDefined("GeomLProp_CLProps::Normal(...):"
                           "Curvature is null or infinity");
  }

  GeomProp::NormalResult aResult = GeomProp::ComputeNormal(myDerivArr[0], myDerivArr[1], myLinTol);
  if (!aResult.IsDefined)
  {
    throw LProp_NotDefined("GeomLProp_CLProps::Normal(...):"
                           "Normal is not defined");
  }

  N = aResult.Direction;
}

//=================================================================================================

void GeomLProp_CLProps::CentreOfCurvature(gp_Pnt& P)
{
  if (std::abs(Curvature()) <= myLinTol)
  {
    throw LProp_NotDefined();
  }

  GeomProp::CentreResult aResult =
    GeomProp::ComputeCentreOfCurvature(myPnt, myDerivArr[0], myDerivArr[1], myLinTol);
  if (!aResult.IsDefined)
  {
    throw LProp_NotDefined();
  }

  P = aResult.Centre;
}
