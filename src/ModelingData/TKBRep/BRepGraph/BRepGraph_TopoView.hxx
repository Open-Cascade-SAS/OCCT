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

//! @brief Unified read-only view over topology definitions, adjacency, and representations.
//!
//! Provides topology definition lookup, representation lookup, and read-only
//! adjacency queries over the incidence-table model stored in BRepGraph.
//! Obtained via BRepGraph::Topo().
//!
//! ## Soft-deletion convention
//! Count methods (NbFaces, NbEdges, etc.) return totals including soft-removed
//! nodes. Use NbActive* variants to exclude removed nodes. Definition accessors
//! (Face, Edge, etc.) do not filter removed nodes - callers should check
//! IsRemoved() if needed.
//!
//! ## TopoView vs RefsView naming
//! TopoView accessors take definition IDs (BRepGraph_FaceId, BRepGraph_ShellId, etc.)
//! and return definition structs (FaceDef, ShellDef). RefsView accessors take
//! reference IDs (BRepGraph_FaceRefId, BRepGraph_ShellRefId) and return
//! reference-entry structs carrying per-use orientation and location.
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

  //! @name Reverse-index backed queries (O(1), zero allocation)

  //! Return cached face count for an edge - O(1).
  //! @param[in] theEdge typed edge definition identifier
  [[nodiscard]] Standard_EXPORT int NbFacesOfEdge(const BRepGraph_EdgeId theEdge) const;

  //! Return wire definition indices that contain a given edge (safe reference, never null).
  //! @param[in] theEdge typed edge definition identifier
  [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_WireId>& WiresOfEdge(
    const BRepGraph_EdgeId theEdge) const;

  //! Return coedge indices referencing the given edge (safe reference, never null).
  //! @param[in] theEdge typed edge definition identifier
  [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_CoEdgeId>& CoEdgesOfEdge(
    const BRepGraph_EdgeId theEdge) const;

  //! Return face indices containing the given edge (safe reference, never null).
  //! Derived from CoEdge.FaceDefId links during Build()/BuildDelta().
  //! @param[in] theEdge typed edge definition identifier
  [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_FaceId>& FacesOfEdge(
    const BRepGraph_EdgeId theEdge) const;

  //! Return edge indices incident to the given vertex (safe reference, never null).
  //! @param[in] theVertex typed vertex definition identifier
  [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_EdgeId>& EdgesOfVertex(
    const BRepGraph_VertexId theVertex) const;

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
  [[nodiscard]] Standard_EXPORT int NbNodes() const;

  //! Check if a node has been soft-removed.
  //! @param[in] theNode node to check
  //! @return true if the node was marked as removed
  [[nodiscard]] Standard_EXPORT bool IsRemoved(const BRepGraph_NodeId theNode) const;

  //! @name Representation count accessors
  //!
  //! Representations use dense 0-based indexing. Iterate with:
  //! @code
  //!   for (int i = 0; i < aGraph.Topo().NbSurfaces(); ++i)
  //!   {
  //!     const BRepGraphInc::SurfaceRep& aRep =
  //!       aGraph.Topo().SurfaceRep(BRepGraph_SurfaceRepId(i));
  //!   }
  //! @endcode

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

  //! Number of active (non-removed) surface representations.
  [[nodiscard]] Standard_EXPORT int NbActiveSurfaces() const;

  //! Number of active (non-removed) 3D curve representations.
  [[nodiscard]] Standard_EXPORT int NbActiveCurves3D() const;

  //! Number of active (non-removed) 2D curve representations.
  [[nodiscard]] Standard_EXPORT int NbActiveCurves2D() const;

  //! Number of active (non-removed) triangulation representations.
  [[nodiscard]] Standard_EXPORT int NbActiveTriangulations() const;

  //! Number of active (non-removed) 3D polygon representations.
  [[nodiscard]] Standard_EXPORT int NbActivePolygons3D() const;

  //! Number of active (non-removed) 2D polygon representations.
  [[nodiscard]] Standard_EXPORT int NbActivePolygons2D() const;

  //! Number of active (non-removed) polygon-on-triangulation representations.
  [[nodiscard]] Standard_EXPORT int NbActivePolygonsOnTri() const;

  //! @name Computed spatial adjacency queries (allocate result vector)

  //! Return all face definitions sharing the same surface as the given face.
  //! @param[in] theFace face definition identifier
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_FaceId> SameDomainFaces(
    const BRepGraph_FaceId theFace) const;

  //! Return all edges shared between two faces.
  //! @param[in] theFaceA first face definition identifier
  //! @param[in] theFaceB second face definition identifier
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_EdgeId> SharedEdges(
    const BRepGraph_FaceId theFaceA,
    const BRepGraph_FaceId theFaceB) const;

  //! Return all faces adjacent to a face (sharing at least one edge).
  //! @param[in] theFace face definition identifier
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_FaceId> AdjacentFaces(
    const BRepGraph_FaceId theFace) const;

  //! Return all edge definitions in a face (collected from all wires via coedges).
  //! @param[in] theFace face definition identifier
  //! @return unique edge definition ids (deduplicated)
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_EdgeId> EdgesOfFace(
    const BRepGraph_FaceId theFace) const;

  //! Return outer wire definition id of a face, or invalid if none.
  //! @param[in] theFace typed face definition identifier
  //! @return outer wire definition id
  [[nodiscard]] Standard_EXPORT BRepGraph_WireId
    OuterWireOfFace(const BRepGraph_FaceId theFace) const;

  //! Return start, end, and internal vertex definitions for an edge.
  //! @param[in] theEdge edge definition identifier
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_VertexId> VerticesOfEdge(
    const BRepGraph_EdgeId theEdge) const;

  //! Return all edges sharing a vertex with the given edge (excluding itself).
  //! @param[in] theEdge edge definition identifier
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_EdgeId> AdjacentEdges(
    const BRepGraph_EdgeId theEdge) const;

  //! True if the edge is referenced by exactly one face (boundary edge).
  //! @param[in] theEdge typed edge definition identifier
  [[nodiscard]] Standard_EXPORT bool IsBoundaryEdge(const BRepGraph_EdgeId theEdge) const;

  //! True if the edge is referenced by exactly two faces (manifold interior edge).
  //! @param[in] theEdge typed edge definition identifier
  [[nodiscard]] Standard_EXPORT bool IsManifoldEdge(const BRepGraph_EdgeId theEdge) const;

  //! Return all face definitions that share at least one edge with the given vertex.
  //! @param[in] theVertex vertex definition identifier
  //! @return unique face definition ids
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_FaceId> FacesOfVertex(
    const BRepGraph_VertexId theVertex) const;

  //! @name Representation accessors

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

  //! @name Assembly definition accessors
  //!
  //! Product and Occurrence definitions live in the incidence storage alongside
  //! topology nodes. This view exposes only direct definition lookup and counts.
  //! For assembly structure queries and placement/path traversal, use PathView.

  //! Number of product definitions.
  [[nodiscard]] Standard_EXPORT int NbProducts() const;

  //! Number of occurrence definitions.
  [[nodiscard]] Standard_EXPORT int NbOccurrences() const;

  //! Number of active (non-removed) product definitions.
  [[nodiscard]] Standard_EXPORT int NbActiveProducts() const;

  //! Number of active (non-removed) occurrence definitions.
  [[nodiscard]] Standard_EXPORT int NbActiveOccurrences() const;

  //! Access product definition by typed identifier.
  //! @param[in] theProduct typed product definition identifier
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::ProductDef& Product(
    const BRepGraph_ProductId theProduct) const;

  //! Access occurrence definition by typed identifier.
  //! @param[in] theOccurrence typed occurrence definition identifier
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::OccurrenceDef& Occurrence(
    const BRepGraph_OccurrenceId theOccurrence) const;

private:
  friend class BRepGraph;
  friend struct BRepGraph_Data;
  friend class BRepGraph_Tool;
  friend class BRepGraph_Analyze;

  //! @name Private PCurve lookup (implementation detail)
  //! Public API is BRepGraph_Tool::Edge::FindPCurve() which delegates here.

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

  explicit TopoView(const BRepGraph* theGraph)
      : myGraph(theGraph)
  {
  }

  const BRepGraph* myGraph;
};

#endif // _BRepGraph_TopoView_HeaderFile
