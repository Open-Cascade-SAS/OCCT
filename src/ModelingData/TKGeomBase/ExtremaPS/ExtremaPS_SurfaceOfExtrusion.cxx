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

#include <ExtremaPS_SurfaceOfExtrusion.hxx>

#include <MathOpt_Brent.hxx>
#include <MathUtils_Core.hxx>

#include <cmath>

//==================================================================================================

ExtremaPS_SurfaceOfExtrusion::ExtremaPS_SurfaceOfExtrusion(
  const occ::handle<Geom_SurfaceOfLinearExtrusion>& theSurface)
    : mySurface(theSurface),
      myDomain(std::nullopt),
      myAdaptor(theSurface),
      myCurveExtrema(theSurface->BasisCurve())
{
  initCache();
}

//==================================================================================================

ExtremaPS_SurfaceOfExtrusion::ExtremaPS_SurfaceOfExtrusion(
  const occ::handle<Geom_SurfaceOfLinearExtrusion>& theSurface,
  const ExtremaPS::Domain2D&                        theDomain)
    : mySurface(theSurface),
      myDomain(theDomain),
      myAdaptor(theSurface),
      myCurveExtrema(theSurface->BasisCurve(), theDomain.UMin, theDomain.UMax)
{
  initCache();
}

//==================================================================================================

void ExtremaPS_SurfaceOfExtrusion::initCache()
{
  if (mySurface.IsNull())
  {
    return;
  }

  // Cache extrusion direction for fast computation
  const gp_Dir& aDir = mySurface->Direction();
  myDirX             = aDir.X();
  myDirY             = aDir.Y();
  myDirZ             = aDir.Z();
}

//==================================================================================================

gp_Pnt ExtremaPS_SurfaceOfExtrusion::Value(double theU, double theV) const
{
  return myAdaptor.Value(theU, theV);
}

//==================================================================================================

