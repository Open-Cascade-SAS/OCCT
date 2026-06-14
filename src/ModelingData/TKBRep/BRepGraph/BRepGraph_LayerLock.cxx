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

#include <BRepGraph_LayerLock.hxx>

#include <BRepGraph_ChildExplorer.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_ParentExplorer.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <Standard_Assert.hxx>
#include <Standard_GUID.hxx>
#include <Standard_ProgramError.hxx>

#include <utility>

IMPLEMENT_STANDARD_RTTIEXT(BRepGraph_LayerLock, BRepGraph_Layer)

namespace
{
using OwnerMap = NCollection_FlatDataMap<BRepGraph_ItemId, Standard_GUID>;
}

//=================================================================================================

BRepGraph_LayerLock::ScopedOwnerEdit::ScopedOwnerEdit(BRepGraph_LayerLock&   theLayer,
                                                      const BRepGraph_ItemId theItem,
                                                      const Standard_GUID&   theOwnerId
                                                      [[maybe_unused]])
    : myLayer(&theLayer),
      myItem(theItem),
      myIsActive(false)
{
  Standard_ProgramError_Raise_if(!theItem.IsValid(),
                                 "BRepGraph_LayerLock::ScopedOwnerEdit: invalid item");
  Standard_GUID anOwnerId [[maybe_unused]];
  Standard_ProgramError_Raise_if(!theLayer.FindOwnerId(theItem, anOwnerId)
                                   || anOwnerId != theOwnerId,
                                 "BRepGraph_LayerLock::ScopedOwnerEdit: owner mismatch");
  theLayer.setItemOwned(theItem, false);
  myIsActive = true;
}

//=================================================================================================

BRepGraph_LayerLock::ScopedOwnerEdit::~ScopedOwnerEdit()
{
  if (myLayer != nullptr && myIsActive)
  {
    myLayer->setItemOwned(myItem, true);
  }
}

//=================================================================================================

BRepGraph_LayerLock::ScopedOwnerEdit::ScopedOwnerEdit(ScopedOwnerEdit&& theOther) noexcept
    : myLayer(theOther.myLayer),
      myItem(theOther.myItem),
      myIsActive(theOther.myIsActive)
{
  theOther.myLayer    = nullptr;
  theOther.myIsActive = false;
}

//=================================================================================================

BRepGraph_LayerLock::ScopedOwnerEdit& BRepGraph_LayerLock::ScopedOwnerEdit::operator=(
  ScopedOwnerEdit&& theOther) noexcept
{
  if (this != &theOther)
  {
    if (myLayer != nullptr && myIsActive)
    {
      myLayer->setItemOwned(myItem, true);
    }
    myLayer             = theOther.myLayer;
    myItem              = theOther.myItem;
    myIsActive          = theOther.myIsActive;
    theOther.myLayer    = nullptr;
    theOther.myIsActive = false;
  }
  return *this;
}

//=================================================================================================

BRepGraph_LayerLock::BRepGraph_LayerLock() = default;

//=================================================================================================

const Standard_GUID& BRepGraph_LayerLock::GetID()
{
  static const Standard_GUID THE_ID("6f4f4d72-2e64-4ad9-8d02-d3833fe4b9d3");
  return THE_ID;
}

//=================================================================================================

const Standard_GUID& BRepGraph_LayerLock::ID() const
{
  return GetID();
}

//=================================================================================================

bool BRepGraph_LayerLock::FindOwnerId(const BRepGraph_ItemId theItem,
                                      Standard_GUID&         theOwnerId) const
{
  if (!theItem.IsValid())
  {
    return false;
  }

  // Refs: check direct entry, then traverse via parent node.
  if (theItem.ItemDomain() == BRepGraph_ItemId::Domain::Reference)
  {
    const Standard_GUID* aDirectId = myRefOwners.Seek(theItem);
    if (aDirectId != nullptr)
    {
      theOwnerId = *aDirectId;
      return true;
    }
    BRepGraph_ItemId aRootItem;
    if (findRootRefId(theItem.RefId(), aRootItem))
    {
      const OwnerMap* aMap =
        aRootItem.ItemDomain() == BRepGraph_ItemId::Domain::Node ? &myNodeOwners : &myRefOwners;
      const Standard_GUID* anOwnerId = aMap->Seek(aRootItem);
      if (anOwnerId != nullptr)
      {
        theOwnerId = *anOwnerId;
        return true;
      }
    }
    return false;
  }

  // Nodes: check direct entry, then traverse upward.
  if (theItem.ItemDomain() == BRepGraph_ItemId::Domain::Node)
  {
    const Standard_GUID* aDirectId = myNodeOwners.Seek(theItem);
    if (aDirectId != nullptr)
    {
      theOwnerId = *aDirectId;
      return true;
    }
    BRepGraph_ItemId aRootItem;
    if (findRootNodeId(theItem.NodeId(), aRootItem))
    {
      const Standard_GUID* anOwnerId = myNodeOwners.Seek(aRootItem);
      if (anOwnerId != nullptr)
      {
        theOwnerId = *anOwnerId;
        return true;
      }
    }
    return false;
  }

  return false;
}

