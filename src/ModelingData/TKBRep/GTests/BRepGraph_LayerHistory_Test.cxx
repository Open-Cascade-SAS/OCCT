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

#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_RepId.hxx>
#include <BRepGraph_LayerHistory.hxx>
#include <BRepGraph_ReverseIterator.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_UIDsView.hxx>
#include "BRepGraph_RefTestTools.hxx"
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepTools_History.hxx>
#include <Geom_Plane.hxx>
#include <NCollection_DynamicArray.hxx>
#include <NCollection_FlatMap.hxx>
#include <Standard_Failure.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>

#include <limits>

#include <gtest/gtest.h>

class BRepGraph_LayerHistoryTest : public testing::Test
{
protected:
  void SetUp() override
  {
    BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
    myGraph.Clear();
    [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes1 =
      myGraph.Shapes().Add(aBoxMaker.Shape());
  }

  BRepGraph myGraph;
};

static NCollection_LinearVector<BRepGraph_WireId> collectWiresOfEdge(const BRepGraph& theGraph,
                                                                     const BRepGraph_EdgeId theEdge)
{
  NCollection_LinearVector<BRepGraph_WireId> aWires;
  for (BRepGraph_WiresOfEdge aWireIt = theGraph.Topo().Edges().WiresOf(theEdge); aWireIt.More();
       aWireIt.Next())
  {
    aWires.Append(aWireIt.CurrentId());
  }
  return aWires;
}

// ============================================================
// History chain tests
// ============================================================

TEST_F(BRepGraph_LayerHistoryTest, FindOriginal_ChainABC_ReturnsA)
{
  // Build a chain: edge0 -> edge1 -> edge2 via two ApplyModification calls.
  const BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId       anEdge1;
  BRepGraph_NodeId       anEdge2;

  myGraph.Editor().Gen().ApplyModification(
    anEdge0,
    [&](BRepGraph& theGraph,
        BRepGraph_NodeId /*theTarget*/) -> NCollection_LinearVector<BRepGraph_NodeId> {
      // Simulate producing a new edge node at index NbEdgeDefs.
      anEdge1 = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, theGraph.Topo().Edges().Nb());
      NCollection_LinearVector<BRepGraph_NodeId> aResult;
      aResult.Append(anEdge1);
      return aResult;
    },
    "Step1");

  myGraph.Editor().Gen().ApplyModification(
    anEdge1,
    [&](BRepGraph& theGraph,
        BRepGraph_NodeId /*theTarget*/) -> NCollection_LinearVector<BRepGraph_NodeId> {
      anEdge2 = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, theGraph.Topo().Edges().Nb());
      NCollection_LinearVector<BRepGraph_NodeId> aResult;
      aResult.Append(anEdge2);
      return aResult;
    },
    "Step2");

  const BRepGraph_NodeId anOriginal =
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->FindOriginal(anEdge2);
  EXPECT_TRUE(anOriginal.IsValid());
  EXPECT_EQ(anOriginal, anEdge0);
}

TEST_F(BRepGraph_LayerHistoryTest, FindDerived_ChainABC_ContainsBAndC)
{
  const BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId       anEdge1;
  BRepGraph_NodeId       anEdge2;

  myGraph.Editor().Gen().ApplyModification(
    anEdge0,
    [&](BRepGraph& theGraph, BRepGraph_NodeId) -> NCollection_LinearVector<BRepGraph_NodeId> {
      anEdge1 = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, theGraph.Topo().Edges().Nb());
      NCollection_LinearVector<BRepGraph_NodeId> aResult;
      aResult.Append(anEdge1);
      return aResult;
    },
    "Step1");

  myGraph.Editor().Gen().ApplyModification(
    anEdge1,
    [&](BRepGraph& theGraph, BRepGraph_NodeId) -> NCollection_LinearVector<BRepGraph_NodeId> {
      anEdge2 = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, theGraph.Topo().Edges().Nb());
      NCollection_LinearVector<BRepGraph_NodeId> aResult;
      aResult.Append(anEdge2);
      return aResult;
    },
    "Step2");

  const NCollection_LinearVector<BRepGraph_NodeId> aDerived =
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->FindDerived(anEdge0);
  bool hasEdge1 = false;
  bool hasEdge2 = false;
  for (const BRepGraph_NodeId& aDerivedId : aDerived)
  {
    if (aDerivedId == anEdge1)
    {
      hasEdge1 = true;
    }
    if (aDerivedId == anEdge2)
    {
      hasEdge2 = true;
    }
  }
  EXPECT_TRUE(hasEdge1);
  EXPECT_TRUE(hasEdge2);
}

TEST_F(BRepGraph_LayerHistoryTest, FindOriginal_UnmodifiedNode_ReturnsSelf)
{
  // FindOriginal returns the node itself when it is not derived from anything.
  const BRepGraph_NodeId aFace(BRepGraph_NodeId::Kind::Face, 0);
  const BRepGraph_NodeId anOriginal =
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->FindOriginal(aFace);
  EXPECT_TRUE(anOriginal.IsValid());
  EXPECT_EQ(anOriginal, aFace);
}

TEST_F(BRepGraph_LayerHistoryTest, FindDerived_UnmodifiedNode_ReturnsEmpty)
{
  const BRepGraph_NodeId                           aFace(BRepGraph_NodeId::Kind::Face, 0);
  const NCollection_LinearVector<BRepGraph_NodeId> aDerived =
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->FindDerived(aFace);
  EXPECT_EQ(aDerived.Size(), size_t(0));
}

TEST_F(BRepGraph_LayerHistoryTest, Disabled_RecordHistory_NoRecordStored)
{
  const size_t aNbBefore = myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords();
  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->SetEnabled(false);

  const BRepGraph_NodeId                     anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  NCollection_LinearVector<BRepGraph_NodeId> aReplacements;
  aReplacements.Append(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, 1));
  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record("Disabled",
                                                                   anEdge0,
                                                                   aReplacements.ToArray1());

  EXPECT_EQ(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords(), aNbBefore);
}

