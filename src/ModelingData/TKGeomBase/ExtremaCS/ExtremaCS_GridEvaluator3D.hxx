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

#ifndef _ExtremaCS_GridEvaluator3D_HeaderFile
#define _ExtremaCS_GridEvaluator3D_HeaderFile

#include <ExtremaCS.hxx>
#include <gp_Vec.hxx>
#include <MathSys_Newton3D.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_Vector.hxx>
#include <Precision.hxx>

#include <algorithm>
#include <cmath>
#include <limits>

//! @brief Template grid-based curve-surface extrema computation class.
//!
//! Provides grid-based extrema finding algorithm for a curve and a surface.
//! The 3D grid samples (t, u, v) parameter space where:
//! - t varies along the curve
//! - (u, v) varies over the surface
//!
//! Algorithm:
//! 1. Build 1D grid of curve points and derivatives
//! 2. Build 2D grid of surface points and derivatives
//! 3. For each curve point, find candidate surface cells (gradient sign changes)
//! 4. Newton3DCurveSurface refinement on each candidate
//!
//! @tparam CurveEval Evaluator type for curve (must have Value, D1, D2, Domain)
//! @tparam SurfaceEval Evaluator type for surface (must have Value, D1, D2)
template <typename CurveEval, typename SurfaceEval>
class ExtremaCS_GridEvaluator3D
{
public:
  //! Default grid resolution for curve.
  static constexpr int THE_DEFAULT_CURVE_GRID = 32;

  //! Default grid resolution for surface (per direction).
  static constexpr int THE_DEFAULT_SURFACE_GRID = 20;

  //! Minimum grid resolution.
  static constexpr int THE_MIN_GRID_SIZE = 8;

  //! Maximum grid resolution.
  static constexpr int THE_MAX_GRID_SIZE = 128;

  //! Cached curve point with pre-computed data.
  struct CurvePoint
  {
    double T;     //!< Parameter value
    gp_Pnt Point; //!< Curve point C(t)
    gp_Vec D1;    //!< First derivative C'(t)
    gp_Vec D2;    //!< Second derivative C''(t)
  };

  //! Cached surface point with pre-computed data.
  struct SurfacePoint
  {
    double U, V;     //!< Parameter values
    gp_Pnt Point;    //!< Surface point S(u,v)
    gp_Vec D1U, D1V; //!< First derivatives
    gp_Vec D2UU, D2VV, D2UV; //!< Second derivatives
  };

  //! Candidate cell for Newton refinement.
  struct Candidate
  {
    int    CurveIdx;    //!< Index in curve grid
    int    SurfIdxU;    //!< U index in surface grid
    int    SurfIdxV;    //!< V index in surface grid
    double StartT;      //!< Starting t for Newton
    double StartU;      //!< Starting u for Newton
    double StartV;      //!< Starting v for Newton
    double EstDist;     //!< Estimated squared distance
    double GradMag;     //!< Gradient magnitude (smaller = closer to extremum)
  };

  //! Constructor with curve and surface evaluators.
  ExtremaCS_GridEvaluator3D(const CurveEval&           theCurve,
                            const SurfaceEval&         theSurface,
                            const ExtremaCS::Domain3D& theDomain)
      : myCurve(theCurve),
        mySurface(theSurface),
        myDomain(theDomain),
        myCurveGridSize(THE_DEFAULT_CURVE_GRID),
        mySurfaceGridSize(THE_DEFAULT_SURFACE_GRID)
  {
  }

  //! Constructor with custom grid resolution.
  ExtremaCS_GridEvaluator3D(const CurveEval&           theCurve,
                            const SurfaceEval&         theSurface,
                            const ExtremaCS::Domain3D& theDomain,
                            int                        theCurveGridSize,
                            int                        theSurfaceGridSize)
      : myCurve(theCurve),
        mySurface(theSurface),
        myDomain(theDomain),
        myCurveGridSize(std::clamp(theCurveGridSize, THE_MIN_GRID_SIZE, THE_MAX_GRID_SIZE)),
        mySurfaceGridSize(std::clamp(theSurfaceGridSize, THE_MIN_GRID_SIZE, THE_MAX_GRID_SIZE))
  {
  }

