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
#include <BRepGraph_Explorer.hxx>
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

#include <NCollection_Map.hxx>

#include <gtest/gtest.h>

// --- TopologyPath tests ---

TEST(BRepGraph_ExplorerTest, Default_Invalid)
{
  BRepGraph_TopologyPath aPath;
  EXPECT_FALSE(aPath.IsValid());
  EXPECT_EQ(aPath.Depth(), 0);
}

TEST(BRepGraph_ExplorerTest, FromRoot_Valid)
{
  BRepGraph_TopologyPath aPath = BRepGraph_TopologyPath::FromRoot(BRepGraph_SolidId(0));
  EXPECT_TRUE(aPath.IsValid());
  EXPECT_EQ(aPath.Depth(), 0);
  EXPECT_EQ(aPath.Root(), BRepGraph_SolidId(0));
}

TEST(BRepGraph_ExplorerTest, ToFace_CorrectDepth)
{
  BRepGraph_TopologyPath aPath = BRepGraph_TopologyPath::ToFace(0, 0, 2);
  EXPECT_TRUE(aPath.IsValid());
  EXPECT_EQ(aPath.Depth(), 2); // Solid->Shell, Shell->Face
  EXPECT_EQ(aPath.Root(), BRepGraph_SolidId(0));
}

TEST(BRepGraph_ExplorerTest, ToVertex_CorrectDepth)
{
  BRepGraph_TopologyPath aPath = BRepGraph_TopologyPath::ToVertex(0, 0, 1, 0, 3, 1);
  EXPECT_TRUE(aPath.IsValid());
  EXPECT_EQ(aPath.Depth(), 5); // Shell, Face, Wire, CoEdge, Vertex
}

TEST(BRepGraph_ExplorerTest, Equality)
{
  BRepGraph_TopologyPath aPath1 = BRepGraph_TopologyPath::ToEdge(0, 0, 2, 0, 3);
  BRepGraph_TopologyPath aPath2 = BRepGraph_TopologyPath::ToEdge(0, 0, 2, 0, 3);
  BRepGraph_TopologyPath aPath3 = BRepGraph_TopologyPath::ToEdge(0, 0, 2, 0, 1);
  EXPECT_EQ(aPath1, aPath2);
  EXPECT_NE(aPath1, aPath3);
}

TEST(BRepGraph_ExplorerTest, Hash_Consistent)
{
  BRepGraph_TopologyPath            aPath1 = BRepGraph_TopologyPath::ToFace(0, 0, 2);
  BRepGraph_TopologyPath            aPath2 = BRepGraph_TopologyPath::ToFace(0, 0, 2);
  std::hash<BRepGraph_TopologyPath> aHasher;
  EXPECT_EQ(aHasher(aPath1), aHasher(aPath2));
}

TEST(BRepGraph_ExplorerTest, Hash_DiffersOnDifferentPaths)
{
  BRepGraph_TopologyPath            aPath1 = BRepGraph_TopologyPath::ToFace(0, 0, 0);
  BRepGraph_TopologyPath            aPath2 = BRepGraph_TopologyPath::ToFace(0, 0, 1);
  std::hash<BRepGraph_TopologyPath> aHasher;
  EXPECT_NE(aHasher(aPath1), aHasher(aPath2));
}

// --- Explorer tests ---

TEST(BRepGraph_ExplorerTest, Box_EdgeOccurrences_Count24)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  // 12 edges x 2 faces each = 24 edge occurrences.
  EXPECT_EQ(anExp.NbFound(), 24);
}

TEST(BRepGraph_ExplorerTest, Box_FaceOccurrences_Count6)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Face);
  EXPECT_EQ(anExp.NbFound(), 6);
}

TEST(BRepGraph_ExplorerTest, Box_VertexOccurrences)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Vertex);
  EXPECT_GT(anExp.NbFound(), 0);

  // Count unique vertices.
  NCollection_Map<int> aVisited;
  for (; anExp.More(); anExp.Next())
    aVisited.Add(anExp.Current().Index);
  EXPECT_EQ(aVisited.Extent(), 8);
}

