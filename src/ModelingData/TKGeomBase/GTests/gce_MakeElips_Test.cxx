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
#include <gce_MakeElips.hxx>
#include <gp.hxx>
#include <gp_Elips.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

TEST(gce_MakeElipsTest, FromPoints_Valid_Done)
{
  const gp_Pnt aCenter(0.0, 0.0, 0.0);
  const gp_Pnt aS1(10.0, 0.0, 0.0);
  const gp_Pnt aS2(0.0, 5.0, 0.0);

  gce_MakeElips aMaker(aS1, aS2, aCenter);
  ASSERT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);

  const gp_Elips& anElips = aMaker.Value();
  EXPECT_NEAR(anElips.MajorRadius(), 10.0, Precision::Confusion());
  EXPECT_NEAR(anElips.MinorRadius(), 5.0, Precision::Confusion());
}

TEST(gce_MakeElipsTest, FromPoints_Collinear_InvertAxis)
{
  const gp_Pnt aCenter(0.0, 0.0, 0.0);
  const gp_Pnt aS1(10.0, 0.0, 0.0);
  const gp_Pnt aS2(5.0, 0.0, 0.0);

  gce_MakeElips aMaker(aS1, aS2, aCenter);
  EXPECT_FALSE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_InvertAxis);
}

TEST(gce_MakeElipsTest, FromPoints_TinyCrossProduct_InvertAxis)
{
  const double aRes = gp::Resolution();
  const gp_Pnt aCenter(0.0, 0.0, 0.0);
  const gp_Pnt aS1(2.0 * aRes, 0.0, 0.0);
  const gp_Pnt aS2(0.0, 1.1 * aRes, 0.0);

  gce_MakeElips aMaker(aS1, aS2, aCenter);
  EXPECT_FALSE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_InvertAxis);
}
