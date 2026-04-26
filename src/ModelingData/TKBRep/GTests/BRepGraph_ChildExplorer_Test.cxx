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
#include <BRepGraph_EditorView.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <BRepGraph_ChildExplorer.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Builder.hxx>

#include <BRep_Builder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>

#include <NCollection_Map.hxx>

#include <gtest/gtest.h>

namespace
{
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

// --- Explorer tests ---

TEST(BRepGraph_ChildExplorerTest, Box_EdgeOccurrences_Count24)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (BRepGraph_ChildExplorer anExp(aGraph,
                                     BRepGraph_SolidId::Start(),
                                     BRepGraph_NodeId::Kind::Edge);
       anExp.More();
       anExp.Next())
    ++aCount;
  // 12 edges x 2 faces each = 24 edge occurrences.
  EXPECT_EQ(aCount, 24);
}

TEST(BRepGraph_ChildExplorerTest, Box_FaceOccurrences_Count6)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aFaceCount = 0;
  for (BRepGraph_ChildExplorer anExp(aGraph,
                                     BRepGraph_SolidId::Start(),
                                     BRepGraph_NodeId::Kind::Face);
       anExp.More();
       anExp.Next())
    ++aFaceCount;
  EXPECT_EQ(aFaceCount, 6);
}

TEST(BRepGraph_ChildExplorerTest, Box_VertexOccurrences)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // Count unique vertices.
  int                  aCount = 0;
  NCollection_Map<int> aVisited;
  for (BRepGraph_ChildExplorer anExp(aGraph,
                                     BRepGraph_SolidId::Start(),
                                     BRepGraph_NodeId::Kind::Vertex);
       anExp.More();
       anExp.Next())
  {
    aVisited.Add(anExp.Current().DefId.Index);
    ++aCount;
  }
  EXPECT_GT(aCount, 0);
  EXPECT_EQ(aVisited.Extent(), 8);
}

TEST(BRepGraph_ChildExplorerTest, Face_EdgeOccurrences_4)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (BRepGraph_ChildExplorer anExp(aGraph,
                                     BRepGraph_FaceId::Start(),
                                     BRepGraph_NodeId::Kind::Edge);
       anExp.More();
       anExp.Next())
    ++aCount;
  EXPECT_EQ(aCount, 4);
}

TEST(BRepGraph_ChildExplorerTest, InvalidRoot_Empty)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_NodeId(), BRepGraph_NodeId::Kind::Edge);
  EXPECT_FALSE(anExp.More());
}

TEST(BRepGraph_ChildExplorerTest, RootEqualsTarget_ReturnsSelf)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_FaceId::Start(), BRepGraph_NodeId::Kind::Face);
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.Current().DefId, BRepGraph_FaceId::Start());
  anExp.Next();
  EXPECT_FALSE(anExp.More());
}

TEST(BRepGraph_ChildExplorerTest, AvoidKind_Shell_SkipsContainedFaces)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ChildExplorer anExp(aGraph,
                                BRepGraph_SolidId::Start(),
                                BRepGraph_NodeId::Kind::Face,
                                BRepGraph_NodeId::Kind::Shell,
                                false);
  EXPECT_FALSE(anExp.More());
}

TEST(BRepGraph_ChildExplorerTest, AvoidKind_EmitBoundary_ReturnsFacesInsteadOfEdges)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aFaceCount = 0;
  for (BRepGraph_ChildExplorer anExp(aGraph,
                                     BRepGraph_SolidId::Start(),
                                     BRepGraph_NodeId::Kind::Edge,
                                     BRepGraph_NodeId::Kind::Face,
                                     true);
       anExp.More();
       anExp.Next())
  {
    EXPECT_EQ(anExp.Current().DefId.NodeKind, BRepGraph_NodeId::Kind::Face);
    ++aFaceCount;
  }
  EXPECT_EQ(aFaceCount, 6);
}

TEST(BRepGraph_ChildExplorerTest, AvoidKind_SameAsTarget_IsIgnored)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aFaceCount = 0;
  for (BRepGraph_ChildExplorer anExp(aGraph,
                                     BRepGraph_SolidId::Start(),
                                     BRepGraph_NodeId::Kind::Face,
                                     BRepGraph_NodeId::Kind::Face,
                                     false);
       anExp.More();
       anExp.Next())
  {
    EXPECT_EQ(anExp.Current().DefId.NodeKind, BRepGraph_NodeId::Kind::Face);
    ++aFaceCount;
  }
  EXPECT_EQ(aFaceCount, 6);
}

