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

#include <ExtremaSS_CylinderCone.hxx>
#include <gp_Ax3.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pnt.hxx>

#include <cmath>

//! Test fixture for Cylinder-Cone extrema tests.
class ExtremaSS_CylinderConeTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1e-7;
};

//==================================================================================================
// Basic Tests
//==================================================================================================

TEST_F(ExtremaSS_CylinderConeTest, SeparatedCylinderAndCone_FindsMinimum)
{
  // Cylinder along Z axis at origin
  const gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);

  // Cone with apex at (15, 0, 0) pointing away
  const gp_Ax3  aConeAxis(gp_Pnt(15, 0, 0), gp_Dir(1, 0, 0));
  const gp_Cone aCone(aConeAxis, M_PI / 4.0, 0.0);

  ExtremaSS_CylinderCone   anEval(aCyl, aCone);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum: distance from cylinder surface to cone apex = 15 - 2 = 13
  const double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinSqDist), 13.0, THE_TOL);
}

TEST_F(ExtremaSS_CylinderConeTest, CoaxialCylinderAndCone_FindsMinimum)
{
  // Coaxial cylinder and cone
  const gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 3.0);
  const gp_Ax3      aConeAxis(gp_Pnt(0, 0, 10), gp_Dir(0, 0, -1));
  const gp_Cone     aCone(aConeAxis, M_PI / 6.0, 0.0);

  ExtremaSS_CylinderCone   anEval(aCyl, aCone);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Should find solutions
  ASSERT_TRUE(aResult.Status == ExtremaSS::Status::OK
              || aResult.Status == ExtremaSS::Status::InfiniteSolutions);
}

TEST_F(ExtremaSS_CylinderConeTest, ParallelAxes_Separated)
{
  // Cylinder along Z
  const gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);

  // Cone with parallel axis
  const gp_Ax3  aConeAxis(gp_Pnt(10, 0, 0), gp_Dir(0, 0, 1));
  const gp_Cone aCone(aConeAxis, M_PI / 6.0, 0.0);

  ExtremaSS_CylinderCone   anEval(aCyl, aCone);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaSS_CylinderConeTest, SkewAxes_FindsMinimum)
{
  // Cylinder along Z
  const gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 1.0);

  // Cone with skew axis
  const gp_Ax3  aConeAxis(gp_Pnt(10, 5, 0), gp_Dir(0, 1, 0));
  const gp_Cone aCone(aConeAxis, M_PI / 4.0, 0.0);

  ExtremaSS_CylinderCone   anEval(aCyl, aCone);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaSS_CylinderConeTest, IntersectingCylinderAndCone_MinDistanceZero)
{
  // Cylinder at origin
  const gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  // Cone that intersects cylinder
  const gp_Ax3  aConeAxis(gp_Pnt(3, 0, 0), gp_Dir(1, 0, 0));
  const gp_Cone aCone(aConeAxis, M_PI / 4.0, 0.0);

  ExtremaSS_CylinderCone   anEval(aCyl, aCone);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  if (aResult.Status == ExtremaSS::Status::OK)
  {
    const double aMinSqDist = aResult.MinSquareDistance();
    EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
  }
}

//==================================================================================================
// Swapped Order Tests
//==================================================================================================

TEST_F(ExtremaSS_CylinderConeTest, SwappedOrder_SameResult)
{
  const gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  const gp_Ax3      aConeAxis(gp_Pnt(15, 0, 0), gp_Dir(1, 0, 0));
  const gp_Cone     aCone(aConeAxis, M_PI / 4.0, 0.0);

  ExtremaSS_CylinderCone anEval1(aCyl, aCone);
  ExtremaSS_CylinderCone anEval2(aCone, aCyl);

  const ExtremaSS::Result& aResult1 = anEval1.Perform(THE_TOL);
  const ExtremaSS::Result& aResult2 = anEval2.Perform(THE_TOL);

  ASSERT_EQ(aResult1.Status, aResult2.Status);

  if (aResult1.Status == ExtremaSS::Status::OK)
  {
    EXPECT_NEAR(aResult1.MinSquareDistance(), aResult2.MinSquareDistance(), THE_TOL);
  }
}

//==================================================================================================
// SearchMode Tests
//==================================================================================================

TEST_F(ExtremaSS_CylinderConeTest, SearchModeMin_OnlyFindsMinimum)
{
  const gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  const gp_Ax3      aConeAxis(gp_Pnt(15, 0, 0), gp_Dir(1, 0, 0));
  const gp_Cone     aCone(aConeAxis, M_PI / 4.0, 0.0);

  ExtremaSS_CylinderCone   anEval(aCyl, aCone);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL, ExtremaSS::SearchMode::Min);

  if (aResult.Status == ExtremaSS::Status::OK)
  {
    for (int i = 0; i < aResult.NbExt(); ++i)
    {
      EXPECT_TRUE(aResult[i].IsMinimum);
    }
  }
}

//==================================================================================================
// Edge Cases
//==================================================================================================

TEST_F(ExtremaSS_CylinderConeTest, SmallConeAngle_NearCylinder)
{
  const gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  const gp_Ax3      aConeAxis(gp_Pnt(10, 0, 0), gp_Dir(1, 0, 0));
  const gp_Cone     aCone(aConeAxis, 0.01, 0.0); // Very small angle

  ExtremaSS_CylinderCone   anEval(aCyl, aCone);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaSS_CylinderConeTest, LargeConeAngle_WideOpening)
{
  const gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  const gp_Ax3      aConeAxis(gp_Pnt(15, 0, 0), gp_Dir(1, 0, 0));
  const gp_Cone     aCone(aConeAxis, M_PI / 3.0, 0.0); // 60 degrees

  ExtremaSS_CylinderCone   anEval(aCyl, aCone);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaSS_CylinderConeTest, PerpendicularAxes_MeetingAtPoint)
{
  const gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 1.0);
  const gp_Ax3      aConeAxis(gp_Pnt(5, 0, 0), gp_Dir(1, 0, 0));
  const gp_Cone     aCone(aConeAxis, M_PI / 4.0, 0.0);

  ExtremaSS_CylinderCone   anEval(aCyl, aCone);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum: from cylinder at X=1 to cone apex at X=5
  const double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinSqDist), 4.0, THE_TOL);
}

TEST_F(ExtremaSS_CylinderConeTest, ConeWithNonZeroRefRadius)
{
  const gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  const gp_Ax3      aConeAxis(gp_Pnt(15, 0, 0), gp_Dir(1, 0, 0));
  const gp_Cone     aCone(aConeAxis, M_PI / 4.0, 3.0); // Non-zero ref radius

  ExtremaSS_CylinderCone   anEval(aCyl, aCone);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);
}