TEST_F(BRepGraph_LayerHistoryTest, Disabled_ApplyModification_ModifierStillRuns)
{
  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->SetEnabled(false);
  bool aModifierRan = false;

  const BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  myGraph.Editor().Gen().ApplyModification(
    anEdge0,
    [&](BRepGraph&, BRepGraph_NodeId) -> NCollection_LinearVector<BRepGraph_NodeId> {
      aModifierRan = true;
      return NCollection_LinearVector<BRepGraph_NodeId>();
    },
    "DisabledOp");

  EXPECT_TRUE(aModifierRan);
}

TEST_F(BRepGraph_LayerHistoryTest, ReEnabled_RecordsAfterReEnable)
{
  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->SetEnabled(false);
  EXPECT_FALSE(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->IsEnabled());

  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->SetEnabled(true);
  EXPECT_TRUE(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->IsEnabled());

  const size_t aNbBefore = myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords();
  const BRepGraph_NodeId                     anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  NCollection_LinearVector<BRepGraph_NodeId> aReplacements;
  aReplacements.Append(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, 1));
  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record("ReEnabled",
                                                                   anEdge0,
                                                                   aReplacements.ToArray1());

  EXPECT_EQ(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords(), aNbBefore + 1);
}

TEST_F(BRepGraph_LayerHistoryTest, ApplyModification_EmptyReplacements)
{
  const BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  const size_t aNbBefore = myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords();

  myGraph.Editor().Gen().ApplyModification(
    anEdge0,
    [](BRepGraph&, BRepGraph_NodeId) -> NCollection_LinearVector<BRepGraph_NodeId> {
      return NCollection_LinearVector<BRepGraph_NodeId>();
    },
    "Delete");

  EXPECT_EQ(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords(), aNbBefore + 1);
  const BRepGraph_LayerHistory::Event& aRec =
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record(
      myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords() - 1);
  EXPECT_TRUE(aRec.OperationName.IsEqual("Delete"));
}

TEST_F(BRepGraph_LayerHistoryTest, ApplyModification_MultipleReplacements)
{
  const BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId       aNew1;
  BRepGraph_NodeId       aNew2;

  myGraph.Editor().Gen().ApplyModification(
    anEdge0,
    [&](BRepGraph& theGraph, BRepGraph_NodeId) -> NCollection_LinearVector<BRepGraph_NodeId> {
      const uint32_t aBase = theGraph.Topo().Edges().Nb();
      aNew1                = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, aBase);
      aNew2                = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, aBase + 1);
      NCollection_LinearVector<BRepGraph_NodeId> aResult;
      aResult.Append(aNew1);
      aResult.Append(aNew2);
      return aResult;
    },
    "Split");

  const NCollection_LinearVector<BRepGraph_NodeId> aDerived =
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->FindDerived(anEdge0);
  bool hasNew1 = false;
  bool hasNew2 = false;
  for (const BRepGraph_NodeId& aDerivedId : aDerived)
  {
    if (aDerivedId == aNew1)
    {
      hasNew1 = true;
    }
    if (aDerivedId == aNew2)
    {
      hasNew2 = true;
    }
  }
  EXPECT_TRUE(hasNew1);
  EXPECT_TRUE(hasNew2);
}

TEST_F(BRepGraph_LayerHistoryTest, RecordHistory_EmptyReplacements_Stored)
{
  const size_t aNbBefore = myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords();
  const BRepGraph_NodeId                     anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  NCollection_LinearVector<BRepGraph_NodeId> anEmpty;
  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record("Erase",
                                                                   anEdge0,
                                                                   anEmpty.ToArray1());

  EXPECT_EQ(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords(), aNbBefore + 1);
}

TEST_F(BRepGraph_LayerHistoryTest, HistoryRecord_SequenceNumber_Monotonic)
{
  const BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  const BRepGraph_NodeId anEdge1(BRepGraph_NodeId::Kind::Edge, 1);
  const BRepGraph_NodeId anEdge2(BRepGraph_NodeId::Kind::Edge, 2);

  NCollection_LinearVector<BRepGraph_NodeId> aRepl;
  aRepl.Append(anEdge1);
  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record("Op1",
                                                                   anEdge0,
                                                                   aRepl.ToArray1());

  NCollection_LinearVector<BRepGraph_NodeId> aRepl2;
  aRepl2.Append(anEdge2);
  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record("Op2",
                                                                   anEdge1,
                                                                   aRepl2.ToArray1());

  const size_t aNb = myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords();
  ASSERT_GE(aNb, 2);
  const BRepGraph_LayerHistory::Event& aRec1 =
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record(aNb - 2);
  const BRepGraph_LayerHistory::Event& aRec2 =
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record(aNb - 1);
  EXPECT_LT(aRec1.SequenceNumber, aRec2.SequenceNumber);
}

TEST_F(BRepGraph_LayerHistoryTest, HistoryRecord_OperationName_Stored)
{
  const BRepGraph_NodeId                     anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  NCollection_LinearVector<BRepGraph_NodeId> aRepl;
  aRepl.Append(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, 1));
  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record("MyCustomOp",
                                                                   anEdge0,
                                                                   aRepl.ToArray1());

  const BRepGraph_LayerHistory::Event& aRec =
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record(
      myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords() - 1);
  EXPECT_TRUE(aRec.OperationName.IsEqual("MyCustomOp"));
}

TEST_F(BRepGraph_LayerHistoryTest, NbHistoryRecords_AfterMultipleOps_Correct)
{
  const size_t aNbBefore = myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords();

  const BRepGraph_NodeId                     anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  NCollection_LinearVector<BRepGraph_NodeId> aRepl;
  aRepl.Append(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, 1));

  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record("A", anEdge0, aRepl.ToArray1());
  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record("B", anEdge0, aRepl.ToArray1());
  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record("C", anEdge0, aRepl.ToArray1());

  EXPECT_EQ(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords(), aNbBefore + 3);
}

