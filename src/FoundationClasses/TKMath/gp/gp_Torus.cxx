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

#include <gp_Torus.hxx>

#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <Standard_DimensionError.hxx>

#include <cmath>

void gp_Torus::Coefficients(NCollection_Array1<double>& theCoef) const
{
  //  R = majorRadius;
  //  r = minorRadius.

  //  X = (R + r*cos(V))*cos(U)
  //  Y = (R + r*cos(V))*sin(U)
  //  Z = r*sin(V)

  // Therefore,
  //   4*R*R*(r*r - Z*Z) = (X*X + Y*Y + Z*Z - R*R - r*r)^2
  // Or
  //   X^4+Y^4+Z^4+
  //   2*((X*Y)^2+(X*Z)^2+(Y*Z)^2)-
  //   2*(R^2+r^2)*(X^2+Y^2)+
  //   2*(R^2-r^2)*Z^2+(R^2-r^2)^2 = 0.0

  Standard_DimensionError_Raise_if(theCoef.Size() < 35,
                                   "gp_Torus::theCoefficients(): Dimension mismatch");

  // Expand (|P-O|^2 - R^2 - r^2)^2 + 4*R^2*((P-O).N)^2 - 4*R^2*r^2.
  // The quartic part is rotationally invariant; only the quadratic axis
  // term depends on orientation.
  const gp_XYZ& aCenter    = pos.Location().XYZ();
  const gp_XYZ& anAxis     = pos.Direction().XYZ();
  const double  aR2        = majorRadius * majorRadius;
  const double  ar2        = minorRadius * minorRadius;
  const double  aCenter2   = aCenter.SquareModulus();
  const double  aSumRadius = aR2 + ar2;
  const double  aDelta     = (majorRadius - minorRadius) * (majorRadius + minorRadius);
  const double  aQ         = aCenter2 - aSumRadius;
  const double  aH         = aCenter.Dot(anAxis);
  for (size_t anIndex = 0; anIndex < 35; ++anIndex)
  {
    theCoef.ChangeAt(anIndex) = 0.0;
  }
  theCoef.ChangeAt(0) = theCoef.ChangeAt(1) = theCoef.ChangeAt(2) = 1.0;
  theCoef.ChangeAt(9) = theCoef.ChangeAt(10) = theCoef.ChangeAt(11) = 2.0;
  const double aX = aCenter.X(), aY = aCenter.Y(), aZ = aCenter.Z();
  const double aNX = anAxis.X(), aNY = anAxis.Y(), aNZ = anAxis.Z();
  theCoef.ChangeAt(15) = -4.0 * aX;
  theCoef.ChangeAt(16) = -4.0 * aY;
  theCoef.ChangeAt(17) = -4.0 * aZ;
  theCoef.ChangeAt(18) = -4.0 * aY;
  theCoef.ChangeAt(19) = -4.0 * aZ;
  theCoef.ChangeAt(20) = -4.0 * aX;
  theCoef.ChangeAt(21) = -4.0 * aZ;
  theCoef.ChangeAt(22) = -4.0 * aX;
  theCoef.ChangeAt(23) = -4.0 * aY;
  theCoef.ChangeAt(25) = 2.0 * aQ + 4.0 * aX * aX + 4.0 * aR2 * aNX * aNX;
  theCoef.ChangeAt(26) = 2.0 * aQ + 4.0 * aY * aY + 4.0 * aR2 * aNY * aNY;
  theCoef.ChangeAt(27) = 2.0 * aQ + 4.0 * aZ * aZ + 4.0 * aR2 * aNZ * aNZ;
  theCoef.ChangeAt(28) = 8.0 * (aX * aY + aR2 * aNX * aNY);
  theCoef.ChangeAt(29) = 8.0 * (aX * aZ + aR2 * aNX * aNZ);
  theCoef.ChangeAt(30) = 8.0 * (aY * aZ + aR2 * aNY * aNZ);
  theCoef.ChangeAt(31) = -4.0 * aQ * aX - 8.0 * aR2 * aH * aNX;
  theCoef.ChangeAt(32) = -4.0 * aQ * aY - 8.0 * aR2 * aH * aNY;
  theCoef.ChangeAt(33) = -4.0 * aQ * aZ - 8.0 * aR2 * aH * aNZ;
  theCoef.ChangeAt(34) =
    std::fma(aCenter2, aCenter2 - 2.0 * aSumRadius, std::fma(4.0 * aR2, aH * aH, aDelta * aDelta));
}

void gp_Torus::Mirror(const gp_Pnt& P) noexcept
{
  pos.Mirror(P);
}

gp_Torus gp_Torus::Mirrored(const gp_Pnt& P) const noexcept
{
  gp_Torus C = *this;
  C.pos.Mirror(P);
  return C;
}

void gp_Torus::Mirror(const gp_Ax1& A1) noexcept
{
  pos.Mirror(A1);
}

gp_Torus gp_Torus::Mirrored(const gp_Ax1& A1) const noexcept
{
  gp_Torus C = *this;
  C.pos.Mirror(A1);
  return C;
}

void gp_Torus::Mirror(const gp_Ax2& A2) noexcept
{
  pos.Mirror(A2);
}

gp_Torus gp_Torus::Mirrored(const gp_Ax2& A2) const noexcept
{
  gp_Torus C = *this;
  C.pos.Mirror(A2);
  return C;
}
