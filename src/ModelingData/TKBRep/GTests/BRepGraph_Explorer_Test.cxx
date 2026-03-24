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
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_Explorer.hxx>
#include <BRepGraph_SpatialView.hxx>
#include <BRepGraph_TopologyPath.hxx>
#include <BRepGraph_Tool.hxx>

#include <BRep_Builder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_CompSolid.hxx>

#include <NCollection_Map.hxx>

#include <gtest/gtest.h>

// --- TopologyPath tests ---

TEST(BRepGraphTopologyPath, Default_Invalid)
{
  BRepGraph_TopologyPath aPath;
  EXPECT_FALSE(aPath.IsValid());
  EXPECT_EQ(aPath.Depth(), 0);
}

TEST(BRepGraphTopologyPath, FromRoot_Valid)
{
  BRepGraph_TopologyPath aPath = BRepGraph_TopologyPath::FromRoot(BRepGraph_NodeId::Solid(0));
  EXPECT_TRUE(aPath.IsValid());
  EXPECT_EQ(aPath.Depth(), 0);
  EXPECT_EQ(aPath.Root(), BRepGraph_NodeId::Solid(0));
}

TEST(BRepGraphTopologyPath, ToFace_CorrectDepth)
{
  BRepGraph_TopologyPath aPath = BRepGraph_TopologyPath::ToFace(0, 0, 2);
  EXPECT_TRUE(aPath.IsValid());
  EXPECT_EQ(aPath.Depth(), 2); // Solid->Shell, Shell->Face
  EXPECT_EQ(aPath.Root(), BRepGraph_NodeId::Solid(0));
}

TEST(BRepGraphTopologyPath, ToVertex_CorrectDepth)
{
  BRepGraph_TopologyPath aPath = BRepGraph_TopologyPath::ToVertex(0, 0, 1, 0, 3, 1);
  EXPECT_TRUE(aPath.IsValid());
  EXPECT_EQ(aPath.Depth(), 5); // Shell, Face, Wire, CoEdge, Vertex
}

TEST(BRepGraphTopologyPath, Equality)
{
  BRepGraph_TopologyPath aPath1 = BRepGraph_TopologyPath::ToEdge(0, 0, 2, 0, 3);
  BRepGraph_TopologyPath aPath2 = BRepGraph_TopologyPath::ToEdge(0, 0, 2, 0, 3);
  BRepGraph_TopologyPath aPath3 = BRepGraph_TopologyPath::ToEdge(0, 0, 2, 0, 1);
  EXPECT_EQ(aPath1, aPath2);
  EXPECT_NE(aPath1, aPath3);
}

TEST(BRepGraphTopologyPath, Hash_Consistent)
{
  BRepGraph_TopologyPath aPath1 = BRepGraph_TopologyPath::ToFace(0, 0, 2);
  BRepGraph_TopologyPath aPath2 = BRepGraph_TopologyPath::ToFace(0, 0, 2);
  std::hash<BRepGraph_TopologyPath> aHasher;
  EXPECT_EQ(aHasher(aPath1), aHasher(aPath2));
}

TEST(BRepGraphTopologyPath, Hash_DiffersOnDifferentPaths)
{
  BRepGraph_TopologyPath aPath1 = BRepGraph_TopologyPath::ToFace(0, 0, 0);
  BRepGraph_TopologyPath aPath2 = BRepGraph_TopologyPath::ToFace(0, 0, 1);
  std::hash<BRepGraph_TopologyPath> aHasher;
  EXPECT_NE(aHasher(aPath1), aHasher(aPath2));
}

// --- Explorer tests ---

TEST(BRepGraphExplorer, Box_EdgeOccurrences_Count24)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_NodeId::Solid(0), BRepGraph_NodeId::Kind::Edge);
  // 12 edges x 2 faces each = 24 edge occurrences.
  EXPECT_EQ(anExp.NbFound(), 24);
}

TEST(BRepGraphExplorer, Box_FaceOccurrences_Count6)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_NodeId::Solid(0), BRepGraph_NodeId::Kind::Face);
  EXPECT_EQ(anExp.NbFound(), 6);
}

