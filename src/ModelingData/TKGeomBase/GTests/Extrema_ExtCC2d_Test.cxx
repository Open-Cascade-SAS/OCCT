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

#include <Extrema_ExtCC2d.hxx>
#include <Extrema_POnCurv2d.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dAPI_ExtremaCurveCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>

#include <gtest/gtest.h>

namespace
{
occ::handle<Geom2d_TrimmedCurve> segment2d(const gp_Pnt2d& theP1, const gp_Pnt2d& theP2)
{
  gp_Dir2d                 aDir(theP2.XY() - theP1.XY());
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(theP1, aDir);
  return new Geom2d_TrimmedCurve(aLine, 0, theP1.Distance(theP2));
}
} // namespace

// Companion to the Extrema_ExtCC fix above: Extrema_ExtCC2d::Points() is rewritten to bound
// against mypoints directly, matching the 3D sibling's style, for consistency between the two
// classes. Unlike Extrema_ExtCC, mynbext and mypoints.Length()/2 are an invariant here (both
// Results() overloads only ever touch them together), so this is a no-op: NbExt() already
// reports 0 in every case that would otherwise leave mypoints short.
TEST(Extrema_ExtCC2dTest, Points_ParallelOverlapping_NbExtIsZero)
{
  Geom2dAdaptor_Curve aC1(segment2d(gp_Pnt2d(0, 0), gp_Pnt2d(10, 0)));
  Geom2dAdaptor_Curve aC2(segment2d(gp_Pnt2d(3, 1), gp_Pnt2d(13, 1)));

  Extrema_ExtCC2d anExt(aC1, aC2);
  ASSERT_TRUE(anExt.IsDone());
  EXPECT_TRUE(anExt.IsParallel());
  EXPECT_EQ(anExt.NbExt(), 0);
}

TEST(Extrema_ExtCC2dTest, Points_Intersecting_ReturnsRealPair)
{
  Geom2dAdaptor_Curve aC1(segment2d(gp_Pnt2d(0, 0), gp_Pnt2d(10, 0)));
  Geom2dAdaptor_Curve aC2(segment2d(gp_Pnt2d(5, -5), gp_Pnt2d(5, 5)));

  Extrema_ExtCC2d anExt(aC1, aC2);
  ASSERT_TRUE(anExt.IsDone());
  EXPECT_FALSE(anExt.IsParallel());
  ASSERT_EQ(anExt.NbExt(), 1);

  Extrema_POnCurv2d aP1, aP2;
  EXPECT_NO_THROW(anExt.Points(1, aP1, aP2));
}

// Geom2dAPI_ExtremaCurveCurve::IsParallel() (new, matches the 3D GeomAPI_ExtremaCurveCurve's
// existing convenience) forwards to the already-public Extrema_ExtCC2d::IsParallel().
TEST(Geom2dAPI_ExtremaCurveCurveTest, IsParallel_ForwardsToExtCC2d)
{
  occ::handle<Geom2d_TrimmedCurve> aC1 = segment2d(gp_Pnt2d(0, 0), gp_Pnt2d(10, 0));
  occ::handle<Geom2d_TrimmedCurve> aC2 = segment2d(gp_Pnt2d(3, 1), gp_Pnt2d(13, 1));

  Geom2dAPI_ExtremaCurveCurve anExt(aC1,
                                    aC2,
                                    aC1->FirstParameter(),
                                    aC1->LastParameter(),
                                    aC2->FirstParameter(),
                                    aC2->LastParameter());
  EXPECT_TRUE(anExt.IsParallel());

  occ::handle<Geom2d_TrimmedCurve> aC3 = segment2d(gp_Pnt2d(5, -5), gp_Pnt2d(5, 5));
  Geom2dAPI_ExtremaCurveCurve      aNonParallel(aC1,
                                           aC3,
                                           aC1->FirstParameter(),
                                           aC1->LastParameter(),
                                           aC3->FirstParameter(),
                                           aC3->LastParameter());
  EXPECT_FALSE(aNonParallel.IsParallel());
}
