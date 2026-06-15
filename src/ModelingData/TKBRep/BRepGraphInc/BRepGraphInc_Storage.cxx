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

#include <BRepGraphInc_Storage.hxx>

#include <NCollection_Array1.hxx>

#include <algorithm>
#include <mutex>
#include <shared_mutex>
#include <type_traits>

#include "BRepGraphInc_WireOrder.pxx"

namespace
{

template <typename T>
bool containsNodeIndex(const NCollection_DynamicArray<T>* theVec, const uint32_t theIndex)
{
  if (theVec == nullptr)
  {
    return false;
  }
  for (const T& anElem : *theVec)
  {
    if (anElem.Index == theIndex)
    {
      return true;
    }
  }
  return false;
}

template <typename StoreT, typename TypeIdT>
const typename StoreT::ValueType* findInStore(const StoreT& theStore, const TypeIdT theId)
{
  return theId.IsValid(theStore.Nb()) ? &theStore.Get(theId) : nullptr;
}

template <typename StoreT, typename TypeIdT>
typename StoreT::ValueType* changeFindInStore(StoreT& theStore, const TypeIdT theId)
{
  return theId.IsValid(theStore.Nb()) ? &theStore.Change(theId) : nullptr;
}

template <typename T>
void prepareDynamicArray(NCollection_DynamicArray<T>& theArray, const uint32_t theCount)
{
  if (theCount == 0)
  {
    theArray.Clear(true);
    return;
  }
  theArray.SetValue(static_cast<size_t>(theCount - 1), T());
}

template <typename T>
const NCollection_LinearVector<T>& emptyLinearVector()
{
  static const NCollection_LinearVector<T> THE_EMPTY_VECTOR;
  return THE_EMPTY_VECTOR;
}

template <typename IdT>
bool containsRelationId(const NCollection_LinearVector<IdT>& theIds, const IdT theId)
{
  for (const IdT& anId : theIds)
  {
    if (anId == theId)
    {
      return true;
    }
  }
  return false;
}

template <typename IdT>
void appendUniqueRelationId(NCollection_LinearVector<IdT>& theIds, const IdT theId)
{
  if (!containsRelationId(theIds, theId))
  {
    theIds.Append(theId);
  }
}

// Unchecked append for rebuild paths where duplicates are structurally impossible
// (each source ref is processed exactly once after vectors are cleared).
template <typename IdT>
void appendRelationIdDirect(NCollection_LinearVector<IdT>& theIds, const IdT theId)
{
  theIds.Append(theId);
}

template <typename IdT>
bool eraseRelationId(NCollection_LinearVector<IdT>& theIds, const IdT theId)
{
  for (size_t anIndex = 0; anIndex < theIds.Size(); ++anIndex)
  {
    if (theIds.Value(anIndex) == theId)
    {
      theIds.Erase(anIndex);
      return true;
    }
  }
  return false;
}

template <typename RelationT>
void prepareRelationTable(NCollection_DynamicArray<RelationT>& theTable, const uint32_t theCount)
{
  if (theCount == 0)
  {
    theTable.Clear(true);
    return;
  }
  if (theTable.Size() < theCount)
  {
    theTable.SetValue(static_cast<size_t>(theCount - 1), RelationT());
  }
}

bool wireHasFaceUse(const BRepGraphInc_Storage& theStorage,
                    const BRepGraph_WireId      theWire,
                    const BRepGraph_FaceId      theFace)
{
  if (!theWire.IsValid(theStorage.NbWires()) || !theFace.IsValid(theStorage.NbFaces()))
  {
    return false;
  }
  for (const BRepGraph_WireRefId& aWireRefId : theStorage.WireRelations(theWire).ParentWireRefIds)
  {
    if (aWireRefId.IsValid(theStorage.NbWireRefs()) && !theStorage.IsRemoved(aWireRefId)
        && theStorage.WireRef(aWireRefId).ParentFaceId == theFace)
    {
      return true;
    }
  }
  return false;
}

void clearWireFaceContext(BRepGraphInc_Storage&  theStorage,
                          const BRepGraph_WireId theWire,
                          const BRepGraph_FaceId theFace)
{
  if (!theWire.IsValid(theStorage.NbWires()) || !theFace.IsValid(theStorage.NbFaces()))
  {
    return;
  }

  for (const BRepGraph_CoEdgeId& aCoEdgeId : theStorage.WireRelations(theWire).CoEdgeIds)
  {
    if (!aCoEdgeId.IsValid(theStorage.NbCoEdges()) || theStorage.IsRemoved(aCoEdgeId)
        || theStorage.CoEdge(aCoEdgeId).FaceId != theFace)
    {
      continue;
    }

    BRepGraphInc::CoEdgeDef& aCoEdge = theStorage.ChangeCoEdge(aCoEdgeId);
    if (aCoEdge.Curve2DRepId.IsValid())
    {
      theStorage.MarkRemoved(aCoEdge.Curve2DRepId);
      aCoEdge.Curve2DRepId = BRepGraph_CoEdgeCurve2DRepId();
    }
    if (aCoEdge.Polygon2DRepId.IsValid())
    {
      theStorage.MarkRemoved(aCoEdge.Polygon2DRepId);
      aCoEdge.Polygon2DRepId = BRepGraph_CoEdgePolygon2DRepId();
    }
    if (aCoEdge.PolygonOnTriRepId.IsValid())
    {
      theStorage.MarkRemoved(aCoEdge.PolygonOnTriRepId);
      aCoEdge.PolygonOnTriRepId = BRepGraph_CoEdgePolygonOnTriRepId();
    }
    aCoEdge.FaceId = BRepGraph_FaceId();
  }
}

template <typename IdT, typename KeepT>
void pruneRelationIds(NCollection_LinearVector<IdT>& theIds, const KeepT& theKeep)
{
  for (size_t anIndex = 0; anIndex < theIds.Size();)
  {
    if (!theKeep(theIds.Value(anIndex)))
    {
      theIds.Erase(anIndex);
      continue;
    }
    ++anIndex;
  }
}

template <typename IdT>
void assignRelationIds(NCollection_LinearVector<IdT>& theTarget,
                       const NCollection_Array1<IdT>& theSource)
{
  theTarget.Clear(false);
  theTarget.Reserve(theSource.Size());
  for (const IdT& anId : theSource)
  {
    theTarget.Append(anId);
  }
}

template <typename DefStoreT>
void prepareDefStore(DefStoreT&     theStore,
                     const uint32_t theCount,
                     const occ::handle<NCollection_BaseAllocator>&)
{
  prepareDynamicArray(theStore.Entities, theCount);
  theStore.RemovedFlags.Resize(theCount);
  theStore.OwnedFlags.Resize(theCount);
  theStore.GuardFlags.Resize(theCount);
  theStore.HasCompoundParentFlags.Resize(theCount);
  theStore.HasOccurrenceParentFlags.Resize(theCount);
  theStore.NbActive = theCount;
}

template <typename RefStoreT>
void prepareRefStore(RefStoreT& theStore, const uint32_t theCount)
{
  prepareDynamicArray(theStore.Refs, theCount);
  theStore.RemovedFlags.Resize(theCount);
  theStore.OwnedFlags.Resize(theCount);
  theStore.GuardFlags.Resize(theCount);
  theStore.NbActive = theCount;
}

template <typename RepStoreT>
void prepareRepStore(RepStoreT& theStore, const uint32_t theCount)
{
  prepareDynamicArray(theStore.Uses, theCount);
  theStore.RemovedFlags.Resize(theCount);
  theStore.NbActive = theCount;
}

template <typename StoreT, typename TypeIdT>
void recountActiveStore(StoreT& theStore, TypeIdT)
{
  theStore.NbActive = 0;
  for (TypeIdT anId = TypeIdT::Start(); anId.IsValid(theStore.Nb()); ++anId)
  {
    if (!theStore.RemovedFlags.Test(anId.Index))
    {
      ++theStore.NbActive;
    }
  }
}

bool isNodeRemoved(const BRepGraphInc_Storage& theStorage, const BRepGraph_NodeId theNode)
{
  if (!theNode.IsValid())
  {
    return true;
  }
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      return theStorage.IsRemoved(BRepGraph_VertexId::FromNodeId(theNode));
    case BRepGraph_NodeId::Kind::Edge:
      return theStorage.IsRemoved(BRepGraph_EdgeId::FromNodeId(theNode));
    case BRepGraph_NodeId::Kind::CoEdge:
      return theStorage.IsRemoved(BRepGraph_CoEdgeId::FromNodeId(theNode));
    case BRepGraph_NodeId::Kind::Wire:
      return theStorage.IsRemoved(BRepGraph_WireId::FromNodeId(theNode));
    case BRepGraph_NodeId::Kind::Face:
      return theStorage.IsRemoved(BRepGraph_FaceId::FromNodeId(theNode));
    case BRepGraph_NodeId::Kind::Shell:
      return theStorage.IsRemoved(BRepGraph_ShellId::FromNodeId(theNode));
    case BRepGraph_NodeId::Kind::Solid:
      return theStorage.IsRemoved(BRepGraph_SolidId::FromNodeId(theNode));
    case BRepGraph_NodeId::Kind::Compound:
      return theStorage.IsRemoved(BRepGraph_CompoundId::FromNodeId(theNode));
    case BRepGraph_NodeId::Kind::CompSolid:
      return theStorage.IsRemoved(BRepGraph_CompSolidId::FromNodeId(theNode));
    case BRepGraph_NodeId::Kind::Product:
      return theStorage.IsRemoved(BRepGraph_ProductId::FromNodeId(theNode));
    case BRepGraph_NodeId::Kind::Occurrence:
      return theStorage.IsRemoved(BRepGraph_OccurrenceId::FromNodeId(theNode));
    default:
      return true;
  }
}

bool coEdgeOrientedVertices(const BRepGraphInc_Storage& theStorage,
                            const BRepGraph_CoEdgeId    theCoEdgeId,
                            BRepGraph_VertexId&         theStartVertex,
                            BRepGraph_VertexId&         theEndVertex)
{
  if (!theCoEdgeId.IsValid(theStorage.NbCoEdges()) || theStorage.IsRemoved(theCoEdgeId))
  {
    return false;
  }

  const BRepGraphInc::CoEdgeDef& aCoEdge = theStorage.CoEdge(theCoEdgeId);
  if (!aCoEdge.ChildEdgeId.IsValid(theStorage.NbEdges())
      || theStorage.IsRemoved(aCoEdge.ChildEdgeId))
  {
    return false;
  }

  const BRepGraphInc::EdgeDef& anEdge = theStorage.Edge(aCoEdge.ChildEdgeId);
  const BRepGraph_VertexRefId  aStartRef =
    (aCoEdge.Orientation == TopAbs_FORWARD) ? anEdge.StartVertexRefId : anEdge.EndVertexRefId;
  const BRepGraph_VertexRefId anEndRef =
    (aCoEdge.Orientation == TopAbs_FORWARD) ? anEdge.EndVertexRefId : anEdge.StartVertexRefId;
  if (!aStartRef.IsValid(theStorage.NbVertexRefs()) || theStorage.IsRemoved(aStartRef)
      || !anEndRef.IsValid(theStorage.NbVertexRefs()) || theStorage.IsRemoved(anEndRef))
  {
    return false;
  }

  theStartVertex = theStorage.VertexRef(aStartRef).ChildVertexId;
  theEndVertex   = theStorage.VertexRef(anEndRef).ChildVertexId;
  return theStartVertex.IsValid(theStorage.NbVertices()) && !theStorage.IsRemoved(theStartVertex)
         && theEndVertex.IsValid(theStorage.NbVertices()) && !theStorage.IsRemoved(theEndVertex);
}

bool coEdgeIdsAreUnique(const NCollection_LinearVector<BRepGraph_CoEdgeId>& theCoEdgeIds)
{
  return BRepGraphInc_WireOrder::CoEdgeIdsAreUnique(theCoEdgeIds.ToArray1());
}

} // namespace

//=================================================================================================

BRepGraphInc_Storage::BRepGraphInc_Storage()
    : myRootProductIds(8),
      myDeferredModified(128),
      myDeferredRefModified(128),
      myVertices(256, myAllocator),
      myEdges(256, myAllocator),
      myCoEdges(256, myAllocator),
      myWires(256, myAllocator),
      myFaces(256, myAllocator),
      myShells(256, myAllocator),
      mySolids(256, myAllocator),
      myCompounds(256, myAllocator),
      myCompSolids(256, myAllocator),
      myProducts(64, myAllocator),
      myOccurrences(256, myAllocator),
      myShellRefs(256, myAllocator),
      myFaceRefs(256, myAllocator),
      myWireRefs(256, myAllocator),
      myVertexRefs(256, myAllocator),
      mySolidRefs(256, myAllocator),
      myChildRefs(256, myAllocator),
      myOccurrenceRefs(256, myAllocator),
      myFaceRelations(256, myAllocator),
      myWireRelations(256, myAllocator),
      myEdgeRelations(256, myAllocator),
      myShellRelations(256, myAllocator),
      mySolidRelations(256, myAllocator),
      myCompoundRelations(256, myAllocator),
      myCompSolidRelations(256, myAllocator),
      myVertexRelations(256, myAllocator),
      myProductRelations(64, myAllocator),
      myOccurrenceRelations(64, myAllocator),
      myNodeToCompounds(1),
      myNodeToOccurrences(1),
      myEdgeCurves3D(256, myAllocator),
      myEdgePolygons3D(256, myAllocator),
      myCoEdgeCurves2D(256, myAllocator),
      myCoEdgePolygons2D(256, myAllocator),
      myCoEdgePolygonsOnTri(256, myAllocator),
      myFaceSurfaces(256, myAllocator),
      myFaceTriangulations(256, myAllocator),
      myUIDToNodeId(1),
      myRefUIDToRefId(1),
      myShapeToNodeId(1),
      myOriginalShapes(1)
{
  myAllocator->SetThreadSafe(true);
}

//=================================================================================================

BRepGraphInc_Storage::~BRepGraphInc_Storage()
{
  Clear();
}

//=================================================================================================

bool BRepGraphInc_Storage::IsEmpty() const
{
  return myVertices.Nb() == 0 && myEdges.Nb() == 0 && myCoEdges.Nb() == 0 && myWires.Nb() == 0
         && myFaces.Nb() == 0 && myShells.Nb() == 0 && mySolids.Nb() == 0 && myCompounds.Nb() == 0
         && myCompSolids.Nb() == 0 && myProducts.Nb() == 0 && myOccurrences.Nb() == 0;
}

//=================================================================================================

bool BRepGraphInc_Storage::HasAnyGuard() const
{
  return myVertices.GuardFlags.HasAnyBitSet() || myEdges.GuardFlags.HasAnyBitSet()
         || myCoEdges.GuardFlags.HasAnyBitSet() || myWires.GuardFlags.HasAnyBitSet()
         || myFaces.GuardFlags.HasAnyBitSet() || myShells.GuardFlags.HasAnyBitSet()
         || mySolids.GuardFlags.HasAnyBitSet() || myCompounds.GuardFlags.HasAnyBitSet()
         || myCompSolids.GuardFlags.HasAnyBitSet() || myProducts.GuardFlags.HasAnyBitSet()
         || myOccurrences.GuardFlags.HasAnyBitSet() || myVertexRefs.GuardFlags.HasAnyBitSet()
         || myShellRefs.GuardFlags.HasAnyBitSet() || myFaceRefs.GuardFlags.HasAnyBitSet()
         || myWireRefs.GuardFlags.HasAnyBitSet() || mySolidRefs.GuardFlags.HasAnyBitSet()
         || myChildRefs.GuardFlags.HasAnyBitSet() || myOccurrenceRefs.GuardFlags.HasAnyBitSet();
}

//=================================================================================================

uint32_t BRepGraphInc_Storage::NextNodeUIDCounter(const BRepGraph_NodeId::Kind theKind) const
{
  switch (theKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      return myVertices.NextUIDCounter.load(std::memory_order_relaxed);
    case BRepGraph_NodeId::Kind::Edge:
      return myEdges.NextUIDCounter.load(std::memory_order_relaxed);
    case BRepGraph_NodeId::Kind::CoEdge:
      return myCoEdges.NextUIDCounter.load(std::memory_order_relaxed);
    case BRepGraph_NodeId::Kind::Wire:
      return myWires.NextUIDCounter.load(std::memory_order_relaxed);
    case BRepGraph_NodeId::Kind::Face:
      return myFaces.NextUIDCounter.load(std::memory_order_relaxed);
    case BRepGraph_NodeId::Kind::Shell:
      return myShells.NextUIDCounter.load(std::memory_order_relaxed);
    case BRepGraph_NodeId::Kind::Solid:
      return mySolids.NextUIDCounter.load(std::memory_order_relaxed);
    case BRepGraph_NodeId::Kind::Compound:
      return myCompounds.NextUIDCounter.load(std::memory_order_relaxed);
    case BRepGraph_NodeId::Kind::CompSolid:
      return myCompSolids.NextUIDCounter.load(std::memory_order_relaxed);
    case BRepGraph_NodeId::Kind::Product:
      return myProducts.NextUIDCounter.load(std::memory_order_relaxed);
    case BRepGraph_NodeId::Kind::Occurrence:
      return myOccurrences.NextUIDCounter.load(std::memory_order_relaxed);
    default:
      break;
  }
  Standard_ASSERT_VOID(false, "NextNodeUIDCounter: unhandled Kind");
  return 0;
}

//=================================================================================================

void BRepGraphInc_Storage::SetNextNodeUIDCounter(const BRepGraph_NodeId::Kind theKind,
                                                 const uint32_t               theCounter)
{
  switch (theKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      myVertices.NextUIDCounter.store(theCounter, std::memory_order_relaxed);
      return;
    case BRepGraph_NodeId::Kind::Edge:
      myEdges.NextUIDCounter.store(theCounter, std::memory_order_relaxed);
      return;
    case BRepGraph_NodeId::Kind::CoEdge:
      myCoEdges.NextUIDCounter.store(theCounter, std::memory_order_relaxed);
      return;
    case BRepGraph_NodeId::Kind::Wire:
      myWires.NextUIDCounter.store(theCounter, std::memory_order_relaxed);
      return;
    case BRepGraph_NodeId::Kind::Face:
      myFaces.NextUIDCounter.store(theCounter, std::memory_order_relaxed);
      return;
    case BRepGraph_NodeId::Kind::Shell:
      myShells.NextUIDCounter.store(theCounter, std::memory_order_relaxed);
      return;
    case BRepGraph_NodeId::Kind::Solid:
      mySolids.NextUIDCounter.store(theCounter, std::memory_order_relaxed);
      return;
    case BRepGraph_NodeId::Kind::Compound:
      myCompounds.NextUIDCounter.store(theCounter, std::memory_order_relaxed);
      return;
    case BRepGraph_NodeId::Kind::CompSolid:
      myCompSolids.NextUIDCounter.store(theCounter, std::memory_order_relaxed);
      return;
    case BRepGraph_NodeId::Kind::Product:
      myProducts.NextUIDCounter.store(theCounter, std::memory_order_relaxed);
      return;
    case BRepGraph_NodeId::Kind::Occurrence:
      myOccurrences.NextUIDCounter.store(theCounter, std::memory_order_relaxed);
      return;
    default:
      break;
  }
  Standard_ASSERT_VOID(false, "SetNextNodeUIDCounter: unhandled Kind");
}

