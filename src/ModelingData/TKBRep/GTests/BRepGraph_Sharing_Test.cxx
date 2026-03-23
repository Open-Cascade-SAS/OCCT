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
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_UsagesView.hxx>
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

class BRepGraphSharingTest : public testing::Test
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
// Usage counts per def kind
// =========================================================================

TEST_F(BRepGraphSharingTest, EdgeDef_EachHasExactlyTwoUsages)
{
  ASSERT_TRUE(myGraph.IsDone());
  EXPECT_EQ(myGraph.Defs().NbEdges(), 12);
  for (int anIdx = 0; anIdx < myGraph.Defs().NbEdges(); ++anIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& aDef = myGraph.Defs().Edge(anIdx);
    EXPECT_EQ(aDef.Usages.Length(), 2)
      << "Edge def " << anIdx << " expected 2 usages, got " << aDef.Usages.Length();
  }
}

TEST_F(BRepGraphSharingTest, VertexDef_EachHasSixUsages)
{
  ASSERT_TRUE(myGraph.IsDone());
  EXPECT_EQ(myGraph.Defs().NbVertices(), 8);
  for (int anIdx = 0; anIdx < myGraph.Defs().NbVertices(); ++anIdx)
  {
    const BRepGraph_TopoNode::VertexDef& aDef = myGraph.Defs().Vertex(anIdx);
    // Each vertex is shared by 3 edges, and each edge has 2 usages (one per adjacent face),
    // so each vertex gets 3*2=6 vertex usages.
    EXPECT_EQ(aDef.Usages.Length(), 6)
      << "Vertex def " << anIdx << " expected 6 usages, got " << aDef.Usages.Length();
  }
}

TEST_F(BRepGraphSharingTest, FaceDef_EachHasExactlyOneUsage)
{
  ASSERT_TRUE(myGraph.IsDone());
  EXPECT_EQ(myGraph.Defs().NbFaces(), 6);
  for (int anIdx = 0; anIdx < myGraph.Defs().NbFaces(); ++anIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aDef = myGraph.Defs().Face(anIdx);
    EXPECT_EQ(aDef.Usages.Length(), 1)
      << "Face def " << anIdx << " expected 1 usage, got " << aDef.Usages.Length();
  }
}

TEST_F(BRepGraphSharingTest, SolidDef_HasExactlyOneUsage)
{
  ASSERT_TRUE(myGraph.IsDone());
  EXPECT_EQ(myGraph.Defs().NbSolids(), 1);
  const BRepGraph_TopoNode::SolidDef& aDef = myGraph.Defs().Solid(0);
  EXPECT_EQ(aDef.Usages.Length(), 1);
}

TEST_F(BRepGraphSharingTest, ShellDef_HasExactlyOneUsage)
{
  ASSERT_TRUE(myGraph.IsDone());
  EXPECT_EQ(myGraph.Defs().NbShells(), 1);
  const BRepGraph_TopoNode::ShellDef& aDef = myGraph.Defs().Shell(0);
  EXPECT_EQ(aDef.Usages.Length(), 1);
}

// =========================================================================
// Round-trip consistency
// =========================================================================

TEST_F(BRepGraphSharingTest, DefOf_EachEdgeUsage_RoundTrips)
{
  ASSERT_TRUE(myGraph.IsDone());
  for (int anIdx = 0; anIdx < myGraph.Defs().NbEdges(); ++anIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& aDef = myGraph.Defs().Edge(anIdx);
    for (int aUsIdx = 0; aUsIdx < aDef.Usages.Length(); ++aUsIdx)
    {
      const BRepGraph_UsageId& aUsageId = aDef.Usages.Value(aUsIdx);
      BRepGraph_NodeId aResolvedDef = myGraph.DefOf(aUsageId);
      EXPECT_EQ(aResolvedDef.NodeKind, aDef.Id.NodeKind);
      EXPECT_EQ(aResolvedDef.Index, aDef.Id.Index);
    }
  }
}

