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
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_TopoView.hxx>

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

// --- Explorer convenience tests ---

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
  const NCollection_Vector<BRepGraph_FaceRefId>& aFaceRefIds = aGraph.Refs().Faces().IdsOf(aShellId);

  NCollection_Vector<int> anExpectedFaceIds;
  for (int i = 0; i < aFaceRefIds.Length(); ++i)
  {
    const BRepGraphInc::FaceRef& aRef = aGraph.Refs().Faces().Entry(aFaceRefIds.Value(i));
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
  const NCollection_Vector<BRepGraph_FaceRefId>& aFaceRefIds = aGraph.Refs().Faces().IdsOf(aShellId);
  ASSERT_GT(aFaceRefIds.Length(), 0);

  const BRepGraph_FaceRefId aRemovedRef = aFaceRefIds.Value(0);
  const BRepGraph_FaceId aRemovedFaceId = aGraph.Refs().Faces().Entry(aRemovedRef).FaceDefId;

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
  const NCollection_Vector<BRepGraph_WireRefId>& aWireRefs = aGraph.Refs().Wires().IdsOf(aFaceId);
  ASSERT_GT(aWireRefs.Length(), 0);

  const BRepGraph_WireId aWireId = aGraph.Refs().Wires().Entry(aWireRefs.Value(0)).WireDefId;
  const NCollection_Vector<BRepGraph_CoEdgeRefId>& aCoEdgeRefs = aGraph.Refs().CoEdges().IdsOf(aWireId);

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
