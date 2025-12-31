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

#include <ExtremaCC_CircleCircle.hxx>

#include <ElCLib.hxx>
#include <ExtremaCC_Circle.hxx>
#include <ExtremaCC_DistanceFunction.hxx>
#include <gp_Pln.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>

#include <cmath>

//==================================================================================================

ExtremaCC_CircleCircle::ExtremaCC_CircleCircle(const gp_Circ& theCircle1, const gp_Circ& theCircle2)
    : myCircle1(theCircle1),
      myCircle2(theCircle2),
      myDomain(std::nullopt)
{
}

//==================================================================================================

ExtremaCC_CircleCircle::ExtremaCC_CircleCircle(const gp_Circ&              theCircle1,
                                               const gp_Circ&              theCircle2,
                                               const ExtremaCC::Domain2D& theDomain)
    : myCircle1(theCircle1),
      myCircle2(theCircle2),
      myDomain(theDomain)
{
}

//==================================================================================================

const ExtremaCC::Result& ExtremaCC_CircleCircle::Perform(double                theTol,
                                                         ExtremaCC::SearchMode theMode) const
{
  (void)theMode; // All extrema are computed analytically

  myResult.Clear();

  // Try coplanar case first
  if (performCoplanar(theTol))
  {
    return myResult;
  }

  // Non-coplanar circles require numerical approach
  performNonCoplanar(theTol);

  return myResult;
}

//==================================================================================================

const ExtremaCC::Result& ExtremaCC_CircleCircle::PerformWithEndpoints(
  double                theTol,
  ExtremaCC::SearchMode theMode) const
{
  (void)Perform(theTol, theMode);

  // Add endpoint extrema if domain is bounded
  if (myResult.Status == ExtremaCC::Status::OK && myDomain.has_value())
  {
    ExtremaCC_Circle aEval1(myCircle1, myDomain->Curve1);
    ExtremaCC_Circle aEval2(myCircle2, myDomain->Curve2);
    ExtremaCC::AddEndpointExtrema(myResult, *myDomain, aEval1, aEval2, theTol, theMode);
  }

  return myResult;
}

//==================================================================================================

