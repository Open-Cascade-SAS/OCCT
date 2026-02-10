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

#include <Hermit.hxx>

#include <Geom2d_BSplineCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>

// Helper to create a simple rational BSpline curve (degree 2, 3 poles)
// with weights that produce distinct endpoint weight values.
static Handle(Geom_BSplineCurve) MakeRationalBSpline3D(const double theW1,
                                                       const double theW2,
                                                       const double theW3)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2) = gp_Pnt(1.0, 1.0, 0.0);
  aPoles(3) = gp_Pnt(2.0, 0.0, 0.0);

  NCollection_Array1<double> aWeights(1, 3);
  aWeights(1) = theW1;
  aWeights(2) = theW2;
  aWeights(3) = theW3;

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 3;
  aMults(2) = 3;

  return new Geom_BSplineCurve(aPoles, aWeights, aKnots, aMults, 2);
}

// Helper to create a simple rational BSpline 2D curve (degree 2, 3 poles).
static Handle(Geom2d_BSplineCurve) MakeRationalBSpline2D(const double theW1,
                                                         const double theW2,
                                                         const double theW3)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles(1) = gp_Pnt2d(0.0, 0.0);
  aPoles(2) = gp_Pnt2d(1.0, 1.0);
  aPoles(3) = gp_Pnt2d(2.0, 0.0);

  NCollection_Array1<double> aWeights(1, 3);
  aWeights(1) = theW1;
  aWeights(2) = theW2;
  aWeights(3) = theW3;

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 3;
  aMults(2) = 3;

  return new Geom2d_BSplineCurve(aPoles, aWeights, aKnots, aMults, 2);
}

TEST(HermitTest, Solution3D_UniformWeights_ReturnsValidCurve)
{
  // Uniform weights => weight function is constant 1.0 everywhere
  Handle(Geom_BSplineCurve) aBS = MakeRationalBSpline3D(1.0, 1.0, 1.0);

  Handle(Geom2d_BSplineCurve) aResult = Hermit::Solution(aBS);

  ASSERT_FALSE(aResult.IsNull());
  EXPECT_GE(aResult->NbPoles(), 4);

  // The result a(u) * D(u) should have value 1 at endpoints
  // For uniform weights D(u)=1 everywhere, so a(0)=1, a(1)=1
  const gp_Pnt2d aP0 = aResult->Value(aResult->FirstParameter());
  const gp_Pnt2d aP1 = aResult->Value(aResult->LastParameter());

  EXPECT_NEAR(aP0.Y(), 1.0, 1.0e-6);
  EXPECT_NEAR(aP1.Y(), 1.0, 1.0e-6);
}

TEST(HermitTest, Solution3D_DistinctWeights_ReturnsValidCurve)
{
  // Different weights at endpoints: w(0)=2, w(1)=3
  Handle(Geom_BSplineCurve) aBS = MakeRationalBSpline3D(2.0, 1.5, 3.0);

  Handle(Geom2d_BSplineCurve) aResult = Hermit::Solution(aBS);

  ASSERT_FALSE(aResult.IsNull());

  // a(0) should be 1/w(0) = 0.5, a(1) should be 1/w(1) = 1/3
  const gp_Pnt2d aP0 = aResult->Value(aResult->FirstParameter());
  const gp_Pnt2d aP1 = aResult->Value(aResult->LastParameter());

  EXPECT_NEAR(aP0.Y(), 0.5, 1.0e-4);
  EXPECT_NEAR(aP1.Y(), 1.0 / 3.0, 1.0e-4);
}

TEST(HermitTest, Solution3D_HighWeightRatio_Endpoint)
{
  // Large weight ratio: w(0) small, w(1) large - tests the Pole0 < Pole3 branch
  Handle(Geom_BSplineCurve) aBS = MakeRationalBSpline3D(0.5, 1.0, 5.0);

  Handle(Geom2d_BSplineCurve) aResult = Hermit::Solution(aBS);

  ASSERT_FALSE(aResult.IsNull());

  const gp_Pnt2d aP0 = aResult->Value(aResult->FirstParameter());
  const gp_Pnt2d aP1 = aResult->Value(aResult->LastParameter());

  // a(u)*D(u) = 1 at endpoints => a(0) = 1/w(0) = 2, a(1) = 1/w(1) = 0.2
  EXPECT_NEAR(aP0.Y(), 1.0 / 0.5, 1.0e-4);
  EXPECT_NEAR(aP1.Y(), 1.0 / 5.0, 1.0e-4);
}

TEST(HermitTest, Solution3D_ReversedWeightRatio_Endpoint)
{
  // Reversed ratio: w(0) large, w(1) small - tests the Pole0 > Pole3 branch
  Handle(Geom_BSplineCurve) aBS = MakeRationalBSpline3D(5.0, 1.0, 0.5);

  Handle(Geom2d_BSplineCurve) aResult = Hermit::Solution(aBS);

  ASSERT_FALSE(aResult.IsNull());

  const gp_Pnt2d aP0 = aResult->Value(aResult->FirstParameter());
  const gp_Pnt2d aP1 = aResult->Value(aResult->LastParameter());

  EXPECT_NEAR(aP0.Y(), 1.0 / 5.0, 1.0e-4);
  EXPECT_NEAR(aP1.Y(), 1.0 / 0.5, 1.0e-4);
}

