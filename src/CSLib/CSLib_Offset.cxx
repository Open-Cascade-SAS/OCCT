// Copyright (c) 2015-... OPEN CASCADE SAS
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

#include <CSLib_Offset.hxx>

#include <gp_Dir2d.hxx>
#include <gp_XY.hxx>

#include <Standard_NullValue.hxx>


// ==========  Offset values for 2D curves  ==========

void CSLib_Offset::D0(const gp_Pnt2d&  theBasePoint,
                      const gp_Vec2d&  theBaseDeriv,
                      Standard_Real    theOffset,
                      Standard_Boolean , // unused
                      gp_Pnt2d&        theResPoint)
{
  if (theBaseDeriv.SquareMagnitude() <= gp::Resolution())
    Standard_NullValue::Raise("CSLib_Offset: Undefined normal vector "
                              "because tangent vector has zero-magnitude!");

  gp_Dir2d aNormal(theBaseDeriv.Y(), -theBaseDeriv.X());
  theResPoint.SetCoord(theBasePoint.X() + aNormal.X() * theOffset,
                       theBasePoint.Y() + aNormal.Y() * theOffset);
}

void CSLib_Offset::D1(const gp_Pnt2d& theBasePoint,
                      const gp_Vec2d& theBaseD1,
                      const gp_Vec2d& theBaseD2,
                      Standard_Real   theOffset,
                      Standard_Boolean , // unused
                      gp_Pnt2d&       theResPoint,
                      gp_Vec2d&       theResDeriv)
{
  // P(u) = p(u) + Offset * Ndir / R
  // with R = || p' ^ Z|| and Ndir = P' ^ Z

  // P'(u) = p'(u) + (Offset / R**2) * (DNdir/DU * R -  Ndir * (DR/R))

  gp_XY Ndir(theBaseD1.Y(), -theBaseD1.X());
  gp_XY DNdir(theBaseD2.Y(), -theBaseD2.X());
  Standard_Real R2 = Ndir.SquareModulus();
  Standard_Real R  = Sqrt (R2);
  Standard_Real R3 = R * R2;
  Standard_Real Dr = Ndir.Dot(DNdir);
  if (R3 <= gp::Resolution())
  {
    if (R2 <= gp::Resolution())
      Standard_NullValue::Raise("CSLib_Offset: Null derivative");
    //We try another computation but the stability is not very good.
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

  // P(u)
  D0(theBasePoint, theBaseD1, theOffset, Standard_False, theResPoint);
  // P'(u)
  theResDeriv = theBaseD1.Added(gp_Vec2d(DNdir));
}

void CSLib_Offset::D2(const gp_Pnt2d&  theBasePoint,
                      const gp_Vec2d&  theBaseD1,
                      const gp_Vec2d&  theBaseD2,
                      const gp_Vec2d&  theBaseD3,
                      Standard_Real    theOffset,
                      Standard_Boolean theIsDirectionChange,
                      gp_Pnt2d&        theResPoint,
                      gp_Vec2d&        theResD1,
                      gp_Vec2d&        theResD2)
{
  // P(u) = p(u) + Offset * Ndir / R
  // with R = || p' ^ Z|| and Ndir = P' ^ Z

  // P'(u) = p'(u) + (Offset / R**2) * (DNdir/DU * R -  Ndir * (DR/R))

  // P"(u) = p"(u) + (Offset / R) * (D2Ndir/DU - DNdir * (2.0 * Dr/ R**2) +
  //         Ndir * ( (3.0 * Dr**2 / R**4) - (D2r / R**2)))

  gp_XY Ndir(theBaseD1.Y(), -theBaseD1.X());
  gp_XY DNdir(theBaseD2.Y(), -theBaseD2.X());
  gp_XY D2Ndir(theBaseD3.Y(), -theBaseD3.X());
  Standard_Real R2  = Ndir.SquareModulus();
  Standard_Real R   = Sqrt(R2);
  Standard_Real R3  = R2 * R;
  Standard_Real R4  = R2 * R2;
  Standard_Real R5  = R3 * R2;
  Standard_Real Dr  = Ndir.Dot(DNdir);
  Standard_Real D2r = Ndir.Dot(D2Ndir) + DNdir.Dot (DNdir);
  if (R5 <= gp::Resolution())
  {
    if (R4 <= gp::Resolution())
      Standard_NullValue::Raise("CSLib_Offset: Null derivative");
    //We try another computation but the stability is not very good dixit ISG.
    // V2 = P" (U) :
    D2Ndir.Subtract(DNdir.Multiplied (2.0 * Dr / R2));
    D2Ndir.Add(Ndir.Multiplied (((3.0 * Dr * Dr)/R4) - (D2r/R2)));
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
    D2Ndir.Subtract(DNdir.Multiplied (2.0 * theOffset * Dr / R3));
    D2Ndir.Add (Ndir.Multiplied(theOffset * (((3.0 * Dr * Dr) / R5) - (D2r / R3))));

    // V1 = P' (U) 
    DNdir.Multiply(theOffset / R);
    DNdir.Subtract(Ndir.Multiplied(theOffset * Dr / R3));
  }

  // P(u) :
  D0(theBasePoint, theBaseD1, theOffset, theIsDirectionChange, theResPoint);
  // P'(u) :
  theResD1 = theBaseD1.Added(gp_Vec2d(DNdir));
  // P"(u) :
  if (theIsDirectionChange)
    theResD2 = -theBaseD2;
  else
    theResD2 = theBaseD2;
  theResD2.Add(gp_Vec2d(D2Ndir));
}

void CSLib_Offset::D3(const gp_Pnt2d&  theBasePoint,
                      const gp_Vec2d&  theBaseD1,
                      const gp_Vec2d&  theBaseD2,
                      const gp_Vec2d&  theBaseD3,
                      const gp_Vec2d&  theBaseD4,
                      Standard_Real    theOffset,
                      Standard_Boolean theIsDirectionChange,
                      gp_Pnt2d&        theResPoint,
                      gp_Vec2d&        theResD1,
                      gp_Vec2d&        theResD2,
                      gp_Vec2d&        theResD3)
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

  gp_XY Ndir(theBaseD1.Y(), -theBaseD1.X());
  gp_XY DNdir(theBaseD2.Y(), -theBaseD2.X());
  gp_XY D2Ndir(theBaseD3.Y(), -theBaseD3.X());
  gp_XY D3Ndir(theBaseD4.Y(), -theBaseD4.X());
  Standard_Real R2  = Ndir.SquareModulus();
  Standard_Real R   = Sqrt (R2);
  Standard_Real R3  = R2 * R;
  Standard_Real R4  = R2 * R2;
  Standard_Real R5  = R3 * R2;
  Standard_Real R6  = R3 * R3;
  Standard_Real R7  = R5 * R2;
  Standard_Real Dr  = Ndir.Dot(DNdir);
  Standard_Real D2r = Ndir.Dot(D2Ndir) + DNdir.Dot (DNdir);
  Standard_Real D3r = Ndir.Dot(D3Ndir) + 3.0 * DNdir.Dot (D2Ndir);

  if (R7 <= gp::Resolution()) 
  {
    if (R6 <= gp::Resolution())
      Standard_NullValue::Raise("CSLib_Offset: Null derivative");
    //We try another computation but the stability is not very good dixit ISG.
    // V3 = P"' (U) :
    D3Ndir.Subtract (D2Ndir.Multiplied (3.0 * theOffset * Dr / R2));
    D3Ndir.Subtract (
      (DNdir.Multiplied ((3.0 * theOffset) * ((D2r/R2) + (Dr*Dr)/R4))));
    D3Ndir.Add (Ndir.Multiplied (
      (theOffset * (6.0*Dr*Dr/R4 + 6.0*Dr*D2r/R4 - 15.0*Dr*Dr*Dr/R6 - D3r))));
    D3Ndir.Multiply (theOffset/R);
    // V2 = P" (U) :
    R4 = R2 * R2;
    D2Ndir.Subtract (DNdir.Multiplied (2.0 * Dr / R2));
    D2Ndir.Subtract (Ndir.Multiplied (((3.0 * Dr * Dr)/R4) - (D2r/R2)));
    D2Ndir.Multiply (theOffset / R);
    // V1 = P' (U) :
    DNdir.Multiply(R);
    DNdir.Subtract (Ndir.Multiplied (Dr/R));
    DNdir.Multiply (theOffset/R2);
  }
  else
  {
    // Same computation as IICURV in EUCLID-IS because the stability is better.
    // V3 = P"' (U) :
    D3Ndir.Multiply (theOffset/R);
    D3Ndir.Subtract (D2Ndir.Multiplied (3.0 * theOffset * Dr / R3));
    D3Ndir.Subtract (DNdir.Multiplied (
      ((3.0 * theOffset) * ((D2r/R3) + (Dr*Dr)/R5))) );
    D3Ndir.Add (Ndir.Multiplied (
      (theOffset * (6.0*Dr*Dr/R5 + 6.0*Dr*D2r/R5 - 15.0*Dr*Dr*Dr/R7 - D3r))));
    // V2 = P" (U) :
    D2Ndir.Multiply (theOffset/R);
    D2Ndir.Subtract (DNdir.Multiplied (2.0 * theOffset * Dr / R3));
    D2Ndir.Subtract (Ndir.Multiplied (
      theOffset * (((3.0 * Dr * Dr) / R5) - (D2r / R3))));
    // V1 = P' (U) :
    DNdir.Multiply (theOffset/R);
    DNdir.Subtract (Ndir.Multiplied (theOffset*Dr/R3));
  }

  // P(u)
  D0(theBasePoint, theBaseD1, theOffset, theIsDirectionChange, theResPoint);
  // P'(u)
  theResD1 = theBaseD1.Added(gp_Vec2d(DNdir));
  // P"(u)
  theResD2 = theBaseD2.Added(gp_Vec2d(D2Ndir));
  // P"'(u)
  if (theIsDirectionChange)
    theResD3 = -theBaseD3;
  else
    theResD3 = theBaseD3;
  theResD3.Add(gp_Vec2d(D2Ndir));
}


