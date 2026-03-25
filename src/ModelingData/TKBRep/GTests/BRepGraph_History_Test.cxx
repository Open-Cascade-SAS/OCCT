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
#include <BRepGraph_AttrRegistry.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_Mutator.hxx>
#include <BRepGraphInc_IncidenceRef.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <NCollection_Vector.hxx>
#include <Standard_Failure.hxx>
#include <Standard_GUID.hxx>
#include <TCollection_AsciiString.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

class BRepGraphHistoryTest : public testing::Test
{
protected:
  void SetUp() override
  {
    BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
    myGraph.Build(aBoxMaker.Shape());
  }

  BRepGraph myGraph;
};

// ============================================================
// History chain tests
// ============================================================

TEST_F(BRepGraphHistoryTest, FindOriginal_ChainABC_ReturnsA)
{
  // Build a chain: edge0 -> edge1 -> edge2 via two ApplyModification calls.
  const BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId       anEdge1;
  BRepGraph_NodeId       anEdge2;

  myGraph.ApplyModification(
    anEdge0,
    [&](BRepGraph& theGraph,
        BRepGraph_NodeId /*theTarget*/) -> NCollection_Vector<BRepGraph_NodeId> {
      // Simulate producing a new edge node at index NbEdgeDefs.
      anEdge1 = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, theGraph.Defs().NbEdges());
      NCollection_Vector<BRepGraph_NodeId> aResult;
      aResult.Append(anEdge1);
      return aResult;
    },
    "Step1");

  myGraph.ApplyModification(
    anEdge1,
    [&](BRepGraph& theGraph,
        BRepGraph_NodeId /*theTarget*/) -> NCollection_Vector<BRepGraph_NodeId> {
      anEdge2 = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, theGraph.Defs().NbEdges());
      NCollection_Vector<BRepGraph_NodeId> aResult;
      aResult.Append(anEdge2);
      return aResult;
    },
    "Step2");

  const BRepGraph_NodeId anOriginal = myGraph.History().FindOriginal(anEdge2);
  EXPECT_TRUE(anOriginal.IsValid());
  EXPECT_EQ(anOriginal, anEdge0);
}

TEST_F(BRepGraphHistoryTest, FindDerived_ChainABC_ContainsBAndC)
{
  const BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId       anEdge1;
  BRepGraph_NodeId       anEdge2;

  myGraph.ApplyModification(
    anEdge0,
    [&](BRepGraph& theGraph, BRepGraph_NodeId) -> NCollection_Vector<BRepGraph_NodeId> {
      anEdge1 = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, theGraph.Defs().NbEdges());
      NCollection_Vector<BRepGraph_NodeId> aResult;
      aResult.Append(anEdge1);
      return aResult;
    },
    "Step1");

  myGraph.ApplyModification(
    anEdge1,
    [&](BRepGraph& theGraph, BRepGraph_NodeId) -> NCollection_Vector<BRepGraph_NodeId> {
      anEdge2 = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, theGraph.Defs().NbEdges());
      NCollection_Vector<BRepGraph_NodeId> aResult;
      aResult.Append(anEdge2);
      return aResult;
    },
    "Step2");

  const NCollection_Vector<BRepGraph_NodeId> aDerived = myGraph.History().FindDerived(anEdge0);
  bool                                       hasEdge1 = false;
  bool                                       hasEdge2 = false;
  for (int i = 0; i < aDerived.Length(); ++i)
  {
    if (aDerived.Value(i) == anEdge1)
      hasEdge1 = true;
    if (aDerived.Value(i) == anEdge2)
      hasEdge2 = true;
  }
  EXPECT_TRUE(hasEdge1);
  EXPECT_TRUE(hasEdge2);
}

TEST_F(BRepGraphHistoryTest, FindOriginal_UnmodifiedNode_ReturnsSelf)
{
  // FindOriginal returns the node itself when it is not derived from anything.
  const BRepGraph_NodeId aFace(BRepGraph_NodeId::Kind::Face, 0);
  const BRepGraph_NodeId anOriginal = myGraph.History().FindOriginal(aFace);
  EXPECT_TRUE(anOriginal.IsValid());
  EXPECT_EQ(anOriginal, aFace);
}

TEST_F(BRepGraphHistoryTest, FindDerived_UnmodifiedNode_ReturnsEmpty)
{
  const BRepGraph_NodeId                     aFace(BRepGraph_NodeId::Kind::Face, 0);
  const NCollection_Vector<BRepGraph_NodeId> aDerived = myGraph.History().FindDerived(aFace);
  EXPECT_EQ(aDerived.Length(), 0);
}

