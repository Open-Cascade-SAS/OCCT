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

#include <BSplCLib_GridEvaluator.hxx>
#include <BSplCLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColStd_HArray1OfReal.hxx>

// Helper function to create a simple cubic B-spline curve.
// Control points form a simple arc.
static void CreateSimpleBSplineCurve(Handle(TColgp_HArray1OfPnt)&   thePoles,
                                     Handle(TColStd_HArray1OfReal)& theFlatKnots,
                                     int&                           theDegree)
{
  theDegree = 3;

  // 5 control points
  thePoles = new TColgp_HArray1OfPnt(1, 5);
  thePoles->SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  thePoles->SetValue(2, gp_Pnt(1.0, 2.0, 0.0));
  thePoles->SetValue(3, gp_Pnt(2.0, 2.0, 1.0));
  thePoles->SetValue(4, gp_Pnt(3.0, 1.0, 1.0));
  thePoles->SetValue(5, gp_Pnt(4.0, 0.0, 0.0));

  // Flat knots for degree 3 with 5 control points
  // n + p + 1 = 5 + 3 + 1 = 9 knots
  // Clamped knot vector: [0, 0, 0, 0, 0.5, 1, 1, 1, 1]
  theFlatKnots = new TColStd_HArray1OfReal(1, 9);
  theFlatKnots->SetValue(1, 0.0);
  theFlatKnots->SetValue(2, 0.0);
  theFlatKnots->SetValue(3, 0.0);
  theFlatKnots->SetValue(4, 0.0);
  theFlatKnots->SetValue(5, 0.5);
  theFlatKnots->SetValue(6, 1.0);
  theFlatKnots->SetValue(7, 1.0);
  theFlatKnots->SetValue(8, 1.0);
  theFlatKnots->SetValue(9, 1.0);
}

// Helper function to create a rational B-spline curve (circular arc).
static void CreateRationalBSplineCurve(Handle(TColgp_HArray1OfPnt)&   thePoles,
                                       Handle(TColStd_HArray1OfReal)& theWeights,
                                       Handle(TColStd_HArray1OfReal)& theFlatKnots,
                                       int&                           theDegree)
{
  theDegree = 2;

  // 3 control points for a 90-degree circular arc
  thePoles = new TColgp_HArray1OfPnt(1, 3);
  thePoles->SetValue(1, gp_Pnt(1.0, 0.0, 0.0));
  thePoles->SetValue(2, gp_Pnt(1.0, 1.0, 0.0));
  thePoles->SetValue(3, gp_Pnt(0.0, 1.0, 0.0));

  // Weights for circular arc
  theWeights = new TColStd_HArray1OfReal(1, 3);
  theWeights->SetValue(1, 1.0);
  theWeights->SetValue(2, 1.0 / std::sqrt(2.0));
  theWeights->SetValue(3, 1.0);

  // Flat knots: [0,0,0,1,1,1]
  theFlatKnots = new TColStd_HArray1OfReal(1, 6);
  theFlatKnots->SetValue(1, 0.0);
  theFlatKnots->SetValue(2, 0.0);
  theFlatKnots->SetValue(3, 0.0);
  theFlatKnots->SetValue(4, 1.0);
  theFlatKnots->SetValue(5, 1.0);
  theFlatKnots->SetValue(6, 1.0);
}

// Helper function to create Bezier curve control points.
static Handle(TColgp_HArray1OfPnt) CreateBezierCurve()
{
  // Cubic Bezier curve (degree 3)
  Handle(TColgp_HArray1OfPnt) aPoles = new TColgp_HArray1OfPnt(1, 4);
  aPoles->SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles->SetValue(2, gp_Pnt(1.0, 3.0, 0.0));
  aPoles->SetValue(3, gp_Pnt(3.0, 3.0, 0.0));
  aPoles->SetValue(4, gp_Pnt(4.0, 0.0, 0.0));
  return aPoles;
}

TEST(BSplCLib_GridEvaluatorTest, DefaultConstructor)
{
  BSplCLib_GridEvaluator anEval;
  EXPECT_FALSE(anEval.IsInitialized());
  EXPECT_EQ(anEval.NbParams(), 0);
}

