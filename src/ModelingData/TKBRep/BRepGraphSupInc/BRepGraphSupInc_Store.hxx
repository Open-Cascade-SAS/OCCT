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

#ifndef _BRepGraphSupInc_Store_HeaderFile
#define _BRepGraphSupInc_Store_HeaderFile

#include <BRepGraphSupInc_Definition.hxx>
#include <BRepGraphSupInc_ItemUID.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_RevisionComponent.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_FlatDataMap.hxx>
#include <NCollection_Sequence.hxx>

#include <atomic>

class BRepGraphSupInc_CopyContext;
class BRepGraphSupInc_StoreRegistry;
class BRepGraphSupInc_Storage;

//! Polymorphic contract for independently registered supplemental stores.
//! Store implementations own their ItemUID counter space and may use transient dense IDs
//! internally; clients must not retain those IDs across compaction.
class BRepGraphSupInc_Store : public Standard_Transient
{
public:
  //! Return the stable GUID identifying this store type.
  //! @return store-type GUID used for registry lookup
  [[nodiscard]] virtual const Standard_GUID& ID() const = 0;

  //! Return the process-unique runtime identifier assigned by the owning registry, or zero when
  //! unregistered. It is not retained outside the active process.
  [[nodiscard]] uint32_t StoreId() const noexcept { return myStoreId.load(); }

  //! Return the human-readable store name.
  //! @return display name of this store type
  [[nodiscard]] virtual const TCollection_AsciiString& Name() const = 0;

  //! Describe how this store participates in immutable revisions.
  //! The default retention is unspecified and rejects revision creation until
  //! persistence or intentional transience is declared.
  [[nodiscard]] virtual BRepGraph_RevisionComponent::ComponentDescriptor RevisionDescriptor() const
  {
    BRepGraph_RevisionComponent::ComponentDescriptor aDescriptor;
    aDescriptor.StableGUID      = ID();
    aDescriptor.ComponentDomain = BRepGraph_RevisionComponent::Domain::SupplementalStore;
    return aDescriptor;
  }

  //! Capture detached immutable revision for this persistent store.
  [[nodiscard]] virtual occ::handle<BRepGraph_RevisionComponent> CaptureRevision(
    const BRepGraph&,
    BRepGraph_RevisionStatus::Diagnostics&) const
  {
    return occ::handle<BRepGraph_RevisionComponent>();
  }

  //! Find the store-local dense definition ID of an active item.
  //! The returned ID is transient and can change after compaction.
  //! @param[in] theUID runtime item identifier to query
  //! @return matching active dense definition ID, or an invalid ID
  [[nodiscard]] virtual BRepGraphSupInc_DefinitionId FindDefinitionByUID(
    const BRepGraphSupInc_ItemUID& theUID) const = 0;

  //! Test whether this store owns an active item with the UID.
  //! @param[in] theUID runtime item identifier to query
  //! @return true when this store owns an active matching item
  [[nodiscard]] virtual bool Has(const BRepGraphSupInc_ItemUID& theUID) const
  {
    return FindDefinitionByUID(theUID).IsValid();
  }

  //! Test whether the store contains no allocated records, including removed records.
  //! @return true only when the store is empty
  [[nodiscard]] virtual bool IsEmpty() const = 0;

  //! Return store types that must be copied before this store.
  //! @return ordered dependency GUIDs; the default has no dependencies
  [[nodiscard]] virtual const NCollection_Sequence<Standard_GUID>& Dependencies() const
  {
    static const NCollection_Sequence<Standard_GUID> THE_EMPTY;
    return THE_EMPTY;
  }

protected:
  //! Construct a UID in this store's active-process registration space.
  [[nodiscard]] BRepGraphSupInc_ItemUID itemUID(const uint32_t theKind,
                                                const uint32_t theCounter) const noexcept
  {
    return {myStoreId.load(), theKind, theCounter};
  }

  //! Soft-remove an item by runtime UID.
  //! @param[in] theUID runtime item identifier to remove
  //! @return true when an active matching item was removed
  virtual bool Remove(const BRepGraphSupInc_ItemUID& theUID) = 0;