TEST_F(BRepGraph_LayerHistoryTest, FindOriginal_TwoApply_TransitiveTrace)
{
  const BRepGraph_NodeId aVtx0(BRepGraph_NodeId::Kind::Vertex, 0);
  BRepGraph_NodeId       aVtx1;
  BRepGraph_NodeId       aVtx2;

  myGraph.Editor().Gen().ApplyModification(
    aVtx0,
    [&](BRepGraph& theGraph, BRepGraph_NodeId) -> NCollection_LinearVector<BRepGraph_NodeId> {
      aVtx1 = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Vertex, theGraph.Topo().Vertices().Nb());
      NCollection_LinearVector<BRepGraph_NodeId> aResult;
      aResult.Append(aVtx1);
      return aResult;
    },
    "Move1");

  myGraph.Editor().Gen().ApplyModification(
    aVtx1,
    [&](BRepGraph& theGraph, BRepGraph_NodeId) -> NCollection_LinearVector<BRepGraph_NodeId> {
      aVtx2 = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Vertex, theGraph.Topo().Vertices().Nb());
      NCollection_LinearVector<BRepGraph_NodeId> aResult;
      aResult.Append(aVtx2);
      return aResult;
    },
    "Move2");

  // FindOriginal from the end of a 2-step chain should reach the root.
  const BRepGraph_NodeId anOriginal =
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->FindOriginal(aVtx2);
  EXPECT_TRUE(anOriginal.IsValid());
  EXPECT_EQ(anOriginal, aVtx0);

  // Intermediate node should also trace back to root.
  const BRepGraph_NodeId anOriginal1 =
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->FindOriginal(aVtx1);
  EXPECT_TRUE(anOriginal1.IsValid());
  EXPECT_EQ(anOriginal1, aVtx0);
}

TEST_F(BRepGraph_LayerHistoryTest, ApplyModification_WhenModifierThrows_DoesNotRecordHistory)
{
  const size_t aNbRecordsBefore =
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords();

  const BRepGraph_NodeId anEdge(BRepGraph_NodeId::Kind::Edge, 0);

#if !defined(No_Exception)
  EXPECT_THROW(myGraph.Editor().Gen().ApplyModification(
                 anEdge,
                 [](BRepGraph&, BRepGraph_NodeId) -> NCollection_LinearVector<BRepGraph_NodeId> {
                   throw Standard_Failure("Synthetic failure");
                 },
                 "ThrowingModification"),
               Standard_Failure);
#endif

  EXPECT_EQ(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords(),
            aNbRecordsBefore);
}

TEST_F(BRepGraph_LayerHistoryTest, SplitEdge_RewritesAllContainingWires)
{
  ASSERT_GT(myGraph.Topo().Edges().Nb(), 0u);

  const BRepGraph_EdgeId          anEdgeId(0);
  const std::pair<double, double> anEdgeRange = BRepGraph_Tool::Edge::Range(myGraph, anEdgeId);

  const double aSplitParam = 0.5 * (anEdgeRange.first + anEdgeRange.second);

  const uint32_t           aNbVerticesBefore = myGraph.Topo().Vertices().Nb();
  const gp_Pnt             aSplitPoint(1.0, 2.0, 3.0);
  const BRepGraph_VertexId aSplitVertex = myGraph.Editor().Vertices().Add(aSplitPoint, 1.0e-7);

  ASSERT_TRUE(aSplitVertex.IsValid());
  EXPECT_EQ(myGraph.Topo().Vertices().Nb(), aNbVerticesBefore + 1);

  const NCollection_LinearVector<BRepGraph_WireId> aWireIndices =
    collectWiresOfEdge(myGraph, anEdgeId);
  ASSERT_GT(aWireIndices.Size(), 0u);

  const uint32_t aNbEdgesBefore       = myGraph.Topo().Edges().Nb();
  const uint32_t aNbActiveEdgesBefore = myGraph.Topo().Edges().NbActive();

  BRepGraph_EdgeId aSubA;
  BRepGraph_EdgeId aSubB;

  myGraph.Editor().Edges().Split(anEdgeId, aSplitVertex, aSplitParam, aSubA, aSubB);

  ASSERT_TRUE(aSubA.IsValid());
  ASSERT_TRUE(aSubB.IsValid());
  EXPECT_EQ(myGraph.Topo().Edges().Nb(), aNbEdgesBefore + 2);
  EXPECT_EQ(myGraph.Topo().Edges().NbActive(), aNbActiveEdgesBefore + 1);

  for (const BRepGraph_WireId& aWireId : aWireIndices)
  {
    const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdgeIds =
      myGraph.Topo().Wires().Relations(aWireId).CoEdgeIds;

    bool             hasOld        = false;
    bool             hasSubA       = false;
    bool             hasSubB       = false;
    constexpr size_t THE_NOT_FOUND = std::numeric_limits<size_t>::max();
    size_t           aSubAOrd      = THE_NOT_FOUND;
    size_t           aSubBOrd      = THE_NOT_FOUND;

    for (size_t anIdx = 0; anIdx < aCoEdgeIds.Size(); ++anIdx)
    {
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        myGraph.Topo().CoEdges().Definition(aCoEdgeIds.Value(anIdx));
      const BRepGraph_NodeId anId(aCoEdge.ChildEdgeId);
      if (anId == anEdgeId)
      {
        hasOld = true;
      }
      else if (anId == aSubA)
      {
        hasSubA  = true;
        aSubAOrd = anIdx;
      }
      else if (anId == aSubB)
      {
        hasSubB  = true;
        aSubBOrd = anIdx;
      }
    }

    EXPECT_FALSE(hasOld);
    EXPECT_TRUE(hasSubA);
    EXPECT_TRUE(hasSubB);
    EXPECT_NE(aSubAOrd, THE_NOT_FOUND);
    EXPECT_NE(aSubBOrd, THE_NOT_FOUND);
    const size_t aDistance = (aSubAOrd < aSubBOrd) ? (aSubBOrd - aSubAOrd) : (aSubAOrd - aSubBOrd);
    EXPECT_TRUE(aDistance == 1 || aDistance + 1 == aCoEdgeIds.Size());
  }
}