TEST(BSplCLib_GridEvaluatorTest, Initialize)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  const bool             isOk = anEval.Initialize(aDeg,
                                      aPoles,
                                      Handle(TColStd_HArray1OfReal)(), // non-rational
                                      aKnots,
                                      false,  // not rational
                                      false); // not periodic

  EXPECT_TRUE(isOk);
  EXPECT_TRUE(anEval.IsInitialized());
}

TEST(BSplCLib_GridEvaluatorTest, InitializeValidation_InvalidDegree)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;

  // Degree 0 should fail
  const bool isOk =
    anEval.Initialize(0, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);
  EXPECT_FALSE(isOk);
  EXPECT_FALSE(anEval.IsInitialized());
}

TEST(BSplCLib_GridEvaluatorTest, InitializeValidation_RationalWithoutWeights)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;

  // Rational curve without weights should fail
  const bool isOk = anEval.Initialize(aDeg,
                                      aPoles,
                                      Handle(TColStd_HArray1OfReal)(), // No weights
                                      aKnots,
                                      true, // Rational flag set
                                      false);
  EXPECT_FALSE(isOk);
  EXPECT_FALSE(anEval.IsInitialized());
}

TEST(BSplCLib_GridEvaluatorTest, PrepareUniformParams)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);

  anEval.PrepareParams(0.0, 1.0, 10);

  EXPECT_EQ(anEval.NbParams(), 10);

  // Check that parameters are in range
  for (int i = 1; i <= anEval.NbParams(); ++i)
  {
    auto aParam = anEval.Param(i);
    ASSERT_TRUE(aParam.has_value());
    EXPECT_GE(*aParam, 0.0);
    EXPECT_LE(*aParam, 1.0);
  }
}

TEST(BSplCLib_GridEvaluatorTest, PrepareUniformParamsWithExactBoundaries)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);

  // With theIncludeEnds = true (default)
  anEval.PrepareParams(0.0, 1.0, 10, true);

  // First and last parameters should be exactly at boundaries
  auto aFirstParam = anEval.Param(1);
  auto aLastParam  = anEval.Param(anEval.NbParams());

  ASSERT_TRUE(aFirstParam.has_value());
  ASSERT_TRUE(aLastParam.has_value());
  EXPECT_DOUBLE_EQ(*aFirstParam, 0.0);
  EXPECT_DOUBLE_EQ(*aLastParam, 1.0);
}

TEST(BSplCLib_GridEvaluatorTest, PrepareUniformParamsWithoutBoundaries)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);

  // With theIncludeEnds = false
  anEval.PrepareParams(0.0, 1.0, 10, false);

  // First and last parameters should NOT be exactly at boundaries
  auto aFirstParam = anEval.Param(1);
  auto aLastParam  = anEval.Param(anEval.NbParams());

  ASSERT_TRUE(aFirstParam.has_value());
  ASSERT_TRUE(aLastParam.has_value());
  EXPECT_GT(*aFirstParam, 0.0);
  EXPECT_LT(*aLastParam, 1.0);
}

TEST(BSplCLib_GridEvaluatorTest, PrepareKnotAlignedParams)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);

  anEval.PrepareParamsFromKnots(0.0, 1.0, 5);

  // Should have at least 5 samples
  EXPECT_GE(anEval.NbParams(), 5);

  // Check that span indices are valid
  for (int i = 1; i <= anEval.NbParams(); ++i)
  {
    auto aData = anEval.ParamData(i);
    ASSERT_TRUE(aData.has_value());
    EXPECT_GE(aData->SpanIndex, aDeg);
  }
}

TEST(BSplCLib_GridEvaluatorTest, D0Evaluation)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);

  anEval.PrepareParams(0.0, 1.0, 10);

  // Evaluate and compare with direct BSplCLib evaluation
  for (int i = 1; i <= anEval.NbParams(); ++i)
  {
    auto aPtGrid = anEval.Value(i);
    ASSERT_TRUE(aPtGrid.has_value());

    // Direct evaluation using BSplCLib for comparison
    gp_Pnt aPtDirect;
    auto   aData = anEval.ParamData(i);
    ASSERT_TRUE(aData.has_value());

    BSplCLib::D0(aData->Param,
                 aData->SpanIndex,
                 aDeg,
                 false, // not periodic
                 aPoles->Array1(),
                 nullptr,
                 aKnots->Array1(),
                 nullptr, // NoMults for flat knots
                 aPtDirect);

    EXPECT_NEAR(aPtGrid->X(), aPtDirect.X(), Precision::Confusion());
    EXPECT_NEAR(aPtGrid->Y(), aPtDirect.Y(), Precision::Confusion());
    EXPECT_NEAR(aPtGrid->Z(), aPtDirect.Z(), Precision::Confusion());
  }
}

