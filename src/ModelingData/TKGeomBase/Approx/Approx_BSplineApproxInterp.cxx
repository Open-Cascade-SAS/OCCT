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

#include <Approx_BSplineApproxInterp.hxx>

#include <BSplCLib.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <MathLin_Gauss.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>

#include <algorithm>
#include <cmath>

namespace
{

//! Maximum number of Newton iterations for point-on-curve projection.
constexpr int THE_MAX_PROJECTION_ITER = 10;

//! Small value to avoid unstable normalization by near-zero error.
constexpr double THE_ERROR_NORM_EPS = 1.0e-6;

//! Initial multiplier for previous-error bootstrap in optimization loop.
constexpr double THE_OLD_ERROR_SCALE = 2.0;

//! Threshold for treating LU forward-substitution accumulator as non-zero.
constexpr double THE_LU_NONZERO_EPS = 1.0e-30;

//! Threshold for Newton denominator degeneracy during projection.
constexpr double THE_NEWTON_D2F_EPS = 1.0e-30;

//! Number of boundary knots in open knot vector (start and end).
constexpr int THE_BOUNDARY_KNOT_COUNT = 2;

//! Divisor for alpha exponent in centripetal/chord-length parameterization.
constexpr double THE_PARAM_ALPHA_DIVISOR = 2.0;

//! Final normalized parameter value.
constexpr double THE_PARAM_LAST_VALUE = 1.0;

//! Continuity row indices in the continuity constraint matrix.
constexpr int THE_CONT_ROW_C1 = 1;
constexpr int THE_CONT_ROW_C2 = 2;
constexpr int THE_CONT_ROW_C0 = 3;

//! Number of continuity constraints for closed curves.
constexpr int THE_CONT_COUNT_C0_C1_C2 = 3;
constexpr int THE_CONT_COUNT_C1_C2    = 2;

//! Inserts a kink knot into pre-built knot/multiplicity arrays.
//! If a matching knot exists within tolerance, its multiplicity is raised.
//! Otherwise, a new knot is added (arrays are resized).
//! @param[in]     theKnot   knot value to insert
//! @param[in]     theDegree desired and maximum multiplicity
//! @param[in,out] theKnots  knot values (may be reallocated)
//! @param[in,out] theMults  multiplicities (may be reallocated)
//! @param[in]     theTol    tolerance for matching existing knots
void insertKinkKnot(double                      theKnot,
                    int                         theDegree,
                    NCollection_Array1<double>& theKnots,
                    NCollection_Array1<int>&    theMults,
                    double                      theTol)
{
  // Search for an existing knot within tolerance.
  for (int i = theKnots.Lower(); i <= theKnots.Upper(); ++i)
  {
    if (std::abs(theKnots(i) - theKnot) < theTol)
    {
      // Existing knot found - raise multiplicity to at least degree (never decrease).
      theMults(i) = std::max(theMults(i), theDegree);
      return;
    }
  }

  // New knot - find insertion position to keep sorted order.
  int aPos = theKnots.Lower();
  while (aPos <= theKnots.Upper() && theKnots(aPos) < theKnot)
  {
    ++aPos;
  }

  // Build new arrays with one extra element.
  const int                  aNbOld = theKnots.Length();
  NCollection_Array1<double> aNewKnots(theKnots.Lower(), theKnots.Lower() + aNbOld);
  NCollection_Array1<int>    aNewMults(theMults.Lower(), theMults.Lower() + aNbOld);

  int aWriteIdx = aNewKnots.Lower();
  for (int i = theKnots.Lower(); i < aPos; ++i)
  {
    aNewKnots(aWriteIdx) = theKnots(i);
    aNewMults(aWriteIdx) = theMults(i);
    ++aWriteIdx;
  }
  aNewKnots(aWriteIdx) = theKnot;
  aNewMults(aWriteIdx) = theDegree;
  ++aWriteIdx;
  for (int i = aPos; i <= theKnots.Upper(); ++i)
  {
    aNewKnots(aWriteIdx) = theKnots(i);
    aNewMults(aWriteIdx) = theMults(i);
    ++aWriteIdx;
  }

  theKnots = std::move(aNewKnots);
  theMults = std::move(aNewMults);
}

} // anonymous namespace

//=================================================================================================

