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

#include <IntImpParGen.hxx>
#include <IntRes2d_Transition.hxx>
#include <gp_Vec2d.hxx>
#include <gtest/gtest.h>

TEST(IntImpParGenTest, TransitionIsIndependentOfParameterSpeed)
{
  for (double aScale : {1.e-150, 1.e-14, 1.0, 1.e150})
  {
    SCOPED_TRACE(aScale);
    gp_Vec2d            aTangent1(aScale, 3.0 * aScale);
    gp_Vec2d            aTangent2(-aScale, 2.0 * aScale);
    IntRes2d_Transition aTransition1, aTransition2;
    ASSERT_TRUE(IntImpParGen::DetermineTransition(IntRes2d_Middle,
                                                  aTangent1,
                                                  aTransition1,
                                                  IntRes2d_Middle,
                                                  aTangent2,
                                                  aTransition2,
                                                  1.e-7));
    EXPECT_EQ(aTransition1.TransitionType(), IntRes2d_Out);
    EXPECT_EQ(aTransition2.TransitionType(), IntRes2d_In);
  }
}

TEST(IntImpParGenTest, NonzeroFirstDerivativeKeepsItsOrientation)
{
  // On approach to a stationary point, the second derivative can point
  // opposite to the first derivative. A slow parameterization still has
  // a defined traversal direction.
  gp_Vec2d            aTangent1(1.e-14, 3.e-14), aTangent2(-1.e-15, 2.e-15);
  const gp_Vec2d      aDerivative2OnFirst(-133.0, -400.0), aDerivative2OnSecond(-1.0, 2.0);
  IntRes2d_Transition aTransition1, aTransition2;
  IntImpParGen::DetermineTransition(IntRes2d_Middle,
                                    aTangent1,
                                    aDerivative2OnFirst,
                                    aTransition1,
                                    IntRes2d_Middle,
                                    aTangent2,
                                    aDerivative2OnSecond,
                                    aTransition2,
                                    1.e-7);
  EXPECT_EQ(aTransition1.TransitionType(), IntRes2d_Out);
  EXPECT_EQ(aTransition2.TransitionType(), IntRes2d_In);
  EXPECT_EQ(aTangent1.X(), 1.e-14);
  EXPECT_EQ(aTangent2.X(), -1.e-15);
}

TEST(IntImpParGenTest, ZeroDerivativesRemainUndecided)
{
  gp_Vec2d            aTangent1(0.0, 0.0), aTangent2(1.0, 0.0);
  const gp_Vec2d      aDerivative2(0.0, 0.0);
  IntRes2d_Transition aTransition1, aTransition2;
  EXPECT_FALSE(IntImpParGen::DetermineTransition(IntRes2d_Middle,
                                                 aTangent1,
                                                 aTransition1,
                                                 IntRes2d_Middle,
                                                 aTangent2,
                                                 aTransition2,
                                                 1.e-7));
  IntImpParGen::DetermineTransition(IntRes2d_Middle,
                                    aTangent1,
                                    aDerivative2,
                                    aTransition1,
                                    IntRes2d_Middle,
                                    aTangent2,
                                    aDerivative2,
                                    aTransition2,
                                    1.e-7);
  EXPECT_EQ(aTransition1.TransitionType(), IntRes2d_Undecided);
  EXPECT_EQ(aTransition2.TransitionType(), IntRes2d_Undecided);
}
