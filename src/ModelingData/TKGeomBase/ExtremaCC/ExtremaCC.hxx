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

#ifndef _ExtremaCC_HeaderFile
#define _ExtremaCC_HeaderFile

#include <gp_Pnt.hxx>
#include <MathUtils_Domain.hxx>
#include <NCollection_Vector.hxx>
#include <Precision.hxx>

#include <limits>
#include <optional>

//! @file ExtremaCC.hxx
//! @brief Common types and utilities for Curve-Curve extrema computation.
//!
//! The ExtremaCC package provides modern C++ implementation of curve-curve
//! extrema computation using std::variant for curve pair type dispatch.
//! It supports both analytical solutions (for elementary curve pairs) and
//! grid-based numerical solutions with Newton refinement.

namespace ExtremaCC
{

//==================================================================================================
//! @name Precision Constants
//! Centralized tolerance and precision values used throughout the ExtremaCC package.
//==================================================================================================

//! Default tolerance for root finding and distance comparison.
constexpr double THE_DEFAULT_TOLERANCE = Precision::Confusion();

//! Tolerance for parameter domain comparison.
constexpr double THE_PARAM_TOLERANCE = Precision::PConfusion();

//! Ratio of parameter range for neighbor point sampling.
constexpr double THE_NEIGHBOR_STEP_RATIO = 0.01;

//! Ratio of parameter range for search interval expansion.
constexpr double THE_INTERVAL_EXPAND_RATIO = 0.05;

//! Multiplier for X (parameter) tolerance in Newton refinement.
constexpr double THE_NEWTON_XTOL_FACTOR = 0.01;

//! Multiplier for F (function) tolerance in Newton refinement.
constexpr double THE_NEWTON_FTOL_FACTOR = 0.001;

//! Threshold for considering gradient near-zero.
constexpr double THE_NEAR_ZERO_GRADIENT_FACTOR = 10.0;

//! Maximum number of Newton iterations for 2D refinement.
constexpr int THE_MAX_NEWTON_ITERATIONS = 30;

//! Number of samples for iterative grid refinement fallback.
constexpr int THE_REFINEMENT_NB_SAMPLES = 20;

//! Number of refinement passes when Newton fails.
constexpr int THE_REFINEMENT_NB_PASSES = 3;

//! Minimum number of samples per curve for Bezier.
constexpr int THE_BEZIER_MIN_SAMPLES = 16;

//! Multiplier for degree to compute Bezier samples.
constexpr int THE_BEZIER_DEGREE_MULTIPLIER = 3;

//! Default number of samples for general curves.
constexpr int THE_OTHER_CURVE_NB_SAMPLES = 32;

//! Multiplier for BSpline samples per knot span.
constexpr int THE_BSPLINE_SPAN_MULTIPLIER = 2;

//! Tolerance for detecting parallel lines.
constexpr double THE_PARALLEL_TOLERANCE = 1.0e-10;

//! Tolerance for detecting coplanar geometry.
constexpr double THE_COPLANAR_TOLERANCE = 1.0e-10;

//! 1D parameter domain for a single curve.
using Domain1D = MathUtils::Domain1D;

//! 2D parameter domain for a curve pair (u1 on curve1, u2 on curve2).
struct Domain2D
{
  Domain1D Curve1; //!< Parameter domain for first curve
  Domain1D Curve2; //!< Parameter domain for second curve

  //! Default constructor (unbounded domains).
  Domain2D() = default;

  //! Constructor with explicit domains.
  Domain2D(const Domain1D& theDomain1, const Domain1D& theDomain2)
      : Curve1(theDomain1),
        Curve2(theDomain2)
  {
  }

  //! Constructor with explicit bounds.
  Domain2D(double theU1Min, double theU1Max, double theU2Min, double theU2Max)
      : Curve1{theU1Min, theU1Max},
        Curve2{theU2Min, theU2Max}
  {
  }

