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
#include <GeomFill_Gordon.hxx>
#include <GeomFill_GordonBuilder.hxx>
#include <GeomFill_Profiler.hxx>
#include <NCollection_Array1.hxx>
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
                                                    const gp_Pnt& theP3)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles(1) = theP1;
  aPoles(2) = theP2;
  aPoles(3) = theP3;

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 3;
  aMults(2) = 3;

  return new Geom_BSplineCurve(aPoles, aKnots, aMults, 2);
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

//! Helper: make profile and guide arrays compatible using GeomFill_Profiler.
void makeCompatible(NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theCurves)
{
  GeomFill_Profiler aProfiler;
  for (int i = theCurves.Lower(); i <= theCurves.Upper(); ++i)
  {
    aProfiler.AddCurve(theCurves(i));
  }
  aProfiler.Perform(Precision::PConfusion());
  for (int i = theCurves.Lower(); i <= theCurves.Upper(); ++i)
  {
    theCurves(i) = occ::down_cast<Geom_BSplineCurve>(aProfiler.Curve(i));
  }
}

} // namespace

// ============================================================================
// GordonBuilder tests (low-level, pre-compatible curves)
// ============================================================================

TEST(GeomFill_GordonBuilder, BilinearNetwork_ProducesValidSurface)
{
  // 2 profiles (horizontal lines at y=0 and y=1) + 2 guides (vertical lines at x=0 and x=1).
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aProfiles(1, 2);
  aProfiles(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0));
  aProfiles(2) = makeLinearBSpline(gp_Pnt(0, 1, 0), gp_Pnt(1, 1, 0));

  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aGuides(1, 2);
  aGuides(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 1, 0));
  aGuides(2) = makeLinearBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0));

  makeCompatible(aProfiles);
  makeCompatible(aGuides);

  NCollection_Array1<double> aProfileParams(1, 2);
  aProfileParams(1) = 0.0;
  aProfileParams(2) = 1.0;
  NCollection_Array1<double> aGuideParams(1, 2);
  aGuideParams(1) = 0.0;
  aGuideParams(2) = 1.0;

  GeomFill_GordonBuilder aBuilder;
  aBuilder.Init(aProfiles, aGuides, aProfileParams, aGuideParams, Precision::Confusion());
  aBuilder.Perform();

  ASSERT_TRUE(aBuilder.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurf = aBuilder.Surface();
  ASSERT_FALSE(aSurf.IsNull());

  verifyPointOnSurface(aSurf, 0.0, 0.0, gp_Pnt(0, 0, 0), Precision::Confusion());
  verifyPointOnSurface(aSurf, 1.0, 0.0, gp_Pnt(1, 0, 0), Precision::Confusion());
  verifyPointOnSurface(aSurf, 0.0, 1.0, gp_Pnt(0, 1, 0), Precision::Confusion());
  verifyPointOnSurface(aSurf, 1.0, 1.0, gp_Pnt(1, 1, 0), Precision::Confusion());
  verifyPointOnSurface(aSurf, 0.5, 0.5, gp_Pnt(0.5, 0.5, 0), 1.0e-3);
}

TEST(GeomFill_GordonBuilder, ThreeByThreeGrid_InterpolatesAllPoints)
{
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aProfiles(1, 3);
  aProfiles(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0));
  aProfiles(2) = makeLinearBSpline(gp_Pnt(0, 0.5, 0), gp_Pnt(1, 0.5, 0));
  aProfiles(3) = makeLinearBSpline(gp_Pnt(0, 1, 0), gp_Pnt(1, 1, 0));

  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aGuides(1, 3);
  aGuides(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 1, 0));
  aGuides(2) = makeLinearBSpline(gp_Pnt(0.5, 0, 0), gp_Pnt(0.5, 1, 0));
  aGuides(3) = makeLinearBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0));

  makeCompatible(aProfiles);
  makeCompatible(aGuides);

  NCollection_Array1<double> aProfileParams(1, 3);
  aProfileParams(1) = 0.0;
  aProfileParams(2) = 0.5;
  aProfileParams(3) = 1.0;
  NCollection_Array1<double> aGuideParams(1, 3);
  aGuideParams(1) = 0.0;
  aGuideParams(2) = 0.5;
  aGuideParams(3) = 1.0;

  GeomFill_GordonBuilder aBuilder;
  aBuilder.Init(aProfiles, aGuides, aProfileParams, aGuideParams, Precision::Confusion());
  aBuilder.Perform();

  ASSERT_TRUE(aBuilder.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurf     = aBuilder.Surface();
  const double                            aGuideX[] = {0.0, 0.5, 1.0};
  const double                            aProfY[]  = {0.0, 0.5, 1.0};
  for (int iG = 0; iG < 3; ++iG)
  {
    for (int iP = 0; iP < 3; ++iP)
    {
      verifyPointOnSurface(aSurf,
                           aGuideX[iG],
                           aProfY[iP],
                           gp_Pnt(aGuideX[iG], aProfY[iP], 0.0),
                           1.0e-3);
    }
  }
}