TEST(BRepGraph_ChildExplorerTest, AllDescendants_Recursive_YieldsAllKinds)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aShellCount  = 0;
  int aFaceCount   = 0;
  int aWireCount   = 0;
  int aCoEdgeCount = 0;
  int anEdgeCount  = 0;
  int aVertexCount = 0;
  for (BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId::Start()); anExp.More();
       anExp.Next())
  {
    switch (anExp.Current().DefId.NodeKind)
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
      case BRepGraph_NodeId::Kind::CoEdge:
        ++aCoEdgeCount;
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
  EXPECT_EQ(aCoEdgeCount, 24); // 4 CoEdges per face, 6 faces
  EXPECT_EQ(anEdgeCount, 24);  // Each CoEdge leads to one Edge (occurrence-based)
  EXPECT_EQ(aVertexCount, 48); // Each Edge visited with 2 vertices
}

TEST(BRepGraph_ChildExplorerTest, AllDescendants_AvoidFaceBoundary_StopsBelowFaces)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aShellCount = 0;
  int aFaceCount  = 0;
  for (BRepGraph_ChildExplorer anExp(aGraph,
                                     BRepGraph_SolidId::Start(),
                                     BRepGraph_NodeId::Kind::Face,
                                     true);
       anExp.More();
       anExp.Next())
  {
    if (anExp.Current().DefId.NodeKind == BRepGraph_NodeId::Kind::Shell)
    {
      ++aShellCount;
      continue;
    }

    EXPECT_EQ(anExp.Current().DefId.NodeKind, BRepGraph_NodeId::Kind::Face);
    ++aFaceCount;
  }

  EXPECT_EQ(aShellCount, 1);
  EXPECT_EQ(aFaceCount, 6);
}

// --- Cached location/orientation tests ---

TEST(BRepGraph_ChildExplorerTest, NoCumLoc_IdentityLocation)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  for (BRepGraph_ChildExplorer
         anExp(aGraph, BRepGraph_SolidId::Start(), BRepGraph_NodeId::Kind::Edge, false, true);
       anExp.More();
       anExp.Next())
  {
    EXPECT_TRUE(anExp.Current().Location.IsIdentity());
  }
}

TEST(BRepGraph_ChildExplorerTest, NoCumOri_ForwardOrientation)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  for (BRepGraph_ChildExplorer
         anExp(aGraph, BRepGraph_SolidId::Start(), BRepGraph_NodeId::Kind::Edge, true, false);
       anExp.More();
       anExp.Next())
  {
    EXPECT_EQ(anExp.Current().Orientation, TopAbs_FORWARD);
  }
}

// --- Path-based composition tests ---

TEST(BRepGraph_ChildExplorerTest, GlobalLocation_Box_Identity)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // All paths in a simple box should compose to identity.
  BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId::Start(), BRepGraph_NodeId::Kind::Edge);
  for (; anExp.More(); anExp.Next())
  {
    EXPECT_TRUE(anExp.Current().Location.IsIdentity());
  }
}

TEST(BRepGraph_ChildExplorerTest, GlobalOrientation_BoxEdges_ForwardOrReversed)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId::Start(), BRepGraph_NodeId::Kind::Edge);
  for (; anExp.More(); anExp.Next())
  {
    TopAbs_Orientation anOri = anExp.Current().Orientation;
    EXPECT_TRUE(anOri == TopAbs_FORWARD || anOri == TopAbs_REVERSED);
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
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, aComp);
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (BRepGraph_ChildExplorer anExp(aGraph,
                                     BRepGraph_CompoundId::Start(),
                                     BRepGraph_NodeId::Kind::Face);
       anExp.More();
       anExp.Next())
    ++aCount;
  EXPECT_EQ(aCount, 12); // 6 + 6 faces
}

// --- Explorer convenience tests ---