  //! @brief Perform extrema computation.
  //! @param theResult result storage
  //! @param theTol tolerance
  //! @param theMode search mode
  void Perform(ExtremaCS::Result&    theResult,
               double                theTol,
               ExtremaCS::SearchMode theMode) const
  {
    theResult.Clear();

    // Build grids
    buildCurveGrid();
    buildSurfaceGrid();

    // Full grid scan + refinement
    scanGrid(theTol, theMode);
    refineCandidates(theResult, theTol, theMode);
    addGridMinFallback(theResult, theTol, theMode);

    if (!theResult.Extrema.IsEmpty())
    {
      theResult.Status = ExtremaCS::Status::OK;
    }
  }

private:
  //! Pre-computed gradient data for a grid cell point.
  struct GridCellData
  {
    double F1;   //!< Gradient component F1 = (C-S).dC/dt
    double F2;   //!< Gradient component F2 = -(C-S).dS/du
    double F3;   //!< Gradient component F3 = -(C-S).dS/dv
    double Dist; //!< Squared distance ||C-S||^2
  };

  //! Entry for sorting candidates.
  struct SortEntry
  {
    int    Idx;
    double Dist;
    double GradMag;
  };

  //! @brief Build grid for curve.
  void buildCurveGrid() const
  {
    const int aNbSamples = myCurveGridSize;
    const double aTMin = myDomain.Curve.Min;
    const double aTMax = myDomain.Curve.Max;
    const double aStep = (aTMax - aTMin) / (aNbSamples - 1);

    myCurveGrid.Resize(0, aNbSamples - 1, false);

    for (int i = 0; i < aNbSamples; ++i)
    {
      double aT = aTMin + i * aStep;
      if (i == aNbSamples - 1) aT = aTMax;

      gp_Pnt aPt;
      gp_Vec aD1, aD2;
      myCurve.D2(aT, aPt, aD1, aD2);

      myCurveGrid(i).T     = aT;
      myCurveGrid(i).Point = aPt;
      myCurveGrid(i).D1    = aD1;
      myCurveGrid(i).D2    = aD2;
    }
  }

  //! @brief Build grid for surface.
  void buildSurfaceGrid() const
  {
    const int aNbU = mySurfaceGridSize;
    const int aNbV = mySurfaceGridSize;
    const double aUMin = myDomain.Surface.UMin;
    const double aUMax = myDomain.Surface.UMax;
    const double aVMin = myDomain.Surface.VMin;
    const double aVMax = myDomain.Surface.VMax;
    const double aStepU = (aUMax - aUMin) / (aNbU - 1);
    const double aStepV = (aVMax - aVMin) / (aNbV - 1);

    mySurfaceGrid.Resize(0, aNbU - 1, 0, aNbV - 1, false);

    for (int i = 0; i < aNbU; ++i)
    {
      double aU = aUMin + i * aStepU;
      if (i == aNbU - 1) aU = aUMax;

      for (int j = 0; j < aNbV; ++j)
      {
        double aV = aVMin + j * aStepV;
        if (j == aNbV - 1) aV = aVMax;

        gp_Pnt aPt;
        gp_Vec aD1U, aD1V, aD2UU, aD2VV, aD2UV;
        mySurface.D2(aU, aV, aPt, aD1U, aD1V, aD2UU, aD2VV, aD2UV);

        SurfacePoint& aSP = mySurfaceGrid(i, j);
        aSP.U    = aU;
        aSP.V    = aV;
        aSP.Point = aPt;
        aSP.D1U   = aD1U;
        aSP.D1V   = aD1V;
        aSP.D2UU  = aD2UU;
        aSP.D2VV  = aD2VV;
        aSP.D2UV  = aD2UV;
      }
    }
  }