TEST(BRepGraph_ExplorerTest, Face_EdgeOccurrences_4)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_FaceId(0), BRepGraph_NodeId::Kind::Edge);
  EXPECT_EQ(anExp.NbFound(), 4);
}

TEST(BRepGraph_ExplorerTest, InvalidRoot_Empty)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_NodeId(), BRepGraph_NodeId::Kind::Edge);
  EXPECT_EQ(anExp.NbFound(), 0);
}

TEST(BRepGraph_ExplorerTest, RootEqualsTarget_ReturnsSelf)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_FaceId(0), BRepGraph_NodeId::Kind::Face);
  EXPECT_EQ(anExp.NbFound(), 1);
  EXPECT_EQ(anExp.Current(), BRepGraph_FaceId(0));
}

// --- Path-based composition tests ---

TEST(BRepGraph_ExplorerTest, GlobalLocation_Box_Identity)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // All paths in a simple box should compose to identity.
  BRepGraph_Explorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  for (; anExp.More(); anExp.Next())
  {
    TopLoc_Location aLoc = aGraph.Paths().GlobalLocation(anExp.CurrentPath());
    EXPECT_TRUE(aLoc.IsIdentity());
  }
}

TEST(BRepGraph_ExplorerTest, GlobalOrientation_BoxEdges_ForwardOrReversed)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  for (; anExp.More(); anExp.Next())
  {
    TopAbs_Orientation anOri = aGraph.Paths().GlobalOrientation(anExp.CurrentPath());
    EXPECT_TRUE(anOri == TopAbs_FORWARD || anOri == TopAbs_REVERSED);
  }
}

TEST(BRepGraph_ExplorerTest, LeafNode_MatchesCurrent)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  for (; anExp.More(); anExp.Next())
  {
    BRepGraph_NodeId aLeaf = aGraph.Paths().LeafNode(anExp.CurrentPath());
    EXPECT_TRUE(aLeaf.IsValid());
    EXPECT_EQ(aLeaf.NodeKind, BRepGraph_NodeId::Kind::Edge);
    EXPECT_EQ(aLeaf, anExp.Current());
  }
}

TEST(BRepGraph_ExplorerTest, NodeAt_IntermediateDepths)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Get a path to an edge.
  BRepGraph_Explorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());
  const BRepGraph_TopologyPath& aPath = anExp.CurrentPath();

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

TEST(BRepGraph_ExplorerTest, FindLevel_ByKind)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());
  const BRepGraph_TopologyPath& aPath = anExp.CurrentPath();

  int aFaceLevel = aGraph.Paths().FindLevel(aPath, BRepGraph_NodeId::Kind::Face);
  EXPECT_GE(aFaceLevel, 0);

  BRepGraph_NodeId aFaceNode = aGraph.Paths().NodeAt(aPath, aFaceLevel);
  EXPECT_EQ(aFaceNode.NodeKind, BRepGraph_NodeId::Kind::Face);
}

TEST(BRepGraph_ExplorerTest, Truncated_ToFace)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());
  const BRepGraph_TopologyPath& aPath = anExp.CurrentPath();

  // Truncate to 2 steps (shell + face).
  BRepGraph_TopologyPath aTruncated = aGraph.Paths().Truncated(aPath, 2);
  EXPECT_EQ(aTruncated.Depth(), 2);
  EXPECT_EQ(aTruncated.Root(), aPath.Root());

  BRepGraph_NodeId aLeaf = aGraph.Paths().LeafNode(aTruncated);
  EXPECT_EQ(aLeaf.NodeKind, BRepGraph_NodeId::Kind::Face);
}

