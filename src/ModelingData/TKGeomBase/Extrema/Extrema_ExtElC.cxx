// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <ElCLib.hxx>
#include <Extrema_ExtElC.hxx>
#include <Extrema_ExtElC2d.hxx>
#include <Extrema_POnCurv.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <math_DirectPolynomialRoots.hxx>
#include <math_FunctionSetRoot.hxx>
#include <math_FunctionSetWithDerivatives.hxx>
#include <math_Matrix.hxx>
#include <math_TrigonometricFunctionRoots.hxx>
#include <math_Vector.hxx>
#include <NCollection_Vector.hxx>
#include <Precision.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

#include <algorithm>

namespace
{

// Global constants used throughout the file.
constexpr double THE_TWO_PI              = 2.0 * M_PI;
constexpr double THE_HALF_PI             = M_PI / 2.0;
constexpr double THE_THREE_HALF_PI       = 3.0 * M_PI / 2.0;
constexpr int    THE_NB_GRID_SAMPLES     = 8;
constexpr int    THE_NB_AXIS_SAMPLES     = 4;
constexpr int    THE_MAX_EXTREMA         = 6;
constexpr int    THE_MAX_NEWTON_ITER     = 30;
constexpr double THE_DUPLICATE_TOL       = 0.01;
constexpr double THE_COEF_ZERO_TOL       = 1.0e-12;
constexpr double THE_TRIG_PRECISION_BASE = 1.0e-8;
constexpr double THE_TRIG_PRECISION_MULT = 1.0e-12;

//! Axis-aligned sample values for direct search in non-coplanar circle extrema.
constexpr double THE_AXIS_VALUES[THE_NB_AXIS_SAMPLES] = {0.0, THE_HALF_PI, M_PI, THE_THREE_HALF_PI};

//! Refines direction vector to align with coordinate axes when very close to ±1.
//! @param[in,out] theDir Direction to refine
void RefineDir(gp_Dir& theDir)
{
  constexpr double aEps = RealEpsilon();
  const double     aX   = theDir.X();
  const double     aY   = theDir.Y();
  const double     aZ   = theDir.Z();

  if (std::abs(std::abs(aX) - 1.0) < aEps)
  {
    theDir.SetCoord(aX > 0.0 ? 1.0 : -1.0, 0.0, 0.0);
  }
  else if (std::abs(std::abs(aY) - 1.0) < aEps)
  {
    theDir.SetCoord(0.0, aY > 0.0 ? 1.0 : -1.0, 0.0);
  }
  else if (std::abs(std::abs(aZ) - 1.0) < aEps)
  {
    theDir.SetCoord(0.0, 0.0, aZ > 0.0 ? 1.0 : -1.0);
  }
}

//! Function class for computing extrema between two circles.
//! The extrema conditions are:
//!   F1 = D · T1 = 0 (distance vector perpendicular to tangent of C1)
//!   F2 = D · T2 = 0 (distance vector perpendicular to tangent of C2)
//! where D = P2 - P1, and T1, T2 are tangent vectors.
class Extrema_FuncExtCircCirc : public math_FunctionSetWithDerivatives
{
public:
  //! Constructor with two circles.
  //! @param[in] theC1 First circle
  //! @param[in] theC2 Second circle
  Extrema_FuncExtCircCirc(const gp_Circ& theC1, const gp_Circ& theC2)
      : myO1(theC1.Location()),
        myO2(theC2.Location()),
        myX1(theC1.XAxis().Direction()),
        myY1(theC1.YAxis().Direction()),
        myX2(theC2.XAxis().Direction()),
        myY2(theC2.YAxis().Direction()),
        myR1(theC1.Radius()),
        myR2(theC2.Radius()),
        myVx1(0.0),
        myVy1(0.0),
        myVx2(0.0),
        myVy2(0.0),
        myA11(0.0),
        myA12(0.0),
        myA21(0.0),
        myA22(0.0)
  {
    const gp_Vec aV(myO1, myO2);
    myVx1 = aV.Dot(myX1);
    myVy1 = aV.Dot(myY1);
    myVx2 = aV.Dot(myX2);
    myVy2 = aV.Dot(myY2);
    myA11 = myX1.Dot(myX2);
    myA12 = myX1.Dot(myY2);
    myA21 = myY1.Dot(myX2);
    myA22 = myY1.Dot(myY2);
  }

  //! Returns number of variables (2 for u, v parameters).
  int NbVariables() const override { return 2; }

  //! Returns number of equations (2 for extrema conditions).
  int NbEquations() const override { return 2; }

  //! Computes function values at given parameters.
  //! @param[in] theUV Parameter vector (u, v)
  //! @param[out] theF Function values (F1, F2)
  //! @return Always true
  bool Value(const math_Vector& theUV, math_Vector& theF) override
  {
    const double aU    = theUV(1);
    const double aV    = theUV(2);
    const double aCosU = std::cos(aU);
    const double aSinU = std::sin(aU);
    const double aCosV = std::cos(aV);
    const double aSinV = std::sin(aV);

    // F1: (V + R2*P2).T1 = 0, where T1 = (-sinU, cosU) in local coords
    const double aVT1 = -aSinU * myVx1 + aCosU * myVy1;
    const double aP2T1 =
      (aCosV * myA11 + aSinV * myA12) * (-aSinU) + (aCosV * myA21 + aSinV * myA22) * aCosU;
    theF(1) = aVT1 + myR2 * aP2T1;

    // F2: (V - R1*P1).T2 = 0, where T2 = (-sinV, cosV) in local coords
    const double aVT2 = -aSinV * myVx2 + aCosV * myVy2;
    const double aP1T2 =
      (aCosU * myA11 + aSinU * myA21) * (-aSinV) + (aCosU * myA12 + aSinU * myA22) * aCosV;
    theF(2) = aVT2 - myR1 * aP1T2;

    return true;
  }

