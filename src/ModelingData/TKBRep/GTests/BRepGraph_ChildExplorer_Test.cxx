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
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_ChildExplorer.hxx>
#include <BRepGraph_PathView.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_TopologyPath.hxx>

#include <BRep_Builder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_CompSolid.hxx>

#include <NCollection_IncAllocator.hxx>
#include <NCollection_Map.hxx>

#include <gtest/gtest.h>

namespace
{
template <class theValueType>
static void expectSameSequence(const NCollection_Vector<theValueType>& theLeft,
                               const NCollection_Vector<theValueType>& theRight)
{
  ASSERT_EQ(theLeft.Length(), theRight.Length());
  for (int i = 0; i < theLeft.Length(); ++i)
  {
    EXPECT_EQ(theLeft.Value(i), theRight.Value(i));
  }
}

static void expectSameOccurrenceEntries(
  const NCollection_Vector<BRepGraph::PathView::OccurrenceEntry>& theLeft,
  const NCollection_Vector<BRepGraph::PathView::OccurrenceEntry>& theRight)
{
  ASSERT_EQ(theLeft.Length(), theRight.Length());
  for (int i = 0; i < theLeft.Length(); ++i)
  {
    EXPECT_EQ(theLeft.Value(i).Path, theRight.Value(i).Path);
    EXPECT_TRUE(theLeft.Value(i).Location.IsEqual(theRight.Value(i).Location));
    EXPECT_EQ(theLeft.Value(i).Orientation, theRight.Value(i).Orientation);
  }
}

static occ::handle<NCollection_BaseAllocator> pathAllocator()
{
  return new NCollection_IncAllocator();
}

static BRepGraph_ChildExplorer makeDirectChildExplorer(const BRepGraph&       theGraph,
                                                       const BRepGraph_NodeId theRoot,
                                                       BRepGraph_NodeId::Kind theTargetKind)
{
  return BRepGraph_ChildExplorer(theGraph,
                                 theRoot,
                                 theTargetKind,
                                 BRepGraph_ChildExplorer::TraversalMode::DirectChildren);
}

static BRepGraph_ChildExplorer makeDirectChildExplorer(const BRepGraph&         theGraph,
                                                       const BRepGraph_NodeId   theRoot,
                                                       BRepGraph_NodeId::Kind   theTargetKind,
                                                       const TopLoc_Location&   theLoc,
                                                       const TopAbs_Orientation theOri)
{
  return BRepGraph_ChildExplorer(theGraph,
                                 theRoot,
                                 theTargetKind,
                                 theLoc,
                                 theOri,
                                 BRepGraph_ChildExplorer::TraversalMode::DirectChildren);
}
} // namespace

// --- TopologyPath tests ---

TEST(BRepGraph_ChildExplorerTest, Default_Invalid)
{
  BRepGraph_TopologyPath aPath;
  EXPECT_FALSE(aPath.IsValid());
  EXPECT_EQ(aPath.Depth(), 0);
}

TEST(BRepGraph_ChildExplorerTest, FromRoot_Valid)
{
  BRepGraph_TopologyPath aPath = BRepGraph_TopologyPath::FromRoot(BRepGraph_SolidId(0),
                                                                  pathAllocator());
  EXPECT_TRUE(aPath.IsValid());
  EXPECT_EQ(aPath.Depth(), 0);
  EXPECT_EQ(aPath.Root(), BRepGraph_SolidId(0));
}

TEST(BRepGraph_ChildExplorerTest, ToFace_CorrectDepth)
{
  BRepGraph_TopologyPath aPath = BRepGraph_TopologyPath::ToFace(0, 0, 2, pathAllocator());
  EXPECT_TRUE(aPath.IsValid());
  EXPECT_EQ(aPath.Depth(), 2); // Solid->Shell, Shell->Face
  EXPECT_EQ(aPath.Root(), BRepGraph_SolidId(0));
}

TEST(BRepGraph_ChildExplorerTest, ToVertex_CorrectDepth)
{
  BRepGraph_TopologyPath aPath =
    BRepGraph_TopologyPath::ToVertex(0, 0, 1, 0, 3, 1, pathAllocator());
  EXPECT_TRUE(aPath.IsValid());
  EXPECT_EQ(aPath.Depth(), 5); // Shell, Face, Wire, CoEdge, Vertex
}

TEST(BRepGraph_ChildExplorerTest, Equality)
{
  BRepGraph_TopologyPath aPath1 = BRepGraph_TopologyPath::ToEdge(0, 0, 2, 0, 3,
                                                                 pathAllocator());
  BRepGraph_TopologyPath aPath2 = BRepGraph_TopologyPath::ToEdge(0, 0, 2, 0, 3,
                                                                 pathAllocator());
  BRepGraph_TopologyPath aPath3 = BRepGraph_TopologyPath::ToEdge(0, 0, 2, 0, 1,
                                                                 pathAllocator());
  EXPECT_EQ(aPath1, aPath2);
  EXPECT_NE(aPath1, aPath3);
}

TEST(BRepGraph_ChildExplorerTest, Hash_Consistent)
{
  BRepGraph_TopologyPath            aPath1 = BRepGraph_TopologyPath::ToFace(0, 0, 2,
                                                                  pathAllocator());
  BRepGraph_TopologyPath            aPath2 = BRepGraph_TopologyPath::ToFace(0, 0, 2,
                                                                  pathAllocator());
  std::hash<BRepGraph_TopologyPath> aHasher;
  EXPECT_EQ(aHasher(aPath1), aHasher(aPath2));
}

TEST(BRepGraph_ChildExplorerTest, Hash_DiffersOnDifferentPaths)
{
  BRepGraph_TopologyPath            aPath1 = BRepGraph_TopologyPath::ToFace(0, 0, 0,
                                                                  pathAllocator());
  BRepGraph_TopologyPath            aPath2 = BRepGraph_TopologyPath::ToFace(0, 0, 1,
                                                                  pathAllocator());
  std::hash<BRepGraph_TopologyPath> aHasher;
  EXPECT_NE(aHasher(aPath1), aHasher(aPath2));
}

// --- Explorer tests ---

TEST(BRepGraph_ChildExplorerTest, Box_EdgeOccurrences_Count24)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
       anExp.More(); anExp.Next())
    ++aCount;
  // 12 edges x 2 faces each = 24 edge occurrences.
  EXPECT_EQ(aCount, 24);
}

TEST(BRepGraph_ChildExplorerTest, Box_FaceOccurrences_Count6)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aFaceCount = 0;
  for (BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Face);
       anExp.More(); anExp.Next())
    ++aFaceCount;
  EXPECT_EQ(aFaceCount, 6);
}

TEST(BRepGraph_ChildExplorerTest, Box_VertexOccurrences)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Count unique vertices.
  int                  aCount = 0;
  NCollection_Map<int> aVisited;
  for (BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Vertex);
       anExp.More(); anExp.Next())
  {
    aVisited.Add(anExp.Current().Index);
    ++aCount;
  }
  EXPECT_GT(aCount, 0);
  EXPECT_EQ(aVisited.Extent(), 8);
}

TEST(BRepGraph_ChildExplorerTest, Face_EdgeOccurrences_4)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_FaceId(0), BRepGraph_NodeId::Kind::Edge);
       anExp.More(); anExp.Next())
    ++aCount;
  EXPECT_EQ(aCount, 4);
}

TEST(BRepGraph_ChildExplorerTest, InvalidRoot_Empty)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_NodeId(), BRepGraph_NodeId::Kind::Edge);
  EXPECT_FALSE(anExp.More());
}

TEST(BRepGraph_ChildExplorerTest, RootEqualsTarget_ReturnsSelf)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_FaceId(0), BRepGraph_NodeId::Kind::Face);
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current(), BRepGraph_FaceId(0));
  anExp.Next();
  EXPECT_FALSE(anExp.More());
}

TEST(BRepGraph_ChildExplorerTest, AvoidKind_Shell_SkipsContainedFaces)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ChildExplorer anExp(aGraph,
                                BRepGraph_SolidId(0),
                                BRepGraph_NodeId::Kind::Face,
                                BRepGraph_NodeId::Kind::Shell,
                                false);
  EXPECT_FALSE(anExp.More());
}

TEST(BRepGraph_ChildExplorerTest, AvoidKind_EmitBoundary_ReturnsFacesInsteadOfEdges)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aFaceCount = 0;
  for (BRepGraph_ChildExplorer anExp(aGraph,
                                     BRepGraph_SolidId(0),
                                     BRepGraph_NodeId::Kind::Edge,
                                     BRepGraph_NodeId::Kind::Face,
                                     true);
       anExp.More(); anExp.Next())
  {
    EXPECT_EQ(anExp.Current().NodeKind, BRepGraph_NodeId::Kind::Face);
    ++aFaceCount;
  }
  EXPECT_EQ(aFaceCount, 6);
}

