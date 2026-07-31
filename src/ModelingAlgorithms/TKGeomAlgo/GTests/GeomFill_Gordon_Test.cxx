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

#include <BSplCLib.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomFill_Gordon.hxx>
#include <GeomFill_NetworkSurface.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray1.hxx>
#include <Precision.hxx>
#include <StdFail_NotDone.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

#include <cmath>

namespace
{

//! Helper: create a linear BSpline curve from two points, parametrized on [0,1].
occ::handle<Geom_BSplineCurve> makeLinearBSpline(const gp_Pnt& theP1, const gp_Pnt& theP2)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 2);
  aPoles(1) = theP1;
  aPoles(2) = theP2;

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 2;
  aMults(2) = 2;

  return new Geom_BSplineCurve(aPoles, aKnots, aMults, 1);
}

//! Helper: create a quadratic BSpline curve through 3 control points on [0,1].
occ::handle<Geom_BSplineCurve> makeQuadraticBSpline(const gp_Pnt& theP1,
                                                    const gp_Pnt& theP2,
                                                    const gp_Pnt& theP3,
                                                    const double  theFirstParameter = 0.0,
                                                    const double  theLastParameter  = 1.0)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles(1) = theP1;
  aPoles(2) = theP2;
  aPoles(3) = theP3;

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = theFirstParameter;
  aKnots(2) = theLastParameter;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 3;
  aMults(2) = 3;

  return new Geom_BSplineCurve(aPoles, aKnots, aMults, 2);
}

NCollection_Array2<gp_Pnt> makeIntersectionGrid(
  const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theProfiles,
  const NCollection_Array1<double>&                         theGuideParams)
{
  NCollection_Array2<gp_Pnt> aPoints(1,
                                     static_cast<int>(theGuideParams.Size()),
                                     1,
                                     static_cast<int>(theProfiles.Size()));
  for (size_t aProfileIdx = 0; aProfileIdx < theProfiles.Size(); ++aProfileIdx)
  {
    for (size_t aGuideIdx = 0; aGuideIdx < theGuideParams.Size(); ++aGuideIdx)
    {
      aPoints(static_cast<int>(aGuideIdx) + 1, static_cast<int>(aProfileIdx) + 1) =
        theProfiles.At(aProfileIdx)->Value(theGuideParams.At(aGuideIdx));
    }
  }
  return aPoints;
}

NCollection_Array2<double> makeUnitWeightGrid(
  const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theProfiles,
  const NCollection_Array1<double>&                         theGuideParams)
{
  NCollection_Array2<double> aWeights(1,
                                      static_cast<int>(theGuideParams.Size()),
                                      1,
                                      static_cast<int>(theProfiles.Size()));
  aWeights.Init(1.0);
  return aWeights;
}

occ::handle<Geom_BSplineCurve> makeRationalQuadraticBSpline(const gp_Pnt& theP1,
                                                            const gp_Pnt& theP2,
                                                            const gp_Pnt& theP3,
                                                            const double  theMiddleWeight)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles(1) = theP1;
  aPoles(2) = theP2;
  aPoles(3) = theP3;

  NCollection_Array1<double> aWeights(1, 3);
  aWeights(1) = 1.0;
  aWeights(2) = theMiddleWeight;
  aWeights(3) = 1.0;

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 3;
  aMults(2) = 3;

  return new Geom_BSplineCurve(aPoles, aWeights, aKnots, aMults, 2);
}

occ::handle<Geom_BSplineCurve> makeWeightedLinearBSpline(const gp_Pnt& theP1,
                                                         const gp_Pnt& theP2,
                                                         const double  theWeight)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 2);
  aPoles(1) = theP1;
  aPoles(2) = theP2;

  NCollection_Array1<double> aWeights(1, 2);
  aWeights(1) = theWeight;
  aWeights(2) = theWeight;

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 2;
  aMults(2) = 2;

  return new Geom_BSplineCurve(aPoles, aWeights, aKnots, aMults, 1, false, false);
}

occ::handle<Geom_BSplineCurve> makeRationalLineBSpline(const gp_Pnt& theP1,
                                                       const gp_Pnt& theP2,
                                                       const int     theDegree)
{
  NCollection_Array1<gp_Pnt> aPoles(1, theDegree + 1);
  NCollection_Array1<double> aWeights(1, theDegree + 1);
  for (int aPoleIdx = 1; aPoleIdx <= theDegree + 1; ++aPoleIdx)
  {
    const double aRatio = static_cast<double>(aPoleIdx - 1) / static_cast<double>(theDegree);
    aPoles(aPoleIdx)    = gp_Pnt(theP1.XYZ() * (1.0 - aRatio) + theP2.XYZ() * aRatio);
    aWeights(aPoleIdx)  = (aPoleIdx == (theDegree + 2) / 2) ? 0.5 : 1.0;
  }

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = theDegree + 1;
  aMults(2) = theDegree + 1;

  return new Geom_BSplineCurve(aPoles, aWeights, aKnots, aMults, theDegree);
}

occ::handle<Geom_BSplineCurve> makeRationalQuadraticLineBSpline(const gp_Pnt& theP1,
                                                                const gp_Pnt& theP2,
                                                                const double  theStartWeight,
                                                                const double  theMiddleWeight,
                                                                const double  theEndWeight)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles(1) = theP1;
  aPoles(2) = gp_Pnt((theP1.XYZ() + theP2.XYZ()) * 0.5);
  aPoles(3) = theP2;

  NCollection_Array1<double> aWeights(1, 3);
  aWeights(1) = theStartWeight;
  aWeights(2) = theMiddleWeight;
  aWeights(3) = theEndWeight;

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 3;
  aMults(2) = 3;

  return new Geom_BSplineCurve(aPoles, aWeights, aKnots, aMults, 2);
}

occ::handle<Geom_BSplineCurve> makePeriodicProfile(const double theY)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 5);
  aPoles(1) = gp_Pnt(1.0, theY, 0.0);
  aPoles(2) = gp_Pnt(0.309, theY, 0.951);
  aPoles(3) = gp_Pnt(-0.809, theY, 0.588);
  aPoles(4) = gp_Pnt(-0.809, theY, -0.588);
  aPoles(5) = gp_Pnt(0.309, theY, -0.951);

  NCollection_Array1<double> aKnots(1, 6);
  for (int aKnotIdx = 1; aKnotIdx <= 6; ++aKnotIdx)
  {
    aKnots(aKnotIdx) = 0.2 * static_cast<double>(aKnotIdx - 1);
  }

  NCollection_Array1<int> aMults(1, 6);
  aMults.Init(1);

  return new Geom_BSplineCurve(aPoles, aKnots, aMults, 3, true);
}

occ::handle<Geom_BSplineCurve> makePeriodicGuide(const double theX)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 5);
  aPoles(1) = gp_Pnt(theX, 1.0, 0.0);
  aPoles(2) = gp_Pnt(theX, 0.309, 0.951);
  aPoles(3) = gp_Pnt(theX, -0.809, 0.588);
  aPoles(4) = gp_Pnt(theX, -0.809, -0.588);
  aPoles(5) = gp_Pnt(theX, 0.309, -0.951);

  NCollection_Array1<double> aKnots(1, 6);
  for (int aKnotIdx = 1; aKnotIdx <= 6; ++aKnotIdx)
  {
    aKnots(aKnotIdx) = 0.2 * static_cast<double>(aKnotIdx - 1);
  }

  NCollection_Array1<int> aMults(1, 6);
  aMults.Init(1);

  return new Geom_BSplineCurve(aPoles, aKnots, aMults, 3, true);
}

occ::handle<Geom_BSplineCurve> makeClosedBSplineProfile(const double theY,
                                                        const double theXRadius,
                                                        const double theZRadius)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 9);
  aPoles(1) = gp_Pnt(theXRadius, theY, 0.0);
  aPoles(2) = gp_Pnt(theXRadius, theY, 0.7 * theZRadius);
  aPoles(3) = gp_Pnt(0.3 * theXRadius, theY, theZRadius);
  aPoles(4) = gp_Pnt(-0.7 * theXRadius, theY, 0.8 * theZRadius);
  aPoles(5) = gp_Pnt(-theXRadius, theY, 0.0);
  aPoles(6) = gp_Pnt(-0.7 * theXRadius, theY, -0.8 * theZRadius);
  aPoles(7) = gp_Pnt(0.3 * theXRadius, theY, -theZRadius);
  aPoles(8) = gp_Pnt(theXRadius, theY, -0.7 * theZRadius);
  aPoles(9) = gp_Pnt(theXRadius, theY, 0.0);

  NCollection_Array1<double> aKnots(1, 7);
  for (int aKnotIdx = 1; aKnotIdx <= 7; ++aKnotIdx)
  {
    aKnots(aKnotIdx) = static_cast<double>(aKnotIdx - 1) / 6.0;
  }

  NCollection_Array1<int> aMults(1, 7);
  aMults(1) = 4;
  aMults(7) = 4;
  for (int aKnotIdx = 2; aKnotIdx < 7; ++aKnotIdx)
  {
    aMults(aKnotIdx) = 1;
  }
  return new Geom_BSplineCurve(aPoles, aKnots, aMults, 3, false);
}

occ::handle<Geom_BSplineCurve> makeInterpolatedPeriodicProfile(const double theY,
                                                               const int    theSeamShift)
{
  constexpr int    THE_NB_POINTS = 8;
  constexpr double THE_PI        = 3.14159265358979323846;

  occ::handle<NCollection_HArray1<gp_Pnt>> aPoints =
    new NCollection_HArray1<gp_Pnt>(1, THE_NB_POINTS);
  for (int aPointIdx = 0; aPointIdx < THE_NB_POINTS; ++aPointIdx)
  {
    const double anAngle =
      2.0 * THE_PI * static_cast<double>(aPointIdx + theSeamShift) / THE_NB_POINTS;
    aPoints->SetValue(aPointIdx + 1, gp_Pnt(std::cos(anAngle), theY, std::sin(anAngle)));
  }

  GeomAPI_Interpolate anInterpolator(aPoints, true, Precision::Confusion());
  anInterpolator.Perform();
  return anInterpolator.Curve();
}

//! Helper: create a cubic BSpline through 4 interpolation points on [0,1].
occ::handle<Geom_BSplineCurve> makeCubicInterpBSpline(const gp_Pnt& theP1,
                                                      const gp_Pnt& theP2,
                                                      const gp_Pnt& theP3,
                                                      const gp_Pnt& theP4)
{
  occ::handle<NCollection_HArray1<gp_Pnt>> aPnts = new NCollection_HArray1<gp_Pnt>(1, 4);
  aPnts->SetValue(1, theP1);
  aPnts->SetValue(2, theP2);
  aPnts->SetValue(3, theP3);
  aPnts->SetValue(4, theP4);

  occ::handle<NCollection_HArray1<double>> aParams = new NCollection_HArray1<double>(1, 4);
  aParams->SetValue(1, 0.0);
  aParams->SetValue(2, 1.0 / 3.0);
  aParams->SetValue(3, 2.0 / 3.0);
  aParams->SetValue(4, 1.0);

  GeomAPI_Interpolate anInterp(aPnts, aParams, false, Precision::Confusion());
  anInterp.Perform();
  return anInterp.Curve();
}

//! Helper: create a sinusoidal BSpline curve via interpolation.
//! X varies linearly, Z = amplitude * sin(pi * t), Y = constY.
occ::handle<Geom_BSplineCurve> makeSineBSpline(double theXStart,
                                               double theXEnd,
                                               double theY,
                                               double theAmplitude,
                                               int    theNbPts = 11)
{
  occ::handle<NCollection_HArray1<gp_Pnt>> aPnts   = new NCollection_HArray1<gp_Pnt>(1, theNbPts);
  occ::handle<NCollection_HArray1<double>> aParams = new NCollection_HArray1<double>(1, theNbPts);

  for (int i = 1; i <= theNbPts; ++i)
  {
    double aT = static_cast<double>(i - 1) / static_cast<double>(theNbPts - 1);
    double aX = theXStart + aT * (theXEnd - theXStart);
    double aZ = theAmplitude * std::sin(M_PI * aT);
    aPnts->SetValue(i, gp_Pnt(aX, theY, aZ));
    aParams->SetValue(i, aT);
  }

  GeomAPI_Interpolate anInterp(aPnts, aParams, false, Precision::Confusion());
  anInterp.Perform();
  return anInterp.Curve();
}

occ::handle<Geom_BSplineCurve> makeMeanderingProfile(const double theY)
{
  occ::handle<NCollection_HArray1<gp_Pnt>> aPnts = new NCollection_HArray1<gp_Pnt>(1, 7);
  aPnts->SetValue(1, gp_Pnt(0.0, theY, 0.0));
  aPnts->SetValue(2, gp_Pnt(0.62, theY + 0.04, 0.01));
  aPnts->SetValue(3, gp_Pnt(0.38, theY - 0.04, -0.01));
  aPnts->SetValue(4, gp_Pnt(0.5, theY, 0.0));
  aPnts->SetValue(5, gp_Pnt(0.64, theY + 0.03, 0.01));
  aPnts->SetValue(6, gp_Pnt(0.44, theY - 0.02, -0.01));
  aPnts->SetValue(7, gp_Pnt(1.0, theY, 0.0));

  occ::handle<NCollection_HArray1<double>> aParams = new NCollection_HArray1<double>(1, 7);
  for (int aParamIdx = 1; aParamIdx <= 7; ++aParamIdx)
  {
    aParams->SetValue(aParamIdx, static_cast<double>(aParamIdx - 1) / 6.0);
  }

  GeomAPI_Interpolate anInterp(aPnts, aParams, false, Precision::Confusion());
  anInterp.Perform();
  return anInterp.Curve();
}

//! Helper: verify surface passes through a point at given parameters.
void verifyPointOnSurface(const occ::handle<Geom_BSplineSurface>& theSurf,
                          double                                  theU,
                          double                                  theV,
                          const gp_Pnt&                           theExpected,
                          double                                  theTol)
{
  gp_Pnt aSurfPt = theSurf->Value(theU, theV);
  double aDist   = aSurfPt.Distance(theExpected);
  EXPECT_LT(aDist, theTol) << "Surface point at (" << theU << ", " << theV << ") = (" << aSurfPt.X()
                           << ", " << aSurfPt.Y() << ", " << aSurfPt.Z()
                           << ") differs from expected (" << theExpected.X() << ", "
                           << theExpected.Y() << ", " << theExpected.Z() << ") by " << aDist;
}

using HeightFunction = double (*)(double, double);

double wavyHeight(const double theU, const double theV)
{
  return 0.35 * std::sin(M_PI * theU) * (0.4 + theV)
         + 0.12 * std::sin(2.0 * M_PI * theV) * (1.0 - theU);
}

double saddleHeight(const double theU, const double theV)
{
  return 0.6 * (theU - 0.5) * (theV - 0.5) + 0.15 * std::sin(M_PI * theU) * std::sin(M_PI * theV);
}