  //! @brief Scan 3D grid to find candidate cells for extrema.
  void scanGrid(double theTol, ExtremaCS::SearchMode theMode) const
  {
    myCandidates.Clear();

    const int aNbT = myCurveGrid.Length();
    const int aNbU = mySurfaceGrid.UpperRow() - mySurfaceGrid.LowerRow() + 1;
    const int aNbV = mySurfaceGrid.UpperCol() - mySurfaceGrid.LowerCol() + 1;

    if (aNbT < 2 || aNbU < 2 || aNbV < 2)
    {
      return;
    }

    // Tolerance for near-zero gradient detection
    const double aTolF   = theTol * 10.0;
    const double aTolFSq = aTolF * aTolF;

    // Reset global min/max tracking
    myMinDist = std::numeric_limits<double>::max();
    myMaxDist = -std::numeric_limits<double>::max();
    myMinT = myMinU = myMinV = 0;
    myMaxT = myMaxU = myMaxV = 0;

    // Note: intersection detection removed - handled by separate intersection algorithms
    (void)theTol; // Used for grid Newton refinement below

    // For each curve point, find closest surface cell and check for extrema
    for (int iT = 0; iT < aNbT; ++iT)
    {
      const CurvePoint& aCP = myCurveGrid(iT);

      for (int iU = 0; iU < aNbU; ++iU)
      {
        for (int iV = 0; iV < aNbV; ++iV)
        {
          const SurfacePoint& aSP = mySurfaceGrid(iU, iV);

          // Vector D = C - S (from surface to curve)
          const double aDx = aCP.Point.X() - aSP.Point.X();
          const double aDy = aCP.Point.Y() - aSP.Point.Y();
          const double aDz = aCP.Point.Z() - aSP.Point.Z();

          // Squared distance
          const double aDist = aDx * aDx + aDy * aDy + aDz * aDz;

          // Track global min/max
          if (aDist < myMinDist)
          {
            myMinDist = aDist;
            myMinT = iT;
            myMinU = iU;
            myMinV = iV;
          }
          if (aDist > myMaxDist)
          {
            myMaxDist = aDist;
            myMaxT = iT;
            myMaxU = iU;
            myMaxV = iV;
          }

          // For interior cells, check for gradient sign changes
          if (iT < aNbT - 1 && iU < aNbU - 1 && iV < aNbV - 1)
          {
            // Gradient components at current point
            // F1 = D . dC/dt
            const double aF1 = aDx * aCP.D1.X() + aDy * aCP.D1.Y() + aDz * aCP.D1.Z();
            // F2 = -D . dS/du
            const double aF2 = -(aDx * aSP.D1U.X() + aDy * aSP.D1U.Y() + aDz * aSP.D1U.Z());
            // F3 = -D . dS/dv
            const double aF3 = -(aDx * aSP.D1V.X() + aDy * aSP.D1V.Y() + aDz * aSP.D1V.Z());

            const double aGradSq = aF1 * aF1 + aF2 * aF2 + aF3 * aF3;

            // Check for near-zero gradient
            if (aGradSq < aTolFSq)
            {
              addCandidate(iT, iU, iV, aDist, aGradSq);
              continue;
            }

            // Check sign changes in neighboring cells
            // This is a simplified 3D cell check - we look at immediate neighbors
            bool aHasSignChange = checkNeighborSignChanges(iT, iU, iV, aF1, aF2, aF3);
            if (aHasSignChange)
            {
              addCandidate(iT, iU, iV, aDist, aGradSq);
            }
          }
        }
      }
    }

    // Add global minimum as candidate
    if (theMode == ExtremaCS::SearchMode::Min || theMode == ExtremaCS::SearchMode::MinMax)
    {
      Candidate aCand;
      aCand.CurveIdx = myMinT;
      aCand.SurfIdxU = myMinU;
      aCand.SurfIdxV = myMinV;
      aCand.StartT   = myCurveGrid(myMinT).T;
      aCand.StartU   = mySurfaceGrid(myMinU, myMinV).U;
      aCand.StartV   = mySurfaceGrid(myMinU, myMinV).V;
      aCand.EstDist  = myMinDist;
      aCand.GradMag  = 0.0;
      myCandidates.Append(aCand);
    }

    // Add global maximum as candidate
    if (theMode == ExtremaCS::SearchMode::Max || theMode == ExtremaCS::SearchMode::MinMax)
    {
      Candidate aCand;
      aCand.CurveIdx = myMaxT;
      aCand.SurfIdxU = myMaxU;
      aCand.SurfIdxV = myMaxV;
      aCand.StartT   = myCurveGrid(myMaxT).T;
      aCand.StartU   = mySurfaceGrid(myMaxU, myMaxV).U;
      aCand.StartV   = mySurfaceGrid(myMaxU, myMaxV).V;
      aCand.EstDist  = myMaxDist;
      aCand.GradMag  = 0.0;
      myCandidates.Append(aCand);
    }
  }

