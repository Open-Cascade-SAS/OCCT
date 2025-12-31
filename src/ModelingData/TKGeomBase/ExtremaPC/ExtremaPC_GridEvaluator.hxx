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
#include <utility>

//! @brief Grid-based point-curve extrema computation class.
//!
//! Provides grid-based extrema finding algorithm with cached state for
//! optimal performance on repeated queries. Used by BSpline, Bezier,
//! Offset, and other curve evaluators.
//!
//! Algorithm:
//! 1. Build grid of (parameter, point, D1) from GeomGridEval
//! 2. Linear scan of grid to find candidate intervals (sign changes in F(u))
//! 3. Newton refinement on each candidate
//! 4. Optional endpoint handling
//!
//! All temporary vectors are stored as mutable fields and reused via Clear()
//! to avoid repeated heap allocations.
class ExtremaPC_GridEvaluator
{
public:
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

  //! Default constructor.
  ExtremaPC_GridEvaluator() = default;

  //! @brief Build grid from GeomGridEval D1 results.
  //!
  //! @tparam GridEval type with EvaluateGridD1(params) method
  //! @param theEval grid evaluator
  //! @param theParams parameter values (math_Vector with Array1() accessor)
  template <typename GridEval>
  void BuildGrid(GridEval& theEval, const math_Vector& theParams)
  {
    // Use Array1() accessor to pass to GeomGridEval which expects NCollection_Array1
    NCollection_Array1<GeomGridEval::CurveD1> aD1Grid = theEval.EvaluateGridD1(theParams.Array1());

    const int aNbParams = theParams.Length();

    // Resize grid if needed
    if (myGrid.Size() != aNbParams)
    {
      myGrid = NCollection_Array1<GridPoint>(0, aNbParams - 1);
    }

    for (int i = 0; i < aNbParams; ++i)
    {
      const int aD1Idx = aD1Grid.Lower() + i;

      myGrid[i].Param = theParams(theParams.Lower() + i);
      myGrid[i].Point = aD1Grid.Value(aD1Idx).Point;
      myGrid[i].D1    = aD1Grid.Value(aD1Idx).D1;
    }
  }

  //! Returns the cached grid.
  const NCollection_Array1<GridPoint>& Grid() const { return myGrid; }

  //! Returns mutable reference to the result for post-processing.
  ExtremaPC::Result& Result() const { return myResult; }

  //! @brief Perform extrema computation using cached grid (interior only).
  //!
  //! @param theCurve curve adaptor
  //! @param theP query point
  //! @param theDomain parameter domain
  //! @param theTol tolerance
  //! @param theMode search mode
  //! @return const reference to result with interior extrema only
  [[nodiscard]] const ExtremaPC::Result& Perform(const Adaptor3d_Curve&     theCurve,
                                                 const gp_Pnt&              theP,
                                                 const ExtremaPC::Domain1D& theDomain,
                                                 double                     theTol,
                                                 ExtremaPC::SearchMode      theMode) const
  {
    myResult.Clear();
    scanGrid(theP, theTol, theMode);
    refineCandidates(theCurve, theP, theDomain, theTol, theMode);

    if (!myResult.Extrema.IsEmpty())
    {
      myResult.Status = ExtremaPC::Status::OK;
    }
    return myResult;
  }

