// Copyright (c) 1997-1999 Matra Datavision
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

#define No_Standard_RangeError
#define No_Standard_OutOfRange
#define No_Standard_DimensionError

#include <math_DirectPolynomialRoots.hxx>
#include <StdFail_InfiniteSolutions.hxx>

#include <algorithm>
#include <array>
#include <cmath>

// @file math_DirectPolynomialRoots.cxx
// @brief Implementation of direct polynomial root finding algorithms
//
// This implementation provides robust numerical methods for finding all real
// roots of polynomials up to degree 4. The algorithms are based on classical
// algebraic methods enhanced with modern numerical techniques for stability.
//
// Algorithm References:
//   - ALGORITHMES NUMERIQUES ANALYSE ET MISE EN OEUVRE, tome 2
//     (equations et systemes non lineaires) J. VIGNES editions TECHNIP
//   - Ferrari's method for quartic equations
//   - Cardano's formula for cubic equations
//   - Numerically stable quadratic formula
//   - Newton-Raphson refinement for improved accuracy

namespace
{

/// Threshold below which coefficients are considered effectively zero
const Standard_Real ZERO_THRESHOLD = 1.0e-30;

/// Machine epsilon for floating-point arithmetic precision
const Standard_Real MACHINE_EPSILON = RealEpsilon();

/// Floating-point radix (base) for coefficient scaling operations
const Standard_Real FLOATING_RADIX = 2.0;

/// Inverse of natural logarithm of radix for exponent calculations
const Standard_Real INV_LOG_RADIX = 1.0 / Log(2.0);

/// Maximum number of Newton-Raphson refinement iterations
constexpr Standard_Integer MAX_NEWTON_ITERATIONS = 10;

/// Upper limit to prevent numerical overflow in computations
constexpr Standard_Real OVERFLOW_LIMIT = 1.0e+80;

// Evaluates polynomial at point X using Horner's method
// Coefficients: Poly[0]*X^(N-1) + Poly[1]*X^(N-2) + ... + Poly[N-1]
Standard_Real EvaluatePolynomial(const Standard_Integer theN,
                                 const Standard_Real*   thePoly,
                                 const Standard_Real    theX)
{
  Standard_Real aResult = thePoly[0];
  for (Standard_Integer i = 1; i < theN; ++i)
  {
    aResult = aResult * theX + thePoly[i];
  }
  return aResult;
}

// Evaluates polynomial and its derivative simultaneously
void EvaluatePolynomialWithDerivative(const Standard_Integer theN,
                                      const Standard_Real*   thePoly,
                                      const Standard_Real    theX,
                                      Standard_Real&         theValue,
                                      Standard_Real&         theDerivative)
{
  theValue      = thePoly[0] * theX + thePoly[1];
  theDerivative = thePoly[0];

  for (Standard_Integer i = 2; i < theN; ++i)
  {
    theDerivative = theDerivative * theX + theValue;
    theValue      = theValue * theX + thePoly[i];
  }
}

// Refines root using Newton-Raphson iterations
Standard_Real RefineRoot(const Standard_Integer theN,
                         const Standard_Real*   thePoly,
                         const Standard_Real    theInitialGuess)
{
  Standard_Real       aValue        = 0.0;
  Standard_Real       aDerivative   = 0.0;
  Standard_Real       aSolution     = theInitialGuess;
  const Standard_Real aInitialValue = EvaluatePolynomial(theN, thePoly, theInitialGuess);

  for (Standard_Integer iter = 1; iter < MAX_NEWTON_ITERATIONS; ++iter)
  {
    EvaluatePolynomialWithDerivative(theN, thePoly, aSolution, aValue, aDerivative);

    if (Abs(aDerivative) <= ZERO_THRESHOLD)
    {
      break;
    }

    const Standard_Real aDelta = -aValue / aDerivative;

    if (Abs(aDelta) <= MACHINE_EPSILON * Abs(aSolution))
    {
      break;
    }

    aSolution += aDelta;
  }

  // Return improved solution only if it's better
  return (Abs(aValue) <= Abs(aInitialValue)) ? aSolution : theInitialGuess;
}

// Single variadic template for polynomial root refinement and improvement
// Automatically handles any number of coefficients
template <typename... Coeffs>
inline Standard_Real RefinePolynomialRoot(const Standard_Real theInitialGuess, Coeffs... theCoeffs)
{
  const Standard_Real aCoeffs[] = {theCoeffs...};
  constexpr size_t    N         = sizeof...(Coeffs);
  return RefineRoot(N, aCoeffs, theInitialGuess);
}

// Helper method to scale and refine all roots of a polynomial
template <typename... Args>
inline void ScaleAndRefineAllRoots(Standard_Real*         theRoots,
                                   const Standard_Integer theNbRoots,
                                   const Standard_Real    theScaleFactor,
                                   Args... theCoeffs)
{
  for (Standard_Integer i = 0; i < theNbRoots; ++i)
  {
    theRoots[i] *= theScaleFactor;
    theRoots[i] = RefinePolynomialRoot(theRoots[i], theCoeffs...);
  }
}

// Computes base-2 exponent for coefficient scaling
Standard_Integer ComputeBaseExponent(const Standard_Real theValue)
{
  if (theValue > 1.0)
  {
    return static_cast<Standard_Integer>(Log(theValue) * INV_LOG_RADIX);
  }
  else if (theValue < -1.0)
  {
    return static_cast<Standard_Integer>(-Log(-theValue) * INV_LOG_RADIX);
  }
  return 0;
}

// Scale coefficients to avoid overflow/underflow
struct ScaledCoefficients
{
  /// Scaled coefficient values
  Standard_Real A, B, C, D, E;

