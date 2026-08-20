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

#ifndef _ExtremaPC2d_HeaderFile
#define _ExtremaPC2d_HeaderFile

#include <gp_Pnt2d.hxx>
#include <MathUtils_Domain.hxx>
#include <NCollection_LinearVector.hxx>
#include <Precision.hxx>

#include <cmath>
#include <cstddef>
#include <limits>

//! @file ExtremaPC2d.hxx
//! @brief Common types and utilities for Point-Curve extrema computation.
//!
//! The ExtremaPC2d package provides modern C++ implementation of point-curve
//! extrema computation using std::variant for curve type dispatch and
//! analytical and derivative-aware numerical algorithms.

namespace ExtremaPC2d
{

//! Default tolerance for root finding and distance comparison.
//! Used when no explicit tolerance is provided.
constexpr double THE_DEFAULT_TOLERANCE = Precision::Confusion();

//! Returns true when a geometric tolerance is usable by extrema algorithms.
inline bool IsValidTolerance(double theTolerance)
{
  return std::isfinite(theTolerance) && theTolerance > 0.0;
}

//! Returns true when both point coordinates are finite.
inline bool IsFinitePoint(const gp_Pnt2d& thePoint)
{
  return std::isfinite(thePoint.X()) && !Precision::IsInfinite(thePoint.X())
         && std::isfinite(thePoint.Y()) && !Precision::IsInfinite(thePoint.Y());
}

//! Tolerance for parameter domain comparison (cache validation).
//! Uses PConfusion which is appropriate for parametric space.
constexpr double THE_PARAM_TOLERANCE = Precision::PConfusion();

//! Maximum ratio of parameter range for one-sided endpoint classification.
constexpr double THE_NEIGHBOR_STEP_RATIO = 1.0e-6;

//! Minimum number of samples for Bezier curves.
constexpr size_t THE_BEZIER_MIN_SAMPLES = 24;

//! Multiplier for degree to compute Bezier samples: samples = max(min, multiplier * (degree + 1)).
constexpr size_t THE_BEZIER_DEGREE_MULTIPLIER = 3;

//! Default number of samples for general (other) curves.
constexpr size_t THE_OTHER_CURVE_NB_SAMPLES = 64;

//! Fallback number of samples for BSpline curves when curve is null.
constexpr size_t THE_BSPLINE_FALLBACK_SAMPLES = 32;

//! Multiplier for BSpline curve samples per knot span: samples = multiplier * (degree + 1).
//! For a degree 3 curve: 2*4 = 8 samples per span.
//! This is higher than the surface counterpart because curves are 1D and require
//! finer sampling to detect extrema reliably. Surfaces use degree+2 per direction,
//! resulting in (degree+2)^2 samples per cell, which provides adequate coverage.
constexpr size_t THE_BSPLINE_SPAN_MULTIPLIER = 2;

//! 1D parameter domain for curves (alias for MathUtils::Domain1D).
using Domain1D = MathUtils::Domain1D;

//! Returns true when a finite domain spans an integral number of periods.
inline bool IsClosedPeriodicDomain(const Domain1D& theDomain, double thePeriod, double theTolerance)
{
  if (!theDomain.IsValid() || !std::isfinite(thePeriod) || thePeriod <= 0.0
      || !std::isfinite(theTolerance) || theTolerance < 0.0)
  {
    return false;
  }

  const double aNbPeriods = std::round(theDomain.Length() / thePeriod);
  return aNbPeriods >= 1.0 && std::abs(theDomain.Length() - aNbPeriods * thePeriod) <= theTolerance;
}

//! Returns true when a parameter can be evaluated as a finite endpoint.
inline bool IsFiniteParameter(double theParameter)
{
  return std::isfinite(theParameter) && !Precision::IsInfinite(theParameter);
}

//! Status of extrema computation.
enum class Status
{
  OK,                //!< Computation succeeded, finite number of extrema found
  NotDone,           //!< Computation not performed
  InfiniteSolutions, //!< Infinite solutions exist (e.g., point at circle center)
  NoSolution,        //!< No extrema found in the given parameter range
  InvalidInput,      //!< Invalid tolerance or parameter domain
  NumericalError     //!< Numerical issues during computation
};

//! Search mode for extrema computation.
//! Controls which extrema to find, enabling performance optimizations.
enum class SearchMode
{
  MinMax, //!< Find all local extrema (both minima and maxima) - default
  Min,    //!< Find local minima only
  Max     //!< Find local maxima only
};

//! Result of a single extremum computation.
struct ExtremumResult
{
  double   Parameter = 0.0;        //!< Parameter value on curve
  gp_Pnt2d Point;                  //!< Point on curve at parameter
  double   SquareDistance = 0.0;   //!< Square of the distance from query point to curve point
  bool     IsMinimum      = true;  //!< True if this is a local minimum
  bool     IsMaximum      = false; //!< True if this is a local maximum
};

//! Result of extrema computation containing all found extrema.
//! Non-copyable to enforce use of const reference from Perform().
struct Result
{
  //! Sentinel returned by index queries when no extremum exists.
  static constexpr size_t INVALID_INDEX = static_cast<size_t>(-1);

