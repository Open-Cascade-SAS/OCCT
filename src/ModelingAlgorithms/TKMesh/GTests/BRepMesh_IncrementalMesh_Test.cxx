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

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

#include <gtest/gtest.h>

// Test OCC26407: BRepMesh_Delaun must not fail on a planar polygon with frontier edges.
// The key check is that GetStatusFlags() == 0 (success) after meshing.
// Migrated from QABugs_19.cxx OCC26407
TEST(BRepMesh_IncrementalMeshTest, OCC26407_PlanarPolygonMeshStatus)
{
  // Hardcoded octagon-like polygon lying in the Z=88.5 plane
  std::vector<gp_Pnt> aPnts = {
    gp_Pnt(587.90000000000009094947, 40.6758179230516248026106, 88.5),
    gp_Pnt(807.824182076948432040808, 260.599999999999965893949, 88.5),
    gp_Pnt(644.174182076948454778176, 424.249999999999943156581, 88.5000000000000142108547),
    gp_Pnt(629.978025792618950617907, 424.25, 88.5),
    gp_Pnt(793.628025792618700506864, 260.599999999999852207111, 88.5),
    gp_Pnt(587.900000000000204636308, 54.8719742073813492311274, 88.5),
    gp_Pnt(218.521974207381418864315, 424.250000000000056843419, 88.5),
    gp_Pnt(204.325817923051886282337, 424.249999999999943156581, 88.5)};

  std::vector<TopoDS_Vertex> aVertices;
  aVertices.reserve(aPnts.size());
  for (const gp_Pnt& aPnt : aPnts)
  {
    aVertices.push_back(BRepBuilderAPI_MakeVertex(aPnt));
  }

  BRepBuilderAPI_MakeWire aWireBuilder;
  for (size_t i = 0; i < aVertices.size(); ++i)
  {
    const TopoDS_Vertex& aV = aVertices[i];
    const TopoDS_Vertex& aW = aVertices[(i + 1) % aVertices.size()];
    aWireBuilder.Add(BRepBuilderAPI_MakeEdge(aV, aW));
  }
  ASSERT_TRUE(aWireBuilder.IsDone()) << "Wire construction failed";

  const gp_Pnt& aV0         = aPnts[0];
  const gp_Pnt& aV1         = aPnts[1];
  const gp_Pnt& aV2         = aPnts[aPnts.size() - 1];
  const gp_Vec  aFaceNormal = gp_Vec(aV0, aV1).Crossed(gp_Vec(aV0, aV2));

  const TopoDS_Face aFace = BRepBuilderAPI_MakeFace(gp_Pln(aV0, aFaceNormal), aWireBuilder.Wire());

  BRepMesh_IncrementalMesh aMesher(aFace, 1.e-7);
  EXPECT_EQ(aMesher.GetStatusFlags(), 0)
    << "Meshing of the planar polygon face should succeed (status 0)";
}
