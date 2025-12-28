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
const double ZERO_THRESHOLD = 1.0e-30;

/// Machine epsilon for floating-point arithmetic precision
const double MACHINE_EPSILON = RealEpsilon();

/// Floating-point radix (base) for coefficient scaling operations
const double FLOATING_RADIX = 2.0;

/// Inverse of natural logarithm of radix for exponent calculations
const double INV_LOG_RADIX = 1.0 / std::log(2.0);

/// Maximum number of Newton-Raphson refinement iterations
constexpr int MAX_NEWTON_ITERATIONS = 10;

/// Upper limit to prevent numerical overflow in computations
constexpr double OVERFLOW_LIMIT = 1.0e+80;

// Evaluates polynomial at point X using Horner's method
// Coefficients: Poly[0]*X^(N-1) + Poly[1]*X^(N-2) + ... + Poly[N-1]
double EvaluatePolynomial(const int theN,
                                 const double*   thePoly,
                                 const double    theX)
{
  double aResult = thePoly[0];
  for (int i = 1; i < theN; ++i)
  {
    aResult = aResult * theX + thePoly[i];
  }
  return aResult;
}

// Evaluates polynomial and its derivative simultaneously
void EvaluatePolynomialWithDerivative(const int theN,
                                      const double*   thePoly,
                                      const double    theX,
                                      double&         theValue,
                                      double&         theDerivative)
{
  theValue      = thePoly[0] * theX + thePoly[1];
  theDerivative = thePoly[0];

  for (int i = 2; i < theN; ++i)
  {
    theDerivative = theDerivative * theX + theValue;
    theValue      = theValue * theX + thePoly[i];
  }
}

// Refines root using Newton-Raphson iterations
double RefineRoot(const int theN,
                         const double*   thePoly,
                         const double    theInitialGuess)
{
  double       aValue        = 0.0;
  double       aDerivative   = 0.0;
  double       aSolution     = theInitialGuess;
  const double aInitialValue = EvaluatePolynomial(theN, thePoly, theInitialGuess);

  for (int iter = 1; iter < MAX_NEWTON_ITERATIONS; ++iter)
  {
    EvaluatePolynomialWithDerivative(theN, thePoly, aSolution, aValue, aDerivative);

    if (std::abs(aDerivative) <= ZERO_THRESHOLD)
    {
      break;
    }

    const double aDelta = -aValue / aDerivative;

    if (std::abs(aDelta) <= MACHINE_EPSILON * std::abs(aSolution))
    {
      break;
    }

    aSolution += aDelta;
  }

  // Return improved solution only if it's better
  return (std::abs(aValue) <= std::abs(aInitialValue)) ? aSolution : theInitialGuess;
}

// Single variadic template for polynomial root refinement and improvement
// Automatically handles any number of coefficients
template <typename... Coeffs>
inline double RefinePolynomialRoot(const double theInitialGuess, Coeffs... theCoeffs)
{
  const double aCoeffs[] = {theCoeffs...};
  const size_t        N         = sizeof...(Coeffs);
  return RefineRoot(N, aCoeffs, theInitialGuess);
}

// Helper method to scale and refine all roots of a polynomial
template <typename... Args>
inline void ScaleAndRefineAllRoots(double*         theRoots,
                                   const int theNbRoots,
                                   const double    theScaleFactor,
                                   Args... theCoeffs)
{
  for (int i = 0; i < theNbRoots; ++i)
  {
    theRoots[i] *= theScaleFactor;
    theRoots[i] = RefinePolynomialRoot(theRoots[i], theCoeffs...);
  }
}

// Computes base-2 exponent for coefficient scaling
int ComputeBaseExponent(const double theValue)
{
  if (theValue > 1.0)
  {
    return static_cast<int>(std::log(theValue) * INV_LOG_RADIX);
  }
  else if (theValue < -1.0)
  {
    return static_cast<int>(-std::log(-theValue) * INV_LOG_RADIX);
  }
  return 0;
}

// Scale coefficients to avoid overflow/underflow
struct ScaledCoefficients
{
  /// Scaled coefficient values
  double A, B, C, D, E;

  /// Scale factor for converting roots back to original coordinate system
  double ScaleFactor;