TEST_F(BRepGraph_LayerHistoryTest, SplitEdge_IgnoresRemovedCoEdgeEntries)
{
  ASSERT_GT(myGraph.Topo().Edges().Nb(), 0u);

  const BRepGraph_EdgeId          anEdgeId(0);
  const std::pair<double, double> anEdgeRange = BRepGraph_Tool::Edge::Range(myGraph, anEdgeId);
  const double                    aSplitParam = 0.5 * (anEdgeRange.first + anEdgeRange.second);

  const NCollection_LinearVector<BRepGraph_WireId> aWireIndices =
    collectWiresOfEdge(myGraph, anEdgeId);
  ASSERT_GT(aWireIndices.Size(), 1);

  const BRepGraph_WireId                              aWireId = aWireIndices.Value(0);
  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aWireCoEdgesBefore =
    myGraph.Topo().Wires().Relations(aWireId).CoEdgeIds;
  ASSERT_GT(aWireCoEdgesBefore.Size(), 0u);

  constexpr size_t   THE_NOT_FOUND = std::numeric_limits<size_t>::max();
  BRepGraph_CoEdgeId aCoEdgeToRemove;
  size_t             aRemovedOrd = THE_NOT_FOUND;
  for (size_t aRefOrd = 0; aRefOrd < aWireCoEdgesBefore.Size(); ++aRefOrd)
  {
    const BRepGraph_CoEdgeId       aCoEdgeId = aWireCoEdgesBefore.Value(aRefOrd);
    const BRepGraphInc::CoEdgeDef& aCoEdge   = myGraph.Topo().CoEdges().Definition(aCoEdgeId);
    if (aCoEdge.ChildEdgeId == anEdgeId)
    {
      aCoEdgeToRemove = aCoEdgeId;
      aRemovedOrd     = aRefOrd;
      break;
    }
  }
  ASSERT_TRUE(aCoEdgeToRemove.IsValid(myGraph.Topo().CoEdges().Nb()));
  ASSERT_NE(aRemovedOrd, THE_NOT_FOUND);

  myGraph.Editor().Gen().RemoveNode(BRepGraph_NodeId(aCoEdgeToRemove));
  ASSERT_TRUE(aCoEdgeToRemove.IsRemoved(myGraph));
  const BRepGraph_CoEdgeId aRemovedCoEdgeId = aCoEdgeToRemove;

  const size_t aRemovedWireNbActiveBefore =
    myGraph.Topo().Wires().Relations(aWireId).CoEdgeIds.Size();

  const BRepGraph_VertexId aSplitVertex =
    myGraph.Editor().Vertices().Add(gp_Pnt(4.0, 5.0, 6.0), 1.0e-7);
  ASSERT_TRUE(aSplitVertex.IsValid());

  BRepGraph_EdgeId aSubA;
  BRepGraph_EdgeId aSubB;
  myGraph.Editor().Edges().Split(anEdgeId, aSplitVertex, aSplitParam, aSubA, aSubB);
  ASSERT_TRUE(aSubA.IsValid());
  ASSERT_TRUE(aSubB.IsValid());

  EXPECT_TRUE(aCoEdgeToRemove.IsRemoved(myGraph));
  EXPECT_EQ(myGraph.Topo().Wires().Relations(aWireId).CoEdgeIds.Size(), aRemovedWireNbActiveBefore);
  const BRepGraphInc::CoEdgeDef& aRemovedCoEdgeAfter =
    myGraph.Topo().CoEdges().Definition(aRemovedCoEdgeId);
  EXPECT_EQ(aRemovedCoEdgeAfter.ChildEdgeId, anEdgeId);

  bool hasSubA = false;
  bool hasSubB = false;
  for (BRepGraph_CoEdgeId aCoEdgeId = BRepGraph_CoEdgeId::Start();
       aCoEdgeId.IsValid(myGraph.Topo().CoEdges().Nb());
       ++aCoEdgeId)
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = myGraph.Topo().CoEdges().Definition(aCoEdgeId);
    if (aCoEdgeId.IsRemoved(myGraph))
    {
      continue;
    }
    const BRepGraph_NodeId anId(aCoEdge.ChildEdgeId);
    if (anId == aSubA)
    {
      hasSubA = true;
    }
    if (anId == aSubB)
    {
      hasSubB = true;
    }
  }
  EXPECT_TRUE(hasSubA);
  EXPECT_TRUE(hasSubB);
}

TEST_F(BRepGraph_LayerHistoryTest, ApplyModification_SplitEdge_RecordsBothDerivedNodes)
{
  ASSERT_GT(myGraph.Topo().Edges().Nb(), 0u);

  const BRepGraph_EdgeId          anEdgeId(0);
  const std::pair<double, double> anEdgeRange = BRepGraph_Tool::Edge::Range(myGraph, anEdgeId);
  const double                    aSplitParam = 0.5 * (anEdgeRange.first + anEdgeRange.second);

  const BRepGraph_VertexId aSplitVertex =
    myGraph.Editor().Vertices().Add(gp_Pnt(4.0, 5.0, 6.0), 1.0e-7);
  ASSERT_TRUE(aSplitVertex.IsValid());

  const size_t aNbRecordsBefore =
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords();

  myGraph.Editor().Gen().ApplyModification(
    anEdgeId,
    [&](BRepGraph&       theGraph,
        BRepGraph_NodeId theTarget) -> NCollection_LinearVector<BRepGraph_NodeId> {
      BRepGraph_EdgeId aSubA;
      BRepGraph_EdgeId aSubB;
      theGraph.Editor().Edges().Split(BRepGraph_EdgeId::FromNodeId(theTarget),
                                      aSplitVertex,
                                      aSplitParam,
                                      aSubA,
                                      aSubB);

      NCollection_LinearVector<BRepGraph_NodeId> aResult;
      aResult.Append(aSubA);
      aResult.Append(aSubB);
      return aResult;
    },
    "Split");

  EXPECT_EQ(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords(),
            aNbRecordsBefore + 1);

  const NCollection_LinearVector<BRepGraph_NodeId> aDerived =
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->FindDerived(anEdgeId);
  EXPECT_GE(aDerived.Size(), 2);
}