  /// Scale factor for converting roots back to original coordinate system
  Standard_Real ScaleFactor;

  void ScaleQuartic(Standard_Real theA,
                    Standard_Real theB,
                    Standard_Real theC,
                    Standard_Real theD,
                    Standard_Real theE)
  {
    const Standard_Integer aExp       = ComputeBaseExponent(theE) / 4;
    ScaleFactor                       = Pow(FLOATING_RADIX, aExp);
    const Standard_Real aScaleFactor2 = ScaleFactor * ScaleFactor;

    A = theA / ScaleFactor;
    B = theB / aScaleFactor2;
    C = theC / (aScaleFactor2 * ScaleFactor);
    D = theD / (aScaleFactor2 * aScaleFactor2);
    E = theE / (aScaleFactor2 * aScaleFactor2);
  }

  void ScaleCubic(Standard_Real theA, Standard_Real theB, Standard_Real theC, Standard_Real theD)
  {
    const Standard_Integer aExp       = ComputeBaseExponent(theD) / 3;
    ScaleFactor                       = Pow(FLOATING_RADIX, aExp);
    const Standard_Real aScaleFactor2 = ScaleFactor * ScaleFactor;

    A = theA / ScaleFactor;
    B = theB / aScaleFactor2;
    C = theC / (aScaleFactor2 * ScaleFactor);
    D = theD; // Not used for cubic, but kept for consistency
  }
};

// Computes special discriminant for P < 0 cases with improved numerical stability
Standard_Real ComputeSpecialDiscriminant(const Standard_Real theBeta,
                                         const Standard_Real theGamma,
                                         const Standard_Real theDel,
                                         const Standard_Real theA1)
{
  const Standard_Real aSigma = theBeta * theGamma / 3.0 - 2.0 * theBeta * theBeta * theBeta / 27.0;
  const Standard_Real aPsi   = theGamma * theGamma * (4.0 * theGamma - theBeta * theBeta) / 27.0;

  Standard_Real aD1;
  if (aSigma >= 0.0)
  {
    aD1 = aSigma + 2.0 * Sqrt(-theA1);
  }
  else
  {
    aD1 = aSigma - 2.0 * Sqrt(-theA1);
  }

  const Standard_Real aD2 = aPsi / aD1;

  if (Abs(theDel - aD1) >= 18.0 * MACHINE_EPSILON * (Abs(theDel) + Abs(aD1))
      && Abs(theDel - aD2) >= 24.0 * MACHINE_EPSILON * (Abs(theDel) + Abs(aD2)))
  {
    return (theDel - aD1) * (theDel - aD2) / 4.0;
  }

  return 0.0;
}

// Solves cubic equation with three real roots using trigonometric method
void SolveCubicThreeRealRoots(const Standard_Real theBeta,
                              const Standard_Real theGamma,
                              const Standard_Real theDel,
                              const Standard_Real theP,
                              const Standard_Real theQ,
                              const Standard_Real theDiscr,
                              Standard_Real*      theRoots)
{
  if (theBeta == 0.0 && theQ == 0.0)
  {
    // Special case: x^3 + Px = 0
    theRoots[0] = Sqrt(-theP);
    theRoots[1] = -theRoots[0];
    theRoots[2] = 0.0;
  }
  else
  {
    const Standard_Real aSb    = (theBeta >= 0.0) ? 1.0 : -1.0;
    const Standard_Real aOmega = ATan(0.5 * theQ / Sqrt(-theDiscr));
    const Standard_Real aSp3   = Sqrt(-theP / 3.0);
    const Standard_Real aY1    = -2.0 * aSb * aSp3 * Cos(M_PI / 6.0 - aSb * aOmega / 3.0);

    theRoots[0] = -theBeta / 3.0 + aY1;

    if (theBeta * theQ <= 0.0)
    {
      theRoots[1] = -theBeta / 3.0 + 2.0 * aSp3 * Sin(aOmega / 3.0);
    }
    else
    {
      // Alternative formula for better accuracy
      const Standard_Real aDbg  = theDel - theBeta * theGamma;
      const Standard_Real aSdbg = (aDbg >= 0.0) ? 1.0 : -1.0;
      const Standard_Real aDen1 =
        8.0 * theBeta * theBeta / 9.0 - 4.0 * theBeta * aY1 / 3.0 - 2.0 * theQ / aY1;
      const Standard_Real aDen2 = 2.0 * aY1 * aY1 - theQ / aY1;
      theRoots[1]               = aDbg / aDen1 + aSdbg * Sqrt(-27.0 * theDiscr) / aDen2;
    }

    // Use Vieta's formula for the third root
    theRoots[2] = -theDel / (theRoots[0] * theRoots[1]);
  }
}

// Solves cubic equation with one real root using Cardano's formula
void SolveCubicOneRealRoot(const Standard_Real theBeta,
                           const Standard_Real theDel,
                           const Standard_Real theP,
                           const Standard_Real theQ,
                           const Standard_Real theDiscr,
                           Standard_Real*      theRoots)
{
  Standard_Real aU = Sqrt(theDiscr) + Abs(theQ / 2.0);
  aU               = (aU >= 0.0) ? Pow(aU, 1.0 / 3.0) : -Pow(Abs(aU), 1.0 / 3.0);

  Standard_Real aH;
  if (theP >= 0.0)
  {
    aH = aU * aU + theP / 3.0 + (theP / aU) * (theP / aU) / 9.0;
  }
  else
  {
    aH = aU * Abs(theQ) / (aU * aU - theP / 3.0);
  }

  if (theBeta * theQ >= 0.0)
  {
    if (Abs(aH) <= RealSmall() && Abs(theQ) <= RealSmall())
    {
      theRoots[0] = -theBeta / 3.0 - aU + theP / (3.0 * aU);
    }
    else
    {
      theRoots[0] = -theBeta / 3.0 - theQ / aH;
    }
  }
  else
  {
    theRoots[0] = -theDel / (theBeta * theBeta / 9.0 + aH - theBeta * theQ / (3.0 * aH));
  }
}

// Solves cubic equation with multiple roots (discriminant = 0)
void SolveCubicMultipleRoots(const Standard_Real theBeta,
                             const Standard_Real theGamma,
                             const Standard_Real theDel,
                             const Standard_Real theP,
                             const Standard_Real theQ,
                             Standard_Real*      theRoots,
                             Standard_Integer&   theNbRoots)
{
  theNbRoots               = 3;
  const Standard_Real aSq  = (theQ >= 0.0) ? 1.0 : -1.0;
  const Standard_Real aSp3 = Sqrt(-theP / 3.0);

  if (theBeta * theQ <= 0.0)
  {
    theRoots[0] = -theBeta / 3.0 + aSq * aSp3;
    theRoots[1] = theRoots[0];

    if (theBeta * theQ == 0.0)
    {
      theRoots[2] = -theBeta / 3.0 - 2.0 * aSq * aSp3;
    }
    else
    {
      theRoots[2] = -theDel / (theRoots[0] * theRoots[1]);
    }
  }
  else
  {
    theRoots[0] = -theGamma / (theBeta + 3.0 * aSq * aSp3);
    theRoots[1] = theRoots[0];
    theRoots[2] = -theBeta / 3.0 - 2.0 * aSq * aSp3;
  }
}

// Determines if quartic should be reduced to lower degree
Standard_Boolean ShouldReduceDegreeQuartic(const Standard_Real theA,
                                           const Standard_Real theB,
                                           const Standard_Real theC,
                                           const Standard_Real theD,
                                           const Standard_Real theE)
{
  if (Abs(theA) <= ZERO_THRESHOLD)
  {
    return true;
  }

  // Modified by jgv, 22.01.09 for numerical stability
  Standard_Real aMaxCoeff = ZERO_THRESHOLD;
  aMaxCoeff               = Max(aMaxCoeff, Abs(theB));
  aMaxCoeff               = Max(aMaxCoeff, Abs(theC));
  aMaxCoeff               = Max(aMaxCoeff, Abs(theD));
  aMaxCoeff               = Max(aMaxCoeff, Abs(theE));

  if (aMaxCoeff > ZERO_THRESHOLD)
  {
    aMaxCoeff = Epsilon(100.0 * aMaxCoeff);
  }

  if (Abs(theA) <= aMaxCoeff)
  {
    const Standard_Real aMaxCoeff1000 = 1000.0 * aMaxCoeff;
    Standard_Boolean    aWithA        = false;

    if (Abs(theB) > ZERO_THRESHOLD && Abs(theB) <= aMaxCoeff1000)
      aWithA = true;
    if (Abs(theC) > ZERO_THRESHOLD && Abs(theC) <= aMaxCoeff1000)
      aWithA = true;
    if (Abs(theD) > ZERO_THRESHOLD && Abs(theD) <= aMaxCoeff1000)
      aWithA = true;
    if (Abs(theE) > ZERO_THRESHOLD && Abs(theE) <= aMaxCoeff1000)
      aWithA = true;

    return !aWithA;
  }

  return false;
}

// Constructs and solves Ferrari's resolvent cubic equation
Standard_Real SolveFerrariResolvent(const Standard_Real theA,
                                    const Standard_Real theB,
                                    const Standard_Real theC,
                                    const Standard_Real theD,
                                    Standard_Boolean&   theSuccess)
{
  // Construct resolvent cubic: Y^3 + R3*Y^2 + S3*Y + T3 = 0
  const Standard_Real aR3 = -theB;
  const Standard_Real aS3 = theA * theC - 4.0 * theD;
  const Standard_Real aT3 = theD * (4.0 * theB - theA * theA) - theC * theC;

  math_DirectPolynomialRoots aCubicSolver(1.0, aR3, aS3, aT3);

  if (!aCubicSolver.IsDone())
  {
    theSuccess = false;
    return 0.0;
  }

  theSuccess = true;

  // Choose the largest root for numerical stability
  Standard_Real aY0 = aCubicSolver.Value(1);
  for (Standard_Integer i = 2; i <= aCubicSolver.NbSolutions(); ++i)
  {
    if (aCubicSolver.Value(i) > aY0)
    {
      aY0 = aCubicSolver.Value(i);
    }
  }

  return aY0;
}

// Structure for quartic factorization into two quadratic equations
struct QuarticFactorization
{
  /// Linear coefficient of first quadratic factor
  Standard_Real P1;
  /// Constant coefficient of first quadratic factor
  Standard_Real Q1;

