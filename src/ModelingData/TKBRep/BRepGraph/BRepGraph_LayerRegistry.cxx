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
  const int*           aSlot = myGuidToSlot.Seek(aGUID);
  if (aSlot != nullptr)
  {
    myLayers.ChangeValue(*aSlot) = theLayer;
    recomputeSubscribedKindsMask();
    return *aSlot;
  }

  const int aNewSlot = myLayers.Length();
  myLayers.Append(theLayer);
  myGuidToSlot.Bind(aGUID, aNewSlot);
  mySubscribedKindsMask |= theLayer->SubscribedKinds();
  return aNewSlot;
}

//=================================================================================================

void BRepGraph_LayerRegistry::UnregisterLayer(const Standard_GUID& theGUID)
{
  const int* aSlotPtr = myGuidToSlot.Seek(theGUID);
  if (aSlotPtr == nullptr)
    return;

  const int aSlot     = *aSlotPtr;
  const int aLastSlot = myLayers.Length() - 1;
  if (aSlot != aLastSlot)
  {
    const occ::handle<BRepGraph_Layer>& aLastLayer = myLayers.Value(aLastSlot);
    myLayers.ChangeValue(aSlot)                    = aLastLayer;
    myGuidToSlot.ChangeFind(aLastLayer->ID())      = aSlot;
  }

  myLayers.EraseLast();
  myGuidToSlot.UnBind(theGUID);
  recomputeSubscribedKindsMask();
}

//=================================================================================================

occ::handle<BRepGraph_Layer> BRepGraph_LayerRegistry::FindLayer(const Standard_GUID& theGUID) const
{
  const int* aSlot = myGuidToSlot.Seek(theGUID);
  return aSlot != nullptr ? myLayers.Value(*aSlot) : occ::handle<BRepGraph_Layer>();
}

//=================================================================================================

int BRepGraph_LayerRegistry::FindSlot(const Standard_GUID& theGUID) const
{
  const int* aSlot = myGuidToSlot.Seek(theGUID);
  return aSlot != nullptr ? *aSlot : -1;
}

//=================================================================================================

const occ::handle<BRepGraph_Layer>& BRepGraph_LayerRegistry::Layer(const int theSlot) const
{
  Standard_OutOfRange_Raise_if(
    theSlot < 0 || theSlot >= myLayers.Length(),
    "BRepGraph_LayerRegistry::Layer() - invalid slot");
  return myLayers.Value(theSlot);
}

//=================================================================================================

void BRepGraph_LayerRegistry::DispatchOnNodeRemoved(const BRepGraph_NodeId theNode,
                                                    const BRepGraph_NodeId theReplacement) noexcept
{
  for (int aLayerIdx = 0; aLayerIdx < myLayers.Length(); ++aLayerIdx)
  {
    myLayers.Value(aLayerIdx)->OnNodeRemoved(theNode, theReplacement);
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::DispatchNodeModified(const BRepGraph_NodeId theNode) noexcept
{
  if (!HasModificationSubscribers())
    return;

  const int aKindBit = BRepGraph_Layer::KindBit(theNode.NodeKind);
  for (int aLayerIdx = 0; aLayerIdx < myLayers.Length(); ++aLayerIdx)
  {
    const occ::handle<BRepGraph_Layer>& aLayer = myLayers.Value(aLayerIdx);
    if ((aLayer->SubscribedKinds() & aKindBit) != 0)
      aLayer->OnNodeModified(theNode);
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::DispatchNodesModified(
  const NCollection_Vector<BRepGraph_NodeId>& theModifiedNodes,
  const int                                   theModifiedKindsMask) noexcept
{
  if (!HasModificationSubscribers() || theModifiedKindsMask == 0)
    return;

  for (int aLayerIdx = 0; aLayerIdx < myLayers.Length(); ++aLayerIdx)
  {
    const occ::handle<BRepGraph_Layer>& aLayer = myLayers.Value(aLayerIdx);
    if ((aLayer->SubscribedKinds() & theModifiedKindsMask) != 0)
      aLayer->OnNodesModified(theModifiedNodes);
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::DispatchOnCompact(
  const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theRemapMap) noexcept
{
  for (int aLayerIdx = 0; aLayerIdx < myLayers.Length(); ++aLayerIdx)
  {
    myLayers.Value(aLayerIdx)->OnCompact(theRemapMap);
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::ClearAll() noexcept
{
  for (int aLayerIdx = 0; aLayerIdx < myLayers.Length(); ++aLayerIdx)
  {
    myLayers.Value(aLayerIdx)->Clear();
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::InvalidateAll() noexcept
{
  for (int aLayerIdx = 0; aLayerIdx < myLayers.Length(); ++aLayerIdx)
  {
    myLayers.Value(aLayerIdx)->InvalidateAll();
  }
}

//=================================================================================================

void BRepGraph_LayerRegistry::recomputeSubscribedKindsMask()
{
  mySubscribedKindsMask = 0;
  for (int aLayerIdx = 0; aLayerIdx < myLayers.Length(); ++aLayerIdx)
  {
    mySubscribedKindsMask |= myLayers.Value(aLayerIdx)->SubscribedKinds();
  }
}