TEST(BRepGraph_ChildExplorerTest, AvoidKind_SameAsTarget_IsIgnored)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aFaceCount = 0;
  for (BRepGraph_ChildExplorer anExp(aGraph,
                                     BRepGraph_SolidId(0),
                                     BRepGraph_NodeId::Kind::Face,
                                     BRepGraph_NodeId::Kind::Face,
                                     false);
       anExp.More(); anExp.Next())
  {
    EXPECT_EQ(anExp.Current().NodeKind, BRepGraph_NodeId::Kind::Face);
    ++aFaceCount;
  }
  EXPECT_EQ(aFaceCount, 6);
}

TEST(BRepGraph_ChildExplorerTest, AllDescendants_Recursive_YieldsResolvedKinds)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aShellCount  = 0;
  int aFaceCount   = 0;
  int aWireCount   = 0;
  int anEdgeCount  = 0;
  int aVertexCount = 0;
  for (BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId(0)); anExp.More(); anExp.Next())
  {
    switch (anExp.Current().NodeKind)
    {
      case BRepGraph_NodeId::Kind::Shell:
        ++aShellCount;
        break;
      case BRepGraph_NodeId::Kind::Face:
        ++aFaceCount;
        break;
      case BRepGraph_NodeId::Kind::Wire:
        ++aWireCount;
        break;
      case BRepGraph_NodeId::Kind::Edge:
        ++anEdgeCount;
        break;
      case BRepGraph_NodeId::Kind::Vertex:
        ++aVertexCount;
        break;
      default:
        FAIL() << "Unexpected kind in unfiltered child traversal";
        break;
    }
  }

  EXPECT_EQ(aShellCount, 1);
  EXPECT_EQ(aFaceCount, 6);
  EXPECT_EQ(aWireCount, 6);
  EXPECT_EQ(anEdgeCount, 24);
  EXPECT_EQ(aVertexCount, 48);
}

TEST(BRepGraph_ChildExplorerTest, AllDescendants_AvoidFaceBoundary_StopsBelowFaces)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aShellCount = 0;
  int aFaceCount  = 0;
  for (BRepGraph_ChildExplorer anExp(aGraph,
                                     BRepGraph_SolidId(0),
                                     BRepGraph_NodeId::Kind::Face,
                                     true);
       anExp.More(); anExp.Next())
  {
    if (anExp.Current().NodeKind == BRepGraph_NodeId::Kind::Shell)
    {
      ++aShellCount;
      continue;
    }

    EXPECT_EQ(anExp.Current().NodeKind, BRepGraph_NodeId::Kind::Face);
    ++aFaceCount;
  }

  EXPECT_EQ(aShellCount, 1);
  EXPECT_EQ(aFaceCount, 6);
}

// --- Cached location/orientation tests ---

TEST(BRepGraph_ChildExplorerTest, CachedLocation_MatchesPathView)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Verify cached Location() matches PathView::GlobalLocation() for every edge occurrence.
  for (BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
       anExp.More(); anExp.Next())
  {
    TopLoc_Location aCached  = anExp.Location();
    TopLoc_Location aFromPV  = aGraph.Paths().GlobalLocation(anExp.CurrentPath(pathAllocator()));
    EXPECT_TRUE(aCached.IsEqual(aFromPV));
  }
}

TEST(BRepGraph_ChildExplorerTest, CachedOrientation_MatchesPathView)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  for (BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
       anExp.More(); anExp.Next())
  {
    TopAbs_Orientation aCached = anExp.Orientation();
    TopAbs_Orientation aFromPV =
      aGraph.Paths().GlobalOrientation(anExp.CurrentPath(pathAllocator()));
    EXPECT_EQ(aCached, aFromPV);
  }
}

TEST(BRepGraph_ChildExplorerTest, NoCumLoc_IdentityLocation)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  for (BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge,
                                false, true);
       anExp.More(); anExp.Next())
  {
    EXPECT_TRUE(anExp.Location().IsIdentity());
  }
}

TEST(BRepGraph_ChildExplorerTest, NoCumOri_ForwardOrientation)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  for (BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge,
                                true, false);
       anExp.More(); anExp.Next())
  {
    EXPECT_EQ(anExp.Orientation(), TopAbs_FORWARD);
  }
}

// --- Path-based composition tests ---

TEST(BRepGraph_ChildExplorerTest, GlobalLocation_Box_Identity)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // All paths in a simple box should compose to identity.
  BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  for (; anExp.More(); anExp.Next())
  {
    EXPECT_TRUE(anExp.Location().IsIdentity());
  }
}

TEST(BRepGraph_ChildExplorerTest, GlobalOrientation_BoxEdges_ForwardOrReversed)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  for (; anExp.More(); anExp.Next())
  {
    TopAbs_Orientation anOri = anExp.Orientation();
    EXPECT_TRUE(anOri == TopAbs_FORWARD || anOri == TopAbs_REVERSED);
  }
}

TEST(BRepGraph_ChildExplorerTest, LeafNode_MatchesCurrent)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  for (; anExp.More(); anExp.Next())
  {
    BRepGraph_NodeId aLeaf = aGraph.Paths().LeafNode(anExp.CurrentPath(pathAllocator()));
    EXPECT_TRUE(aLeaf.IsValid());
    EXPECT_EQ(aLeaf.NodeKind, BRepGraph_NodeId::Kind::Edge);
    EXPECT_EQ(aLeaf, anExp.Current());
  }
}

TEST(BRepGraph_ChildExplorerTest, NodeAt_IntermediateDepths)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Get a path to an edge.
  BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());
  const BRepGraph_TopologyPath aPath = anExp.CurrentPath(pathAllocator());

  // Level 0 = shell, level 1 = face, level 2 = wire, level 3 = coedge (-> edge via 1:1).
  EXPECT_EQ(aPath.Depth(), 4); // 4 steps: shell, face, wire, coedge

  BRepGraph_NodeId aShell = aGraph.Paths().NodeAt(aPath, 0);
  EXPECT_TRUE(aShell.IsValid());
  EXPECT_EQ(aShell.NodeKind, BRepGraph_NodeId::Kind::Shell);

  BRepGraph_NodeId aFace = aGraph.Paths().NodeAt(aPath, 1);
  EXPECT_TRUE(aFace.IsValid());
  EXPECT_EQ(aFace.NodeKind, BRepGraph_NodeId::Kind::Face);

  BRepGraph_NodeId aWire = aGraph.Paths().NodeAt(aPath, 2);
  EXPECT_TRUE(aWire.IsValid());
  EXPECT_EQ(aWire.NodeKind, BRepGraph_NodeId::Kind::Wire);
}

TEST(BRepGraph_ChildExplorerTest, FindLevel_ByKind)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());
  const BRepGraph_TopologyPath aPath = anExp.CurrentPath(pathAllocator());

  int aFaceLevel = aGraph.Paths().FindLevel(aPath, BRepGraph_NodeId::Kind::Face);
  EXPECT_GE(aFaceLevel, 0);

  BRepGraph_NodeId aFaceNode = aGraph.Paths().NodeAt(aPath, aFaceLevel);
  EXPECT_EQ(aFaceNode.NodeKind, BRepGraph_NodeId::Kind::Face);
}

TEST(BRepGraph_ChildExplorerTest, Truncated_ToFace)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());
  const BRepGraph_TopologyPath aPath = anExp.CurrentPath(pathAllocator());

  // Truncate to 2 steps (shell + face).
  BRepGraph_TopologyPath aTruncated = aGraph.Paths().Truncated(aPath, 2, pathAllocator());
  EXPECT_EQ(aTruncated.Depth(), 2);
  EXPECT_EQ(aTruncated.Root(), aPath.Root());

  BRepGraph_NodeId aLeaf = aGraph.Paths().LeafNode(aTruncated);
  EXPECT_EQ(aLeaf.NodeKind, BRepGraph_NodeId::Kind::Face);
}

