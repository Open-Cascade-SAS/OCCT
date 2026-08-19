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

#include "MathPoly_Utils.hxx"

//! Polynomial root finding algorithms.
namespace MathPoly
{
//! Solve linear equation: a*x + b = 0
//! Handles degenerate cases (a = 0).
//! @param theA coefficient of x
//! @param theB constant term
//! @return result containing 0 or 1 root, or infinite solutions flag
#ifdef _MSC_VER
  #pragma warning(push)
  #pragma warning(disable : 4723) // potential divide by 0 - guarded by IsZero() check
#endif
inline PolyResult Linear(double theA, double theB)
{
  const double aCoefficients[2] = {theB, theA};
  return Utils::SolveDirect(aCoefficients, 1);
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
inline PolyResult Quadratic(double theA, double theB, double theC)
{
  const double aCoefficients[3] = {theC, theB, theA};
  return Utils::SolveDirect(aCoefficients, 2);
}

} // namespace MathPoly

#endif // _MathPoly_Quadratic_HeaderFile
