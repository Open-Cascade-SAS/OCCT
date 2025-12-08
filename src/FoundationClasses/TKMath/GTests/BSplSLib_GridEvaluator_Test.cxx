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

#include <BSplSLib_GridEvaluator.hxx>
#include <BSplSLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>

// Helper function to create a simple planar B-spline surface (degree 2x2).
// Control points form a flat plane z = x + y.
static void CreateSimpleBSplineSurface(TColgp_Array2OfPnt&   thePoles,
                                       TColStd_Array1OfReal& theUFlatKnots,
                                       TColStd_Array1OfReal& theVFlatKnots,
                                       int&                  theDegreeU,
                                       int&                  theDegreeV)
{
  theDegreeU = 2;
  theDegreeV = 2;

  // 4x4 control points
  thePoles.Resize(1, 4, 1, 4, false);
  for (int i = 1; i <= 4; ++i)
  {
    for (int j = 1; j <= 4; ++j)
    {
      const double x = (i - 1) / 3.0;
      const double y = (j - 1) / 3.0;
      const double z = x + y; // Simple plane z = x + y
      thePoles.SetValue(i, j, gp_Pnt(x, y, z));
    }
  }

  // Flat knots for degree 2 with 2 internal spans in each direction
  // Knot vector: [0, 0, 0, 0.5, 1, 1, 1]
  theUFlatKnots.Resize(1, 7, false);
  theUFlatKnots.SetValue(1, 0.0);
  theUFlatKnots.SetValue(2, 0.0);
  theUFlatKnots.SetValue(3, 0.0);
  theUFlatKnots.SetValue(4, 0.5);
  theUFlatKnots.SetValue(5, 1.0);
  theUFlatKnots.SetValue(6, 1.0);
  theUFlatKnots.SetValue(7, 1.0);

  theVFlatKnots.Resize(1, 7, false);
  theVFlatKnots.SetValue(1, 0.0);
  theVFlatKnots.SetValue(2, 0.0);
  theVFlatKnots.SetValue(3, 0.0);
  theVFlatKnots.SetValue(4, 0.5);
  theVFlatKnots.SetValue(5, 1.0);
  theVFlatKnots.SetValue(6, 1.0);
  theVFlatKnots.SetValue(7, 1.0);
}

// Helper function to create Bezier surface control points.
static void CreateBezierSurface(TColgp_Array2OfPnt& thePoles)
{
  // Bicubic Bezier surface (degree 3x3)
  thePoles.Resize(1, 4, 1, 4, false);
  for (int i = 1; i <= 4; ++i)
  {
    for (int j = 1; j <= 4; ++j)
    {
      const double x = (i - 1) / 3.0;
      const double y = (j - 1) / 3.0;
      // Simple paraboloid-like surface
      const double z = x * x + y * y;
      thePoles.SetValue(i, j, gp_Pnt(x, y, z));
    }
  }
}