//=================================================================================================

bool BRepGraph_LayerLock::HasOwner(const BRepGraph_ItemId theItem) const
{
  BRepGraph* aGraph = AttachedGraph();
  if (aGraph == nullptr || !theItem.IsValid())
  {
    return false;
  }

  const BRepGraphInc_Storage& aStorage = aGraph->incStorage();

  switch (theItem.ItemDomain())
  {
    case BRepGraph_ItemId::Domain::Node:
      return BRepGraph_NodeId::Visit(theItem.NodeId(), [&](const auto& theTypedId) {
        return aStorage.IsOwned(theTypedId);
      });
    case BRepGraph_ItemId::Domain::Reference:
      return BRepGraph_RefId::Visit(theItem.RefId(), [&](const auto& theTypedId) {
        return aStorage.IsOwned(theTypedId);
      });
    case BRepGraph_ItemId::Domain::None:
      return false;
  }
  return false;
}

//=================================================================================================

void BRepGraph_LayerLock::ReserveOwners(const size_t theNbOwners)
{
  myNodeOwners.Reserve(static_cast<size_t>(myNodeOwners.Extent()) + theNbOwners);
  myRefOwners.Reserve(static_cast<size_t>(myRefOwners.Extent()) + theNbOwners);
}

//=================================================================================================

void BRepGraph_LayerLock::SetOwner(const BRepGraph_ItemId theItem, const Standard_GUID& theOwnerId)
{
  const bool aWasSet = SetOwner(theItem, theOwnerId, true);
  Standard_ASSERT_RAISE(aWasSet, "BRepGraph_LayerLock::SetOwner: rejected or invalid item");
}

//=================================================================================================

bool BRepGraph_LayerLock::SetOwner(const BRepGraph_ItemId theItem,
                                   const Standard_GUID&   theOwnerId,
                                   const bool             theToUpdateRevision)
{
  if (!theItem.IsValid())
  {
    return false;
  }

  // Check if item already has a direct root entry.
  OwnerMap* aMap =
    theItem.ItemDomain() == BRepGraph_ItemId::Domain::Reference ? &myRefOwners : &myNodeOwners;

  Standard_GUID* anExisting = aMap->ChangeSeek(theItem);
  if (anExisting != nullptr)
  {
    // Direct entry exists.
    if (*anExisting == theOwnerId)
    {
      // Same GUID: idempotent re-registration. Ensure bit is set.
      setItemOwned(theItem, true);
      if (theToUpdateRevision)
      {
        touch();
      }
      return false; // No storage change.
    }
    // Different GUID on same item: reject.
    return false;
  }

  Standard_GUID anAncestorOwnerId;
  if (isCoveredByAncestor(theItem, anAncestorOwnerId))
  {
    if (anAncestorOwnerId == theOwnerId)
    {
      setItemOwned(theItem, true);
      if (theToUpdateRevision)
      {
        touch();
      }
    }
    return false;
  }

  NCollection_LinearVector<BRepGraph_ItemId> aSameOwnerDescendantRoots;
  if (theItem.ItemDomain() == BRepGraph_ItemId::Domain::Node)
  {
    BRepGraph* aGraph = AttachedGraph();
    if (aGraph != nullptr)
    {
      for (BRepGraph_ChildExplorer anExp(*aGraph, theItem.NodeId()); anExp.More(); anExp.Next())
      {
        const BRepGraph_ItemId aChildItem(anExp.Current().DefId);
        if (const Standard_GUID* aChildOwner = myNodeOwners.Seek(aChildItem))
        {
          if (*aChildOwner != theOwnerId)
          {
            return false;
          }
          aSameOwnerDescendantRoots.Append(aChildItem);
        }

        const BRepGraph_RefId aRefId = anExp.CurrentRef();
        if (aRefId.IsValid())
        {
          const BRepGraph_ItemId aRefItem(aRefId);
          if (const Standard_GUID* aRefOwner = myRefOwners.Seek(aRefItem))
          {
            if (*aRefOwner != theOwnerId)
            {
              return false;
            }
            aSameOwnerDescendantRoots.Append(aRefItem);
          }
        }
      }
    }
  }

  for (const BRepGraph_ItemId& aDescendantRoot : aSameOwnerDescendantRoots)
  {
    if (aDescendantRoot.ItemDomain() == BRepGraph_ItemId::Domain::Node)
    {
      myNodeOwners.UnBind(aDescendantRoot);
    }
    else if (aDescendantRoot.ItemDomain() == BRepGraph_ItemId::Domain::Reference)
    {
      myRefOwners.UnBind(aDescendantRoot);
    }
  }

  // New root entry.
  aMap->Bind(theItem, theOwnerId);
  setItemOwned(theItem, true);

  // Propagate bit-flag to descendants for nodes.
  if (theItem.ItemDomain() == BRepGraph_ItemId::Domain::Node)
  {
    expandOwnership(theItem.NodeId(), true);
  }

  if (theToUpdateRevision)
  {
    touch();
  }
  return true;
}

