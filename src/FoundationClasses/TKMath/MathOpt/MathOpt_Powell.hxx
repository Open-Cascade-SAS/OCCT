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

#ifndef _MathOpt_Powell_HeaderFile
#define _MathOpt_Powell_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Config.hxx>
#include <MathUtils_Core.hxx>
#include <MathUtils_LineSearch.hxx>

#include <cmath>

namespace MathOpt
{
using namespace MathUtils;

//! Powell's conjugate direction method for N-dimensional minimization.
//! A gradient-free optimization algorithm that uses conjugate directions.
//!
//! Algorithm:
//! 1. Start with N linearly independent directions (coordinate axes)
//! 2. Perform line minimization along each direction
//! 3. Replace one direction with the overall displacement direction
//! 4. Repeat until convergence
//!
//! Advantages:
//! - No gradient required
//! - Generates conjugate directions for quadratic functions
//! - Robust for non-smooth functions
//!
//! Disadvantages:
//! - Slower than gradient methods for smooth functions
//! - May lose direction independence over iterations
//!
//! @tparam Function type with Value(const math_Vector&, double&) method
//! @param theFunc function to minimize
//! @param theStartingPoint initial guess (N-dimensional)
//! @param theConfig solver configuration
//! @return result containing minimum location and value
template <typename Function>
VectorResult Powell(Function&          theFunc,
                    const math_Vector& theStartingPoint,
                    const Config&      theConfig = Config())
{
  VectorResult aResult;

  const int aLower = theStartingPoint.Lower();
  const int aUpper = theStartingPoint.Upper();
  const int aN     = aUpper - aLower + 1;

  // Current point
  math_Vector aX(aLower, aUpper);
  aX = theStartingPoint;

  // Function value at current point
  double aFx = 0.0;
  if (!theFunc.Value(aX, aFx))
  {
    aResult.Status = Status::NumericalError;
    return aResult;
  }

  // Initialize direction set to coordinate axes
  // Directions stored as rows of matrix
  math_Matrix aDirections(1, aN, 1, aN, 0.0);
  for (int i = 1; i <= aN; ++i)
  {
    aDirections(i, i) = 1.0;
  }

  // Working vectors
  math_Vector aDir(aLower, aUpper);
  math_Vector aXOld(aLower, aUpper);
  math_Vector aPt(aLower, aUpper);
  math_Vector aPtt(aLower, aUpper);
  math_Vector aXit(aLower, aUpper);

  for (int anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    aResult.NbIterations = anIter + 1;

    const double aFp = aFx;
    aXOld            = aX;

    // Track largest decrease and its direction index
    double aDel  = 0.0;
    int    aIBig = 0;

    // Minimize along each direction
    for (int i = 1; i <= aN; ++i)
    {
      // Extract direction i
      for (int j = aLower; j <= aUpper; ++j)
      {
        aDir(j) = aDirections(i, j - aLower + 1);
      }

      const double aFpPrev = aFx;

      // Line minimization along direction
      MathUtils::LineSearchResult aLineResult =
        MathUtils::ExactLineSearch(theFunc, aX, aDir, 10.0, theConfig.XTolerance);

      if (aLineResult.IsValid)
      {
        // Update position
        for (int j = aLower; j <= aUpper; ++j)
        {
          aX(j) += aLineResult.Alpha * aDir(j);
        }
        aFx = aLineResult.FNew;

        // Track direction with largest decrease
        const double aDecrease = aFpPrev - aFx;
        if (aDecrease > aDel)
        {
          aDel  = aDecrease;
          aIBig = i;
        }
      }
    }

    // Check convergence
    if (2.0 * std::abs(aFp - aFx)
        <= theConfig.FTolerance * (std::abs(aFp) + std::abs(aFx) + MathUtils::THE_ZERO_TOL))
    {
      aResult.Status   = Status::OK;
      aResult.Solution = aX;
      aResult.Value    = aFx;
      return aResult;
    }

    // Construct extrapolated point and new direction
    for (int j = aLower; j <= aUpper; ++j)
    {
      aPtt(j) = 2.0 * aX(j) - aXOld(j);
      aXit(j) = aX(j) - aXOld(j);
    }

    // Evaluate at extrapolated point
    double aFptt = 0.0;
    if (!theFunc.Value(aPtt, aFptt))
    {
      // If evaluation fails, continue with current directions
      continue;
    }

    // Check if new direction should be added
    if (aFptt < aFp)
    {
      const double aT = 2.0 * (aFp - 2.0 * aFx + aFptt) * MathUtils::Sqr(aFp - aFx - aDel)
                        - aDel * MathUtils::Sqr(aFp - aFptt);

      if (aT < 0.0)
      {
        // Minimize along new direction
        MathUtils::LineSearchResult aLineResult =
          MathUtils::ExactLineSearch(theFunc, aX, aXit, 10.0, theConfig.XTolerance);

        if (aLineResult.IsValid)
        {
          // Update position
          for (int j = aLower; j <= aUpper; ++j)
          {
            aX(j) += aLineResult.Alpha * aXit(j);
          }
          aFx = aLineResult.FNew;

          // Replace direction with largest decrease
          if (aIBig > 0)
          {
            for (int j = 1; j <= aN; ++j)
            {
              aDirections(aIBig, j) = aDirections(aN, j);
              aDirections(aN, j)    = aXit(aLower + j - 1);
            }
          }
        }
      }
    }

    // Check X convergence
    double aMaxDiff = 0.0;
    for (int j = aLower; j <= aUpper; ++j)
    {
      aMaxDiff = std::max(aMaxDiff, std::abs(aX(j) - aXOld(j)));
    }
    if (aMaxDiff < theConfig.XTolerance)
    {
      aResult.Status   = Status::OK;
      aResult.Solution = aX;
      aResult.Value    = aFx;
      return aResult;
    }
  }

  // Maximum iterations reached
  aResult.Status   = Status::MaxIterations;
  aResult.Solution = aX;
  aResult.Value    = aFx;
  return aResult;
}

//! Powell's method with custom initial directions.
//! Allows specifying the initial direction set instead of coordinate axes.
//!
//! @tparam Function type with Value(const math_Vector&, double&) method
//! @param theFunc function to minimize
//! @param theStartingPoint initial guess
//! @param theInitialDirections initial direction set (N x N matrix, directions as rows)
//! @param theConfig solver configuration
//! @return result containing minimum location and value
template <typename Function>
VectorResult PowellWithDirections(Function&          theFunc,
                                  const math_Vector& theStartingPoint,
                                  const math_Matrix& theInitialDirections,
                                  const Config&      theConfig = Config())
{
  VectorResult aResult;

  const int aLower = theStartingPoint.Lower();
  const int aUpper = theStartingPoint.Upper();
  const int aN     = aUpper - aLower + 1;

  // Validate dimensions
  if (theInitialDirections.RowNumber() != aN || theInitialDirections.ColNumber() != aN)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  math_Vector aX(aLower, aUpper);
  aX = theStartingPoint;

  double aFx = 0.0;
  if (!theFunc.Value(aX, aFx))
  {
    aResult.Status = Status::NumericalError;
    return aResult;
  }

  // Copy initial directions
  math_Matrix aDirections(1, aN, 1, aN);
  aDirections = theInitialDirections;

  math_Vector aDir(aLower, aUpper);
  math_Vector aXOld(aLower, aUpper);
  math_Vector aPtt(aLower, aUpper);
  math_Vector aXit(aLower, aUpper);

  for (int anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    aResult.NbIterations = anIter + 1;

    const double aFp = aFx;
    aXOld            = aX;

    double aDel  = 0.0;
    int    aIBig = 0;

    for (int i = 1; i <= aN; ++i)
    {
      for (int j = aLower; j <= aUpper; ++j)
      {
        aDir(j) = aDirections(i, j - aLower + 1);
      }

      const double aFpPrev = aFx;

      MathUtils::LineSearchResult aLineResult =
        MathUtils::ExactLineSearch(theFunc, aX, aDir, 10.0, theConfig.XTolerance);

      if (aLineResult.IsValid)
      {
        for (int j = aLower; j <= aUpper; ++j)
        {
          aX(j) += aLineResult.Alpha * aDir(j);
        }
        aFx = aLineResult.FNew;

        const double aDecrease = aFpPrev - aFx;
        if (aDecrease > aDel)
        {
          aDel  = aDecrease;
          aIBig = i;
        }
      }
    }

    // Check convergence
    if (2.0 * std::abs(aFp - aFx)
        <= theConfig.FTolerance * (std::abs(aFp) + std::abs(aFx) + MathUtils::THE_ZERO_TOL))
    {
      aResult.Status   = Status::OK;
      aResult.Solution = aX;
      aResult.Value    = aFx;
      return aResult;
    }

    // Construct extrapolated point
    for (int j = aLower; j <= aUpper; ++j)
    {
      aPtt(j) = 2.0 * aX(j) - aXOld(j);
      aXit(j) = aX(j) - aXOld(j);
    }

    double aFptt = 0.0;
    if (!theFunc.Value(aPtt, aFptt))
    {
      continue;
    }

    if (aFptt < aFp)
    {
      const double aT = 2.0 * (aFp - 2.0 * aFx + aFptt) * MathUtils::Sqr(aFp - aFx - aDel)
                        - aDel * MathUtils::Sqr(aFp - aFptt);

      if (aT < 0.0)
      {
        MathUtils::LineSearchResult aLineResult =
          MathUtils::ExactLineSearch(theFunc, aX, aXit, 10.0, theConfig.XTolerance);

        if (aLineResult.IsValid)
        {
          for (int j = aLower; j <= aUpper; ++j)
          {
            aX(j) += aLineResult.Alpha * aXit(j);
          }
          aFx = aLineResult.FNew;

          if (aIBig > 0)
          {
            for (int j = 1; j <= aN; ++j)
            {
              aDirections(aIBig, j) = aDirections(aN, j);
              aDirections(aN, j)    = aXit(aLower + j - 1);
            }
          }
        }
      }
    }

    double aMaxDiff = 0.0;
    for (int j = aLower; j <= aUpper; ++j)
    {
      aMaxDiff = std::max(aMaxDiff, std::abs(aX(j) - aXOld(j)));
    }
    if (aMaxDiff < theConfig.XTolerance)
    {
      aResult.Status   = Status::OK;
      aResult.Solution = aX;
      aResult.Value    = aFx;
      return aResult;
    }
  }

  aResult.Status   = Status::MaxIterations;
  aResult.Solution = aX;
  aResult.Value    = aFx;
  return aResult;
}

} // namespace MathOpt

#endif // _MathOpt_Powell_HeaderFile
