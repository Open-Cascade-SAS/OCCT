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
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_Layer.hxx>
#include <BRepGraph_MutView.hxx>
#include <BRepGraph_MutationGuard.hxx>
#include <BRepGraph_NameLayer.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

// Test layer that records modification events.
class BRepGraph_ModTrackingLayer : public BRepGraph_Layer
{
public:
  BRepGraph_ModTrackingLayer(const TCollection_AsciiString& theName, int theSubscribedKinds)
      : myName(theName),
        mySubscribedKinds(theSubscribedKinds)
  {
  }

  const TCollection_AsciiString& Name() const override { return myName; }

  int SubscribedKinds() const override { return mySubscribedKinds; }

  void OnNodeModified(const BRepGraph_NodeId theNode) override
  {
    myImmediateEvents.Append(theNode);
  }

  void OnNodesModified(const NCollection_Vector<BRepGraph_NodeId>& theNodes) override
  {
    myBatchEvents = theNodes;
    ++myBatchCallCount;
  }

  void OnNodeRemoved(const BRepGraph_NodeId /*theNode*/,
                     const BRepGraph_NodeId /*theReplacement*/) override
  {
  }

  void OnCompact(
    const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& /*theRemapMap*/) override
  {
  }

  void InvalidateAll() override {}

  void Clear() override
  {
    myImmediateEvents.Clear();
    myBatchEvents.Clear();
    myBatchCallCount = 0;
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

  DEFINE_STANDARD_RTTIEXT(BRepGraph_ModTrackingLayer, BRepGraph_Layer)

private:
  TCollection_AsciiString myName;
  int                     mySubscribedKinds;
};

IMPLEMENT_STANDARD_RTTIEXT(BRepGraph_ModTrackingLayer, BRepGraph_Layer)

class BRepGraphEventBusTest : public testing::Test
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

TEST_F(BRepGraphEventBusTest, ZeroCost_NoSubscribers)
{
  // Mutate edge without any subscribing layer - verify no crash.
  {
    auto aMut       = myGraph.MutEdge(0);
    aMut->Tolerance = 0.5;
  }
  EXPECT_TRUE(myGraph.Defs().Edge(0).IsModified);
}

TEST_F(BRepGraphEventBusTest, ImmediateMode_SingleEdge)
{
  const int aAllKinds = BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Edge)
                        | BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Wire)
                        | BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Face)
                        | BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Shell)
                        | BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Solid);
  occ::handle<BRepGraph_ModTrackingLayer> aLayer =
    new BRepGraph_ModTrackingLayer("Tracker", aAllKinds);
  myGraph.RegisterLayer(aLayer);

  {
    auto aMut       = myGraph.MutEdge(0);
    aMut->Tolerance = 0.5;
  }

  // Edge(0) should have an immediate event.
  EXPECT_TRUE(aLayer->HasImmediateEventFor(BRepGraph_NodeId::Edge(0)));
  // At least one event total (edge + propagated parents).
  EXPECT_GT(aLayer->myImmediateEvents.Length(), 0);
}

TEST_F(BRepGraphEventBusTest, ImmediateMode_UpwardPropagation)
{
  const int aAllKinds = BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Edge)
                        | BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Wire)
                        | BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Face)
                        | BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Shell)
                        | BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Solid);
  occ::handle<BRepGraph_ModTrackingLayer> aLayer =
    new BRepGraph_ModTrackingLayer("Tracker", aAllKinds);
  myGraph.RegisterLayer(aLayer);

  {
    auto aMut       = myGraph.MutEdge(0);
    aMut->Tolerance = 0.5;
  }

  // Upward propagation: Edge -> Wire -> Face -> Shell -> Solid.
  EXPECT_GT(aLayer->CountImmediateEventsOfKind(BRepGraph_NodeId::Kind::Edge), 0);
  EXPECT_GT(aLayer->CountImmediateEventsOfKind(BRepGraph_NodeId::Kind::Wire), 0);
  EXPECT_GT(aLayer->CountImmediateEventsOfKind(BRepGraph_NodeId::Kind::Face), 0);
  EXPECT_GT(aLayer->CountImmediateEventsOfKind(BRepGraph_NodeId::Kind::Shell), 0);
  EXPECT_GT(aLayer->CountImmediateEventsOfKind(BRepGraph_NodeId::Kind::Solid), 0);
}

