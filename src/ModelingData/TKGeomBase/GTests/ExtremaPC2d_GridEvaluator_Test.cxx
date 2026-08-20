// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <ExtremaPC2d_GridEvaluator.hxx>

#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Dir2d.hxx>
#include <NCollection_Array1.hxx>

#include <gtest/gtest.h>

#include <cmath>

//=================================================================================================

TEST(ExtremaPC2d_GridEvaluatorTest, UniformParamsCoverDomainExactly)
{
  const math_Vector aParams = ExtremaPC2d_GridEvaluator::BuildUniformParams(-3.0, 5.0, 7);
  ASSERT_EQ(aParams.Size(), 7u);
  EXPECT_EQ(aParams.At(0), -3.0);
  EXPECT_EQ(aParams.At(6), 5.0);
  for (size_t anIndex = 1; anIndex < aParams.Size(); ++anIndex)
  {
    EXPECT_GT(aParams.At(anIndex), aParams.At(anIndex - 1));
  }
}

TEST(ExtremaPC2d_GridEvaluatorTest, UniformParamsRejectInvalidSampleCount)
{
#if !defined(No_Exception) && !defined(No_Standard_RangeError)
  EXPECT_THROW(ExtremaPC2d_GridEvaluator::BuildUniformParams(0.0, 1.0, 1), Standard_RangeError);
#else
  GTEST_SKIP() << "Standard_RangeError raising is disabled in this build.";
#endif
}

//=================================================================================================

TEST(ExtremaPC2d_GridEvaluatorTest, InvalidPartitionAndToleranceAreRejected)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 2);
  aPoles(1)                              = gp_Pnt2d(0.0, 0.0);
  aPoles(2)                              = gp_Pnt2d(1.0, 0.0);
  occ::handle<Geom2d_BezierCurve> aCurve = new Geom2d_BezierCurve(aPoles);
  Geom2dAdaptor_Curve             anAdaptor(aCurve);
  ExtremaPC2d_GridEvaluator       anEvaluator;
  EXPECT_EQ(
    anEvaluator.Perform(anAdaptor, gp_Pnt2d(), {0.0, 1.0}, 1.0e-8, ExtremaPC2d::SearchMode::MinMax)
      .Status,
    ExtremaPC2d::Status::InvalidInput);

  math_Vector aParams(3);
  aParams.ChangeAt(0) = 0.0;
  aParams.ChangeAt(1) = 0.5;
  aParams.ChangeAt(2) = 1.0;
  anEvaluator.SetParams(aParams);
  EXPECT_EQ(
    anEvaluator.Perform(anAdaptor, gp_Pnt2d(), {1.0, 0.0}, 1.0e-8, ExtremaPC2d::SearchMode::MinMax)
      .Status,
    ExtremaPC2d::Status::InvalidInput);
  EXPECT_EQ(
    anEvaluator.Perform(anAdaptor, gp_Pnt2d(), {0.0, 1.0}, 0.0, ExtremaPC2d::SearchMode::MinMax)
      .Status,
    ExtremaPC2d::Status::InvalidInput);
}

//=================================================================================================

TEST(ExtremaPC2d_GridEvaluatorTest, ConstantCurveHasInfiniteSolutions)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles.Init(gp_Pnt2d(2.0, -1.0));
  occ::handle<Geom2d_BezierCurve> aCurve = new Geom2d_BezierCurve(aPoles);
  Geom2dAdaptor_Curve             anAdaptor(aCurve);
  ExtremaPC2d_GridEvaluator       anEvaluator;
  anEvaluator.SetParams(ExtremaPC2d_GridEvaluator::BuildUniformParams(0.0, 1.0, 8));
  const ExtremaPC2d::Result& aResult = anEvaluator.Perform(anAdaptor,
                                                           gp_Pnt2d(5.0, 3.0),
                                                           {0.0, 1.0},
                                                           1.0e-8,
                                                           ExtremaPC2d::SearchMode::MinMax);
  EXPECT_TRUE(aResult.IsInfinite());
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 25.0, 1.0e-12);
}

//=================================================================================================

TEST(ExtremaPC2d_GridEvaluatorTest, ConstantCircleDistanceHasInfiniteSolutions)
{
  occ::handle<Geom2d_Circle> aCircle =
    new Geom2d_Circle(gp_Ax22d(gp_Pnt2d(), gp_Dir2d(1.0, 0.0), true), 3.0);
  Geom2dAdaptor_Curve       anAdaptor(aCircle);
  ExtremaPC2d_GridEvaluator anEvaluator;
  anEvaluator.SetParams(ExtremaPC2d_GridEvaluator::BuildUniformParams(0.0, 2.0 * M_PI, 32));
  const ExtremaPC2d::Result& aResult = anEvaluator.Perform(anAdaptor,
                                                           gp_Pnt2d(),
                                                           {0.0, 2.0 * M_PI},
                                                           1.0e-8,
                                                           ExtremaPC2d::SearchMode::MinMax);
  EXPECT_TRUE(aResult.IsInfinite());
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 9.0, 1.0e-10);
}

//=================================================================================================

TEST(ExtremaPC2d_GridEvaluatorTest, QuadraticCurveFindsCentralMinimum)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles(1)                              = gp_Pnt2d(-2.0, 0.0);
  aPoles(2)                              = gp_Pnt2d(0.0, 2.0);
  aPoles(3)                              = gp_Pnt2d(2.0, 0.0);
  occ::handle<Geom2d_BezierCurve> aCurve = new Geom2d_BezierCurve(aPoles);
  Geom2dAdaptor_Curve             anAdaptor(aCurve);
  ExtremaPC2d_GridEvaluator       anEvaluator;
  anEvaluator.SetParams(ExtremaPC2d_GridEvaluator::BuildUniformParams(0.0, 1.0, 24));
  const ExtremaPC2d::Result& aResult =
    anEvaluator.Perform(anAdaptor, gp_Pnt2d(), {0.0, 1.0}, 1.0e-9, ExtremaPC2d::SearchMode::Min);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 1);
  EXPECT_NEAR(aResult[0].Parameter, 0.5, 1.0e-8);
  EXPECT_NEAR(aResult[0].SquareDistance, 1.0, 1.0e-8);
  EXPECT_TRUE(aResult[0].IsMinimum);
}

TEST(ExtremaPC2d_GridEvaluatorTest, ShortOpenCurveIsNotClosed)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 2);
  aPoles(1)                              = gp_Pnt2d(0.0, 0.0);
  aPoles(2)                              = gp_Pnt2d(0.5 * Precision::Confusion(), 0.0);
  occ::handle<Geom2d_BezierCurve> aCurve = new Geom2d_BezierCurve(aPoles);
  Geom2dAdaptor_Curve             anAdaptor(aCurve);
  EXPECT_FALSE(ExtremaPC2d_GridEvaluator::IsClosedDomain(anAdaptor, {0.0, 1.0}));
}
