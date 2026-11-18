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

#include <Precision.hxx>

#include <gtest/gtest.h>

#include <limits>

TEST(PrecisionTest, IsFiniteUsesOcctInfiniteRange)
{
  EXPECT_TRUE(Precision::IsFinite(0.0));
  EXPECT_TRUE(Precision::IsFinite(0.49 * Precision::Infinite()));
  EXPECT_FALSE(Precision::IsFinite(0.5 * Precision::Infinite()));
  EXPECT_FALSE(Precision::IsFinite(-0.5 * Precision::Infinite()));
  EXPECT_FALSE(Precision::IsFinite(Precision::Infinite()));
  EXPECT_FALSE(Precision::IsFinite(-Precision::Infinite()));
}

TEST(PrecisionTest, IsFiniteRejectsIeeeSpecialValues)
{
  EXPECT_FALSE(Precision::IsFinite(std::numeric_limits<double>::infinity()));
  EXPECT_FALSE(Precision::IsFinite(-std::numeric_limits<double>::infinity()));
  EXPECT_FALSE(Precision::IsFinite(std::numeric_limits<double>::quiet_NaN()));
}