// Helper function to create a rational B-spline surface (spherical patch).
static void CreateRationalBSplineSurface(TColgp_Array2OfPnt&   thePoles,
                                         TColStd_Array2OfReal& theWeights,
                                         TColStd_Array1OfReal& theUFlatKnots,
                                         TColStd_Array1OfReal& theVFlatKnots,
                                         int&                  theDegreeU,
                                         int&                  theDegreeV)
{
  theDegreeU = 2;
  theDegreeV = 2;

  // 3x3 control points
  thePoles.Resize(1, 3, 1, 3, false);
  theWeights.Resize(1, 3, 1, 3, false);

  // Create a simple rational surface patch
  const double aW = 1.0 / std::sqrt(2.0);

  thePoles.SetValue(1, 1, gp_Pnt(1.0, 0.0, 0.0));
  thePoles.SetValue(1, 2, gp_Pnt(1.0, 1.0, 0.0));
  thePoles.SetValue(1, 3, gp_Pnt(0.0, 1.0, 0.0));
  thePoles.SetValue(2, 1, gp_Pnt(1.0, 0.0, 1.0));
  thePoles.SetValue(2, 2, gp_Pnt(1.0, 1.0, 1.0));
  thePoles.SetValue(2, 3, gp_Pnt(0.0, 1.0, 1.0));
  thePoles.SetValue(3, 1, gp_Pnt(0.0, 0.0, 1.0));
  thePoles.SetValue(3, 2, gp_Pnt(0.0, 0.0, 1.0));
  thePoles.SetValue(3, 3, gp_Pnt(0.0, 0.0, 1.0));

  theWeights.SetValue(1, 1, 1.0);
  theWeights.SetValue(1, 2, aW);
  theWeights.SetValue(1, 3, 1.0);
  theWeights.SetValue(2, 1, aW);
  theWeights.SetValue(2, 2, 0.5);
  theWeights.SetValue(2, 3, aW);
  theWeights.SetValue(3, 1, 1.0);
  theWeights.SetValue(3, 2, aW);
  theWeights.SetValue(3, 3, 1.0);

  // Flat knots: [0,0,0,1,1,1]
  theUFlatKnots.Resize(1, 6, false);
  theUFlatKnots.SetValue(1, 0.0);
  theUFlatKnots.SetValue(2, 0.0);
  theUFlatKnots.SetValue(3, 0.0);
  theUFlatKnots.SetValue(4, 1.0);
  theUFlatKnots.SetValue(5, 1.0);
  theUFlatKnots.SetValue(6, 1.0);

  theVFlatKnots.Resize(1, 6, false);
  theVFlatKnots.SetValue(1, 0.0);
  theVFlatKnots.SetValue(2, 0.0);
  theVFlatKnots.SetValue(3, 0.0);
  theVFlatKnots.SetValue(4, 1.0);
  theVFlatKnots.SetValue(5, 1.0);
  theVFlatKnots.SetValue(6, 1.0);
}

TEST(BSplSLib_GridEvaluatorTest, DefaultConstructor)
{
  BSplSLib_GridEvaluator anEval;
  EXPECT_FALSE(anEval.IsInitialized());
  EXPECT_EQ(anEval.NbUParams(), 0);
  EXPECT_EQ(anEval.NbVParams(), 0);
}

TEST(BSplSLib_GridEvaluatorTest, Initialize)
{
  TColgp_Array2OfPnt   aPoles;
  TColStd_Array1OfReal aUKnots, aVKnots;
  int                  aDegU, aDegV;

  CreateSimpleBSplineSurface(aPoles, aUKnots, aVKnots, aDegU, aDegV);

  BSplSLib_GridEvaluator anEval;
  const bool             isOk = anEval.Initialize(aDegU,
                                      aDegV,
                                      aPoles,
                                      nullptr, // non-rational
                                      aUKnots,
                                      aVKnots,
                                      false, // not U-rational
                                      false, // not V-rational
                                      false, // not U-periodic
                                      false);  // not V-periodic

  EXPECT_TRUE(isOk);
  EXPECT_TRUE(anEval.IsInitialized());
}

TEST(BSplSLib_GridEvaluatorTest, InitializeValidation_InvalidDegree)
{
  TColgp_Array2OfPnt   aPoles;
  TColStd_Array1OfReal aUKnots, aVKnots;
  int                  aDegU, aDegV;

  CreateSimpleBSplineSurface(aPoles, aUKnots, aVKnots, aDegU, aDegV);

  BSplSLib_GridEvaluator anEval;

  // Degree 0 in U should fail
  bool isOk = anEval.Initialize(0, aDegV, aPoles, nullptr, aUKnots, aVKnots, false, false, false, false);
  EXPECT_FALSE(isOk);

  // Degree 0 in V should fail
  isOk = anEval.Initialize(aDegU, 0, aPoles, nullptr, aUKnots, aVKnots, false, false, false, false);
  EXPECT_FALSE(isOk);
}

