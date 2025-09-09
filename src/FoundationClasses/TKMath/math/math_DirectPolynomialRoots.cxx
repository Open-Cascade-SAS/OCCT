// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2025 OPEN CASCADE SAS
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

// @file math_DirectPolynomialRoots.cxx
// @brief Implementation of polynomial root finding using direct algebraic methods
//
// This implementation follows Ferrari's method (1540) for quartic equations:
//
// Given: ax^4 + bx^3 + cx^2 + dx + e = 0
//
// Step 1: Normalize by dividing by 'a' and substitute x = X - b/4a to eliminate
//         the cubic term, obtaining: X^4 + AX^2 + BX + C = 0
//
// Step 2: If B = 0, solve as biquadratic (X^4 + AX^2 + C = 0)
//
// Step 3: Otherwise, introduce auxiliary variable u and construct:
//         (X^2 + u/2)^2 = (u - A)X^2 - BX + u^2/4 - C
//
// Step 4: Force the right side to be a perfect square by solving the
//         resolvent cubic: u^3 - Au^2 - 4Cu + 4AC - B^2 = 0
//
// Step 5: Use a root u of this cubic to factor into two quadratics and solve
//
// References:
// - J. VIGNES "Algorithmes Numeriques Analyse et Mise en Oeuvre" Tome 2
// - L. Ferrari's method (1540) for quartic equations
// - Modern numerical analysis techniques for stability

#define No_Standard_RangeError
#define No_Standard_OutOfRange
#define No_Standard_DimensionError

#include <math_DirectPolynomialRoots.hxx>
#include <StdFail_InfiniteSolutions.hxx>
#include <Standard.hxx>

#include <algorithm>
#include <array>
#include <cmath>
#include <complex>
#include <limits>
#include <numeric>

