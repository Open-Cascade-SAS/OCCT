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

//! Creates a multi-span B-spline surface (degree 3x3) with multiple spans in each direction.
//! @param[out] thePoles      control points array (6x6)
//! @param[out] theUFlatKnots flat knot vector in U direction
//! @param[out] theVFlatKnots flat knot vector in V direction
//! @param[out] theDegreeU    polynomial degree in U direction
//! @param[out] theDegreeV    polynomial degree in V direction
void createMultiSpanBSplineSurface(Handle(TColgp_HArray2OfPnt)&   thePoles,
                                   Handle(TColStd_HArray1OfReal)& theUFlatKnots,
                                   Handle(TColStd_HArray1OfReal)& theVFlatKnots,
                                   int&                           theDegreeU,
                                   int&                           theDegreeV)
{
  theDegreeU = 3;
  theDegreeV = 3;

  // 6x6 control points for 2 spans in each direction
  thePoles = new TColgp_HArray2OfPnt(1, 6, 1, 6);
  for (int i = 1; i <= 6; ++i)
  {
    for (int j = 1; j <= 6; ++j)
    {
      const double x = (i - 1) / 5.0;
      const double y = (j - 1) / 5.0;
      // Add some curvature
      const double z = 0.5 * std::sin(x * 3.14159) * std::sin(y * 3.14159);
      thePoles->SetValue(i, j, gp_Pnt(x, y, z));
    }
  }

  // Flat knots: 6 poles + degree 3 + 1 = 10 knots per direction
  // Clamped with 2 internal knots: [0,0,0,0, 0.5, 0.75, 1,1,1,1]
  theUFlatKnots = new TColStd_HArray1OfReal(1, 10);
  theUFlatKnots->SetValue(1, 0.0);
  theUFlatKnots->SetValue(2, 0.0);
  theUFlatKnots->SetValue(3, 0.0);
  theUFlatKnots->SetValue(4, 0.0);
  theUFlatKnots->SetValue(5, 0.4);
  theUFlatKnots->SetValue(6, 0.7);
  theUFlatKnots->SetValue(7, 1.0);
  theUFlatKnots->SetValue(8, 1.0);
  theUFlatKnots->SetValue(9, 1.0);
  theUFlatKnots->SetValue(10, 1.0);

  theVFlatKnots = new TColStd_HArray1OfReal(1, 10);
  theVFlatKnots->SetValue(1, 0.0);
  theVFlatKnots->SetValue(2, 0.0);
  theVFlatKnots->SetValue(3, 0.0);
  theVFlatKnots->SetValue(4, 0.0);
  theVFlatKnots->SetValue(5, 0.35);
  theVFlatKnots->SetValue(6, 0.65);
  theVFlatKnots->SetValue(7, 1.0);
  theVFlatKnots->SetValue(8, 1.0);
  theVFlatKnots->SetValue(9, 1.0);
  theVFlatKnots->SetValue(10, 1.0);
}