  void ScaleQuartic(double theA,
                    double theB,
                    double theC,
                    double theD,
                    double theE)
  {
    const int aExp       = ComputeBaseExponent(theE) / 4;
    ScaleFactor                       = std::pow(FLOATING_RADIX, aExp);
    const double aScaleFactor2 = ScaleFactor * ScaleFactor;

    A = theA / ScaleFactor;
    B = theB / aScaleFactor2;
    C = theC / (aScaleFactor2 * ScaleFactor);
    D = theD / (aScaleFactor2 * aScaleFactor2);
    E = theE / (aScaleFactor2 * aScaleFactor2);
  }

  void ScaleCubic(double theA, double theB, double theC, double theD)
  {
    const int aExp       = ComputeBaseExponent(theD) / 3;
    ScaleFactor                       = std::pow(FLOATING_RADIX, aExp);
    const double aScaleFactor2 = ScaleFactor * ScaleFactor;

    A = theA / ScaleFactor;
    B = theB / aScaleFactor2;
    C = theC / (aScaleFactor2 * ScaleFactor);
    D = theD; // Not used for cubic, but kept for consistency
  }
};

// Computes special discriminant for P < 0 cases with improved numerical stability
double ComputeSpecialDiscriminant(const double theBeta,
                                         const double theGamma,
                                         const double theDel,
                                         const double theA1)
{
  const double aSigma = theBeta * theGamma / 3.0 - 2.0 * theBeta * theBeta * theBeta / 27.0;
  const double aPsi   = theGamma * theGamma * (4.0 * theGamma - theBeta * theBeta) / 27.0;

  double aD1;
  if (aSigma >= 0.0)
  {
    aD1 = aSigma + 2.0 * std::sqrt(-theA1);
  }
  else
  {
    aD1 = aSigma - 2.0 * std::sqrt(-theA1);
  }

  const double aD2 = aPsi / aD1;

  if (std::abs(theDel - aD1) >= 18.0 * MACHINE_EPSILON * (std::abs(theDel) + std::abs(aD1))
      && std::abs(theDel - aD2) >= 24.0 * MACHINE_EPSILON * (std::abs(theDel) + std::abs(aD2)))
  {
    return (theDel - aD1) * (theDel - aD2) / 4.0;
  }

  return 0.0;
}

// Solves cubic equation with three real roots using trigonometric method
void SolveCubicThreeRealRoots(const double theBeta,
                              const double theGamma,
                              const double theDel,
                              const double theP,
                              const double theQ,
                              const double theDiscr,
                              double*      theRoots)
{
  if (theBeta == 0.0 && theQ == 0.0)
  {
    // Special case: x^3 + Px = 0
    theRoots[0] = std::sqrt(-theP);
    theRoots[1] = -theRoots[0];
    theRoots[2] = 0.0;
  }
  else
  {
    const double aSb    = (theBeta >= 0.0) ? 1.0 : -1.0;
    const double aOmega = std::atan(0.5 * theQ / std::sqrt(-theDiscr));
    const double aSp3   = std::sqrt(-theP / 3.0);
    const double aY1    = -2.0 * aSb * aSp3 * std::cos(M_PI / 6.0 - aSb * aOmega / 3.0);

    theRoots[0] = -theBeta / 3.0 + aY1;

    if (theBeta * theQ <= 0.0)
    {
      theRoots[1] = -theBeta / 3.0 + 2.0 * aSp3 * std::sin(aOmega / 3.0);
    }
    else
    {
      // Alternative formula for better accuracy
      const double aDbg  = theDel - theBeta * theGamma;
      const double aSdbg = (aDbg >= 0.0) ? 1.0 : -1.0;
      const double aDen1 =
        8.0 * theBeta * theBeta / 9.0 - 4.0 * theBeta * aY1 / 3.0 - 2.0 * theQ / aY1;
      const double aDen2 = 2.0 * aY1 * aY1 - theQ / aY1;
      theRoots[1]               = aDbg / aDen1 + aSdbg * std::sqrt(-27.0 * theDiscr) / aDen2;
    }

    // Use Vieta's formula for the third root
    theRoots[2] = -theDel / (theRoots[0] * theRoots[1]);
  }
}