TEST(BRepGraph_ChildExplorerTest, NodeOf_Kind_Face)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId::Start(), BRepGraph_NodeId::Kind::Edge);
  ASSERT_TRUE(anExp.More());

  BRepGraph_NodeId aFace = anExp.NodeOf(BRepGraph_NodeId::Kind::Face);
  EXPECT_TRUE(aFace.IsValid());
  EXPECT_EQ(aFace.NodeKind, BRepGraph_NodeId::Kind::Face);
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
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, aInner);
  ASSERT_TRUE(aGraph.IsDone());

  // Should not crash (stack overflow) and should find the box's faces.
  int aCount = 0;
  for (BRepGraph_ChildExplorer anExp(aGraph,
                                     BRepGraph_CompoundId::Start(),
                                     BRepGraph_NodeId::Kind::Face);
       anExp.More();
       anExp.Next())
    ++aCount;
  EXPECT_EQ(aCount, 6);
}

TEST(BRepGraph_ChildExplorerTest, Recreate_ResetAndReexplore)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int                     aFaceCount = 0;
  BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_SolidId::Start(), BRepGraph_NodeId::Kind::Face);
  for (; anExp.More(); anExp.Next())
    ++aFaceCount;
  EXPECT_EQ(aFaceCount, 6);

  // Recreate targeting edges.
  int aEdgeCount = 0;
  for (BRepGraph_ChildExplorer anEdgeExp(aGraph,
                                         BRepGraph_SolidId::Start(),
                                         BRepGraph_NodeId::Kind::Edge);
       anEdgeExp.More();
       anEdgeExp.Next())
    ++aEdgeCount;
  EXPECT_EQ(aEdgeCount, 24);
}

// ============ Regression tests: CoEdge/Occurrence target reachability ============

TEST(BRepGraph_ChildExplorerTest, CoEdgeTarget_Reachable)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  // CoEdge target from Solid must find all coedges (24 edge occurrences = 24 coedges).
  int aCount = 0;
  for (BRepGraph_ChildExplorer anExp(aGraph,
                                     BRepGraph_SolidId::Start(),
                                     BRepGraph_NodeId::Kind::CoEdge);
       anExp.More();
       anExp.Next())
  {
    EXPECT_EQ(anExp.Current().DefId.NodeKind, BRepGraph_NodeId::Kind::CoEdge);
    ++aCount;
  }
  EXPECT_EQ(aCount, 24);
}

TEST(BRepGraph_ChildExplorerTest, CoEdgeTarget_FromFace_Count4)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (BRepGraph_ChildExplorer anExp(aGraph,
                                     BRepGraph_FaceId::Start(),
                                     BRepGraph_NodeId::Kind::CoEdge);
       anExp.More();
       anExp.Next())
  {
    EXPECT_EQ(anExp.Current().DefId.NodeKind, BRepGraph_NodeId::Kind::CoEdge);
    ++aCount;
  }
  EXPECT_EQ(aCount, 4);
}

TEST(BRepGraph_ChildExplorerTest, DirectChildren_ShellFaces_CountAndOrder)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_ShellId                              aShellId(0);
  const NCollection_DynamicArray<BRepGraph_FaceRefId>& aFaceRefIds =
    aGraph.Refs().Faces().IdsOf(aShellId);

  NCollection_DynamicArray<int> anExpectedFaceIds;
  for (const BRepGraph_FaceRefId& aFaceRefId : aFaceRefIds)
  {
    const BRepGraphInc::FaceRef& aRef = aGraph.Refs().Faces().Entry(aFaceRefId);
    if (!aRef.IsRemoved)
      anExpectedFaceIds.Append(aRef.FaceDefId.Index);
  }

  NCollection_DynamicArray<int> anActualFaceIds;
  for (BRepGraph_ChildExplorer anIt =
         makeDirectChildExplorer(aGraph, aShellId, BRepGraph_NodeId::Kind::Face);
       anIt.More();
       anIt.Next())
  {
    ASSERT_EQ(anIt.Current().DefId.NodeKind, BRepGraph_NodeId::Kind::Face);
    anActualFaceIds.Append(anIt.Current().DefId.Index);
  }

  ASSERT_EQ(anActualFaceIds.Length(), anExpectedFaceIds.Length());
  for (int i = 0; i < anExpectedFaceIds.Length(); ++i)
    EXPECT_EQ(anActualFaceIds.Value(i), anExpectedFaceIds.Value(i));
}

