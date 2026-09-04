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

#ifndef _BRepGraphSupInc_Storage_HeaderFile
#define _BRepGraphSupInc_Storage_HeaderFile

#include <BRepGraphSupInc_StoreRegistry.hxx>
#include <BRepGraphSupInc_TopologyStore.hxx>
#include <Standard_DefineAlloc.hxx>

//! Standalone storage facade for registered supplemental-incidence stores.
//! It owns the registry and coordinates supplemental-store copying.
class BRepGraphSupInc_Storage
{
public:
  DEFINE_STANDARD_ALLOC

  //! Process-unique runtime identifier of a registered supplemental store. Zero is invalid.
  //! StoreId is an ItemUID field alias and is not retained outside the active process.
  using StoreId = uint32_t;

  BRepGraphSupInc_Storage()                                          = default;
  BRepGraphSupInc_Storage(const BRepGraphSupInc_Storage&)            = delete;
  BRepGraphSupInc_Storage& operator=(const BRepGraphSupInc_Storage&) = delete;

  BRepGraphSupInc_Storage(BRepGraphSupInc_Storage&&) noexcept            = default;
  BRepGraphSupInc_Storage& operator=(BRepGraphSupInc_Storage&&) noexcept = default;

  //! Return the registered store service collection.
  //! @return immutable registry owned by this storage
  [[nodiscard]] const BRepGraphSupInc_StoreRegistry& Registry() const { return myRegistry; }

  //! Return the registered store service collection for modification.
  //! @return mutable registry owned by this storage
  BRepGraphSupInc_StoreRegistry& ChangeRegistry() { return myRegistry; }

  //! Find an active supplemental definition by runtime UID.
  //! @param[in] theUID runtime item identifier to query
  //! @return store-local dense definition ID, or an invalid ID
  [[nodiscard]] Standard_EXPORT BRepGraphSupInc_DefinitionId
    FindDefinitionByUID(const BRepGraphSupInc_ItemUID& theUID) const;

  //! Test whether a UID names an active record in its registered owning store.
  //! @param[in] theUID runtime item identifier to query
  //! @return true when an active record exists
  [[nodiscard]] Standard_EXPORT bool Has(const BRepGraphSupInc_ItemUID& theUID) const;

  //! Soft-remove an item through its owning store.
  //! @param[in] theUID runtime item identifier to remove
  //! @return true only when an active item was removed.
  Standard_EXPORT bool Remove(const BRepGraphSupInc_ItemUID& theUID);

  //! Add an attachment to the built-in topology store.
  //! @return allocated dense attachment ID, or an invalid ID when the input is invalid
  [[nodiscard]] Standard_EXPORT BRepGraphSupInc_TopologyId
    AddTopology(const BRepGraph_NodeId                        theOwner,
                const BRepGraphSupInc::TopologyAttachmentKind theKind,
                const TopoDS_Shape&                           theShape);

  //! Remove an attachment from the built-in topology store.
  [[nodiscard]] Standard_EXPORT bool RemoveTopology(const BRepGraphSupInc_TopologyId theID);

  //! Remove every attachment owned by a core topology node.
  Standard_EXPORT void RemoveTopologyOwner(const BRepGraph_NodeId theOwner);

  //! Move or remove attachments after a core-node replacement.
  Standard_EXPORT void ReplaceTopologyOwner(const BRepGraph_NodeId theOldOwner,
                                            const BRepGraph_NodeId theNewOwner);

  //! Dispatch core-node remapping to every registered store after a core graph transformation.
  Standard_EXPORT void RemapCoreNodes(
    const NCollection_FlatDataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theNodeMap);

  //! Clear all store data while retaining all registrations and ItemUID counter spaces.
  Standard_EXPORT void Clear() noexcept;

  //! Test whether registered matching stores are compatible for copying.
  //! Source stores without a target registration are created by CopyTo() through NewEmpty().
  //! @return false when a matching target store is incompatible
  [[nodiscard]] Standard_EXPORT bool IsCopySupported(
    const BRepGraphSupInc_Storage& theTarget) const;

  //! Copy active definitions into target stores and populate the supplied UID mapping context.
  //! Every registered compatible source store is copied through its Store contract. Every copied
  //! source UID maps to a newly allocated UID in an independently registered target store.
  //! @param[in,out] theTarget target storage receiving compatible stores and active records
  //! @param[in,out] theContext source-to-target mapping context bound to these storages
  //! @return false when store compatibility, registration, or copying fails
  [[nodiscard]] Standard_EXPORT bool CopyTo(BRepGraphSupInc_Storage&     theTarget,
                                            BRepGraphSupInc_CopyContext& theContext) const;

  //! Validate all registered stores, dependencies, and existing target-store compatibility for a
  //! node-scoped copy without changing target storage. Missing target stores are created and
  //! retained in the context without registration.
  //! @param[in] theTarget target storage
  //! @param[in] theContext source-to-target mapping context bound to these storages
  //! @return false when dependencies, compatibility, or mapped-record preflight is invalid
  [[nodiscard]] Standard_EXPORT bool PreflightRecordsForMappedNodesTo(
    const BRepGraphSupInc_Storage& theTarget,
    BRepGraphSupInc_CopyContext&   theContext) const;

  //! Copy records selected by mapped core nodes through every registered store contract.
  //! Stores without node-scoped ownership perform no work.
  //! @param[in,out] theTarget target storage receiving compatible stores and selected records
  //! @param[in,out] theContext source-to-target mapping context bound to these storages
  //! Successful PreflightRecordsForMappedNodesTo() is a precondition.
  Standard_EXPORT void CopyRecordsForMappedNodesTo(BRepGraphSupInc_Storage&     theTarget,
                                                   BRepGraphSupInc_CopyContext& theContext) const;

  //! Compact every registered store while preserving active runtime UIDs.
  //! Dense IDs may change as removed records are discarded.
  Standard_EXPORT void Compact();

  //! Return true when at least one registered store retains removable records.
  [[nodiscard]] Standard_EXPORT bool NeedsCompaction() const;

  //! Prepare a detached compacted copy in another storage while retaining active ItemUIDs.
  //! The target must be empty. It is updated only after every store is cloned successfully.
  [[nodiscard]] Standard_EXPORT bool CloneCompactedTo(
    BRepGraphSupInc_Storage&                                           theTarget,
    const NCollection_FlatDataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theNodeMap) const;

private:
  BRepGraphSupInc_StoreRegistry myRegistry;
};

#endif // _BRepGraphSupInc_Storage_HeaderFile
