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

// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.

#ifndef _BRepGraphSupInc_TopologyStore_HeaderFile
#define _BRepGraphSupInc_TopologyStore_HeaderFile

#include <BRepGraphSupInc_Store.hxx>
#include <BRepGraphSupInc_TopologyDefinition.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_FlatDataMap.hxx>
#include <NCollection_LinearVector.hxx>

//! Dense graph-owned topology attachment store with stable active-process UIDs.
//! Each attachment owns a supplemental shape and belongs to a core topology node. Dense
//! topology IDs are transient positions that may change after compaction. CopyTo() copies each
//! attachment's TopoDS_Shape value as store data; core copy policies do not alter it.
class BRepGraphSupInc_TopologyStore : public BRepGraphSupInc_Store
{
public:
  DEFINE_STANDARD_RTTIEXT(BRepGraphSupInc_TopologyStore, BRepGraphSupInc_Store)

  //! Return the stable GUID of this store type.
  //! @return store-type GUID used for registry lookup
  [[nodiscard]] Standard_EXPORT static const Standard_GUID& GetID();
  //! Item-kind discriminator used by TopologyDef ItemUID values in this store.
  static constexpr uint32_t THE_TOPOLOGY_KIND = 1;

  //! Return this store's stable GUID.
  //! @return store-type GUID
  [[nodiscard]] const Standard_GUID& ID() const override { return GetID(); }

  //! Return this store's display name.
  //! @return display name
  [[nodiscard]] Standard_EXPORT const TCollection_AsciiString& Name() const override;
  //! Find an active attachment by runtime UID as a generic dense definition ID.
  //! @param[in] theUID runtime item identifier to query
  //! @return matching dense definition ID, or an invalid ID
  [[nodiscard]] Standard_EXPORT BRepGraphSupInc_DefinitionId
    FindDefinitionByUID(const BRepGraphSupInc_ItemUID& theUID) const override;

  //! Test whether no allocated attachment records remain.
  //! @return true when the store is empty
  [[nodiscard]] bool IsEmpty() const override { return myItems.IsEmpty(); }

protected:
  //! Soft-remove an active attachment by runtime UID.
  //! @param[in] theUID runtime attachment identifier to remove
  //! @return true when an active attachment was removed
  Standard_EXPORT bool Remove(const BRepGraphSupInc_ItemUID& theUID) override;
  //! Clear all attachments while retaining runtime UID counter allocation history.
  Standard_EXPORT void Clear() noexcept override;
  //! Discard removed records while preserving active runtime UIDs and rebuilding dense IDs.
  Standard_EXPORT void Compact() override;

  [[nodiscard]] bool NeedsCompaction() const override { return myItems.Size() != myActiveCount; }

  //! Remap attachment owners after a core graph transformation.
  //! Active attachments without a mapping are removed.
  //! @param[in] theNodeMap source-to-target core-node mapping
  Standard_EXPORT void RemapCoreNodes(
    const NCollection_FlatDataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theNodeMap) override;
  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraphSupInc_Store> CloneForCompaction(
    const NCollection_FlatDataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theNodeMap) const override;
  //! Test whether a target store has the same type and schema.
  //! @param[in] theTarget target store to validate
  //! @return true when the target is compatible
  [[nodiscard]] Standard_EXPORT bool IsCompatible(
    const BRepGraphSupInc_Store& theTarget) const override;
  //! Create an empty store of this type and schema.
  //! @return new empty topology store
  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraphSupInc_Store> NewEmpty() const override;
  //! Copy active attachments after their owners can be mapped, including their direct shape values.
  //! @param[in,out] theContext source-to-target copy context
  //! @return true when every active attachment is copied and mapped
  [[nodiscard]] Standard_EXPORT bool CopyTo(BRepGraphSupInc_CopyContext& theContext) const override;
  //! Test whether any active attachment owner is mapped in the context.
  //! @param[in] theContext source-to-target copy context
  //! @return true when node-scoped copying needs this store
  [[nodiscard]] Standard_EXPORT bool HasRecordsForMappedNodes(
    const BRepGraphSupInc_CopyContext& theContext) const override;
  //! Copy active attachments whose owners are mapped in the context.
  //! @param[in,out] theContext source-to-target copy context
  Standard_EXPORT void CopyRecordsForMappedNodes(
    BRepGraphSupInc_CopyContext& theContext) const override;

public:
  //! Find an active attachment by dense ID.
  //! @param[in] theID transient dense attachment ID
  //! @return active attachment or null when the ID is invalid or removed
  [[nodiscard]] Standard_EXPORT const BRepGraphSupInc::TopologyDef* Find(
    const BRepGraphSupInc_TopologyId theID) const;
  //! Find an active attachment by runtime UID.
  //! This typed lookup complements FindDefinitionByUID().
  //! @param[in] theUID runtime attachment identifier to find
  //! @return matching dense ID or an invalid ID
  [[nodiscard]] Standard_EXPORT BRepGraphSupInc_TopologyId
    FindByUID(const BRepGraphSupInc_ItemUID& theUID) const;
  //! Return the runtime UID of an active attachment.
  //! @param[in] theID transient dense attachment ID
  //! @return runtime attachment UID or an invalid UID
  [[nodiscard]] Standard_EXPORT BRepGraphSupInc_ItemUID
    ItemUID(const BRepGraphSupInc_TopologyId theID) const;
  //! Return active attachment dense IDs owned by a core topology node.
  //! The returned reference is invalidated by any mutation of this store.
  //! @param[in] theOwner core topology node to query
  //! @return attachment ID vector, or a shared empty vector when none are attached
  [[nodiscard]] Standard_EXPORT const NCollection_LinearVector<BRepGraphSupInc_TopologyId>&
                                      AttachedTo(const BRepGraph_NodeId theOwner) const;
  //! Validate active records' owner-kind and shape.
  //! @throws Standard_ProgramError when an active record is inconsistent
  Standard_EXPORT void Validate() const;