double rippleHeight(const double theU, const double theV)
{
  return 0.2 * std::sin(2.0 * M_PI * theU) * std::cos(M_PI * theV)
         + 0.1 * std::sin(3.0 * M_PI * theV);
}

struct AnalyticNetwork
{
  NCollection_Array1<occ::handle<Geom_Curve>> Profiles;
  NCollection_Array1<occ::handle<Geom_Curve>> Guides;

  AnalyticNetwork(const size_t   theNbProfiles,
                  const size_t   theNbGuides,
                  HeightFunction theHeight,
                  const gp_XYZ&  theOrigin = gp_XYZ(0.0, 0.0, 0.0),
                  const double   theScale  = 1.0)
      : Profiles(1, static_cast<int>(theNbProfiles)),
        Guides(1, static_cast<int>(theNbGuides))
  {
    for (size_t aProfileIdx = 0; aProfileIdx < theNbProfiles; ++aProfileIdx)
    {
      const double aV = static_cast<double>(aProfileIdx) / static_cast<double>(theNbProfiles - 1);
      occ::handle<NCollection_HArray1<gp_Pnt>> aPoints =
        new NCollection_HArray1<gp_Pnt>(1, static_cast<int>(theNbGuides));
      occ::handle<NCollection_HArray1<double>> aParams =
        new NCollection_HArray1<double>(1, static_cast<int>(theNbGuides));
      for (size_t aGuideIdx = 0; aGuideIdx < theNbGuides; ++aGuideIdx)
      {
        const double aU = static_cast<double>(aGuideIdx) / static_cast<double>(theNbGuides - 1);
        aPoints->SetValue(
          static_cast<int>(aGuideIdx) + 1,
          gp_Pnt(theOrigin + gp_XYZ(theScale * aU, theScale * aV, theScale * theHeight(aU, aV))));
        aParams->SetValue(static_cast<int>(aGuideIdx) + 1, aU);
      }
      GeomAPI_Interpolate anInterpolator(aPoints, aParams, false, Precision::Confusion());
      anInterpolator.Perform();
      Profiles.ChangeAt(aProfileIdx) = anInterpolator.Curve();
    }

    for (size_t aGuideIdx = 0; aGuideIdx < theNbGuides; ++aGuideIdx)
    {
      const double aU = static_cast<double>(aGuideIdx) / static_cast<double>(theNbGuides - 1);
      occ::handle<NCollection_HArray1<gp_Pnt>> aPoints =
        new NCollection_HArray1<gp_Pnt>(1, static_cast<int>(theNbProfiles));
      occ::handle<NCollection_HArray1<double>> aParams =
        new NCollection_HArray1<double>(1, static_cast<int>(theNbProfiles));
      for (size_t aProfileIdx = 0; aProfileIdx < theNbProfiles; ++aProfileIdx)
      {
        const double aV = static_cast<double>(aProfileIdx) / static_cast<double>(theNbProfiles - 1);
        aPoints->SetValue(
          static_cast<int>(aProfileIdx) + 1,
          gp_Pnt(theOrigin + gp_XYZ(theScale * aU, theScale * aV, theScale * theHeight(aU, aV))));
        aParams->SetValue(static_cast<int>(aProfileIdx) + 1, aV);
      }
      GeomAPI_Interpolate anInterpolator(aPoints, aParams, false, Precision::Confusion());
      anInterpolator.Perform();
      Guides.ChangeAt(aGuideIdx) = anInterpolator.Curve();
    }
  }
};

void verifyUniformNetworkInterpolation(const occ::handle<Geom_BSplineSurface>& theSurface,
                                       const AnalyticNetwork&                  theNetwork,
                                       const double                            theTolerance)
{
  ASSERT_FALSE(theSurface.IsNull());
  constexpr size_t THE_NB_SAMPLES = 32;
  const double     aUMin          = theSurface->UKnot(1);
  const double     aUMax          = theSurface->UKnot(theSurface->NbUKnots());
  const double     aVMin          = theSurface->VKnot(1);
  const double     aVMax          = theSurface->VKnot(theSurface->NbVKnots());
  for (size_t aProfileIdx = 0; aProfileIdx < theNetwork.Profiles.Size(); ++aProfileIdx)
  {
    const double aV = aVMin
                      + static_cast<double>(aProfileIdx)
                          / static_cast<double>(theNetwork.Profiles.Size() - 1) * (aVMax - aVMin);
    for (size_t aSampleIdx = 0; aSampleIdx <= THE_NB_SAMPLES; ++aSampleIdx)
    {
      const double aParam = static_cast<double>(aSampleIdx) / THE_NB_SAMPLES;
      const double aU     = aUMin + aParam * (aUMax - aUMin);
      EXPECT_LE(
        theSurface->Value(aU, aV).Distance(theNetwork.Profiles.At(aProfileIdx)->Value(aParam)),
        theTolerance);
    }
  }
  for (size_t aGuideIdx = 0; aGuideIdx < theNetwork.Guides.Size(); ++aGuideIdx)
  {
    const double aU = aUMin
                      + static_cast<double>(aGuideIdx)
                          / static_cast<double>(theNetwork.Guides.Size() - 1) * (aUMax - aUMin);
    for (size_t aSampleIdx = 0; aSampleIdx <= THE_NB_SAMPLES; ++aSampleIdx)
    {
      const double aParam = static_cast<double>(aSampleIdx) / THE_NB_SAMPLES;
      const double aV     = aVMin + aParam * (aVMax - aVMin);
      EXPECT_LE(theSurface->Value(aU, aV).Distance(theNetwork.Guides.At(aGuideIdx)->Value(aParam)),
                theTolerance);
    }
  }
}

} // namespace

// ============================================================================
// GeomFill_Gordon tests (high-level, automatic intersection detection)
// ============================================================================

TEST(GeomFill_Gordon, SimpleLineNetwork_ProducesValidSurface)
{
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 2);
  aProfiles(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 0.0, 1.0);
  aProfiles(2) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 1, 0), gp_Dir(1, 0, 0)), 0.0, 1.0);

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 2);
  aGuides(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0)), 0.0, 1.0);
  aGuides(2) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(1, 0, 0), gp_Dir(0, 1, 0)), 0.0, 1.0);

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone());
  EXPECT_EQ(aGordon.Status(), GeomFill_Gordon::ResultStatus::Done);
  EXPECT_EQ(aGordon.Report().Status, GeomFill_Gordon::ResultStatus::Done);
  EXPECT_EQ(aGordon.Report().FailedStage, GeomFill_Gordon::BuildStage::NotStarted);
  EXPECT_FALSE(aGordon.Report().IsApproximate);
  EXPECT_LE(aGordon.Report().MaxContactGap, Precision::Confusion());
  EXPECT_LE(aGordon.Report().MaxProfileDeviation, 1.0e-7);
  EXPECT_LE(aGordon.Report().MaxGuideDeviation, 1.0e-7);

  const occ::handle<Geom_BSplineSurface>& aSurf = aGordon.Surface();
  ASSERT_FALSE(aSurf.IsNull());

  verifyPointOnSurface(aSurf, 0.0, 0.0, gp_Pnt(0, 0, 0), 1.0e-3);
  verifyPointOnSurface(aSurf, 1.0, 0.0, gp_Pnt(1, 0, 0), 1.0e-3);
  verifyPointOnSurface(aSurf, 0.0, 1.0, gp_Pnt(0, 1, 0), 1.0e-3);
  verifyPointOnSurface(aSurf, 1.0, 1.0, gp_Pnt(1, 1, 0), 1.0e-3);
}

TEST(GeomFill_Gordon, EmptyInput_NotDone)
{
  NCollection_Array1<occ::handle<Geom_Curve>> aOneProfile(1, 1);
  aOneProfile(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 0.0, 1.0);
  NCollection_Array1<occ::handle<Geom_Curve>> aOneGuide(1, 1);
  aOneGuide(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0)), 0.0, 1.0);

  GeomFill_Gordon aGordon;
  aGordon.Init(aOneProfile, aOneGuide, Precision::Confusion());
  aGordon.Perform();

  EXPECT_FALSE(aGordon.IsDone());
  EXPECT_EQ(aGordon.Status(), GeomFill_Gordon::ResultStatus::InvalidInput);
}

TEST(GeomFill_Gordon, SingleProfileOrGuide_NotDone)
{
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 1);
  aProfiles(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 0.0, 1.0);

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 2);
  aGuides(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0)), 0.0, 1.0);
  aGuides(2) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(1, 0, 0), gp_Dir(0, 1, 0)), 0.0, 1.0);

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());
  aGordon.Perform();

  EXPECT_FALSE(aGordon.IsDone());
  EXPECT_EQ(aGordon.Status(), GeomFill_Gordon::ResultStatus::InvalidInput);
}

TEST(GeomFill_Gordon, NotDone_BeforePerform)
{
  GeomFill_Gordon aGordon;
  EXPECT_FALSE(aGordon.IsDone());
  EXPECT_EQ(aGordon.Status(), GeomFill_Gordon::ResultStatus::NotStarted);
  EXPECT_EQ(aGordon.Report().Status, GeomFill_Gordon::ResultStatus::NotStarted);
  EXPECT_EQ(aGordon.Report().FailedStage, GeomFill_Gordon::BuildStage::NotStarted);
  EXPECT_FALSE(aGordon.Report().IsApproximate);
  EXPECT_THROW((void)aGordon.Surface(), StdFail_NotDone);
}

TEST(GeomFill_Gordon, ReinitResetsReportAndSurface)
{
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 2);
  aProfiles(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 0.0, 1.0);
  aProfiles(2) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 1, 0), gp_Dir(1, 0, 0)), 0.0, 1.0);

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 2);
  aGuides(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0)), 0.0, 1.0);
  aGuides(2) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(1, 0, 0), gp_Dir(0, 1, 0)), 0.0, 1.0);

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone());
  EXPECT_EQ(aGordon.Report().Status, GeomFill_Gordon::ResultStatus::Done);
  ASSERT_FALSE(aGordon.Surface().IsNull());

  NCollection_Array1<occ::handle<Geom_Curve>> aOneProfile(1, 1);
  aOneProfile(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 0.0, 1.0);
  NCollection_Array1<occ::handle<Geom_Curve>> aOneGuide(1, 1);
  aOneGuide(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0)), 0.0, 1.0);

  aGordon.Init(aOneProfile, aOneGuide, Precision::Confusion());
  EXPECT_FALSE(aGordon.IsDone());
  EXPECT_EQ(aGordon.Status(), GeomFill_Gordon::ResultStatus::NotStarted);
  EXPECT_EQ(aGordon.Report().Status, GeomFill_Gordon::ResultStatus::NotStarted);
  EXPECT_EQ(aGordon.Report().FailedStage, GeomFill_Gordon::BuildStage::NotStarted);
  EXPECT_FALSE(aGordon.Report().IsApproximate);
  EXPECT_NEAR(aGordon.Report().MaxContactGap, 0.0, 1.0e-12);
  EXPECT_NEAR(aGordon.Report().MaxReparametrizationDeviation, 0.0, 1.0e-12);
  EXPECT_NEAR(aGordon.Report().MaxProfileDeviation, 0.0, 1.0e-12);
  EXPECT_NEAR(aGordon.Report().MaxGuideDeviation, 0.0, 1.0e-12);
  EXPECT_NEAR(aGordon.Report().MaxApproximationDeviation, 0.0, 1.0e-12);
  EXPECT_THROW((void)aGordon.Surface(), StdFail_NotDone);

  aGordon.Perform();
  EXPECT_FALSE(aGordon.IsDone());
  EXPECT_EQ(aGordon.Status(), GeomFill_Gordon::ResultStatus::InvalidInput);
  EXPECT_EQ(aGordon.Report().Status, GeomFill_Gordon::ResultStatus::InvalidInput);
  EXPECT_EQ(aGordon.Report().FailedStage, GeomFill_Gordon::BuildStage::InputConversion);
}

TEST(GeomFill_Gordon, DisjointNetworkReportsContactDiscoveryFailure)
{
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 2);
  aProfiles(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 0.0, 1.0);
  aProfiles(2) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 1, 0), gp_Dir(1, 0, 0)), 0.0, 1.0);

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 2);
  aGuides(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(2, 0, 0), gp_Dir(0, 1, 0)), 0.0, 1.0);
  aGuides(2) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(3, 0, 0), gp_Dir(0, 1, 0)), 0.0, 1.0);

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());
  aGordon.Perform();

  EXPECT_FALSE(aGordon.IsDone());
  EXPECT_FALSE(aGordon.IsApproximate());
  EXPECT_EQ(aGordon.Status(), GeomFill_Gordon::ResultStatus::IntersectionFailed);
  EXPECT_EQ(aGordon.Report().Status, GeomFill_Gordon::ResultStatus::IntersectionFailed);
  EXPECT_EQ(aGordon.Report().FailedStage, GeomFill_Gordon::BuildStage::ContactDiscovery);
  EXPECT_FALSE(aGordon.Report().IsApproximate);
  EXPECT_NEAR(aGordon.Report().MaxContactGap, 0.0, 1.0e-12);
  EXPECT_NEAR(aGordon.Report().MaxReparametrizationDeviation, 0.0, 1.0e-12);
  EXPECT_THROW((void)aGordon.Surface(), StdFail_NotDone);
}

TEST(GeomFill_Gordon, ParallelMode_DefaultSingleThreadAndMatchesParallelResult)
{
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 3);
  aProfiles(1) = makeQuadraticBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0.5, 0, 0.4), gp_Pnt(1, 0, 0));
  aProfiles(2) = makeQuadraticBSpline(gp_Pnt(0, 0.5, 0), gp_Pnt(0.5, 0.5, 0.6), gp_Pnt(1, 0.5, 0));
  aProfiles(3) = makeQuadraticBSpline(gp_Pnt(0, 1, 0), gp_Pnt(0.5, 1, 0.4), gp_Pnt(1, 1, 0));

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 2);
  aGuides(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 1, 0));
  aGuides(2) = makeLinearBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0));

  GeomFill_Gordon aSingleThread;
  EXPECT_FALSE(aSingleThread.IsParallelMode());
  aSingleThread.Init(aProfiles, aGuides, Precision::Confusion());
  aSingleThread.Perform();
  ASSERT_TRUE(aSingleThread.IsDone());

  GeomFill_Gordon aParallel;
  aParallel.SetParallelMode(true);
  EXPECT_TRUE(aParallel.IsParallelMode());
  aParallel.Init(aProfiles, aGuides, Precision::Confusion());
  aParallel.Perform();
  ASSERT_TRUE(aParallel.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurfSingle = aSingleThread.Surface();
  const occ::handle<Geom_BSplineSurface>& aSurfPar    = aParallel.Surface();

  const gp_Pnt aMidSingle = aSurfSingle->Value(0.5, 0.5);
  const gp_Pnt aMidPar    = aSurfPar->Value(0.5, 0.5);
  EXPECT_LT(aMidSingle.Distance(aMidPar), 1.0e-9);
}