namespace
{
// ========================================================================
// Numerical Constants
// ========================================================================

// Fundamental constants
constexpr Standard_Real EPSILON       = std::numeric_limits<Standard_Real>::epsilon();
constexpr Standard_Real RADIX         = 2.0;
constexpr Standard_Real LN2           = 0.693147180559945309417; // Natural logarithm of 2
constexpr Standard_Real INV_LOG_RADIX = 1.0 / LN2;               // 1/ln(2)

// Tolerance constants for various numerical operations
constexpr Standard_Real ZERO_TOLERANCE = 1.0e-30; // General zero tolerance
constexpr Standard_Real BIQUADRATIC_TOLERANCE =
  1.0e-14; // Biquadratic detection - stringent to avoid false positives
constexpr Standard_Real NEWTON_DERIVATIVE_MIN =
  1.0e-15; // Minimum derivative for Newton-Raphson iteration
constexpr Standard_Real NEWTON_TARGET_DEFAULT =
  1.0e-15; // Default target tolerance for Newton-Raphson refinement
constexpr Standard_Real DUPLICATE_ROOT_TOL = 1.0e-10; // Tolerance for detecting duplicate roots

// Iteration limits
constexpr Standard_Integer MAX_NEWTON_ITERATIONS =
  100; // Maximum iterations for aggressive Newton-Raphson

// ========================================================================
// Utility Functions
// ========================================================================

//@brief Evaluate polynomial using Horner's method
//@param theCoeffs Polynomial coefficients in descending order
//@param theX Value at which to evaluate
//@return Polynomial value at theX
template <std::size_t N>
Standard_Real EvaluatePolynomial(const std::array<Standard_Real, N>& theCoeffs,
                                 const Standard_Real                 theX)
{
  Standard_Real aResult = theCoeffs[0];
  for (std::size_t i = 1; i < N; ++i)
  {
    aResult = aResult * theX + theCoeffs[i];
  }
  return aResult;
}

//@brief Evaluate polynomial and its derivative simultaneously using Horner's method
//@param theCoeffs Polynomial coefficients in descending order
//@param theX Value at which to evaluate
//@param theValue Output: polynomial value
//@param theDerivative Output: derivative value
template <std::size_t N>
void EvaluatePolynomialWithDerivative(const std::array<Standard_Real, N>& theCoeffs,
                                      const Standard_Real                 theX,
                                      Standard_Real&                      theValue,
                                      Standard_Real&                      theDerivative)
{
  theValue      = theCoeffs[0] * theX + theCoeffs[1];
  theDerivative = theCoeffs[0];

  for (std::size_t i = 2; i < N; ++i)
  {
    theDerivative = theDerivative * theX + theValue;
    theValue      = theValue * theX + theCoeffs[i];
  }
}

// Aggressive Newton-Raphson refinement with many iterations for high precision
template <std::size_t N>
Standard_Real RefineRootAggressively(const std::array<Standard_Real, N>& theCoeffs,
                                     const Standard_Real                 theInitialGuess,
                                     const Standard_Real theTargetTolerance = NEWTON_TARGET_DEFAULT)
{

  Standard_Real aX = theInitialGuess;

  for (Standard_Integer anIter = 0; anIter < MAX_NEWTON_ITERATIONS; ++anIter)
  {
    Standard_Real aValue, aDerivative;
    EvaluatePolynomialWithDerivative(theCoeffs, aX, aValue, aDerivative);

    if (std::abs(aValue) < theTargetTolerance)
    {
      return aX; // Achieved target accuracy
    }

    if (std::abs(aDerivative) < NEWTON_DERIVATIVE_MIN)
    {
      break; // Derivative too small
    }

    const Standard_Real aDelta = aValue / aDerivative;
    if (std::abs(aDelta) < theTargetTolerance)
    {
      break; // Converged
    }

    aX -= aDelta;
  }

  return aX;
}

// Simple aggressive refinement for all roots using direct Newton-Raphson
template <std::size_t N>
void RefineAllRootsWithDeflation(const std::array<Standard_Real, N>& theCoeffs,
                                 Standard_Real*                      theRoots,
                                 const Standard_Integer              theNbRoots)
{
  // Refine each root individually using direct Newton-Raphson
  for (Standard_Integer i = 0; i < theNbRoots; ++i)
  {
    const Standard_Real aRefinedRoot = RefineRootAggressively(theCoeffs, theRoots[i]);
    if (aRefinedRoot != std::numeric_limits<Standard_Real>::max())
    {
      theRoots[i] = aRefinedRoot;
    }
  }
}

// Compute base-2 exponent of a number for scaling purposes
inline Standard_Integer ComputeBaseExponent(const Standard_Real theValue)
{
  const Standard_Real anAbsValue = std::abs(theValue);
  if (anAbsValue > 1.0)
  {
    return static_cast<Standard_Integer>(std::log(anAbsValue) * INV_LOG_RADIX);
  }
  return 0;
}

//@brief Sort roots in ascending order and remove duplicates
//@param theRoots Array of roots
//@param theNbRoots Number of roots
void SortAndRemoveDuplicateRoots(Standard_Real* theRoots, Standard_Integer& theNbRoots)
{
  if (theNbRoots <= 1)
  {
    return;
  }

  // Sort roots
  std::sort(theRoots, theRoots + theNbRoots);

  // Remove duplicates
  Standard_Integer aUniqueCount = 1;

  for (Standard_Integer i = 1; i < theNbRoots; ++i)
  {
    if (std::abs(theRoots[i] - theRoots[aUniqueCount - 1]) > DUPLICATE_ROOT_TOL)
    {
      theRoots[aUniqueCount++] = theRoots[i];
    }
  }

  theNbRoots = aUniqueCount;
}

// ========================================================================
// Quadratic Equation Solver
// ========================================================================

// @brief Solve quadratic equation ax^2 + bx + c = 0 with numerical stability
// @param theA Leading coefficient
// @param theB Linear coefficient
// @param theC Constant term
// @param theRoots Output array for roots
// @return Number of real roots found
Standard_Integer SolveQuadratic(const Standard_Real theA,
                                const Standard_Real theB,
                                const Standard_Real theC,
                                Standard_Real       theRoots[2])
{
  // Handle degenerate case
  if (std::abs(theA) < ZERO_TOLERANCE)
  {
    if (std::abs(theB) < ZERO_TOLERANCE)
    {
      return 0;
    }
    theRoots[0] = -theC / theB;
    return 1;
  }

  // Normalize coefficients
  const Standard_Real aB = theB / theA;
  const Standard_Real aC = theC / theA;

  // Compute discriminant with care for numerical stability
  const Standard_Real aDiscriminant = aB * aB - 4.0 * aC;

  if (aDiscriminant < -EPSILON)
  {
    return 0; // No real roots
  }

  if (std::abs(aDiscriminant) < EPSILON)
  {
    // One repeated root
    theRoots[0] = -aB * 0.5;
    return 1;
  }

  // Two distinct real roots - use numerically stable formula
  const Standard_Real aSqrtDiscriminant = std::sqrt(aDiscriminant);

  if (aB > 0)
  {
    const Standard_Real aTemp = -0.5 * (aB + aSqrtDiscriminant);
    theRoots[0]               = aTemp;
    theRoots[1]               = aC / aTemp;
  }
  else
  {
    const Standard_Real aTemp = -0.5 * (aB - aSqrtDiscriminant);
    theRoots[0]               = aC / aTemp;
    theRoots[1]               = aTemp;
  }

  return 2;
}

// ========================================================================
// Cubic Equation Solver (Cardano's Method)
// ========================================================================

// @brief Solve cubic equation x^3 + ax^2 + bx + c = 0 using Cardano's method
// @param theA Quadratic coefficient
// @param theB Linear coefficient
// @param theC Constant term
// @param theRoots Output array for roots
// @return Number of real roots found
Standard_Integer SolveCubicNormalized(const Standard_Real theA,
                                      const Standard_Real theB,
                                      const Standard_Real theC,
                                      Standard_Real       theRoots[3])
{
  // Vieta substitution: x = y - a/3 to eliminate quadratic term
  const Standard_Real aThirdA = theA / 3.0;
  const Standard_Real aP      = theB - theA * aThirdA;
  const Standard_Real aQ      = theC - theB * aThirdA + 2.0 * aThirdA * aThirdA * aThirdA;

  // Check for special case: y^3 + q = 0
  if (std::abs(aP) < EPSILON)
  {
    theRoots[0] = std::cbrt(-aQ) - aThirdA;
    return 1;
  }

  // Compute discriminant
  const Standard_Real aDiscriminant = -4.0 * aP * aP * aP - 27.0 * aQ * aQ;

  if (aDiscriminant > EPSILON)
  {
    // Three distinct real roots - use trigonometric solution
    const Standard_Real aM          = 2.0 * std::sqrt(-aP / 3.0);
    const Standard_Real aTheta      = std::acos(3.0 * aQ / (aP * aM)) / 3.0;
    const Standard_Real aTwoPiThird = 2.0 * M_PI / 3.0;

    theRoots[0] = aM * std::cos(aTheta) - aThirdA;
    theRoots[1] = aM * std::cos(aTheta - aTwoPiThird) - aThirdA;
    theRoots[2] = aM * std::cos(aTheta + aTwoPiThird) - aThirdA;

    return 3;
  }
  else if (std::abs(aDiscriminant) < EPSILON)
  {
    // One single and one double root
    const Standard_Real aRoot = 3.0 * aQ / aP;
    theRoots[0]               = aRoot - aThirdA;
    theRoots[1]               = -0.5 * aRoot - aThirdA;
    return 2;
  }
  else
  {
    // One real root - use Cardano's formula
    const Standard_Real aSqrtDisc = std::sqrt(-aDiscriminant / 108.0);
    const Standard_Real aU        = std::cbrt(-aQ / 2.0 + aSqrtDisc);
    const Standard_Real aV        = std::cbrt(-aQ / 2.0 - aSqrtDisc);

    theRoots[0] = aU + aV - aThirdA;
    return 1;
  }
}

// ========================================================================
// Quartic Equation Solver (Ferrari's Method) - MODERN IMPLEMENTATION
// ========================================================================

// @brief Detect if polynomial is biquadratic (x^4 + ax^2 + b = 0)
// @param theA Cubic coefficient
// @param theB Quadratic coefficient
// @param theC Linear coefficient
// @param theD Constant term
// @return True if polynomial is biquadratic
inline bool IsBiquadratic(const Standard_Real theA,
                          const Standard_Real theB,
                          const Standard_Real theC,
                          const Standard_Real theD)
{
  const Standard_Real aMaxCoeff =
    std::max({std::abs(theA), std::abs(theB), std::abs(theC), std::abs(theD)});

  // Use very strict tolerance to avoid false positives
  return (std::abs(theA) < BIQUADRATIC_TOLERANCE * aMaxCoeff)
         && (std::abs(theC) < BIQUADRATIC_TOLERANCE * aMaxCoeff);
}

// @brief Solve biquadratic equation x^4 + bx^2 + d = 0
// @param theB Quadratic coefficient
// @param theD Constant term
// @param theRoots Output array for roots
// @return Number of real roots found
Standard_Integer SolveBiquadratic(const Standard_Real theB,
                                  const Standard_Real theD,
                                  Standard_Real       theRoots[4])
{
  // Solve as quadratic in y = x^2
  Standard_Real          aQuadRoots[2];
  const Standard_Integer aNbQuadRoots = SolveQuadratic(1.0, theB, theD, aQuadRoots);

  Standard_Integer aNbRoots = 0;
  for (Standard_Integer i = 0; i < aNbQuadRoots; ++i)
  {
    if (aQuadRoots[i] > EPSILON)
    {
      const Standard_Real aSqrt = std::sqrt(aQuadRoots[i]);
      theRoots[aNbRoots++]      = -aSqrt;
      theRoots[aNbRoots++]      = aSqrt;
    }
    else if (std::abs(aQuadRoots[i]) < EPSILON)
    {
      theRoots[aNbRoots++] = 0.0;
    }
  }

  return aNbRoots;
}

// @brief Solve quartic equation x^4 + ax^3 + bx^2 + cx + d = 0 using Ferrari's method
// Uses the original OCCT approach with modern C++ improvements
// @param theA Cubic coefficient
// @param theB Quadratic coefficient
// @param theC Linear coefficient
// @param theD Constant term
// @param theRoots Output array for roots
// @return Number of real roots found
Standard_Integer SolveQuarticFerrari(const Standard_Real theA,
                                     const Standard_Real theB,
                                     const Standard_Real theC,
                                     const Standard_Real theD,
                                     Standard_Real       theRoots[4])
{
  // Check for biquadratic case first
  if (IsBiquadratic(theA, theB, theC, theD))
  {
    return SolveBiquadratic(theB, theD, theRoots);
  }

  // Use original OCCT Ferrari method for robustness
  // This follows the proven approach from the working implementation

  // Resolvent cubic coefficients
  const Standard_Real R3 = -theB;
  const Standard_Real S3 = theA * theC - 4.0 * theD;
  const Standard_Real T3 = theD * (4.0 * theB - theA * theA) - theC * theC;

  // Solve resolvent cubic: y^3 + R3*y^2 + S3*y + T3 = 0
  Standard_Real          aCubicRoots[3];
  const Standard_Integer aNbCubicRoots = SolveCubicNormalized(R3, S3, T3, aCubicRoots);

  if (aNbCubicRoots == 0)
  {
    return 0; // No solutions
  }

  // Choose the best resolvent root (largest real value for stability)
  Standard_Real aY0 = aCubicRoots[0];
  for (Standard_Integer i = 1; i < aNbCubicRoots; ++i)
  {
    if (aCubicRoots[i] > aY0)
    {
      aY0 = aCubicRoots[i];
    }
  }

  // Check discriminant for square root extraction
  const Standard_Real aDiscr  = theA * aY0 * 0.5 - theC;
  const Standard_Real aSdiscr = (aDiscr >= 0.0) ? 1.0 : -1.0;

  // Compute P0 and Q0
  Standard_Real aP0 = theA * theA * 0.25 - theB + aY0;
  if (aP0 < 0.0)
    aP0 = 0.0;
  aP0 = std::sqrt(aP0);

  Standard_Real aQ0 = aY0 * aY0 * 0.25 - theD;
  if (aQ0 < 0.0)
    aQ0 = 0.0;
  aQ0 = std::sqrt(aQ0);

  // Form two quadratics
  const Standard_Real aAdemi    = theA * 0.5;
  const Standard_Real aYdemi    = aY0 * 0.5;
  const Standard_Real aSdiscrQ0 = aSdiscr * aQ0;

  const Standard_Real aP  = aAdemi + aP0;
  const Standard_Real aQ  = aYdemi + aSdiscrQ0;
  const Standard_Real aP1 = aAdemi - aP0;
  const Standard_Real aQ1 = aYdemi - aSdiscrQ0;

  // Solve two quadratics: x^2 + P*x + Q = 0 and x^2 + P1*x + Q1 = 0
  Standard_Integer aNbRoots = 0;

  Standard_Real          aQuadRoots1[2];
  const Standard_Integer aNb1 = SolveQuadratic(1.0, aP, aQ, aQuadRoots1);
  for (Standard_Integer i = 0; i < aNb1; ++i)
  {
    theRoots[aNbRoots++] = aQuadRoots1[i];
  }

  Standard_Real          aQuadRoots2[2];
  const Standard_Integer aNb2 = SolveQuadratic(1.0, aP1, aQ1, aQuadRoots2);
  for (Standard_Integer i = 0; i < aNb2; ++i)
  {
    theRoots[aNbRoots++] = aQuadRoots2[i];
  }

  return aNbRoots;
}

} // anonymous namespace

