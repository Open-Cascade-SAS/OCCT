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

#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

TEST(Geom_PlaneTest, ConstructFromAx3)
{
  gp_Ax3             anAx3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Plane) aPlane = new Geom_Plane(anAx3);
  ASSERT_FALSE(aPlane.IsNull());
  EXPECT_TRUE(aPlane->Location().IsEqual(gp_Pnt(0.0, 0.0, 0.0), Precision::Confusion()));
}

TEST(Geom_PlaneTest, ConstructFromPointAndDir)
{
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pnt(1.0, 2.0, 3.0), gp_Dir(0.0, 0.0, 1.0));
  ASSERT_FALSE(aPlane.IsNull());
  EXPECT_TRUE(aPlane->Location().IsEqual(gp_Pnt(1.0, 2.0, 3.0), Precision::Confusion()));
}

TEST(Geom_PlaneTest, D0Evaluation)
{
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Pnt             aPnt;

  // XY plane at origin: D0(U,V) = (U,V,0) for default axis
  aPlane->D0(3.0, 4.0, aPnt);
  EXPECT_NEAR(aPnt.X(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aPnt.Y(), 4.0, Precision::Confusion());
  EXPECT_NEAR(aPnt.Z(), 0.0, Precision::Confusion());
}

TEST(Geom_PlaneTest, Coefficients)
{
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pnt(0.0, 0.0, 5.0), gp_Dir(0.0, 0.0, 1.0));
  double             anA, aB, aC, aD;
  aPlane->Coefficients(anA, aB, aC, aD);
  EXPECT_NEAR(anA, 0.0, Precision::Confusion());
  EXPECT_NEAR(aB, 0.0, Precision::Confusion());
  EXPECT_NEAR(aC, 1.0, Precision::Confusion());
  EXPECT_NEAR(aD, -5.0, Precision::Confusion());
}

TEST(Geom_PlaneTest, UIso)
{
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Curve) aUIso  = aPlane->UIso(2.0);
  ASSERT_FALSE(aUIso.IsNull());

  // UIso at U=2 on XY plane should be a line at x=2, varying in Y
  Handle(Geom_Line) aLine = Handle(Geom_Line)::DownCast(aUIso);
  ASSERT_FALSE(aLine.IsNull());

  gp_Pnt aPnt;
  aLine->D0(0.0, aPnt);
  EXPECT_NEAR(aPnt.X(), 2.0, Precision::Confusion());
}

TEST(Geom_PlaneTest, VIso)
{
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Curve) aVIso  = aPlane->VIso(3.0);
  ASSERT_FALSE(aVIso.IsNull());

  // VIso at V=3 on XY plane should be a line at y=3, varying in X
  Handle(Geom_Line) aLine = Handle(Geom_Line)::DownCast(aVIso);
  ASSERT_FALSE(aLine.IsNull());

  gp_Pnt aPnt;
  aLine->D0(0.0, aPnt);
  EXPECT_NEAR(aPnt.Y(), 3.0, Precision::Confusion());
}

TEST(Geom_PlaneTest, Transform)
{
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Trsf            aTrsf;
  aTrsf.SetTranslation(gp_Vec(0.0, 0.0, 10.0));
  aPlane->Transform(aTrsf);
  EXPECT_NEAR(aPlane->Location().Z(), 10.0, Precision::Confusion());
}

TEST(Geom_PlaneTest, Copy)
{
  Handle(Geom_Plane)    aPlane = new Geom_Plane(gp_Pnt(1.0, 2.0, 3.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Geometry) aCopy  = aPlane->Copy();
  ASSERT_FALSE(aCopy.IsNull());
  Handle(Geom_Plane) aCopyPlane = Handle(Geom_Plane)::DownCast(aCopy);
  ASSERT_FALSE(aCopyPlane.IsNull());
  EXPECT_TRUE(aCopyPlane->Location().IsEqual(gp_Pnt(1.0, 2.0, 3.0), Precision::Confusion()));
}
