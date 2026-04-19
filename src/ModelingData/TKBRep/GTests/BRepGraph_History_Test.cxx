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
#include <BRepGraph_History.hxx>
#include <BRepGraph_TopoView.hxx>
#include "BRepGraph_RefTestTools.hxx"
#include <BRepGraph_Builder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <NCollection_Vector.hxx>
#include <Standard_Failure.hxx>
#include <TCollection_AsciiString.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

class BRepGraph_HistoryTest : public testing::Test
{
protected:
  void SetUp() override
  {
    BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
    BRepGraph_Builder::Perform(myGraph, aBoxMaker.Shape());
  }

  BRepGraph myGraph;
};

// ============================================================
// History chain tests
// ============================================================

TEST_F(BRepGraph_HistoryTest, FindOriginal_ChainABC_ReturnsA)
{
  // Build a chain: edge0 -> edge1 -> edge2 via two ApplyModification calls.
  const BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId       anEdge1;
  BRepGraph_NodeId       anEdge2;

  myGraph.Editor().Gen().ApplyModification(
    anEdge0,
    [&](BRepGraph& theGraph,
        BRepGraph_NodeId /*theTarget*/) -> NCollection_Vector<BRepGraph_NodeId> {
      // Simulate producing a new edge node at index NbEdgeDefs.
      anEdge1 = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, theGraph.Topo().Edges().Nb());
      NCollection_Vector<BRepGraph_NodeId> aResult;
      aResult.Append(anEdge1);
      return aResult;
    },
    "Step1");

  myGraph.Editor().Gen().ApplyModification(
    anEdge1,
    [&](BRepGraph& theGraph,
        BRepGraph_NodeId /*theTarget*/) -> NCollection_Vector<BRepGraph_NodeId> {
      anEdge2 = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, theGraph.Topo().Edges().Nb());
      NCollection_Vector<BRepGraph_NodeId> aResult;
      aResult.Append(anEdge2);
      return aResult;
    },
    "Step2");

  const BRepGraph_NodeId anOriginal = myGraph.History().FindOriginal(anEdge2);
  EXPECT_TRUE(anOriginal.IsValid());
  EXPECT_EQ(anOriginal, anEdge0);
}

TEST_F(BRepGraph_HistoryTest, FindDerived_ChainABC_ContainsBAndC)
{
  const BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId       anEdge1;
  BRepGraph_NodeId       anEdge2;

  myGraph.Editor().Gen().ApplyModification(
    anEdge0,
    [&](BRepGraph& theGraph, BRepGraph_NodeId) -> NCollection_Vector<BRepGraph_NodeId> {
      anEdge1 = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, theGraph.Topo().Edges().Nb());
      NCollection_Vector<BRepGraph_NodeId> aResult;
      aResult.Append(anEdge1);
      return aResult;
    },
    "Step1");

  myGraph.Editor().Gen().ApplyModification(
    anEdge1,
    [&](BRepGraph& theGraph, BRepGraph_NodeId) -> NCollection_Vector<BRepGraph_NodeId> {
      anEdge2 = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, theGraph.Topo().Edges().Nb());
      NCollection_Vector<BRepGraph_NodeId> aResult;
      aResult.Append(anEdge2);
      return aResult;
    },
    "Step2");

  const NCollection_Vector<BRepGraph_NodeId> aDerived = myGraph.History().FindDerived(anEdge0);
  bool                                       hasEdge1 = false;
  bool                                       hasEdge2 = false;
  for (const BRepGraph_NodeId& aDerivedId : aDerived)
  {
    if (aDerivedId == anEdge1)
      hasEdge1 = true;
    if (aDerivedId == anEdge2)
      hasEdge2 = true;
  }
  EXPECT_TRUE(hasEdge1);
  EXPECT_TRUE(hasEdge2);
}

