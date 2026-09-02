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

#include <cstdint>

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
    //! Enter scoped owner edit.
    //! @param[in,out] theLayer owner layer
    //! @param[in] theItem owned item to edit
    //! @param[in] theOwnerId owner GUID that must match the resolved owner root
    Standard_EXPORT ScopedOwnerEdit(BRepGraph_LayerLock&   theLayer,
                                    const BRepGraph_ItemId theItem,
                                    const Standard_GUID&   theOwnerId);

    //! Leave scoped owner edit and restore the item ownership flag.
    Standard_EXPORT ~ScopedOwnerEdit();

    ScopedOwnerEdit(const ScopedOwnerEdit&)            = delete;
    ScopedOwnerEdit& operator=(const ScopedOwnerEdit&) = delete;

    //! Move scoped owner edit.
    //! @param[in,out] theOther source scope
    Standard_EXPORT                  ScopedOwnerEdit(ScopedOwnerEdit&& theOther) noexcept;

    //! Move scoped owner edit.
    //! @param[in,out] theOther source scope
    //! @return this scope.
    Standard_EXPORT ScopedOwnerEdit& operator=(ScopedOwnerEdit&& theOther) noexcept;

  private:
    BRepGraph_LayerLock* myLayer = nullptr;
    BRepGraph_ItemId     myItem;
    bool                 myIsActive = false;
  };

  //! Create lock-owner storage.
  Standard_EXPORT BRepGraph_LayerLock();

  //! Return fixed layer type GUID.
  //! @return fixed layer type GUID.
  [[nodiscard]] Standard_EXPORT static const Standard_GUID& GetID();

  //! Return this layer type GUID.
  //! @return this layer type GUID.
  [[nodiscard]] Standard_EXPORT const Standard_GUID& ID() const override;

  //! Return owner ID for an item.
  //! Traverses upward for nodes/refs to find the root owner entry.
  //! @param[in] theItem graph item
  //! @param[out] theOwnerId resolved owner GUID
  //! @return true when the item has a resolved owner and output owner GUID was filled.
  [[nodiscard]] Standard_EXPORT bool FindOwnerId(const BRepGraph_ItemId theItem,
                                                 Standard_GUID&         theOwnerId) const;

  //! Return root owner item and owner ID for an item.
  //! Traverses ancestor relations and resolves the stored owner root directly.
  //! @param[in] theItem graph item
  //! @param[out] theRootItem resolved root owner item
  //! @param[out] theOwnerId resolved owner GUID
  //! @return true when the item has a resolved owner root.
  [[nodiscard]] Standard_EXPORT bool FindOwnerRoot(const BRepGraph_ItemId theItem,
                                                   BRepGraph_ItemId&      theRootItem,
                                                   Standard_GUID&         theOwnerId) const;

  //! Return owner ID for a node.
  //! @param[in] theNode graph node
  //! @param[out] theOwnerId resolved owner GUID
  //! @return true when the node has a resolved owner.
  [[nodiscard]] bool FindOwnerId(const BRepGraph_NodeId theNode, Standard_GUID& theOwnerId) const
  {
    return FindOwnerId(BRepGraph_ItemId(theNode), theOwnerId);
  }

  //! Return owner ID for a reference.
  //! @param[in] theRef graph reference
  //! @param[out] theOwnerId resolved owner GUID
  //! @return true when the reference has a resolved owner.
  [[nodiscard]] bool FindOwnerId(const BRepGraph_RefId theRef, Standard_GUID& theOwnerId) const
  {
    return FindOwnerId(BRepGraph_ItemId(theRef), theOwnerId);
  }

  //! Check an item's IsOwned bit-flag.
  //! This is an O(1) check. Use FindOwnerId() to resolve the actual owner GUID.
  //! @param[in] theItem graph item
  //! @return true if the item is owned.
  [[nodiscard]] Standard_EXPORT bool HasOwner(const BRepGraph_ItemId theItem) const;

  //! Check a node's IsOwned bit-flag.
  //! @param[in] theNode graph node
  //! @return true if the node is owned.
  [[nodiscard]] bool HasOwner(const BRepGraph_NodeId theNode) const
  {
    return HasOwner(BRepGraph_ItemId(theNode));
  }

  //! Check a reference's IsOwned bit-flag.
  //! @param[in] theRef graph reference
  //! @return true if the reference is owned.
  [[nodiscard]] bool HasOwner(const BRepGraph_RefId theRef) const
  {
    return HasOwner(BRepGraph_ItemId(theRef));
  }

  //! Register an owner ID and set the graph item's ownership flag.
  //! For nodes, propagates the IsOwned bit-flag to all descendants.
  //! Rejects if the item is already covered by an ancestor root with a different GUID.
  //! @param[in] theItem graph item
  //! @param[in] theOwnerId owner GUID
  Standard_EXPORT void SetOwner(const BRepGraph_ItemId theItem, const Standard_GUID& theOwnerId);

  //! Register an owner ID and set the graph item's ownership flag.
  //! Generation update can be deferred by bulk callers.
  //! @param[in] theItem graph item
  //! @param[in] theOwnerId owner GUID
  //! @param[in] theToUpdateGeneration true to update layer generation immediately
  //! @return true if owner storage changed.
  Standard_EXPORT bool SetOwner(const BRepGraph_ItemId theItem,
                                const Standard_GUID&   theOwnerId,
                                const bool             theToUpdateGeneration);

  //! Register an owner ID and set the node ownership flag.
  //! @param[in] theNode graph node
  //! @param[in] theOwnerId owner GUID
  void SetOwner(const BRepGraph_NodeId theNode, const Standard_GUID& theOwnerId)
  {
    SetOwner(BRepGraph_ItemId(theNode), theOwnerId);
  }

  //! Register an owner ID and set the reference ownership flag.
  //! @param[in] theRef graph reference
  //! @param[in] theOwnerId owner GUID
  void SetOwner(const BRepGraph_RefId theRef, const Standard_GUID& theOwnerId)
  {
    SetOwner(BRepGraph_ItemId(theRef), theOwnerId);
  }

  //! Remove an owner and clear the graph item's ownership flag.
  //! For node roots, clears the IsOwned bit-flag on all descendants.
  //! @param[in] theItem graph item
  Standard_EXPORT void UnsetOwner(const BRepGraph_ItemId theItem);

  //! Remove an owner and clear the graph item's ownership flag if owner ID matches.
  //! @param[in] theItem graph item
  //! @param[in] theOwnerId owner GUID
  Standard_EXPORT void UnsetOwner(const BRepGraph_ItemId theItem, const Standard_GUID& theOwnerId);

  //! Remove an owner and clear the node ownership flag.
  //! @param[in] theNode graph node
  void UnsetOwner(const BRepGraph_NodeId theNode) { UnsetOwner(BRepGraph_ItemId(theNode)); }

  //! Remove an owner and clear the reference ownership flag.
  //! @param[in] theRef graph reference
  void UnsetOwner(const BRepGraph_RefId theRef) { UnsetOwner(BRepGraph_ItemId(theRef)); }

  //! Check whether at least one root owner entry exists.
  //! @return true if at least one root owner entry exists.
  [[nodiscard]] bool HasOwners() const
  {
    return !myNodeOwners.IsEmpty() || !myRefOwners.IsEmpty();
  }

  //! Reserve owner map buckets for bulk registration.
  //! @param[in] theNbOwners expected number of owner roots
  Standard_EXPORT void ReserveOwners(const size_t theNbOwners);

  //! Mark owner metadata changed after a bulk update.
  Standard_EXPORT void TouchOwners();

  //! Begin a bulk owner update.
  //!
  //! Owner bits are cleared immediately, while rebuilding bits covered by the
  //! remaining roots is postponed until EndBulkOwnerUpdate().
  Standard_EXPORT void BeginBulkOwnerUpdate();

  //! Finish a bulk owner update and rebuild ownership flags once when needed.
  Standard_EXPORT void EndBulkOwnerUpdate();

  //! Return layer type name.
  //! @return layer type name.
  Standard_EXPORT const TCollection_AsciiString& Name() const override;

  //! Remove owner data for a removed node.
  //! @param[in] theNode removed node
  Standard_EXPORT void OnNodeRemoved(const BRepGraph_NodeId theNode) noexcept override;

  //! Migrate owner data after node replacement.
  //! @param[in] theOldNode replaced node
  //! @param[in] theNewNode replacement node
  Standard_EXPORT void OnNodeReplaced(const BRepGraph_NodeId theOldNode,
                                      const BRepGraph_NodeId theNewNode) noexcept override;

  //! Copy owner data to the target graph.
  //! @param[in] theCopy source, target, and item remap context
  Standard_EXPORT void CopyTo(const BRepGraph_CopyRemap& theCopy) const override;

  //! Remove owner data for a removed reference.
  //! @param[in] theRef removed reference
  Standard_EXPORT void OnRefRemoved(const BRepGraph_RefId theRef) noexcept override;

  //! Mark all owner data dirty.
  Standard_EXPORT void InvalidateAll() noexcept override;

  //! Remove all owner data.
  Standard_EXPORT void Clear() noexcept override;

  DEFINE_STANDARD_RTTIEXT(BRepGraph_LayerLock, BRepGraph_Layer)

