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

#include <Law_Linear.hxx>

#include <gtest/gtest.h>

#include <limits>
#include <cmath>

TEST(Law_LinearTest, FiniteConstantValue)
{
  Law_Linear aLaw;
  aLaw.Set(0.0, 1.0e308, 2.0, 1.0e308);
  EXPECT_EQ(aLaw.Value(1.0), 1.0e308);
  double aValue, aD1, aD2;
  aLaw.D2(1.0, aValue, aD1, aD2);
  EXPECT_EQ(aValue, 1.0e308);
  EXPECT_EQ(aD1, 0.0);
  EXPECT_EQ(aD2, 0.0);
  EXPECT_EQ(aLaw.Trim(.5, 1.5, 0.0)->Value(1.0), 1.0e308);
}

//==================================================================================================

TEST(Law_LinearTest, WideDomainAndValueRange)
{
  const double aMax = std::numeric_limits<double>::max();
  Law_Linear   aLaw;
  aLaw.Set(-aMax, -aMax, aMax, aMax);
  double aValue, aD1;
  aLaw.D1(0.0, aValue, aD1);
  EXPECT_EQ(aValue, 0.0);
  EXPECT_EQ(aD1, 1.0);
  aLaw.Set(1.0, 2.0, 3.0, 6.0);
  EXPECT_EQ(aLaw.Value(0.0), 0.0);
  EXPECT_EQ(aLaw.Value(4.0), 8.0);
  aLaw.Set(0.0, 0.0, 1.0e308, 1.0e308);
  EXPECT_DOUBLE_EQ(aLaw.Value(1.0), 1.0);
  const double aSmall = std::numeric_limits<double>::denorm_min();
  EXPECT_EQ(aLaw.Value(aSmall), aSmall);
  aLaw.Set(-aMax, -0.6 * aMax, -0.5 * aMax, -0.3 * aMax);
  EXPECT_NEAR(aLaw.Value(aMax) / aMax, 0.6, 1.0e-15);
}

//==================================================================================================

TEST(Law_LinearTest, NearEndpointPreservesSmallInterpolatedValue)
{
  const double aParameter = std::nextafter(3.0, 0.0);
  const double anExpected = (3.0 - aParameter) / 3.0;
  for (const bool isReversed : {false, true})
  {
    Law_Linear aLaw;
    if (isReversed)
    {
      aLaw.Set(3.0, 0.0, 0.0, 1.0);
    }
    else
    {
      aLaw.Set(0.0, 1.0, 3.0, 0.0);
    }
    EXPECT_DOUBLE_EQ(aLaw.Value(aParameter), anExpected);
    double aValue, aD1, aD2;
    aLaw.D2(aParameter, aValue, aD1, aD2);
    EXPECT_DOUBLE_EQ(aValue, anExpected);
    EXPECT_DOUBLE_EQ(aD1, -1.0 / 3.0);
    EXPECT_DOUBLE_EQ(aD2, 0.0);
    const occ::handle<Law_Function> aTrim = aLaw.Trim(aParameter, 3.0, 0.0);
    EXPECT_DOUBLE_EQ(aTrim->Value(aParameter), anExpected);
  }
}