TEST(BRepGraphExplorer, Box_VertexOccurrences)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_NodeId::Solid(0), BRepGraph_NodeId::Kind::Vertex);
  EXPECT_GT(anExp.NbFound(), 0);

  // Count unique vertices.
  NCollection_Map<int> aVisited;
  for (; anExp.More(); anExp.Next())
    aVisited.Add(anExp.Current().Index);
  EXPECT_EQ(aVisited.Extent(), 8);
}

TEST(BRepGraphExplorer, Face_EdgeOccurrences_4)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_NodeId::Face(0), BRepGraph_NodeId::Kind::Edge);
  EXPECT_EQ(anExp.NbFound(), 4);
}

TEST(BRepGraphExplorer, InvalidRoot_Empty)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_NodeId(), BRepGraph_NodeId::Kind::Edge);
  EXPECT_EQ(anExp.NbFound(), 0);
}

TEST(BRepGraphExplorer, RootEqualsTarget_ReturnsSelf)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_NodeId::Face(0), BRepGraph_NodeId::Kind::Face);
  EXPECT_EQ(anExp.NbFound(), 1);
  EXPECT_EQ(anExp.Current(), BRepGraph_NodeId::Face(0));
}

// --- Path-based composition tests ---

TEST(BRepGraphSpatialView, GlobalLocation_Box_Identity)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // All paths in a simple box should compose to identity.
  BRepGraph_Explorer anExp(aGraph, BRepGraph_NodeId::Solid(0), BRepGraph_NodeId::Kind::Edge);
  for (; anExp.More(); anExp.Next())
  {
    TopLoc_Location aLoc = aGraph.Spatial().GlobalLocation(anExp.CurrentPath());
    EXPECT_TRUE(aLoc.IsIdentity());
  }
}

TEST(BRepGraphSpatialView, GlobalOrientation_BoxEdges_ForwardOrReversed)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_NodeId::Solid(0), BRepGraph_NodeId::Kind::Edge);
  for (; anExp.More(); anExp.Next())
  {
    TopAbs_Orientation anOri = aGraph.Spatial().GlobalOrientation(anExp.CurrentPath());
    EXPECT_TRUE(anOri == TopAbs_FORWARD || anOri == TopAbs_REVERSED);
  }
}

TEST(BRepGraphSpatialView, LeafNode_MatchesCurrent)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_NodeId::Solid(0), BRepGraph_NodeId::Kind::Edge);
  for (; anExp.More(); anExp.Next())
  {
    BRepGraph_NodeId aLeaf = aGraph.Spatial().LeafNode(anExp.CurrentPath());
    EXPECT_TRUE(aLeaf.IsValid());
    EXPECT_EQ(aLeaf.NodeKind, BRepGraph_NodeId::Kind::Edge);
    EXPECT_EQ(aLeaf, anExp.Current());
  }
}

TEST(BRepGraphSpatialView, NodeAt_IntermediateDepths)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Get a path to an edge.
  BRepGraph_Explorer anExp(aGraph, BRepGraph_NodeId::Solid(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());
  const BRepGraph_TopologyPath& aPath = anExp.CurrentPath();

  // Level 0 = shell, level 1 = face, level 2 = wire, level 3 = coedge (-> edge via 1:1).
  EXPECT_EQ(aPath.Depth(), 4); // 4 steps: shell, face, wire, coedge

  BRepGraph_NodeId aShell = aGraph.Spatial().NodeAt(aPath, 0);
  EXPECT_TRUE(aShell.IsValid());
  EXPECT_EQ(aShell.NodeKind, BRepGraph_NodeId::Kind::Shell);

  BRepGraph_NodeId aFace = aGraph.Spatial().NodeAt(aPath, 1);
  EXPECT_TRUE(aFace.IsValid());
  EXPECT_EQ(aFace.NodeKind, BRepGraph_NodeId::Kind::Face);

  BRepGraph_NodeId aWire = aGraph.Spatial().NodeAt(aPath, 2);
  EXPECT_TRUE(aWire.IsValid());
  EXPECT_EQ(aWire.NodeKind, BRepGraph_NodeId::Kind::Wire);
}

TEST(BRepGraphSpatialView, NodeTransform_NoTransform_Identity)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  gp_Trsf aTrsf = aGraph.Spatial().NodeTransform(BRepGraph_NodeId::Face(0));
  EXPECT_NEAR(aTrsf.Value(1, 4), 0.0, Precision::Confusion());
  EXPECT_NEAR(aTrsf.Value(2, 4), 0.0, Precision::Confusion());
  EXPECT_NEAR(aTrsf.Value(3, 4), 0.0, Precision::Confusion());
}

