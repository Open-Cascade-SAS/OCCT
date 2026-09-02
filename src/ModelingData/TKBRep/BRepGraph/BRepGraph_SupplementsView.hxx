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

#ifndef _BRepGraph_SupplementsView_HeaderFile
#define _BRepGraph_SupplementsView_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraphSupInc_Storage.hxx>
#include <BRepGraphSupInc_TopologyDefinition.hxx>

//! Access graph-owned optional supplemental definition stores.
//! Graph-level supplemental mutation is performed through this view.
class BRepGraph::SupplementsView
{
public:
  explicit SupplementsView(BRepGraph* theGraph = nullptr)
      : myGraph(theGraph)
  {
  }

  //! Register an independently configured supplemental store.
  Standard_EXPORT bool RegisterStore(const occ::handle<BRepGraphSupInc_Store>& theStore);

  //! Find an optional registered store by stable store GUID.
  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraphSupInc_Store> FindStore(
    const Standard_GUID& theID) const;

  //! Find or create the typed store in this graph.
  template <typename StoreT>
  [[nodiscard]] occ::handle<StoreT> EnsureStore()
  {
    return storage().ChangeRegistry().EnsureStore<StoreT>();
  }

  //! Unregister an empty store.
  Standard_EXPORT bool UnregisterStore(const Standard_GUID& theID);

  //! Find an active supplemental definition by runtime UID.
  //! @param[in] theUID runtime item identifier to query
  //! @return store-local dense definition ID, or an invalid ID
  [[nodiscard]] Standard_EXPORT BRepGraphSupInc_DefinitionId FindDefinitionByUID(
    const BRepGraphSupInc_ItemUID& theUID) const;

  //! Return true when an active supplemental definition exists.
  [[nodiscard]] Standard_EXPORT bool Has(const BRepGraphSupInc_ItemUID& theUID) const;

  //! Remove a supplemental definition and notify registered graph layers.
  Standard_EXPORT bool Remove(const BRepGraphSupInc_ItemUID& theUID);

  //! Attach a non-null shape to an active compatible core owner.
  [[nodiscard]] Standard_EXPORT BRepGraphSupInc_TopologyId Attach(
    const BRepGraph_NodeId theOwner,
    const BRepGraphSupInc::TopologyAttachmentKind theKind,
    const TopoDS_Shape& theShape);

  //! Return topology attachment ids in owner insertion order.
  [[nodiscard]] Standard_EXPORT const NCollection_LinearVector<BRepGraphSupInc_TopologyId>&
    Attachments(const BRepGraph_NodeId theOwner) const;

  //! Return active topology attachment data.
  [[nodiscard]] Standard_EXPORT const BRepGraphSupInc::TopologyDef* Attachment(
    const BRepGraphSupInc_TopologyId theID) const;

  //! Return the direct shape of an attachment.
  [[nodiscard]] Standard_EXPORT const TopoDS_Shape* AttachmentShape(
    const BRepGraphSupInc::TopologyDef& theAttachment) const;

  //! Remove one topology attachment and invalidate its owner shape cache.
  Standard_EXPORT bool RemoveAttachment(const BRepGraphSupInc_TopologyId theID);

  //! Remove all topology attachments owned by a removed core node.
  Standard_EXPORT void RemoveOwnerAttachments(const BRepGraph_NodeId theOwner);

  //! Move same-kind owner attachments or remove them for an incompatible replacement.
  Standard_EXPORT void ReplaceOwnerAttachments(const BRepGraph_NodeId theOldOwner,
                                                const BRepGraph_NodeId theNewOwner);

  //! Compact all supplemental stores while preserving active-process UIDs.
  Standard_EXPORT void Compact();

private:
  [[nodiscard]] Standard_EXPORT BRepGraphSupInc_Storage& storage() const;

private:
  BRepGraph* myGraph = nullptr;
};

#endif // _BRepGraph_SupplementsView_HeaderFile