  /// Linear coefficient of second quadratic factor
  Standard_Real P2;
  /// Constant coefficient of second quadratic factor
  Standard_Real Q2;
};

// Factors quartic into two quadratics using Ferrari's method
QuarticFactorization FactorQuarticViaFerrari(const Standard_Real theA,
                                             const Standard_Real theB,
                                             const Standard_Real theC,
                                             const Standard_Real theD,
                                             const Standard_Real theY0)
{
  QuarticFactorization aFactors;

  // Compute discriminant and parameters
  const Standard_Real aDiscr  = theA * theY0 * 0.5 - theC;
  const Standard_Real aSdiscr = (aDiscr >= 0.0) ? 1.0 : -1.0;

  // Compute P0 and Q0 for the quadratic factors
  Standard_Real aP0 = theA * theA * 0.25 - theB + theY0;
  aP0               = (aP0 < 0.0) ? 0.0 : Sqrt(aP0);

  Standard_Real aQ0 = theY0 * theY0 * 0.25 - theD;
  aQ0               = (aQ0 < 0.0) ? 0.0 : Sqrt(aQ0);

  // Form coefficients for the two quadratic equations
  const Standard_Real aAdemi    = theA * 0.5;
  const Standard_Real aYdemi    = theY0 * 0.5;
  const Standard_Real aSdiscrQ0 = aSdiscr * aQ0;

  aFactors.P1 = aAdemi + aP0;
  aFactors.Q1 = aYdemi + aSdiscrQ0;
  aFactors.P2 = aAdemi - aP0;
  aFactors.Q2 = aYdemi - aSdiscrQ0;

  // Clean up near-zero coefficients
  const Standard_Real anEps = 100.0 * MACHINE_EPSILON;

  if (Abs(aFactors.P1) <= anEps)
    aFactors.P1 = 0.0;
  if (Abs(aFactors.P2) <= anEps)
    aFactors.P2 = 0.0;
  if (Abs(aFactors.Q1) <= anEps)
    aFactors.Q1 = 0.0;
  if (Abs(aFactors.Q2) <= anEps)
    aFactors.Q2 = 0.0;

  return aFactors;
}

} // namespace