// ==========  Offset values for 3D curves  ==========

void CSLib_Offset::D0(const gp_Pnt&    theBasePoint,
                      const gp_Vec&    theBaseDeriv,
                      const gp_Dir&    theOffsetDirection,
                      Standard_Real    theOffsetValue,
                      Standard_Boolean , // unused
                      gp_Pnt&          theResPoint)
{
  gp_XYZ Ndir = (theBaseDeriv.XYZ()).Crossed(theOffsetDirection.XYZ());
  Standard_Real R = Ndir.Modulus();
  if (R <= gp::Resolution())
    Standard_NullValue::Raise("CSLib_Offset: Undefined normal vector "
          "because tangent vector has zero-magnitude!");

  Ndir.Multiply(theOffsetValue / R);
  Ndir.Add(theBasePoint.XYZ());
  theResPoint.SetXYZ(Ndir);
}

void CSLib_Offset::D1(const gp_Pnt&   theBasePoint,
                      const gp_Vec&   theBaseD1,
                      const gp_Vec&   theBaseD2,
                      const gp_Dir&   theOffsetDirection,
                      Standard_Real   theOffsetValue,
                      Standard_Boolean , // unused
                      gp_Pnt&         theResPoint,
                      gp_Vec&         theResDeriv)
{
  // P(u) = p(u) + Offset * Ndir / R
  // with R = || p' ^ V|| and Ndir = P' ^ direction (local normal direction)

  // P'(u) = p'(u) + (Offset / R**2) * (DNdir/DU * R -  Ndir * (DR/R))

  gp_XYZ Ndir  = (theBaseD1.XYZ()).Crossed(theOffsetDirection.XYZ());
  gp_XYZ DNdir = (theBaseD2.XYZ()).Crossed(theOffsetDirection.XYZ());
  Standard_Real R2 = Ndir.SquareModulus();
  Standard_Real R  = Sqrt (R2);
  Standard_Real R3 = R * R2;
  Standard_Real Dr = Ndir.Dot (DNdir);
  if (R3 <= gp::Resolution()) {
    if (R2 <= gp::Resolution())
      Standard_NullValue::Raise("CSLib_Offset: Null derivative");
    //We try another computation but the stability is not very good.
    DNdir.Multiply(R);
    DNdir.Subtract(Ndir.Multiplied(Dr / R));
    DNdir.Multiply(theOffsetValue / R2);
  }
  else {
    // Same computation as IICURV in EUCLID-IS because the stability is
    // better
    DNdir.Multiply(theOffsetValue / R);
    DNdir.Subtract(Ndir.Multiplied(theOffsetValue * Dr / R3));
  }

  // P(u)
  D0(theBasePoint, theBaseD1, theOffsetDirection, theOffsetValue, Standard_False, theResPoint);
  // P'(u)
  theResDeriv = theBaseD1.Added(gp_Vec(DNdir));
}