TEST(BSplCLib_GridEvaluatorTest, D1Evaluation)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);

  anEval.PrepareParams(0.0, 1.0, 5);

  // Test D1 evaluation
  gp_Pnt     aPt;
  gp_Vec     aD1;
  const bool isOk = anEval.D1(3, aPt, aD1);

  EXPECT_TRUE(isOk);
  // For a B-spline curve, derivative should be non-zero
  EXPECT_GT(aD1.Magnitude(), Precision::Confusion());
}

TEST(BSplCLib_GridEvaluatorTest, D2Evaluation)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);

  anEval.PrepareParams(0.0, 1.0, 5);

  // Test D2 evaluation - should not crash
  gp_Pnt     aPt;
  gp_Vec     aD1, aD2;
  const bool isOk = anEval.D2(3, aPt, aD1, aD2);

  EXPECT_TRUE(isOk);
  // Point should be valid (not NaN)
  EXPECT_FALSE(std::isnan(aPt.X()));
}

TEST(BSplCLib_GridEvaluatorTest, InvalidIndexReturnsNullopt)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);
  anEval.PrepareParams(0.0, 1.0, 10);

  // Invalid indices
  EXPECT_FALSE(anEval.Value(0).has_value());
  EXPECT_FALSE(anEval.Value(-1).has_value());
  EXPECT_FALSE(anEval.Value(11).has_value());
  EXPECT_FALSE(anEval.Param(0).has_value());
  EXPECT_FALSE(anEval.ParamData(100).has_value());

  // D0/D1/D2 should return false for invalid indices
  gp_Pnt aPt;
  gp_Vec aD1, aD2;
  EXPECT_FALSE(anEval.D0(0, aPt));
  EXPECT_FALSE(anEval.D1(0, aPt, aD1));
  EXPECT_FALSE(anEval.D2(0, aPt, aD1, aD2));
}

TEST(BSplCLib_GridEvaluatorTest, EndpointsMatch)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);

  anEval.PrepareParams(0.0, 1.0, 10, true);

  // First and last evaluated points should match first/last control points
  // (for clamped B-spline)
  auto aFirstPt = anEval.Value(1);
  auto aLastPt  = anEval.Value(anEval.NbParams());

  ASSERT_TRUE(aFirstPt.has_value());
  ASSERT_TRUE(aLastPt.has_value());

  // Should be close to first control point (0, 0, 0) and last (4, 0, 0)
  EXPECT_NEAR(aFirstPt->X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aFirstPt->Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aLastPt->X(), 4.0, Precision::Confusion());
  EXPECT_NEAR(aLastPt->Y(), 0.0, Precision::Confusion());
}

TEST(BSplCLib_GridEvaluatorTest, RationalCurveEvaluation)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aWeights;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateRationalBSplineCurve(aPoles, aWeights, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  const bool             isOk = anEval.Initialize(aDeg, aPoles, aWeights, aKnots, true, false);

  ASSERT_TRUE(isOk);

  anEval.PrepareParams(0.0, 1.0, 10, true);

  // For a circular arc, all points should be at distance 1 from origin
  for (int i = 1; i <= anEval.NbParams(); ++i)
  {
    auto aPt = anEval.Value(i);
    ASSERT_TRUE(aPt.has_value());

    const double aRadius = std::sqrt(aPt->X() * aPt->X() + aPt->Y() * aPt->Y());
    EXPECT_NEAR(aRadius, 1.0, 1e-10);
  }
}

TEST(BSplCLib_GridEvaluatorTest, BezierCurveInitialization)
{
  Handle(TColgp_HArray1OfPnt) aPoles = CreateBezierCurve();

  BSplCLib_GridEvaluator anEval;
  const bool             isOk = anEval.InitializeBezier(aPoles, Handle(TColStd_HArray1OfReal)());

  ASSERT_TRUE(isOk);
  EXPECT_TRUE(anEval.IsInitialized());
}

