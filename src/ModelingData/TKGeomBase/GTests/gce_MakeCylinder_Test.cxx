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

#include <gce_MakeCylinder.hxx>
#include <gp_Ax1.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

#include <cmath>

TEST(gce_MakeCylinderTest, FromAxis_YBranch_DoneAndOrthogonalXAxis)
{
  const gp_Ax1 aAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 1.0));

  gce_MakeCylinder aMaker(aAxis, 5.0);
  ASSERT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);

  const gp_Cylinder& aCylinder = aMaker.Value();
  EXPECT_NEAR(aCylinder.Radius(), 5.0, Precision::Confusion());
  EXPECT_TRUE(aCylinder.Axis().Direction().IsParallel(aAxis.Direction(), Precision::Angular()));

  const double aDot =
    aCylinder.Axis().Direction().XYZ().Dot(aCylinder.Position().XDirection().XYZ());
  EXPECT_NEAR(aDot, 0.0, Precision::Angular());
}

TEST(gce_MakeCylinderTest, FromPoints_ZBranch_DoneAndOrthogonalXAxis)
{
  const gp_Pnt aP1(0.0, 0.0, 0.0);
  const gp_Pnt aP2(0.0, 0.0, 10.0);
  const gp_Pnt aP3(3.0, 4.0, 0.0);

  gce_MakeCylinder aMaker(aP1, aP2, aP3);
  ASSERT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);

  const gp_Cylinder& aCylinder = aMaker.Value();
  EXPECT_NEAR(aCylinder.Radius(), 5.0, Precision::Confusion());
  EXPECT_TRUE(
    aCylinder.Axis().Direction().IsParallel(gp_Dir(aP2.XYZ() - aP1.XYZ()), Precision::Angular()));

  const double aDot =
    aCylinder.Axis().Direction().XYZ().Dot(aCylinder.Position().XDirection().XYZ());
  EXPECT_NEAR(aDot, 0.0, Precision::Angular());
}

TEST(gce_MakeCylinderTest, FromAxis_NegativeRadius_NegativeRadius)
{
  const gp_Ax1 aAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));

  gce_MakeCylinder aMaker(aAxis, -1.0);
  EXPECT_FALSE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_NegativeRadius);
}