void CSLib_Offset::D2(const gp_Pnt&    theBasePoint,
                      const gp_Vec&    theBaseD1,
                      const gp_Vec&    theBaseD2,
                      const gp_Vec&    theBaseD3,
                      const gp_Dir&    theOffsetDirection,
                      Standard_Real    theOffsetValue,
                      Standard_Boolean theIsDirectionChange,
                      gp_Pnt&          theResPoint,
                      gp_Vec&          theResD1,
                      gp_Vec&          theResD2)
{
  // P(u) = p(u) + Offset * Ndir / R
  // with R = || p' ^ V|| and Ndir = P' ^ direction (local normal direction)

  // P'(u) = p'(u) + (Offset / R**2) * (DNdir/DU * R -  Ndir * (DR/R))

  // P"(u) = p"(u) + (Offset / R) * (D2Ndir/DU - DNdir * (2.0 * Dr/ R**2) +
  //         Ndir * ( (3.0 * Dr**2 / R**4) - (D2r / R**2)))

  gp_XYZ Ndir   = (theBaseD1.XYZ()).Crossed(theOffsetDirection.XYZ());
  gp_XYZ DNdir  = (theBaseD2.XYZ()).Crossed(theOffsetDirection.XYZ());
  gp_XYZ D2Ndir = (theBaseD3.XYZ()).Crossed(theOffsetDirection.XYZ());
  Standard_Real R2  = Ndir.SquareModulus();
  Standard_Real R   = Sqrt (R2);
  Standard_Real R3  = R2 * R;
  Standard_Real R4  = R2 * R2;
  Standard_Real R5  = R3 * R2;
  Standard_Real Dr  = Ndir.Dot (DNdir);
  Standard_Real D2r = Ndir.Dot (D2Ndir) + DNdir.Dot (DNdir);
  
  if (R5 <= gp::Resolution()) {
    if (R4 <= gp::Resolution())
      Standard_NullValue::Raise("CSLib_Offset: Null derivative");
    //We try another computation but the stability is not very good
    //dixit ISG.
    // V2 = P" (U) :
    R4 = R2 * R2;
    D2Ndir.Subtract (DNdir.Multiplied (2.0 * Dr / R2));
    D2Ndir.Add (Ndir.Multiplied (((3.0 * Dr * Dr)/R4) - (D2r/R2)));
    D2Ndir.Multiply (theOffsetValue / R);

    // V1 = P' (U) :
    DNdir.Multiply(R);
    DNdir.Subtract (Ndir.Multiplied (Dr/R));
    DNdir.Multiply (theOffsetValue/R2);
  }
  else {
    // Same computation as IICURV in EUCLID-IS because the stability is
    // better.
    // V2 = P" (U) :
    D2Ndir.Multiply (theOffsetValue/R);
    D2Ndir.Subtract (DNdir.Multiplied (2.0 * theOffsetValue * Dr / R3));
    D2Ndir.Add (Ndir.Multiplied (theOffsetValue * (((3.0 * Dr * Dr) / R5) - (D2r / R3))));

    // V1 = P' (U) :
    DNdir.Multiply (theOffsetValue/R);
    DNdir.Subtract (Ndir.Multiplied (theOffsetValue*Dr/R3));        
  }

  // P(u) :
  D0(theBasePoint, theBaseD1, theOffsetDirection, theOffsetValue, theIsDirectionChange, theResPoint);
  // P'(u) :
  theResD1 = theBaseD1.Added(gp_Vec(DNdir));
  // P"(u) :
  if (theIsDirectionChange)
    theResD2 = -theBaseD2;
  else
    theResD2 = theBaseD2;
  theResD2.Add(gp_Vec(D2Ndir));
}

