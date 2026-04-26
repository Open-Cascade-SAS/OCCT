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

#ifndef _BRepGraph_ShapesView_HeaderFile
#define _BRepGraph_ShapesView_HeaderFile

#include <BRepGraph.hxx>

//! @brief Read-only view for TopoDS_Shape reconstruction from graph data.
//!
//! Reconstructs TopoDS shapes from graph nodes on demand, with caching
//! for repeated access. Topology nodes are delegated to the incidence-table
//! reconstruction backend, while Product / Occurrence nodes are assembled at
//! the facade level using product-local roots and occurrence placement chains.
//! Provides lookup from original construction-time shapes back to their graph
//! NodeIds via TShape pointer comparison. Shape() is the stable cached public
//! route for repeated access; Reconstruct() forces a fresh rebuild with the
//! same node-kind semantics and bypasses the persistent reconstructed-shape cache.
//! BRepGraph_Builder::Add() and Compact() clear the persistent reconstructed-shape cache.
//! Obtained via BRepGraph::Shapes().
class BRepGraph::ShapesView
{
public:
  //! Return or reconstruct a TopoDS_Shape for a node.
  //! Prefer this route for repeated public queries.
  //! Returns a cached shape when available and valid; otherwise reconstructs.
  //! Topology definition nodes (Vertex..CompSolid) reconstruct their topology
  //! directly, without assembly wrappers.
  //! Product nodes are reconstructed in product-local coordinates.
  //! Occurrence nodes are reconstructed with cumulative occurrence placement.
  //! @param[in] theNode node identifier
  //! @return corresponding TopoDS_Shape, or null shape for invalid/removed nodes
  [[nodiscard]] Standard_EXPORT TopoDS_Shape Shape(const BRepGraph_NodeId theNode) const;

  //! Check if the node has an original shape from graph construction.
  //! Editor-created and mutation-derived nodes have no original.
  //! @param[in] theNode node identifier
  //! @return true if an original shape exists
  [[nodiscard]] Standard_EXPORT bool HasOriginal(const BRepGraph_NodeId theNode) const;

  //! Return a pointer to the original TopoDS_Shape stored during graph construction.
  //! This is the non-throw lookup counterpart of OriginalOf().
  //! @param[in] theNode node identifier
  //! @return pointer to original shape for an active node, or nullptr when absent/invalid/removed
  [[nodiscard]] Standard_EXPORT const TopoDS_Shape* FindOriginal(
    const BRepGraph_NodeId theNode) const;

  //! Return the original TopoDS_Shape stored during graph construction.
  //! @param[in] theNode node identifier
  //! @return reference to the exact TopoDS_Shape stored during graph construction
  //! @exception Standard_ProgramError if no original shape exists
  [[nodiscard]] Standard_EXPORT const TopoDS_Shape& OriginalOf(
    const BRepGraph_NodeId theNode) const;

  //! Reconstruct a TopoDS_Shape from a graph node without using the persistent cache.
  //! Use this when the caller explicitly needs a fresh rebuild instead of the
  //! shared cached shape returned by Shape(). This method does not populate the
  //! persistent reconstructed-shape cache.
  //! Topology definition nodes reconstruct topology directly.
  //! Product nodes are reconstructed in product-local coordinates.
  //! Occurrence nodes are reconstructed with cumulative occurrence placement.
  //! @param[in] theRoot definition node identifier
  //! @return reconstructed shape, or null shape for invalid/removed nodes
  [[nodiscard]] Standard_EXPORT TopoDS_Shape Reconstruct(const BRepGraph_NodeId theRoot) const;

  //! Look up the definition NodeId for a shape from graph construction input.
  //! Uses TShape pointer comparison (same semantics as IsSame()).
  //! Synthetic Product / Occurrence reconstructions are not given dedicated
  //! TShape bindings, so lookup is only guaranteed for construction-time topology.
  //! Programmatically created Builder().Add*() nodes can still be located by
  //! UID or by direct iteration over Topo() definitions.
  //! @param[in] theShape shape to look up
  //! @return active node identifier, or invalid NodeId if the shape is absent or removed
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId FindNode(const TopoDS_Shape& theShape) const;

  //! Check if a shape is known to the graph (was part of construction input).
  //! Uses TShape pointer comparison (same semantics as IsSame()).
  //! Synthetic Product / Occurrence reconstructions are not given dedicated
  //! TShape bindings, so this is only guaranteed for construction-time topology.
  //! Programmatically created Builder().Add*() nodes can still be located by
  //! UID or by direct iteration over Topo() definitions.
  //! @param[in] theShape shape to check
  //! @return true if the shape has a corresponding active definition node
  [[nodiscard]] Standard_EXPORT bool HasNode(const TopoDS_Shape& theShape) const;

private:
  friend class BRepGraph;
  friend struct BRepGraph_Data;

  explicit ShapesView(const BRepGraph* theGraph)
      : myGraph(theGraph)
  {
  }

  const BRepGraph* myGraph;
};

#endif // _BRepGraph_ShapesView_HeaderFile
