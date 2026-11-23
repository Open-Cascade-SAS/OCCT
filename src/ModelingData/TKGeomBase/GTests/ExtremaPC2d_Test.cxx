// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt.

#include <ExtremaPC2d.hxx>

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <type_traits>

namespace
{
struct LinearEvaluator2d
{
  gp_Pnt2d Value(double theU) const { return gp_Pnt2d(theU, 0.0); }
};

struct RapidEndpointEvaluator2d
{
  gp_Pnt2d Value(double theU) const { return gp_Pnt2d(1.0 + theU - 200.0 * theU * theU, 0.0); }
};
} // namespace

//=================================================================================================

TEST(ExtremaPC2dTest, Result_DefaultClearAndTraits)
{
  static_assert(!std::is_copy_constructible_v<ExtremaPC2d::Result>);
  static_assert(!std::is_copy_assignable_v<ExtremaPC2d::Result>);
  static_assert(std::is_move_constructible_v<ExtremaPC2d::Result>);

  ExtremaPC2d::Result aResult;
  EXPECT_EQ(aResult.Status, ExtremaPC2d::Status::NotDone);
  EXPECT_EQ(aResult.NbExt(), 0);
  EXPECT_EQ(aResult.MinIndex(), ExtremaPC2d::Result::INVALID_INDEX);
  EXPECT_EQ(aResult.MaxIndex(), ExtremaPC2d::Result::INVALID_INDEX);
  EXPECT_TRUE(std::isinf(aResult.MinSquareDistance()));
  EXPECT_EQ(aResult.MaxSquareDistance(), 0.0);

  aResult.Status = ExtremaPC2d::Status::OK;
  aResult.Extrema.Append({0.0, gp_Pnt2d(), 9.0, true, false});
  aResult.Extrema.Append({1.0, gp_Pnt2d(), 4.0, true, false});
  aResult.Extrema.Append({2.0, gp_Pnt2d(), 9.0, false, true});
  EXPECT_EQ(aResult.MinIndex(), 1);
  EXPECT_EQ(aResult.MaxIndex(), 0);
  aResult.Clear();
  EXPECT_EQ(aResult.Status, ExtremaPC2d::Status::NotDone);
  EXPECT_EQ(aResult.NbExt(), 0);
}

//=================================================================================================

TEST(ExtremaPC2dTest, ToleranceAndFiniteParameterValidation)
{
  EXPECT_TRUE(ExtremaPC2d::IsValidTolerance(1.0e-9));
  EXPECT_FALSE(ExtremaPC2d::IsValidTolerance(0.0));
  EXPECT_FALSE(ExtremaPC2d::IsValidTolerance(-1.0));
  EXPECT_FALSE(ExtremaPC2d::IsValidTolerance(std::numeric_limits<double>::infinity()));
  EXPECT_FALSE(ExtremaPC2d::IsValidTolerance(std::numeric_limits<double>::quiet_NaN()));
  EXPECT_TRUE(ExtremaPC2d::IsFiniteParameter(1.0));
  EXPECT_FALSE(ExtremaPC2d::IsFiniteParameter(Precision::Infinite()));
  EXPECT_FALSE(ExtremaPC2d::IsFiniteParameter(std::numeric_limits<double>::infinity()));
}

//=================================================================================================

TEST(ExtremaPC2dTest, ClosedPeriodicDomainValidation)
{
  constexpr double aTol = 1.0e-10;
  EXPECT_TRUE(ExtremaPC2d::IsClosedPeriodicDomain({0.0, 2.0 * M_PI}, 2.0 * M_PI, aTol));
  EXPECT_TRUE(ExtremaPC2d::IsClosedPeriodicDomain({7.0, 7.0 + 4.0 * M_PI}, 2.0 * M_PI, aTol));
  EXPECT_FALSE(ExtremaPC2d::IsClosedPeriodicDomain({0.0, M_PI}, 2.0 * M_PI, aTol));
  EXPECT_FALSE(ExtremaPC2d::IsClosedPeriodicDomain({1.0, 0.0}, 2.0 * M_PI, aTol));
  EXPECT_FALSE(ExtremaPC2d::IsClosedPeriodicDomain({0.0, 2.0 * M_PI}, 0.0, aTol));
  EXPECT_FALSE(ExtremaPC2d::IsClosedPeriodicDomain({0.0, 2.0 * M_PI}, 2.0 * M_PI, -1.0));
}

//=================================================================================================

TEST(ExtremaPC2dTest, EndpointClassificationAndModes)
{
  const LinearEvaluator2d anEvaluator;
  const gp_Pnt2d          aQuery(-2.0, 3.0);
  ExtremaPC2d::Result     aResult;
  ExtremaPC2d::AddEndpointExtrema(aResult,
                                  aQuery,
                                  {0.0, 10.0},
                                  anEvaluator,
                                  ExtremaPC2d::SearchMode::MinMax,
                                  false);
  ASSERT_EQ(aResult.NbExt(), 2);
  EXPECT_NEAR(aResult[0].Parameter, 0.0, 1.0e-12);
  EXPECT_TRUE(aResult[0].IsMinimum);
  EXPECT_NEAR(aResult[1].Parameter, 10.0, 1.0e-12);
  EXPECT_TRUE(aResult[1].IsMaximum);

  aResult.Clear();
  ExtremaPC2d::AddEndpointExtrema(aResult,
                                  aQuery,
                                  {0.0, 10.0},
                                  anEvaluator,
                                  ExtremaPC2d::SearchMode::Min,
                                  false);
  ASSERT_EQ(aResult.NbExt(), 1);
  EXPECT_NEAR(aResult[0].Parameter, 0.0, 1.0e-12);
}

//=================================================================================================

TEST(ExtremaPC2dTest, SingletonAndClosedEndpointBehavior)
{
  const LinearEvaluator2d anEvaluator;
  ExtremaPC2d::Result     aResult;
  ExtremaPC2d::AddEndpointExtrema(aResult,
                                  gp_Pnt2d(5.0, 3.0),
                                  {2.0, 2.0},
                                  anEvaluator,
                                  ExtremaPC2d::SearchMode::MinMax,
                                  false);
  ASSERT_EQ(aResult.NbExt(), 1);
  EXPECT_TRUE(aResult[0].IsMinimum);
  EXPECT_TRUE(aResult[0].IsMaximum);

  aResult.Clear();
  ExtremaPC2d::AddEndpointExtrema(aResult,
                                  gp_Pnt2d(),
                                  {0.0, 1.0},
                                  anEvaluator,
                                  ExtremaPC2d::SearchMode::MinMax,
                                  true);
  EXPECT_EQ(aResult.NbExt(), 0);
}

TEST(ExtremaPC2dTest, EndpointClassificationUsesLocalBehavior)
{
  const RapidEndpointEvaluator2d anEvaluator;
  ExtremaPC2d::Result            aResult;
  ExtremaPC2d::AddEndpointExtrema(aResult,
                                  gp_Pnt2d(),
                                  {0.0, 1.0},
                                  anEvaluator,
                                  ExtremaPC2d::SearchMode::Min,
                                  false);
  ASSERT_EQ(aResult.NbExt(), 1);
  EXPECT_NEAR(aResult[0].Parameter, 0.0, Precision::PConfusion());
  EXPECT_TRUE(aResult[0].IsMinimum);
}
