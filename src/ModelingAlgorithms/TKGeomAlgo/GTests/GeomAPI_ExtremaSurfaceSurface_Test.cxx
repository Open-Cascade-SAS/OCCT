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

#include <gtest/gtest.h>

#include <GeomAPI_ExtremaSurfaceSurface.hxx>
#include <Geom_Plane.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>

// Migrated from tests/lowalgos/extss/bug29712_44. The DRAW script contains a
// same-object typo in both extrema calls; its expected distance of 10.0 clearly
// describes the two parallel planes created immediately above, so this test
// preserves the intended regression check.
TEST(GeomAPI_ExtremaSurfaceSurfaceTest, OCC29712_44_ParallelPlanes)
{
  const occ::handle<Geom_Plane> aPlane1 =
    new Geom_Plane(gp_Pln(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)));
  const occ::handle<Geom_Plane> aPlane2 =
    new Geom_Plane(gp_Pln(gp_Pnt(0.0, 0.0, 10.0), gp_Dir(0.0, 0.0, 1.0)));

  GeomAPI_ExtremaSurfaceSurface anExtrema(aPlane1, aPlane2);
  ASSERT_TRUE(anExtrema.Extrema().IsParallel());
  EXPECT_NEAR(anExtrema.LowerDistance(), 10.0, 1.e-7);
}

// Extra validation for the source typo: a plane compared with itself must have
// zero lower distance, rather than the distance between s1 and s2.
TEST(GeomAPI_ExtremaSurfaceSurfaceTest, OCC29712_44_SelfComparisonHasZeroDistance)
{
  const occ::handle<Geom_Plane> aPlane =
    new Geom_Plane(gp_Pln(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)));

  GeomAPI_ExtremaSurfaceSurface anExtrema(aPlane, aPlane);
  ASSERT_TRUE(anExtrema.Extrema().IsParallel());
  EXPECT_NEAR(anExtrema.LowerDistance(), 0.0, 1.e-7);
}
