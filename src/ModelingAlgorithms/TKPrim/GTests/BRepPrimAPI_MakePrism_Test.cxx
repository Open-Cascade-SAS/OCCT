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

#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRep_Tool.hxx>
#include <Geom_BezierCurve.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

#include <gtest/gtest.h>

#include <cmath>

TEST(BRepPrimAPI_MakePrism_Test, OCC31294_GeneratedListForNonBaseShape)
{
  // Bug OCC31294: Modeling Algorithms - Regression relatively 7.3.0.
  // Crash in method BRepPrimAPI_MakePrism::Generated(...) if input sub-shape
  // does not belong to the base shape
  //
  // This test verifies that calling Generated() with a shape that doesn't belong
  // to the base shape doesn't crash and returns an empty list

  BRepBuilderAPI_MakeVertex aMkVert(gp_Pnt(0., 0., 0.));
  BRepBuilderAPI_MakeVertex aMkDummy(gp_Pnt(0., 0., 0.));
  BRepPrimAPI_MakePrism     aMkPrism(aMkVert.Shape(), gp_Vec(0., 0., 1.));

  // Check that Generated() returns 1 shape for the vertex used to create the prism
  int aNbGen = aMkPrism.Generated(aMkVert.Shape()).Extent();
  EXPECT_EQ(aNbGen, 1);

  // Check that Generated() returns 0 shapes for a vertex not used to create the prism
  // (this should not crash)
  int aNbDummy = aMkPrism.Generated(aMkDummy.Shape()).Extent();
  EXPECT_EQ(aNbDummy, 0);
}

TEST(BRepPrimAPI_MakePrism_Test, RegularityPreservesShallowCorners)
{
  for (double anAngle : {0.0, 1.e-9, 0.001, 0.002})
  {
    SCOPED_TRACE(anAngle);
    const TopoDS_Vertex aJoin  = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));
    const TopoDS_Vertex aStart = BRepBuilderAPI_MakeVertex(gp_Pnt(-1, 0, 0));
    const TopoDS_Vertex anEnd =
      BRepBuilderAPI_MakeVertex(gp_Pnt(std::cos(anAngle), std::sin(anAngle), 0));
    const TopoDS_Edge     aFirst  = BRepBuilderAPI_MakeEdge(aStart, aJoin);
    const TopoDS_Edge     aSecond = BRepBuilderAPI_MakeEdge(aJoin, anEnd);
    BRepPrimAPI_MakePrism aPrism(BRepBuilderAPI_MakeWire(aFirst, aSecond).Wire(), gp_Vec(0, 0, 2));
    ASSERT_TRUE(aPrism.IsDone());
    const TopoDS_Edge anEdge = TopoDS::Edge(aPrism.Generated(aJoin).First());
    const TopoDS_Face aFace1 = TopoDS::Face(aPrism.Generated(aFirst).First());
    const TopoDS_Face aFace2 = TopoDS::Face(aPrism.Generated(aSecond).First());
    EXPECT_EQ(BRep_Tool::Continuity(anEdge, aFace1, aFace2) >= GeomAbs_G1, anAngle == 0.0);
  }
}

TEST(BRepPrimAPI_MakePrism_Test, ClosedCurveRegularityPreservesShallowCorners)
{
  for (double anAngle : {0.0, 1.e-9, 0.001, 0.002})
  {
    SCOPED_TRACE(anAngle);
    // The closing tangents are (1, 0, 0) and (cos(angle), sin(angle), 0).
    NCollection_Array1<gp_Pnt> aPoles(1, 5);
    aPoles(1) = aPoles(5)                = gp_Pnt(0, 0, 0);
    aPoles(2)                            = gp_Pnt(1, 0, 0);
    aPoles(3)                            = gp_Pnt(0, 2, 0);
    aPoles(4)                            = gp_Pnt(-std::cos(anAngle), -std::sin(anAngle), 0);
    const occ::handle<Geom_Curve> aCurve = new Geom_BezierCurve(aPoles);
    const TopoDS_Vertex           aJoin  = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0));
    const TopoDS_Edge     aProfile       = BRepBuilderAPI_MakeEdge(aCurve, aJoin, aJoin, 0.0, 1.0);
    BRepPrimAPI_MakePrism aPrism(aProfile, gp_Vec(0, 0, 2));
    ASSERT_TRUE(aPrism.IsDone());
    const TopoDS_Edge anEdge = TopoDS::Edge(aPrism.Generated(aJoin).First());
    const TopoDS_Face aFace  = TopoDS::Face(aPrism.Generated(aProfile).First());
    ASSERT_TRUE(BRep_Tool::IsClosed(anEdge, aFace));
    EXPECT_EQ(BRep_Tool::Continuity(anEdge, aFace, aFace) >= GeomAbs_G1, anAngle == 0.0);
  }
}
