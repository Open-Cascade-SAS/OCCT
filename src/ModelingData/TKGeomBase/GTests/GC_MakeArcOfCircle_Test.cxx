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

#include <GC_MakeArcOfCircle.hxx>
#include <Precision.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

#include <gtest/gtest.h>

TEST(GC_MakeArcOfCircleTest, FromPointsAndTangent_Done)
{
  const gp_Pnt aP1(1.0, 0.0, 0.0);
  const gp_Pnt aP2(0.0, 1.0, 0.0);
  const gp_Vec aTangentAtP1(0.0, 1.0, 0.0);

  GC_MakeArcOfCircle aMaker(aP1, aTangentAtP1, aP2);
  ASSERT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);
  EXPECT_FALSE(aMaker.Value().IsNull());
}

TEST(GC_MakeArcOfCircleTest, FromThreePoints_Done)
{
  GC_MakeArcOfCircle aMaker(gp_Pnt(1.0, 0.0, 0.0), gp_Pnt(0.0, 1.0, 0.0), gp_Pnt(-1.0, 0.0, 0.0));
  ASSERT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);
  EXPECT_FALSE(aMaker.Value().IsNull());
}

TEST(GC_MakeArcOfCircleTest, FromPointsAndTangent_ConfusionScale_Done)
{
  const double aScale = 2.0 * Precision::Confusion();
  const gp_Pnt aP1(aScale, 0.0, 0.0);
  const gp_Pnt aP2(0.0, aScale, 0.0);
  const gp_Vec aTangentAtP1(0.0, 1.0, 0.0);

  GC_MakeArcOfCircle aMaker(aP1, aTangentAtP1, aP2);
  ASSERT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);
  ASSERT_FALSE(aMaker.Value().IsNull());
  EXPECT_TRUE(aMaker.Value()->StartPoint().IsEqual(aP1, Precision::Confusion()));
}
