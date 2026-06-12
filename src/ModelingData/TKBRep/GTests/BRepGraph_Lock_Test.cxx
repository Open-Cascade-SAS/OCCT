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
#include <BRepGraph_LayerDeferred.hxx>
#include <BRepGraph_LayerLock.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_Compact.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Geom_Surface.hxx>
#include <Standard_ProgramError.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <gtest/gtest.h>

namespace
{

BRepGraph makeBoxGraph()
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aResult =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  return aGraph;
}

const Standard_GUID& testOwnerId()
{
  static const Standard_GUID THE_OWNER_ID("2ee13474-4cc6-4f6f-bde2-f0e4dcf9e5f1");
  return THE_OWNER_ID;
}

const Standard_GUID& otherOwnerId()
{
  static const Standard_GUID THE_OWNER_ID("63c7da5f-4adc-4024-85f2-329525dc76b7");
  return THE_OWNER_ID;
}

template <typename ItemIdT>
void lockItem(BRepGraph& theGraph, const ItemIdT theItem)
{
  theGraph.LayerRegistry().Ensure<BRepGraph_LayerLock>()->SetOwner(theItem, testOwnerId());
}

void unlockItem(BRepGraph& theGraph, const BRepGraph_VertexId theItem)
{
  theGraph.LayerRegistry().Ensure<BRepGraph_LayerLock>()->UnsetOwner(
    BRepGraph_ItemId(static_cast<BRepGraph_NodeId>(theItem)),
    testOwnerId());
}

bool isOwned(const BRepGraph& theGraph, const BRepGraph_VertexId theVertex)
{
  const occ::handle<BRepGraph_LayerLock> aLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_LayerLock>();
  return !aLayer.IsNull() && aLayer->HasOwner(theVertex);
}

bool isOwned(const BRepGraph& theGraph, const BRepGraph_FaceId theFace)
{
  const occ::handle<BRepGraph_LayerLock> aLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_LayerLock>();
  return !aLayer.IsNull() && aLayer->HasOwner(theFace);
}

bool isOwned(const BRepGraph& theGraph, const BRepGraph_VertexRefId theRef)
{
  const occ::handle<BRepGraph_LayerLock> aLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_LayerLock>();
  return !aLayer.IsNull() && aLayer->HasOwner(theRef);
}

bool isOwned(const BRepGraph& theGraph, const BRepGraph_WireRefId theRef)
{
  const occ::handle<BRepGraph_LayerLock> aLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_LayerLock>();
  return !aLayer.IsNull() && aLayer->HasOwner(theRef);
}

class BRepGraph_LayerItemDispatchProbe : public BRepGraph_Layer
{
public:
  const Standard_GUID& ID() const override
  {
    static const Standard_GUID THE_ID("9f11b126-430a-4535-aa4a-8b8a4446dd5f");
    return THE_ID;
  }

  const TCollection_AsciiString& Name() const override
  {
    static const TCollection_AsciiString THE_NAME("ItemDispatchProbe");
    return THE_NAME;
  }

  int SubscribedKinds() const override
  {
    return BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Face);
  }

  int SubscribedRefKinds() const override
  {
    return BRepGraph_Layer::RefKindBit(BRepGraph_RefId::Kind::Vertex);
  }

  void OnNodeRemoved(const BRepGraph_NodeId theNode) noexcept override
  {
    myRemovedItems.Append(BRepGraph_ItemId(theNode));
  }

  void OnRefRemoved(const BRepGraph_RefId theRef) noexcept override
  {
    myRemovedItems.Append(BRepGraph_ItemId(theRef));
  }

  void OnNodeModified(const BRepGraph_NodeId theNode) noexcept override
  {
    myModifiedItems.Append(BRepGraph_ItemId(theNode));
  }

  void OnRefModified(const BRepGraph_RefId theRef) noexcept override
  {
    myModifiedItems.Append(BRepGraph_ItemId(theRef));
  }

  void CopyTo(const BRepGraph_CopyRemap&) const override {}

  void InvalidateAll() noexcept override {}

  void Clear() noexcept override
  {
    myRemovedItems.Clear();
    myModifiedItems.Clear();
  }

  const NCollection_DynamicArray<BRepGraph_ItemId>& RemovedItems() const { return myRemovedItems; }

  const NCollection_DynamicArray<BRepGraph_ItemId>& ModifiedItems() const
  {
    return myModifiedItems;
  }

