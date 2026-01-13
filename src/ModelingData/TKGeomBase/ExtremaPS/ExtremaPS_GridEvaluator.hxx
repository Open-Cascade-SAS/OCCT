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
#include <gp_Vec.hxx>
#include <math_Vector.hxx>
#include <MathUtils_Config.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_Vector.hxx>
#include <Precision.hxx>

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

    // Resize grid (reuses memory if size unchanged)
    myGrid.Resize(0, aNbU - 1, 0, aNbV - 1, false);
    myGridData.Resize(0, aNbU - 1, 0, aNbV - 1, false);

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

    // Fast path: try from cached solution for spatially coherent queries
    // This avoids full grid scan when possible
    if (tryFromCachedSolution(theSurface, theP, theDomain, theTol, theMode))
    {
      return myResult;
    }

    // Full grid scan + refinement (fallback when cache doesn't work)
    scanGrid(theP, theTol, theMode);

    // Add cached solution as additional candidate for Newton (hint optimization)
    // This improves convergence when query points are spatially coherent
    addCachedAsCandidate(theP, theDomain);

    refineCandidates(theSurface, theP, theDomain, theTol, theMode);
    addGridMinFallback(theSurface, theP, theDomain, theTol, theMode);

    if (!myResult.Extrema.IsEmpty())
    {
      myResult.Status = ExtremaPS::Status::OK;
      // Cache the best solution for next query
      updateSolutionCache(theP);
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

  //! Pre-computed data for each grid point (Fu, Fv, squared distance).
  struct GridPointData
  {
    double Fu;   //!< Gradient component in U
    double Fv;   //!< Gradient component in V
    double Dist; //!< Squared distance to query point
  };

  //! @brief Scan 2D grid to find candidate cells for extrema.
  //! Uses pre-computed grid point data to avoid redundant calculations.
  //! @note theTol is intentionally unused - gradient tolerance uses Precision::Confusion() for consistency.
  void scanGrid(const gp_Pnt& theP, [[maybe_unused]] double theTol, ExtremaPS::SearchMode theMode) const
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

    // Tolerance for near-zero gradient detection (use squared for faster comparison)
    // Use Precision::Confusion() as base, NOT theTol which is distance tolerance and can be ~1.0
    const double aTolF   = Precision::Confusion() * ExtremaPS::THE_GRADIENT_TOL_FACTOR;
    const double aTolFSq = aTolF * aTolF;
    const double aScaleRatioSq =
      ExtremaPS::THE_DISTANCE_SCALE_RATIO * ExtremaPS::THE_DISTANCE_SCALE_RATIO;

    // Reset global min/max tracking
    myMinDist = std::numeric_limits<double>::max();
    myMaxDist = -std::numeric_limits<double>::max();
    myMinI = myMinJ = myMaxI = myMaxJ = 0;

    // Pre-compute Fu, Fv, Dist for ALL grid points once (avoids redundant computation)
    // Note: myGridData is already allocated in BuildGrid() with correct size
    for (int i = 0; i < aNbU; ++i)
    {
      for (int j = 0; j < aNbV; ++j)
      {
        const GridPoint& aGP = myGrid(i, j);
        const double     aDx = aGP.Point.X() - aPx;
        const double     aDy = aGP.Point.Y() - aPy;
        const double     aDz = aGP.Point.Z() - aPz;

        GridPointData& aData = myGridData(i, j);
        aData.Fu             = aDx * aGP.DU.X() + aDy * aGP.DU.Y() + aDz * aGP.DU.Z();
        aData.Fv             = aDx * aGP.DV.X() + aDy * aGP.DV.Y() + aDz * aGP.DV.Z();
        aData.Dist           = aDx * aDx + aDy * aDy + aDz * aDz;

        // Track global min/max during pre-computation (cached for addGridMinFallback)
        if (aData.Dist < myMinDist)
        {
          myMinDist = aData.Dist;
          myMinI    = i;
          myMinJ    = j;
        }
        if (aData.Dist > myMaxDist)
        {
          myMaxDist = aData.Dist;
          myMaxI    = i;
          myMaxJ    = j;
        }
      }
    }

    // Scan cells using pre-computed data
    for (int i = 0; i < aNbU - 1; ++i)
    {
      for (int j = 0; j < aNbV - 1; ++j)
      {
        // Access pre-computed data for 4 corners
        const GridPointData& aD00 = myGridData(i, j);
        const GridPointData& aD10 = myGridData(i + 1, j);
        const GridPointData& aD01 = myGridData(i, j + 1);
        const GridPointData& aD11 = myGridData(i + 1, j + 1);

        // Compute squared gradient magnitudes for all corners (needed for both checks)
        const double aGrad00 = aD00.Fu * aD00.Fu + aD00.Fv * aD00.Fv;
        const double aGrad10 = aD10.Fu * aD10.Fu + aD10.Fv * aD10.Fv;
        const double aGrad01 = aD01.Fu * aD01.Fu + aD01.Fv * aD01.Fv;
        const double aGrad11 = aD11.Fu * aD11.Fu + aD11.Fv * aD11.Fv;

        // Compute min distance and min gradient magnitude using fast min
        double aMinDist    = aD00.Dist;
        double aMinGradMag = aGrad00;
        if (aD10.Dist < aMinDist)
          aMinDist = aD10.Dist;
        if (aD01.Dist < aMinDist)
          aMinDist = aD01.Dist;
        if (aD11.Dist < aMinDist)
          aMinDist = aD11.Dist;
        if (aGrad10 < aMinGradMag)
          aMinGradMag = aGrad10;
        if (aGrad01 < aMinGradMag)
          aMinGradMag = aGrad01;
        if (aGrad11 < aMinGradMag)
          aMinGradMag = aGrad11;

        // Squared tolerance for near-zero gradient (avoids sqrt)
        const double aTolRelSq = std::max(aTolFSq, aMinDist * aScaleRatioSq);

        // Check for near-zero gradient (use squared magnitudes)
        const bool aNearZero = (aGrad00 < aTolRelSq) || (aGrad10 < aTolRelSq)
                               || (aGrad01 < aTolRelSq) || (aGrad11 < aTolRelSq);

        // Early accept if near-zero gradient found
        bool aAddCandidate = aNearZero;

        // Only check sign changes if not already accepted
        if (!aAddCandidate)
        {
          // Check for sign changes in Fu and Fv (both required)
          const bool aFuSignChange = (aD00.Fu * aD10.Fu < 0.0) || (aD01.Fu * aD11.Fu < 0.0)
                                     || (aD00.Fu * aD01.Fu < 0.0) || (aD10.Fu * aD11.Fu < 0.0);
          if (aFuSignChange)
          {
            const bool aFvSignChange = (aD00.Fv * aD10.Fv < 0.0) || (aD01.Fv * aD11.Fv < 0.0)
                                       || (aD00.Fv * aD01.Fv < 0.0) || (aD10.Fv * aD11.Fv < 0.0);
            aAddCandidate = aFvSignChange;
          }
        }

        if (aAddCandidate)
        {
          Candidate aCand;
          aCand.IdxU    = i;
          aCand.IdxV    = j;
          aCand.StartU  = (myGrid(i, j).U + myGrid(i + 1, j + 1).U) * 0.5;
          aCand.StartV  = (myGrid(i, j).V + myGrid(i + 1, j + 1).V) * 0.5;
          aCand.EstDist = aMinDist;
          aCand.GradMag = aMinGradMag;
          myCandidates.Append(aCand);
        }
      }
    }

    // Add global minimum candidate (always useful as Newton starting point)
    if (theMode == ExtremaPS::SearchMode::Min || theMode == ExtremaPS::SearchMode::MinMax)
    {
      const int aCellI = std::max(0, std::min(aNbU - 2, myMinI));
      const int aCellJ = std::max(0, std::min(aNbV - 2, myMinJ));

      Candidate aCand;
      aCand.IdxU    = aCellI;
      aCand.IdxV    = aCellJ;
      aCand.StartU  = myGrid(myMinI, myMinJ).U;
      aCand.StartV  = myGrid(myMinI, myMinJ).V;
      aCand.EstDist = myMinDist;
      aCand.GradMag = 0.0; // Global min has priority
      myCandidates.Append(aCand);
    }

    if (theMode == ExtremaPS::SearchMode::Max || theMode == ExtremaPS::SearchMode::MinMax)
    {
      const int aCellI = std::max(0, std::min(aNbU - 2, myMaxI));
      const int aCellJ = std::max(0, std::min(aNbV - 2, myMaxJ));

      Candidate aCand;
      aCand.IdxU    = aCellI;
      aCand.IdxV    = aCellJ;
      aCand.StartU  = myGrid(myMaxI, myMaxJ).U;
      aCand.StartV  = myGrid(myMaxI, myMaxJ).V;
      aCand.EstDist = myMaxDist;
      aCand.GradMag = 0.0; // Global max has priority
      myCandidates.Append(aCand);
    }
  }

  //! @brief Add cached solution as an additional candidate for Newton refinement.
  //!
  //! This is a "hint" optimization - the cached solution is used as an additional
  //! starting point for Newton, improving convergence when query points are spatially coherent.
  //! The grid scan is always performed to ensure correctness.
  //!
  //! @param theP query point
  //! @param theDomain 2D parameter domain
  void addCachedAsCandidate(const gp_Pnt& theP, const ExtremaPS::Domain2D& theDomain) const
  {
    if (myCacheCount == 0)
      return;

    // Find nearest cached solution
    int    aNearestIdx  = (myCacheIndex + ExtremaPS::THE_CACHE_SIZE - 1) % ExtremaPS::THE_CACHE_SIZE;
    double aNearestDist = theP.SquareDistance(myCachedSolutions[aNearestIdx].QueryPoint);

    for (int i = 1; i < myCacheCount; ++i)
    {
      int    aIdx  = (myCacheIndex + ExtremaPS::THE_CACHE_SIZE - 1 - i) % ExtremaPS::THE_CACHE_SIZE;
      double aDist = theP.SquareDistance(myCachedSolutions[aIdx].QueryPoint);
      if (aDist < aNearestDist)
      {
        aNearestDist = aDist;
        aNearestIdx  = aIdx;
      }
    }

    // Only use cache if query point is spatially close
    if (aNearestDist > ExtremaPS::THE_COHERENCE_THRESHOLD_SQ)
      return;

    const CachedSolution& aNearest = myCachedSolutions[aNearestIdx];

    // Compute starting point with optional trajectory prediction
    double aStartU = aNearest.U;
    double aStartV = aNearest.V;

    if (myCacheCount >= 3)
    {
      // Get 3 most recent points for trajectory prediction
      int aIdx2 = (myCacheIndex + ExtremaPS::THE_CACHE_SIZE - 1) % ExtremaPS::THE_CACHE_SIZE;
      int aIdx1 = (myCacheIndex + ExtremaPS::THE_CACHE_SIZE - 2) % ExtremaPS::THE_CACHE_SIZE;
      int aIdx0 = (myCacheIndex + ExtremaPS::THE_CACHE_SIZE - 3) % ExtremaPS::THE_CACHE_SIZE;

      const CachedSolution& aS0 = myCachedSolutions[aIdx0];
      const CachedSolution& aS1 = myCachedSolutions[aIdx1];
      const CachedSolution& aS2 = myCachedSolutions[aIdx2];

      gp_Vec aV01(aS0.QueryPoint, aS1.QueryPoint);
      gp_Vec aV12(aS1.QueryPoint, aS2.QueryPoint);
      double aMag01 = aV01.Magnitude();
      double aMag12 = aV12.Magnitude();

      if (aMag01 > Precision::Confusion() && aMag12 > Precision::Confusion())
      {
        double aRatio = aMag12 / aMag01;
        if (aRatio > ExtremaPS::THE_TRAJECTORY_MIN_RATIO
            && aRatio < ExtremaPS::THE_TRAJECTORY_MAX_RATIO)
        {
          double aCos = aV01.Dot(aV12) / (aMag01 * aMag12);
          if (aCos > ExtremaPS::THE_TRAJECTORY_MIN_COS)
          {
            // Use quadratic extrapolation if 4+ points available and trajectory is smooth
            // Quadratic: U_next = 3*U2 - 3*U1 + U0 (includes velocity + acceleration)
            if (myCacheCount >= 4 && aCos > ExtremaPS::THE_TRAJECTORY_QUADRATIC_COS)
            {
              aStartU = 3.0 * aS2.U - 3.0 * aS1.U + aS0.U;
              aStartV = 3.0 * aS2.V - 3.0 * aS1.V + aS0.V;
            }
            else
            {
              // Linear extrapolation for 3 points or lower coherence
              double aDeltaU = aS2.U - aS1.U;
              double aDeltaV = aS2.V - aS1.V;
              aStartU        = aS2.U + aDeltaU;
              aStartV        = aS2.V + aDeltaV;
            }
            aStartU = std::max(theDomain.UMin, std::min(theDomain.UMax, aStartU));
            aStartV = std::max(theDomain.VMin, std::min(theDomain.VMax, aStartV));
          }
        }
      }
    }

    // Find the grid cell containing the cached solution using binary search O(log n)
    const int aNbU = myGrid.UpperRow() - myGrid.LowerRow() + 1;
    const int aNbV = myGrid.UpperCol() - myGrid.LowerCol() + 1;

    // Binary search for U cell
    int aCellI = 0;
    {
      int aLo = 0, aHi = aNbU - 2;
      while (aLo <= aHi)
      {
        int aMid = (aLo + aHi) / 2;
        if (aStartU < myGrid(aMid, 0).U)
          aHi = aMid - 1;
        else if (aStartU > myGrid(aMid + 1, 0).U)
          aLo = aMid + 1;
        else
        {
          aCellI = aMid;
          break;
        }
      }
      if (aLo > aHi)
        aCellI = std::max(0, std::min(aNbU - 2, aLo));
    }

    // Binary search for V cell
    int aCellJ = 0;
    {
      int aLo = 0, aHi = aNbV - 2;
      while (aLo <= aHi)
      {
        int aMid = (aLo + aHi) / 2;
        if (aStartV < myGrid(0, aMid).V)
          aHi = aMid - 1;
        else if (aStartV > myGrid(0, aMid + 1).V)
          aLo = aMid + 1;
        else
        {
          aCellJ = aMid;
          break;
        }
      }
      if (aLo > aHi)
        aCellJ = std::max(0, std::min(aNbV - 2, aLo));
    }

    // Compute proper estimates from grid cell (like regular grid candidates)
    // This ensures cache candidate competes fairly with grid candidates
    // (aNbU, aNbV already defined above for binary search)

    // Ensure cell indices are valid for data access
    const int aDataI = std::max(0, std::min(aNbU - 2, aCellI));
    const int aDataJ = std::max(0, std::min(aNbV - 2, aCellJ));

    // Get grid data at cell corners
    const GridPointData& aD00 = myGridData(aDataI, aDataJ);
    const GridPointData& aD10 = myGridData(aDataI + 1, aDataJ);
    const GridPointData& aD01 = myGridData(aDataI, aDataJ + 1);
    const GridPointData& aD11 = myGridData(aDataI + 1, aDataJ + 1);

    // Compute min distance from cell corners (proper estimate)
    double aMinDist = aD00.Dist;
    if (aD10.Dist < aMinDist)
      aMinDist = aD10.Dist;
    if (aD01.Dist < aMinDist)
      aMinDist = aD01.Dist;
    if (aD11.Dist < aMinDist)
      aMinDist = aD11.Dist;

    // Compute min gradient magnitude from cell corners
    const double aGrad00 = aD00.Fu * aD00.Fu + aD00.Fv * aD00.Fv;
    const double aGrad10 = aD10.Fu * aD10.Fu + aD10.Fv * aD10.Fv;
    const double aGrad01 = aD01.Fu * aD01.Fu + aD01.Fv * aD01.Fv;
    const double aGrad11 = aD11.Fu * aD11.Fu + aD11.Fv * aD11.Fv;
    double       aMinGradMag = aGrad00;
    if (aGrad10 < aMinGradMag)
      aMinGradMag = aGrad10;
    if (aGrad01 < aMinGradMag)
      aMinGradMag = aGrad01;
    if (aGrad11 < aMinGradMag)
      aMinGradMag = aGrad11;

    // Add candidate with proper estimates (trajectory-predicted starting point)
    Candidate aCand;
    aCand.IdxU    = aCellI;
    aCand.IdxV    = aCellJ;
    aCand.StartU  = aStartU;
    aCand.StartV  = aStartV;
    aCand.EstDist = aMinDist;    // Actual distance estimate from grid
    aCand.GradMag = aMinGradMag; // Actual gradient magnitude from grid
    myCandidates.Append(aCand);
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
      std::sort(mySortedEntries.begin(),
                mySortedEntries.end(),
                [](const SortEntry& a, const SortEntry& b) {
                  if (std::abs(a.Dist - b.Dist)
                      > ExtremaPS::THE_RELATIVE_TOLERANCE * std::max(a.Dist, b.Dist))
                    return a.Dist < b.Dist;
                  return a.GradMag < b.GradMag;
                });
    }
    else if (theMode == ExtremaPS::SearchMode::Max)
    {
      std::sort(mySortedEntries.begin(),
                mySortedEntries.end(),
                [](const SortEntry& a, const SortEntry& b) {
                  if (std::abs(a.Dist - b.Dist)
                      > ExtremaPS::THE_RELATIVE_TOLERANCE * std::max(a.Dist, b.Dist))
                    return a.Dist > b.Dist;
                  return a.GradMag < b.GradMag;
                });
    }

    double aBestSqDist = (theMode == ExtremaPS::SearchMode::Min)
                           ? std::numeric_limits<double>::max()
                           : -std::numeric_limits<double>::max();

    const int aMaxCandidates = std::min(mySortedEntries.Length(), ExtremaPS::THE_MAX_CANDIDATES_TO_PROCESS);
    for (int s = 0; s < aMaxCandidates; ++s)
    {
      const SortEntry& anEntry   = mySortedEntries.Value(s);
      double           anEstDist = anEntry.Dist;

      // Early termination: skip candidates that are clearly worse than the best found.
      // For Min mode: skip if estimated distance > best * (1 + margin).
      // For Max mode: skip if estimated distance < best * threshold.
      if (theMode == ExtremaPS::SearchMode::Min
          && anEstDist > aBestSqDist * (1.0 + ExtremaPS::THE_MIN_SKIP_MARGIN))
        break;
      if (theMode == ExtremaPS::SearchMode::Max
          && anEstDist < aBestSqDist * ExtremaPS::THE_MAX_SKIP_THRESHOLD)
        break;

      const Candidate& aCand = myCandidates.Value(anEntry.Idx);

      // Newton from cell center
      double aBestRootU = 0.0;
      double aBestRootV = 0.0;
      bool   aConverged = false;

      // Solve using Newton from cell center
      ExtremaPS_Newton::Result aNewtonRes = ExtremaPS_Newton::Solve(aFunc,
                                                                     aCand.StartU,
                                                                     aCand.StartV,
                                                                     theDomain.UMin,
                                                                     theDomain.UMax,
                                                                     theDomain.VMin,
                                                                     theDomain.VMax,
                                                                     theTol);
      if (aNewtonRes.IsDone)
      {
        aBestRootU = aNewtonRes.U;
        aBestRootV = aNewtonRes.V;
        aConverged = true;
      }

      // Fallback if no Newton converged: use the best grid corner directly
      if (!aConverged)
      {
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

        aBestRootU = myGrid(aBestI, aBestJ).U;
        aBestRootV = myGrid(aBestI, aBestJ).V;

        // Check if gradient is near zero (valid extremum at grid point)
        // Use Precision::Confusion() as base, NOT theTol which is distance tolerance
        double aFu, aFv;
        aFunc.Value(aBestRootU, aBestRootV, aFu, aFv);
        double aFNorm = std::sqrt(aFu * aFu + aFv * aFv);
        if (aFNorm < Precision::Confusion() * ExtremaPS::THE_GRADIENT_TOL_FACTOR)
        {
          aConverged = true;
        }
      }

      double aRootU = aBestRootU;
      double aRootV = aBestRootV;

      if (!aConverged)
        continue;

      gp_Pnt aPt     = theSurface.Value(aRootU, aRootV);
      double aSqDist = theP.SquareDistance(aPt);

      // Check for duplicate using spatial distance (not parameter space)
      bool aSkip = false;
      for (int r = 0; r < myFoundRoots.Length(); ++r)
      {
        gp_Pnt aPrevPt = theSurface.Value(myFoundRoots.Value(r).first, myFoundRoots.Value(r).second);
        if (aPt.SquareDistance(aPrevPt) < theTol * theTol)
        {
          aSkip = true;
          break;
        }
      }
      if (aSkip)
        continue;

      // Classify as min or max using second derivative with degeneracy check
      double aFu, aFv, aDFuu, aDFuv, aDFvv;
      aFunc.ValueAndJacobian(aRootU, aRootV, aFu, aFv, aDFuu, aDFuv, aDFvv);

      // Compute Hessian determinant and check for degeneracy
      const double aDet = aDFuu * aDFvv - aDFuv * aDFuv;
      const double aHessianScale = std::max(std::abs(aDFuu) * std::abs(aDFvv),
                                            MathUtils::THE_HESSIAN_DEGENERACY_ABS);
      const bool aIsDegenerate = std::abs(aDet) < MathUtils::THE_HESSIAN_DEGENERACY_REL * aHessianScale;

      // Skip degenerate critical points (saddles or flat regions)
      if (aIsDegenerate || aDet < 0.0)
        continue;

      bool aIsMin = (aDFuu > 0.0 && aDet > 0.0);

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
  //! Uses cached min indices from scanGrid to avoid rescanning.
  void addGridMinFallback(const Adaptor3d_Surface&   theSurface,
                          const gp_Pnt&              theP,
                          const ExtremaPS::Domain2D& theDomain,
                          double                     theTol,
                          ExtremaPS::SearchMode      theMode) const
  {
    if (theMode != ExtremaPS::SearchMode::Min && theMode != ExtremaPS::SearchMode::MinMax)
      return;

    // Use cached min indices from scanGrid (no rescan needed)
    const double aGridMinU    = myGrid(myMinI, myMinJ).U;
    const double aGridMinV    = myGrid(myMinI, myMinJ).V;
    const double aGridMinDist = myMinDist;

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
    gp_Pnt aRefinedPt   = myGrid(myMinI, myMinJ).Point;
    double aRefinedDist = aGridMinDist;

    if (aNewtonRes.IsDone)
    {
      // Don't clamp - Newton already handles soft bounds
      // This allows solutions slightly outside domain if gradient is near zero
      aRefinedU    = aNewtonRes.U;
      aRefinedV    = aNewtonRes.V;
      aRefinedPt   = theSurface.Value(aRefinedU, aRefinedV);
      aRefinedDist = theP.SquareDistance(aRefinedPt);
    }

    double aResultMinDist = myResult.Extrema.IsEmpty() ? std::numeric_limits<double>::max()
                                                       : myResult.MinSquareDistance();

    if (aRefinedDist < aResultMinDist * ExtremaPS::THE_REFINED_DIST_THRESHOLD)
    {
      // Check for duplicate
      bool aSkip = false;
      for (int i = 0; i < myResult.Extrema.Length(); ++i)
      {
        const auto& anExisting = myResult.Extrema.Value(i);
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
        myResult.Extrema.Append(anExt);
      }
    }
  }

private:
  NCollection_Array2<GridPoint> myGrid; //!< Cached grid

  // Mutable cached temporaries (reused via Clear())
  mutable ExtremaPS::Result                             myResult;     //!< Reusable result
  mutable NCollection_Vector<Candidate>                 myCandidates; //!< Candidates from grid scan
  mutable NCollection_Vector<std::pair<double, double>> myFoundRoots; //!< Found roots for dedup
  mutable NCollection_Vector<SortEntry>     mySortedEntries;          //!< Sorted candidate indices
  mutable NCollection_Array2<GridPointData> myGridData;               //!< Pre-computed Fu/Fv/Dist

  //! @brief Try to find extremum starting from cached solution (spatial coherence optimization).
  //!
  //! Uses ring buffer of last 3 solutions. When 3+ points are available,
  //! attempts trajectory prediction (linear extrapolation) for better initial guess.
  //!
  //! This is the fast path for coherent scanning (sequential points).
  //! Falls back to full grid scan if:
  //! - No cache available
  //! - Query point not spatially close to cached queries
  //! - Newton doesn't converge
  //! - Validation against grid corners fails
  //!
  //! @return true if cached solution produced a valid result, false to fall back to grid scan
  bool tryFromCachedSolution(const Adaptor3d_Surface&   theSurface,
                             const gp_Pnt&              theP,
                             const ExtremaPS::Domain2D& theDomain,
                             double                     theTol,
                             ExtremaPS::SearchMode      theMode) const
  {
    if (myCacheCount == 0)
      return false;

    // Find nearest cached solution (not just most recent)
    int    aNearestIdx  = (myCacheIndex + ExtremaPS::THE_CACHE_SIZE - 1) % ExtremaPS::THE_CACHE_SIZE;
    double aNearestDist = theP.SquareDistance(myCachedSolutions[aNearestIdx].QueryPoint);

    for (int i = 1; i < myCacheCount; ++i)
    {
      int    aIdx  = (myCacheIndex + ExtremaPS::THE_CACHE_SIZE - 1 - i) % ExtremaPS::THE_CACHE_SIZE;
      double aDist = theP.SquareDistance(myCachedSolutions[aIdx].QueryPoint);
      if (aDist < aNearestDist)
      {
        aNearestDist = aDist;
        aNearestIdx  = aIdx;
      }
    }

    if (aNearestDist > ExtremaPS::THE_COHERENCE_THRESHOLD_SQ)
      return false;

    const CachedSolution& aNearest = myCachedSolutions[aNearestIdx];

    // Compute starting point: use trajectory prediction if 3 points available
    double aStartU = aNearest.U;
    double aStartV = aNearest.V;

    if (myCacheCount >= 3)
    {
      // Get 3 most recent points (in order: oldest, middle, newest)
      int aIdx2 = (myCacheIndex + ExtremaPS::THE_CACHE_SIZE - 1) % ExtremaPS::THE_CACHE_SIZE; // newest
      int aIdx1 = (myCacheIndex + ExtremaPS::THE_CACHE_SIZE - 2) % ExtremaPS::THE_CACHE_SIZE; // middle
      int aIdx0 = (myCacheIndex + ExtremaPS::THE_CACHE_SIZE - 3) % ExtremaPS::THE_CACHE_SIZE; // oldest

      const CachedSolution& aS0 = myCachedSolutions[aIdx0];
      const CachedSolution& aS1 = myCachedSolutions[aIdx1];
      const CachedSolution& aS2 = myCachedSolutions[aIdx2];

      // Check if the 3 query points form a roughly linear trajectory
      gp_Vec aV01(aS0.QueryPoint, aS1.QueryPoint);
      gp_Vec aV12(aS1.QueryPoint, aS2.QueryPoint);
      double aMag01 = aV01.Magnitude();
      double aMag12 = aV12.Magnitude();

      // Only predict if steps are similar in magnitude (ratio within acceptable range)
      if (aMag01 > Precision::Confusion() && aMag12 > Precision::Confusion())
      {
        double aRatio = aMag12 / aMag01;
        if (aRatio > ExtremaPS::THE_TRAJECTORY_MIN_RATIO
            && aRatio < ExtremaPS::THE_TRAJECTORY_MAX_RATIO)
        {
          // Compute cosine of angle between consecutive steps
          double aCos = aV01.Dot(aV12) / (aMag01 * aMag12);
          if (aCos > ExtremaPS::THE_TRAJECTORY_MIN_COS) // Roughly same direction (< ~45 degrees)
          {
            // Use quadratic extrapolation if 4+ points available and trajectory is smooth
            // Quadratic: U_next = 3*U2 - 3*U1 + U0 (includes velocity + acceleration)
            if (myCacheCount >= 4 && aCos > ExtremaPS::THE_TRAJECTORY_QUADRATIC_COS)
            {
              aStartU = 3.0 * aS2.U - 3.0 * aS1.U + aS0.U;
              aStartV = 3.0 * aS2.V - 3.0 * aS1.V + aS0.V;
            }
            else
            {
              // Linear extrapolation for 3 points or lower coherence
              double aDeltaU = aS2.U - aS1.U;
              double aDeltaV = aS2.V - aS1.V;
              aStartU        = aS2.U + aDeltaU;
              aStartV        = aS2.V + aDeltaV;
            }
            // Clamp to domain
            aStartU = std::max(theDomain.UMin, std::min(theDomain.UMax, aStartU));
            aStartV = std::max(theDomain.VMin, std::min(theDomain.VMax, aStartV));
          }
        }
      }
    }

    // Try Newton from predicted/cached starting point
    ExtremaPS_DistanceFunction aFunc(theSurface, theP);
    ExtremaPS_Newton::Result   aNewtonRes = ExtremaPS_Newton::Solve(aFunc,
                                                                  aStartU,
                                                                  aStartV,
                                                                  theDomain.UMin,
                                                                  theDomain.UMax,
                                                                  theDomain.VMin,
                                                                  theDomain.VMax,
                                                                  theTol);

    if (!aNewtonRes.IsDone)
      return false;

    double aRootU = aNewtonRes.U;
    double aRootV = aNewtonRes.V;

    // Verify the solution is valid (gradient near zero)
    // Use Precision::Confusion() as base, NOT theTol which is distance tolerance
    double aFu, aFv;
    aFunc.Value(aRootU, aRootV, aFu, aFv);
    double aFNorm   = aFu * aFu + aFv * aFv;
    double aGradTol = Precision::Confusion() * ExtremaPS::THE_GRADIENT_TOL_FACTOR;
    if (aFNorm > aGradTol * aGradTol)
      return false;

    // Valid solution found - compute point and distance
    gp_Pnt aPt     = theSurface.Value(aRootU, aRootV);
    double aSqDist = theP.SquareDistance(aPt);

    // Classify as min or max using second derivative with degeneracy check
    double aDFuu, aDFuv, aDFvv;
    aFunc.ValueAndJacobian(aRootU, aRootV, aFu, aFv, aDFuu, aDFuv, aDFvv);

    // Compute Hessian determinant and check for degeneracy
    const double aDet = aDFuu * aDFvv - aDFuv * aDFuv;
    const double aHessianScale = std::max(std::abs(aDFuu) * std::abs(aDFvv),
                                          MathUtils::THE_HESSIAN_DEGENERACY_ABS);
    const bool aIsDegenerate = std::abs(aDet) < MathUtils::THE_HESSIAN_DEGENERACY_REL * aHessianScale;

    // Reject degenerate critical points (saddles or flat regions)
    if (aIsDegenerate || aDet < 0.0)
      return false;

    bool aIsMin = (aDFuu > 0.0 && aDet > 0.0);

    // Filter by search mode
    if (theMode == ExtremaPS::SearchMode::Min && !aIsMin)
      return false;
    if (theMode == ExtremaPS::SearchMode::Max && aIsMin)
      return false;

    // CRITICAL VALIDATION: Check if cache result is reasonable.
    // For coherent scanning (sequential nearby points), if trajectory prediction
    // is correct, the result should be close to the surface. If Newton converged
    // to a far local minimum, fall back to grid search for the global minimum.
    // Use tolerance squared as sanity threshold - solutions with significantly
    // larger distance indicate wrong convergence basin.
    const double aSanityThresholdSq = theTol * theTol * ExtremaPS::THE_CACHE_SANITY_FACTOR;
    if (theMode == ExtremaPS::SearchMode::Min && aSqDist > aSanityThresholdSq)
      return false; // Cache result too far - likely wrong local minimum

    ExtremaPS::ExtremumResult anExt;
    anExt.U              = aRootU;
    anExt.V              = aRootV;
    anExt.Point          = aPt;
    anExt.SquareDistance = aSqDist;
    anExt.IsMinimum      = aIsMin;
    myResult.Extrema.Append(anExt);
    myResult.Status = ExtremaPS::Status::OK;

    // Update cache with new solution
    addToCache(theP, aRootU, aRootV);

    return true;
  }

  //! @brief Add a solution to the ring buffer cache.
  void addToCache(const gp_Pnt& theP, double theU, double theV) const
  {
    myCachedSolutions[myCacheIndex].QueryPoint = theP;
    myCachedSolutions[myCacheIndex].U          = theU;
    myCachedSolutions[myCacheIndex].V          = theV;
    myCacheIndex                               = (myCacheIndex + 1) % ExtremaPS::THE_CACHE_SIZE;
    if (myCacheCount < ExtremaPS::THE_CACHE_SIZE)
      ++myCacheCount;
  }

  //! @brief Update the solution cache with the best result.
  void updateSolutionCache(const gp_Pnt& theP) const
  {
    if (myResult.Extrema.IsEmpty())
      return;

    // Find the minimum distance result
    double aBestDist = std::numeric_limits<double>::max();
    int    aBestIdx  = 0;
    for (int i = 0; i < myResult.Extrema.Length(); ++i)
    {
      if (myResult.Extrema.Value(i).SquareDistance < aBestDist)
      {
        aBestDist = myResult.Extrema.Value(i).SquareDistance;
        aBestIdx  = i;
      }
    }

    addToCache(theP, myResult.Extrema.Value(aBestIdx).U, myResult.Extrema.Value(aBestIdx).V);
  }

  // Cached global min/max indices from last scanGrid
  mutable int    myMinI = 0, myMinJ = 0; //!< Grid indices of global minimum
  mutable int    myMaxI = 0, myMaxJ = 0; //!< Grid indices of global maximum
  mutable double myMinDist = 0.0;        //!< Global minimum squared distance
  mutable double myMaxDist = 0.0;        //!< Global maximum squared distance

  // Cached solutions for spatial coherence optimization (ring buffer)
  // Uses centralized constant from ExtremaPS.hxx
  struct CachedSolution
  {
    gp_Pnt QueryPoint;
    double U = 0.0;
    double V = 0.0;
  };

  mutable CachedSolution myCachedSolutions[ExtremaPS::THE_CACHE_SIZE]; //!< Ring buffer of cached solutions
  mutable int            myCacheCount = 0;                             //!< Number of valid entries
  mutable int            myCacheIndex = 0;                             //!< Next write index (circular)
};

#endif // _ExtremaPS_GridEvaluator_HeaderFile