TEST(BSplCLib_GridEvaluatorTest, BezierCurveEvaluation)
{
  Handle(TColgp_HArray1OfPnt) aPoles = CreateBezierCurve();

  BSplCLib_GridEvaluator anEval;
  anEval.InitializeBezier(aPoles, Handle(TColStd_HArray1OfReal)());

  anEval.PrepareParams(0.0, 1.0, 10, true);

  // First and last points should match control points
  auto aFirstPt = anEval.Value(1);
  auto aLastPt  = anEval.Value(anEval.NbParams());

  ASSERT_TRUE(aFirstPt.has_value());
  ASSERT_TRUE(aLastPt.has_value());

  EXPECT_NEAR(aFirstPt->X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aFirstPt->Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aLastPt->X(), 4.0, Precision::Confusion());
  EXPECT_NEAR(aLastPt->Y(), 0.0, Precision::Confusion());
}

TEST(BSplCLib_GridEvaluatorTest, BezierCurveValidation_TooFewPoles)
{
  Handle(TColgp_HArray1OfPnt) aPoles = new TColgp_HArray1OfPnt(1, 1);
  aPoles->SetValue(1, gp_Pnt(0, 0, 0));

  BSplCLib_GridEvaluator anEval;
  const bool             isOk = anEval.InitializeBezier(aPoles, Handle(TColStd_HArray1OfReal)());

  EXPECT_FALSE(isOk);
  EXPECT_FALSE(anEval.IsInitialized());
}

TEST(BSplCLib_GridEvaluatorTest, ConsistencyBetweenMethods)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  // Create two evaluators - one with uniform, one with knot-aligned params
  BSplCLib_GridEvaluator anEvalUniform;
  anEvalUniform.Initialize(aDeg, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);
  anEvalUniform.PrepareParams(0.0, 1.0, 20, true);

  BSplCLib_GridEvaluator anEvalKnots;
  anEvalKnots.Initialize(aDeg, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);
  anEvalKnots.PrepareParamsFromKnots(0.0, 1.0, 20, true);

  // Both should produce valid results at endpoints
  auto aPt1 = anEvalUniform.Value(1);
  auto aPt2 = anEvalKnots.Value(1);

  ASSERT_TRUE(aPt1.has_value());
  ASSERT_TRUE(aPt2.has_value());

  // First point should be near (0,0,0) since that's the start
  EXPECT_NEAR(aPt1->X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aPt1->Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aPt2->X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aPt2->Y(), 0.0, Precision::Confusion());
}

TEST(BSplCLib_GridEvaluatorTest, SingleSpanBSpline)
{
  // Linear B-spline (degree 1, single span)
  const int aDegree = 1;

  Handle(TColgp_HArray1OfPnt) aPoles = new TColgp_HArray1OfPnt(1, 2);
  aPoles->SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles->SetValue(2, gp_Pnt(1.0, 1.0, 0.0));

  // Flat knots: [0,0,1,1]
  Handle(TColStd_HArray1OfReal) aKnots = new TColStd_HArray1OfReal(1, 4);
  aKnots->SetValue(1, 0.0);
  aKnots->SetValue(2, 0.0);
  aKnots->SetValue(3, 1.0);
  aKnots->SetValue(4, 1.0);

  BSplCLib_GridEvaluator anEval;
  const bool             isOk =
    anEval.Initialize(aDegree, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);

  ASSERT_TRUE(isOk);

  anEval.PrepareParams(0.0, 1.0, 5, true);

  // Points should lie on the line from (0,0,0) to (1,1,0)
  for (int i = 1; i <= anEval.NbParams(); ++i)
  {
    auto aPt = anEval.Value(i);
    ASSERT_TRUE(aPt.has_value());

    // For a linear curve, X should equal Y
    EXPECT_NEAR(aPt->X(), aPt->Y(), Precision::Confusion());
  }
}

