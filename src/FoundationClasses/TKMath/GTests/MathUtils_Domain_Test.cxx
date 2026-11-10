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

#include <MathUtils_Domain.hxx>

#include <gtest/gtest.h>

#include <limits>

TEST(MathUtils_DomainTest, FiniteMidpoints)
{
  const double aMax   = std::numeric_limits<double>::max();
  const double aSmall = std::numeric_limits<double>::denorm_min();
  EXPECT_EQ(MathUtils::Domain1D(aMax, aMax).Mid(), aMax);
  EXPECT_EQ(MathUtils::Domain1D(-aMax, -aMax).Mid(), -aMax);
  EXPECT_EQ(MathUtils::Domain1D(-aMax, aMax).Mid(), 0.0);
  EXPECT_EQ(MathUtils::Domain1D(aSmall, aSmall).Mid(), aSmall);
  const MathUtils::Domain2D aDomain(aMax, aMax, -aMax, -aMax);
  EXPECT_EQ(aDomain.UMid(), aMax);
  EXPECT_EQ(aDomain.VMid(), -aMax);
}
