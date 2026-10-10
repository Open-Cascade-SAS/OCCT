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

#include <BRepGraph_LayerSupplementRegistry.hxx>

#include <NCollection_LinearVector.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_ProgramError.hxx>

#include <limits>
#include <mutex>

//=================================================================================================

BRepGraph_LayerSupplementRegistry::BRepGraph_LayerSupplementRegistry() = default;

//=================================================================================================

BRepGraph_LayerSupplementRegistry::BRepGraph_LayerSupplementRegistry(
  BRepGraph_LayerSupplementRegistry&& theOther) noexcept
{
  std::unique_lock<std::shared_mutex> aLock(theOther.myMutex);
  myLayers         = std::move(theOther.myLayers);
  myGuidToSlot     = std::move(theOther.myGuidToSlot);
  myGraph          = theOther.myGraph;
  theOther.myGraph = nullptr;
}

//=================================================================================================

BRepGraph_LayerSupplementRegistry& BRepGraph_LayerSupplementRegistry::operator=(
  BRepGraph_LayerSupplementRegistry&& theOther) noexcept
{
  if (this != &theOther)
  {
    std::unique_lock<std::shared_mutex> aThisLock(myMutex, std::defer_lock);
    std::unique_lock<std::shared_mutex> anOtherLock(theOther.myMutex, std::defer_lock);
    std::lock(aThisLock, anOtherLock);

    clearLayersLocked();
    myLayers         = std::move(theOther.myLayers);
    myGuidToSlot     = std::move(theOther.myGuidToSlot);
    myGraph          = theOther.myGraph;
    theOther.myGraph = nullptr;
  }
  return *this;
}

//=================================================================================================

uint32_t BRepGraph_LayerSupplementRegistry::RegisterLayer(
  const occ::handle<BRepGraph_LayerSupplementBase>& theLayer)
{
  std::unique_lock<std::shared_mutex> aLock(myMutex);
  return registerLayerLocked(theLayer);
}

//=================================================================================================

uint32_t BRepGraph_LayerSupplementRegistry::registerLayerLocked(
  const occ::handle<BRepGraph_LayerSupplementBase>& theLayer)
{
  Standard_ProgramError_Raise_if(theLayer.IsNull(),
                                 "BRepGraph_LayerSupplementRegistry::RegisterLayer() - null layer");

  const Standard_GUID& aGUID = theLayer->ID();
  const uint32_t*      aSlot = myGuidToSlot.Seek(aGUID);
  if (aSlot != nullptr)
  {
    const occ::handle<BRepGraph_LayerSupplementBase>& aPrevious =
      myLayers.Value(static_cast<size_t>(*aSlot));
    if (!aPrevious.IsNull() && aPrevious.get() != theLayer.get())
    {
      aPrevious->detachContext();
    }
    theLayer->attachGraph(myGraph);
    myLayers.ChangeValue(static_cast<size_t>(*aSlot)) = theLayer;
    return *aSlot;
  }

  Standard_OutOfRange_Raise_if(myLayers.Size() > std::numeric_limits<uint32_t>::max(),
                               "BRepGraph_LayerSupplementRegistry - too many registered layers");
  const uint32_t aNewSlot = static_cast<uint32_t>(myLayers.Size());
  theLayer->attachGraph(myGraph);
  myLayers.Append(theLayer);
  myGuidToSlot.Bind(aGUID, aNewSlot);
  return aNewSlot;
}

//=================================================================================================

void BRepGraph_LayerSupplementRegistry::UnregisterLayer(const Standard_GUID& theGUID)
{
  std::unique_lock<std::shared_mutex> aLock(myMutex);
  const uint32_t*                     aSlotPtr = myGuidToSlot.Seek(theGUID);
  if (aSlotPtr == nullptr)
  {
    return;
  }

  const uint32_t aSlot     = *aSlotPtr;
  const uint32_t aLastSlot = static_cast<uint32_t>(myLayers.Size() - 1);
  if (aSlot != aLastSlot)
  {
    const occ::handle<BRepGraph_LayerSupplementBase>& aLastLayer =
      myLayers.Value(static_cast<size_t>(aLastSlot));
    myLayers.ChangeValue(static_cast<size_t>(aSlot)) = aLastLayer;
    myGuidToSlot.ChangeFind(aLastLayer->ID())        = aSlot;
  }

  myLayers.EraseLast();
  myGuidToSlot.UnBind(theGUID);
}

//=================================================================================================

