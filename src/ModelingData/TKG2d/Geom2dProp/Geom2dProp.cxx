// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <Geom2dProp.hxx>

#include <cmath>

//==================================================================================================

Geom2dProp::TangentResult Geom2dProp::ComputeTangent(const gp_Vec2d& theD1,
                                                     const gp_Vec2d& theD2,
                                                     const gp_Vec2d& theD3,
                                                     const double    theTol)
{
  const double aTol2 = theTol * theTol;

  // Try first derivative
  if (theD1.SquareMagnitude() > aTol2)
  {
    return {gp_Dir2d(theD1), true};
  }

  // Try second derivative
  if (theD2.SquareMagnitude() > aTol2)
  {
    return {gp_Dir2d(theD2), true};
  }

  // Try third derivative
  if (theD3.SquareMagnitude() > aTol2)
  {
    return {gp_Dir2d(theD3), true};
  }

  return {{}, false};
}

//==================================================================================================

Geom2dProp::TangentResult Geom2dProp::ComputeTangent(const gp_Vec2d& theD1,
                                                     const gp_Vec2d& theD2,
                                                     const gp_Vec2d& theD3,
                                                     const double    theTol,
                                                     const gp_Pnt2d& thePntBefore,
                                                     const gp_Pnt2d& thePntAfter)
{
  const double aTol2 = theTol * theTol;

  // If D1 is non-null, use it directly (no sign ambiguity).
  if (theD1.SquareMagnitude() > aTol2)
  {
    return {gp_Dir2d(theD1), true};
  }

  // D1 is null: find first non-null higher derivative.
  gp_Vec2d aVec;
  if (theD2.SquareMagnitude() > aTol2)
    aVec = theD2;
  else if (theD3.SquareMagnitude() > aTol2)
    aVec = theD3;
  else
    return {{}, false};

  // Correct sign of higher-order derivative using finite-difference direction.
  const gp_Vec2d aFiniteDiff(thePntBefore, thePntAfter);
  if (aVec.Dot(aFiniteDiff) < 0.0)
    aVec.Reverse();

  return {gp_Dir2d(aVec), true};
}

//==================================================================================================

Geom2dProp::CurvatureResult Geom2dProp::ComputeCurvature(const gp_Vec2d& theD1,
                                                         const gp_Vec2d& theD2,
                                                         const double    theTol)
{
  const double aTol2 = theTol * theTol;
  const double aDD1  = theD1.SquareMagnitude();

  // If first derivative is null, curvature is infinite (singular point).
  if (aDD1 <= aTol2)
  {
    return {0.0, true, true};
  }

  const double aDD2 = theD2.SquareMagnitude();

  // If second derivative is null, curvature is zero.
  if (aDD2 <= aTol2)
  {
    return {0.0, true, false};
  }

  // Cross magnitude squared: |D1 x D2|^2
  const double aN = theD1.CrossSquareMagnitude(theD2);

  // If D1 and D2 are collinear, curvature is zero.
  const double aT = aN / aDD1 / aDD2;
  if (aT <= aTol2)
  {
    return {0.0, true, false};
  }

  // Curvature = |D1 x D2| / |D1|^3
  const double aCurvature = std::sqrt(aN) / aDD1 / std::sqrt(aDD1);
  return {aCurvature, true, false};
}

//==================================================================================================

Geom2dProp::NormalResult Geom2dProp::ComputeNormal(const gp_Vec2d& theD1,
                                                   const gp_Vec2d& theD2,
                                                   const double    theTol)
{
  // First compute curvature to check if normal is defined.
  const CurvatureResult aCurvRes = ComputeCurvature(theD1, theD2, theTol);
  if (!aCurvRes.IsDefined || aCurvRes.IsInfinite || std::abs(aCurvRes.Value) <= theTol)
  {
    return {{}, false};
  }

  // Normal = D2 * (D1.D1) - D1 * (D1.D2)
  // This is equivalent to D1 x (D2 x D1) in 2D using the vector triple product identity.
  const gp_Vec2d aNorm = theD2 * theD1.Dot(theD1) - theD1 * theD1.Dot(theD2);
  if (aNorm.SquareMagnitude() <= theTol * theTol)
  {
    return {{}, false};
  }
  return {gp_Dir2d(aNorm), true};
}

//==================================================================================================

Geom2dProp::CentreResult Geom2dProp::ComputeCentreOfCurvature(const gp_Pnt2d& thePnt,
                                                              const gp_Vec2d& theD1,
                                                              const gp_Vec2d& theD2,
                                                              const double    theTol)
{
  const CurvatureResult aCurvRes = ComputeCurvature(theD1, theD2, theTol);
  if (!aCurvRes.IsDefined || aCurvRes.IsInfinite || std::abs(aCurvRes.Value) <= theTol)
  {
    return {{}, false};
  }

  // Normal vector (unnormalized) = D2 * (D1.D1) - D1 * (D1.D2)
  gp_Vec2d aNorm = theD2 * theD1.Dot(theD1) - theD1 * theD1.Dot(theD2);
  if (aNorm.SquareMagnitude() <= theTol * theTol)
  {
    return {{}, false};
  }
  aNorm.Normalize();
  aNorm.Divide(aCurvRes.Value);

  return {thePnt.Translated(aNorm), true};
}
