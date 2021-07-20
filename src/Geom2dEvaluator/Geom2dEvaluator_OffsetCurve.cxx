// Created on: 2015-09-21
// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <Geom2dEvaluator_OffsetCurve.hxx>
#include <Geom2dEvaluator.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Standard_NullValue.hxx>


IMPLEMENT_STANDARD_RTTIEXT(Geom2dEvaluator_OffsetCurve,Geom2dEvaluator_Curve)

Geom2dEvaluator_OffsetCurve::Geom2dEvaluator_OffsetCurve(
        const Handle(Geom2d_Curve)& theBase,
        const Standard_Real theOffset)
  : Geom2dEvaluator_Curve(),
    myBaseCurve(theBase),
    myOffset(theOffset)
{
}

Geom2dEvaluator_OffsetCurve::Geom2dEvaluator_OffsetCurve(
        const Handle(Geom2dAdaptor_Curve)& theBase,
        const Standard_Real theOffset)
  : Geom2dEvaluator_Curve(),
    myBaseAdaptor(theBase),
    myOffset(theOffset)
{
}

void Geom2dEvaluator_OffsetCurve::D0(const Standard_Real theU,
                                           gp_Pnt2d& theValue) const
{
  gp_Vec2d aD1;
  BaseD1(theU, theValue, aD1);
  Geom2dEvaluator::CalculateD0(theValue, aD1, myOffset);
}

void Geom2dEvaluator_OffsetCurve::D1(const Standard_Real theU,
                                           gp_Pnt2d& theValue,
                                           gp_Vec2d& theD1) const
{
  gp_Vec2d aD2;
  BaseD2(theU, theValue, theD1, aD2);
  Geom2dEvaluator::CalculateD1(theValue, theD1, aD2, myOffset);
}

void Geom2dEvaluator_OffsetCurve::D2(const Standard_Real theU,
                                           gp_Pnt2d& theValue,
                                           gp_Vec2d& theD1,
                                           gp_Vec2d& theD2) const
{
  gp_Vec2d aD3;
  BaseD3(theU, theValue, theD1, theD2, aD3);

  Standard_Boolean isDirectionChange = Standard_False;
  if (theD1.SquareMagnitude() <= gp::Resolution())
  {
    gp_Vec2d aDummyD4;
    isDirectionChange = AdjustDerivative(3, theU, theD1, theD2, aD3, aDummyD4);
  }

  Geom2dEvaluator::CalculateD2(theValue, theD1, theD2, aD3, isDirectionChange, myOffset);
}

void Geom2dEvaluator_OffsetCurve::D3(const Standard_Real theU,
                                           gp_Pnt2d& theValue,
                                           gp_Vec2d& theD1,
                                           gp_Vec2d& theD2,
                                           gp_Vec2d& theD3) const
{
  gp_Vec2d aD4;
  BaseD4(theU, theValue, theD1, theD2, theD3, aD4);

  Standard_Boolean isDirectionChange = Standard_False;
  if (theD1.SquareMagnitude() <= gp::Resolution())
    isDirectionChange = AdjustDerivative(4, theU, theD1, theD2, theD3, aD4);

  Geom2dEvaluator::CalculateD3(theValue, theD1, theD2, theD3, aD4, isDirectionChange, myOffset);
}

gp_Vec2d Geom2dEvaluator_OffsetCurve::DN(const Standard_Real theU,
                                         const Standard_Integer theDeriv) const
{
  Standard_RangeError_Raise_if(theDeriv < 1, "Geom2dEvaluator_OffsetCurve::DN(): theDeriv < 1");

  gp_Pnt2d aPnt;
  gp_Vec2d aDummy, aDN;
  switch (theDeriv)
  {
  case 1:
    D1(theU, aPnt, aDN);
    break;
  case 2:
    D2(theU, aPnt, aDummy, aDN);
    break;
  case 3:
    D3(theU, aPnt, aDummy, aDummy, aDN);
    break;
  default:
    aDN = BaseDN(theU, theDeriv);
  }
  return aDN;
}

Handle(Geom2dEvaluator_Curve) Geom2dEvaluator_OffsetCurve::ShallowCopy() const
{
  Handle(Geom2dEvaluator_OffsetCurve) aCopy;
  if (!myBaseAdaptor.IsNull())
  {
    aCopy = new Geom2dEvaluator_OffsetCurve(Handle(Geom2dAdaptor_Curve)::DownCast(myBaseAdaptor->ShallowCopy()),
                                            myOffset);
  }
  else
  {
    aCopy = new Geom2dEvaluator_OffsetCurve(myBaseCurve, myOffset);
  }

  return aCopy;
}


void Geom2dEvaluator_OffsetCurve::BaseD0(const Standard_Real theU,
                                               gp_Pnt2d& theValue) const
{
  if (!myBaseAdaptor.IsNull())
    myBaseAdaptor->D0(theU, theValue);
  else
    myBaseCurve->D0(theU, theValue);
}

