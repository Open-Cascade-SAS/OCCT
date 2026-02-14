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

#include <ExtremaCC_LineParabola.hxx>

#include <ElCLib.hxx>
#include <ExtremaCC_Line.hxx>
#include <ExtremaCC_Parabola.hxx>
#include <gp_Vec.hxx>
#include <math_DirectPolynomialRoots.hxx>

#include <cmath>

//==================================================================================================

ExtremaCC_LineParabola::ExtremaCC_LineParabola(const gp_Lin& theLine, const gp_Parab& theParabola)
    : myLine(theLine),
      myParabola(theParabola),
      myDomain(std::nullopt)
{
}

//==================================================================================================

ExtremaCC_LineParabola::ExtremaCC_LineParabola(const gp_Lin&               theLine,
                                               const gp_Parab&             theParabola,
                                               const ExtremaCC::Domain2D& theDomain)
    : myLine(theLine),
      myParabola(theParabola),
      myDomain(theDomain)
{
}

//==================================================================================================

const ExtremaCC::Result& ExtremaCC_LineParabola::Perform(double                theTol,
                                                         ExtremaCC::SearchMode theMode) const
{
  (void)theMode; // All extrema are computed analytically

  myResult.Clear();

  // Transform to parabola's coordinate system
  gp_Dir        aD  = myLine.Direction();
  const gp_Dir  aD1 = aD; // Keep original for final projection
  const gp_Dir& aX2 = myParabola.XAxis().Direction();
  const gp_Dir& aY2 = myParabola.YAxis().Direction();
  const gp_Dir& aZ2 = myParabola.Axis().Direction();

  // Direction D in parabola's frame
  const double aDx = aD.Dot(aX2);
  const double aDy = aD.Dot(aY2);
  const double aDz = aD.Dot(aZ2);
  aD.SetCoord(aDx, aDy, aDz);

  // Vector O2O1 in parabola's frame
  const gp_Pnt& aO1 = myLine.Location();
  const gp_Pnt& aO2 = myParabola.Location();
  gp_Vec        aO2O1(aO2, aO1);
  aO2O1.SetCoord(aO2O1.Dot(aX2), aO2O1.Dot(aY2), aO2O1.Dot(aZ2));

  // V = (O2O1.D)*D - O2O1
  const gp_XYZ aVxyz = (aD.XYZ() * aO2O1.Dot(aD)) - aO2O1.XYZ();

  // Build coefficients for cubic polynomial equation
  // For parabola P(y) = (y^2/(2p), y, 0), T(y) = (y/p, 1, 0)
  const double aP  = myParabola.Parameter();
  const double aA1 = (1.0 - aDx * aDx) / (2.0 * aP * aP);
  const double aA2 = -3.0 * aDx * aDy / (2.0 * aP);
  const double aA3 = 1.0 - aDy * aDy + aVxyz.X() / aP;
  const double aA4 = aVxyz.Y();

  // Solve cubic polynomial: A1*y^3 + A2*y^2 + A3*y + A4 = 0
  math_DirectPolynomialRoots aSolver(aA1, aA2, aA3, aA4);

  if (!aSolver.IsDone())
  {
    myResult.Status = ExtremaCC::Status::NumericalError;
    return myResult;
  }

  // Store solutions
  const int aNbSol = aSolver.NbSolutions();
  for (int i = 1; i <= aNbSol; ++i)
  {
    const double aU2 = aSolver.Value(i); // Parameter on parabola
    const gp_Pnt aP2 = ElCLib::Value(aU2, myParabola);
    const double aU1 = gp_Vec(aO1, aP2).Dot(aD1); // Parameter on line

    addSolution(aU1, aU2, theTol);
  }

  myResult.Status = ExtremaCC::Status::OK;
  return myResult;
}

//==================================================================================================

const ExtremaCC::Result& ExtremaCC_LineParabola::PerformWithEndpoints(
  double                theTol,
  ExtremaCC::SearchMode theMode) const
{
  (void)Perform(theTol, theMode);

  // Add endpoint extrema if domain is bounded
  if (myResult.Status == ExtremaCC::Status::OK && myDomain.has_value())
  {
    ExtremaCC_Line     aEval1(myLine);
    ExtremaCC_Parabola aEval2(myParabola);
    ExtremaCC::AddEndpointExtrema(myResult, *myDomain, aEval1, aEval2, theTol, theMode);
  }

  return myResult;
}

//==================================================================================================

void ExtremaCC_LineParabola::addSolution(double theU1, double theU2, double theTol) const
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
  const gp_Pnt aP1 = ElCLib::Value(theU1, myLine);
  const gp_Pnt aP2 = ElCLib::Value(theU2, myParabola);

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