TEST_F(BRepGraph_LayerHistoryTest, FindModified_PerKindLookup_ReturnsOnlyModified)
{
  const BRepGraph_NodeId anOrig(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId       aRepl;

  myGraph.Editor().Gen().ApplyModification(
    anOrig,
    [&](BRepGraph& theGraph, BRepGraph_NodeId) -> NCollection_LinearVector<BRepGraph_NodeId> {
      aRepl = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, theGraph.Topo().Edges().Nb());
      NCollection_LinearVector<BRepGraph_NodeId> aResult;
      aResult.Append(aRepl);
      return aResult;
    },
    "Modify");

  const NCollection_LinearVector<BRepGraph_NodeId>* aMod =
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->FindModified(anOrig);
  ASSERT_NE(aMod, nullptr);
  EXPECT_EQ(aMod->Size(), 1);
  EXPECT_EQ(aMod->Value(0), aRepl);

  const NCollection_LinearVector<BRepGraph_NodeId>* aGen =
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->FindGenerated(anOrig);
  EXPECT_EQ(aGen, nullptr);
}

TEST_F(BRepGraph_LayerHistoryTest, FindGenerated_PerKindLookup_ReturnsOnlyGenerated)
{
  const BRepGraph_NodeId anOrig(BRepGraph_NodeId::Kind::Edge, 0);
  const BRepGraph_NodeId anRepl(BRepGraph_NodeId::Kind::Edge, 1);

  NCollection_LinearVector<BRepGraph_NodeId> aReplVec;
  aReplVec.Append(anRepl);

  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record(
    "Generate",
    anOrig,
    aReplVec.ToArray1(),
    BRepGraph_LayerHistory::Kind::Generated);

  const NCollection_LinearVector<BRepGraph_NodeId>* aGen =
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->FindGenerated(anOrig);
  ASSERT_NE(aGen, nullptr);
  EXPECT_EQ(aGen->Size(), 1);
  EXPECT_EQ(aGen->Value(0), anRepl);

  const NCollection_LinearVector<BRepGraph_NodeId>* aMod =
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->FindModified(anOrig);
  EXPECT_EQ(aMod, nullptr);
}

TEST_F(BRepGraph_LayerHistoryTest, RecordDeleted_MarksNodesAsDeleted)
{
  const BRepGraph_NodeId anEdgeA(BRepGraph_NodeId::Kind::Edge, 0);
  const BRepGraph_NodeId anEdgeB(BRepGraph_NodeId::Kind::Edge, 1);

  NCollection_LinearVector<BRepGraph_NodeId> aDeleted;
  aDeleted.Append(anEdgeA);
  aDeleted.Append(anEdgeB);

  const size_t aNbBefore = myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords();
  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->RecordDeleted("DeleteEdges",
                                                                          aDeleted.ToArray1());
  EXPECT_EQ(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords(), aNbBefore + 1);

  EXPECT_TRUE(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->IsDeleted(anEdgeA));
  EXPECT_TRUE(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->IsDeleted(anEdgeB));

  const NCollection_FlatMap<BRepGraph_NodeId>& aDeletedSet =
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->DeletedNodes();
  EXPECT_TRUE(aDeletedSet.Contains(anEdgeA));
  EXPECT_TRUE(aDeletedSet.Contains(anEdgeB));
}

TEST_F(BRepGraph_LayerHistoryTest, RecordDeleted_EmptyList_IsNoop)
{
  const size_t aNbBefore = myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords();
  NCollection_LinearVector<BRepGraph_NodeId> aEmpty;
  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->RecordDeleted("Noop",
                                                                          aEmpty.ToArray1());
  EXPECT_EQ(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords(), aNbBefore);
}

TEST_F(BRepGraph_LayerHistoryTest, Record_EmptyReplacements_AutoDeleted)
{
  const BRepGraph_NodeId                     anOrig(BRepGraph_NodeId::Kind::Edge, 0);
  NCollection_LinearVector<BRepGraph_NodeId> aEmpty;

  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record(
    "Consumed",
    anOrig,
    aEmpty.ToArray1(),
    BRepGraph_LayerHistory::Kind::Generated);

  EXPECT_TRUE(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->IsDeleted(anOrig));
  EXPECT_EQ(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords(), 1);
  EXPECT_EQ(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record(size_t(0)).RecordKind,
            BRepGraph_LayerHistory::Kind::Deleted);
}

TEST_F(BRepGraph_LayerHistoryTest, FindDerived_ModifiedAndGenerated_CollectsBoth)
{
  const BRepGraph_NodeId anOrig(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId       aModRepl;
  BRepGraph_NodeId       aGenRepl;

  myGraph.Editor().Gen().ApplyModification(
    anOrig,
    [&](BRepGraph& theGraph, BRepGraph_NodeId) -> NCollection_LinearVector<BRepGraph_NodeId> {
      aModRepl = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, theGraph.Topo().Edges().Nb());
      NCollection_LinearVector<BRepGraph_NodeId> aResult;
      aResult.Append(aModRepl);
      return aResult;
    },
    "FirstModified");

  // Record a Generated event on the Modified result.
  aGenRepl = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, 2);
  NCollection_LinearVector<BRepGraph_NodeId> aGenRepVec;
  aGenRepVec.Append(aGenRepl);
  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record(
    "ThenGenerated",
    aModRepl,
    aGenRepVec.ToArray1(),
    BRepGraph_LayerHistory::Kind::Generated);

  const NCollection_LinearVector<BRepGraph_NodeId> aAll =
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->FindDerived(anOrig);
  EXPECT_GE(aAll.Size(), 1);
  bool aFoundGen = false;
  for (const BRepGraph_NodeId& aNode : aAll)
  {
    if (aNode == aGenRepl)
    {
      aFoundGen = true;
    }
  }
  EXPECT_TRUE(aFoundGen);
  // Verify the Generated record is in the per-kind map.
  EXPECT_NE(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->FindGenerated(aModRepl),
            nullptr);
  EXPECT_NE(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->FindModified(anOrig),
            nullptr);
}