TEST(BSplSLib_GridEvaluatorTest, InitializeValidation_RationalWithoutWeights)
{
  TColgp_Array2OfPnt   aPoles;
  TColStd_Array1OfReal aUKnots, aVKnots;
  int                  aDegU, aDegV;

  CreateSimpleBSplineSurface(aPoles, aUKnots, aVKnots, aDegU, aDegV);

  BSplSLib_GridEvaluator anEval;

  // Rational surface without weights should fail
  const bool isOk =
    anEval.Initialize(aDegU, aDegV, aPoles, nullptr, aUKnots, aVKnots, true, false, false, false);
  EXPECT_FALSE(isOk);
  EXPECT_FALSE(anEval.IsInitialized());
}

TEST(BSplSLib_GridEvaluatorTest, PrepareUniformParams)
{
  TColgp_Array2OfPnt   aPoles;
  TColStd_Array1OfReal aUKnots, aVKnots;
  int                  aDegU, aDegV;

  CreateSimpleBSplineSurface(aPoles, aUKnots, aVKnots, aDegU, aDegV);

  BSplSLib_GridEvaluator anEval;
  anEval.Initialize(aDegU, aDegV, aPoles, nullptr, aUKnots, aVKnots, false, false, false, false);

  anEval.PrepareUParams(0.0, 1.0, 5);
  anEval.PrepareVParams(0.0, 1.0, 5);

  EXPECT_EQ(anEval.NbUParams(), 5);
  EXPECT_EQ(anEval.NbVParams(), 5);

  // Check that parameters are in range
  for (int i = 1; i <= anEval.NbUParams(); ++i)
  {
    auto aParam = anEval.UParam(i);
    ASSERT_TRUE(aParam.has_value());
    EXPECT_GE(*aParam, 0.0);
    EXPECT_LE(*aParam, 1.0);
  }
}

TEST(BSplSLib_GridEvaluatorTest, PrepareUniformParamsWithExactBoundaries)
{
  TColgp_Array2OfPnt   aPoles;
  TColStd_Array1OfReal aUKnots, aVKnots;
  int                  aDegU, aDegV;

  CreateSimpleBSplineSurface(aPoles, aUKnots, aVKnots, aDegU, aDegV);

  BSplSLib_GridEvaluator anEval;
  anEval.Initialize(aDegU, aDegV, aPoles, nullptr, aUKnots, aVKnots, false, false, false, false);

  // With theIncludeEnds = true (default)
  anEval.PrepareUParams(0.0, 1.0, 5, true);
  anEval.PrepareVParams(0.0, 1.0, 5, true);

  // First and last parameters should be exactly at boundaries
  auto aFirstU = anEval.UParam(1);
  auto aLastU  = anEval.UParam(anEval.NbUParams());
  auto aFirstV = anEval.VParam(1);
  auto aLastV  = anEval.VParam(anEval.NbVParams());

  ASSERT_TRUE(aFirstU.has_value());
  ASSERT_TRUE(aLastU.has_value());
  ASSERT_TRUE(aFirstV.has_value());
  ASSERT_TRUE(aLastV.has_value());

  EXPECT_DOUBLE_EQ(*aFirstU, 0.0);
  EXPECT_DOUBLE_EQ(*aLastU, 1.0);
  EXPECT_DOUBLE_EQ(*aFirstV, 0.0);
  EXPECT_DOUBLE_EQ(*aLastV, 1.0);
}

TEST(BSplSLib_GridEvaluatorTest, PrepareKnotAlignedParams)
{
  TColgp_Array2OfPnt   aPoles;
  TColStd_Array1OfReal aUKnots, aVKnots;
  int                  aDegU, aDegV;

  CreateSimpleBSplineSurface(aPoles, aUKnots, aVKnots, aDegU, aDegV);

  BSplSLib_GridEvaluator anEval;
  anEval.Initialize(aDegU, aDegV, aPoles, nullptr, aUKnots, aVKnots, false, false, false, false);

  anEval.PrepareUParamsFromKnots(0.0, 1.0, 3);
  anEval.PrepareVParamsFromKnots(0.0, 1.0, 3);

  // Should have at least 3 samples
  EXPECT_GE(anEval.NbUParams(), 3);
  EXPECT_GE(anEval.NbVParams(), 3);

  // Check that span indices are valid
  for (int i = 1; i <= anEval.NbUParams(); ++i)
  {
    auto aData = anEval.UParamData(i);
    ASSERT_TRUE(aData.has_value());
    EXPECT_GE(aData->SpanIndex, aDegU);
  }
}