const ExtremaPS::Result& ExtremaPS_SurfaceOfExtrusion::Perform(const gp_Pnt&         theP,
                                                               double                theTol,
                                                               ExtremaPS::SearchMode theMode) const
{
  myResult.Clear();
  mySamples.Clear();
  myCandidates.Clear();

  if (mySurface.IsNull())
  {
    myResult.Status = ExtremaPS::Status::NotDone;
    return myResult;
  }

  const Handle(Geom_Curve)& aBasisCurve = mySurface->BasisCurve();

  // Get curve parameter bounds
  double aUMin = aBasisCurve->FirstParameter();
  double aUMax = aBasisCurve->LastParameter();
  if (myDomain.has_value())
  {
    aUMin = std::max(aUMin, myDomain->UMin);
    aUMax = std::min(aUMax, myDomain->UMax);
  }

  // For extrusion surfaces, the correct distance formula is:
  // dist^2(u) = |C(u) - P|^2 - ((C(u) - P).D)^2
  // This is the squared perpendicular distance from P to the line through C(u) with direction D.
  // The optimal V for each u is: V(u) = (P - C(u)).D

  // Functor class for MathOpt::Brent
  struct DistanceFunc
  {
    const gp_Pnt&                             myP;
    const Handle(Geom_Curve)&                 myCurve;
    double                                    myDirX, myDirY, myDirZ;
    const std::optional<ExtremaPS::Domain2D>& myDom;

    DistanceFunc(const gp_Pnt&                             theP,
                 const Handle(Geom_Curve)&                 theCurve,
                 double                                    theDirX,
                 double                                    theDirY,
                 double                                    theDirZ,
                 const std::optional<ExtremaPS::Domain2D>& theDomain)
        : myP(theP),
          myCurve(theCurve),
          myDirX(theDirX),
          myDirY(theDirY),
          myDirZ(theDirZ),
          myDom(theDomain)
    {
    }

    bool Value(double theU, double& theF) const
    {
      const gp_Pnt aCurvePt = myCurve->Value(theU);
      const double aWx      = aCurvePt.X() - myP.X();
      const double aWy      = aCurvePt.Y() - myP.Y();
      const double aWz      = aCurvePt.Z() - myP.Z();
      const double aW2      = aWx * aWx + aWy * aWy + aWz * aWz;
      const double aWD      = aWx * myDirX + aWy * myDirY + aWz * myDirZ;

      if (myDom.has_value())
      {
        double       aOptV = std::max(myDom->VMin, std::min(myDom->VMax, -aWD));
        const gp_Pnt aSurfPt(aCurvePt.X() + aOptV * myDirX,
                             aCurvePt.Y() + aOptV * myDirY,
                             aCurvePt.Z() + aOptV * myDirZ);
        theF = myP.SquareDistance(aSurfPt);
      }
      else
      {
        theF = aW2 - aWD * aWD;
      }
      return true;
    }
  };

  DistanceFunc aFunc(theP, aBasisCurve, myDirX, myDirY, myDirZ, myDomain);

  // Lambda to compute distance squared and optimal V at parameter u
  auto computeDistanceAndV = [&](double theU) -> std::pair<double, double> {
    const gp_Pnt aCurvePt = aBasisCurve->Value(theU);
    const double aWx      = aCurvePt.X() - theP.X();
    const double aWy      = aCurvePt.Y() - theP.Y();
    const double aWz      = aCurvePt.Z() - theP.Z();
    const double aW2      = aWx * aWx + aWy * aWy + aWz * aWz;
    const double aWD      = aWx * myDirX + aWy * myDirY + aWz * myDirZ;
    double       aOptV    = -aWD;

    if (myDomain.has_value())
    {
      aOptV = std::max(myDomain->VMin, std::min(myDomain->VMax, aOptV));
      const gp_Pnt aSurfPt(aCurvePt.X() + aOptV * myDirX,
                           aCurvePt.Y() + aOptV * myDirY,
                           aCurvePt.Z() + aOptV * myDirZ);
      return {theP.SquareDistance(aSurfPt), aOptV};
    }
    return {aW2 - aWD * aWD, aOptV};
  };

  // Sample the curve to find candidate extrema
  const int    aNSamples = ExtremaPS::THE_OTHER_SURFACE_NB_SAMPLES;
  const double aDU       = (aUMax - aUMin) / aNSamples;

  mySamples.SetIncrement(aNSamples + 1);

  for (int i = 0; i <= aNSamples; ++i)
  {
    const double aU       = aUMin + i * aDU;
    auto [aSqDist, aOptV] = computeDistanceAndV(aU);
    mySamples.Append({aU, aOptV, aSqDist});
  }

  // Find local minima and maxima from samples
  // Add endpoints as candidates
  myCandidates.Append(mySamples.First());
  if (aNSamples > 0)
  {
    myCandidates.Append(mySamples.Last());
  }

  // Find interior local extrema
  for (int i = 1; i < aNSamples; ++i)
  {
    const double aPrev = mySamples.Value(i - 1).SqDist;
    const double aCurr = mySamples.Value(i).SqDist;
    const double aNext = mySamples.Value(i + 1).SqDist;

    const bool aIsLocalMin = aCurr < aPrev && aCurr < aNext;
    const bool aIsLocalMax = aCurr > aPrev && aCurr > aNext;

    if (aIsLocalMin || aIsLocalMax)
    {
      myCandidates.Append(mySamples.Value(i));
    }
  }

  // Refine each candidate using Brent's method with very tight tolerance
  // Use squared tolerance for parameter refinement to achieve theTol accuracy in distance
  MathUtils::Config aBrentConfig(theTol * theTol, ExtremaPS::THE_MAX_GOLDEN_ITERATIONS);

  for (int iCand = 0; iCand < myCandidates.Length(); ++iCand)
  {
    Sample& aCand = myCandidates.ChangeValue(iCand);

    // Determine search interval
    const double aLeft  = std::max(aUMin, aCand.U - aDU);
    const double aRight = std::min(aUMax, aCand.U + aDU);

    // Use Brent's method for refinement
    MathUtils::ScalarResult aRes = MathOpt::Brent(aFunc, aLeft, aRight, aBrentConfig);

    if (aRes.Root.has_value())
    {
      auto [aRefinedSqDist, aRefV] = computeDistanceAndV(*aRes.Root);
      aCand.U                      = *aRes.Root;
      aCand.V                      = aRefV;
      aCand.SqDist                 = aRefinedSqDist;
    }
  }

  // Filter candidates by search mode and remove duplicates
  for (int iCand = 0; iCand < myCandidates.Length(); ++iCand)
  {
    const Sample& aCand = myCandidates.Value(iCand);

    // Determine if minimum or maximum by comparing with neighbors
    auto [aSqDistL, aVL] = computeDistanceAndV(std::max(aUMin, aCand.U - theTol));
    auto [aSqDistR, aVR] = computeDistanceAndV(std::min(aUMax, aCand.U + theTol));
    const bool aIsMin    = aCand.SqDist <= aSqDistL && aCand.SqDist <= aSqDistR;

    // Filter by mode
    if (theMode == ExtremaPS::SearchMode::Min && !aIsMin)
    {
      continue;
    }
    if (theMode == ExtremaPS::SearchMode::Max && aIsMin)
    {
      continue;
    }

    // Check for duplicates
    bool aIsDuplicate = false;
    for (int j = 0; j < myResult.Extrema.Length(); ++j)
    {
      if (std::abs(myResult.Extrema.Value(j).U - aCand.U) < theTol)
      {
        aIsDuplicate = true;
        break;
      }
    }
    if (aIsDuplicate)
    {
      continue;
    }

    // Compute surface point
    const gp_Pnt aCurvePt = aBasisCurve->Value(aCand.U);
    const gp_Pnt aSurfPt(aCurvePt.X() + aCand.V * myDirX,
                         aCurvePt.Y() + aCand.V * myDirY,
                         aCurvePt.Z() + aCand.V * myDirZ);

    ExtremaPS::ExtremumResult anExt;
    anExt.U              = aCand.U;
    anExt.V              = aCand.V;
    anExt.Point          = aSurfPt;
    anExt.SquareDistance = aCand.SqDist;
    anExt.IsMinimum      = aIsMin;
    myResult.Extrema.Append(anExt);
  }

  myResult.Status =
    myResult.Extrema.IsEmpty() ? ExtremaPS::Status::NoSolution : ExtremaPS::Status::OK;
  return myResult;
}

//==================================================================================================

const ExtremaPS::Result& ExtremaPS_SurfaceOfExtrusion::PerformWithBoundary(
  const gp_Pnt&         theP,
  double                theTol,
  ExtremaPS::SearchMode theMode) const
{
  // Start with interior extrema
  (void)Perform(theP, theTol, theMode);

  // If infinite solutions or natural domain (no boundaries), return immediately
  if (myResult.IsInfinite() || !myDomain.has_value())
  {
    return myResult;
  }

  // Bounded domain - add boundary extrema
  ExtremaPS::AddBoundaryExtrema(myResult, theP, *myDomain, *this, theTol, theMode);

  // Update status
  if (myResult.Extrema.IsEmpty())
  {
    myResult.Status = ExtremaPS::Status::NoSolution;
  }
  else
  {
    myResult.Status = ExtremaPS::Status::OK;
  }

  return myResult;
}
