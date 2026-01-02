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

#ifndef _ExtremaCS_HeaderFile
#define _ExtremaCS_HeaderFile

#include <gp_Pnt.hxx>
#include <MathUtils_Domain.hxx>
#include <NCollection_Vector.hxx>
#include <Precision.hxx>

#include <algorithm>
#include <cmath>
#include <limits>
#include <optional>

//! @file ExtremaCS.hxx
//! @brief Common types and utilities for Curve-Surface extrema computation.
//!
//! The ExtremaCS package provides modern C++ implementation of curve-surface
//! extrema computation using std::variant for type dispatch and optimized
//! Newton-based refinement (MathSys_Newton3D).
//!
//! @section Algorithm
//!
//! Finding extrema between a curve C(t) and surface S(u,v) requires solving
//! the 3D optimization problem: minimize ||C(t) - S(u,v)||^2
//!
//! Gradient conditions for extrema:
//! - dF/dt = 2*(C-S).dC/dt = 0
//! - dF/du = -2*(C-S).dS/du = 0
//! - dF/dv = -2*(C-S).dS/dv = 0
//!
//! @section API Design
//!
//! Each curve-surface pair evaluator provides:
//! - `Perform()` - finds interior extrema only (fastest)
//! - `PerformWithBoundary()` - includes boundary extrema
//!
//! @section Symmetry
//!
//! Unlike curve-curve pairs, curve-surface pairs have a natural asymmetry
//! (one curve parameter, two surface parameters). Results always maintain
//! the convention: ParameterOnCurve for t, ParameterOnSurfaceU/V for (u,v).

namespace ExtremaCS
{

//==================================================================================================
//! @name Precision Constants
//! Centralized tolerance and precision values used throughout the ExtremaCS package.
//==================================================================================================

//! Default tolerance for distance comparison and root finding.
constexpr double THE_DEFAULT_TOLERANCE = Precision::Confusion();

//! Tolerance for parameter domain comparison.
constexpr double THE_PARAM_TOLERANCE = Precision::PConfusion();

//! Two Pi constant for periodic parameter calculations.
constexpr double THE_TWO_PI = 2.0 * M_PI;

//! Half Pi constant.
constexpr double THE_HALF_PI = M_PI / 2.0;

//! Tolerance for angular comparisons (parallel detection).
constexpr double THE_ANGULAR_TOLERANCE = Precision::Angular();

//! Relative tolerance for distance comparison in sorting/filtering.
constexpr double THE_RELATIVE_TOLERANCE = Precision::Confusion();

//! Default number of samples for curve parameter.
constexpr int THE_DEFAULT_CURVE_SAMPLES = 20;

//! Default number of samples for surface parameters (per direction).
constexpr int THE_DEFAULT_SURFACE_SAMPLES = 15;

//! Maximum number of Newton iterations for 3D refinement.
constexpr int THE_MAX_NEWTON_ITERATIONS = 25;

//! Tolerance multiplier for Newton X (parameter) convergence.
constexpr double THE_NEWTON_XTOL_FACTOR = 0.01;

//! Tolerance multiplier for Newton F (function) convergence.
constexpr double THE_NEWTON_FTOL_FACTOR = 0.001;

//! Number of refinement samples when Newton fails.
constexpr int THE_REFINEMENT_NB_SAMPLES = 20;

//! Number of refinement passes when Newton fails.
constexpr int THE_REFINEMENT_NB_PASSES = 3;

//! Tolerance for detecting coplanar geometry.
constexpr double THE_COPLANAR_TOLERANCE = 1.0e-10;

//! Tolerance for detecting parallel lines.
constexpr double THE_PARALLEL_TOLERANCE = 1.0e-10;

//! Multiplier for BSpline samples per knot span.
constexpr int THE_BSPLINE_SPAN_MULTIPLIER = 2;

//! Minimum samples for Bezier curves.
constexpr int THE_BEZIER_MIN_SAMPLES = 16;

//! Multiplier for Bezier degree to compute samples.
constexpr int THE_BEZIER_DEGREE_MULTIPLIER = 3;

//! Bring Domain1D and Domain2D into ExtremaCS namespace.
using MathUtils::Domain1D;
using MathUtils::Domain2D;

//==================================================================================================
//! @name Core Types
//==================================================================================================

//! 3D parameter domain for curve-surface pair.
//! Combines 1D curve domain (t) with 2D surface domain (u, v).
struct Domain3D
{
  Domain1D Curve;   //!< Parameter domain for curve (t)
  Domain2D Surface; //!< Parameter domain for surface (u, v)

