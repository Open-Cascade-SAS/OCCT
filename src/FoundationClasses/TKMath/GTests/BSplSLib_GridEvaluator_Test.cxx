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
#include <TColgp_HArray2OfPnt.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>

namespace
{

//! Creates a quadratic B-spline surface (degree 2x2) with planar geometry z = x + y.
//! Surface has 4x4 control points and two spans in each direction.
//! @param[out] thePoles      control points array
//! @param[out] theUFlatKnots flat knot vector in U direction
//! @param[out] theVFlatKnots flat knot vector in V direction
//! @param[out] theDegreeU    polynomial degree in U direction
//! @param[out] theDegreeV    polynomial degree in V direction
void createQuadraticBSplineSurface(Handle(TColgp_HArray2OfPnt)&   thePoles,
                                   Handle(TColStd_HArray1OfReal)& theUFlatKnots,
                                   Handle(TColStd_HArray1OfReal)& theVFlatKnots,
                                   int&                           theDegreeU,
                                   int&                           theDegreeV)
{
  theDegreeU = 2;
  theDegreeV = 2;

  thePoles = new TColgp_HArray2OfPnt(1, 4, 1, 4);
  for (int i = 1; i <= 4; ++i)
  {
    for (int j = 1; j <= 4; ++j)
    {
      const double x = (i - 1) / 3.0;
      const double y = (j - 1) / 3.0;
      const double z = x + y;
      thePoles->SetValue(i, j, gp_Pnt(x, y, z));
    }
  }

  // Clamped knot vector with one internal knot at 0.5
  theUFlatKnots = new TColStd_HArray1OfReal(1, 7);
  theUFlatKnots->SetValue(1, 0.0);
  theUFlatKnots->SetValue(2, 0.0);
  theUFlatKnots->SetValue(3, 0.0);
  theUFlatKnots->SetValue(4, 0.5);
  theUFlatKnots->SetValue(5, 1.0);
  theUFlatKnots->SetValue(6, 1.0);
  theUFlatKnots->SetValue(7, 1.0);

  theVFlatKnots = new TColStd_HArray1OfReal(1, 7);
  theVFlatKnots->SetValue(1, 0.0);
  theVFlatKnots->SetValue(2, 0.0);
  theVFlatKnots->SetValue(3, 0.0);
  theVFlatKnots->SetValue(4, 0.5);
  theVFlatKnots->SetValue(5, 1.0);
  theVFlatKnots->SetValue(6, 1.0);
  theVFlatKnots->SetValue(7, 1.0);
}

//! Creates a bicubic Bezier surface with paraboloid-like geometry z = x^2 + y^2.
//! @return control points array (4x4 poles for degree 3x3)
Handle(TColgp_HArray2OfPnt) createBicubicBezierSurface()
{
  Handle(TColgp_HArray2OfPnt) aPoles = new TColgp_HArray2OfPnt(1, 4, 1, 4);
  for (int i = 1; i <= 4; ++i)
  {
    for (int j = 1; j <= 4; ++j)
    {
      const double x = (i - 1) / 3.0;
      const double y = (j - 1) / 3.0;
      const double z = x * x + y * y;
      aPoles->SetValue(i, j, gp_Pnt(x, y, z));
    }
  }
  return aPoles;
}

//! Creates a quadratic rational B-spline surface (degree 2x2) with 3x3 control points.
//! @param[out] thePoles      control points array
//! @param[out] theWeights    weights array for rational surface
//! @param[out] theUFlatKnots flat knot vector in U direction
//! @param[out] theVFlatKnots flat knot vector in V direction
//! @param[out] theDegreeU    polynomial degree in U direction
//! @param[out] theDegreeV    polynomial degree in V direction
void createRationalBSplineSurface(Handle(TColgp_HArray2OfPnt)&   thePoles,
                                  Handle(TColStd_HArray2OfReal)& theWeights,
                                  Handle(TColStd_HArray1OfReal)& theUFlatKnots,
                                  Handle(TColStd_HArray1OfReal)& theVFlatKnots,
                                  int&                           theDegreeU,
                                  int&                           theDegreeV)
{
  theDegreeU = 2;
  theDegreeV = 2;

  thePoles   = new TColgp_HArray2OfPnt(1, 3, 1, 3);
  theWeights = new TColStd_HArray2OfReal(1, 3, 1, 3);

  const double aW = 1.0 / std::sqrt(2.0);

  thePoles->SetValue(1, 1, gp_Pnt(1.0, 0.0, 0.0));
  thePoles->SetValue(1, 2, gp_Pnt(1.0, 1.0, 0.0));
  thePoles->SetValue(1, 3, gp_Pnt(0.0, 1.0, 0.0));
  thePoles->SetValue(2, 1, gp_Pnt(1.0, 0.0, 1.0));
  thePoles->SetValue(2, 2, gp_Pnt(1.0, 1.0, 1.0));
  thePoles->SetValue(2, 3, gp_Pnt(0.0, 1.0, 1.0));
  thePoles->SetValue(3, 1, gp_Pnt(0.0, 0.0, 1.0));
  thePoles->SetValue(3, 2, gp_Pnt(0.0, 0.0, 1.0));
  thePoles->SetValue(3, 3, gp_Pnt(0.0, 0.0, 1.0));

  theWeights->SetValue(1, 1, 1.0);
  theWeights->SetValue(1, 2, aW);
  theWeights->SetValue(1, 3, 1.0);
  theWeights->SetValue(2, 1, aW);
  theWeights->SetValue(2, 2, 0.5);
  theWeights->SetValue(2, 3, aW);
  theWeights->SetValue(3, 1, 1.0);
  theWeights->SetValue(3, 2, aW);
  theWeights->SetValue(3, 3, 1.0);

  // Clamped Bezier-style knots for single span
  theUFlatKnots = new TColStd_HArray1OfReal(1, 6);
  theUFlatKnots->SetValue(1, 0.0);
  theUFlatKnots->SetValue(2, 0.0);
  theUFlatKnots->SetValue(3, 0.0);
  theUFlatKnots->SetValue(4, 1.0);
  theUFlatKnots->SetValue(5, 1.0);
  theUFlatKnots->SetValue(6, 1.0);

  theVFlatKnots = new TColStd_HArray1OfReal(1, 6);
  theVFlatKnots->SetValue(1, 0.0);
  theVFlatKnots->SetValue(2, 0.0);
  theVFlatKnots->SetValue(3, 0.0);
  theVFlatKnots->SetValue(4, 1.0);
  theVFlatKnots->SetValue(5, 1.0);
  theVFlatKnots->SetValue(6, 1.0);
}

} // namespace