TEST(BRepGraphSpatialView, FindLevel_ByKind)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_NodeId::Solid(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());
  const BRepGraph_TopologyPath& aPath = anExp.CurrentPath();

  int aFaceLevel = aGraph.Spatial().FindLevel(aPath, BRepGraph_NodeId::Kind::Face);
  EXPECT_GE(aFaceLevel, 0);

  BRepGraph_NodeId aFaceNode = aGraph.Spatial().NodeAt(aPath, aFaceLevel);
  EXPECT_EQ(aFaceNode.NodeKind, BRepGraph_NodeId::Kind::Face);
}

TEST(BRepGraphSpatialView, Truncated_ToFace)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_NodeId::Solid(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());
  const BRepGraph_TopologyPath& aPath = anExp.CurrentPath();

  // Truncate to 2 steps (shell + face).
  BRepGraph_TopologyPath aTruncated = aGraph.Spatial().Truncated(aPath, 2);
  EXPECT_EQ(aTruncated.Depth(), 2);
  EXPECT_EQ(aTruncated.Root(), aPath.Root());

  BRepGraph_NodeId aLeaf = aGraph.Spatial().LeafNode(aTruncated);
  EXPECT_EQ(aLeaf.NodeKind, BRepGraph_NodeId::Kind::Face);
}

TEST(BRepGraphExplorer, SharedEdge_DifferentPaths)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_NodeId::Solid(0), BRepGraph_NodeId::Kind::Edge);

  NCollection_DataMap<int, NCollection_Vector<BRepGraph_TopologyPath>> aEdgePaths;
  for (; anExp.More(); anExp.Next())
  {
    const int anEdgeIdx = anExp.Current().Index;
    if (!aEdgePaths.IsBound(anEdgeIdx))
      aEdgePaths.Bind(anEdgeIdx, NCollection_Vector<BRepGraph_TopologyPath>());
    aEdgePaths.ChangeFind(anEdgeIdx).Append(anExp.CurrentPath());
  }

  // Each edge in a box is shared by exactly 2 faces = 2 distinct paths.
  for (NCollection_DataMap<int, NCollection_Vector<BRepGraph_TopologyPath>>::Iterator
         anIter(aEdgePaths);
       anIter.More();
       anIter.Next())
  {
    EXPECT_EQ(anIter.Value().Length(), 2)
      << "Edge " << anIter.Key() << " should have 2 paths";
    // The two paths must differ.
    EXPECT_NE(anIter.Value().Value(0), anIter.Value().Value(1));
  }
}

// --- Compound tests ---

TEST(BRepGraphExplorer, Compound_FaceCount)
{
  // Build a compound of two boxes.
  TopoDS_Compound aComp;
  BRep_Builder    aBB;
  aBB.MakeCompound(aComp);
  aBB.Add(aComp, BRepPrimAPI_MakeBox(10, 10, 10).Shape());
  aBB.Add(aComp, BRepPrimAPI_MakeBox(20, 20, 20).Shape());

  BRepGraph aGraph;
  aGraph.Build(aComp);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_NodeId::Compound(0), BRepGraph_NodeId::Kind::Face);
  EXPECT_EQ(anExp.NbFound(), 12); // 6 + 6 faces
}

// --- Reverse path discovery tests ---

TEST(BRepGraphSpatialView, PathsTo_SharedEdge_TwoPaths)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Each edge in a box is shared by 2 faces, so PathsTo should find 2 paths.
  NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Spatial().PathsTo(BRepGraph_NodeId::Edge(0));
  EXPECT_EQ(aPaths.Length(), 2);
}

TEST(BRepGraphSpatialView, PathsTo_Face_SinglePath)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Each face appears once under a solid.
  NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Spatial().PathsTo(BRepGraph_NodeId::Face(0));
  EXPECT_EQ(aPaths.Length(), 1);
}

TEST(BRepGraphSpatialView, PathsFromTo_FiltersByRoot)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Spatial().PathsFromTo(BRepGraph_NodeId::Solid(0), BRepGraph_NodeId::Edge(0));
  EXPECT_EQ(aPaths.Length(), 2);
}