TEST(BRepGraph_ChildExplorerTest, DirectChildren_ShellFaces_ExposeParentAndRef)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_ShellId aShellId(0);
  int                     anOrdinal = 0;
  for (BRepGraph_ChildExplorer anIt =
         makeDirectChildExplorer(aGraph, aShellId, BRepGraph_NodeId::Kind::Face);
       anIt.More();
       anIt.Next(), ++anOrdinal)
  {
    const BRepGraph_FaceRefId aFaceRefId = aGraph.Refs().Faces().IdsOf(aShellId).Value(anOrdinal);
    EXPECT_EQ(anIt.CurrentParent(), BRepGraph_NodeId(aShellId));
    EXPECT_EQ(anIt.CurrentLinkKind(), BRepGraph_ChildExplorer::LinkKind::Reference);
    EXPECT_EQ(anIt.CurrentRef(), BRepGraph_RefId(aFaceRefId));
  }
}

TEST(BRepGraph_ChildExplorerTest, DirectChildren_ProductShapeRoot_ViaOccurrenceRef)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_ProductId aProductId = aGraph.Editor().Products().LinkProductToTopology(BRepGraph_SolidId::Start());
  ASSERT_TRUE(aProductId.IsValid());

  // In the new model, products reference children through OccurrenceRefIds.
  // The shape root is reached via an occurrence whose ChildDefId is the topology node.
  BRepGraph_ChildExplorer anIt(aGraph,
                               aProductId,
                               BRepGraph_ChildExplorer::TraversalMode::DirectChildren);
  ASSERT_TRUE(anIt.More());
  // The first child should be an occurrence node (not the solid directly).
  EXPECT_EQ(anIt.Current().DefId.NodeKind, BRepGraph_NodeId::Kind::Occurrence);
  EXPECT_EQ(anIt.CurrentParent(), BRepGraph_NodeId(aProductId));
  // The occurrence is linked via a reference.
  EXPECT_TRUE(anIt.CurrentRef().IsValid());

  anIt.Next();
  EXPECT_FALSE(anIt.More());
}

TEST(BRepGraph_ChildExplorerTest, RootEqualsTarget_LinkKindNone)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_ChildExplorer anExp(aGraph, BRepGraph_FaceId::Start(), BRepGraph_NodeId::Kind::Face);
  ASSERT_TRUE(anExp.More());
  EXPECT_EQ(anExp.CurrentLinkKind(), BRepGraph_ChildExplorer::LinkKind::None);
  EXPECT_FALSE(anExp.CurrentParent().IsValid());
  EXPECT_FALSE(anExp.CurrentRef().IsValid());
}

TEST(BRepGraph_ChildExplorerTest, DirectChildren_ProductOccurrences_ExposeOccurrenceRefs)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_ProductId aPart      = aGraph.Editor().Products().LinkProductToTopology(BRepGraph_SolidId::Start());
  const BRepGraph_ProductId anAssembly = aGraph.Editor().Products().CreateEmptyProduct();
  ASSERT_TRUE(aPart.IsValid());
  ASSERT_TRUE(anAssembly.IsValid());

  const BRepGraph_OccurrenceId anOcc0 =
    aGraph.Editor().Products().LinkProducts(anAssembly, aPart, TopLoc_Location());
  const BRepGraph_OccurrenceId anOcc1 =
    aGraph.Editor().Products().LinkProducts(anAssembly, aPart, TopLoc_Location());
  ASSERT_TRUE(anOcc0.IsValid());
  ASSERT_TRUE(anOcc1.IsValid());

  const NCollection_DynamicArray<BRepGraph_OccurrenceRefId>& anOccurrenceRefs =
    aGraph.Refs().Occurrences().IdsOf(anAssembly);
  ASSERT_EQ(anOccurrenceRefs.Length(), 2);

  int anOrdinal = 0;
  for (BRepGraph_ChildExplorer anIt(aGraph,
                                    anAssembly,
                                    BRepGraph_ChildExplorer::TraversalMode::DirectChildren);
       anIt.More();
       anIt.Next(), ++anOrdinal)
  {
    EXPECT_EQ(anIt.CurrentParent(), BRepGraph_NodeId(anAssembly));
    EXPECT_EQ(anIt.CurrentRef(), BRepGraph_RefId(anOccurrenceRefs.Value(anOrdinal)));
  }
  EXPECT_EQ(anOrdinal, 2);
}

