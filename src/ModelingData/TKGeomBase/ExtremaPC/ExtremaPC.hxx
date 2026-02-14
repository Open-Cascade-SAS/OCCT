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

#ifndef _ExtremaPC_HeaderFile
#define _ExtremaPC_HeaderFile

#include <gp_Pnt.hxx>
#include <MathUtils_Domain.hxx>
#include <NCollection_Vector.hxx>
#include <Precision.hxx>

#include <limits>
#include <optional>

//! @file ExtremaPC.hxx
//! @brief Common types and utilities for Point-Curve extrema computation.
//!
//! The ExtremaPC package provides modern C++ implementation of point-curve
//! extrema computation using std::variant for curve type dispatch and
//! BVH-based hierarchical algorithms for numerical curves.

namespace ExtremaPC
{

//==================================================================================================
//! @name Precision Constants
//! Centralized tolerance and precision values used throughout the ExtremaPC package.
//! These replace magic numbers to improve code clarity and maintainability.
//==================================================================================================

//! Default tolerance for root finding and distance comparison.
//! Used when no explicit tolerance is provided.
constexpr double THE_DEFAULT_TOLERANCE = Precision::Confusion();

//! Tolerance for parameter domain comparison (cache validation).
//! Uses PConfusion which is appropriate for parametric space.
constexpr double THE_PARAM_TOLERANCE = Precision::PConfusion();

//! Ratio of parameter range for neighbor point sampling.
//! Used to evaluate if an endpoint is a local extremum.
constexpr double THE_NEIGHBOR_STEP_RATIO = 0.01;

//! Ratio of parameter range for search interval expansion.
//! Used in grid-based methods to expand candidate intervals.
constexpr double THE_INTERVAL_EXPAND_RATIO = 0.05;

//! Ratio of parameter range for refinement step.
//! Used in iterative refinement algorithms.
constexpr double THE_REFINEMENT_STEP_RATIO = 0.001;

//! Multiplier for X (parameter) tolerance in Newton refinement.
constexpr double THE_NEWTON_XTOL_FACTOR = 0.01;

//! Multiplier for F (function) tolerance in Newton refinement.
constexpr double THE_NEWTON_FTOL_FACTOR = 0.001;

//! Default search radius for hint-based search (fraction of parameter range).
constexpr double THE_HINT_SEARCH_RADIUS = 0.1;

//! Factor for narrowing parameter range during refinement iterations.
constexpr double THE_RANGE_NARROWING_FACTOR = 0.25;

//! Threshold for skipping max candidates that are clearly worse.
//! If estimated distance < best * threshold, skip candidate.
constexpr double THE_MAX_SKIP_THRESHOLD = 0.9;

//! Multiplier for near-zero F detection during grid scan.
//! F values smaller than tolerance * this factor are considered near-zero.
constexpr double THE_NEAR_ZERO_F_FACTOR = 10.0;

//! Multiplier for fallback F tolerance when Newton fails.
//! Used to accept grid points as approximate solutions.
constexpr double THE_FALLBACK_F_FACTOR = 100.0;

//! Maximum number of Newton iterations for refinement.
constexpr int THE_MAX_NEWTON_ITERATIONS = 20;

//! Number of samples for iterative grid refinement fallback.
constexpr int THE_REFINEMENT_NB_SAMPLES = 20;

//! Number of refinement passes when Newton fails.
constexpr int THE_REFINEMENT_NB_PASSES = 3;

//! Minimum number of samples for Bezier curves.
constexpr int THE_BEZIER_MIN_SAMPLES = 24;

//! Multiplier for degree to compute Bezier samples: samples = max(min, multiplier * (degree + 1)).
constexpr int THE_BEZIER_DEGREE_MULTIPLIER = 3;

//! Default number of samples for general (other) curves.
constexpr int THE_OTHER_CURVE_NB_SAMPLES = 64;

//! Fallback number of samples for BSpline curves when curve is null.
constexpr int THE_BSPLINE_FALLBACK_SAMPLES = 32;

//! Multiplier for BSpline curve samples per knot span: samples = multiplier * (degree + 1).
//! For a degree 3 curve: 2*4 = 8 samples per span.
//! This is higher than the surface counterpart because curves are 1D and require
//! finer sampling to detect extrema reliably. Surfaces use degree+2 per direction,
//! resulting in (degree+2)^2 samples per cell, which provides adequate coverage.
constexpr int THE_BSPLINE_SPAN_MULTIPLIER = 2;

//! 1D parameter domain for curves (alias for MathUtils::Domain1D).
using Domain1D = MathUtils::Domain1D;

//! Status of extrema computation.
enum class Status
{
  OK,                //!< Computation succeeded, finite number of extrema found
  NotDone,           //!< Computation not performed
  InfiniteSolutions, //!< Infinite solutions exist (e.g., point at circle center)
  NoSolution,        //!< No extrema found in the given parameter range
  NumericalError     //!< Numerical issues during computation
};

//! Search mode for extrema computation.
//! Controls which extrema to find, enabling performance optimizations.
enum class SearchMode
{
  MinMax, //!< Find all extrema (both minima and maxima) - default
  Min,    //!< Find only minimum distance (enables early termination in BVH)
  Max     //!< Find only maximum distance
};

//! Result of a single extremum computation.
struct ExtremumResult
{
  double Parameter     = 0.0;  //!< Parameter value on curve
  gp_Pnt Point;                //!< Point on curve at parameter
  double SquareDistance = 0.0; //!< Square of the distance from query point to curve point
  bool   IsMinimum     = true; //!< True if this is a local minimum, false if maximum
};

//! Result of extrema computation containing all found extrema.
//! Non-copyable to enforce use of const reference from Perform().
struct Result
{
  Status                             Status = Status::NotDone; //!< Computation status
  NCollection_Vector<ExtremumResult> Extrema{8};               //!< Collection of found extrema