TEST(BSplSLib_GridEvaluatorTest, D0Evaluation)
{
  TColgp_Array2OfPnt   aPoles;
  TColStd_Array1OfReal aUKnots, aVKnots;
  int                  aDegU, aDegV;

  CreateSimpleBSplineSurface(aPoles, aUKnots, aVKnots, aDegU, aDegV);

  BSplSLib_GridEvaluator anEval;
  anEval.Initialize(aDegU, aDegV, aPoles, nullptr, aUKnots, aVKnots, false, false, false, false);

  anEval.PrepareUParams(0.0, 1.0, 5);
  anEval.PrepareVParams(0.0, 1.0, 5);

  // Evaluate and compare with direct BSplSLib evaluation
  for (int iu = 1; iu <= anEval.NbUParams(); ++iu)
  {
    for (int iv = 1; iv <= anEval.NbVParams(); ++iv)
    {
      auto aPtGrid = anEval.Value(iu, iv);
      ASSERT_TRUE(aPtGrid.has_value());

      // Direct evaluation using BSplSLib for comparison
      gp_Pnt aPtDirect;
      auto   aUData = anEval.UParamData(iu);
      auto   aVData = anEval.VParamData(iv);
      ASSERT_TRUE(aUData.has_value());
      ASSERT_TRUE(aVData.has_value());

      BSplSLib::D0(aUData->Param,
                   aVData->Param,
                   aUData->SpanIndex,
                   aVData->SpanIndex,
                   aPoles,
                   nullptr,
                   aUKnots,
                   aVKnots,
                   nullptr,
                   nullptr,
                   aDegU,
                   aDegV,
                   false,
                   false,
                   false,
                   false,
                   aPtDirect);

      EXPECT_NEAR(aPtGrid->X(), aPtDirect.X(), Precision::Confusion());
      EXPECT_NEAR(aPtGrid->Y(), aPtDirect.Y(), Precision::Confusion());
      EXPECT_NEAR(aPtGrid->Z(), aPtDirect.Z(), Precision::Confusion());
    }
  }
}

TEST(BSplSLib_GridEvaluatorTest, D1Evaluation)
{
  TColgp_Array2OfPnt   aPoles;
  TColStd_Array1OfReal aUKnots, aVKnots;
  int                  aDegU, aDegV;

  CreateSimpleBSplineSurface(aPoles, aUKnots, aVKnots, aDegU, aDegV);

  BSplSLib_GridEvaluator anEval;
  anEval.Initialize(aDegU, aDegV, aPoles, nullptr, aUKnots, aVKnots, false, false, false, false);

  anEval.PrepareUParams(0.0, 1.0, 3);
  anEval.PrepareVParams(0.0, 1.0, 3);

  // Test D1 evaluation
  gp_Pnt     aPt;
  gp_Vec     aDU, aDV;
  const bool isOk = anEval.D1(2, 2, aPt, aDU, aDV);

  EXPECT_TRUE(isOk);
  // For a B-spline surface, derivatives should be non-zero
  EXPECT_GT(aDU.Magnitude() + aDV.Magnitude(), Precision::Confusion());
}