  //! Returns true if both domains are finite.
  bool IsFinite() const { return Curve1.IsFinite() && Curve2.IsFinite(); }
};

//! Status of extrema computation.
enum class Status
{
  OK,                //!< Computation succeeded, finite number of extrema found
  NotDone,           //!< Computation not performed
  InfiniteSolutions, //!< Infinite solutions exist (e.g., parallel lines, coincident curves)
  NoSolution,        //!< No extrema found in the given parameter range
  NumericalError     //!< Numerical issues during computation
};

//! Search mode for extrema computation.
enum class SearchMode
{
  MinMax, //!< Find all extrema (both minima and maxima) - default
  Min,    //!< Find only minimum distance
  Max     //!< Find only maximum distance
};

//! Result of a single extremum computation between two curves.
struct ExtremumResult
{
  double Parameter1      = 0.0;  //!< Parameter value on first curve
  double Parameter2      = 0.0;  //!< Parameter value on second curve
  gp_Pnt Point1;                 //!< Point on first curve at Parameter1
  gp_Pnt Point2;                 //!< Point on second curve at Parameter2
  double SquareDistance  = 0.0;  //!< Square of the distance between Point1 and Point2
  bool   IsMinimum       = true; //!< True if this is a local minimum, false if maximum
};

//! Result of extrema computation containing all found extrema.
//! Non-copyable to enforce use of const reference from Perform().
struct Result
{
  Status                             Status = Status::NotDone; //!< Computation status
  NCollection_Vector<ExtremumResult> Extrema{8};               //!< Collection of found extrema

  //! For infinite solutions (parallel/coincident curves), stores the constant squared distance.
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

  //! Returns the index of the closest extremum (0-based), or -1 if empty.
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