TEST(BSplCLib_GridEvaluatorTest, HandleAccessors)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aWeights;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateRationalBSplineCurve(aPoles, aWeights, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, aWeights, aKnots, true, false);

  // Test accessor methods
  EXPECT_EQ(anEval.Degree(), aDeg);
  EXPECT_TRUE(anEval.IsRational());
  EXPECT_FALSE(anEval.IsPeriodic());

  // Test Handle accessors
  EXPECT_FALSE(anEval.Poles().IsNull());
  EXPECT_FALSE(anEval.Weights().IsNull());
  EXPECT_FALSE(anEval.FlatKnots().IsNull());

  // Verify the handles contain the expected data
  EXPECT_EQ(anEval.Poles()->Length(), 3);
  EXPECT_EQ(anEval.Weights()->Length(), 3);
  EXPECT_EQ(anEval.FlatKnots()->Length(), 6);
}

// Helper to create a multi-span B-spline curve with 3 spans.
static void CreateMultiSpanBSplineCurve(Handle(TColgp_HArray1OfPnt)&   thePoles,
                                        Handle(TColStd_HArray1OfReal)& theFlatKnots,
                                        int&                           theDegree)
{
  theDegree = 3;

  // 7 control points for 3 spans
  thePoles = new TColgp_HArray1OfPnt(1, 7);
  thePoles->SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  thePoles->SetValue(2, gp_Pnt(1.0, 2.0, 0.0));
  thePoles->SetValue(3, gp_Pnt(2.0, 3.0, 1.0));
  thePoles->SetValue(4, gp_Pnt(3.0, 2.5, 1.5));
  thePoles->SetValue(5, gp_Pnt(4.0, 1.5, 1.0));
  thePoles->SetValue(6, gp_Pnt(5.0, 0.5, 0.5));
  thePoles->SetValue(7, gp_Pnt(6.0, 0.0, 0.0));

  // Flat knots for degree 3 with 7 control points: n+p+1 = 7+3+1 = 11 knots
  // Clamped knot vector with 3 internal knots: [0,0,0,0, 0.33, 0.5, 0.67, 1,1,1,1]
  theFlatKnots = new TColStd_HArray1OfReal(1, 11);
  theFlatKnots->SetValue(1, 0.0);
  theFlatKnots->SetValue(2, 0.0);
  theFlatKnots->SetValue(3, 0.0);
  theFlatKnots->SetValue(4, 0.0);
  theFlatKnots->SetValue(5, 0.33);
  theFlatKnots->SetValue(6, 0.5);
  theFlatKnots->SetValue(7, 0.67);
  theFlatKnots->SetValue(8, 1.0);
  theFlatKnots->SetValue(9, 1.0);
  theFlatKnots->SetValue(10, 1.0);
  theFlatKnots->SetValue(11, 1.0);
}

// Test that cache is properly used across multiple spans.
TEST(BSplCLib_GridEvaluatorTest, CacheReuseWithinSpan)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateMultiSpanBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);

  // Prepare many points - they should use the cache efficiently
  anEval.PrepareParams(0.0, 1.0, 100);

  // Evaluate all points - cache should be reused within each span
  for (int i = 1; i <= anEval.NbParams(); ++i)
  {
    auto aPt = anEval.Value(i);
    ASSERT_TRUE(aPt.has_value());
    EXPECT_FALSE(std::isnan(aPt->X()));
    EXPECT_FALSE(std::isnan(aPt->Y()));
    EXPECT_FALSE(std::isnan(aPt->Z()));
  }
}

// Test evaluation across all spans with knot-aligned parameters.
TEST(BSplCLib_GridEvaluatorTest, MultiSpanKnotAlignedEvaluation)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateMultiSpanBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);

  anEval.PrepareParamsFromKnots(0.0, 1.0, 10);

  // Should have samples in each span
  EXPECT_GE(anEval.NbParams(), 10);

  // Verify span indices are properly distributed
  int aLastSpan    = -1;
  int aSpanChanges = 0;
  for (int i = 1; i <= anEval.NbParams(); ++i)
  {
    auto aData = anEval.ParamData(i);
    ASSERT_TRUE(aData.has_value());

    if (aData->SpanIndex != aLastSpan)
    {
      aSpanChanges++;
      aLastSpan = aData->SpanIndex;
    }

    // Evaluate point
    auto aPt = anEval.Value(i);
    ASSERT_TRUE(aPt.has_value());
    EXPECT_FALSE(std::isnan(aPt->X()));
  }

  // Should have crossed multiple spans
  EXPECT_GE(aSpanChanges, 2);
}

