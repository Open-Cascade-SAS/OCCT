// Copyright (c) 2025 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.

#ifndef _MathPoly_Quartic_HeaderFile
#define _MathPoly_Quartic_HeaderFile

#include "MathPoly_Cubic.hxx"

namespace MathPoly
{

//! Solves a*x^4 + b*x^3 + c*x^2 + d*x + e = 0 for all distinct real roots.
//! Multiplicities are reported explicitly in PolyResult::Multiplicities.
inline PolyResult Quartic(double theA, double theB, double theC, double theD, double theE)
{
  const double aCoefficients[5] = {theE, theD, theC, theB, theA};
  return Utils::SolveDirect(aCoefficients, 4);
}

} // namespace MathPoly

#endif // _MathPoly_Quartic_HeaderFile