  //! For infinite solutions, stores the constant squared distance.
  //! Only meaningful when Status == Status::InfiniteSolutions.
  double InfiniteSquareDistance = 0.0;

  //! Default constructor.
  Result() = default;

  //! Copy constructor is deleted.
  Result(const Result&) = delete;

  //! Copy assignment is deleted.
  Result& operator=(const Result&) = delete;

  //! Move constructor.
  Result(Result&&) = default;

  //! Move assignment.
  Result& operator=(Result&&) = default;

  //! Returns true if computation succeeded with finite number of extrema.
  bool IsDone() const { return Status == Status::OK; }

  //! Returns true if there are infinite solutions.
  bool IsInfinite() const { return Status == Status::InfiniteSolutions; }

  //! Returns number of extrema found (0 if infinite or failed).
  int NbExt() const { return Extrema.Length(); }

  //! Access extremum by 0-based index.
  const ExtremumResult& operator[](int theIndex) const { return Extrema.Value(theIndex); }

  //! Returns the squared distance of the closest extremum.
  //! Returns infinity if no extrema found.
  double MinSquareDistance() const
  {
    if (Extrema.IsEmpty())
    {
      return std::numeric_limits<double>::infinity();
    }
    double aMinSqDist = Extrema.Value(0).SquareDistance;
    for (int i = 1; i < Extrema.Length(); ++i)
    {
      if (Extrema.Value(i).SquareDistance < aMinSqDist)
      {
        aMinSqDist = Extrema.Value(i).SquareDistance;
      }
    }
    return aMinSqDist;
  }

  //! Returns the index of the closest extremum (0-based).
  //! Returns -1 if no extrema found.
  int MinIndex() const
  {
    if (Extrema.IsEmpty())
    {
      return -1;
    }
    int    aMinIdx    = 0;
    double aMinSqDist = Extrema.Value(0).SquareDistance;
    for (int i = 1; i < Extrema.Length(); ++i)
    {
      if (Extrema.Value(i).SquareDistance < aMinSqDist)
      {
        aMinSqDist = Extrema.Value(i).SquareDistance;
        aMinIdx    = i;
      }
    }
    return aMinIdx;
  }