TEST(BRepGraph_ChildExplorerTest, SharedEdge_DifferentPaths)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);

  NCollection_DataMap<int, NCollection_Vector<BRepGraph_TopologyPath>> aEdgePaths;
  for (; anExp.More(); anExp.Next())
  {
    const int anEdgeIdx = anExp.Current().Index;
    if (!aEdgePaths.IsBound(anEdgeIdx))
      aEdgePaths.Bind(anEdgeIdx, NCollection_Vector<BRepGraph_TopologyPath>(8));
    aEdgePaths.ChangeFind(anEdgeIdx).Append(anExp.CurrentPath(pathAllocator()));
  }

  // Each edge in a box is shared by exactly 2 faces = 2 distinct paths.
  for (NCollection_DataMap<int, NCollection_Vector<BRepGraph_TopologyPath>>::Iterator anIter(
         aEdgePaths);
       anIter.More();
       anIter.Next())
  {
    EXPECT_EQ(anIter.Value().Length(), 2) << "Edge " << anIter.Key() << " should have 2 paths";
    // The two paths must differ.
    EXPECT_NE(anIter.Value().Value(0), anIter.Value().Value(1));
  }
}

// --- Compound tests ---

TEST(BRepGraph_ChildExplorerTest, Compound_FaceCount)
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

  int aCount = 0;
  for (BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_CompoundId(0), BRepGraph_NodeId::Kind::Face);
       anExp.More(); anExp.Next())
    ++aCount;
  EXPECT_EQ(aCount, 12); // 6 + 6 faces
}

// --- Reverse path discovery tests ---

TEST(BRepGraph_ChildExplorerTest, PathsTo_SharedEdge_TwoPaths)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Each edge in a box is shared by 2 faces, so PathsTo should find 2 paths.
  const NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Paths().PathsTo(BRepGraph_EdgeId(0), pathAllocator());
  EXPECT_EQ(aPaths.Length(), 2);
}

TEST(BRepGraph_ChildExplorerTest, PathsTo_Face_SinglePath)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Each face appears once under a solid.
  const NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Paths().PathsTo(BRepGraph_FaceId(0), pathAllocator());
  EXPECT_EQ(aPaths.Length(), 1);
}

TEST(BRepGraph_ChildExplorerTest, PathsFromTo_FiltersByRoot)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Paths().PathsFromTo(BRepGraph_SolidId(0), BRepGraph_EdgeId(0), pathAllocator());
  EXPECT_EQ(aPaths.Length(), 2);
}

TEST(BRepGraph_ChildExplorerTest, PathsTo_MatchesExplorer)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // For each explorer result, PathsTo should contain the same path.
  BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());

  BRepGraph_NodeId aFirstEdge = anExp.Current();
  const NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Paths().PathsTo(aFirstEdge, pathAllocator());
  // The explorer path should be among PathsTo results.
  bool aFound = false;
  for (int i = 0; i < aPaths.Length(); ++i)
  {
    if (aPaths.Value(i) == anExp.CurrentPath(pathAllocator()))
    {
      aFound = true;
      break;
    }
  }
  EXPECT_TRUE(aFound);
}

TEST(BRepGraph_ChildExplorerTest, ForEachPathTo_MatchesPathsTo)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const occ::handle<NCollection_BaseAllocator> anAlloc = pathAllocator();
  NCollection_Vector<BRepGraph_TopologyPath>   aEnumerated(8, anAlloc);
  aGraph.Paths().ForEachPathTo(BRepGraph_EdgeId(0),
                               anAlloc,
                               [&](const BRepGraph_TopologyPath& thePath)
                               { aEnumerated.Append(BRepGraph_TopologyPath(thePath, anAlloc)); });

  const NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Paths().PathsTo(BRepGraph_EdgeId(0), anAlloc);
  expectSameSequence(aEnumerated, aPaths);
}

TEST(BRepGraph_ChildExplorerTest, ForEachPathFromTo_MatchesPathsFromTo)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const occ::handle<NCollection_BaseAllocator> anAlloc = pathAllocator();
  NCollection_Vector<BRepGraph_TopologyPath>   aEnumerated(8, anAlloc);
  aGraph.Paths().ForEachPathFromTo(BRepGraph_SolidId(0),
                                   BRepGraph_EdgeId(0),
                                   anAlloc,
                                   [&](const BRepGraph_TopologyPath& thePath)
                                   { aEnumerated.Append(BRepGraph_TopologyPath(thePath, anAlloc)); });

  const NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Paths().PathsFromTo(BRepGraph_SolidId(0), BRepGraph_EdgeId(0), anAlloc);
  expectSameSequence(aEnumerated, aPaths);
}

TEST(BRepGraph_ChildExplorerTest, PathsTo_MatchesExplorer_WhenEarlierCompoundRefRemoved)
{
  // Build a compound of two boxes to get two child refs under one parent.
  TopoDS_Compound aComp;
  BRep_Builder    aBB;
  aBB.MakeCompound(aComp);
  aBB.Add(aComp, BRepPrimAPI_MakeBox(10, 10, 10).Shape());
  aBB.Add(aComp, BRepPrimAPI_MakeBox(20, 20, 20).Shape());

  BRepGraph aGraph;
  aGraph.Build(aComp);
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraphInc::CompoundDef& aRootComp =
    aGraph.Topo().Compounds().Definition(BRepGraph_CompoundId(0));
  ASSERT_GE(aRootComp.ChildRefIds.Length(), 2);

  const BRepGraph_ChildRefId aRemovedRefId = aRootComp.ChildRefIds.Value(0);
  const BRepGraph_NodeId     aSecondChild =
    aGraph.Refs().Child(aRootComp.ChildRefIds.Value(1)).ChildDefId;
  ASSERT_TRUE(aSecondChild.IsValid());

  // Capture one face path under the second child before removal.
  BRepGraph_TopologyPath aExpectedPath;
  BRepGraph_NodeId       aLeafNode;
  bool                   aPathFound = false;
  for (BRepGraph_ChildExplorer anExp(aGraph,
                                BRepGraph_CompoundId(0),
                                BRepGraph_NodeId::Kind::Face);
       anExp.More();
       anExp.Next())
  {
    const BRepGraph_TopologyPath aPath = anExp.CurrentPath(pathAllocator());
    if (aPath.Depth() > 0 && aGraph.Paths().NodeAt(aPath, 0) == aSecondChild)
    {
      aExpectedPath = aPath;
      aLeafNode     = anExp.Current();
      aPathFound    = true;
      break;
    }
  }
  ASSERT_TRUE(aPathFound);
  ASSERT_TRUE(aLeafNode.IsValid());

  // Remove the first child ref (earlier ordinal).
  {
    BRepGraph_MutGuard<BRepGraphInc::ChildRef> aMut =
      aGraph.Builder().MutChildRef(aRemovedRefId);
    aMut->IsRemoved = true;
  }
  EXPECT_TRUE(aGraph.Refs().Child(aRemovedRefId).IsRemoved);

  // Explorer should keep raw step indices and still report the same path.
  bool aExplorerHasPath = false;
  for (BRepGraph_ChildExplorer anExp(aGraph,
                                BRepGraph_CompoundId(0),
                                BRepGraph_NodeId::Kind::Face);
       anExp.More();
       anExp.Next())
  {
    if (anExp.CurrentPath(pathAllocator()) == aExpectedPath)
    {
      aExplorerHasPath = true;
      break;
    }
  }
  EXPECT_TRUE(aExplorerHasPath);

  // Reverse walk should agree with explorer.
  const NCollection_Vector<BRepGraph_TopologyPath> aReversePaths =
    aGraph.Paths().PathsTo(aLeafNode, pathAllocator());
  bool aReverseHasPath = false;
  for (int i = 0; i < aReversePaths.Length(); ++i)
  {
    if (aReversePaths.Value(i) == aExpectedPath)
    {
      aReverseHasPath = true;
      break;
    }
  }
  EXPECT_TRUE(aReverseHasPath);
}