  //! Default constructor.
  Domain3D() = default;

  //! Constructor from separate domains.
  Domain3D(const Domain1D& theCurveDomain, const Domain2D& theSurfaceDomain)
      : Curve(theCurveDomain),
        Surface(theSurfaceDomain)
  {
  }

  //! Constructor with explicit bounds.
  Domain3D(double theTMin,
           double theTMax,
           double theUMin,
           double theUMax,
           double theVMin,
           double theVMax)
      : Curve{theTMin, theTMax},
        Surface{theUMin, theUMax, theVMin, theVMax}
  {
  }

  //! Returns true if all domains are finite.
  bool IsFinite() const { return Curve.IsFinite() && Surface.IsFinite(); }
};

//! Status of extrema computation.
enum class Status
{
  OK,                //!< Computation succeeded, finite number of extrema found
  NotDone,           //!< Computation not performed
  InfiniteSolutions, //!< Infinite solutions exist (e.g., curve lies on surface)
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

//! Result of a single curve-surface extremum computation.
struct ExtremumResult
{
  double ParameterOnCurve    = 0.0; //!< Parameter value on curve (t)
  double ParameterOnSurfaceU = 0.0; //!< U parameter value on surface
  double ParameterOnSurfaceV = 0.0; //!< V parameter value on surface
  gp_Pnt PointOnCurve;              //!< Point on curve at ParameterOnCurve
  gp_Pnt PointOnSurface;            //!< Point on surface at (U, V)
  double SquareDistance = 0.0;      //!< Square of the distance between points
  bool   IsMinimum      = true;     //!< True if this is a local minimum, false if maximum
};

//! Result of extrema computation containing all found extrema.
//! Non-copyable to enforce use of const reference from Perform().
struct Result
{
  Status Status = Status::NotDone; //!< Computation status

