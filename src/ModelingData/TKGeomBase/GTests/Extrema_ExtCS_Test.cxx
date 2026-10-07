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

#include <Extrema_ExtCS.hxx>
#include <Extrema_POnCurv.hxx>
#include <Extrema_POnSurf.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Standard_OutOfRange.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

// NbExt() counts mySqDist, and the analytic parallel branch appends a distance with no matching
// point pair, so Points() must bound against the point sequence rather than against NbExt().
TEST(Extrema_ExtCS_Test, LineParallelToPlaneHasADistanceButNoPoints)
{
  occ::handle<Geom_Line>  aLine  = new Geom_Line(gp_Pnt(0, 0, 5), gp_Dir(1, 0, 0));
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  GeomAdaptor_Curve   aC(aLine, -10.0, 10.0);
  GeomAdaptor_Surface aS(aPlane, -10.0, 10.0, -10.0, 10.0);

  Extrema_ExtCS aExt(aC, aS, 1.0e-6, 1.0e-6);
  ASSERT_TRUE(aExt.IsDone());
  ASSERT_TRUE(aExt.IsParallel());

  // The distance is a real measurement and stays available.
  ASSERT_EQ(aExt.NbExt(), 1);
  EXPECT_NEAR(aExt.SquareDistance(1), 25.0, 1.0e-7);

  // The point pair is not: an equidistant family has no unique witness.
  Extrema_POnCurv aPC;
  Extrema_POnSurf aPS;
  EXPECT_THROW(aExt.Points(1, aPC, aPS), Standard_OutOfRange);
}

// The tighter bound must not refuse a result that does have points.
TEST(Extrema_ExtCS_Test, LineAboveSphereStillReportsItsPoints)
{
  occ::handle<Geom_Line>             aLine = new Geom_Line(gp_Pnt(0, 0, 40), gp_Dir(1, 0, 0));
  occ::handle<Geom_SphericalSurface> aSph =
    new Geom_SphericalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 3.0);

  GeomAdaptor_Curve   aC(aLine, -10.0, 10.0);
  GeomAdaptor_Surface aS(aSph, 0.0, 2.0 * M_PI, -M_PI_2, M_PI_2);

  Extrema_ExtCS aExt(aC, aS, 1.0e-6, 1.0e-6);
  ASSERT_TRUE(aExt.IsDone());
  ASSERT_FALSE(aExt.IsParallel());
  ASSERT_GT(aExt.NbExt(), 0);

  for (int anI = 1; anI <= aExt.NbExt(); ++anI)
  {
    Extrema_POnCurv aPC;
    Extrema_POnSurf aPS;
    ASSERT_NO_THROW(aExt.Points(anI, aPC, aPS));
    EXPECT_NEAR(aPC.Value().SquareDistance(aPS.Value()), aExt.SquareDistance(anI), 1.0e-5);
  }
}
