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

#ifndef _ExtremaCC_GridEvaluator2D_HeaderFile
#define _ExtremaCC_GridEvaluator2D_HeaderFile

#include <ExtremaCC.hxx>
#include <ExtremaCC_DistanceFunction.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_Vector.hxx>
#include <Precision.hxx>

#include <algorithm>
#include <cmath>
#include <limits>

//! @brief Template grid-based curve-curve extrema computation class.
//!
//! Provides grid-based extrema finding algorithm for two curves.
//! The 2D grid samples (u1, u2) parameter space where:
//! - u1 varies along rows (first curve)
//! - u2 varies along columns (second curve)
//!
//! Algorithm:
//! 1. Build 2D grid of curve points and derivatives
//! 2. Scan grid to find candidate cells (gradient sign changes)
//! 3. Newton refinement on each candidate
//!
//! @tparam Curve1Eval Evaluator type for first curve (must have Value, D1, Domain)
//! @tparam Curve2Eval Evaluator type for second curve
template <typename Curve1Eval, typename Curve2Eval>
class ExtremaCC_GridEvaluator2D
{
public:
  //! Cached grid point with pre-computed data.
  struct CurvePoint
  {
    double U;     //!< Parameter value
    gp_Pnt Point; //!< Curve point C(u)
    gp_Vec D1;    //!< First derivative C'(u)
  };

  //! Candidate cell for Newton refinement.
  struct Candidate
  {
    int    Idx1;    //!< Index in first curve grid
    int    Idx2;    //!< Index in second curve grid
    double StartU1; //!< Starting u1 for Newton
    double StartU2; //!< Starting u2 for Newton
    double EstDist; //!< Estimated squared distance
    double GradMag; //!< Gradient magnitude (smaller = closer to extremum)
  };

  //! Constructor with two curve evaluators and domain.
  ExtremaCC_GridEvaluator2D(const Curve1Eval&           theCurve1,
                            const Curve2Eval&           theCurve2,
                            const ExtremaCC::Domain2D& theDomain)
      : myCurve1(theCurve1),
        myCurve2(theCurve2),
        myDomain(theDomain)
  {
  }

  //! @brief Perform extrema computation.
  //! @param theResult result storage
  //! @param theTol tolerance
  //! @param theMode search mode
  void Perform(ExtremaCC::Result&    theResult,
               double                theTol,
               ExtremaCC::SearchMode theMode) const
  {
    theResult.Clear();

    // Build grids
    buildGrids();

    // Full grid scan + refinement
    scanGrid(theTol, theMode);
    refineCandidates(theResult, theTol, theMode);
    addGridMinFallback(theResult, theTol, theMode);

    if (!theResult.Extrema.IsEmpty())
    {
      theResult.Status = ExtremaCC::Status::OK;
    }
  }

private:
  //! Pre-computed gradient data for a grid cell point.
  struct GridCellData
  {
    double F1;   //!< Gradient component F1 = (C1-C2).C1'
    double F2;   //!< Gradient component F2 = (C2-C1).C2'
    double Dist; //!< Squared distance ||C1-C2||^2
  };

  //! Entry for sorting candidates.
  struct SortEntry
  {
    int    Idx;
    double Dist;
    double GradMag;
  };

  //! @brief Build grids for both curves.
  void buildGrids() const
  {
    // Determine grid size based on domain extent
    constexpr int aNbSamples = 32;

    const double aU1Min  = myDomain.Curve1.Min;
    const double aU1Max  = myDomain.Curve1.Max;
    const double aU2Min  = myDomain.Curve2.Min;
    const double aU2Max  = myDomain.Curve2.Max;
    const double aStep1  = (aU1Max - aU1Min) / (aNbSamples - 1);
    const double aStep2  = (aU2Max - aU2Min) / (aNbSamples - 1);

    myGrid1.Resize(0, aNbSamples - 1, false);
    myGrid2.Resize(0, aNbSamples - 1, false);

    // Build grid 1
    for (int i = 0; i < aNbSamples; ++i)
    {
      double aU = aU1Min + i * aStep1;
      if (i == aNbSamples - 1) aU = aU1Max;

      gp_Pnt aPt;
      gp_Vec aD1;
      myCurve1.D1(aU, aPt, aD1);

      myGrid1(i).U     = aU;
      myGrid1(i).Point = aPt;
      myGrid1(i).D1    = aD1;
    }

    // Build grid 2
    for (int i = 0; i < aNbSamples; ++i)
    {
      double aU = aU2Min + i * aStep2;
      if (i == aNbSamples - 1) aU = aU2Max;

      gp_Pnt aPt;
      gp_Vec aD1;
      myCurve2.D1(aU, aPt, aD1);

      myGrid2(i).U     = aU;
      myGrid2(i).Point = aPt;
      myGrid2(i).D1    = aD1;
    }
  }