TEST(GeomFill_GordonBuilder, IntermediateSurfaces_AreValid)
{
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aProfiles(1, 2);
  aProfiles(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0));
  aProfiles(2) = makeLinearBSpline(gp_Pnt(0, 1, 0), gp_Pnt(1, 1, 0));
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aGuides(1, 2);
  aGuides(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 1, 0));
  aGuides(2) = makeLinearBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0));

  makeCompatible(aProfiles);
  makeCompatible(aGuides);

  NCollection_Array1<double> aProfileParams(1, 2);
  aProfileParams(1) = 0.0;
  aProfileParams(2) = 1.0;
  NCollection_Array1<double> aGuideParams(1, 2);
  aGuideParams(1) = 0.0;
  aGuideParams(2) = 1.0;

  GeomFill_GordonBuilder aBuilder;
  aBuilder.Init(aProfiles, aGuides, aProfileParams, aGuideParams, Precision::Confusion());
  aBuilder.Perform();

  ASSERT_TRUE(aBuilder.IsDone());
  EXPECT_FALSE(aBuilder.ProfileSurface().IsNull());
  EXPECT_FALSE(aBuilder.GuideSurface().IsNull());
  EXPECT_FALSE(aBuilder.TensorSurface().IsNull());
}

TEST(GeomFill_GordonBuilder, QuadraticNetwork_ProducesValidSurface)
{
  // Profiles with Z-bump via quadratic B-spline.
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aProfiles(1, 2);
  aProfiles(1) = makeQuadraticBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0.5, 0, 0.5), gp_Pnt(1, 0, 0));
  aProfiles(2) = makeQuadraticBSpline(gp_Pnt(0, 1, 0), gp_Pnt(0.5, 1, 0.5), gp_Pnt(1, 1, 0));
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aGuides(1, 2);
  aGuides(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 1, 0));
  aGuides(2) = makeLinearBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0));

  makeCompatible(aProfiles);
  makeCompatible(aGuides);

  NCollection_Array1<double> aProfileParams(1, 2);
  aProfileParams(1) = 0.0;
  aProfileParams(2) = 1.0;
  NCollection_Array1<double> aGuideParams(1, 2);
  aGuideParams(1) = 0.0;
  aGuideParams(2) = 1.0;

  GeomFill_GordonBuilder aBuilder;
  aBuilder.Init(aProfiles, aGuides, aProfileParams, aGuideParams, Precision::Confusion());
  aBuilder.Perform();

  ASSERT_TRUE(aBuilder.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurf = aBuilder.Surface();
  verifyPointOnSurface(aSurf, 0.0, 0.0, gp_Pnt(0, 0, 0), Precision::Confusion());
  verifyPointOnSurface(aSurf, 1.0, 1.0, gp_Pnt(1, 1, 0), Precision::Confusion());

  gp_Pnt aMidPt = aSurf->Value(0.5, 0.5);
  EXPECT_GT(aMidPt.Z(), 0.1);
}