TEST_F(BRepGraph_HistoryTest, FindOriginal_UnmodifiedNode_ReturnsSelf)
{
  // FindOriginal returns the node itself when it is not derived from anything.
  const BRepGraph_NodeId aFace(BRepGraph_NodeId::Kind::Face, 0);
  const BRepGraph_NodeId anOriginal = myGraph.History().FindOriginal(aFace);
  EXPECT_TRUE(anOriginal.IsValid());
  EXPECT_EQ(anOriginal, aFace);
}

TEST_F(BRepGraph_HistoryTest, FindDerived_UnmodifiedNode_ReturnsEmpty)
{
  const BRepGraph_NodeId                     aFace(BRepGraph_NodeId::Kind::Face, 0);
  const NCollection_Vector<BRepGraph_NodeId> aDerived = myGraph.History().FindDerived(aFace);
  EXPECT_EQ(aDerived.Length(), 0);
}

TEST_F(BRepGraph_HistoryTest, Disabled_RecordHistory_NoRecordStored)
{
  const int aNbBefore = myGraph.History().NbRecords();
  myGraph.History().SetEnabled(false);

  const BRepGraph_NodeId               anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  NCollection_Vector<BRepGraph_NodeId> aReplacements;
  aReplacements.Append(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, 1));
  myGraph.History().Record("Disabled", anEdge0, aReplacements);

  EXPECT_EQ(myGraph.History().NbRecords(), aNbBefore);
}

TEST_F(BRepGraph_HistoryTest, Disabled_ApplyModification_ModifierStillRuns)
{
  myGraph.History().SetEnabled(false);
  bool aModifierRan = false;

  const BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  myGraph.Editor().Gen().ApplyModification(
    anEdge0,
    [&](BRepGraph&, BRepGraph_NodeId) -> NCollection_Vector<BRepGraph_NodeId> {
      aModifierRan = true;
      return NCollection_Vector<BRepGraph_NodeId>();
    },
    "DisabledOp");

  EXPECT_TRUE(aModifierRan);
}

TEST_F(BRepGraph_HistoryTest, ReEnabled_RecordsAfterReEnable)
{
  myGraph.History().SetEnabled(false);
  EXPECT_FALSE(myGraph.History().IsEnabled());

  myGraph.History().SetEnabled(true);
  EXPECT_TRUE(myGraph.History().IsEnabled());

  const int                            aNbBefore = myGraph.History().NbRecords();
  const BRepGraph_NodeId               anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  NCollection_Vector<BRepGraph_NodeId> aReplacements;
  aReplacements.Append(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, 1));
  myGraph.History().Record("ReEnabled", anEdge0, aReplacements);

  EXPECT_EQ(myGraph.History().NbRecords(), aNbBefore + 1);
}

TEST_F(BRepGraph_HistoryTest, ApplyModification_EmptyReplacements)
{
  const BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  const int              aNbBefore = myGraph.History().NbRecords();

  myGraph.Editor().Gen().ApplyModification(
    anEdge0,
    [](BRepGraph&, BRepGraph_NodeId) -> NCollection_Vector<BRepGraph_NodeId> {
      return NCollection_Vector<BRepGraph_NodeId>();
    },
    "Delete");

  EXPECT_EQ(myGraph.History().NbRecords(), aNbBefore + 1);
  const BRepGraph_HistoryRecord& aRec = myGraph.History().Record(myGraph.History().NbRecords() - 1);
  EXPECT_TRUE(aRec.OperationName.IsEqual("Delete"));
}