TEST_F(BRepGraphSharingTest, DefOf_EachFaceUsage_RoundTrips)
{
  ASSERT_TRUE(myGraph.IsDone());
  for (int anIdx = 0; anIdx < myGraph.Defs().NbFaces(); ++anIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aDef = myGraph.Defs().Face(anIdx);
    for (int aUsIdx = 0; aUsIdx < aDef.Usages.Length(); ++aUsIdx)
    {
      const BRepGraph_UsageId& aUsageId = aDef.Usages.Value(aUsIdx);
      BRepGraph_NodeId aResolvedDef = myGraph.DefOf(aUsageId);
      EXPECT_EQ(aResolvedDef.NodeKind, aDef.Id.NodeKind);
      EXPECT_EQ(aResolvedDef.Index, aDef.Id.Index);
    }
  }
}

TEST_F(BRepGraphSharingTest, DefOf_EachVertexUsage_RoundTrips)
{
  ASSERT_TRUE(myGraph.IsDone());
  for (int anIdx = 0; anIdx < myGraph.Defs().NbVertices(); ++anIdx)
  {
    const BRepGraph_TopoNode::VertexDef& aDef = myGraph.Defs().Vertex(anIdx);
    for (int aUsIdx = 0; aUsIdx < aDef.Usages.Length(); ++aUsIdx)
    {
      const BRepGraph_UsageId& aUsageId = aDef.Usages.Value(aUsIdx);
      BRepGraph_NodeId aResolvedDef = myGraph.DefOf(aUsageId);
      EXPECT_EQ(aResolvedDef.NodeKind, aDef.Id.NodeKind);
      EXPECT_EQ(aResolvedDef.Index, aDef.Id.Index);
    }
  }
}

// =========================================================================
// Containment hierarchy
// =========================================================================

TEST_F(BRepGraphSharingTest, SolidUsage_ContainsOneShellUsage)
{
  ASSERT_TRUE(myGraph.IsDone());
  EXPECT_EQ(myGraph.Usages().NbSolids(), 1);
  const BRepGraph_TopoNode::SolidUsage& aSolidUsage = myGraph.Usages().Solid(0);
  EXPECT_EQ(aSolidUsage.ShellUsages.Length(), 1);
}

TEST_F(BRepGraphSharingTest, ShellUsage_ContainsSixFaceUsages)
{
  ASSERT_TRUE(myGraph.IsDone());
  EXPECT_EQ(myGraph.Usages().NbShells(), 1);
  const BRepGraph_TopoNode::ShellUsage& aShellUsage = myGraph.Usages().Shell(0);
  EXPECT_EQ(aShellUsage.FaceUsages.Length(), 6);
}

TEST_F(BRepGraphSharingTest, FaceUsage_OuterWireUsage_BackRef)
{
  ASSERT_TRUE(myGraph.IsDone());
  for (int anIdx = 0; anIdx < myGraph.Usages().NbFaces(); ++anIdx)
  {
    const BRepGraph_TopoNode::FaceUsage& aFaceUsage = myGraph.Usages().Face(anIdx);
    ASSERT_TRUE(aFaceUsage.OuterWireUsage.IsValid())
      << "Face usage " << anIdx << " has no outer wire usage";
    const BRepGraph_TopoNode::WireUsage& aWireUsage =
      myGraph.Usages().Wire(aFaceUsage.OuterWireUsage.Index);
    EXPECT_EQ(aWireUsage.OwnerFaceUsage.Index, aFaceUsage.UsageId.Index)
      << "Outer wire usage of face " << anIdx << " does not back-reference the face";
  }
}

TEST_F(BRepGraphSharingTest, WireUsage_EdgeUsagesCount_FourPerBoxFace)
{
  ASSERT_TRUE(myGraph.IsDone());
  for (int anIdx = 0; anIdx < myGraph.Usages().NbWires(); ++anIdx)
  {
    const BRepGraph_TopoNode::WireUsage& aWireUsage = myGraph.Usages().Wire(anIdx);
    EXPECT_GT(aWireUsage.EdgeUsages.Length(), 0)
      << "Wire usage " << anIdx << " has no edge usages";
    // Box face wires have 4 edges
    EXPECT_EQ(aWireUsage.EdgeUsages.Length(), 4)
      << "Wire usage " << anIdx << " expected 4 edge usages for box face";
  }
}

