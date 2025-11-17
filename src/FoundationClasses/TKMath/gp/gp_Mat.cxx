// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef OCCT_DEBUG
  #define No_Standard_OutOfRange
  #define No_Standard_ConstructionError
#endif

#include <gp_Mat.hxx>

#include <gp_GTrsf.hxx>
#include <gp_XYZ.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Dump.hxx>

//=================================================================================================

gp_Mat::gp_Mat(const gp_XYZ& theCol1, const gp_XYZ& theCol2, const gp_XYZ& theCol3)
{
  myMat[0][0] = theCol1.X();
  myMat[1][0] = theCol1.Y();
  myMat[2][0] = theCol1.Z();
  myMat[0][1] = theCol2.X();
  myMat[1][1] = theCol2.Y();
  myMat[2][1] = theCol2.Z();
  myMat[0][2] = theCol3.X();
  myMat[1][2] = theCol3.Y();
  myMat[2][2] = theCol3.Z();
}

//=================================================================================================

void gp_Mat::SetCol(const Standard_Integer theCol, const gp_XYZ& theValue)
{
  Standard_OutOfRange_Raise_if(theCol < 1 || theCol > 3, " ");
  if (theCol == 1)
  {
    myMat[0][0] = theValue.X();
    myMat[1][0] = theValue.Y();
    myMat[2][0] = theValue.Z();
  }
  else if (theCol == 2)
  {
    myMat[0][1] = theValue.X();
    myMat[1][1] = theValue.Y();
    myMat[2][1] = theValue.Z();
  }
  else
  {
    myMat[0][2] = theValue.X();
    myMat[1][2] = theValue.Y();
    myMat[2][2] = theValue.Z();
  }
}

//=================================================================================================

void gp_Mat::SetCols(const gp_XYZ& theCol1, const gp_XYZ& theCol2, const gp_XYZ& theCol3)
{
  myMat[0][0] = theCol1.X();
  myMat[1][0] = theCol1.Y();
  myMat[2][0] = theCol1.Z();
  myMat[0][1] = theCol2.X();
  myMat[1][1] = theCol2.Y();
  myMat[2][1] = theCol2.Z();
  myMat[0][2] = theCol3.X();
  myMat[1][2] = theCol3.Y();
  myMat[2][2] = theCol3.Z();
}

//=================================================================================================

void gp_Mat::SetCross(const gp_XYZ& theRef)
{
  const Standard_Real X = theRef.X();
  const Standard_Real Y = theRef.Y();
  const Standard_Real Z = theRef.Z();
  myMat[0][0] = myMat[1][1] = myMat[2][2] = 0.0;

  myMat[0][1] = -Z;
  myMat[0][2] = Y;
  myMat[1][2] = -X;
  myMat[1][0] = Z;
  myMat[2][0] = -Y;
  myMat[2][1] = X;
}

//=================================================================================================

void gp_Mat::SetDot(const gp_XYZ& theRef) noexcept
{
  const Standard_Real X = theRef.X();
  const Standard_Real Y = theRef.Y();
  const Standard_Real Z = theRef.Z();

  myMat[0][0] = X * X;
  myMat[1][1] = Y * Y;
  myMat[2][2] = Z * Z;
  myMat[0][1] = X * Y;
  myMat[0][2] = X * Z;
  myMat[1][2] = Y * Z;

  myMat[1][0] = myMat[0][1];
  myMat[2][0] = myMat[0][2];
  myMat[2][1] = myMat[1][2];
}

//=================================================================================================

