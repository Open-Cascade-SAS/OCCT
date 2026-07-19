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

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Line.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

//=================================================================================================

// Bug BUC60870: BRepExtrema_DistShapeShape should correctly compute the minimum distance
// between a linear edge and a vertex.
// The edge runs from (0,0,0) to (0,1,0); the vertex is at (0,0.3,1).
// The theoretical minimum distance is 1.0 (perpendicular from (0,0.3,1) to (0,0.3,0)).
TEST(BRepExtrema_DistShapeShapeTest, BUC60870_EdgeToVertexMinimumDistance)
{
  const TopoDS_Edge   anEdge  = BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(0, 1, 0)).Edge();
  const TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0.3, 1)).Vertex();

  BRepExtrema_DistShapeShape aDist(anEdge, aVertex, 2.0);

  ASSERT_TRUE(aDist.IsDone()) << "BRepExtrema_DistShapeShape failed to compute";
  ASSERT_GE(aDist.NbSolution(), 1) << "Expected at least one solution";

  const double aTheorDist  = 1.0;
  const double aPercentMax = 0.01; // 1% relative tolerance
  EXPECT_NEAR(aDist.Value(), aTheorDist, aTheorDist * aPercentMax)
    << "Minimum distance deviates from expected value 1.0";
}

//=================================================================================================

// Verify that BRepExtrema_DistShapeShape does not crash when computing the distance
// between two edges and one of them has no 3D curve (null Geom_Curve from BRep_Tool::Curve).
// This exercises the null-check in the PERFORM_C0 helper.
TEST(BRepExtrema_DistShapeShapeTest, EdgeEdge_Null3DCurve_NoCrash)
{
  const TopoDS_Edge aEdge1 = BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0)).Edge();

  const occ::handle<Geom_Circle> aCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(5, 0, 0), gp_Dir(0, 0, 1)), 1.0);
  TopoDS_Edge aEdge2 = BRepBuilderAPI_MakeEdge(aCircle).Edge();

  BRep_Builder aBuilder;
  aBuilder.UpdateEdge(aEdge2, occ::handle<Geom_Curve>(), 1e-7);

  BRepExtrema_DistShapeShape aDist(aEdge1, aEdge2, 10.0);
  (void)aDist;
}

//=================================================================================================

// Verify that BRepExtrema_DistShapeShape does not crash when computing the distance
// between an edge with no 3D curve and a face.
// This exercises the null-check in Perform(Edge, Face).
TEST(BRepExtrema_DistShapeShapeTest, EdgeFace_Null3DCurve_NoCrash)
{
  const occ::handle<Geom_Circle> aCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(5, 0, 0), gp_Dir(0, 0, 1)), 1.0);
  TopoDS_Edge aEdge = BRepBuilderAPI_MakeEdge(aCircle).Edge();

  BRep_Builder aBuilder;
  aBuilder.UpdateEdge(aEdge, occ::handle<Geom_Curve>(), 1e-7);

  const TopoDS_Face aFace =
    BRepBuilderAPI_MakeFace(gp_Pln(gp_Pnt(0, 0, 5), gp_Dir(0, 0, 1)), -10, 10, -10, 10);

  BRepExtrema_DistShapeShape aDist(aEdge, aFace, 10.0);
  (void)aDist;
}