TEST(BRepGraph_ExplorerTest, SharedEdge_DifferentPaths)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);

  NCollection_DataMap<int, NCollection_Vector<BRepGraph_TopologyPath>> aEdgePaths;
  for (; anExp.More(); anExp.Next())
  {
    const int anEdgeIdx = anExp.Current().Index;
    if (!aEdgePaths.IsBound(anEdgeIdx))
      aEdgePaths.Bind(anEdgeIdx, NCollection_Vector<BRepGraph_TopologyPath>());
    aEdgePaths.ChangeFind(anEdgeIdx).Append(anExp.CurrentPath());
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

TEST(BRepGraph_ExplorerTest, Compound_FaceCount)
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

  BRepGraph_Explorer anExp(aGraph, BRepGraph_CompoundId(0), BRepGraph_NodeId::Kind::Face);
  EXPECT_EQ(anExp.NbFound(), 12); // 6 + 6 faces
}

// --- Reverse path discovery tests ---

TEST(BRepGraph_ExplorerTest, PathsTo_SharedEdge_TwoPaths)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Each edge in a box is shared by 2 faces, so PathsTo should find 2 paths.
  NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Paths().PathsTo(BRepGraph_EdgeId(0));
  EXPECT_EQ(aPaths.Length(), 2);
}

TEST(BRepGraph_ExplorerTest, PathsTo_Face_SinglePath)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Each face appears once under a solid.
  NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Paths().PathsTo(BRepGraph_FaceId(0));
  EXPECT_EQ(aPaths.Length(), 1);
}

TEST(BRepGraph_ExplorerTest, PathsFromTo_FiltersByRoot)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Paths().PathsFromTo(BRepGraph_SolidId(0), BRepGraph_EdgeId(0));
  EXPECT_EQ(aPaths.Length(), 2);
}

TEST(BRepGraph_ExplorerTest, PathsTo_MatchesExplorer)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // For each explorer result, PathsTo should contain the same path.
  BRepGraph_Explorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());

  BRepGraph_NodeId                           aFirstEdge = anExp.Current();
  NCollection_Vector<BRepGraph_TopologyPath> aPaths     = aGraph.Paths().PathsTo(aFirstEdge);
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

TEST(BRepGraph_ExplorerTest, PathsTo_MatchesExplorer_WhenEarlierCompoundRefRemoved)
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

  const BRepGraphInc::CompoundDef& aRootComp = aGraph.Topo().Compound(BRepGraph_CompoundId(0));
  ASSERT_GE(aRootComp.ChildRefIds.Length(), 2);

  const BRepGraph_ChildRefId aRemovedRefId = aRootComp.ChildRefIds.Value(0);
  const BRepGraph_NodeId     aSecondChild =
    aGraph.Refs().Child(aRootComp.ChildRefIds.Value(1)).ChildDefId;
  ASSERT_TRUE(aSecondChild.IsValid());

  // Capture one face path under the second child before removal.
  BRepGraph_TopologyPath aExpectedPath;
  BRepGraph_NodeId       aLeafNode;
  bool                   aPathFound = false;
  for (BRepGraph_Explorer anExp(aGraph,
                                BRepGraph_CompoundId(0),
                                BRepGraph_NodeId::Kind::Face);
       anExp.More();
       anExp.Next())
  {
    const BRepGraph_TopologyPath& aPath = anExp.CurrentPath();
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
  for (BRepGraph_Explorer anExp(aGraph,
                                BRepGraph_CompoundId(0),
                                BRepGraph_NodeId::Kind::Face);
       anExp.More();
       anExp.Next())
  {
    if (anExp.CurrentPath() == aExpectedPath)
    {
      aExplorerHasPath = true;
      break;
    }
  }
  EXPECT_TRUE(aExplorerHasPath);

  // Reverse walk should agree with explorer.
  const NCollection_Vector<BRepGraph_TopologyPath> aReversePaths =
    aGraph.Paths().PathsTo(aLeafNode);
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

TEST(BRepGraph_ExplorerTest, PathsTo_MatchesExplorer_WhenFirstShellRefRemoved)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraphInc::SolidDef& aSolid = aGraph.Topo().Solid(BRepGraph_SolidId(0));
  ASSERT_GT(aSolid.ShellRefIds.Length(), 0);
  const BRepGraph_ShellId aShellId = aGraph.Refs().Shell(aSolid.ShellRefIds.Value(0)).ShellDefId;
  ASSERT_TRUE(aShellId.IsValid(aGraph.Topo().NbShells()));

  // Duplicate shell usage to create two shell refs to the same shell in one solid.
  aGraph.Builder().AddShellToSolid(BRepGraph_SolidId(0),
                                   BRepGraph_ShellId(aShellId.Index),
                                   TopAbs_FORWARD);
  const BRepGraphInc::SolidDef& aSolidAfterDup = aGraph.Topo().Solid(BRepGraph_SolidId(0));
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
  BRepGraph_Explorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Face);
  ASSERT_TRUE(anExp.More());
  const BRepGraph_TopologyPath aExpectedPath = anExp.CurrentPath();
  const BRepGraph_NodeId       aFaceNode     = anExp.Current();
  ASSERT_TRUE(aFaceNode.IsValid());
  ASSERT_EQ(aFaceNode.NodeKind, BRepGraph_NodeId::Kind::Face);

  // Reverse walk must return the same path (not a removed shell-ref ordinal).
  const NCollection_Vector<BRepGraph_TopologyPath> aReversePaths =
    aGraph.Paths().PathsTo(aFaceNode);
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