TEST_F(BRepGraphSharingTest, EdgeUsage_VertexUsages_BothValid)
{
  ASSERT_TRUE(myGraph.IsDone());
  for (int anIdx = 0; anIdx < myGraph.Usages().NbEdges(); ++anIdx)
  {
    const BRepGraph_TopoNode::EdgeUsage& anEdgeUsage = myGraph.Usages().Edge(anIdx);
    EXPECT_TRUE(anEdgeUsage.StartVertexUsage.IsValid())
      << "Edge usage " << anIdx << " has invalid start vertex usage";
    EXPECT_TRUE(anEdgeUsage.EndVertexUsage.IsValid())
      << "Edge usage " << anIdx << " has invalid end vertex usage";
  }
}

// =========================================================================
// Orientation and sharing
// =========================================================================

TEST_F(BRepGraphSharingTest, SharedEdge_TwoUsages_DifferentOrientation)
{
  ASSERT_TRUE(myGraph.IsDone());
  int aDiffOrientCount = 0;
  for (int anIdx = 0; anIdx < myGraph.Defs().NbEdges(); ++anIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& aDef = myGraph.Defs().Edge(anIdx);
    if (aDef.Usages.Length() != 2)
      continue;
    const BRepGraph_TopoNode::EdgeUsage& aUsage0 =
      myGraph.Usages().Edge(aDef.Usages.Value(0).Index);
    const BRepGraph_TopoNode::EdgeUsage& aUsage1 =
      myGraph.Usages().Edge(aDef.Usages.Value(1).Index);
    if (aUsage0.Orientation != aUsage1.Orientation)
    {
      ++aDiffOrientCount;
    }
  }
  // In a box, shared edges between adjacent faces must have opposite orientations
  // to maintain consistent face normals. All 12 edges are shared by 2 faces.
  EXPECT_GT(aDiffOrientCount, 0)
    << "Expected at least some shared edges with different orientations";
}

TEST_F(BRepGraphSharingTest, NonClosedEdge_StartEnd_Different)
{
  ASSERT_TRUE(myGraph.IsDone());
  for (int anIdx = 0; anIdx < myGraph.Defs().NbEdges(); ++anIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& aDef = myGraph.Defs().Edge(anIdx);
    if (aDef.IsDegenerate)
      continue;
    // Box edges are not closed, so start and end vertex defs must differ
    EXPECT_NE(aDef.StartVertexDefId.Index, aDef.EndVertexDefId.Index)
      << "Non-degenerate edge def " << anIdx
      << " has identical start and end vertex def ids";
  }
}

TEST_F(BRepGraphSharingTest, VertexUsage_TransformedPoint_MatchesDefForIdentityLoc)
{
  ASSERT_TRUE(myGraph.IsDone());
  for (int anIdx = 0; anIdx < myGraph.Usages().NbVertices(); ++anIdx)
  {
    const BRepGraph_TopoNode::VertexUsage& aUsage = myGraph.Usages().Vertex(anIdx);
    BRepGraph_NodeId aDefId = myGraph.DefOf(aUsage.UsageId);
    const BRepGraph_TopoNode::VertexDef& aDef = myGraph.Defs().Vertex(aDefId.Index);
    // For a simple box without additional locations, transformed point should match def point
    if (aUsage.GlobalLocation.IsIdentity())
    {
      EXPECT_NEAR(aUsage.TransformedPoint.X(), aDef.Point.X(), Precision::Confusion());
      EXPECT_NEAR(aUsage.TransformedPoint.Y(), aDef.Point.Y(), Precision::Confusion());
      EXPECT_NEAR(aUsage.TransformedPoint.Z(), aDef.Point.Z(), Precision::Confusion());
    }
  }
}

// =========================================================================
// TShape sharing in compounds (standalone tests)
// =========================================================================

