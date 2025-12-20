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

#ifndef _MathUtils_Poly_HeaderFile
#define _MathUtils_Poly_HeaderFile

#include <MathUtils_Core.hxx>

#include <cmath>
#include <array>

//! Modern math solver utilities.
namespace MathUtils
{

//! Evaluate polynomial using Horner's method.
//! Coefficients are ordered as [a0, a1, a2, ...] for P(x) = a0 + a1*x + a2*x^2 + ...
//! @param theCoeffs coefficient array (constant term first)
//! @param theDegree polynomial degree
//! @param theX evaluation point
//! @return P(theX)
inline double EvalPoly(const double* theCoeffs, int theDegree, double theX)
{
  double aResult = theCoeffs[theDegree];
  for (int i = theDegree - 1; i >= 0; --i)
  {
    aResult = aResult * theX + theCoeffs[i];
  }
  return aResult;
}

//! Evaluate polynomial and its derivative using Horner's method.
//! Computes both P(x) and P'(x) efficiently in a single pass.
//! @param theCoeffs coefficient array (constant term first)
//! @param theDegree polynomial degree
//! @param theX evaluation point
//! @param[out] theValue P(theX)
//! @param[out] theDeriv P'(theX)
inline void EvalPolyDeriv(const double* theCoeffs,
                          int           theDegree,
                          double        theX,
                          double&       theValue,
                          double&       theDeriv)
{
  theValue = theCoeffs[theDegree];
  theDeriv = 0.0;
  for (int i = theDegree - 1; i >= 0; --i)
  {
    theDeriv = theDeriv * theX + theValue;
    theValue = theValue * theX + theCoeffs[i];
  }
}

//! Evaluate polynomial with coefficients in descending order.
//! Coefficients are ordered as [an, a(n-1), ..., a1, a0] for P(x) = an*x^n + ... + a0.
//! @param theCoeffs coefficient array (leading term first)
//! @param theDegree polynomial degree
//! @param theX evaluation point
//! @return P(theX)
inline double EvalPolyDesc(const double* theCoeffs, int theDegree, double theX)
{
  double aResult = theCoeffs[0];
  for (int i = 1; i <= theDegree; ++i)
  {
    aResult = aResult * theX + theCoeffs[i];
  }
  return aResult;
}

//! Newton-Raphson refinement for polynomial root.
//! Polishes an approximate root to higher precision.
//! @param theCoeffs coefficient array (constant term first)
//! @param theDegree polynomial degree
//! @param theRoot initial root estimate
//! @param theMaxIter maximum refinement iterations
//! @return refined root value
inline double RefinePolyRoot(const double* theCoeffs,
                             int           theDegree,
                             double        theRoot,
                             int           theMaxIter = 5)
{
  double aX = theRoot;
  for (int i = 0; i < theMaxIter; ++i)
  {
    double aF = 0.0;
    double aDf = 0.0;
    EvalPolyDeriv(theCoeffs, theDegree, aX, aF, aDf);

    if (IsZero(aDf))
    {
      break;
    }

    const double aDx = aF / aDf;
    aX -= aDx;

    if (std::abs(aDx) < THE_EPSILON * std::max(1.0, std::abs(aX)))
    {
      break;
    }
  }
  return aX;
}

//! Newton-Raphson refinement with coefficients in descending order.
//! @param theCoeffs coefficient array (leading term first)
//! @param theDegree polynomial degree
//! @param theRoot initial root estimate
//! @param theMaxIter maximum refinement iterations
//! @return refined root value
inline double RefinePolyRootDesc(const double* theCoeffs,
                                 int           theDegree,
                                 double        theRoot,
                                 int           theMaxIter = 5)
{
  // Convert to ascending order for refinement
  std::array<double, 5> aAsc;
  for (int i = 0; i <= theDegree; ++i)
  {
    aAsc[i] = theCoeffs[theDegree - i];
  }
  return RefinePolyRoot(aAsc.data(), theDegree, theRoot, theMaxIter);
}

//! Sort roots in ascending order (simple insertion sort for small arrays).
//! @param theRoots array of roots
//! @param theCount number of roots
inline void SortRoots(double* theRoots, size_t theCount)
{
  for (size_t i = 1; i < theCount; ++i)
  {
    const double aKey = theRoots[i];
    size_t j = i;
    while (j > 0 && theRoots[j - 1] > aKey)
    {
      theRoots[j] = theRoots[j - 1];
      --j;
    }
    theRoots[j] = aKey;
  }
}

//! Remove duplicate roots within tolerance.
//! @param theRoots array of sorted roots
//! @param theCount current number of roots
//! @param theTolerance tolerance for duplicate detection
//! @return new count after removing duplicates
inline size_t RemoveDuplicateRoots(double* theRoots, size_t theCount, double theTolerance = 1.0e-10)
{
  if (theCount <= 1)
  {
    return theCount;
  }

  size_t aNewCount = 1;
  for (size_t i = 1; i < theCount; ++i)
  {
    if (std::abs(theRoots[i] - theRoots[aNewCount - 1]) > theTolerance)
    {
      theRoots[aNewCount] = theRoots[i];
      ++aNewCount;
    }
  }
  return aNewCount;
}

//! Compute depressed cubic coefficients.
//! Transforms x^3 + bx^2 + cx + d to t^3 + pt + q via x = t - b/3.
//! @param theB coefficient of x^2 (after dividing by leading coeff)
//! @param theC coefficient of x
//! @param theD constant term
//! @param[out] theP coefficient of t in depressed form
//! @param[out] theQ constant term in depressed form
//! @param[out] theShift substitution shift (b/3)
inline void DepressCubic(double  theB,
                         double  theC,
                         double  theD,
                         double& theP,
                         double& theQ,
                         double& theShift)
{
  theShift = theB / 3.0;
  const double aB2 = theB * theB;
  theP = theC - aB2 / 3.0;
  theQ = theD - theB * theC / 3.0 + 2.0 * aB2 * theB / 27.0;
}

//! Compute depressed quartic coefficients.
//! Transforms x^4 + bx^3 + cx^2 + dx + e to t^4 + pt^2 + qt + r via x = t - b/4.
//! @param theB coefficient of x^3 (after dividing by leading coeff)
//! @param theC coefficient of x^2
//! @param theD coefficient of x
//! @param theE constant term
//! @param[out] theP coefficient of t^2 in depressed form
//! @param[out] theQ coefficient of t in depressed form
//! @param[out] theR constant term in depressed form
//! @param[out] theShift substitution shift (b/4)
inline void DepressQuartic(double  theB,
                           double  theC,
                           double  theD,
                           double  theE,
                           double& theP,
                           double& theQ,
                           double& theR,
                           double& theShift)
{
  theShift = theB / 4.0;
  const double aB2 = theB * theB;
  const double aB3 = aB2 * theB;
  const double aB4 = aB2 * aB2;

  theP = theC - 3.0 * aB2 / 8.0;
  theQ = theD - theB * theC / 2.0 + aB3 / 8.0;
  theR = theE - theB * theD / 4.0 + aB2 * theC / 16.0 - 3.0 * aB4 / 256.0;
}

} // namespace MathUtils

#endif // _MathUtils_Poly_HeaderFile