//=================================================================================================

uint32_t BRepGraphInc_Storage::NextRefUIDCounter(const BRepGraph_RefId::Kind theKind) const
{
  switch (theKind)
  {
    case BRepGraph_RefId::Kind::Shell:
      return myShellRefs.NextUIDCounter.load(std::memory_order_relaxed);
    case BRepGraph_RefId::Kind::Face:
      return myFaceRefs.NextUIDCounter.load(std::memory_order_relaxed);
    case BRepGraph_RefId::Kind::Wire:
      return myWireRefs.NextUIDCounter.load(std::memory_order_relaxed);
    case BRepGraph_RefId::Kind::Vertex:
      return myVertexRefs.NextUIDCounter.load(std::memory_order_relaxed);
    case BRepGraph_RefId::Kind::Solid:
      return mySolidRefs.NextUIDCounter.load(std::memory_order_relaxed);
    case BRepGraph_RefId::Kind::Child:
      return myChildRefs.NextUIDCounter.load(std::memory_order_relaxed);
    case BRepGraph_RefId::Kind::Occurrence:
      return myOccurrenceRefs.NextUIDCounter.load(std::memory_order_relaxed);
    default:
      break;
  }
  Standard_ASSERT_VOID(false, "NextRefUIDCounter: unhandled Kind");
  return 0;
}

//=================================================================================================

void BRepGraphInc_Storage::SetNextRefUIDCounter(const BRepGraph_RefId::Kind theKind,
                                                const uint32_t              theCounter)
{
  switch (theKind)
  {
    case BRepGraph_RefId::Kind::Shell:
      myShellRefs.NextUIDCounter.store(theCounter, std::memory_order_relaxed);
      return;
    case BRepGraph_RefId::Kind::Face:
      myFaceRefs.NextUIDCounter.store(theCounter, std::memory_order_relaxed);
      return;
    case BRepGraph_RefId::Kind::Wire:
      myWireRefs.NextUIDCounter.store(theCounter, std::memory_order_relaxed);
      return;
    case BRepGraph_RefId::Kind::Vertex:
      myVertexRefs.NextUIDCounter.store(theCounter, std::memory_order_relaxed);
      return;
    case BRepGraph_RefId::Kind::Solid:
      mySolidRefs.NextUIDCounter.store(theCounter, std::memory_order_relaxed);
      return;
    case BRepGraph_RefId::Kind::Child:
      myChildRefs.NextUIDCounter.store(theCounter, std::memory_order_relaxed);
      return;
    case BRepGraph_RefId::Kind::Occurrence:
      myOccurrenceRefs.NextUIDCounter.store(theCounter, std::memory_order_relaxed);
      return;
    default:
      break;
  }
  Standard_ASSERT_VOID(false, "SetNextRefUIDCounter: unhandled Kind");
}

//=================================================================================================

BRepGraph_UID BRepGraphInc_Storage::AllocateNodeUID(const BRepGraph_NodeId theNodeId)
{
  if (!theNodeId.IsValid())
  {
    return BRepGraph_UID();
  }

  // Get per-type counter from the appropriate store and advance it.
  const uint32_t aCounter = NextNodeUIDCounter(theNodeId.NodeKind);
  SetNextNodeUIDCounter(theNodeId.NodeKind, aCounter + 1);

  // Write counter into entity struct via per-kind mutable accessor.
  switch (theNodeId.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      ChangeVertex(BRepGraph_VertexId(theNodeId)).UID = aCounter;
      break;
    case BRepGraph_NodeId::Kind::Edge:
      ChangeEdge(BRepGraph_EdgeId(theNodeId)).UID = aCounter;
      break;
    case BRepGraph_NodeId::Kind::CoEdge:
      ChangeCoEdge(BRepGraph_CoEdgeId(theNodeId)).UID = aCounter;
      break;
    case BRepGraph_NodeId::Kind::Wire:
      ChangeWire(BRepGraph_WireId(theNodeId)).UID = aCounter;
      break;
    case BRepGraph_NodeId::Kind::Face:
      ChangeFace(BRepGraph_FaceId(theNodeId)).UID = aCounter;
      break;
    case BRepGraph_NodeId::Kind::Shell:
      ChangeShell(BRepGraph_ShellId(theNodeId)).UID = aCounter;
      break;
    case BRepGraph_NodeId::Kind::Solid:
      ChangeSolid(BRepGraph_SolidId(theNodeId)).UID = aCounter;
      break;
    case BRepGraph_NodeId::Kind::Compound:
      ChangeCompound(BRepGraph_CompoundId(theNodeId)).UID = aCounter;
      break;
    case BRepGraph_NodeId::Kind::CompSolid:
      ChangeCompSolid(BRepGraph_CompSolidId(theNodeId)).UID = aCounter;
      break;
    case BRepGraph_NodeId::Kind::Product:
      ChangeProduct(BRepGraph_ProductId(theNodeId)).UID = aCounter;
      break;
    case BRepGraph_NodeId::Kind::Occurrence:
      ChangeOccurrence(BRepGraph_OccurrenceId(theNodeId)).UID = aCounter;
      break;
    default:
      break;
  }

  // Build UID and bind reverse map.
  BRepGraph_UID aUID(theNodeId.NodeKind, aCounter);
  {
    std::unique_lock<std::shared_mutex> aLock(myUIDToNodeIdMutex);
    myUIDToNodeId.Bind(aUID, theNodeId);
  }
  return aUID;
}

//=================================================================================================

BRepGraph_RefUID BRepGraphInc_Storage::AllocateRefUID(const BRepGraph_RefId theRefId)
{
  if (!theRefId.IsValid())
  {
    return BRepGraph_RefUID();
  }

  BRepGraphInc::BaseRef* aRef = ChangeBaseRef(theRefId);
  if (aRef == nullptr)
  {
    return BRepGraph_RefUID();
  }

  // Get per-type counter from the appropriate store and advance it.
  const uint32_t aCounter = NextRefUIDCounter(theRefId.RefKind);
  SetNextRefUIDCounter(theRefId.RefKind, aCounter + 1);

  aRef->UID = aCounter;

  // Build RefUID and bind reverse map.
  BRepGraph_RefUID aUID(theRefId.RefKind, aCounter);
  {
    std::unique_lock<std::shared_mutex> aLock(myRefUIDToRefIdMutex);
    myRefUIDToRefId.Bind(aUID, theRefId);
  }
  return aUID;
}

//=================================================================================================