TEST_F(BRepGraph_HistoryTest, ApplyModification_MultipleReplacements)
{
  const BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId       aNew1;
  BRepGraph_NodeId       aNew2;

  myGraph.Editor().Gen().ApplyModification(
    anEdge0,
    [&](BRepGraph& theGraph, BRepGraph_NodeId) -> NCollection_Vector<BRepGraph_NodeId> {
      const int aBase = theGraph.Topo().Edges().Nb();
      aNew1           = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, aBase);
      aNew2           = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, aBase + 1);
      NCollection_Vector<BRepGraph_NodeId> aResult;
      aResult.Append(aNew1);
      aResult.Append(aNew2);
      return aResult;
    },
    "Split");

  const NCollection_Vector<BRepGraph_NodeId> aDerived = myGraph.History().FindDerived(anEdge0);
  bool                                       hasNew1  = false;
  bool                                       hasNew2  = false;
  for (const BRepGraph_NodeId& aDerivedId : aDerived)
  {
    if (aDerivedId == aNew1)
      hasNew1 = true;
    if (aDerivedId == aNew2)
      hasNew2 = true;
  }
  EXPECT_TRUE(hasNew1);
  EXPECT_TRUE(hasNew2);
}

TEST_F(BRepGraph_HistoryTest, RecordHistory_EmptyReplacements_Stored)
{
  const int                            aNbBefore = myGraph.History().NbRecords();
  const BRepGraph_NodeId               anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  NCollection_Vector<BRepGraph_NodeId> anEmpty;
  myGraph.History().Record("Erase", anEdge0, anEmpty);

  EXPECT_EQ(myGraph.History().NbRecords(), aNbBefore + 1);
}

TEST_F(BRepGraph_HistoryTest, HistoryRecord_SequenceNumber_Monotonic)
{
  const BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  const BRepGraph_NodeId anEdge1(BRepGraph_NodeId::Kind::Edge, 1);
  const BRepGraph_NodeId anEdge2(BRepGraph_NodeId::Kind::Edge, 2);

  NCollection_Vector<BRepGraph_NodeId> aRepl;
  aRepl.Append(anEdge1);
  myGraph.History().Record("Op1", anEdge0, aRepl);

  NCollection_Vector<BRepGraph_NodeId> aRepl2;
  aRepl2.Append(anEdge2);
  myGraph.History().Record("Op2", anEdge1, aRepl2);

  const int aNb = myGraph.History().NbRecords();
  ASSERT_GE(aNb, 2);
  const BRepGraph_HistoryRecord& aRec1 = myGraph.History().Record(aNb - 2);
  const BRepGraph_HistoryRecord& aRec2 = myGraph.History().Record(aNb - 1);
  EXPECT_LT(aRec1.SequenceNumber, aRec2.SequenceNumber);
}

TEST_F(BRepGraph_HistoryTest, HistoryRecord_OperationName_Stored)
{
  const BRepGraph_NodeId               anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  NCollection_Vector<BRepGraph_NodeId> aRepl;
  aRepl.Append(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, 1));
  myGraph.History().Record("MyCustomOp", anEdge0, aRepl);

  const BRepGraph_HistoryRecord& aRec = myGraph.History().Record(myGraph.History().NbRecords() - 1);
  EXPECT_TRUE(aRec.OperationName.IsEqual("MyCustomOp"));
}

TEST_F(BRepGraph_HistoryTest, NbHistoryRecords_AfterMultipleOps_Correct)
{
  const int aNbBefore = myGraph.History().NbRecords();

  const BRepGraph_NodeId               anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  NCollection_Vector<BRepGraph_NodeId> aRepl;
  aRepl.Append(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, 1));

  myGraph.History().Record("A", anEdge0, aRepl);
  myGraph.History().Record("B", anEdge0, aRepl);
  myGraph.History().Record("C", anEdge0, aRepl);

  EXPECT_EQ(myGraph.History().NbRecords(), aNbBefore + 3);
}