  //! Collection of found extrema.
  //! Uses small increment (8) since curve-surface pairs typically have few extrema.
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

//! Configuration for extrema computation.
struct Config
{
  double                  Tolerance = THE_DEFAULT_TOLERANCE; //!< Tolerance for root finding
  std::optional<Domain3D> Domain;                            //!< Parameter domain
  int                     NbCurveSamples   = THE_DEFAULT_CURVE_SAMPLES;   //!< Samples for curve
  int                     NbUSamples       = THE_DEFAULT_SURFACE_SAMPLES; //!< U samples for surface
  int                     NbVSamples       = THE_DEFAULT_SURFACE_SAMPLES; //!< V samples for surface
  SearchMode              Mode             = SearchMode::MinMax;          //!< Search mode
  bool                    IncludeEndpoints = true; //!< Include boundary extrema
};

//==================================================================================================
//! @name Utility Functions
//==================================================================================================

//! @brief Normalize angle to [0, 2*PI) range.
//! @param theAngle angle in radians
//! @return normalized angle in [0, 2*PI)
inline double NormalizeAngle(double theAngle)
{
  double aResult = std::fmod(theAngle, THE_TWO_PI);
  if (aResult < 0.0)
  {
    aResult += THE_TWO_PI;
  }
  return aResult;
}

//! @brief Safe arc cosine with clamping to avoid NaN.
//! Clamps the input to [-1, 1] range before computing acos.
//! @param theValue value to compute acos of
//! @return arc cosine in [0, PI]
inline double SafeAcos(double theValue)
{
  return std::acos(std::clamp(theValue, -1.0, 1.0));
}

//! @brief Safe arc sine with clamping to avoid NaN.
//! Clamps the input to [-1, 1] range before computing asin.
//! @param theValue value to compute asin of
//! @return arc sine in [-PI/2, PI/2]
inline double SafeAsin(double theValue)
{
  return std::asin(std::clamp(theValue, -1.0, 1.0));
}

//! @brief Check if a point pair is duplicate in the result.
//! @param theResult existing result to check against
//! @param theT parameter on curve
//! @param theU U parameter on surface
//! @param theV V parameter on surface
//! @param thePtC 3D point on curve
//! @param thePtS 3D point on surface
//! @param theTol tolerance for comparison
//! @return true if point pair is duplicate
inline bool IsDuplicateExtremum(const Result& theResult,
                                double        theT,
                                double        theU,
                                double        theV,
                                const gp_Pnt& thePtC,
                                const gp_Pnt& thePtS,
                                double        theTol)
{
  const double aTolSq = theTol * theTol;
  for (int i = 0; i < theResult.Extrema.Length(); ++i)
  {
    const ExtremumResult& anExt = theResult.Extrema.Value(i);
    // Check parameter proximity
    if (std::abs(anExt.ParameterOnCurve - theT) < theTol
        && std::abs(anExt.ParameterOnSurfaceU - theU) < theTol
        && std::abs(anExt.ParameterOnSurfaceV - theV) < theTol)
    {
      return true;
    }
    // Check 3D point proximity
    if (anExt.PointOnCurve.SquareDistance(thePtC) < aTolSq
        && anExt.PointOnSurface.SquareDistance(thePtS) < aTolSq)
    {
      return true;
    }
  }
  return false;
}

//! @brief Add an extremum to result if not duplicate.
//! @param theResult result to add extremum to
//! @param theT parameter on curve
//! @param theU U parameter on surface
//! @param theV V parameter on surface
//! @param thePtC 3D point on curve
//! @param thePtS 3D point on surface
//! @param theSqDist squared distance between points
//! @param theIsMin true if minimum, false if maximum
//! @param theTol tolerance for duplicate detection
//! @return true if extremum was added
inline bool AddExtremum(Result&       theResult,
                        double        theT,
                        double        theU,
                        double        theV,
                        const gp_Pnt& thePtC,
                        const gp_Pnt& thePtS,
                        double        theSqDist,
                        bool          theIsMin,
                        double        theTol)
{
  if (IsDuplicateExtremum(theResult, theT, theU, theV, thePtC, thePtS, theTol))
  {
    return false;
  }

  ExtremumResult anExt;
  anExt.ParameterOnCurve    = theT;
  anExt.ParameterOnSurfaceU = theU;
  anExt.ParameterOnSurfaceV = theV;
  anExt.PointOnCurve        = thePtC;
  anExt.PointOnSurface      = thePtS;
  anExt.SquareDistance      = theSqDist;
  anExt.IsMinimum           = theIsMin;
  theResult.Extrema.Append(anExt);
  return true;
}

//! @brief Check if parameters are within domain bounds.
//! @param theT curve parameter
//! @param theU surface U parameter
//! @param theV surface V parameter
//! @param theDomain optional domain to check against
//! @param theTol tolerance for bound check
//! @return true if within bounds (or no domain specified)
inline bool IsWithinDomain(double                         theT,
                           double                         theU,
                           double                         theV,
                           const std::optional<Domain3D>& theDomain,
                           double                         theTol)
{
  if (!theDomain.has_value())
  {
    return true;
  }

  const Domain3D& aDom = *theDomain;
  return (theT >= aDom.Curve.Min - theTol && theT <= aDom.Curve.Max + theTol
          && theU >= aDom.Surface.UMin - theTol && theU <= aDom.Surface.UMax + theTol
          && theV >= aDom.Surface.VMin - theTol && theV <= aDom.Surface.VMax + theTol);
}

//! @brief Clamp parameters to domain bounds.
//! @param theT curve parameter (modified in place)
//! @param theU surface U parameter (modified in place)
//! @param theV surface V parameter (modified in place)
//! @param theDomain domain to clamp to
inline void ClampToDomain(double& theT, double& theU, double& theV, const Domain3D& theDomain)
{
  theT = std::clamp(theT, theDomain.Curve.Min, theDomain.Curve.Max);
  theU = std::clamp(theU, theDomain.Surface.UMin, theDomain.Surface.UMax);
  theV = std::clamp(theV, theDomain.Surface.VMin, theDomain.Surface.VMax);
}

} // namespace ExtremaCS

#endif // _ExtremaCS_HeaderFile
