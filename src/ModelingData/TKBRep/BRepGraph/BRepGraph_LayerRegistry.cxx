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

#include <BRepGraph_LayerRegistry.hxx>

#include <BRepGraph.hxx>
#include <NCollection_LinearVector.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_ProgramError.hxx>

#include <limits>
#include <mutex>

//=================================================================================================

BRepGraph_LayerRegistry::BRepGraph_LayerRegistry() = default;

//=================================================================================================

BRepGraph_LayerRegistry::BRepGraph_LayerRegistry(BRepGraph_LayerRegistry&& theOther) noexcept
{
  std::unique_lock<std::shared_mutex> aLock(theOther.myMutex);
  myLayers     = std::move(theOther.myLayers);
  myGuidToSlot = std::move(theOther.myGuidToSlot);
  mySubscribedKindsMask.store(theOther.mySubscribedKindsMask.load(std::memory_order_relaxed),
                              std::memory_order_relaxed);
  mySubscribedRefKindsMask.store(theOther.mySubscribedRefKindsMask.load(std::memory_order_relaxed),
                                 std::memory_order_relaxed);
  myGraph = theOther.myGraph;
  theOther.mySubscribedKindsMask.store(0, std::memory_order_relaxed);
  theOther.mySubscribedRefKindsMask.store(0, std::memory_order_relaxed);
  theOther.myGraph = nullptr;
}

//=================================================================================================

BRepGraph_LayerRegistry& BRepGraph_LayerRegistry::operator=(
  BRepGraph_LayerRegistry&& theOther) noexcept
{
  if (this != &theOther)
  {
    std::unique_lock<std::shared_mutex> aThisLock(myMutex, std::defer_lock);
    std::unique_lock<std::shared_mutex> anOtherLock(theOther.myMutex, std::defer_lock);
    std::lock(aThisLock, anOtherLock);

    detachAllLocked();
    myLayers     = std::move(theOther.myLayers);
    myGuidToSlot = std::move(theOther.myGuidToSlot);
    mySubscribedKindsMask.store(theOther.mySubscribedKindsMask.load(std::memory_order_relaxed),
                                std::memory_order_relaxed);
    mySubscribedRefKindsMask.store(
      theOther.mySubscribedRefKindsMask.load(std::memory_order_relaxed),
      std::memory_order_relaxed);
    myGraph = theOther.myGraph;
    theOther.mySubscribedKindsMask.store(0, std::memory_order_relaxed);
    theOther.mySubscribedRefKindsMask.store(0, std::memory_order_relaxed);
    theOther.myGraph = nullptr;
  }
  return *this;
}

//=================================================================================================

uint32_t BRepGraph_LayerRegistry::RegisterLayer(const occ::handle<BRepGraph_Layer>& theLayer)
{
  std::unique_lock<std::shared_mutex> aLock(myMutex);
  return registerLayerLocked(theLayer);
}

//=================================================================================================

uint32_t BRepGraph_LayerRegistry::registerLayerLocked(const occ::handle<BRepGraph_Layer>& theLayer)
{
  Standard_ProgramError_Raise_if(theLayer.IsNull(),
                                 "BRepGraph_LayerRegistry::RegisterLayer() - null layer");

  const Standard_GUID& aGUID = theLayer->ID();
  const uint32_t*      aSlot = myGuidToSlot.Seek(aGUID);
  if (aSlot != nullptr)
  {
    const occ::handle<BRepGraph_Layer>& aPrev = myLayers.Value(static_cast<size_t>(*aSlot));
    if (!aPrev.IsNull() && aPrev.get() != theLayer.get())
    {
      aPrev->detachContext();
    }
    theLayer->attachGraph(myGraph);
    myLayers.ChangeValue(static_cast<size_t>(*aSlot)) = theLayer;
    recomputeSubscribedKindsMask();
    return *aSlot;
  }

  Standard_OutOfRange_Raise_if(myLayers.Size() > std::numeric_limits<uint32_t>::max(),
                               "BRepGraph_LayerRegistry - too many registered layers");
  const uint32_t aNewSlot = static_cast<uint32_t>(myLayers.Size());
  theLayer->attachGraph(myGraph);
  myLayers.Append(theLayer);
  myGuidToSlot.Bind(aGUID, aNewSlot);
  mySubscribedKindsMask.store(mySubscribedKindsMask.load(std::memory_order_relaxed)
                                | theLayer->SubscribedKinds(),
                              std::memory_order_relaxed);
  mySubscribedRefKindsMask.store(mySubscribedRefKindsMask.load(std::memory_order_relaxed)
                                   | theLayer->SubscribedRefKinds(),
                                 std::memory_order_relaxed);
  return aNewSlot;
}

//=================================================================================================