  //! Computes Jacobian matrix at given parameters.
  //! @param[in] theUV Parameter vector (u, v)
  //! @param[out] theD Jacobian matrix
  //! @return Always true
  bool Derivatives(const math_Vector& theUV, math_Matrix& theD) override
  {
    const double aU    = theUV(1);
    const double aV    = theUV(2);
    const double aCosU = std::cos(aU);
    const double aSinU = std::sin(aU);
    const double aCosV = std::cos(aV);
    const double aSinV = std::sin(aV);

    // dF1/du
    theD(1, 1) = -aCosU * myVx1 - aSinU * myVy1
                 + myR2
                     * ((aCosV * myA11 + aSinV * myA12) * (-aCosU)
                        + (aCosV * myA21 + aSinV * myA22) * (-aSinU));
    // dF1/dv
    theD(1, 2) =
      myR2
      * ((-aSinV * myA11 + aCosV * myA12) * (-aSinU) + (-aSinV * myA21 + aCosV * myA22) * aCosU);
    // dF2/du
    theD(2, 1) =
      -myR1
      * ((-aSinU * myA11 + aCosU * myA21) * (-aSinV) + (-aSinU * myA12 + aCosU * myA22) * aCosV);
    // dF2/dv
    theD(2, 2) = -aCosV * myVx2 - aSinV * myVy2
                 - myR1
                     * ((aCosU * myA11 + aSinU * myA21) * (-aCosV)
                        + (aCosU * myA12 + aSinU * myA22) * (-aSinV));

    return true;
  }

  //! Computes both function values and Jacobian.
  //! @param[in] theUV Parameter vector (u, v)
  //! @param[out] theF Function values
  //! @param[out] theD Jacobian matrix
  //! @return True if both computations succeed
  bool Values(const math_Vector& theUV, math_Vector& theF, math_Matrix& theD) override
  {
    return Value(theUV, theF) && Derivatives(theUV, theD);
  }