private:
  //! Set or clear the IsOwned bit-flag on a single item.
  //! @param[in] theItem graph item
  //! @param[in] theIsOwned ownership flag value
  Standard_EXPORT void setItemOwned(const BRepGraph_ItemId theItem, const bool theIsOwned) const;

  //! Propagate the IsOwned bit-flag to all descendants of a root node.
  //! Walks nodes via ChildExplorer, refs via CurrentRef(), and reps via definition fields.
  //! @param[in] theRoot root node
  //! @param[in] theIsOwned ownership flag value
  void expandOwnership(const BRepGraph_NodeId theRoot, const bool theIsOwned);

  //! Rebuild fast IsOwned bit-flags from remaining root maps.
  void rebuildOwnedFlagsFromRoots();

  //! Find the root node entry that covers a given node.
  //! Checks direct map entry first, then traverses upward via ParentExplorer.
  //! @param[in] theNode graph node
  //! @param[out] theRootItem resolved root owner item
  //! @return true if a root owner item covers the node.
  [[nodiscard]] bool findRootNodeId(const BRepGraph_NodeId theNode,
                                    BRepGraph_ItemId&      theRootItem) const;

  //! Find the root entry that covers a given ref.
  //! Checks direct map entry first, then traverses via parent node.
  //! @param[in] theRef graph reference
  //! @param[out] theRootItem resolved root owner item
  //! @return true if a root owner item covers the reference.
  [[nodiscard]] bool findRootRefId(const BRepGraph_RefId theRef,
                                   BRepGraph_ItemId&     theRootItem) const;

  //! Get the parent node of a ref from its storage struct.
  //! @param[in] theRef graph reference
  //! @return parent node id, or invalid node id if none exists.
  [[nodiscard]] BRepGraph_NodeId parentNodeId(const BRepGraph_RefId theRef) const;

  //! Check if an item is already covered by an ancestor root.
  //! Fill the root owner GUID output when an ancestor root exists.
  //! @param[in] theItem graph item
  //! @param[out] theRootOwnerId resolved ancestor owner GUID
  //! @return true if an ancestor owner root covers the item.
  [[nodiscard]] bool isCoveredByAncestor(const BRepGraph_ItemId theItem,
                                         Standard_GUID&         theRootOwnerId) const;

  //! Remove a root entry from the appropriate map and clear its bit-flag.
  //! @param[in] theItem owner root item
  void removeRootEntry(const BRepGraph_ItemId theItem) noexcept;

  NCollection_FlatDataMap<BRepGraph_ItemId, Standard_GUID> myNodeOwners;
  NCollection_FlatDataMap<BRepGraph_ItemId, Standard_GUID> myRefOwners;
  uint32_t                                                  myBulkOwnerUpdateDepth = 0;
  bool                                                      myHasBulkOwnerChanges  = false;
};

#endif // _BRepGraph_LayerLock_HeaderFile
