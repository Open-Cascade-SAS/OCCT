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

#include <Standard_OutOfRange.hxx>

//=================================================================================================

int BRepGraph_LayerRegistry::RegisterLayer(const occ::handle<BRepGraph_Layer>& theLayer)
{
  if (theLayer.IsNull())
    return -1;

  const Standard_GUID& aGUID = theLayer->ID();
  const uint32_t*      aSlot = myGuidToSlot.Seek(aGUID);
  if (aSlot != nullptr)
  {
    const occ::handle<BRepGraph_Layer>& aPrev = myLayers.Value(static_cast<size_t>(*aSlot));
    if (!aPrev.IsNull() && aPrev.get() != theLayer.get())
    {
      aPrev->setOwningGraph(nullptr);
    }
    theLayer->setOwningGraph(myOwningGraph);
    myLayers.ChangeValue(static_cast<size_t>(*aSlot)) = theLayer;
    recomputeSubscribedKindsMask();
    return static_cast<int>(*aSlot);
  }

  const uint32_t aNewSlot = static_cast<uint32_t>(myLayers.Size());
  theLayer->setOwningGraph(myOwningGraph);
  myLayers.Append(theLayer);
  myGuidToSlot.Bind(aGUID, aNewSlot);
  mySubscribedKindsMask |= theLayer->SubscribedKinds();
  mySubscribedRefKindsMask |= theLayer->SubscribedRefKinds();
  return static_cast<int>(aNewSlot);
}

//=================================================================================================

void BRepGraph_LayerRegistry::UnregisterLayer(const Standard_GUID& theGUID)
{
  const uint32_t* aSlotPtr = myGuidToSlot.Seek(theGUID);
  if (aSlotPtr == nullptr)
    return;

  const uint32_t                     aSlot     = *aSlotPtr;
  const uint32_t                     aLastSlot = static_cast<uint32_t>(myLayers.Size()) - 1;
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
    aRemoved->setOwningGraph(nullptr);
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::SetOwningGraph(BRepGraph* theGraph) noexcept
{
  myOwningGraph = theGraph;
  for (const occ::handle<BRepGraph_Layer>& aLayer : myLayers)
  {
    if (!aLayer.IsNull())
      aLayer->setOwningGraph(theGraph);
  }
}

//=================================================================================================

occ::handle<BRepGraph_Layer> BRepGraph_LayerRegistry::FindLayer(const Standard_GUID& theGUID) const
{
  const uint32_t* aSlot = myGuidToSlot.Seek(theGUID);
  return aSlot != nullptr ? myLayers.Value(static_cast<size_t>(*aSlot))
                          : occ::handle<BRepGraph_Layer>();
}

//=================================================================================================

int BRepGraph_LayerRegistry::FindSlot(const Standard_GUID& theGUID) const
{
  const uint32_t* aSlot = myGuidToSlot.Seek(theGUID);
  return aSlot != nullptr ? static_cast<int>(*aSlot) : -1;
}

//=================================================================================================

const occ::handle<BRepGraph_Layer>& BRepGraph_LayerRegistry::Layer(const int theSlot) const
{
  Standard_OutOfRange_Raise_if(theSlot < 0 || static_cast<uint32_t>(theSlot) >= myLayers.Size(),
                               "BRepGraph_LayerRegistry::Layer() - invalid slot");
  return myLayers.Value(static_cast<size_t>(theSlot));
}

//=================================================================================================

void BRepGraph_LayerRegistry::DispatchOnNodeRemoved(const BRepGraph_NodeId theNode,
                                                    const BRepGraph_NodeId theReplacement) noexcept
{
  for (const occ::handle<BRepGraph_Layer>& aLayer : myLayers)
  {
    aLayer->OnNodeRemoved(theNode, theReplacement);
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::DispatchNodeModified(const BRepGraph_NodeId theNode) noexcept
{
  if (!HasModificationSubscribers())
    return;

  const int aKindBit = BRepGraph_Layer::KindBit(theNode.NodeKind);
  for (const occ::handle<BRepGraph_Layer>& aLayer : myLayers)
  {
    if ((aLayer->SubscribedKinds() & aKindBit) != 0)
      aLayer->OnNodeModified(theNode);
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::DispatchNodesModified(
  const NCollection_DynamicArray<BRepGraph_NodeId>& theModifiedNodes,
  const int                                         theModifiedKindsMask) noexcept
{
  if (!HasModificationSubscribers() || theModifiedKindsMask == 0)
    return;

  for (const occ::handle<BRepGraph_Layer>& aLayer : myLayers)
  {
    if ((aLayer->SubscribedKinds() & theModifiedKindsMask) != 0)
      aLayer->OnNodesModified(theModifiedNodes);
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::DispatchOnCompact(
  const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theRemapMap) noexcept
{
  for (const occ::handle<BRepGraph_Layer>& aLayer : myLayers)
  {
    aLayer->OnCompact(theRemapMap);
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::ClearAll() noexcept
{
  for (const occ::handle<BRepGraph_Layer>& aLayer : myLayers)
  {
    aLayer->Clear();
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::InvalidateAll() noexcept
{
  for (const occ::handle<BRepGraph_Layer>& aLayer : myLayers)
  {
    aLayer->InvalidateAll();
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::DispatchOnRefRemoved(const BRepGraph_RefId theRef) noexcept
{
  for (const occ::handle<BRepGraph_Layer>& aLayer : myLayers)
  {
    aLayer->OnRefRemoved(theRef);
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::DispatchRefModified(const BRepGraph_RefId theRef) noexcept
{
  if (!HasRefModificationSubscribers())
    return;

  const int aRefKindBit = BRepGraph_Layer::RefKindBit(theRef.RefKind);
  for (const occ::handle<BRepGraph_Layer>& aLayer : myLayers)
  {
    if ((aLayer->SubscribedRefKinds() & aRefKindBit) != 0)
      aLayer->OnRefModified(theRef);
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::DispatchRefsModified(
  const NCollection_DynamicArray<BRepGraph_RefId>& theModifiedRefs,
  const int                                        theModifiedRefKindsMask) noexcept
{
  if (!HasRefModificationSubscribers() || theModifiedRefKindsMask == 0)
    return;

  for (const occ::handle<BRepGraph_Layer>& aLayer : myLayers)
  {
    if ((aLayer->SubscribedRefKinds() & theModifiedRefKindsMask) != 0)
      aLayer->OnRefsModified(theModifiedRefs, theModifiedRefKindsMask);
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::recomputeSubscribedKindsMask()
{
  mySubscribedKindsMask    = 0;
  mySubscribedRefKindsMask = 0;
  for (const occ::handle<BRepGraph_Layer>& aLayer : myLayers)
  {
    mySubscribedKindsMask |= aLayer->SubscribedKinds();
    mySubscribedRefKindsMask |= aLayer->SubscribedRefKinds();
  }
}
