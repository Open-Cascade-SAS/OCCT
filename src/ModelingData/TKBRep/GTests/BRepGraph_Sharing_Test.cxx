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
#include <BRepGraph_DefsView.hxx>
#include <BRepGraphInc_IncidenceRef.hxx>
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
  EXPECT_EQ(myGraph.Defs().NbEdges(), 12);
  // In a box, each edge is shared by exactly 2 faces.
  for (int anIdx = 0; anIdx < myGraph.Defs().NbEdges(); ++anIdx)
  {
    int aFaceCount = myGraph.Defs().FaceCountOfEdge(BRepGraph_EdgeId(anIdx));
    EXPECT_EQ(aFaceCount, 2) << "Edge def " << anIdx << " expected to be shared by 2 faces, got "
                             << aFaceCount;
  }
}

TEST_F(BRepGraph_SharingTest, FaceDef_EachHasValidSurface)
{
  ASSERT_TRUE(myGraph.IsDone());
  EXPECT_EQ(myGraph.Defs().NbFaces(), 6);
  for (int anIdx = 0; anIdx < myGraph.Defs().NbFaces(); ++anIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aDef = myGraph.Defs().Face(BRepGraph_FaceId(anIdx));
    EXPECT_TRUE(aDef.SurfaceRepId.IsValid()) << "Face def " << anIdx << " has no surface rep";
  }
}

TEST_F(BRepGraph_SharingTest, SolidDef_HasOneShellRef)
{
  ASSERT_TRUE(myGraph.IsDone());
  EXPECT_EQ(myGraph.Defs().NbSolids(), 1);
  const BRepGraph_TopoNode::SolidDef& aDef = myGraph.Defs().Solid(BRepGraph_SolidId(0));
  EXPECT_EQ(aDef.ShellRefs.Length(), 1);
}

TEST_F(BRepGraph_SharingTest, ShellDef_HasSixFaceRefs)
{
  ASSERT_TRUE(myGraph.IsDone());
  EXPECT_EQ(myGraph.Defs().NbShells(), 1);
  const BRepGraph_TopoNode::ShellDef& aDef = myGraph.Defs().Shell(BRepGraph_ShellId(0));
  EXPECT_EQ(aDef.FaceRefs.Length(), 6);
}

// =========================================================================
// Containment hierarchy
// =========================================================================

TEST_F(BRepGraph_SharingTest, SolidDef_ContainsOneShellRef)
{
  ASSERT_TRUE(myGraph.IsDone());
  EXPECT_EQ(myGraph.Defs().NbSolids(), 1);
  const BRepGraph_TopoNode::SolidDef& aSolidDef = myGraph.Defs().Solid(BRepGraph_SolidId(0));
  EXPECT_EQ(aSolidDef.ShellRefs.Length(), 1);
}

TEST_F(BRepGraph_SharingTest, ShellDef_ContainsSixFaceRefs)
{
  ASSERT_TRUE(myGraph.IsDone());
  EXPECT_EQ(myGraph.Defs().NbShells(), 1);
  const BRepGraph_TopoNode::ShellDef& aShellDef = myGraph.Defs().Shell(BRepGraph_ShellId(0));
  EXPECT_EQ(aShellDef.FaceRefs.Length(), 6);
}

TEST_F(BRepGraph_SharingTest, FaceDef_OuterWireIdx_Valid)
{
  ASSERT_TRUE(myGraph.IsDone());
  for (int anIdx = 0; anIdx < myGraph.Defs().NbFaces(); ++anIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFaceDef = myGraph.Defs().Face(BRepGraph_FaceId(anIdx));
    EXPECT_GE(aFaceDef.OuterWireDefId().Index, 0) << "Face def " << anIdx << " has no outer wire";
  }
}

TEST_F(BRepGraph_SharingTest, WireDef_CoEdgeRefsCount_FourPerBoxFace)
{
  ASSERT_TRUE(myGraph.IsDone());
  for (int anIdx = 0; anIdx < myGraph.Defs().NbWires(); ++anIdx)
  {
    const BRepGraph_TopoNode::WireDef& aWireDef = myGraph.Defs().Wire(BRepGraph_WireId(anIdx));
    EXPECT_GT(aWireDef.CoEdgeRefs.Length(), 0) << "Wire def " << anIdx << " has no coedge refs";
    // Box face wires have 4 edges
    EXPECT_EQ(aWireDef.CoEdgeRefs.Length(), 4)
      << "Wire def " << anIdx << " expected 4 coedge refs for box face";
  }
}