Approx_BSplineApproxInterp::Approx_BSplineApproxInterp(const NCollection_Array1<gp_Pnt>& thePoints,
                                                       int  theNbControlPts,
                                                       int  theDegree,
                                                       bool theContinuousIfClosed)
    : myPoints(1, std::max(thePoints.Length(), 1)),
      myDegree(std::max(theDegree, 1)),
      myNbControlPts(std::max(theNbControlPts, theDegree + 1)),
      myContinuousIfClosed(theContinuousIfClosed)
{
  // Clamp control point count: must be >= degree + 1 and <= number of points.
  myNbControlPts = std::min(myNbControlPts, thePoints.Length());
  // If clamping reduced control points below degree + 1, reduce degree accordingly.
  if (myNbControlPts < myDegree + 1)
  {
    myDegree = std::max(myNbControlPts - 1, 1);
  }

  // Copy points to 1-based internal array and initialize all as approximated.
  for (int i = 0; i < thePoints.Length(); ++i)
  {
    myPoints.SetValue(i + 1, thePoints.Value(thePoints.Lower() + i));
    myApproximated.Append(i);
  }
}

//=================================================================================================

void Approx_BSplineApproxInterp::InterpolatePoint(int thePointIndex, bool theWithKink)
{
  // Move from approximated to interpolated set.
  for (int i = 0; i < myApproximated.Length(); ++i)
  {
    if (myApproximated.Value(i) == thePointIndex)
    {
      // Swap with last and remove last (order doesn't matter).
      if (i < myApproximated.Length() - 1)
      {
        myApproximated.ChangeValue(i) = myApproximated.Value(myApproximated.Length() - 1);
      }
      myApproximated.EraseLast();
      myInterpolated.Append(thePointIndex);
      if (theWithKink)
      {
        myKinks.Append(thePointIndex);
      }
      return;
    }
  }
}

//=================================================================================================

void Approx_BSplineApproxInterp::Perform()
{
  const NCollection_Array1<double> aParams = computeParameters(myAlpha);
  Perform(aParams);
}

//=================================================================================================

void Approx_BSplineApproxInterp::Perform(const NCollection_Array1<double>& theParams)
{
  myIsDone   = false;
  myMaxError = 0.0;
  myCurve.Nullify();

  if (theParams.Length() != myPoints.Length())
  {
    return;
  }

  // Compute knots from parameters.
  NCollection_Array1<double> aKnots;
  NCollection_Array1<int>    aMults;
  computeKnots(myNbControlPts, theParams, aKnots, aMults);

  myIsDone = solve(theParams, aKnots, aMults);
}

//=================================================================================================

void Approx_BSplineApproxInterp::PerformOptimal(int theMaxIter)
{
  const NCollection_Array1<double> aParams = computeParameters(myAlpha);
  PerformOptimal(aParams, theMaxIter);
}

//=================================================================================================

void Approx_BSplineApproxInterp::PerformOptimal(const NCollection_Array1<double>& theParams,
                                                int                               theMaxIter)
{
  myIsDone   = false;
  myMaxError = 0.0;
  myCurve.Nullify();

  if (theParams.Length() != myPoints.Length())
  {
    return;
  }

  NCollection_Array1<double> aParams(theParams.Lower(), theParams.Upper());
  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    aParams.SetValue(i, theParams.Value(i));
  }

  // Compute knots from parameters.
  NCollection_Array1<double> aKnots;
  NCollection_Array1<int>    aMults;
  computeKnots(myNbControlPts, aParams, aKnots, aMults);

  // Initial solve.
  if (!solve(aParams, aKnots, aMults))
  {
    return;
  }

  double anOldError = myMaxError * THE_OLD_ERROR_SCALE;
  int    anIter     = 0;

  while (myMaxError > 0.0
         && (anOldError - myMaxError) / std::max(myMaxError, THE_ERROR_NORM_EPS) > myConvergenceTol
         && anIter < theMaxIter)
  {
    anOldError = myMaxError;

    optimizeParameters(myCurve, aParams);
    if (!solve(aParams, aKnots, aMults))
    {
      return;
    }

    ++anIter;
  }

  myIsDone = true;
}

//=================================================================================================