void BRepGraph_LayerSupplementRegistry::Attach(BRepGraph* theGraph) noexcept
{
  std::unique_lock<std::shared_mutex> aLock(myMutex);
  myGraph = theGraph;
  for (const occ::handle<BRepGraph_LayerSupplementBase>& aLayer : myLayers)
  {
    if (!aLayer.IsNull())
    {
      aLayer->attachGraph(myGraph);
    }
  }
}

//=================================================================================================

void BRepGraph_LayerSupplementRegistry::Detach() noexcept
{
  std::unique_lock<std::shared_mutex> aLock(myMutex);
  for (const occ::handle<BRepGraph_LayerSupplementBase>& aLayer : myLayers)
  {
    if (!aLayer.IsNull())
    {
      aLayer->detachContext();
    }
  }
  myGraph = nullptr;
}

//=================================================================================================

void BRepGraph_LayerSupplementRegistry::Relocate(BRepGraph* theGraph) noexcept
{
  std::unique_lock<std::shared_mutex> aLock(myMutex);
  myGraph = theGraph;
  for (const occ::handle<BRepGraph_LayerSupplementBase>& aLayer : myLayers)
  {
    if (!aLayer.IsNull())
    {
      aLayer->relocateGraph(theGraph);
    }
  }
}

//=================================================================================================

occ::handle<BRepGraph_LayerSupplementBase> BRepGraph_LayerSupplementRegistry::FindLayer(
  const Standard_GUID& theGUID) const
{
  std::shared_lock<std::shared_mutex> aLock(myMutex);
  return findLayerLocked(theGUID);
}

//=================================================================================================

occ::handle<BRepGraph_LayerSupplementBase> BRepGraph_LayerSupplementRegistry::findLayerLocked(
  const Standard_GUID& theGUID) const
{
  const uint32_t* aSlot = myGuidToSlot.Seek(theGUID);
  return aSlot != nullptr ? myLayers.Value(static_cast<size_t>(*aSlot))
                          : occ::handle<BRepGraph_LayerSupplementBase>();
}

//=================================================================================================

occ::handle<BRepGraph_LayerSupplementBase> BRepGraph_LayerSupplementRegistry::ensureLayer(
  const Standard_GUID&                                               theGUID,
  const std::function<occ::handle<BRepGraph_LayerSupplementBase>()>& theFactory)
{
  {
    std::shared_lock<std::shared_mutex>        aLock(myMutex);
    occ::handle<BRepGraph_LayerSupplementBase> aLayer = findLayerLocked(theGUID);
    if (!aLayer.IsNull())
    {
      return aLayer;
    }
  }

  std::unique_lock<std::shared_mutex>        aLock(myMutex);
  occ::handle<BRepGraph_LayerSupplementBase> aLayer = findLayerLocked(theGUID);
  if (aLayer.IsNull())
  {
    aLayer = theFactory();
    registerLayerLocked(aLayer);
  }
  return aLayer;
}

//=================================================================================================

bool BRepGraph_LayerSupplementRegistry::FindSlot(const Standard_GUID& theGUID,
                                                 uint32_t&            theSlot) const
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

occ::handle<BRepGraph_LayerSupplementBase> BRepGraph_LayerSupplementRegistry::Layer(
  const uint32_t theSlot) const
{
  return layerAt(theSlot);
}

//=================================================================================================

occ::handle<BRepGraph_LayerSupplementBase> BRepGraph_LayerSupplementRegistry::layerAt(
  const uint32_t theSlot) const
{
  std::shared_lock<std::shared_mutex> aLock(myMutex);
  if (static_cast<size_t>(theSlot) >= myLayers.Size())
  {
    return occ::handle<BRepGraph_LayerSupplementBase>();
  }
  return myLayers.Value(static_cast<size_t>(theSlot));
}

//=================================================================================================

void BRepGraph_LayerSupplementRegistry::DispatchOnItemRemoved(
  const BRepGraph_ItemId theItem) noexcept
{
  if (!theItem.IsValid())
  {
    return;
  }

  for (uint32_t aSlot = 0;; ++aSlot)
  {
    const occ::handle<BRepGraph_LayerSupplementBase> aLayer = layerAt(aSlot);
    if (aLayer.IsNull())
    {
      return;
    }
    aLayer->OnItemRemoved(theItem);
  }
}

//=================================================================================================

