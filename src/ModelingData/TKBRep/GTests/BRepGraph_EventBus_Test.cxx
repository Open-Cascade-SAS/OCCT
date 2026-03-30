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
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_Layer.hxx>
#include <BRepGraph_DeferredScope.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraphAlgo_Compact.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Precision.hxx>
#include <Standard_GUID.hxx>

#include <gtest/gtest.h>

// Test layer that records modification events.
class BRepGraph_ModTrackingLayer : public BRepGraph_Layer
{
public:
  BRepGraph_ModTrackingLayer(const TCollection_AsciiString& theName,
                             const int                     theSubscribedKinds,
                             const Standard_GUID&          theId = Standard_GUID(
                               "2f9b6a5c-1f2d-4a88-9c1c-7a0c16a10004"))
      : myName(theName),
        mySubscribedKinds(theSubscribedKinds),
        myId(theId)
  {
  }

  const Standard_GUID& ID() const override { return myId; }

  const TCollection_AsciiString& Name() const override { return myName; }

  int SubscribedKinds() const override { return mySubscribedKinds; }

  void OnNodeModified(const BRepGraph_NodeId theNode) noexcept override
  {
    myImmediateEvents.Append(theNode);
  }

  void OnNodesModified(const NCollection_Vector<BRepGraph_NodeId>& theNodes) noexcept override
  {
    myBatchEvents = theNodes;
    ++myBatchCallCount;
  }

  void OnNodeRemoved(const BRepGraph_NodeId theNode,
                     const BRepGraph_NodeId theReplacement) noexcept override
  {
    myLastRemovedNode = theNode;
    myLastReplacement = theReplacement;
    ++myRemoveCallCount;
  }

  void OnCompact(
    const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theRemapMap) noexcept override
  {
    myLastRemapMap.Clear();
    for (NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>::Iterator anIter(theRemapMap);
         anIter.More();
         anIter.Next())
    {
      myLastRemapMap.Bind(anIter.Key(), anIter.Value());
    }
    ++myCompactCallCount;
  }

  void InvalidateAll() noexcept override {}

  void Clear() noexcept override
  {
    myImmediateEvents.Clear();
    myBatchEvents.Clear();
    myBatchCallCount = 0;
    myRemoveCallCount = 0;
    myCompactCallCount = 0;
    myLastRemovedNode = BRepGraph_NodeId();
    myLastReplacement = BRepGraph_NodeId();
    myLastRemapMap.Clear();
  }

  bool HasImmediateEventFor(BRepGraph_NodeId theNode) const
  {
    for (int i = 0; i < myImmediateEvents.Length(); ++i)
      if (myImmediateEvents.Value(i) == theNode)
        return true;
    return false;
  }

  bool HasBatchEventFor(BRepGraph_NodeId theNode) const
  {
    for (int i = 0; i < myBatchEvents.Length(); ++i)
      if (myBatchEvents.Value(i) == theNode)
        return true;
    return false;
  }

  int CountImmediateEventsOfKind(BRepGraph_NodeId::Kind theKind) const
  {
    int aCount = 0;
    for (int i = 0; i < myImmediateEvents.Length(); ++i)
      if (myImmediateEvents.Value(i).NodeKind == theKind)
        ++aCount;
    return aCount;
  }

  NCollection_Vector<BRepGraph_NodeId> myImmediateEvents;
  NCollection_Vector<BRepGraph_NodeId> myBatchEvents;
  int                                  myBatchCallCount = 0;
  int                                  myRemoveCallCount = 0;
  int                                  myCompactCallCount = 0;
  BRepGraph_NodeId                     myLastRemovedNode;
  BRepGraph_NodeId                     myLastReplacement;
  NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId> myLastRemapMap;

  DEFINE_STANDARD_RTTIEXT(BRepGraph_ModTrackingLayer, BRepGraph_Layer)

private:
  TCollection_AsciiString myName;
  int                     mySubscribedKinds;
  Standard_GUID           myId;
};

IMPLEMENT_STANDARD_RTTIEXT(BRepGraph_ModTrackingLayer, BRepGraph_Layer)