const occ::handle<Geom_BSplineCurve>& Approx_BSplineApproxInterp::Curve() const
{
  return myCurve;
}

//=================================================================================================

NCollection_Array1<double> Approx_BSplineApproxInterp::computeParameters(double theAlpha) const
{
  const int                  aNbPts = myPoints.Length();
  NCollection_Array1<double> aParams(1, aNbPts);

  aParams.SetValue(1, 0.0);
  double aSum = 0.0;
  for (int i = 2; i <= aNbPts; ++i)
  {
    double aLen2 = myPoints.Value(i - 1).SquareDistance(myPoints.Value(i));
    aSum += std::pow(aLen2, theAlpha / THE_PARAM_ALPHA_DIVISOR);
    aParams.SetValue(i, aSum);
  }

  // Normalize to [0, 1].
  const double aTmax = aParams.Value(aNbPts);
  if (aTmax > 0.0)
  {
    for (int i = 2; i <= aNbPts; ++i)
    {
      aParams.SetValue(i, aParams.Value(i) / aTmax);
    }
  }
  aParams.SetValue(aNbPts, THE_PARAM_LAST_VALUE);
  return aParams;
}

//=================================================================================================

void Approx_BSplineApproxInterp::computeKnots(int                               theNbCP,
                                              const NCollection_Array1<double>& theParams,
                                              NCollection_Array1<double>&       theKnots,
                                              NCollection_Array1<int>&          theMults) const
{
  const int anOrder = myDegree + 1;
  if (theNbCP < anOrder)
  {
    return;
  }

  // Find parameter range.
  double aUMin = theParams.Value(theParams.Lower());
  double aUMax = theParams.Value(theParams.Lower());
  for (int i = theParams.Lower() + 1; i <= theParams.Upper(); ++i)
  {
    aUMin = std::min(aUMin, theParams.Value(i));
    aUMax = std::max(aUMax, theParams.Value(i));
  }

  // Number of internal knots (simple multiplicity).
  const int aNbInternal  = theNbCP - anOrder;
  const int aNbBaseKnots = aNbInternal + THE_BOUNDARY_KNOT_COUNT; // start + internal + end

  // Build base knot vector as Array1 with known size.
  theKnots = NCollection_Array1<double>(1, aNbBaseKnots);
  theMults = NCollection_Array1<int>(1, aNbBaseKnots);

  // Start knot with full multiplicity.
  theKnots(1) = aUMin;
  theMults(1) = anOrder;

  // Uniform internal knots.
  for (int i = 1; i <= aNbInternal; ++i)
  {
    double aKnot =
      aUMin + (aUMax - aUMin) * static_cast<double>(i) / static_cast<double>(aNbInternal + 1);
    theKnots(i + 1) = aKnot;
    theMults(i + 1) = 1;
  }

  // End knot with full multiplicity.
  theKnots(aNbBaseKnots) = aUMax;
  theMults(aNbBaseKnots) = anOrder;

  // Insert kink knots with high multiplicity (may grow arrays).
  for (int i = 0; i < myKinks.Length(); ++i)
  {
    const int aKinkIdx = myKinks.Value(i);
    // 0-based point index -> 1-based param array index
    const double aKinkParam = theParams.Value(theParams.Lower() + aKinkIdx);
    insertKinkKnot(aKinkParam, myDegree, theKnots, theMults, myKnotInsertTol);
  }
}

//=================================================================================================

math_Matrix Approx_BSplineApproxInterp::buildBasisMatrix(
  const NCollection_Array1<double>& theFlatKnots,
  const NCollection_Array1<double>& theParams,
  int                               theDerivOrder) const
{
  const int   aNbCP = theFlatKnots.Length() - myDegree - 1;
  math_Matrix aBasisMat(1, theParams.Length(), 1, aNbCP, 0.0);

  math_Matrix aBspBasis(1, theDerivOrder + 1, 1, myDegree + 1, 0.0);

  for (int iParm = 1; iParm <= theParams.Length(); ++iParm)
  {
    aBspBasis.Init(0.0);
    int aFirstNonZero = 0;
    BSplCLib::EvalBsplineBasis(theDerivOrder,
                               myDegree + 1,
                               theFlatKnots,
                               theParams.Value(theParams.Lower() + iParm - 1),
                               aFirstNonZero,
                               aBspBasis);

    if (theDerivOrder > 0)
    {
      // Extract the derivative row.
      for (int j = 1; j <= myDegree + 1; ++j)
      {
        const int aCol = aFirstNonZero + j - 1;
        if (aCol >= 1 && aCol <= aNbCP)
        {
          aBasisMat(iParm, aCol) = aBspBasis(theDerivOrder + 1, j);
        }
      }
    }
    else
    {
      // Extract the value row.
      for (int j = 1; j <= myDegree + 1; ++j)
      {
        const int aCol = aFirstNonZero + j - 1;
        if (aCol >= 1 && aCol <= aNbCP)
        {
          aBasisMat(iParm, aCol) = aBspBasis(1, j);
        }
      }
    }
  }
  return aBasisMat;
}

