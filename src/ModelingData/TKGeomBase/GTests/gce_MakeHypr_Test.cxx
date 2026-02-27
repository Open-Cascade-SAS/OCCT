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

#include <gce_MakeHypr.hxx>
#include <gce_MakeHypr2d.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Hypr.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

// ==================== gce_MakeHypr (3D) ====================

TEST(gce_MakeHyprTest, FromAxis_ValidRadii_Done)
{
  gce_MakeHypr aMaker(gp_Ax2(), 10.0, 5.0);
  EXPECT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);

  const gp_Hypr& aHypr = aMaker.Value();
  EXPECT_NEAR(aHypr.MajorRadius(), 10.0, Precision::Confusion());
  EXPECT_NEAR(aHypr.MinorRadius(), 5.0, Precision::Confusion());
}

TEST(gce_MakeHyprTest, FromAxis_MajorLessThanMinor_Done)
{
  gce_MakeHypr aMaker(gp_Ax2(), 3.0, 7.0);
  EXPECT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);

  const gp_Hypr& aHypr = aMaker.Value();
  EXPECT_NEAR(aHypr.MajorRadius(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aHypr.MinorRadius(), 7.0, Precision::Confusion());
}

TEST(gce_MakeHyprTest, FromAxis_EqualRadii_Done)
{
  gce_MakeHypr aMaker(gp_Ax2(), 5.0, 5.0);
  EXPECT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);
}

TEST(gce_MakeHyprTest, FromAxis_ZeroRadii_Done)
{
  gce_MakeHypr aMaker(gp_Ax2(), 0.0, 0.0);
  EXPECT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);
}

TEST(gce_MakeHyprTest, FromAxis_NegativeMajor_NegativeRadius)
{
  gce_MakeHypr aMaker(gp_Ax2(), -1.0, 5.0);
  EXPECT_FALSE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_NegativeRadius);
}

TEST(gce_MakeHyprTest, FromAxis_NegativeMinor_NegativeRadius)
{
  gce_MakeHypr aMaker(gp_Ax2(), 5.0, -1.0);
  EXPECT_FALSE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_NegativeRadius);
}

TEST(gce_MakeHyprTest, FromAxis_BothNegative_NegativeRadius)
{
  gce_MakeHypr aMaker(gp_Ax2(), -3.0, -2.0);
  EXPECT_FALSE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_NegativeRadius);
}

TEST(gce_MakeHyprTest, FromPoints_Valid_Done)
{
  const gp_Pnt aCenter(0.0, 0.0, 0.0);
  const gp_Pnt aS1(10.0, 0.0, 0.0);
  const gp_Pnt aS2(0.0, 5.0, 0.0);

  gce_MakeHypr aMaker(aS1, aS2, aCenter);
  EXPECT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);

  const gp_Hypr& aHypr = aMaker.Value();
  EXPECT_NEAR(aHypr.MajorRadius(), 10.0, Precision::Confusion());
  EXPECT_NEAR(aHypr.MinorRadius(), 5.0, Precision::Confusion());
}

TEST(gce_MakeHyprTest, FromPoints_MajorLessThanMinor_Done)
{
  const gp_Pnt aCenter(0.0, 0.0, 0.0);
  const gp_Pnt aS1(3.0, 0.0, 0.0);
  const gp_Pnt aS2(0.0, 7.0, 0.0);

  gce_MakeHypr aMaker(aS1, aS2, aCenter);
  EXPECT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);

  const gp_Hypr& aHypr = aMaker.Value();
  EXPECT_NEAR(aHypr.MajorRadius(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aHypr.MinorRadius(), 7.0, Precision::Confusion());
}

TEST(gce_MakeHyprTest, FromPoints_ConfusedS1Center_ConfusedPoints)
{
  const gp_Pnt aCenter(1.0, 2.0, 3.0);
  const gp_Pnt aS1(1.0, 2.0, 3.0);
  const gp_Pnt aS2(4.0, 5.0, 6.0);

  gce_MakeHypr aMaker(aS1, aS2, aCenter);
  EXPECT_FALSE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_ConfusedPoints);
}

TEST(gce_MakeHyprTest, FromPoints_ConfusedS2Center_ConfusedPoints)
{
  const gp_Pnt aCenter(1.0, 2.0, 3.0);
  const gp_Pnt aS1(4.0, 5.0, 6.0);
  const gp_Pnt aS2(1.0, 2.0, 3.0);

  gce_MakeHypr aMaker(aS1, aS2, aCenter);
  EXPECT_FALSE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_ConfusedPoints);
}

TEST(gce_MakeHyprTest, FromPoints_ConfusedS1S2_ConfusedPoints)
{
  const gp_Pnt aCenter(0.0, 0.0, 0.0);
  const gp_Pnt aS1(5.0, 5.0, 5.0);
  const gp_Pnt aS2(5.0, 5.0, 5.0);

  gce_MakeHypr aMaker(aS1, aS2, aCenter);
  EXPECT_FALSE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_ConfusedPoints);
}

