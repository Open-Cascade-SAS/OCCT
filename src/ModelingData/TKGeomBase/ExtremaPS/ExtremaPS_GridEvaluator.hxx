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

//! @brief Grid-based point-surface extrema computation class.
//!
//! Provides grid-based extrema finding algorithm with cached state for
//! optimal performance on repeated queries. Used by BSpline, Bezier,
//! Offset, and other surface evaluators.
//!
//! Algorithm:
//! 1. Build 2D grid of (u, v, point, dU, dV) from GeomGridEval_Surface
//! 2. Scan grid to find candidate cells (where gradient points toward query point)
//! 3. Newton refinement on each candidate using ExtremaPS_Newton::Solve
//! 4. Optional boundary handling
//!
//! All temporary vectors are stored as mutable fields and reused via Clear()
//! to avoid repeated heap allocations.
class ExtremaPS_GridEvaluator
{
public:
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

  //! Default constructor.
  ExtremaPS_GridEvaluator() = default;

  //! @brief Build 2D grid from GeomGridEval D1 results.
  //!
  //! @tparam GridEval type with EvaluateGridD1(uParams, vParams) method
  //! @param theEval grid evaluator
  //! @param theUParams U parameter values (math_Vector with Array1() accessor)
  //! @param theVParams V parameter values (math_Vector with Array1() accessor)
  template <typename GridEval>
  void BuildGrid(GridEval& theEval, const math_Vector& theUParams, const math_Vector& theVParams)
  {
    // Use Array1() accessor to pass to GeomGridEval which expects NCollection_Array1
    NCollection_Array2<GeomGridEval::SurfD1> aD1Grid =
      theEval.EvaluateGridD1(theUParams.Array1(), theVParams.Array1());

    const int aNbU = theUParams.Length();
    const int aNbV = theVParams.Length();

    // Resize grid if needed
    if (myGrid.RowLength() != aNbU || myGrid.ColLength() != aNbV)
    {
      myGrid = NCollection_Array2<GridPoint>(0, aNbU - 1, 0, aNbV - 1);
    }

    for (int i = 0; i < aNbU; ++i)
    {
      for (int j = 0; j < aNbV; ++j)
      {
        const int aD1UIdx = aD1Grid.LowerRow() + i;
        const int aD1VIdx = aD1Grid.LowerCol() + j;

        myGrid(i, j).U     = theUParams(theUParams.Lower() + i);
        myGrid(i, j).V     = theVParams(theVParams.Lower() + j);
        myGrid(i, j).Point = aD1Grid.Value(aD1UIdx, aD1VIdx).Point;
        myGrid(i, j).DU    = aD1Grid.Value(aD1UIdx, aD1VIdx).D1U;
        myGrid(i, j).DV    = aD1Grid.Value(aD1UIdx, aD1VIdx).D1V;
      }
    }
  }

  //! Returns the cached grid.
  const NCollection_Array2<GridPoint>& Grid() const { return myGrid; }

  //! Returns mutable reference to the result for post-processing (e.g., adding boundary extrema).
  ExtremaPS::Result& Result() const { return myResult; }

  //! @brief Perform extrema computation using cached grid (interior only).
  //!
  //! @param theSurface surface adaptor
  //! @param theP query point
  //! @param theDomain 2D parameter domain
  //! @param theTol tolerance
  //! @param theMode search mode
  //! @return const reference to result with interior extrema only
  [[nodiscard]] const ExtremaPS::Result& Perform(const Adaptor3d_Surface&   theSurface,
                                                 const gp_Pnt&              theP,
                                                 const ExtremaPS::Domain2D& theDomain,
                                                 double                     theTol,
                                                 ExtremaPS::SearchMode      theMode) const
  {
    myResult.Clear();
    scanGrid(theP, theTol, theMode);
    refineCandidates(theSurface, theP, theDomain, theTol, theMode);
    addGridMinFallback(theSurface, theP, theDomain, theTol, theMode);

    if (!myResult.Extrema.IsEmpty())
    {
      myResult.Status = ExtremaPS::Status::OK;
    }
    return myResult;
  }

  //! @brief Build uniform parameter grids.
  //! @return math_Vector with 1-based indexing
  static math_Vector BuildUniformParams(double theMin, double theMax, int theNbSamples)
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

private:
  //! Entry for sorting candidates.
  struct SortEntry
  {
    int    Idx;
    double Dist;
    double GradMag;
  };