void CSLib_Offset::D3(const gp_Pnt&    theBasePoint,
                      const gp_Vec&    theBaseD1,
                      const gp_Vec&    theBaseD2,
                      const gp_Vec&    theBaseD3,
                      const gp_Vec&    theBaseD4,
                      const gp_Dir&    theOffsetDirection,
                      Standard_Real    theOffsetValue,
                      Standard_Boolean theIsDirectionChange,
                      gp_Pnt&          theResPoint,
                      gp_Vec&          theResD1,
                      gp_Vec&          theResD2,
                      gp_Vec&          theResD3)
{
  // P(u) = p(u) + Offset * Ndir / R
  // with R = || p' ^ V|| and Ndir = P' ^ direction (local normal direction)

  // P'(u) = p'(u) + (Offset / R**2) * (DNdir/DU * R -  Ndir * (DR/R))

  // P"(u) = p"(u) + (Offset / R) * (D2Ndir/DU - DNdir * (2.0 * Dr/ R**2) +
  //         Ndir * ( (3.0 * Dr**2 / R**4) - (D2r / R**2)))

  //P"'(u) = p"'(u) + (Offset / R) * (D3Ndir - (3.0 * Dr/R**2) * D2Ndir -
  //         (3.0 * D2r / R2) * DNdir + (3.0 * Dr * Dr / R4) * DNdir -
  //         (D3r/R2) * Ndir + (6.0 * Dr * Dr / R4) * Ndir +
  //         (6.0 * Dr * D2r / R4) * Ndir - (15.0 * Dr* Dr* Dr /R6) * Ndir

  gp_XYZ Ndir   = (theBaseD1.XYZ()).Crossed(theOffsetDirection.XYZ());
  gp_XYZ DNdir  = (theBaseD2.XYZ()).Crossed(theOffsetDirection.XYZ());
  gp_XYZ D2Ndir = (theBaseD3.XYZ()).Crossed(theOffsetDirection.XYZ());
  gp_XYZ D3Ndir = (theBaseD4.XYZ()).Crossed(theOffsetDirection.XYZ());
  Standard_Real R2  = Ndir.SquareModulus();
  Standard_Real R   = Sqrt (R2);
  Standard_Real R3  = R2 * R;
  Standard_Real R4  = R2 * R2;
  Standard_Real R5  = R3 * R2;
  Standard_Real R6  = R3 * R3;
  Standard_Real R7  = R5 * R2;
  Standard_Real Dr  = Ndir.Dot (DNdir);
  Standard_Real D2r = Ndir.Dot (D2Ndir) + DNdir.Dot (DNdir);
  Standard_Real D3r = Ndir.Dot (D3Ndir) + 3.0 * DNdir.Dot (D2Ndir);
  if (R7 <= gp::Resolution()) {
    if (R6 <= gp::Resolution())
      Standard_NullValue::Raise("CSLib_Offset: Null derivative");
    // V3 = P"' (U) :
    D3Ndir.Subtract (D2Ndir.Multiplied (3.0 * Dr / R2));
    D3Ndir.Subtract (DNdir.Multiplied (3.0 * ((D2r/R2) + (Dr*Dr/R4))));
    D3Ndir.Add (Ndir.Multiplied (6.0*Dr*Dr/R4 + 6.0*Dr*D2r/R4 - 15.0*Dr*Dr*Dr/R6 - D3r));
    D3Ndir.Multiply (theOffsetValue/R);
    // V2 = P" (U) :
    R4 = R2 * R2;
    D2Ndir.Subtract (DNdir.Multiplied (2.0 * Dr / R2));
    D2Ndir.Subtract (Ndir.Multiplied ((3.0 * Dr * Dr / R4) - (D2r / R2)));
    D2Ndir.Multiply (theOffsetValue / R);
    // V1 = P' (U) :
    DNdir.Multiply(R);
    DNdir.Subtract (Ndir.Multiplied (Dr/R));
    DNdir.Multiply (theOffsetValue/R2);
  }
  else {
    // V3 = P"' (U) :
    D3Ndir.Divide (R);
    D3Ndir.Subtract (D2Ndir.Multiplied (3.0 * Dr / R3));
    D3Ndir.Subtract (DNdir.Multiplied ((3.0 * ((D2r/R3) + (Dr*Dr)/R5))));
    D3Ndir.Add (Ndir.Multiplied (6.0*Dr*Dr/R5 + 6.0*Dr*D2r/R5 - 15.0*Dr*Dr*Dr/R7 - D3r));
    D3Ndir.Multiply (theOffsetValue);
    // V2 = P" (U) :
    D2Ndir.Divide (R);
    D2Ndir.Subtract (DNdir.Multiplied (2.0 * Dr / R3));
    D2Ndir.Subtract (Ndir.Multiplied ((3.0 * Dr * Dr / R5) - (D2r / R3)));
    D2Ndir.Multiply (theOffsetValue);
    // V1 = P' (U) :
    DNdir.Multiply (theOffsetValue/R);
    DNdir.Subtract (Ndir.Multiplied (theOffsetValue*Dr/R3));
  }

  // P(u)
  D0(theBasePoint, theBaseD1, theOffsetDirection, theOffsetValue, theIsDirectionChange, theResPoint);
  // P'(u)
  theResD1 = theBaseD1.Added(gp_Vec(DNdir));
  // P"(u)
  theResD2 = theBaseD2.Added(gp_Vec(D2Ndir));
  // P"'(u)
  if (theIsDirectionChange)
    theResD3 = -theBaseD3;
  else
    theResD3 = theBaseD3;
  theResD3.Add(gp_Vec(D2Ndir));
}