void gp_Mat::SetRotation(const gp_XYZ& theAxis, const Standard_Real theAng)
{
  // Rodrigues' rotation formula: R = I + sin(theta)K + (1-cos(theta))K^2
  // Where K is the skew-symmetric matrix of the normalized axis
  const gp_XYZ aV = theAxis.Normalized();

  const Standard_Real A = aV.X();
  const Standard_Real B = aV.Y();
  const Standard_Real C = aV.Z();

  // Precompute trigonometric values
  const Standard_Real aCos   = cos(theAng);
  const Standard_Real aSin   = sin(theAng);
  const Standard_Real aOmCos = 1.0 - aCos; // One minus cosine

  // Precompute terms
  const Standard_Real A2 = A * A;
  const Standard_Real B2 = B * B;
  const Standard_Real C2 = C * C;
  const Standard_Real AB = A * B;
  const Standard_Real AC = A * C;
  const Standard_Real BC = B * C;

  // Direct matrix computation: R = I + sin(theta)K + (1-cos(theta))K^2
  // K^2 diagonal terms are -(sum of other two squared components)
  // K^2 off-diagonal terms are products of components
  myMat[0][0] = 1.0 + aOmCos * (-(B2 + C2));
  myMat[0][1] = aOmCos * AB - aSin * C;
  myMat[0][2] = aOmCos * AC + aSin * B;

  myMat[1][0] = aOmCos * AB + aSin * C;
  myMat[1][1] = 1.0 + aOmCos * (-(A2 + C2));
  myMat[1][2] = aOmCos * BC - aSin * A;

  myMat[2][0] = aOmCos * AC - aSin * B;
  myMat[2][1] = aOmCos * BC + aSin * A;
  myMat[2][2] = 1.0 + aOmCos * (-(A2 + B2));
}

//=================================================================================================

void gp_Mat::SetRow(const Standard_Integer theRow, const gp_XYZ& theValue)
{
  Standard_OutOfRange_Raise_if(theRow < 1 || theRow > 3, " ");
  if (theRow == 1)
  {
    myMat[0][0] = theValue.X();
    myMat[0][1] = theValue.Y();
    myMat[0][2] = theValue.Z();
  }
  else if (theRow == 2)
  {
    myMat[1][0] = theValue.X();
    myMat[1][1] = theValue.Y();
    myMat[1][2] = theValue.Z();
  }
  else
  {
    myMat[2][0] = theValue.X();
    myMat[2][1] = theValue.Y();
    myMat[2][2] = theValue.Z();
  }
}

//=================================================================================================

void gp_Mat::SetRows(const gp_XYZ& theRow1, const gp_XYZ& theRow2, const gp_XYZ& theRow3)
{
  myMat[0][0] = theRow1.X();
  myMat[0][1] = theRow1.Y();
  myMat[0][2] = theRow1.Z();
  myMat[1][0] = theRow2.X();
  myMat[1][1] = theRow2.Y();
  myMat[1][2] = theRow2.Z();
  myMat[2][0] = theRow3.X();
  myMat[2][1] = theRow3.Y();
  myMat[2][2] = theRow3.Z();
}

//=================================================================================================

gp_XYZ gp_Mat::Column(const Standard_Integer theCol) const
{
  Standard_OutOfRange_Raise_if(theCol < 1 || theCol > 3, "gp_Mat::Column() - wrong index");
  if (theCol == 1)
    return gp_XYZ(myMat[0][0], myMat[1][0], myMat[2][0]);
  if (theCol == 2)
    return gp_XYZ(myMat[0][1], myMat[1][1], myMat[2][1]);
  return gp_XYZ(myMat[0][2], myMat[1][2], myMat[2][2]);
}

//=================================================================================================

gp_XYZ gp_Mat::Diagonal() const
{
  return gp_XYZ(myMat[0][0], myMat[1][1], myMat[2][2]);
}

//=================================================================================================

gp_XYZ gp_Mat::Row(const Standard_Integer theRow) const
{
  Standard_OutOfRange_Raise_if(theRow < 1 || theRow > 3, "gp_Mat::Row() - wrong index");
  if (theRow == 1)
    return gp_XYZ(myMat[0][0], myMat[0][1], myMat[0][2]);
  if (theRow == 2)
    return gp_XYZ(myMat[1][0], myMat[1][1], myMat[1][2]);
  return gp_XYZ(myMat[2][0], myMat[2][1], myMat[2][2]);
}