// Test reinitialization resets the cache properly.
TEST(BSplCLib_GridEvaluatorTest, ReinitializationResetsCache)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);
  anEval.PrepareParams(0.0, 1.0, 10);

  // Evaluate to populate cache
  auto aPt1 = anEval.Value(5);
  ASSERT_TRUE(aPt1.has_value());

  // Create a different curve
  Handle(TColgp_HArray1OfPnt) aNewPoles = new TColgp_HArray1OfPnt(1, 5);
  aNewPoles->SetValue(1, gp_Pnt(10.0, 0.0, 0.0));
  aNewPoles->SetValue(2, gp_Pnt(11.0, 2.0, 0.0));
  aNewPoles->SetValue(3, gp_Pnt(12.0, 2.0, 1.0));
  aNewPoles->SetValue(4, gp_Pnt(13.0, 1.0, 1.0));
  aNewPoles->SetValue(5, gp_Pnt(14.0, 0.0, 0.0));

  // Reinitialize with new poles
  anEval.Initialize(aDeg, aNewPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);
  anEval.PrepareParams(0.0, 1.0, 10);

  // Evaluate - should get points from new curve, not cached old ones
  auto aPt2 = anEval.Value(1);
  ASSERT_TRUE(aPt2.has_value());
  EXPECT_NEAR(aPt2->X(), 10.0, Precision::Confusion());
}

// Test D1 derivatives are consistent across span boundaries.
TEST(BSplCLib_GridEvaluatorTest, DerivativeContinuityAcrossSpans)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);

  // Evaluate near knot at 0.5
  anEval.PrepareParams(0.49, 0.51, 3, true);

  gp_Pnt aPt1, aPt2, aPt3;
  gp_Vec aD1_1, aD1_2, aD1_3;

  ASSERT_TRUE(anEval.D1(1, aPt1, aD1_1));
  ASSERT_TRUE(anEval.D1(2, aPt2, aD1_2));
  ASSERT_TRUE(anEval.D1(3, aPt3, aD1_3));

  // For C2 continuous B-spline, derivatives should be similar near knot
  EXPECT_LT((aD1_1 - aD1_2).Magnitude(), 0.5);
  EXPECT_LT((aD1_2 - aD1_3).Magnitude(), 0.5);
}

// Test evaluation at exact knot values.
TEST(BSplCLib_GridEvaluatorTest, EvaluationAtKnotValues)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);

  // Prepare parameters exactly at knot values
  anEval.PrepareParams(0.0, 1.0, 3, true);

  // Manually check that we can evaluate at 0.0, 0.5, 1.0
  auto aPt0   = anEval.Value(1);
  auto aPtMid = anEval.Value(2);
  auto aPt1   = anEval.Value(3);

  ASSERT_TRUE(aPt0.has_value());
  ASSERT_TRUE(aPtMid.has_value());
  ASSERT_TRUE(aPt1.has_value());

  // Start and end should match control points
  EXPECT_NEAR(aPt0->X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aPt1->X(), 4.0, Precision::Confusion());
}

// Test with very high number of samples to stress test cache.
TEST(BSplCLib_GridEvaluatorTest, HighSampleCountStressTest)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateMultiSpanBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);

  // Large number of samples
  anEval.PrepareParams(0.0, 1.0, 1000);

  EXPECT_EQ(anEval.NbParams(), 1000);

  // Evaluate all points
  gp_Pnt aPrevPt;
  for (int i = 1; i <= anEval.NbParams(); ++i)
  {
    gp_Pnt aPt;
    ASSERT_TRUE(anEval.D0(i, aPt));
    EXPECT_FALSE(std::isnan(aPt.X()));

    // Points should form a continuous curve - distance between consecutive points should be small
    if (i > 1)
    {
      const double aDist = aPt.Distance(aPrevPt);
      EXPECT_LT(aDist, 0.1); // Reasonable distance for dense sampling
    }
    aPrevPt = aPt;
  }
}