  //! @brief Scan 2D grid to find candidate cells for extrema.
  void scanGrid(double theTol, ExtremaCC::SearchMode theMode) const
  {
    myCandidates.Clear();

    const int aNb1 = myGrid1.Length();
    const int aNb2 = myGrid2.Length();

    if (aNb1 < 2 || aNb2 < 2)
    {
      return;
    }

    // Tolerance for near-zero gradient detection
    const double aTolF   = theTol * ExtremaCC::THE_NEAR_ZERO_GRADIENT_FACTOR;
    const double aTolFSq = aTolF * aTolF;

    // Pre-compute gradient data for all grid points
    myGridData.Resize(0, aNb1 - 1, 0, aNb2 - 1, false);

    // Reset global min/max tracking
    myMinDist = std::numeric_limits<double>::max();
    myMaxDist = -std::numeric_limits<double>::max();
    myMinI = myMinJ = myMaxI = myMaxJ = 0;
    myHasZeroDist = false;

    // Tolerance for zero-distance detection (intersection)
    const double aZeroDistTol = theTol * theTol;

    for (int i = 0; i < aNb1; ++i)
    {
      const CurvePoint& aCP1 = myGrid1(i);

      for (int j = 0; j < aNb2; ++j)
      {
        const CurvePoint& aCP2 = myGrid2(j);

        // Vector from C2 to C1
        const double aDx = aCP1.Point.X() - aCP2.Point.X();
        const double aDy = aCP1.Point.Y() - aCP2.Point.Y();
        const double aDz = aCP1.Point.Z() - aCP2.Point.Z();

        GridCellData& aData = myGridData(i, j);

        // F1 = (C1-C2).C1'
        aData.F1 = aDx * aCP1.D1.X() + aDy * aCP1.D1.Y() + aDz * aCP1.D1.Z();

        // F2 = (C2-C1).C2' = -(C1-C2).C2'
        aData.F2 = -(aDx * aCP2.D1.X() + aDy * aCP2.D1.Y() + aDz * aCP2.D1.Z());

        // Squared distance
        aData.Dist = aDx * aDx + aDy * aDy + aDz * aDz;

        // Track global min/max
        if (aData.Dist < myMinDist)
        {
          myMinDist = aData.Dist;
          myMinI    = i;
          myMinJ    = j;

          // Early detection of intersection (zero distance)
          if (aData.Dist < aZeroDistTol && theMode != ExtremaCC::SearchMode::Max)
          {
            myHasZeroDist = true;
          }
        }
        if (aData.Dist > myMaxDist)
        {
          myMaxDist = aData.Dist;
          myMaxI    = i;
          myMaxJ    = j;
        }
      }
    }

    // Early termination: if zero-distance found and searching for minimum only,
    // skip the full cell scan and add the intersection point directly
    if (myHasZeroDist && theMode == ExtremaCC::SearchMode::Min)
    {
      Candidate aCand;
      aCand.Idx1    = std::max(0, std::min(aNb1 - 2, myMinI));
      aCand.Idx2    = std::max(0, std::min(aNb2 - 2, myMinJ));
      aCand.StartU1 = myGrid1(myMinI).U;
      aCand.StartU2 = myGrid2(myMinJ).U;
      aCand.EstDist = myMinDist;
      aCand.GradMag = 0.0;
      myCandidates.Append(aCand);
      return; // Skip full cell scan - we found the minimum (intersection)
    }

    // Scan cells for sign changes in gradient
    for (int i = 0; i < aNb1 - 1; ++i)
    {
      for (int j = 0; j < aNb2 - 1; ++j)
      {
        // Access pre-computed data for 4 corners
        const GridCellData& aD00 = myGridData(i, j);
        const GridCellData& aD10 = myGridData(i + 1, j);
        const GridCellData& aD01 = myGridData(i, j + 1);
        const GridCellData& aD11 = myGridData(i + 1, j + 1);

        // Compute squared gradient magnitudes
        const double aGrad00 = aD00.F1 * aD00.F1 + aD00.F2 * aD00.F2;
        const double aGrad10 = aD10.F1 * aD10.F1 + aD10.F2 * aD10.F2;
        const double aGrad01 = aD01.F1 * aD01.F1 + aD01.F2 * aD01.F2;
        const double aGrad11 = aD11.F1 * aD11.F1 + aD11.F2 * aD11.F2;

        // Compute min distance and min gradient magnitude
        double aMinDist = std::min({aD00.Dist, aD10.Dist, aD01.Dist, aD11.Dist});
        double aMinGrad = std::min({aGrad00, aGrad10, aGrad01, aGrad11});

        // Dynamic tolerance based on distance
        const double aTolRelSq = std::max(aTolFSq, aMinDist * 1.0e-4);

        // Check for near-zero gradient
        bool aNearZero =
          (aGrad00 < aTolRelSq) || (aGrad10 < aTolRelSq) || (aGrad01 < aTolRelSq) || (aGrad11 < aTolRelSq);

        bool aAddCandidate = aNearZero;

        if (!aAddCandidate)
        {
          // Check for sign changes in F1 (gradient in u1 direction)
          bool aF1SignChange = (aD00.F1 * aD10.F1 < 0.0) || (aD01.F1 * aD11.F1 < 0.0)
                               || (aD00.F1 * aD01.F1 < 0.0) || (aD10.F1 * aD11.F1 < 0.0);

          if (aF1SignChange)
          {
            // Check for sign changes in F2 (gradient in u2 direction)
            bool aF2SignChange = (aD00.F2 * aD10.F2 < 0.0) || (aD01.F2 * aD11.F2 < 0.0)
                                 || (aD00.F2 * aD01.F2 < 0.0) || (aD10.F2 * aD11.F2 < 0.0);
            aAddCandidate = aF2SignChange;
          }
        }

        if (aAddCandidate)
        {
          Candidate aCand;
          aCand.Idx1    = i;
          aCand.Idx2    = j;
          aCand.StartU1 = (myGrid1(i).U + myGrid1(i + 1).U) * 0.5;
          aCand.StartU2 = (myGrid2(j).U + myGrid2(j + 1).U) * 0.5;
          aCand.EstDist = aMinDist;
          aCand.GradMag = aMinGrad;
          myCandidates.Append(aCand);
        }
      }
    }

    // Add global minimum as candidate (always useful)
    if (theMode == ExtremaCC::SearchMode::Min || theMode == ExtremaCC::SearchMode::MinMax)
    {
      Candidate aCand;
      aCand.Idx1    = std::max(0, std::min(aNb1 - 2, myMinI));
      aCand.Idx2    = std::max(0, std::min(aNb2 - 2, myMinJ));
      aCand.StartU1 = myGrid1(myMinI).U;
      aCand.StartU2 = myGrid2(myMinJ).U;
      aCand.EstDist = myMinDist;
      aCand.GradMag = 0.0;
      myCandidates.Append(aCand);
    }

    // Add global maximum as candidate
    if (theMode == ExtremaCC::SearchMode::Max || theMode == ExtremaCC::SearchMode::MinMax)
    {
      Candidate aCand;
      aCand.Idx1    = std::max(0, std::min(aNb1 - 2, myMaxI));
      aCand.Idx2    = std::max(0, std::min(aNb2 - 2, myMaxJ));
      aCand.StartU1 = myGrid1(myMaxI).U;
      aCand.StartU2 = myGrid2(myMaxJ).U;
      aCand.EstDist = myMaxDist;
      aCand.GradMag = 0.0;
      myCandidates.Append(aCand);
    }
  }

