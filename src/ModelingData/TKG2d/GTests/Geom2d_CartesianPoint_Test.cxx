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

#include <Geom2d_CartesianPoint.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

#include <cmath>

TEST(Geom2d_CartesianPointTest, ConstructFromCoords)
{
  occ::handle<Geom2d_CartesianPoint> aPnt = new Geom2d_CartesianPoint(3.0, 4.0);
  EXPECT_NEAR(aPnt->X(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aPnt->Y(), 4.0, Precision::Confusion());
}

TEST(Geom2d_CartesianPointTest, ConstructFromPnt2d)
{
  occ::handle<Geom2d_CartesianPoint> aPnt =
    new Geom2d_CartesianPoint(gp_Pnt2d(5.0, 6.0));
  EXPECT_NEAR(aPnt->X(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aPnt->Y(), 6.0, Precision::Confusion());
}

TEST(Geom2d_CartesianPointTest, SetCoord)
{
  occ::handle<Geom2d_CartesianPoint> aPnt = new Geom2d_CartesianPoint(0.0, 0.0);
  aPnt->SetCoord(7.0, 8.0);
  EXPECT_NEAR(aPnt->X(), 7.0, Precision::Confusion());
  EXPECT_NEAR(aPnt->Y(), 8.0, Precision::Confusion());
}

TEST(Geom2d_CartesianPointTest, SetPnt2d)
{
  occ::handle<Geom2d_CartesianPoint> aPnt = new Geom2d_CartesianPoint(0.0, 0.0);
  aPnt->SetPnt2d(gp_Pnt2d(1.0, 2.0));
  EXPECT_NEAR(aPnt->X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aPnt->Y(), 2.0, Precision::Confusion());
}

TEST(Geom2d_CartesianPointTest, SetX_SetY)
{
  occ::handle<Geom2d_CartesianPoint> aPnt = new Geom2d_CartesianPoint(0.0, 0.0);
  aPnt->SetX(10.0);
  aPnt->SetY(20.0);
  EXPECT_NEAR(aPnt->X(), 10.0, Precision::Confusion());
  EXPECT_NEAR(aPnt->Y(), 20.0, Precision::Confusion());
}

TEST(Geom2d_CartesianPointTest, Coord)
{
  occ::handle<Geom2d_CartesianPoint> aPnt = new Geom2d_CartesianPoint(3.0, 4.0);
  double aX = 0.0, aY = 0.0;
  aPnt->Coord(aX, aY);
  EXPECT_NEAR(aX, 3.0, Precision::Confusion());
  EXPECT_NEAR(aY, 4.0, Precision::Confusion());
}

TEST(Geom2d_CartesianPointTest, Pnt2d)
{
  occ::handle<Geom2d_CartesianPoint> aPnt = new Geom2d_CartesianPoint(3.0, 4.0);
  const gp_Pnt2d                     aGpPnt = aPnt->Pnt2d();
  EXPECT_NEAR(aGpPnt.X(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aGpPnt.Y(), 4.0, Precision::Confusion());
}

TEST(Geom2d_CartesianPointTest, Distance)
{
  occ::handle<Geom2d_CartesianPoint> aP1 = new Geom2d_CartesianPoint(0.0, 0.0);
  occ::handle<Geom2d_CartesianPoint> aP2 = new Geom2d_CartesianPoint(3.0, 4.0);
  EXPECT_NEAR(aP1->Distance(aP2), 5.0, Precision::Confusion());
}

TEST(Geom2d_CartesianPointTest, SquareDistance)
{
  occ::handle<Geom2d_CartesianPoint> aP1 = new Geom2d_CartesianPoint(0.0, 0.0);
  occ::handle<Geom2d_CartesianPoint> aP2 = new Geom2d_CartesianPoint(3.0, 4.0);
  EXPECT_NEAR(aP1->SquareDistance(aP2), 25.0, Precision::Confusion());
}

TEST(Geom2d_CartesianPointTest, Distance_SamePoint)
{
  occ::handle<Geom2d_CartesianPoint> aPnt = new Geom2d_CartesianPoint(5.0, 5.0);
  EXPECT_NEAR(aPnt->Distance(aPnt), 0.0, Precision::Confusion());
}

TEST(Geom2d_CartesianPointTest, Transform_Translation)
{
  occ::handle<Geom2d_CartesianPoint> aPnt = new Geom2d_CartesianPoint(1.0, 2.0);

  gp_Trsf2d aTrsf;
  aTrsf.SetTranslation(gp_Vec2d(3.0, 4.0));
  aPnt->Transform(aTrsf);

  EXPECT_NEAR(aPnt->X(), 4.0, Precision::Confusion());
  EXPECT_NEAR(aPnt->Y(), 6.0, Precision::Confusion());
}

TEST(Geom2d_CartesianPointTest, Transform_Rotation)
{
  occ::handle<Geom2d_CartesianPoint> aPnt = new Geom2d_CartesianPoint(1.0, 0.0);

  gp_Trsf2d aTrsf;
  aTrsf.SetRotation(gp_Pnt2d(0.0, 0.0), M_PI / 2.0);
  aPnt->Transform(aTrsf);

  EXPECT_NEAR(aPnt->X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aPnt->Y(), 1.0, Precision::Confusion());
}

TEST(Geom2d_CartesianPointTest, Copy)
{
  occ::handle<Geom2d_CartesianPoint> aPnt = new Geom2d_CartesianPoint(3.0, 4.0);
  occ::handle<Geom2d_Geometry>       aCopyGeom = aPnt->Copy();
  occ::handle<Geom2d_CartesianPoint> aCopy =
    occ::down_cast<Geom2d_CartesianPoint>(aCopyGeom);

  ASSERT_FALSE(aCopy.IsNull());
  EXPECT_NEAR(aCopy->X(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aCopy->Y(), 4.0, Precision::Confusion());

  aCopy->SetCoord(0.0, 0.0);
  EXPECT_NEAR(aPnt->X(), 3.0, Precision::Confusion());
}