//=================================================================================================

math_DirectPolynomialRoots::math_DirectPolynomialRoots(const Standard_Real theA,
                                                       const Standard_Real theB,
                                                       const Standard_Real theC,
                                                       const Standard_Real theD,
                                                       const Standard_Real theE)
{
  myInfiniteStatus = Standard_False;
  myDone           = Standard_True;
  Solve(theA, theB, theC, theD, theE);
}

//=================================================================================================

math_DirectPolynomialRoots::math_DirectPolynomialRoots(const Standard_Real theA,
                                                       const Standard_Real theB,
                                                       const Standard_Real theC,
                                                       const Standard_Real theD)
{
  myDone           = Standard_True;
  myInfiniteStatus = Standard_False;
  Solve(theA, theB, theC, theD);
}

//=================================================================================================

math_DirectPolynomialRoots::math_DirectPolynomialRoots(const Standard_Real theA,
                                                       const Standard_Real theB,
                                                       const Standard_Real theC)
{
  myDone           = Standard_True;
  myInfiniteStatus = Standard_False;
  Solve(theA, theB, theC);
}

//=================================================================================================

math_DirectPolynomialRoots::math_DirectPolynomialRoots(const Standard_Real theA,
                                                       const Standard_Real theB)
{
  myDone           = Standard_True;
  myInfiniteStatus = Standard_False;
  Solve(theA, theB);
}