TEST(BRepGraph_ChildExplorerTest, PathsTo_MatchesExplorer_WhenFirstShellRefRemoved)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraphInc::SolidDef& aSolid = aGraph.Topo().Solids().Definition(BRepGraph_SolidId(0));
  ASSERT_GT(aSolid.ShellRefIds.Length(), 0);
  const BRepGraph_ShellId aShellId = aGraph.Refs().Shell(aSolid.ShellRefIds.Value(0)).ShellDefId;
  ASSERT_TRUE(aShellId.IsValid(aGraph.Topo().NbShells()));

  // Duplicate shell usage to create two shell refs to the same shell in one solid.
  aGraph.Builder().AddShellToSolid(BRepGraph_SolidId(0),
                                   BRepGraph_ShellId(aShellId.Index),
                                   TopAbs_FORWARD);
  const BRepGraphInc::SolidDef& aSolidAfterDup =
    aGraph.Topo().Solids().Definition(BRepGraph_SolidId(0));
  ASSERT_GE(aSolidAfterDup.ShellRefIds.Length(), 2);

  // Remove the first shell ref so the active one is not the first matching shell usage.
  const BRepGraph_ShellRefId aRemovedShellRefId = aSolidAfterDup.ShellRefIds.Value(0);
  {
    BRepGraph_MutGuard<BRepGraphInc::ShellRef> aMut =
      aGraph.Builder().MutShellRef(aRemovedShellRefId);
    aMut->IsRemoved = true;
  }
  EXPECT_TRUE(aGraph.Refs().Shell(aRemovedShellRefId).IsRemoved);

  // Pick one face path from explorer.
  BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Face);
  ASSERT_TRUE(anExp.More());
  const BRepGraph_TopologyPath aExpectedPath = anExp.CurrentPath(pathAllocator());
  const BRepGraph_NodeId       aFaceNode     = anExp.Current();
  ASSERT_TRUE(aFaceNode.IsValid());
  ASSERT_EQ(aFaceNode.NodeKind, BRepGraph_NodeId::Kind::Face);

  // Reverse walk must return the same path (not a removed shell-ref ordinal).
  const NCollection_Vector<BRepGraph_TopologyPath> aReversePaths =
    aGraph.Paths().PathsTo(aFaceNode, pathAllocator());
  bool aReverseHasPath = false;
  for (int i = 0; i < aReversePaths.Length(); ++i)
  {
    if (aReversePaths.Value(i) == aExpectedPath)
    {
      aReverseHasPath = true;
      break;
    }
  }
  EXPECT_TRUE(aReverseHasPath);
}

// --- Path filtering tests ---

TEST(BRepGraph_ChildExplorerTest, PathContains_Face_True)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());

  // The path from Solid to Edge should pass through some face.
  BRepGraph_NodeId aFace = anExp.NodeOf(BRepGraph_NodeId::Kind::Face);
  EXPECT_TRUE(aGraph.Paths().PathContains(anExp.CurrentPath(pathAllocator()), aFace));
}

TEST(BRepGraph_ChildExplorerTest, FilterByInclude_SharedEdge)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Get all paths to Edge(0), then filter by one of its faces.
  const NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Paths().PathsTo(BRepGraph_EdgeId(0), pathAllocator());
  ASSERT_EQ(aPaths.Length(), 2);

  // Find the face in the first path.
  BRepGraph_NodeId aFaceInPath0 =
    aGraph.Paths().LeafNode(aGraph.Paths().Truncated(aPaths.Value(0), 2, pathAllocator()));

  const NCollection_Vector<BRepGraph_TopologyPath> aFiltered =
    aGraph.Paths().FilterByInclude(aPaths, aFaceInPath0, pathAllocator());
  EXPECT_EQ(aFiltered.Length(), 1);
}

TEST(BRepGraph_ChildExplorerTest, FilterByExclude_Face)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Paths().PathsTo(BRepGraph_EdgeId(0), pathAllocator());
  ASSERT_EQ(aPaths.Length(), 2);

  BRepGraph_NodeId aFaceInPath0 =
    aGraph.Paths().LeafNode(aGraph.Paths().Truncated(aPaths.Value(0), 2, pathAllocator()));

  const NCollection_Vector<BRepGraph_TopologyPath> aExcluded =
    aGraph.Paths().FilterByExclude(aPaths, aFaceInPath0, pathAllocator());
  EXPECT_EQ(aExcluded.Length(), 1);
}

TEST(BRepGraph_ChildExplorerTest, FilterByIncludeExclude_AllocatorReturn)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const NCollection_Vector<BRepGraph_TopologyPath> aIncludedPaths =
    aGraph.Paths().PathsTo(BRepGraph_EdgeId(0), pathAllocator());
  ASSERT_EQ(aIncludedPaths.Length(), 2);

  const BRepGraph_NodeId aFaceInPath0 =
    aGraph.Paths().LeafNode(aGraph.Paths().Truncated(aIncludedPaths.Value(0), 2, pathAllocator()));

  const NCollection_Vector<BRepGraph_TopologyPath> anIncludedResult =
    aGraph.Paths().FilterByInclude(aIncludedPaths, aFaceInPath0, pathAllocator());
  EXPECT_EQ(anIncludedResult.Length(), 1);
  EXPECT_TRUE(aGraph.Paths().PathContains(anIncludedResult.Value(0), aFaceInPath0));

  const NCollection_Vector<BRepGraph_TopologyPath> aExcludedPaths =
    aGraph.Paths().PathsTo(BRepGraph_EdgeId(0), pathAllocator());
  ASSERT_EQ(aExcludedPaths.Length(), 2);

  const NCollection_Vector<BRepGraph_TopologyPath> anExcludedResult =
    aGraph.Paths().FilterByExclude(aExcludedPaths, aFaceInPath0, pathAllocator());
  EXPECT_EQ(anExcludedResult.Length(), 1);
  EXPECT_FALSE(aGraph.Paths().PathContains(anExcludedResult.Value(0), aFaceInPath0));
}

// --- Explorer convenience tests ---

TEST(BRepGraph_ChildExplorerTest, LocationOf_Kind_Face)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());

  // LocationOf(Face) should match LocationAt(face level).
  TopLoc_Location aFaceLoc = anExp.LocationOf(BRepGraph_NodeId::Kind::Face);
  int aFaceLevel =
    aGraph.Paths().FindLevel(anExp.CurrentPath(pathAllocator()), BRepGraph_NodeId::Kind::Face);
  TopLoc_Location aFaceLocAt = anExp.LocationAt(aFaceLevel);
  EXPECT_TRUE(aFaceLoc.IsEqual(aFaceLocAt));
}

TEST(BRepGraph_ChildExplorerTest, NodeOf_Kind_Face)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());

  BRepGraph_NodeId aFace = anExp.NodeOf(BRepGraph_NodeId::Kind::Face);
  EXPECT_TRUE(aFace.IsValid());
  EXPECT_EQ(aFace.NodeKind, BRepGraph_NodeId::Kind::Face);
}

TEST(BRepGraph_ChildExplorerTest, NodeLocations_EdgeHasTwoOccurrences)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Each box edge is shared by 2 faces = 2 occurrence locations.
  const NCollection_Vector<BRepGraph::PathView::OccurrenceEntry> aEntries =
    aGraph.Paths().NodeLocations(BRepGraph_EdgeId(0), pathAllocator());
  EXPECT_EQ(aEntries.Length(), 2);

  // Both locations should be identity for a simple box.
  for (int i = 0; i < aEntries.Length(); ++i)
    EXPECT_TRUE(aEntries.Value(i).Location.IsIdentity());
}

TEST(BRepGraph_ChildExplorerTest, NodeLocations_FaceHasOneOccurrence)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const NCollection_Vector<BRepGraph::PathView::OccurrenceEntry> aEntries =
    aGraph.Paths().NodeLocations(BRepGraph_FaceId(0), pathAllocator());
  EXPECT_EQ(aEntries.Length(), 1);
}

TEST(BRepGraph_ChildExplorerTest, NodeLocations_VertexMultipleOccurrences)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // A box corner vertex is shared by 3 edges, each shared by 2 faces.
  const NCollection_Vector<BRepGraph::PathView::OccurrenceEntry> aEntries =
    aGraph.Paths().NodeLocations(BRepGraph_VertexId(0), pathAllocator());
  EXPECT_GT(aEntries.Length(), 0);
}

TEST(BRepGraph_ChildExplorerTest, ForEachNodeLocation_MatchesNodeLocations)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const occ::handle<NCollection_BaseAllocator> anAlloc = pathAllocator();
  NCollection_Vector<BRepGraph::PathView::OccurrenceEntry> aEnumerated(8, anAlloc);
  aGraph.Paths().ForEachNodeLocation(BRepGraph_EdgeId(0),
                                     anAlloc,
                                     [&](const BRepGraph::PathView::OccurrenceEntry& theEntry)
                                     {
                                       aEnumerated.Append(
                                         BRepGraph::PathView::OccurrenceEntry{BRepGraph_TopologyPath(
                                                                                theEntry.Path,
                                                                                anAlloc),
                                                                              theEntry.Location,
                                                                              theEntry.Orientation});
                                     });

  const NCollection_Vector<BRepGraph::PathView::OccurrenceEntry> aEntries =
    aGraph.Paths().NodeLocations(BRepGraph_EdgeId(0), anAlloc);
  expectSameOccurrenceEntries(aEnumerated, aEntries);
}

TEST(BRepGraph_ChildExplorerTest, PathsTo_InvalidNode_Empty)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Invalid NodeId should return empty.
  const NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Paths().PathsTo(BRepGraph_NodeId(), pathAllocator());
  EXPECT_EQ(aPaths.Length(), 0);

  // Out-of-range index should return empty.
  const NCollection_Vector<BRepGraph_TopologyPath> aPaths2 =
    aGraph.Paths().PathsTo(BRepGraph_EdgeId(99999), pathAllocator());
  EXPECT_EQ(aPaths2.Length(), 0);
}

