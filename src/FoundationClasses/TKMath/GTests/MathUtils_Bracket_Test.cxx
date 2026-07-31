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

#include <MathUtils_Bracket.hxx>

#include <cmath>

namespace
{
class QuadraticMinimum
{
public:
  bool Value(double theX, double& theF) const
  {
    theF = (theX - 2.0) * (theX - 2.0);
    return true;
  }
};

class QuadraticWithPrecomputedEndpoints
{
public:
  bool Value(double theX, double& theF) const
  {
    if (std::abs(theX - 0.0) < 1.0e-15 || std::abs(theX - 1.0) < 1.0e-15)
    {
      return false;
    }
    theF = (theX - 2.0) * (theX - 2.0);
    return true;
  }
};
} // namespace

TEST(MathUtils_BracketTest, BracketMinimum_WithLimits_Succeeds)
{
  QuadraticMinimum aFunc;

  MathUtils::MinBracketOptions anOptions;
  anOptions.MaxIterations = 50;
  anOptions.UseLimits     = true;
  anOptions.LeftLimit     = 0.0;
  anOptions.RightLimit    = 5.0;

  const MathUtils::MinBracketResult aResult = MathUtils::BracketMinimum(aFunc, 0.0, 1.0, anOptions);
  ASSERT_TRUE(aResult.IsValid);
  EXPECT_GE(aResult.A, anOptions.LeftLimit);
  EXPECT_LE(aResult.C, anOptions.RightLimit);
  EXPECT_LT(aResult.Fb, aResult.Fa);
  EXPECT_LT(aResult.Fb, aResult.Fc);
}

TEST(MathUtils_BracketTest, BracketMinimum_WithRestrictiveLimits_Fails)
{
  QuadraticMinimum aFunc;

  MathUtils::MinBracketOptions anOptions;
  anOptions.MaxIterations = 50;
  anOptions.UseLimits     = true;
  anOptions.LeftLimit     = 0.0;
  anOptions.RightLimit    = 1.0;

  const MathUtils::MinBracketResult aResult = MathUtils::BracketMinimum(aFunc, 0.0, 0.5, anOptions);
  EXPECT_FALSE(aResult.IsValid);
}

TEST(MathUtils_BracketTest, BracketMinimum_UsesPrecomputedEndpointValues)
{
  QuadraticWithPrecomputedEndpoints aFunc;

  MathUtils::MinBracketOptions anOptions;
  anOptions.MaxIterations = 50;
  anOptions.HasFA         = true;
  anOptions.HasFB         = true;
  anOptions.FA            = 4.0;
  anOptions.FB            = 1.0;

  const MathUtils::MinBracketResult aResult = MathUtils::BracketMinimum(aFunc, 0.0, 1.0, anOptions);
  ASSERT_TRUE(aResult.IsValid);
  EXPECT_LT(aResult.Fb, aResult.Fa);
  EXPECT_LT(aResult.Fb, aResult.Fc);
}
