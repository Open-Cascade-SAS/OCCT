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

#ifndef _BRepGraph_BackRefManager_HeaderFile
#define _BRepGraph_BackRefManager_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_RelEdge.hxx>
#include <Standard_DefineAlloc.hxx>

class BRepGraph;

//! @brief Centralized back-reference maintenance for BRepGraph.
//!
//! Manages relation edges. Edge-to-wire reverse mapping is provided
//! by BRepGraphInc_ReverseIndex (rebuilt after mutation).
class BRepGraph_BackRefManager
{
public:
  DEFINE_STANDARD_ALLOC

  // --- RelEdge back-refs ---

  //! Add a directed relation edge between two nodes, updating both
  //! outgoing and incoming maps.
  //! @param[in,out] theGraph     graph to update
  //! @param[in] theFrom          source node
  //! @param[in] theTo            target node
  //! @param[in] theKind          semantic kind of the relation edge
  //! @return index of the new edge in the outgoing vector
  static Standard_EXPORT int AddRelEdge(BRepGraph&                    theGraph,
                                        const BRepGraph_NodeId        theFrom,
                                        const BRepGraph_NodeId        theTo,
                                        const BRepGraph_RelEdge::Kind theKind);

  //! Remove all relation edges of a given kind between two nodes.
  //! @param[in,out] theGraph     graph to update
  //! @param[in] theFrom          source node
  //! @param[in] theTo            target node
  //! @param[in] theKind          semantic kind to match
  static Standard_EXPORT void RemoveRelEdges(BRepGraph&                    theGraph,
                                             const BRepGraph_NodeId        theFrom,
                                             const BRepGraph_NodeId        theTo,
                                             const BRepGraph_RelEdge::Kind theKind);

  //! Remove all relation edges (both outgoing and incoming) for a node.
  //! @param[in,out] theGraph     graph to update
  //! @param[in] theNode          node to clear
  static Standard_EXPORT void ClearRelEdges(BRepGraph& theGraph, const BRepGraph_NodeId theNode);

  // --- Bulk ---

  //! Clear all back-reference containers (geometry back-refs, edge-to-wire map,
  //! and relation edge maps).
  //! @param[in,out] theGraph     graph to clear back-refs for
  static Standard_EXPORT void ClearAll(BRepGraph& theGraph);

  //! Clear all back-references and rebuild them by scanning all definitions.
  //! @param[in,out] theGraph     graph to rebuild back-refs for
  static Standard_EXPORT void RebuildAll(BRepGraph& theGraph);

private:
  BRepGraph_BackRefManager() = delete;
};

#endif // _BRepGraph_BackRefManager_HeaderFile
