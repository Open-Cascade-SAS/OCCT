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

#include <Approx_BSplineApproxInterp.hxx>
#include <Geom_BSplineCurve.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

#include <cmath>

namespace
{

//! Generates points on a line from (0,0,0) to (1,0,0).
NCollection_Array1<gp_Pnt> makeLinePoints(int theNbPts)
{
  NCollection_Array1<gp_Pnt> aPts(1, theNbPts);
  for (int i = 1; i <= theNbPts; ++i)
  {
    double aT = static_cast<double>(i - 1) / static_cast<double>(theNbPts - 1);
    aPts.SetValue(i, gp_Pnt(aT, 0.0, 0.0));
  }
  return aPts;
}

//! Generates points on a sine wave: X = t, Y = 0, Z = sin(pi*t) for t in [0,1].
NCollection_Array1<gp_Pnt> makeSinePoints(int theNbPts)
{
  NCollection_Array1<gp_Pnt> aPts(1, theNbPts);
  for (int i = 1; i <= theNbPts; ++i)
  {
    double aT = static_cast<double>(i - 1) / static_cast<double>(theNbPts - 1);
    aPts.SetValue(i, gp_Pnt(aT, 0.0, std::sin(M_PI * aT)));
  }
  return aPts;
}

//! Generates uniform parameters [0,1] for given point count.
NCollection_Array1<double> makeUniformParams(int theNbPts)
{
  NCollection_Array1<double> aParams(1, theNbPts);
  for (int i = 1; i <= theNbPts; ++i)
  {
    aParams.SetValue(i, static_cast<double>(i - 1) / static_cast<double>(theNbPts - 1));
  }
  return aParams;
}

} // anonymous namespace

TEST(Approx_BSplineApproxInterpTest, PureApprox_LinePoints_ProducesLowError)
{
  const int                  aNbPts  = 20;
  NCollection_Array1<gp_Pnt> aPts    = makeLinePoints(aNbPts);
  NCollection_Array1<double> aParams = makeUniformParams(aNbPts);

  Approx_BSplineApproxInterp anApprox(aPts, 6, 3, false);
  anApprox.Perform(aParams);

  ASSERT_TRUE(anApprox.IsDone());
  EXPECT_LT(anApprox.MaxError(), 1.0e-10);

  const Handle(Geom_BSplineCurve)& aCurve = anApprox.Curve();
  ASSERT_FALSE(aCurve.IsNull());
  EXPECT_EQ(aCurve->Degree(), 3);

  // Check curve endpoints.
  EXPECT_NEAR(aCurve->Value(0.0).X(), 0.0, 1.0e-10);
  EXPECT_NEAR(aCurve->Value(1.0).X(), 1.0, 1.0e-10);
}

TEST(Approx_BSplineApproxInterpTest, InterpolateEndpoints_ExactMatch)
{
  const int                  aNbPts  = 30;
  NCollection_Array1<gp_Pnt> aPts    = makeSinePoints(aNbPts);
  NCollection_Array1<double> aParams = makeUniformParams(aNbPts);

  Approx_BSplineApproxInterp anApprox(aPts, 10, 3, false);
  anApprox.InterpolatePoint(0);          // first point
  anApprox.InterpolatePoint(aNbPts - 1); // last point
  anApprox.Perform(aParams);

  ASSERT_TRUE(anApprox.IsDone());

  const Handle(Geom_BSplineCurve)& aCurve = anApprox.Curve();
  ASSERT_FALSE(aCurve.IsNull());

  // Endpoints must be interpolated exactly.
  EXPECT_NEAR(aCurve->Value(0.0).Distance(aPts.Value(1)), 0.0, 1.0e-12);
  EXPECT_NEAR(aCurve->Value(1.0).Distance(aPts.Value(aNbPts)), 0.0, 1.0e-12);

  // Overall approximation error must be reasonable.
  EXPECT_LT(anApprox.MaxError(), 0.05);
}

TEST(Approx_BSplineApproxInterpTest, InterpolateMidpoint_ExactMatch)
{
  const int                  aNbPts  = 21;
  NCollection_Array1<gp_Pnt> aPts    = makeSinePoints(aNbPts);
  NCollection_Array1<double> aParams = makeUniformParams(aNbPts);

  Approx_BSplineApproxInterp anApprox(aPts, 12, 3, false);
  anApprox.InterpolatePoint(0);
  anApprox.InterpolatePoint(10); // midpoint
  anApprox.InterpolatePoint(aNbPts - 1);
  anApprox.Perform(aParams);

  ASSERT_TRUE(anApprox.IsDone());

  const Handle(Geom_BSplineCurve)& aCurve = anApprox.Curve();
  ASSERT_FALSE(aCurve.IsNull());

  // Midpoint must be interpolated exactly.
  EXPECT_NEAR(aCurve->Value(0.5).Distance(aPts.Value(11)), 0.0, 1.0e-12);
}