TEST_F(BRepGraph_LayerHistoryTest, RecordBatch_ExplicitGeneratedKind_StoredCorrectly)
{
  const BRepGraph_NodeId anOrig(BRepGraph_NodeId::Kind::Edge, 0);
  const BRepGraph_NodeId anRepl(BRepGraph_NodeId::Kind::Edge, 1);

  NCollection_LinearVector<BRepGraph_NodeId> aOriginals;
  NCollection_LinearVector<BRepGraph_NodeId> aReplacements;
  aOriginals.Append(anOrig);
  aReplacements.Append(anRepl);

  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->RecordBatch(
    "BatchGen",
    aOriginals.ToArray1(),
    aReplacements.ToArray1(),
    TCollection_AsciiString(),
    BRepGraph_LayerHistory::Kind::Generated);

  EXPECT_EQ(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords(), 1);
  EXPECT_EQ(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record(size_t(0)).RecordKind,
            BRepGraph_LayerHistory::Kind::Generated);
  EXPECT_NE(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->FindGenerated(anOrig),
            nullptr);
  EXPECT_EQ(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->FindModified(anOrig),
            nullptr);
}

TEST_F(BRepGraph_LayerHistoryTest, Record_ExplicitModifiedKind_DefaultMatches)
{
  NCollection_LinearVector<BRepGraph_NodeId> aRepls;
  aRepls.Append(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, 1));

  const size_t aNbBefore = myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords();
  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record(
    "ExplicitMod",
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, 0),
    aRepls.ToArray1(),
    BRepGraph_LayerHistory::Kind::Modified);
  EXPECT_EQ(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords(), aNbBefore + 1);
  EXPECT_EQ(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record(aNbBefore).RecordKind,
            BRepGraph_LayerHistory::Kind::Modified);
}

TEST_F(BRepGraph_LayerHistoryTest, IsDeleted_UnrelatedNode_ReturnsFalse)
{
  const BRepGraph_NodeId aNode(BRepGraph_NodeId::Kind::Edge, 0);
  EXPECT_FALSE(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->IsDeleted(aNode));
}

TEST_F(BRepGraph_LayerHistoryTest, RecordReplaced_MapsImageAndMarksDeleted)
{
  const BRepGraph_NodeId anOrig(BRepGraph_NodeId::Kind::Face, 0);
  const BRepGraph_NodeId aReplacement(BRepGraph_NodeId::Kind::Face, 1);

  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->RecordReplaced("ReplaceFace",
                                                                           anOrig,
                                                                           aReplacement);

  ASSERT_EQ(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords(), size_t(1));
  EXPECT_EQ(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record(size_t(0)).RecordKind,
            BRepGraph_LayerHistory::Kind::Replaced);
  EXPECT_TRUE(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->IsDeleted(anOrig));

  const NCollection_LinearVector<BRepGraph_NodeId>* aModified =
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->FindModified(anOrig);
  ASSERT_NE(aModified, nullptr);
  ASSERT_EQ(aModified->Size(), size_t(1));
  EXPECT_EQ(aModified->Value(0), aReplacement);

  const NCollection_LinearVector<BRepGraph_NodeId>* anOrigins =
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->FindOriginals(aReplacement);
  ASSERT_NE(anOrigins, nullptr);
  ASSERT_EQ(anOrigins->Size(), size_t(1));
  EXPECT_EQ(anOrigins->Value(0), anOrig);
}

TEST_F(BRepGraph_LayerHistoryTest, StructuralReplacement_DoesNotEmitSemanticHistory)
{
  ASSERT_GT(myGraph.Topo().Faces().Nb(), 1);
  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Clear();

  const BRepGraph_FaceId aRemoved(1);
  const BRepGraph_FaceId aReplacement(0);
  myGraph.Editor().Gen().ReplaceNode(aRemoved, aReplacement);

  EXPECT_EQ(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->NbRecords(), size_t(0));
  EXPECT_FALSE(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->IsDeleted(aRemoved));
  EXPECT_EQ(myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->FindModified(aRemoved),
            nullptr);
}

TEST_F(BRepGraph_LayerHistoryTest, FindOriginals_DerivedWithTwoParents_ReturnsBoth)
{
  const BRepGraph_NodeId anOrigA(BRepGraph_NodeId::Kind::Face, 0);
  const BRepGraph_NodeId anOrigB(BRepGraph_NodeId::Kind::Face, 1);
  const BRepGraph_NodeId aDerived(BRepGraph_NodeId::Kind::Edge, 0);

  NCollection_LinearVector<BRepGraph_NodeId> aFirst;
  aFirst.Append(aDerived);
  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record("FirstParent",
                                                                   anOrigA,
                                                                   aFirst.ToArray1());

  NCollection_LinearVector<BRepGraph_NodeId> aSecond;
  aSecond.Append(aDerived);
  myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->Record("SecondParent",
                                                                   anOrigB,
                                                                   aSecond.ToArray1());

  const NCollection_LinearVector<BRepGraph_NodeId>* anOrigins =
    myGraph.LayerRegistry().Ensure<BRepGraph_LayerHistory>()->FindOriginals(aDerived);
  ASSERT_NE(anOrigins, nullptr);
  EXPECT_EQ(anOrigins->Size(), size_t(2));

  bool aHasA = false;
  bool aHasB = false;
  for (const BRepGraph_NodeId& anOrigin : *anOrigins)
  {
    if (anOrigin == anOrigA)
    {
      aHasA = true;
    }
    if (anOrigin == anOrigB)
    {
      aHasB = true;
    }
  }
  EXPECT_TRUE(aHasA);
  EXPECT_TRUE(aHasB);
}