  //! @brief Check if gradient sign changes in neighboring cells.
  bool checkNeighborSignChanges(int iT, int iU, int iV,
                                 double theF1, double theF2, double theF3) const
  {
    const int aNbT = myCurveGrid.Length();
    const int aNbU = mySurfaceGrid.UpperRow() - mySurfaceGrid.LowerRow() + 1;
    const int aNbV = mySurfaceGrid.UpperCol() - mySurfaceGrid.LowerCol() + 1;

    // Check T+1 neighbor
    if (iT + 1 < aNbT)
    {
      const CurvePoint& aCP = myCurveGrid(iT + 1);
      const SurfacePoint& aSP = mySurfaceGrid(iU, iV);
      const double aDx = aCP.Point.X() - aSP.Point.X();
      const double aDy = aCP.Point.Y() - aSP.Point.Y();
      const double aDz = aCP.Point.Z() - aSP.Point.Z();
      const double aF1 = aDx * aCP.D1.X() + aDy * aCP.D1.Y() + aDz * aCP.D1.Z();
      if (theF1 * aF1 < 0.0) return true;
    }

    // Check U+1 neighbor
    if (iU + 1 < aNbU)
    {
      const CurvePoint& aCP = myCurveGrid(iT);
      const SurfacePoint& aSP = mySurfaceGrid(iU + 1, iV);
      const double aDx = aCP.Point.X() - aSP.Point.X();
      const double aDy = aCP.Point.Y() - aSP.Point.Y();
      const double aDz = aCP.Point.Z() - aSP.Point.Z();
      const double aF2 = -(aDx * aSP.D1U.X() + aDy * aSP.D1U.Y() + aDz * aSP.D1U.Z());
      if (theF2 * aF2 < 0.0) return true;
    }

    // Check V+1 neighbor
    if (iV + 1 < aNbV)
    {
      const CurvePoint& aCP = myCurveGrid(iT);
      const SurfacePoint& aSP = mySurfaceGrid(iU, iV + 1);
      const double aDx = aCP.Point.X() - aSP.Point.X();
      const double aDy = aCP.Point.Y() - aSP.Point.Y();
      const double aDz = aCP.Point.Z() - aSP.Point.Z();
      const double aF3 = -(aDx * aSP.D1V.X() + aDy * aSP.D1V.Y() + aDz * aSP.D1V.Z());
      if (theF3 * aF3 < 0.0) return true;
    }

    return false;
  }

  //! @brief Add a candidate to the list.
  void addCandidate(int theT, int theU, int theV, double theDist, double theGradMag) const
  {
    // Check for duplicate
    for (int c = 0; c < myCandidates.Length(); ++c)
    {
      const Candidate& aExist = myCandidates.Value(c);
      if (aExist.CurveIdx == theT && aExist.SurfIdxU == theU && aExist.SurfIdxV == theV)
      {
        return; // Duplicate
      }
    }

    Candidate aCand;
    aCand.CurveIdx = theT;
    aCand.SurfIdxU = theU;
    aCand.SurfIdxV = theV;
    aCand.StartT   = myCurveGrid(theT).T;
    aCand.StartU   = mySurfaceGrid(theU, theV).U;
    aCand.StartV   = mySurfaceGrid(theU, theV).V;
    aCand.EstDist  = theDist;
    aCand.GradMag  = theGradMag;
    myCandidates.Append(aCand);
  }