TEST(GeomFill_Gordon, CurvedBSplineNetwork_ProducesValidSurface)
{
  // 3 quadratic profiles with Z-curvature + 2 linear guides.
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 3);
  aProfiles(1) = makeQuadraticBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0.5, 0, 0.4), gp_Pnt(1, 0, 0));
  aProfiles(2) = makeQuadraticBSpline(gp_Pnt(0, 0.5, 0), gp_Pnt(0.5, 0.5, 0.6), gp_Pnt(1, 0.5, 0));
  aProfiles(3) = makeQuadraticBSpline(gp_Pnt(0, 1, 0), gp_Pnt(0.5, 1, 0.4), gp_Pnt(1, 1, 0));

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 2);
  aGuides(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 1, 0));
  aGuides(2) = makeLinearBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0));

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurf = aGordon.Surface();
  ASSERT_FALSE(aSurf.IsNull());

  gp_Pnt aMidPt = aSurf->Value(0.5 * (aSurf->UKnot(1) + aSurf->UKnot(aSurf->NbUKnots())),
                               0.5 * (aSurf->VKnot(1) + aSurf->VKnot(aSurf->NbVKnots())));
  EXPECT_NEAR(aMidPt.Z(), 0.3, 1.0e-6) << "Surface should have Z-bump at center";
}

TEST(GeomFill_Gordon, NonAffineProfileReparametrization_PreservesProfilesAndGuideContinuity)
{
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 3);
  aProfiles(1) = makeCubicInterpBSpline(gp_Pnt(0, 0, 0),
                                        gp_Pnt(0.25, 0, 0.4),
                                        gp_Pnt(0.75, 0, -0.3),
                                        gp_Pnt(1, 0, 0));
  aProfiles(2) = makeCubicInterpBSpline(gp_Pnt(0, 0.5, 0),
                                        gp_Pnt(0.25, 0.5, 0.2),
                                        gp_Pnt(0.75, 0.5, -0.5),
                                        gp_Pnt(1, 0.5, 0));
  aProfiles(3) = makeCubicInterpBSpline(gp_Pnt(0, 1, 0),
                                        gp_Pnt(0.25, 1, 0.5),
                                        gp_Pnt(0.75, 1, -0.2),
                                        gp_Pnt(1, 1, 0));

  occ::handle<NCollection_HArray1<gp_Pnt>> aGuidePoints = new NCollection_HArray1<gp_Pnt>(1, 3);
  aGuidePoints->SetValue(1, aProfiles(1)->Value(0.4));
  aGuidePoints->SetValue(2, aProfiles(2)->Value(0.5));
  aGuidePoints->SetValue(3, aProfiles(3)->Value(0.6));
  occ::handle<NCollection_HArray1<double>> aGuideParams = new NCollection_HArray1<double>(1, 3);
  aGuideParams->SetValue(1, 0.0);
  aGuideParams->SetValue(2, 0.5);
  aGuideParams->SetValue(3, 1.0);
  GeomAPI_Interpolate anInterpolator(aGuidePoints, aGuideParams, false, Precision::Confusion());
  anInterpolator.Perform();
  ASSERT_TRUE(anInterpolator.IsDone());

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 3);
  aGuides(1) = makeLinearBSpline(aProfiles(1)->Value(0.0), aProfiles(3)->Value(0.0));
  aGuides(2) = anInterpolator.Curve();
  aGuides(3) = makeLinearBSpline(aProfiles(1)->Value(1.0), aProfiles(3)->Value(1.0));

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone()) << static_cast<int>(aGordon.Status());
  const occ::handle<Geom_BSplineSurface>& aSurface = aGordon.Surface();
  EXPECT_LE(aGordon.Report().MaxProfileDeviation, Precision::Confusion());
  EXPECT_LE(aGordon.Report().MaxGuideDeviation, Precision::Confusion());
  const double aMiddleProfileV = 0.5 * (aSurface->VKnot(1) + aSurface->VKnot(aSurface->NbVKnots()));
  constexpr int THE_NB_SAMPLES = 32;
  for (int aSampleIdx = 0; aSampleIdx <= THE_NB_SAMPLES; ++aSampleIdx)
  {
    const double aRatio = static_cast<double>(aSampleIdx) / THE_NB_SAMPLES;
    const double aU =
      aSurface->UKnot(1) + aRatio * (aSurface->UKnot(aSurface->NbUKnots()) - aSurface->UKnot(1));
    const double aProfile1Param = aRatio <= 0.5 ? 0.8 * aRatio : 1.2 * aRatio - 0.2;
    const double aProfile3Param = aRatio <= 0.5 ? 1.2 * aRatio : 0.2 + 0.8 * aRatio;

    EXPECT_LE(aSurface->Value(aU, aSurface->VKnot(1)).Distance(aProfiles(1)->Value(aProfile1Param)),
              Precision::Confusion());
    EXPECT_LE(aSurface->Value(aU, aMiddleProfileV).Distance(aProfiles(2)->Value(aRatio)),
              Precision::Confusion());
    EXPECT_LE(aSurface->Value(aU, aSurface->VKnot(aSurface->NbVKnots()))
                .Distance(aProfiles(3)->Value(aProfile3Param)),
              Precision::Confusion());
  }

  const double aGuideU = 0.5 * (aSurface->UKnot(1) + aSurface->UKnot(aSurface->NbUKnots()));
  for (int aSampleIdx = 0; aSampleIdx <= THE_NB_SAMPLES; ++aSampleIdx)
  {
    const double aRatio = static_cast<double>(aSampleIdx) / THE_NB_SAMPLES;
    const double aV =
      aSurface->VKnot(1) + aRatio * (aSurface->VKnot(aSurface->NbVKnots()) - aSurface->VKnot(1));
    EXPECT_LE(aSurface->Value(aGuideU, aV).Distance(aGuides(2)->Value(aRatio)),
              Precision::Confusion());
  }
}

TEST(GeomFill_Gordon, MixedCurveTypes_ProducesValidSurface)
{
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 2);
  aProfiles(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 0.0, 2.0);
  aProfiles(2) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 2, 0), gp_Dir(1, 0, 0)), 0.0, 2.0);

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 2);
  aGuides(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0)), 0.0, 2.0);
  aGuides(2) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(2, 0, 0), gp_Dir(0, 1, 0)), 0.0, 2.0);

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone());
  ASSERT_FALSE(aGordon.Surface().IsNull());

  verifyPointOnSurface(aGordon.Surface(),
                       aGordon.Surface()->UKnot(1),
                       aGordon.Surface()->VKnot(1),
                       gp_Pnt(0, 0, 0),
                       0.01);
}

TEST(GeomFill_Gordon, PeriodicBSplineProfiles_AreExpandedBeforeConstruction)
{
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 3);
  aProfiles(1) = makePeriodicProfile(0.0);
  aProfiles(2) = makePeriodicProfile(0.5);
  aProfiles(3) = makePeriodicProfile(1.0);
  ASSERT_TRUE(occ::down_cast<Geom_BSplineCurve>(aProfiles(1))->IsPeriodic());

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 3);
  aGuides(1) = makeLinearBSpline(aProfiles(1)->Value(0.0), aProfiles(3)->Value(0.0));
  aGuides(2) = makeLinearBSpline(aProfiles(1)->Value(0.5), aProfiles(3)->Value(0.5));
  aGuides(3) = makeLinearBSpline(aProfiles(1)->Value(1.0), aProfiles(3)->Value(1.0));

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone());
  ASSERT_FALSE(aGordon.Surface().IsNull());
  EXPECT_TRUE(aGordon.Surface()->IsUPeriodic());
}

TEST(GeomFill_Gordon, PeriodicBSplineGuides_AreExpandedBeforeConstruction)
{
  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 3);
  aGuides(1) = makePeriodicGuide(0.0);
  aGuides(2) = makePeriodicGuide(0.5);
  aGuides(3) = makePeriodicGuide(1.0);
  ASSERT_TRUE(occ::down_cast<Geom_BSplineCurve>(aGuides(1))->IsPeriodic());

  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 3);
  aProfiles(1) = makeLinearBSpline(aGuides(1)->Value(0.0), aGuides(3)->Value(0.0));
  aProfiles(2) = makeLinearBSpline(aGuides(1)->Value(0.5), aGuides(3)->Value(0.5));
  aProfiles(3) = makeLinearBSpline(aGuides(1)->Value(1.0), aGuides(3)->Value(1.0));

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone());
  ASSERT_FALSE(aGordon.Surface().IsNull());
  EXPECT_TRUE(aGordon.Surface()->IsVPeriodic());
}

TEST(GeomFill_Gordon, InterpolatedPeriodicProfilesWithAlignedSeams_InterpolateWithoutDeviation)
{
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 2);
  aProfiles(1) = makeInterpolatedPeriodicProfile(0.0, 0);
  aProfiles(2) = makeInterpolatedPeriodicProfile(1.0, 2);

  const occ::handle<Geom_BSplineCurve> aFirstProfile =
    occ::down_cast<Geom_BSplineCurve>(aProfiles(1));
  const occ::handle<Geom_BSplineCurve> aSecondProfile =
    occ::down_cast<Geom_BSplineCurve>(aProfiles(2));
  const gp_Pnt                aFirstSeam = aFirstProfile->Value(aFirstProfile->FirstParameter());
  GeomAPI_ProjectPointOnCurve aSeamProjection(gp_Pnt(aFirstSeam.X(), 1.0, aFirstSeam.Z()),
                                              aSecondProfile);
  ASSERT_GT(aSeamProjection.NbPoints(), 0);
  aSecondProfile->SetOrigin(aSeamProjection.LowerDistanceParameter(), Precision::Confusion());
  const double aFirstProfileMiddle =
    0.5 * (aFirstProfile->FirstParameter() + aFirstProfile->LastParameter());
  const double aSecondProfileMiddle =
    0.5 * (aSecondProfile->FirstParameter() + aSecondProfile->LastParameter());
  const gp_Pnt aSecondSeam   = aSecondProfile->Value(aSecondProfile->FirstParameter());
  const gp_Pnt aFirstMiddle  = aFirstProfile->Value(aFirstProfileMiddle);
  const gp_Pnt aSecondMiddle = aSecondProfile->Value(aSecondProfileMiddle);
  ASSERT_LE(aFirstSeam.Distance(gp_Pnt(aSecondSeam.X(), aFirstSeam.Y(), aSecondSeam.Z())),
            Precision::Confusion());
  ASSERT_LE(aFirstMiddle.Distance(gp_Pnt(aSecondMiddle.X(), aFirstMiddle.Y(), aSecondMiddle.Z())),
            Precision::Confusion());

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 3);
  aGuides(1) = makeLinearBSpline(aFirstProfile->Value(aFirstProfile->FirstParameter()),
                                 aSecondProfile->Value(aSecondProfile->FirstParameter()));
  aGuides(2) = makeLinearBSpline(aFirstProfile->Value(aFirstProfileMiddle),
                                 aSecondProfile->Value(aSecondProfileMiddle));
  aGuides(3) = makeLinearBSpline(aFirstProfile->Value(aFirstProfile->FirstParameter()),
                                 aSecondProfile->Value(aSecondProfile->FirstParameter()));

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone()) << static_cast<int>(aGordon.Status());
  const occ::handle<Geom_BSplineSurface>& aSurface = aGordon.Surface();
  EXPECT_TRUE(aSurface->IsUPeriodic());
  EXPECT_LE(aGordon.Report().MaxProfileDeviation, Precision::Confusion());
  EXPECT_LE(aGordon.Report().MaxGuideDeviation, Precision::Confusion());

  const double aMiddleV = 0.5 * (aSurface->VKnot(1) + aSurface->VKnot(aSurface->NbVKnots()));
  const Geom_Surface::ResD1 aFirstSeamD1 = aSurface->EvalD1(aSurface->UKnot(1), aMiddleV);
  const Geom_Surface::ResD1 aLastSeamD1 =
    aSurface->EvalD1(aSurface->UKnot(aSurface->NbUKnots()), aMiddleV);
  EXPECT_LE(aFirstSeamD1.D1U.Angle(aLastSeamD1.D1U), Precision::Angular());

  constexpr int THE_NB_SAMPLES = 16;
  for (int aSampleIdx = 0; aSampleIdx <= THE_NB_SAMPLES; ++aSampleIdx)
  {
    const double aRatio = static_cast<double>(aSampleIdx) / THE_NB_SAMPLES;
    const double aU =
      aSurface->UKnot(1) + aRatio * (aSurface->UKnot(aSurface->NbUKnots()) - aSurface->UKnot(1));
    const double aFirstProfileParam =
      aFirstProfile->FirstParameter()
      + aRatio * (aFirstProfile->LastParameter() - aFirstProfile->FirstParameter());
    const double aSecondProfileParam =
      aSecondProfile->FirstParameter()
      + aRatio * (aSecondProfile->LastParameter() - aSecondProfile->FirstParameter());
    EXPECT_LE(
      aSurface->Value(aU, aSurface->VKnot(1)).Distance(aFirstProfile->Value(aFirstProfileParam)),
      Precision::Confusion());
    EXPECT_LE(aSurface->Value(aU, aSurface->VKnot(aSurface->NbVKnots()))
                .Distance(aSecondProfile->Value(aSecondProfileParam)),
              Precision::Confusion());
  }
}

TEST(GeomFill_Gordon, MultipleIntersections_SelectsMonotoneBranch)
{
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 3);
  aProfiles(1) = makeMeanderingProfile(0.0);
  aProfiles(2) = makeMeanderingProfile(0.5);
  aProfiles(3) = makeMeanderingProfile(1.0);

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 3);
  aGuides(1) = makeLinearBSpline(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(0.0, 1.0, 0.0));
  aGuides(2) = makeLinearBSpline(gp_Pnt(0.5, 0.0, 0.0), gp_Pnt(0.5, 1.0, 0.0));
  aGuides(3) = makeLinearBSpline(gp_Pnt(1.0, 0.0, 0.0), gp_Pnt(1.0, 1.0, 0.0));

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, 1.0e-5);
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone()) << static_cast<int>(aGordon.Status());
  ASSERT_FALSE(aGordon.Surface().IsNull());
  EXPECT_LE(aGordon.Report().MaxProfileDeviation, 1.0e-5);
  EXPECT_LE(aGordon.Report().MaxGuideDeviation, 1.0e-5);
  verifyPointOnSurface(aGordon.Surface(), 0.0, 0.0, gp_Pnt(0.0, 0.0, 0.0), 1.0e-3);
  verifyPointOnSurface(aGordon.Surface(), 1.0, 0.0, gp_Pnt(1.0, 0.0, 0.0), 1.0e-3);
  verifyPointOnSurface(aGordon.Surface(), 0.0, 1.0, gp_Pnt(0.0, 1.0, 0.0), 1.0e-3);
  verifyPointOnSurface(aGordon.Surface(), 1.0, 1.0, gp_Pnt(1.0, 1.0, 0.0), 1.0e-3);

  const gp_Pnt aFirstCenter = aGordon.Surface()->Value(0.5, 0.5);
  aGordon.Perform();
  ASSERT_TRUE(aGordon.IsDone()) << static_cast<int>(aGordon.Status());
  EXPECT_LE(aFirstCenter.Distance(aGordon.Surface()->Value(0.5, 0.5)), Precision::Confusion());
}