  //! @brief Refine candidates using 2D Newton's method.
  void refineCandidates(ExtremaCC::Result&    theResult,
                        double                theTol,
                        ExtremaCC::SearchMode theMode) const
  {
    theResult.Status = ExtremaCC::Status::OK;
    myFoundRoots.Clear();
    mySortedEntries.Clear();

    ExtremaCC_DistanceFunction<Curve1Eval, Curve2Eval> aFunc(myCurve1, myCurve2);

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

    // Sort by distance (ascending for Min, descending for Max)
    if (theMode == ExtremaCC::SearchMode::Min)
    {
      std::sort(mySortedEntries.begin(),
                mySortedEntries.end(),
                [](const SortEntry& a, const SortEntry& b) {
                  if (std::abs(a.Dist - b.Dist) > 1.0e-10 * std::max(a.Dist, b.Dist))
                    return a.Dist < b.Dist;
                  return a.GradMag < b.GradMag;
                });
    }
    else if (theMode == ExtremaCC::SearchMode::Max)
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
      const SortEntry&  anEntry = mySortedEntries.Value(s);
      const Candidate& aCand   = myCandidates.Value(anEntry.Idx);

      // Newton bounds from grid cell (with small expansion)
      double aCellU1Min = myGrid1(aCand.Idx1).U;
      double aCellU1Max = myGrid1(aCand.Idx1 + 1).U;
      double aCellU2Min = myGrid2(aCand.Idx2).U;
      double aCellU2Max = myGrid2(aCand.Idx2 + 1).U;

      double aExpand1 = (aCellU1Max - aCellU1Min) * 0.1;
      double aExpand2 = (aCellU2Max - aCellU2Min) * 0.1;
      aCellU1Min      = std::max(myDomain.Curve1.Min, aCellU1Min - aExpand1);
      aCellU1Max      = std::min(myDomain.Curve1.Max, aCellU1Max + aExpand1);
      aCellU2Min      = std::max(myDomain.Curve2.Min, aCellU2Min - aExpand2);
      aCellU2Max      = std::min(myDomain.Curve2.Max, aCellU2Max + aExpand2);

      // Newton iteration
      ExtremaCC_Newton::Result aNewtonRes = ExtremaCC_Newton::Solve(aFunc,
                                                                     aCand.StartU1,
                                                                     aCand.StartU2,
                                                                     aCellU1Min,
                                                                     aCellU1Max,
                                                                     aCellU2Min,
                                                                     aCellU2Max,
                                                                     theTol);

      double aRootU1    = 0.0;
      double aRootU2    = 0.0;
      bool   aConverged = false;

      if (aNewtonRes.IsDone)
      {
        aRootU1    = std::max(myDomain.Curve1.Min, std::min(myDomain.Curve1.Max, aNewtonRes.U1));
        aRootU2    = std::max(myDomain.Curve2.Min, std::min(myDomain.Curve2.Max, aNewtonRes.U2));
        aConverged = true;
      }
      else
      {
        // Fallback: find best grid point in the candidate cell
        double aBestDist = std::numeric_limits<double>::max();
        int    aBestI1   = aCand.Idx1;
        int    aBestI2   = aCand.Idx2;

        for (int di = 0; di <= 1; ++di)
        {
          for (int dj = 0; dj <= 1; ++dj)
          {
            int    i     = aCand.Idx1 + di;
            int    j     = aCand.Idx2 + dj;
            double aDist = myGridData(i, j).Dist;
            if (aDist < aBestDist)
            {
              aBestDist = aDist;
              aBestI1   = i;
              aBestI2   = j;
            }
          }
        }

        aRootU1 = myGrid1(aBestI1).U;
        aRootU2 = myGrid2(aBestI2).U;

        // Try Newton again from best corner
        ExtremaCC_Newton::Result aRetryRes = ExtremaCC_Newton::Solve(aFunc,
                                                                      aRootU1,
                                                                      aRootU2,
                                                                      aCellU1Min,
                                                                      aCellU1Max,
                                                                      aCellU2Min,
                                                                      aCellU2Max,
                                                                      theTol * 10.0);

        if (aRetryRes.IsDone)
        {
          aRootU1    = std::max(myDomain.Curve1.Min, std::min(myDomain.Curve1.Max, aRetryRes.U1));
          aRootU2    = std::max(myDomain.Curve2.Min, std::min(myDomain.Curve2.Max, aRetryRes.U2));
          aConverged = true;
        }
        else
        {
          // Final fallback: check if gradient is near zero at best grid point
          double aF1, aF2;
          aFunc.Value(aRootU1, aRootU2, aF1, aF2);
          double aFNorm = std::sqrt(aF1 * aF1 + aF2 * aF2);
          if (aFNorm < theTol * 100.0)
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
        if (std::abs(aRootU1 - myFoundRoots.Value(r).first) < theTol
            && std::abs(aRootU2 - myFoundRoots.Value(r).second) < theTol)
        {
          aSkip = true;
          break;
        }
      }
      if (aSkip)
        continue;

      gp_Pnt aPt1    = myCurve1.Value(aRootU1);
      gp_Pnt aPt2    = myCurve2.Value(aRootU2);
      double aSqDist = aPt1.SquareDistance(aPt2);

      // Classify as min or max using second derivative (Hessian determinant)
      double aF1, aF2, aJ11, aJ12, aJ22;
      aFunc.ValueAndJacobian(aRootU1, aRootU2, aF1, aF2, aJ11, aJ12, aJ22);
      bool aIsMin = (aJ11 > 0.0 && aJ11 * aJ22 - aJ12 * aJ12 > 0.0);

      // Filter by search mode
      if (theMode == ExtremaCC::SearchMode::Min && !aIsMin)
        continue;
      if (theMode == ExtremaCC::SearchMode::Max && aIsMin)
        continue;

      ExtremaCC::ExtremumResult anExt;
      anExt.Parameter1     = aRootU1;
      anExt.Parameter2     = aRootU2;
      anExt.Point1         = aPt1;
      anExt.Point2         = aPt2;
      anExt.SquareDistance = aSqDist;
      anExt.IsMinimum      = aIsMin;
      theResult.Extrema.Append(anExt);

      myFoundRoots.Append(std::make_pair(aRootU1, aRootU2));
    }

    if (theResult.Extrema.IsEmpty() && myCandidates.IsEmpty())
    {
      theResult.Status = ExtremaCC::Status::NoSolution;
    }
  }