  //! @brief Refine candidates using 3D Newton's method.
  void refineCandidates(ExtremaCS::Result&    theResult,
                        double                theTol,
                        ExtremaCS::SearchMode theMode) const
  {
    myFoundRoots.Clear();
    mySortedEntries.Clear();

    // Build sorted entries
    for (int c = 0; c < myCandidates.Length(); ++c)
    {
      const Candidate& aCand = myCandidates.Value(c);
      SortEntry anEntry;
      anEntry.Idx     = c;
      anEntry.Dist    = aCand.EstDist;
      anEntry.GradMag = aCand.GradMag;
      mySortedEntries.Append(anEntry);
    }

    // Sort by distance (ascending for Min, descending for Max)
    if (theMode == ExtremaCS::SearchMode::Min)
    {
      std::sort(mySortedEntries.begin(),
                mySortedEntries.end(),
                [](const SortEntry& a, const SortEntry& b) {
                  if (std::abs(a.Dist - b.Dist) > 1.0e-10 * std::max(a.Dist, b.Dist))
                    return a.Dist < b.Dist;
                  return a.GradMag < b.GradMag;
                });
    }
    else if (theMode == ExtremaCS::SearchMode::Max)
    {
      std::sort(mySortedEntries.begin(),
                mySortedEntries.end(),
                [](const SortEntry& a, const SortEntry& b) {
                  if (std::abs(a.Dist - b.Dist) > 1.0e-10 * std::max(a.Dist, b.Dist))
                    return a.Dist > b.Dist;
                  return a.GradMag < b.GradMag;
                });
    }

    for (int s = 0; s < mySortedEntries.Length(); ++s)
    {
      const SortEntry& anEntry = mySortedEntries.Value(s);
      const Candidate& aCand = myCandidates.Value(anEntry.Idx);

      // Newton bounds from grid cell (with small expansion)
      const int aNbT = myCurveGrid.Length();
      const int aNbU = mySurfaceGrid.UpperRow() - mySurfaceGrid.LowerRow() + 1;
      const int aNbV = mySurfaceGrid.UpperCol() - mySurfaceGrid.LowerCol() + 1;

      int iT0 = std::max(0, aCand.CurveIdx - 1);
      int iT1 = std::min(aNbT - 1, aCand.CurveIdx + 1);
      int iU0 = std::max(0, aCand.SurfIdxU - 1);
      int iU1 = std::min(aNbU - 1, aCand.SurfIdxU + 1);
      int iV0 = std::max(0, aCand.SurfIdxV - 1);
      int iV1 = std::min(aNbV - 1, aCand.SurfIdxV + 1);

      double aTMin = myCurveGrid(iT0).T;
      double aTMax = myCurveGrid(iT1).T;
      double aUMin = mySurfaceGrid(iU0, 0).U;
      double aUMax = mySurfaceGrid(iU1, 0).U;
      double aVMin = mySurfaceGrid(0, iV0).V;
      double aVMax = mySurfaceGrid(0, iV1).V;

      // Clamp to domain
      aTMin = std::max(myDomain.Curve.Min, aTMin);
      aTMax = std::min(myDomain.Curve.Max, aTMax);
      aUMin = std::max(myDomain.Surface.UMin, aUMin);
      aUMax = std::min(myDomain.Surface.UMax, aUMax);
      aVMin = std::max(myDomain.Surface.VMin, aVMin);
      aVMax = std::min(myDomain.Surface.VMax, aVMax);

      // Newton3DCurveSurface iteration
      MathSys::Newton3DResult aNewtonRes = MathSys::Newton3DCurveSurface(
        myCurve,
        mySurface,
        aCand.StartT,
        aCand.StartU,
        aCand.StartV,
        aTMin,
        aTMax,
        aUMin,
        aUMax,
        aVMin,
        aVMax,
        theTol * ExtremaCS::THE_NEWTON_FTOL_FACTOR,
        ExtremaCS::THE_MAX_NEWTON_ITERATIONS);

      double aRootT = 0.0;
      double aRootU = 0.0;
      double aRootV = 0.0;
      bool aConverged = false;

      if (aNewtonRes.IsDone())
      {
        aRootT = std::clamp(aNewtonRes.X1, myDomain.Curve.Min, myDomain.Curve.Max);
        aRootU = std::clamp(aNewtonRes.X2, myDomain.Surface.UMin, myDomain.Surface.UMax);
        aRootV = std::clamp(aNewtonRes.X3, myDomain.Surface.VMin, myDomain.Surface.VMax);
        aConverged = true;
      }
      else
      {
        // Fallback: use best grid point
        aRootT = aCand.StartT;
        aRootU = aCand.StartU;
        aRootV = aCand.StartV;

        // Check if gradient is reasonably small at grid point
        gp_Pnt aPtC = myCurve.Value(aRootT);
        gp_Pnt aPtS = mySurface.Value(aRootU, aRootV);

        gp_Vec aD1C;
        {
          gp_Pnt aTmpPt;
          myCurve.D1(aRootT, aTmpPt, aD1C);
        }
        gp_Vec aD1U, aD1V;
        {
          gp_Pnt aTmpPt;
          mySurface.D1(aRootU, aRootV, aTmpPt, aD1U, aD1V);
        }

        gp_Vec aD(aPtS, aPtC);
        double aF1 = aD.Dot(aD1C);
        double aF2 = -aD.Dot(aD1U);
        double aF3 = -aD.Dot(aD1V);
        double aFNorm = std::sqrt(aF1 * aF1 + aF2 * aF2 + aF3 * aF3);

        if (aFNorm < theTol * 100.0)
        {
          aConverged = true;
        }
      }

      if (!aConverged)
        continue;

      // Check for duplicate
      bool aSkip = false;
      for (int r = 0; r < myFoundRoots.Length(); ++r)
      {
        const auto& aRoot = myFoundRoots.Value(r);
        if (std::abs(aRootT - std::get<0>(aRoot)) < theTol
            && std::abs(aRootU - std::get<1>(aRoot)) < theTol
            && std::abs(aRootV - std::get<2>(aRoot)) < theTol)
        {
          aSkip = true;
          break;
        }
      }
      if (aSkip)
        continue;

      gp_Pnt aPtC = myCurve.Value(aRootT);
      gp_Pnt aPtS = mySurface.Value(aRootU, aRootV);
      double aSqDist = aPtC.SquareDistance(aPtS);

      // Classify as min or max using Hessian
      bool aIsMin = classifyExtremum(aRootT, aRootU, aRootV);

      // Filter by search mode
      if (theMode == ExtremaCS::SearchMode::Min && !aIsMin)
        continue;
      if (theMode == ExtremaCS::SearchMode::Max && aIsMin)
        continue;

      ExtremaCS::ExtremumResult anExt;
      anExt.ParameterOnCurve    = aRootT;
      anExt.ParameterOnSurfaceU = aRootU;
      anExt.ParameterOnSurfaceV = aRootV;
      anExt.PointOnCurve        = aPtC;
      anExt.PointOnSurface      = aPtS;
      anExt.SquareDistance      = aSqDist;
      anExt.IsMinimum           = aIsMin;
      theResult.Extrema.Append(anExt);

      myFoundRoots.Append(std::make_tuple(aRootT, aRootU, aRootV));
    }
  }