void Geom2dEvaluator_OffsetCurve::BaseD1(const Standard_Real theU,
                                               gp_Pnt2d& theValue,
                                               gp_Vec2d& theD1) const
{
  if (!myBaseAdaptor.IsNull())
    myBaseAdaptor->D1(theU, theValue, theD1);
  else
    myBaseCurve->D1(theU, theValue, theD1);
}

void Geom2dEvaluator_OffsetCurve::BaseD2(const Standard_Real theU,
                                               gp_Pnt2d& theValue,
                                               gp_Vec2d& theD1,
                                               gp_Vec2d& theD2) const
{
  if (!myBaseAdaptor.IsNull())
    myBaseAdaptor->D2(theU, theValue, theD1, theD2);
  else
    myBaseCurve->D2(theU, theValue, theD1, theD2);
}

void Geom2dEvaluator_OffsetCurve::BaseD3(const Standard_Real theU,
                                               gp_Pnt2d& theValue,
                                               gp_Vec2d& theD1,
                                               gp_Vec2d& theD2,
                                               gp_Vec2d& theD3) const
{
  if (!myBaseAdaptor.IsNull())
    myBaseAdaptor->D3(theU, theValue, theD1, theD2, theD3);
  else
    myBaseCurve->D3(theU, theValue, theD1, theD2, theD3);
}

void Geom2dEvaluator_OffsetCurve::BaseD4(const Standard_Real theU,
                                               gp_Pnt2d& theValue,
                                               gp_Vec2d& theD1,
                                               gp_Vec2d& theD2,
                                               gp_Vec2d& theD3,
                                               gp_Vec2d& theD4) const
{
  if (!myBaseAdaptor.IsNull())
  {
    myBaseAdaptor->D3(theU, theValue, theD1, theD2, theD3);
    theD4 = myBaseAdaptor->DN(theU, 4);
  }
  else
  {
    myBaseCurve->D3(theU, theValue, theD1, theD2, theD3);
    theD4 = myBaseCurve->DN(theU, 4);
  }
}

gp_Vec2d Geom2dEvaluator_OffsetCurve::BaseDN(const Standard_Real theU,
                                             const Standard_Integer theDeriv) const
{
  if (!myBaseAdaptor.IsNull())
    return myBaseAdaptor->DN(theU, theDeriv);
  return myBaseCurve->DN(theU, theDeriv);
}




Standard_Boolean Geom2dEvaluator_OffsetCurve::AdjustDerivative(
    const Standard_Integer theMaxDerivative, const Standard_Real theU,
    gp_Vec2d& theD1, gp_Vec2d& theD2, gp_Vec2d& theD3, gp_Vec2d& theD4) const
{
  static const Standard_Real aTol = gp::Resolution();
  static const Standard_Real aMinStep = 1e-7;
  static const Standard_Integer aMaxDerivOrder = 3;

  Standard_Boolean isDirectionChange = Standard_False;
  Standard_Real anUinfium;
  Standard_Real anUsupremum;
  if (!myBaseAdaptor.IsNull())
  {
    anUinfium = myBaseAdaptor->FirstParameter();
    anUsupremum = myBaseAdaptor->LastParameter();
  }
  else
  {
    anUinfium = myBaseCurve->FirstParameter();
    anUsupremum = myBaseCurve->LastParameter();
  }

  static const Standard_Real DivisionFactor = 1.e-3;
  Standard_Real du;
  if ((anUsupremum >= RealLast()) || (anUinfium <= RealFirst()))
    du = 0.0;
  else
    du = anUsupremum - anUinfium;

  const Standard_Real aDelta = Max(du * DivisionFactor, aMinStep);

  //Derivative is approximated by Taylor-series
  Standard_Integer anIndex = 1; //Derivative order
  gp_Vec2d V;

  do
  {
    V = BaseDN(theU, ++anIndex);
  } while ((V.SquareMagnitude() <= aTol) && anIndex < aMaxDerivOrder);

  Standard_Real u;

  if (theU - anUinfium < aDelta)
    u = theU + aDelta;
  else
    u = theU - aDelta;

  gp_Pnt2d P1, P2;
  BaseD0(Min(theU, u), P1);
  BaseD0(Max(theU, u), P2);

  gp_Vec2d V1(P1, P2);
  isDirectionChange = V.Dot(V1) < 0.0;
  Standard_Real aSign = isDirectionChange ? -1.0 : 1.0;

  theD1 = V * aSign;
  gp_Vec2d* aDeriv[3] = { &theD2, &theD3, &theD4 };
  for (Standard_Integer i = 1; i < theMaxDerivative; i++)
    *(aDeriv[i - 1]) = BaseDN(theU, anIndex + i) * aSign;

  return isDirectionChange;
}