// Verifies that cache is properly used across a large grid evaluation.
TEST(BSplSLib_GridEvaluatorTest, CacheReuseWithinSpan)
{
  Handle(TColgp_HArray2OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aUKnots, aVKnots;
  int                           aDegU, aDegV;

  createMultiSpanBSplineSurface(aPoles, aUKnots, aVKnots, aDegU, aDegV);

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

  // Prepare many points - they should use the cache efficiently
  anEval.PrepareUParams(0.0, 1.0, 50);
  anEval.PrepareVParams(0.0, 1.0, 50);

  // Evaluate all grid points - cache should be reused within each span
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

// Verifies that evaluation across all spans with knot-aligned parameters works correctly.
TEST(BSplSLib_GridEvaluatorTest, MultiSpanKnotAlignedEvaluation)
{
  Handle(TColgp_HArray2OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aUKnots, aVKnots;
  int                           aDegU, aDegV;

  createMultiSpanBSplineSurface(aPoles, aUKnots, aVKnots, aDegU, aDegV);

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

  anEval.PrepareUParamsFromKnots(0.0, 1.0, 5);
  anEval.PrepareVParamsFromKnots(0.0, 1.0, 5);

  // Should have samples in each span
  EXPECT_GE(anEval.NbUParams(), 5);
  EXPECT_GE(anEval.NbVParams(), 5);

  // Verify U span indices are properly distributed
  int aLastUSpan    = -1;
  int aUSpanChanges = 0;
  for (int i = 1; i <= anEval.NbUParams(); ++i)
  {
    auto aData = anEval.UParamData(i);
    ASSERT_TRUE(aData.has_value());
    if (aData->SpanIndex != aLastUSpan)
    {
      aUSpanChanges++;
      aLastUSpan = aData->SpanIndex;
    }
  }
  // Should have crossed multiple spans
  EXPECT_GE(aUSpanChanges, 2);

  // Evaluate all grid points
  for (int iu = 1; iu <= anEval.NbUParams(); ++iu)
  {
    for (int iv = 1; iv <= anEval.NbVParams(); ++iv)
    {
      auto aPt = anEval.Value(iu, iv);
      ASSERT_TRUE(aPt.has_value());
      EXPECT_FALSE(std::isnan(aPt->X()));
    }
  }
}

// Verifies that reinitialization properly resets the internal cache.
TEST(BSplSLib_GridEvaluatorTest, ReinitializationResetsCache)
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

  // Evaluate to populate cache
  auto aPt1 = anEval.Value(3, 3);
  ASSERT_TRUE(aPt1.has_value());

  // Create a different surface with offset control points
  Handle(TColgp_HArray2OfPnt) aNewPoles = new TColgp_HArray2OfPnt(1, 4, 1, 4);
  for (int i = 1; i <= 4; ++i)
  {
    for (int j = 1; j <= 4; ++j)
    {
      const double x = (i - 1) / 3.0 + 10.0; // Offset by 10
      const double y = (j - 1) / 3.0 + 10.0;
      const double z = x + y;
      aNewPoles->SetValue(i, j, gp_Pnt(x, y, z));
    }
  }

  // Reinitialize with new poles
  anEval.Initialize(aDegU,
                    aDegV,
                    aNewPoles,
                    Handle(TColStd_HArray2OfReal)(),
                    aUKnots,
                    aVKnots,
                    false,
                    false,
                    false,
                    false);
  anEval.PrepareUParams(0.0, 1.0, 5);
  anEval.PrepareVParams(0.0, 1.0, 5);

  // Evaluate - should get points from new surface, not cached old ones
  auto aPt2 = anEval.Value(1, 1);
  ASSERT_TRUE(aPt2.has_value());
  EXPECT_NEAR(aPt2->X(), 10.0, Precision::Confusion());
  EXPECT_NEAR(aPt2->Y(), 10.0, Precision::Confusion());
}

// Verifies that D1 derivatives are consistent across span boundaries.
TEST(BSplSLib_GridEvaluatorTest, DerivativeContinuityAcrossSpans)
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

  // Evaluate near knot at 0.5 in U direction
  anEval.PrepareUParams(0.49, 0.51, 3, true);
  anEval.PrepareVParams(0.5, 0.5, 1, true); // Fixed V

  gp_Pnt aPt1, aPt2, aPt3;
  gp_Vec aDU1, aDV1, aDU2, aDV2, aDU3, aDV3;

  ASSERT_TRUE(anEval.D1(1, 1, aPt1, aDU1, aDV1));
  ASSERT_TRUE(anEval.D1(2, 1, aPt2, aDU2, aDV2));
  ASSERT_TRUE(anEval.D1(3, 1, aPt3, aDU3, aDV3));

  // For C1 continuous B-spline, derivatives should be similar near knot
  EXPECT_LT((aDU1 - aDU2).Magnitude(), 0.5);
  EXPECT_LT((aDU2 - aDU3).Magnitude(), 0.5);
}

// Verifies that evaluation works correctly at exact knot values.
TEST(BSplSLib_GridEvaluatorTest, EvaluationAtKnotValues)
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

  // Prepare parameters exactly at knot values
  anEval.PrepareUParams(0.0, 1.0, 3, true); // 0.0, 0.5, 1.0
  anEval.PrepareVParams(0.0, 1.0, 3, true);

  // Evaluate corners
  auto aCorner00 = anEval.Value(1, 1);
  auto aCorner11 = anEval.Value(3, 3);

  ASSERT_TRUE(aCorner00.has_value());
  ASSERT_TRUE(aCorner11.has_value());

  // Corners should match control points for clamped surface
  EXPECT_NEAR(aCorner00->X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aCorner00->Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aCorner11->X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aCorner11->Y(), 1.0, Precision::Confusion());
}