// Minimal layer with default SubscribedKinds() behavior from base class.
class BRepGraph_DefaultLayer : public BRepGraph_Layer
{
public:
  const Standard_GUID& ID() const override
  {
    static const Standard_GUID THE_ID("2f9b6a5c-1f2d-4a88-9c1c-7a0c16a10007");
    return THE_ID;
  }

  const TCollection_AsciiString& Name() const override
  {
    static const TCollection_AsciiString THE_NAME("DefaultLayer");
    return THE_NAME;
  }

  void OnNodeRemoved(const BRepGraph_NodeId,
                     const BRepGraph_NodeId) noexcept override
  {
  }

  void OnCompact(
    const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>&) noexcept override
  {
  }

  void InvalidateAll() noexcept override
  {
  }

  void Clear() noexcept override
  {
  }

  DEFINE_STANDARD_RTTIEXT(BRepGraph_DefaultLayer, BRepGraph_Layer)
};

IMPLEMENT_STANDARD_RTTIEXT(BRepGraph_DefaultLayer, BRepGraph_Layer)

class BRepGraph_EventBusTest : public testing::Test
{
protected:
  void SetUp() override
  {
    BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
    const TopoDS_Shape& aBox = aBoxMaker.Shape();
    myGraph.Build(aBox);
    ASSERT_TRUE(myGraph.IsDone());
  }

  BRepGraph myGraph;
};

TEST_F(BRepGraph_EventBusTest, ZeroCost_NoSubscribers)
{
  // Mutate edge without any subscribing layer - verify no crash.
  {
    BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aMut = myGraph.Builder().MutEdge(BRepGraph_EdgeId(0));
    aMut->Tolerance                              = 0.5;
  }
  EXPECT_GT(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).OwnGen, 0u);
}

TEST_F(BRepGraph_EventBusTest, ImmediateMode_SingleEdge)
{
  const int aAllKinds = BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Edge)
                        | BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Wire)
                        | BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Face)
                        | BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Shell)
                        | BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Solid);
  occ::handle<BRepGraph_ModTrackingLayer> aLayer =
    new BRepGraph_ModTrackingLayer("Tracker", aAllKinds);
  myGraph.LayerRegistry().RegisterLayer(aLayer);

  {
    BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aMut = myGraph.Builder().MutEdge(BRepGraph_EdgeId(0));
    aMut->Tolerance                              = 0.5;
  }

  // Edge(0) should have an immediate event.
  EXPECT_TRUE(aLayer->HasImmediateEventFor(BRepGraph_NodeId::Edge(0)));
  // At least one event total (edge + propagated parents).
  EXPECT_GT(aLayer->myImmediateEvents.Length(), 0);
}

TEST_F(BRepGraph_EventBusTest, ImmediateMode_UpwardPropagation)
{
  const int aAllKinds = BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Edge)
                        | BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Wire)
                        | BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Face)
                        | BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Shell)
                        | BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Solid);
  occ::handle<BRepGraph_ModTrackingLayer> aLayer =
    new BRepGraph_ModTrackingLayer("Tracker", aAllKinds);
  myGraph.LayerRegistry().RegisterLayer(aLayer);

  {
    BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aMut = myGraph.Builder().MutEdge(BRepGraph_EdgeId(0));
    aMut->Tolerance                              = 0.5;
  }

  // Only directly mutated node gets immediate dispatch.
  // Parents get SubtreeGen incremented but NO dispatch (mutex-free propagation).
  EXPECT_GT(aLayer->CountImmediateEventsOfKind(BRepGraph_NodeId::Kind::Edge), 0);
  EXPECT_EQ(aLayer->CountImmediateEventsOfKind(BRepGraph_NodeId::Kind::Wire), 0);
  EXPECT_EQ(aLayer->CountImmediateEventsOfKind(BRepGraph_NodeId::Kind::Face), 0);
  EXPECT_EQ(aLayer->CountImmediateEventsOfKind(BRepGraph_NodeId::Kind::Shell), 0);
  EXPECT_EQ(aLayer->CountImmediateEventsOfKind(BRepGraph_NodeId::Kind::Solid), 0);
  // Verify SubtreeGen was propagated upward despite no dispatch.
  EXPECT_GT(myGraph.Topo().Wire(BRepGraph_WireId(0)).SubtreeGen, 0u);
  EXPECT_GT(myGraph.Topo().Face(BRepGraph_FaceId(0)).SubtreeGen, 0u);
}

