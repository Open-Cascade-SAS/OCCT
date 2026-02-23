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

#include <Geom2d_AxisPlacement.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

#include <cmath>

TEST(Geom2d_AxisPlacementTest, ConstructFromAx2d)
{
  gp_Ax2d                           anAxis(gp_Pnt2d(1.0, 2.0), gp_Dir2d(1.0, 0.0));
  occ::handle<Geom2d_AxisPlacement> anAP = new Geom2d_AxisPlacement(anAxis);

  EXPECT_NEAR(anAP->Location().X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(anAP->Location().Y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(anAP->Direction().X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(anAP->Direction().Y(), 0.0, Precision::Confusion());
}

TEST(Geom2d_AxisPlacementTest, ConstructFromPointAndDir)
{
  occ::handle<Geom2d_AxisPlacement> anAP =
    new Geom2d_AxisPlacement(gp_Pnt2d(3.0, 4.0), gp_Dir2d(0.0, 1.0));

  EXPECT_NEAR(anAP->Location().X(), 3.0, Precision::Confusion());
  EXPECT_NEAR(anAP->Direction().Y(), 1.0, Precision::Confusion());
}

TEST(Geom2d_AxisPlacementTest, SetLocation)
{
  occ::handle<Geom2d_AxisPlacement> anAP =
    new Geom2d_AxisPlacement(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  anAP->SetLocation(gp_Pnt2d(5.0, 6.0));

  EXPECT_NEAR(anAP->Location().X(), 5.0, Precision::Confusion());
  EXPECT_NEAR(anAP->Location().Y(), 6.0, Precision::Confusion());
}

TEST(Geom2d_AxisPlacementTest, SetDirection)
{
  occ::handle<Geom2d_AxisPlacement> anAP =
    new Geom2d_AxisPlacement(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  anAP->SetDirection(gp_Dir2d(0.0, 1.0));

  EXPECT_NEAR(anAP->Direction().X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(anAP->Direction().Y(), 1.0, Precision::Confusion());
}

TEST(Geom2d_AxisPlacementTest, SetAxis)
{
  occ::handle<Geom2d_AxisPlacement> anAP =
    new Geom2d_AxisPlacement(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));

  gp_Ax2d aNewAxis(gp_Pnt2d(10.0, 20.0), gp_Dir2d(0.0, 1.0));
  anAP->SetAxis(aNewAxis);

  EXPECT_NEAR(anAP->Location().X(), 10.0, Precision::Confusion());
  EXPECT_NEAR(anAP->Direction().Y(), 1.0, Precision::Confusion());
}

TEST(Geom2d_AxisPlacementTest, Ax2d)
{
  occ::handle<Geom2d_AxisPlacement> anAP =
    new Geom2d_AxisPlacement(gp_Pnt2d(1.0, 2.0), gp_Dir2d(1.0, 0.0));

  const gp_Ax2d anAxis = anAP->Ax2d();
  EXPECT_NEAR(anAxis.Location().X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(anAxis.Direction().X(), 1.0, Precision::Confusion());
}

TEST(Geom2d_AxisPlacementTest, Angle_Perpendicular)
{
  occ::handle<Geom2d_AxisPlacement> anAP1 =
    new Geom2d_AxisPlacement(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  occ::handle<Geom2d_AxisPlacement> anAP2 =
    new Geom2d_AxisPlacement(gp_Pnt2d(0.0, 0.0), gp_Dir2d(0.0, 1.0));

  EXPECT_NEAR(anAP1->Angle(anAP2), M_PI / 2.0, Precision::Confusion());
}

TEST(Geom2d_AxisPlacementTest, Angle_Parallel)
{
  occ::handle<Geom2d_AxisPlacement> anAP1 =
    new Geom2d_AxisPlacement(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  occ::handle<Geom2d_AxisPlacement> anAP2 =
    new Geom2d_AxisPlacement(gp_Pnt2d(5.0, 5.0), gp_Dir2d(1.0, 0.0));

  EXPECT_NEAR(anAP1->Angle(anAP2), 0.0, Precision::Confusion());
}

TEST(Geom2d_AxisPlacementTest, Angle_Opposite)
{
  occ::handle<Geom2d_AxisPlacement> anAP1 =
    new Geom2d_AxisPlacement(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  occ::handle<Geom2d_AxisPlacement> anAP2 =
    new Geom2d_AxisPlacement(gp_Pnt2d(0.0, 0.0), gp_Dir2d(-1.0, 0.0));

  EXPECT_NEAR(std::abs(anAP1->Angle(anAP2)), M_PI, Precision::Confusion());
}

TEST(Geom2d_AxisPlacementTest, Reverse)
{
  occ::handle<Geom2d_AxisPlacement> anAP =
    new Geom2d_AxisPlacement(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  anAP->Reverse();

  EXPECT_NEAR(anAP->Direction().X(), -1.0, Precision::Confusion());
  EXPECT_NEAR(anAP->Direction().Y(), 0.0, Precision::Confusion());
  // Location unchanged
  EXPECT_NEAR(anAP->Location().X(), 0.0, Precision::Confusion());
}

TEST(Geom2d_AxisPlacementTest, Reversed)
{
  occ::handle<Geom2d_AxisPlacement> anAP =
    new Geom2d_AxisPlacement(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  occ::handle<Geom2d_AxisPlacement> aReversed = anAP->Reversed();

  EXPECT_NEAR(aReversed->Direction().X(), -1.0, Precision::Confusion());
  // Original unchanged
  EXPECT_NEAR(anAP->Direction().X(), 1.0, Precision::Confusion());
}

TEST(Geom2d_AxisPlacementTest, Transform_Translation)
{
  occ::handle<Geom2d_AxisPlacement> anAP =
    new Geom2d_AxisPlacement(gp_Pnt2d(1.0, 2.0), gp_Dir2d(1.0, 0.0));

  gp_Trsf2d aTrsf;
  aTrsf.SetTranslation(gp_Vec2d(10.0, 20.0));
  anAP->Transform(aTrsf);

  EXPECT_NEAR(anAP->Location().X(), 11.0, Precision::Confusion());
  EXPECT_NEAR(anAP->Location().Y(), 22.0, Precision::Confusion());
  // Direction unchanged
  EXPECT_NEAR(anAP->Direction().X(), 1.0, Precision::Confusion());
}

TEST(Geom2d_AxisPlacementTest, Transform_Rotation)
{
  occ::handle<Geom2d_AxisPlacement> anAP =
    new Geom2d_AxisPlacement(gp_Pnt2d(1.0, 0.0), gp_Dir2d(1.0, 0.0));

  gp_Trsf2d aTrsf;
  aTrsf.SetRotation(gp_Pnt2d(0.0, 0.0), M_PI / 2.0);
  anAP->Transform(aTrsf);

  EXPECT_NEAR(anAP->Location().X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(anAP->Location().Y(), 1.0, Precision::Confusion());
  EXPECT_NEAR(anAP->Direction().X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(anAP->Direction().Y(), 1.0, Precision::Confusion());
}

TEST(Geom2d_AxisPlacementTest, Copy)
{
  occ::handle<Geom2d_AxisPlacement> anAP =
    new Geom2d_AxisPlacement(gp_Pnt2d(1.0, 2.0), gp_Dir2d(0.0, 1.0));
  occ::handle<Geom2d_Geometry>      aCopyGeom = anAP->Copy();
  occ::handle<Geom2d_AxisPlacement> aCopy     = occ::down_cast<Geom2d_AxisPlacement>(aCopyGeom);

  ASSERT_FALSE(aCopy.IsNull());
  EXPECT_NEAR(aCopy->Location().X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aCopy->Direction().Y(), 1.0, Precision::Confusion());

  // Verify independence
  aCopy->SetLocation(gp_Pnt2d(99.0, 99.0));
  EXPECT_NEAR(anAP->Location().X(), 1.0, Precision::Confusion());
}