TEST(gce_MakeHyprTest, FromPoints_Collinear_ColinearPoints)
{
  const gp_Pnt aCenter(0.0, 0.0, 0.0);
  const gp_Pnt aS1(10.0, 0.0, 0.0);
  const gp_Pnt aS2(5.0, 0.0, 0.0);

  gce_MakeHypr aMaker(aS1, aS2, aCenter);
  EXPECT_FALSE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_ColinearPoints);
}

// ==================== gce_MakeHypr2d (2D) ====================

TEST(gce_MakeHypr2dTest, FromMajorAxis_ValidRadii_Done)
{
  gce_MakeHypr2d aMaker(gp_Ax2d(), 10.0, 5.0, true);
  EXPECT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);

  const gp_Hypr2d& aHypr = aMaker.Value();
  EXPECT_NEAR(aHypr.MajorRadius(), 10.0, Precision::Confusion());
  EXPECT_NEAR(aHypr.MinorRadius(), 5.0, Precision::Confusion());
}

TEST(gce_MakeHypr2dTest, FromMajorAxis_MajorLessThanMinor_Done)
{
  gce_MakeHypr2d aMaker(gp_Ax2d(), 3.0, 7.0, true);
  EXPECT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);

  const gp_Hypr2d& aHypr = aMaker.Value();
  EXPECT_NEAR(aHypr.MajorRadius(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aHypr.MinorRadius(), 7.0, Precision::Confusion());
}

TEST(gce_MakeHypr2dTest, FromMajorAxis_NegativeRadius_NegativeRadius)
{
  gce_MakeHypr2d aMaker(gp_Ax2d(), -1.0, 5.0, true);
  EXPECT_FALSE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_NegativeRadius);
}

TEST(gce_MakeHypr2dTest, FromAx22d_ValidRadii_Done)
{
  gce_MakeHypr2d aMaker(gp_Ax22d(), 8.0, 4.0);
  EXPECT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);
}

TEST(gce_MakeHypr2dTest, FromAx22d_NegativeRadius_NegativeRadius)
{
  gce_MakeHypr2d aMaker(gp_Ax22d(), 5.0, -2.0);
  EXPECT_FALSE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_NegativeRadius);
}

TEST(gce_MakeHypr2dTest, FromPoints_Valid_Done)
{
  const gp_Pnt2d aCenter(0.0, 0.0);
  const gp_Pnt2d aS1(10.0, 0.0);
  const gp_Pnt2d aS2(0.0, 5.0);

  gce_MakeHypr2d aMaker(aS1, aS2, aCenter);
  EXPECT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);

  const gp_Hypr2d& aHypr = aMaker.Value();
  EXPECT_NEAR(aHypr.MajorRadius(), 10.0, Precision::Confusion());
  EXPECT_NEAR(aHypr.MinorRadius(), 5.0, Precision::Confusion());
}

TEST(gce_MakeHypr2dTest, FromPoints_MajorLessThanMinor_Done)
{
  const gp_Pnt2d aCenter(0.0, 0.0);
  const gp_Pnt2d aS1(3.0, 0.0);
  const gp_Pnt2d aS2(0.0, 7.0);

  gce_MakeHypr2d aMaker(aS1, aS2, aCenter);
  EXPECT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);

  const gp_Hypr2d& aHypr = aMaker.Value();
  EXPECT_NEAR(aHypr.MajorRadius(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aHypr.MinorRadius(), 7.0, Precision::Confusion());
}

TEST(gce_MakeHypr2dTest, FromPoints_ConfusedS1Center_ConfusedPoints)
{
  const gp_Pnt2d aCenter(1.0, 2.0);
  const gp_Pnt2d aS1(1.0, 2.0);
  const gp_Pnt2d aS2(4.0, 5.0);

  gce_MakeHypr2d aMaker(aS1, aS2, aCenter);
  EXPECT_FALSE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_ConfusedPoints);
}

TEST(gce_MakeHypr2dTest, FromPoints_ConfusedS2Center_ConfusedPoints)
{
  const gp_Pnt2d aCenter(1.0, 2.0);
  const gp_Pnt2d aS1(4.0, 5.0);
  const gp_Pnt2d aS2(1.0, 2.0);

  gce_MakeHypr2d aMaker(aS1, aS2, aCenter);
  EXPECT_FALSE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_ConfusedPoints);
}

TEST(gce_MakeHypr2dTest, FromPoints_Collinear_ColinearPoints)
{
  const gp_Pnt2d aCenter(0.0, 0.0);
  const gp_Pnt2d aS1(10.0, 0.0);
  const gp_Pnt2d aS2(5.0, 0.0);

  gce_MakeHypr2d aMaker(aS1, aS2, aCenter);
  EXPECT_FALSE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_ColinearPoints);
}
