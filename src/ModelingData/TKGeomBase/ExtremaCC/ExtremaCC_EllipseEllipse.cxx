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

#include <ExtremaCC_EllipseEllipse.hxx>

#include <ElCLib.hxx>
#include <ExtremaCC_Ellipse.hxx>
#include <ExtremaCC_GridEvaluator2D.hxx>
#include <gp_Pln.hxx>
#include <Precision.hxx>

#include <cmath>

//==================================================================================================

ExtremaCC_EllipseEllipse::ExtremaCC_EllipseEllipse(const gp_Elips& theEllipse1,
                                                   const gp_Elips& theEllipse2)
    : myEllipse1(theEllipse1),
      myEllipse2(theEllipse2),
      myDomain(std::nullopt)
{
}

//==================================================================================================

ExtremaCC_EllipseEllipse::ExtremaCC_EllipseEllipse(const gp_Elips&            theEllipse1,
                                                   const gp_Elips&            theEllipse2,
                                                   const ExtremaCC::Domain2D& theDomain)
    : myEllipse1(theEllipse1),
      myEllipse2(theEllipse2),
      myDomain(theDomain)
{
}

//==================================================================================================

const ExtremaCC::Result& ExtremaCC_EllipseEllipse::Perform(double                theTol,
                                                           ExtremaCC::SearchMode theMode) const
{
  myResult.Clear();

  // Try coplanar fast-path for concentric ellipses with aligned axes
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

  ExtremaCC_Ellipse aEval1(myEllipse1, aDomain.Curve1);
  ExtremaCC_Ellipse aEval2(myEllipse2, aDomain.Curve2);

  ExtremaCC_GridEvaluator2D<ExtremaCC_Ellipse, ExtremaCC_Ellipse> aGridEval(aEval1,
                                                                            aEval2,
                                                                            aDomain);
  aGridEval.Perform(myResult, theTol, theMode);

  return myResult;
}

//==================================================================================================

const ExtremaCC::Result& ExtremaCC_EllipseEllipse::PerformWithEndpoints(
  double                theTol,
  ExtremaCC::SearchMode theMode) const
{
  (void)Perform(theTol, theMode);

  if (myResult.Status == ExtremaCC::Status::OK && myDomain.has_value())
  {
    ExtremaCC_Ellipse aEval1(myEllipse1, myDomain->Curve1);
    ExtremaCC_Ellipse aEval2(myEllipse2, myDomain->Curve2);
    ExtremaCC::AddEndpointExtrema(myResult, *myDomain, aEval1, aEval2, theTol, theMode);
  }

  return myResult;
}

//==================================================================================================

bool ExtremaCC_EllipseEllipse::performCoplanar(double theTol) const
{
  const double aTolA  = Precision::Angular();
  const double aTolD  = theTol;
  const double aTolD2 = aTolD * aTolD;

  const gp_Pnt& aP1 = myEllipse1.Location();
  const gp_Dir& aD1 = myEllipse1.Axis().Direction();
  const gp_Pnt& aP2 = myEllipse2.Location();
  const gp_Dir& aD2 = myEllipse2.Axis().Direction();
  const gp_Dir& aX1 = myEllipse1.XAxis().Direction();
  const gp_Dir& aX2 = myEllipse2.XAxis().Direction();

  // Check if ellipses are in the same plane
  gp_Pln       aPln1(aP1, aD1);
  const double aD22         = aPln1.SquareDistance(aP2);
  const bool   bIsSamePlane = aD1.IsParallel(aD2, aTolA) && aD22 < aTolD2;

  if (!bIsSamePlane)
  {
    return false; // Not coplanar - use numerical algorithm
  }

  // Check if ellipses are concentric
  const double aDC2        = aP1.SquareDistance(aP2);
  const bool   bConcentric = aDC2 < aTolD2;

  if (!bConcentric)
  {
    return false; // Non-concentric coplanar - use numerical algorithm
  }

  // Check if axes are aligned (same X-axis direction or opposite)
  const bool bAlignedAxes = aX1.IsParallel(aX2, aTolA);

  if (!bAlignedAxes)
  {
    return false; // Concentric but rotated - use numerical algorithm
  }

  // Concentric ellipses with aligned axes: extrema at 0, pi/2, pi, 3pi/2
  // For aligned ellipses, the same parameter gives corresponding axis points
  // If X-axes are opposite, we need to adjust the second parameter

  const bool bSameDirection = aX1.Dot(aX2) > 0.0;

  // Extrema occur at the axes: 0 (major), pi/2 (minor), pi (-major), 3pi/2 (-minor)
  constexpr double aParams[4] = {0.0, M_PI / 2.0, M_PI, 3.0 * M_PI / 2.0};

  for (int i = 0; i < 4; ++i)
  {
    const double aU1 = aParams[i];
    // If X-axes are opposite, parameter on second ellipse is shifted by pi
    const double aU2 = bSameDirection ? aParams[i] : std::fmod(aParams[i] + M_PI, 2.0 * M_PI);

    addSolution(aU1, aU2, theTol);
  }

  myResult.Status = ExtremaCC::Status::OK;
  return true;
}

//==================================================================================================

void ExtremaCC_EllipseEllipse::addSolution(double theU1, double theU2, double theTol) const
{
  // Check bounds if domain is specified
  if (myDomain.has_value())
  {
    const bool aOutside1 =
      (theU1 < myDomain->Curve1.Min - theTol) || (theU1 > myDomain->Curve1.Max + theTol);
    const bool aOutside2 =
      (theU2 < myDomain->Curve2.Min - theTol) || (theU2 > myDomain->Curve2.Max + theTol);

    if (aOutside1 || aOutside2)
    {
      return; // Outside domain
    }

    // Clamp to bounds
    theU1 = std::max(myDomain->Curve1.Min, std::min(myDomain->Curve1.Max, theU1));
    theU2 = std::max(myDomain->Curve2.Min, std::min(myDomain->Curve2.Max, theU2));
  }

  // Compute points
  const gp_Pnt aP1 = ElCLib::Value(theU1, myEllipse1);
  const gp_Pnt aP2 = ElCLib::Value(theU2, myEllipse2);

  // Check for duplicate solutions
  constexpr double aDupTol = 1.0e-10;
  for (int i = 0; i < myResult.Extrema.Length(); ++i)
  {
    const auto& anExt = myResult.Extrema(i);
    if (std::abs(anExt.Parameter1 - theU1) < aDupTol
        && std::abs(anExt.Parameter2 - theU2) < aDupTol)
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