  //! Computes points on circles at given parameters.
  //! @param[in] theU Parameter on first circle
  //! @param[in] theV Parameter on second circle
  //! @param[out] theP1 Point on first circle
  //! @param[out] theP2 Point on second circle
  void Points(const double theU, const double theV, gp_Pnt& theP1, gp_Pnt& theP2) const
  {
    const double aCosU = std::cos(theU);
    const double aSinU = std::sin(theU);
    const double aCosV = std::cos(theV);
    const double aSinV = std::sin(theV);
    theP1              = gp_Pnt(myO1.XYZ() + myR1 * aCosU * myX1.XYZ() + myR1 * aSinU * myY1.XYZ());
    theP2              = gp_Pnt(myO2.XYZ() + myR2 * aCosV * myX2.XYZ() + myR2 * aSinV * myY2.XYZ());
  }

private:
  gp_Pnt myO1;  //!< Center of first circle
  gp_Pnt myO2;  //!< Center of second circle
  gp_Dir myX1;  //!< X direction of first circle
  gp_Dir myY1;  //!< Y direction of first circle
  gp_Dir myX2;  //!< X direction of second circle
  gp_Dir myY2;  //!< Y direction of second circle
  double myR1;  //!< Radius of first circle
  double myR2;  //!< Radius of second circle
  double myVx1; //!< V.X1 component
  double myVy1; //!< V.Y1 component
  double myVx2; //!< V.X2 component
  double myVy2; //!< V.Y2 component
  double myA11; //!< X1.X2 dot product
  double myA12; //!< X1.Y2 dot product
  double myA21; //!< Y1.X2 dot product
  double myA22; //!< Y1.Y2 dot product
};

//! Candidate for an extremum point between two circles.
struct ExtremumCandidate
{
  double U;      //!< Parameter on first circle
  double V;      //!< Parameter on second circle
  double SqDist; //!< Squared distance between points
};

//! Normalizes angle to [0, 2*PI) range.
//! @param[in] theAngle Angle to normalize
//! @return Normalized angle
inline double normalizeAngle(double theAngle)
{
  while (theAngle < 0.0)
  {
    theAngle += THE_TWO_PI;
  }
  while (theAngle >= THE_TWO_PI)
  {
    theAngle -= THE_TWO_PI;
  }
  return theAngle;
}

//! Checks if a candidate is duplicate of any existing result.
//! @param[in] theResults Existing results
//! @param[in] theU Parameter u to check
//! @param[in] theV Parameter v to check
//! @return True if duplicate
bool isDuplicateCandidate(const NCollection_Vector<ExtremumCandidate>& theResults,
                          const double                                 theU,
                          const double                                 theV)
{
  for (int k = 0; k < theResults.Size(); ++k)
  {
    const double aDu         = std::abs(theResults(k).U - theU);
    const double aDv         = std::abs(theResults(k).V - theV);
    const double aDuPeriodic = std::min(aDu, THE_TWO_PI - aDu);
    const double aDvPeriodic = std::min(aDv, THE_TWO_PI - aDv);
    if (aDuPeriodic < THE_DUPLICATE_TOL && aDvPeriodic < THE_DUPLICATE_TOL)
    {
      return true;
    }
  }
  return false;
}

//! Computes extrema between two non-coplanar circles using math_FunctionSetRoot.
//! @param[in] theC1 First circle
//! @param[in] theC2 Second circle
//! @param[out] theNbExt Number of extrema found
//! @param[out] theSqDist Array of squared distances
//! @param[out] thePoints Array of extrema point pairs
//! @return True if at least one extremum was found
bool computeNonCoplanarCircleExtrema(const gp_Circ&  theC1,
                                     const gp_Circ&  theC2,
                                     int&            theNbExt,
                                     double*         theSqDist,
                                     Extrema_POnCurv thePoints[][2])
{
  theNbExt = 0;

  Extrema_FuncExtCircCirc aFunc(theC1, theC2);

  // Set up bounds and tolerances for the solver (static to avoid repeated construction).
  static const math_Vector aTol(1, 2, Precision::PConfusion());
  static const math_Vector aInfBound(1, 2, 0.0);
  static const math_Vector aSupBound(1, 2, THE_TWO_PI);

  constexpr double aDelta = THE_TWO_PI / THE_NB_GRID_SAMPLES;

  NCollection_Vector<ExtremumCandidate> aResults;

  // Grid sampling with centered starting points.
  for (int i = 0; i < THE_NB_GRID_SAMPLES; ++i)
  {
    for (int j = 0; j < THE_NB_GRID_SAMPLES; ++j)
    {
      math_Vector aStart(1, 2);
      aStart(1) = (i + 0.5) * aDelta;
      aStart(2) = (j + 0.5) * aDelta;

      math_FunctionSetRoot aSolver(aFunc, aTol, THE_MAX_NEWTON_ITER);
      aSolver.Perform(aFunc, aStart, aInfBound, aSupBound);

      if (aSolver.IsDone())
      {
        const math_Vector& aRoot = aSolver.Root();
        const double       aU    = normalizeAngle(aRoot(1));
        const double       aV    = normalizeAngle(aRoot(2));

        gp_Pnt aP1, aP2;
        aFunc.Points(aU, aV, aP1, aP2);
        const double aSqDist = aP1.SquareDistance(aP2);

        if (!isDuplicateCandidate(aResults, aU, aV))
        {
          aResults.Append({aU, aV, aSqDist});
        }
      }
    }
  }

  // Direct search for minimum at axis-aligned positions.
  // This handles cases where Newton's method fails due to singular Jacobian
  // (e.g., when circles share the same center in perpendicular planes).
  double aMinSqDist = RealLast();
  double aMinU = 0.0, aMinV = 0.0;

  for (int i = 0; i < THE_NB_AXIS_SAMPLES; ++i)
  {
    for (int j = 0; j < THE_NB_AXIS_SAMPLES; ++j)
    {
      gp_Pnt aP1, aP2;
      aFunc.Points(THE_AXIS_VALUES[i], THE_AXIS_VALUES[j], aP1, aP2);
      const double aSqDist = aP1.SquareDistance(aP2);
      if (aSqDist < aMinSqDist)
      {
        aMinSqDist = aSqDist;
        aMinU      = THE_AXIS_VALUES[i];
        aMinV      = THE_AXIS_VALUES[j];
      }
    }
  }

  // Add the minimum found at axis positions if not duplicate.
  if (!isDuplicateCandidate(aResults, aMinU, aMinV))
  {
    aResults.Append({aMinU, aMinV, aMinSqDist});
  }

  // Sort results by squared distance to ensure minimum is included in the first 6.
  std::sort(
    aResults.begin(),
    aResults.end(),
    [](const ExtremumCandidate& a, const ExtremumCandidate& b) { return a.SqDist < b.SqDist; });

  // Copy results to output arrays.
  for (int i = 0; i < aResults.Size() && theNbExt < THE_MAX_EXTREMA; ++i)
  {
    const ExtremumCandidate& aCandidate = aResults(i);
    gp_Pnt                   aP1, aP2;
    aFunc.Points(aCandidate.U, aCandidate.V, aP1, aP2);
    theSqDist[theNbExt] = aCandidate.SqDist;
    thePoints[theNbExt][0].SetValues(aCandidate.U, aP1);
    thePoints[theNbExt][1].SetValues(aCandidate.V, aP2);
    ++theNbExt;
  }

  return theNbExt > 0;
}

//! Internal class for finding sorted roots of a trigonometric polynomial.
//! Duplicated code with IntAna_IntQuadQuad.cxx (lbr, March 26 1998).
//! Reliable solution for near-zero coefficient problems with retry
//! if coefficient < Precision::PConfusion() (jct, April 27 1998).
class ExtremaExtElC_TrigonometricRoots
{
public:
  //! Constructor that computes roots of the trigonometric equation:
  //!   CC*cos^2 + 2*SC*cos*sin + C*cos + S*sin + Cte = 0
  //! in the interval [Binf, Bsup].
  //! @param[in] theCC Coefficient of cos^2
  //! @param[in] theSC Coefficient of cos*sin (half of actual)
  //! @param[in] theC Coefficient of cos
  //! @param[in] theS Coefficient of sin
  //! @param[in] theCte Constant term
  //! @param[in] theBinf Lower bound of interval
  //! @param[in] theBsup Upper bound of interval
  ExtremaExtElC_TrigonometricRoots(const double theCC,
                                   const double theSC,
                                   const double theC,
                                   const double theS,
                                   const double theCte,
                                   const double theBinf,
                                   const double theBsup)
      : myRoots{0.0, 0.0, 0.0, 0.0},
        myNbRoots(0),
        myIsDone(false),
        myInfiniteRoots(false)
  {
    const double aTolCoef = Precision::PConfusion();

    // Working copies of coefficients that may be modified on retry.
    double aCC  = theCC;
    double aSC  = theSC;
    double aC   = theC;
    double aS   = theS;
    double aCte = theCte;

    for (int aNbTry = 1; aNbTry <= 2 && !myIsDone; ++aNbTry)
    {
      math_TrigonometricFunctionRoots aTrigRoots(aCC, aSC, aC, aS, aCte, theBinf, theBsup);

      if (aTrigRoots.IsDone())
      {
        myIsDone = true;
        if (aTrigRoots.InfiniteRoots())
        {
          myInfiniteRoots = true;
        }
        else
        {
          processRoots(aTrigRoots, theCC, theSC, theC, theS, theCte, aTolCoef);
        }
      }
      else
      {
        // Try to set very small coefficients to zero.
        if (std::abs(theCC) < aTolCoef)
        {
          aCC = 0.0;
        }
        if (std::abs(theSC) < aTolCoef)
        {
          aSC = 0.0;
        }
        if (std::abs(theC) < aTolCoef)
        {
          aC = 0.0;
        }
        if (std::abs(theS) < aTolCoef)
        {
          aS = 0.0;
        }
        if (std::abs(theCte) < aTolCoef)
        {
          aCte = 0.0;
        }
      }
    }
  }

  //! Returns true if computation was successful.
  bool IsDone() const { return myIsDone; }

  //! Checks if given value is one of the roots.
  //! @param[in] theU Value to check
  //! @return True if theU is a root
  bool IsARoot(const double theU) const
  {
    const double aEps = RealEpsilon();
    for (int i = 0; i < myNbRoots; ++i)
    {
      if (std::abs(theU - myRoots[i]) <= aEps)
      {
        return true;
      }
      if (std::abs(theU - myRoots[i] - THE_TWO_PI) <= aEps)
      {
        return true;
      }
    }
    return false;
  }

  //! Returns number of solutions.
  int NbSolutions() const
  {
    if (!myIsDone)
    {
      throw StdFail_NotDone();
    }
    return myNbRoots;
  }

  //! Returns true if equation has infinite roots.
  bool InfiniteRoots() const
  {
    if (!myIsDone)
    {
      throw StdFail_NotDone();
    }
    return myInfiniteRoots;
  }

