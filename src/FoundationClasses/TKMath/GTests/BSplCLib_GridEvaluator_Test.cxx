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
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>

// Helper function to create a simple cubic BSpline curve
// Control points form a simple arc
static void CreateSimpleBSplineCurve(TColgp_Array1OfPnt&   thePoles,
                                     TColStd_Array1OfReal& theFlatKnots,
                                     int&                  theDegree)
{
  theDegree = 3;

  // 5 control points
  thePoles.Resize(1, 5, false);
  thePoles.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  thePoles.SetValue(2, gp_Pnt(1.0, 2.0, 0.0));
  thePoles.SetValue(3, gp_Pnt(2.0, 2.0, 1.0));
  thePoles.SetValue(4, gp_Pnt(3.0, 1.0, 1.0));
  thePoles.SetValue(5, gp_Pnt(4.0, 0.0, 0.0));

  // Flat knots for degree 3 with 5 control points
  // n + p + 1 = 5 + 3 + 1 = 9 knots
  // Clamped knot vector: [0, 0, 0, 0, 0.5, 1, 1, 1, 1]
  theFlatKnots.Resize(1, 9, false);
  theFlatKnots.SetValue(1, 0.0);
  theFlatKnots.SetValue(2, 0.0);
  theFlatKnots.SetValue(3, 0.0);
  theFlatKnots.SetValue(4, 0.0);
  theFlatKnots.SetValue(5, 0.5);
  theFlatKnots.SetValue(6, 1.0);
  theFlatKnots.SetValue(7, 1.0);
  theFlatKnots.SetValue(8, 1.0);
  theFlatKnots.SetValue(9, 1.0);
}

TEST(BSplCLib_GridEvaluatorTest, DefaultConstructor)
{
  BSplCLib_GridEvaluator anEval;
  EXPECT_FALSE(anEval.IsInitialized());
  EXPECT_EQ(anEval.NbParams(), 0);
}

TEST(BSplCLib_GridEvaluatorTest, Initialize)
{
  TColgp_Array1OfPnt   aPoles;
  TColStd_Array1OfReal aKnots;
  int                  aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg,
                    aPoles,
                    nullptr, // non-rational
                    aKnots,
                    false,   // not rational
                    false);  // not periodic

  EXPECT_TRUE(anEval.IsInitialized());
}

TEST(BSplCLib_GridEvaluatorTest, PrepareUniformParams)
{
  TColgp_Array1OfPnt   aPoles;
  TColStd_Array1OfReal aKnots;
  int                  aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, nullptr, aKnots, false, false);

  anEval.PrepareParams(0.0, 1.0, 10);

  EXPECT_EQ(anEval.NbParams(), 10);

  // Check that parameters are in range
  for (int i = 1; i <= anEval.NbParams(); ++i)
  {
    EXPECT_GE(anEval.Param(i), 0.0);
    EXPECT_LE(anEval.Param(i), 1.0);
  }
}

TEST(BSplCLib_GridEvaluatorTest, PrepareKnotAlignedParams)
{
  TColgp_Array1OfPnt   aPoles;
  TColStd_Array1OfReal aKnots;
  int                  aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, nullptr, aKnots, false, false);

  anEval.PrepareParamsFromKnots(0.0, 1.0, 5);

  // Should have at least 5 samples
  EXPECT_GE(anEval.NbParams(), 5);

  // Check that span indices are valid
  for (int i = 1; i <= anEval.NbParams(); ++i)
  {
    const auto& aData = anEval.ParamData(i);
    EXPECT_GE(aData.SpanIndex, aDeg);
  }
}

