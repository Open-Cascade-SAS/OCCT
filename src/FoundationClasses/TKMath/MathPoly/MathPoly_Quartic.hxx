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

#ifndef _MathPoly_Quartic_HeaderFile
#define _MathPoly_Quartic_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Core.hxx>
#include <MathUtils_Poly.hxx>
#include <MathPoly_Quadratic.hxx>
#include <MathPoly_Cubic.hxx>

#include <cmath>

//! Polynomial root finding algorithms.
namespace MathPoly
{
using namespace MathUtils;

//! Solve quartic equation: a*x^4 + b*x^3 + c*x^2 + d*x + e = 0
//! Uses Ferrari's method with modern numerical enhancements.
//!
//! Algorithm:
//! 1. Handle lower degree cases (a = 0 -> cubic)
//! 2. Transform to depressed quartic: t^4 + pt^2 + qt + r = 0 via x = t - b/(4a)
//! 3. Solve Ferrari's resolvent cubic
//! 4. Factor quartic into two quadratics
//! 5. Solve both quadratics
//! 6. Apply Newton-Raphson refinement
//!
//! @param theA coefficient of x^4
//! @param theB coefficient of x^3
//! @param theC coefficient of x^2
//! @param theD coefficient of x
//! @param theE constant term
//! @return result containing 0, 1, 2, 3, or 4 real roots (sorted in ascending order)
inline MathUtils::PolyResult Quartic(double theA,
                                     double theB,
                                     double theC,
                                     double theD,
                                     double theE)
{
  MathUtils::PolyResult aResult;

  // Reduce to cubic if leading coefficient is zero
  if (MathUtils::IsZero(theA))
  {
    return Cubic(theB, theC, theD, theE);
  }

  // Scale coefficients for numerical stability
  const double aScale =
    std::max({std::abs(theA), std::abs(theB), std::abs(theC), std::abs(theD), std::abs(theE)});
  if (aScale < MathUtils::THE_ZERO_TOL)
  {
    aResult.Status = MathUtils::Status::InfiniteSolutions;
    return aResult;
  }

  // Normalize to monic quartic: x^4 + ax^3 + bx^2 + cx + d = 0
  const double a = theB / theA;
  const double b = theC / theA;
  const double c = theD / theA;
  const double d = theE / theA;

  // Substitute x = t - a/4 to get depressed quartic: t^4 + pt^2 + qt + r = 0
  const double aShift = a / 4.0;
  const double a2     = a * a;
  const double a3     = a2 * a;
  const double a4     = a2 * a2;

  const double p = b - 3.0 * a2 / 8.0;
  const double q = c - a * b / 2.0 + a3 / 8.0;
  const double r = d - a * c / 4.0 + a2 * b / 16.0 - 3.0 * a4 / 256.0;

  // Store original coefficients for refinement
  const double aCoeffs[5] = {theE, theD, theC, theB, theA};

  // Special case: biquadratic (q = 0) -> t^4 + pt^2 + r = 0
  if (MathUtils::IsZero(q))
  {
    // Substitute u = t^2 to get u^2 + pu + r = 0
    MathUtils::PolyResult aQuadResult = Quadratic(1.0, p, r);

    if (!aQuadResult.IsDone())
    {
      aResult.Status = aQuadResult.Status;
      return aResult;
    }

    aResult.Status  = MathUtils::Status::OK;
    aResult.NbRoots = 0;

    for (size_t i = 0; i < aQuadResult.NbRoots; ++i)
    {
      const double u = aQuadResult.Roots[i];
      if (u >= -MathUtils::THE_ZERO_TOL)
      {
        if (u <= MathUtils::THE_ZERO_TOL)
        {
          // u = 0 -> t = 0
          aResult.Roots[aResult.NbRoots++] = -aShift;
        }
        else
        {
          // u > 0 -> t = +/-sqrt(u)
          const double aSqrtU              = std::sqrt(u);
          aResult.Roots[aResult.NbRoots++] = aSqrtU - aShift;
          aResult.Roots[aResult.NbRoots++] = -aSqrtU - aShift;
        }
      }
    }

    // Refine and sort roots
    for (size_t i = 0; i < aResult.NbRoots; ++i)
    {
      aResult.Roots[i] = MathUtils::RefinePolyRoot(aCoeffs, 4, aResult.Roots[i]);
    }
    MathUtils::SortRoots(aResult.Roots.data(), aResult.NbRoots);
    aResult.NbRoots = MathUtils::RemoveDuplicateRoots(aResult.Roots.data(), aResult.NbRoots);

    return aResult;
  }

  // Use resolvent: z^3 + 2pz^2 + (p^2 - 4r)z - q^2 = 0
  MathUtils::PolyResult aCubicResult = Cubic(1.0, 2.0 * p, p * p - 4.0 * r, -q * q);

  if (!aCubicResult.IsDone() || aCubicResult.NbRoots == 0)
  {
    aResult.Status = MathUtils::Status::NumericalError;
    return aResult;
  }

  // Find a positive root of the resolvent (there's always at least one)
  double z = aCubicResult.Roots[aCubicResult.NbRoots - 1]; // Largest root

  // Ensure z is positive (or at least non-negative)
  if (z < -MathUtils::THE_ZERO_TOL)
  {
    // Try other roots
    for (size_t i = 0; i < aCubicResult.NbRoots; ++i)
    {
      if (aCubicResult.Roots[i] >= -MathUtils::THE_ZERO_TOL)
      {
        z = std::max(0.0, aCubicResult.Roots[i]);
        break;
      }
    }
  }
  z = std::max(0.0, z);

  // Now factor: t^4 + pt^2 + qt + r = (t^2 + st + u)(t^2 - st + v)
  // where s = sqrt(z), u = (p + z)/2 + q/(2s), v = (p + z)/2 - q/(2s)

  const double s = std::sqrt(z);
  double       u, v;

  if (MathUtils::IsZero(s))
  {
    // Special case: s = 0
    // t^4 + pt^2 + r = (t^2 + u)(t^2 + v) where u + v = p, uv = r
    MathUtils::PolyResult aUVResult = Quadratic(1.0, p, r);
    if (!aUVResult.IsDone() || aUVResult.NbRoots < 2)
    {
      // Degenerate case
      u = p / 2.0;
      v = p / 2.0;
    }
    else
    {
      u = aUVResult.Roots[0];
      v = aUVResult.Roots[1];
    }

    // Solve t^2 + u = 0 and t^2 + v = 0
    aResult.Status  = MathUtils::Status::OK;
    aResult.NbRoots = 0;

    if (u <= MathUtils::THE_ZERO_TOL)
    {
      const double aSqrt               = std::sqrt(std::max(0.0, -u));
      aResult.Roots[aResult.NbRoots++] = aSqrt - aShift;
      if (aSqrt > MathUtils::THE_ZERO_TOL)
      {
        aResult.Roots[aResult.NbRoots++] = -aSqrt - aShift;
      }
    }

    if (v <= MathUtils::THE_ZERO_TOL)
    {
      const double aSqrt               = std::sqrt(std::max(0.0, -v));
      aResult.Roots[aResult.NbRoots++] = aSqrt - aShift;
      if (aSqrt > MathUtils::THE_ZERO_TOL)
      {
        aResult.Roots[aResult.NbRoots++] = -aSqrt - aShift;
      }
    }
  }
  else
  {
    // General case
    const double aHalfPPlusZ = (p + z) / 2.0;
    const double aQOver2S    = q / (2.0 * s);

    u = aHalfPPlusZ - aQOver2S;
    v = aHalfPPlusZ + aQOver2S;

    // Solve t^2 + st + u = 0
    MathUtils::PolyResult aQuad1 = Quadratic(1.0, s, u);

    // Solve t^2 - st + v = 0
    MathUtils::PolyResult aQuad2 = Quadratic(1.0, -s, v);

    aResult.Status  = MathUtils::Status::OK;
    aResult.NbRoots = 0;

    if (aQuad1.IsDone())
    {
      for (size_t i = 0; i < aQuad1.NbRoots; ++i)
      {
        aResult.Roots[aResult.NbRoots++] = aQuad1.Roots[i] - aShift;
      }
    }

    if (aQuad2.IsDone())
    {
      for (size_t i = 0; i < aQuad2.NbRoots; ++i)
      {
        aResult.Roots[aResult.NbRoots++] = aQuad2.Roots[i] - aShift;
      }
    }
  }

  // Refine all roots using Newton-Raphson
  for (size_t i = 0; i < aResult.NbRoots; ++i)
  {
    aResult.Roots[i] = MathUtils::RefinePolyRoot(aCoeffs, 4, aResult.Roots[i]);
  }

  // Sort roots and remove duplicates
  MathUtils::SortRoots(aResult.Roots.data(), aResult.NbRoots);
  aResult.NbRoots = MathUtils::RemoveDuplicateRoots(aResult.Roots.data(), aResult.NbRoots);

  return aResult;
}

} // namespace MathPoly

#endif // _MathPoly_Quartic_HeaderFile