//=================================================================================================

void math_DirectPolynomialRoots::Solve(const Standard_Real theA,
                                       const Standard_Real theB,
                                       const Standard_Real theC,
                                       const Standard_Real theD,
                                       const Standard_Real theE)
{
  // Check for degree reduction
  if (ShouldReduceDegreeQuartic(theA, theB, theC, theD, theE))
  {
    Solve(theB, theC, theD, theE);
    return;
  }

  // Normalize coefficients
  const Standard_Real A = theB / theA;
  const Standard_Real B = theC / theA;
  const Standard_Real C = theD / theA;
  const Standard_Real D = theE / theA;

  // Scale coefficients to avoid overflow/underflow
  ScaledCoefficients aScaled;
  aScaled.ScaleQuartic(A, B, C, D, D);

  // Solve Ferrari's resolvent cubic
  Standard_Boolean    aSuccess = Standard_False;
  const Standard_Real aY0 =
    SolveFerrariResolvent(aScaled.A, aScaled.B, aScaled.C, aScaled.D, aSuccess);

  if (!aSuccess)
  {
    myDone = Standard_False;
    return;
  }

  // Factor into two quadratics
  const QuarticFactorization aFactors =
    FactorQuarticViaFerrari(aScaled.A, aScaled.B, aScaled.C, aScaled.D, aY0);

  // Solve first quadratic
  math_DirectPolynomialRoots aQuadratic1(1.0, aFactors.P1, aFactors.Q1);
  if (!aQuadratic1.IsDone())
  {
    myDone = Standard_False;
    return;
  }

  // Solve second quadratic
  math_DirectPolynomialRoots aQuadratic2(1.0, aFactors.P2, aFactors.Q2);
  if (!aQuadratic2.IsDone())
  {
    myDone = Standard_False;
    return;
  }

  // Collect all roots
  myNbSol                 = aQuadratic1.NbSolutions() + aQuadratic2.NbSolutions();
  Standard_Integer aIndex = 0;

  for (Standard_Integer i = 0; i < aQuadratic1.NbSolutions(); ++i)
  {
    myRoots[aIndex++] = aQuadratic1.myRoots[i];
  }

  for (Standard_Integer i = 0; i < aQuadratic2.NbSolutions(); ++i)
  {
    myRoots[aIndex++] = aQuadratic2.myRoots[i];
  }

  // Apply inverse scaling and Newton-Raphson refinement to all roots
  ScaleAndRefineAllRoots(myRoots, myNbSol, aScaled.ScaleFactor, theA, theB, theC, theD, theE);
}