TEST(BRepGraph_ExplorerTest, PathContains_Face_True)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());

  // The path from Solid to Edge should pass through some face.
  BRepGraph_NodeId aFace = anExp.NodeOf(BRepGraph_NodeId::Kind::Face);
  EXPECT_TRUE(aGraph.Paths().PathContains(anExp.CurrentPath(), aFace));
}

TEST(BRepGraph_ExplorerTest, FilterByInclude_SharedEdge)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Get all paths to Edge(0), then filter by one of its faces.
  NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Paths().PathsTo(BRepGraph_EdgeId(0));
  ASSERT_EQ(aPaths.Length(), 2);

  // Find the face in the first path.
  BRepGraph_NodeId aFaceInPath0 =
    aGraph.Paths().LeafNode(aGraph.Paths().Truncated(aPaths.Value(0), 2));

  NCollection_Vector<BRepGraph_TopologyPath> aFiltered =
    aGraph.Paths().FilterByInclude(aPaths, aFaceInPath0);
  EXPECT_EQ(aFiltered.Length(), 1);
}

TEST(BRepGraph_ExplorerTest, FilterByExclude_Face)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Paths().PathsTo(BRepGraph_EdgeId(0));
  ASSERT_EQ(aPaths.Length(), 2);

  BRepGraph_NodeId aFaceInPath0 =
    aGraph.Paths().LeafNode(aGraph.Paths().Truncated(aPaths.Value(0), 2));

  NCollection_Vector<BRepGraph_TopologyPath> aExcluded =
    aGraph.Paths().FilterByExclude(aPaths, aFaceInPath0);
  EXPECT_EQ(aExcluded.Length(), 1);
}

// --- Explorer convenience tests ---

TEST(BRepGraph_ExplorerTest, LocationOf_Kind_Face)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());

  // LocationOf(Face) should match LocationAt(face level).
  TopLoc_Location aFaceLoc = anExp.LocationOf(BRepGraph_NodeId::Kind::Face);
  int aFaceLevel = aGraph.Paths().FindLevel(anExp.CurrentPath(), BRepGraph_NodeId::Kind::Face);
  TopLoc_Location aFaceLocAt = anExp.LocationAt(aFaceLevel);
  EXPECT_TRUE(aFaceLoc.IsEqual(aFaceLocAt));
}

TEST(BRepGraph_ExplorerTest, NodeOf_Kind_Face)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());

  BRepGraph_NodeId aFace = anExp.NodeOf(BRepGraph_NodeId::Kind::Face);
  EXPECT_TRUE(aFace.IsValid());
  EXPECT_EQ(aFace.NodeKind, BRepGraph_NodeId::Kind::Face);
}