//=================================================================================================

math_Matrix Approx_BSplineApproxInterp::buildContinuityMatrix(
  int                               theNbCtrPnts,
  int                               theNbContinuity,
  const NCollection_Array1<double>& theParams,
  const NCollection_Array1<double>& theFlatKnots) const
{
  math_Matrix aContinuity(1, theNbContinuity, 1, theNbCtrPnts, 0.0);

  const double aFirstParam = theParams.Value(theParams.Lower());
  const double aLastParam  = theParams.Value(theParams.Upper());

  NCollection_Array1<double> aParamFirst(1, 1);
  aParamFirst.SetValue(1, aFirstParam);
  NCollection_Array1<double> aParamLast(1, 1);
  aParamLast.SetValue(1, aLastParam);

  // C1 condition: first derivative at start equals first derivative at end.
  math_Matrix aDiff1First = buildBasisMatrix(theFlatKnots, aParamFirst, THE_CONT_ROW_C1);
  math_Matrix aDiff1Last  = buildBasisMatrix(theFlatKnots, aParamLast, THE_CONT_ROW_C1);
  for (int j = 1; j <= theNbCtrPnts; ++j)
  {
    aContinuity(THE_CONT_ROW_C1, j) = aDiff1First(1, j) - aDiff1Last(1, j);
  }

  // C2 condition: second derivative at start equals second derivative at end.
  if (theNbContinuity >= THE_CONT_ROW_C2)
  {
    math_Matrix aDiff2First = buildBasisMatrix(theFlatKnots, aParamFirst, THE_CONT_ROW_C2);
    math_Matrix aDiff2Last  = buildBasisMatrix(theFlatKnots, aParamLast, THE_CONT_ROW_C2);
    for (int j = 1; j <= theNbCtrPnts; ++j)
    {
      aContinuity(THE_CONT_ROW_C2, j) = aDiff2First(1, j) - aDiff2Last(1, j);
    }
  }

  // C0 condition (only if first/last not already interpolated).
  if (theNbContinuity >= THE_CONT_ROW_C0)
  {
    math_Matrix aDiff0First = buildBasisMatrix(theFlatKnots, aParamFirst, 0);
    math_Matrix aDiff0Last  = buildBasisMatrix(theFlatKnots, aParamLast, 0);
    for (int j = 1; j <= theNbCtrPnts; ++j)
    {
      aContinuity(THE_CONT_ROW_C0, j) = aDiff0First(1, j) - aDiff0Last(1, j);
    }
  }

  return aContinuity;
}

//=================================================================================================

