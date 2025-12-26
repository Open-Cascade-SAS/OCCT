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

#ifndef _ExtremaPC_GridEvaluator_HeaderFile
#define _ExtremaPC_GridEvaluator_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <ExtremaPC.hxx>
#include <ExtremaPC_DistanceFunction.hxx>
#include <GeomGridEval.hxx>
#include <math_Vector.hxx>
#include <MathRoot_Newton.hxx>
#include <MathUtils_Config.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Vector.hxx>

#include <algorithm>
#include <cmath>
#include <limits>
#include <optional>
#include <tuple>
#include <utility>

//! @brief Grid-based point-curve extrema computation utilities.
//!
//! Provides common algorithm for grid-based extrema finding that can be used
//! by BSpline, Bezier, Offset, and other curve evaluators.
//!
//! Algorithm:
//! 1. Build grid of (parameter, point, D1) from GeomGridEval
//! 2. Linear scan of grid to find candidate intervals (sign changes in F(u))
//! 3. Newton refinement on each candidate
//! 4. Optional endpoint handling
namespace ExtremaPC_GridEvaluator
{

//! Cached grid point with pre-computed data.
struct GridPoint
{
  double Param; //!< Parameter value
  gp_Pnt Point; //!< Curve point C(u)
  gp_Vec D1;    //!< First derivative C'(u)
};

//! Type of candidate extremum detected during grid scan.
enum class CandidateType
{
  SignChange, //!< F(u) changes sign between grid points
  NearZero    //!< F(u) is very small at grid point
};

//! Candidate interval for Newton refinement.
struct Candidate
{
  CandidateType Type;   //!< Type of candidate
  int           IdxLo;  //!< Lower grid index
  int           IdxHi;  //!< Upper grid index (same as IdxLo for NearZero)
  double        StartU; //!< Starting point for Newton
};

//! @brief Build grid from GeomGridEval D1 results.
//!
//! @tparam GridEval type with EvaluateGridD1(params) method returning NCollection_Array1<GeomGridEval::CurveD1>
//! @param theEval grid evaluator (must have EvaluateGridD1 accepting params)
//! @param theParams parameter values (math_Vector with Array1() accessor)
//! @return array of GridPoint (0-based indexing)
template <typename GridEval>
inline NCollection_Array1<GridPoint> BuildGrid(GridEval&          theEval,
                                               const math_Vector& theParams)
{
  // Use Array1() accessor to pass to GeomGridEval which expects NCollection_Array1
  NCollection_Array1<GeomGridEval::CurveD1> aD1Grid = theEval.EvaluateGridD1(theParams.Array1());

  const int                   aNbParams = theParams.Length();
  NCollection_Array1<GridPoint> aGrid(0, aNbParams - 1);

  for (int i = 0; i < aNbParams; ++i)
  {
    const int aD1Idx = aD1Grid.Lower() + i;

    aGrid[i].Param = theParams(theParams.Lower() + i);
    aGrid[i].Point = aD1Grid.Value(aD1Idx).Point;
    aGrid[i].D1    = aD1Grid.Value(aD1Idx).D1;
  }

  return aGrid;
}

//! @brief Build uniform parameter grid.
//!
//! Creates uniformly spaced parameters in [theUMin, theUMax].
//! @param theUMin lower parameter bound
//! @param theUMax upper parameter bound
//! @param theNbSamples number of samples
//! @return math_Vector with 1-based indexing
inline math_Vector BuildUniformParams(double theUMin, double theUMax, int theNbSamples)
{
  math_Vector  aParams(1, theNbSamples);
  const double aStep = (theUMax - theUMin) / (theNbSamples - 1);

  for (int i = 1; i <= theNbSamples; ++i)
  {
    aParams(i) = theUMin + (i - 1) * aStep;
  }
  // Ensure exact endpoint
  aParams(theNbSamples) = theUMax;

  return aParams;
}

//! @brief Scan grid to find candidate intervals for extrema.
//!
//! Detects sign changes in F(u) = (C(u) - P) . C'(u) between grid points
//! and near-zero F values at grid points.
//!
//! @param theGrid cached grid points
//! @param theP query point
//! @param theTol tolerance for near-zero detection
//! @param theMode search mode
//! @return vector of candidate intervals
inline NCollection_Vector<Candidate> ScanGrid(const NCollection_Array1<GridPoint>& theGrid,
                                               const gp_Pnt&                         theP,
                                               double                                theTol,
                                               ExtremaPC::SearchMode                 theMode)
{
  NCollection_Vector<Candidate> aCandidates(8); // Small bucket for typical extrema count
  const int aNbGrid = theGrid.Size();

  if (aNbGrid < 2)
  {
    return aCandidates;
  }

  // Track processed indices to avoid duplicate Newton calls
  NCollection_Array1<bool> aProcessed(0, aNbGrid - 1);
  aProcessed.Init(false);

  double aPrevF    = 0.0;
  double aPrevDist = 0.0;
  bool   aPrevValid = false;

  for (int i = 0; i < aNbGrid; ++i)
  {
    const GridPoint& aGP = theGrid[i];

    // Compute distance function value: F(u) = (C(u) - P) . C'(u)
    gp_Vec aVec(theP, aGP.Point);
    double aF    = aVec.Dot(aGP.D1);
    double aDist = aVec.SquareMagnitude();

    // Check for sign change with previous point
    if (aPrevValid && aPrevF * aF < 0.0 && !aProcessed[i - 1])
    {
      Candidate aCand;
      aCand.Type  = CandidateType::SignChange;
      aCand.IdxLo = i - 1;
      aCand.IdxHi = i;
      // Use linear interpolation for better starting point (secant method)
      double aFLo = aPrevF;
      double aFHi = aF;
      double aULo = theGrid[i - 1].Param;
      double aUHi = aGP.Param;
      aCand.StartU = aULo - aFLo * (aUHi - aULo) / (aFHi - aFLo);
      aCandidates.Append(aCand);
      aProcessed[i - 1] = true;
      aProcessed[i]     = true;
    }

    // Check for near-zero F (direct hit on extremum)
    if (std::abs(aF) < theTol * 10.0 && !aProcessed[i])
    {
      Candidate aCand;
      aCand.Type   = CandidateType::NearZero;
      aCand.IdxLo  = i;
      aCand.IdxHi  = i;
      aCand.StartU = aGP.Param;
      aCandidates.Append(aCand);
      aProcessed[i] = true;
    }

    // Check for local extremum by distance comparison (3-point test)
    if (i > 0 && i < aNbGrid - 1 && !aProcessed[i])
    {
      double aNextDist = theP.SquareDistance(theGrid[i + 1].Point);

      // Local minimum: distance decreases then increases
      bool aIsLocalMin = (aDist <= aPrevDist && aDist <= aNextDist);
      // Local maximum: distance increases then decreases
      bool aIsLocalMax = (aDist >= aPrevDist && aDist >= aNextDist);

      if ((theMode == ExtremaPC::SearchMode::Min || theMode == ExtremaPC::SearchMode::MinMax) && aIsLocalMin)
      {
        Candidate aCand;
        aCand.Type   = CandidateType::NearZero;
        aCand.IdxLo  = i;
        aCand.IdxHi  = i;
        aCand.StartU = aGP.Param;
        aCandidates.Append(aCand);
        aProcessed[i] = true;
      }
      else if ((theMode == ExtremaPC::SearchMode::Max || theMode == ExtremaPC::SearchMode::MinMax) && aIsLocalMax && !aIsLocalMin)
      {
        Candidate aCand;
        aCand.Type   = CandidateType::NearZero;
        aCand.IdxLo  = i;
        aCand.IdxHi  = i;
        aCand.StartU = aGP.Param;
        aCandidates.Append(aCand);
        aProcessed[i] = true;
      }
    }

    aPrevF     = aF;
    aPrevDist  = aDist;
    aPrevValid = true;
  }

  return aCandidates;
}

//! Configuration for iterative grid refinement fallback.
struct RefinementConfig
{
  int    MaxRefinementPasses   = 3;    //!< Maximum number of grid refinement passes
  int    RefinementGridSize    = 20;   //!< Grid size for each refinement pass
  double RangeNarrowingFactor  = ExtremaPC::THE_RANGE_NARROWING_FACTOR; //!< Factor to narrow range on each pass
  bool   EnableRefinement      = true; //!< Enable iterative refinement when Newton fails
};

//! @brief Refine a single candidate using Newton with iterative grid fallback.
//!
//! When Newton fails, narrows the parameter range and retries with finer grid.
//! Inspired by ShapeAnalysis_Curve::ProjectOnSegments pattern.
//!
//! @param theCand candidate to refine
//! @param theGrid cached grid points
//! @param theCurve curve adaptor
//! @param theP query point
//! @param theUMin parameter range lower bound
//! @param theUMax parameter range upper bound
//! @param theTol tolerance
//! @param theConfig Newton configuration
//! @param theRefConfig refinement configuration
//! @param theFoundRoots already found roots (for duplicate check)
//! @return optional (parameter, point, sqDistance, isMin) if found
inline std::optional<std::tuple<double, gp_Pnt, double, bool>>
RefineSingleCandidate(const Candidate&                     theCand,
                      const NCollection_Array1<GridPoint>& theGrid,
                      const Adaptor3d_Curve&               theCurve,
                      const gp_Pnt&                        theP,
                      double                               theUMin,
                      double                               theUMax,
                      double                               theTol,
                      const MathUtils::Config&             theConfig,
                      const RefinementConfig&              theRefConfig,
                      const NCollection_Vector<double>&    theFoundRoots)
{
  ExtremaPC_DistanceFunction aFunc(theCurve, theP);

  // Determine initial Newton bounds based on candidate type
  double aULo, aUHi;
  if (theCand.Type == CandidateType::SignChange)
  {
    aULo = theGrid[theCand.IdxLo].Param;
    aUHi = theGrid[theCand.IdxHi].Param;
  }
  else
  {
    double aExpand = (theUMax - theUMin) * ExtremaPC::THE_INTERVAL_EXPAND_RATIO;
    aULo = std::max(theUMin, theCand.StartU - aExpand);
    aUHi = std::min(theUMax, theCand.StartU + aExpand);
  }

  double aStartU = theCand.StartU;

  // Try Newton refinement
  MathUtils::ScalarResult aNewtonRes = MathRoot::NewtonBounded(aFunc, aStartU, aULo, aUHi, theConfig);

  // If Newton succeeds, return result
  if (aNewtonRes.IsDone())
  {
    double aRootU = std::max(theUMin, std::min(theUMax, *aNewtonRes.Root));

    // Check for duplicate
    for (int r = 0; r < theFoundRoots.Length(); ++r)
    {
      if (std::abs(aRootU - theFoundRoots.Value(r)) < theTol)
      {
        return std::nullopt;
      }
    }

    gp_Pnt aPt     = theCurve.Value(aRootU);
    double aSqDist = theP.SquareDistance(aPt);
    double aDF     = aNewtonRes.Derivative.value_or(0.0);
    bool   aIsMin  = (aDF > 0.0);

    return std::make_tuple(aRootU, aPt, aSqDist, aIsMin);
  }

  // Newton failed - try iterative grid refinement if enabled
  if (!theRefConfig.EnableRefinement)
  {
    return std::nullopt;
  }

  // Iterative refinement: progressively narrow range and resample
  double aRefUMin = aULo;
  double aRefUMax = aUHi;
  double aBestU   = aStartU;
  double aBestDist = std::numeric_limits<double>::max();

  for (int aPass = 0; aPass < theRefConfig.MaxRefinementPasses; ++aPass)
  {
    // Sample finer grid in narrowed range
    const int    aNbSamples = theRefConfig.RefinementGridSize;
    const double aStep      = (aRefUMax - aRefUMin) / (aNbSamples - 1);

    double aLocalBestU    = aBestU;
    double aLocalBestDist = aBestDist;

    for (int i = 0; i < aNbSamples; ++i)
    {
      double aU    = aRefUMin + i * aStep;
      gp_Pnt aPt   = theCurve.Value(aU);
      double aDist = theP.SquareDistance(aPt);

      if (aDist < aLocalBestDist)
      {
        aLocalBestDist = aDist;
        aLocalBestU    = aU;
      }
    }

    aBestU    = aLocalBestU;
    aBestDist = aLocalBestDist;

    // Narrow range around best point for next iteration
    double aRangeHalf = (aRefUMax - aRefUMin) * theRefConfig.RangeNarrowingFactor * 0.5;
    aRefUMin = std::max(theUMin, aBestU - aRangeHalf);
    aRefUMax = std::min(theUMax, aBestU + aRangeHalf);

    // Try Newton again with refined starting point
    MathUtils::ScalarResult aRetryRes = MathRoot::NewtonBounded(aFunc, aBestU, aRefUMin, aRefUMax, theConfig);
    if (aRetryRes.IsDone())
    {
      double aRootU = std::max(theUMin, std::min(theUMax, *aRetryRes.Root));

      // Check for duplicate
      for (int r = 0; r < theFoundRoots.Length(); ++r)
      {
        if (std::abs(aRootU - theFoundRoots.Value(r)) < theTol)
        {
          return std::nullopt;
        }
      }

      gp_Pnt aPt     = theCurve.Value(aRootU);
      double aSqDist = theP.SquareDistance(aPt);
      double aDF     = aRetryRes.Derivative.value_or(0.0);
      bool   aIsMin  = (aDF > 0.0);

      return std::make_tuple(aRootU, aPt, aSqDist, aIsMin);
    }
  }

  // All refinement attempts failed - use best grid point as fallback
  // Check for duplicate
  for (int r = 0; r < theFoundRoots.Length(); ++r)
  {
    if (std::abs(aBestU - theFoundRoots.Value(r)) < theTol)
    {
      return std::nullopt;
    }
  }

  // Verify this is actually an extremum by checking F(u) is near zero
  gp_Pnt aPt;
  gp_Vec aD1;
  theCurve.D1(aBestU, aPt, aD1);
  gp_Vec aVec(theP, aPt);
  double aF = aVec.Dot(aD1);

  if (std::abs(aF) < theTol * 100.0) // Relaxed tolerance for fallback
  {
    double aSqDist = theP.SquareDistance(aPt);
    // Classify as min/max using second derivative approximation
    double aStep   = (theUMax - theUMin) * ExtremaPC::THE_REFINEMENT_STEP_RATIO;
    double aDistPlus  = theP.SquareDistance(theCurve.Value(std::min(theUMax, aBestU + aStep)));
    double aDistMinus = theP.SquareDistance(theCurve.Value(std::max(theUMin, aBestU - aStep)));
    bool   aIsMin     = (aSqDist <= aDistPlus) && (aSqDist <= aDistMinus);

    return std::make_tuple(aBestU, aPt, aSqDist, aIsMin);
  }

  return std::nullopt;
}

//! @brief Refine candidates using Newton's method and build result.
//!
//! For Min mode, candidates are sorted by estimated distance and
//! early termination is applied when no better result is possible.
//! When Newton fails, iterative grid refinement is applied as fallback.
//!
//! @param theCandidates candidate intervals from ScanGrid
//! @param theGrid cached grid points
//! @param theCurve curve adaptor for Newton refinement
//! @param theP query point
//! @param theUMin parameter range lower bound
//! @param theUMax parameter range upper bound
//! @param theTol tolerance
//! @param theMode search mode
//! @param theRefConfig optional refinement configuration
//! @return extrema result
inline ExtremaPC::Result RefineCandidates(const NCollection_Vector<Candidate>&  theCandidates,
                                          const NCollection_Array1<GridPoint>&  theGrid,
                                          const Adaptor3d_Curve&                theCurve,
                                          const gp_Pnt&                         theP,
                                          double                                theUMin,
                                          double                                theUMax,
                                          double                                theTol,
                                          ExtremaPC::SearchMode                 theMode,
                                          const RefinementConfig&               theRefConfig = RefinementConfig())
{
  ExtremaPC::Result aResult;
  aResult.Status = ExtremaPC::Status::OK;

  // Track found roots to avoid duplicates
  NCollection_Vector<double> aFoundRoots(8); // Small bucket for roots

  // Newton configuration - balanced tolerance and iterations
  MathUtils::Config aConfig;
  aConfig.XTolerance    = theTol * ExtremaPC::THE_NEWTON_XTOL_FACTOR;
  aConfig.FTolerance    = theTol * ExtremaPC::THE_NEWTON_FTOL_FACTOR;
  aConfig.MaxIterations = 20;

  // For Min/Max mode, compute estimated distances and sort candidates
  NCollection_Vector<std::pair<int, double>> aSortedIndices(8); // Small bucket for candidates
  for (int c = 0; c < theCandidates.Length(); ++c)
  {
    const Candidate& aCand = theCandidates.Value(c);
    double anEstDist = theP.SquareDistance(theGrid[aCand.IdxLo].Point);
    aSortedIndices.Append(std::make_pair(c, anEstDist));
  }

  // Sort by estimated distance for Min mode (ascending), Max mode (descending)
  if (theMode == ExtremaPC::SearchMode::Min)
  {
    std::sort(aSortedIndices.begin(),
              aSortedIndices.end(),
              [](const std::pair<int, double>& a, const std::pair<int, double>& b) { return a.second < b.second; });
  }
  else if (theMode == ExtremaPC::SearchMode::Max)
  {
    std::sort(aSortedIndices.begin(),
              aSortedIndices.end(),
              [](const std::pair<int, double>& a, const std::pair<int, double>& b) { return a.second > b.second; });
  }

  // Best distance found so far (for early termination)
  double aBestSqDist = (theMode == ExtremaPC::SearchMode::Min) ? std::numeric_limits<double>::max()
                                                               : -std::numeric_limits<double>::max();

  for (int s = 0; s < aSortedIndices.Length(); ++s)
  {
    int              c         = aSortedIndices.Value(s).first;
    double           anEstDist = aSortedIndices.Value(s).second;
    const Candidate& aCand     = theCandidates.Value(c);

    // Early termination: for Min mode, if estimate is worse than best found,
    // remaining candidates (sorted) can't improve. Allow margin for Newton refinement.
    constexpr double aMinSkipThreshold = 2.0 - ExtremaPC::THE_MAX_SKIP_THRESHOLD; // 1.1 when threshold is 0.9
    if (theMode == ExtremaPC::SearchMode::Min && anEstDist > aBestSqDist * aMinSkipThreshold)
    {
      break;
    }
    if (theMode == ExtremaPC::SearchMode::Max && anEstDist < aBestSqDist * ExtremaPC::THE_MAX_SKIP_THRESHOLD)
    {
      break;
    }

    // Skip if too close to already found root
    bool aSkip = false;
    for (int r = 0; r < aFoundRoots.Length(); ++r)
    {
      if (std::abs(aCand.StartU - aFoundRoots.Value(r)) < theTol)
      {
        aSkip = true;
        break;
      }
    }
    if (aSkip)
    {
      continue;
    }

    // Refine candidate with Newton + iterative grid fallback
    auto aRefineRes = RefineSingleCandidate(aCand, theGrid, theCurve, theP,
                                             theUMin, theUMax, theTol,
                                             aConfig, theRefConfig, aFoundRoots);

    if (aRefineRes.has_value())
    {
      auto [aRootU, aPt, aSqDist, aIsMin] = *aRefineRes;

      // Filter by mode
      bool aKeep = false;
      if (theMode == ExtremaPC::SearchMode::MinMax)
      {
        aKeep = true;
      }
      else if (theMode == ExtremaPC::SearchMode::Min && aIsMin)
      {
        aKeep = true;
      }
      else if (theMode == ExtremaPC::SearchMode::Max && !aIsMin)
      {
        aKeep = true;
      }

      if (aKeep)
      {
        ExtremaPC::ExtremumResult anExt;
        anExt.Parameter      = aRootU;
        anExt.Point          = aPt;
        anExt.SquareDistance = aSqDist;
        anExt.IsMinimum      = aIsMin;
        aResult.Extrema.Append(anExt);

        aFoundRoots.Append(aRootU);

        // Update best distance for early termination
        if (theMode == ExtremaPC::SearchMode::Min && aSqDist < aBestSqDist)
        {
          aBestSqDist = aSqDist;
        }
        else if (theMode == ExtremaPC::SearchMode::Max && aSqDist > aBestSqDist)
        {
          aBestSqDist = aSqDist;
        }
      }
    }
  }

  if (aResult.Extrema.IsEmpty() && theCandidates.IsEmpty())
  {
    aResult.Status = ExtremaPC::Status::NoSolution;
  }

  return aResult;
}

//! @brief Wrapper for point-on-curve evaluation (for AddEndpointExtrema).
class PointEvaluator
{
public:
  PointEvaluator(const Adaptor3d_Curve& theCurve)
      : myCurve(&theCurve)
  {
  }

