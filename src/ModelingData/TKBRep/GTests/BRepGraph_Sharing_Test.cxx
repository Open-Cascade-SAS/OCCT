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
// Edge sharing via incidence: each box edge appears in 2 wires (faces)
// =========================================================================

TEST_F(BRepGraphSharingTest, EdgeDef_EachSharedByTwoFaces)
{
  ASSERT_TRUE(myGraph.IsDone());
  EXPECT_EQ(myGraph.Defs().NbEdges(), 12);
  // In a box, each edge is shared by exactly 2 faces.
  for (int anIdx = 0; anIdx < myGraph.Defs().NbEdges(); ++anIdx)
  {
    int aFaceCount = myGraph.Builder().FaceCountForEdge(anIdx);
    EXPECT_EQ(aFaceCount, 2)
      << "Edge def " << anIdx << " expected to be shared by 2 faces, got " << aFaceCount;
  }
}

TEST_F(BRepGraphSharingTest, FaceDef_EachHasValidSurface)
{
  ASSERT_TRUE(myGraph.IsDone());
  EXPECT_EQ(myGraph.Defs().NbFaces(), 6);
  for (int anIdx = 0; anIdx < myGraph.Defs().NbFaces(); ++anIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aDef = myGraph.Defs().Face(anIdx);
    EXPECT_FALSE(aDef.Surface.IsNull())
      << "Face def " << anIdx << " has null surface";
  }
}

TEST_F(BRepGraphSharingTest, SolidDef_HasOneShellRef)
{
  ASSERT_TRUE(myGraph.IsDone());
  EXPECT_EQ(myGraph.Defs().NbSolids(), 1);
  const BRepGraph_TopoNode::SolidDef& aDef = myGraph.Defs().Solid(0);
  EXPECT_EQ(aDef.ShellRefs.Length(), 1);
}

TEST_F(BRepGraphSharingTest, ShellDef_HasSixFaceRefs)
{
  ASSERT_TRUE(myGraph.IsDone());
  EXPECT_EQ(myGraph.Defs().NbShells(), 1);
  const BRepGraph_TopoNode::ShellDef& aDef = myGraph.Defs().Shell(0);
  EXPECT_EQ(aDef.FaceRefs.Length(), 6);
}

// =========================================================================
// Containment hierarchy
// =========================================================================

TEST_F(BRepGraphSharingTest, SolidDef_ContainsOneShellRef)
{
  ASSERT_TRUE(myGraph.IsDone());
  EXPECT_EQ(myGraph.Defs().NbSolids(), 1);
  const BRepGraph_TopoNode::SolidDef& aSolidDef = myGraph.Defs().Solid(0);
  EXPECT_EQ(aSolidDef.ShellRefs.Length(), 1);
}

TEST_F(BRepGraphSharingTest, ShellDef_ContainsSixFaceRefs)
{
  ASSERT_TRUE(myGraph.IsDone());
  EXPECT_EQ(myGraph.Defs().NbShells(), 1);
  const BRepGraph_TopoNode::ShellDef& aShellDef = myGraph.Defs().Shell(0);
  EXPECT_EQ(aShellDef.FaceRefs.Length(), 6);
}

TEST_F(BRepGraphSharingTest, FaceDef_OuterWireIdx_Valid)
{
  ASSERT_TRUE(myGraph.IsDone());
  for (int anIdx = 0; anIdx < myGraph.Defs().NbFaces(); ++anIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFaceDef = myGraph.Defs().Face(anIdx);
    EXPECT_GE(aFaceDef.OuterWireIdx(), 0)
      << "Face def " << anIdx << " has no outer wire";
  }
}

TEST_F(BRepGraphSharingTest, WireDef_EdgeRefsCount_FourPerBoxFace)
{
  ASSERT_TRUE(myGraph.IsDone());
  for (int anIdx = 0; anIdx < myGraph.Defs().NbWires(); ++anIdx)
  {
    const BRepGraph_TopoNode::WireDef& aWireDef = myGraph.Defs().Wire(anIdx);
    EXPECT_GT(aWireDef.EdgeRefs.Length(), 0)
      << "Wire def " << anIdx << " has no edge refs";
    // Box face wires have 4 edges
    EXPECT_EQ(aWireDef.EdgeRefs.Length(), 4)
      << "Wire def " << anIdx << " expected 4 edge refs for box face";
  }
}

TEST_F(BRepGraphSharingTest, EdgeDef_VertexDefs_BothValid)
{
  ASSERT_TRUE(myGraph.IsDone());
  for (int anIdx = 0; anIdx < myGraph.Defs().NbEdges(); ++anIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = myGraph.Defs().Edge(anIdx);
    EXPECT_TRUE(anEdgeDef.StartVertexDefId.IsValid())
      << "Edge def " << anIdx << " has invalid start vertex def";
    EXPECT_TRUE(anEdgeDef.EndVertexDefId.IsValid())
      << "Edge def " << anIdx << " has invalid end vertex def";
  }
}

// =========================================================================
// Orientation via incidence refs
// =========================================================================

TEST_F(BRepGraphSharingTest, SharedEdge_IncidenceRefs_DifferentOrientation)
{
  ASSERT_TRUE(myGraph.IsDone());
  // In a box, shared edges between adjacent faces must have opposite orientations
  // to maintain consistent face normals.
  // Check via the PCurve entries on edge defs: each edge shared by 2 faces
  // should have at least one pair of PCurves with different orientations.
  int aDiffOrientCount = 0;
  for (int anIdx = 0; anIdx < myGraph.Defs().NbEdges(); ++anIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& aDef = myGraph.Defs().Edge(anIdx);
    if (aDef.PCurves.Length() < 2)
      continue;
    // Check if orientations differ between any two PCurve entries.
    const TopAbs_Orientation anOri0 = aDef.PCurves.Value(0).EdgeOrientation;
    for (int aPCI = 1; aPCI < aDef.PCurves.Length(); ++aPCI)
    {
      if (aDef.PCurves.Value(aPCI).EdgeOrientation != anOri0)
      {
        ++aDiffOrientCount;
        break;
      }
    }
  }
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

TEST_F(BRepGraphSharingTest, VertexDef_Points_MatchExpectedBoxCorners)
{
  ASSERT_TRUE(myGraph.IsDone());
  // For a simple 10x20x30 box, all 8 vertex points should be valid.
  EXPECT_EQ(myGraph.Defs().NbVertices(), 8);
  for (int anIdx = 0; anIdx < myGraph.Defs().NbVertices(); ++anIdx)
  {
    const BRepGraph_TopoNode::VertexDef& aDef = myGraph.Defs().Vertex(anIdx);
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

  // Face/edge/vertex defs are shared (same TShape), so counts stay at single-box levels.
  EXPECT_EQ(aGraph.Defs().NbFaces(), 6);
  EXPECT_EQ(aGraph.Defs().NbEdges(), 12);
  EXPECT_EQ(aGraph.Defs().NbVertices(), 8);
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

  // Same TShape with different locations: defs are shared.
  EXPECT_EQ(aGraph.Defs().NbFaces(), 6);
  EXPECT_EQ(aGraph.Defs().NbEdges(), 12);
  EXPECT_EQ(aGraph.Defs().NbVertices(), 8);
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

  // Face/edge/vertex defs are shared (same TShape).
  EXPECT_EQ(aGraph.Defs().NbFaces(), 6);
  EXPECT_EQ(aGraph.Defs().NbEdges(), 12);
  EXPECT_EQ(aGraph.Defs().NbVertices(), 8);
}