private:
  NCollection_DynamicArray<BRepGraph_ItemId> myRemovedItems;
  NCollection_DynamicArray<BRepGraph_ItemId> myModifiedItems;
};

} // namespace

TEST(BRepGraph_LockTest, NewItemsAreUnlockedByDefault)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_VertexId    aVertexId = BRepGraph_VertexId::Start();
  const BRepGraph_EdgeId      anEdgeId  = BRepGraph_EdgeId::Start();
  const BRepGraph_VertexRefId aVertexRefId =
    aGraph.Topo().Edges().Definition(anEdgeId).StartVertexRefId;

  EXPECT_FALSE(isOwned(aGraph, aVertexId));
  EXPECT_FALSE(isOwned(aGraph, aVertexRefId));
}

TEST(BRepGraph_LockTest, LockLayerControlsStorageFlag)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_VertexId aVertexId = BRepGraph_VertexId::Start();
  EXPECT_FALSE(isOwned(aGraph, aVertexId));

  lockItem(aGraph, aVertexId);
  EXPECT_TRUE(isOwned(aGraph, aVertexId));

  unlockItem(aGraph, aVertexId);
  EXPECT_FALSE(isOwned(aGraph, aVertexId));
}

TEST(BRepGraph_LockTest, LockedNodeRejectsMutableGuard)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_VertexId aVertexId = BRepGraph_VertexId::Start();
  lockItem(aGraph, aVertexId);

#ifndef No_Exception
  EXPECT_THROW({ (void)aGraph.Editor().Vertices().Mut(aVertexId); }, Standard_ProgramError);
#endif
}

TEST(BRepGraph_LockTest, LockedReferenceRejectsMutableGuard)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_VertexRefId aVertexRefId =
    aGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).StartVertexRefId;
  lockItem(aGraph, aVertexRefId);

#ifndef No_Exception
  EXPECT_THROW({ (void)aGraph.Editor().Vertices().MutRef(aVertexRefId); }, Standard_ProgramError);
#endif
}

TEST(BRepGraph_LockTest, LockedNodeRejectsRemoval)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_VertexId aVertexId = BRepGraph_VertexId::Start();
  lockItem(aGraph, aVertexId);

#ifndef No_Exception
  EXPECT_THROW({ aGraph.Editor().Gen().RemoveNode(aVertexId); }, Standard_ProgramError);
#endif
}

TEST(BRepGraph_LockTest, LockedParentRejectsStructuralAdd)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_ShellId aShellId = BRepGraph_ShellId::Start();
  lockItem(aGraph, aShellId);

#ifndef No_Exception
  const BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start();
  EXPECT_THROW(
    { (void)aGraph.Editor().Shells().Append(aShellId, aFaceId, TopAbs_FORWARD); },
    Standard_ProgramError);
#endif
}

TEST(BRepGraph_LockTest, LockedNodeRejectsDirectSetterWithoutMutation)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_VertexId aVertexId = BRepGraph_VertexId::Start();
  const gp_Pnt             aPoint    = aGraph.Topo().Vertices().Definition(aVertexId).Point;
  lockItem(aGraph, aVertexId);

#ifndef No_Exception
  EXPECT_THROW(
    { aGraph.Editor().Vertices().SetPoint(aVertexId, gp_Pnt(1.0, 2.0, 3.0)); },
    Standard_ProgramError);
#endif
  EXPECT_NEAR(aGraph.Topo().Vertices().Definition(aVertexId).Point.Distance(aPoint), 0.0, 1.0e-12);
}

TEST(BRepGraph_LockTest, LockedReferenceRejectsOrientationSetterWithoutMutation)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_VertexRefId aVertexRefId =
    aGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).StartVertexRefId;
  const TopAbs_Orientation anOrientation = aGraph.Refs().Vertices().Entry(aVertexRefId).Orientation;
  lockItem(aGraph, aVertexRefId);

#ifndef No_Exception
  EXPECT_THROW(
    { aGraph.Editor().Vertices().SetRefOrientation(aVertexRefId, TopAbs_REVERSED); },
    Standard_ProgramError);
#endif
  EXPECT_EQ(aGraph.Refs().Vertices().Entry(aVertexRefId).Orientation, anOrientation);
}

TEST(BRepGraph_LockTest, LockedRepresentationRejectsDirectSetterWithoutMutation)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  const occ::handle<Geom_Surface> aSurface =
    aGraph.Topo().Faces().Surface(BRepGraph_FaceId::Start());
  ASSERT_FALSE(aSurface.IsNull());

  EXPECT_EQ(aGraph.Topo().Faces().Surface(BRepGraph_FaceId::Start()).get(), aSurface.get());
}