void BRepGraph_LayerRegistry::UnregisterLayer(const Standard_GUID& theGUID)
{
  std::unique_lock<std::shared_mutex> aLock(myMutex);
  const uint32_t*                     aSlotPtr = myGuidToSlot.Seek(theGUID);
  if (aSlotPtr == nullptr)
  {
    return;
  }

  const uint32_t                     aSlot     = *aSlotPtr;
  const uint32_t                     aLastSlot = static_cast<uint32_t>(myLayers.Size() - 1);
  const occ::handle<BRepGraph_Layer> aRemoved  = myLayers.Value(static_cast<size_t>(aSlot));
  if (aSlot != aLastSlot)
  {
    const occ::handle<BRepGraph_Layer>& aLastLayer = myLayers.Value(static_cast<size_t>(aLastSlot));
    myLayers.ChangeValue(static_cast<size_t>(aSlot)) = aLastLayer;
    myGuidToSlot.ChangeFind(aLastLayer->ID())        = aSlot;
  }

  myLayers.EraseLast();
  myGuidToSlot.UnBind(theGUID);
  recomputeSubscribedKindsMask();
  if (!aRemoved.IsNull())
  {
    aRemoved->detachContext();
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::Attach(BRepGraph* theGraph) noexcept
{
  std::unique_lock<std::shared_mutex> aLock(myMutex);
  myGraph = theGraph;
  for (const occ::handle<BRepGraph_Layer>& aLayer : myLayers)
  {
    if (!aLayer.IsNull())
    {
      aLayer->attachGraph(myGraph);
    }
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::Detach() noexcept
{
  std::unique_lock<std::shared_mutex> aLock(myMutex);
  detachAllLocked();
  myGraph = nullptr;
}

//=================================================================================================

occ::handle<BRepGraph_Layer> BRepGraph_LayerRegistry::FindLayer(const Standard_GUID& theGUID) const
{
  std::shared_lock<std::shared_mutex> aLock(myMutex);
  return findLayerLocked(theGUID);
}

//=================================================================================================

occ::handle<BRepGraph_Layer> BRepGraph_LayerRegistry::findLayerLocked(
  const Standard_GUID& theGUID) const
{
  const uint32_t* aSlot = myGuidToSlot.Seek(theGUID);
  return aSlot != nullptr ? myLayers.Value(static_cast<size_t>(*aSlot))
                          : occ::handle<BRepGraph_Layer>();
}

//=================================================================================================

occ::handle<BRepGraph_Layer> BRepGraph_LayerRegistry::ensureLayer(
  const Standard_GUID&                                 theGUID,
  const std::function<occ::handle<BRepGraph_Layer>()>& theFactory)
{
  // Fast path: shared lock for read-only lookup.
  {
    std::shared_lock<std::shared_mutex> aLock(myMutex);
    occ::handle<BRepGraph_Layer>        aLayer = findLayerLocked(theGUID);
    if (!aLayer.IsNull())
    {
      return aLayer;
    }
  }

  // Slow path: exclusive lock for creation.
  {
    std::unique_lock<std::shared_mutex> aLock(myMutex);
    occ::handle<BRepGraph_Layer>        aLayer = findLayerLocked(theGUID);
    if (aLayer.IsNull())
    {
      aLayer = theFactory();
      registerLayerLocked(aLayer);
    }
    return aLayer;
  }
}

//=================================================================================================

bool BRepGraph_LayerRegistry::FindSlot(const Standard_GUID& theGUID, uint32_t& theSlot) const
{
  std::shared_lock<std::shared_mutex> aLock(myMutex);
  const uint32_t*                     aSlot = myGuidToSlot.Seek(theGUID);
  if (aSlot == nullptr)
  {
    return false;
  }
  theSlot = *aSlot;
  return true;
}

//=================================================================================================

occ::handle<BRepGraph_Layer> BRepGraph_LayerRegistry::Layer(const uint32_t theSlot) const
{
  std::shared_lock<std::shared_mutex> aLock(myMutex);
  if (static_cast<size_t>(theSlot) >= myLayers.Size())
  {
    return occ::handle<BRepGraph_Layer>();
  }
  return myLayers.Value(static_cast<size_t>(theSlot));
}

//=================================================================================================

occ::handle<BRepGraph_Layer> BRepGraph_LayerRegistry::layerAt(const uint32_t theSlot) const
{
  std::shared_lock<std::shared_mutex> aLock(myMutex);
  if (static_cast<size_t>(theSlot) >= myLayers.Size())
  {
    return occ::handle<BRepGraph_Layer>();
  }
  return myLayers.Value(static_cast<size_t>(theSlot));
}

//=================================================================================================

void BRepGraph_LayerRegistry::DispatchOnNodeRemoved(const BRepGraph_NodeId theNode) noexcept
{
  for (uint32_t aSlot = 0;; ++aSlot)
  {
    occ::handle<BRepGraph_Layer> aLayer = layerAt(aSlot);
    if (aLayer.IsNull())
    {
      return;
    }
    aLayer->OnNodeRemoved(theNode);
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::DispatchOnItemRemoved(const BRepGraph_ItemId theItem) noexcept
{
  if (!theItem.IsValid())
  {
    return;
  }

  for (uint32_t aSlot = 0;; ++aSlot)
  {
    occ::handle<BRepGraph_Layer> aLayer = layerAt(aSlot);
    if (aLayer.IsNull())
    {
      return;
    }
    aLayer->OnItemRemoved(theItem);
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::DispatchOnNodeReplaced(const BRepGraph_NodeId theOldNode,
                                                     const BRepGraph_NodeId theNewNode) noexcept
{
  for (uint32_t aSlot = 0;; ++aSlot)
  {
    occ::handle<BRepGraph_Layer> aLayer = layerAt(aSlot);
    if (aLayer.IsNull())
    {
      return;
    }
    aLayer->OnNodeReplaced(theOldNode, theNewNode);
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::DispatchNodeModified(const BRepGraph_NodeId theNode) noexcept
{
  // Lock-free early exit: check subscription mask without mutex.
  if (mySubscribedKindsMask.load(std::memory_order_acquire) == 0)
  {
    return;
  }

  const int aKindBit = BRepGraph_Layer::KindBit(theNode.NodeKind);
  for (uint32_t aSlot = 0;; ++aSlot)
  {
    occ::handle<BRepGraph_Layer> aLayer = layerAt(aSlot);
    if (aLayer.IsNull())
    {
      return;
    }
    if ((aLayer->SubscribedKinds() & aKindBit) != 0)
    {
      aLayer->OnNodeModified(theNode);
    }
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::DispatchItemModified(const BRepGraph_ItemId theItem) noexcept
{
  if (!theItem.IsValid())
  {
    return;
  }

  switch (theItem.ItemDomain())
  {
    case BRepGraph_ItemId::Domain::Node:
      DispatchNodeModified(theItem.NodeId());
      return;
    case BRepGraph_ItemId::Domain::Reference:
      DispatchRefModified(theItem.RefId());
      return;
    case BRepGraph_ItemId::Domain::None:
      return;
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::DispatchNodesModified(
  const NCollection_Array1<BRepGraph_NodeId>& theModifiedNodes,
  const int                                   theModifiedKindsMask) noexcept
{
  // Lock-free early exit: check subscription mask without mutex.
  if (mySubscribedKindsMask.load(std::memory_order_acquire) == 0 || theModifiedKindsMask == 0)
  {
    return;
  }

  for (uint32_t aSlot = 0;; ++aSlot)
  {
    occ::handle<BRepGraph_Layer> aLayer = layerAt(aSlot);
    if (aLayer.IsNull())
    {
      return;
    }
    if ((aLayer->SubscribedKinds() & theModifiedKindsMask) != 0)
    {
      aLayer->OnNodesModified(theModifiedNodes);
    }
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::CopyLayersTo(
  BRepGraph&                                                         theTargetGraph,
  const NCollection_FlatDataMap<BRepGraph_ItemId, BRepGraph_ItemId>& theItemRemap,
  const BRepGraph_CopyRemap::Mode                                    theMode) const
{
  if (theMode == BRepGraph_CopyRemap::Mode::Compact)
  {
    BRepGraph_LayerRegistry&            aSelf = const_cast<BRepGraph_LayerRegistry&>(*this);
    std::unique_lock<std::shared_mutex> aLock(aSelf.myMutex);
    BRepGraph*                          aSourceGraph = myGraph;
    // Compact: collect old layer handles, unregister all, call CopyTo on each
    // which creates fresh layers in the target (same graph) via Ensure<T>().
    NCollection_LinearVector<occ::handle<BRepGraph_Layer>> aOldLayers(myLayers.Size());
    for (const occ::handle<BRepGraph_Layer>& aLayer : myLayers)
    {
      if (!aLayer.IsNull())
      {
        aOldLayers.Append(aLayer);
      }
    }

    aSelf.detachAllLocked();
    aSelf.myLayers.Clear();
    aSelf.myGuidToSlot.Clear();
    aSelf.mySubscribedKindsMask.store(0, std::memory_order_relaxed);
    aSelf.mySubscribedRefKindsMask.store(0, std::memory_order_relaxed);
    aLock.unlock();

    // Call CopyTo on each old (now detached) layer.
    const BRepGraph_CopyRemap aCopy(*aSourceGraph, theTargetGraph, theItemRemap, theMode);
    for (const occ::handle<BRepGraph_Layer>& aLayer : aOldLayers)
    {
      aLayer->CopyTo(aCopy);
    }
  }
  else
  {
    BRepGraph* aSourceGraph = nullptr;
    {
      std::shared_lock<std::shared_mutex> aLock(myMutex);
      aSourceGraph = myGraph;
    }

    // Copy: standard pass-through to each layer's CopyTo.
    const BRepGraph_CopyRemap aCopy(*aSourceGraph, theTargetGraph, theItemRemap, theMode);
    for (uint32_t aSlot = 0;; ++aSlot)
    {
      occ::handle<BRepGraph_Layer> aLayer = layerAt(aSlot);
      if (aLayer.IsNull())
      {
        return;
      }
      aLayer->CopyTo(aCopy);
    }
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::CopyLayersTo(BRepGraph&                       theTargetGraph,
                                           BRepGraph_CopyRemap::MappingKind theMappingKind,
                                           BRepGraph_CopyRemap::Mode        theMode) const
{
  BRepGraph* aSourceGraph = nullptr;
  {
    std::shared_lock<std::shared_mutex> aLock(myMutex);
    aSourceGraph = myGraph;
  }

  const BRepGraph_CopyRemap aCopy(*aSourceGraph, theTargetGraph, theMappingKind, theMode);
  for (uint32_t aSlot = 0;; ++aSlot)
  {
    occ::handle<BRepGraph_Layer> aLayer = layerAt(aSlot);
    if (aLayer.IsNull())
    {
      return;
    }
    aLayer->CopyTo(aCopy);
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::ClearAll() noexcept
{
  for (uint32_t aSlot = 0;; ++aSlot)
  {
    occ::handle<BRepGraph_Layer> aLayer = layerAt(aSlot);
    if (aLayer.IsNull())
    {
      return;
    }
    aLayer->Clear();
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::InvalidateAll() noexcept
{
  for (uint32_t aSlot = 0;; ++aSlot)
  {
    occ::handle<BRepGraph_Layer> aLayer = layerAt(aSlot);
    if (aLayer.IsNull())
    {
      return;
    }
    aLayer->InvalidateAll();
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::DispatchOnRefRemoved(const BRepGraph_RefId theRef) noexcept
{
  for (uint32_t aSlot = 0;; ++aSlot)
  {
    occ::handle<BRepGraph_Layer> aLayer = layerAt(aSlot);
    if (aLayer.IsNull())
    {
      return;
    }
    aLayer->OnRefRemoved(theRef);
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::DispatchRefModified(const BRepGraph_RefId theRef) noexcept
{
  // Lock-free early exit: check subscription mask without mutex.
  if (mySubscribedRefKindsMask.load(std::memory_order_acquire) == 0)
  {
    return;
  }

  const int aRefKindBit = BRepGraph_Layer::RefKindBit(theRef.RefKind);
  for (uint32_t aSlot = 0;; ++aSlot)
  {
    occ::handle<BRepGraph_Layer> aLayer = layerAt(aSlot);
    if (aLayer.IsNull())
    {
      return;
    }
    if ((aLayer->SubscribedRefKinds() & aRefKindBit) != 0)
    {
      aLayer->OnRefModified(theRef);
    }
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::DispatchRefsModified(
  const NCollection_Array1<BRepGraph_RefId>& theModifiedRefs,
  const int                                  theModifiedRefKindsMask) noexcept
{
  // Lock-free early exit: check subscription mask without mutex.
  if (mySubscribedRefKindsMask.load(std::memory_order_acquire) == 0 || theModifiedRefKindsMask == 0)
  {
    return;
  }

  for (uint32_t aSlot = 0;; ++aSlot)
  {
    occ::handle<BRepGraph_Layer> aLayer = layerAt(aSlot);
    if (aLayer.IsNull())
    {
      return;
    }
    if ((aLayer->SubscribedRefKinds() & theModifiedRefKindsMask) != 0)
    {
      aLayer->OnRefsModified(theModifiedRefs);
    }
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::detachAllLocked() noexcept
{
  for (const occ::handle<BRepGraph_Layer>& aLayer : myLayers)
  {
    if (!aLayer.IsNull())
    {
      aLayer->detachContext();
    }
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::recomputeSubscribedKindsMask()
{
  uint32_t aKindsMask    = 0;
  uint32_t aRefKindsMask = 0;
  for (const occ::handle<BRepGraph_Layer>& aLayer : myLayers)
  {
    aKindsMask |= aLayer->SubscribedKinds();
    aRefKindsMask |= aLayer->SubscribedRefKinds();
  }
  mySubscribedKindsMask.store(aKindsMask, std::memory_order_relaxed);
  mySubscribedRefKindsMask.store(aRefKindsMask, std::memory_order_relaxed);
}