// Verifies that default-constructed evaluator is in uninitialized state.
TEST(BSplSLib_GridEvaluatorTest, DefaultConstructor)
{
  BSplSLib_GridEvaluator anEval;
  EXPECT_FALSE(anEval.IsInitialized());
  EXPECT_EQ(anEval.NbUParams(), 0);
  EXPECT_EQ(anEval.NbVParams(), 0);
}

// Verifies successful initialization with valid B-spline surface data.
TEST(BSplSLib_GridEvaluatorTest, Initialize)
{
  Handle(TColgp_HArray2OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aUKnots, aVKnots;
  int                           aDegU, aDegV;

  createQuadraticBSplineSurface(aPoles, aUKnots, aVKnots, aDegU, aDegV);

  BSplSLib_GridEvaluator anEval;
  const bool             isOk = anEval.Initialize(aDegU,
                                      aDegV,
                                      aPoles,
                                      Handle(TColStd_HArray2OfReal)(),
                                      aUKnots,
                                      aVKnots,
                                      false,
                                      false,
                                      false,
                                      false);

  EXPECT_TRUE(isOk);
  EXPECT_TRUE(anEval.IsInitialized());
}

// Verifies that initialization fails when degree is zero.
TEST(BSplSLib_GridEvaluatorTest, InitializeValidation_InvalidDegree)
{
  Handle(TColgp_HArray2OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aUKnots, aVKnots;
  int                           aDegU, aDegV;

  createQuadraticBSplineSurface(aPoles, aUKnots, aVKnots, aDegU, aDegV);

  BSplSLib_GridEvaluator anEval;

  bool isOk = anEval.Initialize(0,
                                aDegV,
                                aPoles,
                                Handle(TColStd_HArray2OfReal)(),
                                aUKnots,
                                aVKnots,
                                false,
                                false,
                                false,
                                false);
  EXPECT_FALSE(isOk);

  isOk = anEval.Initialize(aDegU,
                           0,
                           aPoles,
                           Handle(TColStd_HArray2OfReal)(),
                           aUKnots,
                           aVKnots,
                           false,
                           false,
                           false,
                           false);
  EXPECT_FALSE(isOk);
}

// Verifies that initialization fails when rational flag is set but weights are null.
TEST(BSplSLib_GridEvaluatorTest, InitializeValidation_RationalWithoutWeights)
{
  Handle(TColgp_HArray2OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aUKnots, aVKnots;
  int                           aDegU, aDegV;

  createQuadraticBSplineSurface(aPoles, aUKnots, aVKnots, aDegU, aDegV);

  BSplSLib_GridEvaluator anEval;

  const bool isOk = anEval.Initialize(aDegU,
                                      aDegV,
                                      aPoles,
                                      Handle(TColStd_HArray2OfReal)(),
                                      aUKnots,
                                      aVKnots,
                                      true,
                                      false,
                                      false,
                                      false);
  EXPECT_FALSE(isOk);
  EXPECT_FALSE(anEval.IsInitialized());
}

// Verifies that uniform parameter preparation creates the correct number of samples.
TEST(BSplSLib_GridEvaluatorTest, PrepareUniformParams)
{
  Handle(TColgp_HArray2OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aUKnots, aVKnots;
  int                           aDegU, aDegV;

  createQuadraticBSplineSurface(aPoles, aUKnots, aVKnots, aDegU, aDegV);

  BSplSLib_GridEvaluator anEval;
  anEval.Initialize(aDegU,
                    aDegV,
                    aPoles,
                    Handle(TColStd_HArray2OfReal)(),
                    aUKnots,
                    aVKnots,
                    false,
                    false,
                    false,
                    false);

  anEval.PrepareUParams(0.0, 1.0, 5);
  anEval.PrepareVParams(0.0, 1.0, 5);

  EXPECT_EQ(anEval.NbUParams(), 5);
  EXPECT_EQ(anEval.NbVParams(), 5);

  for (int i = 1; i <= anEval.NbUParams(); ++i)
  {
    auto aParam = anEval.UParam(i);
    ASSERT_TRUE(aParam.has_value());
    EXPECT_GE(*aParam, 0.0);
    EXPECT_LE(*aParam, 1.0);
  }
}

// Verifies that uniform parameters include exact boundary values when requested.
TEST(BSplSLib_GridEvaluatorTest, PrepareUniformParamsWithExactBoundaries)
{
  Handle(TColgp_HArray2OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aUKnots, aVKnots;
  int                           aDegU, aDegV;

  createQuadraticBSplineSurface(aPoles, aUKnots, aVKnots, aDegU, aDegV);

  BSplSLib_GridEvaluator anEval;
  anEval.Initialize(aDegU,
                    aDegV,
                    aPoles,
                    Handle(TColStd_HArray2OfReal)(),
                    aUKnots,
                    aVKnots,
                    false,
                    false,
                    false,
                    false);

  anEval.PrepareUParams(0.0, 1.0, 5, true);
  anEval.PrepareVParams(0.0, 1.0, 5, true);

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

// Verifies that knot-aligned parameter preparation creates valid span indices.
TEST(BSplSLib_GridEvaluatorTest, PrepareKnotAlignedParams)
{
  Handle(TColgp_HArray2OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aUKnots, aVKnots;
  int                           aDegU, aDegV;

  createQuadraticBSplineSurface(aPoles, aUKnots, aVKnots, aDegU, aDegV);

  BSplSLib_GridEvaluator anEval;
  anEval.Initialize(aDegU,
                    aDegV,
                    aPoles,
                    Handle(TColStd_HArray2OfReal)(),
                    aUKnots,
                    aVKnots,
                    false,
                    false,
                    false,
                    false);

  anEval.PrepareUParamsFromKnots(0.0, 1.0, 3);
  anEval.PrepareVParamsFromKnots(0.0, 1.0, 3);

  EXPECT_GE(anEval.NbUParams(), 3);
  EXPECT_GE(anEval.NbVParams(), 3);

  for (int i = 1; i <= anEval.NbUParams(); ++i)
  {
    auto aData = anEval.UParamData(i);
    ASSERT_TRUE(aData.has_value());
    EXPECT_GE(aData->SpanIndex, aDegU);
  }
}

// Verifies that D0 evaluation produces results identical to direct BSplSLib::D0 calls.
TEST(BSplSLib_GridEvaluatorTest, D0Evaluation)
{
  Handle(TColgp_HArray2OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aUKnots, aVKnots;
  int                           aDegU, aDegV;

  createQuadraticBSplineSurface(aPoles, aUKnots, aVKnots, aDegU, aDegV);

  BSplSLib_GridEvaluator anEval;
  anEval.Initialize(aDegU,
                    aDegV,
                    aPoles,
                    Handle(TColStd_HArray2OfReal)(),
                    aUKnots,
                    aVKnots,
                    false,
                    false,
                    false,
                    false);

  anEval.PrepareUParams(0.0, 1.0, 5);
  anEval.PrepareVParams(0.0, 1.0, 5);

  for (int iu = 1; iu <= anEval.NbUParams(); ++iu)
  {
    for (int iv = 1; iv <= anEval.NbVParams(); ++iv)
    {
      auto aPtGrid = anEval.Value(iu, iv);
      ASSERT_TRUE(aPtGrid.has_value());

      gp_Pnt aPtDirect;
      auto   aUData = anEval.UParamData(iu);
      auto   aVData = anEval.VParamData(iv);
      ASSERT_TRUE(aUData.has_value());
      ASSERT_TRUE(aVData.has_value());

      BSplSLib::D0(aUData->Param,
                   aVData->Param,
                   aUData->SpanIndex,
                   aVData->SpanIndex,
                   aPoles->Array2(),
                   nullptr,
                   aUKnots->Array1(),
                   aVKnots->Array1(),
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

// Verifies that D1 evaluation computes non-zero first derivatives.
TEST(BSplSLib_GridEvaluatorTest, D1Evaluation)
{
  Handle(TColgp_HArray2OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aUKnots, aVKnots;
  int                           aDegU, aDegV;

  createQuadraticBSplineSurface(aPoles, aUKnots, aVKnots, aDegU, aDegV);

  BSplSLib_GridEvaluator anEval;
  anEval.Initialize(aDegU,
                    aDegV,
                    aPoles,
                    Handle(TColStd_HArray2OfReal)(),
                    aUKnots,
                    aVKnots,
                    false,
                    false,
                    false,
                    false);

  anEval.PrepareUParams(0.0, 1.0, 3);
  anEval.PrepareVParams(0.0, 1.0, 3);

  gp_Pnt     aPt;
  gp_Vec     aDU, aDV;
  const bool isOk = anEval.D1(2, 2, aPt, aDU, aDV);

  EXPECT_TRUE(isOk);
  EXPECT_GT(aDU.Magnitude() + aDV.Magnitude(), Precision::Confusion());
}

// Verifies that D2 evaluation completes without producing NaN values.
TEST(BSplSLib_GridEvaluatorTest, D2Evaluation)
{
  Handle(TColgp_HArray2OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aUKnots, aVKnots;
  int                           aDegU, aDegV;

  createQuadraticBSplineSurface(aPoles, aUKnots, aVKnots, aDegU, aDegV);

  BSplSLib_GridEvaluator anEval;
  anEval.Initialize(aDegU,
                    aDegV,
                    aPoles,
                    Handle(TColStd_HArray2OfReal)(),
                    aUKnots,
                    aVKnots,
                    false,
                    false,
                    false,
                    false);

  anEval.PrepareUParams(0.0, 1.0, 3);
  anEval.PrepareVParams(0.0, 1.0, 3);

  gp_Pnt     aPt;
  gp_Vec     aDU, aDV, aDUU, aDVV, aDUV;
  const bool isOk = anEval.D2(2, 2, aPt, aDU, aDV, aDUU, aDVV, aDUV);

  EXPECT_TRUE(isOk);
  EXPECT_FALSE(std::isnan(aPt.X()));
}

// Verifies that out-of-range indices return std::nullopt or false.
TEST(BSplSLib_GridEvaluatorTest, InvalidIndexReturnsNullopt)
{
  Handle(TColgp_HArray2OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aUKnots, aVKnots;
  int                           aDegU, aDegV;

  createQuadraticBSplineSurface(aPoles, aUKnots, aVKnots, aDegU, aDegV);

  BSplSLib_GridEvaluator anEval;
  anEval.Initialize(aDegU,
                    aDegV,
                    aPoles,
                    Handle(TColStd_HArray2OfReal)(),
                    aUKnots,
                    aVKnots,
                    false,
                    false,
                    false,
                    false);
  anEval.PrepareUParams(0.0, 1.0, 5);
  anEval.PrepareVParams(0.0, 1.0, 5);

  EXPECT_FALSE(anEval.Value(0, 1).has_value());
  EXPECT_FALSE(anEval.Value(1, 0).has_value());
  EXPECT_FALSE(anEval.Value(6, 1).has_value());
  EXPECT_FALSE(anEval.Value(1, 6).has_value());
  EXPECT_FALSE(anEval.UParam(0).has_value());
  EXPECT_FALSE(anEval.VParam(0).has_value());

  gp_Pnt aPt;
  gp_Vec aDU, aDV, aDUU, aDVV, aDUV;
  EXPECT_FALSE(anEval.D0(0, 1, aPt));
  EXPECT_FALSE(anEval.D1(0, 1, aPt, aDU, aDV));
  EXPECT_FALSE(anEval.D2(0, 1, aPt, aDU, aDV, aDUU, aDVV, aDUV));
}

// Verifies that rational surface evaluation produces valid non-NaN coordinates.
TEST(BSplSLib_GridEvaluatorTest, RationalSurfaceEvaluation)
{
  Handle(TColgp_HArray2OfPnt)   aPoles;
  Handle(TColStd_HArray2OfReal) aWeights;
  Handle(TColStd_HArray1OfReal) aUKnots, aVKnots;
  int                           aDegU, aDegV;

  createRationalBSplineSurface(aPoles, aWeights, aUKnots, aVKnots, aDegU, aDegV);

  BSplSLib_GridEvaluator anEval;
  const bool             isOk =
    anEval.Initialize(aDegU, aDegV, aPoles, aWeights, aUKnots, aVKnots, true, true, false, false);

  ASSERT_TRUE(isOk);

  anEval.PrepareUParams(0.0, 1.0, 5, true);
  anEval.PrepareVParams(0.0, 1.0, 5, true);

  for (int iu = 1; iu <= anEval.NbUParams(); ++iu)
  {
    for (int iv = 1; iv <= anEval.NbVParams(); ++iv)
    {
      auto aPt = anEval.Value(iu, iv);
      ASSERT_TRUE(aPt.has_value());
      EXPECT_FALSE(std::isnan(aPt->X()));
      EXPECT_FALSE(std::isnan(aPt->Y()));
      EXPECT_FALSE(std::isnan(aPt->Z()));
    }
  }
}

// Verifies that InitializeBezier succeeds with valid control points.
TEST(BSplSLib_GridEvaluatorTest, BezierSurfaceInitialization)
{
  Handle(TColgp_HArray2OfPnt) aPoles = createBicubicBezierSurface();

  BSplSLib_GridEvaluator anEval;
  const bool             isOk = anEval.InitializeBezier(aPoles, Handle(TColStd_HArray2OfReal)());

  ASSERT_TRUE(isOk);
  EXPECT_TRUE(anEval.IsInitialized());
}

// Verifies that Bezier surface corner points match the control polygon corners.
TEST(BSplSLib_GridEvaluatorTest, BezierSurfaceEvaluation)
{
  Handle(TColgp_HArray2OfPnt) aPoles = createBicubicBezierSurface();

  BSplSLib_GridEvaluator anEval;
  anEval.InitializeBezier(aPoles, Handle(TColStd_HArray2OfReal)());

  anEval.PrepareUParams(0.0, 1.0, 5, true);
  anEval.PrepareVParams(0.0, 1.0, 5, true);

  auto aCorner00 = anEval.Value(1, 1);
  auto aCorner10 = anEval.Value(anEval.NbUParams(), 1);
  auto aCorner01 = anEval.Value(1, anEval.NbVParams());
  auto aCorner11 = anEval.Value(anEval.NbUParams(), anEval.NbVParams());

  ASSERT_TRUE(aCorner00.has_value());
  ASSERT_TRUE(aCorner10.has_value());
  ASSERT_TRUE(aCorner01.has_value());
  ASSERT_TRUE(aCorner11.has_value());

  EXPECT_NEAR(aCorner00->X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aCorner00->Y(), 0.0, Precision::Confusion());

  EXPECT_NEAR(aCorner10->X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aCorner10->Y(), 0.0, Precision::Confusion());

  EXPECT_NEAR(aCorner01->X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aCorner01->Y(), 1.0, Precision::Confusion());

  EXPECT_NEAR(aCorner11->X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aCorner11->Y(), 1.0, Precision::Confusion());
}

// Verifies that InitializeBezier fails when control point array has insufficient size.
TEST(BSplSLib_GridEvaluatorTest, BezierSurfaceValidation_TooFewPoles)
{
  Handle(TColgp_HArray2OfPnt) aPoles = new TColgp_HArray2OfPnt(1, 1, 1, 2);
  aPoles->SetValue(1, 1, gp_Pnt(0, 0, 0));
  aPoles->SetValue(1, 2, gp_Pnt(1, 0, 0));

  BSplSLib_GridEvaluator anEval;
  const bool             isOk = anEval.InitializeBezier(aPoles, Handle(TColStd_HArray2OfReal)());

  EXPECT_FALSE(isOk);
  EXPECT_FALSE(anEval.IsInitialized());
}

// Verifies that uniform and knot-aligned parameter methods produce consistent corner values.
TEST(BSplSLib_GridEvaluatorTest, ConsistencyWithKnotAlignedParams)
{
  Handle(TColgp_HArray2OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aUKnots, aVKnots;
  int                           aDegU, aDegV;

  createQuadraticBSplineSurface(aPoles, aUKnots, aVKnots, aDegU, aDegV);

  BSplSLib_GridEvaluator anEvalUniform;
  anEvalUniform.Initialize(aDegU,
                           aDegV,
                           aPoles,
                           Handle(TColStd_HArray2OfReal)(),
                           aUKnots,
                           aVKnots,
                           false,
                           false,
                           false,
                           false);
  anEvalUniform.PrepareUParams(0.0, 1.0, 10, true);
  anEvalUniform.PrepareVParams(0.0, 1.0, 10, true);

  BSplSLib_GridEvaluator anEvalKnots;
  anEvalKnots.Initialize(aDegU,
                         aDegV,
                         aPoles,
                         Handle(TColStd_HArray2OfReal)(),
                         aUKnots,
                         aVKnots,
                         false,
                         false,
                         false,
                         false);
  anEvalKnots.PrepareUParamsFromKnots(0.0, 1.0, 10, true);
  anEvalKnots.PrepareVParamsFromKnots(0.0, 1.0, 10, true);

  auto aPt1 = anEvalUniform.Value(1, 1);
  auto aPt2 = anEvalKnots.Value(1, 1);

  ASSERT_TRUE(aPt1.has_value());
  ASSERT_TRUE(aPt2.has_value());

  EXPECT_NEAR(aPt1->X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aPt1->Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aPt2->X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aPt2->Y(), 0.0, Precision::Confusion());
}

// Verifies that Handle accessor methods return valid data after initialization.
TEST(BSplSLib_GridEvaluatorTest, HandleAccessors)
{
  Handle(TColgp_HArray2OfPnt)   aPoles;
  Handle(TColStd_HArray2OfReal) aWeights;
  Handle(TColStd_HArray1OfReal) aUKnots, aVKnots;
  int                           aDegU, aDegV;

  createRationalBSplineSurface(aPoles, aWeights, aUKnots, aVKnots, aDegU, aDegV);

  BSplSLib_GridEvaluator anEval;
  anEval.Initialize(aDegU, aDegV, aPoles, aWeights, aUKnots, aVKnots, true, true, false, false);

  EXPECT_EQ(anEval.DegreeU(), aDegU);
  EXPECT_EQ(anEval.DegreeV(), aDegV);
  EXPECT_TRUE(anEval.IsURational());
  EXPECT_TRUE(anEval.IsVRational());
  EXPECT_FALSE(anEval.IsUPeriodic());
  EXPECT_FALSE(anEval.IsVPeriodic());

  EXPECT_FALSE(anEval.Poles().IsNull());
  EXPECT_FALSE(anEval.Weights().IsNull());
  EXPECT_FALSE(anEval.UFlatKnots().IsNull());
  EXPECT_FALSE(anEval.VFlatKnots().IsNull());

  EXPECT_EQ(anEval.Poles()->ColLength(), 3);
  EXPECT_EQ(anEval.Poles()->RowLength(), 3);
  EXPECT_EQ(anEval.Weights()->ColLength(), 3);
  EXPECT_EQ(anEval.Weights()->RowLength(), 3);
  EXPECT_EQ(anEval.UFlatKnots()->Length(), 6);
  EXPECT_EQ(anEval.VFlatKnots()->Length(), 6);
}