  //! @brief Build uniform parameter grid.
  //! @return math_Vector with 1-based indexing
  static math_Vector BuildUniformParams(double theUMin, double theUMax, int theNbSamples)
  {
    math_Vector  aParams(1, theNbSamples);
    const double aStep = (theUMax - theUMin) / (theNbSamples - 1);

    for (int i = 1; i <= theNbSamples; ++i)
    {
      aParams(i) = theUMin + (i - 1) * aStep;
    }
    aParams(theNbSamples) = theUMax; // Ensure exact endpoint

    return aParams;
  }

private:
  //! @brief Scan grid to find candidate intervals for extrema.
  void scanGrid(const gp_Pnt& theP, double theTol, ExtremaPC::SearchMode theMode) const
  {
    myCandidates.Clear();
    const int aNbGrid = myGrid.Size();

    if (aNbGrid < 2)
    {
      return;
    }

    // Resize processed array if needed
    if (myProcessed.Size() != aNbGrid)
    {
      myProcessed = NCollection_Array1<bool>(0, aNbGrid - 1);
    }
    myProcessed.Init(false);

    double aPrevF     = 0.0;
    double aPrevDist  = 0.0;
    bool   aPrevValid = false;

    for (int i = 0; i < aNbGrid; ++i)
    {
      const GridPoint& aGP = myGrid[i];

      // Compute distance function value: F(u) = (C(u) - P) . C'(u)
      gp_Vec aVec(theP, aGP.Point);
      double aF    = aVec.Dot(aGP.D1);
      double aDist = aVec.SquareMagnitude();

      // Check for sign change with previous point
      if (aPrevValid && aPrevF * aF < 0.0 && !myProcessed[i - 1])
      {
        Candidate aCand;
        aCand.Type  = CandidateType::SignChange;
        aCand.IdxLo = i - 1;
        aCand.IdxHi = i;
        // Use linear interpolation for better starting point (secant method)
        double aFLo  = aPrevF;
        double aFHi  = aF;
        double aULo  = myGrid[i - 1].Param;
        double aUHi  = aGP.Param;
        aCand.StartU = aULo - aFLo * (aUHi - aULo) / (aFHi - aFLo);
        myCandidates.Append(aCand);
        myProcessed[i - 1] = true;
        myProcessed[i]     = true;
      }

      // Check for near-zero F (direct hit on extremum)
      if (std::abs(aF) < theTol * ExtremaPC::THE_NEAR_ZERO_F_FACTOR && !myProcessed[i])
      {
        Candidate aCand;
        aCand.Type   = CandidateType::NearZero;
        aCand.IdxLo  = i;
        aCand.IdxHi  = i;
        aCand.StartU = aGP.Param;
        myCandidates.Append(aCand);
        myProcessed[i] = true;
      }

      // Check for local extremum by distance comparison (3-point test)
      if (i > 0 && i < aNbGrid - 1 && !myProcessed[i])
      {
        double aNextDist = theP.SquareDistance(myGrid[i + 1].Point);

        // Local minimum: distance decreases then increases
        bool aIsLocalMin = (aDist <= aPrevDist && aDist <= aNextDist);
        // Local maximum: distance increases then decreases
        bool aIsLocalMax = (aDist >= aPrevDist && aDist >= aNextDist);

        if ((theMode == ExtremaPC::SearchMode::Min || theMode == ExtremaPC::SearchMode::MinMax)
            && aIsLocalMin)
        {
          Candidate aCand;
          aCand.Type   = CandidateType::NearZero;
          aCand.IdxLo  = i;
          aCand.IdxHi  = i;
          aCand.StartU = aGP.Param;
          myCandidates.Append(aCand);
          myProcessed[i] = true;
        }
        else if ((theMode == ExtremaPC::SearchMode::Max || theMode == ExtremaPC::SearchMode::MinMax)
                 && aIsLocalMax && !aIsLocalMin)
        {
          Candidate aCand;
          aCand.Type   = CandidateType::NearZero;
          aCand.IdxLo  = i;
          aCand.IdxHi  = i;
          aCand.StartU = aGP.Param;
          myCandidates.Append(aCand);
          myProcessed[i] = true;
        }
      }

      aPrevF     = aF;
      aPrevDist  = aDist;
      aPrevValid = true;
    }
  }

