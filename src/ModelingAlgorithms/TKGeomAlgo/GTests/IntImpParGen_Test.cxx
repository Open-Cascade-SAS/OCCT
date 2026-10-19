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
#include <Precision.hxx>
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

TEST(IntImpParGenTest, StationaryEndpointTransitionIsIndependentOfScale)
{
  for (const double aScale : {1.e-150, 1.0, 1.e150})
  {
    SCOPED_TRACE(aScale);
    for (const IntRes2d_Position aPosition : {IntRes2d_Head, IntRes2d_End})
    {
      SCOPED_TRACE(aPosition);
      for (const bool isFirst : {false, true})
      {
        SCOPED_TRACE(isFirst);
        gp_Vec2d            aTangent(0.0, -1.e-15 * aScale);
        gp_Vec2d            aLineTangent(0.0, aScale);
        const gp_Vec2d      aSecondDerivative(6.0 * aScale, 0.0);
        const gp_Vec2d      aLineSecondDerivative(0.0, 0.0);
        IntRes2d_Transition aCurveTransition, aLineTransition;
        if (isFirst)
        {
          IntImpParGen::DetermineTransition(aPosition,
                                            aTangent,
                                            aSecondDerivative,
                                            aCurveTransition,
                                            IntRes2d_Middle,
                                            aLineTangent,
                                            aLineSecondDerivative,
                                            aLineTransition,
                                            1.e-7);
          EXPECT_EQ(aCurveTransition.TransitionType(), IntRes2d_Out);
          EXPECT_EQ(aLineTransition.TransitionType(), IntRes2d_In);
        }
        else
        {
          IntImpParGen::DetermineTransition(IntRes2d_Middle,
                                            aLineTangent,
                                            aLineSecondDerivative,
                                            aLineTransition,
                                            aPosition,
                                            aTangent,
                                            aSecondDerivative,
                                            aCurveTransition,
                                            1.e-7);
          EXPECT_EQ(aCurveTransition.TransitionType(), IntRes2d_Out);
          EXPECT_EQ(aLineTransition.TransitionType(), IntRes2d_In);
        }
      }
    }
  }
}

TEST(IntImpParGenTest, RegularEndpointRetainsFirstDerivative)
{
  for (const double aScale : {1.e-150, 1.0, 1.e150})
  {
    SCOPED_TRACE(aScale);
    for (const double aCurvature : {0.0, 0.5 / Precision::PConfusion()})
    {
      SCOPED_TRACE(aCurvature);
      gp_Vec2d            aTangent1(aScale, 0.0), aTangent2(0.0, aScale);
      const gp_Vec2d      aSecondDerivative(-aCurvature * aScale, 0.0);
      IntRes2d_Transition aTransition1, aTransition2;
      IntImpParGen::DetermineTransition(IntRes2d_Head,
                                        aTangent1,
                                        aSecondDerivative,
                                        aTransition1,
                                        IntRes2d_End,
                                        aTangent2,
                                        gp_Vec2d(),
                                        aTransition2,
                                        1.e-7);
      EXPECT_EQ(aTransition1.TransitionType(), IntRes2d_Out);
      EXPECT_EQ(aTransition2.TransitionType(), IntRes2d_In);
      EXPECT_EQ(aTangent1.X(), aScale);
      EXPECT_EQ(aTangent2.Y(), aScale);
    }
  }
}