void BRepGraph_LayerSupplementRegistry::DispatchOnNodeReplaced(
  const BRepGraph_NodeId theOldNode,
  const BRepGraph_NodeId theNewNode) noexcept
{
  for (uint32_t aSlot = 0;; ++aSlot)
  {
    const occ::handle<BRepGraph_LayerSupplementBase> aLayer = layerAt(aSlot);
    if (aLayer.IsNull())
    {
      return;
    }
    aLayer->OnNodeReplaced(theOldNode, theNewNode);
  }
}

//=================================================================================================

void BRepGraph_LayerSupplementRegistry::DispatchOnSupplementRemoved(
  const BRepGraphSupInc_ItemUID& theUID) noexcept
{
  if (!theUID.IsValid())
  {
    return;
  }

  for (uint32_t aSlot = 0;; ++aSlot)
  {
    const occ::handle<BRepGraph_LayerSupplementBase> aLayer = layerAt(aSlot);
    if (aLayer.IsNull())
    {
      return;
    }
    aLayer->OnSupplementRemoved(theUID);
  }
}

//=================================================================================================

void BRepGraph_LayerSupplementRegistry::ClearAll() noexcept
{
  for (uint32_t aSlot = 0;; ++aSlot)
  {
    const occ::handle<BRepGraph_LayerSupplementBase> aLayer = layerAt(aSlot);
    if (aLayer.IsNull())
    {
      return;
    }
    aLayer->Clear();
  }
}

//=================================================================================================

void BRepGraph_LayerSupplementRegistry::CopyLayersTo(
  BRepGraph&                                                         theTargetGraph,
  const NCollection_FlatDataMap<BRepGraph_ItemId, BRepGraph_ItemId>& theItemRemap,
  const BRepGraph_CopyRemap::Mode                                    theMode,
  const BRepGraphSupInc_CopyContext&                                 theSupplementCopy) const
{
  BRepGraph*                                                           aSourceGraph = nullptr;
  NCollection_LinearVector<occ::handle<BRepGraph_LayerSupplementBase>> aLayers;
  {
    std::shared_lock<std::shared_mutex> aLock(myMutex);
    aSourceGraph = myGraph;
    for (const occ::handle<BRepGraph_LayerSupplementBase>& aLayer : myLayers)
    {
      if (!aLayer.IsNull())
      {
        aLayers.Append(aLayer);
      }
    }
  }
  Standard_ProgramError_Raise_if(
    aSourceGraph == nullptr,
    "BRepGraph_LayerSupplementRegistry::CopyLayersTo() - detached source registry");

  const BRepGraph_CopyRemap aCopy(*aSourceGraph, theTargetGraph, theItemRemap, theMode);
  for (const occ::handle<BRepGraph_LayerSupplementBase>& aLayer : aLayers)
  {
    aLayer->CopySupplementalTo(aCopy, theSupplementCopy);
  }
}

//=================================================================================================

void BRepGraph_LayerSupplementRegistry::CopyLayersTo(
  BRepGraph&                             theTargetGraph,
  const BRepGraph_CopyRemap::MappingKind theMappingKind,
  const BRepGraph_CopyRemap::Mode        theMode,
  const BRepGraphSupInc_CopyContext&     theSupplementCopy) const
{
  BRepGraph*                                                           aSourceGraph = nullptr;
  NCollection_LinearVector<occ::handle<BRepGraph_LayerSupplementBase>> aLayers;
  {
    std::shared_lock<std::shared_mutex> aLock(myMutex);
    aSourceGraph = myGraph;
    for (const occ::handle<BRepGraph_LayerSupplementBase>& aLayer : myLayers)
    {
      if (!aLayer.IsNull())
      {
        aLayers.Append(aLayer);
      }
    }
  }
  Standard_ProgramError_Raise_if(
    aSourceGraph == nullptr,
    "BRepGraph_LayerSupplementRegistry::CopyLayersTo() - detached source registry");

  const BRepGraph_CopyRemap aCopy(*aSourceGraph, theTargetGraph, theMappingKind, theMode);
  for (const occ::handle<BRepGraph_LayerSupplementBase>& aLayer : aLayers)
  {
    aLayer->CopySupplementalTo(aCopy, theSupplementCopy);
  }
}

//=================================================================================================

void BRepGraph_LayerSupplementRegistry::clearLayersLocked() noexcept
{
  for (const occ::handle<BRepGraph_LayerSupplementBase>& aLayer : myLayers)
  {
    if (!aLayer.IsNull())
    {
      aLayer->detachContext();
    }
  }
  myLayers.Clear();
  myGuidToSlot.Clear();
}