// ========================================================================
// math_DirectPolynomialRoots Implementation
// ========================================================================

//====================================================================================================

math_DirectPolynomialRoots::math_DirectPolynomialRoots(const Standard_Real A,
                                                       const Standard_Real B,
                                                       const Standard_Real C,
                                                       const Standard_Real D,
                                                       const Standard_Real E)
{
  Solve(A, B, C, D, E);
}

//====================================================================================================

math_DirectPolynomialRoots::math_DirectPolynomialRoots(const Standard_Real A,
                                                       const Standard_Real B,
                                                       const Standard_Real C,
                                                       const Standard_Real D)
{
  Solve(A, B, C, D);
}

//====================================================================================================

math_DirectPolynomialRoots::math_DirectPolynomialRoots(const Standard_Real A,
                                                       const Standard_Real B,
                                                       const Standard_Real C)
{
  Solve(A, B, C);
}

//====================================================================================================

math_DirectPolynomialRoots::math_DirectPolynomialRoots(const Standard_Real A, const Standard_Real B)
{
  Solve(A, B);
}

//====================================================================================================

void math_DirectPolynomialRoots::Solve(const Standard_Real A,
                                       const Standard_Real B,
                                       const Standard_Real C,
                                       const Standard_Real D,
                                       const Standard_Real E)
{
  Done           = Standard_True;
  InfiniteStatus = Standard_False;
  NbSol          = 0;

  // Check for degenerate cases
  const Standard_Real anAbsA = std::abs(A);

  if (anAbsA < ZERO_TOLERANCE)
  {
    // Degenerate to cubic
    Solve(B, C, D, E);
    return;
  }

  // Normalize coefficients
  const Standard_Real aNormB = B / A;
  const Standard_Real aNormC = C / A;
  const Standard_Real aNormD = D / A;
  const Standard_Real aNormE = E / A;

  // Apply scaling for numerical stability
  const Standard_Real aMaxCoeff =
    std::max({std::abs(aNormB), std::abs(aNormC), std::abs(aNormD), std::abs(aNormE)});

  if (aMaxCoeff > 1.0e8)
  {
    // Scale coefficients to improve numerical stability
    const Standard_Integer anExponent = ComputeBaseExponent(aMaxCoeff) / 4;
    const Standard_Real    aScale     = std::pow(RADIX, anExponent);
    const Standard_Real    aScale2    = aScale * aScale;
    const Standard_Real    aScale3    = aScale2 * aScale;
    const Standard_Real    aScale4    = aScale2 * aScale2;

    const Standard_Real aScaledB = aNormB / aScale;
    const Standard_Real aScaledC = aNormC / aScale2;
    const Standard_Real aScaledD = aNormD / aScale3;
    const Standard_Real aScaledE = aNormE / aScale4;

    // Solve scaled equation
    NbSol = SolveQuarticFerrari(aScaledB, aScaledC, aScaledD, aScaledE, TheRoots);

    // Scale back roots
    for (Standard_Integer i = 0; i < NbSol; ++i)
    {
      TheRoots[i] *= aScale;
    }
  }
  else
  {
    // Solve directly
    NbSol = SolveQuarticFerrari(aNormB, aNormC, aNormD, aNormE, TheRoots);
  }

  // Refine all roots using aggressive multi-stage Newton-Raphson with deflation
  const std::array<Standard_Real, 5> aOrigCoeffs = {A, B, C, D, E};
  RefineAllRootsWithDeflation(aOrigCoeffs, TheRoots, NbSol);

  // Sort and remove duplicates
  SortAndRemoveDuplicateRoots(TheRoots, NbSol);
}

