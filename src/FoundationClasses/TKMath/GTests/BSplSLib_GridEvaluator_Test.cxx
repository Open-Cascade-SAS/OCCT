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

// Helper function to create a simple planar BSpline surface (degree 2x2)
// Control points form a flat plane z = x + y
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
      double x = (i - 1) / 3.0;
      double y = (j - 1) / 3.0;
      double z = x + y; // Simple plane z = x + y
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
  anEval.Initialize(aDegU,
                    aDegV,
                    aPoles,
                    nullptr, // non-rational
                    aUKnots,
                    aVKnots,
                    false, // not U-rational
                    false, // not V-rational
                    false, // not U-periodic
                    false);// not V-periodic

  EXPECT_TRUE(anEval.IsInitialized());
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
    EXPECT_GE(anEval.UParam(i), 0.0);
    EXPECT_LE(anEval.UParam(i), 1.0);
  }
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
    const auto& aData = anEval.UParamData(i);
    EXPECT_GE(aData.SpanIndex, aDegU);
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
      gp_Pnt aPtGrid = anEval.Value(iu, iv);

      // Direct evaluation using BSplSLib for comparison
      gp_Pnt aPtDirect;
      const auto& aUData = anEval.UParamData(iu);
      const auto& aVData = anEval.VParamData(iv);

      BSplSLib::D0(aUData.Param,
                   aVData.Param,
                   aUData.SpanIndex,
                   aVData.SpanIndex,
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

      EXPECT_NEAR(aPtGrid.X(), aPtDirect.X(), Precision::Confusion());
      EXPECT_NEAR(aPtGrid.Y(), aPtDirect.Y(), Precision::Confusion());
      EXPECT_NEAR(aPtGrid.Z(), aPtDirect.Z(), Precision::Confusion());
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
  gp_Pnt aPt;
  gp_Vec aDU, aDV;
  anEval.D1(2, 2, aPt, aDU, aDV);

  // For a BSpline surface, derivatives should be non-zero
  // (unless it's a degenerate case)
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
  gp_Pnt aPt;
  gp_Vec aDU, aDV, aDUU, aDVV, aDUV;
  anEval.D2(2, 2, aPt, aDU, aDV, aDUU, aDVV, aDUV);

  // Point should be valid
  EXPECT_FALSE(aPt.X() != aPt.X()); // Not NaN
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
  anEvalUniform.PrepareUParams(0.0, 1.0, 10);
  anEvalUniform.PrepareVParams(0.0, 1.0, 10);

  BSplSLib_GridEvaluator anEvalKnots;
  anEvalKnots.Initialize(aDegU, aDegV, aPoles, nullptr, aUKnots, aVKnots, false, false, false, false);
  anEvalKnots.PrepareUParamsFromKnots(0.0, 1.0, 10);
  anEvalKnots.PrepareVParamsFromKnots(0.0, 1.0, 10);

  // Both should produce valid results at corners
  gp_Pnt aPt1 = anEvalUniform.Value(1, 1);
  gp_Pnt aPt2 = anEvalKnots.Value(1, 1);

  // First point should be near (0,0,0) since that's the corner
  EXPECT_NEAR(aPt1.X(), 0.0, 0.1);
  EXPECT_NEAR(aPt1.Y(), 0.0, 0.1);
  EXPECT_NEAR(aPt2.X(), 0.0, 0.1);
  EXPECT_NEAR(aPt2.Y(), 0.0, 0.1);
}