TEST(BRepGraphSpatialView, PathsTo_MatchesExplorer)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // For each explorer result, PathsTo should contain the same path.
  BRepGraph_Explorer anExp(aGraph, BRepGraph_NodeId::Solid(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());

  BRepGraph_NodeId aFirstEdge = anExp.Current();
  NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Spatial().PathsTo(aFirstEdge);
  // The explorer path should be among PathsTo results.
  bool aFound = false;
  for (int i = 0; i < aPaths.Length(); ++i)
  {
    if (aPaths.Value(i) == anExp.CurrentPath())
    {
      aFound = true;
      break;
    }
  }
  EXPECT_TRUE(aFound);
}

// --- Path filtering tests ---

TEST(BRepGraphSpatialView, PathContains_Face_True)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_NodeId::Solid(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());

  // The path from Solid to Edge should pass through some face.
  BRepGraph_NodeId aFace = anExp.NodeOf(BRepGraph_NodeId::Kind::Face);
  EXPECT_TRUE(aGraph.Spatial().PathContains(anExp.CurrentPath(), aFace));
}

TEST(BRepGraphSpatialView, FilterByInclude_SharedEdge)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Get all paths to Edge(0), then filter by one of its faces.
  NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Spatial().PathsTo(BRepGraph_NodeId::Edge(0));
  ASSERT_EQ(aPaths.Length(), 2);

  // Find the face in the first path.
  BRepGraph_NodeId aFaceInPath0 = aGraph.Spatial().LeafNode(
    aGraph.Spatial().Truncated(aPaths.Value(0), 2));

  NCollection_Vector<BRepGraph_TopologyPath> aFiltered =
    aGraph.Spatial().FilterByInclude(aPaths, aFaceInPath0);
  EXPECT_EQ(aFiltered.Length(), 1);
}

TEST(BRepGraphSpatialView, FilterByExclude_Face)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Spatial().PathsTo(BRepGraph_NodeId::Edge(0));
  ASSERT_EQ(aPaths.Length(), 2);

  BRepGraph_NodeId aFaceInPath0 = aGraph.Spatial().LeafNode(
    aGraph.Spatial().Truncated(aPaths.Value(0), 2));

  NCollection_Vector<BRepGraph_TopologyPath> aExcluded =
    aGraph.Spatial().FilterByExclude(aPaths, aFaceInPath0);
  EXPECT_EQ(aExcluded.Length(), 1);
}

// --- Explorer convenience tests ---

TEST(BRepGraphExplorer, LocationOf_Kind_Face)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_NodeId::Solid(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());

  // LocationOf(Face) should match LocationAt(face level).
  TopLoc_Location aFaceLoc = anExp.LocationOf(BRepGraph_NodeId::Kind::Face);
  int aFaceLevel = aGraph.Spatial().FindLevel(anExp.CurrentPath(), BRepGraph_NodeId::Kind::Face);
  TopLoc_Location aFaceLocAt = anExp.LocationAt(aFaceLevel);
  EXPECT_TRUE(aFaceLoc.IsEqual(aFaceLocAt));
}

TEST(BRepGraphExplorer, NodeOf_Kind_Face)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_NodeId::Solid(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());

  BRepGraph_NodeId aFace = anExp.NodeOf(BRepGraph_NodeId::Kind::Face);
  EXPECT_TRUE(aFace.IsValid());
  EXPECT_EQ(aFace.NodeKind, BRepGraph_NodeId::Kind::Face);
}

// --- Precompute and NodeLocations tests ---

TEST(BRepGraphSpatialView, PrecomputeLocations_StoredOnNode)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Precompute face locations on solid root.
  BRepGraph::SpatialView::PrecomputeLocations(
    aGraph, BRepGraph_NodeId::Solid(0), BRepGraph_NodeId::Kind::Face);

  // Read back from cache.
  NCollection_Vector<BRepGraph::SpatialView::OccurrenceEntry> aCached =
    aGraph.Spatial().GetPrecomputed(BRepGraph_NodeId::Solid(0));
  EXPECT_EQ(aCached.Length(), 6); // 6 faces in a box
}

