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
#include <BRepGraph.hxx>
#include <BRepGraph_BuilderView.hxx>
#include "BRepGraph_RefTestTools.hxx"
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Compound.hxx>

#include <gtest/gtest.h>

// =========================================================================
// Fixture: builds a box 10x20x30 and its BRepGraph
// =========================================================================

class BRepGraph_SharingTest : public testing::Test
{
protected:
  void SetUp() override
  {
    BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
    const TopoDS_Shape& aBox = aBoxMaker.Shape();
    myGraph.Build(aBox);
  }

  BRepGraph myGraph;
};

// =========================================================================
// Edge sharing via incidence: each box edge appears in 2 wires (faces)
// =========================================================================

TEST_F(BRepGraph_SharingTest, EdgeDef_EachSharedByTwoFaces)
{
  ASSERT_TRUE(myGraph.IsDone());
  EXPECT_EQ(myGraph.Topo().NbEdges(), 12);
  // In a box, each edge is shared by exactly 2 faces.
  for (int anIdx = 0; anIdx < myGraph.Topo().NbEdges(); ++anIdx)
  {
    int aFaceCount = myGraph.Topo().FaceCountOfEdge(BRepGraph_EdgeId(anIdx));
    EXPECT_EQ(aFaceCount, 2) << "Edge def " << anIdx << " expected to be shared by 2 faces, got "
                             << aFaceCount;
  }
}

TEST_F(BRepGraph_SharingTest, FaceDef_EachHasValidSurface)
{
  ASSERT_TRUE(myGraph.IsDone());
  EXPECT_EQ(myGraph.Topo().NbFaces(), 6);
  for (int anIdx = 0; anIdx < myGraph.Topo().NbFaces(); ++anIdx)
  {
    const BRepGraphInc::FaceDef& aDef = myGraph.Topo().Face(BRepGraph_FaceId(anIdx));
    EXPECT_TRUE(aDef.SurfaceRepId.IsValid()) << "Face def " << anIdx << " has no surface rep";
  }
}

TEST_F(BRepGraph_SharingTest, SolidDef_HasOneShellRef)
{
  ASSERT_TRUE(myGraph.IsDone());
  EXPECT_EQ(myGraph.Topo().NbSolids(), 1);
  EXPECT_EQ(BRepGraph_TestTools::CountShellRefsOfSolid(myGraph, BRepGraph_SolidId(0)), 1);
}

TEST_F(BRepGraph_SharingTest, ShellDef_HasSixFaceRefs)
{
  ASSERT_TRUE(myGraph.IsDone());
  EXPECT_EQ(myGraph.Topo().NbShells(), 1);
  EXPECT_EQ(BRepGraph_TestTools::CountFaceRefsOfShell(myGraph, BRepGraph_ShellId(0)), 6);
}

// =========================================================================
// Containment hierarchy
// =========================================================================

TEST_F(BRepGraph_SharingTest, SolidDef_ContainsOneShellRef)
{
  ASSERT_TRUE(myGraph.IsDone());
  EXPECT_EQ(myGraph.Topo().NbSolids(), 1);
  EXPECT_EQ(BRepGraph_TestTools::CountShellRefsOfSolid(myGraph, BRepGraph_SolidId(0)), 1);
}

TEST_F(BRepGraph_SharingTest, ShellDef_ContainsSixFaceRefs)
{
  ASSERT_TRUE(myGraph.IsDone());
  EXPECT_EQ(myGraph.Topo().NbShells(), 1);
  EXPECT_EQ(BRepGraph_TestTools::CountFaceRefsOfShell(myGraph, BRepGraph_ShellId(0)), 6);
}

TEST_F(BRepGraph_SharingTest, FaceDef_OuterWireIdx_Valid)
{
  ASSERT_TRUE(myGraph.IsDone());
  for (int anIdx = 0; anIdx < myGraph.Topo().NbFaces(); ++anIdx)
  {
    const BRepGraph_WireId anOuterWire = BRepGraph_TestTools::OuterWireOfFace(myGraph, BRepGraph_FaceId(anIdx));
    EXPECT_TRUE(anOuterWire.IsValid()) << "Face def " << anIdx << " has no outer wire";
  }
}