TEST(BRepGraph_ChildExplorerTest, DirectChildren_RemovedFaceRef_IsSkipped)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_ShellId                              aShellId(0);
  const NCollection_DynamicArray<BRepGraph_FaceRefId>& aFaceRefIds =
    aGraph.Refs().Faces().IdsOf(aShellId);
  ASSERT_GT(aFaceRefIds.Length(), 0);

  const BRepGraph_FaceRefId aRemovedRef    = aFaceRefIds.Value(0);
  const BRepGraph_FaceId    aRemovedFaceId = aGraph.Refs().Faces().Entry(aRemovedRef).FaceDefId;

  {
    BRepGraph_MutGuard<BRepGraphInc::FaceRef> aFaceRef =
      aGraph.Editor().Faces().MutRef(aRemovedRef);
    aFaceRef->IsRemoved = true;
  }

  int aCount = 0;
  for (BRepGraph_ChildExplorer anIt =
         makeDirectChildExplorer(aGraph, aShellId, BRepGraph_NodeId::Kind::Face);
       anIt.More();
       anIt.Next())
  {
    EXPECT_NE(anIt.Current().DefId, BRepGraph_NodeId(aRemovedFaceId));
    ++aCount;
  }

  EXPECT_EQ(aCount, aFaceRefIds.Length() - 1);
}

TEST(BRepGraph_ChildExplorerTest, DirectChildren_WireChildren_AreCoEdges)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_FaceId                               aFaceId(0);
  const NCollection_DynamicArray<BRepGraph_WireRefId>& aWireRefs =
    aGraph.Refs().Wires().IdsOf(aFaceId);
  ASSERT_GT(aWireRefs.Length(), 0);

  const BRepGraph_WireId aWireId = aGraph.Refs().Wires().Entry(aWireRefs.Value(0)).WireDefId;
  const NCollection_DynamicArray<BRepGraph_CoEdgeRefId>& aCoEdgeRefs =
    aGraph.Refs().CoEdges().IdsOf(aWireId);

  // Wire's direct children are CoEdges (no 1:1 collapse).
  int aCount = 0;
  for (BRepGraph_ChildExplorer anIt =
         makeDirectChildExplorer(aGraph, aWireId, BRepGraph_NodeId::Kind::CoEdge);
       anIt.More();
       anIt.Next())
  {
    EXPECT_EQ(anIt.Current().DefId.NodeKind, BRepGraph_NodeId::Kind::CoEdge);
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
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, aComp);
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (BRepGraph_ChildExplorer anIt = makeDirectChildExplorer(aGraph,
                                                              BRepGraph_CompoundId::Start(),
                                                              BRepGraph_NodeId::Kind::Solid);
       anIt.More();
       anIt.Next())
  {
    EXPECT_TRUE(BRepGraph_NodeId::IsTopologyKind(anIt.Current().DefId.NodeKind));
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
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, aComp);
  ASSERT_TRUE(aGraph.IsDone());

  NCollection_DataMap<int, TopLoc_Location>    aExpectedLoc;
  NCollection_DataMap<int, TopAbs_Orientation> aExpectedOri;
  for (BRepGraph_ChildExplorer anExp(aGraph,
                                     BRepGraph_CompoundId::Start(),
                                     BRepGraph_NodeId::Kind::Face);
       anExp.More();
       anExp.Next())
  {
    const int aFaceIdx = anExp.Current().DefId.Index;
    if (!aExpectedLoc.IsBound(aFaceIdx))
    {
      aExpectedLoc.Bind(aFaceIdx, anExp.Current().Location);
      aExpectedOri.Bind(aFaceIdx, anExp.Current().Orientation);
    }
  }

  int aVisited = 0;
  for (BRepGraph_ChildExplorer aSolidIt = makeDirectChildExplorer(aGraph,
                                                                  BRepGraph_CompoundId::Start(),
                                                                  BRepGraph_NodeId::Kind::Solid);
       aSolidIt.More();
       aSolidIt.Next())
  {
    const BRepGraphInc::NodeInstance aSolidUsage = aSolidIt.Current();
    for (BRepGraph_ChildExplorer aShellIt = makeDirectChildExplorer(aGraph,
                                                                    aSolidUsage.DefId,
                                                                    BRepGraph_NodeId::Kind::Shell,
                                                                    aSolidUsage.Location,
                                                                    aSolidUsage.Orientation);
         aShellIt.More();
         aShellIt.Next())
    {
      const BRepGraphInc::NodeInstance aShellUsage = aShellIt.Current();
      for (BRepGraph_ChildExplorer aFaceIt = makeDirectChildExplorer(aGraph,
                                                                     aShellUsage.DefId,
                                                                     BRepGraph_NodeId::Kind::Face,
                                                                     aShellUsage.Location,
                                                                     aShellUsage.Orientation);
           aFaceIt.More();
           aFaceIt.Next())
      {
        const BRepGraphInc::NodeInstance aFaceUsage = aFaceIt.Current();
        ASSERT_EQ(aFaceUsage.DefId.NodeKind, BRepGraph_NodeId::Kind::Face);
        const int aFaceIdx = aFaceUsage.DefId.Index;
        ASSERT_TRUE(aExpectedLoc.IsBound(aFaceIdx));
        EXPECT_TRUE(aFaceUsage.Location.IsEqual(aExpectedLoc.Find(aFaceIdx)));
        EXPECT_EQ(aFaceUsage.Orientation, aExpectedOri.Find(aFaceIdx));
        ++aVisited;
      }
    }
  }

  EXPECT_EQ(aVisited, aExpectedLoc.Extent());
}