bool Approx_BSplineApproxInterp::solve(const NCollection_Array1<double>& theParams,
                                       const NCollection_Array1<double>& theKnots,
                                       const NCollection_Array1<int>&    theMults)
{
  // Compute flat knots.
  const int                  aNbFlatKnots = BSplCLib::KnotSequenceLength(theMults, myDegree, false);
  NCollection_Array1<double> aFlatKnots(1, aNbFlatKnots);
  BSplCLib::KnotSequence(theKnots, theMults, myDegree, false, aFlatKnots);

  const int aNbApprox = myApproximated.Length();
  const int aNbInterp = myInterpolated.Length();

  const bool aMakeClosed   = isClosed();
  int        aNbContinuity = 0;
  if (aMakeClosed)
  {
    aNbContinuity = THE_CONT_COUNT_C0_C1_C2; // C0 + C1 + C2
    if (isFirstAndLastInterpolated())
    {
      aNbContinuity = THE_CONT_COUNT_C1_C2; // Remove C0 - already enforced by interpolation.
    }
  }

  const int aNbCtrPnts = aNbFlatKnots - myDegree - 1;
  if (aNbCtrPnts < aNbInterp + aNbContinuity || aNbCtrPnts < myDegree + 1 + aNbContinuity)
  {
    return false;
  }
  if (aNbApprox == 0 && aNbCtrPnts != aNbInterp + aNbContinuity)
  {
    return false;
  }

  // Build KKT system.
  const int   aNbVars = aNbCtrPnts + aNbInterp + aNbContinuity;
  math_Matrix aLHS(1, aNbVars, 1, aNbVars, 0.0);

  math_Vector aRHSx(1, aNbVars, 0.0);
  math_Vector aRHSy(1, aNbVars, 0.0);
  math_Vector aRHSz(1, aNbVars, 0.0);

  // Upper-left block: A^T * A and right-hand side A^T * b for approximated points.
  if (aNbApprox > 0)
  {
    NCollection_Array1<double> anApproxParams(1, aNbApprox);
    math_Vector                aBx(1, aNbApprox);
    math_Vector                aBy(1, aNbApprox);
    math_Vector                aBz(1, aNbApprox);

    for (int i = 0; i < aNbApprox; ++i)
    {
      const int     aIdx = myApproximated.Value(i); // 0-based point index
      const gp_Pnt& aPnt = myPoints.Value(aIdx + 1);
      aBx(i + 1)         = aPnt.X();
      aBy(i + 1)         = aPnt.Y();
      aBz(i + 1)         = aPnt.Z();
      anApproxParams.SetValue(i + 1, theParams.Value(theParams.Lower() + aIdx));
    }

    math_Matrix anA  = buildBasisMatrix(aFlatKnots, anApproxParams, 0);
    math_Matrix anAt = anA.Transposed();

    aLHS.Set(1, aNbCtrPnts, 1, aNbCtrPnts, anAt.Multiplied(anA));

    math_Vector aAtBx = anAt.Multiplied(aBx);
    math_Vector aAtBy = anAt.Multiplied(aBy);
    math_Vector aAtBz = anAt.Multiplied(aBz);
    aRHSx.Set(1, aNbCtrPnts, aAtBx);
    aRHSy.Set(1, aNbCtrPnts, aAtBy);
    aRHSz.Set(1, aNbCtrPnts, aAtBz);
  }

  // Constraint blocks: C (interpolation) and L (continuity).
  if (aNbInterp + aNbContinuity > 0)
  {
    // Right-hand side for constraints.
    math_Vector aDx(1, aNbInterp + aNbContinuity, 0.0);
    math_Vector aDy(1, aNbInterp + aNbContinuity, 0.0);
    math_Vector aDz(1, aNbInterp + aNbContinuity, 0.0);

    if (aNbInterp > 0)
    {
      NCollection_Array1<double> anInterpParams(1, aNbInterp);
      for (int i = 0; i < aNbInterp; ++i)
      {
        const int     aIdx = myInterpolated.Value(i); // 0-based point index
        const gp_Pnt& aPnt = myPoints.Value(aIdx + 1);
        aDx(i + 1)         = aPnt.X();
        aDy(i + 1)         = aPnt.Y();
        aDz(i + 1)         = aPnt.Z();
        anInterpParams.SetValue(i + 1, theParams.Value(theParams.Lower() + aIdx));
      }

      math_Matrix aC  = buildBasisMatrix(aFlatKnots, anInterpParams, 0);
      math_Matrix aCt = aC.Transposed();

      // C^T in upper-right, C in lower-left.
      aLHS.Set(1, aNbCtrPnts, aNbCtrPnts + 1, aNbCtrPnts + aNbInterp, aCt);
      aLHS.Set(aNbCtrPnts + 1, aNbCtrPnts + aNbInterp, 1, aNbCtrPnts, aC);
    }

    // Continuity constraints for closed curves.
    if (aMakeClosed && aNbContinuity > 0)
    {
      math_Matrix aContMat =
        buildContinuityMatrix(aNbCtrPnts, aNbContinuity, theParams, aFlatKnots);
      math_Matrix aContMatT = aContMat.Transposed();

      const int aContRowStart = aNbCtrPnts + aNbInterp + 1;
      const int aContRowEnd   = aNbCtrPnts + aNbInterp + aNbContinuity;
      const int aContColStart = aContRowStart;
      const int aContColEnd   = aContRowEnd;

      aLHS.Set(aContRowStart, aContRowEnd, 1, aNbCtrPnts, aContMat);
      aLHS.Set(1, aNbCtrPnts, aContColStart, aContColEnd, aContMatT);
    }

    aRHSx.Set(aNbCtrPnts + 1, aNbVars, aDx);
    aRHSy.Set(aNbCtrPnts + 1, aNbVars, aDy);
    aRHSz.Set(aNbCtrPnts + 1, aNbVars, aDz);
  }

  // LU-factorize the KKT matrix once, then solve for X, Y, Z by back-substitution.
  MathLin::LUResult aLURes = MathLin::LU(aLHS, myMinPivot);
  if (!aLURes.IsDone())
  {
    return false;
  }

  const math_Matrix&        aLU    = *aLURes.LU;
  const math_IntegerVector& aPivot = *aLURes.Pivot;

  // Helper: solve LU * x = b using pre-computed factorization.
  auto solveLU = [&](const math_Vector& theRHS) -> math_Vector {
    math_Vector aX            = theRHS;
    int         aFirstNonZero = 0;
    for (int i = aX.Lower(); i <= aX.Upper(); ++i)
    {
      const int aPivotIdx = aPivot(i);
      double    aSum      = aX(aPivotIdx);
      aX(aPivotIdx)       = aX(i);
      if (aFirstNonZero != 0)
      {
        for (int j = aFirstNonZero; j < i; ++j)
        {
          aSum -= aLU(i, j) * aX(j);
        }
      }
      else if (std::abs(aSum) > THE_LU_NONZERO_EPS)
      {
        aFirstNonZero = i;
      }
      aX(i) = aSum;
    }
    for (int i = aX.Upper(); i >= aX.Lower(); --i)
    {
      double aSum = aX(i);
      for (int j = i + 1; j <= aX.Upper(); ++j)
      {
        aSum -= aLU(i, j) * aX(j);
      }
      aX(i) = aSum / aLU(i, i);
    }
    return aX;
  };

  const math_Vector aCPx = solveLU(aRHSx);
  const math_Vector aCPy = solveLU(aRHSy);
  const math_Vector aCPz = solveLU(aRHSz);

  // Extract control points.
  NCollection_Array1<gp_Pnt> aPoles(1, aNbCtrPnts);
  for (int i = 1; i <= aNbCtrPnts; ++i)
  {
    aPoles.SetValue(i, gp_Pnt(aCPx.Value(i), aCPy.Value(i), aCPz.Value(i)));
  }

  myCurve = new Geom_BSplineCurve(aPoles, theKnots, theMults, myDegree, false);

  // Compute max error at approximated points.
  const GeomAdaptor_Curve& aCurveAdaptor = curveAdaptor(myCurve);
  myMaxError                             = 0.0;
  for (int i = 0; i < aNbApprox; ++i)
  {
    const int     aIdx    = myApproximated.Value(i);
    const gp_Pnt& aPnt    = myPoints.Value(aIdx + 1);
    const double  aParam  = theParams.Value(theParams.Lower() + aIdx);
    const double  anError = aCurveAdaptor.EvalD0(aParam).Distance(aPnt);
    myMaxError            = std::max(myMaxError, anError);
  }

  return true;
}

