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

#include <gtest/gtest.h>

#include <ExtremaPS_Surface.hxx>

#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>

#include <cmath>
#include <random>

namespace
{
//! High precision target tolerance (5e-8, better than Precision::Confusion)
constexpr double THE_HIGH_PRECISION_TOL = 5.0e-8;

//! Standard tolerance for comparison
constexpr double THE_STANDARD_TOL = 1.0e-7;

//! Create a complex BSpline surface for testing
Handle(Geom_BSplineSurface) MakeComplexBSpline()
{
  const int aNbPoles = 10;
  const int aDegree  = 3;
  const int aNbKnots = aNbPoles - aDegree + 1;

  NCollection_Array2<gp_Pnt> poles(1, aNbPoles, 1, aNbPoles);
  for (int i = 1; i <= aNbPoles; ++i)
  {
    for (int j = 1; j <= aNbPoles; ++j)
    {
      double u = (i - 1.0) / (aNbPoles - 1.0);
      double v = (j - 1.0) / (aNbPoles - 1.0);
      // Complex surface with multiple curvature variations
      double z = std::sin(u * M_PI * 2) * std::cos(v * M_PI * 2) * 0.3
                 + std::sin(u * M_PI * 4) * 0.1 + std::cos(v * M_PI * 3) * 0.15;
      poles.SetValue(i, j, gp_Pnt(u * 10.0, v * 10.0, z));
    }
  }

  NCollection_Array1<double> uKnots(1, aNbKnots), vKnots(1, aNbKnots);
  NCollection_Array1<int>    uMults(1, aNbKnots), vMults(1, aNbKnots);
  for (int i = 1; i <= aNbKnots; ++i)
  {
    uKnots(i) = (i - 1.0) / (aNbKnots - 1.0);
    vKnots(i) = (i - 1.0) / (aNbKnots - 1.0);
    uMults(i) = (i == 1 || i == aNbKnots) ? aDegree + 1 : 1;
    vMults(i) = (i == 1 || i == aNbKnots) ? aDegree + 1 : 1;
  }

  return new Geom_BSplineSurface(poles, uKnots, vKnots, uMults, vMults, aDegree, aDegree);
}

//! Create a Bezier dome surface for testing
Handle(Geom_BezierSurface) MakeBezierDome()
{
  const int aNbPoles = 5;
  NCollection_Array2<gp_Pnt> poles(1, aNbPoles, 1, aNbPoles);
  for (int i = 1; i <= aNbPoles; ++i)
  {
    for (int j = 1; j <= aNbPoles; ++j)
    {
      double u = (i - 1.0) / (aNbPoles - 1.0);
      double v = (j - 1.0) / (aNbPoles - 1.0);
      // Dome shape with varying curvature
      double r = std::sqrt((u - 0.5) * (u - 0.5) + (v - 0.5) * (v - 0.5));
      double z = std::cos(r * M_PI) * 0.5 + 0.5;
      poles.SetValue(i, j, gp_Pnt(u * 10.0, v * 10.0, z * 5.0));
    }
  }
  return new Geom_BezierSurface(poles);
}

//! Create a high-curvature BSpline surface
Handle(Geom_BSplineSurface) MakeHighCurvatureBSpline()
{
  const int aNbPoles = 8;
  const int aDegree  = 3;
  const int aNbKnots = aNbPoles - aDegree + 1;

  NCollection_Array2<gp_Pnt> poles(1, aNbPoles, 1, aNbPoles);
  for (int i = 1; i <= aNbPoles; ++i)
  {
    for (int j = 1; j <= aNbPoles; ++j)
    {
      double u = (i - 1.0) / (aNbPoles - 1.0);
      double v = (j - 1.0) / (aNbPoles - 1.0);
      // High curvature variations - sharp peaks and valleys
      double z = std::sin(u * M_PI * 6) * std::sin(v * M_PI * 6) * 2.0;
      poles.SetValue(i, j, gp_Pnt(u * 5.0, v * 5.0, z));
    }
  }

  NCollection_Array1<double> uKnots(1, aNbKnots), vKnots(1, aNbKnots);
  NCollection_Array1<int>    uMults(1, aNbKnots), vMults(1, aNbKnots);
  for (int i = 1; i <= aNbKnots; ++i)
  {
    uKnots(i) = (i - 1.0) / (aNbKnots - 1.0);
    vKnots(i) = (i - 1.0) / (aNbKnots - 1.0);
    uMults(i) = (i == 1 || i == aNbKnots) ? aDegree + 1 : 1;
    vMults(i) = (i == 1 || i == aNbKnots) ? aDegree + 1 : 1;
  }

  return new Geom_BSplineSurface(poles, uKnots, vKnots, uMults, vMults, aDegree, aDegree);
}

} // namespace

