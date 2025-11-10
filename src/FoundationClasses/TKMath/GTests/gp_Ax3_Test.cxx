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

#include <gp_Ax3.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

TEST(gp_Ax3_Test, OCC29406_SetDirectionPreservesOrientation)
{
  // Test Main (Z) direction
  {
    // gp_Ax3::SetDirection() test
    gp_Ax3 anAx1, anAx2, anAx3, anAx4;
    anAx3.ZReverse();
    anAx4.ZReverse();

    Standard_Boolean bDirect1 = anAx1.Direct();
    anAx1.SetDirection(gp::DX());
    EXPECT_EQ(bDirect1, anAx1.Direct()) << "Coordinate system orientation should be preserved";
    EXPECT_TRUE(gp::DX().IsEqual(anAx1.Direction(), Precision::Angular()));

    Standard_Boolean bDirect2 = anAx2.Direct();
    anAx2.SetDirection(-gp::DX());
    EXPECT_EQ(bDirect2, anAx2.Direct());
    EXPECT_TRUE((-gp::DX()).IsEqual(anAx2.Direction(), Precision::Angular()));

    Standard_Boolean bDirect3 = anAx3.Direct();
    anAx3.SetDirection(gp::DX());
    EXPECT_EQ(bDirect3, anAx3.Direct());
    EXPECT_TRUE(gp::DX().IsEqual(anAx3.Direction(), Precision::Angular()));

    Standard_Boolean bDirect4 = anAx4.Direct();
    anAx4.SetDirection(-gp::DX());
    EXPECT_EQ(bDirect4, anAx4.Direct());
    EXPECT_TRUE((-gp::DX()).IsEqual(anAx4.Direction(), Precision::Angular()));

    // gp_Ax3::SetAxis() test
    gp_Ax3 anAx5, anAx6;
    gp_Ax1 anAx0_1(gp::Origin(), gp::DX());
    gp_Ax1 anAx0_2(gp::Origin(), -gp::DX());

    Standard_Boolean bDirect5 = anAx5.Direct();
    anAx5.SetAxis(anAx0_1);
    EXPECT_EQ(bDirect5, anAx5.Direct());
    EXPECT_TRUE(anAx0_1.Direction().IsEqual(anAx5.Direction(), Precision::Angular()));

    Standard_Boolean bDirect6 = anAx6.Direct();
    anAx6.SetAxis(anAx0_2);
    EXPECT_EQ(bDirect6, anAx6.Direct());
    EXPECT_TRUE(anAx0_2.Direction().IsEqual(anAx6.Direction(), Precision::Angular()));
  }

  // Test X direction
  {
    gp_Ax3 anAx1, anAx2, anAx3, anAx4;
    anAx3.XReverse();
    anAx4.XReverse();

    Standard_Boolean bDirect1 = anAx1.Direct();
    anAx1.SetXDirection(gp::DZ());
    EXPECT_EQ(bDirect1, anAx1.Direct());
    gp_Dir aGoodY1 = anAx1.Direction().Crossed(gp::DZ());
    if (anAx1.Direct())
    {
      EXPECT_TRUE(aGoodY1.IsEqual(anAx1.YDirection(), Precision::Angular()));
    }
    else
    {
      EXPECT_TRUE(aGoodY1.IsOpposite(anAx1.YDirection(), Precision::Angular()));
    }

    Standard_Boolean bDirect2 = anAx2.Direct();
    anAx2.SetXDirection(-gp::DZ());
    EXPECT_EQ(bDirect2, anAx2.Direct());

    Standard_Boolean bDirect3 = anAx3.Direct();
    anAx3.SetXDirection(gp::DZ());
    EXPECT_EQ(bDirect3, anAx3.Direct());

    Standard_Boolean bDirect4 = anAx4.Direct();
    anAx4.SetXDirection(-gp::DZ());
    EXPECT_EQ(bDirect4, anAx4.Direct());
  }

  // Test Y direction
  {
    gp_Ax3 anAx1, anAx2, anAx3, anAx4;
    anAx3.YReverse();
    anAx4.YReverse();

    Standard_Boolean bDirect1 = anAx1.Direct();
    anAx1.SetYDirection(gp::DZ());
    EXPECT_EQ(bDirect1, anAx1.Direct());
    gp_Dir aGoodX1 = anAx1.Direction().Crossed(gp::DZ());
    if (anAx1.Direct())
    {
      EXPECT_TRUE(aGoodX1.IsOpposite(anAx1.XDirection(), Precision::Angular()));
    }
    else
    {
      EXPECT_TRUE(aGoodX1.IsEqual(anAx1.XDirection(), Precision::Angular()));
    }

    Standard_Boolean bDirect2 = anAx2.Direct();
    anAx2.SetYDirection(-gp::DZ());
    EXPECT_EQ(bDirect2, anAx2.Direct());

    Standard_Boolean bDirect3 = anAx3.Direct();
    anAx3.SetYDirection(gp::DZ());
    EXPECT_EQ(bDirect3, anAx3.Direct());

    Standard_Boolean bDirect4 = anAx4.Direct();
    anAx4.SetYDirection(-gp::DZ());
    EXPECT_EQ(bDirect4, anAx4.Direct());
  }
}