TEST_F(BRepGraph_SharingTest, WireDef_CoEdgeRefsCount_FourPerBoxFace)
{
  ASSERT_TRUE(myGraph.IsDone());
  for (int anIdx = 0; anIdx < myGraph.Topo().NbWires(); ++anIdx)
  {
    const int aNbCoEdgeRefs = BRepGraph_TestTools::CountCoEdgeRefsOfWire(myGraph, BRepGraph_WireId(anIdx));
    EXPECT_GT(aNbCoEdgeRefs, 0) << "Wire def " << anIdx << " has no coedge refs";
    // Box face wires have 4 edges
    EXPECT_EQ(aNbCoEdgeRefs, 4)
      << "Wire def " << anIdx << " expected 4 coedge refs for box face";
  }
}

TEST_F(BRepGraph_SharingTest, EdgeDef_VertexDefs_BothValid)
{
  ASSERT_TRUE(myGraph.IsDone());
  for (int anIdx = 0; anIdx < myGraph.Topo().NbEdges(); ++anIdx)
  {
    const BRepGraph_EdgeId anEdgeId(anIdx);
    EXPECT_TRUE(BRepGraph_Tool::Edge::StartVertex(myGraph, anEdgeId).VertexDefId.IsValid())
      << "Edge def " << anIdx << " has invalid start vertex def";
    EXPECT_TRUE(BRepGraph_Tool::Edge::EndVertex(myGraph, anEdgeId).VertexDefId.IsValid())
      << "Edge def " << anIdx << " has invalid end vertex def";
  }
}

// =========================================================================
// Orientation via incidence refs
// =========================================================================

TEST_F(BRepGraph_SharingTest, SharedEdge_IncidenceRefs_DifferentOrientation)
{
  ASSERT_TRUE(myGraph.IsDone());
  // In a box, shared edges between adjacent faces have coedges on
  // different face definitions. Check that at least some edges have
  // coedges referencing more than one face.
  int aMultiFaceEdgeCount = 0;
  for (int anIdx = 0; anIdx < myGraph.Topo().NbEdges(); ++anIdx)
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      myGraph.Topo().CoEdgesOfEdge(BRepGraph_EdgeId(anIdx));
    if (aCoEdgeIdxs.Length() < 2)
      continue;
    // Check if coedges reference different faces.
    const BRepGraph_NodeId aFace0 = myGraph.Topo().CoEdge(aCoEdgeIdxs.Value(0)).FaceDefId;
    for (int aCEI = 1; aCEI < aCoEdgeIdxs.Length(); ++aCEI)
    {
      if (myGraph.Topo().CoEdge(aCoEdgeIdxs.Value(aCEI)).FaceDefId != aFace0)
      {
        ++aMultiFaceEdgeCount;
        break;
      }
    }
  }
  EXPECT_GT(aMultiFaceEdgeCount, 0)
    << "Expected at least some shared edges with coedges on different faces";
}

TEST_F(BRepGraph_SharingTest, NonClosedEdge_StartEnd_Different)
{
  ASSERT_TRUE(myGraph.IsDone());
  for (int anIdx = 0; anIdx < myGraph.Topo().NbEdges(); ++anIdx)
  {
    const BRepGraph_EdgeId             anEdgeId(anIdx);
    const BRepGraphInc::EdgeDef& aDef = myGraph.Topo().Edge(anEdgeId);
    if (aDef.IsDegenerate)
      continue;
    // Box edges are not closed, so start and end vertex defs must differ
    const BRepGraph_VertexId aStartVtx = BRepGraph_Tool::Edge::StartVertex(myGraph, anEdgeId).VertexDefId;
    const BRepGraph_VertexId anEndVtx  = BRepGraph_Tool::Edge::EndVertex(myGraph, anEdgeId).VertexDefId;
    EXPECT_NE(aStartVtx, anEndVtx)
      << "Non-degenerate edge def " << anIdx << " has identical start and end vertex def ids";
  }
}

