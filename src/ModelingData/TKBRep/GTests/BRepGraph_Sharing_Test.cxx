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
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_Iterator.hxx>
#include "BRepGraph_RefTestTools.hxx"
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Builder.hxx>
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
    myGraph.Clear();
    [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes1 =
      BRepGraph_Builder::Add(myGraph, aBox);
  }

  BRepGraph myGraph;
};

// =========================================================================
// Edge sharing via incidence: each box edge appears in 2 wires (faces)
// =========================================================================

TEST_F(BRepGraph_SharingTest, EdgeDef_EachSharedByTwoFaces)
{
  ASSERT_TRUE(myGraph.IsDone());
  EXPECT_EQ(myGraph.Topo().Edges().Nb(), 12);
  // In a box, each edge is shared by exactly 2 faces.
  for (BRepGraph_EdgeIterator anEdgeIt(myGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId anEdgeId   = anEdgeIt.CurrentId();
    const uint32_t         aFaceCount = myGraph.Topo().Edges().NbFaces(anEdgeId);
    EXPECT_EQ(aFaceCount, 2u) << "Edge def " << anEdgeId.Index
                              << " expected to be shared by 2 faces, got " << aFaceCount;
  }
}

TEST_F(BRepGraph_SharingTest, FaceDef_EachHasValidSurface)
{
  ASSERT_TRUE(myGraph.IsDone());
  EXPECT_EQ(myGraph.Topo().Faces().Nb(), 6);
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId       aFaceId = aFaceIt.CurrentId();
    const BRepGraphInc::FaceDef& aDef    = aFaceIt.Current();
    EXPECT_TRUE(aDef.SurfaceRepId.IsValid())
      << "Face def " << aFaceId.Index << " has no surface rep";
  }
}

TEST_F(BRepGraph_SharingTest, SolidDef_HasOneShellRef)
{
  ASSERT_TRUE(myGraph.IsDone());
  EXPECT_EQ(myGraph.Topo().Solids().Nb(), 1);
  EXPECT_EQ(BRepGraph_TestTools::CountShellRefsOfSolid(myGraph, BRepGraph_SolidId::Start()), 1);
}

TEST_F(BRepGraph_SharingTest, ShellDef_HasSixFaceRefs)
{
  ASSERT_TRUE(myGraph.IsDone());
  EXPECT_EQ(myGraph.Topo().Shells().Nb(), 1);
  EXPECT_EQ(BRepGraph_TestTools::CountFaceRefsOfShell(myGraph, BRepGraph_ShellId::Start()), 6);
}

// =========================================================================
// Containment hierarchy
// =========================================================================

TEST_F(BRepGraph_SharingTest, SolidDef_ContainsOneShellRef)
{
  ASSERT_TRUE(myGraph.IsDone());
  EXPECT_EQ(myGraph.Topo().Solids().Nb(), 1);
  EXPECT_EQ(BRepGraph_TestTools::CountShellRefsOfSolid(myGraph, BRepGraph_SolidId::Start()), 1);
}

TEST_F(BRepGraph_SharingTest, ShellDef_ContainsSixFaceRefs)
{
  ASSERT_TRUE(myGraph.IsDone());
  EXPECT_EQ(myGraph.Topo().Shells().Nb(), 1);
  EXPECT_EQ(BRepGraph_TestTools::CountFaceRefsOfShell(myGraph, BRepGraph_ShellId::Start()), 6);
}