  //! @brief Scan 2D grid to find candidate cells for extrema.
  void scanGrid(const gp_Pnt& theP, double theTol, ExtremaPS::SearchMode theMode) const
  {
    myCandidates.Clear();

    const int aNbU = myGrid.UpperRow() - myGrid.LowerRow() + 1;
    const int aNbV = myGrid.UpperCol() - myGrid.LowerCol() + 1;

    if (aNbU < 2 || aNbV < 2)
    {
      return;
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
    int    aMinI = 0, aMinJ = 0, aMaxI = 0, aMaxJ = 0;

    // Helper to compute Fu, Fv, and distance for a grid point
    auto computeGridPointData = [&](int i, int j, double& outFu, double& outFv, double& outDist) {
      const GridPoint& aGP  = myGrid(i, j);
      const double     aDx  = aGP.Point.X() - aPx;
      const double     aDy  = aGP.Point.Y() - aPy;
      const double     aDz  = aGP.Point.Z() - aPz;
      outFu                 = aDx * aGP.DU.X() + aDy * aGP.DU.Y() + aDz * aGP.DU.Z();
      outFv                 = aDx * aGP.DV.X() + aDy * aGP.DV.Y() + aDz * aGP.DV.Z();
      outDist               = aDx * aDx + aDy * aDy + aDz * aDz;
    };

    // Single-pass: scan cells and find candidates
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
        if (aDist00 < aGlobalMinDist)
        {
          aGlobalMinDist = aDist00;
          aMinI          = i;
          aMinJ          = j;
        }
        if (aDist10 < aGlobalMinDist)
        {
          aGlobalMinDist = aDist10;
          aMinI          = i + 1;
          aMinJ          = j;
        }
        if (aDist01 < aGlobalMinDist)
        {
          aGlobalMinDist = aDist01;
          aMinI          = i;
          aMinJ          = j + 1;
        }
        if (aDist11 < aGlobalMinDist)
        {
          aGlobalMinDist = aDist11;
          aMinI          = i + 1;
          aMinJ          = j + 1;
        }

        if (aDist00 > aGlobalMaxDist)
        {
          aGlobalMaxDist = aDist00;
          aMaxI          = i;
          aMaxJ          = j;
        }
        if (aDist10 > aGlobalMaxDist)
        {
          aGlobalMaxDist = aDist10;
          aMaxI          = i + 1;
          aMaxJ          = j;
        }
        if (aDist01 > aGlobalMaxDist)
        {
          aGlobalMaxDist = aDist01;
          aMaxI          = i;
          aMaxJ          = j + 1;
        }
        if (aDist11 > aGlobalMaxDist)
        {
          aGlobalMaxDist = aDist11;
          aMaxI          = i + 1;
          aMaxJ          = j + 1;
        }

        // Check for sign changes in Fu and Fv
        const bool aFuSignChange = (aFu00 * aFu10 < 0.0) || (aFu01 * aFu11 < 0.0)
                                   || (aFu00 * aFu01 < 0.0) || (aFu10 * aFu11 < 0.0);

        const bool aFvSignChange = (aFv00 * aFv10 < 0.0) || (aFv01 * aFv11 < 0.0)
                                   || (aFv00 * aFv01 < 0.0) || (aFv10 * aFv11 < 0.0);

        // Compute min distance in cell for tolerance scaling
        const double aMinDist   = std::min({aDist00, aDist10, aDist01, aDist11});
        const double aDistScale = std::sqrt(aMinDist) * ExtremaPS::THE_DISTANCE_SCALE_RATIO;
        const double aTolRel    = std::max(aTolF, aDistScale);

        // Check for near-zero gradient at any corner
        const bool aNearZero = (std::abs(aFu00) < aTolRel && std::abs(aFv00) < aTolRel)
                               || (std::abs(aFu10) < aTolRel && std::abs(aFv10) < aTolRel)
                               || (std::abs(aFu01) < aTolRel && std::abs(aFv01) < aTolRel)
                               || (std::abs(aFu11) < aTolRel && std::abs(aFv11) < aTolRel);

        // Add candidate if sign changes in both directions or near-zero gradient
        if ((aFuSignChange && aFvSignChange) || aNearZero)
        {
          const double aStartU = (myGrid(i, j).U + myGrid(i + 1, j + 1).U) * 0.5;
          const double aStartV = (myGrid(i, j).V + myGrid(i + 1, j + 1).V) * 0.5;

          // Compute minimum gradient magnitude
          const double aGrad00     = aFu00 * aFu00 + aFv00 * aFv00;
          const double aGrad10     = aFu10 * aFu10 + aFv10 * aFv10;
          const double aGrad01     = aFu01 * aFu01 + aFv01 * aFv01;
          const double aGrad11     = aFu11 * aFu11 + aFv11 * aFv11;
          const double aMinGradMag = std::min({aGrad00, aGrad10, aGrad01, aGrad11});

          Candidate aCand;
          aCand.IdxU    = i;
          aCand.IdxV    = j;
          aCand.StartU  = aStartU;
          aCand.StartV  = aStartV;
          aCand.EstDist = aMinDist;
          aCand.GradMag = aMinGradMag;
          myCandidates.Append(aCand);
        }
      }
    }