  //! Clear all data while preserving the registered store service and its UID counter space.
  virtual void Clear() noexcept = 0;

  //! Remove inactive records while preserving active item counters.
  //! Transient dense IDs may change.
  virtual void Compact() = 0;

  //! Return true when the store retains inactive records that compaction can discard.
  [[nodiscard]] virtual bool NeedsCompaction() const { return false; }

  //! Remap core-node references after core graph compaction.
  //! Stores without core-node references leave this as a no-op.
  //! @param[in] theNodeMap source-to-target core-node mapping
  virtual void RemapCoreNodes(const NCollection_FlatDataMap<BRepGraph_NodeId, BRepGraph_NodeId>&) {}

  //! Build a detached, compacted store copy while preserving every active runtime UID.
  //! Implementations with compaction-visible state must override this method. Returning null
  //! rejects graph compaction before the source graph is modified.
  [[nodiscard]] virtual occ::handle<BRepGraphSupInc_Store> CloneForCompaction(
    const NCollection_FlatDataMap<BRepGraph_NodeId, BRepGraph_NodeId>&) const
  {
    return occ::handle<BRepGraphSupInc_Store>();
  }

  //! Test whether this source store can copy into a target store.
  //! @param[in] theTarget target store to validate
  //! @return true when the target implements the same store-type contract
  [[nodiscard]] virtual bool IsCompatible(const BRepGraphSupInc_Store& theTarget) const = 0;

  //! Construct an empty store of this type.
  //! The returned store must be compatible with this store and must not be registered.
  //! @return newly allocated empty compatible store
  [[nodiscard]] virtual occ::handle<BRepGraphSupInc_Store> NewEmpty() const = 0;

  //! Copy active store data into the target described by the context.
  //! The implementation controls the representation and copy semantics of its data. It must
  //! allocate target records in the target store and add a context mapping for every copied UID.
  //! @param[in,out] theContext source-to-target mapping and target-store context
  //! @return true when all active records were copied and mapped
  [[nodiscard]] virtual bool CopyTo(BRepGraphSupInc_CopyContext&) const = 0;

  //! Test whether this store has records selected by the mapped core nodes in the context.
  //! Stores without core-owned records leave this as false.
  //! @param[in] theContext source-to-target core-node mapping context
  //! @return true when node-scoped copying requires a target store
  [[nodiscard]] virtual bool HasRecordsForMappedNodes(const BRepGraphSupInc_CopyContext&) const
  {
    return false;
  }

  //! Test whether records selected by mapped core nodes can be copied.
  //! Called during node-copy preflight after node mappings are available and before core mutation.
  //! @param[in] theContext source-to-target mapping and target-store context
  //! @return true when CopyRecordsForMappedNodes() can complete after successful preflight
  [[nodiscard]] virtual bool CanCopyRecordsForMappedNodes(const BRepGraphSupInc_CopyContext&) const
  {
    return true;
  }

  //! Copy records whose core owners have a mapping in the context.
  //! Successful CanCopyRecordsForMappedNodes() preflight is a precondition; this operation must
  //! not report a recoverable failure. Stores without core-owned records leave this as a no-op.
  //! @param[in,out] theContext source-to-target mapping and target-store context
  virtual void CopyRecordsForMappedNodes(BRepGraphSupInc_CopyContext&) const {}

private:
  friend class BRepGraphSupInc_StoreRegistry;
  friend class BRepGraphSupInc_Storage;

  [[nodiscard]] bool bindRegistry(const uint32_t theStoreId) noexcept
  {
    uint32_t anExpectedId = 0;
    return theStoreId != 0
           && myStoreId.compare_exchange_strong(anExpectedId, theStoreId);
  }

  void unbindRegistry(const uint32_t theStoreId) noexcept
  {
    uint32_t anExpectedId = theStoreId;
    (void)myStoreId.compare_exchange_strong(anExpectedId, 0);
  }

private:
  std::atomic<uint32_t> myStoreId{0};

  DEFINE_STANDARD_RTTI_INLINE(BRepGraphSupInc_Store, Standard_Transient)
};

#endif // _BRepGraphSupInc_Store_HeaderFile