TEST(BRepGraph_ChildExplorerTest, Recursive_SharedProduct_ChildrenHaveDistinctContexts)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_ProductId aPart = aGraph.Editor().Products().LinkProductToTopology(BRepGraph_SolidId::Start());
  ASSERT_TRUE(aPart.IsValid());
  const BRepGraph_ProductId anAssembly = aGraph.Editor().Products().CreateEmptyProduct();
  ASSERT_TRUE(anAssembly.IsValid());

  gp_Trsf aT1;
  aT1.SetTranslation(gp_Vec(10.0, 0.0, 0.0));
  gp_Trsf aT2;
  aT2.SetTranslation(gp_Vec(20.0, 0.0, 0.0));

  const BRepGraph_OccurrenceId anOcc1 =
    aGraph.Editor().Products().LinkProducts(anAssembly, aPart, TopLoc_Location(aT1));
  const BRepGraph_OccurrenceId anOcc2 =
    aGraph.Editor().Products().LinkProducts(anAssembly, aPart, TopLoc_Location(aT2));
  ASSERT_TRUE(anOcc1.IsValid());
  ASSERT_TRUE(anOcc2.IsValid());

  // Recursive traversal through Assembly->Occurrence->Product(part)->Solid.
  int             aCount = 0;
  TopLoc_Location aLoc1;
  TopLoc_Location aLoc2;
  for (BRepGraph_ChildExplorer anIt(aGraph, anAssembly, BRepGraph_NodeId::Kind::Solid); anIt.More();
       anIt.Next())
  {
    ASSERT_EQ(anIt.Current().DefId, BRepGraph_NodeId(BRepGraph_SolidId::Start()));
    if (aCount == 0)
      aLoc1 = anIt.Current().Location;
    else if (aCount == 1)
      aLoc2 = anIt.Current().Location;
    ++aCount;
  }

  ASSERT_EQ(aCount, 2);
  EXPECT_FALSE(aLoc1.IsEqual(aLoc2));
}

