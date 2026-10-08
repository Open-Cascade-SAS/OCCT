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

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepFill_TrimEdgeTool.hxx>
#include <Bisector_Bisec.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_Plane.hxx>
#include <Precision.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <gp.hxx>

#include <gtest/gtest.h>

TEST(BRepFill_TrimEdgeToolTest, UnequalIntersectionCountsPreserveCommonRoot)
{
  const occ::handle<Geom2d_CartesianPoint> aLeft  = new Geom2d_CartesianPoint(-1.0, 0.0);
  const occ::handle<Geom2d_CartesianPoint> aRight = new Geom2d_CartesianPoint(1.0, 0.0);
  Bisector_Bisec                           aBisector;
  aBisector
    .Perform(aLeft, aRight, gp_Pnt2d(0.0, 1.0), gp_Vec2d(-1.0, 0.0), gp_Vec2d(1.0, 0.0), 1.0);
  const occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp::XOY());
  for (double aRoot : {0.001, 0.2})
  {
    SCOPED_TRACE(aRoot);
    NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
    aPoles.ChangeAt(0)                        = gp_Pnt2d(0.1 * (1.0 - aRoot * aRoot), -1.0);
    aPoles.ChangeAt(1)                        = gp_Pnt2d(0.1 * (-1.0 - aRoot * aRoot), 0.0);
    aPoles.ChangeAt(2)                        = gp_Pnt2d(0.1 * (1.0 - aRoot * aRoot), 1.0);
    const occ::handle<Geom2d_Curve> aParabola = new Geom2d_BezierCurve(aPoles);
    const TopoDS_Edge aParabolicEdge = BRepBuilderAPI_MakeEdge(aParabola, aPlane, 0.0, 1.0);
    for (double aSign : {-1.0, 1.0})
    {
      SCOPED_TRACE(aSign);
      const gp_Pnt2d                  aCommonPoint(0.0, aSign * aRoot);
      const occ::handle<Geom2d_Curve> aLine = new Geom2d_Line(aCommonPoint, gp_Dir2d(1.0, 0.0));
      const TopoDS_Edge aLinearEdge         = BRepBuilderAPI_MakeEdge(aLine, aPlane, -1.0, 1.0);
      for (bool isReversed : {false, true})
      {
        SCOPED_TRACE(isReversed);
        const TopoDS_Edge&               aFirstEdge   = isReversed ? aLinearEdge : aParabolicEdge;
        const TopoDS_Edge&               aSecondEdge  = isReversed ? aParabolicEdge : aLinearEdge;
        const occ::handle<Geom2d_Curve>& aFirstCurve  = isReversed ? aLine : aParabola;
        const occ::handle<Geom2d_Curve>& aSecondCurve = isReversed ? aParabola : aLine;
        BRepFill_TrimEdgeTool            aTrimmer(aBisector, aLeft, aRight, 1.0);
        NCollection_Sequence<gp_Pnt>     aParameters;
        aTrimmer.IntersectWith(aFirstEdge,
                               aSecondEdge,
                               aFirstEdge,
                               aSecondEdge,
                               TopoDS_Vertex(),
                               TopoDS_Vertex(),
                               GeomAbs_Arc,
                               false,
                               aParameters);
        ASSERT_EQ(aParameters.Length(), 1);
        const gp_Pnt& aParameter = aParameters.First();
        EXPECT_LE(aFirstCurve->Value(aParameter.Y()).Distance(aCommonPoint),
                  Precision::Confusion());
        EXPECT_LE(aSecondCurve->Value(aParameter.Z()).Distance(aCommonPoint),
                  Precision::Confusion());
        EXPECT_LE(aBisector.Value()->Value(aParameter.X()).Distance(aCommonPoint),
                  Precision::Confusion());
      }
    }
  }
}

TEST(BRepFill_TrimEdgeToolTest, UnequalIntersectionCountsPreserveAllCommonRoots)
{
  const occ::handle<Geom2d_CartesianPoint> aLeft  = new Geom2d_CartesianPoint(-1.0, 0.0);
  const occ::handle<Geom2d_CartesianPoint> aRight = new Geom2d_CartesianPoint(1.0, 0.0);
  Bisector_Bisec                           aBisector;
  aBisector
    .Perform(aLeft, aRight, gp_Pnt2d(0.0, 1.0), gp_Vec2d(-1.0, 0.0), gp_Vec2d(1.0, 0.0), 1.0);
  constexpr double             aRoot = 0.4;
  NCollection_Array1<gp_Pnt2d> aCubicPoles(1, 4);
  aCubicPoles.ChangeAt(0) = gp_Pnt2d(-1.0 + aRoot * aRoot, -1.0);
  aCubicPoles.ChangeAt(1) = gp_Pnt2d(1.0 + aRoot * aRoot / 3.0, -1.0 / 3.0);
  aCubicPoles.ChangeAt(2) = gp_Pnt2d(-1.0 - aRoot * aRoot / 3.0, 1.0 / 3.0);
  aCubicPoles.ChangeAt(3) = gp_Pnt2d(1.0 - aRoot * aRoot, 1.0);
  NCollection_Array1<gp_Pnt2d> aQuadraticPoles(1, 3);
  aQuadraticPoles.ChangeAt(0)                = gp_Pnt2d(1.0 - aRoot * aRoot, -1.0);
  aQuadraticPoles.ChangeAt(1)                = gp_Pnt2d(-1.0 - aRoot * aRoot, 0.0);
  aQuadraticPoles.ChangeAt(2)                = gp_Pnt2d(1.0 - aRoot * aRoot, 1.0);
  const occ::handle<Geom2d_Curve> aCubic     = new Geom2d_BezierCurve(aCubicPoles);
  const occ::handle<Geom2d_Curve> aQuadratic = new Geom2d_BezierCurve(aQuadraticPoles);
  const occ::handle<Geom_Plane>   aPlane     = new Geom_Plane(gp::XOY());
  const TopoDS_Edge               aCubicEdge = BRepBuilderAPI_MakeEdge(aCubic, aPlane, 0.0, 1.0);
  const TopoDS_Edge     aQuadraticEdge = BRepBuilderAPI_MakeEdge(aQuadratic, aPlane, 0.0, 1.0);
  BRepFill_TrimEdgeTool aTrimmer(aBisector, aLeft, aRight, 1.0);
  NCollection_Sequence<gp_Pnt> aParameters;
  aTrimmer.IntersectWith(aCubicEdge,
                         aQuadraticEdge,
                         aCubicEdge,
                         aQuadraticEdge,
                         TopoDS_Vertex(),
                         TopoDS_Vertex(),
                         GeomAbs_Arc,
                         false,
                         aParameters);
  ASSERT_EQ(aParameters.Length(), 2);
  for (int anIndex = 1; anIndex <= 2; ++anIndex)
  {
    const gp_Pnt2d aCommonPoint(0.0, anIndex == 1 ? aRoot : -aRoot);
    const gp_Pnt&  aParameter = aParameters(anIndex);
    EXPECT_LE(aCubic->Value(aParameter.Y()).Distance(aCommonPoint), Precision::Confusion());
    EXPECT_LE(aQuadratic->Value(aParameter.Z()).Distance(aCommonPoint), Precision::Confusion());
    EXPECT_LE(aBisector.Value()->Value(aParameter.X()).Distance(aCommonPoint),
              Precision::Confusion());
  }
}