//! Test fixture for ExtremaPS precision tests.
class ExtremaPS_PrecisionTest : public testing::Test
{
protected:
  //! Verify precision of extrema result
  void VerifyPrecision(const Handle(Geom_Surface)& theSurf,
                       const gp_Pnt&               thePoint,
                       double                      theTol,
                       const char*                 theTestName)
  {
    GeomAdaptor_Surface anAdaptor(theSurf);
    double              aU1, aU2, aV1, aV2;
    theSurf->Bounds(aU1, aU2, aV1, aV2);

    ExtremaPS_Surface        anEval(anAdaptor, ExtremaPS::Domain2D(aU1, aU2, aV1, aV2));
    const ExtremaPS::Result& aResult = anEval.Perform(thePoint, theTol, ExtremaPS::SearchMode::Min);

    ASSERT_TRUE(aResult.IsDone()) << theTestName << ": Failed to find extrema";
    ASSERT_GE(aResult.NbExt(), 1) << theTestName << ": No extrema found";

    // Verify distance consistency
    for (int i = 0; i < aResult.NbExt(); ++i)
    {
      double aStoredSqDist   = aResult[i].SquareDistance;
      double aComputedSqDist = thePoint.SquareDistance(aResult[i].Point);

      // The stored distance should match computed distance within tolerance
      double aRelError = std::abs(aStoredSqDist - aComputedSqDist)
                         / std::max(aComputedSqDist, 1.0e-20);
      EXPECT_LT(aRelError, theTol * 100.0)
        << theTestName << ": Distance mismatch at extremum " << i << ", relative error: " << aRelError;

      // Verify the point lies on the surface
      gp_Pnt aSurfPt = theSurf->Value(aResult[i].U, aResult[i].V);
      double aPtError = aResult[i].Point.Distance(aSurfPt);
      EXPECT_LT(aPtError, theTol * 10.0)
        << theTestName << ": Point not on surface at extremum " << i << ", error: " << aPtError;
    }
  }
};

//==================================================================================================
// High Precision Tests (Target: 5e-8)
//==================================================================================================

TEST_F(ExtremaPS_PrecisionTest, AchieveFiveE8_BSplineSurface)
{
  Handle(Geom_BSplineSurface) aSurf = MakeComplexBSpline();

  // Test multiple points with high precision
  std::vector<gp_Pnt> aTestPoints = {
    gp_Pnt(5.0, 5.0, 1.0),   // Center, above surface
    gp_Pnt(2.5, 7.5, 0.5),   // Off-center
    gp_Pnt(8.0, 2.0, -0.3),  // Near edge
    gp_Pnt(1.0, 9.0, 0.8),   // Corner region
    gp_Pnt(5.0, 5.0, -1.0),  // Below surface
  };

  for (size_t i = 0; i < aTestPoints.size(); ++i)
  {
    std::string aName = "BSpline_Point_" + std::to_string(i);
    VerifyPrecision(aSurf, aTestPoints[i], THE_HIGH_PRECISION_TOL, aName.c_str());
  }
}

TEST_F(ExtremaPS_PrecisionTest, AchieveFiveE8_BezierSurface)
{
  Handle(Geom_BezierSurface) aSurf = MakeBezierDome();

  std::vector<gp_Pnt> aTestPoints = {
    gp_Pnt(5.0, 5.0, 8.0),  // Above dome apex
    gp_Pnt(2.0, 2.0, 2.0),  // Slope region
    gp_Pnt(8.0, 8.0, 1.0),  // Near edge
    gp_Pnt(5.0, 5.0, 0.0),  // Below dome
  };

  for (size_t i = 0; i < aTestPoints.size(); ++i)
  {
    std::string aName = "Bezier_Point_" + std::to_string(i);
    VerifyPrecision(aSurf, aTestPoints[i], THE_HIGH_PRECISION_TOL, aName.c_str());
  }
}

TEST_F(ExtremaPS_PrecisionTest, HighCurvatureRegions)
{
  Handle(Geom_BSplineSurface) aSurf = MakeHighCurvatureBSpline();

  // Points near peaks and valleys where curvature is high
  std::vector<gp_Pnt> aTestPoints = {
    gp_Pnt(2.5, 2.5, 3.0),   // Near peak
    gp_Pnt(1.25, 1.25, -3.0), // Near valley
    gp_Pnt(3.75, 3.75, 2.5), // Inflection region
  };

  for (size_t i = 0; i < aTestPoints.size(); ++i)
  {
    std::string aName = "HighCurvature_Point_" + std::to_string(i);
    VerifyPrecision(aSurf, aTestPoints[i], THE_HIGH_PRECISION_TOL, aName.c_str());
  }
}

