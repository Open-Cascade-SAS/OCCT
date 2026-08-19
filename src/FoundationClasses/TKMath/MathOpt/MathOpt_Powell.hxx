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
#include "MathOpt_Utils.hxx"

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

  const size_t aN = theStartingPoint.Size();

  if (!Utils::IsValidConfig(theConfig) || !Utils::IsFinite(theStartingPoint))
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Current point
  math_Vector aX(aN);
  for (size_t anIdx = 0; anIdx < aN; ++anIdx)
  {
    aX.ChangeAt(anIdx) = theStartingPoint.At(anIdx);
  }

  // Function value at current point
  double       aFx          = 0.0;
  const Status aValueStatus = Utils::ValueStatus(theFunc, aX, aFx);
  if (aValueStatus != Status::OK)
  {
    aResult.Status = aValueStatus;
    return aResult;
  }

  // Initialize direction set to coordinate axes
  // Directions stored as rows of matrix
  math_Matrix aDirections(aN, aN, 0.0);
  for (size_t i = 0; i < aN; ++i)
  {
    aDirections.ChangeAt(i, i) = 1.0;
  }

  // Working vectors
  math_Vector aDir(aN);
  math_Vector aXOld(aN);
  math_Vector aPtt(aN);
  math_Vector aXit(aN);

  for (uint32_t anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    aResult.NbIterations = anIter + 1;

    const double aFp = aFx;
    aXOld            = aX;

    // Track largest decrease and its direction index
    double aDel  = 0.0;
    size_t aIBig = 0;

    // Minimize along each direction
    for (size_t i = 0; i < aN; ++i)
    {
      // Extract direction i
      for (size_t j = 0; j < aN; ++j)
      {
        aDir.ChangeAt(j) = aDirections.At(i, j);
      }

      const double aFpPrev = aFx;

      // Line minimization along direction
      MathUtils::LineSearchResult aLineResult =
        MathUtils::ExactLineSearch(theFunc,
                                   aX,
                                   aDir,
                                   MathUtils::THE_EXACT_LINE_SEARCH_ALPHA_MAX,
                                   theConfig.XTolerance);

      if (!aLineResult.IsValid && Utils::IsLineSearchEvaluationError(aLineResult))
      {
        aResult.Status   = aLineResult.Status;
        aResult.Solution = aX;
        aResult.Value    = aFx;
        return aResult;
      }

      if (aLineResult.IsValid && std::isfinite(aLineResult.Alpha) && std::isfinite(aLineResult.FNew)
          && aLineResult.FNew <= aFx)
      {
        // Update position
        for (size_t j = 0; j < aN; ++j)
        {
          aX.ChangeAt(j) += aLineResult.Alpha * aDir.At(j);
        }
        aFx = aLineResult.FNew;

        // Track direction with largest decrease
        const double aDecrease = aFpPrev - aFx;
        if (aDecrease > aDel)
        {
          aDel  = aDecrease;
          aIBig = i + 1;
        }
      }
    }

    // Check convergence
    if (2.0 * std::abs(aFp - aFx)
        <= theConfig.FTolerance * (std::abs(aFp) + std::abs(aFx) + MathUtils::THE_ZERO_TOL))
    {
      Status anEvaluationStatus = Status::OK;
      aResult.Status =
        Utils::IsStationary(theFunc, aX, aFx, theConfig, anEvaluationStatus)
          ? Status::OK
          : (anEvaluationStatus != Status::OK ? anEvaluationStatus : Status::NotConverged);
      aResult.Solution = aX;
      aResult.Value    = aFx;
      return aResult;
    }

    // Construct extrapolated point and new direction
    for (size_t j = 0; j < aN; ++j)
    {
      aPtt.ChangeAt(j) = 2.0 * aX.At(j) - aXOld.At(j);
      aXit.ChangeAt(j) = aX.At(j) - aXOld.At(j);
    }

    // Evaluate at extrapolated point
    double       aFptt      = 0.0;
    const Status aPttStatus = Utils::ValueStatus(theFunc, aPtt, aFptt);
    if (aPttStatus != Status::OK)
    {
      aResult.Status   = aPttStatus;
      aResult.Solution = aX;
      aResult.Value    = aFx;
      return aResult;
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
          MathUtils::ExactLineSearch(theFunc,
                                     aX,
                                     aXit,
                                     MathUtils::THE_EXACT_LINE_SEARCH_ALPHA_MAX,
                                     theConfig.XTolerance);

        if (!aLineResult.IsValid && Utils::IsLineSearchEvaluationError(aLineResult))
        {
          aResult.Status   = aLineResult.Status;
          aResult.Solution = aX;
          aResult.Value    = aFx;
          return aResult;
        }

        if (aLineResult.IsValid && std::isfinite(aLineResult.Alpha)
            && std::isfinite(aLineResult.FNew) && aLineResult.FNew <= aFx)
        {
          // Update position
          for (size_t j = 0; j < aN; ++j)
          {
            aX.ChangeAt(j) += aLineResult.Alpha * aXit.At(j);
          }
          aFx = aLineResult.FNew;

          // Replace direction with largest decrease
          if (aIBig > 0)
          {
            for (size_t j = 0; j < aN; ++j)
            {
              aDirections.ChangeAt(aIBig - 1, j) = aDirections.At(aN - 1, j);
              aDirections.ChangeAt(aN - 1, j)    = aXit.At(j);
            }
          }
        }
      }
    }

    // Check X convergence
    double aMaxDiff = 0.0;
    for (size_t j = 0; j < aN; ++j)
    {
      aMaxDiff = std::max(aMaxDiff, std::abs(aX.At(j) - aXOld.At(j)));
    }
    if (aMaxDiff < theConfig.XTolerance)
    {
      Status anEvaluationStatus = Status::OK;
      aResult.Status =
        Utils::IsStationary(theFunc, aX, aFx, theConfig, anEvaluationStatus)
          ? Status::OK
          : (anEvaluationStatus != Status::OK ? anEvaluationStatus : Status::NotConverged);
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

  const size_t aN = theStartingPoint.Size();

  // Validate dimensions
  if (!Utils::IsValidConfig(theConfig) || !Utils::IsFinite(theStartingPoint)
      || theInitialDirections.RowSize() != aN || theInitialDirections.ColSize() != aN
      || !Utils::IsFinite(theInitialDirections))
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  math_Vector aX(aN);
  for (size_t anIdx = 0; anIdx < aN; ++anIdx)
  {
    aX.ChangeAt(anIdx) = theStartingPoint.At(anIdx);
  }

  double       aFx          = 0.0;
  const Status aValueStatus = Utils::ValueStatus(theFunc, aX, aFx);
  if (aValueStatus != Status::OK)
  {
    aResult.Status = aValueStatus;
    return aResult;
  }

  // Copy initial directions
  math_Matrix aDirections(aN, aN);
  for (size_t aRow = 0; aRow < aN; ++aRow)
  {
    for (size_t aCol = 0; aCol < aN; ++aCol)
    {
      aDirections.ChangeAt(aRow, aCol) = theInitialDirections.At(aRow, aCol);
    }
  }

  math_Vector aDir(aN);
  math_Vector aXOld(aN);
  math_Vector aPtt(aN);
  math_Vector aXit(aN);

  for (uint32_t anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    aResult.NbIterations = anIter + 1;

    const double aFp = aFx;
    aXOld            = aX;

    double aDel  = 0.0;
    size_t aIBig = 0;

    for (size_t i = 0; i < aN; ++i)
    {
      for (size_t j = 0; j < aN; ++j)
      {
        aDir.ChangeAt(j) = aDirections.At(i, j);
      }

      const double aFpPrev = aFx;

      MathUtils::LineSearchResult aLineResult =
        MathUtils::ExactLineSearch(theFunc,
                                   aX,
                                   aDir,
                                   MathUtils::THE_EXACT_LINE_SEARCH_ALPHA_MAX,
                                   theConfig.XTolerance);

      if (!aLineResult.IsValid && Utils::IsLineSearchEvaluationError(aLineResult))
      {
        aResult.Status   = aLineResult.Status;
        aResult.Solution = aX;
        aResult.Value    = aFx;
        return aResult;
      }

      if (aLineResult.IsValid && std::isfinite(aLineResult.Alpha) && std::isfinite(aLineResult.FNew)
          && aLineResult.FNew <= aFx)
      {
        for (size_t j = 0; j < aN; ++j)
        {
          aX.ChangeAt(j) += aLineResult.Alpha * aDir.At(j);
        }
        aFx = aLineResult.FNew;

        const double aDecrease = aFpPrev - aFx;
        if (aDecrease > aDel)
        {
          aDel  = aDecrease;
          aIBig = i + 1;
        }
      }
    }

    // Check convergence
    if (2.0 * std::abs(aFp - aFx)
        <= theConfig.FTolerance * (std::abs(aFp) + std::abs(aFx) + MathUtils::THE_ZERO_TOL))
    {
      Status anEvaluationStatus = Status::OK;
      aResult.Status =
        Utils::IsStationary(theFunc, aX, aFx, theConfig, anEvaluationStatus)
          ? Status::OK
          : (anEvaluationStatus != Status::OK ? anEvaluationStatus : Status::NotConverged);
      aResult.Solution = aX;
      aResult.Value    = aFx;
      return aResult;
    }

    // Construct extrapolated point
    for (size_t j = 0; j < aN; ++j)
    {
      aPtt.ChangeAt(j) = 2.0 * aX.At(j) - aXOld.At(j);
      aXit.ChangeAt(j) = aX.At(j) - aXOld.At(j);
    }

    double       aFptt      = 0.0;
    const Status aPttStatus = Utils::ValueStatus(theFunc, aPtt, aFptt);
    if (aPttStatus != Status::OK)
    {
      aResult.Status   = aPttStatus;
      aResult.Solution = aX;
      aResult.Value    = aFx;
      return aResult;
    }

    if (aFptt < aFp)
    {
      const double aT = 2.0 * (aFp - 2.0 * aFx + aFptt) * MathUtils::Sqr(aFp - aFx - aDel)
                        - aDel * MathUtils::Sqr(aFp - aFptt);

      if (aT < 0.0)
      {
        MathUtils::LineSearchResult aLineResult =
          MathUtils::ExactLineSearch(theFunc,
                                     aX,
                                     aXit,
                                     MathUtils::THE_EXACT_LINE_SEARCH_ALPHA_MAX,
                                     theConfig.XTolerance);

        if (!aLineResult.IsValid && Utils::IsLineSearchEvaluationError(aLineResult))
        {
          aResult.Status   = aLineResult.Status;
          aResult.Solution = aX;
          aResult.Value    = aFx;
          return aResult;
        }

        if (aLineResult.IsValid && std::isfinite(aLineResult.Alpha)
            && std::isfinite(aLineResult.FNew) && aLineResult.FNew <= aFx)
        {
          for (size_t j = 0; j < aN; ++j)
          {
            aX.ChangeAt(j) += aLineResult.Alpha * aXit.At(j);
          }
          aFx = aLineResult.FNew;

          if (aIBig > 0)
          {
            for (size_t j = 0; j < aN; ++j)
            {
              aDirections.ChangeAt(aIBig - 1, j) = aDirections.At(aN - 1, j);
              aDirections.ChangeAt(aN - 1, j)    = aXit.At(j);
            }
          }
        }
      }
    }

    double aMaxDiff = 0.0;
    for (size_t j = 0; j < aN; ++j)
    {
      aMaxDiff = std::max(aMaxDiff, std::abs(aX.At(j) - aXOld.At(j)));
    }
    if (aMaxDiff < theConfig.XTolerance)
    {
      Status anEvaluationStatus = Status::OK;
      aResult.Status =
        Utils::IsStationary(theFunc, aX, aFx, theConfig, anEvaluationStatus)
          ? Status::OK
          : (anEvaluationStatus != Status::OK ? anEvaluationStatus : Status::NotConverged);
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
