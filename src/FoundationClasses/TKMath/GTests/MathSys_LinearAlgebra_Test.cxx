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

#include <MathSys_LinearAlgebra.hxx>

#include <gtest/gtest.h>

TEST(MathSys_LinearAlgebraTest, FailedUnscalingHasNoDirection)
{
  const math_Matrix                aMatrix(size_t(1), size_t(1), 1.0e-308);
  const math_Vector                aRight(size_t(1), 10.0);
  const MathSys::Utils::LinearStep aResult = MathSys::Utils::SolveLinearized(aMatrix, aRight);
  EXPECT_EQ(aResult.Status, MathUtils::Status::NumericalError);
  EXPECT_FALSE(aResult.Solution.has_value());
}

//==================================================================================================

TEST(MathSys_LinearAlgebraTest, FiniteUnscalingKeepsDirection)
{
  const math_Matrix                aMatrix(size_t(1), size_t(1), 1.0e-200);
  const math_Vector                aRight(size_t(1), 2.0);
  const MathSys::Utils::LinearStep aResult = MathSys::Utils::SolveLinearized(aMatrix, aRight);
  ASSERT_EQ(aResult.Status, MathUtils::Status::OK);
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR(aResult.Solution->At(0) / 2.0e200, 1.0, 1.0e-14);
}