TEST(BRepGraph_LockTest, LockLayerOwnerControlsLockFlag)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_VertexId         aVertexId = BRepGraph_VertexId::Start();
  occ::handle<BRepGraph_LayerLock> aLayer    = aGraph.LayerRegistry().Ensure<BRepGraph_LayerLock>();
  const Standard_GUID              anOwnerId("2ee13474-4cc6-4f6f-bde2-f0e4dcf9e5f1");

  aLayer->SetOwner(aVertexId, anOwnerId);

  EXPECT_TRUE(isOwned(aGraph, aVertexId));
  ASSERT_TRUE(aLayer->HasOwner(aVertexId));
  Standard_GUID aFoundOwnerId;
  ASSERT_TRUE(aLayer->FindOwnerId(aVertexId, aFoundOwnerId));
  EXPECT_EQ(aFoundOwnerId, anOwnerId);

  aLayer->UnsetOwner(aVertexId);
  EXPECT_FALSE(isOwned(aGraph, aVertexId));
  EXPECT_FALSE(aLayer->HasOwner(aVertexId));
}

TEST(BRepGraph_LockTest, RejectsDifferentOwnerWhenAncestorOwnsNode)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_SolidId          aSolidId = BRepGraph_SolidId::Start();
  const BRepGraph_FaceId           aFaceId  = BRepGraph_FaceId::Start();
  occ::handle<BRepGraph_LayerLock> aLayer   = aGraph.LayerRegistry().Ensure<BRepGraph_LayerLock>();

  ASSERT_TRUE(aLayer->SetOwner(BRepGraph_ItemId(aSolidId), testOwnerId(), false));
  EXPECT_TRUE(aLayer->HasOwner(aFaceId));
  EXPECT_FALSE(aLayer->SetOwner(BRepGraph_ItemId(aFaceId), otherOwnerId(), false));

  Standard_GUID aFoundOwnerId;
  ASSERT_TRUE(aLayer->FindOwnerId(aFaceId, aFoundOwnerId));
  EXPECT_EQ(aFoundOwnerId, testOwnerId());
}

TEST(BRepGraph_LockTest, ParentOwnerRejectsDifferentOwnedDescendant)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_SolidId          aSolidId = BRepGraph_SolidId::Start();
  const BRepGraph_FaceId           aFaceId  = BRepGraph_FaceId::Start();
  occ::handle<BRepGraph_LayerLock> aLayer   = aGraph.LayerRegistry().Ensure<BRepGraph_LayerLock>();

  ASSERT_TRUE(aLayer->SetOwner(BRepGraph_ItemId(aFaceId), testOwnerId(), false));
  EXPECT_FALSE(aLayer->SetOwner(BRepGraph_ItemId(aSolidId), otherOwnerId(), false));
  EXPECT_TRUE(aLayer->HasOwner(aFaceId));
  EXPECT_FALSE(aLayer->HasOwner(aSolidId));
}

TEST(BRepGraph_LockTest, ParentOwnerCollapsesSameOwnedDescendant)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_SolidId          aSolidId = BRepGraph_SolidId::Start();
  const BRepGraph_FaceId           aFaceId  = BRepGraph_FaceId::Start();
  occ::handle<BRepGraph_LayerLock> aLayer   = aGraph.LayerRegistry().Ensure<BRepGraph_LayerLock>();

  ASSERT_TRUE(aLayer->SetOwner(BRepGraph_ItemId(aFaceId), testOwnerId(), false));
  EXPECT_TRUE(aLayer->SetOwner(BRepGraph_ItemId(aSolidId), testOwnerId(), false));
  EXPECT_TRUE(aLayer->HasOwner(aFaceId));
  EXPECT_TRUE(aLayer->HasOwner(aSolidId));

  aLayer->UnsetOwner(aSolidId);
  EXPECT_FALSE(aLayer->HasOwner(aSolidId));
  EXPECT_FALSE(aLayer->HasOwner(aFaceId));
}