  //! Returns the squared distance of the farthest extremum.
  //! Returns 0 if no extrema found.
  double MaxSquareDistance() const
  {
    if (Extrema.IsEmpty())
    {
      return 0.0;
    }
    double aMaxSqDist = Extrema.Value(0).SquareDistance;
    for (int i = 1; i < Extrema.Length(); ++i)
    {
      if (Extrema.Value(i).SquareDistance > aMaxSqDist)
      {
        aMaxSqDist = Extrema.Value(i).SquareDistance;
      }
    }
    return aMaxSqDist;
  }

  //! Returns the index of the farthest extremum (0-based).
  //! Returns -1 if no extrema found.
  int MaxIndex() const
  {
    if (Extrema.IsEmpty())
    {
      return -1;
    }
    int    aMaxIdx    = 0;
    double aMaxSqDist = Extrema.Value(0).SquareDistance;
    for (int i = 1; i < Extrema.Length(); ++i)
    {
      if (Extrema.Value(i).SquareDistance > aMaxSqDist)
      {
        aMaxSqDist = Extrema.Value(i).SquareDistance;
        aMaxIdx    = i;
      }
    }
    return aMaxIdx;
  }

  //! Clear the result for reuse.
  //! Preserves allocated memory in Extrema vector.
  void Clear()
  {
    Status = Status::NotDone;
    Extrema.Clear();
    InfiniteSquareDistance = 0.0;
  }
};

//! Configuration for extrema computation.
struct Config
{
  double                  Tolerance        = THE_DEFAULT_TOLERANCE; //!< Tolerance for root finding
  std::optional<Domain1D> Domain;                    //!< Parameter domain (nullopt = use natural/unbounded)
  int                     NbSamples        = 32;     //!< Number of samples for numerical methods
  SearchMode              Mode             = SearchMode::MinMax; //!< Search mode (MinMax, Min, or Max)
  bool                    IncludeEndpoints = true;   //!< Include endpoints as potential extrema
};

//! @brief Adds endpoint extrema to result for bounded curves.
//!
//! This function adds the curve endpoints as extrema when:
//! - The parameter bounds are finite
//! - The endpoint is a true local extremum (checked via neighbor distance)
//! - The endpoint doesn't duplicate an existing extremum
//!
//! An endpoint is considered a local minimum if the distance to a neighboring
//! point on the curve is greater than the distance to the endpoint.
//! An endpoint is considered a local maximum if the distance to a neighboring
//! point is less than the distance to the endpoint.
//!
//! @tparam CurveEvaluator Type with Value(double) method returning gp_Pnt
//! @param theResult result to add endpoints to
//! @param theP query point
//! @param theDomain parameter domain
//! @param theEval curve evaluator
//! @param theTol tolerance for duplicate detection
//! @param theMode search mode
template <typename CurveEvaluator>
inline void AddEndpointExtrema(Result&               theResult,
                               const gp_Pnt&         theP,
                               const Domain1D&       theDomain,
                               const CurveEvaluator& theEval,
                               double                theTol,
                               SearchMode            theMode)
{
  // Check for infinite bounds
  if (!theDomain.IsFinite())
  {
    return;
  }

  const double theUMin = theDomain.Min;
  const double theUMax = theDomain.Max;

  // Helper to check if parameter or point already exists in result
  auto isDuplicate = [&](double theU, const gp_Pnt& thePt) -> bool {
    for (int i = 0; i < theResult.Extrema.Length(); ++i)
    {
      // Check parameter proximity
      if (std::abs(theResult.Extrema.Value(i).Parameter - theU) < theTol)
      {
        return true;
      }
      // Check point proximity (handles periodic curves where different params map to same point)
      double aSqDist = theResult.Extrema.Value(i).Point.SquareDistance(thePt);
      if (aSqDist < theTol * theTol)
      {
        return true;
      }
    }
    return false;
  };

  // Evaluate endpoints
  gp_Pnt aPtMin = theEval.Value(theUMin);
  gp_Pnt aPtMax = theEval.Value(theUMax);

  double aSqDistMin = theP.SquareDistance(aPtMin);
  double aSqDistMax = theP.SquareDistance(aPtMax);

  // Sample step for neighbor point evaluation
  double aStep = (theUMax - theUMin) * THE_NEIGHBOR_STEP_RATIO;
  if (aStep < theTol)
  {
    aStep = theTol;
  }

  // Check if UMin is a local extremum by comparing to neighbor
  gp_Pnt  aNeighborMin    = theEval.Value(theUMin + aStep);
  double  aNeighborDistMin = theP.SquareDistance(aNeighborMin);
  bool    aIsMinAtUMin    = (aSqDistMin <= aNeighborDistMin);
  bool    aIsMaxAtUMin    = (aSqDistMin >= aNeighborDistMin);

  // Check if UMax is a local extremum by comparing to neighbor
  gp_Pnt  aNeighborMax    = theEval.Value(theUMax - aStep);
  double  aNeighborDistMax = theP.SquareDistance(aNeighborMax);
  bool    aIsMinAtUMax    = (aSqDistMax <= aNeighborDistMax);
  bool    aIsMaxAtUMax    = (aSqDistMax >= aNeighborDistMax);

  // Also check if endpoints themselves are duplicates (for periodic curves)
  bool aEndpointsAreSame = aPtMin.SquareDistance(aPtMax) < theTol * theTol;

  // For periodic/closed curves, there are no true endpoints to consider
  if (aEndpointsAreSame)
  {
    return;
  }

  // Add endpoints only if they are true local extrema matching the search mode
  if (theMode == SearchMode::Min || theMode == SearchMode::MinMax)
  {
    // Add UMin if it's a local minimum
    if (aIsMinAtUMin && !isDuplicate(theUMin, aPtMin))
    {
      ExtremumResult anExt;
      anExt.Parameter      = theUMin;
      anExt.Point          = aPtMin;
      anExt.SquareDistance = aSqDistMin;
      anExt.IsMinimum      = true;
      theResult.Extrema.Append(anExt);
    }
    // Add UMax if it's a local minimum (and not same point as UMin)
    if (aIsMinAtUMax && !aEndpointsAreSame && !isDuplicate(theUMax, aPtMax))
    {
      ExtremumResult anExt;
      anExt.Parameter      = theUMax;
      anExt.Point          = aPtMax;
      anExt.SquareDistance = aSqDistMax;
      anExt.IsMinimum      = true;
      theResult.Extrema.Append(anExt);
    }
  }

  if (theMode == SearchMode::Max || theMode == SearchMode::MinMax)
  {
    // Add UMin if it's a local maximum
    if (aIsMaxAtUMin && !aIsMinAtUMin && !isDuplicate(theUMin, aPtMin))
    {
      ExtremumResult anExt;
      anExt.Parameter      = theUMin;
      anExt.Point          = aPtMin;
      anExt.SquareDistance = aSqDistMin;
      anExt.IsMinimum      = false;
      theResult.Extrema.Append(anExt);
    }
    // Add UMax if it's a local maximum (and not same point as UMin)
    if (aIsMaxAtUMax && !aIsMinAtUMax && !aEndpointsAreSame && !isDuplicate(theUMax, aPtMax))
    {
      ExtremumResult anExt;
      anExt.Parameter      = theUMax;
      anExt.Point          = aPtMax;
      anExt.SquareDistance = aSqDistMax;
      anExt.IsMinimum      = false;
      theResult.Extrema.Append(anExt);
    }
  }
}

} // namespace ExtremaPC

#endif // _ExtremaPC_HeaderFile
