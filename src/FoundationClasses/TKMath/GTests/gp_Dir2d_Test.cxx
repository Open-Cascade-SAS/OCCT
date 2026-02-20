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

#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_XY.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>

#include <gtest/gtest.h>

TEST(gp_Dir2dTest, CoordinateConstructor)
{
  gp_Dir2d aDir(1.0, 0.0);
  EXPECT_NEAR(aDir.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aDir.Y(), 0.0, Precision::Confusion());
}

TEST(gp_Dir2dTest, XYConstructor)
{
  gp_XY aXY(0.0, 3.0);
  gp_Dir2d aDir(aXY);
  EXPECT_NEAR(aDir.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aDir.Y(), 1.0, Precision::Confusion());
}

TEST(gp_Dir2dTest, Vec2dConstructor)
{
  gp_Vec2d aVec(3.0, 0.0);
  gp_Dir2d aDir(aVec);
  EXPECT_NEAR(aDir.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aDir.Y(), 0.0, Precision::Confusion());
}

TEST(gp_Dir2dTest, Angle)
{
  gp_Dir2d aD1(1.0, 0.0);
  gp_Dir2d aD2(0.0, 1.0);
  EXPECT_NEAR(aD1.Angle(aD2), M_PI_2, Precision::Angular());
}

TEST(gp_Dir2dTest, IsEqual)
{
  gp_Dir2d aD1(1.0, 0.0);
  gp_Dir2d aD2(1.0, 0.0);
  EXPECT_TRUE(aD1.IsEqual(aD2, Precision::Angular()));
}

TEST(gp_Dir2dTest, IsNormal)
{
  gp_Dir2d aD1(1.0, 0.0);
  gp_Dir2d aD2(0.0, 1.0);
  EXPECT_TRUE(aD1.IsNormal(aD2, Precision::Angular()));
}

TEST(gp_Dir2dTest, IsOpposite)
{
  gp_Dir2d aD1(1.0, 0.0);
  gp_Dir2d aD2(-1.0, 0.0);
  EXPECT_TRUE(aD1.IsOpposite(aD2, Precision::Angular()));
}

TEST(gp_Dir2dTest, IsParallel)
{
  gp_Dir2d aD1(1.0, 0.0);
  gp_Dir2d aD2(-1.0, 0.0);
  EXPECT_TRUE(aD1.IsParallel(aD2, Precision::Angular()));
}

TEST(gp_Dir2dTest, Dot)
{
  gp_Dir2d aD1(1.0, 0.0);
  gp_Dir2d aD2(0.0, 1.0);
  EXPECT_NEAR(aD1.Dot(aD2), 0.0, Precision::Confusion());

  gp_Dir2d aD3(1.0, 0.0);
  EXPECT_NEAR(aD1.Dot(aD3), 1.0, Precision::Confusion());
}

TEST(gp_Dir2dTest, Crossed)
{
  gp_Dir2d aD1(1.0, 0.0);
  gp_Dir2d aD2(0.0, 1.0);
  // D1.X*D2.Y - D1.Y*D2.X = 1*1 - 0*0 = 1
  EXPECT_NEAR(aD1.Crossed(aD2), 1.0, Precision::Confusion());
}

TEST(gp_Dir2dTest, Rotate)
{
  gp_Dir2d aDir(1.0, 0.0);
  gp_Dir2d aRotated = aDir.Rotated(M_PI_2);
  EXPECT_NEAR(aRotated.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aRotated.Y(), 1.0, Precision::Confusion());
}

TEST(gp_Dir2dTest, Mirror)
{
  gp_Dir2d aDir(1.0, 0.0);
  // Mirror through the Y axis
  gp_Ax2d anAxis(gp_Pnt2d(0.0, 0.0), gp_Dir2d(0.0, 1.0));
  gp_Dir2d aMirrored = aDir.Mirrored(anAxis);
  // Mirroring (1,0) through axis with direction (0,1):
  // d_mirror = 2*(d.axis_dir)*axis_dir - d = 2*0*(0,1) - (1,0) = (-1,0)
  EXPECT_NEAR(aMirrored.X(), -1.0, Precision::Confusion());
  EXPECT_NEAR(aMirrored.Y(), 0.0, Precision::Confusion());
}

TEST(gp_Dir2dTest, Transform)
{
  gp_Dir2d  aDir(1.0, 0.0);
  gp_Trsf2d aTrsf;
  aTrsf.SetRotation(gp_Pnt2d(0.0, 0.0), M_PI);
  gp_Dir2d aTransformed = aDir.Transformed(aTrsf);
  EXPECT_NEAR(aTransformed.X(), -1.0, Precision::Confusion());
  EXPECT_NEAR(aTransformed.Y(), 0.0, Precision::Confusion());
}

TEST(gp_Dir2dTest, ZeroMagnitudeThrows)
{
  EXPECT_THROW(gp_Dir2d aDir(0.0, 0.0), Standard_ConstructionError);
}