//=================================================================================================

void BRepGraph_LayerLock::TouchOwners()
{
  touch();
}

//=================================================================================================

void BRepGraph_LayerLock::UnsetOwner(const BRepGraph_ItemId theItem)
{
  if (!theItem.IsValid())
  {
    return;
  }

  bool wasRemoved = false;

  if (theItem.ItemDomain() == BRepGraph_ItemId::Domain::Node)
  {
    wasRemoved = myNodeOwners.UnBind(theItem);
    if (wasRemoved)
    {
      setItemOwned(theItem, false);
      expandOwnership(theItem.NodeId(), false);
      rebuildOwnedFlagsFromRoots();
    }
  }
  else if (theItem.ItemDomain() == BRepGraph_ItemId::Domain::Reference)
  {
    wasRemoved = myRefOwners.UnBind(theItem);
    if (wasRemoved)
    {
      setItemOwned(theItem, false);
      rebuildOwnedFlagsFromRoots();
    }
  }

  if (wasRemoved)
  {
    touch();
  }
}

//=================================================================================================

void BRepGraph_LayerLock::UnsetOwner(const BRepGraph_ItemId theItem,
                                     const Standard_GUID&   theOwnerId)
{
  if (!theItem.IsValid())
  {
    return;
  }

  // Verify the root entry matches the GUID before unsetting.
  const Standard_GUID* anOwnerId = nullptr;
  if (theItem.ItemDomain() == BRepGraph_ItemId::Domain::Node)
  {
    anOwnerId = myNodeOwners.Seek(theItem);
  }
  else if (theItem.ItemDomain() == BRepGraph_ItemId::Domain::Reference)
  {
    anOwnerId = myRefOwners.Seek(theItem);
  }
  if (anOwnerId == nullptr || *anOwnerId != theOwnerId)
  {
    return;
  }
  UnsetOwner(theItem);
}

//=================================================================================================

const TCollection_AsciiString& BRepGraph_LayerLock::Name() const
{
  static const TCollection_AsciiString THE_NAME("Lock");
  return THE_NAME;
}

//=================================================================================================

void BRepGraph_LayerLock::OnNodeRemoved(const BRepGraph_NodeId theNode) noexcept
{
  const BRepGraph_ItemId anItem(theNode);
  if (myNodeOwners.UnBind(anItem))
  {
    setItemOwned(anItem, false);
    expandOwnership(theNode, false);
    rebuildOwnedFlagsFromRoots();
    touch();
  }
}

//=================================================================================================

void BRepGraph_LayerLock::OnNodeReplaced(const BRepGraph_NodeId theOldNode,
                                         const BRepGraph_NodeId theNewNode) noexcept
{
  const BRepGraph_ItemId anOldItem(theOldNode);
  const BRepGraph_ItemId aNewItem(theNewNode);
  const Standard_GUID*   anOwnerId = myNodeOwners.Seek(anOldItem);
  if (anOwnerId == nullptr)
  {
    return;
  }

  const Standard_GUID anOwnerIdCopy = *anOwnerId;
  myNodeOwners.UnBind(anOldItem);
  myNodeOwners.Bind(aNewItem, anOwnerIdCopy);
  setItemOwned(anOldItem, false);
  setItemOwned(aNewItem, true);
  touch();
}