TEST(GeomFill_Gordon, FourByThreeGrid_NonUniformParams)
{
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 4);
  aProfiles(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0.0, 0), gp_Dir(1, 0, 0)), 0.0, 1.0);
  aProfiles(2) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0.2, 0), gp_Dir(1, 0, 0)), 0.0, 1.0);
  aProfiles(3) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0.7, 0), gp_Dir(1, 0, 0)), 0.0, 1.0);
  aProfiles(4) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 1.0, 0), gp_Dir(1, 0, 0)), 0.0, 1.0);

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 3);
  aGuides(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0.0, 0, 0), gp_Dir(0, 1, 0)), 0.0, 1.0);
  aGuides(2) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0.3, 0, 0), gp_Dir(0, 1, 0)), 0.0, 1.0);
  aGuides(3) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(1.0, 0, 0), gp_Dir(0, 1, 0)), 0.0, 1.0);

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone());
  ASSERT_FALSE(aGordon.Surface().IsNull());
}

TEST(GeomFill_Gordon, SurfaceAlongInputCurves_IsAccurate)
{
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 2);
  aProfiles(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 0.0, 1.0);
  aProfiles(2) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 1, 0), gp_Dir(1, 0, 0)), 0.0, 1.0);

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 2);
  aGuides(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0)), 0.0, 1.0);
  aGuides(2) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(1, 0, 0), gp_Dir(0, 1, 0)), 0.0, 1.0);

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone());
  const occ::handle<Geom_BSplineSurface>& aSurf = aGordon.Surface();

  const double aUMin = aSurf->UKnot(1);
  const double aUMax = aSurf->UKnot(aSurf->NbUKnots());
  const double aVMin = aSurf->VKnot(1);
  const double aVMax = aSurf->VKnot(aSurf->NbVKnots());

  constexpr int THE_NB_SAMPLES = 20;

  // Along first profile (V = VMin): Y should be 0, Z should be 0.
  for (int i = 0; i <= THE_NB_SAMPLES; ++i)
  {
    double aU      = aUMin + i * (aUMax - aUMin) / THE_NB_SAMPLES;
    gp_Pnt aSurfPt = aSurf->Value(aU, aVMin);
    EXPECT_NEAR(aSurfPt.Y(), 0.0, 0.01);
    EXPECT_NEAR(aSurfPt.Z(), 0.0, 0.01);
  }

  // Along last profile (V = VMax): Y should be 1.
  for (int i = 0; i <= THE_NB_SAMPLES; ++i)
  {
    double aU      = aUMin + i * (aUMax - aUMin) / THE_NB_SAMPLES;
    gp_Pnt aSurfPt = aSurf->Value(aU, aVMax);
    EXPECT_NEAR(aSurfPt.Y(), 1.0, 0.01);
    EXPECT_NEAR(aSurfPt.Z(), 0.0, 0.01);
  }
}

TEST(GeomFill_Gordon, ReversedCurveNetwork_ProducesValidSurface)
{
  // Reversed profile and guide directions.
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 2);
  aProfiles(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(1, 0, 0), gp_Dir(-1, 0, 0)), 0.0, 1.0);
  aProfiles(2) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 1, 0), gp_Dir(1, 0, 0)), 0.0, 1.0);

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 2);
  aGuides(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0)), 0.0, 1.0);
  aGuides(2) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(1, 1, 0), gp_Dir(0, -1, 0)), 0.0, 1.0);

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurf = aGordon.Surface();
  ASSERT_FALSE(aSurf.IsNull());

  const double aUMin = aSurf->UKnot(1);
  const double aUMax = aSurf->UKnot(aSurf->NbUKnots());
  const double aVMin = aSurf->VKnot(1);
  const double aVMax = aSurf->VKnot(aSurf->NbVKnots());

  // All corners should span unit square despite reversed input.
  gp_Pnt aCorners[4] = {aSurf->Value(aUMin, aVMin),
                        aSurf->Value(aUMax, aVMin),
                        aSurf->Value(aUMin, aVMax),
                        aSurf->Value(aUMax, aVMax)};

  double aXMin = RealLast(), aXMax = -RealLast();
  double aYMin = RealLast(), aYMax = -RealLast();
  for (int i = 0; i < 4; ++i)
  {
    EXPECT_NEAR(aCorners[i].Z(), 0.0, 0.01);
    aXMin = std::min(aXMin, aCorners[i].X());
    aXMax = std::max(aXMax, aCorners[i].X());
    aYMin = std::min(aYMin, aCorners[i].Y());
    aYMax = std::max(aYMax, aCorners[i].Y());
  }
  EXPECT_NEAR(aXMin, 0.0, 0.01);
  EXPECT_NEAR(aXMax, 1.0, 0.01);
  EXPECT_NEAR(aYMin, 0.0, 0.01);
  EXPECT_NEAR(aYMax, 1.0, 0.01);
}

TEST(GeomFill_Gordon, CubicInterpolatedNetwork_ProducesValidSurface)
{
  // Cubic interpolated profiles (S-shaped in Z) + linear guides.
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 3);
  aProfiles(1) = makeCubicInterpBSpline(gp_Pnt(0, 0, 0),
                                        gp_Pnt(0.33, 0, 0.3),
                                        gp_Pnt(0.66, 0, -0.3),
                                        gp_Pnt(1, 0, 0));
  aProfiles(2) = makeCubicInterpBSpline(gp_Pnt(0, 0.5, 0),
                                        gp_Pnt(0.33, 0.5, 0.5),
                                        gp_Pnt(0.66, 0.5, -0.5),
                                        gp_Pnt(1, 0.5, 0));
  aProfiles(3) = makeCubicInterpBSpline(gp_Pnt(0, 1, 0),
                                        gp_Pnt(0.33, 1, 0.3),
                                        gp_Pnt(0.66, 1, -0.3),
                                        gp_Pnt(1, 1, 0));

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 2);
  aGuides(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 1, 0));
  aGuides(2) = makeLinearBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0));

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, 0.01);
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone());
  ASSERT_FALSE(aGordon.Surface().IsNull());

  // Surface should have non-trivial Z variation.
  const occ::handle<Geom_BSplineSurface>& aSurf = aGordon.Surface();
  double aUMid = 0.5 * (aSurf->UKnot(1) + aSurf->UKnot(aSurf->NbUKnots()));
  double aVMid = 0.5 * (aSurf->VKnot(1) + aSurf->VKnot(aSurf->NbVKnots()));

  // Verify some Z variation exists (the S-curve shape).
  gp_Pnt aP1 = aSurf->Value(aUMid * 0.5, aVMid);
  gp_Pnt aP2 = aSurf->Value(aUMid * 1.5, aVMid);
  EXPECT_NEAR(aP1.Z(), 0.6328125, 1.0e-9) << "S-curve first lobe should be preserved";
  EXPECT_NEAR(aP2.Z(), -0.6328125, 1.0e-9) << "S-curve second lobe should be preserved";
}

TEST(GeomFill_Gordon, SinusoidalProfiles_SurfacePreservesShape)
{
  // Sinusoidal profiles at different Y positions + linear guides.
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 3);
  aProfiles(1) = makeSineBSpline(0.0, 1.0, 0.0, 0.3);
  aProfiles(2) = makeSineBSpline(0.0, 1.0, 0.5, 0.5);
  aProfiles(3) = makeSineBSpline(0.0, 1.0, 1.0, 0.3);

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 2);
  aGuides(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 1, 0));
  aGuides(2) = makeLinearBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0));

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, 0.01);
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurf = aGordon.Surface();
  ASSERT_FALSE(aSurf.IsNull());

  // At the center of the surface (U=0.5), the sine peak should be maximum.
  double aUMid = 0.5 * (aSurf->UKnot(1) + aSurf->UKnot(aSurf->NbUKnots()));
  double aVMid = 0.5 * (aSurf->VKnot(1) + aSurf->VKnot(aSurf->NbVKnots()));

  gp_Pnt aMidPt = aSurf->Value(aUMid, aVMid);
  EXPECT_NEAR(aMidPt.Z(), 0.5, 0.05) << "Center should preserve middle sine amplitude";
}

TEST(GeomFill_Gordon, ThreeByThreeGrid_SurfaceInterpolatesAllCurves)
{
  // 3 profiles + 3 guides. Check that the Gordon surface interpolates all
  // input curves by dense sampling along each one.
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 3);
  aProfiles(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 0.0, 1.0);
  aProfiles(2) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0.5, 0), gp_Dir(1, 0, 0)), 0.0, 1.0);
  aProfiles(3) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 1, 0), gp_Dir(1, 0, 0)), 0.0, 1.0);

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 3);
  aGuides(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0)), 0.0, 1.0);
  aGuides(2) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0.5, 0, 0), gp_Dir(0, 1, 0)), 0.0, 1.0);
  aGuides(3) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(1, 0, 0), gp_Dir(0, 1, 0)), 0.0, 1.0);

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurf  = aGordon.Surface();
  const double                            aUMin  = aSurf->UKnot(1);
  const double                            aUMax  = aSurf->UKnot(aSurf->NbUKnots());
  const double                            aVMin  = aSurf->VKnot(1);
  const double                            aVMax  = aSurf->VKnot(aSurf->NbVKnots());
  constexpr int                           THE_NB = 10;

  // Profile 1 at V=VMin: Y~0.
  for (int i = 0; i <= THE_NB; ++i)
  {
    double aU = aUMin + i * (aUMax - aUMin) / THE_NB;
    EXPECT_NEAR(aSurf->Value(aU, aVMin).Y(), 0.0, 0.01);
  }

  // Profile 3 at V=VMax: Y~1.
  for (int i = 0; i <= THE_NB; ++i)
  {
    double aU = aUMin + i * (aUMax - aUMin) / THE_NB;
    EXPECT_NEAR(aSurf->Value(aU, aVMax).Y(), 1.0, 0.01);
  }

  // Guide 1 at U=UMin: X~0.
  for (int i = 0; i <= THE_NB; ++i)
  {
    double aV = aVMin + i * (aVMax - aVMin) / THE_NB;
    EXPECT_NEAR(aSurf->Value(aUMin, aV).X(), 0.0, 0.01);
  }

  // Guide 3 at U=UMax: X~1.
  for (int i = 0; i <= THE_NB; ++i)
  {
    double aV = aVMin + i * (aVMax - aVMin) / THE_NB;
    EXPECT_NEAR(aSurf->Value(aUMax, aV).X(), 1.0, 0.01);
  }
}

TEST(GeomFill_Gordon, ScaledGeometry_LargeCoordinates)
{
  // Large-scale geometry: coordinates in thousands.
  // Tests that scale-relative tolerances work correctly.
  const double aScale = 1000.0;

  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 2);
  aProfiles(1) =
    new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 0.0, aScale);
  aProfiles(2) =
    new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, aScale, 0), gp_Dir(1, 0, 0)), 0.0, aScale);

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 2);
  aGuides(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0)), 0.0, aScale);
  aGuides(2) =
    new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(aScale, 0, 0), gp_Dir(0, 1, 0)), 0.0, aScale);

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, 0.1);
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurf = aGordon.Surface();
  ASSERT_FALSE(aSurf.IsNull());

  double aUMid  = 0.5 * (aSurf->UKnot(1) + aSurf->UKnot(aSurf->NbUKnots()));
  double aVMid  = 0.5 * (aSurf->VKnot(1) + aSurf->VKnot(aSurf->NbVKnots()));
  gp_Pnt aMidPt = aSurf->Value(aUMid, aVMid);

  // Center of a 1000x1000 bilinear patch should be near (500, 500, 0).
  EXPECT_NEAR(aMidPt.X(), 500.0, 1.0);
  EXPECT_NEAR(aMidPt.Y(), 500.0, 1.0);
  EXPECT_NEAR(aMidPt.Z(), 0.0, 1.0);
}

TEST(GeomFill_Gordon, ScaledGeometry_SmallCoordinates)
{
  // Micro-scale geometry: coordinates in millionths.
  const double aScale = 1.0e-3;

  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 2);
  aProfiles(1) =
    new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 0.0, aScale);
  aProfiles(2) =
    new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, aScale, 0), gp_Dir(1, 0, 0)), 0.0, aScale);

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 2);
  aGuides(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0)), 0.0, aScale);
  aGuides(2) =
    new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(aScale, 0, 0), gp_Dir(0, 1, 0)), 0.0, aScale);

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, aScale * 1.0e-4);
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone());
  ASSERT_FALSE(aGordon.Surface().IsNull());
}

TEST(GeomFill_Gordon, AllReversedCurves_ProducesValidSurface)
{
  // Every single curve is reversed. The sort network should fix them all.
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 2);
  aProfiles(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(1, 0, 0), gp_Dir(-1, 0, 0)), 0.0, 1.0);
  aProfiles(2) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(1, 1, 0), gp_Dir(-1, 0, 0)), 0.0, 1.0);

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 2);
  aGuides(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 1, 0), gp_Dir(0, -1, 0)), 0.0, 1.0);
  aGuides(2) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(1, 1, 0), gp_Dir(0, -1, 0)), 0.0, 1.0);

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone());
  ASSERT_FALSE(aGordon.Surface().IsNull());
}

TEST(GeomFill_Gordon, ShuffledInputOrder_ProducesValidSurface)
{
  // Curves provided in non-sorted order. Sort network should handle it.
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 3);
  // Provide profiles out of order: y=1, y=0.5, y=0.
  aProfiles(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 1.0, 0), gp_Dir(1, 0, 0)), 0.0, 1.0);
  aProfiles(2) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0.5, 0), gp_Dir(1, 0, 0)), 0.0, 1.0);
  aProfiles(3) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0.0, 0), gp_Dir(1, 0, 0)), 0.0, 1.0);

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 2);
  // Guides also out of order: x=1, x=0.
  aGuides(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(1, 0, 0), gp_Dir(0, 1, 0)), 0.0, 1.0);
  aGuides(2) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0)), 0.0, 1.0);

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurf = aGordon.Surface();
  ASSERT_FALSE(aSurf.IsNull());

  // Despite shuffled order, surface should be a valid unit-square patch.
  double aUMin = aSurf->UKnot(1), aUMax = aSurf->UKnot(aSurf->NbUKnots());
  double aVMin = aSurf->VKnot(1), aVMax = aSurf->VKnot(aSurf->NbVKnots());
  gp_Pnt aMid = aSurf->Value(0.5 * (aUMin + aUMax), 0.5 * (aVMin + aVMax));
  EXPECT_NEAR(aMid.Z(), 0.0, 0.01);
}