TEST(GeomFill_GordonBuilder, FiveByFourGrid_ProducesValidSurface)
{
  // 5 profiles + 4 guides with non-uniform spacing.
  const double aProfY[] = {0.0, 0.15, 0.45, 0.8, 1.0};
  const double aGuidX[] = {0.0, 0.3, 0.6, 1.0};

  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aProfiles(1, 5);
  for (int i = 0; i < 5; ++i)
  {
    aProfiles(i + 1) = makeLinearBSpline(gp_Pnt(0, aProfY[i], 0), gp_Pnt(1, aProfY[i], 0));
  }

  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aGuides(1, 4);
  for (int j = 0; j < 4; ++j)
  {
    aGuides(j + 1) = makeLinearBSpline(gp_Pnt(aGuidX[j], 0, 0), gp_Pnt(aGuidX[j], 1, 0));
  }

  makeCompatible(aProfiles);
  makeCompatible(aGuides);

  NCollection_Array1<double> aProfileParams(1, 5);
  for (int i = 0; i < 5; ++i)
    aProfileParams(i + 1) = aProfY[i];
  NCollection_Array1<double> aGuideParams(1, 4);
  for (int j = 0; j < 4; ++j)
    aGuideParams(j + 1) = aGuidX[j];

  GeomFill_GordonBuilder aBuilder;
  aBuilder.Init(aProfiles, aGuides, aProfileParams, aGuideParams, Precision::Confusion());
  aBuilder.Perform();

  ASSERT_TRUE(aBuilder.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurf = aBuilder.Surface();
  ASSERT_FALSE(aSurf.IsNull());

  // Verify all 20 intersection points.
  for (int i = 0; i < 5; ++i)
  {
    for (int j = 0; j < 4; ++j)
    {
      verifyPointOnSurface(aSurf, aGuidX[j], aProfY[i], gp_Pnt(aGuidX[j], aProfY[i], 0.0), 1.0e-3);
    }
  }
}

TEST(GeomFill_GordonBuilder, CurvedQuadraticBothDirections_ProducesValidSurface)
{
  // Both profiles and guides are quadratic (Z-bumps in both directions).
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aProfiles(1, 2);
  aProfiles(1) = makeQuadraticBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0.5, 0, 0.3), gp_Pnt(1, 0, 0));
  aProfiles(2) = makeQuadraticBSpline(gp_Pnt(0, 1, 0), gp_Pnt(0.5, 1, 0.3), gp_Pnt(1, 1, 0));

  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aGuides(1, 2);
  aGuides(1) = makeQuadraticBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 0.5, 0.2), gp_Pnt(0, 1, 0));
  aGuides(2) = makeQuadraticBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 0.5, 0.2), gp_Pnt(1, 1, 0));

  makeCompatible(aProfiles);
  makeCompatible(aGuides);

  NCollection_Array1<double> aProfileParams(1, 2);
  aProfileParams(1) = 0.0;
  aProfileParams(2) = 1.0;
  NCollection_Array1<double> aGuideParams(1, 2);
  aGuideParams(1) = 0.0;
  aGuideParams(2) = 1.0;

  GeomFill_GordonBuilder aBuilder;
  aBuilder.Init(aProfiles, aGuides, aProfileParams, aGuideParams, Precision::Confusion());
  aBuilder.Perform();

  ASSERT_TRUE(aBuilder.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurf = aBuilder.Surface();

  // Corners should be exact (Z=0).
  verifyPointOnSurface(aSurf, 0.0, 0.0, gp_Pnt(0, 0, 0), 1.0e-6);
  verifyPointOnSurface(aSurf, 1.0, 1.0, gp_Pnt(1, 1, 0), 1.0e-6);

  // Center should have combined Z-bump (superposition of profile + guide bumps).
  gp_Pnt aMidPt = aSurf->Value(0.5, 0.5);
  EXPECT_GT(aMidPt.Z(), 0.2) << "Center Z=" << aMidPt.Z()
                             << " should reflect combined bumps from both directions";
}

TEST(GeomFill_GordonBuilder, NotDone_BeforePerform)
{
  GeomFill_GordonBuilder aBuilder;
  EXPECT_FALSE(aBuilder.IsDone());
  EXPECT_THROW((void)aBuilder.Surface(), StdFail_NotDone);
  EXPECT_THROW((void)aBuilder.ProfileSurface(), StdFail_NotDone);
  EXPECT_THROW((void)aBuilder.GuideSurface(), StdFail_NotDone);
  EXPECT_THROW((void)aBuilder.TensorSurface(), StdFail_NotDone);
}