  //! @brief Classify extremum as minimum or maximum using Hessian.
  bool classifyExtremum(double theT, double theU, double theV) const
  {
    gp_Pnt aPtC, aPtS;
    gp_Vec aD1C, aD2C;
    gp_Vec aD1U, aD1V, aD2UU, aD2VV, aD2UV;

    myCurve.D2(theT, aPtC, aD1C, aD2C);
    mySurface.D2(theU, theV, aPtS, aD1U, aD1V, aD2UU, aD2VV, aD2UV);

    gp_Vec aD(aPtS, aPtC);

    // Hessian of D^2 (3x3 matrix)
    // H[0][0] = dF1/dt = dC/dt.dC/dt + D.d2C/dt2
    double aH00 = aD1C.Dot(aD1C) + aD.Dot(aD2C);

    // H[1][1] = dF2/du = dS/du.dS/du - D.d2S/du2
    double aH11 = aD1U.Dot(aD1U) - aD.Dot(aD2UU);

    // H[2][2] = dF3/dv = dS/dv.dS/dv - D.d2S/dv2
    double aH22 = aD1V.Dot(aD1V) - aD.Dot(aD2VV);

    // For minimum: H must be positive definite
    // Simplified check: all diagonal elements positive
    // (full check would require all principal minors > 0)
    return (aH00 > 0.0 && aH11 > 0.0 && aH22 > 0.0);
  }