TEST_F(BRepGraph_EventBusTest, ImmediateMode_KindFilter)
{
  // Subscribe only to Face.
  occ::handle<BRepGraph_ModTrackingLayer> aLayer =
    new BRepGraph_ModTrackingLayer("FaceOnly",
                                   BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Face));
  myGraph.LayerRegistry().RegisterLayer(aLayer);

  {
    BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aMut = myGraph.Builder().MutEdge(BRepGraph_EdgeId(0));
    aMut->Tolerance                              = 0.5;
  }

  // No face dispatch from upward propagation (mutex-free SubtreeGen only).
  // Edge is directly mutated but Face-only subscription filters it out.
  EXPECT_EQ(aLayer->CountImmediateEventsOfKind(BRepGraph_NodeId::Kind::Edge), 0);
  EXPECT_EQ(aLayer->CountImmediateEventsOfKind(BRepGraph_NodeId::Kind::Face), 0);
  // But SubtreeGen was propagated.
  EXPECT_GT(myGraph.Topo().Face(BRepGraph_FaceId(0)).SubtreeGen, 0u);
}

TEST_F(BRepGraph_EventBusTest, DeferredMode_BatchDispatch)
{
  const int aAllKinds = BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Edge)
                        | BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Wire)
                        | BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Face)
                        | BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Shell)
                        | BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Solid);
  occ::handle<BRepGraph_ModTrackingLayer> aLayer =
    new BRepGraph_ModTrackingLayer("Tracker", aAllKinds);
  myGraph.LayerRegistry().RegisterLayer(aLayer);

  myGraph.Builder().BeginDeferredInvalidation();
  myGraph.Builder().MutEdge(BRepGraph_EdgeId(0))->Tolerance = 0.5;
  myGraph.Builder().MutEdge(BRepGraph_EdgeId(1))->Tolerance = 0.6;
  myGraph.Builder().MutEdge(BRepGraph_EdgeId(2))->Tolerance = 0.7;
  myGraph.Builder().EndDeferredInvalidation();

  // OnNodesModified called exactly once.
  EXPECT_EQ(aLayer->myBatchCallCount, 1);
  // Batch contains at least the 3 edges + propagated parents.
  EXPECT_GE(aLayer->myBatchEvents.Length(), 3);
  EXPECT_TRUE(aLayer->HasBatchEventFor(BRepGraph_NodeId::Edge(0)));
  EXPECT_TRUE(aLayer->HasBatchEventFor(BRepGraph_NodeId::Edge(1)));
  EXPECT_TRUE(aLayer->HasBatchEventFor(BRepGraph_NodeId::Edge(2)));
}

TEST_F(BRepGraph_EventBusTest, DeferredMode_NoImmediateDispatch)
{
  const int aEdgeBit = BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Edge);
  occ::handle<BRepGraph_ModTrackingLayer> aLayer =
    new BRepGraph_ModTrackingLayer("Tracker", aEdgeBit);
  myGraph.LayerRegistry().RegisterLayer(aLayer);

  myGraph.Builder().BeginDeferredInvalidation();
  myGraph.Builder().MutEdge(BRepGraph_EdgeId(0))->Tolerance = 0.5;

  // During deferred mode: OnNodeModified must NOT be called.
  EXPECT_EQ(aLayer->myImmediateEvents.Length(), 0);

  myGraph.Builder().EndDeferredInvalidation();

  // Immediate events still empty - only batch was dispatched.
  EXPECT_EQ(aLayer->myImmediateEvents.Length(), 0);
  EXPECT_EQ(aLayer->myBatchCallCount, 1);
}

