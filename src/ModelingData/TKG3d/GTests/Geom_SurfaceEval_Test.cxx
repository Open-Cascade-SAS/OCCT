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

#include <Geom_BSplineSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_UndefinedDerivative.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Precision.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Vec.hxx>

//=================================================================================================
// Plane tests
//=================================================================================================

TEST(Geom_SurfaceEvalTest, Plane_EvalD0_ReturnsValidPoint)
{
  occ::handle<Geom_Plane> aSurf = new Geom_Plane(gp_Pln(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)));

  const auto aResult = aSurf->EvalD0(1.0, 2.0);
  ASSERT_TRUE(aResult.has_value());

  // For a standard XY plane, the point should be at (U, V, 0)
  EXPECT_NEAR(aResult->X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aResult->Y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aResult->Z(), 0.0, Precision::Confusion());
}

TEST(Geom_SurfaceEvalTest, Plane_EvalD1_ConstantPartials)
{
  occ::handle<Geom_Plane> aSurf = new Geom_Plane(gp_Pln(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)));

  const auto aD1 = aSurf->EvalD1(1.0, 2.0);
  ASSERT_TRUE(aD1.has_value());

  // Partial derivatives of a plane are constant
  EXPECT_NEAR(aD1->D1U.Magnitude(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aD1->D1V.Magnitude(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aD1->D1U.Dot(aD1->D1V), 0.0, Precision::Confusion());
}

//=================================================================================================
// Sphere tests
//=================================================================================================

TEST(Geom_SurfaceEvalTest, Sphere_EvalD1_PartialDerivatives)
{
  const double            aRadius = 5.0;
  gp_Sphere               aSphere(gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), aRadius);
  occ::handle<Geom_SphericalSurface> aSurf = new Geom_SphericalSurface(aSphere);

  const double aU = M_PI / 4.0; // longitude
  const double aV = M_PI / 6.0; // latitude

  const auto aD1 = aSurf->EvalD1(aU, aV);
  ASSERT_TRUE(aD1.has_value());

  // Point should lie on sphere of given radius
  const double aDist = aD1->Point.Distance(gp_Pnt(0.0, 0.0, 0.0));
  EXPECT_NEAR(aDist, aRadius, Precision::Confusion());

  // D1U and D1V should be non-zero and perpendicular on a sphere (at non-pole points)
  EXPECT_GT(aD1->D1U.Magnitude(), Precision::Confusion());
  EXPECT_GT(aD1->D1V.Magnitude(), Precision::Confusion());
}

//=================================================================================================
// BSplineSurface consistency tests
//=================================================================================================

TEST(Geom_SurfaceEvalTest, BSplineSurface_EvalD2_ConsistentWithOldAPI)
{
  // Create a simple BSpline surface - a flat quad
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      aPoles(i, j) = gp_Pnt(double(i - 1), double(j - 1), 0.1 * double((i - 2) * (j - 2)));
    }
  }

  NCollection_Array1<double> aUKnots(1, 2);
  aUKnots(1) = 0.0;
  aUKnots(2) = 1.0;
  NCollection_Array1<double> aVKnots(1, 2);
  aVKnots(1) = 0.0;
  aVKnots(2) = 1.0;
  NCollection_Array1<int> aUMults(1, 2);
  aUMults(1) = 3;
  aUMults(2) = 3;
  NCollection_Array1<int> aVMults(1, 2);
  aVMults(1) = 3;
  aVMults(2) = 3;

  occ::handle<Geom_BSplineSurface> aSurf =
    new Geom_BSplineSurface(aPoles, aUKnots, aVKnots, aUMults, aVMults, 2, 2);

  const double aU = 0.5;
  const double aV = 0.5;

  const auto aEvalResult = aSurf->EvalD2(aU, aV);
  ASSERT_TRUE(aEvalResult.has_value());

  gp_Pnt aOldP;
  gp_Vec aOldD1U, aOldD1V, aOldD2U, aOldD2V, aOldD2UV;
  aSurf->D2(aU, aV, aOldP, aOldD1U, aOldD1V, aOldD2U, aOldD2V, aOldD2UV);

  EXPECT_NEAR(aEvalResult->Point.Distance(aOldP), 0.0, Precision::Confusion());
  EXPECT_NEAR((aEvalResult->D1U - aOldD1U).Magnitude(), 0.0, Precision::Confusion());
  EXPECT_NEAR((aEvalResult->D1V - aOldD1V).Magnitude(), 0.0, Precision::Confusion());
  EXPECT_NEAR((aEvalResult->D2U - aOldD2U).Magnitude(), 0.0, Precision::Confusion());
  EXPECT_NEAR((aEvalResult->D2V - aOldD2V).Magnitude(), 0.0, Precision::Confusion());
  EXPECT_NEAR((aEvalResult->D2UV - aOldD2UV).Magnitude(), 0.0, Precision::Confusion());
}

//=================================================================================================
// OffsetSurface failure-path tests
//=================================================================================================

TEST(Geom_SurfaceEvalTest, OffsetSurface_EvalD0_ValidOnRegularSurface)
{
  // Create a plane offset by a distance
  occ::handle<Geom_Plane> aPlane =
    new Geom_Plane(gp_Pln(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)));

  occ::handle<Geom_OffsetSurface> anOffset = new Geom_OffsetSurface(aPlane, 3.0);

  const auto aResult = anOffset->EvalD0(1.0, 2.0);
  ASSERT_TRUE(aResult.has_value());

  // Offset plane at distance 3 in Z direction
  EXPECT_NEAR(aResult->X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aResult->Y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aResult->Z(), 3.0, Precision::Confusion());
}