  //! @brief Add grid minimum fallback if no results found.
  void addGridMinFallback(ExtremaCC::Result&    theResult,
                          double                theTol,
                          ExtremaCC::SearchMode theMode) const
  {
    if (theMode != ExtremaCC::SearchMode::Min && theMode != ExtremaCC::SearchMode::MinMax)
      return;

    if (!theResult.Extrema.IsEmpty())
      return;

    // Use cached min from grid scan
    const double aGridMinU1   = myGrid1(myMinI).U;
    const double aGridMinU2   = myGrid2(myMinJ).U;
    const double aGridMinDist = myMinDist;

    // Try Newton refinement from the best grid point
    ExtremaCC_DistanceFunction<Curve1Eval, Curve2Eval> aFunc(myCurve1, myCurve2);
    ExtremaCC_Newton::Result aNewtonRes = ExtremaCC_Newton::Solve(aFunc,
                                                                   aGridMinU1,
                                                                   aGridMinU2,
                                                                   myDomain.Curve1.Min,
                                                                   myDomain.Curve1.Max,
                                                                   myDomain.Curve2.Min,
                                                                   myDomain.Curve2.Max,
                                                                   theTol);

    double aRefinedU1   = aGridMinU1;
    double aRefinedU2   = aGridMinU2;
    double aRefinedDist = aGridMinDist;

    if (aNewtonRes.IsDone)
    {
      aRefinedU1   = std::max(myDomain.Curve1.Min, std::min(myDomain.Curve1.Max, aNewtonRes.U1));
      aRefinedU2   = std::max(myDomain.Curve2.Min, std::min(myDomain.Curve2.Max, aNewtonRes.U2));
      aRefinedDist = aFunc.SquareDistance(aRefinedU1, aRefinedU2);
    }

    gp_Pnt aPt1 = myCurve1.Value(aRefinedU1);
    gp_Pnt aPt2 = myCurve2.Value(aRefinedU2);

    ExtremaCC::ExtremumResult anExt;
    anExt.Parameter1     = aRefinedU1;
    anExt.Parameter2     = aRefinedU2;
    anExt.Point1         = aPt1;
    anExt.Point2         = aPt2;
    anExt.SquareDistance = aRefinedDist;
    anExt.IsMinimum      = true;
    theResult.Extrema.Append(anExt);
    theResult.Status = ExtremaCC::Status::OK;
  }

private:
  const Curve1Eval&   myCurve1; //!< First curve evaluator
  const Curve2Eval&   myCurve2; //!< Second curve evaluator
  ExtremaCC::Domain2D myDomain; //!< Parameter domain

  // Mutable cached temporaries
  mutable NCollection_Array1<CurvePoint>              myGrid1;         //!< Cached grid for first curve
  mutable NCollection_Array1<CurvePoint>              myGrid2;         //!< Cached grid for second curve
  mutable NCollection_Vector<Candidate>               myCandidates;    //!< Candidates from grid
  mutable NCollection_Vector<std::pair<double, double>> myFoundRoots;  //!< Found roots for dedup
  mutable NCollection_Vector<SortEntry>               mySortedEntries; //!< Sorted candidates
  mutable NCollection_Array2<GridCellData>            myGridData;      //!< Pre-computed F1/F2/Dist

  // Cached global min/max indices from last scanGrid
  mutable int    myMinI = 0, myMinJ = 0;
  mutable int    myMaxI = 0, myMaxJ = 0;
  mutable double myMinDist = 0.0;
  mutable double myMaxDist = 0.0;
  mutable bool   myHasZeroDist = false; //!< True if intersection found during grid scan
};

#endif // _ExtremaCC_GridEvaluator2D_HeaderFile
