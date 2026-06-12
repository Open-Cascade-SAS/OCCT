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

#ifndef _BRepGraph_UIDsView_HeaderFile
#define _BRepGraph_UIDsView_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraph_VersionStamp.hxx>

class Standard_GUID;

//! @brief Read-only view for persistent node and reference identifiers.
//!
//! UIDs are (Kind, Counter) pairs that persist across graph mutations
//! (Compact, node removal). Counters are monotonic and independent of vector
//! indices. Clear() starts a new graph generation and refreshes the graph
//! GUID, enabling stale-reference detection when a graph is rebuilt.
//! Provides bidirectional NodeId/UID and RefId/RefUID resolution.
//!
//! Version stamps are exposed here for graph-owned cache and layer freshness
//! checks. They reuse node/reference UID identity and do not introduce a
//! persistent representation identity.
class BRepGraph::UIDsView
{
public:
  //! Return the UID assigned to a node.
  //! @param[in] theNode node identifier
  //! @return UID for the active node, or invalid UID if theNode is out of bounds or removed
  [[nodiscard]] Standard_EXPORT BRepGraph_UID Of(const BRepGraph_NodeId theNode) const;

  //! Return the RefUID assigned to a reference.
  //! @param[in] theRefId reference identifier
  //! @return RefUID for the active reference, or invalid RefUID if theRefId is out of bounds or
  //! removed
  [[nodiscard]] Standard_EXPORT BRepGraph_RefUID Of(const BRepGraph_RefId theRefId) const;

  //! Return the persistent UID assigned to a generic graph item.
  //! @param[in] theItem definition-node or reference-entry item id
  //! @return durable item UID, or invalid UID if the item is out of bounds or removed
  [[nodiscard]] Standard_EXPORT BRepGraph_ItemUID Of(const BRepGraph_ItemId theItem) const;

  //! Resolve a UID back to a NodeId using the internal reverse index.
  //! @param[in] theUID unique identifier to resolve
  //! @return corresponding active NodeId, or invalid NodeId if not found/removed
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId NodeIdFrom(const BRepGraph_UID& theUID) const;

  //! Resolve a RefUID back to a RefId using the internal reverse index.
  //! @param[in] theUID unique reference identifier to resolve
  //! @return corresponding active RefId, or invalid RefId if not found/removed
  [[nodiscard]] Standard_EXPORT BRepGraph_RefId RefIdFrom(const BRepGraph_RefUID& theUID) const;

  //! Resolve a generic item UID back to a transient item id.
  //! @param[in] theUID durable node/reference item identity
  //! @return active item id, or invalid item id if the UID cannot be resolved
  [[nodiscard]] Standard_EXPORT BRepGraph_ItemId ItemIdFrom(const BRepGraph_ItemUID& theUID) const;

  //! Check if a UID is valid and exists in this graph generation.
  //! @param[in] theUID unique identifier to check
  //! @return true if the UID resolves to an active node in this graph generation
  [[nodiscard]] Standard_EXPORT bool Has(const BRepGraph_UID& theUID) const;

  //! Check if a RefUID is valid and exists in this graph generation.
  //! @param[in] theUID unique reference identifier to check
  //! @return true if the RefUID resolves to an active reference in this graph generation
  [[nodiscard]] Standard_EXPORT bool Has(const BRepGraph_RefUID& theUID) const;

  //! Check if a generic item UID exists in this graph generation.
  [[nodiscard]] Standard_EXPORT bool Has(const BRepGraph_ItemUID& theUID) const;

  //! Return the current generation counter (incremented on each BRepGraph::Clear()).
  //! @return graph generation number
  [[nodiscard]] Standard_EXPORT uint32_t Generation() const;

  //! Return the graph-level identity GUID.
  //! Generated randomly at BRepGraph::Clear() time; changes on each rebuild.
  //! @return reference to the graph identity GUID
  [[nodiscard]] Standard_EXPORT const Standard_GUID& GraphGUID() const;

  //! Produce a version stamp for the given node.
  //! Combines the node's UID with its current OwnGen and graph Generation.
  //! @param[in] theNode node identifier
  //! @return version stamp, or invalid stamp if theNode is invalid, removed, or out of bounds
  [[nodiscard]] Standard_EXPORT BRepGraph_VersionStamp
    StampOf(const BRepGraph_NodeId theNode) const;

  //! Produce a version stamp for the given reference.
  //! Combines the reference's RefUID with its current OwnGen and graph Generation.
  //! @param[in] theRefId reference identifier
  //! @return version stamp, or invalid stamp if theRefId is invalid, removed, or out of bounds
  [[nodiscard]] Standard_EXPORT BRepGraph_VersionStamp
    StampOf(const BRepGraph_RefId theRefId) const;

  //! Produce a version stamp for an owner-scoped use record.
  //! Use records have no durable UID or mutation generation; the stamp uses the owning
  //! definition-node UID, OwnGen, and graph Generation.
  //! @param[in] theRepId use-record identifier
  //! @return version stamp, or invalid stamp if theRepId is invalid, removed, or out of bounds
  [[nodiscard]] Standard_EXPORT BRepGraph_VersionStamp
    StampOf(const BRepGraph_RepId theRepId) const;

  //! Produce a version stamp for the given definition-node or reference-entry item.
  [[nodiscard]] Standard_EXPORT BRepGraph_VersionStamp
    StampOf(const BRepGraph_ItemId theItem) const;

  //! Check if a previously-taken stamp is stale.
  //! A stamp is stale when the stamped item has been mutated,
  //! removed, or the graph was rebuilt since the stamp was taken.
  //! @param[in] theStamp version stamp to check
  //! @return true if the stamp no longer matches the current graph state
  [[nodiscard]] Standard_EXPORT bool IsStale(const BRepGraph_VersionStamp& theStamp) const;

private:
  friend class BRepGraph;
  friend struct BRepGraph_Data;

  explicit UIDsView(BRepGraph* theGraph)
      : myGraph(theGraph)
  {
  }

  BRepGraph* myGraph;
};

#endif // _BRepGraph_UIDsView_HeaderFile