TEST_F(BRepGraph_HistoryTest, FindOriginal_TwoApply_TransitiveTrace)
{
  const BRepGraph_NodeId aVtx0(BRepGraph_NodeId::Kind::Vertex, 0);
  BRepGraph_NodeId       aVtx1;
  BRepGraph_NodeId       aVtx2;

  myGraph.Editor().Gen().ApplyModification(
    aVtx0,
    [&](BRepGraph& theGraph, BRepGraph_NodeId) -> NCollection_Vector<BRepGraph_NodeId> {
      aVtx1 = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Vertex, theGraph.Topo().Vertices().Nb());
      NCollection_Vector<BRepGraph_NodeId> aResult;
      aResult.Append(aVtx1);
      return aResult;
    },
    "Move1");

  myGraph.Editor().Gen().ApplyModification(
    aVtx1,
    [&](BRepGraph& theGraph, BRepGraph_NodeId) -> NCollection_Vector<BRepGraph_NodeId> {
      aVtx2 = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Vertex, theGraph.Topo().Vertices().Nb());
      NCollection_Vector<BRepGraph_NodeId> aResult;
      aResult.Append(aVtx2);
      return aResult;
    },
    "Move2");

  // FindOriginal from the end of a 2-step chain should reach the root.
  const BRepGraph_NodeId anOriginal = myGraph.History().FindOriginal(aVtx2);
  EXPECT_TRUE(anOriginal.IsValid());
  EXPECT_EQ(anOriginal, aVtx0);

  // Intermediate node should also trace back to root.
  const BRepGraph_NodeId anOriginal1 = myGraph.History().FindOriginal(aVtx1);
  EXPECT_TRUE(anOriginal1.IsValid());
  EXPECT_EQ(anOriginal1, aVtx0);
}

TEST_F(BRepGraph_HistoryTest, ApplyModification_WhenModifierThrows_DoesNotRecordHistory)
{
  const int aNbRecordsBefore = myGraph.History().NbRecords();

  const BRepGraph_NodeId anEdge(BRepGraph_NodeId::Kind::Edge, 0);

#if !defined(No_Exception)
  EXPECT_THROW(myGraph.Editor().Gen().ApplyModification(
                 anEdge,
                 [](BRepGraph&, BRepGraph_NodeId) -> NCollection_Vector<BRepGraph_NodeId> {
                   throw Standard_Failure("Synthetic failure");
                 },
                 "ThrowingModification"),
               Standard_Failure);
#endif

  EXPECT_EQ(myGraph.History().NbRecords(), aNbRecordsBefore);
}