TEST(BRepGraph_LockTest, RemovedRootOwnerCallbackClearsDescendantOwnedFlags)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_SolidId            aSolidId = BRepGraph_SolidId::Start();
  const BRepGraph_FaceId             aFaceId  = BRepGraph_FaceId::Start();
  const BRepGraphInc::FaceRelations& aFaceRel = aGraph.Topo().Faces().Relations(aFaceId);
  ASSERT_FALSE(aFaceRel.WireRefIds.IsEmpty());
  const BRepGraph_WireRefId aWireRefId = aFaceRel.WireRefIds.First();

  occ::handle<BRepGraph_LayerLock> aLayer = aGraph.LayerRegistry().Ensure<BRepGraph_LayerLock>();
  ASSERT_TRUE(aLayer->SetOwner(BRepGraph_ItemId(aSolidId), testOwnerId(), false));
  EXPECT_TRUE(aLayer->HasOwner(aSolidId));
  EXPECT_TRUE(aLayer->HasOwner(aFaceId));
  EXPECT_TRUE(aLayer->HasOwner(aWireRefId));

  aGraph.LayerRegistry().DispatchOnNodeRemoved(aSolidId);

  EXPECT_FALSE(aLayer->HasOwner(aSolidId));
  EXPECT_FALSE(aLayer->HasOwner(aFaceId));
  EXPECT_FALSE(aLayer->HasOwner(aWireRefId));
}

TEST(BRepGraph_LockTest, DeferredLayerRegistersMultipleRepresentationsAndLocksItem)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_FaceId               aFaceId = BRepGraph_FaceId::Start();
  occ::handle<BRepGraph_LayerDeferred> aLayer =
    aGraph.LayerRegistry().Ensure<BRepGraph_LayerDeferred>();

  aLayer->RegisterDeferred(aFaceId,
                           "TestProvider",
                           "test-source",
                           BRepGraph_LayerDeferred::RepresentationKind::Geometry,
                           "surface",
                           42);
  aLayer->RegisterDeferred(aFaceId,
                           "TestProvider",
                           "test-source",
                           BRepGraph_LayerDeferred::RepresentationKind::Mesh,
                           "triangulation",
                           7);

  EXPECT_TRUE(isOwned(aGraph, aFaceId));
  const BRepGraph_LayerDeferred::Entry* anEntry = aLayer->FindDeferred(aFaceId);
  ASSERT_NE(anEntry, nullptr);
  EXPECT_EQ(anEntry->Provider, TCollection_AsciiString("TestProvider"));
  EXPECT_EQ(anEntry->SourceKey, TCollection_AsciiString("test-source"));
  EXPECT_EQ(anEntry->Representations.Size(), 2);

  aLayer->UnregisterDeferred(aFaceId);
  EXPECT_FALSE(isOwned(aGraph, aFaceId));
  EXPECT_FALSE(aLayer->HasDeferred(aFaceId));
}

TEST(BRepGraph_LockTest, DeferredLayerClearsOwnerOnNodeRemoved)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_FaceId               aFaceId = BRepGraph_FaceId::Start();
  occ::handle<BRepGraph_LayerDeferred> aLayer =
    aGraph.LayerRegistry().Ensure<BRepGraph_LayerDeferred>();

  aLayer->RegisterDeferred(aFaceId,
                           "TestProvider",
                           "test-source",
                           BRepGraph_LayerDeferred::RepresentationKind::Geometry,
                           "surface",
                           42);
  ASSERT_TRUE(isOwned(aGraph, aFaceId));
  ASSERT_TRUE(aLayer->HasDeferred(aFaceId));

  aGraph.LayerRegistry().DispatchOnNodeRemoved(aFaceId);

  EXPECT_FALSE(isOwned(aGraph, aFaceId));
  EXPECT_FALSE(aLayer->HasDeferred(aFaceId));
}

TEST(BRepGraph_LockTest, ItemDispatchDelegatesToTypedCallbacks)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  occ::handle<BRepGraph_LayerItemDispatchProbe> aLayer = new BRepGraph_LayerItemDispatchProbe();
  aGraph.LayerRegistry().RegisterLayer(aLayer);

  const BRepGraph_FaceId      aFaceId = BRepGraph_FaceId::Start();
  const BRepGraph_VertexRefId aVertexRefId =
    aGraph.Topo().Edges().Definition(BRepGraph_EdgeId::Start()).StartVertexRefId;

  aGraph.LayerRegistry().DispatchOnItemRemoved(BRepGraph_ItemId(aFaceId));
  aGraph.LayerRegistry().DispatchOnItemRemoved(BRepGraph_ItemId(aVertexRefId));

  ASSERT_EQ(aLayer->RemovedItems().Size(), 2);
  EXPECT_TRUE(aLayer->RemovedItems().Value(0).IsNode());
  EXPECT_EQ(aLayer->RemovedItems().Value(0).NodeId(), BRepGraph_NodeId(aFaceId));
  EXPECT_TRUE(aLayer->RemovedItems().Value(1).IsReference());
  EXPECT_EQ(aLayer->RemovedItems().Value(1).RefId(), BRepGraph_RefId(aVertexRefId));

  aGraph.LayerRegistry().DispatchItemModified(BRepGraph_ItemId(aFaceId));
  aGraph.LayerRegistry().DispatchItemModified(BRepGraph_ItemId(aVertexRefId));

  ASSERT_EQ(aLayer->ModifiedItems().Size(), 2);
  EXPECT_TRUE(aLayer->ModifiedItems().Value(0).IsNode());
  EXPECT_EQ(aLayer->ModifiedItems().Value(0).NodeId(), BRepGraph_NodeId(aFaceId));
  EXPECT_TRUE(aLayer->ModifiedItems().Value(1).IsReference());
  EXPECT_EQ(aLayer->ModifiedItems().Value(1).RefId(), BRepGraph_RefId(aVertexRefId));
}