TEST(BSplCLib_GridEvaluatorTest, D0Evaluation)
{
  TColgp_Array1OfPnt   aPoles;
  TColStd_Array1OfReal aKnots;
  int                  aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, nullptr, aKnots, false, false);

  anEval.PrepareParams(0.0, 1.0, 10);

  // Evaluate and compare with direct BSplCLib evaluation
  for (int i = 1; i <= anEval.NbParams(); ++i)
  {
    gp_Pnt aPtGrid = anEval.Value(i);

    // Direct evaluation using BSplCLib for comparison
    gp_Pnt       aPtDirect;
    const auto& aData = anEval.ParamData(i);

    BSplCLib::D0(aData.Param,
                 aData.SpanIndex,
                 aDeg,
                 false, // not periodic
                 aPoles,
                 nullptr,
                 aKnots,
                 nullptr, // NoMults for flat knots
                 aPtDirect);

    EXPECT_NEAR(aPtGrid.X(), aPtDirect.X(), Precision::Confusion());
    EXPECT_NEAR(aPtGrid.Y(), aPtDirect.Y(), Precision::Confusion());
    EXPECT_NEAR(aPtGrid.Z(), aPtDirect.Z(), Precision::Confusion());
  }
}

TEST(BSplCLib_GridEvaluatorTest, D1Evaluation)
{
  TColgp_Array1OfPnt   aPoles;
  TColStd_Array1OfReal aKnots;
  int                  aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, nullptr, aKnots, false, false);

  anEval.PrepareParams(0.0, 1.0, 5);

  // Test D1 evaluation
  gp_Pnt aPt;
  gp_Vec aD1;
  anEval.D1(3, aPt, aD1);

  // For a BSpline curve, derivative should be non-zero
  EXPECT_GT(aD1.Magnitude(), Precision::Confusion());
}

TEST(BSplCLib_GridEvaluatorTest, D2Evaluation)
{
  TColgp_Array1OfPnt   aPoles;
  TColStd_Array1OfReal aKnots;
  int                  aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, nullptr, aKnots, false, false);

  anEval.PrepareParams(0.0, 1.0, 5);

  // Test D2 evaluation - should not crash
  gp_Pnt aPt;
  gp_Vec aD1, aD2;
  anEval.D2(3, aPt, aD1, aD2);

  // Point should be valid
  EXPECT_FALSE(aPt.X() != aPt.X()); // Not NaN
}

TEST(BSplCLib_GridEvaluatorTest, EndpointsMatch)
{
  TColgp_Array1OfPnt   aPoles;
  TColStd_Array1OfReal aKnots;
  int                  aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, nullptr, aKnots, false, false);

  anEval.PrepareParams(0.0, 1.0, 10);

  // First and last evaluated points should be close to first/last control points
  // (for clamped BSpline)
  gp_Pnt aFirstPt = anEval.Value(1);
  gp_Pnt aLastPt  = anEval.Value(anEval.NbParams());

  // Should be close to first control point (0, 0, 0) and last (4, 0, 0)
  EXPECT_NEAR(aFirstPt.X(), 0.0, 0.1);
  EXPECT_NEAR(aFirstPt.Y(), 0.0, 0.1);
  EXPECT_NEAR(aLastPt.X(), 4.0, 0.1);
  EXPECT_NEAR(aLastPt.Y(), 0.0, 0.1);
}

TEST(BSplCLib_GridEvaluatorTest, ConsistencyBetweenMethods)
{
  TColgp_Array1OfPnt   aPoles;
  TColStd_Array1OfReal aKnots;
  int                  aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  // Create two evaluators - one with uniform, one with knot-aligned params
  BSplCLib_GridEvaluator anEvalUniform;
  anEvalUniform.Initialize(aDeg, aPoles, nullptr, aKnots, false, false);
  anEvalUniform.PrepareParams(0.0, 1.0, 20);

  BSplCLib_GridEvaluator anEvalKnots;
  anEvalKnots.Initialize(aDeg, aPoles, nullptr, aKnots, false, false);
  anEvalKnots.PrepareParamsFromKnots(0.0, 1.0, 20);

  // Both should produce valid results at endpoints
  gp_Pnt aPt1 = anEvalUniform.Value(1);
  gp_Pnt aPt2 = anEvalKnots.Value(1);

  // First point should be near (0,0,0) since that's the start
  EXPECT_NEAR(aPt1.X(), 0.0, 0.1);
  EXPECT_NEAR(aPt1.Y(), 0.0, 0.1);
  EXPECT_NEAR(aPt2.X(), 0.0, 0.1);
  EXPECT_NEAR(aPt2.Y(), 0.0, 0.1);
}