TEST(BRepGraph_ChildExplorerTest, PathView_DeterministicComputedQueries)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const NCollection_Vector<BRepGraph_TopologyPath> aPathsToFirst =
    aGraph.Paths().PathsTo(BRepGraph_EdgeId(0), pathAllocator());
  const NCollection_Vector<BRepGraph_TopologyPath> aPathsToSecond =
    aGraph.Paths().PathsTo(BRepGraph_EdgeId(0), pathAllocator());
  expectSameSequence(aPathsToFirst, aPathsToSecond);

  const NCollection_Vector<BRepGraph_TopologyPath> aPathsFromToFirst =
    aGraph.Paths().PathsFromTo(BRepGraph_SolidId(0), BRepGraph_EdgeId(0), pathAllocator());
  const NCollection_Vector<BRepGraph_TopologyPath> aPathsFromToSecond =
    aGraph.Paths().PathsFromTo(BRepGraph_SolidId(0), BRepGraph_EdgeId(0), pathAllocator());
  expectSameSequence(aPathsFromToFirst, aPathsFromToSecond);

  ASSERT_GT(aPathsToFirst.Length(), 0);
  const BRepGraph_NodeId aFaceInPath0 =
    aGraph.Paths().LeafNode(
      aGraph.Paths().Truncated(aPathsToFirst.Value(0), 2, pathAllocator()));

  const NCollection_Vector<BRepGraph_TopologyPath> aIncludeFirst =
    aGraph.Paths().FilterByInclude(aPathsToFirst, aFaceInPath0, pathAllocator());
  const NCollection_Vector<BRepGraph_TopologyPath> aIncludeSecond =
    aGraph.Paths().FilterByInclude(aPathsToFirst, aFaceInPath0, pathAllocator());
  expectSameSequence(aIncludeFirst, aIncludeSecond);

  const NCollection_Vector<BRepGraph_TopologyPath> aExcludeFirst =
    aGraph.Paths().FilterByExclude(aPathsToFirst, aFaceInPath0, pathAllocator());
  const NCollection_Vector<BRepGraph_TopologyPath> aExcludeSecond =
    aGraph.Paths().FilterByExclude(aPathsToFirst, aFaceInPath0, pathAllocator());
  expectSameSequence(aExcludeFirst, aExcludeSecond);

  const NCollection_Vector<BRepGraph::PathView::OccurrenceEntry> aLocationsFirst =
    aGraph.Paths().NodeLocations(BRepGraph_EdgeId(0), pathAllocator());
  const NCollection_Vector<BRepGraph::PathView::OccurrenceEntry> aLocationsSecond =
    aGraph.Paths().NodeLocations(BRepGraph_EdgeId(0), pathAllocator());
  expectSameOccurrenceEntries(aLocationsFirst, aLocationsSecond);

  const NCollection_Vector<BRepGraph_NodeId> aNodesFirst =
    aGraph.Paths().AllNodesOnPath(aPathsToFirst.Value(0), pathAllocator());
  const NCollection_Vector<BRepGraph_NodeId> aNodesSecond =
    aGraph.Paths().AllNodesOnPath(aPathsToFirst.Value(0), pathAllocator());
  expectSameSequence(aNodesFirst, aNodesSecond);
}

TEST(BRepGraph_ChildExplorerTest, PathView_InvalidInputsReturnEmpty)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const NCollection_Vector<BRepGraph_TopologyPath> aPathsToEdge =
    aGraph.Paths().PathsTo(BRepGraph_EdgeId(0), pathAllocator());
  EXPECT_EQ(aPathsToEdge.Length(), 2);
  EXPECT_EQ(aGraph.Paths().PathsTo(BRepGraph_NodeId(), pathAllocator()).Length(), 0);

  EXPECT_EQ(aGraph.Paths().PathsFromTo(BRepGraph_SolidId(0), BRepGraph_EdgeId(0), pathAllocator()).Length(), 2);
  EXPECT_EQ(aGraph.Paths().PathsFromTo(BRepGraph_NodeId(), BRepGraph_EdgeId(0), pathAllocator()).Length(), 0);

  const NCollection_Vector<BRepGraph_TopologyPath> aInputPaths =
    aGraph.Paths().PathsTo(BRepGraph_EdgeId(0), pathAllocator());
  ASSERT_EQ(aInputPaths.Length(), 2);
  const BRepGraph_NodeId aFaceInPath0 =
    aGraph.Paths().LeafNode(aGraph.Paths().Truncated(aInputPaths.Value(0), 2, pathAllocator()));

  EXPECT_EQ(aGraph.Paths().FilterByInclude(aInputPaths, aFaceInPath0, pathAllocator()).Length(), 1);
  EXPECT_EQ(aGraph.Paths().FilterByInclude(aInputPaths, BRepGraph_NodeId(), pathAllocator()).Length(), 0);

  EXPECT_EQ(aGraph.Paths().FilterByExclude(aInputPaths, aFaceInPath0, pathAllocator()).Length(), 1);
  EXPECT_EQ(aGraph.Paths().FilterByExclude(aInputPaths, BRepGraph_NodeId(), pathAllocator()).Length(),
            aInputPaths.Length());

  EXPECT_EQ(aGraph.Paths().NodeLocations(BRepGraph_EdgeId(0), pathAllocator()).Length(), 2);
  EXPECT_EQ(aGraph.Paths().NodeLocations(BRepGraph_NodeId(), pathAllocator()).Length(), 0);

  EXPECT_EQ(aGraph.Paths().AllNodesOnPath(BRepGraph_TopologyPath(), pathAllocator()).Length(), 0);
  EXPECT_GT(aGraph.Paths().AllNodesOnPath(aInputPaths.Value(0), pathAllocator()).Length(), 0);
}

TEST(BRepGraph_ChildExplorerTest, PathView_TempAllocator_Reuse)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const occ::handle<NCollection_IncAllocator> anAlloc = new NCollection_IncAllocator();

  NCollection_Vector<BRepGraph_TopologyPath> aPaths(8, anAlloc);
  aPaths = aGraph.Paths().PathsTo(BRepGraph_EdgeId(0), anAlloc);
  ASSERT_EQ(aPaths.Length(), 2);

  aPaths.Clear(false);
  anAlloc->Reset(false);

  aPaths = aGraph.Paths().PathsTo(BRepGraph_FaceId(0), anAlloc);
  ASSERT_EQ(aPaths.Length(), 1);

  NCollection_Vector<BRepGraph::PathView::OccurrenceEntry> aEntries(8, anAlloc);
  aEntries = aGraph.Paths().NodeLocations(BRepGraph_EdgeId(0), anAlloc);
  ASSERT_EQ(aEntries.Length(), 2);
}

// --- Depth guard and re-initialization tests ---

TEST(BRepGraph_ChildExplorerTest, DeepCompound_NoStackOverflow)
{
  // Build a deeply nested compound: Compound(Compound(Compound(...(Box)...))).
  TopoDS_Shape  aInner            = BRepPrimAPI_MakeBox(1, 1, 1).Shape();
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
  int aCount = 0;
  for (BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_CompoundId(0), BRepGraph_NodeId::Kind::Face);
       anExp.More(); anExp.Next())
    ++aCount;
  EXPECT_EQ(aCount, 6);
}

TEST(BRepGraph_ChildExplorerTest, Recreate_ResetAndReexplore)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aFaceCount = 0;
  BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Face);
  for (; anExp.More(); anExp.Next())
    ++aFaceCount;
  EXPECT_EQ(aFaceCount, 6);

  // Recreate targeting edges.
  int aEdgeCount = 0;
  for (BRepGraph_ChildExplorer anEdgeExp(aGraph,
                                         BRepGraph_SolidId(0),
                                         BRepGraph_NodeId::Kind::Edge);
       anEdgeExp.More(); anEdgeExp.Next())
    ++aEdgeCount;
  EXPECT_EQ(aEdgeCount, 24);
}

// --- Reverse walk with Compound/CompSolid parents ---

TEST(BRepGraph_ChildExplorerTest, PathsTo_NestedCompound_HasCompoundRoot)
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
  const NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Paths().PathsTo(BRepGraph_FaceId(0), pathAllocator());
  ASSERT_GT(aPaths.Length(), 0);
  // Root should be the outer compound (index 0 - outermost).
  EXPECT_EQ(aPaths.Value(0).Root().NodeKind, BRepGraph_NodeId::Kind::Compound);
}