    // Add global minimum candidate (always useful as Newton starting point)
    if (theMode == ExtremaPS::SearchMode::Min || theMode == ExtremaPS::SearchMode::MinMax)
    {
      const int aCellI = std::max(0, std::min(aNbU - 2, aMinI));
      const int aCellJ = std::max(0, std::min(aNbV - 2, aMinJ));

      Candidate aCand;
      aCand.IdxU    = aCellI;
      aCand.IdxV    = aCellJ;
      aCand.StartU  = myGrid(aMinI, aMinJ).U;
      aCand.StartV  = myGrid(aMinI, aMinJ).V;
      aCand.EstDist = aGlobalMinDist;
      aCand.GradMag = 0.0; // Global min has priority
      myCandidates.Append(aCand);
    }

    if (theMode == ExtremaPS::SearchMode::Max || theMode == ExtremaPS::SearchMode::MinMax)
    {
      const int aCellI = std::max(0, std::min(aNbU - 2, aMaxI));
      const int aCellJ = std::max(0, std::min(aNbV - 2, aMaxJ));

      Candidate aCand;
      aCand.IdxU    = aCellI;
      aCand.IdxV    = aCellJ;
      aCand.StartU  = myGrid(aMaxI, aMaxJ).U;
      aCand.StartV  = myGrid(aMaxI, aMaxJ).V;
      aCand.EstDist = aGlobalMaxDist;
      aCand.GradMag = 0.0; // Global max has priority
      myCandidates.Append(aCand);
    }
  }

  //! @brief Refine candidates using 2D Newton's method with grid fallback.
  void refineCandidates(const Adaptor3d_Surface&   theSurface,
                        const gp_Pnt&              theP,
                        const ExtremaPS::Domain2D& theDomain,
                        double                     theTol,
                        ExtremaPS::SearchMode      theMode) const
  {
    myResult.Status = ExtremaPS::Status::OK;
    myFoundRoots.Clear();
    mySortedEntries.Clear();

    ExtremaPS_DistanceFunction aFunc(theSurface, theP);

    // Build sorted entries
    for (int c = 0; c < myCandidates.Length(); ++c)
    {
      const Candidate& aCand = myCandidates.Value(c);
      SortEntry        anEntry;
      anEntry.Idx     = c;
      anEntry.Dist    = aCand.EstDist;
      anEntry.GradMag = aCand.GradMag;
      mySortedEntries.Append(anEntry);
    }

    if (theMode == ExtremaPS::SearchMode::Min)
    {
      std::sort(mySortedEntries.begin(), mySortedEntries.end(), [](const SortEntry& a, const SortEntry& b) {
        if (std::abs(a.Dist - b.Dist) > ExtremaPS::THE_RELATIVE_TOLERANCE * std::max(a.Dist, b.Dist))
          return a.Dist < b.Dist;
        return a.GradMag < b.GradMag;
      });
    }
    else if (theMode == ExtremaPS::SearchMode::Max)
    {
      std::sort(mySortedEntries.begin(), mySortedEntries.end(), [](const SortEntry& a, const SortEntry& b) {
        if (std::abs(a.Dist - b.Dist) > ExtremaPS::THE_RELATIVE_TOLERANCE * std::max(a.Dist, b.Dist))
          return a.Dist > b.Dist;
        return a.GradMag < b.GradMag;
      });
    }

    double aBestSqDist = (theMode == ExtremaPS::SearchMode::Min) ? std::numeric_limits<double>::max()
                                                                 : -std::numeric_limits<double>::max();

    for (int s = 0; s < mySortedEntries.Length(); ++s)
    {
      const SortEntry& anEntry   = mySortedEntries.Value(s);
      double           anEstDist = anEntry.Dist;

      // Early termination
      constexpr double aMinSkipThreshold = 2.0 - ExtremaPS::THE_MAX_SKIP_THRESHOLD;
      if (theMode == ExtremaPS::SearchMode::Min && anEstDist > aBestSqDist * aMinSkipThreshold)
        break;
      if (theMode == ExtremaPS::SearchMode::Max && anEstDist < aBestSqDist * ExtremaPS::THE_MAX_SKIP_THRESHOLD)
        break;

      const Candidate& aCand = myCandidates.Value(anEntry.Idx);

      // Newton bounds from grid cell
      double aCellUMin = myGrid(aCand.IdxU, aCand.IdxV).U;
      double aCellUMax = myGrid(aCand.IdxU + 1, aCand.IdxV).U;
      double aCellVMin = myGrid(aCand.IdxU, aCand.IdxV).V;
      double aCellVMax = myGrid(aCand.IdxU, aCand.IdxV + 1).V;

      // Expand bounds slightly
      double aExpandU = (aCellUMax - aCellUMin) * ExtremaPS::THE_CELL_EXPAND_RATIO;
      double aExpandV = (aCellVMax - aCellVMin) * ExtremaPS::THE_CELL_EXPAND_RATIO;
      aCellUMin       = std::max(theDomain.UMin, aCellUMin - aExpandU);
      aCellUMax       = std::min(theDomain.UMax, aCellUMax + aExpandU);
      aCellVMin       = std::max(theDomain.VMin, aCellVMin - aExpandV);
      aCellVMax       = std::min(theDomain.VMax, aCellVMax + aExpandV);

      // Newton iteration
      ExtremaPS_Newton::Result aNewtonRes = ExtremaPS_Newton::Solve(
        aFunc, aCand.StartU, aCand.StartV, aCellUMin, aCellUMax, aCellVMin, aCellVMax, theTol);

      double aRootU     = 0.0;
      double aRootV     = 0.0;
      bool   aConverged = false;

      if (aNewtonRes.IsDone)
      {
        aRootU     = std::max(theDomain.UMin, std::min(theDomain.UMax, aNewtonRes.U));
        aRootV     = std::max(theDomain.VMin, std::min(theDomain.VMax, aNewtonRes.V));
        aConverged = true;
      }
      else
      {
        // Newton fallback: Find best grid point in the candidate cell
        double aBestGridDist = std::numeric_limits<double>::max();
        int    aBestI        = aCand.IdxU;
        int    aBestJ        = aCand.IdxV;

        for (int di = 0; di <= 1; ++di)
        {
          for (int dj = 0; dj <= 1; ++dj)
          {
            int    i     = aCand.IdxU + di;
            int    j     = aCand.IdxV + dj;
            double aDist = theP.SquareDistance(myGrid(i, j).Point);
            if (aDist < aBestGridDist)
            {
              aBestGridDist = aDist;
              aBestI        = i;
              aBestJ        = j;
            }
          }
        }

        aRootU = myGrid(aBestI, aBestJ).U;
        aRootV = myGrid(aBestI, aBestJ).V;

        // Try Newton one more time from the best corner
        ExtremaPS_Newton::Result aRetryRes = ExtremaPS_Newton::Solve(
          aFunc, aRootU, aRootV, aCellUMin, aCellUMax, aCellVMin, aCellVMax, theTol * 10.0);

        if (aRetryRes.IsDone)
        {
          aRootU     = std::max(theDomain.UMin, std::min(theDomain.UMax, aRetryRes.U));
          aRootV     = std::max(theDomain.VMin, std::min(theDomain.VMax, aRetryRes.V));
          aConverged = true;
        }
        else
        {
          // Final fallback: use the best grid point directly
          double aFu, aFv;
          aFunc.Value(aRootU, aRootV, aFu, aFv);
          double aFNorm = std::sqrt(aFu * aFu + aFv * aFv);

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
      for (int r = 0; r < myFoundRoots.Length(); ++r)
      {
        if (std::abs(aRootU - myFoundRoots.Value(r).first) < theTol
            && std::abs(aRootV - myFoundRoots.Value(r).second) < theTol)
        {
          aSkip = true;
          break;
        }
      }
      if (aSkip)
        continue;

      gp_Pnt aPt     = theSurface.Value(aRootU, aRootV);
      double aSqDist = theP.SquareDistance(aPt);

      // Classify as min or max using second derivative
      double aFu, aFv, aDFuu, aDFuv, aDFvv;
      aFunc.ValueAndJacobian(aRootU, aRootV, aFu, aFv, aDFuu, aDFuv, aDFvv);
      bool aIsMin = (aDFuu > 0.0 && aDFuu * aDFvv - aDFuv * aDFuv > 0.0);

      ExtremaPS::ExtremumResult anExt;
      anExt.U              = aRootU;
      anExt.V              = aRootV;
      anExt.Point          = aPt;
      anExt.SquareDistance = aSqDist;
      anExt.IsMinimum      = aIsMin;
      myResult.Extrema.Append(anExt);

      myFoundRoots.Append(std::make_pair(aRootU, aRootV));

      if (theMode == ExtremaPS::SearchMode::Min && aSqDist < aBestSqDist)
        aBestSqDist = aSqDist;
      else if (theMode == ExtremaPS::SearchMode::Max && aSqDist > aBestSqDist)
        aBestSqDist = aSqDist;
    }

    if (myResult.Extrema.IsEmpty() && myCandidates.IsEmpty())
    {
      myResult.Status = ExtremaPS::Status::NoSolution;
    }
  }

  //! @brief Add grid minimum fallback if needed.
  void addGridMinFallback(const Adaptor3d_Surface&   theSurface,
                          const gp_Pnt&              theP,
                          const ExtremaPS::Domain2D& theDomain,
                          double                     theTol,
                          ExtremaPS::SearchMode      theMode) const
  {
    if (theMode != ExtremaPS::SearchMode::Min && theMode != ExtremaPS::SearchMode::MinMax)
      return;

    const int aNbU = myGrid.UpperRow() - myGrid.LowerRow() + 1;
    const int aNbV = myGrid.UpperCol() - myGrid.LowerCol() + 1;

    double aGridMinDist = std::numeric_limits<double>::max();
    double aGridMinU = 0.0, aGridMinV = 0.0;
    int    aGridMinI = 0, aGridMinJ = 0;

    for (int i = 0; i < aNbU; ++i)
    {
      for (int j = 0; j < aNbV; ++j)
      {
        double aDist = theP.SquareDistance(myGrid(i, j).Point);
        if (aDist < aGridMinDist)
        {
          aGridMinDist = aDist;
          aGridMinU    = myGrid(i, j).U;
          aGridMinV    = myGrid(i, j).V;
          aGridMinI    = i;
          aGridMinJ    = j;
        }
      }
    }

    // Try Newton refinement from the best grid point
    ExtremaPS_DistanceFunction aFunc(theSurface, theP);
    ExtremaPS_Newton::Result   aNewtonRes = ExtremaPS_Newton::Solve(
      aFunc, aGridMinU, aGridMinV, theDomain.UMin, theDomain.UMax, theDomain.VMin, theDomain.VMax, theTol);

    double aRefinedU    = aGridMinU;
    double aRefinedV    = aGridMinV;
    gp_Pnt aRefinedPt   = myGrid(aGridMinI, aGridMinJ).Point;
    double aRefinedDist = aGridMinDist;

    if (aNewtonRes.IsDone)
    {
      aRefinedU    = std::max(theDomain.UMin, std::min(theDomain.UMax, aNewtonRes.U));
      aRefinedV    = std::max(theDomain.VMin, std::min(theDomain.VMax, aNewtonRes.V));
      aRefinedPt   = theSurface.Value(aRefinedU, aRefinedV);
      aRefinedDist = theP.SquareDistance(aRefinedPt);
    }

    double aResultMinDist =
      myResult.Extrema.IsEmpty() ? std::numeric_limits<double>::max() : myResult.MinSquareDistance();

    if (aRefinedDist < aResultMinDist * ExtremaPS::THE_REFINED_DIST_THRESHOLD)
    {
      // Check for duplicate
      bool aSkip = false;
      for (int i = 0; i < myResult.Extrema.Length(); ++i)
      {
        const auto& anExisting = myResult.Extrema.Value(i);
        if (std::abs(anExisting.U - aRefinedU) < theTol && std::abs(anExisting.V - aRefinedV) < theTol)
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
        myResult.Extrema.Append(anExt);
      }
    }
  }

private:
  NCollection_Array2<GridPoint> myGrid; //!< Cached grid

  // Mutable cached temporaries (reused via Clear())
  mutable ExtremaPS::Result                          myResult;        //!< Reusable result
  mutable NCollection_Vector<Candidate>              myCandidates;    //!< Candidates from grid scan
  mutable NCollection_Vector<std::pair<double, double>> myFoundRoots; //!< Found roots for dedup
  mutable NCollection_Vector<SortEntry>              mySortedEntries; //!< Sorted candidate indices
};

#endif // _ExtremaPS_GridEvaluator_HeaderFile