TEST(BRepGraph_ChildExplorerTest, Recursive_ProductPartRootContext_ComposedWithOccurrence)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  const BRepGraph_ProductId aPart = aGraph.Editor().Products().LinkProductToTopology(BRepGraph_SolidId::Start());
  ASSERT_TRUE(aPart.IsValid());
  const BRepGraph_ProductId anAssembly = aGraph.Editor().Products().CreateEmptyProduct();
  ASSERT_TRUE(anAssembly.IsValid());

  gp_Trsf aOccTrsf;
  aOccTrsf.SetTranslation(gp_Vec(10.0, 0.0, 0.0));
  const BRepGraph_OccurrenceId anOcc =
    aGraph.Editor().Products().LinkProducts(anAssembly, aPart, TopLoc_Location(aOccTrsf));
  ASSERT_TRUE(anOcc.IsValid());

  gp_Trsf aRootTrsf;
  aRootTrsf.SetTranslation(gp_Vec(0.0, 20.0, 0.0));
  // Set the root location on the topology-root occurrence ref of the part product.
  {
    const BRepGraphInc::ProductDef& aPartDef = aGraph.Topo().Products().Definition(aPart);
    for (const BRepGraph_OccurrenceRefId& aRefId : aPartDef.OccurrenceRefIds)
    {
      const BRepGraphInc::OccurrenceRef& aOccRef = aGraph.Refs().Occurrences().Entry(aRefId);
      const BRepGraphInc::OccurrenceDef& anOccDef =
        aGraph.Topo().Occurrences().Definition(aOccRef.OccurrenceDefId);
      if (BRepGraph_NodeId::IsTopologyKind(anOccDef.ChildDefId.NodeKind))
      {
        BRepGraph_MutGuard<BRepGraphInc::OccurrenceRef> aMutRef =
          aGraph.Editor().Occurrences().MutRef(aRefId);
        aMutRef->LocalLocation = TopLoc_Location(aRootTrsf);
        break;
      }
    }
  }

  // Recursive traversal through Assembly->Occurrence->Product(part)->Solid.
  BRepGraph_ChildExplorer anIt(aGraph, anAssembly, BRepGraph_NodeId::Kind::Solid);
  ASSERT_TRUE(anIt.More());
  EXPECT_EQ(anIt.Current().DefId, BRepGraph_NodeId(BRepGraph_SolidId::Start()));

  const BRepGraphInc::NodeInstance aUsage = anIt.Current();
  const TopLoc_Location anExpectedLoc     = TopLoc_Location(aOccTrsf) * TopLoc_Location(aRootTrsf);
  const gp_Trsf&        anActualTrsf      = aUsage.Location.Transformation();
  const gp_Trsf&        anExpectedTrsf    = anExpectedLoc.Transformation();
  EXPECT_NEAR(anActualTrsf.TranslationPart().X(),
              anExpectedTrsf.TranslationPart().X(),
              Precision::Confusion());
  EXPECT_NEAR(anActualTrsf.TranslationPart().Y(),
              anExpectedTrsf.TranslationPart().Y(),
              Precision::Confusion());
  EXPECT_NEAR(anActualTrsf.TranslationPart().Z(),
              anExpectedTrsf.TranslationPart().Z(),
              Precision::Confusion());

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
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, aComp);
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (BRepGraph_ChildExplorer anIt = makeDirectChildExplorer(aGraph,
                                                              BRepGraph_CompoundId::Start(),
                                                              BRepGraph_NodeId::Kind::Solid);
       anIt.More();
       anIt.Next())
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
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, aComp);
  ASSERT_TRUE(aGraph.IsDone());

  int aFaceCount = 0;
  for (BRepGraph_ChildExplorer anExp(aGraph,
                                     BRepGraph_CompoundId::Start(),
                                     BRepGraph_NodeId::Kind::Face);
       anExp.More();
       anExp.Next())
    ++aFaceCount;
  // Each box has 6 faces.
  EXPECT_EQ(aFaceCount, THE_NB_CHILDREN * 6);
}

TEST(BRepGraph_ChildExplorerTest, StructuredBindings_NodeInstance)
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
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, aComp);
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (BRepGraph_ChildExplorer anExp(aGraph,
                                     BRepGraph_CompoundId::Start(),
                                     BRepGraph_NodeId::Kind::Face);
       anExp.More();
       anExp.Next())
  {
    const auto [aNodeId, aLoc, anOri] = anExp.Current();
    EXPECT_EQ(aNodeId.NodeKind, BRepGraph_NodeId::Kind::Face);
    EXPECT_FALSE(aLoc.IsIdentity());
    EXPECT_TRUE(anOri == TopAbs_FORWARD || anOri == TopAbs_REVERSED);
    ++aCount;
  }
  EXPECT_EQ(aCount, 6);
}

TEST(BRepGraph_ChildExplorerTest, RangeFor_NodeInstance)
{
  BRepGraph aGraph;
  aGraph.Clear(); (void)BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10, 20, 30).Shape());
  ASSERT_TRUE(aGraph.IsDone());

  int aCount = 0;
  for (const BRepGraphInc::NodeInstance& aUsage :
       BRepGraph_ChildExplorer(aGraph, BRepGraph_SolidId::Start(), BRepGraph_NodeId::Kind::Face))
  {
    EXPECT_EQ(aUsage.DefId.NodeKind, BRepGraph_NodeId::Kind::Face);
    ++aCount;
  }
  EXPECT_EQ(aCount, 6);
}