TEST_F(BRepGraph_LayerHistoryTest, RecordUid_Modified_AppendsAuditRecord)
{
  const BRepGraph_UID anOrig(BRepGraph_NodeId::Kind::Face, 10);
  const BRepGraph_UID aRepl(BRepGraph_NodeId::Kind::Face, 11);

  NCollection_LinearVector<BRepGraph_UID> aReplacements;
  aReplacements.Append(aRepl);

  BRepGraph_LayerHistory aHist;
  aHist.RecordUid("UidMod",
                  anOrig,
                  aReplacements.ToArray1(),
                  BRepGraph_LayerHistory::Kind::Modified);

  ASSERT_EQ(aHist.NbRecords(), size_t(1));
  const BRepGraph_LayerHistory::Event& aRecord = aHist.Record(size_t(0));
  EXPECT_TRUE(aRecord.OperationName.IsEqual("UidMod"));
  EXPECT_EQ(aRecord.RecordKind, BRepGraph_LayerHistory::Kind::Modified);
  ASSERT_TRUE(aRecord.UidMapping.IsBound(anOrig));
  ASSERT_EQ(aRecord.UidMapping.Find(anOrig).Size(), size_t(1));
  EXPECT_EQ(aRecord.UidMapping.Find(anOrig).Value(0), aRepl);
  EXPECT_TRUE(aRecord.Mapping.IsEmpty());
  EXPECT_TRUE(aHist.HasKnownInput(anOrig));

  const NCollection_LinearVector<BRepGraph_UID>* aMod = aHist.FindModified(anOrig);
  ASSERT_NE(aMod, nullptr);
  ASSERT_EQ(aMod->Size(), size_t(1));
  EXPECT_EQ(aMod->Value(0), aRepl);
}

TEST_F(BRepGraph_LayerHistoryTest, RecordDeletedUid_AppendsAuditRecord)
{
  const BRepGraph_UID anOrigA(BRepGraph_NodeId::Kind::Edge, 20);
  const BRepGraph_UID anOrigB(BRepGraph_NodeId::Kind::Edge, 21);

  NCollection_LinearVector<BRepGraph_UID> aDeleted;
  aDeleted.Append(anOrigA);
  aDeleted.Append(anOrigB);

  BRepGraph_LayerHistory aHist;
  aHist.RecordDeletedUid("UidDelete", aDeleted.ToArray1());

  ASSERT_EQ(aHist.NbRecords(), size_t(1));
  const BRepGraph_LayerHistory::Event& aRecord = aHist.Record(size_t(0));
  EXPECT_TRUE(aRecord.OperationName.IsEqual("UidDelete"));
  EXPECT_EQ(aRecord.RecordKind, BRepGraph_LayerHistory::Kind::Deleted);
  ASSERT_TRUE(aRecord.UidMapping.IsBound(anOrigA));
  ASSERT_TRUE(aRecord.UidMapping.IsBound(anOrigB));
  EXPECT_TRUE(aRecord.UidMapping.Find(anOrigA).IsEmpty());
  EXPECT_TRUE(aRecord.UidMapping.Find(anOrigB).IsEmpty());
  EXPECT_TRUE(aHist.IsDeleted(anOrigA));
  EXPECT_TRUE(aHist.IsDeleted(anOrigB));
  EXPECT_TRUE(aHist.HasKnownInput(anOrigA));
  EXPECT_TRUE(aHist.HasKnownInput(anOrigB));
}

// ============================================================
// Absorb (BRepTools_History -> BRepGraph_LayerHistory bridge) tests
// ============================================================

namespace
{
TopoDS_Shape MakeVertexShape(const double theX, const double theY, const double theZ)
{
  BRepBuilderAPI_MakeVertex aMaker(gp_Pnt(theX, theY, theZ));
  return aMaker.Shape();
}
} // namespace

TEST_F(BRepGraph_LayerHistoryTest, Absorb_NullSource_NoOp)
{
  BRepGraph_LayerHistory                                                       aHist;
  NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher> anInputs;
  NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher> anOutputs;
  const occ::handle<BRepTools_History>                                         aNullSrc;
  aHist.Absorb(anInputs, anOutputs, aNullSrc, "NullSrc");
  EXPECT_EQ(aHist.NbRecords(), size_t(0));
}

TEST_F(BRepGraph_LayerHistoryTest, Absorb_EmptyInputs_NoOp)
{
  BRepGraph_LayerHistory                                                       aHist;
  NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher> anInputs;
  NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher> anOutputs;
  const occ::handle<BRepTools_History> aSrc = new BRepTools_History();
  aHist.Absorb(anInputs, anOutputs, aSrc, "EmptyInputs");
  EXPECT_EQ(aHist.NbRecords(), size_t(0));
}

TEST_F(BRepGraph_LayerHistoryTest, Absorb_ModifiedOnly_EmitsModifiedRecord)
{
  const TopoDS_Shape     anInShape  = MakeVertexShape(0, 0, false);
  const TopoDS_Shape     anOutShape = MakeVertexShape(1, 0, false);
  const BRepGraph_NodeId anInNode(BRepGraph_NodeId::Kind::Vertex, 100);
  const BRepGraph_NodeId anOutNode(BRepGraph_NodeId::Kind::Vertex, 101);

  occ::handle<BRepTools_History> aSrc = new BRepTools_History();
  aSrc->AddModified(anInShape, anOutShape);

  NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher> anInputs;
  anInputs.Bind(anInShape, anInNode);
  NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher> anOutputs;
  anOutputs.Bind(anOutShape, anOutNode);

  BRepGraph_LayerHistory aHist;
  aHist.Absorb(anInputs, anOutputs, aSrc, "Mod");

  ASSERT_EQ(aHist.NbRecords(), size_t(1));
  EXPECT_EQ(aHist.Record(size_t(0)).RecordKind, BRepGraph_LayerHistory::Kind::Modified);

  const NCollection_LinearVector<BRepGraph_NodeId>* aMod = aHist.FindModified(anInNode);
  ASSERT_NE(aMod, nullptr);
  ASSERT_EQ(aMod->Size(), size_t(1));
  EXPECT_EQ(aMod->Value(0), anOutNode);
  EXPECT_EQ(aHist.FindGenerated(anInNode), nullptr);
  EXPECT_FALSE(aHist.IsDeleted(anInNode));
}

