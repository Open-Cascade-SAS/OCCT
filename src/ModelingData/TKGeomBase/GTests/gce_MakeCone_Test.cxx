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

#include <gce_MakeCone.hxx>
#include <gp_Ax2.hxx>
#include <gp_Cone.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

#include <cmath>

TEST(gce_MakeConeTest, FromConeAndDist_Valid_Done)
{
  const gp_Cone aBaseCone(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), M_PI / 6.0, 10.0);
  const double  aDist = 2.0;

  gce_MakeCone aMaker(aBaseCone, aDist);
  EXPECT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);

  const gp_Cone& aCone            = aMaker.Value();
  const double   anExpectedRadius = aBaseCone.RefRadius() + aDist / std::cos(aBaseCone.SemiAngle());
  EXPECT_NEAR(aCone.RefRadius(), anExpectedRadius, Precision::Confusion());
  EXPECT_NEAR(aCone.SemiAngle(), aBaseCone.SemiAngle(), Precision::Confusion());
  EXPECT_TRUE(aCone.Location().IsEqual(aBaseCone.Location(), Precision::Confusion()));
  EXPECT_TRUE(
    aCone.Axis().Direction().IsParallel(aBaseCone.Axis().Direction(), Precision::Angular()));
}

TEST(gce_MakeConeTest, FromConeAndDist_TooNegative_NegativeRadius)
{
  const gp_Cone aBaseCone(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), M_PI / 6.0, 10.0);
  const double  aDist = -(aBaseCone.RefRadius() * std::cos(aBaseCone.SemiAngle()) + 1.0);

  gce_MakeCone aMaker(aBaseCone, aDist);
  EXPECT_FALSE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_NegativeRadius);
}

TEST(gce_MakeConeTest, FromConeAndPoint_Valid_Done)
{
  const gp_Cone aBaseCone(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), M_PI / 6.0, 10.0);
  const double  aVParam       = 4.0;
  const double  aTargetRadius = 12.0;
  const double  aPointRadius  = aTargetRadius + aVParam * std::tan(aBaseCone.SemiAngle());
  const gp_Pnt  aPoint(aPointRadius, 0.0, aVParam);

  gce_MakeCone aMaker(aBaseCone, aPoint);
  EXPECT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);

  const gp_Cone& aCone = aMaker.Value();
  EXPECT_NEAR(aCone.RefRadius(), aTargetRadius, Precision::Confusion());
}

TEST(gce_MakeConeTest, FromConeAndPoint_NoValidParallelCone_NegativeRadius)
{
  const gp_Cone aBaseCone(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), M_PI / 6.0, 10.0);
  const gp_Pnt  aPoint(1.0, 0.0, 20.0);

  gce_MakeCone aMaker(aBaseCone, aPoint);
  EXPECT_FALSE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_NegativeRadius);
}

TEST(gce_MakeConeTest, FromConeAndPoint_TwoCandidates_ChoosesNearestToBaseCone)
{
  const gp_Cone aBaseCone(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), M_PI / 6.0, 10.0);
  const gp_Pnt  aPoint(5.0, 0.0, -20.0);

  gce_MakeCone aMaker(aBaseCone, aPoint);
  ASSERT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);

  const double aRadius = gp_Lin(aBaseCone.Axis()).Distance(aPoint);
  const double aVParam =
    (aPoint.XYZ() - aBaseCone.Location().XYZ()).Dot(aBaseCone.Axis().Direction().XYZ());
  const double aTan   = std::tan(aBaseCone.SemiAngle());
  const double aCos   = std::cos(aBaseCone.SemiAngle());
  const double aCand1 = aRadius - aVParam * aTan;
  const double aCand2 = -aRadius - aVParam * aTan;
  const double aDist1 = std::abs((aCand1 - aBaseCone.RefRadius()) * aCos);
  const double aDist2 = std::abs((aCand2 - aBaseCone.RefRadius()) * aCos);
  const double anExpR = (aDist1 <= aDist2) ? aCand1 : aCand2;

  EXPECT_NEAR(aMaker.Value().RefRadius(), anExpR, Precision::Confusion());
}

TEST(gce_MakeConeTest, FromTwoPointsAndTwoRadii_YBranch_DoneAndOrthogonalXAxis)
{
  const gp_Pnt aP1(0.0, 0.0, 0.0);
  const gp_Pnt aP2(0.0, 10.0, 10.0);

  gce_MakeCone aMaker(aP1, aP2, 3.0, 2.0);
  ASSERT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);

  const gp_Cone& aCone = aMaker.Value();
  const gp_Dir   aAxis = gp_Dir(aP2.XYZ() - aP1.XYZ());
  EXPECT_TRUE(aCone.Axis().Direction().IsParallel(aAxis, Precision::Angular()));
  const double aDot = aCone.Axis().Direction().XYZ().Dot(aCone.Position().XDirection().XYZ());
  EXPECT_NEAR(aDot, 0.0, Precision::Angular());
}

TEST(gce_MakeConeTest, FromTwoPointsAndTwoRadii_ZBranch_DoneAndOrthogonalXAxis)
{
  const gp_Pnt aP1(0.0, 0.0, 0.0);
  const gp_Pnt aP2(0.0, 0.0, 10.0);

  gce_MakeCone aMaker(aP1, aP2, 4.0, 2.0);
  ASSERT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);

  const gp_Cone& aCone = aMaker.Value();
  EXPECT_TRUE(
    aCone.Axis().Direction().IsParallel(gp_Dir(aP2.XYZ() - aP1.XYZ()), Precision::Angular()));
  const double aDot = aCone.Axis().Direction().XYZ().Dot(aCone.Position().XDirection().XYZ());
  EXPECT_NEAR(aDot, 0.0, Precision::Angular());
}

TEST(gce_MakeConeTest, FromFourPoints_YBranch_DoneAndOrthogonalXAxis)
{
  const gp_Pnt aP1(0.0, 0.0, 0.0);
  const gp_Pnt aP2(0.0, 10.0, 10.0);
  const gp_Pnt aP3(3.0, 0.0, 0.0);
  const gp_Pnt aP4(5.0, 4.0, 4.0);

  gce_MakeCone aMaker(aP1, aP2, aP3, aP4);
  ASSERT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);

  const gp_Cone& aCone = aMaker.Value();
  const gp_Dir   aAxis = gp_Dir(aP2.XYZ() - aP1.XYZ());
  EXPECT_TRUE(aCone.Axis().Direction().IsParallel(aAxis, Precision::Angular()));
  const double aDot = aCone.Axis().Direction().XYZ().Dot(aCone.Position().XDirection().XYZ());
  EXPECT_NEAR(aDot, 0.0, Precision::Angular());
}

TEST(gce_MakeConeTest, FromFourPoints_ZBranch_DoneAndOrthogonalXAxis)
{
  const gp_Pnt aP1(0.0, 0.0, 0.0);
  const gp_Pnt aP2(0.0, 0.0, 10.0);
  const gp_Pnt aP3(4.0, 0.0, 0.0);
  const gp_Pnt aP4(2.0, 0.0, 5.0);

  gce_MakeCone aMaker(aP1, aP2, aP3, aP4);
  ASSERT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);

  const gp_Cone& aCone = aMaker.Value();
  EXPECT_TRUE(
    aCone.Axis().Direction().IsParallel(gp_Dir(aP2.XYZ() - aP1.XYZ()), Precision::Angular()));
  const double aDot = aCone.Axis().Direction().XYZ().Dot(aCone.Position().XDirection().XYZ());
  EXPECT_NEAR(aDot, 0.0, Precision::Angular());
}