//=================================================================================================

void BRepGraph_LayerLock::CopyTo(const BRepGraph_CopyRemap& theCopy) const
{
  if (myNodeOwners.IsEmpty() && myRefOwners.IsEmpty())
  {
    return;
  }

  occ::handle<BRepGraph_LayerLock> aTarget =
    theCopy.TargetGraph().LayerRegistry().Ensure<BRepGraph_LayerLock>();
  aTarget->ReserveOwners(static_cast<size_t>(myNodeOwners.Extent() + myRefOwners.Extent()));
  bool hasCopied = false;

  // Copy node root entries.
  for (OwnerMap::Iterator anIt(myNodeOwners); anIt.More(); anIt.Next())
  {
    const BRepGraph_ItemId aTargetItem = theCopy.TargetItem(anIt.Key());
    if (!aTargetItem.IsValid())
    {
      continue;
    }
    hasCopied = aTarget->SetOwner(aTargetItem, anIt.Value(), false) || hasCopied;
  }

  // Copy ref root entries.
  for (OwnerMap::Iterator anIt(myRefOwners); anIt.More(); anIt.Next())
  {
    const BRepGraph_ItemId aTargetItem = theCopy.TargetItem(anIt.Key());
    if (!aTargetItem.IsValid())
    {
      continue;
    }
    hasCopied = aTarget->SetOwner(aTargetItem, anIt.Value(), false) || hasCopied;
  }

  if (hasCopied)
  {
    aTarget->TouchOwners();
  }
}

//=================================================================================================

void BRepGraph_LayerLock::OnRefRemoved(const BRepGraph_RefId theRef) noexcept
{
  const BRepGraph_ItemId anItem(theRef);
  if (myRefOwners.UnBind(anItem))
  {
    setItemOwned(anItem, false);
    touch();
  }
}

//=================================================================================================

void BRepGraph_LayerLock::InvalidateAll() noexcept
{
  Clear();
}

//=================================================================================================

void BRepGraph_LayerLock::Clear() noexcept
{
  // Clear bits on all node root entries and their descendants.
  for (OwnerMap::Iterator anIt(myNodeOwners); anIt.More(); anIt.Next())
  {
    setItemOwned(anIt.Key(), false);
    expandOwnership(anIt.Key().NodeId(), false);
  }
  myNodeOwners.Clear(true);

  // Clear bits on all ref root entries.
  for (OwnerMap::Iterator anIt(myRefOwners); anIt.More(); anIt.Next())
  {
    setItemOwned(anIt.Key(), false);
  }
  myRefOwners.Clear(true);

  touch();
}

//=================================================================================================

void BRepGraph_LayerLock::setItemOwned(const BRepGraph_ItemId theItem, const bool theIsOwned) const
{
  BRepGraph* aGraph = AttachedGraph();
  if (aGraph == nullptr)
  {
    return;
  }

  BRepGraphInc_Storage& aStorage = aGraph->incStorage();

  switch (theItem.ItemDomain())
  {
    case BRepGraph_ItemId::Domain::Node: {
      BRepGraph_NodeId::Visit(theItem.NodeId(), [&](const auto& theTypedId) {
        aStorage.SetOwned(theTypedId, theIsOwned);
      });
      return;
    }
    case BRepGraph_ItemId::Domain::Reference: {
      BRepGraph_RefId::Visit(theItem.RefId(), [&](const auto& theTypedId) {
        aStorage.SetOwned(theTypedId, theIsOwned);
      });
      return;
    }
    case BRepGraph_ItemId::Domain::None:
      return;
  }
}

//=================================================================================================

void BRepGraph_LayerLock::expandOwnership(const BRepGraph_NodeId theRoot, const bool theIsOwned)
{
  BRepGraph* aGraph = AttachedGraph();
  if (aGraph == nullptr)
  {
    return;
  }

  for (BRepGraph_ChildExplorer anExp(*aGraph, theRoot); anExp.More(); anExp.Next())
  {
    // Set/clear bit on child node and on the relation ref used to reach it.
    setItemOwned(BRepGraph_ItemId(anExp.Current().DefId), theIsOwned);
    const BRepGraph_RefId aRef = anExp.CurrentRef();
    if (aRef.IsValid())
    {
      setItemOwned(BRepGraph_ItemId(aRef), theIsOwned);
    }
  }
}