bool ExtremaCC_CircleCircle::performCoplanar(double theTol) const
{
  const double aTolA  = Precision::Angular();
  const double aTolD  = theTol;
  const double aTolD2 = aTolD * aTolD;

  const gp_Pnt& aPc1 = myCircle1.Location();
  const gp_Dir& aDc1 = myCircle1.Axis().Direction();
  const gp_Pnt& aPc2 = myCircle2.Location();
  const gp_Dir& aDc2 = myCircle2.Axis().Direction();

  // Check if circles are in the same plane
  gp_Pln        aPlc1(aPc1, aDc1);
  const double aD2          = aPlc1.SquareDistance(aPc2);
  const bool   bIsSamePlane = aDc1.IsParallel(aDc2, aTolA) && aD2 < aTolD2;

  if (!bIsSamePlane)
  {
    return false; // Not coplanar - use numerical algorithm
  }

  // Circles are coplanar
  const double aDC2       = aPc1.SquareDistance(aPc2);
  const bool   bIsSameAxe = aDC2 < aTolD2;

  if (bIsSameAxe)
  {
    // Concentric circles - infinite solutions at constant distance
    myResult.Status                 = ExtremaCC::Status::InfiniteSolutions;
    const double aDR                = myCircle1.Radius() - myCircle2.Radius();
    myResult.InfiniteSquareDistance = aDR * aDR;
    return true;
  }

  // Non-concentric coplanar circles
  // Ensure aC1 has the larger radius
  bool     aSwapped = false;
  gp_Circ  aC1 = myCircle1;
  gp_Circ  aC2 = myCircle2;
  double   aR1 = myCircle1.Radius();
  double   aR2 = myCircle2.Radius();

  if (aR2 > aR1)
  {
    aSwapped = true;
    aC1 = myCircle2;
    aC2 = myCircle1;
    std::swap(aR1, aR2);
  }

  const gp_Pnt& aP1 = aC1.Location();
  const gp_Pnt& aP2 = aC2.Location();
  const double  aD12 = aP1.Distance(aP2);

  const gp_Vec aVec12(aP1, aP2);
  const gp_Dir aDir12(aVec12);

  // Four extrema along line of centers
  gp_Pnt aP11, aP12, aP21, aP22;
  aP11.SetXYZ(aP1.XYZ() - aR1 * aDir12.XYZ());
  aP12.SetXYZ(aP1.XYZ() + aR1 * aDir12.XYZ());
  aP21.SetXYZ(aP2.XYZ() - aR2 * aDir12.XYZ());
  aP22.SetXYZ(aP2.XYZ() + aR2 * aDir12.XYZ());

  const double aT11 = ElCLib::Parameter(aC1, aP11);
  const double aT12 = ElCLib::Parameter(aC1, aP12);
  const double aT21 = ElCLib::Parameter(aC2, aP21);
  const double aT22 = ElCLib::Parameter(aC2, aP22);

  // Store solutions (swap back if needed)
  if (!aSwapped)
  {
    addSolution(aT11, aT21, theTol); // P11, P21
    addSolution(aT11, aT22, theTol); // P11, P22
    addSolution(aT12, aT21, theTol); // P12, P21
    addSolution(aT12, aT22, theTol); // P12, P22
  }
  else
  {
    addSolution(aT21, aT11, theTol); // P21, P11
    addSolution(aT22, aT11, theTol); // P22, P11
    addSolution(aT21, aT12, theTol); // P21, P12
    addSolution(aT22, aT12, theTol); // P22, P12
  }

  // Check for intersections
  const bool bOut = aD12 > (aR1 + aR2 + aTolD);
  const bool bIn  = aD12 < (aR1 - aR2 - aTolD);

  if (!bOut && !bIn)
  {
    // Circles intersect - find intersection points
    const double aAlpha = 0.5 * (aR1 * aR1 - aR2 * aR2 + aD12 * aD12) / aD12;
    double       aVal   = aR1 * aR1 - aAlpha * aAlpha;
    if (aVal < 0.0)
    {
      aVal = -aVal; // Numerical tolerance
    }
    const double aBeta = std::sqrt(aVal);

    gp_Pnt aPt;
    aPt.SetXYZ(aP1.XYZ() + aAlpha * aDir12.XYZ());

    const gp_Dir aDLt = aDc1 ^ aDir12;

    gp_Pnt aPL1, aPL2;
    aPL1.SetXYZ(aPt.XYZ() + aBeta * aDLt.XYZ());
    aPL2.SetXYZ(aPt.XYZ() - aBeta * aDLt.XYZ());

    const double aDist2    = aPL1.SquareDistance(aPL2);
    const bool   bTwoInter = aDist2 > aTolD2;

    double aT1_1 = ElCLib::Parameter(aC1, aPL1);
    double aT2_1 = ElCLib::Parameter(aC2, aPL1);

    if (!aSwapped)
    {
      addSolution(aT1_1, aT2_1, theTol);
    }
    else
    {
      addSolution(aT2_1, aT1_1, theTol);
    }

    if (bTwoInter)
    {
      double aT1_2 = ElCLib::Parameter(aC1, aPL2);
      double aT2_2 = ElCLib::Parameter(aC2, aPL2);

      if (!aSwapped)
      {
        addSolution(aT1_2, aT2_2, theTol);
      }
      else
      {
        addSolution(aT2_2, aT1_2, theTol);
      }
    }
  }

  myResult.Status = ExtremaCC::Status::OK;
  return true;
}

//==================================================================================================