TEST(GeomFill_Gordon, SaddleSurface_BothDirectionsCurved)
{
  // Saddle shape: profiles curve up in Z, guides curve down in Z.
  // This creates a hyperbolic paraboloid-like surface.
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 2);
  aProfiles(1) = makeQuadraticBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0.5, 0, 0.5), gp_Pnt(1, 0, 0));
  aProfiles(2) = makeQuadraticBSpline(gp_Pnt(0, 1, 0), gp_Pnt(0.5, 1, 0.5), gp_Pnt(1, 1, 0));

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 2);
  aGuides(1) = makeQuadraticBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 0.5, -0.5), gp_Pnt(0, 1, 0));
  aGuides(2) = makeQuadraticBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 0.5, -0.5), gp_Pnt(1, 1, 0));

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, 0.01);
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurf = aGordon.Surface();
  ASSERT_FALSE(aSurf.IsNull());

  // Surface should have Z > 0 in the profile hump direction and Z < 0 in guide hump direction,
  // creating a saddle-like shape. Just verify it's a valid non-planar surface.
  double aUMid     = 0.5 * (aSurf->UKnot(1) + aSurf->UKnot(aSurf->NbUKnots()));
  double aVMid     = 0.5 * (aSurf->VKnot(1) + aSurf->VKnot(aSurf->NbVKnots()));
  gp_Pnt aCenterPt = aSurf->Value(aUMid, aVMid);

  // The center is the superposition of profile hump (+0.5) and guide dip (-0.5).
  // Gordon formula may not give exactly 0, but it should be a well-defined surface.
  EXPECT_NEAR(aCenterPt.Z(), 0.0, 0.5)
    << "Saddle surface center Z should be moderate, got " << aCenterPt.Z();
}

TEST(GeomFill_Gordon, AsymmetricNetwork_FiveProfilesTwoGuides)
{
  // Highly asymmetric: 5 profiles but only 2 guides.
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 5);
  for (int i = 0; i < 5; ++i)
  {
    double aY = i * 0.25;
    aProfiles(i + 1) =
      new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, aY, 0), gp_Dir(1, 0, 0)), 0.0, 1.0);
  }

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 2);
  aGuides(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0)), 0.0, 1.0);
  aGuides(2) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(1, 0, 0), gp_Dir(0, 1, 0)), 0.0, 1.0);

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone());
  ASSERT_FALSE(aGordon.Surface().IsNull());
}

TEST(GeomFill_Gordon, AsymmetricNetwork_TwoProfilesFiveGuides)
{
  // Highly asymmetric: 2 profiles but 5 guides.
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 2);
  aProfiles(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 0.0, 1.0);
  aProfiles(2) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 1, 0), gp_Dir(1, 0, 0)), 0.0, 1.0);

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 5);
  for (int i = 0; i < 5; ++i)
  {
    double aX = i * 0.25;
    aGuides(i + 1) =
      new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(aX, 0, 0), gp_Dir(0, 1, 0)), 0.0, 1.0);
  }

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone());
  ASSERT_FALSE(aGordon.Surface().IsNull());
}

TEST(GeomFill_Gordon, NonPlanarNetwork_CurvesInBothDirections)
{
  // Both profiles and guides are curved in 3D (non-planar network).
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 2);
  aProfiles(1) = makeSineBSpline(0.0, 1.0, 0.0, 0.2);
  aProfiles(2) = makeSineBSpline(0.0, 1.0, 1.0, 0.2);

  // Guides: also sinusoidal but in Y direction with Z offset.
  occ::handle<NCollection_HArray1<gp_Pnt>> aGuidPnts1  = new NCollection_HArray1<gp_Pnt>(1, 11);
  occ::handle<NCollection_HArray1<gp_Pnt>> aGuidPnts2  = new NCollection_HArray1<gp_Pnt>(1, 11);
  occ::handle<NCollection_HArray1<double>> aGuidParams = new NCollection_HArray1<double>(1, 11);
  for (int i = 1; i <= 11; ++i)
  {
    double aT = (i - 1) / 10.0;
    // Guides go from Y=0 to Y=1. Z must match profiles at intersections.
    // At X=0: profile Z = sin(pi*0)*amp = 0 for both profiles.
    // At X=1: same.
    // So guide endpoints have Z=0.
    double aZ1 = 0.15 * std::sin(M_PI * aT);
    double aZ2 = 0.15 * std::sin(M_PI * aT);
    aGuidPnts1->SetValue(i, gp_Pnt(0.0, aT, aZ1));
    aGuidPnts2->SetValue(i, gp_Pnt(1.0, aT, aZ2));
    aGuidParams->SetValue(i, aT);
  }

  GeomAPI_Interpolate anInterp1(aGuidPnts1, aGuidParams, false, Precision::Confusion());
  anInterp1.Perform();
  GeomAPI_Interpolate anInterp2(aGuidPnts2, aGuidParams, false, Precision::Confusion());
  anInterp2.Perform();

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 2);
  aGuides(1) = anInterp1.Curve();
  aGuides(2) = anInterp2.Curve();

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, 0.01);
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurf = aGordon.Surface();
  ASSERT_FALSE(aSurf.IsNull());

  // Surface should have non-zero Z everywhere except at corners.
  double aUMid  = 0.5 * (aSurf->UKnot(1) + aSurf->UKnot(aSurf->NbUKnots()));
  double aVMid  = 0.5 * (aSurf->VKnot(1) + aSurf->VKnot(aSurf->NbVKnots()));
  gp_Pnt aMidPt = aSurf->Value(aUMid, aVMid);
  EXPECT_NEAR(aMidPt.Z(), 0.35, 0.05)
    << "Non-planar network should preserve combined profile/guide elevation";
}

TEST(GeomFill_Gordon, SurfaceContinuity_IsSmooth)
{
  // Verify that the resulting surface is at least C1 continuous by checking
  // that derivatives are smooth across the parameter domain.
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 3);
  aProfiles(1) = makeQuadraticBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0.5, 0, 0.3), gp_Pnt(1, 0, 0));
  aProfiles(2) = makeQuadraticBSpline(gp_Pnt(0, 0.5, 0), gp_Pnt(0.5, 0.5, 0.5), gp_Pnt(1, 0.5, 0));
  aProfiles(3) = makeQuadraticBSpline(gp_Pnt(0, 1, 0), gp_Pnt(0.5, 1, 0.3), gp_Pnt(1, 1, 0));

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 2);
  aGuides(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 1, 0));
  aGuides(2) = makeLinearBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0));

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, 0.01);
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurf = aGordon.Surface();
  const double                            aUMin = aSurf->UKnot(1);
  const double                            aUMax = aSurf->UKnot(aSurf->NbUKnots());
  const double                            aVMin = aSurf->VKnot(1);
  const double                            aVMax = aSurf->VKnot(aSurf->NbVKnots());

  // Check derivative continuity along U-isoparameter at V=0.5*(VMin+VMax).
  const double     aV            = 0.5 * (aVMin + aVMax);
  constexpr int    THE_NB        = 50;
  constexpr double THE_DERIV_TOL = 1.0; // Allow generous tolerance for derivative jumps.

  gp_Pnt       aPrevPt;
  gp_Vec       aPrevDU, aPrevDV;
  const double aStep = (aUMax - aUMin) / THE_NB;
  aSurf->D1(aUMin + 2.0 * aStep, aV, aPrevPt, aPrevDU, aPrevDV);

  for (int i = 3; i <= THE_NB - 2; ++i)
  {
    double aU = aUMin + i * aStep;
    gp_Pnt aPt;
    gp_Vec aDU, aDV;
    aSurf->D1(aU, aV, aPt, aDU, aDV);

    // Derivatives should change gradually, not jump.
    double aDUChange = aDU.Subtracted(aPrevDU).Magnitude();
    EXPECT_LT(aDUChange, THE_DERIV_TOL) << "DU jump at U=" << aU << ": " << aDUChange;

    aPrevDU = aDU;
    aPrevDV = aDV;
  }
}

TEST(GeomFill_Gordon, ArcLikeProfiles_NonRational)
{
  // Profiles are arc-like curves (interpolated semicircle shape) in XZ plane
  // at different Y positions. Non-rational since Gordon rejects rational curves.
  // Guides are straight lines connecting endpoints.

  // Build arc-like profiles via interpolation of semicircle-like points.
  auto makeArcProfile = [](double theY) -> occ::handle<Geom_BSplineCurve> {
    constexpr int                            THE_NB  = 9;
    occ::handle<NCollection_HArray1<gp_Pnt>> aPnts   = new NCollection_HArray1<gp_Pnt>(1, THE_NB);
    occ::handle<NCollection_HArray1<double>> aParams = new NCollection_HArray1<double>(1, THE_NB);
    for (int i = 1; i <= THE_NB; ++i)
    {
      double aT     = static_cast<double>(i - 1) / (THE_NB - 1);
      double aAngle = M_PI * aT;
      // Semicircle: X = 0.5 + 0.5*cos(pi - angle), Z = 0.3*sin(angle)
      double aX = 0.5 - 0.5 * std::cos(aAngle);
      double aZ = 0.3 * std::sin(aAngle);
      aPnts->SetValue(i, gp_Pnt(aX, theY, aZ));
      aParams->SetValue(i, aT);
    }
    GeomAPI_Interpolate anInterp(aPnts, aParams, false, Precision::Confusion());
    anInterp.Perform();
    return anInterp.Curve();
  };

  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 2);
  aProfiles(1) = makeArcProfile(0.0);
  aProfiles(2) = makeArcProfile(1.0);

  // Guides connect endpoints: at X=0 and X=1, Z=0.
  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 2);
  aGuides(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 1, 0));
  aGuides(2) = makeLinearBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0));

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, 0.01);
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurf = aGordon.Surface();
  ASSERT_FALSE(aSurf.IsNull());

  // The surface center should have positive Z (from the arc-like profiles).
  double aUMid  = 0.5 * (aSurf->UKnot(1) + aSurf->UKnot(aSurf->NbUKnots()));
  double aVMid  = 0.5 * (aSurf->VKnot(1) + aSurf->VKnot(aSurf->NbVKnots()));
  gp_Pnt aMidPt = aSurf->Value(aUMid, aVMid);
  EXPECT_NEAR(aMidPt.Z(), 0.3, 0.05) << "Surface center should match arc-like profile height";
}

TEST(GeomFill_Gordon, RepeatPerform_GivesSameResult)
{
  // Calling Perform() twice on the same initialized Gordon should produce
  // the same result (no state corruption).
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 2);
  aProfiles(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 0.0, 1.0);
  aProfiles(2) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 1, 0), gp_Dir(1, 0, 0)), 0.0, 1.0);

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 2);
  aGuides(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0)), 0.0, 1.0);
  aGuides(2) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(1, 0, 0), gp_Dir(0, 1, 0)), 0.0, 1.0);

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());

  // First perform.
  aGordon.Perform();
  ASSERT_TRUE(aGordon.IsDone());
  double aUMid1 =
    0.5 * (aGordon.Surface()->UKnot(1) + aGordon.Surface()->UKnot(aGordon.Surface()->NbUKnots()));
  double aVMid1 =
    0.5 * (aGordon.Surface()->VKnot(1) + aGordon.Surface()->VKnot(aGordon.Surface()->NbVKnots()));
  gp_Pnt aMid1 = aGordon.Surface()->Value(aUMid1, aVMid1);

  // Perform again without reinitializing. The algorithm should rebuild its
  // working curves from immutable input handles.
  aGordon.Perform();
  ASSERT_TRUE(aGordon.IsDone());
  double aUMid2 =
    0.5 * (aGordon.Surface()->UKnot(1) + aGordon.Surface()->UKnot(aGordon.Surface()->NbUKnots()));
  double aVMid2 =
    0.5 * (aGordon.Surface()->VKnot(1) + aGordon.Surface()->VKnot(aGordon.Surface()->NbVKnots()));
  gp_Pnt aMid2 = aGordon.Surface()->Value(aUMid2, aVMid2);

  EXPECT_NEAR(aMid1.Distance(aMid2), 0.0, Precision::Confusion())
    << "Repeated Perform should give same result";
  EXPECT_EQ(aGordon.Report().Status, GeomFill_Gordon::ResultStatus::Done);
}

TEST(GeomFill_Gordon, HighDensityNetwork_SixBySix)
{
  // 6 profiles + 6 guides - tests performance and correctness with denser grids.
  constexpr int                               THE_NB = 6;
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, THE_NB);
  for (int i = 0; i < THE_NB; ++i)
  {
    double aY = static_cast<double>(i) / (THE_NB - 1);
    aProfiles(i + 1) =
      new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, aY, 0), gp_Dir(1, 0, 0)), 0.0, 1.0);
  }

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, THE_NB);
  for (int j = 0; j < THE_NB; ++j)
  {
    double aX = static_cast<double>(j) / (THE_NB - 1);
    aGuides(j + 1) =
      new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(aX, 0, 0), gp_Dir(0, 1, 0)), 0.0, 1.0);
  }

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurf = aGordon.Surface();
  ASSERT_FALSE(aSurf.IsNull());

  // Verify all 36 intersection points.
  const double aUMin = aSurf->UKnot(1);
  const double aUMax = aSurf->UKnot(aSurf->NbUKnots());
  const double aVMin = aSurf->VKnot(1);
  const double aVMax = aSurf->VKnot(aSurf->NbVKnots());

  for (int i = 0; i < THE_NB; ++i)
  {
    for (int j = 0; j < THE_NB; ++j)
    {
      double aX      = static_cast<double>(j) / (THE_NB - 1);
      double aY      = static_cast<double>(i) / (THE_NB - 1);
      double aU      = aUMin + aX * (aUMax - aUMin);
      double aV      = aVMin + aY * (aVMax - aVMin);
      gp_Pnt aSurfPt = aSurf->Value(aU, aV);
      EXPECT_NEAR(aSurfPt.Z(), 0.0, 0.02)
        << "Intersection point (" << i << ", " << j << ") Z-deviation";
    }
  }
}