  //! @brief Add grid minimum fallback if no results found.
  void addGridMinFallback(ExtremaCS::Result&    theResult,
                          double                theTol,
                          ExtremaCS::SearchMode theMode) const
  {
    if (theMode != ExtremaCS::SearchMode::Min && theMode != ExtremaCS::SearchMode::MinMax)
      return;

    if (!theResult.Extrema.IsEmpty())
      return;

    // Use cached min from grid scan
    const double aGridMinT = myCurveGrid(myMinT).T;
    const double aGridMinU = mySurfaceGrid(myMinU, myMinV).U;
    const double aGridMinV = mySurfaceGrid(myMinU, myMinV).V;

    // Try Newton refinement from the best grid point
    MathSys::Newton3DResult aNewtonRes = MathSys::Newton3DCurveSurface(
      myCurve,
      mySurface,
      aGridMinT,
      aGridMinU,
      aGridMinV,
      myDomain.Curve.Min,
      myDomain.Curve.Max,
      myDomain.Surface.UMin,
      myDomain.Surface.UMax,
      myDomain.Surface.VMin,
      myDomain.Surface.VMax,
      theTol * ExtremaCS::THE_NEWTON_FTOL_FACTOR,
      ExtremaCS::THE_MAX_NEWTON_ITERATIONS);

    double aRefinedT = aGridMinT;
    double aRefinedU = aGridMinU;
    double aRefinedV = aGridMinV;

    if (aNewtonRes.IsDone())
    {
      aRefinedT = std::clamp(aNewtonRes.X1, myDomain.Curve.Min, myDomain.Curve.Max);
      aRefinedU = std::clamp(aNewtonRes.X2, myDomain.Surface.UMin, myDomain.Surface.UMax);
      aRefinedV = std::clamp(aNewtonRes.X3, myDomain.Surface.VMin, myDomain.Surface.VMax);
    }

    gp_Pnt aPtC = myCurve.Value(aRefinedT);
    gp_Pnt aPtS = mySurface.Value(aRefinedU, aRefinedV);

    ExtremaCS::ExtremumResult anExt;
    anExt.ParameterOnCurve    = aRefinedT;
    anExt.ParameterOnSurfaceU = aRefinedU;
    anExt.ParameterOnSurfaceV = aRefinedV;
    anExt.PointOnCurve        = aPtC;
    anExt.PointOnSurface      = aPtS;
    anExt.SquareDistance      = aPtC.SquareDistance(aPtS);
    anExt.IsMinimum           = true;
    theResult.Extrema.Append(anExt);
    theResult.Status = ExtremaCS::Status::OK;
  }

private:
  const CurveEval&   myCurve;         //!< Curve evaluator
  const SurfaceEval& mySurface;       //!< Surface evaluator
  ExtremaCS::Domain3D myDomain;       //!< Parameter domain
  int myCurveGridSize;                //!< Curve grid resolution
  int mySurfaceGridSize;              //!< Surface grid resolution (per direction)

  // Mutable cached temporaries
  mutable NCollection_Array1<CurvePoint>   myCurveGrid;   //!< Cached grid for curve
  mutable NCollection_Array2<SurfacePoint> mySurfaceGrid; //!< Cached grid for surface
  mutable NCollection_Vector<Candidate>    myCandidates;  //!< Candidates from grid
  mutable NCollection_Vector<std::tuple<double, double, double>> myFoundRoots; //!< Found roots
  mutable NCollection_Vector<SortEntry>    mySortedEntries; //!< Sorted candidates

  // Cached global min/max indices from last scanGrid
  mutable int myMinT = 0, myMinU = 0, myMinV = 0;
  mutable int myMaxT = 0, myMaxU = 0, myMaxV = 0;
  mutable double myMinDist = 0.0;
  mutable double myMaxDist = 0.0;
};

#endif // _ExtremaCS_GridEvaluator3D_HeaderFile
