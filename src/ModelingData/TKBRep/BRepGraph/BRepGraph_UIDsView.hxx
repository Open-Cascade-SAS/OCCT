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

//! @brief Read-only view for persistent unique identifiers.
//!
//! UIDs are (Kind, Counter) pairs that persist across graph mutations
//! (Compact, node removal). Each UID is assigned exactly once and never
//! reused. The Generation field enables stale-reference detection when
//! a graph is rebuilt. Provides bidirectional NodeId/UID resolution.
//! Obtained via BRepGraph::UIDs().
class BRepGraph::UIDsView
{
public:
  //! Return the UID assigned to a node.
  //! @param[in] theNode node identifier
  //! @return UID for the node, or invalid UID if theNode is out of bounds
  [[nodiscard]] Standard_EXPORT BRepGraph_UID Of(const BRepGraph_NodeId theNode) const;

  //! Resolve a UID back to a NodeId (O(n) linear scan).
  //! @param[in] theUID unique identifier to resolve
  //! @return corresponding NodeId, or invalid NodeId if not found
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId NodeIdFrom(const BRepGraph_UID& theUID) const;

  //! Check if a UID is valid and exists in this graph generation.
  //! @param[in] theUID unique identifier to check
  //! @return true if the UID belongs to this graph generation
  [[nodiscard]] Standard_EXPORT bool Has(const BRepGraph_UID& theUID) const;

  //! Return the current generation counter (incremented on each Build).
  //! @return graph generation number
  [[nodiscard]] Standard_EXPORT uint32_t Generation() const;

  //! Return the graph-level identity GUID.
  //! Generated randomly at Build() time; changes on each rebuild.
  //! @return reference to the graph identity GUID
  [[nodiscard]] Standard_EXPORT const Standard_GUID& GraphGUID() const;

  //! Produce a version stamp for the given node.
  //! Combines the node's UID with its current OwnGen and graph Generation.
  //! @param[in] theNode node identifier
  //! @return version stamp, or invalid stamp if theNode is invalid, removed, or out of bounds
  [[nodiscard]] Standard_EXPORT BRepGraph_VersionStamp
    StampOf(const BRepGraph_NodeId theNode) const;

  //! Check if a previously-taken stamp is stale.
  //! A stamp is stale when the node has been mutated, removed,
  //! or the graph was rebuilt since the stamp was taken.
  //! @param[in] theStamp version stamp to check
  //! @return true if the stamp no longer matches the current node state
  [[nodiscard]] Standard_EXPORT bool IsStale(const BRepGraph_VersionStamp& theStamp) const;

private:
  friend class BRepGraph;
  friend struct BRepGraph_Data;

  explicit UIDsView(const BRepGraph* theGraph)
      : myGraph(theGraph)
  {
  }

  const BRepGraph* myGraph;
};

#endif // _BRepGraph_UIDsView_HeaderFile
