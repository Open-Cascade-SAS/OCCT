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

#ifndef _ExtremaPS_HeaderFile
#define _ExtremaPS_HeaderFile

#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <MathOpt_Brent.hxx>
#include <MathUtils_Domain.hxx>
#include <NCollection_Vector.hxx>
#include <Precision.hxx>

#include <cmath>
#include <limits>

//! @file ExtremaPS.hxx
//! @brief Common types and utilities for Point-Surface extrema computation.
//!
//! The ExtremaPS package provides modern C++ implementation of point-surface
//! extrema computation using std::variant for surface type dispatch and
//! grid-based algorithms for numerical surfaces.
//!
//! @section API Design
//!
//! Each surface evaluator provides two methods:
//! - `Perform()` - finds interior extrema only (fastest)
//! - `PerformWithBoundary()` - finds interior + boundary extrema
//!
//! This design is clearer than a boolean parameter and allows for better
//! optimization in the common case where boundary extrema are not needed.

namespace ExtremaPS
{

//==================================================================================================
//! @name Precision Constants
//! Centralized tolerance and precision values used throughout the ExtremaPS package.
//! These replace magic numbers to improve code clarity and maintainability.
//==================================================================================================

//! Default tolerance for distance comparison and projection.
constexpr double THE_DEFAULT_TOLERANCE = Precision::Confusion();

//! Tolerance for parameter domain comparison (cache validation).
constexpr double THE_PARAM_TOLERANCE = Precision::PConfusion();

//! Two Pi constant for periodic surface calculations.
constexpr double THE_TWO_PI = 2.0 * M_PI;

//! Half Pi constant for sphere V parameter calculations.
constexpr double THE_HALF_PI = M_PI / 2.0;

//! Tolerance for cone apex detection.
//! Uses Angular() precision as it relates to angular calculations.
constexpr double THE_CONE_APEX_TOLERANCE = Precision::Angular();

//! Relative tolerance for distance comparison in sorting/filtering.
//! Used when comparing distances that should be "equal".
constexpr double THE_RELATIVE_TOLERANCE = Precision::Confusion();

//! Ratio of cell size for Newton search expansion.
constexpr double THE_CELL_EXPAND_RATIO = 0.1;

//! Ratio of distance for scaling tolerance in cell search.
constexpr double THE_DISTANCE_SCALE_RATIO = 0.1;

//==================================================================================================
//! @name Early Termination and Candidate Processing Constants
//! Constants controlling candidate filtering, early termination, and iteration limits.
//==================================================================================================

//! Threshold for skipping max candidates that are clearly worse.
//! For Max mode: skip if estDist < bestDist * threshold (i.e., 90% of best).
constexpr double THE_MAX_SKIP_THRESHOLD = 0.9;

//! Threshold margin for Min mode early termination.
//! For Min mode: skip if estDist > bestDist * (1 + margin).
//! Value of 0.2 means skip candidates 20% worse than current best.
constexpr double THE_MIN_SKIP_MARGIN = 0.2;

//! Maximum number of candidates to process before stopping.
//! Limits Newton iterations per grid scan for performance.
constexpr int THE_MAX_CANDIDATES_TO_PROCESS = 50;

//! Threshold for refined distance comparison.
//! Grid fallback accepted if refinedDist < currentMin * threshold.
constexpr double THE_REFINED_DIST_THRESHOLD = 1.0;

//! Relaxation factor for gradient-based zero detection.
//! Multiplied with tolerance for more robust extremum detection.
//! Reduced from 1000.0 to 100.0 to achieve better precision (~1e-5 gradient threshold).
constexpr double THE_GRADIENT_TOL_FACTOR = 100.0;

//! Multiplier for Newton retry tolerance when initial Newton fails.
constexpr double THE_NEWTON_RETRY_TOL_FACTOR = 10.0;

//==================================================================================================
//! @name Cache and Coherent Scanning Constants
//! Constants for spatial coherence optimization and trajectory prediction.
//==================================================================================================

//! Squared threshold for spatial coherence optimization.
//! Query points within this squared distance use cached solution.
//! Value of 100.0 corresponds to distance of 10.0 units.
constexpr double THE_COHERENCE_THRESHOLD_SQ = 100.0;

//! Minimum ratio for trajectory prediction (step magnitude ratio).
//! Ratios below this indicate irregular step sizes.
constexpr double THE_TRAJECTORY_MIN_RATIO = 0.5;

//! Maximum ratio for trajectory prediction (step magnitude ratio).
//! Ratios above this indicate acceleration/deceleration too large.
constexpr double THE_TRAJECTORY_MAX_RATIO = 2.0;

//! Minimum cosine for trajectory prediction (direction alignment).
//! Values above this indicate roughly same direction (< ~45 degrees).
constexpr double THE_TRAJECTORY_MIN_COS = 0.7;

//! Minimum cosine for quadratic extrapolation (high coherence).
//! Values above this indicate very smooth trajectory (< ~25 degrees).
constexpr double THE_TRAJECTORY_QUADRATIC_COS = 0.9;

//! Cache size for coherent scanning (number of solutions stored).
//! Increased from 3 to 5 to enable quadratic extrapolation.
constexpr int THE_CACHE_SIZE = 5;

//! Sanity factor for cache result validation.
//! If cache-based Newton result has squared distance > (theTol^2 * factor),
//! it's rejected as likely converging to wrong local minimum.
//! Value of 1e6 allows reasonable tolerance range while catching gross errors.
constexpr double THE_CACHE_SANITY_FACTOR = 1.0e6;

//! Velocity adjustment limit for trajectory prediction.
//! Limits acceleration factor to prevent overshoot.
constexpr double THE_VELOCITY_ADJUST_LIMIT = 1.5;

//! Maximum number of Newton iterations for grid optimization.
constexpr int THE_MAX_GOLDEN_ITERATIONS = 50;

//! Minimum number of samples for Bezier surfaces (per direction).
constexpr int THE_BEZIER_MIN_SAMPLES = 16;

//! Maximum number of samples for Bezier surfaces (per direction).
constexpr int THE_BEZIER_MAX_SAMPLES = 128;

//! Multiplier for Bezier samples: samples = multiplier * (degree + 1).
constexpr int THE_BEZIER_DEGREE_MULTIPLIER = 6;

//! Default number of samples for general surfaces (per direction).
constexpr int THE_OTHER_SURFACE_NB_SAMPLES = 32;

//==================================================================================================
//! @name Periodic Parameter Normalization Helpers
//! These functions normalize periodic parameters to a specified domain range.
//==================================================================================================

//! @brief Normalize a periodic parameter to domain range [theMin, theMin + thePeriod).
//!
//! Shifts theParam by adding/subtracting thePeriod until it falls within the range.
//! This is more efficient than using fmod for parameters close to the range.
//!
//! @param[in,out] theParam parameter value to normalize (modified in place)
//! @param[in] theMin minimum of the target range
//! @param[in] thePeriod period of the parameter (default 2*PI)
inline void NormalizePeriodic(double& theParam, double theMin, double thePeriod = THE_TWO_PI)
{
  while (theParam < theMin)
    theParam += thePeriod;
  while (theParam >= theMin + thePeriod)
    theParam -= thePeriod;
}

//! @brief Normalize U parameter to domain range.
//!
//! Convenience function for normalizing U parameter of a surface.
//!
//! @param[in,out] theU U parameter to normalize
//! @param[in] theDomain 2D domain containing UMin
inline void NormalizeU(double& theU, const MathUtils::Domain2D& theDomain)
{
  NormalizePeriodic(theU, theDomain.UMin, THE_TWO_PI);
}

//! @brief Normalize V parameter to domain range.
//!
//! Convenience function for normalizing V parameter of a surface.
//!
//! @param[in,out] theV V parameter to normalize
//! @param[in] theDomain 2D domain containing VMin
inline void NormalizeV(double& theV, const MathUtils::Domain2D& theDomain)
{
  NormalizePeriodic(theV, theDomain.VMin, THE_TWO_PI);
}

//! @brief Normalize both U and V parameters to domain range.
//!
//! @param[in,out] theU U parameter to normalize
//! @param[in,out] theV V parameter to normalize
//! @param[in] theDomain 2D domain containing UMin and VMin
inline void NormalizeUV(double& theU, double& theV, const MathUtils::Domain2D& theDomain)
{
  NormalizeU(theU, theDomain);
  NormalizeV(theV, theDomain);
}

//! @brief Check if a periodic parameter is within range after normalization.
//!
//! Normalizes theParam to [theMin, theMin + thePeriod) then checks if it falls
//! within [theMin - theTol, theMax + theTol].
//!
//! @param theParam parameter value to check (not modified)
//! @param theMin minimum of the domain range
//! @param theMax maximum of the domain range
//! @param theTol tolerance for range check
//! @param thePeriod period of the parameter (default 2*PI)
//! @return true if parameter is within range after normalization
inline bool IsInPeriodicRange(double theParam,
                              double theMin,
                              double theMax,
                              double theTol,
                              double thePeriod = THE_TWO_PI)
{
  NormalizePeriodic(theParam, theMin, thePeriod);
  return theParam >= theMin - theTol && theParam <= theMax + theTol;
}

//! @brief Shift a periodic parameter to be within partial domain range.
//!
//! Shifts theParam by adding/subtracting thePeriod to bring it into [theMin, theMax].
//! After shifting, clamps to ensure the value is within bounds.
//! This is used for partial (non-full-period) domains.
//!
//! @note Only call this for parameters that are known to be valid (i.e., after
//!       IsInPeriodicRange returned true), otherwise it may loop excessively.
//!
//! @param[in,out] theParam parameter value to shift and clamp
//! @param theMin minimum of the domain range
//! @param theMax maximum of the domain range
//! @param thePeriod period of the parameter (default 2*PI)
inline void ClampToPeriodicRange(double& theParam,
                                 double  theMin,
                                 double  theMax,
                                 double  thePeriod = THE_TWO_PI)
{
  while (theParam < theMin)
    theParam += thePeriod;
  while (theParam > theMax)
    theParam -= thePeriod;
  // Final clamp for numerical safety
  if (theParam < theMin)
    theParam = theMin;
  if (theParam > theMax)
    theParam = theMax;
}

//! Offset added to degree for BSpline surface samples per knot span: samples = degree + offset.
//! For a degree 3 surface: 3+2 = 5 samples per span in each direction.
//! This results in 5x5 = 25 samples per cell, providing adequate coverage.
//! This is lower than the curve counterpart (2*(degree+1)) because surfaces are 2D
//! and the grid already provides N^2 samples per cell.
constexpr int THE_BSPLINE_SPAN_OFFSET = 2;

//! Bring Domain2D into ExtremaPS namespace for convenience.
using MathUtils::Domain2D;

//! Status of extrema computation.
enum class Status
{
  OK,                //!< Computation succeeded, finite number of extrema found
  NotDone,           //!< Computation not performed
  InfiniteSolutions, //!< Infinite solutions exist (e.g., point on plane)
  NoSolution,        //!< No extrema found in the given parameter range
  NumericalError     //!< Numerical issues during computation
};

//! Search mode for extrema computation.
//! Controls which extrema to find, enabling performance optimizations.
enum class SearchMode
{
  MinMax, //!< Find all extrema (both minima and maxima) - default
  Min,    //!< Find only minimum distance (enables early termination)
  Max     //!< Find only maximum distance
};

//! Result of a single extremum computation.
struct ExtremumResult
{
  double U = 0.0;               //!< U parameter value on surface
  double V = 0.0;               //!< V parameter value on surface
  gp_Pnt Point;                 //!< Point on surface at (U, V)
  double SquareDistance = 0.0;  //!< Square of the distance from query point to surface point
  bool   IsMinimum      = true; //!< True if this is a local minimum, false if maximum

