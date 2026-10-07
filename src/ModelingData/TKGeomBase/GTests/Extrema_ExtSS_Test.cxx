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

#include <Extrema_ExtSS.hxx>
#include <Extrema_POnSurf.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Standard_OutOfRange.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

// NbExt() counts mySqDist, and the analytic parallel branch appends a distance with no matching
// point pair, so Points() must bound against the point sequence rather than against NbExt().
TEST(Extrema_ExtSS_Test, ParallelPlanesHaveADistanceButNoPoints)
{
  occ::handle<Geom_Plane> aP1 = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  occ::handle<Geom_Plane> aP2 = new Geom_Plane(gp_Pnt(0, 0, 5), gp_Dir(0, 0, 1));

  GeomAdaptor_Surface aS1(aP1, -10.0, 10.0, -10.0, 10.0);
  GeomAdaptor_Surface aS2(aP2, -10.0, 10.0, -10.0, 10.0);

  Extrema_ExtSS aExt(aS1, aS2, 1.0e-6, 1.0e-6);
  ASSERT_TRUE(aExt.IsDone());
  ASSERT_TRUE(aExt.IsParallel());

  // The distance is a real measurement and stays available.
  ASSERT_EQ(aExt.NbExt(), 1);
  EXPECT_NEAR(aExt.SquareDistance(1), 25.0, 1.0e-7);

  // The point pair is not: an equidistant family has no unique witness.
  Extrema_POnSurf aPS1, aPS2;
  EXPECT_THROW(aExt.Points(1, aPS1, aPS2), Standard_OutOfRange);
}

// The tighter bound must not refuse a result that does have points.
TEST(Extrema_ExtSS_Test, SeparatedSpheresStillReportTheirPoints)
{
  occ::handle<Geom_SphericalSurface> aSph1 =
    new Geom_SphericalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 3.0);
  occ::handle<Geom_SphericalSurface> aSph2 =
    new Geom_SphericalSurface(gp_Ax3(gp_Pnt(20, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  GeomAdaptor_Surface aS1(aSph1, 0.0, 2.0 * M_PI, -M_PI_2, M_PI_2);
  GeomAdaptor_Surface aS2(aSph2, 0.0, 2.0 * M_PI, -M_PI_2, M_PI_2);

  Extrema_ExtSS aExt(aS1, aS2, 1.0e-6, 1.0e-6);
  ASSERT_TRUE(aExt.IsDone());
  ASSERT_FALSE(aExt.IsParallel());
  ASSERT_GT(aExt.NbExt(), 0);

  for (int anI = 1; anI <= aExt.NbExt(); ++anI)
  {
    Extrema_POnSurf aPS1, aPS2;
    ASSERT_NO_THROW(aExt.Points(anI, aPS1, aPS2));
    EXPECT_NEAR(aPS1.Value().SquareDistance(aPS2.Value()), aExt.SquareDistance(anI), 1.0e-5);
  }
}