TEST(BRepGraphSpatialView, NodeLocations_EdgeHasTwoOccurrences)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Each box edge is shared by 2 faces = 2 occurrence locations.
  NCollection_Vector<BRepGraph::SpatialView::OccurrenceEntry> aEntries =
    aGraph.Spatial().NodeLocations(BRepGraph_NodeId::Edge(0));
  EXPECT_EQ(aEntries.Length(), 2);

  // Both locations should be identity for a simple box.
  for (int i = 0; i < aEntries.Length(); ++i)
    EXPECT_TRUE(aEntries.Value(i).Location.IsIdentity());
}

TEST(BRepGraphSpatialView, NodeLocations_FaceHasOneOccurrence)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  NCollection_Vector<BRepGraph::SpatialView::OccurrenceEntry> aEntries =
    aGraph.Spatial().NodeLocations(BRepGraph_NodeId::Face(0));
  EXPECT_EQ(aEntries.Length(), 1);
}

TEST(BRepGraphSpatialView, NodeLocations_VertexMultipleOccurrences)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // A box corner vertex is shared by 3 edges, each shared by 2 faces.
  NCollection_Vector<BRepGraph::SpatialView::OccurrenceEntry> aEntries =
    aGraph.Spatial().NodeLocations(BRepGraph_NodeId::Vertex(0));
  EXPECT_GT(aEntries.Length(), 0);
}

TEST(BRepGraphSpatialView, PathsTo_InvalidNode_Empty)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Invalid NodeId should return empty.
  NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Spatial().PathsTo(BRepGraph_NodeId());
  EXPECT_EQ(aPaths.Length(), 0);

  // Out-of-range index should return empty.
  NCollection_Vector<BRepGraph_TopologyPath> aPaths2 =
    aGraph.Spatial().PathsTo(BRepGraph_NodeId::Edge(99999));
  EXPECT_EQ(aPaths2.Length(), 0);
}

// --- Depth guard and re-initialization tests ---

TEST(BRepGraphExplorer, DeepCompound_NoStackOverflow)
{
  // Build a deeply nested compound: Compound(Compound(Compound(...(Box)...))).
  TopoDS_Shape aInner = BRepPrimAPI_MakeBox(1, 1, 1).Shape();
  constexpr int THE_NESTING_DEPTH = 100;
  for (int i = 0; i < THE_NESTING_DEPTH; ++i)
  {
    TopoDS_Compound aComp;
    BRep_Builder    aBB;
    aBB.MakeCompound(aComp);
    aBB.Add(aComp, aInner);
    aInner = aComp;
  }
  BRepGraph aGraph;
  aGraph.Build(aInner);
  ASSERT_TRUE(aGraph.IsDone());

  // Should not crash (stack overflow) and should find the box's faces.
  BRepGraph_Explorer anExp(aGraph, BRepGraph_NodeId::Compound(0),
                           BRepGraph_NodeId::Kind::Face);
  EXPECT_EQ(anExp.NbFound(), 6);
}

TEST(BRepGraphExplorer, Init_ResetAndReexplore)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_NodeId::Solid(0),
                           BRepGraph_NodeId::Kind::Face);
  EXPECT_EQ(anExp.NbFound(), 6);

  // Re-init targeting edges.
  anExp.Init(aGraph, BRepGraph_NodeId::Solid(0), BRepGraph_NodeId::Kind::Edge);
  EXPECT_EQ(anExp.NbFound(), 24);
}

// --- Reverse walk with Compound/CompSolid parents ---

TEST(BRepGraphSpatialView, PathsTo_NestedCompound_HasCompoundRoot)
{
  // Compound(Compound(Box)): PathsTo(Face) should return paths from outer Compound.
  TopoDS_Compound aInnerComp;
  BRep_Builder    aBB;
  aBB.MakeCompound(aInnerComp);
  aBB.Add(aInnerComp, BRepPrimAPI_MakeBox(10, 10, 10).Shape());

  TopoDS_Compound aOuterComp;
  aBB.MakeCompound(aOuterComp);
  aBB.Add(aOuterComp, aInnerComp);

  BRepGraph aGraph;
  aGraph.Build(aOuterComp);
  ASSERT_TRUE(aGraph.IsDone());

  // PathsTo a face should trace through inner compound to outer compound root.
  NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Spatial().PathsTo(BRepGraph_NodeId::Face(0));
  ASSERT_GT(aPaths.Length(), 0);
  // Root should be the outer compound (index 0 — outermost).
  EXPECT_EQ(aPaths.Value(0).Root().NodeKind, BRepGraph_NodeId::Kind::Compound);
}