  //! Returns the n-th root (1-based index).
  //! @param[in] theN Root index (1 to NbSolutions)
  //! @return Root value
  double Value(const int theN) const
  {
    if (!myIsDone || theN > myNbRoots)
    {
      throw StdFail_NotDone();
    }
    return myRoots[theN - 1];
  }

private:
  //! Processes roots from math_TrigonometricFunctionRoots solver.
  void processRoots(const math_TrigonometricFunctionRoots& theTrigRoots,
                    const double                           theCC,
                    const double                           theSC,
                    const double                           theC,
                    const double                           theS,
                    const double                           theCte,
                    const double                           theTolCoef)
  {
    myNbRoots = theTrigRoots.NbSolutions();

    // Normalize roots to [0, 2*PI] range.
    for (int i = 0; i < myNbRoots; ++i)
    {
      myRoots[i] = theTrigRoots.Value(i + 1);
      if (myRoots[i] < 0.0)
      {
        myRoots[i] += THE_TWO_PI;
      }
      if (myRoots[i] > THE_TWO_PI)
      {
        myRoots[i] -= THE_TWO_PI;
      }
    }

    // Calculate precision based on coefficient magnitudes.
    const double aMaxCoef = std::max(
      {std::abs(theCC), std::abs(theSC), std::abs(theC), std::abs(theS), std::abs(theCte)});
    const double aPrecision = std::max(THE_TRIG_PRECISION_BASE, THE_TRIG_PRECISION_MULT * aMaxCoef);

    // Validate roots and mark invalid ones.
    const int aSvNbRoots = myNbRoots;
    for (int i = 0; i < aSvNbRoots; ++i)
    {
      const double aCos = std::cos(myRoots[i]);
      const double aSin = std::sin(myRoots[i]);
      const double aY   = aCos * (theCC * aCos + 2.0 * theSC * aSin + theC) + theS * aSin + theCte;
      if (std::abs(aY) > aPrecision)
      {
        --myNbRoots;
        myRoots[i] = 1000.0; // Mark as invalid for sorting.
      }
    }

    // Sort roots using std::sort.
    std::sort(myRoots, myRoots + aSvNbRoots);

    // Detect case when polynomial is essentially constant.
    myInfiniteRoots = false;
    if (myNbRoots == 0)
    {
      const double aSumCoef = std::abs(theCC) + std::abs(theSC) + std::abs(theC) + std::abs(theS);
      if (aSumCoef < theTolCoef && std::abs(theCte) < theTolCoef)
      {
        myInfiniteRoots = true;
      }
    }
  }

  double myRoots[4];      //!< Array of root values
  int    myNbRoots;       //!< Number of valid roots
  bool   myIsDone;        //!< Computation status
  bool   myInfiniteRoots; //!< True if infinite roots
};

//! Zeros a coefficient if its absolute value is below tolerance.
//! @param[in,out] theCoef Coefficient to check and possibly zero
//! @param[in] theTol Tolerance threshold (default THE_COEF_ZERO_TOL)
inline void zeroSmallCoef(double& theCoef, const double theTol = THE_COEF_ZERO_TOL)
{
  if (std::abs(theCoef) <= theTol)
  {
    theCoef = 0.0;
  }
}

//! Projects a direction onto a local coordinate system defined by X, Y, Z axes.
//! @param[in] theDir Direction to project
//! @param[in] theX X axis direction
//! @param[in] theY Y axis direction
//! @param[in] theZ Z axis direction
//! @return Projected coordinates as gp_XYZ
constexpr gp_XYZ projectOnAxes(const gp_Dir& theDir,
                               const gp_Dir& theX,
                               const gp_Dir& theY,
                               const gp_Dir& theZ)
{
  return gp_XYZ(theDir.Dot(theX), theDir.Dot(theY), theDir.Dot(theZ));
}

//! Projects a vector onto a local coordinate system defined by X, Y, Z axes.
//! @param[in] theVec Vector to project
//! @param[in] theX X axis direction
//! @param[in] theY Y axis direction
//! @param[in] theZ Z axis direction
//! @return Projected coordinates as gp_XYZ
constexpr gp_XYZ projectOnAxes(const gp_Vec& theVec,
                               const gp_Dir& theX,
                               const gp_Dir& theY,
                               const gp_Dir& theZ)
{
  return gp_XYZ(theVec.Dot(theX), theVec.Dot(theY), theVec.Dot(theZ));
}

//! Initializes the mySqDist array to RealLast().
//! Template allows compiler to know array size at compile time for loop unrolling.
//! @tparam N Array size (deduced from argument)
//! @param[out] theSqDist Array to initialize
template <size_t N>
constexpr void initSqDistArray(double (&theSqDist)[N])
{
  for (size_t i = 0; i < N; ++i)
  {
    theSqDist[i] = RealLast();
  }
}

} // namespace

//=================================================================================================

Extrema_ExtElC::Extrema_ExtElC()
    : myDone(false),
      myIsPar(false),
      myNbExt(0)
{
  initSqDistArray(mySqDist);
}

//=================================================================================================

