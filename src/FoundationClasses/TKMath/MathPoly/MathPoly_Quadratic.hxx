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

#ifndef _MathPoly_Quadratic_HeaderFile
#define _MathPoly_Quadratic_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Core.hxx>

#include <cmath>

//! Polynomial root finding algorithms.
namespace MathPoly
{
using namespace MathUtils;

//! Solve linear equation: a*x + b = 0
//! Handles degenerate cases (a = 0).
//! @param theA coefficient of x
//! @param theB constant term
//! @return result containing 0 or 1 root, or infinite solutions flag
#ifdef _MSC_VER
  #pragma warning(push)
  #pragma warning(disable : 4723) // potential divide by 0 - guarded by IsZero() check
#endif
inline MathUtils::PolyResult Linear(double theA, double theB)
{
  MathUtils::PolyResult aResult;

  if (MathUtils::IsZero(theA))
  {
    if (MathUtils::IsZero(theB))
    {
      aResult.Status = MathUtils::Status::InfiniteSolutions;
    }
    else
    {
      aResult.Status = MathUtils::Status::NoSolution;
    }
  }
  else
  {
    aResult.Status   = MathUtils::Status::OK;
    aResult.NbRoots  = 1;
    aResult.Roots[0] = -theB / theA;
  }

  return aResult;
}
#ifdef _MSC_VER
  #pragma warning(pop)
#endif

//! Solve quadratic equation: a*x^2 + b*x + c = 0
//! Uses numerically stable formulas to avoid catastrophic cancellation.
//!
//! Algorithm:
//! 1. Handle linear case when a = 0
//! 2. Compute discriminant D = b^2 - 4ac
//! 3. For D < 0: no real roots
//! 4. For D = 0: one double root
//! 5. For D > 0: two roots using stable formula
//!
//! @param theA coefficient of x^2
//! @param theB coefficient of x
//! @param theC constant term
//! @return result containing 0, 1, or 2 real roots (sorted in ascending order)
inline MathUtils::PolyResult Quadratic(double theA, double theB, double theC)
{
  MathUtils::PolyResult aResult;

  // Linear case: b*x + c = 0
  if (MathUtils::IsZero(theA))
  {
    return Linear(theB, theC);
  }

  // Scale coefficients for numerical stability
  const double aScale = std::max({std::abs(theA), std::abs(theB), std::abs(theC)});
  if (aScale < MathUtils::THE_ZERO_TOL)
  {
    aResult.Status = MathUtils::Status::InfiniteSolutions;
    return aResult;
  }

  const double aA = theA / aScale;
  const double aB = theB / aScale;
  const double aC = theC / aScale;

  // Compute discriminant
  const double aDisc = aB * aB - 4.0 * aA * aC;

  // Tolerance for discriminant based on coefficient magnitudes
  const double aDiscTol = MathUtils::THE_ZERO_TOL * (aB * aB + std::abs(4.0 * aA * aC));

  if (aDisc < -aDiscTol)
  {
    // No real roots (complex conjugate pair)
    aResult.Status  = MathUtils::Status::OK;
    aResult.NbRoots = 0;
    return aResult;
  }

  if (std::abs(aDisc) <= aDiscTol)
  {
    // Double root
    aResult.Status   = MathUtils::Status::OK;
    aResult.NbRoots  = 1;
    aResult.Roots[0] = -aB / (2.0 * aA);
    return aResult;
  }

  // Two distinct real roots
  // Use numerically stable formula to avoid catastrophic cancellation
  const double aSqrtDisc = std::sqrt(aDisc);

  // q = -0.5 * (b + sign(b) * sqrt(discriminant))
  const double aQ = -0.5 * (aB + MathUtils::SignTransfer(aSqrtDisc, aB));

  aResult.Status   = MathUtils::Status::OK;
  aResult.NbRoots  = 2;
  aResult.Roots[0] = aQ / aA;
  aResult.Roots[1] = aC / aQ;

  // Sort roots in ascending order
  if (aResult.Roots[0] > aResult.Roots[1])
  {
    std::swap(aResult.Roots[0], aResult.Roots[1]);
  }

  return aResult;
}

} // namespace MathPoly

#endif // _MathPoly_Quadratic_HeaderFile