//=================================================================================================

void gp_Mat::Invert()
{
  // Optimized matrix inversion using cached elements
  const Standard_Real a00 = myMat[0][0], a01 = myMat[0][1], a02 = myMat[0][2];
  const Standard_Real a10 = myMat[1][0], a11 = myMat[1][1], a12 = myMat[1][2];
  const Standard_Real a20 = myMat[2][0], a21 = myMat[2][1], a22 = myMat[2][2];

  // Compute adjugate matrix (transpose of cofactor matrix)
  const Standard_Real adj00 = a11 * a22 - a12 * a21;
  const Standard_Real adj10 = a12 * a20 - a10 * a22;
  const Standard_Real adj20 = a10 * a21 - a11 * a20;
  const Standard_Real adj01 = a02 * a21 - a01 * a22;
  const Standard_Real adj11 = a00 * a22 - a02 * a20;
  const Standard_Real adj21 = a01 * a20 - a00 * a21;
  const Standard_Real adj02 = a01 * a12 - a02 * a11;
  const Standard_Real adj12 = a02 * a10 - a00 * a12;
  const Standard_Real adj22 = a00 * a11 - a01 * a10;

  // Compute determinant using first row expansion (reuse computed cofactors)
  const Standard_Real aDet = a00 * adj00 + a01 * adj10 + a02 * adj20;

  Standard_ConstructionError_Raise_if(std::abs(aDet) <= gp::Resolution(),
                                      "gp_Mat::Invert() - matrix has zero determinant");

  // Compute inverse: inv(A) = adj(A) / det(A)
  const Standard_Real aInvDet = 1.0 / aDet;

  // Direct assignment with scaling
  myMat[0][0] = adj00 * aInvDet;
  myMat[1][0] = adj10 * aInvDet;
  myMat[2][0] = adj20 * aInvDet;
  myMat[0][1] = adj01 * aInvDet;
  myMat[1][1] = adj11 * aInvDet;
  myMat[2][1] = adj21 * aInvDet;
  myMat[0][2] = adj02 * aInvDet;
  myMat[1][2] = adj12 * aInvDet;
  myMat[2][2] = adj22 * aInvDet;
}

//=================================================================================================

gp_Mat gp_Mat::Inverted() const
{
  gp_Mat aNewMat(*this);
  aNewMat.Invert();
  return aNewMat;
}

//=================================================================================================

void gp_Mat::Power(const Standard_Integer theN)
{
  // Optimized matrix exponentiation using fast binary exponentiation
  if (theN == 0)
  {
    SetIdentity();
    return;
  }

  if (theN == 1)
  {
    return; // Matrix is already this^1
  }

  if (theN == -1)
  {
    Invert();
    return;
  }

  // Handle negative powers: A^(-n) = (A^(-1))^n
  const Standard_Boolean isNegative = (theN < 0);
  if (isNegative)
  {
    Invert();
  }

  // Fast binary exponentiation for |theN| > 1
  Standard_Integer power = isNegative ? -theN : theN;
  gp_Mat           aBase = *this; // Base for exponentiation
  SetIdentity();                  // Result starts as identity

  // Binary exponentiation: repeatedly square base and multiply when bit is set
  while (power > 0)
  {
    if (power & 1) // If current bit is 1
    {
      Multiply(aBase);
    }
    aBase.Multiply(aBase); // Square the base
    power >>= 1;           // Move to next bit
  }
}

//=================================================================================================

void gp_Mat::DumpJson(Standard_OStream& theOStream, Standard_Integer) const
{
  OCCT_DUMP_VECTOR_CLASS(theOStream,
                         "gp_Mat",
                         9,
                         myMat[0][0],
                         myMat[0][1],
                         myMat[0][2],
                         myMat[1][0],
                         myMat[1][1],
                         myMat[1][2],
                         myMat[2][0],
                         myMat[2][1],
                         myMat[2][2])
}