  gp_Pnt Value(double theU) const { return myCurve->Value(theU); }

private:
  const Adaptor3d_Curve* myCurve;
};

//! @brief Perform grid-based extrema computation with custom parameters.
//!
//! Complete algorithm combining grid building, scanning, and refinement.
//! This overload allows passing custom parameter samples (e.g., knot-aware).
//!
//! @tparam GridEval type with SetParams and EvaluateGridD1 methods
//! @param theEval grid evaluator for batch curve evaluation
//! @param theCurve curve adaptor for Newton refinement
//! @param theP query point
//! @param theParams custom parameter values to sample
//! @param theUMin parameter range lower bound (for Newton bounds)
//! @param theUMax parameter range upper bound (for Newton bounds)
//! @param theTol tolerance
//! @param theMode search mode
//! @param theIncludeEndpoints include endpoints as extrema
//! @return extrema result
template <typename GridEval>
inline ExtremaPC::Result PerformGridBasedWithParams(GridEval&              theEval,
                                                     const Adaptor3d_Curve& theCurve,
                                                     const gp_Pnt&          theP,
                                                     const math_Vector&     theParams,
                                                     double                 theUMin,
                                                     double                 theUMax,
                                                     double                 theTol,
                                                     ExtremaPC::SearchMode  theMode,
                                                     bool                   theIncludeEndpoints)
{
  // Build grid with D1 evaluation using custom params
  NCollection_Array1<GridPoint> aGrid = BuildGrid(theEval, theParams);

  // Scan for candidates
  NCollection_Vector<Candidate> aCandidates = ScanGrid(aGrid, theP, theTol, theMode);

  // Refine candidates
  ExtremaPC::Result aResult = RefineCandidates(aCandidates, aGrid, theCurve, theP, theUMin, theUMax, theTol, theMode);

  // Add endpoint extrema if requested
  if (theIncludeEndpoints)
  {
    PointEvaluator        anEval(theCurve);
    ExtremaPC::Domain1D   aDomain(theUMin, theUMax);
    ExtremaPC::AddEndpointExtrema(aResult, theP, aDomain, anEval, theTol, theMode);
  }

  // Update status based on whether we found any extrema
  if (!aResult.Extrema.IsEmpty())
  {
    aResult.Status = ExtremaPC::Status::OK;
  }

  return aResult;
}

//! @brief Perform grid-based extrema computation with uniform sampling.
//!
//! Complete algorithm combining grid building, scanning, and refinement.
//!
//! @tparam GridEval type with SetParams and EvaluateGridD1 methods
//! @param theEval grid evaluator for batch curve evaluation
//! @param theCurve curve adaptor for Newton refinement
//! @param theP query point
//! @param theUMin parameter range lower bound
//! @param theUMax parameter range upper bound
//! @param theNbSamples number of grid samples
//! @param theTol tolerance
//! @param theMode search mode
//! @param theIncludeEndpoints include endpoints as extrema
//! @return extrema result
template <typename GridEval>
inline ExtremaPC::Result PerformGridBased(GridEval&              theEval,
                                          const Adaptor3d_Curve& theCurve,
                                          const gp_Pnt&          theP,
                                          double                 theUMin,
                                          double                 theUMax,
                                          int                    theNbSamples,
                                          double                 theTol,
                                          ExtremaPC::SearchMode  theMode,
                                          bool                   theIncludeEndpoints)
{
  // Build uniform parameter grid
  math_Vector aParams = BuildUniformParams(theUMin, theUMax, theNbSamples);

  // Delegate to the params-based version
  return PerformGridBasedWithParams(theEval, theCurve, theP, aParams, theUMin, theUMax, theTol, theMode, theIncludeEndpoints);
}

//! @brief Perform extrema using previous solution as hint (NextProject pattern).
//!
//! When projecting sequential points along a curve or path, the previous
//! projection parameter often provides an excellent starting point for Newton.
//! This function first tries Newton from the hint, falling back to full grid
//! search only if that fails.
//!
//! Inspired by ShapeAnalysis_Curve::NextProject pattern.
//!
//! @param theCurve curve adaptor
//! @param theP query point
//! @param theHintU parameter hint (e.g., previous solution)
//! @param theUMin parameter range lower bound
//! @param theUMax parameter range upper bound
//! @param theTol tolerance
//! @param theSearchRadius how far from hint to search (fraction of range)
//! @return extrema result (typically single minimum)
inline ExtremaPC::Result PerformWithHint(const Adaptor3d_Curve& theCurve,
                                          const gp_Pnt&          theP,
                                          double                 theHintU,
                                          double                 theUMin,
                                          double                 theUMax,
                                          double                 theTol,
                                          double                 theSearchRadius = ExtremaPC::THE_HINT_SEARCH_RADIUS)
{
  ExtremaPC::Result aResult;

  // Clamp hint to valid range
  theHintU = std::max(theUMin, std::min(theUMax, theHintU));

  // Newton configuration
  MathUtils::Config aConfig;
  aConfig.XTolerance    = theTol * ExtremaPC::THE_NEWTON_XTOL_FACTOR;
  aConfig.FTolerance    = theTol * ExtremaPC::THE_NEWTON_FTOL_FACTOR;
  aConfig.MaxIterations = 20;

  ExtremaPC_DistanceFunction aFunc(theCurve, theP);

  // Define search bounds around hint
  double aRange    = theUMax - theUMin;
  double aHalfSpan = aRange * theSearchRadius;
  double aLocalMin = std::max(theUMin, theHintU - aHalfSpan);
  double aLocalMax = std::min(theUMax, theHintU + aHalfSpan);

  // Try Newton from hint first
  MathUtils::ScalarResult aNewtonRes = MathRoot::NewtonBounded(aFunc, theHintU, aLocalMin, aLocalMax, aConfig);

  if (aNewtonRes.IsDone())
  {
    double aRootU = std::max(theUMin, std::min(theUMax, *aNewtonRes.Root));
    gp_Pnt aPt    = theCurve.Value(aRootU);
    double aSqDist = theP.SquareDistance(aPt);
    double aDF    = aNewtonRes.Derivative.value_or(0.0);
    bool   aIsMin = (aDF > 0.0);

    ExtremaPC::ExtremumResult anExt;
    anExt.Parameter      = aRootU;
    anExt.Point          = aPt;
    anExt.SquareDistance = aSqDist;
    anExt.IsMinimum      = aIsMin;
    aResult.Extrema.Append(anExt);
    aResult.Status = ExtremaPC::Status::OK;
    return aResult;
  }

  // Newton from hint failed - try iterative grid refinement in local region
  RefinementConfig aRefConfig;
  aRefConfig.MaxRefinementPasses  = 4;  // More passes for robustness
  aRefConfig.RefinementGridSize   = 25;
  // Use slightly larger narrowing factor for hint-based search (more aggressive convergence)
  aRefConfig.RangeNarrowingFactor = ExtremaPC::THE_RANGE_NARROWING_FACTOR * 1.2;

  double aBestU    = theHintU;
  double aBestDist = std::numeric_limits<double>::max();
  double aRefUMin  = aLocalMin;
  double aRefUMax  = aLocalMax;

  for (int aPass = 0; aPass < aRefConfig.MaxRefinementPasses; ++aPass)
  {
    const int    aNbSamples = aRefConfig.RefinementGridSize;
    const double aStep      = (aRefUMax - aRefUMin) / (aNbSamples - 1);

    for (int i = 0; i < aNbSamples; ++i)
    {
      double aU    = aRefUMin + i * aStep;
      gp_Pnt aPt   = theCurve.Value(aU);
      double aDist = theP.SquareDistance(aPt);

      if (aDist < aBestDist)
      {
        aBestDist = aDist;
        aBestU    = aU;
      }
    }

    // Narrow range
    double aRangeHalf = (aRefUMax - aRefUMin) * aRefConfig.RangeNarrowingFactor * 0.5;
    aRefUMin = std::max(theUMin, aBestU - aRangeHalf);
    aRefUMax = std::min(theUMax, aBestU + aRangeHalf);

    // Try Newton with refined point
    MathUtils::ScalarResult aRetryRes = MathRoot::NewtonBounded(aFunc, aBestU, aRefUMin, aRefUMax, aConfig);
    if (aRetryRes.IsDone())
    {
      double aRootU = std::max(theUMin, std::min(theUMax, *aRetryRes.Root));
      gp_Pnt aPt    = theCurve.Value(aRootU);
      double aSqDist = theP.SquareDistance(aPt);
      double aDF    = aRetryRes.Derivative.value_or(0.0);
      bool   aIsMin = (aDF > 0.0);

      ExtremaPC::ExtremumResult anExt;
      anExt.Parameter      = aRootU;
      anExt.Point          = aPt;
      anExt.SquareDistance = aSqDist;
      anExt.IsMinimum      = aIsMin;
      aResult.Extrema.Append(anExt);
      aResult.Status = ExtremaPC::Status::OK;
      return aResult;
    }
  }

  // Use best grid point as fallback
  gp_Pnt aPt;
  gp_Vec aD1;
  theCurve.D1(aBestU, aPt, aD1);
  gp_Vec aVec(theP, aPt);
  double aF = aVec.Dot(aD1);

  if (std::abs(aF) < theTol * 100.0)
  {
    double aSqDist = theP.SquareDistance(aPt);
    double aStep   = aRange * ExtremaPC::THE_REFINEMENT_STEP_RATIO;
    double aDistPlus  = theP.SquareDistance(theCurve.Value(std::min(theUMax, aBestU + aStep)));
    double aDistMinus = theP.SquareDistance(theCurve.Value(std::max(theUMin, aBestU - aStep)));
    bool   aIsMin     = (aSqDist <= aDistPlus) && (aSqDist <= aDistMinus);

    ExtremaPC::ExtremumResult anExt;
    anExt.Parameter      = aBestU;
    anExt.Point          = aPt;
    anExt.SquareDistance = aSqDist;
    anExt.IsMinimum      = aIsMin;
    aResult.Extrema.Append(anExt);
    aResult.Status = ExtremaPC::Status::OK;
    return aResult;
  }

  aResult.Status = ExtremaPC::Status::NoSolution;
  return aResult;
}

//! @brief Perform extrema computation using pre-built cached grid (interior only).
//!
//! This function uses a pre-computed grid for faster repeated queries.
//! The grid should be built once with BuildGrid() and reused for multiple points.
//!
//! @param theGrid cached grid points
//! @param theCurve curve adaptor for Newton refinement
//! @param theP query point
//! @param theDomain parameter domain
//! @param theTol tolerance
//! @param theMode search mode
//! @return extrema result (interior only, no endpoints)
inline ExtremaPC::Result PerformWithCachedGrid(const NCollection_Array1<GridPoint>& theGrid,
                                                const Adaptor3d_Curve&               theCurve,
                                                const gp_Pnt&                        theP,
                                                const ExtremaPC::Domain1D&           theDomain,
                                                double                               theTol,
                                                ExtremaPC::SearchMode                theMode)
{
  // Scan for candidates
  NCollection_Vector<Candidate> aCandidates = ScanGrid(theGrid, theP, theTol, theMode);

  // Refine candidates
  ExtremaPC::Result aResult = RefineCandidates(aCandidates, theGrid, theCurve, theP,
                                                theDomain.Min, theDomain.Max, theTol, theMode);

  return aResult;
}

} // namespace ExtremaPC_GridEvaluator

#endif // _ExtremaPC_GridEvaluator_HeaderFile