//=================================================================================================

void math_DirectPolynomialRoots::Solve(const Standard_Real theA,
                                       const Standard_Real theB,
                                       const Standard_Real theC,
                                       const Standard_Real theD)
{
  // Check for degree reduction
  if (Abs(theA) <= ZERO_THRESHOLD)
  {
    Solve(theB, theC, theD);
    return;
  }

  // Normalize coefficients
  const Standard_Real aBeta  = theB / theA;
  const Standard_Real aGamma = theC / theA;
  const Standard_Real aDel   = theD / theA;

  // Scale to avoid overflow/underflow
  ScaledCoefficients aScaled;
  aScaled.ScaleCubic(aBeta, aGamma, aDel, aDel);

  // Transform to depressed cubic: t^3 + Pt + Q = 0
  const Standard_Real aP1 = aScaled.B;
  const Standard_Real aP2 = -(aScaled.A * aScaled.A) / 3.0;
  Standard_Real       aP  = aP1 + aP2;
  const Standard_Real aEp = 5.0 * MACHINE_EPSILON * (Abs(aP1) + Abs(aP2));
  if (Abs(aP) <= aEp)
    aP = 0.0;

  const Standard_Real aQ1 = aScaled.C;
  const Standard_Real aQ2 = -aScaled.A * aScaled.B / 3.0;
  const Standard_Real aQ3 = 2.0 * (aScaled.A * aScaled.A * aScaled.A) / 27.0;
  Standard_Real       aQ  = aQ1 + aQ2 + aQ3;
  const Standard_Real aEq = 10.0 * MACHINE_EPSILON * (Abs(aQ1) + Abs(aQ2) + Abs(aQ3));
  if (Abs(aQ) <= aEq)
    aQ = 0.0;

  // Check for overflow
  if (Abs(aP) > OVERFLOW_LIMIT)
  {
    myDone = Standard_False;
    return;
  }

  // Compute discriminant
  const Standard_Real aA1    = (aP * aP * aP) / 27.0;
  const Standard_Real aA2    = (aQ * aQ) / 4.0;
  Standard_Real       aDiscr = aA1 + aA2;

  // Special handling for P < 0
  if (aP < 0.0)
  {
    aDiscr = ComputeSpecialDiscriminant(aScaled.A, aScaled.B, aScaled.C, aA1);
  }

  // Solve based on discriminant
  if (aDiscr < 0.0)
  {
    // Three distinct real roots
    myNbSol = 3;
    SolveCubicThreeRealRoots(aScaled.A, aScaled.B, aScaled.C, aP, aQ, aDiscr, myRoots);
  }
  else if (aDiscr > 0.0)
  {
    // One real root
    myNbSol = 1;
    SolveCubicOneRealRoot(aScaled.A, aScaled.C, aP, aQ, aDiscr, myRoots);
  }
  else
  {
    // Multiple roots
    SolveCubicMultipleRoots(aScaled.A, aScaled.B, aScaled.C, aP, aQ, myRoots, myNbSol);
  }

  // Apply inverse scaling and Newton-Raphson refinement to all roots
  ScaleAndRefineAllRoots(myRoots, myNbSol, aScaled.ScaleFactor, theA, theB, theC, theD);
}