//====================================================================================================

void math_DirectPolynomialRoots::Solve(const Standard_Real A,
                                       const Standard_Real B,
                                       const Standard_Real C,
                                       const Standard_Real D)
{
  Done           = Standard_True;
  InfiniteStatus = Standard_False;
  NbSol          = 0;

  // Check for degenerate cases
  const Standard_Real anAbsA = std::abs(A);

  if (anAbsA < ZERO_TOLERANCE)
  {
    // Degenerate to quadratic
    Solve(B, C, D);
    return;
  }

  // Normalize coefficients
  const Standard_Real aNormB = B / A;
  const Standard_Real aNormC = C / A;
  const Standard_Real aNormD = D / A;

  // Apply scaling for numerical stability
  const Standard_Real aMaxCoeff = std::max({std::abs(aNormB), std::abs(aNormC), std::abs(aNormD)});

  if (aMaxCoeff > 1.0e8)
  {
    // Scale coefficients
    const Standard_Integer anExponent = ComputeBaseExponent(aMaxCoeff) / 3;
    const Standard_Real    aScale     = std::pow(RADIX, anExponent);
    const Standard_Real    aScale2    = aScale * aScale;
    const Standard_Real    aScale3    = aScale2 * aScale;

    const Standard_Real aScaledB = aNormB / aScale;
    const Standard_Real aScaledC = aNormC / aScale2;
    const Standard_Real aScaledD = aNormD / aScale3;

    // Solve scaled equation
    NbSol = SolveCubicNormalized(aScaledB, aScaledC, aScaledD, TheRoots);

    // Scale back roots
    for (Standard_Integer i = 0; i < NbSol; ++i)
    {
      TheRoots[i] *= aScale;
    }
  }
  else
  {
    // Solve directly
    NbSol = SolveCubicNormalized(aNormB, aNormC, aNormD, TheRoots);
  }

  // Refine all roots using aggressive multi-stage Newton-Raphson with deflation
  const std::array<Standard_Real, 4> aOrigCoeffs = {A, B, C, D};
  RefineAllRootsWithDeflation(aOrigCoeffs, TheRoots, NbSol);

  // Sort and remove duplicates
  SortAndRemoveDuplicateRoots(TheRoots, NbSol);
}