  ExtremaPC2d::Status Status = ExtremaPC2d::Status::NotDone; //!< Computation status
  NCollection_LinearVector<ExtremumResult> Extrema{8};       //!< Collection of found extrema

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

  //! Returns true if a finite search completed, including a search with no matching extrema.
  bool IsDone() const { return Status == Status::OK || Status == Status::NoSolution; }

  //! Returns true if there are infinite solutions.
  bool IsInfinite() const { return Status == Status::InfiniteSolutions; }

  //! Returns number of extrema found (0 if infinite or failed).
  size_t NbExt() const { return Extrema.Size(); }

  //! Access extremum by 0-based index.
  const ExtremumResult& operator[](size_t theIndex) const { return Extrema.Value(theIndex); }

  //! Returns the squared distance of the closest extremum.
  //! Returns infinity if no extrema found.
  double MinSquareDistance() const
  {
    if (Extrema.IsEmpty())
    {
      return std::numeric_limits<double>::infinity();
    }
    double aMinSqDist = Extrema.Value(0).SquareDistance;
    for (size_t anIndex = 1; anIndex < Extrema.Size(); ++anIndex)
    {
      if (Extrema.Value(anIndex).SquareDistance < aMinSqDist)
      {
        aMinSqDist = Extrema.Value(anIndex).SquareDistance;
      }
    }
    return aMinSqDist;
  }