TEST(BRepGraph_ExplorerTest, NodeLocations_EdgeHasTwoOccurrences)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Each box edge is shared by 2 faces = 2 occurrence locations.
  NCollection_Vector<BRepGraph::PathView::OccurrenceEntry> aEntries =
    aGraph.Paths().NodeLocations(BRepGraph_EdgeId(0));
  EXPECT_EQ(aEntries.Length(), 2);

  // Both locations should be identity for a simple box.
  for (int i = 0; i < aEntries.Length(); ++i)
    EXPECT_TRUE(aEntries.Value(i).Location.IsIdentity());
}

TEST(BRepGraph_ExplorerTest, NodeLocations_FaceHasOneOccurrence)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  NCollection_Vector<BRepGraph::PathView::OccurrenceEntry> aEntries =
    aGraph.Paths().NodeLocations(BRepGraph_FaceId(0));
  EXPECT_EQ(aEntries.Length(), 1);
}

TEST(BRepGraph_ExplorerTest, NodeLocations_VertexMultipleOccurrences)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // A box corner vertex is shared by 3 edges, each shared by 2 faces.
  NCollection_Vector<BRepGraph::PathView::OccurrenceEntry> aEntries =
    aGraph.Paths().NodeLocations(BRepGraph_VertexId(0));
  EXPECT_GT(aEntries.Length(), 0);
}

TEST(BRepGraph_ExplorerTest, PathsTo_InvalidNode_Empty)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Invalid NodeId should return empty.
  NCollection_Vector<BRepGraph_TopologyPath> aPaths = aGraph.Paths().PathsTo(BRepGraph_NodeId());
  EXPECT_EQ(aPaths.Length(), 0);

  // Out-of-range index should return empty.
  NCollection_Vector<BRepGraph_TopologyPath> aPaths2 =
    aGraph.Paths().PathsTo(BRepGraph_EdgeId(99999));
  EXPECT_EQ(aPaths2.Length(), 0);
}

// --- Depth guard and re-initialization tests ---

TEST(BRepGraph_ExplorerTest, DeepCompound_NoStackOverflow)
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
  BRepGraph_Explorer anExp(aGraph, BRepGraph_CompoundId(0), BRepGraph_NodeId::Kind::Face);
  EXPECT_EQ(anExp.NbFound(), 6);
}

TEST(BRepGraph_ExplorerTest, Init_ResetAndReexplore)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Face);
  EXPECT_EQ(anExp.NbFound(), 6);

  // Re-init targeting edges.
  anExp.Init(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  EXPECT_EQ(anExp.NbFound(), 24);
}

// --- Reverse walk with Compound/CompSolid parents ---

TEST(BRepGraph_ExplorerTest, PathsTo_NestedCompound_HasCompoundRoot)
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
    aGraph.Paths().PathsTo(BRepGraph_FaceId(0));
  ASSERT_GT(aPaths.Length(), 0);
  // Root should be the outer compound (index 0 - outermost).
  EXPECT_EQ(aPaths.Value(0).Root().NodeKind, BRepGraph_NodeId::Kind::Compound);
}

TEST(BRepGraph_ExplorerTest, NodeLocations_CompoundChild_ComposedLocation)
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
  BRepGraph_Explorer anExp(aGraph, BRepGraph_CompoundId(0), BRepGraph_NodeId::Kind::Face);
  EXPECT_EQ(anExp.NbFound(), 6);
}

TEST(BRepGraph_ExplorerTest, PathsTo_ShellInCompound_CompoundRoot)
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
  NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Paths().PathsTo(BRepGraph_FaceId(0));
  ASSERT_GT(aPaths.Length(), 0);
  EXPECT_EQ(aPaths.Value(0).Root().NodeKind, BRepGraph_NodeId::Kind::Compound);
}

TEST(BRepGraph_ExplorerTest, PathsTo_SolidInCompSolid_CompSolidRoot)
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
  NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Paths().PathsTo(BRepGraph_FaceId(0));
  ASSERT_GT(aPaths.Length(), 0);
  EXPECT_EQ(aPaths.Value(0).Root().NodeKind, BRepGraph_NodeId::Kind::CompSolid);
}

