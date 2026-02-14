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

#ifndef _ExtremaSS_HeaderFile
#define _ExtremaSS_HeaderFile

#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <MathOpt_Brent.hxx>
#include <MathUtils_Domain.hxx>
#include <NCollection_Vector.hxx>
#include <Precision.hxx>

#include <cmath>
#include <limits>

//! @file ExtremaSS.hxx
//! @brief Common types and utilities for Surface-Surface extrema computation.
//!
//! The ExtremaSS package provides modern C++ implementation of surface-surface
//! extrema computation using std::variant for surface type dispatch and
//! analytical algorithms for elementary surface pairs.
//!
//! @section API Design
//!
//! Each surface pair evaluator provides two methods:
//! - `Perform()` - finds interior extrema only (fastest)
//! - `PerformWithBoundary()` - finds interior + boundary extrema
//!
//! @section Symmetry
//!
//! Each pair class handles both surface orderings internally.
//! For example, ExtremaSS_PlaneSphere accepts both (Plane, Sphere) and
//! (Sphere, Plane) orderings, normalizing internally.

namespace ExtremaSS
{

//==================================================================================================
//! @name Precision Constants
//! Centralized tolerance and precision values used throughout the ExtremaSS package.
//==================================================================================================

//! Default tolerance for distance comparison and projection.
constexpr double THE_DEFAULT_TOLERANCE = Precision::Confusion();

//! Tolerance for parameter domain comparison.
constexpr double THE_PARAM_TOLERANCE = Precision::PConfusion();

//! Two Pi constant for periodic surface calculations.
constexpr double THE_TWO_PI = 2.0 * M_PI;

//! Half Pi constant for sphere V parameter calculations.
constexpr double THE_HALF_PI = M_PI / 2.0;

//! Tolerance for angular comparisons (parallel detection).
constexpr double THE_ANGULAR_TOLERANCE = Precision::Angular();

//! Tolerance for cone apex detection.
constexpr double THE_CONE_APEX_TOLERANCE = Precision::Angular();

//! Relative tolerance for distance comparison in sorting/filtering.
constexpr double THE_RELATIVE_TOLERANCE = Precision::Confusion();

//! Ratio of cell size for Newton search expansion.
constexpr double THE_CELL_EXPAND_RATIO = 0.1;

//! Ratio of distance for scaling tolerance in cell search.
constexpr double THE_DISTANCE_SCALE_RATIO = 0.1;

//! Threshold for skipping max candidates that are clearly worse.
constexpr double THE_MAX_SKIP_THRESHOLD = 0.9;

//! Threshold for refined distance comparison (slightly less than 1.0).
constexpr double THE_REFINED_DIST_THRESHOLD = 0.99;

//! Relaxation factor for gradient-based zero detection.
constexpr double THE_GRADIENT_TOL_FACTOR = 1000.0;

//! Multiplier for Newton retry tolerance when initial Newton fails.
constexpr double THE_NEWTON_RETRY_TOL_FACTOR = 10.0;

//! Maximum number of Newton iterations for optimization.
constexpr int THE_MAX_NEWTON_ITERATIONS = 50;

//! Maximum number of Golden section iterations.
constexpr int THE_MAX_GOLDEN_ITERATIONS = 50;

//! Default number of samples for grid-based surfaces (per direction).
constexpr int THE_DEFAULT_NB_SAMPLES = 20;

//! Bring Domain2D into ExtremaSS namespace for convenience.
using MathUtils::Domain2D;

//==================================================================================================
//! @name Core Types
//==================================================================================================

//! Status of extrema computation.
enum class Status
{
  OK,                //!< Computation succeeded, finite number of extrema found
  NotDone,           //!< Computation not performed
  InfiniteSolutions, //!< Infinite solutions exist (e.g., parallel planes, concentric spheres)
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

//! Result of a single surface-surface extremum computation.
struct ExtremumResult
{
  double U1 = 0.0;              //!< U parameter value on surface 1
  double V1 = 0.0;              //!< V parameter value on surface 1
  double U2 = 0.0;              //!< U parameter value on surface 2
  double V2 = 0.0;              //!< V parameter value on surface 2
  gp_Pnt Point1;                //!< Point on surface 1 at (U1, V1)
  gp_Pnt Point2;                //!< Point on surface 2 at (U2, V2)
  double SquareDistance = 0.0;  //!< Square of the distance between points
  bool   IsMinimum      = true; //!< True if this is a local minimum, false if maximum

  //! Returns UV parameters on surface 1 as gp_Pnt2d.
  gp_Pnt2d UV1() const { return gp_Pnt2d(U1, V1); }