TEST(BRepGraphSharingCompoundTest, CompoundTwoIdenticalBoxes)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  BRep_Builder aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox);
  aBuilder.Add(aCompound, aBox);

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  // Same TShape added twice to compound: each occurrence creates a separate solid def,
  // but sub-shapes (faces, edges, vertices) are shared at the TShape level.
  EXPECT_EQ(aGraph.Defs().NbSolids(), 2);
  EXPECT_EQ(aGraph.Usages().NbSolids(), 2);

  // Face/edge/vertex defs are shared (same TShape), so counts stay at single-box levels.
  EXPECT_EQ(aGraph.Defs().NbFaces(), 6);
  EXPECT_EQ(aGraph.Defs().NbEdges(), 12);
  EXPECT_EQ(aGraph.Defs().NbVertices(), 8);

  // Usages must exceed defs since each compound child creates its own usages.
  EXPECT_GT(aGraph.Usages().NbFaces(), aGraph.Defs().NbFaces());
  EXPECT_GT(aGraph.Usages().NbEdges(), aGraph.Defs().NbEdges());
  EXPECT_GT(aGraph.Usages().NbVertices(), aGraph.Defs().NbVertices());
}

TEST(BRepGraphSharingCompoundTest, CompoundTwoDistinctBoxes)
{
  BRepPrimAPI_MakeBox aBoxMaker1(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox1 = aBoxMaker1.Shape();

  BRepPrimAPI_MakeBox aBoxMaker2(5.0, 15.0, 25.0);
  const TopoDS_Shape& aBox2 = aBoxMaker2.Shape();

  BRep_Builder aBuilder;
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

  // Each solid def has exactly 1 usage
  EXPECT_EQ(aGraph.Usages().NbSolids(), 2);
  for (int anIdx = 0; anIdx < aGraph.Defs().NbSolids(); ++anIdx)
  {
    EXPECT_EQ(aGraph.Defs().Solid(anIdx).Usages.Length(), 1);
  }
}

TEST(BRepGraphSharingCompoundTest, CompoundWithLocation_MoreUsagesThanDefs)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(100.0, 0.0, 0.0));
  TopoDS_Shape aMovedBox = aBox.Moved(TopLoc_Location(aTrsf));

  BRep_Builder aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox);
  aBuilder.Add(aCompound, aMovedBox);

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  // Same TShape with different locations: defs are shared, usages are doubled
  EXPECT_EQ(aGraph.Defs().NbFaces(), 6);
  EXPECT_EQ(aGraph.Defs().NbEdges(), 12);
  EXPECT_EQ(aGraph.Defs().NbVertices(), 8);

  // Usages must exceed defs since each occurrence creates its own usage
  EXPECT_GT(aGraph.Usages().NbFaces(), aGraph.Defs().NbFaces());
  EXPECT_GT(aGraph.Usages().NbEdges(), aGraph.Defs().NbEdges());
  EXPECT_GT(aGraph.Usages().NbVertices(), aGraph.Defs().NbVertices());
}

TEST(BRepGraphSharingCompoundTest, TranslatedCopy_SameTShape_SharedDefs)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(50.0, 50.0, 50.0));
  TopoDS_Shape aCopy = aBox.Moved(TopLoc_Location(aTrsf));

  BRep_Builder aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox);
  aBuilder.Add(aCompound, aCopy);

  BRepGraph aGraph;
  aGraph.Build(aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  // Moved() preserves TShape, so sub-shape definitions are shared,
  // but each solid occurrence in the compound creates a separate solid def.
  EXPECT_EQ(aGraph.Defs().NbSolids(), 2);
  EXPECT_EQ(aGraph.Usages().NbSolids(), 2);

  // Face/edge/vertex defs are shared (same TShape).
  EXPECT_EQ(aGraph.Defs().NbFaces(), 6);
  EXPECT_EQ(aGraph.Defs().NbEdges(), 12);
  EXPECT_EQ(aGraph.Defs().NbVertices(), 8);

  // Usages must exceed defs since each occurrence creates its own usages.
  EXPECT_GT(aGraph.Usages().NbFaces(), aGraph.Defs().NbFaces());
  EXPECT_GT(aGraph.Usages().NbEdges(), aGraph.Defs().NbEdges());
  EXPECT_GT(aGraph.Usages().NbVertices(), aGraph.Defs().NbVertices());
}
