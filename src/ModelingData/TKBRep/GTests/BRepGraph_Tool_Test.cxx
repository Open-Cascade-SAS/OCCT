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

#include <BRepGraph.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_Builder.hxx>
#include <BRepGraph_TopoView.hxx>

#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

// ---------- New ownership and lookup helpers (query-surface-expansion) ----------

class BRepGraph_QuerySurfaceTest : public testing::Test
{
protected:
  void SetUp() override
  {
    BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
    BRepGraph_Builder::Perform(myBoxGraph, aBoxMaker.Shape());

    BRepPrimAPI_MakeCylinder aCylMaker(5.0, 15.0);
    BRepGraph_Builder::Perform(myCylGraph, aCylMaker.Shape());
  }

  BRepGraph myBoxGraph;
  BRepGraph myCylGraph;
};

TEST_F(BRepGraph_QuerySurfaceTest, Face_NbWires_BoxFaceHasOneWire)
{
  const int aNbFaces = myBoxGraph.Topo().Faces().Nb();
  for (BRepGraph_FaceId aFaceId(0); aFaceId.IsValid(aNbFaces); ++aFaceId)
  {
    const int aNb = BRepGraph_Tool::Face::NbWires(myBoxGraph, aFaceId);
    EXPECT_EQ(aNb, 1) << "Box face " << aFaceId.Index << " should have exactly 1 wire";
  }
}

TEST_F(BRepGraph_QuerySurfaceTest, Face_Bounds_BoxFaceHasFiniteBounds)
{
  const BRepGraph_FaceId aFaceId(0);
  double                 uMin = 0.0, uMax = 0.0, vMin = 0.0, vMax = 0.0;
  BRepGraph_Tool::Face::Bounds(myBoxGraph, aFaceId, uMin, uMax, vMin, vMax);

  EXPECT_LT(uMin, uMax) << "UMin should be less than UMax for a valid face";
  EXPECT_LT(vMin, vMax) << "VMin should be less than VMax for a valid face";
  EXPECT_LT(uMin, Precision::Infinite());
  EXPECT_LT(vMin, Precision::Infinite());
}

TEST_F(BRepGraph_QuerySurfaceTest, Face_Bounds_CylinderFaceReturnsSurfaceBounds)
{
  // Verify that Bounds() returns the same values as Surface()->Bounds() for each face.
  const int aNbFaces = myCylGraph.Topo().Faces().Nb();
  for (BRepGraph_FaceId aFaceId(0); aFaceId.IsValid(aNbFaces); ++aFaceId)
  {
    if (!BRepGraph_Tool::Face::HasSurface(myCylGraph, aFaceId))
      continue;

    double uMin = 0.0, uMax = 0.0, vMin = 0.0, vMax = 0.0;
    BRepGraph_Tool::Face::Bounds(myCylGraph, aFaceId, uMin, uMax, vMin, vMax);

    double expUMin = 0.0, expUMax = 0.0, expVMin = 0.0, expVMax = 0.0;
    BRepGraph_Tool::Face::Surface(myCylGraph, aFaceId)->Bounds(expUMin, expUMax, expVMin, expVMax);

    EXPECT_EQ(uMin, expUMin) << "UMin mismatch on face " << aFaceId.Index;
    EXPECT_EQ(uMax, expUMax) << "UMax mismatch on face " << aFaceId.Index;
    EXPECT_EQ(vMin, expVMin) << "VMin mismatch on face " << aFaceId.Index;
    EXPECT_EQ(vMax, expVMax) << "VMax mismatch on face " << aFaceId.Index;
  }
}

TEST_F(BRepGraph_QuerySurfaceTest, Wire_FaceOf_ReturnsValidFace)
{
  const BRepGraph_WireId aWireId(0);
  const BRepGraph_FaceId aFace = BRepGraph_Tool::Wire::FaceOf(myBoxGraph, aWireId);
  EXPECT_TRUE(aFace.IsValid()) << "Wire 0 should have an owning face";
}

TEST_F(BRepGraph_QuerySurfaceTest, Wire_IsOuter_FirstWireOfBoxFaceIsOuter)
{
  const BRepGraph_FaceId aFaceId(0);
  const BRepGraph_WireId anOuterWire = BRepGraph_Tool::Face::OuterWireId(myBoxGraph, aFaceId);
  ASSERT_TRUE(anOuterWire.IsValid());
  EXPECT_TRUE(BRepGraph_Tool::Wire::IsOuter(myBoxGraph, anOuterWire))
    << "OuterWireId-found wire should be flagged IsOuter";
}

TEST_F(BRepGraph_QuerySurfaceTest, Edge_NbFaces_BoxEdgeHasExactlyTwoFaces)
{
  const int aNbEdges = myBoxGraph.Topo().Edges().Nb();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const int aNbFaces = BRepGraph_Tool::Edge::NbFaces(myBoxGraph, anEdgeId);
    EXPECT_EQ(aNbFaces, 2) << "Box edge " << anEdgeId.Index
                           << " should be shared by exactly 2 faces";
  }
}

TEST_F(BRepGraph_QuerySurfaceTest, Edge_IsManifold_BoxEdgesAreManifold)
{
  const int aNbEdges = myBoxGraph.Topo().Edges().Nb();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    EXPECT_TRUE(BRepGraph_Tool::Edge::IsManifold(myBoxGraph, anEdgeId))
      << "Box edge " << anEdgeId.Index << " should be manifold";
    EXPECT_FALSE(BRepGraph_Tool::Edge::IsBoundary(myBoxGraph, anEdgeId))
      << "Box edge " << anEdgeId.Index << " should not be a boundary-only edge";
  }
}

TEST_F(BRepGraph_QuerySurfaceTest, Vertex_NbEdges_BoxVertexHasThreeEdges)
{
  const int aNbVertices = myBoxGraph.Topo().Vertices().Nb();
  for (BRepGraph_VertexId aVertexId(0); aVertexId.IsValid(aNbVertices); ++aVertexId)
  {
    const int aNbEdges = BRepGraph_Tool::Vertex::NbEdges(myBoxGraph, aVertexId);
    EXPECT_EQ(aNbEdges, 3) << "Box vertex " << aVertexId.Index << " should be incident to 3 edges";
  }
}

TEST_F(BRepGraph_QuerySurfaceTest, Shell_IsClosed_BoxShellIsClosed)
{
  ASSERT_GE(myBoxGraph.Topo().Shells().Nb(), 1);
  EXPECT_TRUE(BRepGraph_Tool::Shell::IsClosed(myBoxGraph, BRepGraph_ShellId::Start()))
    << "Box shell should be closed";
}

TEST_F(BRepGraph_QuerySurfaceTest, Shell_NbFaces_BoxShellHasSixFaces)
{
  ASSERT_GE(myBoxGraph.Topo().Shells().Nb(), 1);
  EXPECT_EQ(BRepGraph_Tool::Shell::NbFaces(myBoxGraph, BRepGraph_ShellId::Start()), 6)
    << "Box shell should reference 6 faces";
}