//=================================================================================================

void BRepGraph_LayerLock::rebuildOwnedFlagsFromRoots()
{
  for (OwnerMap::Iterator anIt(myNodeOwners); anIt.More(); anIt.Next())
  {
    setItemOwned(anIt.Key(), true);
    expandOwnership(anIt.Key().NodeId(), true);
  }

  for (OwnerMap::Iterator anIt(myRefOwners); anIt.More(); anIt.Next())
  {
    setItemOwned(anIt.Key(), true);
  }
}

//=================================================================================================

bool BRepGraph_LayerLock::findRootNodeId(const BRepGraph_NodeId theNode,
                                         BRepGraph_ItemId&      theRootItem) const
{
  BRepGraph* aGraph = AttachedGraph();
  if (aGraph == nullptr)
  {
    return false;
  }

  const BRepGraphInc_Storage& aStorage = aGraph->incStorage();

  // Check if this node's bit-flag is set at all.
  bool isOwned = false;
  BRepGraph_NodeId::Visit(theNode,
                          [&](const auto& theTypedId) { isOwned = aStorage.IsOwned(theTypedId); });
  if (!isOwned)
  {
    return false;
  }

  // Check if this node is a root (parent is NOT owned).
  bool parentOwned = false;
  for (auto [anId, aLoc, anOri] :
       BRepGraph_ParentExplorer(*aGraph,
                                theNode,
                                BRepGraph_ParentExplorer::TraversalMode::DirectParents))
  {
    BRepGraph_NodeId::Visit(anId, [&](const auto& theTypedId) {
      if (aStorage.IsOwned(theTypedId))
      {
        parentOwned = true;
      }
    });
    if (parentOwned)
    {
      break;
    }
  }

  if (!parentOwned)
  {
    // This node IS the root.
    theRootItem = BRepGraph_ItemId(theNode);
    return true;
  }

  // Traverse upward to find the root.
  for (auto [anId, aLoc, anOri] : BRepGraph_ParentExplorer(*aGraph, theNode))
  {
    // Check if this ancestor is owned.
    bool ancestorOwned = false;
    BRepGraph_NodeId::Visit(anId, [&](const auto& theTypedId) {
      ancestorOwned = aStorage.IsOwned(theTypedId);
    });
    if (!ancestorOwned)
    {
      // Previous ancestor was the root. We've gone past it.
      break;
    }

    // Check if this ancestor's parent is NOT owned (making it the root).
    bool ancestorParentOwned = false;
    for (auto [aParentId, aPLoc, aPOri] :
         BRepGraph_ParentExplorer(*aGraph,
                                  anId,
                                  BRepGraph_ParentExplorer::TraversalMode::DirectParents))
    {
      BRepGraph_NodeId::Visit(aParentId, [&](const auto& theTypedId) {
        if (aStorage.IsOwned(theTypedId))
        {
          ancestorParentOwned = true;
        }
      });
      if (ancestorParentOwned)
      {
        break;
      }
    }

    if (!ancestorParentOwned)
    {
      theRootItem = BRepGraph_ItemId(anId);
      return true;
    }
  }

  return false;
}

//=================================================================================================

