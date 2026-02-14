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

#ifndef _MathPoly_Laguerre_HeaderFile
#define _MathPoly_Laguerre_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Core.hxx>
#include <MathPoly_Quartic.hxx>

#include <array>
#include <cmath>
#include <complex>
#include <algorithm>

//! Polynomial root finding algorithms using Laguerre's method.
namespace MathPoly
{
using namespace MathUtils;

//! Maximum polynomial degree supported by Laguerre solver.
constexpr int THE_MAX_POLY_DEGREE = 20;

//! Result for general polynomial solver.
struct GeneralPolyResult
{
  MathUtils::Status                                     Status = MathUtils::Status::NotConverged;
  std::array<double, THE_MAX_POLY_DEGREE>               Roots  = {};
  std::array<std::complex<double>, THE_MAX_POLY_DEGREE> ComplexRoots   = {};
  size_t                                                NbRoots        = 0;
  size_t                                                NbComplexRoots = 0;

  bool IsDone() const { return Status == MathUtils::Status::OK; }

  explicit operator bool() const { return IsDone(); }
};

namespace detail
{

//! Evaluate polynomial and its first two derivatives at x (complex version).
//! Polynomial: c[n]*x^n + c[n-1]*x^(n-1) + ... + c[1]*x + c[0]
//! @param theCoeffs coefficients array (c[0] = constant term)
//! @param theDegree polynomial degree
//! @param theX evaluation point
//! @param theP output: polynomial value P(x)
//! @param theDP output: first derivative P'(x)
//! @param theD2P output: second derivative P''(x)
inline void EvaluatePolynomialWithDerivatives(const double*         theCoeffs,
                                              int                   theDegree,
                                              std::complex<double>  theX,
                                              std::complex<double>& theP,
                                              std::complex<double>& theDP,
                                              std::complex<double>& theD2P)
{
  // Horner's method with derivative computation
  theP   = std::complex<double>(theCoeffs[theDegree], 0.0);
  theDP  = std::complex<double>(0.0, 0.0);
  theD2P = std::complex<double>(0.0, 0.0);

  for (int i = theDegree - 1; i >= 0; --i)
  {
    theD2P = theD2P * theX + theDP;
    theDP  = theDP * theX + theP;
    theP   = theP * theX + std::complex<double>(theCoeffs[i], 0.0);
  }
  theD2P *= 2.0;
}

//! Laguerre iteration to find one root of polynomial.
//! @param theCoeffs coefficients array
//! @param theDegree polynomial degree
//! @param theX0 initial guess
//! @param theTol convergence tolerance
//! @param theMaxIter maximum iterations
//! @return refined root estimate
inline std::complex<double> LaguerreIteration(const double*        theCoeffs,
                                              int                  theDegree,
                                              std::complex<double> theX0,
                                              double               theTol,
                                              int                  theMaxIter)
{
  std::complex<double> aX = theX0;
  const double         aN = static_cast<double>(theDegree);

  for (int anIter = 0; anIter < theMaxIter; ++anIter)
  {
    std::complex<double> aP, aDP, aD2P;
    EvaluatePolynomialWithDerivatives(theCoeffs, theDegree, aX, aP, aDP, aD2P);

    const double aAbsP = std::abs(aP);
    if (aAbsP < theTol)
    {
      return aX;
    }

    // Laguerre's formula: x_new = x - n*P / (P' +/- sqrt((n-1)*((n-1)*P'^2 - n*P*P'')))
    std::complex<double> aG  = aDP / aP;
    std::complex<double> aH  = aG * aG - aD2P / aP;
    std::complex<double> aSq = std::sqrt((aN - 1.0) * (aN * aH - aG * aG));

    // Choose denominator with larger magnitude for stability
    std::complex<double> aDenom1 = aG + aSq;
    std::complex<double> aDenom2 = aG - aSq;
    std::complex<double> aDenom  = (std::abs(aDenom1) > std::abs(aDenom2)) ? aDenom1 : aDenom2;

    std::complex<double> aDelta;
    if (std::abs(aDenom) < MathUtils::THE_ZERO_TOL)
    {
      // Fallback: simple Newton step
      if (std::abs(aDP) < MathUtils::THE_ZERO_TOL)
      {
        aDelta = std::complex<double>(1.0 + std::abs(aX), 0.0);
      }
      else
      {
        aDelta = aP / aDP;
      }
    }
    else
    {
      aDelta = std::complex<double>(aN, 0.0) / aDenom;
    }

    aX -= aDelta;

    // Check convergence
    if (std::abs(aDelta) < theTol * (1.0 + std::abs(aX)))
    {
      return aX;
    }
  }

  return aX;
}

//! Deflate polynomial by removing a real root.
//! Divides p(x) by (x - root) to get q(x).
//! @param theCoeffs input coefficients, output deflated coefficients
//! @param theDegree polynomial degree (will be decremented)
//! @param theRoot root to remove
inline void DeflateReal(double* theCoeffs, int& theDegree, double theRoot)
{
  // Synthetic division
  double aCarry = theCoeffs[theDegree];
  for (int i = theDegree - 1; i >= 0; --i)
  {
    double aTemp = theCoeffs[i];
    theCoeffs[i] = aCarry;
    aCarry       = aTemp + aCarry * theRoot;
  }
  --theDegree;
}

//! Deflate polynomial by removing a complex conjugate pair.
//! Divides p(x) by (x^2 + b*x + c) where b = -2*Re(root), c = |root|^2.
//! @param theCoeffs input coefficients, output deflated coefficients
//! @param theDegree polynomial degree (will be decremented by 2)
//! @param theRoot complex root (its conjugate is also removed)
inline void DeflateComplex(double* theCoeffs, int& theDegree, std::complex<double> theRoot)
{
  // Quadratic factor: x^2 + b*x + c where b = -2*re, c = re^2 + im^2
  const double aRe = theRoot.real();
  const double aIm = theRoot.imag();
  const double aB  = -2.0 * aRe;
  const double aC  = aRe * aRe + aIm * aIm;

  // Synthetic division by (x^2 + b*x + c)
  // If p(x) = a_n*x^n + ... + a_0
  // And p(x) = (x^2 + b*x + c) * q(x) + remainder
  // Where q(x) = q_{n-2}*x^{n-2} + ... + q_0

  std::array<double, THE_MAX_POLY_DEGREE + 1> aQuotient;
  aQuotient.fill(0.0);

  // Work from highest degree down
  aQuotient[theDegree - 2] = theCoeffs[theDegree];
  if (theDegree >= 3)
  {
    aQuotient[theDegree - 3] = theCoeffs[theDegree - 1] - aB * aQuotient[theDegree - 2];
  }

  for (int i = theDegree - 4; i >= 0; --i)
  {
    aQuotient[i] = theCoeffs[i + 2] - aB * aQuotient[i + 1] - aC * aQuotient[i + 2];
  }

  // Copy quotient back to coefficients
  for (int i = 0; i <= theDegree - 2; ++i)
  {
    theCoeffs[i] = aQuotient[i];
  }
  theDegree -= 2;
}

//! Refine a real root using Newton-Raphson.
inline double RefineRealRoot(const double* theOrigCoeffs, int theOrigDegree, double theRoot)
{
  constexpr int    THE_MAX_ITER = 10;
  constexpr double THE_TOL      = 1.0e-14;

  double aX = theRoot;
  for (int anIter = 0; anIter < THE_MAX_ITER; ++anIter)
  {
    // Evaluate P and P' using Horner
    double aP  = theOrigCoeffs[theOrigDegree];
    double aDP = 0.0;
    for (int i = theOrigDegree - 1; i >= 0; --i)
    {
      aDP = aDP * aX + aP;
      aP  = aP * aX + theOrigCoeffs[i];
    }

    if (std::abs(aDP) < MathUtils::THE_ZERO_TOL)
    {
      break;
    }

    const double aDelta = aP / aDP;
    aX -= aDelta;

    if (std::abs(aDelta) < THE_TOL * (1.0 + std::abs(aX)))
    {
      break;
    }
  }
  return aX;
}

} // namespace detail

//! Solve polynomial equation using Laguerre's method with deflation.
//! Works for any degree up to THE_MAX_POLY_DEGREE.
//!
//! Algorithm:
//! 1. Use Laguerre iteration to find one root (complex)
//! 2. If root is nearly real, treat it as real and deflate
//! 3. If root is complex, deflate by quadratic factor (conjugate pair)
//! 4. Repeat until all roots found
//! 5. Refine real roots using Newton on original polynomial
//!
//! @param theCoeffs coefficients array [a0, a1, a2, ..., an] for a0 + a1*x + ... + an*x^n
//! @param theDegree polynomial degree
//! @param theTol tolerance for convergence and real/complex discrimination
//! @return GeneralPolyResult containing real and complex roots
inline GeneralPolyResult Laguerre(const double* theCoeffs, int theDegree, double theTol = 1.0e-12)
{
  GeneralPolyResult aResult;

  // Validate input
  if (theDegree < 1 || theDegree > THE_MAX_POLY_DEGREE)
  {
    aResult.Status = MathUtils::Status::InvalidInput;
    return aResult;
  }

  // Check leading coefficient
  if (std::abs(theCoeffs[theDegree]) < MathUtils::THE_ZERO_TOL)
  {
    aResult.Status = MathUtils::Status::InvalidInput;
    return aResult;
  }

  // Copy coefficients for deflation
  std::array<double, THE_MAX_POLY_DEGREE + 1> aWorkCoeffs;
  for (int i = 0; i <= theDegree; ++i)
  {
    aWorkCoeffs[i] = theCoeffs[i];
  }

  // Store original for refinement
  std::array<double, THE_MAX_POLY_DEGREE + 1> aOrigCoeffs;
  for (int i = 0; i <= theDegree; ++i)
  {
    aOrigCoeffs[i] = theCoeffs[i];
  }

  int aDeg = theDegree;

  // Find all roots
  int aStartIdx = 0;
  while (aDeg > 0)
  {
    // Use different starting points to improve convergence
    // Rotate through starting positions
    std::array<std::complex<double>, 4> aStartPoints = {std::complex<double>(0.0, 0.1),
                                                        std::complex<double>(1.0, 0.5),
                                                        std::complex<double>(-0.5, 0.3),
                                                        std::complex<double>(0.5, -0.3)};

    std::complex<double> aX0 = aStartPoints[aStartIdx % 4];
    ++aStartIdx;

    // Use Laguerre iteration
    std::complex<double> aRoot =
      detail::LaguerreIteration(aWorkCoeffs.data(), aDeg, aX0, theTol, 100);

    // Determine if root is real or complex
    const double aImagPart = std::abs(aRoot.imag());
    const double aRealPart = std::abs(aRoot.real());
    const double aScale    = std::max(1.0, aRealPart);

    if (aImagPart < theTol * aScale)
    {
      // Real root
      double aRealRoot = aRoot.real();

      // Refine using Newton on original polynomial
      aRealRoot = detail::RefineRealRoot(aOrigCoeffs.data(), theDegree, aRealRoot);

      aResult.Roots[aResult.NbRoots++] = aRealRoot;

      // Deflate
      detail::DeflateReal(aWorkCoeffs.data(), aDeg, aRealRoot);
    }
    else
    {
      // Complex conjugate pair
      aResult.ComplexRoots[aResult.NbComplexRoots++] = aRoot;
      aResult.ComplexRoots[aResult.NbComplexRoots++] = std::conj(aRoot);

      // Deflate by quadratic
      detail::DeflateComplex(aWorkCoeffs.data(), aDeg, aRoot);
    }
  }

  // Sort real roots
  std::sort(aResult.Roots.begin(), aResult.Roots.begin() + aResult.NbRoots);

  // Remove duplicate real roots
  if (aResult.NbRoots > 1)
  {
    size_t aNewCount = 1;
    for (size_t i = 1; i < aResult.NbRoots; ++i)
    {
      if (std::abs(aResult.Roots[i] - aResult.Roots[aNewCount - 1]) > theTol)
      {
        aResult.Roots[aNewCount++] = aResult.Roots[i];
      }
    }
    aResult.NbRoots = aNewCount;
  }

  aResult.Status = MathUtils::Status::OK;
  return aResult;
}

//! Convenience function: solve polynomial given as array with specified size.
//! @param theCoeffs coefficients [a0, a1, ..., an]
//! @param theSize size of coefficient array (degree + 1)
//! @param theTol tolerance
//! @return GeneralPolyResult
inline GeneralPolyResult LaguerreN(const double* theCoeffs, size_t theSize, double theTol = 1.0e-12)
{
  if (theSize < 2)
  {
    GeneralPolyResult aResult;
    aResult.Status = MathUtils::Status::InvalidInput;
    return aResult;
  }
  return Laguerre(theCoeffs, static_cast<int>(theSize - 1), theTol);
}

//! Solve sextic (degree 6) polynomial: a*x^6 + b*x^5 + c*x^4 + d*x^3 + e*x^2 + f*x + g = 0
//! @param theA coefficient of x^6
//! @param theB coefficient of x^5
//! @param theC coefficient of x^4
//! @param theD coefficient of x^3
//! @param theE coefficient of x^2
//! @param theF coefficient of x
//! @param theG constant term
//! @return PolyResult containing real roots only
inline MathUtils::PolyResult Sextic(double theA,
                                    double theB,
                                    double theC,
                                    double theD,
                                    double theE,
                                    double theF,
                                    double theG)
{
  MathUtils::PolyResult aResult;

  // Handle leading zero coefficient
  if (MathUtils::IsZero(theA))
  {
    // Reduce to quintic, then use Laguerre
    double aCoeffs[6] = {theG, theF, theE, theD, theC, theB};
    auto   aGenResult = Laguerre(aCoeffs, 5);
    if (!aGenResult.IsDone())
    {
      aResult.Status = aGenResult.Status;
      return aResult;
    }
    aResult.Status  = MathUtils::Status::OK;
    aResult.NbRoots = std::min(aGenResult.NbRoots, size_t(4));
    for (size_t i = 0; i < aResult.NbRoots; ++i)
    {
      aResult.Roots[i] = aGenResult.Roots[i];
    }
    return aResult;
  }

  double aCoeffs[7] = {theG, theF, theE, theD, theC, theB, theA};
  auto   aGenResult = Laguerre(aCoeffs, 6);

  if (!aGenResult.IsDone())
  {
    aResult.Status = aGenResult.Status;
    return aResult;
  }

  aResult.Status  = MathUtils::Status::OK;
  aResult.NbRoots = std::min(aGenResult.NbRoots, size_t(4));
  for (size_t i = 0; i < aResult.NbRoots; ++i)
  {
    aResult.Roots[i] = aGenResult.Roots[i];
  }

  return aResult;
}

//! Solve quintic (degree 5) polynomial: a*x^5 + b*x^4 + c*x^3 + d*x^2 + e*x + f = 0
//! @param theA coefficient of x^5
//! @param theB coefficient of x^4
//! @param theC coefficient of x^3
//! @param theD coefficient of x^2
//! @param theE coefficient of x
//! @param theF constant term
//! @return PolyResult containing real roots only
inline MathUtils::PolyResult Quintic(double theA,
                                     double theB,
                                     double theC,
                                     double theD,
                                     double theE,
                                     double theF)
{
  MathUtils::PolyResult aResult;

  if (MathUtils::IsZero(theA))
  {
    // Reduce to quartic - use existing solver
    return Quartic(theB, theC, theD, theE, theF);
  }

  double aCoeffs[6] = {theF, theE, theD, theC, theB, theA};
  auto   aGenResult = Laguerre(aCoeffs, 5);

  if (!aGenResult.IsDone())
  {
    aResult.Status = aGenResult.Status;
    return aResult;
  }

  aResult.Status  = MathUtils::Status::OK;
  aResult.NbRoots = std::min(aGenResult.NbRoots, size_t(4));
  for (size_t i = 0; i < aResult.NbRoots; ++i)
  {
    aResult.Roots[i] = aGenResult.Roots[i];
  }

  return aResult;
}

//! Solve octic (degree 8) polynomial using Laguerre's method.
//! Useful for Circle-Ellipse extrema after Weierstrass substitution.
//! @param theCoeffs coefficients [a0, a1, ..., a8] where polynomial is a0 + a1*x + ... + a8*x^8
//! @return GeneralPolyResult containing all real roots
inline GeneralPolyResult Octic(const double theCoeffs[9])
{
  return Laguerre(theCoeffs, 8);
}

} // namespace MathPoly

#endif // _MathPoly_Laguerre_HeaderFile