TEST(BRepGraph_ChildExplorerTest, NodeLocations_CompoundChild_ComposedLocation)
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
  int aCount = 0;
  for (BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_CompoundId(0), BRepGraph_NodeId::Kind::Face);
       anExp.More(); anExp.Next())
    ++aCount;
  EXPECT_EQ(aCount, 6);
}

TEST(BRepGraph_ChildExplorerTest, PathsTo_ShellInCompound_CompoundRoot)
{
  // Build a compound containing a bare shell (extracted from a box).
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10, 10, 10).Shape();
  TopExp_Explorer    aShellExp(aBox, TopAbs_SHELL);
  ASSERT_TRUE(aShellExp.More());
  const TopoDS_Shape& aShell = aShellExp.Current();

  TopoDS_Compound aComp;
  BRep_Builder    aBB;
  aBB.MakeCompound(aComp);
  aBB.Add(aComp, aShell);

  BRepGraph aGraph;
  aGraph.Build(aComp);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Topo().NbShells(), 0);

  // PathsTo(Face) should trace through Shell to Compound root.
  const NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Paths().PathsTo(BRepGraph_FaceId(0), pathAllocator());
  ASSERT_GT(aPaths.Length(), 0);
  EXPECT_EQ(aPaths.Value(0).Root().NodeKind, BRepGraph_NodeId::Kind::Compound);
}

TEST(BRepGraph_ChildExplorerTest, PathsTo_SolidInCompSolid_CompSolidRoot)
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
  ASSERT_GT(aGraph.Topo().NbCompSolids(), 0);

  // PathsTo(Face) should trace up through Solid to CompSolid root.
  const NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Paths().PathsTo(BRepGraph_FaceId(0), pathAllocator());
  ASSERT_GT(aPaths.Length(), 0);
  EXPECT_EQ(aPaths.Value(0).Root().NodeKind, BRepGraph_NodeId::Kind::CompSolid);
}

TEST(BRepGraph_ChildExplorerTest, PathsTo_CompSolidInCompound_CompoundRoot)
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
  ASSERT_GT(aGraph.Topo().NbCompSolids(), 0);
  ASSERT_GT(aGraph.Topo().NbCompounds(), 0);

  // PathsTo(Face) should trace Solid -> CompSolid -> Compound root.
  const NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Paths().PathsTo(BRepGraph_FaceId(0), pathAllocator());
  ASSERT_GT(aPaths.Length(), 0);
  EXPECT_EQ(aPaths.Value(0).Root().NodeKind, BRepGraph_NodeId::Kind::Compound);
}

// ============ Phase 7: Tests for new PathView methods ============

TEST(BRepGraph_ChildExplorerTest, OrientationAt_IntermediateLevel)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());
  const BRepGraph_TopologyPath aPath = anExp.CurrentPath(pathAllocator());

  TopAbs_Orientation anOri = aGraph.Paths().OrientationAt(aPath, 0);
  EXPECT_TRUE(anOri == TopAbs_FORWARD || anOri == TopAbs_REVERSED);
}

TEST(BRepGraph_ChildExplorerTest, TopologyPath_Truncated_KeepsPrefix)
{
  BRepGraph_TopologyPath aPath{BRepGraph_SolidId(0)};
  // Solid(0) -> Shell(ref 0) -> Face(ref 0) -> Wire(ref 0) -> CoEdge(ref 0)
  aPath = BRepGraph_TopologyPath::ToEdge(0, 0, 0, 0, 0, pathAllocator());
  EXPECT_EQ(aPath.Depth(), 4);

  BRepGraph_TopologyPath aTrunc = aPath.Truncated(2, pathAllocator());
  EXPECT_EQ(aTrunc.Depth(), 2);
  EXPECT_EQ(aTrunc.Root(), aPath.Root());

  // Truncating beyond depth returns a copy.
  BRepGraph_TopologyPath aFull = aPath.Truncated(100, pathAllocator());
  EXPECT_EQ(aFull.Depth(), 4);
  EXPECT_EQ(aFull, aPath);
}

TEST(BRepGraph_ChildExplorerTest, DepthOfKind_Face_CountsOne)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());
  const BRepGraph_TopologyPath aPath = anExp.CurrentPath(pathAllocator());

  // Path Solid->Shell->Face->Wire->CoEdge(->Edge) should have exactly 1 Face.
  EXPECT_EQ(aGraph.Paths().DepthOfKind(aPath, BRepGraph_NodeId::Kind::Face), 1);
  EXPECT_EQ(aGraph.Paths().DepthOfKind(aPath, BRepGraph_NodeId::Kind::Shell), 1);
  EXPECT_EQ(aGraph.Paths().DepthOfKind(aPath, BRepGraph_NodeId::Kind::Solid), 1);
}

TEST(BRepGraph_ChildExplorerTest, CommonAncestor_SharedFace)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Get two edge paths from the same face.
  BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());
  const BRepGraph_TopologyPath aPath1 = anExp.CurrentPath(pathAllocator());
  anExp.Next();
  ASSERT_TRUE(anExp.More());
  const BRepGraph_TopologyPath aPath2 = anExp.CurrentPath(pathAllocator());

  BRepGraph_TopologyPath aCommon = aGraph.Paths().CommonAncestor(aPath1, aPath2,
                                                                 pathAllocator());
  EXPECT_TRUE(aCommon.IsValid());
  // Common ancestor should be at least the solid root.
  EXPECT_EQ(aCommon.Root(), aPath1.Root());
  EXPECT_LE(aCommon.Depth(), aPath1.Depth());
  EXPECT_LE(aCommon.Depth(), aPath2.Depth());
}

TEST(BRepGraph_ChildExplorerTest, CommonAncestor_DifferentRoots_Invalid)
{
  BRepGraph_TopologyPath aPath1{BRepGraph_SolidId(0)};
  BRepGraph_TopologyPath aPath2{BRepGraph_SolidId(1)};

  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_TopologyPath aCommon = aGraph.Paths().CommonAncestor(aPath1, aPath2,
                                                                 pathAllocator());
  EXPECT_FALSE(aCommon.IsValid());
}

TEST(BRepGraph_ChildExplorerTest, IsAncestorOf_TrueForPrefix)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());
  const BRepGraph_TopologyPath aEdgePath = anExp.CurrentPath(pathAllocator());

  // Truncated to face level should be an ancestor.
  BRepGraph_TopologyPath aFacePath = aEdgePath.Truncated(2, pathAllocator());
  EXPECT_TRUE(aGraph.Paths().IsAncestorOf(aFacePath, aEdgePath));
  EXPECT_FALSE(aGraph.Paths().IsAncestorOf(aEdgePath, aFacePath));
}

TEST(BRepGraph_ChildExplorerTest, IsAncestorOf_FalseForNonPrefix)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Two different edge paths are not ancestors of each other.
  BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());
  const BRepGraph_TopologyPath aPath1 = anExp.CurrentPath(pathAllocator());
  // Skip to an edge from a different face.
  for (int i = 0; i < 5 && anExp.More(); ++i)
    anExp.Next();
  if (anExp.More())
  {
    const BRepGraph_TopologyPath aPath2 = anExp.CurrentPath(pathAllocator());
    if (aPath1 != aPath2)
    {
      EXPECT_FALSE(aGraph.Paths().IsAncestorOf(aPath1, aPath2));
    }
  }
}

TEST(BRepGraph_ChildExplorerTest, AllNodesOnPath_BoxEdge_CorrectKinds)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());
  const BRepGraph_TopologyPath aPath = anExp.CurrentPath(pathAllocator());

  const NCollection_Vector<BRepGraph_NodeId> aNodes =
    aGraph.Paths().AllNodesOnPath(aPath, pathAllocator());
  ASSERT_GT(aNodes.Length(), 0);

  // First node should be the solid root.
  EXPECT_EQ(aNodes.Value(0).NodeKind, BRepGraph_NodeId::Kind::Solid);

  // Last node should be an edge (CoEdge->Edge 1:1 expansion).
  EXPECT_EQ(aNodes.Value(aNodes.Length() - 1).NodeKind, BRepGraph_NodeId::Kind::Edge);

  // Should contain Shell, Face, Wire, CoEdge kinds somewhere in the middle.
  bool aHasShell = false, aHasFace = false, aHasWire = false, aHasCoEdge = false;
  for (int i = 0; i < aNodes.Length(); ++i)
  {
    if (aNodes.Value(i).NodeKind == BRepGraph_NodeId::Kind::Shell)
      aHasShell = true;
    if (aNodes.Value(i).NodeKind == BRepGraph_NodeId::Kind::Face)
      aHasFace = true;
    if (aNodes.Value(i).NodeKind == BRepGraph_NodeId::Kind::Wire)
      aHasWire = true;
    if (aNodes.Value(i).NodeKind == BRepGraph_NodeId::Kind::CoEdge)
      aHasCoEdge = true;
  }
  EXPECT_TRUE(aHasShell);
  EXPECT_TRUE(aHasFace);
  EXPECT_TRUE(aHasWire);
  EXPECT_TRUE(aHasCoEdge);
}