BRepGraph_NodeId BRepGraphInc_Storage::FindNodeIdByUID(const BRepGraph_UID& theUID) const
{
  if (!theUID.IsValid())
  {
    return BRepGraph_NodeId();
  }

  EnsureUIDReverseIndex();

  std::shared_lock<std::shared_mutex> aReadLock(myUIDToNodeIdMutex);
  const BRepGraph_NodeId*             aNodeId = myUIDToNodeId.Seek(theUID);
  if (aNodeId == nullptr)
  {
    return BRepGraph_NodeId();
  }

  const auto aCheck = [this](const auto theTypedId) -> bool {
    using TypeId = std::remove_cv_t<decltype(theTypedId)>;

    if constexpr (std::is_same_v<TypeId, BRepGraph_VertexId>)
    {
      return theTypedId.IsValid(NbVertices()) && !IsRemoved(theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_EdgeId>)
    {
      return theTypedId.IsValid(NbEdges()) && !IsRemoved(theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_CoEdgeId>)
    {
      return theTypedId.IsValid(NbCoEdges()) && !IsRemoved(theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_WireId>)
    {
      return theTypedId.IsValid(NbWires()) && !IsRemoved(theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_FaceId>)
    {
      return theTypedId.IsValid(NbFaces()) && !IsRemoved(theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_ShellId>)
    {
      return theTypedId.IsValid(NbShells()) && !IsRemoved(theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_SolidId>)
    {
      return theTypedId.IsValid(NbSolids()) && !IsRemoved(theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_CompoundId>)
    {
      return theTypedId.IsValid(NbCompounds()) && !IsRemoved(theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_CompSolidId>)
    {
      return theTypedId.IsValid(NbCompSolids()) && !IsRemoved(theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_ProductId>)
    {
      return theTypedId.IsValid(NbProducts()) && !IsRemoved(theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_OccurrenceId>)
    {
      return theTypedId.IsValid(NbOccurrences()) && !IsRemoved(theTypedId);
    }
    else
    {
      Standard_ASSERT_RETURN(false, "FindNodeIdByUID: unsupported node id type", false);
    }
  };

  return BRepGraph_NodeId::Visit(*aNodeId, aCheck) ? *aNodeId : BRepGraph_NodeId();
}

//=================================================================================================

BRepGraph_RefId BRepGraphInc_Storage::FindRefIdByUID(const BRepGraph_RefUID& theUID) const
{
  if (!theUID.IsValid())
  {
    return BRepGraph_RefId();
  }

  EnsureRefUIDReverseIndex();

  std::shared_lock<std::shared_mutex> aReadLock(myRefUIDToRefIdMutex);
  const BRepGraph_RefId*              aRefId = myRefUIDToRefId.Seek(theUID);
  if (aRefId == nullptr)
  {
    return BRepGraph_RefId();
  }

  const auto aRefCheck = [this](const auto theTypedId) -> bool { return !IsRemoved(theTypedId); };
  return BRepGraph_RefId::Visit(*aRefId, aRefCheck) ? *aRefId : BRepGraph_RefId();
}

//=================================================================================================

void BRepGraphInc_Storage::ClearStorageForReuse()
{
  myRootProductIds.Clear(true);
  myVertices.Clear(true);
  myEdges.Clear(true);
  myCoEdges.Clear(true);
  myWires.Clear(true);
  myFaces.Clear(true);
  myShells.Clear(true);
  mySolids.Clear(true);
  myCompounds.Clear(true);
  myCompSolids.Clear(true);
  myProducts.Clear(true);
  myOccurrences.Clear(true);
  myShellRefs.Clear(true);
  myFaceRefs.Clear(true);
  myWireRefs.Clear(true);
  myVertexRefs.Clear(true);
  mySolidRefs.Clear(true);
  myChildRefs.Clear(true);
  myOccurrenceRefs.Clear(true);
  myFaceRelations.Clear(true);
  myWireRelations.Clear(true);
  myEdgeRelations.Clear(true);
  myShellRelations.Clear(true);
  mySolidRelations.Clear(true);
  myCompoundRelations.Clear(true);
  myCompSolidRelations.Clear(true);
  myVertexRelations.Clear(true);
  myProductRelations.Clear(true);
  myOccurrenceRelations.Clear(true);
  myEdgeCurves3D.Clear(true);
  myEdgePolygons3D.Clear(true);
  myCoEdgeCurves2D.Clear(true);
  myCoEdgePolygons2D.Clear(true);
  myCoEdgePolygonsOnTri.Clear(true);
  myFaceSurfaces.Clear(true);
  myFaceTriangulations.Clear(true);
  myNodeToCompounds.Clear();
  myNodeToOccurrences.Clear();
  myShapeToNodeId.Clear();
  myOriginalShapes.Clear();
  myAllocator->Reset(false);
}

//=================================================================================================

void BRepGraphInc_Storage::ClearRelations()
{
  myFaceRelations.Clear(true);
  myWireRelations.Clear(true);
  myEdgeRelations.Clear(true);
  myShellRelations.Clear(true);
  mySolidRelations.Clear(true);
  myCompoundRelations.Clear(true);
  myCompSolidRelations.Clear(true);
  myVertexRelations.Clear(true);
  myProductRelations.Clear(true);
  myOccurrenceRelations.Clear(true);
  myNodeToCompounds.Clear();
  myNodeToOccurrences.Clear();
}

//=================================================================================================

const NCollection_LinearVector<BRepGraph_ChildRefId>& BRepGraphInc_Storage::CompoundRefsOfNode(
  const BRepGraph_NodeId theNode) const
{
  if (const NCollection_LinearVector<BRepGraph_ChildRefId>* aRefs = myNodeToCompounds.Seek(theNode))
  {
    return *aRefs;
  }
  return emptyLinearVector<BRepGraph_ChildRefId>();
}

//=================================================================================================

const NCollection_LinearVector<BRepGraph_OccurrenceRefId>& BRepGraphInc_Storage::
  OccurrenceRefsOfNode(const BRepGraph_NodeId theNode) const
{
  if (const NCollection_LinearVector<BRepGraph_OccurrenceRefId>* aRefs =
        myNodeToOccurrences.Seek(theNode))
  {
    return *aRefs;
  }
  return emptyLinearVector<BRepGraph_OccurrenceRefId>();
}

//=================================================================================================

bool BRepGraphInc_Storage::HasCompoundParent(const BRepGraph_NodeId theNode) const
{
  if (!theNode.IsValid())
  {
    return false;
  }
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      return HasCompoundParentTyped(BRepGraph_VertexId(theNode));
    case BRepGraph_NodeId::Kind::Edge:
      return HasCompoundParentTyped(BRepGraph_EdgeId(theNode));
    case BRepGraph_NodeId::Kind::CoEdge:
      return HasCompoundParentTyped(BRepGraph_CoEdgeId(theNode));
    case BRepGraph_NodeId::Kind::Wire:
      return HasCompoundParentTyped(BRepGraph_WireId(theNode));
    case BRepGraph_NodeId::Kind::Face:
      return HasCompoundParentTyped(BRepGraph_FaceId(theNode));
    case BRepGraph_NodeId::Kind::Shell:
      return HasCompoundParentTyped(BRepGraph_ShellId(theNode));
    case BRepGraph_NodeId::Kind::Solid:
      return HasCompoundParentTyped(BRepGraph_SolidId(theNode));
    case BRepGraph_NodeId::Kind::Compound:
      return HasCompoundParentTyped(BRepGraph_CompoundId(theNode));
    case BRepGraph_NodeId::Kind::CompSolid:
      return HasCompoundParentTyped(BRepGraph_CompSolidId(theNode));
    case BRepGraph_NodeId::Kind::Product:
      return HasCompoundParentTyped(BRepGraph_ProductId(theNode));
    case BRepGraph_NodeId::Kind::Occurrence:
      return HasCompoundParentTyped(BRepGraph_OccurrenceId(theNode));
  }
  return false;
}

//=================================================================================================

bool BRepGraphInc_Storage::HasOccurrenceParent(const BRepGraph_NodeId theNode) const
{
  if (!theNode.IsValid())
  {
    return false;
  }
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      return HasOccurrenceParentTyped(BRepGraph_VertexId(theNode));
    case BRepGraph_NodeId::Kind::Edge:
      return HasOccurrenceParentTyped(BRepGraph_EdgeId(theNode));
    case BRepGraph_NodeId::Kind::CoEdge:
      return HasOccurrenceParentTyped(BRepGraph_CoEdgeId(theNode));
    case BRepGraph_NodeId::Kind::Wire:
      return HasOccurrenceParentTyped(BRepGraph_WireId(theNode));
    case BRepGraph_NodeId::Kind::Face:
      return HasOccurrenceParentTyped(BRepGraph_FaceId(theNode));
    case BRepGraph_NodeId::Kind::Shell:
      return HasOccurrenceParentTyped(BRepGraph_ShellId(theNode));
    case BRepGraph_NodeId::Kind::Solid:
      return HasOccurrenceParentTyped(BRepGraph_SolidId(theNode));
    case BRepGraph_NodeId::Kind::Compound:
      return HasOccurrenceParentTyped(BRepGraph_CompoundId(theNode));
    case BRepGraph_NodeId::Kind::CompSolid:
      return HasOccurrenceParentTyped(BRepGraph_CompSolidId(theNode));
    case BRepGraph_NodeId::Kind::Product:
      return HasOccurrenceParentTyped(BRepGraph_ProductId(theNode));
    case BRepGraph_NodeId::Kind::Occurrence:
      return HasOccurrenceParentTyped(BRepGraph_OccurrenceId(theNode));
  }
  return false;
}

//=================================================================================================

void BRepGraphInc_Storage::SetHasCompoundParent(const BRepGraph_NodeId theNode, const bool theVal)
{
  if (!theNode.IsValid())
  {
    return;
  }
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      SetHasCompoundParentTyped(BRepGraph_VertexId(theNode), theVal);
      return;
    case BRepGraph_NodeId::Kind::Edge:
      SetHasCompoundParentTyped(BRepGraph_EdgeId(theNode), theVal);
      return;
    case BRepGraph_NodeId::Kind::CoEdge:
      SetHasCompoundParentTyped(BRepGraph_CoEdgeId(theNode), theVal);
      return;
    case BRepGraph_NodeId::Kind::Wire:
      SetHasCompoundParentTyped(BRepGraph_WireId(theNode), theVal);
      return;
    case BRepGraph_NodeId::Kind::Face:
      SetHasCompoundParentTyped(BRepGraph_FaceId(theNode), theVal);
      return;
    case BRepGraph_NodeId::Kind::Shell:
      SetHasCompoundParentTyped(BRepGraph_ShellId(theNode), theVal);
      return;
    case BRepGraph_NodeId::Kind::Solid:
      SetHasCompoundParentTyped(BRepGraph_SolidId(theNode), theVal);
      return;
    case BRepGraph_NodeId::Kind::Compound:
      SetHasCompoundParentTyped(BRepGraph_CompoundId(theNode), theVal);
      return;
    case BRepGraph_NodeId::Kind::CompSolid:
      SetHasCompoundParentTyped(BRepGraph_CompSolidId(theNode), theVal);
      return;
    case BRepGraph_NodeId::Kind::Product:
      SetHasCompoundParentTyped(BRepGraph_ProductId(theNode), theVal);
      return;
    case BRepGraph_NodeId::Kind::Occurrence:
      SetHasCompoundParentTyped(BRepGraph_OccurrenceId(theNode), theVal);
      return;
  }
}

//=================================================================================================

void BRepGraphInc_Storage::SetHasOccurrenceParent(const BRepGraph_NodeId theNode, const bool theVal)
{
  if (!theNode.IsValid())
  {
    return;
  }
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      SetHasOccurrenceParentTyped(BRepGraph_VertexId(theNode), theVal);
      return;
    case BRepGraph_NodeId::Kind::Edge:
      SetHasOccurrenceParentTyped(BRepGraph_EdgeId(theNode), theVal);
      return;
    case BRepGraph_NodeId::Kind::CoEdge:
      SetHasOccurrenceParentTyped(BRepGraph_CoEdgeId(theNode), theVal);
      return;
    case BRepGraph_NodeId::Kind::Wire:
      SetHasOccurrenceParentTyped(BRepGraph_WireId(theNode), theVal);
      return;
    case BRepGraph_NodeId::Kind::Face:
      SetHasOccurrenceParentTyped(BRepGraph_FaceId(theNode), theVal);
      return;
    case BRepGraph_NodeId::Kind::Shell:
      SetHasOccurrenceParentTyped(BRepGraph_ShellId(theNode), theVal);
      return;
    case BRepGraph_NodeId::Kind::Solid:
      SetHasOccurrenceParentTyped(BRepGraph_SolidId(theNode), theVal);
      return;
    case BRepGraph_NodeId::Kind::Compound:
      SetHasOccurrenceParentTyped(BRepGraph_CompoundId(theNode), theVal);
      return;
    case BRepGraph_NodeId::Kind::CompSolid:
      SetHasOccurrenceParentTyped(BRepGraph_CompSolidId(theNode), theVal);
      return;
    case BRepGraph_NodeId::Kind::Product:
      SetHasOccurrenceParentTyped(BRepGraph_ProductId(theNode), theVal);
      return;
    case BRepGraph_NodeId::Kind::Occurrence:
      SetHasOccurrenceParentTyped(BRepGraph_OccurrenceId(theNode), theVal);
      return;
  }
}

//=================================================================================================

NCollection_LinearVector<BRepGraph_ChildRefId>& BRepGraphInc_Storage::
  ChangeCompoundRefsOfNodeInternal(const BRepGraph_NodeId theNode)
{
  return myNodeToCompounds.TryBound(theNode, NCollection_LinearVector<BRepGraph_ChildRefId>());
}

//=================================================================================================

NCollection_LinearVector<BRepGraph_OccurrenceRefId>& BRepGraphInc_Storage::
  ChangeOccurrenceRefsOfNodeInternal(const BRepGraph_NodeId theNode)
{
  return myNodeToOccurrences.TryBound(theNode,
                                      NCollection_LinearVector<BRepGraph_OccurrenceRefId>());
}

//=================================================================================================

void BRepGraphInc_Storage::RebuildDerivedRelations()
{
  rebuildDerivedRelationsInternal(true);
}

//=================================================================================================

void BRepGraphInc_Storage::RebuildDerivedRelationsPreservingActiveCounts()
{
  rebuildDerivedRelationsInternal(false);
}

//=================================================================================================

void BRepGraphInc_Storage::CopyRemovedFlagsFrom(const BRepGraphInc_Storage& theSource)
{
  myVertices.RemovedFlags    = theSource.myVertices.RemovedFlags;
  myEdges.RemovedFlags       = theSource.myEdges.RemovedFlags;
  myCoEdges.RemovedFlags     = theSource.myCoEdges.RemovedFlags;
  myWires.RemovedFlags       = theSource.myWires.RemovedFlags;
  myFaces.RemovedFlags       = theSource.myFaces.RemovedFlags;
  myShells.RemovedFlags      = theSource.myShells.RemovedFlags;
  mySolids.RemovedFlags      = theSource.mySolids.RemovedFlags;
  myCompounds.RemovedFlags   = theSource.myCompounds.RemovedFlags;
  myCompSolids.RemovedFlags  = theSource.myCompSolids.RemovedFlags;
  myProducts.RemovedFlags    = theSource.myProducts.RemovedFlags;
  myOccurrences.RemovedFlags = theSource.myOccurrences.RemovedFlags;

  myShellRefs.RemovedFlags      = theSource.myShellRefs.RemovedFlags;
  myFaceRefs.RemovedFlags       = theSource.myFaceRefs.RemovedFlags;
  myWireRefs.RemovedFlags       = theSource.myWireRefs.RemovedFlags;
  myVertexRefs.RemovedFlags     = theSource.myVertexRefs.RemovedFlags;
  mySolidRefs.RemovedFlags      = theSource.mySolidRefs.RemovedFlags;
  myChildRefs.RemovedFlags      = theSource.myChildRefs.RemovedFlags;
  myOccurrenceRefs.RemovedFlags = theSource.myOccurrenceRefs.RemovedFlags;
}

//=================================================================================================

void BRepGraphInc_Storage::rebuildDerivedRelationsInternal(const bool theRecountActiveCounts)
{
  prepareRelationTable(myVertexRelations, NbVertices());
  prepareRelationTable(myEdgeRelations, NbEdges());
  prepareRelationTable(myWireRelations, NbWires());
  prepareRelationTable(myFaceRelations, NbFaces());
  prepareRelationTable(myShellRelations, NbShells());
  prepareRelationTable(mySolidRelations, NbSolids());
  prepareRelationTable(myCompoundRelations, NbCompounds());
  prepareRelationTable(myCompSolidRelations, NbCompSolids());
  prepareRelationTable(myProductRelations, NbProducts());
  prepareRelationTable(myOccurrenceRelations, NbOccurrences());

  for (BRepGraph_VertexId aVertexId(0); aVertexId.IsValid(NbVertices()); ++aVertexId)
  {
    ChangeVertexRelationsInternal(aVertexId).EdgeIds.Clear();
  }
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(NbEdges()); ++anEdgeId)
  {
    BRepGraphInc::EdgeRelations& anEdgeRel = ChangeEdgeRelationsInternal(anEdgeId);
    anEdgeRel.CoEdgeIds.Clear();
  }
  for (BRepGraph_WireId aWireId(0); aWireId.IsValid(NbWires()); ++aWireId)
  {
    ChangeWireRelationsInternal(aWireId).ParentWireRefIds.Clear();
  }
  for (BRepGraph_FaceId aFaceId(0); aFaceId.IsValid(NbFaces()); ++aFaceId)
  {
    ChangeFaceRelationsInternal(aFaceId).ParentFaceRefIds.Clear();
  }
  for (BRepGraph_ShellId aShellId(0); aShellId.IsValid(NbShells()); ++aShellId)
  {
    ChangeShellRelationsInternal(aShellId).ParentShellRefIds.Clear();
  }
  for (BRepGraph_SolidId aSolidId(0); aSolidId.IsValid(NbSolids()); ++aSolidId)
  {
    ChangeSolidRelationsInternal(aSolidId).ParentSolidRefIds.Clear();
  }
  for (BRepGraph_OccurrenceId anOccurrenceId(0); anOccurrenceId.IsValid(NbOccurrences());
       ++anOccurrenceId)
  {
    ChangeOccurrenceRelationsInternal(anOccurrenceId).ParentOccurrenceRefIds.Clear();
  }
  myNodeToCompounds.Clear();
  myNodeToOccurrences.Clear();

  // Clear the compound/occurrence parent bitsets for all node kinds.
  myVertices.HasCompoundParentFlags.ClearAll();
  myVertices.HasOccurrenceParentFlags.ClearAll();
  myEdges.HasCompoundParentFlags.ClearAll();
  myEdges.HasOccurrenceParentFlags.ClearAll();
  myCoEdges.HasCompoundParentFlags.ClearAll();
  myCoEdges.HasOccurrenceParentFlags.ClearAll();
  myWires.HasCompoundParentFlags.ClearAll();
  myWires.HasOccurrenceParentFlags.ClearAll();
  myFaces.HasCompoundParentFlags.ClearAll();
  myFaces.HasOccurrenceParentFlags.ClearAll();
  myShells.HasCompoundParentFlags.ClearAll();
  myShells.HasOccurrenceParentFlags.ClearAll();
  mySolids.HasCompoundParentFlags.ClearAll();
  mySolids.HasOccurrenceParentFlags.ClearAll();
  myCompounds.HasCompoundParentFlags.ClearAll();
  myCompounds.HasOccurrenceParentFlags.ClearAll();
  myCompSolids.HasCompoundParentFlags.ClearAll();
  myCompSolids.HasOccurrenceParentFlags.ClearAll();
  myProducts.HasCompoundParentFlags.ClearAll();
  myProducts.HasOccurrenceParentFlags.ClearAll();
  myOccurrences.HasCompoundParentFlags.ClearAll();
  myOccurrences.HasOccurrenceParentFlags.ClearAll();

  for (BRepGraph_WireId aWireId(0); aWireId.IsValid(NbWires()); ++aWireId)
  {
    pruneRelationIds(ChangeWireRelationsInternal(aWireId).CoEdgeIds,
                     [&](const BRepGraph_CoEdgeId theCoEdgeId) {
                       return !IsRemoved(aWireId) && theCoEdgeId.IsValid(NbCoEdges())
                              && !IsRemoved(theCoEdgeId)
                              && CoEdge(theCoEdgeId).ParentWireId == aWireId;
                     });
  }
  for (BRepGraph_FaceId aFaceId(0); aFaceId.IsValid(NbFaces()); ++aFaceId)
  {
    pruneRelationIds(ChangeFaceRelationsInternal(aFaceId).WireRefIds,
                     [&](const BRepGraph_WireRefId theRefId) {
                       return !IsRemoved(aFaceId) && theRefId.IsValid(NbWireRefs())
                              && !IsRemoved(theRefId) && WireRef(theRefId).ParentFaceId == aFaceId;
                     });
  }
  for (BRepGraph_ShellId aShellId(0); aShellId.IsValid(NbShells()); ++aShellId)
  {
    pruneRelationIds(ChangeShellRelationsInternal(aShellId).FaceRefIds,
                     [&](const BRepGraph_FaceRefId theRefId) {
                       return !IsRemoved(aShellId) && theRefId.IsValid(NbFaceRefs())
                              && !IsRemoved(theRefId)
                              && FaceRef(theRefId).ParentShellId == aShellId;
                     });
  }
  for (BRepGraph_SolidId aSolidId(0); aSolidId.IsValid(NbSolids()); ++aSolidId)
  {
    pruneRelationIds(ChangeSolidRelationsInternal(aSolidId).ShellRefIds,
                     [&](const BRepGraph_ShellRefId theRefId) {
                       return !IsRemoved(aSolidId) && theRefId.IsValid(NbShellRefs())
                              && !IsRemoved(theRefId)
                              && ShellRef(theRefId).ParentSolidId == aSolidId;
                     });
  }
  for (BRepGraph_CompSolidId aCompSolidId(0); aCompSolidId.IsValid(NbCompSolids()); ++aCompSolidId)
  {
    pruneRelationIds(ChangeCompSolidRelationsInternal(aCompSolidId).SolidRefIds,
                     [&](const BRepGraph_SolidRefId theRefId) {
                       return !IsRemoved(aCompSolidId) && theRefId.IsValid(NbSolidRefs())
                              && !IsRemoved(theRefId)
                              && SolidRef(theRefId).ParentCompSolidId == aCompSolidId;
                     });
  }
  for (BRepGraph_CompoundId aCompoundId(0); aCompoundId.IsValid(NbCompounds()); ++aCompoundId)
  {
    pruneRelationIds(ChangeCompoundRelationsInternal(aCompoundId).ChildRefIds,
                     [&](const BRepGraph_ChildRefId theRefId) {
                       return !IsRemoved(aCompoundId) && theRefId.IsValid(NbChildRefs())
                              && !IsRemoved(theRefId)
                              && ChildRef(theRefId).ParentCompoundId == aCompoundId;
                     });
  }
  for (BRepGraph_ProductId aProductId(0); aProductId.IsValid(NbProducts()); ++aProductId)
  {
    pruneRelationIds(ChangeProductRelationsInternal(aProductId).OccurrenceRefIds,
                     [&](const BRepGraph_OccurrenceRefId theRefId) {
                       return !IsRemoved(aProductId) && theRefId.IsValid(NbOccurrenceRefs())
                              && !IsRemoved(theRefId)
                              && OccurrenceRef(theRefId).ParentProductId == aProductId;
                     });
  }

  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(NbEdges()); ++anEdgeId)
  {
    if (IsRemoved(anEdgeId))
    {
      continue;
    }
    const BRepGraphInc::EdgeDef& anEdge      = Edge(anEdgeId);
    const BRepGraph_VertexRefId  aStartRefId = anEdge.StartVertexRefId;
    if (aStartRefId.IsValid(NbVertexRefs()) && !IsRemoved(aStartRefId))
    {
      const BRepGraph_VertexId aVertexId = VertexRef(aStartRefId).ChildVertexId;
      if (aVertexId.IsValid(NbVertices()) && !IsRemoved(aVertexId))
      {
        appendRelationIdDirect(ChangeVertexRelationsInternal(aVertexId).EdgeIds, anEdgeId);
      }
    }

    const BRepGraph_VertexRefId anEndRefId = anEdge.EndVertexRefId;
    if (anEndRefId.IsValid(NbVertexRefs()) && !IsRemoved(anEndRefId))
    {
      const BRepGraph_VertexId aVertexId = VertexRef(anEndRefId).ChildVertexId;
      if (aVertexId.IsValid(NbVertices()) && !IsRemoved(aVertexId))
      {
        appendRelationIdDirect(ChangeVertexRelationsInternal(aVertexId).EdgeIds, anEdgeId);
      }
    }
  }

  for (BRepGraph_CoEdgeId aCoEdgeId(0); aCoEdgeId.IsValid(NbCoEdges()); ++aCoEdgeId)
  {
    if (IsRemoved(aCoEdgeId))
    {
      continue;
    }
    const BRepGraphInc::CoEdgeDef& aCoEdge = CoEdge(aCoEdgeId);
    if (aCoEdge.ChildEdgeId.IsValid(NbEdges()) && !IsRemoved(aCoEdge.ChildEdgeId))
    {
      BRepGraphInc::EdgeRelations& anEdgeRel = ChangeEdgeRelationsInternal(aCoEdge.ChildEdgeId);
      appendRelationIdDirect(anEdgeRel.CoEdgeIds, aCoEdgeId);
    }
  }

  for (BRepGraph_FaceId aFaceId(0); aFaceId.IsValid(NbFaces()); ++aFaceId)
  {
    if (IsRemoved(aFaceId))
    {
      continue;
    }
    const BRepGraphInc::FaceRelations& aFaceRel = FaceRelations(aFaceId);
    for (const BRepGraph_WireRefId& aWireRefId : aFaceRel.WireRefIds)
    {
      if (!aWireRefId.IsValid(NbWireRefs()) || IsRemoved(aWireRefId))
      {
        continue;
      }
      const BRepGraph_WireId aWireId = WireRef(aWireRefId).ChildWireId;
      if (!aWireId.IsValid(NbWires()) || IsRemoved(aWireId))
      {
        continue;
      }
      appendRelationIdDirect(ChangeWireRelationsInternal(aWireId).ParentWireRefIds, aWireRefId);
    }
  }

  for (BRepGraph_ShellId aShellId(0); aShellId.IsValid(NbShells()); ++aShellId)
  {
    if (IsRemoved(aShellId))
    {
      continue;
    }
    const BRepGraphInc::ShellRelations& aShellRel = ShellRelations(aShellId);
    for (const BRepGraph_FaceRefId& aFaceRefId : aShellRel.FaceRefIds)
    {
      if (!aFaceRefId.IsValid(NbFaceRefs()) || IsRemoved(aFaceRefId))
      {
        continue;
      }
      const BRepGraph_FaceId aFaceId = FaceRef(aFaceRefId).ChildFaceId;
      if (!aFaceId.IsValid(NbFaces()) || IsRemoved(aFaceId))
      {
        continue;
      }
      appendRelationIdDirect(ChangeFaceRelationsInternal(aFaceId).ParentFaceRefIds, aFaceRefId);
    }
  }

  for (BRepGraph_SolidId aSolidId(0); aSolidId.IsValid(NbSolids()); ++aSolidId)
  {
    if (IsRemoved(aSolidId))
    {
      continue;
    }
    const BRepGraphInc::SolidRelations& aSolidRel = SolidRelations(aSolidId);
    for (const BRepGraph_ShellRefId& aShellRefId : aSolidRel.ShellRefIds)
    {
      if (!aShellRefId.IsValid(NbShellRefs()) || IsRemoved(aShellRefId))
      {
        continue;
      }
      const BRepGraph_ShellId aShellId = ShellRef(aShellRefId).ChildShellId;
      if (!aShellId.IsValid(NbShells()) || IsRemoved(aShellId))
      {
        continue;
      }
      appendRelationIdDirect(ChangeShellRelationsInternal(aShellId).ParentShellRefIds, aShellRefId);
    }
  }

  for (BRepGraph_CompoundId aCompoundId(0); aCompoundId.IsValid(NbCompounds()); ++aCompoundId)
  {
    if (IsRemoved(aCompoundId))
    {
      continue;
    }
    const BRepGraphInc::CompoundRelations& aCompoundRel = CompoundRelations(aCompoundId);
    for (const BRepGraph_ChildRefId& aChildRefId : aCompoundRel.ChildRefIds)
    {
      if (!aChildRefId.IsValid(NbChildRefs()) || IsRemoved(aChildRefId))
      {
        continue;
      }
      const BRepGraph_NodeId aChildNode = ChildRef(aChildRefId).ChildNodeId;
      if (aChildNode.IsValid() && !isNodeRemoved(*this, aChildNode))
      {
        appendRelationIdDirect(ChangeCompoundRefsOfNodeInternal(aChildNode), aChildRefId);
      }
    }
  }

  for (BRepGraph_CompSolidId aCompSolidId(0); aCompSolidId.IsValid(NbCompSolids()); ++aCompSolidId)
  {
    if (IsRemoved(aCompSolidId))
    {
      continue;
    }
    const BRepGraphInc::CompSolidRelations& aCompSolidRel = CompSolidRelations(aCompSolidId);
    for (const BRepGraph_SolidRefId& aSolidRefId : aCompSolidRel.SolidRefIds)
    {
      if (!aSolidRefId.IsValid(NbSolidRefs()) || IsRemoved(aSolidRefId))
      {
        continue;
      }
      const BRepGraph_SolidId aSolidId = SolidRef(aSolidRefId).ChildSolidId;
      if (!aSolidId.IsValid(NbSolids()) || IsRemoved(aSolidId))
      {
        continue;
      }
      appendRelationIdDirect(ChangeSolidRelationsInternal(aSolidId).ParentSolidRefIds, aSolidRefId);
    }
  }

  for (BRepGraph_ProductId aProductId(0); aProductId.IsValid(NbProducts()); ++aProductId)
  {
    if (IsRemoved(aProductId))
    {
      continue;
    }
    const BRepGraphInc::ProductRelations& aProductRel = ProductRelations(aProductId);
    for (const BRepGraph_OccurrenceRefId& anOccurrenceRefId : aProductRel.OccurrenceRefIds)
    {
      if (!anOccurrenceRefId.IsValid(NbOccurrenceRefs()) || IsRemoved(anOccurrenceRefId))
      {
        continue;
      }
      const BRepGraph_OccurrenceId anOccurrenceId =
        OccurrenceRef(anOccurrenceRefId).ChildOccurrenceId;
      if (!anOccurrenceId.IsValid(NbOccurrences()) || IsRemoved(anOccurrenceId))
      {
        continue;
      }
      appendRelationIdDirect(
        ChangeOccurrenceRelationsInternal(anOccurrenceId).ParentOccurrenceRefIds,
        anOccurrenceRefId);
      const BRepGraph_NodeId aChildNode = Occurrence(anOccurrenceId).ChildNodeId;
      if (aChildNode.IsValid() && !isNodeRemoved(*this, aChildNode))
      {
        appendRelationIdDirect(ChangeOccurrenceRefsOfNodeInternal(aChildNode), anOccurrenceRefId);
      }
    }
  }

  // Rebuild compound/occurrence parent bitsets from the populated maps.
  for (NCollection_DataMap<BRepGraph_NodeId,
                           NCollection_LinearVector<BRepGraph_ChildRefId>>::Iterator
         anIt(myNodeToCompounds);
       anIt.More();
       anIt.Next())
  {
    if (!anIt.Value().IsEmpty())
    {
      SetHasCompoundParent(anIt.Key(), true);
    }
  }
  for (NCollection_DataMap<BRepGraph_NodeId,
                           NCollection_LinearVector<BRepGraph_OccurrenceRefId>>::Iterator
         anIt(myNodeToOccurrences);
       anIt.More();
       anIt.Next())
  {
    if (!anIt.Value().IsEmpty())
    {
      SetHasOccurrenceParent(anIt.Key(), true);
    }
  }

  if (theRecountActiveCounts)
  {
    RecountActiveCounts();
  }
}

//=================================================================================================

void BRepGraphInc_Storage::CopyDerivedRelationsFrom(const BRepGraphInc_Storage& theSource)
{
  prepareRelationTable(myVertexRelations, NbVertices());
  prepareRelationTable(myEdgeRelations, NbEdges());
  prepareRelationTable(myWireRelations, NbWires());
  prepareRelationTable(myFaceRelations, NbFaces());
  prepareRelationTable(myShellRelations, NbShells());
  prepareRelationTable(mySolidRelations, NbSolids());
  prepareRelationTable(myCompoundRelations, NbCompounds());
  prepareRelationTable(myCompSolidRelations, NbCompSolids());
  prepareRelationTable(myProductRelations, NbProducts());
  prepareRelationTable(myOccurrenceRelations, NbOccurrences());

  for (BRepGraph_VertexId aV(0); aV.IsValid(NbVertices()); ++aV)
  {
    if (aV.IsValid(theSource.NbVertices()))
      ChangeVertexRelationsInternal(aV).EdgeIds = theSource.VertexRelations(aV).EdgeIds;
  }
  for (BRepGraph_EdgeId anE(0); anE.IsValid(NbEdges()); ++anE)
  {
    if (anE.IsValid(theSource.NbEdges()))
      ChangeEdgeRelationsInternal(anE).CoEdgeIds = theSource.EdgeRelations(anE).CoEdgeIds;
  }
  for (BRepGraph_WireId aW(0); aW.IsValid(NbWires()); ++aW)
  {
    if (aW.IsValid(theSource.NbWires()))
    {
      ChangeWireRelationsInternal(aW).CoEdgeIds = theSource.WireRelations(aW).CoEdgeIds;
      ChangeWireRelationsInternal(aW).ParentWireRefIds =
        theSource.WireRelations(aW).ParentWireRefIds;
    }
  }
  for (BRepGraph_FaceId aF(0); aF.IsValid(NbFaces()); ++aF)
  {
    if (aF.IsValid(theSource.NbFaces()))
    {
      ChangeFaceRelationsInternal(aF).WireRefIds = theSource.FaceRelations(aF).WireRefIds;
      ChangeFaceRelationsInternal(aF).ParentFaceRefIds =
        theSource.FaceRelations(aF).ParentFaceRefIds;
    }
  }
  for (BRepGraph_ShellId aS(0); aS.IsValid(NbShells()); ++aS)
  {
    if (aS.IsValid(theSource.NbShells()))
    {
      ChangeShellRelationsInternal(aS).FaceRefIds = theSource.ShellRelations(aS).FaceRefIds;
      ChangeShellRelationsInternal(aS).ParentShellRefIds =
        theSource.ShellRelations(aS).ParentShellRefIds;
    }
  }
  for (BRepGraph_SolidId aS(0); aS.IsValid(NbSolids()); ++aS)
  {
    if (aS.IsValid(theSource.NbSolids()))
    {
      ChangeSolidRelationsInternal(aS).ShellRefIds = theSource.SolidRelations(aS).ShellRefIds;
      ChangeSolidRelationsInternal(aS).ParentSolidRefIds =
        theSource.SolidRelations(aS).ParentSolidRefIds;
    }
  }
  for (BRepGraph_CompSolidId aCS(0); aCS.IsValid(NbCompSolids()); ++aCS)
  {
    if (aCS.IsValid(theSource.NbCompSolids()))
      ChangeCompSolidRelationsInternal(aCS).SolidRefIds =
        theSource.CompSolidRelations(aCS).SolidRefIds;
  }
  for (BRepGraph_CompoundId aC(0); aC.IsValid(NbCompounds()); ++aC)
  {
    if (aC.IsValid(theSource.NbCompounds()))
      ChangeCompoundRelationsInternal(aC).ChildRefIds = theSource.CompoundRelations(aC).ChildRefIds;
  }
  for (BRepGraph_ProductId aP(0); aP.IsValid(NbProducts()); ++aP)
  {
    if (aP.IsValid(theSource.NbProducts()))
      ChangeProductRelationsInternal(aP).OccurrenceRefIds =
        theSource.ProductRelations(aP).OccurrenceRefIds;
  }
  for (BRepGraph_OccurrenceId anO(0); anO.IsValid(NbOccurrences()); ++anO)
  {
    if (anO.IsValid(theSource.NbOccurrences()))
      ChangeOccurrenceRelationsInternal(anO).ParentOccurrenceRefIds =
        theSource.OccurrenceRelations(anO).ParentOccurrenceRefIds;
  }

  // Copy sparse reverse maps (node -> compound/occurrence child refs).
  myNodeToCompounds.Clear();
  for (NCollection_DataMap<BRepGraph_NodeId,
                           NCollection_LinearVector<BRepGraph_ChildRefId>>::Iterator
         anIt(theSource.myNodeToCompounds);
       anIt.More();
       anIt.Next())
  {
    myNodeToCompounds.Bind(anIt.Key(), anIt.Value());
  }
  myNodeToOccurrences.Clear();
  for (NCollection_DataMap<BRepGraph_NodeId,
                           NCollection_LinearVector<BRepGraph_OccurrenceRefId>>::Iterator
         anIt(theSource.myNodeToOccurrences);
       anIt.More();
       anIt.Next())
  {
    myNodeToOccurrences.Bind(anIt.Key(), anIt.Value());
  }

  // Copy compound/occurrence parent bit-planes for all node kinds.
  // These are indexed by the same type IDs in source and destination (identity copy).
  myVertices.HasCompoundParentFlags      = theSource.myVertices.HasCompoundParentFlags;
  myVertices.HasOccurrenceParentFlags    = theSource.myVertices.HasOccurrenceParentFlags;
  myEdges.HasCompoundParentFlags         = theSource.myEdges.HasCompoundParentFlags;
  myEdges.HasOccurrenceParentFlags       = theSource.myEdges.HasOccurrenceParentFlags;
  myCoEdges.HasCompoundParentFlags       = theSource.myCoEdges.HasCompoundParentFlags;
  myCoEdges.HasOccurrenceParentFlags     = theSource.myCoEdges.HasOccurrenceParentFlags;
  myWires.HasCompoundParentFlags         = theSource.myWires.HasCompoundParentFlags;
  myWires.HasOccurrenceParentFlags       = theSource.myWires.HasOccurrenceParentFlags;
  myFaces.HasCompoundParentFlags         = theSource.myFaces.HasCompoundParentFlags;
  myFaces.HasOccurrenceParentFlags       = theSource.myFaces.HasOccurrenceParentFlags;
  myShells.HasCompoundParentFlags        = theSource.myShells.HasCompoundParentFlags;
  myShells.HasOccurrenceParentFlags      = theSource.myShells.HasOccurrenceParentFlags;
  mySolids.HasCompoundParentFlags        = theSource.mySolids.HasCompoundParentFlags;
  mySolids.HasOccurrenceParentFlags      = theSource.mySolids.HasOccurrenceParentFlags;
  myCompounds.HasCompoundParentFlags     = theSource.myCompounds.HasCompoundParentFlags;
  myCompounds.HasOccurrenceParentFlags   = theSource.myCompounds.HasOccurrenceParentFlags;
  myCompSolids.HasCompoundParentFlags    = theSource.myCompSolids.HasCompoundParentFlags;
  myCompSolids.HasOccurrenceParentFlags  = theSource.myCompSolids.HasOccurrenceParentFlags;
  myProducts.HasCompoundParentFlags      = theSource.myProducts.HasCompoundParentFlags;
  myProducts.HasOccurrenceParentFlags    = theSource.myProducts.HasOccurrenceParentFlags;
  myOccurrences.HasCompoundParentFlags   = theSource.myOccurrences.HasCompoundParentFlags;
  myOccurrences.HasOccurrenceParentFlags = theSource.myOccurrences.HasOccurrenceParentFlags;
}

//=================================================================================================

bool BRepGraphInc_Storage::ValidateRelations() const
{
  for (BRepGraph_CoEdgeId aCoEdgeId(0); aCoEdgeId.IsValid(NbCoEdges()); ++aCoEdgeId)
  {
    if (IsRemoved(aCoEdgeId))
    {
      continue;
    }
    const BRepGraphInc::CoEdgeDef& aCoEdge = CoEdge(aCoEdgeId);
    const bool                     hasActiveFaceContext =
      aCoEdge.FaceId.IsValid(NbFaces()) && !IsRemoved(aCoEdge.FaceId);
    if (aCoEdge.Curve2DRepId.IsValid())
    {
      if (!aCoEdge.Curve2DRepId.IsValid(NbCoEdgeCurves2D()))
      {
        return false;
      }
      if (!IsRemoved(aCoEdge.Curve2DRepId)
          && (!hasActiveFaceContext
              || CoEdgeCurve2DRep(aCoEdge.Curve2DRepId).ParentCoEdgeId != aCoEdgeId))
      {
        return false;
      }
    }
    if (aCoEdge.Polygon2DRepId.IsValid())
    {
      if (!aCoEdge.Polygon2DRepId.IsValid(NbCoEdgePolygons2D()))
      {
        return false;
      }
      if (!IsRemoved(aCoEdge.Polygon2DRepId)
          && (!hasActiveFaceContext
              || CoEdgePolygon2DRep(aCoEdge.Polygon2DRepId).ParentCoEdgeId != aCoEdgeId))
      {
        return false;
      }
    }
    if (aCoEdge.PolygonOnTriRepId.IsValid())
    {
      if (!aCoEdge.PolygonOnTriRepId.IsValid(NbCoEdgePolygonsOnTri()))
      {
        return false;
      }
      if (!IsRemoved(aCoEdge.PolygonOnTriRepId)
          && (!hasActiveFaceContext
              || CoEdgePolygonOnTriRep(aCoEdge.PolygonOnTriRepId).ParentCoEdgeId != aCoEdgeId))
      {
        return false;
      }
    }
    if (aCoEdge.ParentWireId.IsValid(NbWires()))
    {
      if (!containsRelationId(WireRelations(aCoEdge.ParentWireId).CoEdgeIds, aCoEdgeId))
      {
        return false;
      }
    }
    if (aCoEdge.ChildEdgeId.IsValid(NbEdges()))
    {
      const BRepGraphInc::EdgeRelations& anEdgeRel = EdgeRelations(aCoEdge.ChildEdgeId);
      if (!containsRelationId(anEdgeRel.CoEdgeIds, aCoEdgeId))
      {
        return false;
      }
      if (aCoEdge.ParentWireId.IsValid(NbWires()) && aCoEdge.FaceId.IsValid(NbFaces()))
      {
        bool hasFaceWireUse = false;
        for (const BRepGraph_WireRefId& aWireRefId : FaceRelations(aCoEdge.FaceId).WireRefIds)
        {
          if (aWireRefId.IsValid(NbWireRefs()) && !IsRemoved(aWireRefId))
          {
            const BRepGraphInc::WireRef& aRef = WireRef(aWireRefId);
            if (aRef.ParentFaceId == aCoEdge.FaceId && aRef.ChildWireId == aCoEdge.ParentWireId)
            {
              hasFaceWireUse = true;
              break;
            }
          }
        }
        if (!hasFaceWireUse)
        {
          return false;
        }
      }
    }
  }

  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(NbEdges()); ++anEdgeId)
  {
    if (IsRemoved(anEdgeId))
    {
      continue;
    }
    const BRepGraphInc::EdgeRelations& anEdgeRel = EdgeRelations(anEdgeId);
    for (const BRepGraph_CoEdgeId& aCoEdgeId : anEdgeRel.CoEdgeIds)
    {
      if (!aCoEdgeId.IsValid(NbCoEdges()) || IsRemoved(aCoEdgeId)
          || CoEdge(aCoEdgeId).ChildEdgeId != anEdgeId)
      {
        return false;
      }
    }
    const BRepGraphInc::EdgeDef& anEdge       = Edge(anEdgeId);
    const auto                   aCheckVertex = [&](const BRepGraph_VertexRefId theRefId) {
      if (!theRefId.IsValid(NbVertexRefs()) || IsRemoved(theRefId))
      {
        return true;
      }
      const BRepGraph_VertexId aVertexId = VertexRef(theRefId).ChildVertexId;
      return !aVertexId.IsValid(NbVertices()) || IsRemoved(aVertexId)
             || containsRelationId(VertexRelations(aVertexId).EdgeIds, anEdgeId);
    };
    if (!aCheckVertex(anEdge.StartVertexRefId) || !aCheckVertex(anEdge.EndVertexRefId))
    {
      return false;
    }
  }

  for (BRepGraph_WireId aWireId(0); aWireId.IsValid(NbWires()); ++aWireId)
  {
    if (IsRemoved(aWireId))
    {
      continue;
    }
    for (const BRepGraph_CoEdgeId& aCoEdgeId : WireRelations(aWireId).CoEdgeIds)
    {
      if (!aCoEdgeId.IsValid(NbCoEdges()) || IsRemoved(aCoEdgeId)
          || CoEdge(aCoEdgeId).ParentWireId != aWireId)
      {
        return false;
      }
    }
  }

  for (BRepGraph_FaceId aFaceId(0); aFaceId.IsValid(NbFaces()); ++aFaceId)
  {
    if (IsRemoved(aFaceId))
    {
      continue;
    }
    for (const BRepGraph_WireRefId& aWireRefId : FaceRelations(aFaceId).WireRefIds)
    {
      if (!aWireRefId.IsValid(NbWireRefs()) || IsRemoved(aWireRefId))
      {
        return false;
      }
      const BRepGraphInc::WireRef& aRef = WireRef(aWireRefId);
      if (aRef.ParentFaceId != aFaceId || !aRef.ChildWireId.IsValid(NbWires())
          || IsRemoved(aRef.ChildWireId)
          || !containsRelationId(WireRelations(aRef.ChildWireId).ParentWireRefIds, aWireRefId))
      {
        return false;
      }
    }
  }

  for (BRepGraph_ShellId aShellId(0); aShellId.IsValid(NbShells()); ++aShellId)
  {
    if (IsRemoved(aShellId))
    {
      continue;
    }
    for (const BRepGraph_FaceRefId& aFaceRefId : ShellRelations(aShellId).FaceRefIds)
    {
      if (!aFaceRefId.IsValid(NbFaceRefs()) || IsRemoved(aFaceRefId))
      {
        return false;
      }
      const BRepGraphInc::FaceRef& aRef = FaceRef(aFaceRefId);
      if (aRef.ParentShellId != aShellId || !aRef.ChildFaceId.IsValid(NbFaces())
          || IsRemoved(aRef.ChildFaceId)
          || !containsRelationId(FaceRelations(aRef.ChildFaceId).ParentFaceRefIds, aFaceRefId))
      {
        return false;
      }
    }
  }

  for (BRepGraph_SolidId aSolidId(0); aSolidId.IsValid(NbSolids()); ++aSolidId)
  {
    if (IsRemoved(aSolidId))
    {
      continue;
    }
    for (const BRepGraph_ShellRefId& aShellRefId : SolidRelations(aSolidId).ShellRefIds)
    {
      if (!aShellRefId.IsValid(NbShellRefs()) || IsRemoved(aShellRefId))
      {
        return false;
      }
      const BRepGraphInc::ShellRef& aRef = ShellRef(aShellRefId);
      if (aRef.ParentSolidId != aSolidId || !aRef.ChildShellId.IsValid(NbShells())
          || IsRemoved(aRef.ChildShellId)
          || !containsRelationId(ShellRelations(aRef.ChildShellId).ParentShellRefIds, aShellRefId))
      {
        return false;
      }
    }
  }

  for (BRepGraph_CompSolidId aCompSolidId(0); aCompSolidId.IsValid(NbCompSolids()); ++aCompSolidId)
  {
    if (IsRemoved(aCompSolidId))
    {
      continue;
    }
    for (const BRepGraph_SolidRefId& aSolidRefId : CompSolidRelations(aCompSolidId).SolidRefIds)
    {
      if (!aSolidRefId.IsValid(NbSolidRefs()) || IsRemoved(aSolidRefId))
      {
        return false;
      }
      const BRepGraphInc::SolidRef& aRef = SolidRef(aSolidRefId);
      if (aRef.ParentCompSolidId != aCompSolidId || !aRef.ChildSolidId.IsValid(NbSolids())
          || IsRemoved(aRef.ChildSolidId)
          || !containsRelationId(SolidRelations(aRef.ChildSolidId).ParentSolidRefIds, aSolidRefId))
      {
        return false;
      }
    }
  }

  for (BRepGraph_CompoundId aCompoundId(0); aCompoundId.IsValid(NbCompounds()); ++aCompoundId)
  {
    if (IsRemoved(aCompoundId))
    {
      continue;
    }
    for (const BRepGraph_ChildRefId& aChildRefId : CompoundRelations(aCompoundId).ChildRefIds)
    {
      if (!aChildRefId.IsValid(NbChildRefs()) || IsRemoved(aChildRefId)
          || ChildRef(aChildRefId).ParentCompoundId != aCompoundId)
      {
        return false;
      }
      const BRepGraph_NodeId aChildNode = ChildRef(aChildRefId).ChildNodeId;
      if (aChildNode.IsValid() && !isNodeRemoved(*this, aChildNode)
          && !containsRelationId(CompoundRefsOfNode(aChildNode), aChildRefId))
      {
        return false;
      }
    }
  }

  for (BRepGraph_ProductId aProductId(0); aProductId.IsValid(NbProducts()); ++aProductId)
  {
    if (IsRemoved(aProductId))
    {
      continue;
    }
    for (const BRepGraph_OccurrenceRefId& aRefId : ProductRelations(aProductId).OccurrenceRefIds)
    {
      if (!aRefId.IsValid(NbOccurrenceRefs()) || IsRemoved(aRefId)
          || OccurrenceRef(aRefId).ParentProductId != aProductId)
      {
        return false;
      }
      const BRepGraph_OccurrenceId anOccurrenceId = OccurrenceRef(aRefId).ChildOccurrenceId;
      if (anOccurrenceId.IsValid(NbOccurrences()) && !IsRemoved(anOccurrenceId))
      {
        if (!containsRelationId(OccurrenceRelations(anOccurrenceId).ParentOccurrenceRefIds, aRefId))
        {
          return false;
        }
        const BRepGraph_NodeId aChildNode = Occurrence(anOccurrenceId).ChildNodeId;
        if (aChildNode.IsValid() && !isNodeRemoved(*this, aChildNode)
            && !containsRelationId(OccurrenceRefsOfNode(aChildNode), aRefId))
        {
          return false;
        }
      }
    }
  }

  for (BRepGraph_OccurrenceId anOccurrenceId(0); anOccurrenceId.IsValid(NbOccurrences());
       ++anOccurrenceId)
  {
    if (IsRemoved(anOccurrenceId))
    {
      continue;
    }
    for (const BRepGraph_OccurrenceRefId& aRefId :
         OccurrenceRelations(anOccurrenceId).ParentOccurrenceRefIds)
    {
      if (!aRefId.IsValid(NbOccurrenceRefs()) || IsRemoved(aRefId)
          || OccurrenceRef(aRefId).ChildOccurrenceId != anOccurrenceId)
      {
        return false;
      }
      const BRepGraph_ProductId aProductId = OccurrenceRef(aRefId).ParentProductId;
      if (!aProductId.IsValid(NbProducts()) || IsRemoved(aProductId)
          || !containsRelationId(ProductRelations(aProductId).OccurrenceRefIds, aRefId))
      {
        return false;
      }
    }
  }

  return true;
}

//=================================================================================================

bool BRepGraphInc_Storage::ValidateWireCoEdgeOrder(const BRepGraph_WireId theWireId) const
{
  if (!theWireId.IsValid(NbWires()) || IsRemoved(theWireId))
  {
    return false;
  }

  const BRepGraphInc::WireRelations& aWireRel = WireRelations(theWireId);
  if (!coEdgeIdsAreUnique(aWireRel.CoEdgeIds))
  {
    return false;
  }

  BRepGraph_VertexId aPrevEnd;
  bool               hasPrev = false;
  for (const BRepGraph_CoEdgeId& aCoEdgeId : aWireRel.CoEdgeIds)
  {
    if (!aCoEdgeId.IsValid(NbCoEdges()) || IsRemoved(aCoEdgeId)
        || CoEdge(aCoEdgeId).ParentWireId != theWireId)
    {
      return false;
    }

    BRepGraph_VertexId aStart;
    BRepGraph_VertexId anEnd;
    if (!coEdgeOrientedVertices(*this, aCoEdgeId, aStart, anEnd))
    {
      return false;
    }
    if (hasPrev && aStart != aPrevEnd)
    {
      return false;
    }

    aPrevEnd = anEnd;
    hasPrev  = true;
  }
  return true;
}

//=================================================================================================

bool BRepGraphInc_Storage::ValidateWireCoEdgeOrders() const
{
  for (BRepGraph_WireId aWireId(0); aWireId.IsValid(NbWires()); ++aWireId)
  {
    if (!IsRemoved(aWireId) && !ValidateWireCoEdgeOrder(aWireId))
    {
      return false;
    }
  }
  return true;
}

//=================================================================================================

BRepGraphInc_Storage::WireCoEdgeOrderStatus BRepGraphInc_Storage::CanonicalizeWireCoEdgeOrderStatus(
  const BRepGraph_WireId theWireId)
{
  using Status = BRepGraphInc_Storage::WireCoEdgeOrderStatus;
  if (!theWireId.IsValid(NbWires()) || IsRemoved(theWireId))
  {
    return Status::InvalidInput;
  }
  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCurrent = WireRelations(theWireId).CoEdgeIds;
  const NCollection_Array1<BRepGraph_CoEdgeId>        aCurrentView = aCurrent.ToArray1();
  NCollection_LinearVector<BRepGraph_CoEdgeId>        anOrdered;
  const Status                                        aStatus =
    BRepGraphInc_WireOrder::BuildCoEdgeOrder(*this, theWireId, aCurrentView, anOrdered);
  if (aStatus == Status::Reordered || aStatus == Status::ToleranceOrdered
      || aStatus == Status::Partial)
  {
    SetWireCoEdges(theWireId, anOrdered.ToArray1());
  }
  return aStatus;
}

//=================================================================================================

bool BRepGraphInc_Storage::CanonicalizeWireCoEdgeOrder(const BRepGraph_WireId theWireId)
{
  return CanonicalizeWireCoEdgeOrderStatus(theWireId)
         != BRepGraphInc_Storage::WireCoEdgeOrderStatus::InvalidInput;
}

//=================================================================================================

BRepGraph_CoEdgeId BRepGraphInc_Storage::CreateCoEdgeUse(
  const BRepGraph_WireId                theParentWireId,
  const BRepGraph_EdgeId                theChildEdgeId,
  const BRepGraph_FaceId                theFaceId,
  const BRepGraphInc::ParityOrientation theOrientation)
{
  const BRepGraph_CoEdgeId aCoEdgeId = AppendCoEdge();
  BRepGraphInc::CoEdgeDef& aCoEdge   = ChangeCoEdge(aCoEdgeId);
  aCoEdge.ParentWireId               = theParentWireId;
  aCoEdge.ChildEdgeId                = theChildEdgeId;
  aCoEdge.FaceId                     = theFaceId;
  aCoEdge.Orientation                = theOrientation;

  ChangeWireRelationsInternal(theParentWireId).CoEdgeIds.Append(aCoEdgeId);
  BRepGraphInc::EdgeRelations& anEdgeRel = ChangeEdgeRelationsInternal(theChildEdgeId);
  appendUniqueRelationId(anEdgeRel.CoEdgeIds, aCoEdgeId);
  return aCoEdgeId;
}

//=================================================================================================

void BRepGraphInc_Storage::AttachEdgeToVertex(const BRepGraph_EdgeId   theEdgeId,
                                              const BRepGraph_VertexId theVertexId)
{
  if (!theEdgeId.IsValid(NbEdges()) || !theVertexId.IsValid(NbVertices()))
  {
    return;
  }
  appendUniqueRelationId(ChangeVertexRelationsInternal(theVertexId).EdgeIds, theEdgeId);
}

//=================================================================================================

BRepGraph_WireRefId BRepGraphInc_Storage::AttachWireToFace(
  const BRepGraph_FaceId                theParentFaceId,
  const BRepGraph_WireId                theChildWireId,
  const BRepGraphInc::ParityOrientation theOrientation)
{
  const BRepGraph_WireRefId aRefId = AppendWireRef();
  BRepGraphInc::WireRef&    aRef   = ChangeWireRef(aRefId);
  aRef.ParentFaceId                = theParentFaceId;
  aRef.ChildWireId                 = theChildWireId;
  aRef.Orientation                 = theOrientation;
  ChangeFaceRelationsInternal(theParentFaceId).WireRefIds.Append(aRefId);
  appendUniqueRelationId(ChangeWireRelationsInternal(theChildWireId).ParentWireRefIds, aRefId);
  return aRefId;
}

//=================================================================================================

BRepGraph_FaceRefId BRepGraphInc_Storage::AttachFaceToShell(
  const BRepGraph_ShellId               theParentShellId,
  const BRepGraph_FaceId                theChildFaceId,
  const BRepGraphInc::ParityOrientation theOrientation)
{
  const BRepGraph_FaceRefId aRefId = AppendFaceRef();
  BRepGraphInc::FaceRef&    aRef   = ChangeFaceRef(aRefId);
  aRef.ParentShellId               = theParentShellId;
  aRef.ChildFaceId                 = theChildFaceId;
  aRef.Orientation                 = theOrientation;
  ChangeShellRelationsInternal(theParentShellId).FaceRefIds.Append(aRefId);
  appendUniqueRelationId(ChangeFaceRelationsInternal(theChildFaceId).ParentFaceRefIds, aRefId);
  return aRefId;
}

//=================================================================================================

BRepGraph_ShellRefId BRepGraphInc_Storage::AttachShellToSolid(
  const BRepGraph_SolidId               theParentSolidId,
  const BRepGraph_ShellId               theChildShellId,
  const BRepGraphInc::ParityOrientation theOrientation)
{
  const BRepGraph_ShellRefId aRefId = AppendShellRef();
  BRepGraphInc::ShellRef&    aRef   = ChangeShellRef(aRefId);
  aRef.ParentSolidId                = theParentSolidId;
  aRef.ChildShellId                 = theChildShellId;
  aRef.Orientation                  = theOrientation;
  ChangeSolidRelationsInternal(theParentSolidId).ShellRefIds.Append(aRefId);
  appendUniqueRelationId(ChangeShellRelationsInternal(theChildShellId).ParentShellRefIds, aRefId);
  return aRefId;
}

//=================================================================================================

BRepGraph_SolidRefId BRepGraphInc_Storage::AttachSolidToCompSolid(
  const BRepGraph_CompSolidId           theParentCompSolidId,
  const BRepGraph_SolidId               theChildSolidId,
  const BRepGraphInc::ParityOrientation theOrientation)
{
  const BRepGraph_SolidRefId aRefId = AppendSolidRef();
  BRepGraphInc::SolidRef&    aRef   = ChangeSolidRef(aRefId);
  aRef.ParentCompSolidId            = theParentCompSolidId;
  aRef.ChildSolidId                 = theChildSolidId;
  aRef.Orientation                  = theOrientation;
  ChangeCompSolidRelationsInternal(theParentCompSolidId).SolidRefIds.Append(aRefId);
  appendUniqueRelationId(ChangeSolidRelationsInternal(theChildSolidId).ParentSolidRefIds, aRefId);
  return aRefId;
}

//=================================================================================================

BRepGraph_ChildRefId BRepGraphInc_Storage::AttachChildToCompound(
  const BRepGraph_CompoundId            theParentCompoundId,
  const BRepGraph_NodeId                theChildNodeId,
  const TopLoc_Location&                theLocation,
  const BRepGraphInc::ParityOrientation theOrientation)
{
  const BRepGraph_ChildRefId aRefId = AppendChildRef();
  BRepGraphInc::ChildRef&    aRef   = ChangeChildRef(aRefId);
  aRef.ParentCompoundId             = theParentCompoundId;
  aRef.ChildNodeId                  = theChildNodeId;
  aRef.LocalLocation                = theLocation;
  aRef.Orientation                  = theOrientation;
  ChangeCompoundRelationsInternal(theParentCompoundId).ChildRefIds.Append(aRefId);
  if (theChildNodeId.IsValid())
  {
    appendUniqueRelationId(ChangeCompoundRefsOfNodeInternal(theChildNodeId), aRefId);
    SetHasCompoundParent(theChildNodeId, true);
  }
  return aRefId;
}

//=================================================================================================

BRepGraph_OccurrenceRefId BRepGraphInc_Storage::AttachOccurrenceToProduct(
  const BRepGraph_ProductId    theParentProductId,
  const BRepGraph_OccurrenceId theChildOccurrenceId,
  const TopLoc_Location&       theLocation)
{
  const BRepGraph_OccurrenceRefId aRefId = AppendOccurrenceRef();
  BRepGraphInc::OccurrenceRef&    aRef   = ChangeOccurrenceRef(aRefId);
  aRef.ParentProductId                   = theParentProductId;
  aRef.ChildOccurrenceId                 = theChildOccurrenceId;
  aRef.LocalLocation                     = theLocation;
  ChangeProductRelationsInternal(theParentProductId).OccurrenceRefIds.Append(aRefId);
  if (theChildOccurrenceId.IsValid(NbOccurrences()))
  {
    appendUniqueRelationId(
      ChangeOccurrenceRelationsInternal(theChildOccurrenceId).ParentOccurrenceRefIds,
      aRefId);
    const BRepGraph_NodeId aChildNode = Occurrence(theChildOccurrenceId).ChildNodeId;
    if (aChildNode.IsValid())
    {
      appendUniqueRelationId(ChangeOccurrenceRefsOfNodeInternal(aChildNode), aRefId);
      SetHasOccurrenceParent(aChildNode, true);
    }
  }
  return aRefId;
}

//=================================================================================================

bool BRepGraphInc_Storage::DetachCoEdgeUse(const BRepGraph_WireId   theParentWireId,
                                           const BRepGraph_CoEdgeId theCoEdgeId)
{
  if (!theParentWireId.IsValid(NbWires()) || !theCoEdgeId.IsValid(NbCoEdges()))
  {
    return false;
  }

  const BRepGraphInc::CoEdgeDef& aCoEdge  = CoEdge(theCoEdgeId);
  const BRepGraph_EdgeId         anEdgeId = aCoEdge.ChildEdgeId;
  const bool                     isErased =
    eraseRelationId(ChangeWireRelationsInternal(theParentWireId).CoEdgeIds, theCoEdgeId);
  if (!isErased)
  {
    return false;
  }

  if (anEdgeId.IsValid(NbEdges()))
  {
    BRepGraphInc::EdgeRelations& anEdgeRel = ChangeEdgeRelationsInternal(anEdgeId);
    eraseRelationId(anEdgeRel.CoEdgeIds, theCoEdgeId);
  }
  return true;
}

//=================================================================================================

bool BRepGraphInc_Storage::ReplaceCoEdgeUseWithPair(const BRepGraph_WireId   theParentWireId,
                                                    const BRepGraph_CoEdgeId theOldCoEdgeId,
                                                    const BRepGraph_CoEdgeId theNewFirstCoEdgeId,
                                                    const BRepGraph_CoEdgeId theNewSecondCoEdgeId)
{
  if (!theParentWireId.IsValid(NbWires()) || !theOldCoEdgeId.IsValid(NbCoEdges())
      || !theNewFirstCoEdgeId.IsValid(NbCoEdges()) || !theNewSecondCoEdgeId.IsValid(NbCoEdges()))
  {
    return false;
  }

  NCollection_LinearVector<BRepGraph_CoEdgeId>& aWireCoEdges =
    ChangeWireRelationsInternal(theParentWireId).CoEdgeIds;
  size_t aPos = 0;
  for (; aPos < aWireCoEdges.Size(); ++aPos)
  {
    if (aWireCoEdges.Value(aPos) == theOldCoEdgeId)
    {
      break;
    }
  }
  if (aPos >= aWireCoEdges.Size())
  {
    return false;
  }

  BRepGraphInc::CoEdgeDef& aNewFirst  = ChangeCoEdge(theNewFirstCoEdgeId);
  BRepGraphInc::CoEdgeDef& aNewSecond = ChangeCoEdge(theNewSecondCoEdgeId);
  aNewFirst.ParentWireId              = theParentWireId;
  aNewSecond.ParentWireId             = theParentWireId;

  const BRepGraphInc::CoEdgeDef& anOldCoEdge = CoEdge(theOldCoEdgeId);
  if (anOldCoEdge.ChildEdgeId.IsValid(NbEdges()))
  {
    BRepGraphInc::EdgeRelations& anOldEdgeRel =
      ChangeEdgeRelationsInternal(anOldCoEdge.ChildEdgeId);
    eraseRelationId(anOldEdgeRel.CoEdgeIds, theOldCoEdgeId);
  }
  aWireCoEdges.ChangeValue(aPos) = theNewFirstCoEdgeId;
  aWireCoEdges.InsertAfter(aPos, theNewSecondCoEdgeId);

  auto bindEdgeRelations = [&](const BRepGraph_CoEdgeId theCoEdgeId) {
    const BRepGraphInc::CoEdgeDef& aCoEdge = CoEdge(theCoEdgeId);
    if (!aCoEdge.ChildEdgeId.IsValid(NbEdges()))
    {
      return;
    }
    BRepGraphInc::EdgeRelations& anEdgeRel = ChangeEdgeRelationsInternal(aCoEdge.ChildEdgeId);
    appendUniqueRelationId(anEdgeRel.CoEdgeIds, theCoEdgeId);
  };
  bindEdgeRelations(theNewFirstCoEdgeId);
  bindEdgeRelations(theNewSecondCoEdgeId);
  return true;
}

//=================================================================================================

bool BRepGraphInc_Storage::DetachWireFromFace(const BRepGraph_FaceId    theParentFaceId,
                                              const BRepGraph_WireRefId theRefId)
{
  if (!theParentFaceId.IsValid(NbFaces()) || !theRefId.IsValid(NbWireRefs()))
  {
    return false;
  }
  const BRepGraphInc::WireRef& aRef = WireRef(theRefId);
  const bool                   isErased =
    eraseRelationId(ChangeFaceRelationsInternal(theParentFaceId).WireRefIds, theRefId);
  if (!isErased)
  {
    return false;
  }
  if (aRef.ChildWireId.IsValid(NbWires()))
  {
    eraseRelationId(ChangeWireRelationsInternal(aRef.ChildWireId).ParentWireRefIds, theRefId);
    if (!wireHasFaceUse(*this, aRef.ChildWireId, theParentFaceId))
    {
      clearWireFaceContext(*this, aRef.ChildWireId, theParentFaceId);
    }
  }
  return true;
}

//=================================================================================================

bool BRepGraphInc_Storage::DetachFaceFromShell(const BRepGraph_ShellId   theParentShellId,
                                               const BRepGraph_FaceRefId theRefId)
{
  if (!theParentShellId.IsValid(NbShells()) || !theRefId.IsValid(NbFaceRefs()))
  {
    return false;
  }
  const BRepGraphInc::FaceRef& aRef = FaceRef(theRefId);
  const bool                   isErased =
    eraseRelationId(ChangeShellRelationsInternal(theParentShellId).FaceRefIds, theRefId);
  if (aRef.ChildFaceId.IsValid(NbFaces()))
  {
    eraseRelationId(ChangeFaceRelationsInternal(aRef.ChildFaceId).ParentFaceRefIds, theRefId);
  }
  return isErased;
}

//=================================================================================================

bool BRepGraphInc_Storage::DetachShellFromSolid(const BRepGraph_SolidId    theParentSolidId,
                                                const BRepGraph_ShellRefId theRefId)
{
  if (!theParentSolidId.IsValid(NbSolids()) || !theRefId.IsValid(NbShellRefs()))
  {
    return false;
  }
  const BRepGraphInc::ShellRef& aRef = ShellRef(theRefId);
  const bool                    isErased =
    eraseRelationId(ChangeSolidRelationsInternal(theParentSolidId).ShellRefIds, theRefId);
  if (aRef.ChildShellId.IsValid(NbShells()))
  {
    eraseRelationId(ChangeShellRelationsInternal(aRef.ChildShellId).ParentShellRefIds, theRefId);
  }
  return isErased;
}

//=================================================================================================

bool BRepGraphInc_Storage::DetachSolidFromCompSolid(
  const BRepGraph_CompSolidId theParentCompSolidId,
  const BRepGraph_SolidRefId  theRefId)
{
  if (!theParentCompSolidId.IsValid(NbCompSolids()) || !theRefId.IsValid(NbSolidRefs()))
  {
    return false;
  }
  const BRepGraphInc::SolidRef& aRef = SolidRef(theRefId);
  const bool                    isErased =
    eraseRelationId(ChangeCompSolidRelationsInternal(theParentCompSolidId).SolidRefIds, theRefId);
  if (aRef.ChildSolidId.IsValid(NbSolids()))
  {
    eraseRelationId(ChangeSolidRelationsInternal(aRef.ChildSolidId).ParentSolidRefIds, theRefId);
  }
  return isErased;
}

//=================================================================================================

bool BRepGraphInc_Storage::DetachChildFromCompound(const BRepGraph_CompoundId theParentCompoundId,
                                                   const BRepGraph_ChildRefId theRefId)
{
  if (!theParentCompoundId.IsValid(NbCompounds()) || !theRefId.IsValid(NbChildRefs()))
  {
    return false;
  }
  const BRepGraph_NodeId aChildNode = ChildRef(theRefId).ChildNodeId;
  const bool             isErased =
    eraseRelationId(ChangeCompoundRelationsInternal(theParentCompoundId).ChildRefIds, theRefId);
  if (isErased && aChildNode.IsValid())
  {
    if (NCollection_LinearVector<BRepGraph_ChildRefId>* aRefs =
          myNodeToCompounds.ChangeSeek(aChildNode))
    {
      eraseRelationId(*aRefs, theRefId);
      if (aRefs->IsEmpty())
      {
        SetHasCompoundParent(aChildNode, false);
      }
    }
  }
  return isErased;
}

//=================================================================================================

bool BRepGraphInc_Storage::DetachOccurrenceFromProduct(const BRepGraph_ProductId theParentProductId,
                                                       const BRepGraph_OccurrenceRefId theRefId)
{
  if (!theParentProductId.IsValid(NbProducts()) || !theRefId.IsValid(NbOccurrenceRefs()))
  {
    return false;
  }
  BRepGraph_NodeId                   aChildNode;
  const BRepGraphInc::OccurrenceRef& aRef = OccurrenceRef(theRefId);
  if (aRef.ChildOccurrenceId.IsValid(NbOccurrences()))
  {
    aChildNode = Occurrence(aRef.ChildOccurrenceId).ChildNodeId;
  }
  const bool isErased =
    eraseRelationId(ChangeProductRelationsInternal(theParentProductId).OccurrenceRefIds, theRefId);
  if (isErased && aRef.ChildOccurrenceId.IsValid(NbOccurrences()))
  {
    eraseRelationId(
      ChangeOccurrenceRelationsInternal(aRef.ChildOccurrenceId).ParentOccurrenceRefIds,
      theRefId);
  }
  if (isErased && aChildNode.IsValid())
  {
    if (NCollection_LinearVector<BRepGraph_OccurrenceRefId>* aRefs =
          myNodeToOccurrences.ChangeSeek(aChildNode))
    {
      eraseRelationId(*aRefs, theRefId);
      if (aRefs->IsEmpty())
      {
        SetHasOccurrenceParent(aChildNode, false);
      }
    }
  }
  return isErased;
}

//=================================================================================================

void BRepGraphInc_Storage::RebindOccurrenceChild(const BRepGraph_OccurrenceId theOccurrence,
                                                 const BRepGraph_NodeId       theOldChild,
                                                 const BRepGraph_NodeId       theNewChild)
{
  if (theOldChild == theNewChild || !theOccurrence.IsValid(NbOccurrences()))
  {
    return;
  }

  for (BRepGraph_OccurrenceRefId aRefId = BRepGraph_OccurrenceRefId::Start();
       aRefId.IsValid(NbOccurrenceRefs());
       ++aRefId)
  {
    if (IsRemoved(aRefId) || OccurrenceRef(aRefId).ChildOccurrenceId != theOccurrence)
    {
      continue;
    }
    if (theOldChild.IsValid())
    {
      if (NCollection_LinearVector<BRepGraph_OccurrenceRefId>* aRefs =
            myNodeToOccurrences.ChangeSeek(theOldChild))
      {
        eraseRelationId(*aRefs, aRefId);
        if (aRefs->IsEmpty())
        {
          SetHasOccurrenceParent(theOldChild, false);
        }
      }
    }
    if (theNewChild.IsValid())
    {
      appendUniqueRelationId(ChangeOccurrenceRefsOfNodeInternal(theNewChild), aRefId);
      SetHasOccurrenceParent(theNewChild, true);
    }
  }
}

//=================================================================================================

void BRepGraphInc_Storage::RebindVertexEdge(const BRepGraph_VertexId    theOldVertex,
                                            const BRepGraph_VertexId    theNewVertex,
                                            const BRepGraph_EdgeId      theEdge,
                                            const BRepGraph_VertexRefId theExcludingRef)
{
  if (theOldVertex == theNewVertex || !theEdge.IsValid(NbEdges()))
  {
    return;
  }
  if (IsRemoved(theEdge))
  {
    return;
  }

  BRepGraphInc::EdgeDef& anEdge     = ChangeEdge(theEdge);
  auto                   refTargets = [&](const BRepGraph_VertexRefId theRefId,
                        const BRepGraph_VertexId    theVertex) -> bool {
    if (!theRefId.IsValid(NbVertexRefs()) || theRefId == theExcludingRef || IsRemoved(theRefId))
    {
      return false;
    }
    return VertexRef(theRefId).ChildVertexId == theVertex;
  };

  auto rebindRef = [&](const BRepGraph_VertexRefId theRefId) {
    if (!theRefId.IsValid(NbVertexRefs()) || theRefId == theExcludingRef || IsRemoved(theRefId))
    {
      return;
    }
    BRepGraphInc::VertexRef& aRef = ChangeVertexRef(theRefId);
    if (aRef.ChildVertexId == theOldVertex)
    {
      aRef.ChildVertexId = theNewVertex;
    }
  };

  rebindRef(anEdge.StartVertexRefId);
  rebindRef(anEdge.EndVertexRefId);

  if (theOldVertex.IsValid(NbVertices()) && !refTargets(anEdge.StartVertexRefId, theOldVertex)
      && !refTargets(anEdge.EndVertexRefId, theOldVertex))
  {
    eraseRelationId(ChangeVertexRelationsInternal(theOldVertex).EdgeIds, theEdge);
  }
  if (theNewVertex.IsValid(NbVertices())
      && (refTargets(anEdge.StartVertexRefId, theNewVertex)
          || refTargets(anEdge.EndVertexRefId, theNewVertex)))
  {
    appendUniqueRelationId(ChangeVertexRelationsInternal(theNewVertex).EdgeIds, theEdge);
  }
}

//=================================================================================================

void BRepGraphInc_Storage::RebindVertexRef(const BRepGraph_VertexRefId theRefId,
                                           const BRepGraph_VertexId    theOldVertex,
                                           const BRepGraph_VertexId    theNewVertex)
{
  if (theOldVertex == theNewVertex || !theRefId.IsValid(NbVertexRefs()))
  {
    return;
  }

  BRepGraphInc::VertexRef& aRef = ChangeVertexRef(theRefId);
  if (aRef.ChildVertexId != theOldVertex)
  {
    return;
  }

  const BRepGraph_EdgeId anEdgeId = aRef.ParentEdgeId;
  if (!anEdgeId.IsValid(NbEdges()) || IsRemoved(anEdgeId))
  {
    return;
  }
  aRef.ChildVertexId = theNewVertex;

  const BRepGraphInc::EdgeDef& anEdge     = Edge(anEdgeId);
  auto                         refTargets = [&](const BRepGraph_VertexRefId theOtherRef,
                        const BRepGraph_VertexId    theVertex) -> bool {
    if (!theOtherRef.IsValid(NbVertexRefs()) || theOtherRef == theRefId || IsRemoved(theOtherRef))
    {
      return false;
    }
    return VertexRef(theOtherRef).ChildVertexId == theVertex;
  };

  if (theOldVertex.IsValid(NbVertices()) && !refTargets(anEdge.StartVertexRefId, theOldVertex)
      && !refTargets(anEdge.EndVertexRefId, theOldVertex))
  {
    eraseRelationId(ChangeVertexRelationsInternal(theOldVertex).EdgeIds, anEdgeId);
  }
  if (theNewVertex.IsValid(NbVertices()))
  {
    appendUniqueRelationId(ChangeVertexRelationsInternal(theNewVertex).EdgeIds, anEdgeId);
  }
}

//=================================================================================================

void BRepGraphInc_Storage::RebindCoEdgeEdge(const BRepGraph_CoEdgeId theCoEdge,
                                            const BRepGraph_EdgeId   theOldEdge,
                                            const BRepGraph_EdgeId   theNewEdge)
{
  if (theOldEdge == theNewEdge || !theCoEdge.IsValid(NbCoEdges()))
  {
    return;
  }

  if (theOldEdge.IsValid(NbEdges()))
  {
    BRepGraphInc::EdgeRelations& anOldRel = ChangeEdgeRelationsInternal(theOldEdge);
    eraseRelationId(anOldRel.CoEdgeIds, theCoEdge);
  }

  if (theNewEdge.IsValid(NbEdges()))
  {
    BRepGraphInc::EdgeRelations& aNewRel = ChangeEdgeRelationsInternal(theNewEdge);
    appendUniqueRelationId(aNewRel.CoEdgeIds, theCoEdge);
  }
}

//=================================================================================================

void BRepGraphInc_Storage::RebindWireRef(const BRepGraph_WireRefId theRefId,
                                         const BRepGraph_WireId    theOldWire,
                                         const BRepGraph_WireId    theNewWire)
{
  if (theOldWire == theNewWire || !theRefId.IsValid(NbWireRefs()))
  {
    return;
  }
  const BRepGraph_FaceId aParentFace = WireRef(theRefId).ParentFaceId;
  if (theOldWire.IsValid(NbWires()))
  {
    eraseRelationId(ChangeWireRelationsInternal(theOldWire).ParentWireRefIds, theRefId);
    if (aParentFace.IsValid(NbFaces()))
    {
      if (!wireHasFaceUse(*this, theOldWire, aParentFace))
      {
        clearWireFaceContext(*this, theOldWire, aParentFace);
      }
    }
  }
  if (theNewWire.IsValid(NbWires()) && !IsRemoved(theRefId))
  {
    appendUniqueRelationId(ChangeWireRelationsInternal(theNewWire).ParentWireRefIds, theRefId);
  }
}

//=================================================================================================

void BRepGraphInc_Storage::RebindFaceRef(const BRepGraph_FaceRefId theRefId,
                                         const BRepGraph_FaceId    theOldFace,
                                         const BRepGraph_FaceId    theNewFace)
{
  if (theOldFace == theNewFace || !theRefId.IsValid(NbFaceRefs()))
  {
    return;
  }
  if (theOldFace.IsValid(NbFaces()))
  {
    eraseRelationId(ChangeFaceRelationsInternal(theOldFace).ParentFaceRefIds, theRefId);
  }
  if (theNewFace.IsValid(NbFaces()) && !IsRemoved(theRefId))
  {
    appendUniqueRelationId(ChangeFaceRelationsInternal(theNewFace).ParentFaceRefIds, theRefId);
  }
}

//=================================================================================================

void BRepGraphInc_Storage::RebindShellRef(const BRepGraph_ShellRefId theRefId,
                                          const BRepGraph_ShellId    theOldShell,
                                          const BRepGraph_ShellId    theNewShell)
{
  if (theOldShell == theNewShell || !theRefId.IsValid(NbShellRefs()))
  {
    return;
  }
  if (theOldShell.IsValid(NbShells()))
  {
    eraseRelationId(ChangeShellRelationsInternal(theOldShell).ParentShellRefIds, theRefId);
  }
  if (theNewShell.IsValid(NbShells()) && !IsRemoved(theRefId))
  {
    appendUniqueRelationId(ChangeShellRelationsInternal(theNewShell).ParentShellRefIds, theRefId);
  }
}

//=================================================================================================

void BRepGraphInc_Storage::RebindSolidRef(const BRepGraph_SolidRefId theRefId,
                                          const BRepGraph_SolidId    theOldSolid,
                                          const BRepGraph_SolidId    theNewSolid)
{
  if (theOldSolid == theNewSolid || !theRefId.IsValid(NbSolidRefs()))
  {
    return;
  }
  if (theOldSolid.IsValid(NbSolids()))
  {
    eraseRelationId(ChangeSolidRelationsInternal(theOldSolid).ParentSolidRefIds, theRefId);
  }
  if (theNewSolid.IsValid(NbSolids()) && !IsRemoved(theRefId))
  {
    appendUniqueRelationId(ChangeSolidRelationsInternal(theNewSolid).ParentSolidRefIds, theRefId);
  }
}

//=================================================================================================

void BRepGraphInc_Storage::RebindChildRef(const BRepGraph_ChildRefId theRefId,
                                          const BRepGraph_NodeId     theOldChild,
                                          const BRepGraph_NodeId     theNewChild)
{
  if (theOldChild == theNewChild || !theRefId.IsValid(NbChildRefs()))
  {
    return;
  }
  if (theOldChild.IsValid())
  {
    if (NCollection_LinearVector<BRepGraph_ChildRefId>* aRefs =
          myNodeToCompounds.ChangeSeek(theOldChild))
    {
      eraseRelationId(*aRefs, theRefId);
      if (aRefs->IsEmpty())
      {
        SetHasCompoundParent(theOldChild, false);
      }
    }
  }
  if (theNewChild.IsValid() && !IsRemoved(theRefId))
  {
    appendUniqueRelationId(ChangeCompoundRefsOfNodeInternal(theNewChild), theRefId);
    SetHasCompoundParent(theNewChild, true);
  }
}

//=================================================================================================

void BRepGraphInc_Storage::RebindOccurrenceRef(const BRepGraph_OccurrenceRefId theRefId,
                                               const BRepGraph_OccurrenceId    theOldOccurrence,
                                               const BRepGraph_OccurrenceId    theNewOccurrence)
{
  if (theOldOccurrence == theNewOccurrence || !theRefId.IsValid(NbOccurrenceRefs()))
  {
    return;
  }

  BRepGraph_NodeId anOldChild;
  if (theOldOccurrence.IsValid(NbOccurrences()))
  {
    anOldChild = Occurrence(theOldOccurrence).ChildNodeId;
  }
  BRepGraph_NodeId aNewChild;
  if (theNewOccurrence.IsValid(NbOccurrences()))
  {
    aNewChild = Occurrence(theNewOccurrence).ChildNodeId;
  }
  if (anOldChild.IsValid())
  {
    if (NCollection_LinearVector<BRepGraph_OccurrenceRefId>* aRefs =
          myNodeToOccurrences.ChangeSeek(anOldChild))
    {
      eraseRelationId(*aRefs, theRefId);
      if (aRefs->IsEmpty())
      {
        SetHasOccurrenceParent(anOldChild, false);
      }
    }
  }
  if (theOldOccurrence.IsValid(NbOccurrences()))
  {
    eraseRelationId(ChangeOccurrenceRelationsInternal(theOldOccurrence).ParentOccurrenceRefIds,
                    theRefId);
  }
  if (theNewOccurrence.IsValid(NbOccurrences()) && !IsRemoved(theRefId))
  {
    appendUniqueRelationId(
      ChangeOccurrenceRelationsInternal(theNewOccurrence).ParentOccurrenceRefIds,
      theRefId);
  }
  if (aNewChild.IsValid() && !IsRemoved(theRefId))
  {
    appendUniqueRelationId(ChangeOccurrenceRefsOfNodeInternal(aNewChild), theRefId);
    SetHasOccurrenceParent(aNewChild, true);
  }
}

//=================================================================================================

void BRepGraphInc_Storage::ReverseWireCoEdges(const BRepGraph_WireId theWireId)
{
  if (!theWireId.IsValid(NbWires()))
  {
    return;
  }
  NCollection_LinearVector<BRepGraph_CoEdgeId>& aRefs =
    ChangeWireRelationsInternal(theWireId).CoEdgeIds;
  const size_t aNb = aRefs.Size();
  if (aNb < 2)
  {
    return;
  }

  for (size_t i = 0, j = aNb - 1; i < j; ++i, --j)
  {
    const BRepGraph_CoEdgeId aTmp = aRefs.Value(i);
    aRefs.ChangeValue(i)          = aRefs.Value(j);
    aRefs.ChangeValue(j)          = aTmp;
  }
}

//=================================================================================================

void BRepGraphInc_Storage::SetWireCoEdges(
  const BRepGraph_WireId                        theWireId,
  const NCollection_Array1<BRepGraph_CoEdgeId>& theCoEdgeIds)
{
  if (!theWireId.IsValid(NbWires()))
  {
    return;
  }
  assignRelationIds(ChangeWireRelationsInternal(theWireId).CoEdgeIds, theCoEdgeIds);
}

//=================================================================================================

void BRepGraphInc_Storage::SetFaceWireRefs(
  const BRepGraph_FaceId                         theFaceId,
  const NCollection_Array1<BRepGraph_WireRefId>& theWireRefIds)
{
  if (theFaceId.IsValid(NbFaces()))
  {
    assignRelationIds(ChangeFaceRelationsInternal(theFaceId).WireRefIds, theWireRefIds);
  }
}

void BRepGraphInc_Storage::SetShellFaceRefs(
  const BRepGraph_ShellId                        theShellId,
  const NCollection_Array1<BRepGraph_FaceRefId>& theFaceRefIds)
{
  if (theShellId.IsValid(NbShells()))
  {
    assignRelationIds(ChangeShellRelationsInternal(theShellId).FaceRefIds, theFaceRefIds);
  }
}

void BRepGraphInc_Storage::SetSolidShellRefs(
  const BRepGraph_SolidId                         theSolidId,
  const NCollection_Array1<BRepGraph_ShellRefId>& theShellRefIds)
{
  if (theSolidId.IsValid(NbSolids()))
  {
    assignRelationIds(ChangeSolidRelationsInternal(theSolidId).ShellRefIds, theShellRefIds);
  }
}

void BRepGraphInc_Storage::SetCompSolidSolidRefs(
  const BRepGraph_CompSolidId                     theCompSolidId,
  const NCollection_Array1<BRepGraph_SolidRefId>& theSolidRefIds)
{
  if (theCompSolidId.IsValid(NbCompSolids()))
  {
    assignRelationIds(ChangeCompSolidRelationsInternal(theCompSolidId).SolidRefIds, theSolidRefIds);
  }
}

void BRepGraphInc_Storage::SetCompoundChildRefs(
  const BRepGraph_CompoundId                      theCompoundId,
  const NCollection_Array1<BRepGraph_ChildRefId>& theChildRefIds)
{
  if (theCompoundId.IsValid(NbCompounds()))
  {
    assignRelationIds(ChangeCompoundRelationsInternal(theCompoundId).ChildRefIds, theChildRefIds);
  }
}

void BRepGraphInc_Storage::SetProductOccurrenceRefs(
  const BRepGraph_ProductId                            theProductId,
  const NCollection_Array1<BRepGraph_OccurrenceRefId>& theOccurrenceRefIds)
{
  if (theProductId.IsValid(NbProducts()))
  {
    assignRelationIds(ChangeProductRelationsInternal(theProductId).OccurrenceRefIds,
                      theOccurrenceRefIds);
  }
}

//=================================================================================================

void BRepGraphInc_Storage::ClearUIDIndexes()
{
  {
    std::unique_lock<std::shared_mutex> aLock(myUIDToNodeIdMutex);
    myUIDToNodeId.Clear();
    myUIDToNodeIdDirty.store(false, std::memory_order_relaxed);
  }
  {
    std::unique_lock<std::shared_mutex> aLock(myRefUIDToRefIdMutex);
    myRefUIDToRefId.Clear();
    myRefUIDToRefIdDirty.store(false, std::memory_order_relaxed);
  }
}

//=================================================================================================

void BRepGraphInc_Storage::ClearShapeCache()
{
  std::unique_lock<std::shared_mutex> aLock(myCurrentShapesMutex);
  myCurrentShapes.Clear(true);
}

//=================================================================================================

void BRepGraphInc_Storage::ClearCurrentShapes()
{
  ClearShapeCache();
}

//=================================================================================================

void BRepGraphInc_Storage::CopyShapeBindingsFrom(const BRepGraphInc_Storage& theSource)
{
  if (&theSource == this)
  {
    return;
  }

  NCollection_LinearVector<std::pair<TopoDS_Shape, BRepGraph_NodeId>> aShapeBindings;
  NCollection_LinearVector<std::pair<BRepGraph_NodeId, TopoDS_Shape>> aOriginalBindings;
  theSource.ForEachShapeBinding([&](const TopoDS_Shape& aShape, const BRepGraph_NodeId aNodeId) {
    aShapeBindings.Append({aShape, aNodeId});
  });
  theSource.ForEachOriginalBinding([&](const BRepGraph_NodeId aNodeId, const TopoDS_Shape& aShape) {
    aOriginalBindings.Append({aNodeId, aShape});
  });

  std::unique_lock<std::shared_mutex> aLock(myShapeBindingsMutex);
  myShapeToNodeId.Clear();
  myOriginalShapes.Clear();
  for (const auto& [aShape, aNodeId] : aShapeBindings)
  {
    myShapeToNodeId.Bind(aShape, aNodeId);
  }
  for (const auto& [aNodeId, aShape] : aOriginalBindings)
  {
    myOriginalShapes.Bind(aNodeId, aShape);
  }
}

//=================================================================================================

BRepGraph_NodeId BRepGraphInc_Storage::FindDefinitionByShape(const TopoDS_Shape& theShape) const
{
  std::shared_lock<std::shared_mutex> aLock(myShapeBindingsMutex);
  const BRepGraph_NodeId*             aBound = myShapeToNodeId.Seek(theShape);
  if (aBound == nullptr || !aBound->IsValid())
  {
    return BRepGraph_NodeId();
  }
  // Validate the bound node is still active in storage.
  const auto aRemovedCheck = [this](const auto theTypedId) -> bool {
    return !IsRemoved(theTypedId);
  };
  return BRepGraph_NodeId::Visit(*aBound, aRemovedCheck) ? *aBound : BRepGraph_NodeId();
}

//=================================================================================================

bool BRepGraphInc_Storage::HasShapeBinding(const TopoDS_Shape& theShape) const
{
  return FindDefinitionByShape(theShape).IsValid();
}

//=================================================================================================

void BRepGraphInc_Storage::SetDefinitionShapeBinding(const TopoDS_Shape&    theShape,
                                                     const BRepGraph_NodeId theNodeId)
{
  std::unique_lock<std::shared_mutex> aLock(myShapeBindingsMutex);
  if (auto* aBound = myShapeToNodeId.ChangeSeek(theShape))
  {
    *aBound = theNodeId;
    return;
  }
  myShapeToNodeId.Bind(theShape, theNodeId);
}

//=================================================================================================

bool BRepGraphInc_Storage::RemoveDefinitionShapeBinding(const TopoDS_Shape&    theShape,
                                                        const BRepGraph_NodeId theExpectedNodeId)
{
  std::unique_lock<std::shared_mutex> aLock(myShapeBindingsMutex);
  const BRepGraph_NodeId*             aBound = myShapeToNodeId.Seek(theShape);
  if (aBound == nullptr || *aBound != theExpectedNodeId)
  {
    return false;
  }
  myShapeToNodeId.UnBind(theShape);
  return true;
}

//=================================================================================================

TopoDS_Shape BRepGraphInc_Storage::FindOriginal(const BRepGraph_NodeId theNodeId) const
{
  std::shared_lock<std::shared_mutex> aLock(myShapeBindingsMutex);
  const TopoDS_Shape*                 anOriginal = myOriginalShapes.Seek(theNodeId);
  return anOriginal != nullptr ? *anOriginal : TopoDS_Shape();
}

//=================================================================================================

bool BRepGraphInc_Storage::HasOriginal(const BRepGraph_NodeId theNodeId) const
{
  std::shared_lock<std::shared_mutex> aLock(myShapeBindingsMutex);
  return myOriginalShapes.IsBound(theNodeId);
}

//=================================================================================================

void BRepGraphInc_Storage::BindOriginal(const BRepGraph_NodeId theNodeId,
                                        const TopoDS_Shape&    theShape)
{
  std::unique_lock<std::shared_mutex> aLock(myShapeBindingsMutex);
  if (TopoDS_Shape* anOriginal = myOriginalShapes.ChangeSeek(theNodeId))
  {
    *anOriginal = theShape;
    return;
  }
  myOriginalShapes.Bind(theNodeId, theShape);
}

//=================================================================================================

void BRepGraphInc_Storage::UnBindOriginal(const BRepGraph_NodeId theNodeId)
{
  std::unique_lock<std::shared_mutex> aLock(myShapeBindingsMutex);
  myOriginalShapes.UnBind(theNodeId);
}

//=================================================================================================

void BRepGraphInc_Storage::UnbindCurrentShape(const BRepGraph_NodeId theNode)
{
  std::unique_lock<std::shared_mutex> aLock(myCurrentShapesMutex);
  myCurrentShapes.UnBind(theNode);
}

//=================================================================================================

void BRepGraphInc_Storage::ClearDeferredQueues()
{
  myDeferredModified.Clear(true);
  myDeferredRefModified.Clear(true);
}

//=================================================================================================

const BRepGraphInc::BaseRef& BRepGraphInc_Storage::BaseRef(const BRepGraph_RefId theRefId) const
{
  static const BRepGraphInc::BaseRef anInvalid;
  if (!theRefId.IsValid())
  {
    return anInvalid;
  }

  const auto aFindRef = [this](const auto theTypedId) -> const BRepGraphInc::BaseRef* {
    using TypeId = std::remove_cv_t<decltype(theTypedId)>;

    if constexpr (std::is_same_v<TypeId, BRepGraph_ShellRefId>)
    {
      return findInStore(myShellRefs, theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_FaceRefId>)
    {
      return findInStore(myFaceRefs, theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_WireRefId>)
    {
      return findInStore(myWireRefs, theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_VertexRefId>)
    {
      return findInStore(myVertexRefs, theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_SolidRefId>)
    {
      return findInStore(mySolidRefs, theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_ChildRefId>)
    {
      return findInStore(myChildRefs, theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_OccurrenceRefId>)
    {
      return findInStore(myOccurrenceRefs, theTypedId);
    }
    else
    {
      Standard_ASSERT_RETURN(false, "BaseRef: unsupported ref id type", nullptr);
    }
  };

  const BRepGraphInc::BaseRef* aRef = BRepGraph_RefId::Visit(theRefId, aFindRef);
  return aRef != nullptr ? *aRef : anInvalid;
}

//=================================================================================================

BRepGraphInc::BaseRef* BRepGraphInc_Storage::ChangeBaseRef(const BRepGraph_RefId theRefId)
{
  if (!theRefId.IsValid())
  {
    return nullptr;
  }

  const auto aChangeRef = [this](const auto theTypedId) -> BRepGraphInc::BaseRef* {
    using TypeId = std::remove_cv_t<decltype(theTypedId)>;

    if constexpr (std::is_same_v<TypeId, BRepGraph_ShellRefId>)
    {
      return changeFindInStore(myShellRefs, theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_FaceRefId>)
    {
      return changeFindInStore(myFaceRefs, theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_WireRefId>)
    {
      return changeFindInStore(myWireRefs, theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_VertexRefId>)
    {
      return changeFindInStore(myVertexRefs, theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_SolidRefId>)
    {
      return changeFindInStore(mySolidRefs, theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_ChildRefId>)
    {
      return changeFindInStore(myChildRefs, theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_OccurrenceRefId>)
    {
      return changeFindInStore(myOccurrenceRefs, theTypedId);
    }
    else
    {
      Standard_ASSERT_RETURN(false, "ChangeBaseRef: unsupported ref id type", nullptr);
    }
  };

  return BRepGraph_RefId::Visit(theRefId, aChangeRef);
}

//=================================================================================================

void BRepGraphInc_Storage::Clear()
{
  ClearStorageForReuse();
  ClearUIDIndexes();
  ClearShapeCache();
  ClearDeferredQueues();
}

//=================================================================================================

void BRepGraphInc_Storage::PrepareForLoad(const BRepGraphInc_Load::Counts& theCounts)
{
  // Skip full Clear() when storage is already empty - avoids allocator reset + reallocation.
  if (!IsEmpty())
  {
    Clear();
  }

  prepareDefStore(myVertices, theCounts.NbVertices, myAllocator);
  prepareDefStore(myEdges, theCounts.NbEdges, myAllocator);
  prepareDefStore(myCoEdges, theCounts.NbCoEdges, myAllocator);
  prepareDefStore(myWires, theCounts.NbWires, myAllocator);
  prepareDefStore(myFaces, theCounts.NbFaces, myAllocator);
  prepareDefStore(myShells, theCounts.NbShells, myAllocator);
  prepareDefStore(mySolids, theCounts.NbSolids, myAllocator);
  prepareDefStore(myCompounds, theCounts.NbCompounds, myAllocator);
  prepareDefStore(myCompSolids, theCounts.NbCompSolids, myAllocator);
  prepareDefStore(myProducts, theCounts.NbProducts, myAllocator);
  prepareDefStore(myOccurrences, theCounts.NbOccurrences, myAllocator);

  prepareRelationTable(myVertexRelations, theCounts.NbVertices);
  prepareRelationTable(myEdgeRelations, theCounts.NbEdges);
  prepareRelationTable(myWireRelations, theCounts.NbWires);
  prepareRelationTable(myFaceRelations, theCounts.NbFaces);
  prepareRelationTable(myShellRelations, theCounts.NbShells);
  prepareRelationTable(mySolidRelations, theCounts.NbSolids);
  prepareRelationTable(myCompoundRelations, theCounts.NbCompounds);
  prepareRelationTable(myCompSolidRelations, theCounts.NbCompSolids);
  prepareRelationTable(myProductRelations, theCounts.NbProducts);
  prepareRelationTable(myOccurrenceRelations, theCounts.NbOccurrences);

  prepareRefStore(myShellRefs, theCounts.NbShellRefs);
  prepareRefStore(myFaceRefs, theCounts.NbFaceRefs);
  prepareRefStore(myWireRefs, theCounts.NbWireRefs);
  prepareRefStore(myVertexRefs, theCounts.NbVertexRefs);
  prepareRefStore(mySolidRefs, theCounts.NbSolidRefs);
  prepareRefStore(myChildRefs, theCounts.NbChildRefs);
  prepareRefStore(myOccurrenceRefs, theCounts.NbOccurrenceRefs);

  prepareRepStore(myFaceSurfaces, theCounts.NbFaceSurfaceReps);
  prepareRepStore(myEdgeCurves3D, theCounts.NbEdgeCurve3DReps);
  prepareRepStore(myCoEdgeCurves2D, theCounts.NbCoEdgeCurve2DReps);
  prepareRepStore(myFaceTriangulations, theCounts.NbFaceTriangulationReps);
  prepareRepStore(myEdgePolygons3D, theCounts.NbEdgePolygon3DReps);
  prepareRepStore(myCoEdgePolygons2D, theCounts.NbCoEdgePolygon2DReps);
  prepareRepStore(myCoEdgePolygonsOnTri, theCounts.NbCoEdgePolygonOnTriReps);
}

//=================================================================================================

BRepGraphInc_Load::Counts BRepGraphInc_Storage::Counts() const
{
  BRepGraphInc_Load::Counts aCounts;
  aCounts.NbVertices               = NbVertices();
  aCounts.NbEdges                  = NbEdges();
  aCounts.NbCoEdges                = NbCoEdges();
  aCounts.NbWires                  = NbWires();
  aCounts.NbFaces                  = NbFaces();
  aCounts.NbShells                 = NbShells();
  aCounts.NbSolids                 = NbSolids();
  aCounts.NbCompounds              = NbCompounds();
  aCounts.NbCompSolids             = NbCompSolids();
  aCounts.NbProducts               = NbProducts();
  aCounts.NbOccurrences            = NbOccurrences();
  aCounts.NbShellRefs              = NbShellRefs();
  aCounts.NbFaceRefs               = NbFaceRefs();
  aCounts.NbWireRefs               = NbWireRefs();
  aCounts.NbVertexRefs             = NbVertexRefs();
  aCounts.NbSolidRefs              = NbSolidRefs();
  aCounts.NbChildRefs              = NbChildRefs();
  aCounts.NbOccurrenceRefs         = NbOccurrenceRefs();
  aCounts.NbFaceSurfaceReps        = NbFaceSurfaces();
  aCounts.NbEdgeCurve3DReps        = NbEdgeCurves3D();
  aCounts.NbCoEdgeCurve2DReps      = NbCoEdgeCurves2D();
  aCounts.NbFaceTriangulationReps  = NbFaceTriangulations();
  aCounts.NbEdgePolygon3DReps      = NbEdgePolygons3D();
  aCounts.NbCoEdgePolygon2DReps    = NbCoEdgePolygons2D();
  aCounts.NbCoEdgePolygonOnTriReps = NbCoEdgePolygonsOnTri();
  return aCounts;
}

//=================================================================================================

BRepGraphInc_Load::Counts BRepGraphInc_Storage::ActiveCounts() const
{
  BRepGraphInc_Load::Counts aCounts;
  aCounts.NbVertices               = NbActiveVertices();
  aCounts.NbEdges                  = NbActiveEdges();
  aCounts.NbCoEdges                = NbActiveCoEdges();
  aCounts.NbWires                  = NbActiveWires();
  aCounts.NbFaces                  = NbActiveFaces();
  aCounts.NbShells                 = NbActiveShells();
  aCounts.NbSolids                 = NbActiveSolids();
  aCounts.NbCompounds              = NbActiveCompounds();
  aCounts.NbCompSolids             = NbActiveCompSolids();
  aCounts.NbProducts               = NbActiveProducts();
  aCounts.NbOccurrences            = NbActiveOccurrences();
  aCounts.NbShellRefs              = NbActiveShellRefs();
  aCounts.NbFaceRefs               = NbActiveFaceRefs();
  aCounts.NbWireRefs               = NbActiveWireRefs();
  aCounts.NbVertexRefs             = NbActiveVertexRefs();
  aCounts.NbSolidRefs              = NbActiveSolidRefs();
  aCounts.NbChildRefs              = NbActiveChildRefs();
  aCounts.NbOccurrenceRefs         = NbActiveOccurrenceRefs();
  aCounts.NbFaceSurfaceReps        = NbActiveFaceSurfaces();
  aCounts.NbEdgeCurve3DReps        = NbActiveEdgeCurves3D();
  aCounts.NbCoEdgeCurve2DReps      = NbActiveCoEdgeCurves2D();
  aCounts.NbFaceTriangulationReps  = NbActiveFaceTriangulations();
  aCounts.NbEdgePolygon3DReps      = NbActiveEdgePolygons3D();
  aCounts.NbCoEdgePolygon2DReps    = NbActiveCoEdgePolygons2D();
  aCounts.NbCoEdgePolygonOnTriReps = NbActiveCoEdgePolygonsOnTri();
  return aCounts;
}

//=================================================================================================

void BRepGraphInc_Storage::SetActiveCounts(const BRepGraphInc_Load::Counts& theCounts)
{
  myVertices.NbActive            = theCounts.NbVertices;
  myEdges.NbActive               = theCounts.NbEdges;
  myCoEdges.NbActive             = theCounts.NbCoEdges;
  myWires.NbActive               = theCounts.NbWires;
  myFaces.NbActive               = theCounts.NbFaces;
  myShells.NbActive              = theCounts.NbShells;
  mySolids.NbActive              = theCounts.NbSolids;
  myCompounds.NbActive           = theCounts.NbCompounds;
  myCompSolids.NbActive          = theCounts.NbCompSolids;
  myProducts.NbActive            = theCounts.NbProducts;
  myOccurrences.NbActive         = theCounts.NbOccurrences;
  myShellRefs.NbActive           = theCounts.NbShellRefs;
  myFaceRefs.NbActive            = theCounts.NbFaceRefs;
  myWireRefs.NbActive            = theCounts.NbWireRefs;
  myVertexRefs.NbActive          = theCounts.NbVertexRefs;
  mySolidRefs.NbActive           = theCounts.NbSolidRefs;
  myChildRefs.NbActive           = theCounts.NbChildRefs;
  myOccurrenceRefs.NbActive      = theCounts.NbOccurrenceRefs;
  myFaceSurfaces.NbActive        = theCounts.NbFaceSurfaceReps;
  myEdgeCurves3D.NbActive        = theCounts.NbEdgeCurve3DReps;
  myCoEdgeCurves2D.NbActive      = theCounts.NbCoEdgeCurve2DReps;
  myFaceTriangulations.NbActive  = theCounts.NbFaceTriangulationReps;
  myEdgePolygons3D.NbActive      = theCounts.NbEdgePolygon3DReps;
  myCoEdgePolygons2D.NbActive    = theCounts.NbCoEdgePolygon2DReps;
  myCoEdgePolygonsOnTri.NbActive = theCounts.NbCoEdgePolygonOnTriReps;
}

//=================================================================================================

bool BRepGraphInc_Storage::MarkRemoved(const BRepGraph_NodeId theNodeId)
{
  const auto aMarkRemoved = [this](const auto theTypedId) -> bool {
    using TypeId = std::remove_cv_t<decltype(theTypedId)>;

    if constexpr (std::is_same_v<TypeId, BRepGraph_VertexId>)
    {
      return myVertices.MarkRemoved(theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_EdgeId>)
    {
      return myEdges.MarkRemoved(theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_CoEdgeId>)
    {
      return myCoEdges.MarkRemoved(theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_WireId>)
    {
      return myWires.MarkRemoved(theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_FaceId>)
    {
      return myFaces.MarkRemoved(theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_ShellId>)
    {
      return myShells.MarkRemoved(theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_SolidId>)
    {
      return mySolids.MarkRemoved(theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_CompoundId>)
    {
      return myCompounds.MarkRemoved(theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_CompSolidId>)
    {
      return myCompSolids.MarkRemoved(theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_ProductId>)
    {
      return myProducts.MarkRemoved(theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_OccurrenceId>)
    {
      return myOccurrences.MarkRemoved(theTypedId);
    }
    else
    {
      Standard_ASSERT_RETURN(false, "MarkRemoved: unsupported node id type", false);
    }
  };

  const bool isRemoved =
    theNodeId.IsValid() ? BRepGraph_NodeId::Visit(theNodeId, aMarkRemoved) : false;
  if (isRemoved)
  {
    const TopoDS_Shape* aBoundOriginal = myOriginalShapes.Seek(theNodeId);
    TopoDS_Shape        aShapeToUnbind;
    if (aBoundOriginal != nullptr)
    {
      aShapeToUnbind = *aBoundOriginal;
    }
    myOriginalShapes.UnBind(theNodeId);
    if (!aShapeToUnbind.IsNull())
    {
      RemoveDefinitionShapeBinding(aShapeToUnbind, theNodeId);
    }
  }
  return isRemoved;
}

//=================================================================================================

bool BRepGraphInc_Storage::MarkRemoved(const BRepGraph_RepId theRepId)
{
  if (!theRepId.IsValid())
  {
    return false;
  }

  switch (theRepId.RepKind)
  {
    case BRepGraph_RepId::Kind::EdgeCurve3D:
      return myEdgeCurves3D.MarkRemoved(BRepGraph_EdgeCurve3DRepId(theRepId.Index));
    case BRepGraph_RepId::Kind::EdgePolygon3D:
      return myEdgePolygons3D.MarkRemoved(BRepGraph_EdgePolygon3DRepId(theRepId.Index));
    case BRepGraph_RepId::Kind::CoEdgeCurve2D:
      return myCoEdgeCurves2D.MarkRemoved(BRepGraph_CoEdgeCurve2DRepId(theRepId.Index));
    case BRepGraph_RepId::Kind::CoEdgePolygon2D:
      return myCoEdgePolygons2D.MarkRemoved(BRepGraph_CoEdgePolygon2DRepId(theRepId.Index));
    case BRepGraph_RepId::Kind::CoEdgePolygonOnTri:
      return myCoEdgePolygonsOnTri.MarkRemoved(BRepGraph_CoEdgePolygonOnTriRepId(theRepId.Index));
    case BRepGraph_RepId::Kind::FaceSurface:
      return myFaceSurfaces.MarkRemoved(BRepGraph_FaceSurfaceRepId(theRepId.Index));
    case BRepGraph_RepId::Kind::FaceTriangulation:
      return myFaceTriangulations.MarkRemoved(BRepGraph_FaceTriangulationRepId(theRepId.Index));
  }

  return false;
}

//=================================================================================================

void BRepGraphInc_Storage::SetRemoved(const BRepGraph_RepId theRepId, const bool theVal)
{
  if (!theRepId.IsValid())
  {
    return;
  }

  switch (theRepId.RepKind)
  {
    case BRepGraph_RepId::Kind::EdgeCurve3D:
      SetRemoved(BRepGraph_EdgeCurve3DRepId(theRepId.Index), theVal);
      return;
    case BRepGraph_RepId::Kind::EdgePolygon3D:
      SetRemoved(BRepGraph_EdgePolygon3DRepId(theRepId.Index), theVal);
      return;
    case BRepGraph_RepId::Kind::CoEdgeCurve2D:
      SetRemoved(BRepGraph_CoEdgeCurve2DRepId(theRepId.Index), theVal);
      return;
    case BRepGraph_RepId::Kind::CoEdgePolygon2D:
      SetRemoved(BRepGraph_CoEdgePolygon2DRepId(theRepId.Index), theVal);
      return;
    case BRepGraph_RepId::Kind::CoEdgePolygonOnTri:
      SetRemoved(BRepGraph_CoEdgePolygonOnTriRepId(theRepId.Index), theVal);
      return;
    case BRepGraph_RepId::Kind::FaceSurface:
      SetRemoved(BRepGraph_FaceSurfaceRepId(theRepId.Index), theVal);
      return;
    case BRepGraph_RepId::Kind::FaceTriangulation:
      SetRemoved(BRepGraph_FaceTriangulationRepId(theRepId.Index), theVal);
      return;
  }
}

//=================================================================================================

bool BRepGraphInc_Storage::MarkRemovedRef(const BRepGraph_RefId theRefId)
{
  const auto aMarkRemoved = [this](const auto theTypedId) -> bool {
    using TypeId = std::remove_cv_t<decltype(theTypedId)>;

    if constexpr (std::is_same_v<TypeId, BRepGraph_ShellRefId>)
    {
      return myShellRefs.MarkRemoved(theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_FaceRefId>)
    {
      return myFaceRefs.MarkRemoved(theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_WireRefId>)
    {
      return myWireRefs.MarkRemoved(theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_VertexRefId>)
    {
      return myVertexRefs.MarkRemoved(theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_SolidRefId>)
    {
      return mySolidRefs.MarkRemoved(theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_ChildRefId>)
    {
      return myChildRefs.MarkRemoved(theTypedId);
    }
    else if constexpr (std::is_same_v<TypeId, BRepGraph_OccurrenceRefId>)
    {
      return myOccurrenceRefs.MarkRemoved(theTypedId);
    }
    else
    {
      Standard_ASSERT_RETURN(false, "MarkRemovedRef: unsupported ref id type", false);
    }
  };

  return theRefId.IsValid() ? BRepGraph_RefId::Visit(theRefId, aMarkRemoved) : false;
}

//=================================================================================================

void BRepGraphInc_Storage::RecountActiveCounts()
{
  recountActiveStore(myVertices, BRepGraph_VertexId());
  recountActiveStore(myEdges, BRepGraph_EdgeId());
  recountActiveStore(myCoEdges, BRepGraph_CoEdgeId());
  recountActiveStore(myWires, BRepGraph_WireId());
  recountActiveStore(myFaces, BRepGraph_FaceId());
  recountActiveStore(myShells, BRepGraph_ShellId());
  recountActiveStore(mySolids, BRepGraph_SolidId());
  recountActiveStore(myCompounds, BRepGraph_CompoundId());
  recountActiveStore(myCompSolids, BRepGraph_CompSolidId());
  recountActiveStore(myProducts, BRepGraph_ProductId());
  recountActiveStore(myOccurrences, BRepGraph_OccurrenceId());
  recountActiveStore(myShellRefs, BRepGraph_ShellRefId());
  recountActiveStore(myFaceRefs, BRepGraph_FaceRefId());
  recountActiveStore(myWireRefs, BRepGraph_WireRefId());
  recountActiveStore(myVertexRefs, BRepGraph_VertexRefId());
  recountActiveStore(mySolidRefs, BRepGraph_SolidRefId());
  recountActiveStore(myChildRefs, BRepGraph_ChildRefId());
  recountActiveStore(myOccurrenceRefs, BRepGraph_OccurrenceRefId());
  recountActiveStore(myFaceSurfaces, BRepGraph_FaceSurfaceRepId());
  recountActiveStore(myEdgeCurves3D, BRepGraph_EdgeCurve3DRepId());
  recountActiveStore(myCoEdgeCurves2D, BRepGraph_CoEdgeCurve2DRepId());
  recountActiveStore(myFaceTriangulations, BRepGraph_FaceTriangulationRepId());
  recountActiveStore(myEdgePolygons3D, BRepGraph_EdgePolygon3DRepId());
  recountActiveStore(myCoEdgePolygons2D, BRepGraph_CoEdgePolygon2DRepId());
  recountActiveStore(myCoEdgePolygonsOnTri, BRepGraph_CoEdgePolygonOnTriRepId());
}

//=================================================================================================

void BRepGraphInc_Storage::RebuildUIDReverseIndexes()
{
  MarkUIDReverseIndexesDirty();
  EnsureUIDReverseIndex();
  EnsureRefUIDReverseIndex();
}

//=================================================================================================

void BRepGraphInc_Storage::MarkUIDReverseIndexesDirty()
{
  {
    std::unique_lock<std::shared_mutex> aLock(myUIDToNodeIdMutex);
    myUIDToNodeId.Clear();
    myUIDToNodeIdDirty.store(true, std::memory_order_release);
  }
  {
    std::unique_lock<std::shared_mutex> aLock(myRefUIDToRefIdMutex);
    myRefUIDToRefId.Clear();
    myRefUIDToRefIdDirty.store(true, std::memory_order_release);
  }
}

//=================================================================================================

void BRepGraphInc_Storage::EnsureUIDReverseIndex() const
{
  // Lock-free fast path: skip if not dirty.
  if (!myUIDToNodeIdDirty.load(std::memory_order_acquire))
  {
    return;
  }

  std::unique_lock<std::shared_mutex> aLock(myUIDToNodeIdMutex);
  if (!myUIDToNodeIdDirty.load(std::memory_order_relaxed))
  {
    return;
  }

  myUIDToNodeId.Clear();
  const BRepGraph_NodeId::Kind aKinds[] = {BRepGraph_NodeId::Kind::Solid,
                                           BRepGraph_NodeId::Kind::Shell,
                                           BRepGraph_NodeId::Kind::Face,
                                           BRepGraph_NodeId::Kind::Wire,
                                           BRepGraph_NodeId::Kind::Edge,
                                           BRepGraph_NodeId::Kind::Vertex,
                                           BRepGraph_NodeId::Kind::Compound,
                                           BRepGraph_NodeId::Kind::CompSolid,
                                           BRepGraph_NodeId::Kind::CoEdge,
                                           BRepGraph_NodeId::Kind::Product,
                                           BRepGraph_NodeId::Kind::Occurrence};
  // Reserve based on total entity count.
  size_t aNodeUidCount = 0;
  for (const BRepGraph_NodeId::Kind aKind : aKinds)
  {
    switch (aKind)
    {
      case BRepGraph_NodeId::Kind::Vertex:
        aNodeUidCount += myVertices.Nb();
        break;
      case BRepGraph_NodeId::Kind::Edge:
        aNodeUidCount += myEdges.Nb();
        break;
      case BRepGraph_NodeId::Kind::CoEdge:
        aNodeUidCount += myCoEdges.Nb();
        break;
      case BRepGraph_NodeId::Kind::Wire:
        aNodeUidCount += myWires.Nb();
        break;
      case BRepGraph_NodeId::Kind::Face:
        aNodeUidCount += myFaces.Nb();
        break;
      case BRepGraph_NodeId::Kind::Shell:
        aNodeUidCount += myShells.Nb();
        break;
      case BRepGraph_NodeId::Kind::Solid:
        aNodeUidCount += mySolids.Nb();
        break;
      case BRepGraph_NodeId::Kind::Compound:
        aNodeUidCount += myCompounds.Nb();
        break;
      case BRepGraph_NodeId::Kind::CompSolid:
        aNodeUidCount += myCompSolids.Nb();
        break;
      case BRepGraph_NodeId::Kind::Product:
        aNodeUidCount += myProducts.Nb();
        break;
      case BRepGraph_NodeId::Kind::Occurrence:
        aNodeUidCount += myOccurrences.Nb();
        break;
      default:
        break;
    }
  }
  myUIDToNodeId.Reserve(aNodeUidCount);
  for (const BRepGraph_NodeId::Kind aKind : aKinds)
  {
    // Iterate all entities of this kind and read inline UID field.
    const auto bindFromStore = [&](const auto& theStore, const BRepGraph_NodeId::Kind theKind) {
      for (uint32_t i = 0; i < theStore.Nb(); ++i)
      {
        const BRepGraph_NodeId aNodeId(theKind, i);
        // Skip removed entities - their UIDs should not resolve.
        if (theStore.RemovedFlags.Test(i))
        {
          continue;
        }
        const uint32_t aUidCounter = theStore.Entities.Value(static_cast<size_t>(i)).UID;
        if (aUidCounter > 0)
        {
          const BRepGraph_UID aUID(theKind, aUidCounter);
          myUIDToNodeId.Bind(aUID, aNodeId);
        }
      }
    };
    switch (aKind)
    {
      case BRepGraph_NodeId::Kind::Vertex:
        bindFromStore(myVertices, aKind);
        break;
      case BRepGraph_NodeId::Kind::Edge:
        bindFromStore(myEdges, aKind);
        break;
      case BRepGraph_NodeId::Kind::CoEdge:
        bindFromStore(myCoEdges, aKind);
        break;
      case BRepGraph_NodeId::Kind::Wire:
        bindFromStore(myWires, aKind);
        break;
      case BRepGraph_NodeId::Kind::Face:
        bindFromStore(myFaces, aKind);
        break;
      case BRepGraph_NodeId::Kind::Shell:
        bindFromStore(myShells, aKind);
        break;
      case BRepGraph_NodeId::Kind::Solid:
        bindFromStore(mySolids, aKind);
        break;
      case BRepGraph_NodeId::Kind::Compound:
        bindFromStore(myCompounds, aKind);
        break;
      case BRepGraph_NodeId::Kind::CompSolid:
        bindFromStore(myCompSolids, aKind);
        break;
      case BRepGraph_NodeId::Kind::Product:
        bindFromStore(myProducts, aKind);
        break;
      case BRepGraph_NodeId::Kind::Occurrence:
        bindFromStore(myOccurrences, aKind);
        break;
      default:
        break;
    }
  }
  myUIDToNodeIdDirty.store(false, std::memory_order_release);
}

//=================================================================================================

void BRepGraphInc_Storage::EnsureRefUIDReverseIndex() const
{
  // Lock-free fast path: skip if not dirty.
  if (!myRefUIDToRefIdDirty.load(std::memory_order_acquire))
  {
    return;
  }

  std::unique_lock<std::shared_mutex> aLock(myRefUIDToRefIdMutex);
  if (!myRefUIDToRefIdDirty.load(std::memory_order_relaxed))
  {
    return;
  }

  myRefUIDToRefId.Clear();
  const BRepGraph_RefId::Kind aKinds[]     = {BRepGraph_RefId::Kind::Shell,
                                              BRepGraph_RefId::Kind::Face,
                                              BRepGraph_RefId::Kind::Wire,
                                              BRepGraph_RefId::Kind::Vertex,
                                              BRepGraph_RefId::Kind::Solid,
                                              BRepGraph_RefId::Kind::Child,
                                              BRepGraph_RefId::Kind::Occurrence};
  size_t                      aRefUidCount = 0;
  for (const BRepGraph_RefId::Kind aKind : aKinds)
  {
    switch (aKind)
    {
      case BRepGraph_RefId::Kind::Shell:
        aRefUidCount += myShellRefs.Nb();
        break;
      case BRepGraph_RefId::Kind::Face:
        aRefUidCount += myFaceRefs.Nb();
        break;
      case BRepGraph_RefId::Kind::Wire:
        aRefUidCount += myWireRefs.Nb();
        break;
      case BRepGraph_RefId::Kind::Vertex:
        aRefUidCount += myVertexRefs.Nb();
        break;
      case BRepGraph_RefId::Kind::Solid:
        aRefUidCount += mySolidRefs.Nb();
        break;
      case BRepGraph_RefId::Kind::Child:
        aRefUidCount += myChildRefs.Nb();
        break;
      case BRepGraph_RefId::Kind::Occurrence:
        aRefUidCount += myOccurrenceRefs.Nb();
        break;
      default:
        break;
    }
  }
  myRefUIDToRefId.Reserve(aRefUidCount);
  for (const BRepGraph_RefId::Kind aKind : aKinds)
  {
    const auto bindFromStore = [&](const auto& theStore, const BRepGraph_RefId::Kind theKind) {
      for (uint32_t i = 0; i < theStore.Nb(); ++i)
      {
        const BRepGraph_RefId aRefId(theKind, i);
        // Skip removed refs - their UIDs should not resolve.
        if (theStore.RemovedFlags.Test(i))
        {
          continue;
        }
        const uint32_t aUidCounter = theStore.Refs.Value(static_cast<size_t>(i)).UID;
        if (aUidCounter > 0)
        {
          const BRepGraph_RefUID aUID(theKind, aUidCounter);
          myRefUIDToRefId.Bind(aUID, aRefId);
        }
      }
    };
    switch (aKind)
    {
      case BRepGraph_RefId::Kind::Shell:
        bindFromStore(myShellRefs, aKind);
        break;
      case BRepGraph_RefId::Kind::Face:
        bindFromStore(myFaceRefs, aKind);
        break;
      case BRepGraph_RefId::Kind::Wire:
        bindFromStore(myWireRefs, aKind);
        break;
      case BRepGraph_RefId::Kind::Vertex:
        bindFromStore(myVertexRefs, aKind);
        break;
      case BRepGraph_RefId::Kind::Solid:
        bindFromStore(mySolidRefs, aKind);
        break;
      case BRepGraph_RefId::Kind::Child:
        bindFromStore(myChildRefs, aKind);
        break;
      case BRepGraph_RefId::Kind::Occurrence:
        bindFromStore(myOccurrenceRefs, aKind);
        break;
      default:
        break;
    }
  }
  myRefUIDToRefIdDirty.store(false, std::memory_order_release);
}

//=================================================================================================

uint32_t BRepGraphInc_Storage::NbActiveEdgeCurves3D() const
{
  return myEdgeCurves3D.NbActive;
}

//=================================================================================================

uint32_t BRepGraphInc_Storage::NbActiveCoEdgeCurves2D() const
{
  return myCoEdgeCurves2D.NbActive;
}

//=================================================================================================

uint32_t BRepGraphInc_Storage::NbActiveFaceSurfaces() const
{
  return myFaceSurfaces.NbActive;
}

//=================================================================================================

uint32_t BRepGraphInc_Storage::NbActiveFaceTriangulations() const
{
  return myFaceTriangulations.NbActive;
}

//=================================================================================================

uint32_t BRepGraphInc_Storage::NbActiveEdgePolygons3D() const
{
  return myEdgePolygons3D.NbActive;
}

//=================================================================================================

uint32_t BRepGraphInc_Storage::NbActiveCoEdgePolygons2D() const
{
  return myCoEdgePolygons2D.NbActive;
}

//=================================================================================================

uint32_t BRepGraphInc_Storage::NbActiveCoEdgePolygonsOnTri() const
{
  return myCoEdgePolygonsOnTri.NbActive;
}