Extrema_ExtElC::Extrema_ExtElC(const gp_Lin& theC1, const gp_Lin& theC2, const Standard_Real)
    : myDone(false),
      myIsPar(false),
      myNbExt(0)
{
  // Find min distance between 2 straight lines.
  //
  // Method:
  //   Let D1 and D2, be 2 directions of straight lines C1 and C2.
  //   2 cases are considered:
  //   1- if Angle(D1,D2) < AngTol, straight lines are parallel.
  //      The distance is the distance between a point of C1 and the straight line C2.
  //   2- if Angle(D1,D2) > AngTol:
  //      Let P1=C1(u1) and P2=C2(u2).
  //      Then we must find u1 and u2 such as the distance P1P2 is minimal.
  //      The system of equations:
  //        {u1 - (D1.D2)*u2 = L.D1
  //        {(D1.D2)*u1 - u2 = L.D2
  //      has one solution if (D1.D2)^2 != 1 (if straight lines are not parallel).
  initSqDistArray(mySqDist);

  const gp_Dir& aD1     = theC1.Position().Direction();
  const gp_Dir& aD2     = theC2.Position().Direction();
  const double  aCosA   = aD1.Dot(aD2);
  const double  aSqSinA = 1.0 - aCosA * aCosA;

  double aU1 = 0.0, aU2 = 0.0;

  if (aSqSinA < gp::Resolution() || aD1.IsParallel(aD2, Precision::Angular()))
  {
    myIsPar = true;
  }
  else
  {
    const gp_XYZ aL1L2 = theC2.Location().XYZ() - theC1.Location().XYZ();
    const double aD1L  = aD1.XYZ().Dot(aL1L2);
    const double aD2L  = aD2.XYZ().Dot(aL1L2);
    aU1                = (aD1L - aCosA * aD2L) / aSqSinA;
    aU2                = (aCosA * aD1L - aD2L) / aSqSinA;

    myIsPar = Precision::IsInfinite(aU1) || Precision::IsInfinite(aU2);
  }

  if (myIsPar)
  {
    mySqDist[0] = theC2.SquareDistance(theC1.Location());
    myNbExt     = 1;
    myDone      = true;
    return;
  }

  const gp_Pnt aP1(ElCLib::Value(aU1, theC1));
  const gp_Pnt aP2(ElCLib::Value(aU2, theC2));
  mySqDist[myNbExt]   = aP1.SquareDistance(aP2);
  myPoint[myNbExt][0] = Extrema_POnCurv(aU1, aP1);
  myPoint[myNbExt][1] = Extrema_POnCurv(aU2, aP2);
  myNbExt             = 1;
  myDone              = true;
}

//=================================================================================================

Standard_Boolean Extrema_ExtElC::PlanarLineCircleExtrema(const gp_Lin&  theLin,
                                                         const gp_Circ& theCirc)
{
  const gp_Dir &aDirC = theCirc.Axis().Direction(), &aDirL = theLin.Direction();

  if (std::abs(aDirC.Dot(aDirL)) > Precision::Angular())
    return Standard_False;

  // The line is in the circle-plane completely
  //(or parallel to the circle-plane).
  // Therefore, we are looking for extremas and
  // intersections in 2D-space.

  const gp_XYZ& aCLoc = theCirc.Location().XYZ();
  const gp_XYZ &aDCx  = theCirc.Position().XDirection().XYZ(),
               &aDCy  = theCirc.Position().YDirection().XYZ();

  const gp_XYZ& aLLoc = theLin.Location().XYZ();
  const gp_XYZ& aLDir = theLin.Direction().XYZ();

  const gp_XYZ aVecCL(aLLoc - aCLoc);

  // Center of 2D-circle
  const gp_Pnt2d aPC(0.0, 0.0);

  gp_Ax22d  aCircAxis(aPC, gp_Dir2d(gp_Dir2d::D::X), gp_Dir2d(gp_Dir2d::D::Y));
  gp_Circ2d aCirc2d(aCircAxis, theCirc.Radius());

  gp_Pnt2d aPL(aVecCL.Dot(aDCx), aVecCL.Dot(aDCy));
  gp_Dir2d aDL(aLDir.Dot(aDCx), aLDir.Dot(aDCy));
  gp_Lin2d aLin2d(aPL, aDL);

  // Extremas
  Extrema_ExtElC2d anExt2d(aLin2d, aCirc2d, Precision::Confusion());
  // Intersections
  IntAna2d_AnaIntersection anInters(aLin2d, aCirc2d);

  myDone = anExt2d.IsDone() || anInters.IsDone();

  if (!myDone)
    return Standard_True;

  const Standard_Integer aNbExtr = anExt2d.NbExt();
  const Standard_Integer aNbSol  = anInters.NbPoints();

  const Standard_Integer aNbSum = aNbExtr + aNbSol;
  for (Standard_Integer anExtrID = 1; anExtrID <= aNbSum; anExtrID++)
  {
    const Standard_Integer aDelta = anExtrID - aNbExtr;

    Standard_Real aLinPar = 0.0, aCircPar = 0.0;

    if (aDelta < 1)
    {
      Extrema_POnCurv2d aPLin2d, aPCirc2d;
      anExt2d.Points(anExtrID, aPLin2d, aPCirc2d);
      aLinPar  = aPLin2d.Parameter();
      aCircPar = aPCirc2d.Parameter();
    }
    else
    {
      aLinPar  = anInters.Point(aDelta).ParamOnFirst();
      aCircPar = anInters.Point(aDelta).ParamOnSecond();
    }

    const gp_Pnt aPOnL(ElCLib::LineValue(aLinPar, theLin.Position())),
      aPOnC(ElCLib::CircleValue(aCircPar, theCirc.Position(), theCirc.Radius()));

    mySqDist[myNbExt] = aPOnL.SquareDistance(aPOnC);
    myPoint[myNbExt][0].SetValues(aLinPar, aPOnL);
    myPoint[myNbExt][1].SetValues(aCircPar, aPOnC);
    myNbExt++;
  }

  return Standard_True;
}

//=================================================================================================