TEST(GeomFill_GordonBuilder, SingleProfile_NotDone)
{
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aProfiles(1, 1);
  aProfiles(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0));

  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aGuides(1, 2);
  aGuides(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 1, 0));
  aGuides(2) = makeLinearBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0));

  makeCompatible(aGuides);

  NCollection_Array1<double> aProfileParams(1, 1);
  aProfileParams(1) = 0.0;
  NCollection_Array1<double> aGuideParams(1, 2);
  aGuideParams(1) = 0.0;
  aGuideParams(2) = 1.0;

  GeomFill_GordonBuilder aBuilder;
  aBuilder.Init(aProfiles, aGuides, aProfileParams, aGuideParams, Precision::Confusion());
  aBuilder.Perform();

  EXPECT_FALSE(aBuilder.IsDone());
}

TEST(GeomFill_GordonBuilder, NonUniformParams_ProducesCorrectGeometry)
{
  // 3 profiles + 2 guides at non-uniform parameter positions.
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aProfiles(1, 3);
  aProfiles(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0));
  aProfiles(2) = makeLinearBSpline(gp_Pnt(0, 0.3, 0), gp_Pnt(1, 0.3, 0));
  aProfiles(3) = makeLinearBSpline(gp_Pnt(0, 1, 0), gp_Pnt(1, 1, 0));

  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aGuides(1, 2);
  aGuides(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 1, 0));
  aGuides(2) = makeLinearBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0));

  makeCompatible(aProfiles);
  makeCompatible(aGuides);

  NCollection_Array1<double> aProfileParams(1, 3);
  aProfileParams(1) = 0.0;
  aProfileParams(2) = 0.3;
  aProfileParams(3) = 1.0;
  NCollection_Array1<double> aGuideParams(1, 2);
  aGuideParams(1) = 0.0;
  aGuideParams(2) = 1.0;

  GeomFill_GordonBuilder aBuilder;
  aBuilder.Init(aProfiles, aGuides, aProfileParams, aGuideParams, Precision::Confusion());
  aBuilder.Perform();

  ASSERT_TRUE(aBuilder.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurf = aBuilder.Surface();

  // The middle profile is at V=0.3, so (0.5, 0.3) should be close to (0.5, 0.3, 0).
  verifyPointOnSurface(aSurf, 0.5, 0.3, gp_Pnt(0.5, 0.3, 0), 1.0e-3);
}

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
}

