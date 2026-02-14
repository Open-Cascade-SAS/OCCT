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

#include <ExtremaCC_CircleEllipse.hxx>

#include <ElCLib.hxx>
#include <ExtremaCC_Circle.hxx>
#include <ExtremaCC_Ellipse.hxx>
#include <ExtremaCC_GridEvaluator2D.hxx>
#include <gp_Pln.hxx>
#include <Precision.hxx>

#include <cmath>

//==================================================================================================

ExtremaCC_CircleEllipse::ExtremaCC_CircleEllipse(const gp_Circ&  theCircle,
                                                 const gp_Elips& theEllipse)
    : myCircle(theCircle),
      myEllipse(theEllipse),
      myDomain(std::nullopt)
{
}

//==================================================================================================

ExtremaCC_CircleEllipse::ExtremaCC_CircleEllipse(const gp_Circ&              theCircle,
                                                 const gp_Elips&             theEllipse,
                                                 const ExtremaCC::Domain2D& theDomain)
    : myCircle(theCircle),
      myEllipse(theEllipse),
      myDomain(theDomain)
{
}

//==================================================================================================

const ExtremaCC::Result& ExtremaCC_CircleEllipse::Perform(double                theTol,
                                                          ExtremaCC::SearchMode theMode) const
{
  myResult.Clear();

  // Try coplanar fast-path for concentric circle and ellipse with aligned axes
  if (performCoplanar(theTol))
  {
    return myResult;
  }

  // General case: use grid-based numerical approach
  ExtremaCC::Domain2D aDomain;
  if (myDomain.has_value())
  {
    aDomain = *myDomain;
  }
  else
  {
    aDomain.Curve1 = {0.0, 2.0 * M_PI};
    aDomain.Curve2 = {0.0, 2.0 * M_PI};
  }

  // Create evaluators
  ExtremaCC_Circle  aEval1(myCircle, aDomain.Curve1);
  ExtremaCC_Ellipse aEval2(myEllipse, aDomain.Curve2);

  // Use grid-based algorithm
  ExtremaCC_GridEvaluator2D<ExtremaCC_Circle, ExtremaCC_Ellipse> aGridEval(aEval1,
                                                                           aEval2,
                                                                           aDomain);
  aGridEval.Perform(myResult, theTol, theMode);

  return myResult;
}

//==================================================================================================

const ExtremaCC::Result& ExtremaCC_CircleEllipse::PerformWithEndpoints(
  double                theTol,
  ExtremaCC::SearchMode theMode) const
{
  (void)Perform(theTol, theMode);

  // Add endpoint extrema if domain is bounded
  if (myResult.Status == ExtremaCC::Status::OK && myDomain.has_value())
  {
    ExtremaCC_Circle  aEval1(myCircle, myDomain->Curve1);
    ExtremaCC_Ellipse aEval2(myEllipse, myDomain->Curve2);
    ExtremaCC::AddEndpointExtrema(myResult, *myDomain, aEval1, aEval2, theTol, theMode);
  }

  return myResult;
}

//==================================================================================================

bool ExtremaCC_CircleEllipse::performCoplanar(double theTol) const
{
  const double aTolA  = Precision::Angular();
  const double aTolD  = theTol;
  const double aTolD2 = aTolD * aTolD;

  const gp_Pnt& aPc  = myCircle.Location();
  const gp_Dir& aDc  = myCircle.Axis().Direction();
  const gp_Pnt& aPe  = myEllipse.Location();
  const gp_Dir& aDe  = myEllipse.Axis().Direction();
  const gp_Dir& aXc  = myCircle.XAxis().Direction();
  const gp_Dir& aXe  = myEllipse.XAxis().Direction();

  // Check if circle and ellipse are in the same plane
  gp_Pln        aPlnC(aPc, aDc);
  const double  aD2e         = aPlnC.SquareDistance(aPe);
  const bool    bIsSamePlane = aDc.IsParallel(aDe, aTolA) && aD2e < aTolD2;

  if (!bIsSamePlane)
  {
    return false; // Not coplanar - use numerical algorithm
  }

  // Check if circle and ellipse are concentric
  const double aDC2        = aPc.SquareDistance(aPe);
  const bool   bConcentric = aDC2 < aTolD2;

  if (!bConcentric)
  {
    return false; // Non-concentric coplanar - use numerical algorithm
  }

  // Check if axes are aligned (circle X-axis parallel to ellipse X-axis)
  const bool bAlignedAxes = aXc.IsParallel(aXe, aTolA);

  if (!bAlignedAxes)
  {
    return false; // Concentric but rotated - use numerical algorithm
  }

  // Concentric circle and ellipse with aligned axes
  // Extrema occur at the ellipse axes: 0, pi/2, pi, 3pi/2
  // For the circle, the corresponding parameters are the same (both start from X-axis)
  // If axes are opposite, the circle parameter is shifted by pi

  const bool bSameDirection = aXc.Dot(aXe) > 0.0;

  // Extrema at ellipse axes: 0 (major), pi/2 (minor), pi (-major), 3pi/2 (-minor)
  constexpr double aParams[4] = {0.0, M_PI / 2.0, M_PI, 3.0 * M_PI / 2.0};

  for (int i = 0; i < 4; ++i)
  {
    const double aU2 = aParams[i]; // Parameter on ellipse
    // If X-axes are opposite, parameter on circle is shifted by pi
    const double aU1 = bSameDirection ? aParams[i] : std::fmod(aParams[i] + M_PI, 2.0 * M_PI);

    addSolution(aU1, aU2, theTol);
  }

  myResult.Status = ExtremaCC::Status::OK;
  return true;
}

//==================================================================================================

void ExtremaCC_CircleEllipse::addSolution(double theU1, double theU2, double theTol) const
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
  const gp_Pnt aP1 = ElCLib::Value(theU1, myCircle);
  const gp_Pnt aP2 = ElCLib::Value(theU2, myEllipse);

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