TEST_F(BRepGraph_EventBusTest, UnregisterLayer_FlagUpdate)
{
  const int aEdgeBit = BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Edge);
  occ::handle<BRepGraph_ModTrackingLayer> aLayer =
    new BRepGraph_ModTrackingLayer("Tracker", aEdgeBit);
  myGraph.LayerRegistry().RegisterLayer(aLayer);

  // Mutate - should dispatch.
  {
    BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aMut = myGraph.Builder().MutEdge(BRepGraph_EdgeId(0));
    aMut->Tolerance                              = 0.5;
  }
  EXPECT_GT(aLayer->myImmediateEvents.Length(), 0);

  // Unregister and clear.
  myGraph.LayerRegistry().UnregisterLayer(aLayer->ID());
  aLayer->Clear();

  // Mutate again - should NOT dispatch (layer unregistered).
  {
    BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aMut = myGraph.Builder().MutEdge(BRepGraph_EdgeId(1));
    aMut->Tolerance                              = 0.6;
  }
  EXPECT_EQ(aLayer->myImmediateEvents.Length(), 0);
}

TEST_F(BRepGraph_EventBusTest, FindLayer_ByGuid_ReturnsRegisteredLayer)
{
  const int aEdgeBit = BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Edge);
  occ::handle<BRepGraph_ModTrackingLayer> aLayer =
    new BRepGraph_ModTrackingLayer("Tracker", aEdgeBit);

  myGraph.LayerRegistry().RegisterLayer(aLayer);

  EXPECT_EQ(myGraph.LayerRegistry().FindLayer(aLayer->ID()), aLayer);
  EXPECT_GE(myGraph.LayerRegistry().FindSlot(aLayer->ID()), 0);
}

TEST_F(BRepGraph_EventBusTest, OnNodeRemoved_DispatchesReplacement)
{
  occ::handle<BRepGraph_ModTrackingLayer> aLayer =
    new BRepGraph_ModTrackingLayer("Tracker", 0);
  myGraph.LayerRegistry().RegisterLayer(aLayer);

  const BRepGraph_NodeId anOldEdge = BRepGraph_NodeId::Edge(0);
  const BRepGraph_NodeId aNewEdge  = BRepGraph_NodeId::Edge(1);
  myGraph.Builder().RemoveNode(anOldEdge, aNewEdge);

  EXPECT_EQ(aLayer->myRemoveCallCount, 1);
  EXPECT_EQ(aLayer->myLastRemovedNode, anOldEdge);
  EXPECT_EQ(aLayer->myLastReplacement, aNewEdge);
}

TEST_F(BRepGraph_EventBusTest, OnNodeRemoved_DispatchesInvalidReplacementForPureDeletion)
{
  occ::handle<BRepGraph_ModTrackingLayer> aLayer =
    new BRepGraph_ModTrackingLayer("Tracker", 0);
  myGraph.LayerRegistry().RegisterLayer(aLayer);

  const BRepGraph_NodeId aFaceId = BRepGraph_NodeId::Face(0);
  myGraph.Builder().RemoveNode(aFaceId);

  EXPECT_EQ(aLayer->myRemoveCallCount, 1);
  EXPECT_EQ(aLayer->myLastRemovedNode, aFaceId);
  EXPECT_FALSE(aLayer->myLastReplacement.IsValid());
}

TEST_F(BRepGraph_EventBusTest, OnCompact_DispatchesRemapToRegisteredLayers)
{
  occ::handle<BRepGraph_ModTrackingLayer> aLayer =
    new BRepGraph_ModTrackingLayer("Tracker", 0);
  myGraph.LayerRegistry().RegisterLayer(aLayer);

  myGraph.Builder().RemoveNode(BRepGraph_NodeId::Face(0));
  const int aNbFacesBefore = myGraph.Topo().NbFaces();

  const BRepGraphAlgo_Compact::Result aResult = BRepGraphAlgo_Compact::Perform(myGraph);
  (void)aResult;

  EXPECT_EQ(aLayer->myCompactCallCount, 1);
  EXPECT_EQ(myGraph.Topo().NbFaces(), aNbFacesBefore - 1);

  const BRepGraph_NodeId* aNewFaceId = aLayer->myLastRemapMap.Seek(BRepGraph_NodeId::Face(1));
  ASSERT_NE(aNewFaceId, nullptr);
  EXPECT_EQ(aNewFaceId->NodeKind, BRepGraph_NodeId::Kind::Face);
  EXPECT_EQ(aNewFaceId->Index, 0);
  EXPECT_EQ(aLayer->myLastRemapMap.Seek(BRepGraph_NodeId::Face(0)), nullptr);
}