//=================================================================================================

void math_DirectPolynomialRoots::Solve(const Standard_Real theA,
                                       const Standard_Real theB,
                                       const Standard_Real theC)
{
  // Check for degree reduction
  if (Abs(theA) <= ZERO_THRESHOLD)
  {
    Solve(theB, theC);
    return;
  }

  // Solve normalized quadratic x^2 + P*x + Q = 0
  const Standard_Real P = theB / theA;
  const Standard_Real Q = theC / theA;

  // Compute discriminant with error bounds
  const Standard_Real aEpsD    = 3.0 * MACHINE_EPSILON * (P * P + Abs(4.0 * Q));
  Standard_Real       aDiscrim = P * P - 4.0 * Q;

  if (Abs(aDiscrim) <= aEpsD)
  {
    aDiscrim = 0.0;
  }

  if (aDiscrim < 0.0)
  {
    // No real roots
    myNbSol = 0;
  }
  else if (aDiscrim == 0.0)
  {
    // Double root
    myNbSol    = 2;
    myRoots[0] = -0.5 * P;
    myRoots[0] = RefinePolynomialRoot(myRoots[0], 1.0, P, Q);
    myRoots[1] = myRoots[0];
  }
  else
  {
    // Two distinct real roots - use numerically stable formula
    myNbSol = 2;
    if (P > 0.0)
    {
      myRoots[0] = -(P + Sqrt(aDiscrim)) / 2.0;
    }
    else
    {
      myRoots[0] = -(P - Sqrt(aDiscrim)) / 2.0;
    }
    myRoots[0] = RefinePolynomialRoot(myRoots[0], 1.0, P, Q);
    myRoots[1] = Q / myRoots[0];
    myRoots[1] = RefinePolynomialRoot(myRoots[1], 1.0, P, Q);
  }
}

//=================================================================================================

void math_DirectPolynomialRoots::Solve(const Standard_Real theA, const Standard_Real theB)
{
  if (Abs(theA) <= ZERO_THRESHOLD)
  {
    if (Abs(theB) <= ZERO_THRESHOLD)
    {
      // 0 = 0: infinite solutions
      myInfiniteStatus = Standard_True;
      return;
    }
    // 0*x + B = 0: no solution
    myNbSol = 0;
    return;
  }

  // Normal case: unique solution
  myNbSol    = 1;
  myRoots[0] = -theB / theA;
}

//=================================================================================================

void math_DirectPolynomialRoots::Dump(Standard_OStream& theStream) const
{
  theStream << "math_DirectPolynomialRoots ";

  if (!myDone)
  {
    theStream << " Not Done \n";
  }
  else if (myInfiniteStatus)
  {
    theStream << " Status = Infinity Roots \n";
  }
  else
  {
    theStream << " Status = Not Infinity Roots \n";
    theStream << " Number of solutions = " << myNbSol << "\n";
    for (Standard_Integer i = 1; i <= myNbSol; i++)
    {
      theStream << " Solution number " << i << " = " << myRoots[i - 1] << "\n";
    }
  }
}