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

#ifndef _MathPoly_Cubic_HeaderFile
#define _MathPoly_Cubic_HeaderFile

#include "MathPoly_Quadratic.hxx"

//! Polynomial root finding algorithms.
namespace MathPoly
{
//! Solve cubic equation: a*x^3 + b*x^2 + c*x + d = 0
//! Uses derivative-recursive real-root isolation with power-of-two scaling.
//!
//! Algorithm:
//! 1. Handle lower degree cases (a = 0 -> quadratic)
//! 2. Isolate critical points from derivative roots
//! 3. Bisect sign-changing intervals and accept tangential critical roots by scaled residual
//! 4. Classify algebraic multiplicities from successive derivatives
//!
//! @param theA coefficient of x^3
//! @param theB coefficient of x^2
//! @param theC coefficient of x
//! @param theD constant term
//! @return result containing 1, 2, or 3 real roots (sorted in ascending order)
inline PolyResult Cubic(double theA, double theB, double theC, double theD)
{
  const double aCoefficients[4] = {theD, theC, theB, theA};
  return Utils::SolveDirect(aCoefficients, 3);
}

} // namespace MathPoly

#endif // _MathPoly_Cubic_HeaderFile