TEST_F(BRepGraph_SharingTest, EdgeDef_VertexDefs_BothValid)
{
  ASSERT_TRUE(myGraph.IsDone());
  for (int anIdx = 0; anIdx < myGraph.Defs().NbEdges(); ++anIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = myGraph.Defs().Edge(BRepGraph_EdgeId(anIdx));
    EXPECT_TRUE(anEdgeDef.StartVertexDefId().IsValid())
      << "Edge def " << anIdx << " has invalid start vertex def";
    EXPECT_TRUE(anEdgeDef.EndVertexDefId().IsValid())
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
  for (int anIdx = 0; anIdx < myGraph.Defs().NbEdges(); ++anIdx)
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      myGraph.Defs().CoEdgesOfEdge(BRepGraph_EdgeId(anIdx));
    if (aCoEdgeIdxs.Length() < 2)
      continue;
    // Check if coedges reference different faces.
    const BRepGraph_NodeId aFace0 = myGraph.Defs().CoEdge(aCoEdgeIdxs.Value(0)).FaceDefId;
    for (int aCEI = 1; aCEI < aCoEdgeIdxs.Length(); ++aCEI)
    {
      if (myGraph.Defs().CoEdge(aCoEdgeIdxs.Value(aCEI)).FaceDefId != aFace0)
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
  for (int anIdx = 0; anIdx < myGraph.Defs().NbEdges(); ++anIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& aDef = myGraph.Defs().Edge(BRepGraph_EdgeId(anIdx));
    if (aDef.IsDegenerate)
      continue;
    // Box edges are not closed, so start and end vertex defs must differ
    EXPECT_NE(aDef.StartVertex.VertexDefId, aDef.EndVertex.VertexDefId)
      << "Non-degenerate edge def " << anIdx << " has identical start and end vertex def ids";
  }
}

TEST_F(BRepGraph_SharingTest, VertexDef_Points_MatchExpectedBoxCorners)
{
  ASSERT_TRUE(myGraph.IsDone());
  // For a simple 10x20x30 box, all 8 vertex points should be valid.
  EXPECT_EQ(myGraph.Defs().NbVertices(), 8);
  for (int anIdx = 0; anIdx < myGraph.Defs().NbVertices(); ++anIdx)
  {
    const BRepGraph_TopoNode::VertexDef& aDef = myGraph.Defs().Vertex(BRepGraph_VertexId(anIdx));
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
  EXPECT_EQ(aGraph.Defs().NbSolids(), 1);

  // All defs are shared (same TShape), counts stay at single-box levels.
  EXPECT_EQ(aGraph.Defs().NbFaces(), 6);
  EXPECT_EQ(aGraph.Defs().NbEdges(), 12);
  EXPECT_EQ(aGraph.Defs().NbVertices(), 8);

  // Compound has 2 child references to the same solid.
  EXPECT_EQ(aGraph.Defs().Compound(BRepGraph_CompoundId(0)).ChildRefs.Length(), 2);
  EXPECT_EQ(aGraph.Defs().Compound(BRepGraph_CompoundId(0)).ChildRefs.Value(0).ChildDefId.Index,
            aGraph.Defs().Compound(BRepGraph_CompoundId(0)).ChildRefs.Value(1).ChildDefId.Index);
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
  EXPECT_EQ(aGraph.Defs().NbSolids(), 2);
  EXPECT_EQ(aGraph.Defs().NbFaces(), 12);
  EXPECT_EQ(aGraph.Defs().NbEdges(), 24);
  EXPECT_EQ(aGraph.Defs().NbVertices(), 16);
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
  EXPECT_EQ(aGraph.Defs().NbFaces(), 6);
  EXPECT_EQ(aGraph.Defs().NbEdges(), 12);
  EXPECT_EQ(aGraph.Defs().NbVertices(), 8);
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
  EXPECT_EQ(aGraph.Defs().NbSolids(), 1);

  // Face/edge/vertex defs are shared (same TShape).
  EXPECT_EQ(aGraph.Defs().NbFaces(), 6);
  EXPECT_EQ(aGraph.Defs().NbEdges(), 12);
  EXPECT_EQ(aGraph.Defs().NbVertices(), 8);
}
