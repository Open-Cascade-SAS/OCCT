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

#include <Geom2dEvaluator.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_XY.hxx>
#include <Standard_NullValue.hxx>

//=================================================================================================

void Geom2dEvaluator::CalculateD0(gp_Pnt2d&           theValue,
                                  const gp_Vec2d&     theD1,
                                  const Standard_Real theOffset)
{
  if (theD1.SquareMagnitude() <= gp::Resolution())
    throw Standard_NullValue("Geom2dEvaluator: Undefined normal vector "
                             "because tangent vector has zero-magnitude!");

  gp_Dir2d aNormal(theD1.Y(), -theD1.X());
  theValue.ChangeCoord().Add(aNormal.XY() * theOffset);
}

//=================================================================================================

void Geom2dEvaluator::CalculateD1(gp_Pnt2d&           theValue,
                                  gp_Vec2d&           theD1,
                                  const gp_Vec2d&     theD2,
                                  const Standard_Real theOffset)
{
  // P(u) = p(u) + Offset * Ndir / R
  // with R = || p' ^ Z|| and Ndir = P' ^ Z

  // P'(u) = p'(u) + (Offset / R**2) * (DNdir/DU * R -  Ndir * (DR/R))

  gp_XY         Ndir(theD1.Y(), -theD1.X());
  gp_XY         DNdir(theD2.Y(), -theD2.X());
  Standard_Real R2 = Ndir.SquareModulus();
  Standard_Real R  = Sqrt(R2);
  Standard_Real R3 = R * R2;
  Standard_Real Dr = Ndir.Dot(DNdir);
  if (R3 <= gp::Resolution())
  {
    if (R2 <= gp::Resolution())
      throw Standard_NullValue("Geom2dEvaluator_OffsetCurve: Null derivative");
    // We try another computation but the stability is not very good.
    DNdir.Multiply(R);
    DNdir.Subtract(Ndir.Multiplied(Dr / R));
    DNdir.Multiply(theOffset / R2);
  }
  else
  {
    // Same computation as IICURV in EUCLID-IS because the stability is better
    DNdir.Multiply(theOffset / R);
    DNdir.Subtract(Ndir.Multiplied(theOffset * Dr / R3));
  }

  Ndir.Multiply(theOffset / R);
  // P(u)
  theValue.ChangeCoord().Add(Ndir);
  // P'(u)
  theD1.Add(gp_Vec2d(DNdir));
}

//=================================================================================================

void Geom2dEvaluator::CalculateD2(gp_Pnt2d&              theValue,
                                  gp_Vec2d&              theD1,
                                  gp_Vec2d&              theD2,
                                  const gp_Vec2d&        theD3,
                                  const Standard_Boolean theIsDirChange,
                                  const Standard_Real    theOffset)
{
  // P(u) = p(u) + Offset * Ndir / R
  // with R = || p' ^ Z|| and Ndir = P' ^ Z

  // P'(u) = p'(u) + (Offset / R**2) * (DNdir/DU * R -  Ndir * (DR/R))

  // P"(u) = p"(u) + (Offset / R) * (D2Ndir/DU - DNdir * (2.0 * Dr/ R**2) +
  //         Ndir * ( (3.0 * Dr**2 / R**4) - (D2r / R**2)))

  gp_XY         Ndir(theD1.Y(), -theD1.X());
  gp_XY         DNdir(theD2.Y(), -theD2.X());
  gp_XY         D2Ndir(theD3.Y(), -theD3.X());
  Standard_Real R2  = Ndir.SquareModulus();
  Standard_Real R   = Sqrt(R2);
  Standard_Real R3  = R2 * R;
  Standard_Real R4  = R2 * R2;
  Standard_Real R5  = R3 * R2;
  Standard_Real Dr  = Ndir.Dot(DNdir);
  Standard_Real D2r = Ndir.Dot(D2Ndir) + DNdir.Dot(DNdir);
  if (R5 <= gp::Resolution())
  {
    if (R4 <= gp::Resolution())
      throw Standard_NullValue("Geom2dEvaluator: Null derivative");
    // We try another computation but the stability is not very good dixit ISG.
    //  V2 = P" (U) :
    D2Ndir.Subtract(DNdir.Multiplied(2.0 * Dr / R2));
    D2Ndir.Add(Ndir.Multiplied(((3.0 * Dr * Dr) / R4) - (D2r / R2)));
    D2Ndir.Multiply(theOffset / R);

    // V1 = P' (U) :
    DNdir.Multiply(R);
    DNdir.Subtract(Ndir.Multiplied(Dr / R));
    DNdir.Multiply(theOffset / R2);
  }
  else
  {
    // Same computation as IICURV in EUCLID-IS because the stability is better.
    // V2 = P" (U) :
    D2Ndir.Multiply(theOffset / R);
    D2Ndir.Subtract(DNdir.Multiplied(2.0 * theOffset * Dr / R3));
    D2Ndir.Add(Ndir.Multiplied(theOffset * (((3.0 * Dr * Dr) / R5) - (D2r / R3))));

    // V1 = P' (U)
    DNdir.Multiply(theOffset / R);
    DNdir.Subtract(Ndir.Multiplied(theOffset * Dr / R3));
  }

  Ndir.Multiply(theOffset / R);
  // P(u)
  theValue.ChangeCoord().Add(Ndir);
  // P'(u) :
  theD1.Add(gp_Vec2d(DNdir));
  // P"(u) :
  if (theIsDirChange)
    theD2.Reverse();
  theD2.Add(gp_Vec2d(D2Ndir));
}

