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

#ifndef _BRepGraph_LayerLock_HeaderFile
#define _BRepGraph_LayerLock_HeaderFile

#include <BRepGraph_ItemId.hxx>
#include <BRepGraph_Layer.hxx>
#include <NCollection_FlatDataMap.hxx>
#include <TCollection_AsciiString.hxx>

//! Owner metadata layer for owned BRepGraph items.
//!
//! Uses a root-based ownership model: only the highest owned item per group
//! is stored in the map. All descendants receive the fast IsOwned bit-flag
//! via automatic downward propagation. Owner lookup traverses upward to
//! find the root entry.
//!
//! Overlapping roots are forbidden: SetOwner rejects if the item is already
//! covered by an ancestor root with a different GUID.
//!
//! HasOwner() checks the IsOwned bit-flag (O(1)).
//! FindOwnerId() traverses upward to find the root entry (O(depth)).
class BRepGraph_LayerLock : public BRepGraph_Layer
{
public:
  //! Scoped permission for an owner layer to edit one item it owns.
  //!
  //! The scope traverses upward to find the root owner for GUID verification,
  //! then temporarily clears the fast owned bit on the specific item so existing
  //! editor mutation APIs can be reused by the owning layer.
  class ScopedOwnerEdit
  {
  public:
    Standard_EXPORT ScopedOwnerEdit(BRepGraph_LayerLock&   theLayer,
                                    const BRepGraph_ItemId theItem,
                                    const Standard_GUID&   theOwnerId);
    Standard_EXPORT ~ScopedOwnerEdit();

    ScopedOwnerEdit(const ScopedOwnerEdit&)            = delete;
    ScopedOwnerEdit& operator=(const ScopedOwnerEdit&) = delete;

    Standard_EXPORT                  ScopedOwnerEdit(ScopedOwnerEdit&& theOther) noexcept;
    Standard_EXPORT ScopedOwnerEdit& operator=(ScopedOwnerEdit&& theOther) noexcept;

  private:
    BRepGraph_LayerLock* myLayer = nullptr;
    BRepGraph_ItemId     myItem;
    bool                 myIsActive = false;
  };

  //! Create lock-owner storage.
  Standard_EXPORT BRepGraph_LayerLock();

  //! Return fixed layer type GUID.
  [[nodiscard]] Standard_EXPORT static const Standard_GUID& GetID();

  //! Return this layer type GUID.
  [[nodiscard]] Standard_EXPORT const Standard_GUID& ID() const override;

  //! Return owner ID for an item.
  //! Traverses upward for nodes/refs to find the root owner entry.
  //! @return true when the item has a resolved owner and @p theOwnerId was filled.
  [[nodiscard]] Standard_EXPORT bool FindOwnerId(const BRepGraph_ItemId theItem,
                                                 Standard_GUID&         theOwnerId) const;

  //! Return owner ID for a node.
  [[nodiscard]] bool FindOwnerId(const BRepGraph_NodeId theNode, Standard_GUID& theOwnerId) const
  {
    return FindOwnerId(BRepGraph_ItemId(theNode), theOwnerId);
  }

  //! Return owner ID for a reference.
  [[nodiscard]] bool FindOwnerId(const BRepGraph_RefId theRef, Standard_GUID& theOwnerId) const
  {
    return FindOwnerId(BRepGraph_ItemId(theRef), theOwnerId);
  }

  //! Return true if an item's IsOwned bit-flag is set.
  //! This is an O(1) check. Use FindOwnerId() to resolve the actual owner GUID.
  [[nodiscard]] Standard_EXPORT bool HasOwner(const BRepGraph_ItemId theItem) const;

  //! Return true if a node's IsOwned bit-flag is set.
  [[nodiscard]] bool HasOwner(const BRepGraph_NodeId theNode) const
  {
    return HasOwner(BRepGraph_ItemId(theNode));
  }

  //! Return true if a reference's IsOwned bit-flag is set.
  [[nodiscard]] bool HasOwner(const BRepGraph_RefId theRef) const
  {
    return HasOwner(BRepGraph_ItemId(theRef));
  }

  //! Register an owner ID and set the graph item's ownership flag.
  //! For nodes, propagates the IsOwned bit-flag to all descendants.
  //! Rejects if the item is already covered by an ancestor root with a different GUID.
  Standard_EXPORT void SetOwner(const BRepGraph_ItemId theItem, const Standard_GUID& theOwnerId);

  //! Register an owner ID and set the graph item's ownership flag.
  //! Returns true when owner storage changed. Revision update can be deferred by bulk callers.
  Standard_EXPORT bool SetOwner(const BRepGraph_ItemId theItem,
                                const Standard_GUID&   theOwnerId,
                                const bool             theToUpdateRevision);

