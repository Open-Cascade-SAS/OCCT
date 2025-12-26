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

#ifndef _ExtremaPS_GridEvaluator_HeaderFile
#define _ExtremaPS_GridEvaluator_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <ExtremaPS.hxx>
#include <ExtremaPS_DistanceFunction.hxx>
#include <GeomGridEval.hxx>
#include <math_Vector.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_Vector.hxx>

#include <algorithm>
#include <cmath>
#include <limits>

//! @brief Grid-based point-surface extrema computation utilities.
//!
//! Provides common algorithm for grid-based extrema finding that can be used
//! by BSpline, Bezier, Offset, and other surface evaluators.
//!
//! Algorithm:
//! 1. Build 2D grid of (u, v, point, dU, dV) from GeomGridEval_Surface
//! 2. Scan grid to find candidate cells (where gradient points toward query point)
//! 3. Newton refinement on each candidate using ExtremaPS_Newton::Solve
//! 4. Optional boundary handling
namespace ExtremaPS_GridEvaluator
{

//! Cached grid point with pre-computed data.
struct GridPoint
{
  double U;     //!< U parameter value
  double V;     //!< V parameter value
  gp_Pnt Point; //!< Surface point S(u,v)
  gp_Vec DU;    //!< First derivative dS/dU
  gp_Vec DV;    //!< First derivative dS/dV
};

//! Candidate cell for Newton refinement.
struct Candidate
{
  int    IdxU;    //!< U grid index
  int    IdxV;    //!< V grid index
  double StartU;  //!< Starting U for Newton
  double StartV;  //!< Starting V for Newton
  double EstDist; //!< Estimated squared distance
  double GradMag; //!< Gradient magnitude (smaller = closer to extremum)
};

//! @brief Build 2D grid from GeomGridEval D1 results.
//!
//! @tparam GridEval type with EvaluateGridD1(uParams, vParams) method
//! @param theEval grid evaluator
//! @param theUParams U parameter values (math_Vector with Array1() accessor)
//! @param theVParams V parameter values (math_Vector with Array1() accessor)
//! @return 2D array of GridPoint (0-based indexing)
template <typename GridEval>
inline NCollection_Array2<GridPoint> BuildGrid(GridEval&          theEval,
                                               const math_Vector& theUParams,
                                               const math_Vector& theVParams)
{
  // Use Array1() accessor to pass to GeomGridEval which expects NCollection_Array1
  NCollection_Array2<GeomGridEval::SurfD1> aD1Grid =
    theEval.EvaluateGridD1(theUParams.Array1(), theVParams.Array1());

  const int                     aNbU = theUParams.Length();
  const int                     aNbV = theVParams.Length();
  NCollection_Array2<GridPoint> aGrid(0, aNbU - 1, 0, aNbV - 1);

  for (int i = 0; i < aNbU; ++i)
  {
    for (int j = 0; j < aNbV; ++j)
    {
      const int aD1UIdx = aD1Grid.LowerRow() + i;
      const int aD1VIdx = aD1Grid.LowerCol() + j;

      aGrid(i, j).U     = theUParams(theUParams.Lower() + i);
      aGrid(i, j).V     = theVParams(theVParams.Lower() + j);
      aGrid(i, j).Point = aD1Grid.Value(aD1UIdx, aD1VIdx).Point;
      aGrid(i, j).DU    = aD1Grid.Value(aD1UIdx, aD1VIdx).D1U;
      aGrid(i, j).DV    = aD1Grid.Value(aD1UIdx, aD1VIdx).D1V;
    }
  }

  return aGrid;
}

//! @brief Build uniform parameter grids.
//! @return math_Vector with 1-based indexing
inline math_Vector BuildUniformParams(double theMin, double theMax, int theNbSamples)
{
  math_Vector  aParams(1, theNbSamples);
  const double aStep = (theMax - theMin) / (theNbSamples - 1);

  for (int i = 1; i <= theNbSamples; ++i)
  {
    aParams(i) = theMin + (i - 1) * aStep;
  }
  aParams(theNbSamples) = theMax; // Ensure exact endpoint

  return aParams;
}

//! @brief Scan 2D grid to find candidate cells for extrema (optimized single-pass).
//!
//! A cell is a candidate if:
//! - The gradient direction changes within the cell (sign change in Fu or Fv)
//! - Or the gradient is near zero at a grid point
//! - Or a grid point is a local distance minimum among neighbors
//!
//! This optimized version avoids creating large temporary arrays by computing
//! values on-the-fly and using a combined single-pass algorithm.
//!
//! @param theGrid cached grid points
//! @param theP query point
//! @param theTol tolerance
//! @param theMode search mode
//! @return vector of candidate cells
inline NCollection_Vector<Candidate> ScanGrid(const NCollection_Array2<GridPoint>& theGrid,
                                              const gp_Pnt&                        theP,
                                              double                               theTol,
                                              ExtremaPS::SearchMode                theMode)
{
  NCollection_Vector<Candidate> aCandidates(8); // Small bucket for typical extrema count

  const int aNbU = theGrid.UpperRow() - theGrid.LowerRow() + 1;
  const int aNbV = theGrid.UpperCol() - theGrid.LowerCol() + 1;

  if (aNbU < 2 || aNbV < 2)
  {
    return aCandidates;
  }

  // Query point coordinates for fast access
  const double aPx = theP.X();
  const double aPy = theP.Y();
  const double aPz = theP.Z();

  // Tolerance for near-zero gradient detection
  const double aTolF = theTol * ExtremaPS::THE_GRADIENT_TOL_FACTOR;

  // Track global min/max for fallback candidates
  double aGlobalMinDist = std::numeric_limits<double>::max();
  double aGlobalMaxDist = -std::numeric_limits<double>::max();
  int aMinI = 0, aMinJ = 0, aMaxI = 0, aMaxJ = 0;

  // Helper to compute Fu, Fv, and distance for a grid point
  auto computeGridPointData = [&](int i, int j, double& outFu, double& outFv, double& outDist) {
    const GridPoint& aGP = theGrid(i, j);
    const double aDx = aGP.Point.X() - aPx;
    const double aDy = aGP.Point.Y() - aPy;
    const double aDz = aGP.Point.Z() - aPz;
    outFu = aDx * aGP.DU.X() + aDy * aGP.DU.Y() + aDz * aGP.DU.Z();
    outFv = aDx * aGP.DV.X() + aDy * aGP.DV.Y() + aDz * aGP.DV.Z();
    outDist = aDx * aDx + aDy * aDy + aDz * aDz;
  };

  // Single-pass: scan cells and find candidates
  // We process each cell (i,j) to (i+1,j+1) and check for extrema conditions
  for (int i = 0; i < aNbU - 1; ++i)
  {
    for (int j = 0; j < aNbV - 1; ++j)
    {
      // Compute Fu, Fv, Dist for all 4 corners of the cell
      double aFu00, aFv00, aDist00;
      double aFu10, aFv10, aDist10;
      double aFu01, aFv01, aDist01;
      double aFu11, aFv11, aDist11;

      computeGridPointData(i, j, aFu00, aFv00, aDist00);
      computeGridPointData(i + 1, j, aFu10, aFv10, aDist10);
      computeGridPointData(i, j + 1, aFu01, aFv01, aDist01);
      computeGridPointData(i + 1, j + 1, aFu11, aFv11, aDist11);

      // Update global min/max tracking
      if (aDist00 < aGlobalMinDist) { aGlobalMinDist = aDist00; aMinI = i; aMinJ = j; }
      if (aDist10 < aGlobalMinDist) { aGlobalMinDist = aDist10; aMinI = i + 1; aMinJ = j; }
      if (aDist01 < aGlobalMinDist) { aGlobalMinDist = aDist01; aMinI = i; aMinJ = j + 1; }
      if (aDist11 < aGlobalMinDist) { aGlobalMinDist = aDist11; aMinI = i + 1; aMinJ = j + 1; }

      if (aDist00 > aGlobalMaxDist) { aGlobalMaxDist = aDist00; aMaxI = i; aMaxJ = j; }
      if (aDist10 > aGlobalMaxDist) { aGlobalMaxDist = aDist10; aMaxI = i + 1; aMaxJ = j; }
      if (aDist01 > aGlobalMaxDist) { aGlobalMaxDist = aDist01; aMaxI = i; aMaxJ = j + 1; }
      if (aDist11 > aGlobalMaxDist) { aGlobalMaxDist = aDist11; aMaxI = i + 1; aMaxJ = j + 1; }

      // Check for sign changes in Fu and Fv
      const bool aFuSignChange =
        (aFu00 * aFu10 < 0.0) || (aFu01 * aFu11 < 0.0) ||
        (aFu00 * aFu01 < 0.0) || (aFu10 * aFu11 < 0.0);

      const bool aFvSignChange =
        (aFv00 * aFv10 < 0.0) || (aFv01 * aFv11 < 0.0) ||
        (aFv00 * aFv01 < 0.0) || (aFv10 * aFv11 < 0.0);

      // Compute min distance in cell for tolerance scaling
      const double aMinDist = std::min({aDist00, aDist10, aDist01, aDist11});
      const double aDistScale = std::sqrt(aMinDist) * ExtremaPS::THE_DISTANCE_SCALE_RATIO;
      const double aTolRel = std::max(aTolF, aDistScale);

      // Check for near-zero gradient at any corner
      const bool aNearZero =
        (std::abs(aFu00) < aTolRel && std::abs(aFv00) < aTolRel) ||
        (std::abs(aFu10) < aTolRel && std::abs(aFv10) < aTolRel) ||
        (std::abs(aFu01) < aTolRel && std::abs(aFv01) < aTolRel) ||
        (std::abs(aFu11) < aTolRel && std::abs(aFv11) < aTolRel);

      // Add candidate if sign changes in both directions or near-zero gradient
      if ((aFuSignChange && aFvSignChange) || aNearZero)
      {
        const double aStartU = (theGrid(i, j).U + theGrid(i + 1, j + 1).U) * 0.5;
        const double aStartV = (theGrid(i, j).V + theGrid(i + 1, j + 1).V) * 0.5;

        // Compute minimum gradient magnitude
        const double aGrad00 = aFu00 * aFu00 + aFv00 * aFv00;
        const double aGrad10 = aFu10 * aFu10 + aFv10 * aFv10;
        const double aGrad01 = aFu01 * aFu01 + aFv01 * aFv01;
        const double aGrad11 = aFu11 * aFu11 + aFv11 * aFv11;
        const double aMinGradMag = std::min({aGrad00, aGrad10, aGrad01, aGrad11});

        Candidate aCand;
        aCand.IdxU = i;
        aCand.IdxV = j;
        aCand.StartU = aStartU;
        aCand.StartV = aStartV;
        aCand.EstDist = aMinDist;
        aCand.GradMag = aMinGradMag;
        aCandidates.Append(aCand);
      }
    }
  }

  // Add global minimum candidate (always useful as Newton starting point)
  if (theMode == ExtremaPS::SearchMode::Min || theMode == ExtremaPS::SearchMode::MinMax)
  {
    const int aCellI = std::max(0, std::min(aNbU - 2, aMinI));
    const int aCellJ = std::max(0, std::min(aNbV - 2, aMinJ));

    Candidate aCand;
    aCand.IdxU = aCellI;
    aCand.IdxV = aCellJ;
    aCand.StartU = theGrid(aMinI, aMinJ).U;
    aCand.StartV = theGrid(aMinI, aMinJ).V;
    aCand.EstDist = aGlobalMinDist;
    aCand.GradMag = 0.0; // Global min has priority
    aCandidates.Append(aCand);
  }

  if (theMode == ExtremaPS::SearchMode::Max || theMode == ExtremaPS::SearchMode::MinMax)
  {
    const int aCellI = std::max(0, std::min(aNbU - 2, aMaxI));
    const int aCellJ = std::max(0, std::min(aNbV - 2, aMaxJ));

    Candidate aCand;
    aCand.IdxU = aCellI;
    aCand.IdxV = aCellJ;
    aCand.StartU = theGrid(aMaxI, aMaxJ).U;
    aCand.StartV = theGrid(aMaxI, aMaxJ).V;
    aCand.EstDist = aGlobalMaxDist;
    aCand.GradMag = 0.0; // Global max has priority
    aCandidates.Append(aCand);
  }

  return aCandidates;
}

//! @brief Refine candidates using 2D Newton's method with grid fallback.
//!
//! When Newton fails to converge, falls back to the best grid point in the candidate cell.
//! Based on patterns from ShapeAnalysis_Surface::SurfaceNewton.
//! This overload populates an existing Result object.
//!
//! @param[out] theResult result to populate
//! @param[in] theCandidates candidate cells from ScanGrid
//! @param[in] theGrid cached grid points
//! @param[in] theSurface surface adaptor
//! @param[in] theP query point
//! @param[in] theUMin U range lower bound
//! @param[in] theUMax U range upper bound
//! @param[in] theVMin V range lower bound
//! @param[in] theVMax V range upper bound
//! @param[in] theTol tolerance
//! @param[in] theMode search mode
inline void RefineCandidates(ExtremaPS::Result&                   theResult,
                             const NCollection_Vector<Candidate>& theCandidates,
                             const NCollection_Array2<GridPoint>& theGrid,
                             const Adaptor3d_Surface&             theSurface,
                             const gp_Pnt&                        theP,
                             double                               theUMin,
                             double                               theUMax,
                             double                               theVMin,
                             double                               theVMax,
                             double                               theTol,
                             ExtremaPS::SearchMode                theMode)
{
  theResult.Status = ExtremaPS::Status::OK;

  NCollection_Vector<std::pair<double, double>> aFoundRoots(8); // (U, V) pairs, small bucket for roots

  ExtremaPS_DistanceFunction aFunc(theSurface, theP);

  // Sort candidates by combined score: distance + gradient magnitude
  // Smaller gradient = closer to extremum = better Newton starting point
  // This improves convergence speed and accuracy
  struct SortEntry
  {
    int    Idx;
    double Dist;
    double GradMag;
  };

  NCollection_Vector<SortEntry> aSortedEntries(8); // Small bucket for candidates
  for (int c = 0; c < theCandidates.Length(); ++c)
  {
    const Candidate& aCand = theCandidates.Value(c);
    aSortedEntries.Append({c, aCand.EstDist, aCand.GradMag});
  }

  if (theMode == ExtremaPS::SearchMode::Min)
  {
    // Sort by distance first, then by gradient (smaller gradient = better starting point)
    std::sort(aSortedEntries.begin(),
              aSortedEntries.end(),
              [](const SortEntry& a, const SortEntry& b) {
                // Primary: distance (ascending for Min)
                if (std::abs(a.Dist - b.Dist) > ExtremaPS::THE_RELATIVE_TOLERANCE * std::max(a.Dist, b.Dist))
                  return a.Dist < b.Dist;
                // Secondary: gradient magnitude (smaller = better)
                return a.GradMag < b.GradMag;
              });
  }
  else if (theMode == ExtremaPS::SearchMode::Max)
  {
    std::sort(aSortedEntries.begin(),
              aSortedEntries.end(),
              [](const SortEntry& a, const SortEntry& b) {
                if (std::abs(a.Dist - b.Dist) > ExtremaPS::THE_RELATIVE_TOLERANCE * std::max(a.Dist, b.Dist))
                  return a.Dist > b.Dist;
                return a.GradMag < b.GradMag;
              });
  }

  double aBestSqDist = (theMode == ExtremaPS::SearchMode::Min)
                         ? std::numeric_limits<double>::max()
                         : -std::numeric_limits<double>::max();

  for (int s = 0; s < aSortedEntries.Length(); ++s)
  {
    const SortEntry& anEntry   = aSortedEntries.Value(s);
    double           anEstDist = anEntry.Dist;

    // Early termination - candidates are sorted, so if distance exceeds threshold, remaining are
    // worse
    constexpr double aMinSkipThreshold = 2.0 - ExtremaPS::THE_MAX_SKIP_THRESHOLD; // 1.1 when threshold is 0.9
    if (theMode == ExtremaPS::SearchMode::Min && anEstDist > aBestSqDist * aMinSkipThreshold)
      break;
    if (theMode == ExtremaPS::SearchMode::Max && anEstDist < aBestSqDist * ExtremaPS::THE_MAX_SKIP_THRESHOLD)
      break;

    const Candidate& aCand = theCandidates.Value(anEntry.Idx);

    // Newton bounds from grid cell
    double aCellUMin = theGrid(aCand.IdxU, aCand.IdxV).U;
    double aCellUMax = theGrid(aCand.IdxU + 1, aCand.IdxV).U;
    double aCellVMin = theGrid(aCand.IdxU, aCand.IdxV).V;
    double aCellVMax = theGrid(aCand.IdxU, aCand.IdxV + 1).V;

    // Expand bounds slightly
    double aExpandU = (aCellUMax - aCellUMin) * ExtremaPS::THE_CELL_EXPAND_RATIO;
    double aExpandV = (aCellVMax - aCellVMin) * ExtremaPS::THE_CELL_EXPAND_RATIO;
    aCellUMin       = std::max(theUMin, aCellUMin - aExpandU);
    aCellUMax       = std::min(theUMax, aCellUMax + aExpandU);
    aCellVMin       = std::max(theVMin, aCellVMin - aExpandV);
    aCellVMax       = std::min(theVMax, aCellVMax + aExpandV);

    // Newton iteration
    ExtremaPS_Newton::Result aNewtonRes = ExtremaPS_Newton::Solve(aFunc,
                                                                  aCand.StartU,
                                                                  aCand.StartV,
                                                                  aCellUMin,
                                                                  aCellUMax,
                                                                  aCellVMin,
                                                                  aCellVMax,
                                                                  theTol);

    double aRootU     = 0.0;
    double aRootV     = 0.0;
    bool   aConverged = false;

    if (aNewtonRes.IsDone)
    {
      aRootU     = std::max(theUMin, std::min(theUMax, aNewtonRes.U));
      aRootV     = std::max(theVMin, std::min(theVMax, aNewtonRes.V));
      aConverged = true;
    }
    else
    {
      // Newton fallback: Find best grid point in the candidate cell
      // This is based on ShapeAnalysis_Surface pattern: when SurfaceNewton fails,
      // use the best discrete approximation from the grid.
      double aBestGridDist = std::numeric_limits<double>::max();
      int    aBestI        = aCand.IdxU;
      int    aBestJ        = aCand.IdxV;

      // Check all 4 corners of the cell
      for (int di = 0; di <= 1; ++di)
      {
        for (int dj = 0; dj <= 1; ++dj)
        {
          int    i     = aCand.IdxU + di;
          int    j     = aCand.IdxV + dj;
          double aDist = theP.SquareDistance(theGrid(i, j).Point);
          if (aDist < aBestGridDist)
          {
            aBestGridDist = aDist;
            aBestI        = i;
            aBestJ        = j;
          }
        }
      }

      // Use the best corner and try Newton again with this starting point
      aRootU = theGrid(aBestI, aBestJ).U;
      aRootV = theGrid(aBestI, aBestJ).V;

      // Try Newton one more time from the best corner
      ExtremaPS_Newton::Result aRetryRes = ExtremaPS_Newton::Solve(aFunc,
                                                                   aRootU,
                                                                   aRootV,
                                                                   aCellUMin,
                                                                   aCellUMax,
                                                                   aCellVMin,
                                                                   aCellVMax,
                                                                   theTol * 10.0);

      if (aRetryRes.IsDone)
      {
        aRootU     = std::max(theUMin, std::min(theUMax, aRetryRes.U));
        aRootV     = std::max(theVMin, std::min(theVMax, aRetryRes.V));
        aConverged = true;
      }
      else
      {
        // Final fallback: use the best grid point directly
        // Check if gradient is small enough to consider this an approximate extremum
        double aFu, aFv;
        aFunc.Value(aRootU, aRootV, aFu, aFv);
        double aFNorm = std::sqrt(aFu * aFu + aFv * aFv);

        // Accept as approximate extremum if gradient is reasonably small
        // (using relaxed gradient tolerance factor)
        if (aFNorm < theTol * ExtremaPS::THE_GRADIENT_TOL_FACTOR)
        {
          aConverged = true;
        }
      }
    }

    if (!aConverged)
      continue;

    // Check for duplicate
    bool aSkip = false;
    for (int r = 0; r < aFoundRoots.Length(); ++r)
    {
      if (std::abs(aRootU - aFoundRoots.Value(r).first) < theTol
          && std::abs(aRootV - aFoundRoots.Value(r).second) < theTol)
      {
        aSkip = true;
        break;
      }
    }
    if (aSkip)
      continue;

    gp_Pnt aPt     = theSurface.Value(aRootU, aRootV);
    double aSqDist = theP.SquareDistance(aPt);

    // Classify as min or max using second derivative (Hessian positive definite = min)
    // Note: Hessian classification can be unreliable at curved surfaces, so we don't
    // use it for filtering. Instead, keep all converged candidates and let the
    // MinSquareDistance/MaxSquareDistance methods find the actual best result.
    double aFu, aFv, aDFuu, aDFuv, aDFvv;
    aFunc.ValueAndJacobian(aRootU, aRootV, aFu, aFv, aDFuu, aDFuv, aDFvv);
    bool aIsMin = (aDFuu > 0.0 && aDFuu * aDFvv - aDFuv * aDFuv > 0.0);

    // Keep all converged candidates - distance comparison determines actual min/max
    {
      ExtremaPS::ExtremumResult anExt;
      anExt.U              = aRootU;
      anExt.V              = aRootV;
      anExt.Point          = aPt;
      anExt.SquareDistance = aSqDist;
      anExt.IsMinimum      = aIsMin;
      theResult.Extrema.Append(anExt);

      aFoundRoots.Append(std::make_pair(aRootU, aRootV));

      if (theMode == ExtremaPS::SearchMode::Min && aSqDist < aBestSqDist)
        aBestSqDist = aSqDist;
      else if (theMode == ExtremaPS::SearchMode::Max && aSqDist > aBestSqDist)
        aBestSqDist = aSqDist;
    }
  }

  if (theResult.Extrema.IsEmpty() && theCandidates.IsEmpty())
  {
    theResult.Status = ExtremaPS::Status::NoSolution;
  }
}

//! @brief Refine candidates using 2D Newton's method with grid fallback.
//!
//! When Newton fails to converge, falls back to the best grid point in the candidate cell.
//! This overload returns a new Result object.
//!
//! @param[in] theCandidates candidate cells from ScanGrid
//! @param[in] theGrid cached grid points
//! @param[in] theSurface surface adaptor
//! @param[in] theP query point
//! @param[in] theUMin U range lower bound
//! @param[in] theUMax U range upper bound
//! @param[in] theVMin V range lower bound
//! @param[in] theVMax V range upper bound
//! @param[in] theTol tolerance
//! @param[in] theMode search mode
//! @return extrema result
inline ExtremaPS::Result RefineCandidates(const NCollection_Vector<Candidate>& theCandidates,
                                          const NCollection_Array2<GridPoint>& theGrid,
                                          const Adaptor3d_Surface&             theSurface,
                                          const gp_Pnt&                        theP,
                                          double                               theUMin,
                                          double                               theUMax,
                                          double                               theVMin,
                                          double                               theVMax,
                                          double                               theTol,
                                          ExtremaPS::SearchMode                theMode)
{
  ExtremaPS::Result aResult;
  RefineCandidates(aResult, theCandidates, theGrid, theSurface, theP, theUMin, theUMax, theVMin, theVMax, theTol, theMode);
  return aResult;
}

//! @brief Perform extrema computation using pre-built cached grid (interior only).
//!
//! This is the optimized version for repeated queries with the same parameter range.
//! The grid is built once and reused across multiple Perform calls.
//! This overload populates an existing Result object (useful for avoiding allocations).
//!
//! @param[out] theResult result to populate (should be cleared before calling)
//! @param[in] theGrid pre-built grid of (u, v, point, dU, dV)
//! @param[in] theSurface surface adaptor
//! @param[in] theP query point
//! @param[in] theDomain 2D parameter domain
//! @param[in] theTol tolerance
//! @param[in] theMode search mode
inline void PerformWithCachedGrid(ExtremaPS::Result&                   theResult,
                                  const NCollection_Array2<GridPoint>& theGrid,
                                  const Adaptor3d_Surface&             theSurface,
                                  const gp_Pnt&                        theP,
                                  const ExtremaPS::Domain2D&           theDomain,
                                  double                               theTol,
                                  ExtremaPS::SearchMode                theMode)
{
  const int aNbU = theGrid.UpperRow() - theGrid.LowerRow() + 1;
  const int aNbV = theGrid.UpperCol() - theGrid.LowerCol() + 1;

  // Scan for candidates
  NCollection_Vector<Candidate> aCandidates = ScanGrid(theGrid, theP, theTol, theMode);

  // Refine candidates (populates theResult)
  RefineCandidates(theResult,
                   aCandidates,
                   theGrid,
                   theSurface,
                   theP,
                   theDomain.UMin,
                   theDomain.UMax,
                   theDomain.VMin,
                   theDomain.VMax,
                   theTol,
                   theMode);

  // Fallback: refine the best grid point using Newton, then add if it's better than current result
  if (theMode == ExtremaPS::SearchMode::Min || theMode == ExtremaPS::SearchMode::MinMax)
  {
    double aGridMinDist = std::numeric_limits<double>::max();
    double aGridMinU = 0.0, aGridMinV = 0.0;
    int    aGridMinI = 0, aGridMinJ = 0;

    for (int i = 0; i < aNbU; ++i)
    {
      for (int j = 0; j < aNbV; ++j)
      {
        double aDist = theP.SquareDistance(theGrid(i, j).Point);
        if (aDist < aGridMinDist)
        {
          aGridMinDist = aDist;
          aGridMinU    = theGrid(i, j).U;
          aGridMinV    = theGrid(i, j).V;
          aGridMinI    = i;
          aGridMinJ    = j;
        }
      }
    }

    // Try Newton refinement from the best grid point
    ExtremaPS_DistanceFunction aFunc(theSurface, theP);
    ExtremaPS_Newton::Result   aNewtonRes = ExtremaPS_Newton::Solve(aFunc,
                                                                  aGridMinU,
                                                                  aGridMinV,
                                                                  theDomain.UMin,
                                                                  theDomain.UMax,
                                                                  theDomain.VMin,
                                                                  theDomain.VMax,
                                                                  theTol);

    double aRefinedU    = aGridMinU;
    double aRefinedV    = aGridMinV;
    gp_Pnt aRefinedPt   = theGrid(aGridMinI, aGridMinJ).Point;
    double aRefinedDist = aGridMinDist;

    if (aNewtonRes.IsDone)
    {
      aRefinedU    = std::max(theDomain.UMin, std::min(theDomain.UMax, aNewtonRes.U));
      aRefinedV    = std::max(theDomain.VMin, std::min(theDomain.VMax, aNewtonRes.V));
      aRefinedPt   = theSurface.Value(aRefinedU, aRefinedV);
      aRefinedDist = theP.SquareDistance(aRefinedPt);
    }

    double aResultMinDist =
      theResult.Extrema.IsEmpty() ? std::numeric_limits<double>::max() : theResult.MinSquareDistance();

    if (aRefinedDist < aResultMinDist * ExtremaPS::THE_REFINED_DIST_THRESHOLD)
    {
      // Check for duplicate
      bool aSkip = false;
      for (int i = 0; i < theResult.Extrema.Length(); ++i)
      {
        const auto& anExisting = theResult.Extrema.Value(i);
        if (std::abs(anExisting.U - aRefinedU) < theTol
            && std::abs(anExisting.V - aRefinedV) < theTol)
        {
          aSkip = true;
          break;
        }
      }
      if (!aSkip)
      {
        ExtremaPS::ExtremumResult anExt;
        anExt.U              = aRefinedU;
        anExt.V              = aRefinedV;
        anExt.Point          = aRefinedPt;
        anExt.SquareDistance = aRefinedDist;
        anExt.IsMinimum      = true;
        theResult.Extrema.Append(anExt);
      }
    }
  }

  if (!theResult.Extrema.IsEmpty())
  {
    theResult.Status = ExtremaPS::Status::OK;
  }
}

//! @brief Perform extrema computation using pre-built cached grid (interior only).
//!
//! This is the optimized version for repeated queries with the same parameter range.
//! The grid is built once and reused across multiple Perform calls.
//! This overload returns a new Result object.
//!
//! @param[in] theGrid pre-built grid of (u, v, point, dU, dV)
//! @param[in] theSurface surface adaptor
//! @param[in] theP query point
//! @param[in] theDomain 2D parameter domain
//! @param[in] theTol tolerance
//! @param[in] theMode search mode
//! @return extrema result with interior extrema only
inline ExtremaPS::Result PerformWithCachedGrid(const NCollection_Array2<GridPoint>& theGrid,
                                               const Adaptor3d_Surface&             theSurface,
                                               const gp_Pnt&                        theP,
                                               const ExtremaPS::Domain2D&           theDomain,
                                               double                               theTol,
                                               ExtremaPS::SearchMode                theMode)
{
  ExtremaPS::Result aResult;
  PerformWithCachedGrid(aResult, theGrid, theSurface, theP, theDomain, theTol, theMode);
  return aResult;
}

//! @brief Perform extrema computation with boundary using pre-built cached grid.
//!
//! This is the optimized version that includes boundary extrema on edges and corners.
//!
//! @param theGrid pre-built grid of (u, v, point, dU, dV)
//! @param theSurface surface adaptor
//! @param theP query point
//! @param theDomain 2D parameter domain
//! @param theTol tolerance
//! @param theMode search mode
//! @return extrema result with interior + boundary extrema
inline ExtremaPS::Result PerformWithCachedGridAndBoundary(
  const NCollection_Array2<GridPoint>& theGrid,
  const Adaptor3d_Surface&             theSurface,
  const gp_Pnt&                        theP,
  const ExtremaPS::Domain2D&           theDomain,
  double                               theTol,
  ExtremaPS::SearchMode                theMode)
{
  // Start with interior extrema
  ExtremaPS::Result aResult = PerformWithCachedGrid(theGrid, theSurface, theP, theDomain, theTol, theMode);

  // Add boundary extrema
  struct SurfaceEvaluator
  {
    const Adaptor3d_Surface* Surf;

    gp_Pnt Value(double aU, double aV) const { return Surf->Value(aU, aV); }
  };

  SurfaceEvaluator anEval{&theSurface};
  ExtremaPS::AddBoundaryExtrema(aResult, theP, theDomain, anEval, theTol, theMode);

  if (!aResult.Extrema.IsEmpty())
  {
    aResult.Status = ExtremaPS::Status::OK;
  }

  return aResult;
}

//! @brief Perform grid-based extrema computation (interior only).
//!
//! @tparam GridEval type with SetUVParams and EvaluateGridD1 methods
//! @param theEval grid evaluator
//! @param theSurface surface adaptor
//! @param theP query point
//! @param theDomain 2D parameter domain
//! @param theNbUSamples number of U samples
//! @param theNbVSamples number of V samples
//! @param theTol tolerance
//! @param theMode search mode
//! @return extrema result with interior extrema only
template <typename GridEval>
inline ExtremaPS::Result PerformGridBased(GridEval&                  theEval,
                                          const Adaptor3d_Surface&   theSurface,
                                          const gp_Pnt&              theP,
                                          const ExtremaPS::Domain2D& theDomain,
                                          int                        theNbUSamples,
                                          int                        theNbVSamples,
                                          double                     theTol,
                                          ExtremaPS::SearchMode      theMode)
{
  // Build uniform parameter grids
  math_Vector aUParams = BuildUniformParams(theDomain.UMin, theDomain.UMax, theNbUSamples);
  math_Vector aVParams = BuildUniformParams(theDomain.VMin, theDomain.VMax, theNbVSamples);

  // Build grid with D1 evaluation
  NCollection_Array2<GridPoint> aGrid = BuildGrid(theEval, aUParams, aVParams);

  // Use the cached grid version
  return PerformWithCachedGrid(aGrid, theSurface, theP, theDomain, theTol, theMode);
}

//! @brief Perform grid-based extrema computation with boundary.
//!
//! @tparam GridEval type with SetUVParams and EvaluateGridD1 methods
//! @param theEval grid evaluator
//! @param theSurface surface adaptor
//! @param theP query point
//! @param theDomain 2D parameter domain
//! @param theNbUSamples number of U samples
//! @param theNbVSamples number of V samples
//! @param theTol tolerance
//! @param theMode search mode
//! @return extrema result with interior + boundary extrema
template <typename GridEval>
inline ExtremaPS::Result PerformGridBasedWithBoundary(GridEval&                  theEval,
                                                      const Adaptor3d_Surface&   theSurface,
                                                      const gp_Pnt&              theP,
                                                      const ExtremaPS::Domain2D& theDomain,
                                                      int                        theNbUSamples,
                                                      int                        theNbVSamples,
                                                      double                     theTol,
                                                      ExtremaPS::SearchMode      theMode)
{
  // Build uniform parameter grids
  math_Vector aUParams = BuildUniformParams(theDomain.UMin, theDomain.UMax, theNbUSamples);
  math_Vector aVParams = BuildUniformParams(theDomain.VMin, theDomain.VMax, theNbVSamples);

  // Build grid with D1 evaluation
  NCollection_Array2<GridPoint> aGrid = BuildGrid(theEval, aUParams, aVParams);

  // Use the cached grid version with boundary
  return PerformWithCachedGridAndBoundary(aGrid, theSurface, theP, theDomain, theTol, theMode);
}

} // namespace ExtremaPS_GridEvaluator

#endif // _ExtremaPS_GridEvaluator_HeaderFile