TEST_F(BRepGraph_EventBusTest, MultipleSubscribers)
{
  occ::handle<BRepGraph_ModTrackingLayer> aEdgeLayer =
    new BRepGraph_ModTrackingLayer("EdgeTracker",
                                   BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Edge),
                                   Standard_GUID("2f9b6a5c-1f2d-4a88-9c1c-7a0c16a10005"));
  occ::handle<BRepGraph_ModTrackingLayer> aFaceLayer =
    new BRepGraph_ModTrackingLayer("FaceTracker",
                                   BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Face),
                                   Standard_GUID("2f9b6a5c-1f2d-4a88-9c1c-7a0c16a10006"));
  myGraph.LayerRegistry().RegisterLayer(aEdgeLayer);
  myGraph.LayerRegistry().RegisterLayer(aFaceLayer);

  {
    BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aMut = myGraph.Builder().MutEdge(BRepGraph_EdgeId(0));
    aMut->Tolerance                              = 0.5;
  }

  // Edge layer gets edge events (directly mutated), no face events.
  EXPECT_GT(aEdgeLayer->CountImmediateEventsOfKind(BRepGraph_NodeId::Kind::Edge), 0);
  EXPECT_EQ(aEdgeLayer->CountImmediateEventsOfKind(BRepGraph_NodeId::Kind::Face), 0);

  // Face layer gets NO events - parents don't get immediate dispatch.
  EXPECT_EQ(aFaceLayer->CountImmediateEventsOfKind(BRepGraph_NodeId::Kind::Edge), 0);
  EXPECT_EQ(aFaceLayer->CountImmediateEventsOfKind(BRepGraph_NodeId::Kind::Face), 0);
}

TEST_F(BRepGraph_EventBusTest, DefaultSubscribedKinds_Zero)
{
  occ::handle<BRepGraph_DefaultLayer> aDefaultLayer = new BRepGraph_DefaultLayer;
  myGraph.LayerRegistry().RegisterLayer(aDefaultLayer);

  // SubscribedKinds() == 0 in BRepGraph_Layer base default implementation.
  EXPECT_EQ(aDefaultLayer->SubscribedKinds(), 0);

  // Mutate - a layer with default SubscribedKinds() should not receive modification events.
  // This just verifies the default no-subscription path remains a no-op.
  {
    BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aMut = myGraph.Builder().MutEdge(BRepGraph_EdgeId(0));
    aMut->Tolerance                              = 0.5;
  }
  EXPECT_GT(myGraph.Topo().Edge(BRepGraph_EdgeId(0)).OwnGen, 0u);
}

TEST_F(BRepGraph_EventBusTest, DeferredScope_DispatchesOnDestruction)
{
  const int aEdgeBit = BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Edge);
  occ::handle<BRepGraph_ModTrackingLayer> aLayer =
    new BRepGraph_ModTrackingLayer("Tracker", aEdgeBit);
  myGraph.LayerRegistry().RegisterLayer(aLayer);

  {
    BRepGraph_DeferredScope aScope(myGraph);
    myGraph.Builder().MutEdge(BRepGraph_EdgeId(0))->Tolerance = 0.5;

    // During guard scope: no batch dispatch yet.
    EXPECT_EQ(aLayer->myBatchCallCount, 0);
  }

  // After guard destruction: batch dispatched.
  EXPECT_EQ(aLayer->myBatchCallCount, 1);
  EXPECT_TRUE(aLayer->HasBatchEventFor(BRepGraph_NodeId::Edge(0)));
}