TEST_F(BRepGraphEventBusTest, ImmediateMode_KindFilter)
{
  // Subscribe only to Face.
  occ::handle<BRepGraph_ModTrackingLayer> aLayer =
    new BRepGraph_ModTrackingLayer("FaceOnly",
                                   BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Face));
  myGraph.RegisterLayer(aLayer);

  {
    auto aMut       = myGraph.MutEdge(0);
    aMut->Tolerance = 0.5;
  }

  // Should receive face events (from upward propagation), but NOT edge events.
  EXPECT_EQ(aLayer->CountImmediateEventsOfKind(BRepGraph_NodeId::Kind::Edge), 0);
  EXPECT_GT(aLayer->CountImmediateEventsOfKind(BRepGraph_NodeId::Kind::Face), 0);
}

TEST_F(BRepGraphEventBusTest, DeferredMode_BatchDispatch)
{
  const int aAllKinds = BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Edge)
                        | BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Wire)
                        | BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Face)
                        | BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Shell)
                        | BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Solid);
  occ::handle<BRepGraph_ModTrackingLayer> aLayer =
    new BRepGraph_ModTrackingLayer("Tracker", aAllKinds);
  myGraph.RegisterLayer(aLayer);

  myGraph.BeginDeferredInvalidation();
  myGraph.Mut().EdgeDef(0)->Tolerance = 0.5;
  myGraph.Mut().EdgeDef(1)->Tolerance = 0.6;
  myGraph.Mut().EdgeDef(2)->Tolerance = 0.7;
  myGraph.EndDeferredInvalidation();

  // OnNodesModified called exactly once.
  EXPECT_EQ(aLayer->myBatchCallCount, 1);
  // Batch contains at least the 3 edges + propagated parents.
  EXPECT_GE(aLayer->myBatchEvents.Length(), 3);
  EXPECT_TRUE(aLayer->HasBatchEventFor(BRepGraph_NodeId::Edge(0)));
  EXPECT_TRUE(aLayer->HasBatchEventFor(BRepGraph_NodeId::Edge(1)));
  EXPECT_TRUE(aLayer->HasBatchEventFor(BRepGraph_NodeId::Edge(2)));
}

TEST_F(BRepGraphEventBusTest, DeferredMode_NoImmediateDispatch)
{
  const int aEdgeBit = BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Edge);
  occ::handle<BRepGraph_ModTrackingLayer> aLayer =
    new BRepGraph_ModTrackingLayer("Tracker", aEdgeBit);
  myGraph.RegisterLayer(aLayer);

  myGraph.BeginDeferredInvalidation();
  myGraph.Mut().EdgeDef(0)->Tolerance = 0.5;

  // During deferred mode: OnNodeModified must NOT be called.
  EXPECT_EQ(aLayer->myImmediateEvents.Length(), 0);

  myGraph.EndDeferredInvalidation();

  // Immediate events still empty - only batch was dispatched.
  EXPECT_EQ(aLayer->myImmediateEvents.Length(), 0);
  EXPECT_EQ(aLayer->myBatchCallCount, 1);
}

TEST_F(BRepGraphEventBusTest, UnregisterLayer_FlagUpdate)
{
  const int aEdgeBit = BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Edge);
  occ::handle<BRepGraph_ModTrackingLayer> aLayer =
    new BRepGraph_ModTrackingLayer("Tracker", aEdgeBit);
  myGraph.RegisterLayer(aLayer);

  // Mutate - should dispatch.
  {
    auto aMut       = myGraph.MutEdge(0);
    aMut->Tolerance = 0.5;
  }
  EXPECT_GT(aLayer->myImmediateEvents.Length(), 0);

  // Unregister and clear.
  myGraph.UnregisterLayer("Tracker");
  aLayer->Clear();

  // Mutate again - should NOT dispatch (layer unregistered).
  {
    auto aMut       = myGraph.MutEdge(1);
    aMut->Tolerance = 0.6;
  }
  EXPECT_EQ(aLayer->myImmediateEvents.Length(), 0);
}