TEST_F(BRepGraph_HistoryTest, SplitEdge_RewritesAllContainingWires)
{
  ASSERT_GT(myGraph.Topo().Edges().Nb(), 0);

  const BRepGraph_EdgeId       anEdgeId(0);
  const BRepGraphInc::EdgeDef& anEdgeDef = myGraph.Topo().Edges().Definition(anEdgeId);

  const double aSplitParam = 0.5 * (anEdgeDef.ParamFirst + anEdgeDef.ParamLast);

  const int                aNbVerticesBefore = myGraph.Topo().Vertices().Nb();
  const gp_Pnt             aSplitPoint(1.0, 2.0, 3.0);
  const BRepGraph_VertexId aSplitVertex = myGraph.Editor().Vertices().Add(aSplitPoint, 1.0e-7);

  ASSERT_TRUE(aSplitVertex.IsValid());
  EXPECT_EQ(myGraph.Topo().Vertices().Nb(), aNbVerticesBefore + 1);

  const NCollection_Vector<BRepGraph_WireId>& aWireIndices = myGraph.Topo().Edges().Wires(anEdgeId);
  ASSERT_GT(aWireIndices.Length(), 0);

  const int aNbEdgesBefore       = myGraph.Topo().Edges().Nb();
  const int aNbActiveEdgesBefore = myGraph.Topo().Edges().NbActive();

  BRepGraph_EdgeId aSubA;
  BRepGraph_EdgeId aSubB;

  myGraph.Editor().Edges().Split(anEdgeId, aSplitVertex, aSplitParam, aSubA, aSubB);

  ASSERT_TRUE(aSubA.IsValid());
  ASSERT_TRUE(aSubB.IsValid());
  EXPECT_EQ(myGraph.Topo().Edges().Nb(), aNbEdgesBefore + 2);
  EXPECT_EQ(myGraph.Topo().Edges().NbActive(), aNbActiveEdgesBefore + 1);

  for (const BRepGraph_WireId& aWireId : aWireIndices)
  {
    const NCollection_Vector<BRepGraph_CoEdgeRefId> aCoEdgeRefs =
      BRepGraph_TestTools::CoEdgeRefsOfWire(myGraph, aWireId);

    bool hasOld   = false;
    bool hasSubA  = false;
    bool hasSubB  = false;
    int  aSubAOrd = -1;
    int  aSubBOrd = -1;

    for (int anIdx = 0; anIdx < aCoEdgeRefs.Length(); ++anIdx)
    {
      const BRepGraphInc::CoEdgeRef& aCR = myGraph.Refs().CoEdges().Entry(aCoEdgeRefs.Value(anIdx));
      const BRepGraphInc::CoEdgeDef& aCoEdge = myGraph.Topo().CoEdges().Definition(aCR.CoEdgeDefId);
      const BRepGraph_NodeId         anId(aCoEdge.EdgeDefId);
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
    EXPECT_GE(aSubAOrd, 0);
    EXPECT_GE(aSubBOrd, 0);
    EXPECT_EQ(aSubBOrd, aSubAOrd + 1);
  }
}

TEST_F(BRepGraph_HistoryTest, SplitEdge_IgnoresRemovedCoEdgeRefEntries)
{
  ASSERT_GT(myGraph.Topo().Edges().Nb(), 0);

  const BRepGraph_EdgeId       anEdgeId(0);
  const BRepGraphInc::EdgeDef& anEdgeDef = myGraph.Topo().Edges().Definition(anEdgeId);
  const double aSplitParam = 0.5 * (anEdgeDef.ParamFirst + anEdgeDef.ParamLast);

  const NCollection_Vector<BRepGraph_WireId>& aWireIndices = myGraph.Topo().Edges().Wires(anEdgeId);
  ASSERT_GT(aWireIndices.Length(), 1);

  const BRepGraph_WireId                          aWireId = aWireIndices.Value(0);
  const NCollection_Vector<BRepGraph_CoEdgeRefId> aWireRefsBefore =
    BRepGraph_TestTools::CoEdgeRefsOfWire(myGraph, aWireId);
  ASSERT_GT(aWireRefsBefore.Length(), 0);

  BRepGraph_CoEdgeRefId aRefToRemove;
  int                   aRemovedOrd = -1;
  for (int aRefOrd = 0; aRefOrd < aWireRefsBefore.Length(); ++aRefOrd)
  {
    const BRepGraph_CoEdgeRefId    aRefId  = aWireRefsBefore.Value(aRefOrd);
    const BRepGraphInc::CoEdgeRef& aRef    = myGraph.Refs().CoEdges().Entry(aRefId);
    const BRepGraphInc::CoEdgeDef& aCoEdge = myGraph.Topo().CoEdges().Definition(aRef.CoEdgeDefId);
    if (aCoEdge.EdgeDefId == anEdgeId)
    {
      aRefToRemove = aRefId;
      aRemovedOrd  = aRefOrd;
      break;
    }
  }
  ASSERT_TRUE(aRefToRemove.IsValid(myGraph.Refs().CoEdges().Nb()));
  ASSERT_GE(aRemovedOrd, 0);

  {
    BRepGraph_MutGuard<BRepGraphInc::CoEdgeRef> aMut = myGraph.Editor().CoEdges().MutRef(aRefToRemove);
    aMut->IsRemoved                                  = true;
  }
  ASSERT_TRUE(myGraph.Refs().CoEdges().Entry(aRefToRemove).IsRemoved);
  const BRepGraph_CoEdgeId aRemovedCoEdgeId =
    myGraph.Refs().CoEdges().Entry(aRefToRemove).CoEdgeDefId;

  const int aRemovedWireNbActiveBefore =
    BRepGraph_TestTools::CountCoEdgeRefsOfWire(myGraph, aWireId);

  const BRepGraph_VertexId aSplitVertex =
    myGraph.Editor().Vertices().Add(gp_Pnt(4.0, 5.0, 6.0), 1.0e-7);
  ASSERT_TRUE(aSplitVertex.IsValid());

  BRepGraph_EdgeId aSubA;
  BRepGraph_EdgeId aSubB;
  myGraph.Editor().Edges().Split(anEdgeId, aSplitVertex, aSplitParam, aSubA, aSubB);
  ASSERT_TRUE(aSubA.IsValid());
  ASSERT_TRUE(aSubB.IsValid());

  EXPECT_TRUE(myGraph.Refs().CoEdges().Entry(aRefToRemove).IsRemoved);
  EXPECT_EQ(BRepGraph_TestTools::CountCoEdgeRefsOfWire(myGraph, aWireId),
            aRemovedWireNbActiveBefore);
  const BRepGraphInc::CoEdgeDef& aRemovedCoEdgeAfter =
    myGraph.Topo().CoEdges().Definition(aRemovedCoEdgeId);
  EXPECT_EQ(aRemovedCoEdgeAfter.EdgeDefId, anEdgeId);

  bool      hasSubA       = false;
  bool      hasSubB       = false;
  const int aNbCoEdgeRefs = myGraph.Refs().CoEdges().Nb();
  for (BRepGraph_CoEdgeRefId aRefId(0); aRefId.IsValid(aNbCoEdgeRefs); ++aRefId)
  {
    const BRepGraphInc::CoEdgeRef& aRef = myGraph.Refs().CoEdges().Entry(aRefId);
    if (aRef.IsRemoved || !aRef.CoEdgeDefId.IsValid(myGraph.Topo().CoEdges().Nb()))
      continue;
    const BRepGraph_NodeId anId(myGraph.Topo().CoEdges().Definition(aRef.CoEdgeDefId).EdgeDefId);
    if (anId == aSubA)
      hasSubA = true;
    if (anId == aSubB)
      hasSubB = true;
  }
  EXPECT_TRUE(hasSubA);
  EXPECT_TRUE(hasSubB);
}

TEST_F(BRepGraph_HistoryTest, ApplyModification_SplitEdge_RecordsBothDerivedNodes)
{
  ASSERT_GT(myGraph.Topo().Edges().Nb(), 0);

  const BRepGraph_EdgeId       anEdgeId(0);
  const BRepGraphInc::EdgeDef& anEdgeDef = myGraph.Topo().Edges().Definition(anEdgeId);
  const double aSplitParam = 0.5 * (anEdgeDef.ParamFirst + anEdgeDef.ParamLast);

  const BRepGraph_VertexId aSplitVertex =
    myGraph.Editor().Vertices().Add(gp_Pnt(4.0, 5.0, 6.0), 1.0e-7);
  ASSERT_TRUE(aSplitVertex.IsValid());

  const int aNbRecordsBefore = myGraph.History().NbRecords();

  myGraph.Editor().Gen().ApplyModification(
    anEdgeId,
    [&](BRepGraph& theGraph, BRepGraph_NodeId theTarget) -> NCollection_Vector<BRepGraph_NodeId> {
      BRepGraph_EdgeId aSubA;
      BRepGraph_EdgeId aSubB;
      theGraph.Editor().Edges().Split(BRepGraph_EdgeId::FromNodeId(theTarget),
                                   aSplitVertex,
                                   aSplitParam,
                                   aSubA,
                                   aSubB);

      NCollection_Vector<BRepGraph_NodeId> aResult;
      aResult.Append(aSubA);
      aResult.Append(aSubB);
      return aResult;
    },
    "Split");

  EXPECT_EQ(myGraph.History().NbRecords(), aNbRecordsBefore + 1);

  const NCollection_Vector<BRepGraph_NodeId> aDerived = myGraph.History().FindDerived(anEdgeId);
  EXPECT_GE(aDerived.Length(), 2);
}