TEST_F(BRepGraphHistoryTest, Disabled_RecordHistory_NoRecordStored)
{
  const int aNbBefore = myGraph.History().NbRecords();
  myGraph.SetHistoryEnabled(false);

  const BRepGraph_NodeId               anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  NCollection_Vector<BRepGraph_NodeId> aReplacements;
  aReplacements.Append(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, 1));
  myGraph.History().Record("Disabled", anEdge0, aReplacements);

  EXPECT_EQ(myGraph.History().NbRecords(), aNbBefore);
}

TEST_F(BRepGraphHistoryTest, Disabled_ApplyModification_ModifierStillRuns)
{
  myGraph.SetHistoryEnabled(false);
  bool aModifierRan = false;

  const BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  myGraph.ApplyModification(
    anEdge0,
    [&](BRepGraph&, BRepGraph_NodeId) -> NCollection_Vector<BRepGraph_NodeId> {
      aModifierRan = true;
      return NCollection_Vector<BRepGraph_NodeId>();
    },
    "DisabledOp");

  EXPECT_TRUE(aModifierRan);
}

TEST_F(BRepGraphHistoryTest, ReEnabled_RecordsAfterReEnable)
{
  myGraph.SetHistoryEnabled(false);
  EXPECT_FALSE(myGraph.IsHistoryEnabled());

  myGraph.SetHistoryEnabled(true);
  EXPECT_TRUE(myGraph.IsHistoryEnabled());

  const int                            aNbBefore = myGraph.History().NbRecords();
  const BRepGraph_NodeId               anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  NCollection_Vector<BRepGraph_NodeId> aReplacements;
  aReplacements.Append(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, 1));
  myGraph.History().Record("ReEnabled", anEdge0, aReplacements);

  EXPECT_EQ(myGraph.History().NbRecords(), aNbBefore + 1);
}

TEST_F(BRepGraphHistoryTest, ApplyModification_EmptyReplacements)
{
  const BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  const int              aNbBefore = myGraph.History().NbRecords();

  myGraph.ApplyModification(
    anEdge0,
    [](BRepGraph&, BRepGraph_NodeId) -> NCollection_Vector<BRepGraph_NodeId> {
      return NCollection_Vector<BRepGraph_NodeId>();
    },
    "Delete");

  EXPECT_EQ(myGraph.History().NbRecords(), aNbBefore + 1);
  const BRepGraph_HistoryRecord& aRec = myGraph.History().Record(myGraph.History().NbRecords() - 1);
  EXPECT_TRUE(aRec.OperationName.IsEqual("Delete"));
}