TEST(GeomFill_Gordon, QuadraticProfiles_SurfaceMatchesCurvesAtBoundaries)
{
  // Quadratic profiles with Z-bump + linear guides.
  // Verify the surface exactly matches the boundary profiles.
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 2);
  aProfiles(1) = makeQuadraticBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0.5, 0, 1.0), gp_Pnt(1, 0, 0));
  aProfiles(2) = makeQuadraticBSpline(gp_Pnt(0, 1, 0), gp_Pnt(0.5, 1, 1.0), gp_Pnt(1, 1, 0));

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 2);
  aGuides(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 1, 0));
  aGuides(2) = makeLinearBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0));

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, 0.01);
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurf = aGordon.Surface();
  const double                            aUMin = aSurf->UKnot(1);
  const double                            aUMax = aSurf->UKnot(aSurf->NbUKnots());
  const double                            aVMin = aSurf->VKnot(1);
  const double                            aVMax = aSurf->VKnot(aSurf->NbVKnots());

  // First profile (boundary V=VMin): Z should match quadratic bump.
  for (int i = 0; i <= 10; ++i)
  {
    double aT       = i / 10.0;
    double aU       = aUMin + aT * (aUMax - aUMin);
    gp_Pnt aSurfPt  = aSurf->Value(aU, aVMin);
    gp_Pnt aCurvePt = aProfiles(1)->Value(aT);
    EXPECT_LT(aSurfPt.Distance(aCurvePt), 0.05) << "Profile 1 deviation at t=" << aT;
  }

  // Last profile (boundary V=VMax).
  for (int i = 0; i <= 10; ++i)
  {
    double aT       = i / 10.0;
    double aU       = aUMin + aT * (aUMax - aUMin);
    gp_Pnt aSurfPt  = aSurf->Value(aU, aVMax);
    gp_Pnt aCurvePt = aProfiles(2)->Value(aT);
    EXPECT_LT(aSurfPt.Distance(aCurvePt), 0.05) << "Profile 2 deviation at t=" << aT;
  }
}

TEST(GeomFill_Gordon, OffsetPlane_NetworkAtNonZeroZ)
{
  // Network at Z=5 (not at origin). Tests that the algorithm works for
  // arbitrary positions, not just geometry centered at origin.
  const double aZ = 5.0;

  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 2);
  aProfiles(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0, aZ), gp_Dir(1, 0, 0)), 0.0, 1.0);
  aProfiles(2) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 1, aZ), gp_Dir(1, 0, 0)), 0.0, 1.0);

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 2);
  aGuides(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0, aZ), gp_Dir(0, 1, 0)), 0.0, 1.0);
  aGuides(2) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(1, 0, aZ), gp_Dir(0, 1, 0)), 0.0, 1.0);

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurf = aGordon.Surface();
  double aUMid  = 0.5 * (aSurf->UKnot(1) + aSurf->UKnot(aSurf->NbUKnots()));
  double aVMid  = 0.5 * (aSurf->VKnot(1) + aSurf->VKnot(aSurf->NbVKnots()));
  gp_Pnt aMidPt = aSurf->Value(aUMid, aVMid);

  EXPECT_NEAR(aMidPt.Z(), aZ, 0.01);
}

TEST(GeomFill_Gordon, TiltedPlane_DiagonalCurves)
{
  // Profiles and guides on a tilted plane (Z varies linearly with X and Y).
  // Expected surface: Z = X + Y.
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 2);
  aProfiles(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 1));
  aProfiles(2) = makeLinearBSpline(gp_Pnt(0, 1, 1), gp_Pnt(1, 1, 2));

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 2);
  aGuides(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 1, 1));
  aGuides(2) = makeLinearBSpline(gp_Pnt(1, 0, 1), gp_Pnt(1, 1, 2));

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurf = aGordon.Surface();
  const double                            aUMin = aSurf->UKnot(1);
  const double                            aUMax = aSurf->UKnot(aSurf->NbUKnots());
  const double                            aVMin = aSurf->VKnot(1);
  const double                            aVMax = aSurf->VKnot(aSurf->NbVKnots());

  // Sample multiple points and check Z = X + Y.
  for (int i = 0; i <= 5; ++i)
  {
    for (int j = 0; j <= 5; ++j)
    {
      double aU  = aUMin + i * (aUMax - aUMin) / 5.0;
      double aV  = aVMin + j * (aVMax - aVMin) / 5.0;
      gp_Pnt aPt = aSurf->Value(aU, aV);
      EXPECT_NEAR(aPt.Z(), aPt.X() + aPt.Y(), 0.05)
        << "Tilted plane: Z should equal X+Y at (" << aPt.X() << ", " << aPt.Y() << ")";
    }
  }
}

TEST(GeomFill_Gordon, RectangularNotSquare_2x3AspectRatio)
{
  // Rectangular domain with 2:3 aspect ratio.
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 2);
  aProfiles(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 0.0, 2.0);
  aProfiles(2) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 3, 0), gp_Dir(1, 0, 0)), 0.0, 2.0);

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 2);
  aGuides(1) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0)), 0.0, 3.0);
  aGuides(2) = new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(2, 0, 0), gp_Dir(0, 1, 0)), 0.0, 3.0);

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurf = aGordon.Surface();
  double aUMid  = 0.5 * (aSurf->UKnot(1) + aSurf->UKnot(aSurf->NbUKnots()));
  double aVMid  = 0.5 * (aSurf->VKnot(1) + aSurf->VKnot(aSurf->NbVKnots()));
  gp_Pnt aMidPt = aSurf->Value(aUMid, aVMid);

  EXPECT_NEAR(aMidPt.X(), 1.0, 0.1);
  EXPECT_NEAR(aMidPt.Y(), 1.5, 0.1);
}

TEST(GeomFill_Gordon, WavySurface_SinusoidalProfilesAndGuides)
{
  // Both profiles and guides have sinusoidal Z, creating a wavy surface.
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 3);
  aProfiles(1) = makeSineBSpline(0.0, 1.0, 0.0, 0.3);
  aProfiles(2) = makeSineBSpline(0.0, 1.0, 0.5, 0.6);
  aProfiles(3) = makeSineBSpline(0.0, 1.0, 1.0, 0.3);

  // Guides connect profile endpoints consistently.
  // At X=0: all profiles have Z=0. At X=1: all profiles have Z=0.
  // So guides at X=0 and X=1 are straight lines.
  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 2);
  aGuides(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 1, 0));
  aGuides(2) = makeLinearBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0));

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, 0.01);
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurf = aGordon.Surface();
  ASSERT_FALSE(aSurf.IsNull());

  // Center peak: middle profile has amplitude 0.6, so center Z should be high.
  double aUMid  = 0.5 * (aSurf->UKnot(1) + aSurf->UKnot(aSurf->NbUKnots()));
  double aVMid  = 0.5 * (aSurf->VKnot(1) + aSurf->VKnot(aSurf->NbVKnots()));
  gp_Pnt aMidPt = aSurf->Value(aUMid, aVMid);
  EXPECT_NEAR(aMidPt.Z(), 0.6, 0.05) << "Center should match middle sine amplitude";
}

TEST(GeomFill_Gordon, NetworkSurface_UClosedNetwork_ProducesUPeriodicSurface)
{
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aProfiles(1, 3);
  aProfiles(1) = makeQuadraticBSpline(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0), gp_Pnt(0, 0, 0));
  aProfiles(2) = makeQuadraticBSpline(gp_Pnt(0, 1, 0), gp_Pnt(1, 1, 1), gp_Pnt(0, 1, 0));
  aProfiles(3) = makeQuadraticBSpline(gp_Pnt(0, 2, 0), gp_Pnt(1, 2, 0), gp_Pnt(0, 2, 0));

  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aGuides(1, 3);
  aGuides(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 2, 0));
  aGuides(2) = makeLinearBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 2, 0));
  aGuides(3) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 2, 0));

  NCollection_Array1<double> aProfileParams(1, 3);
  aProfileParams(1) = 0.0;
  aProfileParams(2) = 0.5;
  aProfileParams(3) = 1.0;

  NCollection_Array1<double> aGuideParams(1, 3);
  aGuideParams(1) = 0.0;
  aGuideParams(2) = 0.5;
  aGuideParams(3) = 1.0;

  GeomFill_NetworkSurface aNetwork;
  aNetwork.Init(aProfiles,
                aGuides,
                aProfileParams,
                aGuideParams,
                makeIntersectionGrid(aProfiles, aGuideParams),
                makeUnitWeightGrid(aProfiles, aGuideParams),
                Precision::Confusion(),
                true,
                false);
  aNetwork.Perform();

  ASSERT_TRUE(aNetwork.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurf = aNetwork.Surface();
  ASSERT_FALSE(aSurf.IsNull());
  EXPECT_TRUE(aSurf->IsUPeriodic());
}

TEST(GeomFill_Gordon, NetworkSurface_UClosedNetwork_UsesInputToleranceForPeriodicity)
{
  constexpr double aSeamGap   = 1.0e-5;
  constexpr double aTolerance = 1.0e-4;

  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aProfiles(1, 3);
  aProfiles(1) = makeQuadraticBSpline(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0), gp_Pnt(aSeamGap, 0, 0));
  aProfiles(2) = makeQuadraticBSpline(gp_Pnt(0, 1, 0), gp_Pnt(1, 1, 1), gp_Pnt(aSeamGap, 1, 0));
  aProfiles(3) = makeQuadraticBSpline(gp_Pnt(0, 2, 0), gp_Pnt(1, 2, 0), gp_Pnt(aSeamGap, 2, 0));

  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aGuides(1, 3);
  aGuides(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 2, 0));
  aGuides(2) = makeLinearBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 2, 0));
  aGuides(3) = makeLinearBSpline(gp_Pnt(aSeamGap, 0, 0), gp_Pnt(aSeamGap, 2, 0));

  NCollection_Array1<double> aProfileParams(1, 3);
  aProfileParams(1) = 0.0;
  aProfileParams(2) = 0.5;
  aProfileParams(3) = 1.0;

  NCollection_Array1<double> aGuideParams(1, 3);
  aGuideParams(1) = 0.0;
  aGuideParams(2) = 0.5;
  aGuideParams(3) = 1.0;

  GeomFill_NetworkSurface aNetwork;
  aNetwork.Init(aProfiles,
                aGuides,
                aProfileParams,
                aGuideParams,
                makeIntersectionGrid(aProfiles, aGuideParams),
                makeUnitWeightGrid(aProfiles, aGuideParams),
                aTolerance,
                true,
                false);
  aNetwork.Perform();

  ASSERT_TRUE(aNetwork.IsDone()) << static_cast<int>(aNetwork.Status());
  EXPECT_TRUE(aNetwork.Surface()->IsUPeriodic());
}

TEST(GeomFill_Gordon, NetworkSurface_InvalidPreparedNetworkReportsStatus)
{
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aProfiles(1, 1);
  aProfiles(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0));

  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aGuides(1, 2);
  aGuides(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 1, 0));
  aGuides(2) = makeLinearBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0));

  NCollection_Array1<double> aProfileParams(1, 1);
  aProfileParams(1) = 0.0;

  NCollection_Array1<double> aGuideParams(1, 2);
  aGuideParams(1) = 0.0;
  aGuideParams(2) = 1.0;

  GeomFill_NetworkSurface aNetwork;
  aNetwork.Init(aProfiles,
                aGuides,
                aProfileParams,
                aGuideParams,
                makeIntersectionGrid(aProfiles, aGuideParams),
                makeUnitWeightGrid(aProfiles, aGuideParams),
                Precision::Confusion(),
                false,
                false);
  EXPECT_EQ(aNetwork.Status(), GeomFill_NetworkSurface::ResultStatus::NotStarted);

  aNetwork.Perform();

  EXPECT_FALSE(aNetwork.IsDone());
  EXPECT_EQ(aNetwork.Status(), GeomFill_NetworkSurface::ResultStatus::InvalidInput);
  EXPECT_THROW((void)aNetwork.Surface(), StdFail_NotDone);
}

TEST(GeomFill_Gordon, NetworkSurface_InvalidWeightsReportsInvalidInput)
{
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aProfiles(1, 2);
  aProfiles(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0));
  aProfiles(2) = makeLinearBSpline(gp_Pnt(0, 1, 0), gp_Pnt(1, 1, 0));

  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aGuides(1, 2);
  aGuides(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 1, 0));
  aGuides(2) = makeLinearBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0));

  NCollection_Array1<double> aProfileParams(1, 2);
  aProfileParams(1) = 0.0;
  aProfileParams(2) = 1.0;

  NCollection_Array1<double> aGuideParams(1, 2);
  aGuideParams(1) = 0.0;
  aGuideParams(2) = 1.0;

  NCollection_Array2<double> aWeights = makeUnitWeightGrid(aProfiles, aGuideParams);
  aWeights(2, 2)                      = 0.0;

  GeomFill_NetworkSurface aNetwork;
  aNetwork.Init(aProfiles,
                aGuides,
                aProfileParams,
                aGuideParams,
                makeIntersectionGrid(aProfiles, aGuideParams),
                aWeights,
                Precision::Confusion(),
                false,
                false);
  aNetwork.Perform();

  EXPECT_FALSE(aNetwork.IsDone());
  EXPECT_EQ(aNetwork.Status(), GeomFill_NetworkSurface::ResultStatus::InvalidInput);
}

TEST(GeomFill_Gordon, NetworkSurface_CompatibleRationalWeightsProducesRationalSurface)
{
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aProfiles(1, 3);
  aProfiles(1) =
    makeRationalQuadraticBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0.5, 0, 0), gp_Pnt(1, 0, 0), 0.5);
  aProfiles(2) =
    makeRationalQuadraticBSpline(gp_Pnt(0, 0.5, 0), gp_Pnt(0.5, 0.5, 0), gp_Pnt(1, 0.5, 0), 0.5);
  aProfiles(3) =
    makeRationalQuadraticBSpline(gp_Pnt(0, 1, 0), gp_Pnt(0.5, 1, 0), gp_Pnt(1, 1, 0), 0.5);

  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aGuides(1, 3);
  aGuides(1) = makeWeightedLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 1, 0), 1.0);
  aGuides(2) = makeWeightedLinearBSpline(gp_Pnt(0.5, 0, 0), gp_Pnt(0.5, 1, 0), 0.75);
  aGuides(3) = makeWeightedLinearBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0), 1.0);

  NCollection_Array1<double> aProfileParams(1, 3);
  aProfileParams(1) = 0.0;
  aProfileParams(2) = 0.5;
  aProfileParams(3) = 1.0;

  NCollection_Array1<double> aGuideParams(1, 3);
  aGuideParams(1) = 0.0;
  aGuideParams(2) = 0.5;
  aGuideParams(3) = 1.0;

  NCollection_Array2<double> aWeights(1, 3, 1, 3);
  for (int aProfileIdx = 1; aProfileIdx <= 3; ++aProfileIdx)
  {
    aWeights(1, aProfileIdx) = 1.0;
    aWeights(2, aProfileIdx) = 0.75;
    aWeights(3, aProfileIdx) = 1.0;
  }

  GeomFill_NetworkSurface aNetwork;
  aNetwork.Init(aProfiles,
                aGuides,
                aProfileParams,
                aGuideParams,
                makeIntersectionGrid(aProfiles, aGuideParams),
                aWeights,
                Precision::Confusion(),
                false,
                false);
  aNetwork.Perform();

  ASSERT_TRUE(aNetwork.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurface = aNetwork.Surface();
  EXPECT_TRUE(aSurface->IsURational() || aSurface->IsVRational());
}