void ExtremaCC_CircleCircle::performNonCoplanar(double theTol) const
{
  // For non-coplanar circles, use grid-based approach with Newton refinement

  constexpr int aNbSamples = 36; // 10 degree increments

  double aMinDist2 = RealLast();
  double aMaxDist2 = 0.0;
  double aMinU1 = 0.0, aMinU2 = 0.0;
  double aMaxU1 = 0.0, aMaxU2 = 0.0;

  // Domain bounds
  double aU1Min = 0.0, aU1Max = 2.0 * M_PI;
  double aU2Min = 0.0, aU2Max = 2.0 * M_PI;

  if (myDomain.has_value())
  {
    aU1Min = myDomain->Curve1.Min;
    aU1Max = myDomain->Curve1.Max;
    aU2Min = myDomain->Curve2.Min;
    aU2Max = myDomain->Curve2.Max;
  }

  // Coarse grid search
  for (int i = 0; i <= aNbSamples; ++i)
  {
    const double aU1 = aU1Min + (aU1Max - aU1Min) * i / aNbSamples;
    const gp_Pnt aP1 = ElCLib::Value(aU1, myCircle1);

    for (int j = 0; j <= aNbSamples; ++j)
    {
      const double aU2 = aU2Min + (aU2Max - aU2Min) * j / aNbSamples;
      const gp_Pnt aP2 = ElCLib::Value(aU2, myCircle2);

      const double aDist2 = aP1.SquareDistance(aP2);

      if (aDist2 < aMinDist2)
      {
        aMinDist2 = aDist2;
        aMinU1    = aU1;
        aMinU2    = aU2;
      }
      if (aDist2 > aMaxDist2)
      {
        aMaxDist2 = aDist2;
        aMaxU1    = aU1;
        aMaxU2    = aU2;
      }
    }
  }

  // Set up evaluators and distance function
  ExtremaCC::Domain1D aDom1 = myDomain.has_value() ? myDomain->Curve1 : ExtremaCC::Domain1D{0.0, 2.0 * M_PI};
  ExtremaCC::Domain1D aDom2 = myDomain.has_value() ? myDomain->Curve2 : ExtremaCC::Domain1D{0.0, 2.0 * M_PI};
  ExtremaCC_Circle aEval1(myCircle1, aDom1);
  ExtremaCC_Circle aEval2(myCircle2, aDom2);
  ExtremaCC_DistanceFunction<ExtremaCC_Circle, ExtremaCC_Circle> aDistFunc(aEval1, aEval2);

  // Newton refinement for minimum using modern solver
  ExtremaCC_Newton::Result aMinResult = ExtremaCC_Newton::Solve(
    aDistFunc, aMinU1, aMinU2, aU1Min, aU1Max, aU2Min, aU2Max, theTol);

  if (aMinResult.IsDone)
  {
    addSolution(aMinResult.U1, aMinResult.U2, theTol);
  }
  else
  {
    addSolution(aMinU1, aMinU2, theTol); // Fallback to grid point
  }

  // Newton refinement for maximum
  ExtremaCC_Newton::Result aMaxResult = ExtremaCC_Newton::Solve(
    aDistFunc, aMaxU1, aMaxU2, aU1Min, aU1Max, aU2Min, aU2Max, theTol);

  if (aMaxResult.IsDone)
  {
    addSolution(aMaxResult.U1, aMaxResult.U2, theTol);
  }
  else
  {
    addSolution(aMaxU1, aMaxU2, theTol); // Fallback to grid point
  }

  myResult.Status = ExtremaCC::Status::OK;
}

//==================================================================================================

void ExtremaCC_CircleCircle::addSolution(double theU1, double theU2, double theTol) const
{
  // Check bounds if domain is specified
  if (myDomain.has_value())
  {
    const bool aOutside1 = (theU1 < myDomain->Curve1.Min - theTol) ||
                           (theU1 > myDomain->Curve1.Max + theTol);
    const bool aOutside2 = (theU2 < myDomain->Curve2.Min - theTol) ||
                           (theU2 > myDomain->Curve2.Max + theTol);

    if (aOutside1 || aOutside2)
    {
      return; // Outside domain
    }

    // Clamp to bounds
    theU1 = std::max(myDomain->Curve1.Min, std::min(myDomain->Curve1.Max, theU1));
    theU2 = std::max(myDomain->Curve2.Min, std::min(myDomain->Curve2.Max, theU2));
  }

  // Compute points
  const gp_Pnt aP1 = ElCLib::Value(theU1, myCircle1);
  const gp_Pnt aP2 = ElCLib::Value(theU2, myCircle2);

  // Check for duplicate solutions
  constexpr double aDupTol = 1.0e-10;
  for (int i = 0; i < myResult.Extrema.Length(); ++i)
  {
    const auto& anExt = myResult.Extrema(i);
    if (std::abs(anExt.Parameter1 - theU1) < aDupTol &&
        std::abs(anExt.Parameter2 - theU2) < aDupTol)
    {
      return; // Duplicate
    }
  }

  // Store result
  ExtremaCC::ExtremumResult anExt;
  anExt.Parameter1     = theU1;
  anExt.Parameter2     = theU2;
  anExt.Point1         = aP1;
  anExt.Point2         = aP2;
  anExt.SquareDistance = aP1.SquareDistance(aP2);
  anExt.IsMinimum      = true; // Will be classified later if needed

  myResult.Extrema.Append(anExt);
}