  //! Register an owner ID and set the node ownership flag.
  void SetOwner(const BRepGraph_NodeId theNode, const Standard_GUID& theOwnerId)
  {
    SetOwner(BRepGraph_ItemId(theNode), theOwnerId);
  }

  //! Register an owner ID and set the reference ownership flag.
  void SetOwner(const BRepGraph_RefId theRef, const Standard_GUID& theOwnerId)
  {
    SetOwner(BRepGraph_ItemId(theRef), theOwnerId);
  }

  //! Remove an owner and clear the graph item's ownership flag.
  //! For node roots, clears the IsOwned bit-flag on all descendants.
  Standard_EXPORT void UnsetOwner(const BRepGraph_ItemId theItem);

  //! Remove an owner and clear the graph item's ownership flag if owner ID matches.
  Standard_EXPORT void UnsetOwner(const BRepGraph_ItemId theItem, const Standard_GUID& theOwnerId);

  //! Remove an owner and clear the node ownership flag.
  void UnsetOwner(const BRepGraph_NodeId theNode) { UnsetOwner(BRepGraph_ItemId(theNode)); }

  //! Remove an owner and clear the reference ownership flag.
  void UnsetOwner(const BRepGraph_RefId theRef) { UnsetOwner(BRepGraph_ItemId(theRef)); }

  //! Return true if at least one root entry exists.
  [[nodiscard]] bool HasOwners() const
  {
    return myNodeOwners.Extent() != 0 || myRefOwners.Extent() != 0;
  }

  //! Reserve owner map buckets for bulk registration.
  Standard_EXPORT void ReserveOwners(const size_t theNbOwners);

  //! Mark owner metadata changed after a bulk update.
  Standard_EXPORT void TouchOwners();

  Standard_EXPORT const TCollection_AsciiString& Name() const override;
  Standard_EXPORT void OnNodeRemoved(const BRepGraph_NodeId theNode) noexcept override;
  Standard_EXPORT void OnNodeReplaced(const BRepGraph_NodeId theOldNode,
                                      const BRepGraph_NodeId theNewNode) noexcept override;
  Standard_EXPORT void CopyTo(const BRepGraph_CopyRemap& theCopy) const override;
  Standard_EXPORT void OnRefRemoved(const BRepGraph_RefId theRef) noexcept override;
  Standard_EXPORT void InvalidateAll() noexcept override;
  Standard_EXPORT void Clear() noexcept override;

  DEFINE_STANDARD_RTTIEXT(BRepGraph_LayerLock, BRepGraph_Layer)

private:
  //! Set or clear the IsOwned bit-flag on a single item.
  Standard_EXPORT void setItemOwned(const BRepGraph_ItemId theItem, const bool theIsOwned) const;

  //! Propagate the IsOwned bit-flag to all descendants of a root node.
  //! Walks nodes via ChildExplorer, refs via CurrentRef(), and reps via definition fields.
  void expandOwnership(const BRepGraph_NodeId theRoot, const bool theIsOwned);

  //! Rebuild fast IsOwned bit-flags from remaining root maps.
  void rebuildOwnedFlagsFromRoots();

  //! Find the root node entry that covers a given node.
  //! Checks direct map entry first, then traverses upward via ParentExplorer.
  [[nodiscard]] bool findRootNodeId(const BRepGraph_NodeId theNode,
                                    BRepGraph_ItemId&      theRootItem) const;

  //! Find the root entry that covers a given ref.
  //! Checks direct map entry first, then traverses via parent node.
  [[nodiscard]] bool findRootRefId(const BRepGraph_RefId theRef,
                                   BRepGraph_ItemId&     theRootItem) const;

  //! Get the parent node of a ref from its storage struct.
  [[nodiscard]] BRepGraph_NodeId parentNodeId(const BRepGraph_RefId theRef) const;

  //! Check if an item is already covered by an ancestor root.
  //! Returns true if an ancestor root exists. If so, fills theRootOwnerId.
  [[nodiscard]] bool isCoveredByAncestor(const BRepGraph_ItemId theItem,
                                         Standard_GUID&         theRootOwnerId) const;

  //! Remove a root entry from the appropriate map and clear its bit-flag.
  void removeRootEntry(const BRepGraph_ItemId theItem) noexcept;

  NCollection_FlatDataMap<BRepGraph_ItemId, Standard_GUID> myNodeOwners;
  NCollection_FlatDataMap<BRepGraph_ItemId, Standard_GUID> myRefOwners;
};

#endif // _BRepGraph_LayerLock_HeaderFile