TEST(GeomFill_Gordon, NetworkSurface_IncompatibleRationalFamilyReportsStatus)
{
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aProfiles(1, 2);
  aProfiles(1) =
    makeRationalQuadraticBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0.5, 0, 0), gp_Pnt(1, 0, 0), 0.5);
  aProfiles(2) = makeWeightedLinearBSpline(gp_Pnt(0, 1, 0), gp_Pnt(1, 1, 0), 1.0);

  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aGuides(1, 2);
  aGuides(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 1, 0));
  aGuides(2) = makeLinearBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0));

  NCollection_Array1<double> aProfileParams(1, 2);
  aProfileParams(1) = 0.0;
  aProfileParams(2) = 1.0;

  NCollection_Array1<double> aGuideParams(1, 2);
  aGuideParams(1) = 0.0;
  aGuideParams(2) = 1.0;

  GeomFill_NetworkSurface aNetwork;
  aNetwork.Init(aProfiles,
                aGuides,
                aProfileParams,
                aGuideParams,
                makeIntersectionGrid(aProfiles, aGuideParams),
                makeUnitWeightGrid(aProfiles, aGuideParams),
                Precision::Confusion(),
                false,
                false);
  aNetwork.Perform();

  EXPECT_FALSE(aNetwork.IsDone());
  EXPECT_EQ(aNetwork.Status(), GeomFill_NetworkSurface::ResultStatus::CurveCompatibilityFailed);
}

TEST(GeomFill_Gordon, NetworkSurface_RationalDegreeOverflowReportsStatus)
{
  constexpr int aDegree = 9;

  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aProfiles(1, 3);
  aProfiles(1) = makeRationalLineBSpline(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0), aDegree);
  aProfiles(2) = makeRationalLineBSpline(gp_Pnt(0, 0.5, 0), gp_Pnt(1, 0.5, 0), aDegree);
  aProfiles(3) = makeRationalLineBSpline(gp_Pnt(0, 1, 0), gp_Pnt(1, 1, 0), aDegree);

  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aGuides(1, 3);
  aGuides(1) = makeRationalLineBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 1, 0), aDegree);
  aGuides(2) = makeRationalLineBSpline(gp_Pnt(0.5, 0, 0), gp_Pnt(0.5, 1, 0), aDegree);
  aGuides(3) = makeRationalLineBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0), aDegree);

  NCollection_Array1<double> aProfileParams(1, 3);
  aProfileParams(1) = 0.0;
  aProfileParams(2) = 0.5;
  aProfileParams(3) = 1.0;

  NCollection_Array1<double> aGuideParams(1, 3);
  aGuideParams(1) = 0.0;
  aGuideParams(2) = 0.5;
  aGuideParams(3) = 1.0;

  GeomFill_NetworkSurface aNetwork;
  aNetwork.Init(aProfiles,
                aGuides,
                aProfileParams,
                aGuideParams,
                makeIntersectionGrid(aProfiles, aGuideParams),
                makeUnitWeightGrid(aProfiles, aGuideParams),
                Precision::Confusion(),
                false,
                false);
  aNetwork.Perform();

  EXPECT_FALSE(aNetwork.IsDone());
  EXPECT_EQ(aNetwork.Status(), GeomFill_NetworkSurface::ResultStatus::RationalDegreeOverflow);
}

TEST(GeomFill_Gordon, RationalProfilesWithPolynomialGuidesProducesSurface)
{
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 3);
  aProfiles(1) =
    makeRationalQuadraticBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0.5, 0, 0), gp_Pnt(1, 0, 0), 0.5);
  aProfiles(2) =
    makeRationalQuadraticBSpline(gp_Pnt(0, 0.5, 0), gp_Pnt(0.5, 0.5, 0), gp_Pnt(1, 0.5, 0), 0.5);
  aProfiles(3) =
    makeRationalQuadraticBSpline(gp_Pnt(0, 1, 0), gp_Pnt(0.5, 1, 0), gp_Pnt(1, 1, 0), 0.5);

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 3);
  aGuides(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 1, 0));
  aGuides(2) = makeLinearBSpline(gp_Pnt(0.5, 0, 0), gp_Pnt(0.5, 1, 0));
  aGuides(3) = makeLinearBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0));

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone());
  EXPECT_FALSE(aGordon.IsApproximate());
  EXPECT_TRUE(aGordon.Surface()->IsURational() || aGordon.Surface()->IsVRational());
  EXPECT_LE(aGordon.Report().MaxProfileDeviation, Precision::Confusion());
  EXPECT_LE(aGordon.Report().MaxGuideDeviation, Precision::Confusion());
}

TEST(GeomFill_Gordon, PolynomialProfilesWithRationalGuidesProducesSurface)
{
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 3);
  aProfiles(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0));
  aProfiles(2) = makeLinearBSpline(gp_Pnt(0, 0.5, 0), gp_Pnt(1, 0.5, 0));
  aProfiles(3) = makeLinearBSpline(gp_Pnt(0, 1, 0), gp_Pnt(1, 1, 0));

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 3);
  aGuides(1) =
    makeRationalQuadraticBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 0.5, 0), gp_Pnt(0, 1, 0), 0.5);
  aGuides(2) =
    makeRationalQuadraticBSpline(gp_Pnt(0.5, 0, 0), gp_Pnt(0.5, 0.5, 0), gp_Pnt(0.5, 1, 0), 0.5);
  aGuides(3) =
    makeRationalQuadraticBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 0.5, 0), gp_Pnt(1, 1, 0), 0.5);

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone());
  EXPECT_FALSE(aGordon.IsApproximate());
  EXPECT_TRUE(aGordon.Surface()->IsURational() || aGordon.Surface()->IsVRational());
  EXPECT_LE(aGordon.Report().MaxProfileDeviation, Precision::Confusion());
  EXPECT_LE(aGordon.Report().MaxGuideDeviation, Precision::Confusion());
}

TEST(GeomFill_Gordon, ApproximateFallbackCanRecoverRationalDegreeOverflow)
{
  constexpr int aDegree = 9;

  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 3);
  aProfiles(1) = makeRationalLineBSpline(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0), aDegree);
  aProfiles(2) = makeRationalLineBSpline(gp_Pnt(0, 0.5, 0), gp_Pnt(1, 0.5, 0), aDegree);
  aProfiles(3) = makeRationalLineBSpline(gp_Pnt(0, 1, 0), gp_Pnt(1, 1, 0), aDegree);

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 3);
  aGuides(1) = makeRationalLineBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 1, 0), aDegree);
  aGuides(2) = makeRationalLineBSpline(gp_Pnt(0.5, 0, 0), gp_Pnt(0.5, 1, 0), aDegree);
  aGuides(3) = makeRationalLineBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0), aDegree);

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());
  aGordon.SetApproximationMode(GeomFill_Gordon::ApproximationMode::AllowApproximateFallback);
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone()) << static_cast<int>(aGordon.Status());
  EXPECT_TRUE(aGordon.IsApproximate());
  EXPECT_EQ(aGordon.Report().Status, GeomFill_Gordon::ResultStatus::Done);
  EXPECT_TRUE(aGordon.Report().IsApproximate);
  EXPECT_EQ(aGordon.Report().FailedStage, GeomFill_Gordon::BuildStage::NotStarted);
  EXPECT_LE(aGordon.Report().MaxApproximationDeviation, 0.001);
  EXPECT_FALSE(aGordon.Surface().IsNull());

  constexpr size_t                        THE_NB_QUALITY_SAMPLES = 48;
  const occ::handle<Geom_BSplineSurface>& aSurface               = aGordon.Surface();
  const double                            aUMin                  = aSurface->UKnot(1);
  const double                            aUMax         = aSurface->UKnot(aSurface->NbUKnots());
  const double                            aVMin         = aSurface->VKnot(1);
  const double                            aVMax         = aSurface->VKnot(aSurface->NbVKnots());
  double                                  aMaxDeviation = 0.0;
  for (size_t aUIdx = 0; aUIdx <= THE_NB_QUALITY_SAMPLES; ++aUIdx)
  {
    const double aUParam = static_cast<double>(aUIdx) / THE_NB_QUALITY_SAMPLES;
    const double aU      = aUMin + aUParam * (aUMax - aUMin);
    for (size_t aVIdx = 0; aVIdx <= THE_NB_QUALITY_SAMPLES; ++aVIdx)
    {
      const double aVParam       = static_cast<double>(aVIdx) / THE_NB_QUALITY_SAMPLES;
      const double aV            = aVMin + aVParam * (aVMax - aVMin);
      const gp_Pnt aProfilePoint = aProfiles.At(0)->Value(aUParam);
      const gp_Pnt aGuidePoint   = aGuides.At(0)->Value(aVParam);
      const gp_Pnt anExpectedPoint(aProfilePoint.X(), aGuidePoint.Y(), 0.0);
      aMaxDeviation = std::max(aMaxDeviation, aSurface->Value(aU, aV).Distance(anExpectedPoint));
    }
  }
  // An interior Gordon point combines the independently approximated profile and guide skins.
  EXPECT_LE(aMaxDeviation, 0.002);
}

TEST(GeomFill_Gordon, ExactOnlyReportsRationalDegreeOverflow)
{
  constexpr int aDegree = 9;

  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 3);
  aProfiles(1) = makeRationalLineBSpline(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0), aDegree);
  aProfiles(2) = makeRationalLineBSpline(gp_Pnt(0, 0.5, 0), gp_Pnt(1, 0.5, 0), aDegree);
  aProfiles(3) = makeRationalLineBSpline(gp_Pnt(0, 1, 0), gp_Pnt(1, 1, 0), aDegree);

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 3);
  aGuides(1) = makeRationalLineBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 1, 0), aDegree);
  aGuides(2) = makeRationalLineBSpline(gp_Pnt(0.5, 0, 0), gp_Pnt(0.5, 1, 0), aDegree);
  aGuides(3) = makeRationalLineBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0), aDegree);

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());
  EXPECT_EQ(aGordon.GetApproximationMode(), GeomFill_Gordon::ApproximationMode::ExactOnly);
  aGordon.Perform();

  EXPECT_FALSE(aGordon.IsDone());
  EXPECT_FALSE(aGordon.IsApproximate());
  EXPECT_EQ(aGordon.Status(), GeomFill_Gordon::ResultStatus::RationalDegreeOverflow);
  EXPECT_EQ(aGordon.Report().Status, GeomFill_Gordon::ResultStatus::RationalDegreeOverflow);
  EXPECT_EQ(aGordon.Report().FailedStage, GeomFill_Gordon::BuildStage::ExactConstruction);
  EXPECT_FALSE(aGordon.Report().IsApproximate);
}

TEST(GeomFill_Gordon, ExactOnlyReparametrizesRationalCurves)
{
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 3);
  aProfiles(1) = makeRationalQuadraticLineBSpline(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0), 1.0, 0.45, 1.0);
  aProfiles(2) =
    makeRationalQuadraticLineBSpline(gp_Pnt(0, 0.4, 0), gp_Pnt(1, 0.4, 0), 1.0, 1.8, 0.7);
  aProfiles(3) = makeRationalQuadraticLineBSpline(gp_Pnt(0, 1, 0), gp_Pnt(1, 1, 0), 0.8, 0.65, 1.5);

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 3);
  aGuides(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 1, 0));
  aGuides(2) = makeLinearBSpline(gp_Pnt(0.4, 0, 0), gp_Pnt(0.4, 1, 0));
  aGuides(3) = makeLinearBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0));

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());
  aGordon.Perform();

  EXPECT_TRUE(aGordon.IsDone());
  EXPECT_FALSE(aGordon.IsApproximate());
  EXPECT_EQ(aGordon.Status(), GeomFill_Gordon::ResultStatus::Done);
  EXPECT_EQ(aGordon.Report().Status, GeomFill_Gordon::ResultStatus::Done);
  EXPECT_EQ(aGordon.Report().FailedStage, GeomFill_Gordon::BuildStage::NotStarted);
  EXPECT_FALSE(aGordon.Report().IsApproximate);
  EXPECT_NEAR(aGordon.Report().MaxReparametrizationDeviation, 0.0, 1.0e-12);
}

TEST(GeomFill_Gordon, ApproximationModeUsesExactRationalReparametrizationWhenPossible)
{
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 3);
  aProfiles(1) = makeRationalQuadraticLineBSpline(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0), 1.0, 0.45, 1.0);
  aProfiles(2) =
    makeRationalQuadraticLineBSpline(gp_Pnt(0, 0.4, 0), gp_Pnt(1, 0.4, 0), 1.0, 1.8, 0.7);
  aProfiles(3) = makeRationalQuadraticLineBSpline(gp_Pnt(0, 1, 0), gp_Pnt(1, 1, 0), 0.8, 0.65, 1.5);

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 3);
  aGuides(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 1, 0));
  aGuides(2) = makeLinearBSpline(gp_Pnt(0.4, 0, 0), gp_Pnt(0.4, 1, 0));
  aGuides(3) = makeLinearBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0));

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());
  aGordon.SetApproximationMode(GeomFill_Gordon::ApproximationMode::AllowApproximateFallback);
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone()) << static_cast<int>(aGordon.Status());
  EXPECT_FALSE(aGordon.IsApproximate());
  EXPECT_EQ(aGordon.Report().Status, GeomFill_Gordon::ResultStatus::Done);
  EXPECT_FALSE(aGordon.Report().IsApproximate);
  EXPECT_NEAR(aGordon.Report().MaxReparametrizationDeviation, 0.0, 1.0e-12);
  EXPECT_FALSE(aGordon.Surface().IsNull());
}