Extrema_ExtElC::Extrema_ExtElC(const gp_Lin& C1, const gp_Circ& C2, const Standard_Real)
    : myDone(false),
      myIsPar(false),
      myNbExt(0)
{
  // Find extreme distances between straight line C1 and circle C2.
  // Uses trigonometric equation solver math_TrigonometricFunctionRoots.
  initSqDistArray(mySqDist);

  if (PlanarLineCircleExtrema(C1, C2))
  {
    return;
  }

  // Calculate direction in the reference of the circle.
  gp_Dir       aD  = C1.Direction();
  const gp_Dir aD1 = aD;

  const gp_Dir& aX2 = C2.XAxis().Direction();
  const gp_Dir& aY2 = C2.YAxis().Direction();
  const gp_Dir& aZ2 = C2.Axis().Direction();

  double aDx = aD.Dot(aX2);
  double aDy = aD.Dot(aY2);
  double aDz = aD.Dot(aZ2);

  aD.SetCoord(aDx, aDy, aDz);
  RefineDir(aD);
  aD.Coord(aDx, aDy, aDz);

  // Calculate V in circle's reference frame.
  const gp_Pnt aO1 = C1.Location();
  const gp_Pnt aO2 = C2.Location();
  gp_Vec       aO2O1(aO2, aO1);

  const double aTolRO2O1 = gp::Resolution();
  const double aRO2O1    = aO2O1.Magnitude();

  if (aRO2O1 > aTolRO2O1)
  {
    aO2O1.Multiply(1.0 / aRO2O1);
    gp_Dir aDO2O1;
    aDO2O1.SetCoord(aO2O1.Dot(aX2), aO2O1.Dot(aY2), aO2O1.Dot(aZ2));
    RefineDir(aDO2O1);
    aO2O1.SetXYZ(aRO2O1 * aDO2O1.XYZ());
  }
  else
  {
    aO2O1.SetCoord(aO2O1.Dot(aX2), aO2O1.Dot(aY2), aO2O1.Dot(aZ2));
  }

  const gp_XYZ aVxyz = (aD.XYZ() * aO2O1.Dot(aD)) - aO2O1.XYZ();

  // Calculate the coefficients of the trigonometric equation (divided by R).
  const double aR  = C2.Radius();
  double       aA5 = aR * aDx * aDy;
  double       aA1 = -2.0 * aA5;
  double       aA2 = 0.5 * aR * (aDx * aDx - aDy * aDy);
  double       aA3 = aVxyz.Y();
  double       aA4 = -aVxyz.X();

  // Zero small coefficients for numerical stability.
  zeroSmallCoef(aA1);
  zeroSmallCoef(aA2);
  zeroSmallCoef(aA3);
  zeroSmallCoef(aA4);
  zeroSmallCoef(aA5);

  ExtremaExtElC_TrigonometricRoots aSol(aA1, aA2, aA3, aA4, aA5, 0.0, THE_TWO_PI);
  if (!aSol.IsDone())
  {
    return;
  }

  if (aSol.InfiniteRoots())
  {
    myIsPar     = true;
    mySqDist[0] = aR * aR;
    myNbExt     = 1;
    myDone      = true;
    return;
  }

  // Store solutions.
  const int aNbSol = aSol.NbSolutions();
  for (int aNoSol = 1; aNoSol <= aNbSol; ++aNoSol)
  {
    const double aU2  = aSol.Value(aNoSol);
    const gp_Pnt aP2  = ElCLib::Value(aU2, C2);
    const double aU1  = gp_Vec(aO1, aP2).Dot(aD1);
    const gp_Pnt aP1  = ElCLib::Value(aU1, C1);
    mySqDist[myNbExt] = aP1.SquareDistance(aP2);
    myPoint[myNbExt][0].SetValues(aU1, aP1);
    myPoint[myNbExt][1].SetValues(aU2, aP2);
    ++myNbExt;
  }
  myDone = true;
}

//=================================================================================================

Extrema_ExtElC::Extrema_ExtElC(const gp_Lin& C1, const gp_Elips& C2)
    : myDone(false),
      myIsPar(false),
      myNbExt(0)
{
  // Find extreme distances between straight line C1 and ellipse C2.
  // Uses trigonometric equation solver math_TrigonometricFunctionRoots.
  initSqDistArray(mySqDist);

  // Calculate direction in the reference of the ellipse.
  const gp_Dir aD1 = C1.Direction();

  const gp_Dir& aX2 = C2.XAxis().Direction();
  const gp_Dir& aY2 = C2.YAxis().Direction();
  const gp_Dir& aZ2 = C2.Axis().Direction();

  const gp_XYZ aDLocal = projectOnAxes(aD1, aX2, aY2, aZ2);
  const double aDx     = aDLocal.X();
  const double aDy     = aDLocal.Y();

  // Calculate V in ellipse's reference frame.
  const gp_Pnt aO1   = C1.Location();
  const gp_Pnt aO2   = C2.Location();
  const gp_XYZ aO2O1 = projectOnAxes(gp_Vec(aO2, aO1), aX2, aY2, aZ2);
  const gp_XYZ aVxyz = (aDLocal * gp_Vec(aO2O1).Dot(gp_Dir(aDLocal))) - aO2O1;

  // Calculate the coefficients of the trigonometric equation.
  const double aMajR = C2.MajorRadius();
  const double aMinR = C2.MinorRadius();
  const double aR2   = aMajR * aMajR;
  const double ar2   = aMinR * aMinR;

  double aA5 = aMajR * aMinR * aDx * aDy;
  double aA1 = -2.0 * aA5;
  double aA2 = (aR2 * aDx * aDx - ar2 * aDy * aDy - aR2 + ar2) / 2.0;
  double aA3 = aMinR * aVxyz.Y();
  double aA4 = -aMajR * aVxyz.X();

  // Zero small coefficients for numerical stability.
  zeroSmallCoef(aA1);
  zeroSmallCoef(aA2);
  zeroSmallCoef(aA3);
  zeroSmallCoef(aA4);
  zeroSmallCoef(aA5);

  ExtremaExtElC_TrigonometricRoots aSol(aA1, aA2, aA3, aA4, aA5, 0.0, THE_TWO_PI);
  if (!aSol.IsDone())
  {
    return;
  }

  if (aSol.InfiniteRoots())
  {
    myIsPar         = true;
    const gp_Pnt aP = ElCLib::EllipseValue(0.0, C2.Position(), C2.MajorRadius(), C2.MinorRadius());
    mySqDist[0]     = C1.SquareDistance(aP);
    myNbExt         = 1;
    myDone          = true;
    return;
  }

  // Store solutions.
  const int aNbSol = aSol.NbSolutions();
  for (int aNoSol = 1; aNoSol <= aNbSol; ++aNoSol)
  {
    const double aU2    = aSol.Value(aNoSol);
    const gp_Pnt aP2    = ElCLib::Value(aU2, C2);
    const double aU1    = gp_Vec(aO1, aP2).Dot(aD1);
    const gp_Pnt aP1    = ElCLib::Value(aU1, C1);
    mySqDist[myNbExt]   = aP1.SquareDistance(aP2);
    myPoint[myNbExt][0] = Extrema_POnCurv(aU1, aP1);
    myPoint[myNbExt][1] = Extrema_POnCurv(aU2, aP2);
    ++myNbExt;
  }
  myDone = true;
}

//=================================================================================================