//=================================================================================================

const GeomAdaptor_Curve& Approx_BSplineApproxInterp::curveAdaptor(
  const occ::handle<Geom_BSplineCurve>& theCurve) const
{
  if (myCurveAdaptorCache.IsNull())
  {
    myCurveAdaptorCache = new GeomAdaptor_Curve(theCurve);
  }
  else if (myCurveAdaptorCache->Curve() != theCurve)
  {
    myCurveAdaptorCache->Load(theCurve);
  }

  return *myCurveAdaptorCache;
}

//=================================================================================================

double Approx_BSplineApproxInterp::projectOnCurve(const gp_Pnt&            thePnt,
                                                  const GeomAdaptor_Curve& theCurveAdaptor,
                                                  double                   theInitParam,
                                                  double&                  theParam) const
{
  const double aFirstParam = theCurveAdaptor.FirstParameter();
  const double aLastParam  = theCurveAdaptor.LastParameter();
  double       aT          = theInitParam;
  double       aDt         = 0.0;
  double       aF          = 0.0;

  for (int anIter = 0; anIter < THE_MAX_PROJECTION_ITER; ++anIter)
  {
    const Geom_Curve::ResD2 aCurveD2 = theCurveAdaptor.EvalD2(aT);

    const gp_XYZ aDiff = aCurveD2.Point.XYZ() - thePnt.XYZ();
    aF                 = aDiff.SquareModulus();

    const double aDF  = aDiff.Dot(aCurveD2.D1.XYZ());
    const double aD2F = aDiff.Dot(aCurveD2.D2.XYZ()) + aCurveD2.D1.SquareMagnitude();

    if (std::abs(aD2F) < THE_NEWTON_D2F_EPS)
    {
      break;
    }

    aDt          = -aDF / aD2F;
    double aNewT = aT + aDt;

    // Clamp to curve domain.
    if (aNewT < aFirstParam)
    {
      aNewT = aFirstParam;
    }
    else if (aNewT > aLastParam)
    {
      aNewT = aLastParam;
    }
    aT = aNewT;

    if (std::abs(aDt) <= myProjectionTol)
    {
      break;
    }
  }

  theParam = aT;
  return std::sqrt(aF);
}