TEST(BRepGraphSpatialView, NodeLocations_CompoundChild_ComposedLocation)
{
  // Build a compound with a translated box child.
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(100, 0, 0));

  TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10, 10, 10).Shape();
  aBox.Move(TopLoc_Location(aTrsf));

  TopoDS_Compound aComp;
  BRep_Builder    aBB;
  aBB.MakeCompound(aComp);
  aBB.Add(aComp, aBox);

  BRepGraph aGraph;
  aGraph.Build(aComp);
  ASSERT_TRUE(aGraph.IsDone());

  // Explorer from compound root should find all faces.
  BRepGraph_Explorer anExp(aGraph, BRepGraph_NodeId::Compound(0),
                           BRepGraph_NodeId::Kind::Face);
  EXPECT_EQ(anExp.NbFound(), 6);
}

TEST(BRepGraphSpatialView, PathsTo_ShellInCompound_CompoundRoot)
{
  // Build a compound containing a bare shell (extracted from a box).
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10, 10, 10).Shape();
  TopExp_Explorer aShellExp(aBox, TopAbs_SHELL);
  ASSERT_TRUE(aShellExp.More());
  const TopoDS_Shape& aShell = aShellExp.Current();

  TopoDS_Compound aComp;
  BRep_Builder    aBB;
  aBB.MakeCompound(aComp);
  aBB.Add(aComp, aShell);

  BRepGraph aGraph;
  aGraph.Build(aComp);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbShells(), 0);

  // PathsTo(Face) should trace through Shell to Compound root.
  NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Spatial().PathsTo(BRepGraph_NodeId::Face(0));
  ASSERT_GT(aPaths.Length(), 0);
  EXPECT_EQ(aPaths.Value(0).Root().NodeKind, BRepGraph_NodeId::Kind::Compound);
}

TEST(BRepGraphSpatialView, PathsTo_SolidInCompSolid_CompSolidRoot)
{
  // Build a CompSolid containing two box solids.
  TopoDS_CompSolid aCS;
  BRep_Builder     aBB;
  aBB.MakeCompSolid(aCS);

  TopExp_Explorer aSolidExp1(BRepPrimAPI_MakeBox(10, 10, 10).Shape(), TopAbs_SOLID);
  ASSERT_TRUE(aSolidExp1.More());
  aBB.Add(aCS, aSolidExp1.Current());

  TopExp_Explorer aSolidExp2(BRepPrimAPI_MakeBox(20, 20, 20).Shape(), TopAbs_SOLID);
  ASSERT_TRUE(aSolidExp2.More());
  aBB.Add(aCS, aSolidExp2.Current());

  BRepGraph aGraph;
  aGraph.Build(aCS);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbCompSolids(), 0);

  // PathsTo(Face) should trace up through Solid to CompSolid root.
  NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Spatial().PathsTo(BRepGraph_NodeId::Face(0));
  ASSERT_GT(aPaths.Length(), 0);
  EXPECT_EQ(aPaths.Value(0).Root().NodeKind, BRepGraph_NodeId::Kind::CompSolid);
}

TEST(BRepGraphSpatialView, PathsTo_CompSolidInCompound_CompoundRoot)
{
  // Compound(CompSolid(Box)): paths should trace through CompSolid to Compound root.
  TopoDS_CompSolid aCS;
  BRep_Builder     aBB;
  aBB.MakeCompSolid(aCS);

  TopExp_Explorer aSolidExp(BRepPrimAPI_MakeBox(10, 10, 10).Shape(), TopAbs_SOLID);
  ASSERT_TRUE(aSolidExp.More());
  aBB.Add(aCS, aSolidExp.Current());

  TopoDS_Compound aComp;
  aBB.MakeCompound(aComp);
  aBB.Add(aComp, aCS);

  BRepGraph aGraph;
  aGraph.Build(aComp);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbCompSolids(), 0);
  ASSERT_GT(aGraph.Defs().NbCompounds(), 0);

  // PathsTo(Face) should trace Solid -> CompSolid -> Compound root.
  NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Spatial().PathsTo(BRepGraph_NodeId::Face(0));
  ASSERT_GT(aPaths.Length(), 0);
  EXPECT_EQ(aPaths.Value(0).Root().NodeKind, BRepGraph_NodeId::Kind::Compound);
}
