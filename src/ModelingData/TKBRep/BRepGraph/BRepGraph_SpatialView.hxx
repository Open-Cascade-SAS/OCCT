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

#ifndef _BRepGraph_SpatialView_HeaderFile
#define _BRepGraph_SpatialView_HeaderFile

#include <BRepGraph.hxx>

//! @brief Read-only view for topological adjacency queries.
//!
//! Provides definition-level adjacency: adjacent faces, shared edges,
//! same-domain faces, and faces referencing an edge.
//! These queries operate on the reverse index, not on topology paths.
//!
//! Obtained via BRepGraph::Spatial().
class BRepGraph::SpatialView
{
public:
  //! Return all face definitions sharing the same surface as the given face.
  //! @param[in] theFaceDef face definition NodeId
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> SameDomainFaces(
    const BRepGraph_NodeId theFaceDef) const;

  //! Return all face definition NodeIds that reference this edge.
  //! @param[in] theEdgeDef edge definition NodeId
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> FacesOfEdge(
    const BRepGraph_NodeId theEdgeDef) const;

  //! Return all edges shared between two faces.
  //! @param[in] theFaceA first face definition NodeId
  //! @param[in] theFaceB second face definition NodeId
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> SharedEdges(
    const BRepGraph_NodeId theFaceA,
    const BRepGraph_NodeId theFaceB) const;

  //! Return all faces adjacent to a face (sharing at least one edge).
  //! @param[in] theFaceDef face definition NodeId
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> AdjacentFaces(
    const BRepGraph_NodeId theFaceDef) const;

  //! Return all edge definitions in a face (collected from all wires via coedges).
  //! @param[in] theFaceDef face definition NodeId
  //! @return unique edge NodeIds (deduplicated)
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> EdgesOfFace(
    const BRepGraph_NodeId theFaceDef) const;

  //! Return all edge definitions incident to a vertex.
  //! @param[in] theVertexDef vertex definition NodeId
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> EdgesOfVertex(
    const BRepGraph_NodeId theVertexDef) const;

  //! Return start, end, and internal vertex definitions for an edge.
  //! @param[in] theEdgeDef edge definition NodeId
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> VerticesOfEdge(
    const BRepGraph_NodeId theEdgeDef) const;

  //! Return all edges sharing a vertex with the given edge (excluding itself).
  //! @param[in] theEdgeDef edge definition NodeId
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> AdjacentEdges(
    const BRepGraph_NodeId theEdgeDef) const;

  //! Return the number of distinct faces referencing this edge.
  //! 0 = free edge, 1 = boundary, 2 = manifold, 3+ = non-manifold.
  //! O(1) lookup via cached reverse index.
  //! @param[in] theEdgeDef edge definition NodeId
  [[nodiscard]] Standard_EXPORT int FaceCountOfEdge(const BRepGraph_NodeId theEdgeDef) const;

  //! True if the edge is referenced by exactly one face (boundary edge).
  //! @param[in] theEdgeDef edge definition NodeId
  [[nodiscard]] Standard_EXPORT bool IsBoundaryEdge(const BRepGraph_NodeId theEdgeDef) const;

  //! True if the edge is referenced by exactly two faces (manifold interior edge).
  //! @param[in] theEdgeDef edge definition NodeId
  [[nodiscard]] Standard_EXPORT bool IsManifoldEdge(const BRepGraph_NodeId theEdgeDef) const;

private:
  friend class BRepGraph;

  explicit SpatialView(const BRepGraph* theGraph)
      : myGraph(theGraph)
  {
  }

  const BRepGraph* myGraph;
};

inline BRepGraph::SpatialView BRepGraph::Spatial() const
{
  return SpatialView(this);
}

#endif // _BRepGraph_SpatialView_HeaderFile