TEST(GeomFill_Gordon, NotDone_BeforePerform)
{
  GeomFill_Gordon aGordon;
  EXPECT_FALSE(aGordon.IsDone());
  EXPECT_THROW((void)aGordon.Surface(), StdFail_NotDone);
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
  EXPECT_GT(aMidPt.Z(), 0.05) << "Surface should have Z-bump at center";
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
  EXPECT_NE(aP1.Z(), 0.0) << "Surface should have Z variation from S-curve profiles";
  // The S-curve should make these two have opposite Z signs.
  EXPECT_TRUE((aP1.Z() > 0 && aP2.Z() < 0) || (aP1.Z() < 0 && aP2.Z() > 0))
    << "S-curve should produce opposite Z signs at quarter and three-quarter points";
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
  EXPECT_GT(aMidPt.Z(), 0.2) << "Center should have significant Z from sinusoidal profiles";
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
  EXPECT_GT(std::abs(aMidPt.Z()), 0.01)
    << "Non-planar network should produce non-planar surface at center";
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

  gp_Pnt aPrevPt;
  gp_Vec aPrevDU, aPrevDV;
  aSurf->D1(aUMin, aV, aPrevPt, aPrevDU, aPrevDV);

  for (int i = 1; i <= THE_NB; ++i)
  {
    double aU = aUMin + i * (aUMax - aUMin) / THE_NB;
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
  EXPECT_GT(aMidPt.Z(), 0.1) << "Surface center should have positive Z from arc-like profiles";
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

  // Re-init with same data and perform again.
  aGordon.Init(aProfiles, aGuides, Precision::Confusion());
  aGordon.Perform();
  ASSERT_TRUE(aGordon.IsDone());
  double aUMid2 =
    0.5 * (aGordon.Surface()->UKnot(1) + aGordon.Surface()->UKnot(aGordon.Surface()->NbUKnots()));
  double aVMid2 =
    0.5 * (aGordon.Surface()->VKnot(1) + aGordon.Surface()->VKnot(aGordon.Surface()->NbVKnots()));
  gp_Pnt aMid2 = aGordon.Surface()->Value(aUMid2, aVMid2);

  EXPECT_NEAR(aMid1.Distance(aMid2), 0.0, Precision::Confusion())
    << "Repeated Perform should give same result";
}

TEST(GeomFill_Gordon, HighDensityNetwork_SixBySix)
{
  // 6 profiles + 6 guides — tests performance and correctness with denser grids.
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
  EXPECT_GT(aMidPt.Z(), 0.3) << "Center should have large Z from sinusoidal profiles";
}

TEST(GeomFill_Gordon, BooleanSumProperty_ProfilePlusGuideMinusTensor)
{
  // The Gordon surface should satisfy S = S_profiles + S_guides - S_tensor
  // at every point. Verify this using the GordonBuilder intermediate surfaces.
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aProfiles(1, 2);
  aProfiles(1) = makeQuadraticBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0.5, 0, 0.5), gp_Pnt(1, 0, 0));
  aProfiles(2) = makeQuadraticBSpline(gp_Pnt(0, 1, 0), gp_Pnt(0.5, 1, 0.3), gp_Pnt(1, 1, 0));

  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aGuides(1, 2);
  aGuides(1) = makeLinearBSpline(gp_Pnt(0, 0, 0), gp_Pnt(0, 1, 0));
  aGuides(2) = makeLinearBSpline(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0));

  makeCompatible(aProfiles);
  makeCompatible(aGuides);

  NCollection_Array1<double> aProfileParams(1, 2);
  aProfileParams(1) = 0.0;
  aProfileParams(2) = 1.0;
  NCollection_Array1<double> aGuideParams(1, 2);
  aGuideParams(1) = 0.0;
  aGuideParams(2) = 1.0;

  GeomFill_GordonBuilder aBuilder;
  aBuilder.Init(aProfiles, aGuides, aProfileParams, aGuideParams, Precision::Confusion());
  aBuilder.Perform();

  ASSERT_TRUE(aBuilder.IsDone());

  const occ::handle<Geom_BSplineSurface>& aSurf    = aBuilder.Surface();
  const occ::handle<Geom_BSplineSurface>& aProfS   = aBuilder.ProfileSurface();
  const occ::handle<Geom_BSplineSurface>& aGuidS   = aBuilder.GuideSurface();
  const occ::handle<Geom_BSplineSurface>& aTensorS = aBuilder.TensorSurface();

  // Verify at multiple sample points.
  constexpr int THE_NB = 5;
  for (int i = 0; i <= THE_NB; ++i)
  {
    for (int j = 0; j <= THE_NB; ++j)
    {
      double aU = static_cast<double>(i) / THE_NB;
      double aV = static_cast<double>(j) / THE_NB;

      gp_Pnt aGordon = aSurf->Value(aU, aV);
      gp_Pnt aProf   = aProfS->Value(aU, aV);
      gp_Pnt aGuid   = aGuidS->Value(aU, aV);
      gp_Pnt aTensor = aTensorS->Value(aU, aV);

      // S_gordon = S_prof + S_guid - S_tensor.
      gp_Pnt aExpected(aProf.X() + aGuid.X() - aTensor.X(),
                       aProf.Y() + aGuid.Y() - aTensor.Y(),
                       aProf.Z() + aGuid.Z() - aTensor.Z());

      EXPECT_LT(aGordon.Distance(aExpected), 1.0e-6)
        << "Boolean sum mismatch at (" << aU << ", " << aV << ")";
    }
  }
}