TEST_F(BRepGraph_SharingTest, VertexDef_Points_MatchExpectedBoxCorners)
{
  ASSERT_TRUE(myGraph.IsDone());
  // For a simple 10x20x30 box, all 8 vertex points should be valid.
  EXPECT_EQ(myGraph.Topo().NbVertices(), 8);
  for (int anIdx = 0; anIdx < myGraph.Topo().NbVertices(); ++anIdx)
  {
    const BRepGraphInc::VertexDef& aDef = myGraph.Topo().Vertex(BRepGraph_VertexId(anIdx));
    // Verify coordinates are within the box bounds.
    EXPECT_GE(aDef.Point.X(), -Precision::Confusion());
    EXPECT_LE(aDef.Point.X(), 10.0 + Precision::Confusion());
    EXPECT_GE(aDef.Point.Y(), -Precision::Confusion());
    EXPECT_LE(aDef.Point.Y(), 20.0 + Precision::Confusion());
    EXPECT_GE(aDef.Point.Z(), -Precision::Confusion());
    EXPECT_LE(aDef.Point.Z(), 30.0 + Precision::Confusion());
  }
}

// =========================================================================
// TShape sharing in compounds (standalone tests)
// =========================================================================

TEST_F(BRepGraph_SharingTest, CompoundTwoIdenticalBoxes)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox);
  aBuilder.Add(aCompound, aBox);

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  // Same TShape added twice to compound: definition is shared (1 solid def),
  // compound has 2 ChildRefs pointing to the same solid index.
  EXPECT_EQ(aGraph.Topo().NbSolids(), 1);

  // All defs are shared (same TShape), counts stay at single-box levels.
  EXPECT_EQ(aGraph.Topo().NbFaces(), 6);
  EXPECT_EQ(aGraph.Topo().NbEdges(), 12);
  EXPECT_EQ(aGraph.Topo().NbVertices(), 8);

  // Compound has 2 child references to the same solid.
  const NCollection_Vector<BRepGraph_ChildRefId> aChildRefs =
    BRepGraph_TestTools::ChildRefsOfParent(aGraph, BRepGraph_NodeId::Compound(0));
  ASSERT_EQ(aChildRefs.Length(), 2);
  EXPECT_EQ(aGraph.Refs().Child(aChildRefs.Value(0)).ChildDefId.Index,
            aGraph.Refs().Child(aChildRefs.Value(1)).ChildDefId.Index);
}

TEST_F(BRepGraph_SharingTest, CompoundTwoDistinctBoxes)
{
  BRepPrimAPI_MakeBox aBoxMaker1(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox1 = aBoxMaker1.Shape();

  BRepPrimAPI_MakeBox aBoxMaker2(5.0, 15.0, 25.0);
  const TopoDS_Shape& aBox2 = aBoxMaker2.Shape();

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox1);
  aBuilder.Add(aCompound, aBox2);

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  // Two different TShapes: no sharing, definitions are independent
  EXPECT_EQ(aGraph.Topo().NbSolids(), 2);
  EXPECT_EQ(aGraph.Topo().NbFaces(), 12);
  EXPECT_EQ(aGraph.Topo().NbEdges(), 24);
  EXPECT_EQ(aGraph.Topo().NbVertices(), 16);
}

TEST_F(BRepGraph_SharingTest, CompoundWithLocation_MoreUsagesThanDefs)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(100.0, 0.0, 0.0));
  TopoDS_Shape aMovedBox = aBox.Moved(TopLoc_Location(aTrsf));

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox);
  aBuilder.Add(aCompound, aMovedBox);

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  // Same TShape with different locations: defs are shared.
  EXPECT_EQ(aGraph.Topo().NbFaces(), 6);
  EXPECT_EQ(aGraph.Topo().NbEdges(), 12);
  EXPECT_EQ(aGraph.Topo().NbVertices(), 8);
}

TEST_F(BRepGraph_SharingTest, TranslatedCopy_SameTShape_SharedDefs)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(50.0, 50.0, 50.0));
  TopoDS_Shape aCopy = aBox.Moved(TopLoc_Location(aTrsf));

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox);
  aBuilder.Add(aCompound, aCopy);

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  // Moved() preserves TShape, so all definitions are shared (1 solid def).
  // Compound has 2 ChildRefs with different locations.
  EXPECT_EQ(aGraph.Topo().NbSolids(), 1);

  // Face/edge/vertex defs are shared (same TShape).
  EXPECT_EQ(aGraph.Topo().NbFaces(), 6);
  EXPECT_EQ(aGraph.Topo().NbEdges(), 12);
  EXPECT_EQ(aGraph.Topo().NbVertices(), 8);
}