// ============ Edge-case tests for invalid inputs ============

TEST(BRepGraph_ChildExplorerTest, DepthOfKind_InvalidPath_ReturnsZero)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_TopologyPath anInvalid;
  EXPECT_EQ(aGraph.Paths().DepthOfKind(anInvalid, BRepGraph_NodeId::Kind::Face), 0);
}

TEST(BRepGraph_ChildExplorerTest, CommonAncestor_EmptyPaths_Invalid)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_TopologyPath anEmpty;
  BRepGraph_TopologyPath aValid{BRepGraph_SolidId(0)};

  EXPECT_FALSE(aGraph.Paths().CommonAncestor(anEmpty, aValid, pathAllocator()).IsValid());
  EXPECT_FALSE(aGraph.Paths().CommonAncestor(aValid, anEmpty, pathAllocator()).IsValid());
  EXPECT_FALSE(aGraph.Paths().CommonAncestor(anEmpty, anEmpty, pathAllocator()).IsValid());
}

TEST(BRepGraph_ChildExplorerTest, IsAncestorOf_InvalidPaths_ReturnsFalse)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_TopologyPath anEmpty;
  BRepGraph_TopologyPath aValid{BRepGraph_SolidId(0)};

  EXPECT_FALSE(aGraph.Paths().IsAncestorOf(anEmpty, aValid));
  EXPECT_FALSE(aGraph.Paths().IsAncestorOf(aValid, anEmpty));
}

TEST(BRepGraph_ChildExplorerTest, AllNodesOnPath_InvalidPath_Empty)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_TopologyPath anInvalid;
  const NCollection_Vector<BRepGraph_NodeId> aNodes =
    aGraph.Paths().AllNodesOnPath(anInvalid, pathAllocator());
  EXPECT_EQ(aNodes.Length(), 0);
}

TEST(BRepGraph_ChildExplorerTest, ReverseWalkPaths_DepthBudgetZero_ReturnsEmpty)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Normal PathsTo should find paths.
  const NCollection_Vector<BRepGraph_TopologyPath> aNormal =
    aGraph.Paths().PathsTo(BRepGraph_FaceId(0), pathAllocator());
  EXPECT_GT(aNormal.Length(), 0);

  // PathsTo with valid node should terminate even on large graphs.
  const NCollection_Vector<BRepGraph_TopologyPath> aVertex =
    aGraph.Paths().PathsTo(BRepGraph_VertexId(0), pathAllocator());
  EXPECT_GT(aVertex.Length(), 0);
}

// ============ Regression tests: CoEdge/Occurrence target reachability ============

TEST(BRepGraph_ChildExplorerTest, CoEdgeTarget_Reachable)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // CoEdge target from Solid must find all coedges (24 edge occurrences = 24 coedges).
  int aCount = 0;
  for (BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::CoEdge);
       anExp.More(); anExp.Next())
  {
    EXPECT_EQ(anExp.Current().NodeKind, BRepGraph_NodeId::Kind::CoEdge);
    ++aCount;
  }
  EXPECT_EQ(aCount, 24);
}

TEST(BRepGraph_ChildExplorerTest, CoEdgeTarget_FromFace_Count4)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_FaceId(0), BRepGraph_NodeId::Kind::CoEdge);
       anExp.More(); anExp.Next())
  {
    EXPECT_EQ(anExp.Current().NodeKind, BRepGraph_NodeId::Kind::CoEdge);
    ++aCount;
  }
  EXPECT_EQ(aCount, 4);
}

TEST(BRepGraph_ChildExplorerTest, DirectChildren_ShellFaces_CountAndOrder)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_ShellId aShellId(0);
  const NCollection_Vector<BRepGraph_FaceRefId>& aFaceRefIds = aGraph.Refs().FaceRefIdsOf(aShellId);

  NCollection_Vector<int> anExpectedFaceIds;
  for (int i = 0; i < aFaceRefIds.Length(); ++i)
  {
    const BRepGraphInc::FaceRef& aRef = aGraph.Refs().Face(aFaceRefIds.Value(i));
    if (!aRef.IsRemoved)
      anExpectedFaceIds.Append(aRef.FaceDefId.Index);
  }

  NCollection_Vector<int> anActualFaceIds;
  for (BRepGraph_ChildExplorer anIt =
         makeDirectChildExplorer(aGraph, aShellId, BRepGraph_NodeId::Kind::Face);
       anIt.More(); anIt.Next())
  {
    ASSERT_EQ(anIt.Current().NodeKind, BRepGraph_NodeId::Kind::Face);
    anActualFaceIds.Append(anIt.Current().Index);
  }

  ASSERT_EQ(anActualFaceIds.Length(), anExpectedFaceIds.Length());
  for (int i = 0; i < anExpectedFaceIds.Length(); ++i)
    EXPECT_EQ(anActualFaceIds.Value(i), anExpectedFaceIds.Value(i));
}

TEST(BRepGraph_ChildExplorerTest, DirectChildren_RemovedFaceRef_IsSkipped)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_ShellId aShellId(0);
  const NCollection_Vector<BRepGraph_FaceRefId>& aFaceRefIds = aGraph.Refs().FaceRefIdsOf(aShellId);
  ASSERT_GT(aFaceRefIds.Length(), 0);

  const BRepGraph_FaceRefId aRemovedRef = aFaceRefIds.Value(0);
  const BRepGraph_FaceId aRemovedFaceId = aGraph.Refs().Face(aRemovedRef).FaceDefId;

  {
    BRepGraph_MutGuard<BRepGraphInc::FaceRef> aFaceRef = aGraph.Builder().MutFaceRef(aRemovedRef);
    aFaceRef->IsRemoved = true;
  }

  int aCount = 0;
  for (BRepGraph_ChildExplorer anIt =
         makeDirectChildExplorer(aGraph, aShellId, BRepGraph_NodeId::Kind::Face);
       anIt.More(); anIt.Next())
  {
    EXPECT_NE(anIt.Current(), BRepGraph_NodeId(aRemovedFaceId));
    ++aCount;
  }

  EXPECT_EQ(aCount, aFaceRefIds.Length() - 1);
}

TEST(BRepGraph_ChildExplorerTest, DirectChildren_WireChildren_AreResolvedEdges)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_FaceId aFaceId(0);
  const NCollection_Vector<BRepGraph_WireRefId>& aWireRefs = aGraph.Refs().WireRefIdsOf(aFaceId);
  ASSERT_GT(aWireRefs.Length(), 0);

  const BRepGraph_WireId aWireId = aGraph.Refs().Wire(aWireRefs.Value(0)).WireDefId;
  const NCollection_Vector<BRepGraph_CoEdgeRefId>& aCoEdgeRefs = aGraph.Refs().CoEdgeRefIdsOf(aWireId);

  int aCount = 0;
  for (BRepGraph_ChildExplorer anIt =
         makeDirectChildExplorer(aGraph, aWireId, BRepGraph_NodeId::Kind::Edge);
       anIt.More(); anIt.Next())
  {
    EXPECT_EQ(anIt.Current().NodeKind, BRepGraph_NodeId::Kind::Edge);
    ++aCount;
  }

  EXPECT_EQ(aCount, aCoEdgeRefs.Length());
}

TEST(BRepGraph_ChildExplorerTest, DirectChildren_CompoundChildren_Basic)
{
  TopoDS_Compound aComp;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aComp);
  aBuilder.Add(aComp, BRepPrimAPI_MakeBox(10, 10, 10).Shape());
  aBuilder.Add(aComp, BRepPrimAPI_MakeBox(20, 20, 20).Shape());

  BRepGraph aGraph;
  aGraph.Build(aComp);
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (BRepGraph_ChildExplorer anIt =
         makeDirectChildExplorer(aGraph, BRepGraph_CompoundId(0), BRepGraph_NodeId::Kind::Solid);
       anIt.More(); anIt.Next())
  {
    EXPECT_TRUE(BRepGraph_NodeId::IsTopologyKind(anIt.Current().NodeKind));
    ++aCount;
  }

  EXPECT_EQ(aCount, 2);
}