  //! Return the number of allocated attachment records, including removed records.
  //! @return allocated attachment count
  [[nodiscard]] uint32_t Count() const { return static_cast<uint32_t>(myItems.Size()); }

private:
  friend class BRepGraphSupInc_Storage;

  //! Add an attachment with a non-null shape.
  //! @param[in] theOwner owning core topology node
  //! @param[in] theKind semantic role compatible with the owner node kind
  //! @param[in] theShape non-null attached shape
  //! @return newly allocated dense ID or an invalid ID for invalid ownership or shape
  Standard_EXPORT BRepGraphSupInc_TopologyId
    Add(const BRepGraph_NodeId                        theOwner,
        const BRepGraphSupInc::TopologyAttachmentKind theKind,
        const TopoDS_Shape&                           theShape);
  //! Soft-remove an active attachment by dense ID.
  //! @param[in] theID transient dense attachment ID
  //! @return true when an active attachment was removed
  Standard_EXPORT bool Remove(const BRepGraphSupInc_TopologyId theID);
  //! Soft-remove every attachment owned by a core topology node.
  //! @param[in] theOwner core topology node whose attachments are removed
  Standard_EXPORT void RemoveOwner(const BRepGraph_NodeId theOwner);
  //! Transfer attachments from one owner to a same-kind replacement node.
  //! Attachments are removed when either owner is invalid or their node kinds differ.
  //! @param[in] theOldOwner current core topology node
  //! @param[in] theNewOwner replacement core topology node
  Standard_EXPORT void                     ReplaceOwner(const BRepGraph_NodeId theOldOwner,
                                                        const BRepGraph_NodeId theNewOwner);
  [[nodiscard]] static bool                isSupportedOwner(const BRepGraph_NodeId::Kind theKind);
  [[nodiscard]] static bool                isCompatible(const BRepGraph_NodeId::Kind                  theOwnerKind,
                                                        const BRepGraphSupInc::TopologyAttachmentKind theKind);
  [[nodiscard]] uint32_t                   allocateCounter();
  [[nodiscard]] BRepGraphSupInc_TopologyId append(
    const BRepGraph_NodeId                        theOwner,
    const BRepGraphSupInc::TopologyAttachmentKind theKind,
    const TopoDS_Shape&                           theShape,
    const uint32_t                                theOwnGen,
    const uint32_t                                theUID = 0);
  void rebuildIndices();

  NCollection_LinearVector<BRepGraphSupInc::TopologyDef> myItems;
  NCollection_DataMap<BRepGraph_NodeId, NCollection_LinearVector<BRepGraphSupInc_TopologyId>>
                                                                myOwnerToItems;
  NCollection_FlatDataMap<uint32_t, BRepGraphSupInc_TopologyId> myUIDToID;
  NCollection_LinearVector<BRepGraphSupInc_TopologyId>          myEmptyItems;
  uint32_t                                                      myActiveCount = 0;
  uint32_t                                                      myNextCounter = 1;
};

#endif // _BRepGraphSupInc_TopologyStore_HeaderFile