Extrema_ExtElC::Extrema_ExtElC(const gp_Lin& C1, const gp_Hypr& C2)
    : myDone(false),
      myIsPar(false),
      myNbExt(0)
{
  // Find extrema between straight line C1 and hyperbola C2.
  // Uses polynomial equation solver math_DirectPolynomialRoots.
  initSqDistArray(mySqDist);

  // Calculate direction in the reference of the hyperbola.
  const gp_Dir aD1 = C1.Direction();

  const gp_Dir& aX2 = C2.XAxis().Direction();
  const gp_Dir& aY2 = C2.YAxis().Direction();
  const gp_Dir& aZ2 = C2.Axis().Direction();

  const gp_XYZ aDLocal = projectOnAxes(aD1, aX2, aY2, aZ2);
  const double aDx     = aDLocal.X();
  const double aDy     = aDLocal.Y();

  // Calculate V in hyperbola's reference frame.
  const gp_Pnt aO1   = C1.Location();
  const gp_Pnt aO2   = C2.Location();
  const gp_XYZ aO2O1 = projectOnAxes(gp_Vec(aO2, aO1), aX2, aY2, aZ2);

  const gp_XYZ aVxyz = (aDLocal * gp_Vec(aO2O1).Dot(gp_Dir(aDLocal))) - aO2O1;
  const double aVx   = aVxyz.X();
  const double aVy   = aVxyz.Y();

  // Calculate coefficients of the polynomial equation.
  const double aR  = C2.MajorRadius();
  const double ar  = C2.MinorRadius();
  const double aA  = -2.0 * aR * ar * aDx * aDy;
  const double aB  = -aR * aR * aDx * aDx - ar * ar * aDy * aDy + aR * aR + ar * ar;
  const double aA1 = aA + aB;
  const double aA2 = 2.0 * aR * aVx + 2.0 * ar * aVy;
  const double aA4 = -2.0 * aR * aVx + 2.0 * ar * aVy;
  const double aA5 = aA - aB;

  math_DirectPolynomialRoots aSol(aA1, aA2, 0.0, aA4, aA5);
  if (!aSol.IsDone())
  {
    return;
  }

  // Store solutions.
  const int aNbSol = aSol.NbSolutions();
  for (int aNoSol = 1; aNoSol <= aNbSol; ++aNoSol)
  {
    const double aV = aSol.Value(aNoSol);
    if (aV > 0.0)
    {
      const double aU2    = std::log(aV);
      const gp_Pnt aP2    = ElCLib::Value(aU2, C2);
      const double aU1    = gp_Vec(aO1, aP2).Dot(aD1);
      const gp_Pnt aP1    = ElCLib::Value(aU1, C1);
      mySqDist[myNbExt]   = aP1.SquareDistance(aP2);
      myPoint[myNbExt][0] = Extrema_POnCurv(aU1, aP1);
      myPoint[myNbExt][1] = Extrema_POnCurv(aU2, aP2);
      ++myNbExt;
    }
  }
  myDone = true;
}

//=================================================================================================

Extrema_ExtElC::Extrema_ExtElC(const gp_Lin& C1, const gp_Parab& C2)
    : myDone(false),
      myIsPar(false),
      myNbExt(0)
{
  // Find extreme distances between straight line C1 and parabola C2.
  // Uses polynomial equation solver math_DirectPolynomialRoots.
  initSqDistArray(mySqDist);

  // Calculate direction in the reference of the parabola.
  const gp_Dir aD1 = C1.Direction();

  const gp_Dir& aX2 = C2.XAxis().Direction();
  const gp_Dir& aY2 = C2.YAxis().Direction();
  const gp_Dir& aZ2 = C2.Axis().Direction();

  const gp_XYZ aDLocal = projectOnAxes(aD1, aX2, aY2, aZ2);
  const double aDx     = aDLocal.X();
  const double aDy     = aDLocal.Y();

  // Calculate V in parabola's reference frame.
  const gp_Pnt aO1   = C1.Location();
  const gp_Pnt aO2   = C2.Location();
  const gp_XYZ aO2O1 = projectOnAxes(gp_Vec(aO2, aO1), aX2, aY2, aZ2);
  const gp_XYZ aVxyz = (aDLocal * gp_Vec(aO2O1).Dot(gp_Dir(aDLocal))) - aO2O1;

  // Calculate coefficients of the polynomial equation.
  const double aP  = C2.Parameter();
  const double aA1 = (1.0 - aDx * aDx) / (2.0 * aP * aP);
  const double aA2 = -3.0 * aDx * aDy / (2.0 * aP);
  const double aA3 = 1.0 - aDy * aDy + aVxyz.X() / aP;
  const double aA4 = aVxyz.Y();

  math_DirectPolynomialRoots aSol(aA1, aA2, aA3, aA4);
  if (!aSol.IsDone())
  {
    return;
  }

  // Store solutions.
  const int aNbSol = aSol.NbSolutions();
  for (int aNoSol = 1; aNoSol <= aNbSol; ++aNoSol)
  {
    const double aU2    = aSol.Value(aNoSol);
    const gp_Pnt aP2    = ElCLib::Value(aU2, C2);
    const double aU1    = gp_Vec(aO1, aP2).Dot(aD1);
    const gp_Pnt aP1    = ElCLib::Value(aU1, C1);
    mySqDist[myNbExt]   = aP1.SquareDistance(aP2);
    myPoint[myNbExt][0] = Extrema_POnCurv(aU1, aP1);
    myPoint[myNbExt][1] = Extrema_POnCurv(aU2, aP2);
    ++myNbExt;
  }
  myDone = true;
}

//=================================================================================================