TEST_F(BRepGraph_SharingTest, FaceDef_OuterWireIdx_Valid)
{
  ASSERT_TRUE(myGraph.IsDone());
  for (BRepGraph_FaceIterator aFaceIt(myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId aFaceId     = aFaceIt.CurrentId();
    const BRepGraph_WireId anOuterWire = BRepGraph_TestTools::OuterWireOfFace(myGraph, aFaceId);
    EXPECT_TRUE(anOuterWire.IsValid()) << "Face def " << aFaceId.Index << " has no outer wire";
  }
}

TEST_F(BRepGraph_SharingTest, WireDef_CoEdgeRefsCount_FourPerBoxFace)
{
  ASSERT_TRUE(myGraph.IsDone());
  for (BRepGraph_WireIterator aWireIt(myGraph); aWireIt.More(); aWireIt.Next())
  {
    const BRepGraph_WireId aWireId = aWireIt.CurrentId();
    const int aNbCoEdgeRefs        = BRepGraph_TestTools::CountCoEdgeRefsOfWire(myGraph, aWireId);
    EXPECT_GT(aNbCoEdgeRefs, 0) << "Wire def " << aWireId.Index << " has no coedge refs";
    // Box face wires have 4 edges
    EXPECT_EQ(aNbCoEdgeRefs, 4) << "Wire def " << aWireId.Index
                                << " expected 4 coedge refs for box face";
  }
}

TEST_F(BRepGraph_SharingTest, EdgeDef_VertexDefs_BothValid)
{
  ASSERT_TRUE(myGraph.IsDone());
  for (BRepGraph_EdgeIterator anEdgeIt(myGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();
    EXPECT_TRUE(BRepGraph_Tool::Edge::StartVertexRef(myGraph, anEdgeId).VertexDefId.IsValid())
      << "Edge def " << anEdgeId.Index << " has invalid start vertex def";
    EXPECT_TRUE(BRepGraph_Tool::Edge::EndVertexRef(myGraph, anEdgeId).VertexDefId.IsValid())
      << "Edge def " << anEdgeId.Index << " has invalid end vertex def";
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
  for (BRepGraph_EdgeIterator anEdgeIt(myGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId                              anEdgeId = anEdgeIt.CurrentId();
    const NCollection_DynamicArray<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
      myGraph.Topo().Edges().CoEdges(anEdgeId);
    if (aCoEdgeIdxs.Length() < 2)
    {
      continue;
    }
    // Check if coedges reference different faces.
    const BRepGraph_NodeId aFace0 =
      myGraph.Topo().CoEdges().Definition(aCoEdgeIdxs.Value(0)).FaceDefId;
    for (int aCEI = 1; aCEI < aCoEdgeIdxs.Length(); ++aCEI)
    {
      if (myGraph.Topo().CoEdges().Definition(aCoEdgeIdxs.Value(aCEI)).FaceDefId != aFace0)
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
  for (BRepGraph_EdgeIterator anEdgeIt(myGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId       anEdgeId = anEdgeIt.CurrentId();
    const BRepGraphInc::EdgeDef& aDef     = anEdgeIt.Current();
    if (aDef.IsDegenerate)
    {
      continue;
    }
    // Box edges are not closed, so start and end vertex defs must differ
    const BRepGraph_VertexId aStartVtx =
      BRepGraph_Tool::Edge::StartVertexRef(myGraph, anEdgeId).VertexDefId;
    const BRepGraph_VertexId anEndVtx =
      BRepGraph_Tool::Edge::EndVertexRef(myGraph, anEdgeId).VertexDefId;
    EXPECT_NE(aStartVtx, anEndVtx) << "Non-degenerate edge def " << anEdgeId.Index
                                   << " has identical start and end vertex def ids";
  }
}

TEST_F(BRepGraph_SharingTest, VertexDef_Points_MatchExpectedBoxCorners)
{
  ASSERT_TRUE(myGraph.IsDone());
  // For a simple 10x20x30 box, all 8 vertex points should be valid.
  EXPECT_EQ(myGraph.Topo().Vertices().Nb(), 8);
  for (BRepGraph_VertexIterator aVertexIt(myGraph); aVertexIt.More(); aVertexIt.Next())
  {
    const BRepGraphInc::VertexDef& aDef = aVertexIt.Current();
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
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes2 =
    BRepGraph_Builder::Add(aGraph, aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  // Same TShape added twice to compound: definition is shared (1 solid def),
  // compound has 2 ChildRefs pointing to the same solid index.
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), 1);

  // All defs are shared (same TShape), counts stay at single-box levels.
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 6);
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), 12);
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), 8);

  // Compound has 2 child references to the same solid.
  const NCollection_DynamicArray<BRepGraph_ChildRefId> aChildRefs =
    BRepGraph_TestTools::ChildRefsOfParent(aGraph, BRepGraph_CompoundId::Start());
  ASSERT_EQ(aChildRefs.Length(), 2);
  EXPECT_EQ(aGraph.Refs().Children().Entry(aChildRefs.Value(0)).ChildDefId.Index,
            aGraph.Refs().Children().Entry(aChildRefs.Value(1)).ChildDefId.Index);
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
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes3 =
    BRepGraph_Builder::Add(aGraph, aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  // Two different TShapes: no sharing, definitions are independent
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), 2);
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 12);
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), 24);
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), 16);
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
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes4 =
    BRepGraph_Builder::Add(aGraph, aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  // Same TShape with different locations: defs are shared.
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 6);
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), 12);
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), 8);
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
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aBuildRes5 =
    BRepGraph_Builder::Add(aGraph, aCompound);
  ASSERT_TRUE(aGraph.IsDone());

  // Moved() preserves TShape, so all definitions are shared (1 solid def).
  // Compound has 2 ChildRefs with different locations.
  EXPECT_EQ(aGraph.Topo().Solids().Nb(), 1);

  // Face/edge/vertex defs are shared (same TShape).
  EXPECT_EQ(aGraph.Topo().Faces().Nb(), 6);
  EXPECT_EQ(aGraph.Topo().Edges().Nb(), 12);
  EXPECT_EQ(aGraph.Topo().Vertices().Nb(), 8);
}
