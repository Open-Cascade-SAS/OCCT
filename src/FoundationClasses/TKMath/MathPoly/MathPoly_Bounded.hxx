// Copyright (c) 2026 OPEN CASCADE SAS
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

#ifndef _MathPoly_Bounded_HeaderFile
#define _MathPoly_Bounded_HeaderFile

#include <MathPoly_Types.hxx>
#include <MathUtils_Poly.hxx>

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>

namespace MathPoly
{
//! Distinct real roots on a finite interval. Complex roots and multiplicities
//! are deliberately not computed. Root locations are floating-point estimates.
struct BoundedResult
{
  MathUtils::Status                       Status  = MathUtils::Status::NotConverged;
  std::array<double, THE_MAX_POLY_DEGREE> Roots   = {}; //!< Distinct real roots in ascending order
  size_t                                  NbRoots = 0;  //!< Number of valid entries in Roots

  //! Returns true when the bounded root search completed successfully.
  bool IsDone() const { return Status == MathUtils::Status::OK; }

  //! Returns the computation success status.
  explicit operator bool() const { return IsDone(); }
};

//! Find distinct real roots in [theFirst,theLast], using ascending coefficients.
//! Derivative roots partition the interval into monotone pieces. Sign-changing
//! pieces are bisected to adjacent floating-point numbers; stationary roots use
//! a compensated Horner residual. This avoids deflation and complex-root iteration.
//! Degree is limited by THE_MAX_POLY_DEGREE. Constant zero returns InfiniteSolutions.
//! Close multiple roots remain subject to coefficient conditioning; this is not
//! a certified isolator. Normalization preserves small coefficients instead of
//! scaling them into underflow. Scratch storage has fixed size and uses no heap allocation.
//!
//! @param theCoefficients coefficient array in ascending order, constant term first
//! @param theDegree polynomial degree in [0, THE_MAX_POLY_DEGREE]
//! @param theFirst lower bound of the finite search interval
//! @param theLast upper bound of the finite search interval
//! @return distinct real roots in ascending order and computation status
inline BoundedResult Bounded(const double* theCoefficients,
                             const int     theDegree,
                             const double  theFirst,
                             const double  theLast)
{
  BoundedResult aResult;
  if (theCoefficients == nullptr || theDegree < 0 || theDegree > THE_MAX_POLY_DEGREE
      || theFirst > theLast || !std::isfinite(theFirst) || !std::isfinite(theLast))
  {
    aResult.Status = MathUtils::Status::InvalidInput;
    return aResult;
  }
  for (int i = 0; i <= theDegree; ++i)
  {
    if (!std::isfinite(theCoefficients[i]))
    {
      aResult.Status = MathUtils::Status::InvalidInput;
      return aResult;
    }
  }
  constexpr size_t                                     aCapacity     = THE_MAX_POLY_DEGREE + 1;
  std::array<std::array<double, aCapacity>, aCapacity> aCoefficients = {};
  std::array<std::array<double, aCapacity>, aCapacity> aRoots        = {};
  std::array<size_t, aCapacity>                        aCounts       = {};
  int                                                  aDegree       = theDegree;
  while (aDegree > 0 && theCoefficients[aDegree] == 0)
  {
    --aDegree;
  }
  if (aDegree == 0)
  {
    aResult.Status =
      theCoefficients[0] == 0.0 ? MathUtils::Status::InfiniteSolutions : MathUtils::Status::OK;
    return aResult;
  }
  for (int i = 0; i <= aDegree; ++i)
  {
    aCoefficients[aDegree][i] = theCoefficients[i];
  }
  // Normalize each derivative with an exact power of two. Limit downward
  // scaling by the smallest coefficient: erasing a small constant can turn
  // two close roots (or a strictly positive minimum) into a false double root.
  for (int d = aDegree; d >= 1; --d)
  {
    double aScale    = 0.0;
    double aSmallest = std::numeric_limits<double>::max();
    for (size_t i = 0; i <= static_cast<size_t>(d); ++i)
    {
      const double aMagnitude = std::abs(aCoefficients[d][i]);
      aScale                  = std::max(aScale, aMagnitude);
      if (aMagnitude > 0.0)
      {
        aSmallest = std::min(aSmallest, aMagnitude);
      }
    }
    int anExponent = 0;
    std::frexp(aScale, &anExponent);
    // Keep every nonzero coefficient normal, scaling upward if the input
    // contains subnormals. This avoids losing the constant in residual tests
    // around roots whose squared parameter is subnormal.
    const int aSafeExponent =
      std::ilogb(aSmallest) - (std::numeric_limits<double>::min_exponent - 1);
    anExponent = std::min(anExponent, aSafeExponent);
    for (size_t i = 0; i <= static_cast<size_t>(d); ++i)
    {
      aCoefficients[d][i] = std::scalbn(aCoefficients[d][i], -anExponent);
      if (!std::isfinite(aCoefficients[d][i]))
      {
        aResult.Status = MathUtils::Status::NumericalError;
        return aResult;
      }
      if (i > 0)
      {
        aCoefficients[d - 1][i - 1] = static_cast<double>(i) * aCoefficients[d][i];
        if (!std::isfinite(aCoefficients[d - 1][i - 1]))
        {
          // The coefficient range cannot be normalized without losing data.
          aResult.Status = MathUtils::Status::NumericalError;
          return aResult;
        }
      }
    }
  }
  std::array<double, aCapacity + 1> aPoints;
  std::array<double, aCapacity + 1> aValues;
  std::array<bool, aCapacity + 1>   isRoot;
  for (int d = 1; d <= aDegree; ++d)
  {
    // A degree-d polynomial has at most d distinct roots. Guard insertion
    // before writing scratch storage so a numerical false-positive cannot
    // overrun the fixed arrays.
    const auto aAppendRoot = [&](const double theRoot) -> bool {
      if (aCounts[d] >= static_cast<size_t>(d))
      {
        return false;
      }
      aRoots[d][aCounts[d]++] = theRoot;
      return true;
    };

    size_t aCount     = 0;
    aPoints[aCount++] = theFirst;
    for (size_t i = 0; i < aCounts[d - 1]; ++i)
    {
      if (aRoots[d - 1][i] > theFirst && aRoots[d - 1][i] < theLast)
      {
        aPoints[aCount++] = aRoots[d - 1][i];
      }
    }
    aPoints[aCount++] = theLast;
    for (size_t i = 0; i < aCount; ++i)
    {
      double aValue     = aCoefficients[d][d];
      double aMagnitude = std::abs(aValue);
      for (int j = d - 1; j >= 0; --j)
      {
        aValue     = aValue * aPoints[i] + aCoefficients[d][j];
        aMagnitude = aMagnitude * std::abs(aPoints[i]) + std::abs(aCoefficients[d][j]);
      }
      if (!std::isfinite(aValue) || !std::isfinite(aMagnitude))
      {
        aResult.Status = MathUtils::Status::NumericalError;
        return aResult;
      }
      const double aRoundoff = 2 * d * std::numeric_limits<double>::epsilon();
      if (std::abs(aValue) <= aRoundoff * aMagnitude)
      {
        aValue = MathUtils::EvalPolyCompensated(aCoefficients[d].data(), d, aPoints[i]);
        if (!std::isfinite(aValue))
        {
          aResult.Status = MathUtils::Status::NumericalError;
          return aResult;
        }
      }
      aValues[i] = aValue;
      // Only stationary points use second-order residual acceptance after
      // compensation. A first-order threshold merges distinct nearby roots
      // and accepts strictly positive minima. Endpoints must
      // be exact zeros so nearby simple roots are still bracketed and refined.
      isRoot[i] =
        aValue == 0
        || (i > 0 && i + 1 < aCount && std::abs(aValue) <= aRoundoff * aRoundoff * aMagnitude);
    }
    for (size_t i = 0; i < aCount; ++i)
    {
      if (isRoot[i] && (i == 0 || aPoints[i] != aPoints[i - 1]))
      {
        if (!aAppendRoot(aPoints[i]))
        {
          aResult.Status = MathUtils::Status::NumericalError;
          return aResult;
        }
      }
      if (i + 1 == aCount || isRoot[i] || isRoot[i + 1] || (aValues[i] < 0) == (aValues[i + 1] < 0))
      {
        continue;
      }
      double aLeft      = aPoints[i];
      double aRight     = aPoints[i + 1];
      double aLeftValue = aValues[i];
      // Each iteration shrinks a monotone bracket. Stop at machine resolution,
      // not a model-size-dependent parameter tolerance.
      for (;;)
      {
        const double aMiddle =
          (aLeft < 0 && aRight > 0 ? aLeft / 2 + aRight / 2 : aLeft + (aRight - aLeft) / 2);
        if (aMiddle == aLeft || aMiddle == aRight)
        {
          break;
        }
        double aValue     = aCoefficients[d][d];
        double aMagnitude = std::abs(aValue);
        for (int j = d - 1; j >= 0; --j)
        {
          aValue     = aValue * aMiddle + aCoefficients[d][j];
          aMagnitude = aMagnitude * std::abs(aMiddle) + std::abs(aCoefficients[d][j]);
        }
        if (!std::isfinite(aValue) || !std::isfinite(aMagnitude))
        {
          aResult.Status = MathUtils::Status::NumericalError;
          return aResult;
        }
        if (std::abs(aValue) <= 2 * d * std::numeric_limits<double>::epsilon() * aMagnitude)
        {
          aValue = MathUtils::EvalPolyCompensated(aCoefficients[d].data(), d, aMiddle);
          if (!std::isfinite(aValue))
          {
            aResult.Status = MathUtils::Status::NumericalError;
            return aResult;
          }
        }
        if (aValue == 0)
        {
          aLeft = aRight = aMiddle;
          break;
        }
        if ((aValue < 0) == (aLeftValue < 0))
        {
          aLeft      = aMiddle;
          aLeftValue = aValue;
        }
        else
        {
          aRight = aMiddle;
        }
      }
      const double aRoot =
        aLeft < 0 && aRight > 0 ? aLeft / 2 + aRight / 2 : aLeft + (aRight - aLeft) / 2;
      if (!aAppendRoot(aRoot))
      {
        aResult.Status = MathUtils::Status::NumericalError;
        return aResult;
      }
    }
  }
  for (size_t i = 0; i < aCounts[aDegree]; ++i)
  {
    const double aRoot = aRoots[aDegree][i];
    if (aResult.NbRoots == 0 || aRoot != aResult.Roots[aResult.NbRoots - 1])
    {
      aResult.Roots[aResult.NbRoots++] = aRoot;
    }
  }
  aResult.Status = MathUtils::Status::OK;
  return aResult;
}

} // namespace MathPoly

#endif // _MathPoly_Bounded_HeaderFile