  //! Returns the index of the farthest extremum (0-based), or -1 if empty.
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
  double                  Tolerance   = THE_DEFAULT_TOLERANCE; //!< Tolerance for root finding
  std::optional<Domain2D> Domain;           //!< Parameter domain (nullopt = use natural bounds)
  int                     NbSamples1  = 32; //!< Number of samples for first curve
  int                     NbSamples2  = 32; //!< Number of samples for second curve
  SearchMode              Mode        = SearchMode::MinMax; //!< Search mode
  bool                    IncludeEndpoints = true; //!< Include endpoints as potential extrema
};

//! @brief Adds endpoint extrema to result for bounded curve pairs.
//!
//! This function adds the curve pair endpoints as extrema when:
//! - The parameter bounds are finite
//! - The endpoint combination is a true local extremum
//! - The endpoint doesn't duplicate an existing extremum
//!
//! For curve-curve extrema, there are 4 boundary cases to check:
//! - (u1_min, u2) for fixed u1 at minimum
//! - (u1_max, u2) for fixed u1 at maximum
//! - (u1, u2_min) for fixed u2 at minimum
//! - (u1, u2_max) for fixed u2 at maximum
//!
//! @tparam Curve1Eval Type with Value(double) method returning gp_Pnt
//! @tparam Curve2Eval Type with Value(double) method returning gp_Pnt
//! @param theResult result to add endpoints to
//! @param theDomain parameter domain
//! @param theEval1 first curve evaluator
//! @param theEval2 second curve evaluator
//! @param theTol tolerance for duplicate detection
//! @param theMode search mode
template <typename Curve1Eval, typename Curve2Eval>
inline void AddEndpointExtrema(Result&          theResult,
                               const Domain2D&  theDomain,
                               const Curve1Eval& theEval1,
                               const Curve2Eval& theEval2,
                               double           theTol,
                               SearchMode       theMode)
{
  if (!theDomain.IsFinite())
  {
    return;
  }

  const double aU1Min = theDomain.Curve1.Min;
  const double aU1Max = theDomain.Curve1.Max;
  const double aU2Min = theDomain.Curve2.Min;
  const double aU2Max = theDomain.Curve2.Max;

  // Helper to check if a parameter pair already exists in result
  auto isDuplicate = [&](double theU1, double theU2) -> bool {
    for (int i = 0; i < theResult.Extrema.Length(); ++i)
    {
      const ExtremumResult& anExt = theResult.Extrema.Value(i);
      if (std::abs(anExt.Parameter1 - theU1) < theTol &&
          std::abs(anExt.Parameter2 - theU2) < theTol)
      {
        return true;
      }
    }
    return false;
  };

  // Helper to add an endpoint extremum
  auto addEndpoint = [&](double theU1, double theU2, bool theIsMin) {
    if (isDuplicate(theU1, theU2))
    {
      return;
    }
    if (theIsMin && theMode == SearchMode::Max)
    {
      return;
    }
    if (!theIsMin && theMode == SearchMode::Min)
    {
      return;
    }

    ExtremumResult anExt;
    anExt.Parameter1     = theU1;
    anExt.Parameter2     = theU2;
    anExt.Point1         = theEval1.Value(theU1);
    anExt.Point2         = theEval2.Value(theU2);
    anExt.SquareDistance = anExt.Point1.SquareDistance(anExt.Point2);
    anExt.IsMinimum      = theIsMin;
    theResult.Extrema.Append(anExt);
  };

  // Check corners of the domain rectangle
  // Corner: (u1_min, u2_min)
  {
    gp_Pnt aP1 = theEval1.Value(aU1Min);
    gp_Pnt aP2 = theEval2.Value(aU2Min);
    double aSqDist = aP1.SquareDistance(aP2);

    // Compare with neighbors to determine if this is min/max
    double aStep1 = (aU1Max - aU1Min) * THE_NEIGHBOR_STEP_RATIO;
    double aStep2 = (aU2Max - aU2Min) * THE_NEIGHBOR_STEP_RATIO;
    gp_Pnt aNeighbor1 = theEval1.Value(aU1Min + aStep1);
    gp_Pnt aNeighbor2 = theEval2.Value(aU2Min + aStep2);
    double aNeighborDist = aNeighbor1.SquareDistance(aNeighbor2);

    if (aSqDist <= aNeighborDist)
    {
      addEndpoint(aU1Min, aU2Min, true);
    }
    else
    {
      addEndpoint(aU1Min, aU2Min, false);
    }
  }

  // Corner: (u1_min, u2_max)
  {
    gp_Pnt aP1 = theEval1.Value(aU1Min);
    gp_Pnt aP2 = theEval2.Value(aU2Max);
    double aSqDist = aP1.SquareDistance(aP2);

    double aStep1 = (aU1Max - aU1Min) * THE_NEIGHBOR_STEP_RATIO;
    double aStep2 = (aU2Max - aU2Min) * THE_NEIGHBOR_STEP_RATIO;
    gp_Pnt aNeighbor1 = theEval1.Value(aU1Min + aStep1);
    gp_Pnt aNeighbor2 = theEval2.Value(aU2Max - aStep2);
    double aNeighborDist = aNeighbor1.SquareDistance(aNeighbor2);

    if (aSqDist <= aNeighborDist)
    {
      addEndpoint(aU1Min, aU2Max, true);
    }
    else
    {
      addEndpoint(aU1Min, aU2Max, false);
    }
  }

  // Corner: (u1_max, u2_min)
  {
    gp_Pnt aP1 = theEval1.Value(aU1Max);
    gp_Pnt aP2 = theEval2.Value(aU2Min);
    double aSqDist = aP1.SquareDistance(aP2);

    double aStep1 = (aU1Max - aU1Min) * THE_NEIGHBOR_STEP_RATIO;
    double aStep2 = (aU2Max - aU2Min) * THE_NEIGHBOR_STEP_RATIO;
    gp_Pnt aNeighbor1 = theEval1.Value(aU1Max - aStep1);
    gp_Pnt aNeighbor2 = theEval2.Value(aU2Min + aStep2);
    double aNeighborDist = aNeighbor1.SquareDistance(aNeighbor2);

    if (aSqDist <= aNeighborDist)
    {
      addEndpoint(aU1Max, aU2Min, true);
    }
    else
    {
      addEndpoint(aU1Max, aU2Min, false);
    }
  }

  // Corner: (u1_max, u2_max)
  {
    gp_Pnt aP1 = theEval1.Value(aU1Max);
    gp_Pnt aP2 = theEval2.Value(aU2Max);
    double aSqDist = aP1.SquareDistance(aP2);

    double aStep1 = (aU1Max - aU1Min) * THE_NEIGHBOR_STEP_RATIO;
    double aStep2 = (aU2Max - aU2Min) * THE_NEIGHBOR_STEP_RATIO;
    gp_Pnt aNeighbor1 = theEval1.Value(aU1Max - aStep1);
    gp_Pnt aNeighbor2 = theEval2.Value(aU2Max - aStep2);
    double aNeighborDist = aNeighbor1.SquareDistance(aNeighbor2);

    if (aSqDist <= aNeighborDist)
    {
      addEndpoint(aU1Max, aU2Max, true);
    }
    else
    {
      addEndpoint(aU1Max, aU2Max, false);
    }
  }
}

//! @brief Swaps parameters in result when curves were swapped for canonical ordering.
//!
//! When handling symmetric pairs (e.g., LineCircle vs CircleLine), we normalize
//! to alphabetical order. If curves were swapped, this function swaps the
//! result parameters back so they correspond to the original curve order.
//!
//! @param theResult result to swap parameters in
inline void SwapResultParameters(Result& theResult)
{
  for (int i = 0; i < theResult.Extrema.Length(); ++i)
  {
    ExtremumResult& anExt = theResult.Extrema.ChangeValue(i);
    std::swap(anExt.Parameter1, anExt.Parameter2);
    std::swap(anExt.Point1, anExt.Point2);
  }
}

} // namespace ExtremaCC

#endif // _ExtremaCC_HeaderFile