//=================================================================================================

void Geom2dEvaluator::CalculateD3(gp_Pnt2d&              theValue,
                                  gp_Vec2d&              theD1,
                                  gp_Vec2d&              theD2,
                                  gp_Vec2d&              theD3,
                                  const gp_Vec2d&        theD4,
                                  const Standard_Boolean theIsDirChange,
                                  const Standard_Real    theOffset)
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

  gp_XY         Ndir(theD1.Y(), -theD1.X());
  gp_XY         DNdir(theD2.Y(), -theD2.X());
  gp_XY         D2Ndir(theD3.Y(), -theD3.X());
  gp_XY         D3Ndir(theD4.Y(), -theD4.X());
  Standard_Real R2  = Ndir.SquareModulus();
  Standard_Real R   = Sqrt(R2);
  Standard_Real R3  = R2 * R;
  Standard_Real R4  = R2 * R2;
  Standard_Real R5  = R3 * R2;
  Standard_Real R6  = R3 * R3;
  Standard_Real R7  = R5 * R2;
  Standard_Real Dr  = Ndir.Dot(DNdir);
  Standard_Real D2r = Ndir.Dot(D2Ndir) + DNdir.Dot(DNdir);
  Standard_Real D3r = Ndir.Dot(D3Ndir) + 3.0 * DNdir.Dot(D2Ndir);

  if (R7 <= gp::Resolution())
  {
    if (R6 <= gp::Resolution())
      throw Standard_NullValue("Geom2dEvaluator: Null derivative");
    // We try another computation but the stability is not very good dixit ISG.
    //  V3 = P"' (U) :
    D3Ndir.Subtract(D2Ndir.Multiplied(3.0 * theOffset * Dr / R2));
    D3Ndir.Subtract((DNdir.Multiplied((3.0 * theOffset) * ((D2r / R2) + (Dr * Dr) / R4))));
    D3Ndir.Add(Ndir.Multiplied(
      (theOffset * (6.0 * Dr * Dr / R4 + 6.0 * Dr * D2r / R4 - 15.0 * Dr * Dr * Dr / R6 - D3r))));
    D3Ndir.Multiply(theOffset / R);
    // V2 = P" (U) :
    R4 = R2 * R2;
    D2Ndir.Subtract(DNdir.Multiplied(2.0 * Dr / R2));
    D2Ndir.Subtract(Ndir.Multiplied(((3.0 * Dr * Dr) / R4) - (D2r / R2)));
    D2Ndir.Multiply(theOffset / R);
    // V1 = P' (U) :
    DNdir.Multiply(R);
    DNdir.Subtract(Ndir.Multiplied(Dr / R));
    DNdir.Multiply(theOffset / R2);
  }
  else
  {
    // Same computation as IICURV in EUCLID-IS because the stability is better.
    // V3 = P"' (U) :
    D3Ndir.Multiply(theOffset / R);
    D3Ndir.Subtract(D2Ndir.Multiplied(3.0 * theOffset * Dr / R3));
    D3Ndir.Subtract(DNdir.Multiplied(((3.0 * theOffset) * ((D2r / R3) + (Dr * Dr) / R5))));
    D3Ndir.Add(Ndir.Multiplied(
      (theOffset * (6.0 * Dr * Dr / R5 + 6.0 * Dr * D2r / R5 - 15.0 * Dr * Dr * Dr / R7 - D3r))));
    // V2 = P" (U) :
    D2Ndir.Multiply(theOffset / R);
    D2Ndir.Subtract(DNdir.Multiplied(2.0 * theOffset * Dr / R3));
    D2Ndir.Subtract(Ndir.Multiplied(theOffset * (((3.0 * Dr * Dr) / R5) - (D2r / R3))));
    // V1 = P' (U) :
    DNdir.Multiply(theOffset / R);
    DNdir.Subtract(Ndir.Multiplied(theOffset * Dr / R3));
  }

  Ndir.Multiply(theOffset / R);
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
