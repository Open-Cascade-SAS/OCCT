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

#include <BRep_TFace.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Precision.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pln.hxx>

TEST(BRep_TFace_Test, DefaultConstruction_IsPlaneIsFalse)
{
  Handle(BRep_TFace) aTFace = new BRep_TFace();
  EXPECT_FALSE(aTFace->IsPlane()) << "New TFace should have IsPlane == false";
}

TEST(BRep_TFace_Test, Surface_Plane_IsPlaneTrue)
{
  Handle(BRep_TFace) aTFace = new BRep_TFace();
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pln());
  aTFace->Surface(aPlane);
  EXPECT_TRUE(aTFace->IsPlane()) << "Setting Geom_Plane should make IsPlane true";
}

TEST(BRep_TFace_Test, Surface_Cylinder_IsPlaneFalse)
{
  Handle(BRep_TFace)              aTFace = new BRep_TFace();
  Handle(Geom_CylindricalSurface) aCyl =
    new Geom_CylindricalSurface(gp_Ax3(), 1.0);
  aTFace->Surface(aCyl);
  EXPECT_FALSE(aTFace->IsPlane()) << "Setting Geom_CylindricalSurface should make IsPlane false";
}

TEST(BRep_TFace_Test, Surface_TrimmedPlane_IsPlaneTrue)
{
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pln());
  Handle(Geom_RectangularTrimmedSurface) aTrimmed =
    new Geom_RectangularTrimmedSurface(aPlane, 0.0, 1.0, 0.0, 1.0);
  Handle(BRep_TFace) aTFace = new BRep_TFace();
  aTFace->Surface(aTrimmed);
  EXPECT_TRUE(aTFace->IsPlane())
    << "RectangularTrimmedSurface wrapping a plane should make IsPlane true";
}

TEST(BRep_TFace_Test, Surface_OffsetPlane_IsPlaneTrue)
{
  Handle(Geom_Plane)         aPlane  = new Geom_Plane(gp_Pln());
  Handle(Geom_OffsetSurface) anOffset = new Geom_OffsetSurface(aPlane, 5.0);
  Handle(BRep_TFace)         aTFace  = new BRep_TFace();
  aTFace->Surface(anOffset);
  EXPECT_TRUE(aTFace->IsPlane())
    << "OffsetSurface wrapping a plane should make IsPlane true";
}

TEST(BRep_TFace_Test, Surface_Null_IsPlaneFalse)
{
  Handle(BRep_TFace) aTFace = new BRep_TFace();
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pln());
  aTFace->Surface(aPlane);
  EXPECT_TRUE(aTFace->IsPlane()) << "Should be plane initially";

  // Now set null surface
  aTFace->Surface(Handle(Geom_Surface)());
  EXPECT_FALSE(aTFace->IsPlane()) << "Setting null surface should make IsPlane false";
}

TEST(BRep_TFace_Test, Surface_ChangeFromPlaneToNonPlane)
{
  Handle(BRep_TFace) aTFace = new BRep_TFace();
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pln());
  aTFace->Surface(aPlane);
  EXPECT_TRUE(aTFace->IsPlane()) << "Should be plane after setting plane";

  Handle(Geom_CylindricalSurface) aCyl =
    new Geom_CylindricalSurface(gp_Ax3(), 1.0);
  aTFace->Surface(aCyl);
  EXPECT_FALSE(aTFace->IsPlane()) << "Should not be plane after changing to cylinder";
}

TEST(BRep_TFace_Test, EmptyCopy_PreservesIsPlane)
{
  Handle(BRep_TFace) aTFace = new BRep_TFace();
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pln());
  aTFace->Surface(aPlane);
  ASSERT_TRUE(aTFace->IsPlane());

  Handle(TopoDS_TShape)     aCopy     = aTFace->EmptyCopy();
  const Handle(BRep_TFace)& aCopyFace = Handle(BRep_TFace)::DownCast(aCopy);
  ASSERT_FALSE(aCopyFace.IsNull()) << "EmptyCopy should return BRep_TFace";
  EXPECT_TRUE(aCopyFace->IsPlane()) << "EmptyCopy should preserve IsPlane flag";
}

TEST(BRep_TFace_Test, Tolerance_ClampedOnSet)
{
  Handle(BRep_TFace) aTFace = new BRep_TFace();
  aTFace->Tolerance(1e-20);
  EXPECT_GE(aTFace->Tolerance(), Precision::Confusion())
    << "Tolerance should be clamped to Precision::Confusion()";
}