TEST_F(BRepGraph_EventBusTest, DeferredMode_NoModifications_NoDispatch)
{
  const int aEdgeBit = BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Edge);
  occ::handle<BRepGraph_ModTrackingLayer> aLayer =
    new BRepGraph_ModTrackingLayer("Tracker", aEdgeBit);
  myGraph.LayerRegistry().RegisterLayer(aLayer);

  myGraph.Builder().BeginDeferredInvalidation();
  // No mutations.
  myGraph.Builder().EndDeferredInvalidation();

  EXPECT_EQ(aLayer->myBatchCallCount, 0);
  EXPECT_EQ(aLayer->myBatchEvents.Length(), 0);
}

TEST_F(BRepGraph_EventBusTest, KindBit_Helpers)
{
  // Each kind produces a distinct single-bit value derived from the Kind enum.
  using Kind = BRepGraph_NodeId::Kind;
  EXPECT_EQ(BRepGraph_Layer::KindBit(Kind::Solid), 1 << static_cast<int>(Kind::Solid));
  EXPECT_EQ(BRepGraph_Layer::KindBit(Kind::Shell), 1 << static_cast<int>(Kind::Shell));
  EXPECT_EQ(BRepGraph_Layer::KindBit(Kind::Face), 1 << static_cast<int>(Kind::Face));
  EXPECT_EQ(BRepGraph_Layer::KindBit(Kind::Wire), 1 << static_cast<int>(Kind::Wire));
  EXPECT_EQ(BRepGraph_Layer::KindBit(Kind::Edge), 1 << static_cast<int>(Kind::Edge));
  EXPECT_EQ(BRepGraph_Layer::KindBit(Kind::Vertex), 1 << static_cast<int>(Kind::Vertex));
  EXPECT_EQ(BRepGraph_Layer::KindBit(Kind::Compound), 1 << static_cast<int>(Kind::Compound));
  EXPECT_EQ(BRepGraph_Layer::KindBit(Kind::CompSolid), 1 << static_cast<int>(Kind::CompSolid));

  // All kind bits are distinct (no collisions).
  const int aAll = BRepGraph_Layer::KindBit(Kind::Solid) | BRepGraph_Layer::KindBit(Kind::Shell)
                   | BRepGraph_Layer::KindBit(Kind::Face) | BRepGraph_Layer::KindBit(Kind::Wire)
                   | BRepGraph_Layer::KindBit(Kind::Edge) | BRepGraph_Layer::KindBit(Kind::Vertex)
                   | BRepGraph_Layer::KindBit(Kind::Compound)
                   | BRepGraph_Layer::KindBit(Kind::CompSolid);
  // 8 distinct bits set.
  int aBitCount = 0;
  for (int v = aAll; v != 0; v >>= 1)
    aBitCount += (v & 1);
  EXPECT_EQ(aBitCount, 8);
}

TEST_F(BRepGraph_EventBusTest, OverlappingSubscription_EdgeAndFace)
{
  // Layer subscribes to both Edge and Face - should receive events for both kinds.
  const int aEdgeFace = BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Edge)
                        | BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Face);
  occ::handle<BRepGraph_ModTrackingLayer> aLayer =
    new BRepGraph_ModTrackingLayer("EdgeFace", aEdgeFace);
  myGraph.LayerRegistry().RegisterLayer(aLayer);

  {
    BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aMut = myGraph.Builder().MutEdge(BRepGraph_EdgeId(0));
    aMut->Tolerance                              = 0.5;
  }

  // Edge events from direct mutation; NO face events (no parent dispatch).
  EXPECT_GT(aLayer->CountImmediateEventsOfKind(BRepGraph_NodeId::Kind::Edge), 0);
  EXPECT_EQ(aLayer->CountImmediateEventsOfKind(BRepGraph_NodeId::Kind::Face), 0);
  EXPECT_EQ(aLayer->CountImmediateEventsOfKind(BRepGraph_NodeId::Kind::Wire), 0);
}