TEST_F(BRepGraph_LayerHistoryTest, Absorb_GeneratedOnly_EmitsGeneratedRecord)
{
  const TopoDS_Shape     anInShape  = MakeVertexShape(0, 0, false);
  const TopoDS_Shape     anOutShape = MakeVertexShape(2, 0, false);
  const BRepGraph_NodeId anInNode(BRepGraph_NodeId::Kind::Vertex, 200);
  const BRepGraph_NodeId anOutNode(BRepGraph_NodeId::Kind::Vertex, 201);

  occ::handle<BRepTools_History> aSrc = new BRepTools_History();
  aSrc->AddGenerated(anInShape, anOutShape);

  NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher> anInputs;
  anInputs.Bind(anInShape, anInNode);
  NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher> anOutputs;
  anOutputs.Bind(anOutShape, anOutNode);

  BRepGraph_LayerHistory aHist;
  aHist.Absorb(anInputs, anOutputs, aSrc, "Gen");

  ASSERT_EQ(aHist.NbRecords(), size_t(1));
  EXPECT_EQ(aHist.Record(size_t(0)).RecordKind, BRepGraph_LayerHistory::Kind::Generated);

  const NCollection_LinearVector<BRepGraph_NodeId>* aGen = aHist.FindGenerated(anInNode);
  ASSERT_NE(aGen, nullptr);
  ASSERT_EQ(aGen->Size(), size_t(1));
  EXPECT_EQ(aGen->Value(0), anOutNode);
  EXPECT_EQ(aHist.FindModified(anInNode), nullptr);
}

TEST_F(BRepGraph_LayerHistoryTest, Absorb_RemovedTakesPrecedenceOverModified)
{
  // The OCCT bug being guarded against: a shape can show up as both
  // IsRemoved() and in Modified()/Generated().  Absorb must classify it
  // as a deletion event regardless.
  const TopoDS_Shape     anInShape = MakeVertexShape(0, 0, false);
  const TopoDS_Shape     aGhostOut = MakeVertexShape(3, 0, false);
  const BRepGraph_NodeId anInNode(BRepGraph_NodeId::Kind::Vertex, 300);
  const BRepGraph_NodeId aGhostNode(BRepGraph_NodeId::Kind::Vertex, 301);

  occ::handle<BRepTools_History> aSrc = new BRepTools_History();
  aSrc->AddModified(anInShape, aGhostOut);
  aSrc->Remove(anInShape);

  NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher> anInputs;
  anInputs.Bind(anInShape, anInNode);
  NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher> anOutputs;
  anOutputs.Bind(aGhostOut, aGhostNode);

  BRepGraph_LayerHistory aHist;
  aHist.Absorb(anInputs, anOutputs, aSrc, "RemovedWins");

  EXPECT_TRUE(aHist.IsDeleted(anInNode));
  EXPECT_EQ(aHist.FindModified(anInNode), nullptr);
  ASSERT_EQ(aHist.NbRecords(), size_t(1));
  EXPECT_EQ(aHist.Record(size_t(0)).RecordKind, BRepGraph_LayerHistory::Kind::Deleted);
}

TEST_F(BRepGraph_LayerHistoryTest, Absorb_OutputMissingFromMap_DroppedSilently)
{
  const TopoDS_Shape     anInShape  = MakeVertexShape(0, 0, false);
  const TopoDS_Shape     anOutShape = MakeVertexShape(4, 0, false);
  const BRepGraph_NodeId anInNode(BRepGraph_NodeId::Kind::Vertex, 400);

  occ::handle<BRepTools_History> aSrc = new BRepTools_History();
  aSrc->AddModified(anInShape, anOutShape);

  NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher> anInputs;
  anInputs.Bind(anInShape, anInNode);
  // Intentionally do not bind anOutShape in anOutputs.
  NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher> anOutputs;

  BRepGraph_LayerHistory aHist;
  aHist.Absorb(anInputs, anOutputs, aSrc, "Drop");

  EXPECT_EQ(aHist.NbRecords(), size_t(0));
  EXPECT_EQ(aHist.FindModified(anInNode), nullptr);
}

TEST_F(BRepGraph_LayerHistoryTest, ClearReleasesHistoryAndAllowsFreshUidRecording)
{
  BRepGraph_LayerHistory aHist;

  for (uint32_t anIdx = 0; anIdx < 1000; ++anIdx)
  {
    const BRepGraph_UID                     anOriginal(BRepGraph_NodeId::Kind::Edge, anIdx + 1);
    NCollection_LinearVector<BRepGraph_UID> aReplacements(1);
    aReplacements.Append(BRepGraph_UID(BRepGraph_NodeId::Kind::Edge, anIdx + 1001));
    aHist.RecordUid("Stress", anOriginal, aReplacements.ToArray1());
  }
  ASSERT_EQ(aHist.NbRecords(), size_t(1000));

  aHist.Clear();
  ASSERT_EQ(aHist.NbRecords(), size_t(0));

  const BRepGraph_UID                     aFreshOriginal(BRepGraph_NodeId::Kind::Face, 20);
  const BRepGraph_UID                     aFreshReplacement(BRepGraph_NodeId::Kind::Face, 21);
  NCollection_LinearVector<BRepGraph_UID> aFreshReplacements(1);
  aFreshReplacements.Append(aFreshReplacement);
  aHist.RecordUid("AfterClear", aFreshOriginal, aFreshReplacements.ToArray1());

  ASSERT_EQ(aHist.NbRecords(), size_t(1));
  const NCollection_LinearVector<BRepGraph_UID>* aModified = aHist.FindModified(aFreshOriginal);
  ASSERT_NE(aModified, nullptr);
  ASSERT_EQ(aModified->Size(), 1u);
  EXPECT_EQ(aModified->Value(0), aFreshReplacement);
  EXPECT_FALSE(aHist.IsDeleted(aFreshOriginal));
}
