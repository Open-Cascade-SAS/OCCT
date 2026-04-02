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

#include <GeomPlate_BuildPlateSurface.hxx>
#include <GeomPlate_PointConstraint.hxx>
#include <GeomPlate_Surface.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

// Regression test for bug #30: IsDone() must be false after Init() / no-constraint Perform().
TEST(GeomPlate_BuildPlateSurface, OCC525_PerformWithoutConstraints)
{
  GeomPlate_BuildPlateSurface aBuilder;
  aBuilder.Perform();

  EXPECT_FALSE(aBuilder.IsDone());
  EXPECT_TRUE(aBuilder.Surface().IsNull())
    << "Surface should be null when Perform() is called without constraints";
}

// Regression test for bug #19: Surface() must be null after failed recomputation.
// After Init() + empty Perform(), stale surface must not be observable.
TEST(GeomPlate_BuildPlateSurface, Perform_ClearsStaleResult)
{
  GeomPlate_BuildPlateSurface aBuilder(3, 10, 3, 1.e-5, 1.e-4, 0.01, 0.1, false);

  // Add point constraints and solve.
  aBuilder.Add(new GeomPlate_PointConstraint(gp_Pnt(0, 0, 0), 0));
  aBuilder.Add(new GeomPlate_PointConstraint(gp_Pnt(1, 0, 0), 0));
  aBuilder.Add(new GeomPlate_PointConstraint(gp_Pnt(0, 1, 0), 0));
  aBuilder.Add(new GeomPlate_PointConstraint(gp_Pnt(1, 1, 0.1), 0));
  aBuilder.Perform();

  // Init clears constraints, then Perform with no constraints must
  // produce null surface (not stale result from the previous solve).
  aBuilder.Init();
  aBuilder.Perform();
  EXPECT_FALSE(aBuilder.IsDone());
  EXPECT_TRUE(aBuilder.Surface().IsNull())
    << "Surface must be null after Init() + empty Perform()";
}