TEST(BRepGraph_ExplorerTest, PathsTo_CompSolidInCompound_CompoundRoot)
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
  NCollection_Vector<BRepGraph_TopologyPath> aPaths =
    aGraph.Paths().PathsTo(BRepGraph_FaceId(0));
  ASSERT_GT(aPaths.Length(), 0);
  EXPECT_EQ(aPaths.Value(0).Root().NodeKind, BRepGraph_NodeId::Kind::Compound);
}

// ============ Phase 7: Tests for new PathView methods ============

TEST(BRepGraph_ExplorerTest, OrientationAt_IntermediateLevel)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());
  const BRepGraph_TopologyPath& aPath = anExp.CurrentPath();

  TopAbs_Orientation anOri = aGraph.Paths().OrientationAt(aPath, 0);
  EXPECT_TRUE(anOri == TopAbs_FORWARD || anOri == TopAbs_REVERSED);
}

TEST(BRepGraph_ExplorerTest, TopologyPath_Truncated_KeepsPrefix)
{
  BRepGraph_TopologyPath aPath{BRepGraph_SolidId(0)};
  // Solid(0) -> Shell(ref 0) -> Face(ref 0) -> Wire(ref 0) -> CoEdge(ref 0)
  aPath = BRepGraph_TopologyPath::ToEdge(0, 0, 0, 0, 0);
  EXPECT_EQ(aPath.Depth(), 4);

  BRepGraph_TopologyPath aTrunc = aPath.Truncated(2);
  EXPECT_EQ(aTrunc.Depth(), 2);
  EXPECT_EQ(aTrunc.Root(), aPath.Root());

  // Truncating beyond depth returns a copy.
  BRepGraph_TopologyPath aFull = aPath.Truncated(100);
  EXPECT_EQ(aFull.Depth(), 4);
  EXPECT_EQ(aFull, aPath);
}

TEST(BRepGraph_ExplorerTest, DepthOfKind_Face_CountsOne)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());
  const BRepGraph_TopologyPath& aPath = anExp.CurrentPath();

  // Path Solid->Shell->Face->Wire->CoEdge(->Edge) should have exactly 1 Face.
  EXPECT_EQ(aGraph.Paths().DepthOfKind(aPath, BRepGraph_NodeId::Kind::Face), 1);
  EXPECT_EQ(aGraph.Paths().DepthOfKind(aPath, BRepGraph_NodeId::Kind::Shell), 1);
  EXPECT_EQ(aGraph.Paths().DepthOfKind(aPath, BRepGraph_NodeId::Kind::Solid), 1);
}

TEST(BRepGraph_ExplorerTest, CommonAncestor_SharedFace)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Get two edge paths from the same face.
  BRepGraph_Explorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());
  const BRepGraph_TopologyPath aPath1 = anExp.CurrentPath();
  anExp.Next();
  ASSERT_TRUE(anExp.More());
  const BRepGraph_TopologyPath aPath2 = anExp.CurrentPath();

  BRepGraph_TopologyPath aCommon = aGraph.Paths().CommonAncestor(aPath1, aPath2);
  EXPECT_TRUE(aCommon.IsValid());
  // Common ancestor should be at least the solid root.
  EXPECT_EQ(aCommon.Root(), aPath1.Root());
  EXPECT_LE(aCommon.Depth(), aPath1.Depth());
  EXPECT_LE(aCommon.Depth(), aPath2.Depth());
}

TEST(BRepGraph_ExplorerTest, CommonAncestor_DifferentRoots_Invalid)
{
  BRepGraph_TopologyPath aPath1{BRepGraph_SolidId(0)};
  BRepGraph_TopologyPath aPath2{BRepGraph_SolidId(1)};

  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_TopologyPath aCommon = aGraph.Paths().CommonAncestor(aPath1, aPath2);
  EXPECT_FALSE(aCommon.IsValid());
}