TEST(BRepGraph_ChildExplorerTest, DirectChildren_ChainedTraversal_ParityWithRecursiveTraversal)
{
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(100.0, 0.0, 0.0));

  TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10, 20, 30).Shape();
  aBox.Move(TopLoc_Location(aTrsf));

  TopoDS_Compound aComp;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aComp);
  aBuilder.Add(aComp, aBox);

  BRepGraph aGraph;
  aGraph.Build(aComp);
  ASSERT_TRUE(aGraph.IsDone());

  NCollection_DataMap<int, TopLoc_Location>      aExpectedLoc;
  NCollection_DataMap<int, TopAbs_Orientation>   aExpectedOri;
  for (BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_CompoundId(0), BRepGraph_NodeId::Kind::Face);
       anExp.More(); anExp.Next())
  {
    const int aFaceIdx = anExp.Current().Index;
    if (!aExpectedLoc.IsBound(aFaceIdx))
    {
      aExpectedLoc.Bind(aFaceIdx, anExp.Location());
      aExpectedOri.Bind(aFaceIdx, anExp.Orientation());
    }
  }

  int aVisited = 0;
  for (BRepGraph_ChildExplorer aSolidIt =
         makeDirectChildExplorer(aGraph, BRepGraph_CompoundId(0), BRepGraph_NodeId::Kind::Solid);
       aSolidIt.More(); aSolidIt.Next())
  {
    for (BRepGraph_ChildExplorer aShellIt = makeDirectChildExplorer(aGraph,
                                                                    aSolidIt.Current(),
                                                                    BRepGraph_NodeId::Kind::Shell,
                                                                    aSolidIt.Location(),
                                                                    aSolidIt.Orientation());
         aShellIt.More(); aShellIt.Next())
    {
      for (BRepGraph_ChildExplorer aFaceIt = makeDirectChildExplorer(aGraph,
                                                                     aShellIt.Current(),
                                                                     BRepGraph_NodeId::Kind::Face,
                                                                     aShellIt.Location(),
                                                                     aShellIt.Orientation());
           aFaceIt.More(); aFaceIt.Next())
      {
        ASSERT_EQ(aFaceIt.Current().NodeKind, BRepGraph_NodeId::Kind::Face);
        const int aFaceIdx = aFaceIt.Current().Index;
        ASSERT_TRUE(aExpectedLoc.IsBound(aFaceIdx));
        EXPECT_TRUE(aFaceIt.Location().IsEqual(aExpectedLoc.Find(aFaceIdx)));
        EXPECT_EQ(aFaceIt.Orientation(), aExpectedOri.Find(aFaceIdx));
        ++aVisited;
      }
    }
  }

  EXPECT_EQ(aVisited, aExpectedLoc.Extent());
}

TEST(BRepGraph_ChildExplorerTest, DirectChildren_SharedProduct_ChildrenHaveDistinctContexts)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_ProductId aPart = aGraph.Builder().AddProduct(BRepGraph_SolidId(0));
  ASSERT_TRUE(aPart.IsValid());
  const BRepGraph_ProductId anAssembly = aGraph.Builder().AddAssemblyProduct();
  ASSERT_TRUE(anAssembly.IsValid());

  gp_Trsf aT1;
  aT1.SetTranslation(gp_Vec(10.0, 0.0, 0.0));
  gp_Trsf aT2;
  aT2.SetTranslation(gp_Vec(20.0, 0.0, 0.0));

  const BRepGraph_OccurrenceId anOcc1 =
    aGraph.Builder().AddOccurrence(anAssembly, aPart, TopLoc_Location(aT1));
  const BRepGraph_OccurrenceId anOcc2 =
    aGraph.Builder().AddOccurrence(anAssembly, aPart, TopLoc_Location(aT2));
  ASSERT_TRUE(anOcc1.IsValid());
  ASSERT_TRUE(anOcc2.IsValid());

  int             aCount = 0;
  TopLoc_Location aLoc1;
  TopLoc_Location aLoc2;
  for (BRepGraph_ChildExplorer anIt =
         makeDirectChildExplorer(aGraph, anAssembly, BRepGraph_NodeId::Kind::Solid);
       anIt.More(); anIt.Next())
  {
    ASSERT_EQ(anIt.Current(), BRepGraph_NodeId(BRepGraph_SolidId(0)));
    if (aCount == 0)
      aLoc1 = anIt.Location();
    else if (aCount == 1)
      aLoc2 = anIt.Location();
    ++aCount;
  }

  ASSERT_EQ(aCount, 2);
  EXPECT_FALSE(aLoc1.IsEqual(aLoc2));
}

TEST(BRepGraph_ChildExplorerTest, DirectChildren_ProductPartRootContext_ComposedWithOccurrence)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_ProductId aPart = aGraph.Builder().AddProduct(BRepGraph_SolidId(0));
  ASSERT_TRUE(aPart.IsValid());
  const BRepGraph_ProductId anAssembly = aGraph.Builder().AddAssemblyProduct();
  ASSERT_TRUE(anAssembly.IsValid());

  gp_Trsf aOccTrsf;
  aOccTrsf.SetTranslation(gp_Vec(10.0, 0.0, 0.0));
  const BRepGraph_OccurrenceId anOcc =
    aGraph.Builder().AddOccurrence(anAssembly, aPart, TopLoc_Location(aOccTrsf));
  ASSERT_TRUE(anOcc.IsValid());

  gp_Trsf aRootTrsf;
  aRootTrsf.SetTranslation(gp_Vec(0.0, 20.0, 0.0));
  {
    BRepGraph_MutGuard<BRepGraphInc::ProductDef> aMutPart = aGraph.Builder().MutProduct(aPart);
    aMutPart->RootLocation    = TopLoc_Location(aRootTrsf);
    aMutPart->RootOrientation = TopAbs_REVERSED;
  }

  BRepGraph_ChildExplorer anIt =
    makeDirectChildExplorer(aGraph, anAssembly, BRepGraph_NodeId::Kind::Solid);
  ASSERT_TRUE(anIt.More());
  EXPECT_EQ(anIt.Current(), BRepGraph_NodeId(BRepGraph_SolidId(0)));

  const TopLoc_Location anExpectedLoc = TopLoc_Location(aOccTrsf) * TopLoc_Location(aRootTrsf);
  const gp_Trsf&        anActualTrsf  = anIt.Location().Transformation();
  const gp_Trsf&        anExpectedTrsf = anExpectedLoc.Transformation();
  EXPECT_NEAR(anActualTrsf.TranslationPart().X(),
              anExpectedTrsf.TranslationPart().X(),
              Precision::Confusion());
  EXPECT_NEAR(anActualTrsf.TranslationPart().Y(),
              anExpectedTrsf.TranslationPart().Y(),
              Precision::Confusion());
  EXPECT_NEAR(anActualTrsf.TranslationPart().Z(),
              anExpectedTrsf.TranslationPart().Z(),
              Precision::Confusion());
  EXPECT_EQ(anIt.Orientation(), TopAbs_REVERSED);

  anIt.Next();
  EXPECT_FALSE(anIt.More());
}

TEST(BRepGraph_ChildExplorerTest, DirectChildren_HighFanout_DirectChildrenComplete)
{
  TopoDS_Compound aComp;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aComp);

  constexpr int THE_NB_CHILDREN = 80;
  for (int i = 0; i < THE_NB_CHILDREN; ++i)
    aBuilder.Add(aComp, BRepPrimAPI_MakeBox(1.0 + i, 2.0, 3.0).Shape());

  BRepGraph aGraph;
  aGraph.Build(aComp);
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (BRepGraph_ChildExplorer anIt =
         makeDirectChildExplorer(aGraph, BRepGraph_CompoundId(0), BRepGraph_NodeId::Kind::Solid);
       anIt.More(); anIt.Next())
    ++aCount;

  EXPECT_EQ(aCount, THE_NB_CHILDREN);
}

TEST(BRepGraph_ChildExplorerTest, HighFanout_CompletesAllChildren)
{
  // Build a compound with many children to verify no premature traversal termination.
  TopoDS_Compound aComp;
  BRep_Builder    aBB;
  aBB.MakeCompound(aComp);
  constexpr int THE_NB_CHILDREN = 500;
  for (int i = 0; i < THE_NB_CHILDREN; ++i)
    aBB.Add(aComp, BRepPrimAPI_MakeBox(1, 1, 1).Shape());

  BRepGraph aGraph;
  aGraph.Build(aComp);
  ASSERT_TRUE(aGraph.IsDone());

  int aFaceCount = 0;
  for (BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_CompoundId(0), BRepGraph_NodeId::Kind::Face);
       anExp.More(); anExp.Next())
    ++aFaceCount;
  // Each box has 6 faces.
  EXPECT_EQ(aFaceCount, THE_NB_CHILDREN * 6);
}
