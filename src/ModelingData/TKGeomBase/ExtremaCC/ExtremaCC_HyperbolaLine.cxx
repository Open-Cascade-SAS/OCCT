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

#include <ExtremaCC_HyperbolaLine.hxx>

#include <ElCLib.hxx>
#include <ExtremaCC_Hyperbola.hxx>
#include <ExtremaCC_Line.hxx>
#include <gp_Vec.hxx>
#include <math_DirectPolynomialRoots.hxx>

#include <cmath>

//==================================================================================================

ExtremaCC_HyperbolaLine::ExtremaCC_HyperbolaLine(const gp_Hypr& theHyperbola,
                                                 const gp_Lin&  theLine)
    : myHyperbola(theHyperbola),
      myLine(theLine),
      myDomain(std::nullopt)
{
}

//==================================================================================================

ExtremaCC_HyperbolaLine::ExtremaCC_HyperbolaLine(const gp_Hypr&              theHyperbola,
                                                 const gp_Lin&               theLine,
                                                 const ExtremaCC::Domain2D& theDomain)
    : myHyperbola(theHyperbola),
      myLine(theLine),
      myDomain(theDomain)
{
}

//==================================================================================================

const ExtremaCC::Result& ExtremaCC_HyperbolaLine::Perform(double                theTol,
                                                          ExtremaCC::SearchMode theMode) const
{
  (void)theMode; // All extrema are computed analytically

  myResult.Clear();

  // Transform to hyperbola's coordinate system
  const gp_Dir& aD  = myLine.Direction();
  const gp_Dir& aX2 = myHyperbola.XAxis().Direction();
  const gp_Dir& aY2 = myHyperbola.YAxis().Direction();
  const gp_Dir& aZ2 = myHyperbola.Axis().Direction();

  // Direction D in hyperbola's frame
  const double aDx = aD.Dot(aX2);
  const double aDy = aD.Dot(aY2);
  const double aDz = aD.Dot(aZ2);
  gp_Dir       aDLocal(aDx, aDy, aDz);

  // Vector O2O1 in hyperbola's frame
  const gp_Pnt& aO1 = myLine.Location();
  const gp_Pnt& aO2 = myHyperbola.Location();
  gp_Vec        aO2O1(aO2, aO1);
  aO2O1.SetCoord(aO2O1.Dot(aX2), aO2O1.Dot(aY2), aO2O1.Dot(aZ2));

  // V = (O2O1.D)*D - O2O1
  const gp_XYZ aVxyz = (aDLocal.XYZ() * aO2O1.Dot(aDLocal)) - aO2O1.XYZ();
  const double aVx   = aVxyz.X();
  const double aVy   = aVxyz.Y();

  // Build coefficients for polynomial equation (degree 4 in v where v = exp(u))
  const double aR = myHyperbola.MajorRadius();
  const double ar = myHyperbola.MinorRadius();

  const double aa  = -2.0 * aR * ar * aDx * aDy;
  const double ab  = -aR * aR * aDx * aDx - ar * ar * aDy * aDy + aR * aR + ar * ar;
  const double aA1 = aa + ab;
  const double aA2 = 2.0 * aR * aVx + 2.0 * ar * aVy;
  const double aA4 = -2.0 * aR * aVx + 2.0 * ar * aVy;
  const double aA5 = aa - ab;

  // Solve degree-4 polynomial: A1*v^4 + A2*v^3 + 0*v^2 + A4*v + A5 = 0
  math_DirectPolynomialRoots aSolver(aA1, aA2, 0.0, aA4, aA5);

  if (!aSolver.IsDone())
  {
    myResult.Status = ExtremaCC::Status::NumericalError;
    return myResult;
  }

  // Store solutions
  const int aNbSol = aSolver.NbSolutions();
  for (int i = 1; i <= aNbSol; ++i)
  {
    const double aV = aSolver.Value(i);

    // Only positive v values correspond to real hyperbola parameters (v = exp(u))
    if (aV > 0.0)
    {
      const double aU1 = std::log(aV);
      const gp_Pnt aP1 = ElCLib::Value(aU1, myHyperbola);
      const double aU2 = gp_Vec(aO1, aP1).Dot(aD);

      addSolution(aU1, aU2, theTol);
    }
  }

  myResult.Status = ExtremaCC::Status::OK;
  return myResult;
}

//==================================================================================================

const ExtremaCC::Result& ExtremaCC_HyperbolaLine::PerformWithEndpoints(
  double                theTol,
  ExtremaCC::SearchMode theMode) const
{
  (void)Perform(theTol, theMode);

  // Add endpoint extrema if domain is bounded
  if (myResult.Status == ExtremaCC::Status::OK && myDomain.has_value())
  {
    ExtremaCC_Hyperbola aEval1(myHyperbola, myDomain->Curve1);
    ExtremaCC_Line      aEval2(myLine, myDomain->Curve2);
    ExtremaCC::AddEndpointExtrema(myResult, *myDomain, aEval1, aEval2, theTol, theMode);
  }

  return myResult;
}

//==================================================================================================

void ExtremaCC_HyperbolaLine::addSolution(double theU1, double theU2, double theTol) const
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
  const gp_Pnt aP1 = ElCLib::Value(theU1, myHyperbola);
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