  //! @brief Refine candidates using Newton's method.
  void refineCandidates(const Adaptor3d_Curve&     theCurve,
                        const gp_Pnt&              theP,
                        const ExtremaPC::Domain1D& theDomain,
                        double                     theTol,
                        ExtremaPC::SearchMode      theMode) const
  {
    myResult.Status = ExtremaPC::Status::OK;
    myFoundRoots.Clear();
    mySortedIndices.Clear();

    ExtremaPC_DistanceFunction aFunc(theCurve, theP);

    // Newton configuration
    MathUtils::Config aConfig;
    aConfig.XTolerance    = theTol * ExtremaPC::THE_NEWTON_XTOL_FACTOR;
    aConfig.FTolerance    = theTol * ExtremaPC::THE_NEWTON_FTOL_FACTOR;
    aConfig.MaxIterations = ExtremaPC::THE_MAX_NEWTON_ITERATIONS;

    // Build sorted indices by estimated distance
    for (int c = 0; c < myCandidates.Length(); ++c)
    {
      const Candidate& aCand     = myCandidates.Value(c);
      double           anEstDist = theP.SquareDistance(myGrid[aCand.IdxLo].Point);
      mySortedIndices.Append(std::make_pair(c, anEstDist));
    }

    // Sort by estimated distance for Min mode (ascending), Max mode (descending)
    if (theMode == ExtremaPC::SearchMode::Min)
    {
      std::sort(mySortedIndices.begin(),
                mySortedIndices.end(),
                [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
                  return a.second < b.second;
                });
    }
    else if (theMode == ExtremaPC::SearchMode::Max)
    {
      std::sort(mySortedIndices.begin(),
                mySortedIndices.end(),
                [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
                  return a.second > b.second;
                });
    }

    // Best distance found so far (for early termination)
    double aBestSqDist = (theMode == ExtremaPC::SearchMode::Min)
                           ? std::numeric_limits<double>::max()
                           : -std::numeric_limits<double>::max();

    for (int s = 0; s < mySortedIndices.Length(); ++s)
    {
      int              c         = mySortedIndices.Value(s).first;
      double           anEstDist = mySortedIndices.Value(s).second;
      const Candidate& aCand     = myCandidates.Value(c);

      // Early termination: skip candidates that are clearly worse than the best found.
      // For Min mode: skip if estimated distance > best * (2.0 - threshold), i.e., ~1.1x best.
      // For Max mode: skip if estimated distance < best * threshold, i.e., ~0.9x best.
      constexpr double aMinSkipThreshold = 2.0 - ExtremaPC::THE_MAX_SKIP_THRESHOLD;
      if (theMode == ExtremaPC::SearchMode::Min && anEstDist > aBestSqDist * aMinSkipThreshold)
      {
        break;
      }
      if (theMode == ExtremaPC::SearchMode::Max
          && anEstDist < aBestSqDist * ExtremaPC::THE_MAX_SKIP_THRESHOLD)
      {
        break;
      }

      // Skip if too close to already found root
      bool aSkip = false;
      for (int r = 0; r < myFoundRoots.Length(); ++r)
      {
        if (std::abs(aCand.StartU - myFoundRoots.Value(r)) < theTol)
        {
          aSkip = true;
          break;
        }
      }
      if (aSkip)
      {
        continue;
      }

      // Determine Newton bounds
      double aULo, aUHi;
      if (aCand.Type == CandidateType::SignChange)
      {
        aULo = myGrid[aCand.IdxLo].Param;
        aUHi = myGrid[aCand.IdxHi].Param;
      }
      else
      {
        double aExpand = (theDomain.Max - theDomain.Min) * ExtremaPC::THE_INTERVAL_EXPAND_RATIO;
        aULo           = std::max(theDomain.Min, aCand.StartU - aExpand);
        aUHi           = std::min(theDomain.Max, aCand.StartU + aExpand);
      }

      // Try Newton refinement
      MathUtils::ScalarResult aNewtonRes =
        MathRoot::NewtonBounded(aFunc, aCand.StartU, aULo, aUHi, aConfig);

      double aRootU     = 0.0;
      bool   aConverged = false;

      if (aNewtonRes.IsDone())
      {
        aRootU     = std::max(theDomain.Min, std::min(theDomain.Max, *aNewtonRes.Root));
        aConverged = true;
      }
      else
      {
        // Try iterative grid refinement as fallback
        double aBestU    = aCand.StartU;
        double aBestDist = std::numeric_limits<double>::max();
        double aRefUMin  = aULo;
        double aRefUMax  = aUHi;

        for (int aPass = 0; aPass < ExtremaPC::THE_REFINEMENT_NB_PASSES; ++aPass)
        {
          const int    aNbSamples = ExtremaPC::THE_REFINEMENT_NB_SAMPLES;
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
          double aRangeHalf = (aRefUMax - aRefUMin) * ExtremaPC::THE_RANGE_NARROWING_FACTOR * 0.5;
          aRefUMin          = std::max(theDomain.Min, aBestU - aRangeHalf);
          aRefUMax          = std::min(theDomain.Max, aBestU + aRangeHalf);

          // Try Newton with refined point
          MathUtils::ScalarResult aRetryRes =
            MathRoot::NewtonBounded(aFunc, aBestU, aRefUMin, aRefUMax, aConfig);
          if (aRetryRes.IsDone())
          {
            aRootU     = std::max(theDomain.Min, std::min(theDomain.Max, *aRetryRes.Root));
            aConverged = true;
            break;
          }
        }

        // Use best grid point as fallback
        if (!aConverged)
        {
          gp_Pnt aPt;
          gp_Vec aD1;
          theCurve.D1(aBestU, aPt, aD1);
          gp_Vec aVec(theP, aPt);
          double aF = aVec.Dot(aD1);

          if (std::abs(aF) < theTol * ExtremaPC::THE_FALLBACK_F_FACTOR)
          {
            aRootU     = aBestU;
            aConverged = true;
          }
        }
      }

      if (!aConverged)
        continue;

      // Check for duplicate
      bool aDuplicate = false;
      for (int r = 0; r < myFoundRoots.Length(); ++r)
      {
        if (std::abs(aRootU - myFoundRoots.Value(r)) < theTol)
        {
          aDuplicate = true;
          break;
        }
      }
      if (aDuplicate)
        continue;

      gp_Pnt aPt     = theCurve.Value(aRootU);
      double aSqDist = theP.SquareDistance(aPt);

      // Classify as min/max using neighbor sampling
      double aStep = (theDomain.Max - theDomain.Min) * ExtremaPC::THE_REFINEMENT_STEP_RATIO;
      double aDistPlus =
        theP.SquareDistance(theCurve.Value(std::min(theDomain.Max, aRootU + aStep)));
      double aDistMinus =
        theP.SquareDistance(theCurve.Value(std::max(theDomain.Min, aRootU - aStep)));
      bool aIsMin = (aSqDist <= aDistPlus) && (aSqDist <= aDistMinus);

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
        myResult.Extrema.Append(anExt);

        myFoundRoots.Append(aRootU);

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

    if (myResult.Extrema.IsEmpty() && myCandidates.IsEmpty())
    {
      myResult.Status = ExtremaPC::Status::NoSolution;
    }
  }

private:
  NCollection_Array1<GridPoint> myGrid; //!< Cached grid

  // Mutable cached temporaries (reused via Clear())
  mutable ExtremaPC::Result                          myResult;        //!< Reusable result
  mutable NCollection_Vector<Candidate>              myCandidates;    //!< Candidates from grid scan
  mutable NCollection_Vector<double>                 myFoundRoots;    //!< Found roots for dedup
  mutable NCollection_Vector<std::pair<int, double>> mySortedIndices; //!< Sorted candidate indices
  mutable NCollection_Array1<bool>                   myProcessed; //!< Processed flags for grid scan
};

#endif // _ExtremaPC_GridEvaluator_HeaderFile