// Test partial parameter range evaluation.
TEST(BSplCLib_GridEvaluatorTest, PartialRangeEvaluation)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);

  // Evaluate only in [0.2, 0.8] range
  anEval.PrepareParams(0.2, 0.8, 10, true);

  auto aFirstParam = anEval.Param(1);
  auto aLastParam  = anEval.Param(anEval.NbParams());

  ASSERT_TRUE(aFirstParam.has_value());
  ASSERT_TRUE(aLastParam.has_value());

  EXPECT_NEAR(*aFirstParam, 0.2, Precision::Confusion());
  EXPECT_NEAR(*aLastParam, 0.8, Precision::Confusion());

  // All evaluated points should be in valid range
  for (int i = 1; i <= anEval.NbParams(); ++i)
  {
    auto aPt = anEval.Value(i);
    ASSERT_TRUE(aPt.has_value());
    // X coordinate should be between approximately 0.8 and 3.2 for this curve
    EXPECT_GT(aPt->X(), 0.5);
    EXPECT_LT(aPt->X(), 3.5);
  }
}

// Test D2 second derivatives.
TEST(BSplCLib_GridEvaluatorTest, SecondDerivativeEvaluation)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);
  anEval.PrepareParams(0.0, 1.0, 10, true);

  // Evaluate D2 at multiple points
  for (int i = 1; i <= anEval.NbParams(); ++i)
  {
    gp_Pnt aPt;
    gp_Vec aD1, aD2;
    ASSERT_TRUE(anEval.D2(i, aPt, aD1, aD2));

    // Values should not be NaN
    EXPECT_FALSE(std::isnan(aPt.X()));
    EXPECT_FALSE(std::isnan(aD1.X()));
    EXPECT_FALSE(std::isnan(aD2.X()));
  }
}

// Test that minimum samples constraint is respected.
TEST(BSplCLib_GridEvaluatorTest, MinimumSamplesConstraint)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);

  // Request very few samples
  anEval.PrepareParams(0.0, 1.0, 2);
  EXPECT_GE(anEval.NbParams(), 2);

  // Request minimum with knot-aligned
  anEval.PrepareParamsFromKnots(0.0, 1.0, 50);
  EXPECT_GE(anEval.NbParams(), 50);
}

// Verifies EvaluateGrid returns array with correct size and values.
TEST(BSplCLib_GridEvaluatorTest, EvaluateGrid)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);
  anEval.PrepareParams(0.0, 1.0, 10, true);

  // Get all points at once
  NCollection_Array1<gp_Pnt> aPoints = anEval.EvaluateGrid();

  // Check array size matches number of parameters
  EXPECT_EQ(aPoints.Length(), anEval.NbParams());
  EXPECT_EQ(aPoints.Lower(), 1);
  EXPECT_EQ(aPoints.Upper(), 10);

  // Verify values match individual evaluation
  for (int i = 1; i <= anEval.NbParams(); ++i)
  {
    auto aPtSingle = anEval.Value(i);
    ASSERT_TRUE(aPtSingle.has_value());

    const gp_Pnt& aPtGrid = aPoints.Value(i);
    EXPECT_NEAR(aPtGrid.X(), aPtSingle->X(), Precision::Confusion());
    EXPECT_NEAR(aPtGrid.Y(), aPtSingle->Y(), Precision::Confusion());
    EXPECT_NEAR(aPtGrid.Z(), aPtSingle->Z(), Precision::Confusion());
  }

  // First and last should be at curve endpoints
  EXPECT_NEAR(aPoints.Value(1).X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aPoints.Value(10).X(), 4.0, Precision::Confusion());
}

// Verifies EvaluateGrid returns empty array when not initialized.
TEST(BSplCLib_GridEvaluatorTest, EvaluateGrid_NotInitialized)
{
  BSplCLib_GridEvaluator anEval;

  NCollection_Array1<gp_Pnt> aPoints = anEval.EvaluateGrid();

  EXPECT_TRUE(aPoints.IsEmpty());
}

// Verifies EvaluateGrid returns empty array when no parameters prepared.
TEST(BSplCLib_GridEvaluatorTest, EvaluateGrid_NoParams)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);
  // Note: PrepareParams not called

  NCollection_Array1<gp_Pnt> aPoints = anEval.EvaluateGrid();

  EXPECT_TRUE(aPoints.IsEmpty());
}
