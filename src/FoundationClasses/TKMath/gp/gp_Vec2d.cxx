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

// JCV 08/01/90 Modifs suite a l'introduction des classes XY et Mat2d dans gp

#define No_Standard_OutOfRange

#include <gp_Vec2d.hxx>

#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_VectorWithNullMagnitude.hxx>
#include <gp_XY.hxx>

bool gp_Vec2d::IsEqual(const gp_Vec2d&     theOther,
                                   const double theLinearTolerance,
                                   const double theAngularTolerance) const
{
  const double aNorm       = Magnitude();
  const double anOtherNorm = theOther.Magnitude();
  const double aVal        = std::abs(aNorm - anOtherNorm);
  // Check for equal lengths
  const bool isEqualLength = (aVal <= theLinearTolerance);
  // Check for small vectors
  if (aNorm > theLinearTolerance && anOtherNorm > theLinearTolerance)
  {
    const double anAng = std::abs(Angle(theOther));
    // Check for zero angle
    return isEqualLength && (anAng <= theAngularTolerance);
  }
  return isEqualLength;
}

double gp_Vec2d::Angle(const gp_Vec2d& theOther) const
{
  //    Comments:
  //    Above 45 degrees arccos gives the best precision for the
  //    angle calculation. Otherwise it is better to use arcsin.
  //    The errors made are far from negligible when we are
  //    close to zero or 90 degrees.
  //    In 2D the angular values are between -PI and PI
  const double aNorm       = Magnitude();
  const double anOtherNorm = theOther.Magnitude();
  if (aNorm <= gp::Resolution() || anOtherNorm <= gp::Resolution())
    throw gp_VectorWithNullMagnitude();

  const double aD       = aNorm * anOtherNorm;
  const double aCosinus = coord.Dot(theOther.coord) / aD;
  const double aSinus   = coord.Crossed(theOther.coord) / aD;

  // Use M_SQRT1_2 (1/sqrt(2) approximately 0.7071067811865476) for better readability and precision
  constexpr double aCOS_45_DEG = M_SQRT1_2;

  if (aCosinus > -aCOS_45_DEG && aCosinus < aCOS_45_DEG)
  {
    // For angles near +/-90 degrees, use acos for better precision
    return (aSinus > 0.0) ? acos(aCosinus) : -acos(aCosinus);
  }
  else
  {
    // For angles near 0 degrees or +/-180 degrees, use asin for better precision
    if (aCosinus > 0.0)
      return asin(aSinus);
    else
      return (aSinus > 0.0) ? M_PI - asin(aSinus) : -M_PI - asin(aSinus);
  }
}

void gp_Vec2d::Mirror(const gp_Ax2d& theA1)
{
  const gp_XY&        aDirectionXY = theA1.Direction().XY();
  const double aOrigX       = coord.X();
  const double aOrigY       = coord.Y();
  const double aDirX        = aDirectionXY.X();
  const double aDirY        = aDirectionXY.Y();

  // Precompute common terms for reflection matrix
  const double aCrossTerm = 2.0 * aDirX * aDirY;
  const double aXXTerm    = 2.0 * aDirX * aDirX - 1.0;
  const double aYYTerm    = 2.0 * aDirY * aDirY - 1.0;

  coord.SetX(aXXTerm * aOrigX + aCrossTerm * aOrigY);
  coord.SetY(aCrossTerm * aOrigX + aYYTerm * aOrigY);
}

gp_Vec2d gp_Vec2d::Mirrored(const gp_Ax2d& theA1) const
{
  gp_Vec2d Vres = *this;
  Vres.Mirror(theA1);
  return Vres;
}

void gp_Vec2d::Transform(const gp_Trsf2d& theT) noexcept
{
  switch (theT.Form())
  {
    case gp_Identity:
    case gp_Translation:
      break;

    case gp_PntMirror:
      coord.Reverse();
      break;

    case gp_Scale:
      coord.Multiply(theT.ScaleFactor());
      break;

    default:
      coord.Multiply(theT.VectorialPart());
  }
}

void gp_Vec2d::Mirror(const gp_Vec2d& theV)
{
  const double aMagnitude = theV.coord.Modulus();
  if (aMagnitude > gp::Resolution())
  {
    const gp_XY&        aMirrorVecXY = theV.coord;
    const double aOrigX       = coord.X();
    const double aOrigY       = coord.Y();

    // Normalize the mirror vector components
    const double aNormDirX = aMirrorVecXY.X() / aMagnitude;
    const double aNormDirY = aMirrorVecXY.Y() / aMagnitude;

    // Precompute common terms for reflection matrix
    const double aCrossTerm = 2.0 * aNormDirX * aNormDirY;
    const double aXXTerm    = 2.0 * aNormDirX * aNormDirX - 1.0;
    const double aYYTerm    = 2.0 * aNormDirY * aNormDirY - 1.0;

    coord.SetX(aXXTerm * aOrigX + aCrossTerm * aOrigY);
    coord.SetY(aCrossTerm * aOrigX + aYYTerm * aOrigY);
  }
}

gp_Vec2d gp_Vec2d::Mirrored(const gp_Vec2d& theV) const
{
  gp_Vec2d aVres = *this;
  aVres.Mirror(theV);
  return aVres;
}
