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
#include <TopLoc_Location.hxx>

//! @brief Read-only view for spatial queries and topological adjacency.
//!
//! Computes global transforms from per-node locations, finds same-domain
//! faces sharing a surface, discovers face-edge and face-face adjacency
//! relationships, and evaluates global placements for assembly occurrences.
//! Obtained via BRepGraph::Spatial().
class BRepGraph::SpatialView
{
public:
  //! Accumulated global transform for a definition (from per-node location).
  //! @param[in] theDefId definition node identifier
  Standard_EXPORT gp_Trsf GlobalTransform(BRepGraph_NodeId theDefId) const;

  //! Compute the global placement of an occurrence by walking the parent chain.
  //! @param[in] theOccurrenceIdx zero-based occurrence definition index
  Standard_EXPORT TopLoc_Location GlobalPlacement(int theOccurrenceIdx) const;

  //! Return all face definitions sharing the same surface as the given face.
  //! @param[in] theFaceDef face definition NodeId
  Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> SameDomainFaces(
    BRepGraph_NodeId theFaceDef) const;

  //! Return all face definition NodeIds that reference this edge (via PCurve associations).
  //! @param[in] theEdgeDef edge definition NodeId
  Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> FacesOfEdge(
    BRepGraph_NodeId theEdgeDef) const;

  //! Return all edges shared between two faces.
  //! @param[in] theFaceA first face definition NodeId
  //! @param[in] theFaceB second face definition NodeId
  Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> SharedEdges(
    BRepGraph_NodeId theFaceA,
    BRepGraph_NodeId theFaceB) const;

  //! Return all faces adjacent to a face (sharing at least one edge).
  //! @param[in] theFaceDef face definition NodeId
  Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> AdjacentFaces(
    BRepGraph_NodeId theFaceDef) const;

private:
  friend class BRepGraph;
  explicit SpatialView(const BRepGraph* theGraph) : myGraph(theGraph) {}
  const BRepGraph* myGraph;
};

inline BRepGraph::SpatialView BRepGraph::Spatial() const { return SpatialView(this); }

#endif // _BRepGraph_SpatialView_HeaderFile