TEST(BSplSLib_GridEvaluatorTest, D2Evaluation)
{
  TColgp_Array2OfPnt   aPoles;
  TColStd_Array1OfReal aUKnots, aVKnots;
  int                  aDegU, aDegV;

  CreateSimpleBSplineSurface(aPoles, aUKnots, aVKnots, aDegU, aDegV);

  BSplSLib_GridEvaluator anEval;
  anEval.Initialize(aDegU, aDegV, aPoles, nullptr, aUKnots, aVKnots, false, false, false, false);

  anEval.PrepareUParams(0.0, 1.0, 3);
  anEval.PrepareVParams(0.0, 1.0, 3);

  // Test D2 evaluation - should not crash
  gp_Pnt     aPt;
  gp_Vec     aDU, aDV, aDUU, aDVV, aDUV;
  const bool isOk = anEval.D2(2, 2, aPt, aDU, aDV, aDUU, aDVV, aDUV);

  EXPECT_TRUE(isOk);
  // Point should be valid (not NaN)
  EXPECT_FALSE(std::isnan(aPt.X()));
}

TEST(BSplSLib_GridEvaluatorTest, InvalidIndexReturnsNullopt)
{
  TColgp_Array2OfPnt   aPoles;
  TColStd_Array1OfReal aUKnots, aVKnots;
  int                  aDegU, aDegV;

  CreateSimpleBSplineSurface(aPoles, aUKnots, aVKnots, aDegU, aDegV);

  BSplSLib_GridEvaluator anEval;
  anEval.Initialize(aDegU, aDegV, aPoles, nullptr, aUKnots, aVKnots, false, false, false, false);
  anEval.PrepareUParams(0.0, 1.0, 5);
  anEval.PrepareVParams(0.0, 1.0, 5);

  // Invalid indices
  EXPECT_FALSE(anEval.Value(0, 1).has_value());
  EXPECT_FALSE(anEval.Value(1, 0).has_value());
  EXPECT_FALSE(anEval.Value(6, 1).has_value());
  EXPECT_FALSE(anEval.Value(1, 6).has_value());
  EXPECT_FALSE(anEval.UParam(0).has_value());
  EXPECT_FALSE(anEval.VParam(0).has_value());

  // D0/D1/D2 should return false for invalid indices
  gp_Pnt aPt;
  gp_Vec aDU, aDV, aDUU, aDVV, aDUV;
  EXPECT_FALSE(anEval.D0(0, 1, aPt));
  EXPECT_FALSE(anEval.D1(0, 1, aPt, aDU, aDV));
  EXPECT_FALSE(anEval.D2(0, 1, aPt, aDU, aDV, aDUU, aDVV, aDUV));
}

TEST(BSplSLib_GridEvaluatorTest, RationalSurfaceEvaluation)
{
  TColgp_Array2OfPnt   aPoles;
  TColStd_Array2OfReal aWeights;
  TColStd_Array1OfReal aUKnots, aVKnots;
  int                  aDegU, aDegV;

  CreateRationalBSplineSurface(aPoles, aWeights, aUKnots, aVKnots, aDegU, aDegV);

  BSplSLib_GridEvaluator anEval;
  const bool             isOk =
    anEval.Initialize(aDegU, aDegV, aPoles, &aWeights, aUKnots, aVKnots, true, true, false, false);

  ASSERT_TRUE(isOk);

  anEval.PrepareUParams(0.0, 1.0, 5, true);
  anEval.PrepareVParams(0.0, 1.0, 5, true);

  // All evaluations should succeed
  for (int iu = 1; iu <= anEval.NbUParams(); ++iu)
  {
    for (int iv = 1; iv <= anEval.NbVParams(); ++iv)
    {
      auto aPt = anEval.Value(iu, iv);
      ASSERT_TRUE(aPt.has_value());
      // Point should be valid (not NaN)
      EXPECT_FALSE(std::isnan(aPt->X()));
      EXPECT_FALSE(std::isnan(aPt->Y()));
      EXPECT_FALSE(std::isnan(aPt->Z()));
    }
  }
}

TEST(BSplSLib_GridEvaluatorTest, BezierSurfaceInitialization)
{
  TColgp_Array2OfPnt aPoles;
  CreateBezierSurface(aPoles);

  BSplSLib_GridEvaluator anEval;
  const bool             isOk = anEval.InitializeBezier(aPoles, nullptr);

  ASSERT_TRUE(isOk);
  EXPECT_TRUE(anEval.IsInitialized());
}