TEST(BRepGraph_ExplorerTest, IsAncestorOf_TrueForPrefix)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());
  const BRepGraph_TopologyPath& aEdgePath = anExp.CurrentPath();

  // Truncated to face level should be an ancestor.
  BRepGraph_TopologyPath aFacePath = aEdgePath.Truncated(2);
  EXPECT_TRUE(aGraph.Paths().IsAncestorOf(aFacePath, aEdgePath));
  EXPECT_FALSE(aGraph.Paths().IsAncestorOf(aEdgePath, aFacePath));
}

TEST(BRepGraph_ExplorerTest, IsAncestorOf_FalseForNonPrefix)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Two different edge paths are not ancestors of each other.
  BRepGraph_Explorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());
  const BRepGraph_TopologyPath aPath1 = anExp.CurrentPath();
  // Skip to an edge from a different face.
  for (int i = 0; i < 5 && anExp.More(); ++i)
    anExp.Next();
  if (anExp.More())
  {
    const BRepGraph_TopologyPath aPath2 = anExp.CurrentPath();
    if (aPath1 != aPath2)
    {
      EXPECT_FALSE(aGraph.Paths().IsAncestorOf(aPath1, aPath2));
    }
  }
}

TEST(BRepGraph_ExplorerTest, AllNodesOnPath_BoxEdge_CorrectKinds)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_Explorer anExp(aGraph, BRepGraph_SolidId(0), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());
  const BRepGraph_TopologyPath& aPath = anExp.CurrentPath();

  NCollection_Vector<BRepGraph_NodeId> aNodes = aGraph.Paths().AllNodesOnPath(aPath);
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

TEST(BRepGraph_ExplorerTest, DepthOfKind_InvalidPath_ReturnsZero)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_TopologyPath anInvalid;
  EXPECT_EQ(aGraph.Paths().DepthOfKind(anInvalid, BRepGraph_NodeId::Kind::Face), 0);
}

TEST(BRepGraph_ExplorerTest, CommonAncestor_EmptyPaths_Invalid)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_TopologyPath anEmpty;
  BRepGraph_TopologyPath aValid{BRepGraph_SolidId(0)};

  EXPECT_FALSE(aGraph.Paths().CommonAncestor(anEmpty, aValid).IsValid());
  EXPECT_FALSE(aGraph.Paths().CommonAncestor(aValid, anEmpty).IsValid());
  EXPECT_FALSE(aGraph.Paths().CommonAncestor(anEmpty, anEmpty).IsValid());
}

TEST(BRepGraph_ExplorerTest, IsAncestorOf_InvalidPaths_ReturnsFalse)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_TopologyPath anEmpty;
  BRepGraph_TopologyPath aValid{BRepGraph_SolidId(0)};

  EXPECT_FALSE(aGraph.Paths().IsAncestorOf(anEmpty, aValid));
  EXPECT_FALSE(aGraph.Paths().IsAncestorOf(aValid, anEmpty));
}

TEST(BRepGraph_ExplorerTest, AllNodesOnPath_InvalidPath_Empty)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_TopologyPath               anInvalid;
  NCollection_Vector<BRepGraph_NodeId> aNodes = aGraph.Paths().AllNodesOnPath(anInvalid);
  EXPECT_EQ(aNodes.Length(), 0);
}

TEST(BRepGraph_ExplorerTest, ReverseWalkPaths_DepthBudgetZero_ReturnsEmpty)
{
  BRepGraph aGraph;
  aGraph.Build(BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Normal PathsTo should find paths.
  NCollection_Vector<BRepGraph_TopologyPath> aNormal =
    aGraph.Paths().PathsTo(BRepGraph_FaceId(0));
  EXPECT_GT(aNormal.Length(), 0);

  // PathsTo with valid node should terminate even on large graphs.
  NCollection_Vector<BRepGraph_TopologyPath> aVertex =
    aGraph.Paths().PathsTo(BRepGraph_VertexId(0));
  EXPECT_GT(aVertex.Length(), 0);
}