TEST(Approx_BSplineApproxInterpTest, KinkInsertion_PreservesC0Break)
{
  const int                  aNbPts = 21;
  NCollection_Array1<gp_Pnt> aPts(1, aNbPts);
  NCollection_Array1<double> aParams = makeUniformParams(aNbPts);

  // V-shape: two linear segments meeting at index 10 (param 0.5)
  // Left: (0,0,0) to (0.5,0,1)
  // Right: (0.5,0,1) to (1,0,0)
  for (int i = 1; i <= aNbPts; ++i)
  {
    double aT = static_cast<double>(i - 1) / static_cast<double>(aNbPts - 1);
    double aZ = aT <= 0.5 ? 2.0 * aT : 2.0 * (1.0 - aT);
    aPts.SetValue(i, gp_Pnt(aT, 0.0, aZ));
  }

  Approx_BSplineApproxInterp anApprox(aPts, 12, 3, false);
  anApprox.InterpolatePoint(0);
  anApprox.InterpolatePoint(10, true); // kink at V-apex
  anApprox.InterpolatePoint(aNbPts - 1);
  anApprox.Perform(aParams);

  ASSERT_TRUE(anApprox.IsDone());

  const Handle(Geom_BSplineCurve)& aCurve = anApprox.Curve();
  ASSERT_FALSE(aCurve.IsNull());

  // The V-apex must be interpolated exactly.
  EXPECT_NEAR(aCurve->Value(0.5).Distance(gp_Pnt(0.5, 0.0, 1.0)), 0.0, 1.0e-10);

  // Check that the kink created a high-multiplicity knot.
  bool aKinkFound = false;
  for (int i = 1; i <= aCurve->NbKnots(); ++i)
  {
    if (std::abs(aCurve->Knot(i) - 0.5) < 1.0e-3)
    {
      EXPECT_GE(aCurve->Multiplicity(i), aCurve->Degree());
      aKinkFound = true;
      break;
    }
  }
  EXPECT_TRUE(aKinkFound);
}

TEST(Approx_BSplineApproxInterpTest, PerformOptimal_ImprovesError)
{
  const int                  aNbPts  = 50;
  NCollection_Array1<gp_Pnt> aPts    = makeSinePoints(aNbPts);
  NCollection_Array1<double> aParams = makeUniformParams(aNbPts);

  // First, a single Perform to get baseline error.
  Approx_BSplineApproxInterp anApprox1(aPts, 10, 3, false);
  anApprox1.InterpolatePoint(0);
  anApprox1.InterpolatePoint(aNbPts - 1);
  anApprox1.Perform(aParams);
  ASSERT_TRUE(anApprox1.IsDone());
  const double aBaseError = anApprox1.MaxError();

  // Now PerformOptimal to improve.
  Approx_BSplineApproxInterp anApprox2(aPts, 10, 3, false);
  anApprox2.InterpolatePoint(0);
  anApprox2.InterpolatePoint(aNbPts - 1);

  anApprox2.PerformOptimal(aParams, 10);

  ASSERT_TRUE(anApprox2.IsDone());
  const double anOptimalError = anApprox2.MaxError();

  // Optimized error should be no worse than baseline.
  EXPECT_LE(anOptimalError, aBaseError + Precision::Confusion());
}

TEST(Approx_BSplineApproxInterpTest, PureInterpolation_AllPointsExact)
{
  // When nControlPoints == nPoints and all are interpolated, this is pure interpolation.
  const int                  aNbPts  = 8;
  NCollection_Array1<gp_Pnt> aPts    = makeSinePoints(aNbPts);
  NCollection_Array1<double> aParams = makeUniformParams(aNbPts);

  // For pure interpolation: nCP must equal nInterpolated + nContinuity.
  // nCP = 8, all 8 points interpolated, nContinuity = 0 → 8 == 8.
  Approx_BSplineApproxInterp anApprox(aPts, aNbPts, 3, false);
  for (int i = 0; i < aNbPts; ++i)
  {
    anApprox.InterpolatePoint(i);
  }
  anApprox.Perform(aParams);

  ASSERT_TRUE(anApprox.IsDone());

  const Handle(Geom_BSplineCurve)& aCurve = anApprox.Curve();
  ASSERT_FALSE(aCurve.IsNull());

  // All points must be interpolated exactly.
  for (int i = 1; i <= aNbPts; ++i)
  {
    EXPECT_NEAR(aCurve->Value(aParams.Value(i)).Distance(aPts.Value(i)), 0.0, 1.0e-10)
      << "Point " << i << " not interpolated exactly";
  }
}