TEST_F(BRepGraphHistoryTest, ApplyModification_MultipleReplacements)
{
  const BRepGraph_NodeId anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  BRepGraph_NodeId       aNew1;
  BRepGraph_NodeId       aNew2;

  myGraph.ApplyModification(
    anEdge0,
    [&](BRepGraph& theGraph, BRepGraph_NodeId) -> NCollection_Vector<BRepGraph_NodeId> {
      const int aBase = theGraph.Defs().NbEdges();
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
  for (int i = 0; i < aDerived.Length(); ++i)
  {
    if (aDerived.Value(i) == aNew1)
      hasNew1 = true;
    if (aDerived.Value(i) == aNew2)
      hasNew2 = true;
  }
  EXPECT_TRUE(hasNew1);
  EXPECT_TRUE(hasNew2);
}

TEST_F(BRepGraphHistoryTest, RecordHistory_EmptyReplacements_Stored)
{
  const int                            aNbBefore = myGraph.History().NbRecords();
  const BRepGraph_NodeId               anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  NCollection_Vector<BRepGraph_NodeId> anEmpty;
  myGraph.History().Record("Erase", anEdge0, anEmpty);

  EXPECT_EQ(myGraph.History().NbRecords(), aNbBefore + 1);
}

TEST_F(BRepGraphHistoryTest, HistoryRecord_SequenceNumber_Monotonic)
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

TEST_F(BRepGraphHistoryTest, HistoryRecord_OperationName_Stored)
{
  const BRepGraph_NodeId               anEdge0(BRepGraph_NodeId::Kind::Edge, 0);
  NCollection_Vector<BRepGraph_NodeId> aRepl;
  aRepl.Append(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, 1));
  myGraph.History().Record("MyCustomOp", anEdge0, aRepl);

  const BRepGraph_HistoryRecord& aRec = myGraph.History().Record(myGraph.History().NbRecords() - 1);
  EXPECT_TRUE(aRec.OperationName.IsEqual("MyCustomOp"));
}

TEST_F(BRepGraphHistoryTest, NbHistoryRecords_AfterMultipleOps_Correct)
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

TEST_F(BRepGraphHistoryTest, FindOriginal_TwoApply_TransitiveTrace)
{
  const BRepGraph_NodeId aVtx0(BRepGraph_NodeId::Kind::Vertex, 0);
  BRepGraph_NodeId       aVtx1;
  BRepGraph_NodeId       aVtx2;

  myGraph.ApplyModification(
    aVtx0,
    [&](BRepGraph& theGraph, BRepGraph_NodeId) -> NCollection_Vector<BRepGraph_NodeId> {
      aVtx1 = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Vertex, theGraph.Defs().NbVertices());
      NCollection_Vector<BRepGraph_NodeId> aResult;
      aResult.Append(aVtx1);
      return aResult;
    },
    "Move1");

  myGraph.ApplyModification(
    aVtx1,
    [&](BRepGraph& theGraph, BRepGraph_NodeId) -> NCollection_Vector<BRepGraph_NodeId> {
      aVtx2 = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Vertex, theGraph.Defs().NbVertices());
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

// ============================================================
// AttrRegistry tests (standalone, no fixture needed)
// ============================================================

TEST(BRepGraphAttrRegistryTest, Register_SameGUID_SameKey)
{
  const Standard_GUID aGUID("a1b2c3d4-1111-2222-3333-444455556666");
  const int           aKey1 = BRepGraph_AttrRegistry::Register(aGUID);
  const int           aKey2 = BRepGraph_AttrRegistry::Register(aGUID);
  EXPECT_EQ(aKey1, aKey2);
}

TEST(BRepGraphAttrRegistryTest, Register_DifferentGUID_DifferentKey)
{
  const Standard_GUID aGUID1("b1b2c3d4-aaaa-bbbb-cccc-ddddeeee0001");
  const Standard_GUID aGUID2("b1b2c3d4-aaaa-bbbb-cccc-ddddeeee0002");
  const int           aKey1 = BRepGraph_AttrRegistry::Register(aGUID1);
  const int           aKey2 = BRepGraph_AttrRegistry::Register(aGUID2);
  EXPECT_NE(aKey1, aKey2);
}

TEST(BRepGraphAttrRegistryTest, Find_ByGUID_ReturnsCorrectKey)
{
  const Standard_GUID aGUID("c1c2c3c4-1111-2222-3333-aabbccddeeff");
  const int           aExpectedKey = BRepGraph_AttrRegistry::Register(aGUID);

  int        aFoundKey = -1;
  const bool aOk       = BRepGraph_AttrRegistry::Find(aGUID, aFoundKey);
  EXPECT_TRUE(aOk);
  EXPECT_EQ(aFoundKey, aExpectedKey);
}

TEST(BRepGraphAttrRegistryTest, Find_ByKey_ReturnsCorrectGUID)
{
  const Standard_GUID aGUID("d1d2d3d4-5555-6666-7777-888899990000");
  const int           aKey = BRepGraph_AttrRegistry::Register(aGUID);

  Standard_GUID aFoundGUID;
  const bool    aOk = BRepGraph_AttrRegistry::Find(aKey, aFoundGUID);
  EXPECT_TRUE(aOk);
  EXPECT_TRUE(aFoundGUID == aGUID);
}

TEST_F(BRepGraphHistoryTest, ApplyModification_WhenModifierThrows_DoesNotRecordHistory)
{
  const int aNbRecordsBefore = myGraph.History().NbRecords();

  const BRepGraph_NodeId anEdge(BRepGraph_NodeId::Kind::Edge, 0);

  EXPECT_THROW(myGraph.ApplyModification(
                 anEdge,
                 [](BRepGraph&, BRepGraph_NodeId) -> NCollection_Vector<BRepGraph_NodeId> {
                   throw Standard_Failure("Synthetic failure");
                 },
                 "ThrowingModification"),
               Standard_Failure);

  EXPECT_EQ(myGraph.History().NbRecords(), aNbRecordsBefore);
}

TEST_F(BRepGraphHistoryTest, SplitEdge_RewritesAllContainingWires)
{
  ASSERT_GT(myGraph.Defs().NbEdges(), 0);

  const BRepGraph_NodeId             anEdgeId(BRepGraph_NodeId::Kind::Edge, 0);
  const BRepGraph_TopoNode::EdgeDef& anEdgeDef =
    myGraph.Defs().Edge(BRepGraph_EdgeId(anEdgeId.Index));

  const double aSplitParam = 0.5 * (anEdgeDef.ParamFirst + anEdgeDef.ParamLast);

  const int              aNbVerticesBefore = myGraph.Defs().NbVertices();
  const gp_Pnt           aSplitPoint(1.0, 2.0, 3.0);
  const BRepGraph_NodeId aSplitVertex = myGraph.Builder().AddVertexDef(aSplitPoint, 1.0e-7);

  ASSERT_TRUE(aSplitVertex.IsValid());
  EXPECT_EQ(myGraph.Defs().NbVertices(), aNbVerticesBefore + 1);

  const NCollection_Vector<BRepGraph_WireId>& aWireIndices =
    myGraph.Defs().WiresOfEdge(BRepGraph_EdgeId(anEdgeId.Index));
  ASSERT_GT(aWireIndices.Length(), 0);

  const int aNbEdgesBefore = myGraph.Defs().NbEdges();
  const int aNbActiveEdgesBefore = myGraph.Defs().NbActiveEdges();

  BRepGraph_NodeId aSubA;
  BRepGraph_NodeId aSubB;

  BRepGraph_Mutator::SplitEdge(myGraph, anEdgeId, aSplitVertex, aSplitParam, aSubA, aSubB);

  ASSERT_TRUE(aSubA.IsValid());
  ASSERT_TRUE(aSubB.IsValid());
  EXPECT_EQ(myGraph.Defs().NbEdges(), aNbEdgesBefore + 2);
  EXPECT_EQ(myGraph.Defs().NbActiveEdges(), aNbActiveEdgesBefore + 1);

  for (int aWireIter = 0; aWireIter < aWireIndices.Length(); ++aWireIter)
  {
    const BRepGraph_TopoNode::WireDef& aWireDef =
      myGraph.Defs().Wire(BRepGraph_WireId(aWireIndices.Value(aWireIter)));

    bool hasOld  = false;
    bool hasSubA = false;
    bool hasSubB = false;

    for (int anIdx = 0; anIdx < aWireDef.CoEdgeRefs.Length(); ++anIdx)
    {
      const BRepGraphInc::CoEdgeRef&       aCR = aWireDef.CoEdgeRefs.Value(anIdx);
      const BRepGraph_TopoNode::CoEdgeDef& aCoEdge =
        myGraph.Defs().CoEdge(BRepGraph_CoEdgeId(aCR.CoEdgeDefId));
      const BRepGraph_NodeId anId(aCoEdge.EdgeDefId);
      if (anId == anEdgeId)
      {
        hasOld = true;
      }
      else if (anId == aSubA)
      {
        hasSubA = true;
      }
      else if (anId == aSubB)
      {
        hasSubB = true;
      }
    }

    EXPECT_FALSE(hasOld);
    EXPECT_TRUE(hasSubA);
    EXPECT_TRUE(hasSubB);
  }
}

TEST_F(BRepGraphHistoryTest, ApplyModification_SplitEdge_RecordsBothDerivedNodes)
{
  ASSERT_GT(myGraph.Defs().NbEdges(), 0);

  const BRepGraph_NodeId             anEdgeId(BRepGraph_NodeId::Kind::Edge, 0);
  const BRepGraph_TopoNode::EdgeDef& anEdgeDef =
    myGraph.Defs().Edge(BRepGraph_EdgeId(anEdgeId.Index));
  const double aSplitParam = 0.5 * (anEdgeDef.ParamFirst + anEdgeDef.ParamLast);

  const BRepGraph_NodeId aSplitVertex =
    myGraph.Builder().AddVertexDef(gp_Pnt(4.0, 5.0, 6.0), 1.0e-7);
  ASSERT_TRUE(aSplitVertex.IsValid());

  const int aNbRecordsBefore = myGraph.History().NbRecords();

  myGraph.ApplyModification(
    anEdgeId,
    [&](BRepGraph& theGraph, BRepGraph_NodeId theTarget) -> NCollection_Vector<BRepGraph_NodeId> {
      BRepGraph_NodeId aSubA;
      BRepGraph_NodeId aSubB;
      BRepGraph_Mutator::SplitEdge(theGraph, theTarget, aSplitVertex, aSplitParam, aSubA, aSubB);

      NCollection_Vector<BRepGraph_NodeId> aResult;
      aResult.Append(aSubA);
      aResult.Append(aSubB);
      return aResult;
    },
    "SplitEdge");

  EXPECT_EQ(myGraph.History().NbRecords(), aNbRecordsBefore + 1);

  const NCollection_Vector<BRepGraph_NodeId> aDerived = myGraph.History().FindDerived(anEdgeId);
  EXPECT_GE(aDerived.Length(), 2);
}