// Verifies that a high sample count grid evaluation stress tests the cache properly.
TEST(BSplSLib_GridEvaluatorTest, HighSampleCountStressTest)
{
  Handle(TColgp_HArray2OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aUKnots, aVKnots;
  int                           aDegU, aDegV;

  createMultiSpanBSplineSurface(aPoles, aUKnots, aVKnots, aDegU, aDegV);

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

  // Large grid
  anEval.PrepareUParams(0.0, 1.0, 100);
  anEval.PrepareVParams(0.0, 1.0, 100);

  EXPECT_EQ(anEval.NbUParams(), 100);
  EXPECT_EQ(anEval.NbVParams(), 100);

  // Evaluate all points
  for (int iu = 1; iu <= anEval.NbUParams(); ++iu)
  {
    for (int iv = 1; iv <= anEval.NbVParams(); ++iv)
    {
      gp_Pnt aPt;
      ASSERT_TRUE(anEval.D0(iu, iv, aPt));
      EXPECT_FALSE(std::isnan(aPt.X()));

      // Points should be within expected bounds
      EXPECT_GE(aPt.X(), -0.01);
      EXPECT_LE(aPt.X(), 1.01);
      EXPECT_GE(aPt.Y(), -0.01);
      EXPECT_LE(aPt.Y(), 1.01);
    }
  }
}

// Verifies that partial range evaluation constrains parameters correctly.
TEST(BSplSLib_GridEvaluatorTest, PartialRangeEvaluation)
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

  // Evaluate only in [0.2, 0.8] range
  anEval.PrepareUParams(0.2, 0.8, 5, true);
  anEval.PrepareVParams(0.3, 0.7, 5, true);

  auto aFirstU = anEval.UParam(1);
  auto aLastU  = anEval.UParam(anEval.NbUParams());
  auto aFirstV = anEval.VParam(1);
  auto aLastV  = anEval.VParam(anEval.NbVParams());

  ASSERT_TRUE(aFirstU.has_value());
  ASSERT_TRUE(aLastU.has_value());
  ASSERT_TRUE(aFirstV.has_value());
  ASSERT_TRUE(aLastV.has_value());

  EXPECT_NEAR(*aFirstU, 0.2, Precision::Confusion());
  EXPECT_NEAR(*aLastU, 0.8, Precision::Confusion());
  EXPECT_NEAR(*aFirstV, 0.3, Precision::Confusion());
  EXPECT_NEAR(*aLastV, 0.7, Precision::Confusion());

  // All evaluated points should be in valid range
  for (int iu = 1; iu <= anEval.NbUParams(); ++iu)
  {
    for (int iv = 1; iv <= anEval.NbVParams(); ++iv)
    {
      auto aPt = anEval.Value(iu, iv);
      ASSERT_TRUE(aPt.has_value());
      // X and Y coordinates should be in partial range
      EXPECT_GT(aPt->X(), 0.15);
      EXPECT_LT(aPt->X(), 0.85);
      EXPECT_GT(aPt->Y(), 0.25);
      EXPECT_LT(aPt->Y(), 0.75);
    }
  }
}

// Verifies that D2 second derivatives produce valid values.
TEST(BSplSLib_GridEvaluatorTest, SecondDerivativeEvaluation)
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

  // Evaluate D2 at multiple points
  for (int iu = 1; iu <= anEval.NbUParams(); ++iu)
  {
    for (int iv = 1; iv <= anEval.NbVParams(); ++iv)
    {
      gp_Pnt aPt;
      gp_Vec aDU, aDV, aDUU, aDVV, aDUV;
      ASSERT_TRUE(anEval.D2(iu, iv, aPt, aDU, aDV, aDUU, aDVV, aDUV));

      // Values should not be NaN
      EXPECT_FALSE(std::isnan(aPt.X()));
      EXPECT_FALSE(std::isnan(aDU.X()));
      EXPECT_FALSE(std::isnan(aDV.X()));
      EXPECT_FALSE(std::isnan(aDUU.X()));
      EXPECT_FALSE(std::isnan(aDVV.X()));
      EXPECT_FALSE(std::isnan(aDUV.X()));
    }
  }
}

// Verifies that minimum samples constraint is respected.
TEST(BSplSLib_GridEvaluatorTest, MinimumSamplesConstraint)
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

  // Request very few samples
  anEval.PrepareUParams(0.0, 1.0, 2);
  anEval.PrepareVParams(0.0, 1.0, 2);
  EXPECT_GE(anEval.NbUParams(), 2);
  EXPECT_GE(anEval.NbVParams(), 2);

  // Request minimum with knot-aligned
  anEval.PrepareUParamsFromKnots(0.0, 1.0, 20);
  anEval.PrepareVParamsFromKnots(0.0, 1.0, 20);
  EXPECT_GE(anEval.NbUParams(), 20);
  EXPECT_GE(anEval.NbVParams(), 20);
}

// Verifies surface normals can be computed from D1 derivatives.
TEST(BSplSLib_GridEvaluatorTest, SurfaceNormalFromDerivatives)
{
  Handle(TColgp_HArray2OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aUKnots, aVKnots;
  int                           aDegU, aDegV;

  createMultiSpanBSplineSurface(aPoles, aUKnots, aVKnots, aDegU, aDegV);

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

  anEval.PrepareUParams(0.0, 1.0, 10);
  anEval.PrepareVParams(0.0, 1.0, 10);

  // Compute normals at several points
  for (int iu = 2; iu <= anEval.NbUParams() - 1; ++iu)
  {
    for (int iv = 2; iv <= anEval.NbVParams() - 1; ++iv)
    {
      gp_Pnt aPt;
      gp_Vec aDU, aDV;
      ASSERT_TRUE(anEval.D1(iu, iv, aPt, aDU, aDV));

      // Normal = dU x dV
      gp_Vec aNormal = aDU.Crossed(aDV);

      // Normal should be non-zero for non-degenerate surface
      EXPECT_GT(aNormal.Magnitude(), Precision::Confusion());

      // Normal should point in a consistent direction (roughly along Z for this surface)
      if (aNormal.Z() < 0)
      {
        aNormal.Reverse();
      }
      EXPECT_GT(aNormal.Z(), 0.0);
    }
  }
}