TEST_F(BRepGraphEventBusTest, MultipleSubscribers)
{
  occ::handle<BRepGraph_ModTrackingLayer> aEdgeLayer =
    new BRepGraph_ModTrackingLayer("EdgeTracker",
                                   BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Edge));
  occ::handle<BRepGraph_ModTrackingLayer> aFaceLayer =
    new BRepGraph_ModTrackingLayer("FaceTracker",
                                   BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Face));
  myGraph.RegisterLayer(aEdgeLayer);
  myGraph.RegisterLayer(aFaceLayer);

  {
    auto aMut       = myGraph.MutEdge(0);
    aMut->Tolerance = 0.5;
  }

  // Edge layer gets edge events, no face events.
  EXPECT_GT(aEdgeLayer->CountImmediateEventsOfKind(BRepGraph_NodeId::Kind::Edge), 0);
  EXPECT_EQ(aEdgeLayer->CountImmediateEventsOfKind(BRepGraph_NodeId::Kind::Face), 0);

  // Face layer gets face events (from propagation), no edge events.
  EXPECT_EQ(aFaceLayer->CountImmediateEventsOfKind(BRepGraph_NodeId::Kind::Edge), 0);
  EXPECT_GT(aFaceLayer->CountImmediateEventsOfKind(BRepGraph_NodeId::Kind::Face), 0);
}

TEST_F(BRepGraphEventBusTest, DefaultSubscribedKinds_Zero)
{
  occ::handle<BRepGraph_NameLayer> aNameLayer = new BRepGraph_NameLayer;
  myGraph.RegisterLayer(aNameLayer);

  // SubscribedKinds() == 0 for NameLayer (default).
  EXPECT_EQ(aNameLayer->SubscribedKinds(), 0);

  // Mutate - NameLayer should not receive modification events.
  // (We just verify no crash; NameLayer has no event tracking.)
  {
    auto aMut       = myGraph.MutEdge(0);
    aMut->Tolerance = 0.5;
  }
  EXPECT_TRUE(myGraph.Defs().Edge(0).IsModified);
}

TEST_F(BRepGraphEventBusTest, MutationGuard_DispatchesOnDestruction)
{
  const int aEdgeBit = BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Edge);
  occ::handle<BRepGraph_ModTrackingLayer> aLayer =
    new BRepGraph_ModTrackingLayer("Tracker", aEdgeBit);
  myGraph.RegisterLayer(aLayer);

  {
    BRepGraph_MutationGuard aGuard(myGraph);
    myGraph.Mut().EdgeDef(0)->Tolerance = 0.5;

    // During guard scope: no batch dispatch yet.
    EXPECT_EQ(aLayer->myBatchCallCount, 0);
  }

  // After guard destruction: batch dispatched.
  EXPECT_EQ(aLayer->myBatchCallCount, 1);
  EXPECT_TRUE(aLayer->HasBatchEventFor(BRepGraph_NodeId::Edge(0)));
}

TEST_F(BRepGraphEventBusTest, DeferredMode_NoModifications_NoDispatch)
{
  const int aEdgeBit = BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Edge);
  occ::handle<BRepGraph_ModTrackingLayer> aLayer =
    new BRepGraph_ModTrackingLayer("Tracker", aEdgeBit);
  myGraph.RegisterLayer(aLayer);

  myGraph.BeginDeferredInvalidation();
  // No mutations.
  myGraph.EndDeferredInvalidation();

  EXPECT_EQ(aLayer->myBatchCallCount, 0);
  EXPECT_EQ(aLayer->myBatchEvents.Length(), 0);
}

TEST_F(BRepGraphEventBusTest, KindBit_Helpers)
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

TEST_F(BRepGraphEventBusTest, OverlappingSubscription_EdgeAndFace)
{
  // Layer subscribes to both Edge and Face - should receive events for both kinds.
  const int aEdgeFace = BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Edge)
                        | BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Face);
  occ::handle<BRepGraph_ModTrackingLayer> aLayer =
    new BRepGraph_ModTrackingLayer("EdgeFace", aEdgeFace);
  myGraph.RegisterLayer(aLayer);

  {
    auto aMut       = myGraph.MutEdge(0);
    aMut->Tolerance = 0.5;
  }

  // Should see both edge events and face events (from upward propagation).
  EXPECT_GT(aLayer->CountImmediateEventsOfKind(BRepGraph_NodeId::Kind::Edge), 0);
  EXPECT_GT(aLayer->CountImmediateEventsOfKind(BRepGraph_NodeId::Kind::Face), 0);
  // Should NOT see wire events (not subscribed).
  EXPECT_EQ(aLayer->CountImmediateEventsOfKind(BRepGraph_NodeId::Kind::Wire), 0);
}