TEST(GeomFill_Gordon, ExactRationalReparametrizationRepeatPerformKeepsReportStable)
{
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 3);
  aProfiles(1) = makeRationalQuadraticLineBSpline(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0), 1.0, 0.45, 1.0);
  aProfiles(2) =
    makeRationalQuadraticLineBSpline(gp_Pnt(0, 0.4, 0), gp_Pnt(1, 0.4, 0), 1.0, 1.8, 0.7);
  aProfiles(3) = makeRationalQuadraticLineBSpline(gp_Pnt(0, 1, 0), gp_Pnt(1, 1, 0), 0.8, 0.65, 1.5);

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 3);
  aGuides(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 1, 0));
  aGuides(2) = makeLinearBSpline(gp_Pnt(0.4, 0, 0), gp_Pnt(0.4, 1, 0));
  aGuides(3) = makeLinearBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0));

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());
  aGordon.SetApproximationMode(GeomFill_Gordon::ApproximationMode::AllowApproximateFallback);

  aGordon.Perform();
  ASSERT_TRUE(aGordon.IsDone()) << static_cast<int>(aGordon.Status());
  EXPECT_FALSE(aGordon.IsApproximate());
  EXPECT_EQ(aGordon.Report().Status, GeomFill_Gordon::ResultStatus::Done);
  EXPECT_EQ(aGordon.Report().FailedStage, GeomFill_Gordon::BuildStage::NotStarted);
  EXPECT_FALSE(aGordon.Report().IsApproximate);
  EXPECT_NEAR(aGordon.Report().MaxContactGap, 0.0, 1.0e-12);
  EXPECT_NEAR(aGordon.Report().MaxReparametrizationDeviation, 0.0, 1.0e-12);
  EXPECT_NEAR(aGordon.Report().MaxApproximationDeviation, 0.0, 1.0e-12);
  const gp_Pnt aFirstCenter = aGordon.Surface()->Value(0.5, 0.5);

  aGordon.Perform();
  ASSERT_TRUE(aGordon.IsDone()) << static_cast<int>(aGordon.Status());
  EXPECT_FALSE(aGordon.IsApproximate());
  EXPECT_EQ(aGordon.Report().Status, GeomFill_Gordon::ResultStatus::Done);
  EXPECT_EQ(aGordon.Report().FailedStage, GeomFill_Gordon::BuildStage::NotStarted);
  EXPECT_FALSE(aGordon.Report().IsApproximate);
  EXPECT_NEAR(aGordon.Report().MaxContactGap, 0.0, 1.0e-12);
  EXPECT_NEAR(aGordon.Report().MaxReparametrizationDeviation, 0.0, 1.0e-12);
  EXPECT_NEAR(aGordon.Report().MaxApproximationDeviation, 0.0, 1.0e-12);
  const gp_Pnt aSecondCenter = aGordon.Surface()->Value(0.5, 0.5);
  EXPECT_NEAR(aFirstCenter.Distance(aSecondCenter), 0.0, Precision::Confusion());
}

TEST(GeomFill_Gordon, NetworkSurface_VClosedNetwork_ProducesVPeriodicSurface)
{
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aProfiles(1, 3);
  aProfiles(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(2, 0, 0));
  aProfiles(2) = makeLinearBSpline(gp_Pnt(0, 1, 0), gp_Pnt(2, 1, 1));
  aProfiles(3) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(2, 0, 0));

  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aGuides(1, 3);
  aGuides(1) = makeQuadraticBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 1, 0), gp_Pnt(0, 0, 0));
  aGuides(2) = makeQuadraticBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0), gp_Pnt(1, 0, 0));
  aGuides(3) = makeQuadraticBSpline(gp_Pnt(2, 0, 0), gp_Pnt(2, 1, 0), gp_Pnt(2, 0, 0));

  NCollection_Array1<double> aProfileParams(1, 3);
  aProfileParams(1) = 0.0;
  aProfileParams(2) = 0.5;
  aProfileParams(3) = 1.0;

  NCollection_Array1<double> aGuideParams(1, 3);
  aGuideParams(1) = 0.0;
  aGuideParams(2) = 0.5;
  aGuideParams(3) = 1.0;

  GeomFill_NetworkSurface aNetwork;
  aNetwork.Init(aProfiles,
                aGuides,
                aProfileParams,
                aGuideParams,
                makeIntersectionGrid(aProfiles, aGuideParams),
                makeUnitWeightGrid(aProfiles, aGuideParams),
                Precision::Confusion(),
                false,
                true);
  aNetwork.Perform();

  ASSERT_TRUE(aNetwork.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurf = aNetwork.Surface();
  ASSERT_FALSE(aSurf.IsNull());
  EXPECT_TRUE(aSurf->IsVPeriodic());
}

TEST(GeomFill_Gordon, DenseWavyFiveByFiveNetwork_InterpolatesAllInputCurves)
{
  const AnalyticNetwork aNetwork(5, 5, wavyHeight);
  GeomFill_Gordon       aGordon;
  aGordon.Init(aNetwork.Profiles, aNetwork.Guides, Precision::Confusion());
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone()) << static_cast<int>(aGordon.Status());
  EXPECT_FALSE(aGordon.IsApproximate());
  verifyUniformNetworkInterpolation(aGordon.Surface(), aNetwork, Precision::Confusion());
}

TEST(GeomFill_Gordon, DenseSaddleSevenByFourNetwork_InterpolatesAllInputCurves)
{
  const AnalyticNetwork aNetwork(7, 4, saddleHeight);
  GeomFill_Gordon       aGordon;
  aGordon.Init(aNetwork.Profiles, aNetwork.Guides, Precision::Confusion());
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone()) << static_cast<int>(aGordon.Status());
  EXPECT_FALSE(aGordon.IsApproximate());
  verifyUniformNetworkInterpolation(aGordon.Surface(), aNetwork, Precision::Confusion());
}

TEST(GeomFill_Gordon, DenseRippleFourBySevenNetwork_InterpolatesAllInputCurves)
{
  const AnalyticNetwork aNetwork(4, 7, rippleHeight);
  GeomFill_Gordon       aGordon;
  aGordon.Init(aNetwork.Profiles, aNetwork.Guides, Precision::Confusion());
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone()) << static_cast<int>(aGordon.Status());
  EXPECT_FALSE(aGordon.IsApproximate());
  verifyUniformNetworkInterpolation(aGordon.Surface(), aNetwork, Precision::Confusion());
}

TEST(GeomFill_Gordon, DenseWavyEightByEightNetwork_InterpolatesAllInputCurves)
{
  const AnalyticNetwork aNetwork(8, 8, wavyHeight);
  GeomFill_Gordon       aGordon;
  aGordon.Init(aNetwork.Profiles, aNetwork.Guides, Precision::Confusion());
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone()) << static_cast<int>(aGordon.Status());
  EXPECT_FALSE(aGordon.IsApproximate());
  verifyUniformNetworkInterpolation(aGordon.Surface(), aNetwork, Precision::Confusion());
}

TEST(GeomFill_Gordon, TranslatedScaledSixByFiveNetwork_InterpolatesAllInputCurves)
{
  const AnalyticNetwork aNetwork(6, 5, rippleHeight, gp_XYZ(1.0e5, -2.0e5, 3.0e5), 250.0);
  GeomFill_Gordon       aGordon;
  aGordon.Init(aNetwork.Profiles, aNetwork.Guides, 1.0e-5);
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone()) << static_cast<int>(aGordon.Status());
  EXPECT_FALSE(aGordon.IsApproximate());
  verifyUniformNetworkInterpolation(aGordon.Surface(), aNetwork, 1.0e-5);
}

TEST(GeomFill_Gordon, SmallScaledFiveBySixNetwork_InterpolatesAllInputCurves)
{
  const AnalyticNetwork aNetwork(5, 6, saddleHeight, gp_XYZ(0.0, 0.0, 0.0), 0.01);
  GeomFill_Gordon       aGordon;
  aGordon.Init(aNetwork.Profiles, aNetwork.Guides, Precision::Confusion());
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone()) << static_cast<int>(aGordon.Status());
  EXPECT_FALSE(aGordon.IsApproximate());
  verifyUniformNetworkInterpolation(aGordon.Surface(), aNetwork, Precision::Confusion());
}

TEST(GeomFill_Gordon, DenseWavyNetwork_ParallelConstructionMatchesSerialQuality)
{
  const AnalyticNetwork aNetwork(6, 6, wavyHeight);
  GeomFill_Gordon       aSerial;
  aSerial.Init(aNetwork.Profiles, aNetwork.Guides, Precision::Confusion());
  aSerial.Perform();
  ASSERT_TRUE(aSerial.IsDone()) << static_cast<int>(aSerial.Status());

  GeomFill_Gordon aParallel;
  aParallel.SetParallelMode(true);
  aParallel.Init(aNetwork.Profiles, aNetwork.Guides, Precision::Confusion());
  aParallel.Perform();
  ASSERT_TRUE(aParallel.IsDone()) << static_cast<int>(aParallel.Status());

  verifyUniformNetworkInterpolation(aSerial.Surface(), aNetwork, Precision::Confusion());
  verifyUniformNetworkInterpolation(aParallel.Surface(), aNetwork, Precision::Confusion());
  constexpr size_t THE_NB_SAMPLES = 24;
  for (size_t aUIdx = 0; aUIdx <= THE_NB_SAMPLES; ++aUIdx)
  {
    for (size_t aVIdx = 0; aVIdx <= THE_NB_SAMPLES; ++aVIdx)
    {
      const double aU = static_cast<double>(aUIdx) / THE_NB_SAMPLES;
      const double aV = static_cast<double>(aVIdx) / THE_NB_SAMPLES;
      EXPECT_LE(aSerial.Surface()->Value(aU, aV).Distance(aParallel.Surface()->Value(aU, aV)),
                Precision::Confusion());
    }
  }
}

TEST(GeomFill_Gordon, DenseSaddleNetwork_RepeatedPerformMaintainsQuality)
{
  const AnalyticNetwork aNetwork(6, 5, saddleHeight);
  GeomFill_Gordon       aGordon;
  aGordon.Init(aNetwork.Profiles, aNetwork.Guides, Precision::Confusion());
  aGordon.Perform();
  ASSERT_TRUE(aGordon.IsDone()) << static_cast<int>(aGordon.Status());
  const occ::handle<Geom_BSplineSurface> aFirstSurface = aGordon.Surface();

  aGordon.Perform();
  ASSERT_TRUE(aGordon.IsDone()) << static_cast<int>(aGordon.Status());
  verifyUniformNetworkInterpolation(aGordon.Surface(), aNetwork, Precision::Confusion());
  constexpr size_t THE_NB_SAMPLES = 24;
  for (size_t aUIdx = 0; aUIdx <= THE_NB_SAMPLES; ++aUIdx)
  {
    for (size_t aVIdx = 0; aVIdx <= THE_NB_SAMPLES; ++aVIdx)
    {
      const double aU = static_cast<double>(aUIdx) / THE_NB_SAMPLES;
      const double aV = static_cast<double>(aVIdx) / THE_NB_SAMPLES;
      EXPECT_LE(aFirstSurface->Value(aU, aV).Distance(aGordon.Surface()->Value(aU, aV)),
                Precision::Confusion());
    }
  }
}

TEST(GeomFill_Gordon, DenseRippleNineByThreeNetwork_InterpolatesAllInputCurves)
{
  const AnalyticNetwork aNetwork(9, 3, rippleHeight);
  GeomFill_Gordon       aGordon;
  aGordon.Init(aNetwork.Profiles, aNetwork.Guides, Precision::Confusion());
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone()) << static_cast<int>(aGordon.Status());
  EXPECT_FALSE(aGordon.IsApproximate());
  verifyUniformNetworkInterpolation(aGordon.Surface(), aNetwork, Precision::Confusion());
}

TEST(GeomFill_Gordon, DenseWavyThreeByNineNetwork_InterpolatesAllInputCurves)
{
  const AnalyticNetwork aNetwork(3, 9, wavyHeight);
  GeomFill_Gordon       aGordon;
  aGordon.Init(aNetwork.Profiles, aNetwork.Guides, Precision::Confusion());
  aGordon.Perform();

  ASSERT_TRUE(aGordon.IsDone()) << static_cast<int>(aGordon.Status());
  EXPECT_FALSE(aGordon.IsApproximate());
  verifyUniformNetworkInterpolation(aGordon.Surface(), aNetwork, Precision::Confusion());
}

TEST(GeomFill_Gordon, ClosedBSplineProfilesWithInteriorGuides_ProducePeriodicSurface)
{
  NCollection_Array1<occ::handle<Geom_Curve>> aProfiles(1, 2);
  aProfiles(1) = makeClosedBSplineProfile(0.0, 1.0, 1.0);
  aProfiles(2) = makeClosedBSplineProfile(1.0, 1.15, 0.85);
  ASSERT_FALSE(occ::down_cast<Geom_BSplineCurve>(aProfiles(1))->IsPeriodic());
  ASSERT_LE(aProfiles(1)
              ->Value(aProfiles(1)->FirstParameter())
              .Distance(aProfiles(1)->Value(aProfiles(1)->LastParameter())),
            Precision::Confusion());

  NCollection_Array1<occ::handle<Geom_Curve>> aGuides(1, 4);
  NCollection_Array1<double>                  aGuideParameters(1, 4);
  aGuideParameters(1) = 0.12;
  aGuideParameters(2) = 0.34;
  aGuideParameters(3) = 0.59;
  aGuideParameters(4) = 0.83;
  for (int aGuideIdx = aGuides.Lower(); aGuideIdx <= aGuides.Upper(); ++aGuideIdx)
  {
    const double aParameter = aGuideParameters(aGuideIdx);
    aGuides(aGuideIdx) =
      makeLinearBSpline(aProfiles(1)->Value(aParameter), aProfiles(2)->Value(aParameter));
  }

  GeomFill_Gordon aGordon;
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());
  aGordon.Perform();
  ASSERT_TRUE(aGordon.IsDone()) << static_cast<int>(aGordon.Status());
  EXPECT_FALSE(aGordon.IsApproximate());
  EXPECT_TRUE(aGordon.Surface()->IsUPeriodic());
  EXPECT_LE(aGordon.Report().MaxProfileDeviation, Precision::Confusion());
  EXPECT_LE(aGordon.Report().MaxGuideDeviation, Precision::Confusion());
}

TEST(GeomFill_Gordon, NetworkSurface_ClosedNetworkWithNonUnitUParameters_ProducesUPeriodicSurface)
{
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aProfiles(1, 2);
  aProfiles(1) = makeQuadraticBSpline(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0), gp_Pnt(0, 0, 0), 2.0, 5.0);
  aProfiles(2) = makeQuadraticBSpline(gp_Pnt(0, 1, 0), gp_Pnt(1, 1, 1), gp_Pnt(0, 1, 0), 2.0, 5.0);

  NCollection_Array1<double> aGuideParameters(1, 2);
  aGuideParameters(1) = 2.0;
  aGuideParameters(2) = 3.5;
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aGuides(1, 2);
  for (int aGuideIdx = aGuides.Lower(); aGuideIdx <= aGuides.Upper(); ++aGuideIdx)
  {
    const double aParameter = aGuideParameters(aGuideIdx);
    aGuides(aGuideIdx) =
      makeLinearBSpline(aProfiles(1)->Value(aParameter), aProfiles(2)->Value(aParameter));
  }

  NCollection_Array1<double> aProfileParameters(1, 2);
  aProfileParameters(1) = 0.0;
  aProfileParameters(2) = 1.0;

  GeomFill_NetworkSurface aNetwork;
  aNetwork.Init(aProfiles,
                aGuides,
                aProfileParameters,
                aGuideParameters,
                makeIntersectionGrid(aProfiles, aGuideParameters),
                makeUnitWeightGrid(aProfiles, aGuideParameters),
                Precision::Confusion(),
                true,
                false);
  aNetwork.Perform();

  ASSERT_TRUE(aNetwork.IsDone()) << static_cast<int>(aNetwork.Status());
  EXPECT_TRUE(aNetwork.Surface()->IsUPeriodic());
}
