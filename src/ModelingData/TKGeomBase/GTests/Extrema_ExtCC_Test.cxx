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

#include <Extrema_ExtCC.hxx>
#include <Extrema_POnCurv.hxx>
#include <GC_MakeSegment.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Standard_OutOfRange.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

namespace
{
GeomAdaptor_Curve segment(const gp_Pnt& theP1, const gp_Pnt& theP2)
{
  return GeomAdaptor_Curve(GC_MakeSegment(theP1, theP2).Value());
}
} // namespace

// Regression test: NbExt() counts mySqDist, but Points() indexed a different
// container (mypoints), which several parallel-curve branches leave shorter. Two parallel
// segments whose projected ranges overlap have a well-defined distance (NbExt() == 1) but no
// unique closest point pair, so Points(1) used to index past mypoints's actual length.
TEST(Extrema_ExtCCTest, Points_ParallelOverlapping_ThrowsInsteadOfCrashing)
{
  GeomAdaptor_Curve aC1 = segment(gp_Pnt(0, 0, 0), gp_Pnt(10, 0, 0));
  GeomAdaptor_Curve aC2 = segment(gp_Pnt(3, 1, 0), gp_Pnt(13, 1, 0));

  Extrema_ExtCC anExt(aC1, aC2);
  ASSERT_TRUE(anExt.IsDone());
  EXPECT_TRUE(anExt.IsParallel());
  ASSERT_EQ(anExt.NbExt(), 1);
  EXPECT_NEAR(anExt.SquareDistance(1), 1.0, 1.e-9);

  Extrema_POnCurv aP1, aP2;
  EXPECT_THROW(anExt.Points(1, aP1, aP2), Standard_OutOfRange);
}

// Same shape as above, on two unbounded parallel Geom_Line curves via GeomAdaptor_Curve.
TEST(Extrema_ExtCCTest, Points_ParallelUnbounded_ThrowsInsteadOfCrashing)
{
  occ::handle<Geom_Line> aLine1 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  occ::handle<Geom_Line> aLine2 = new Geom_Line(gp_Pnt(0, 5, 0), gp_Dir(1, 0, 0));
  GeomAdaptor_Curve      aC1(aLine1);
  GeomAdaptor_Curve      aC2(aLine2);

  Extrema_ExtCC anExt(aC1, aC2);
  ASSERT_TRUE(anExt.IsDone());
  EXPECT_TRUE(anExt.IsParallel());
  ASSERT_EQ(anExt.NbExt(), 1);

  Extrema_POnCurv aP1, aP2;
  EXPECT_THROW(anExt.Points(1, aP1, aP2), Standard_OutOfRange);
}

// Parallel segments whose projected ranges are disjoint: a real, unique closest pair exists, and
// this fix must not touch it (mypoints does have a matching entry here).
TEST(Extrema_ExtCCTest, Points_ParallelDisjoint_ReturnsRealPair)
{
  GeomAdaptor_Curve aC1 = segment(gp_Pnt(0, 0, 0), gp_Pnt(5, 0, 0));
  GeomAdaptor_Curve aC2 = segment(gp_Pnt(20, 3, 0), gp_Pnt(25, 3, 0));

  Extrema_ExtCC anExt(aC1, aC2);
  ASSERT_TRUE(anExt.IsDone());
  ASSERT_EQ(anExt.NbExt(), 1);

  Extrema_POnCurv aP1, aP2;
  EXPECT_NO_THROW(anExt.Points(1, aP1, aP2));
  EXPECT_NEAR(aP1.Value().Distance(aP2.Value()), std::sqrt(anExt.SquareDistance(1)), 1.e-9);
}

// Two non-parallel, intersecting lines: ordinary case, unaffected by this fix.
TEST(Extrema_ExtCCTest, Points_Intersecting_ReturnsRealPair)
{
  GeomAdaptor_Curve aC1 = segment(gp_Pnt(0, 0, 0), gp_Pnt(10, 0, 0));
  GeomAdaptor_Curve aC2 = segment(gp_Pnt(5, -5, 0), gp_Pnt(5, 5, 0));

  Extrema_ExtCC anExt(aC1, aC2);
  ASSERT_TRUE(anExt.IsDone());
  EXPECT_FALSE(anExt.IsParallel());
  ASSERT_EQ(anExt.NbExt(), 1);

  Extrema_POnCurv aP1, aP2;
  EXPECT_NO_THROW(anExt.Points(1, aP1, aP2));
  EXPECT_NEAR(anExt.SquareDistance(1), 0.0, 1.e-9);
}