//====================================================================================================

void math_DirectPolynomialRoots::Solve(const Standard_Real A,
                                       const Standard_Real B,
                                       const Standard_Real C)
{
  Done           = Standard_True;
  InfiniteStatus = Standard_False;
  NbSol          = 0;

  // Check for degenerate cases
  const Standard_Real anAbsA = std::abs(A);

  if (anAbsA < ZERO_TOLERANCE)
  {
    // Degenerate to linear
    Solve(B, C);
    return;
  }

  // Solve quadratic
  NbSol = SolveQuadratic(A, B, C, TheRoots);

  // Sort roots
  if (NbSol == 2 && TheRoots[0] > TheRoots[1])
  {
    std::swap(TheRoots[0], TheRoots[1]);
  }
}

//====================================================================================================

void math_DirectPolynomialRoots::Solve(const Standard_Real A, const Standard_Real B)
{
  Done           = Standard_True;
  InfiniteStatus = Standard_False;
  NbSol          = 0;

  const Standard_Real anAbsA = std::abs(A);
  const Standard_Real anAbsB = std::abs(B);

  if (anAbsA < ZERO_TOLERANCE)
  {
    if (anAbsB < ZERO_TOLERANCE)
    {
      // Infinite solutions: 0 = 0
      InfiniteStatus = Standard_True;
    }
    // else: No solution: B = 0 with B != 0
  }
  else
  {
    // One solution
    TheRoots[0] = -B / A;
    NbSol       = 1;
  }
}

//====================================================================================================

void math_DirectPolynomialRoots::Dump(Standard_OStream& o) const
{
  o << "math_DirectPolynomialRoots: " << std::endl;
  if (!Done)
  {
    o << "  Status = Not Done" << std::endl;
  }
  else if (InfiniteStatus)
  {
    o << "  Status = Infinite Solutions" << std::endl;
  }
  else
  {
    o << "  Status = Done" << std::endl;
    o << "  Number of solutions = " << NbSol << std::endl;
    for (Standard_Integer i = 0; i < NbSol; ++i)
    {
      o << "    Root[" << (i + 1) << "] = " << TheRoots[i] << std::endl;
    }
  }
}