TEST(BSplSLib_GridEvaluatorTest, BezierSurfaceEvaluation)
{
  TColgp_Array2OfPnt aPoles;
  CreateBezierSurface(aPoles);

  BSplSLib_GridEvaluator anEval;
  anEval.InitializeBezier(aPoles, nullptr);

  anEval.PrepareUParams(0.0, 1.0, 5, true);
  anEval.PrepareVParams(0.0, 1.0, 5, true);

  // Corner points should match control points
  auto aCorner00 = anEval.Value(1, 1);
  auto aCorner10 = anEval.Value(anEval.NbUParams(), 1);
  auto aCorner01 = anEval.Value(1, anEval.NbVParams());
  auto aCorner11 = anEval.Value(anEval.NbUParams(), anEval.NbVParams());

  ASSERT_TRUE(aCorner00.has_value());
  ASSERT_TRUE(aCorner10.has_value());
  ASSERT_TRUE(aCorner01.has_value());
  ASSERT_TRUE(aCorner11.has_value());

  // (0,0) corner
  EXPECT_NEAR(aCorner00->X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aCorner00->Y(), 0.0, Precision::Confusion());

  // (1,0) corner
  EXPECT_NEAR(aCorner10->X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aCorner10->Y(), 0.0, Precision::Confusion());

  // (0,1) corner
  EXPECT_NEAR(aCorner01->X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aCorner01->Y(), 1.0, Precision::Confusion());

  // (1,1) corner
  EXPECT_NEAR(aCorner11->X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aCorner11->Y(), 1.0, Precision::Confusion());
}

TEST(BSplSLib_GridEvaluatorTest, BezierSurfaceValidation_TooFewPoles)
{
  // 1x2 poles - not enough for a surface
  TColgp_Array2OfPnt aPoles(1, 1, 1, 2);
  aPoles.SetValue(1, 1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(1, 2, gp_Pnt(1, 0, 0));

  BSplSLib_GridEvaluator anEval;
  const bool             isOk = anEval.InitializeBezier(aPoles, nullptr);

  EXPECT_FALSE(isOk);
  EXPECT_FALSE(anEval.IsInitialized());
}

TEST(BSplSLib_GridEvaluatorTest, ConsistencyWithKnotAlignedParams)
{
  TColgp_Array2OfPnt   aPoles;
  TColStd_Array1OfReal aUKnots, aVKnots;
  int                  aDegU, aDegV;

  CreateSimpleBSplineSurface(aPoles, aUKnots, aVKnots, aDegU, aDegV);

  // Create two evaluators - one with uniform, one with knot-aligned params
  BSplSLib_GridEvaluator anEvalUniform;
  anEvalUniform.Initialize(aDegU, aDegV, aPoles, nullptr, aUKnots, aVKnots, false, false, false, false);
  anEvalUniform.PrepareUParams(0.0, 1.0, 10, true);
  anEvalUniform.PrepareVParams(0.0, 1.0, 10, true);

  BSplSLib_GridEvaluator anEvalKnots;
  anEvalKnots.Initialize(aDegU, aDegV, aPoles, nullptr, aUKnots, aVKnots, false, false, false, false);
  anEvalKnots.PrepareUParamsFromKnots(0.0, 1.0, 10, true);
  anEvalKnots.PrepareVParamsFromKnots(0.0, 1.0, 10, true);

  // Both should produce valid results at corners
  auto aPt1 = anEvalUniform.Value(1, 1);
  auto aPt2 = anEvalKnots.Value(1, 1);

  ASSERT_TRUE(aPt1.has_value());
  ASSERT_TRUE(aPt2.has_value());

  // First point should be near (0,0,0) since that's the corner
  EXPECT_NEAR(aPt1->X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aPt1->Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aPt2->X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aPt2->Y(), 0.0, Precision::Confusion());
}
