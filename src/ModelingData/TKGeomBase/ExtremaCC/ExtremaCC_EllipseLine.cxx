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

#include <ExtremaCC_EllipseLine.hxx>

#include <ElCLib.hxx>
#include <ExtremaCC_Ellipse.hxx>
#include <ExtremaCC_Line.hxx>
#include <gp_Vec.hxx>
#include <MathRoot_Trig.hxx>

#include <cmath>

//==================================================================================================

ExtremaCC_EllipseLine::ExtremaCC_EllipseLine(const gp_Elips& theEllipse, const gp_Lin& theLine)
    : myEllipse(theEllipse),
      myLine(theLine),
      myDomain(std::nullopt)
{
}

//==================================================================================================

ExtremaCC_EllipseLine::ExtremaCC_EllipseLine(const gp_Elips&             theEllipse,
                                             const gp_Lin&               theLine,
                                             const ExtremaCC::Domain2D& theDomain)
    : myEllipse(theEllipse),
      myLine(theLine),
      myDomain(theDomain)
{
}

//==================================================================================================

const ExtremaCC::Result& ExtremaCC_EllipseLine::Perform(double                theTol,
                                                        ExtremaCC::SearchMode theMode) const
{
  (void)theMode; // All extrema are computed analytically

  myResult.Clear();

  // Transform to ellipse's coordinate system
  const gp_Dir& aD  = myLine.Direction();
  const gp_Dir& aX2 = myEllipse.XAxis().Direction();
  const gp_Dir& aY2 = myEllipse.YAxis().Direction();
  const gp_Dir& aZ2 = myEllipse.Axis().Direction();

  // Direction D in ellipse's frame
  const double aDx = aD.Dot(aX2);
  const double aDy = aD.Dot(aY2);
  const double aDz = aD.Dot(aZ2);
  gp_Dir       aDLocal(aDx, aDy, aDz);

  // Vector O2O1 in ellipse's frame
  const gp_Pnt& aO1 = myLine.Location();
  const gp_Pnt& aO2 = myEllipse.Location();
  gp_Vec        aO2O1(aO2, aO1);
  aO2O1.SetCoord(aO2O1.Dot(aX2), aO2O1.Dot(aY2), aO2O1.Dot(aZ2));

  // V = (O2O1.D)*D - O2O1
  const gp_XYZ aVxyz = (aDLocal.XYZ() * aO2O1.Dot(aDLocal)) - aO2O1.XYZ();

  // Build coefficients for trigonometric equation
  const double aMajR = myEllipse.MajorRadius();
  const double aMinR = myEllipse.MinorRadius();
  const double aR2   = aMajR * aMajR;
  const double ar2   = aMinR * aMinR;

  constexpr double aCoefTol = 1.0e-12;

  double aA5 = aMajR * aMinR * aDx * aDy;
  double aA1 = -2.0 * aA5;
  double aA2 = (aR2 * aDx * aDx - ar2 * aDy * aDy - aR2 + ar2) / 2.0;
  double aA3 = aMinR * aVxyz.Y();
  double aA4 = -aMajR * aVxyz.X();

  // Clean up near-zero coefficients
  if (std::abs(aA1) <= aCoefTol) aA1 = 0.0;
  if (std::abs(aA2) <= aCoefTol) aA2 = 0.0;
  if (std::abs(aA3) <= aCoefTol) aA3 = 0.0;
  if (std::abs(aA4) <= aCoefTol) aA4 = 0.0;
  if (std::abs(aA5) <= aCoefTol) aA5 = 0.0;

  // Solve using MathRoot::Trigonometric (modern solver)
  MathRoot::TrigResult aSolverResult = MathRoot::Trigonometric(aA1, aA2, aA3, aA4, aA5, 0.0, 2.0 * M_PI);

  if (!aSolverResult.IsDone())
  {
    if (aSolverResult.InfiniteRoots)
    {
      myResult.Status = ExtremaCC::Status::InfiniteSolutions;
      const gp_Pnt aP = ElCLib::Value(0.0, myEllipse);
      myResult.InfiniteSquareDistance = myLine.SquareDistance(aP);
      return myResult;
    }
    myResult.Status = ExtremaCC::Status::NumericalError;
    return myResult;
  }

  if (aSolverResult.InfiniteRoots)
  {
    // Parallel case - infinite solutions
    myResult.Status = ExtremaCC::Status::InfiniteSolutions;
    const gp_Pnt aP = ElCLib::Value(0.0, myEllipse);
    myResult.InfiniteSquareDistance = myLine.SquareDistance(aP);
    return myResult;
  }

  // Store solutions
  const int aNbSol = aSolverResult.NbRoots;
  for (int i = 0; i < aNbSol; ++i)
  {
    const double aU1 = aSolverResult.Roots[i];
    const gp_Pnt aP1 = ElCLib::Value(aU1, myEllipse);
    const double aU2 = gp_Vec(aO1, aP1).Dot(aD);

    addSolution(aU1, aU2, theTol);
  }

  myResult.Status = ExtremaCC::Status::OK;
  return myResult;
}

//==================================================================================================

const ExtremaCC::Result& ExtremaCC_EllipseLine::PerformWithEndpoints(
  double                theTol,
  ExtremaCC::SearchMode theMode) const
{
  (void)Perform(theTol, theMode);

  // Add endpoint extrema if domain is bounded
  if (myResult.Status == ExtremaCC::Status::OK && myDomain.has_value())
  {
    ExtremaCC_Ellipse aEval1(myEllipse, myDomain->Curve1);
    ExtremaCC_Line    aEval2(myLine, myDomain->Curve2);
    ExtremaCC::AddEndpointExtrema(myResult, *myDomain, aEval1, aEval2, theTol, theMode);
  }

  return myResult;
}

//==================================================================================================

void ExtremaCC_EllipseLine::addSolution(double theU1, double theU2, double theTol) const
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
  const gp_Pnt aP1 = ElCLib::Value(theU1, myEllipse);
  const gp_Pnt aP2 = ElCLib::Value(theU2, myLine);

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
  anExt.IsMinimum      = true;

  myResult.Extrema.Append(anExt);
}
