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

#ifndef _BRepGraph_TopoView_HeaderFile
#define _BRepGraph_TopoView_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraph_RepId.hxx>
#include <GeomAdaptor_TransformedCurve.hxx>
#include <GeomAdaptor_TransformedSurface.hxx>

class Adaptor3d_CurveOnSurface;

//! @brief Unified read-only view over topology definition and adjacency nodes stored in BRepGraph.
//!
//! Merges the former TopoView (definition lookups, representation counts,
//! PCurve queries) and SpatialView (adjacency queries: adjacent faces,
//! shared edges, same-domain faces, faces referencing an edge, etc.)
//! into a single view class.
//! Obtained via BRepGraph::Topo().
class BRepGraph::TopoView
{
public:
  //! @name Count accessors

  //! Number of solid definitions.
  [[nodiscard]] Standard_EXPORT int NbSolids() const;

  //! Number of shell definitions.
  [[nodiscard]] Standard_EXPORT int NbShells() const;

  //! Number of face definitions.
  [[nodiscard]] Standard_EXPORT int NbFaces() const;

  //! Number of wire definitions.
  [[nodiscard]] Standard_EXPORT int NbWires() const;

  //! Number of edge definitions.
  [[nodiscard]] Standard_EXPORT int NbEdges() const;

  //! Number of vertex definitions.
  [[nodiscard]] Standard_EXPORT int NbVertices() const;

  //! Number of compound definitions.
  [[nodiscard]] Standard_EXPORT int NbCompounds() const;

  //! Number of compsolid definitions.
  [[nodiscard]] Standard_EXPORT int NbCompSolids() const;

  //! Number of coedge definitions.
  [[nodiscard]] Standard_EXPORT int NbCoEdges() const;

  //! Number of active (non-removed) definitions per kind.
  [[nodiscard]] Standard_EXPORT int NbActiveVertices() const;
  [[nodiscard]] Standard_EXPORT int NbActiveEdges() const;
  [[nodiscard]] Standard_EXPORT int NbActiveCoEdges() const;
  [[nodiscard]] Standard_EXPORT int NbActiveWires() const;
  [[nodiscard]] Standard_EXPORT int NbActiveFaces() const;
  [[nodiscard]] Standard_EXPORT int NbActiveShells() const;
  [[nodiscard]] Standard_EXPORT int NbActiveSolids() const;
  [[nodiscard]] Standard_EXPORT int NbActiveCompounds() const;
  [[nodiscard]] Standard_EXPORT int NbActiveCompSolids() const;

  //! Return cached face count for an edge - O(1).
  //! @param[in] theEdge typed edge definition identifier
  [[nodiscard]] Standard_EXPORT int FaceCountOfEdge(const BRepGraph_EdgeId theEdge) const;