  //! Returns UV parameters as gp_Pnt2d.
  gp_Pnt2d UV() const { return gp_Pnt2d(U, V); }
};

//! Result of extrema computation containing all found extrema.
//! Non-copyable to enforce use of const reference from Perform().
struct Result
{
  Status Status = Status::NotDone; //!< Computation status

  //! Collection of found extrema.
  //! Uses small increment (8) since analytical surfaces typically have 1-4 extrema.
  NCollection_Vector<ExtremumResult> Extrema{8};

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

//==================================================================================================
//! @name Boundary Extrema Helpers
//! These template functions add boundary extrema to a result.
//! They are used internally by PerformWithBoundary() methods.
//==================================================================================================

//! @brief Check if a point is duplicate in the result.
//! @param theResult existing result to check against
//! @param theU U parameter of new point
//! @param theV V parameter of new point
//! @param thePt 3D point on surface
//! @param theTol tolerance for comparison
//! @return true if point is duplicate
inline bool IsDuplicateExtremum(const Result& theResult,
                                double        theU,
                                double        theV,
                                const gp_Pnt& thePt,
                                double        theTol)
{
  for (int i = 0; i < theResult.Extrema.Length(); ++i)
  {
    const ExtremumResult& anExt = theResult.Extrema.Value(i);
    if (std::abs(anExt.U - theU) < theTol && std::abs(anExt.V - theV) < theTol)
    {
      return true;
    }
    if (anExt.Point.SquareDistance(thePt) < theTol * theTol)
    {
      return true;
    }
  }
  return false;
}

//! @brief Add corner extrema to result.
//!
//! Checks the four corners of the domain and adds them as extrema.
//! Corners are always potential maximum extrema for bounded surfaces.
//!
//! @tparam SurfaceEvaluator Type with Value(double, double) method returning gp_Pnt
//! @param theResult result to add corner extrema to
//! @param theP query point
//! @param theDomain 2D parameter domain
//! @param theEval surface evaluator
//! @param theTol tolerance for duplicate detection
//! @param theMode search mode (determines if corners are added)
template <typename SurfaceEvaluator>
inline void AddCornerExtrema(Result&                 theResult,
                             const gp_Pnt&           theP,
                             const Domain2D&         theDomain,
                             const SurfaceEvaluator& theEval,
                             double                  theTol,
                             SearchMode              theMode)
{
  // Corners are only relevant for Max or MinMax mode
  if (theMode == SearchMode::Min)
  {
    return;
  }

  // Check for infinite bounds
  if (!theDomain.IsFinite())
  {
    return;
  }

  // Helper to add a corner point
  auto addCorner = [&](double theU, double theV) {
    gp_Pnt aPt     = theEval.Value(theU, theV);
    double aSqDist = theP.SquareDistance(aPt);

    if (IsDuplicateExtremum(theResult, theU, theV, aPt, theTol))
    {
      return;
    }

    ExtremumResult anExt;
    anExt.U              = theU;
    anExt.V              = theV;
    anExt.Point          = aPt;
    anExt.SquareDistance = aSqDist;
    anExt.IsMinimum      = false; // Corners are maxima candidates
    theResult.Extrema.Append(anExt);
  };

  addCorner(theDomain.UMin, theDomain.VMin);
  addCorner(theDomain.UMax, theDomain.VMin);
  addCorner(theDomain.UMin, theDomain.VMax);
  addCorner(theDomain.UMax, theDomain.VMax);
}

//! @brief Add edge extrema to result.
//!
//! For each of the 4 boundary edges, finds the point closest to the query point
//! using Golden section optimization. Only adds the overall best edge point.
//!
//! @tparam SurfaceEvaluator Type with Value(double, double) method returning gp_Pnt
//! @param theResult result to add edge extrema to
//! @param theP query point
//! @param theDomain 2D parameter domain
//! @param theEval surface evaluator
//! @param theTol tolerance for optimization and duplicate detection
//! @param theMode search mode
template <typename SurfaceEvaluator>
inline void AddEdgeExtrema(Result&                 theResult,
                           const gp_Pnt&           theP,
                           const Domain2D&         theDomain,
                           const SurfaceEvaluator& theEval,
                           double                  theTol,
                           SearchMode              theMode)
{
  // Edge minima are only relevant for Min or MinMax mode
  if (theMode == SearchMode::Max)
  {
    return;
  }

  // Check for infinite bounds
  if (!theDomain.IsFinite())
  {
    return;
  }

  // Edge distance function wrapper for MathOpt::Golden
  struct EdgeDistFunc
  {
    const gp_Pnt*           P;
    const SurfaceEvaluator* Eval;
    double                  FixedParam;
    bool                    IsUFixed;
    double                  VMin, VMax; // For V when U is fixed

    bool Value(double theParam, double& theF) const
    {
      double aU  = IsUFixed ? FixedParam : theParam;
      double aV  = IsUFixed ? theParam : FixedParam;
      gp_Pnt aPt = Eval->Value(aU, aV);
      theF       = P->SquareDistance(aPt);
      return true;
    }
  };

  // Helper to find minimum along an edge
  auto findEdgeMin =
    [&](double theFixedParam, bool theIsUFixed, double theParamMin, double theParamMax) -> double {
    EdgeDistFunc aFunc{&theP, &theEval, theFixedParam, theIsUFixed, 0.0, 0.0};

    MathUtils::Config aConfig;
    aConfig.XTolerance    = theTol;
    aConfig.MaxIterations = THE_MAX_GOLDEN_ITERATIONS;

    auto aResult = MathOpt::Golden(aFunc, theParamMin, theParamMax, aConfig);
    return aResult.IsDone() ? *aResult.Root : (theParamMin + theParamMax) * 0.5;
  };

  // Find best point across all 4 edges
  double aBestU      = theDomain.UMin;
  double aBestV      = theDomain.VMin;
  double aBestSqDist = std::numeric_limits<double>::max();

  // Bottom edge (V = VMin): optimize over U
  {
    double aU      = findEdgeMin(theDomain.VMin, false, theDomain.UMin, theDomain.UMax);
    gp_Pnt aPt     = theEval.Value(aU, theDomain.VMin);
    double aSqDist = theP.SquareDistance(aPt);
    if (aSqDist < aBestSqDist)
    {
      aBestSqDist = aSqDist;
      aBestU      = aU;
      aBestV      = theDomain.VMin;
    }
  }

  // Top edge (V = VMax): optimize over U
  {
    double aU      = findEdgeMin(theDomain.VMax, false, theDomain.UMin, theDomain.UMax);
    gp_Pnt aPt     = theEval.Value(aU, theDomain.VMax);
    double aSqDist = theP.SquareDistance(aPt);
    if (aSqDist < aBestSqDist)
    {
      aBestSqDist = aSqDist;
      aBestU      = aU;
      aBestV      = theDomain.VMax;
    }
  }

  // Left edge (U = UMin): optimize over V
  {
    double aV      = findEdgeMin(theDomain.UMin, true, theDomain.VMin, theDomain.VMax);
    gp_Pnt aPt     = theEval.Value(theDomain.UMin, aV);
    double aSqDist = theP.SquareDistance(aPt);
    if (aSqDist < aBestSqDist)
    {
      aBestSqDist = aSqDist;
      aBestU      = theDomain.UMin;
      aBestV      = aV;
    }
  }

  // Right edge (U = UMax): optimize over V
  {
    double aV      = findEdgeMin(theDomain.UMax, true, theDomain.VMin, theDomain.VMax);
    gp_Pnt aPt     = theEval.Value(theDomain.UMax, aV);
    double aSqDist = theP.SquareDistance(aPt);
    if (aSqDist < aBestSqDist)
    {
      aBestSqDist = aSqDist;
      aBestU      = theDomain.UMax;
      aBestV      = aV;
    }
  }

  // Only add if it's better than existing extrema
  if (theResult.Extrema.IsEmpty() || aBestSqDist < theResult.MinSquareDistance())
  {
    gp_Pnt aPt = theEval.Value(aBestU, aBestV);
    if (!IsDuplicateExtremum(theResult, aBestU, aBestV, aPt, theTol))
    {
      ExtremumResult anExt;
      anExt.U              = aBestU;
      anExt.V              = aBestV;
      anExt.Point          = aPt;
      anExt.SquareDistance = aBestSqDist;
      anExt.IsMinimum      = true;
      theResult.Extrema.Append(anExt);
    }
  }
}

//! @brief Add all boundary extrema (edges + corners).
//!
//! Convenience function that combines AddEdgeExtrema and AddCornerExtrema.
//! Used internally by PerformWithBoundary() methods.
//!
//! @tparam SurfaceEvaluator Type with Value(double, double) method returning gp_Pnt
//! @param theResult result to add boundary extrema to
//! @param theP query point
//! @param theDomain 2D parameter domain
//! @param theEval surface evaluator
//! @param theTol tolerance
//! @param theMode search mode
template <typename SurfaceEvaluator>
inline void AddBoundaryExtrema(Result&                 theResult,
                               const gp_Pnt&           theP,
                               const Domain2D&         theDomain,
                               const SurfaceEvaluator& theEval,
                               double                  theTol,
                               SearchMode              theMode)
{
  AddEdgeExtrema(theResult, theP, theDomain, theEval, theTol, theMode);
  AddCornerExtrema(theResult, theP, theDomain, theEval, theTol, theMode);
}

} // namespace ExtremaPS

#endif // _ExtremaPS_HeaderFile