//=================================================================================================

void Approx_BSplineApproxInterp::optimizeParameters(const occ::handle<Geom_BSplineCurve>& theCurve,
                                                    NCollection_Array1<double>& theParams) const
{
  const GeomAdaptor_Curve& aCurveAdaptor = curveAdaptor(theCurve);

  for (int i = 0; i < myApproximated.Length(); ++i)
  {
    const int     aIdx       = myApproximated.Value(i);
    const gp_Pnt& aPnt       = myPoints.Value(aIdx + 1);
    const double  anOldParam = theParams.Value(theParams.Lower() + aIdx);

    double aNewParam = anOldParam;
    projectOnCurve(aPnt, aCurveAdaptor, anOldParam, aNewParam);
    theParams.SetValue(theParams.Lower() + aIdx, aNewParam);
  }
}

//=================================================================================================

bool Approx_BSplineApproxInterp::isClosed() const
{
  if (!myContinuousIfClosed)
  {
    return false;
  }
  const double aDiag = boundingBoxDiagonal();
  const double aTol  = myClosedRelTol * aDiag;
  return myPoints.Value(myPoints.Lower()).IsEqual(myPoints.Value(myPoints.Upper()), aTol);
}

//=================================================================================================

bool Approx_BSplineApproxInterp::isFirstAndLastInterpolated() const
{
  bool      aFirst   = false;
  bool      aLast    = false;
  const int aLastIdx = myPoints.Length() - 1; // 0-based
  for (int i = 0; i < myInterpolated.Length(); ++i)
  {
    if (myInterpolated.Value(i) == 0)
    {
      aFirst = true;
    }
    if (myInterpolated.Value(i) == aLastIdx)
    {
      aLast = true;
    }
  }
  return aFirst && aLast;
}

//=================================================================================================

double Approx_BSplineApproxInterp::boundingBoxDiagonal() const
{
  if (myPoints.Length() <= 0)
  {
    return 0.0;
  }

  gp_Pnt aMin = myPoints.Value(myPoints.Lower());
  gp_Pnt aMax = aMin;

  for (int aPointIdx = myPoints.Lower() + 1; aPointIdx <= myPoints.Upper(); ++aPointIdx)
  {
    const gp_Pnt& aPnt = myPoints.Value(aPointIdx);
    aMin.SetX(std::min(aMin.X(), aPnt.X()));
    aMin.SetY(std::min(aMin.Y(), aPnt.Y()));
    aMin.SetZ(std::min(aMin.Z(), aPnt.Z()));
    aMax.SetX(std::max(aMax.X(), aPnt.X()));
    aMax.SetY(std::max(aMax.Y(), aPnt.Y()));
    aMax.SetZ(std::max(aMax.Z(), aPnt.Z()));
  }

  return aMax.Distance(aMin);
}