TEST(HermitTest, Solution3D_PositivePoles)
{
  // The result curve should have all positive Y coordinates (positive denominator)
  Handle(Geom_BSplineCurve) aBS = MakeRationalBSpline3D(2.0, 3.0, 1.5);

  Handle(Geom2d_BSplineCurve) aResult = Hermit::Solution(aBS);

  ASSERT_FALSE(aResult.IsNull());
  for (int i = 1; i <= aResult->NbPoles(); ++i)
  {
    EXPECT_GT(aResult->Pole(i).Y(), 0.0) << "Pole " << i << " Y should be positive";
  }
}

TEST(HermitTest, Solution2D_UniformWeights_ReturnsValidCurve)
{
  Handle(Geom2d_BSplineCurve) aBS = MakeRationalBSpline2D(1.0, 1.0, 1.0);

  Handle(Geom2d_BSplineCurve) aResult = Hermit::Solution(aBS);

  ASSERT_FALSE(aResult.IsNull());

  const gp_Pnt2d aP0 = aResult->Value(aResult->FirstParameter());
  const gp_Pnt2d aP1 = aResult->Value(aResult->LastParameter());

  EXPECT_NEAR(aP0.Y(), 1.0, 1.0e-6);
  EXPECT_NEAR(aP1.Y(), 1.0, 1.0e-6);
}

TEST(HermitTest, Solution2D_DistinctWeights_ReturnsValidCurve)
{
  Handle(Geom2d_BSplineCurve) aBS = MakeRationalBSpline2D(2.0, 1.5, 3.0);

  Handle(Geom2d_BSplineCurve) aResult = Hermit::Solution(aBS);

  ASSERT_FALSE(aResult.IsNull());

  const gp_Pnt2d aP0 = aResult->Value(aResult->FirstParameter());
  const gp_Pnt2d aP1 = aResult->Value(aResult->LastParameter());

  EXPECT_NEAR(aP0.Y(), 0.5, 1.0e-4);
  EXPECT_NEAR(aP1.Y(), 1.0 / 3.0, 1.0e-4);
}

TEST(HermitTest, Solution2D_HighWeightRatio_Endpoint)
{
  // Large ratio tests the Pole0 < Pole3 branch in 2D PolyTest
  Handle(Geom2d_BSplineCurve) aBS = MakeRationalBSpline2D(0.5, 1.0, 5.0);

  Handle(Geom2d_BSplineCurve) aResult = Hermit::Solution(aBS);

  ASSERT_FALSE(aResult.IsNull());

  const gp_Pnt2d aP0 = aResult->Value(aResult->FirstParameter());
  const gp_Pnt2d aP1 = aResult->Value(aResult->LastParameter());

  EXPECT_NEAR(aP0.Y(), 1.0 / 0.5, 1.0e-4);
  EXPECT_NEAR(aP1.Y(), 1.0 / 5.0, 1.0e-4);
}

TEST(HermitTest, Solutionbis_UniformWeights_KnotsUnchanged)
{
  Handle(Geom_BSplineCurve) aBS = MakeRationalBSpline3D(1.0, 1.0, 1.0);

  double aKnotmin = 0.0;
  double aKnotmax = 1.0;

  Hermit::Solutionbis(aBS, aKnotmin, aKnotmax);

  // For uniform weights, the Hermite coefficients are trivial (1,0,0,1)
  // and no tolerance knots are needed, so the output knots should be
  // the second and second-to-last of the resulting BS2
  EXPECT_GE(aKnotmin, 0.0);
  EXPECT_LE(aKnotmax, 1.0);
}

TEST(HermitTest, Solutionbis_DistinctWeights_ReturnsValidKnots)
{
  Handle(Geom_BSplineCurve) aBS = MakeRationalBSpline3D(2.0, 1.5, 3.0);

  double aKnotmin = 0.0;
  double aKnotmax = 1.0;

  Hermit::Solutionbis(aBS, aKnotmin, aKnotmax);

  EXPECT_GE(aKnotmin, 0.0);
  EXPECT_LE(aKnotmax, 1.0);
  EXPECT_LE(aKnotmin, aKnotmax);
}

TEST(HermitTest, Solution3D_Symmetric_WeightsProduceSymmetricResult)
{
  // Symmetric weights: w(0) == w(1), so a(0) == a(1)
  Handle(Geom_BSplineCurve) aBS = MakeRationalBSpline3D(2.0, 1.0, 2.0);

  Handle(Geom2d_BSplineCurve) aResult = Hermit::Solution(aBS);

  ASSERT_FALSE(aResult.IsNull());

  const gp_Pnt2d aP0 = aResult->Value(aResult->FirstParameter());
  const gp_Pnt2d aP1 = aResult->Value(aResult->LastParameter());

  // Both endpoints should yield the same value: 1/w = 0.5
  EXPECT_NEAR(aP0.Y(), 0.5, 1.0e-4);
  EXPECT_NEAR(aP1.Y(), 0.5, 1.0e-4);
}
