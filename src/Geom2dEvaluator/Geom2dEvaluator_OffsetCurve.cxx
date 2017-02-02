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

#include <Geom2dAdaptor_HCurve.hxx>
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
        const Handle(Geom2dAdaptor_HCurve)& theBase,
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
  CalculateD0(theValue, aD1);
}

void Geom2dEvaluator_OffsetCurve::D1(const Standard_Real theU,
                                           gp_Pnt2d& theValue,
                                           gp_Vec2d& theD1) const
{
  gp_Vec2d aD2;
  BaseD2(theU, theValue, theD1, aD2);
  CalculateD1(theValue, theD1, aD2);
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

  CalculateD2(theValue, theD1, theD2, aD3, isDirectionChange);
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

  CalculateD3(theValue, theD1, theD2, theD3, aD4, isDirectionChange);
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


void Geom2dEvaluator_OffsetCurve::CalculateD0(      gp_Pnt2d& theValue,
                                              const gp_Vec2d& theD1) const
{
  if (theD1.SquareMagnitude() <= gp::Resolution())
    throw Standard_NullValue("Geom2dEvaluator_OffsetCurve: Undefined normal vector "
                             "because tangent vector has zero-magnitude!");

  gp_Dir2d aNormal(theD1.Y(), -theD1.X());
  theValue.ChangeCoord().Add(aNormal.XY() * myOffset);
}

void Geom2dEvaluator_OffsetCurve::CalculateD1(      gp_Pnt2d& theValue,
                                                    gp_Vec2d& theD1,
                                              const gp_Vec2d& theD2) const
{
  // P(u) = p(u) + Offset * Ndir / R
  // with R = || p' ^ Z|| and Ndir = P' ^ Z

  // P'(u) = p'(u) + (Offset / R**2) * (DNdir/DU * R -  Ndir * (DR/R))

  gp_XY Ndir(theD1.Y(), -theD1.X());
  gp_XY DNdir(theD2.Y(), -theD2.X());
  Standard_Real R2 = Ndir.SquareModulus();
  Standard_Real R = Sqrt(R2);
  Standard_Real R3 = R * R2;
  Standard_Real Dr = Ndir.Dot(DNdir);
  if (R3 <= gp::Resolution())
  {
    if (R2 <= gp::Resolution())
      throw Standard_NullValue("Geom2dEvaluator_OffsetCurve: Null derivative");
    //We try another computation but the stability is not very good.
    DNdir.Multiply(R);
    DNdir.Subtract(Ndir.Multiplied(Dr / R));
    DNdir.Multiply(myOffset / R2);
  }
  else
  {
    // Same computation as IICURV in EUCLID-IS because the stability is better
    DNdir.Multiply(myOffset / R);
    DNdir.Subtract(Ndir.Multiplied(myOffset * Dr / R3));
  }

  Ndir.Multiply(myOffset / R);
  // P(u)
  theValue.ChangeCoord().Add(Ndir);
  // P'(u)
  theD1.Add(gp_Vec2d(DNdir));
}

void Geom2dEvaluator_OffsetCurve::CalculateD2(      gp_Pnt2d& theValue,
                                                    gp_Vec2d& theD1,
                                                    gp_Vec2d& theD2,
                                              const gp_Vec2d& theD3,
                                              const Standard_Boolean theIsDirChange) const
{
  // P(u) = p(u) + Offset * Ndir / R
  // with R = || p' ^ Z|| and Ndir = P' ^ Z

  // P'(u) = p'(u) + (Offset / R**2) * (DNdir/DU * R -  Ndir * (DR/R))

  // P"(u) = p"(u) + (Offset / R) * (D2Ndir/DU - DNdir * (2.0 * Dr/ R**2) +
  //         Ndir * ( (3.0 * Dr**2 / R**4) - (D2r / R**2)))

  gp_XY Ndir(theD1.Y(), -theD1.X());
  gp_XY DNdir(theD2.Y(), -theD2.X());
  gp_XY D2Ndir(theD3.Y(), -theD3.X());
  Standard_Real R2 = Ndir.SquareModulus();
  Standard_Real R = Sqrt(R2);
  Standard_Real R3 = R2 * R;
  Standard_Real R4 = R2 * R2;
  Standard_Real R5 = R3 * R2;
  Standard_Real Dr = Ndir.Dot(DNdir);
  Standard_Real D2r = Ndir.Dot(D2Ndir) + DNdir.Dot(DNdir);
  if (R5 <= gp::Resolution())
  {
    if (R4 <= gp::Resolution())
      throw Standard_NullValue("Geom2dEvaluator_OffsetCurve: Null derivative");
    //We try another computation but the stability is not very good dixit ISG.
    // V2 = P" (U) :
    D2Ndir.Subtract(DNdir.Multiplied(2.0 * Dr / R2));
    D2Ndir.Add(Ndir.Multiplied(((3.0 * Dr * Dr) / R4) - (D2r / R2)));
    D2Ndir.Multiply(myOffset / R);

    // V1 = P' (U) :
    DNdir.Multiply(R);
    DNdir.Subtract(Ndir.Multiplied(Dr / R));
    DNdir.Multiply(myOffset / R2);
  }
  else
  {
    // Same computation as IICURV in EUCLID-IS because the stability is better.
    // V2 = P" (U) :
    D2Ndir.Multiply(myOffset / R);
    D2Ndir.Subtract(DNdir.Multiplied(2.0 * myOffset * Dr / R3));
    D2Ndir.Add(Ndir.Multiplied(myOffset * (((3.0 * Dr * Dr) / R5) - (D2r / R3))));

    // V1 = P' (U) 
    DNdir.Multiply(myOffset / R);
    DNdir.Subtract(Ndir.Multiplied(myOffset * Dr / R3));
  }

  Ndir.Multiply(myOffset / R);
  // P(u)
  theValue.ChangeCoord().Add(Ndir);
  // P'(u) :
  theD1.Add(gp_Vec2d(DNdir));
  // P"(u) :
  if (theIsDirChange)
    theD2.Reverse();
  theD2.Add(gp_Vec2d(D2Ndir));
}

void Geom2dEvaluator_OffsetCurve::CalculateD3(      gp_Pnt2d& theValue,
                                                    gp_Vec2d& theD1,
                                                    gp_Vec2d& theD2,
                                                    gp_Vec2d& theD3,
                                              const gp_Vec2d& theD4,
                                              const Standard_Boolean theIsDirChange) const
{
  // P(u) = p(u) + Offset * Ndir / R
  // with R = || p' ^ Z|| and Ndir = P' ^ Z

  // P'(u)  = p'(u) + (Offset / R**2) * (DNdir/DU * R -  Ndir * (DR/R))

  // P"(u)  = p"(u) + (Offset / R) * (D2Ndir/DU - DNdir * (2.0 * Dr/ R**2) +
  //          Ndir * ( (3.0 * Dr**2 / R**4) - (D2r / R**2)))

  // P"'(u) = p"'(u) + (Offset / R) * (D3Ndir - (3.0 * Dr/R**2 ) * D2Ndir -
  //          (3.0 * D2r / R2) * DNdir) + (3.0 * Dr * Dr / R4) * DNdir -
  //          (D3r/R2) * Ndir + (6.0 * Dr * Dr / R4) * Ndir +
  //          (6.0 * Dr * D2r / R4) * Ndir - (15.0 * Dr* Dr* Dr /R6) * Ndir

  gp_XY Ndir(theD1.Y(), -theD1.X());
  gp_XY DNdir(theD2.Y(), -theD2.X());
  gp_XY D2Ndir(theD3.Y(), -theD3.X());
  gp_XY D3Ndir(theD4.Y(), -theD4.X());
  Standard_Real R2 = Ndir.SquareModulus();
  Standard_Real R = Sqrt(R2);
  Standard_Real R3 = R2 * R;
  Standard_Real R4 = R2 * R2;
  Standard_Real R5 = R3 * R2;
  Standard_Real R6 = R3 * R3;
  Standard_Real R7 = R5 * R2;
  Standard_Real Dr = Ndir.Dot(DNdir);
  Standard_Real D2r = Ndir.Dot(D2Ndir) + DNdir.Dot(DNdir);
  Standard_Real D3r = Ndir.Dot(D3Ndir) + 3.0 * DNdir.Dot(D2Ndir);

  if (R7 <= gp::Resolution())
  {
    if (R6 <= gp::Resolution())
      throw Standard_NullValue("Geom2dEvaluator_OffsetCurve: Null derivative");
    //We try another computation but the stability is not very good dixit ISG.
    // V3 = P"' (U) :
    D3Ndir.Subtract(D2Ndir.Multiplied(3.0 * myOffset * Dr / R2));
    D3Ndir.Subtract(
      (DNdir.Multiplied((3.0 * myOffset) * ((D2r / R2) + (Dr*Dr) / R4))));
    D3Ndir.Add(Ndir.Multiplied(
      (myOffset * (6.0*Dr*Dr / R4 + 6.0*Dr*D2r / R4 - 15.0*Dr*Dr*Dr / R6 - D3r))));
    D3Ndir.Multiply(myOffset / R);
    // V2 = P" (U) :
    R4 = R2 * R2;
    D2Ndir.Subtract(DNdir.Multiplied(2.0 * Dr / R2));
    D2Ndir.Subtract(Ndir.Multiplied(((3.0 * Dr * Dr) / R4) - (D2r / R2)));
    D2Ndir.Multiply(myOffset / R);
    // V1 = P' (U) :
    DNdir.Multiply(R);
    DNdir.Subtract(Ndir.Multiplied(Dr / R));
    DNdir.Multiply(myOffset / R2);
  }
  else
  {
    // Same computation as IICURV in EUCLID-IS because the stability is better.
    // V3 = P"' (U) :
    D3Ndir.Multiply(myOffset / R);
    D3Ndir.Subtract(D2Ndir.Multiplied(3.0 * myOffset * Dr / R3));
    D3Ndir.Subtract(DNdir.Multiplied(
      ((3.0 * myOffset) * ((D2r / R3) + (Dr*Dr) / R5))));
    D3Ndir.Add(Ndir.Multiplied(
      (myOffset * (6.0*Dr*Dr / R5 + 6.0*Dr*D2r / R5 - 15.0*Dr*Dr*Dr / R7 - D3r))));
    // V2 = P" (U) :
    D2Ndir.Multiply(myOffset / R);
    D2Ndir.Subtract(DNdir.Multiplied(2.0 * myOffset * Dr / R3));
    D2Ndir.Subtract(Ndir.Multiplied(
      myOffset * (((3.0 * Dr * Dr) / R5) - (D2r / R3))));
    // V1 = P' (U) :
    DNdir.Multiply(myOffset / R);
    DNdir.Subtract(Ndir.Multiplied(myOffset * Dr / R3));
  }

  Ndir.Multiply(myOffset / R);
  // P(u)
  theValue.ChangeCoord().Add(Ndir);
  // P'(u) :
  theD1.Add(gp_Vec2d(DNdir));
  // P"(u)
  theD2.Add(gp_Vec2d(D2Ndir));
  // P"'(u)
  if (theIsDirChange)
    theD3.Reverse();
  theD3.Add(gp_Vec2d(D2Ndir));
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