  //! Returns UV parameters on surface 2 as gp_Pnt2d.
  gp_Pnt2d UV2() const { return gp_Pnt2d(U2, V2); }
};

//! 4D parameter domain for surface pair (domains for both surfaces).
struct Domain4D
{
  Domain2D Domain1; //!< Parameter domain for surface 1
  Domain2D Domain2; //!< Parameter domain for surface 2

  //! Default constructor.
  Domain4D() = default;

  //! Constructor from two 2D domains.
  Domain4D(const Domain2D& theDom1, const Domain2D& theDom2)
      : Domain1(theDom1),
        Domain2(theDom2)
  {
  }

  //! Returns true if both domains are finite.
  bool IsFinite() const { return Domain1.IsFinite() && Domain2.IsFinite(); }
};

//! Result of extrema computation containing all found extrema.
//! Non-copyable to enforce use of const reference from Perform().
struct Result
{
  Status Status = Status::NotDone; //!< Computation status

  //! Collection of found extrema.
  //! Uses small increment (8) since analytical surfaces typically have few extrema.
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
//! @name Utility Functions
//==================================================================================================

//! @brief Check if a point pair is duplicate in the result.
//! @param theResult existing result to check against
//! @param theU1 U parameter on surface 1
//! @param theV1 V parameter on surface 1
//! @param theU2 U parameter on surface 2
//! @param theV2 V parameter on surface 2
//! @param thePt1 3D point on surface 1
//! @param thePt2 3D point on surface 2
//! @param theTol tolerance for comparison
//! @return true if point pair is duplicate
inline bool IsDuplicateExtremum(const Result& theResult,
                                double        theU1,
                                double        theV1,
                                double        theU2,
                                double        theV2,
                                const gp_Pnt& thePt1,
                                const gp_Pnt& thePt2,
                                double        theTol)
{
  const double aTolSq = theTol * theTol;
  for (int i = 0; i < theResult.Extrema.Length(); ++i)
  {
    const ExtremumResult& anExt = theResult.Extrema.Value(i);
    // Check parameter proximity
    if (std::abs(anExt.U1 - theU1) < theTol && std::abs(anExt.V1 - theV1) < theTol
        && std::abs(anExt.U2 - theU2) < theTol && std::abs(anExt.V2 - theV2) < theTol)
    {
      return true;
    }
    // Check 3D point proximity
    if (anExt.Point1.SquareDistance(thePt1) < aTolSq
        && anExt.Point2.SquareDistance(thePt2) < aTolSq)
    {
      return true;
    }
  }
  return false;
}

//! @brief Add an extremum to result if not duplicate.
//! @param theResult result to add extremum to
//! @param theU1 U parameter on surface 1
//! @param theV1 V parameter on surface 1
//! @param theU2 U parameter on surface 2
//! @param theV2 V parameter on surface 2
//! @param thePt1 3D point on surface 1
//! @param thePt2 3D point on surface 2
//! @param theSqDist squared distance between points
//! @param theIsMin true if minimum, false if maximum
//! @param theTol tolerance for duplicate detection
//! @return true if extremum was added
inline bool AddExtremum(Result&       theResult,
                        double        theU1,
                        double        theV1,
                        double        theU2,
                        double        theV2,
                        const gp_Pnt& thePt1,
                        const gp_Pnt& thePt2,
                        double        theSqDist,
                        bool          theIsMin,
                        double        theTol)
{
  if (IsDuplicateExtremum(theResult, theU1, theV1, theU2, theV2, thePt1, thePt2, theTol))
  {
    return false;
  }

  ExtremumResult anExt;
  anExt.U1             = theU1;
  anExt.V1             = theV1;
  anExt.U2             = theU2;
  anExt.V2             = theV2;
  anExt.Point1         = thePt1;
  anExt.Point2         = thePt2;
  anExt.SquareDistance = theSqDist;
  anExt.IsMinimum      = theIsMin;
  theResult.Extrema.Append(anExt);
  return true;
}

//! @brief Swap UV parameters in result if surfaces were swapped.
//! @param theResult result to modify
inline void SwapResultParameters(Result& theResult)
{
  for (int i = 0; i < theResult.Extrema.Length(); ++i)
  {
    ExtremumResult& anExt = theResult.Extrema.ChangeValue(i);
    std::swap(anExt.U1, anExt.U2);
    std::swap(anExt.V1, anExt.V2);
    std::swap(anExt.Point1, anExt.Point2);
  }
}

} // namespace ExtremaSS

#endif // _ExtremaSS_HeaderFile