  //! Returns the index of the closest extremum (0-based).
  //! Returns INVALID_INDEX if no extrema are available.
  size_t MinIndex() const
  {
    if (Extrema.IsEmpty())
    {
      return INVALID_INDEX;
    }
    size_t aMinIdx    = 0;
    double aMinSqDist = Extrema.Value(0).SquareDistance;
    for (size_t anIndex = 1; anIndex < Extrema.Size(); ++anIndex)
    {
      if (Extrema.Value(anIndex).SquareDistance < aMinSqDist)
      {
        aMinSqDist = Extrema.Value(anIndex).SquareDistance;
        aMinIdx    = anIndex;
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
    for (size_t anIndex = 1; anIndex < Extrema.Size(); ++anIndex)
    {
      if (Extrema.Value(anIndex).SquareDistance > aMaxSqDist)
      {
        aMaxSqDist = Extrema.Value(anIndex).SquareDistance;
      }
    }
    return aMaxSqDist;
  }

  //! Returns the index of the farthest extremum (0-based).
  //! Returns INVALID_INDEX if no extrema are available.
  size_t MaxIndex() const
  {
    if (Extrema.IsEmpty())
    {
      return INVALID_INDEX;
    }
    size_t aMaxIdx    = 0;
    double aMaxSqDist = Extrema.Value(0).SquareDistance;
    for (size_t anIndex = 1; anIndex < Extrema.Size(); ++anIndex)
    {
      if (Extrema.Value(anIndex).SquareDistance > aMaxSqDist)
      {
        aMaxSqDist = Extrema.Value(anIndex).SquareDistance;
        aMaxIdx    = anIndex;
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
//! @tparam CurveEvaluator Type with Value(double) method returning gp_Pnt2d
//! @param theResult result to add endpoints to
//! @param theP query point
//! @param theDomain parameter domain
//! @param theEval curve evaluator
//! @param theMode search mode
template <typename CurveEvaluator>
inline void AddEndpointExtrema(Result&               theResult,
                               const gp_Pnt2d&       theP,
                               const Domain1D&       theDomain,
                               const CurveEvaluator& theEval,
                               SearchMode            theMode,
                               bool                  theIsClosed)
{
  if (!theDomain.IsValid())
  {
    return;
  }

  const double theUMin      = theDomain.Min;
  const double theUMax      = theDomain.Max;
  const bool   hasFiniteMin = IsFiniteParameter(theUMin);
  const bool   hasFiniteMax = IsFiniteParameter(theUMax);
  if (!hasFiniteMin && !hasFiniteMax)
  {
    return;
  }

  // Helper to check if parameter or point already exists in result
  auto isDuplicate = [&](double theU) -> bool {
    for (size_t anIndex = 0; anIndex < theResult.Extrema.Size(); ++anIndex)
    {
      // Check parameter proximity
      if (std::abs(theResult.Extrema.Value(anIndex).Parameter - theU) < THE_PARAM_TOLERANCE)
      {
        return true;
      }
    }
    return false;
  };

  if (theUMin == theUMax)
  {
    if (hasFiniteMin && !isDuplicate(theUMin))
    {
      const gp_Pnt2d aPoint = theEval.Value(theUMin);
      ExtremumResult anExt;
      anExt.Parameter      = theUMin;
      anExt.Point          = aPoint;
      anExt.SquareDistance = theP.SquareDistance(aPoint);
      anExt.IsMinimum      = theMode != SearchMode::Max;
      anExt.IsMaximum      = theMode != SearchMode::Min;
      theResult.Extrema.Append(anExt);
    }
    return;
  }

  if (theIsClosed)
  {
    return;
  }

  const bool hasFiniteSpan = hasFiniteMin && hasFiniteMax;
  auto       addEndpoint   = [&](double theParameter, bool theIsLower) {
    if (isDuplicate(theParameter))
    {
      return;
    }

    const double aParameterResolution = Precision::PConfusion();
    const double aStep =
      hasFiniteSpan
        ? std::min((theUMax - theUMin) * 0.5,
                   std::max((theUMax - theUMin) * THE_NEIGHBOR_STEP_RATIO, aParameterResolution))
        : aParameterResolution;
    const gp_Pnt2d aPoint            = theEval.Value(theParameter);
    const gp_Pnt2d aNeighbor         = theEval.Value(theParameter + (theIsLower ? aStep : -aStep));
    const double   aSquareDistance   = theP.SquareDistance(aPoint);
    const double   aNeighborDistance = theP.SquareDistance(aNeighbor);
    const bool     isMinimum         = aSquareDistance <= aNeighborDistance;
    const bool     isMaximum         = aSquareDistance >= aNeighborDistance;

    if ((theMode == SearchMode::Min || theMode == SearchMode::MinMax) && isMinimum)
    {
      theResult.Extrema.Append(ExtremumResult{theParameter, aPoint, aSquareDistance, true, false});
    }
    else if ((theMode == SearchMode::Max || theMode == SearchMode::MinMax) && isMaximum)
    {
      theResult.Extrema.Append(ExtremumResult{theParameter, aPoint, aSquareDistance, false, true});
    }
  };

  if (hasFiniteMin)
  {
    addEndpoint(theUMin, true);
  }
  if (hasFiniteMax)
  {
    addEndpoint(theUMax, false);
  }
}

} // namespace ExtremaPC2d

#endif // _ExtremaPC2d_HeaderFile