TEST(BRepGraph_LockTest, Compact_PreservesDeferredRefEntry)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_FaceId             aRemovedFace = BRepGraph_FaceId::Start();
  const BRepGraph_FaceId             aKeptFace(1);
  const BRepGraphInc::FaceRelations& aFaceRelations = aGraph.Topo().Faces().Relations(aKeptFace);
  ASSERT_FALSE(aFaceRelations.WireRefIds.IsEmpty());
  const BRepGraph_WireRefId aWireRefId = aFaceRelations.WireRefIds.First();

  occ::handle<BRepGraph_LayerDeferred> aLayer =
    aGraph.LayerRegistry().Ensure<BRepGraph_LayerDeferred>();
  aLayer->RegisterDeferred(aWireRefId,
                           "TestProvider",
                           "ref-source",
                           BRepGraph_LayerDeferred::RepresentationKind::Topology,
                           "wire-ref",
                           11);
  ASSERT_TRUE(aLayer->HasDeferred(aWireRefId));
  ASSERT_TRUE(isOwned(aGraph, aWireRefId));

  aGraph.Editor().Gen().RemoveNode(aRemovedFace);
  std::ignore = BRepGraph_Compact::Perform(aGraph);

  // Re-acquire layer after compact (CopyLayersTo replaces old instance).
  aLayer = aGraph.LayerRegistry().Ensure<BRepGraph_LayerDeferred>();

  ASSERT_GE(aGraph.Topo().Faces().Nb(), 1u);
  const BRepGraphInc::FaceRelations& aCompactedFaceRelations =
    aGraph.Topo().Faces().Relations(BRepGraph_FaceId::Start());
  ASSERT_FALSE(aCompactedFaceRelations.WireRefIds.IsEmpty());
  const BRepGraph_WireRefId aCompactedWireRefId = aCompactedFaceRelations.WireRefIds.First();

  EXPECT_TRUE(aLayer->HasDeferred(aCompactedWireRefId));
  EXPECT_TRUE(isOwned(aGraph, aCompactedWireRefId));
}

TEST(BRepGraph_LockTest, Compact_PreservesLockOnExactGeometryRep)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_FaceId           aRemovedFace = BRepGraph_FaceId::Start();
  const BRepGraph_FaceId           aKeptFace(1);
  const BRepGraph_FaceSurfaceRepId aSurfaceRepId =
    aGraph.Topo().Faces().Definition(aKeptFace).SurfaceRepId;
  ASSERT_TRUE(aSurfaceRepId.IsValid());

  occ::handle<BRepGraph_LayerLock> aLayer = aGraph.LayerRegistry().Ensure<BRepGraph_LayerLock>();
  aLayer->SetOwner(aKeptFace, testOwnerId());
  ASSERT_TRUE(aLayer->HasOwner(aKeptFace));

  aGraph.Editor().Gen().RemoveNode(aRemovedFace);
  std::ignore = BRepGraph_Compact::Perform(aGraph);

  // Re-acquire layer after compact (CopyLayersTo replaces old instance).
  aLayer = aGraph.LayerRegistry().Ensure<BRepGraph_LayerLock>();

  const BRepGraph_FaceSurfaceRepId aCompactedSurfaceRepId =
    aGraph.Topo().Faces().Definition(BRepGraph_FaceId::Start()).SurfaceRepId;
  ASSERT_TRUE(aCompactedSurfaceRepId.IsValid());
  EXPECT_TRUE(aLayer->HasOwner(BRepGraph_FaceId::Start()));
}