bool BRepGraph_LayerLock::findRootRefId(const BRepGraph_RefId theRef,
                                        BRepGraph_ItemId&     theRootItem) const
{
  // Check direct entry in ref map.
  const BRepGraph_ItemId anItem(theRef);
  if (myRefOwners.IsBound(anItem))
  {
    theRootItem = anItem;
    return true;
  }

  // Get parent node and traverse from there.
  const BRepGraph_NodeId aParentNode = parentNodeId(theRef);
  if (aParentNode.IsValid())
  {
    return findRootNodeId(aParentNode, theRootItem);
  }

  return false;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph_LayerLock::parentNodeId(const BRepGraph_RefId theRef) const
{
  BRepGraph* aGraph = AttachedGraph();
  if (aGraph == nullptr || !theRef.IsValid())
  {
    return BRepGraph_NodeId();
  }

  const BRepGraphInc_Storage& aStorage = aGraph->incStorage();

  return BRepGraph_RefId::Visit(theRef, [&](const auto& theTypedRef) -> BRepGraph_NodeId {
    using RefT = std::decay_t<decltype(theTypedRef)>;
    if constexpr (std::is_same_v<RefT, BRepGraph_ShellRefId>)
    {
      if (theTypedRef.IsValid(aStorage.NbShellRefs()))
      {
        return aStorage.ShellRef(theTypedRef).ParentSolidId;
      }
    }
    else if constexpr (std::is_same_v<RefT, BRepGraph_FaceRefId>)
    {
      if (theTypedRef.IsValid(aStorage.NbFaceRefs()))
      {
        return aStorage.FaceRef(theTypedRef).ParentShellId;
      }
    }
    else if constexpr (std::is_same_v<RefT, BRepGraph_WireRefId>)
    {
      if (theTypedRef.IsValid(aStorage.NbWireRefs()))
      {
        return aStorage.WireRef(theTypedRef).ParentFaceId;
      }
    }
    else if constexpr (std::is_same_v<RefT, BRepGraph_SolidRefId>)
    {
      if (theTypedRef.IsValid(aStorage.NbSolidRefs()))
      {
        return BRepGraph_NodeId(aStorage.SolidRef(theTypedRef).ParentCompSolidId);
      }
    }
    else if constexpr (std::is_same_v<RefT, BRepGraph_ChildRefId>)
    {
      if (theTypedRef.IsValid(aStorage.NbChildRefs()))
      {
        return BRepGraph_NodeId(aStorage.ChildRef(theTypedRef).ParentCompoundId);
      }
    }
    else if constexpr (std::is_same_v<RefT, BRepGraph_OccurrenceRefId>)
    {
      if (theTypedRef.IsValid(aStorage.NbOccurrenceRefs()))
      {
        return BRepGraph_NodeId(aStorage.OccurrenceRef(theTypedRef).ParentProductId);
      }
    }
    else if constexpr (std::is_same_v<RefT, BRepGraph_VertexRefId>)
    {
      if (theTypedRef.IsValid(aStorage.NbVertexRefs()))
      {
        return BRepGraph_NodeId(aStorage.VertexRef(theTypedRef).ParentEdgeId);
      }
    }
    return BRepGraph_NodeId();
  });
}

//=================================================================================================

bool BRepGraph_LayerLock::isCoveredByAncestor(const BRepGraph_ItemId theItem,
                                              Standard_GUID&         theRootOwnerId) const
{
  if (theItem.ItemDomain() == BRepGraph_ItemId::Domain::Node)
  {
    BRepGraph* aGraph = AttachedGraph();
    if (aGraph == nullptr)
    {
      return false;
    }

    for (auto [anId, aLoc, anOri] : BRepGraph_ParentExplorer(*aGraph, theItem.NodeId()))
    {
      const BRepGraph_ItemId anAncestorItem(anId);
      const Standard_GUID*   anOwnerId = myNodeOwners.Seek(anAncestorItem);
      if (anOwnerId != nullptr)
      {
        theRootOwnerId = *anOwnerId;
        return true;
      }
    }
    return false;
  }

  if (theItem.ItemDomain() == BRepGraph_ItemId::Domain::Reference)
  {
    const BRepGraph_NodeId aParentNode = parentNodeId(theItem.RefId());
    if (aParentNode.IsValid())
    {
      // Check if parent is a root.
      const BRepGraph_ItemId aParentItem(aParentNode);
      const Standard_GUID*   anOwnerId = myNodeOwners.Seek(aParentItem);
      if (anOwnerId != nullptr)
      {
        theRootOwnerId = *anOwnerId;
        return true;
      }
      // Check ancestors of parent.
      return isCoveredByAncestor(aParentItem, theRootOwnerId);
    }
    return false;
  }

  return false;
}

//=================================================================================================

void BRepGraph_LayerLock::removeRootEntry(const BRepGraph_ItemId theItem) noexcept
{
  if (theItem.ItemDomain() == BRepGraph_ItemId::Domain::Node)
  {
    if (myNodeOwners.UnBind(theItem))
    {
      setItemOwned(theItem, false);
      touch();
    }
  }
  else if (theItem.ItemDomain() == BRepGraph_ItemId::Domain::Reference)
  {
    if (myRefOwners.UnBind(theItem))
    {
      setItemOwned(theItem, false);
      touch();
    }
  }
}