// Solves cubic equation with one real root using Cardano's formula
void SolveCubicOneRealRoot(const double theBeta,
                           const double theDel,
                           const double theP,
                           const double theQ,
                           const double theDiscr,
                           double*      theRoots)
{
  double aU = std::sqrt(theDiscr) + std::abs(theQ / 2.0);
  aU               = (aU >= 0.0) ? std::pow(aU, 1.0 / 3.0) : -std::pow(std::abs(aU), 1.0 / 3.0);

  double aH;
  if (theP >= 0.0)
  {
    aH = aU * aU + theP / 3.0 + (theP / aU) * (theP / aU) / 9.0;
  }
  else
  {
    aH = aU * std::abs(theQ) / (aU * aU - theP / 3.0);
  }

  if (theBeta * theQ >= 0.0)
  {
    if (std::abs(aH) <= RealSmall() && std::abs(theQ) <= RealSmall())
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
void SolveCubicMultipleRoots(const double theBeta,
                             const double theGamma,
                             const double theDel,
                             const double theP,
                             const double theQ,
                             double*      theRoots,
                             int&   theNbRoots)
{
  theNbRoots               = 3;
  const double aSq  = (theQ >= 0.0) ? 1.0 : -1.0;
  const double aSp3 = std::sqrt(-theP / 3.0);

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
bool ShouldReduceDegreeQuartic(const double theA,
                                           const double theB,
                                           const double theC,
                                           const double theD,
                                           const double theE)
{
  if (std::abs(theA) <= ZERO_THRESHOLD)
  {
    return true;
  }

  // Modified by jgv, 22.01.09 for numerical stability
  double aMaxCoeff = ZERO_THRESHOLD;
  aMaxCoeff               = std::max(aMaxCoeff, std::abs(theB));
  aMaxCoeff               = std::max(aMaxCoeff, std::abs(theC));
  aMaxCoeff               = std::max(aMaxCoeff, std::abs(theD));
  aMaxCoeff               = std::max(aMaxCoeff, std::abs(theE));

  if (aMaxCoeff > ZERO_THRESHOLD)
  {
    aMaxCoeff = Epsilon(100.0 * aMaxCoeff);
  }

  if (std::abs(theA) <= aMaxCoeff)
  {
    const double aMaxCoeff1000 = 1000.0 * aMaxCoeff;
    bool    aWithA        = false;

    if (std::abs(theB) > ZERO_THRESHOLD && std::abs(theB) <= aMaxCoeff1000)
      aWithA = true;
    if (std::abs(theC) > ZERO_THRESHOLD && std::abs(theC) <= aMaxCoeff1000)
      aWithA = true;
    if (std::abs(theD) > ZERO_THRESHOLD && std::abs(theD) <= aMaxCoeff1000)
      aWithA = true;
    if (std::abs(theE) > ZERO_THRESHOLD && std::abs(theE) <= aMaxCoeff1000)
      aWithA = true;

    return !aWithA;
  }

  return false;
}

// Constructs and solves Ferrari's resolvent cubic equation
double SolveFerrariResolvent(const double theA,
                                    const double theB,
                                    const double theC,
                                    const double theD,
                                    bool&   theSuccess)
{
  // Construct resolvent cubic: Y^3 + R3*Y^2 + S3*Y + T3 = 0
  const double aR3 = -theB;
  const double aS3 = theA * theC - 4.0 * theD;
  const double aT3 = theD * (4.0 * theB - theA * theA) - theC * theC;

  math_DirectPolynomialRoots aCubicSolver(1.0, aR3, aS3, aT3);

  if (!aCubicSolver.IsDone())
  {
    theSuccess = false;
    return 0.0;
  }

  theSuccess = true;

  // Choose the largest root for numerical stability
  double aY0 = aCubicSolver.Value(1);
  for (int i = 2; i <= aCubicSolver.NbSolutions(); ++i)
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
  double P1;
  /// Constant coefficient of first quadratic factor
  double Q1;

  /// Linear coefficient of second quadratic factor
  double P2;
  /// Constant coefficient of second quadratic factor
  double Q2;
};

// Factors quartic into two quadratics using Ferrari's method
QuarticFactorization FactorQuarticViaFerrari(const double theA,
                                             const double theB,
                                             const double theC,
                                             const double theD,
                                             const double theY0)
{
  QuarticFactorization aFactors;

  // Compute discriminant and parameters
  const double aDiscr  = theA * theY0 * 0.5 - theC;
  const double aSdiscr = (aDiscr >= 0.0) ? 1.0 : -1.0;

  // Compute P0 and Q0 for the quadratic factors
  double aP0 = theA * theA * 0.25 - theB + theY0;
  aP0               = (aP0 < 0.0) ? 0.0 : std::sqrt(aP0);

  double aQ0 = theY0 * theY0 * 0.25 - theD;

  // Handle the case where Q0^2 is very close to zero more robustly
  // This fixes the numerical precision issue on Linux/Windows vs macOS
  if (std::abs(aQ0) < 10 * MACHINE_EPSILON)
  {
    aQ0 = 0.0;
  }
  else
  {
    aQ0 = (aQ0 < 0.0) ? 0.0 : std::sqrt(aQ0);
  }

  // Form coefficients for the two quadratic equations
  const double aAdemi    = theA * 0.5;
  const double aYdemi    = theY0 * 0.5;
  const double aSdiscrQ0 = aSdiscr * aQ0;

  aFactors.P1 = aAdemi + aP0;
  aFactors.Q1 = aYdemi + aSdiscrQ0;
  aFactors.P2 = aAdemi - aP0;
  aFactors.Q2 = aYdemi - aSdiscrQ0;

  // Clean up near-zero coefficients
  const double anEps = 100.0 * MACHINE_EPSILON;

  if (std::abs(aFactors.P1) <= anEps)
    aFactors.P1 = 0.0;
  if (std::abs(aFactors.P2) <= anEps)
    aFactors.P2 = 0.0;
  if (std::abs(aFactors.Q1) <= anEps)
    aFactors.Q1 = 0.0;
  if (std::abs(aFactors.Q2) <= anEps)
    aFactors.Q2 = 0.0;

  return aFactors;
}

} // namespace

//=================================================================================================

math_DirectPolynomialRoots::math_DirectPolynomialRoots(const double theA,
                                                       const double theB,
                                                       const double theC,
                                                       const double theD,
                                                       const double theE)
{
  myInfiniteStatus = false;
  myDone           = true;
  Solve(theA, theB, theC, theD, theE);
}

//=================================================================================================

math_DirectPolynomialRoots::math_DirectPolynomialRoots(const double theA,
                                                       const double theB,
                                                       const double theC,
                                                       const double theD)
{
  myDone           = true;
  myInfiniteStatus = false;
  Solve(theA, theB, theC, theD);
}

//=================================================================================================

math_DirectPolynomialRoots::math_DirectPolynomialRoots(const double theA,
                                                       const double theB,
                                                       const double theC)
{
  myDone           = true;
  myInfiniteStatus = false;
  Solve(theA, theB, theC);
}

//=================================================================================================

math_DirectPolynomialRoots::math_DirectPolynomialRoots(const double theA,
                                                       const double theB)
{
  myDone           = true;
  myInfiniteStatus = false;
  Solve(theA, theB);
}

//=================================================================================================

void math_DirectPolynomialRoots::Solve(const double theA,
                                       const double theB,
                                       const double theC,
                                       const double theD,
                                       const double theE)
{
  // Check for degree reduction
  if (ShouldReduceDegreeQuartic(theA, theB, theC, theD, theE))
  {
    Solve(theB, theC, theD, theE);
    return;
  }

  // Normalize coefficients
  const double A = theB / theA;
  const double B = theC / theA;
  const double C = theD / theA;
  const double D = theE / theA;

  // Scale coefficients to avoid overflow/underflow
  ScaledCoefficients aScaled;
  aScaled.ScaleQuartic(A, B, C, D, D);

  // Solve Ferrari's resolvent cubic
  bool    aSuccess = false;
  const double aY0 =
    SolveFerrariResolvent(aScaled.A, aScaled.B, aScaled.C, aScaled.D, aSuccess);

  if (!aSuccess)
  {
    myDone = false;
    return;
  }

  // Factor into two quadratics
  const QuarticFactorization aFactors =
    FactorQuarticViaFerrari(aScaled.A, aScaled.B, aScaled.C, aScaled.D, aY0);

  // Solve first quadratic
  math_DirectPolynomialRoots aQuadratic1(1.0, aFactors.P1, aFactors.Q1);
  if (!aQuadratic1.IsDone())
  {
    myDone = false;
    return;
  }

  // Solve second quadratic
  math_DirectPolynomialRoots aQuadratic2(1.0, aFactors.P2, aFactors.Q2);
  if (!aQuadratic2.IsDone())
  {
    myDone = false;
    return;
  }

  // Collect all roots
  myNbSol                 = aQuadratic1.NbSolutions() + aQuadratic2.NbSolutions();
  int aIndex = 0;

  for (int i = 0; i < aQuadratic1.NbSolutions(); ++i)
  {
    myRoots[aIndex++] = aQuadratic1.myRoots[i];
  }

  for (int i = 0; i < aQuadratic2.NbSolutions(); ++i)
  {
    myRoots[aIndex++] = aQuadratic2.myRoots[i];
  }

  // Apply inverse scaling and Newton-Raphson refinement to all roots
  ScaleAndRefineAllRoots(myRoots, myNbSol, aScaled.ScaleFactor, theA, theB, theC, theD, theE);
}

//=================================================================================================

void math_DirectPolynomialRoots::Solve(const double theA,
                                       const double theB,
                                       const double theC,
                                       const double theD)
{
  // Check for degree reduction
  if (std::abs(theA) <= ZERO_THRESHOLD)
  {
    Solve(theB, theC, theD);
    return;
  }

  // Normalize coefficients
  const double aBeta  = theB / theA;
  const double aGamma = theC / theA;
  const double aDel   = theD / theA;

  // Scale to avoid overflow/underflow
  ScaledCoefficients aScaled;
  aScaled.ScaleCubic(aBeta, aGamma, aDel, aDel);

  // Transform to depressed cubic: t^3 + Pt + Q = 0
  const double aP1 = aScaled.B;
  const double aP2 = -(aScaled.A * aScaled.A) / 3.0;
  double       aP  = aP1 + aP2;
  const double aEp = 5.0 * MACHINE_EPSILON * (std::abs(aP1) + std::abs(aP2));
  if (std::abs(aP) <= aEp)
    aP = 0.0;

  const double aQ1 = aScaled.C;
  const double aQ2 = -aScaled.A * aScaled.B / 3.0;
  const double aQ3 = 2.0 * (aScaled.A * aScaled.A * aScaled.A) / 27.0;
  double       aQ  = aQ1 + aQ2 + aQ3;
  const double aEq =
    10.0 * MACHINE_EPSILON * (std::abs(aQ1) + std::abs(aQ2) + std::abs(aQ3));
  if (std::abs(aQ) <= aEq)
    aQ = 0.0;

  // Check for overflow
  if (std::abs(aP) > OVERFLOW_LIMIT)
  {
    myDone = false;
    return;
  }

  // Compute discriminant
  const double aA1    = (aP * aP * aP) / 27.0;
  const double aA2    = (aQ * aQ) / 4.0;
  double       aDiscr = aA1 + aA2;

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

void math_DirectPolynomialRoots::Solve(const double theA,
                                       const double theB,
                                       const double theC)
{
  // Check for degree reduction
  if (std::abs(theA) <= ZERO_THRESHOLD)
  {
    Solve(theB, theC);
    return;
  }

  // Solve normalized quadratic x^2 + P*x + Q = 0
  const double P = theB / theA;
  const double Q = theC / theA;

  // Compute discriminant with error bounds
  const double aEpsD    = 3.0 * MACHINE_EPSILON * (P * P + std::abs(4.0 * Q));
  double       aDiscrim = P * P - 4.0 * Q;

  if (std::abs(aDiscrim) <= aEpsD)
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
      myRoots[0] = -(P + std::sqrt(aDiscrim)) / 2.0;
    }
    else
    {
      myRoots[0] = -(P - std::sqrt(aDiscrim)) / 2.0;
    }
    myRoots[0] = RefinePolynomialRoot(myRoots[0], 1.0, P, Q);
    myRoots[1] = Q / myRoots[0];
    myRoots[1] = RefinePolynomialRoot(myRoots[1], 1.0, P, Q);
  }
}

//=================================================================================================

void math_DirectPolynomialRoots::Solve(const double theA, const double theB)
{
  if (std::abs(theA) <= ZERO_THRESHOLD)
  {
    if (std::abs(theB) <= ZERO_THRESHOLD)
    {
      // 0 = 0: infinite solutions
      myInfiniteStatus = true;
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
    for (int i = 1; i <= myNbSol; i++)
    {
      theStream << " Solution number " << i << " = " << myRoots[i - 1] << "\n";
    }
  }
}