Extrema_ExtElC::Extrema_ExtElC(const gp_Circ& C1, const gp_Circ& C2)
    : myDone(false),
      myIsPar(false),
      myNbExt(0)
{
  // Find extreme distances between two circles C1 and C2.
  // Handles coplanar and non-coplanar cases.
  initSqDistArray(mySqDist);

  const double aTolA  = Precision::Angular();
  const double aTolD  = Precision::Confusion();
  const double aTolD2 = aTolD * aTolD;

  const gp_Pnt  aPc1 = C1.Location();
  const gp_Dir& aDc1 = C1.Axis().Direction();
  const gp_Pnt  aPc2 = C2.Location();
  const gp_Dir& aDc2 = C2.Axis().Direction();

  const gp_Pln aPlc1(aPc1, aDc1);
  const double aD2          = aPlc1.SquareDistance(aPc2);
  const bool   bIsSamePlane = aDc1.IsParallel(aDc2, aTolA) && aD2 < aTolD2;

  if (!bIsSamePlane)
  {
    // Non-coplanar circles: use numerical approach.
    myDone = computeNonCoplanarCircleExtrema(C1, C2, myNbExt, mySqDist, myPoint);
    return;
  }

  // Here, both circles are in the same plane.
  const double aDC2       = aPc1.SquareDistance(aPc2);
  const bool   bIsSameAxe = aDC2 < aTolD2;

  if (bIsSameAxe)
  {
    // Concentric circles.
    myIsPar          = true;
    myNbExt          = 1;
    myDone           = true;
    const double aDR = C1.Radius() - C2.Radius();
    mySqDist[0]      = aDR * aDR;
    return;
  }

  myDone = true;

  // Ensure aC1 has the larger radius.
  double  aR1 = C1.Radius();
  double  aR2 = C2.Radius();
  int     j1  = 0;
  int     j2  = 1;
  gp_Circ aC1 = C1;
  gp_Circ aC2 = C2;

  if (aR2 > aR1)
  {
    j1  = 1;
    j2  = 0;
    aC1 = C2;
    aC2 = C1;
    aR1 = aC1.Radius();
    aR2 = aC2.Radius();
  }

  const gp_Pnt aPc1L = aC1.Location();
  const gp_Pnt aPc2L = aC2.Location();
  const double aD12  = aPc1L.Distance(aPc2L);
  const gp_Vec aVec12(aPc1L, aPc2L);
  const gp_Dir aDir12(aVec12);

  // Four common solutions along the line connecting centers.
  myNbExt = 4;

  gp_Pnt aP11, aP12, aP21, aP22;
  aP11.SetXYZ(aPc1L.XYZ() - aR1 * aDir12.XYZ());
  aP12.SetXYZ(aPc1L.XYZ() + aR1 * aDir12.XYZ());
  aP21.SetXYZ(aPc2L.XYZ() - aR2 * aDir12.XYZ());
  aP22.SetXYZ(aPc2L.XYZ() + aR2 * aDir12.XYZ());

  const double aT11 = ElCLib::Parameter(aC1, aP11);
  const double aT12 = ElCLib::Parameter(aC1, aP12);
  const double aT21 = ElCLib::Parameter(aC2, aP21);
  const double aT22 = ElCLib::Parameter(aC2, aP22);

  // P11, P21
  myPoint[0][j1].SetValues(aT11, aP11);
  myPoint[0][j2].SetValues(aT21, aP21);
  mySqDist[0] = aP11.SquareDistance(aP21);

  // P11, P22
  myPoint[1][j1].SetValues(aT11, aP11);
  myPoint[1][j2].SetValues(aT22, aP22);
  mySqDist[1] = aP11.SquareDistance(aP22);

  // P12, P21
  myPoint[2][j1].SetValues(aT12, aP12);
  myPoint[2][j2].SetValues(aT21, aP21);
  mySqDist[2] = aP12.SquareDistance(aP21);

  // P12, P22
  myPoint[3][j1].SetValues(aT12, aP12);
  myPoint[3][j2].SetValues(aT22, aP22);
  mySqDist[3] = aP12.SquareDistance(aP22);

  // Check for intersections.
  const bool bOut = aD12 > (aR1 + aR2 + aTolD);
  const bool bIn  = aD12 < (aR1 - aR2 - aTolD);

  if (!bOut && !bIn)
  {
    // Circles intersect - compute intersection points.
    const double aAlpha = 0.5 * (aR1 * aR1 - aR2 * aR2 + aD12 * aD12) / aD12;
    double       aVal   = aR1 * aR1 - aAlpha * aAlpha;
    if (aVal < 0.0)
    {
      aVal = -aVal; // Handle numerical instability.
    }
    const double aBeta = std::sqrt(aVal);

    gp_Pnt aPt;
    aPt.SetXYZ(aPc1L.XYZ() + aAlpha * aDir12.XYZ());

    const gp_Dir aDLt = aDc1 ^ aDir12;
    gp_Pnt       aPL1, aPL2;
    aPL1.SetXYZ(aPt.XYZ() + aBeta * aDLt.XYZ());
    aPL2.SetXYZ(aPt.XYZ() - aBeta * aDLt.XYZ());

    const double aDist2  = aPL1.SquareDistance(aPL2);
    const bool   bNbExt6 = aDist2 > aTolD2;

    myNbExt = 5;
    double aT[2];
    aT[j1] = ElCLib::Parameter(aC1, aPL1);
    aT[j2] = ElCLib::Parameter(aC2, aPL1);
    myPoint[4][j1].SetValues(aT[j1], aPL1);
    myPoint[4][j2].SetValues(aT[j2], aPL1);
    mySqDist[4] = 0.0;

    if (bNbExt6)
    {
      myNbExt = 6;
      aT[j1]  = ElCLib::Parameter(aC1, aPL2);
      aT[j2]  = ElCLib::Parameter(aC2, aPL2);
      myPoint[5][j1].SetValues(aT[j1], aPL2);
      myPoint[5][j2].SetValues(aT[j2], aPL2);
      mySqDist[5] = 0.0;
    }
  }
}

//=================================================================================================

Standard_Boolean Extrema_ExtElC::IsDone() const
{
  return myDone;
}

//=================================================================================================

Standard_Boolean Extrema_ExtElC::IsParallel() const
{
  if (!IsDone())
  {
    throw StdFail_NotDone();
  }
  return myIsPar;
}

//=================================================================================================

Standard_Integer Extrema_ExtElC::NbExt() const
{
  if (!IsDone())
  {
    throw StdFail_NotDone();
  }
  return myNbExt;
}

//=================================================================================================

Standard_Real Extrema_ExtElC::SquareDistance(const Standard_Integer N) const
{
  if (N < 1 || N > NbExt())
  {
    throw Standard_OutOfRange();
  }

  return mySqDist[N - 1];
}

//=================================================================================================

void Extrema_ExtElC::Points(const Standard_Integer N,
                            Extrema_POnCurv&       P1,
                            Extrema_POnCurv&       P2) const
{
  if (N < 1 || N > NbExt())
  {
    throw Standard_OutOfRange();
  }

  P1 = myPoint[N - 1][0];
  P2 = myPoint[N - 1][1];
}