  //! Return wire definition indices that contain a given edge (safe reference, never null).
  //! @param[in] theEdge typed edge definition identifier
  [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_WireId>& WiresOfEdge(
    const BRepGraph_EdgeId theEdge) const;

  //! Return coedge indices referencing the given edge (safe reference, never null).
  //! @param[in] theEdge typed edge definition identifier
  [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_CoEdgeId>& CoEdgesOfEdge(
    const BRepGraph_EdgeId theEdge) const;

  //! @name Definition accessors

  //! Access solid definition by typed identifier.
  //! @param[in] theSolid typed solid definition identifier
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::SolidDef& Solid(
    const BRepGraph_SolidId theSolid) const;

  //! Access shell definition by typed identifier.
  //! @param[in] theShell typed shell definition identifier
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::ShellDef& Shell(
    const BRepGraph_ShellId theShell) const;

  //! Access face definition by typed identifier.
  //! @param[in] theFace typed face definition identifier
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::FaceDef& Face(
    const BRepGraph_FaceId theFace) const;

  //! Access wire definition by typed identifier.
  //! @param[in] theWire typed wire definition identifier
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::WireDef& Wire(
    const BRepGraph_WireId theWire) const;

  //! Access edge definition by typed identifier.
  //! @param[in] theEdge typed edge definition identifier
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::EdgeDef& Edge(
    const BRepGraph_EdgeId theEdge) const;

  //! Access vertex definition by typed identifier.
  //! @param[in] theVertex typed vertex definition identifier
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::VertexDef& Vertex(
    const BRepGraph_VertexId theVertex) const;

  //! Access compound definition by typed identifier.
  //! @param[in] theCompound typed compound definition identifier
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::CompoundDef& Compound(
    const BRepGraph_CompoundId theCompound) const;

  //! Access compsolid definition by typed identifier.
  //! @param[in] theCompSolid typed compsolid definition identifier
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::CompSolidDef& CompSolid(
    const BRepGraph_CompSolidId theCompSolid) const;

  //! Access coedge definition by typed identifier.
  //! @param[in] theCoEdge typed coedge definition identifier
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::CoEdgeDef& CoEdge(
    const BRepGraph_CoEdgeId theCoEdge) const;

  //! Generic topology definition lookup by NodeId.
  //! @param[in] theId node identifier
  //! @return pointer to BaseDef or nullptr if invalid
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::BaseDef* TopoEntity(
    const BRepGraph_NodeId theId) const;

  //! Number of face definitions in a shell (via first usage).
  //! @param[in] theShell typed shell definition identifier
  [[nodiscard]] Standard_EXPORT int NbShellFaces(const BRepGraph_ShellId theShell) const;

  //! Access face definition NodeId belonging to a shell by index.
  //! @param[in] theShell typed shell definition identifier
  //! @param[in] theFaceIndex zero-based face index within the shell
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId ShellFaceEntity(const BRepGraph_ShellId theShell,
                                                              const int theFaceIndex) const;

  //! Total number of nodes in the graph (all topology + assembly kinds).
  [[nodiscard]] Standard_EXPORT size_t NbNodes() const;

  //! Check if a node has been soft-removed.
  //! @param[in] theNode node to check
  //! @return true if the node was marked as removed
  [[nodiscard]] Standard_EXPORT bool IsRemoved(const BRepGraph_NodeId theNode) const;

  //! @name Representation count accessors

  //! Number of surface representations.
  [[nodiscard]] Standard_EXPORT int NbSurfaces() const;

  //! Number of 3D curve representations.
  [[nodiscard]] Standard_EXPORT int NbCurves3D() const;

  //! Number of 2D curve (PCurve) representations.
  [[nodiscard]] Standard_EXPORT int NbCurves2D() const;

  //! Number of triangulation representations.
  [[nodiscard]] Standard_EXPORT int NbTriangulations() const;

  //! Number of 3D polygon representations.
  [[nodiscard]] Standard_EXPORT int NbPolygons3D() const;

  //! Number of 2D polygon representations.
  [[nodiscard]] Standard_EXPORT int NbPolygons2D() const;

  //! Number of polygon-on-triangulation representations.
  [[nodiscard]] Standard_EXPORT int NbPolygonsOnTri() const;

  //! @name Geometry query methods

  //! Find the CoEdge for an edge on a given face, or nullptr if none exists.
  //! @param[in] theEdgeEntity edge definition NodeId
  //! @param[in] theFaceEntity face definition NodeId
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::CoEdgeDef* FindPCurve(
    const BRepGraph_NodeId theEdgeEntity,
    const BRepGraph_NodeId theFaceEntity) const;

  //! Find the CoEdge for an edge/face/orientation triple (seam edge support).
  //! @param[in] theEdgeEntity           edge definition NodeId
  //! @param[in] theFaceEntity           face definition NodeId
  //! @param[in] theEdgeOrientation   edge orientation on the face
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::CoEdgeDef* FindPCurve(
    const BRepGraph_NodeId   theEdgeEntity,
    const BRepGraph_NodeId   theFaceEntity,
    const TopAbs_Orientation theEdgeOrientation) const;

  //! @name Spatial adjacency queries

  //! Return all face definitions sharing the same surface as the given face.
  //! @param[in] theFaceEntity face definition NodeId
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> SameDomainFaces(
    const BRepGraph_NodeId theFaceEntity) const;

  //! Return all face definition NodeIds that reference this edge.
  //! @param[in] theEdgeEntity edge definition NodeId
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> FacesOfEdge(
    const BRepGraph_NodeId theEdgeEntity) const;

  //! Return all edges shared between two faces.
  //! @param[in] theFaceA first face definition NodeId
  //! @param[in] theFaceB second face definition NodeId
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> SharedEdges(
    const BRepGraph_NodeId theFaceA,
    const BRepGraph_NodeId theFaceB) const;

  //! Return all faces adjacent to a face (sharing at least one edge).
  //! @param[in] theFaceEntity face definition NodeId
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> AdjacentFaces(
    const BRepGraph_NodeId theFaceEntity) const;

  //! Return all edge definitions in a face (collected from all wires via coedges).
  //! @param[in] theFaceEntity face definition NodeId
  //! @return unique edge NodeIds (deduplicated)
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> EdgesOfFace(
    const BRepGraph_NodeId theFaceEntity) const;

  //! Return outer wire definition id of a face, or invalid if none.
  //! @param[in] theFace typed face definition identifier
  //! @return outer wire definition id
  [[nodiscard]] Standard_EXPORT BRepGraph_WireId OuterWireOfFace(
    const BRepGraph_FaceId theFace) const;

  //! Return all edge definitions incident to a vertex.
  //! @param[in] theVertexEntity vertex definition NodeId
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> EdgesOfVertex(
    const BRepGraph_NodeId theVertexEntity) const;

  //! Return start, end, and internal vertex definitions for an edge.
  //! @param[in] theEdgeEntity edge definition NodeId
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> VerticesOfEdge(
    const BRepGraph_NodeId theEdgeEntity) const;

  //! Return all edges sharing a vertex with the given edge (excluding itself).
  //! @param[in] theEdgeEntity edge definition NodeId
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> AdjacentEdges(
    const BRepGraph_NodeId theEdgeEntity) const;

  //! Return the number of distinct faces referencing this edge.
  //! 0 = free edge, 1 = boundary, 2 = manifold, 3+ = non-manifold.
  //! O(1) lookup via cached reverse index.
  //! @param[in] theEdgeEntity edge definition NodeId
  [[nodiscard]] Standard_EXPORT int FaceCountOfEdge(const BRepGraph_NodeId theEdgeEntity) const;

  //! True if the edge is referenced by exactly one face (boundary edge).
  //! @param[in] theEdgeEntity edge definition NodeId
  [[nodiscard]] Standard_EXPORT bool IsBoundaryEdge(const BRepGraph_NodeId theEdgeEntity) const;

  //! True if the edge is referenced by exactly two faces (manifold interior edge).
  //! @param[in] theEdgeEntity edge definition NodeId
  [[nodiscard]] Standard_EXPORT bool IsManifoldEdge(const BRepGraph_NodeId theEdgeEntity) const;

private:
  friend class BRepGraph;
  friend struct BRepGraph_Data;
  friend class BRepGraph_Tool;

  //! Access surface representation by typed identifier.
  //! @param[in] theRep typed surface representation identifier
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::SurfaceRep& SurfaceRep(
    const BRepGraph_SurfaceRepId theRep) const;

  //! Access 3D curve representation by typed identifier.
  //! @param[in] theRep typed 3D curve representation identifier
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::Curve3DRep& Curve3DRep(
    const BRepGraph_Curve3DRepId theRep) const;

  //! Access 2D curve representation by typed identifier.
  //! @param[in] theRep typed 2D curve representation identifier
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::Curve2DRep& Curve2DRep(
    const BRepGraph_Curve2DRepId theRep) const;

  //! Access triangulation representation by typed identifier.
  //! @param[in] theRep typed triangulation representation identifier
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::TriangulationRep& TriangulationRep(
    const BRepGraph_TriangulationRepId theRep) const;

  //! Access 3D polygon representation by typed identifier.
  //! @param[in] theRep typed 3D polygon representation identifier
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::Polygon3DRep& Polygon3DRep(
    const BRepGraph_Polygon3DRepId theRep) const;

  //! Access 2D polygon representation by typed identifier.
  //! @param[in] theRep typed 2D polygon representation identifier
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::Polygon2DRep& Polygon2DRep(
    const BRepGraph_Polygon2DRepId theRep) const;

  //! Access polygon-on-triangulation representation by typed identifier.
  //! @param[in] theRep typed polygon-on-triangulation representation identifier
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::PolygonOnTriRep& PolygonOnTriRep(
    const BRepGraph_PolygonOnTriRepId theRep) const;

  explicit TopoView(const BRepGraph* theGraph)
      : myGraph(theGraph)
  {
  }

  const BRepGraph* myGraph;
};

#endif // _BRepGraph_TopoView_HeaderFile