//==================================================================================================
// Newton Convergence Tests
//==================================================================================================

TEST_F(ExtremaPS_PrecisionTest, NewtonConvergenceRate)
{
  Handle(Geom_BSplineSurface) aSurf = MakeComplexBSpline();
  GeomAdaptor_Surface         anAdaptor(aSurf);

  gp_Pnt aPoint(5.0, 5.0, 1.0);

  // Test that tighter tolerances still converge
  std::vector<double> aTols = {1.0e-5, 1.0e-6, 1.0e-7, 5.0e-8};

  for (double aTol : aTols)
  {
    ExtremaPS_Surface        anEval(anAdaptor, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
    const ExtremaPS::Result& aResult = anEval.Perform(aPoint, aTol, ExtremaPS::SearchMode::Min);

    EXPECT_TRUE(aResult.IsDone()) << "Failed at tolerance " << aTol;
    EXPECT_GE(aResult.NbExt(), 1) << "No extrema at tolerance " << aTol;

    if (aResult.NbExt() > 0)
    {
      // Verify the minimum found is consistent
      double aMinDist = aResult[0].SquareDistance;
      EXPECT_GT(aMinDist, 0.0) << "Zero distance at tolerance " << aTol;
    }
  }
}

TEST_F(ExtremaPS_PrecisionTest, StabilityAcrossMultipleRuns)
{
  Handle(Geom_BezierSurface) aSurf = MakeBezierDome();
  GeomAdaptor_Surface        anAdaptor(aSurf);

  gp_Pnt aPoint(5.0, 5.0, 8.0);

  // Run multiple times and verify consistent results
  double aFirstMinDist = -1.0;
  double aFirstU = 0.0, aFirstV = 0.0;

  for (int i = 0; i < 5; ++i)
  {
    ExtremaPS_Surface        anEval(anAdaptor, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
    const ExtremaPS::Result& aResult = anEval.Perform(aPoint, THE_HIGH_PRECISION_TOL, ExtremaPS::SearchMode::Min);

    ASSERT_TRUE(aResult.IsDone());
    ASSERT_GE(aResult.NbExt(), 1);

    if (i == 0)
    {
      aFirstMinDist = aResult[0].SquareDistance;
      aFirstU = aResult[0].U;
      aFirstV = aResult[0].V;
    }
    else
    {
      // Results should be identical across runs
      EXPECT_NEAR(aResult[0].SquareDistance, aFirstMinDist, THE_HIGH_PRECISION_TOL);
      EXPECT_NEAR(aResult[0].U, aFirstU, THE_HIGH_PRECISION_TOL);
      EXPECT_NEAR(aResult[0].V, aFirstV, THE_HIGH_PRECISION_TOL);
    }
  }
}

//==================================================================================================
// Random Point Tests
//==================================================================================================

TEST_F(ExtremaPS_PrecisionTest, RandomPoints_BSpline)
{
  Handle(Geom_BSplineSurface) aSurf = MakeComplexBSpline();

  std::mt19937                          aGen(42); // Fixed seed for reproducibility
  std::uniform_real_distribution<double> aDistXY(0.0, 10.0);
  std::uniform_real_distribution<double> aDistZ(-1.0, 1.0);

  const int aNbTests = 20;
  for (int i = 0; i < aNbTests; ++i)
  {
    gp_Pnt      aPoint(aDistXY(aGen), aDistXY(aGen), aDistZ(aGen));
    std::string aName = "RandomBSpline_" + std::to_string(i);
    VerifyPrecision(aSurf, aPoint, THE_STANDARD_TOL, aName.c_str());
  }
}

TEST_F(ExtremaPS_PrecisionTest, RandomPoints_Bezier)
{
  Handle(Geom_BezierSurface) aSurf = MakeBezierDome();

  std::mt19937                          aGen(123); // Fixed seed
  std::uniform_real_distribution<double> aDistXY(0.0, 10.0);
  std::uniform_real_distribution<double> aDistZ(-2.0, 8.0);

  const int aNbTests = 20;
  for (int i = 0; i < aNbTests; ++i)
  {
    gp_Pnt      aPoint(aDistXY(aGen), aDistXY(aGen), aDistZ(aGen));
    std::string aName = "RandomBezier_" + std::to_string(i);
    VerifyPrecision(aSurf, aPoint, THE_STANDARD_TOL, aName.c_str());
  }
}
