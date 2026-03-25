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

#ifndef _BRepGraph_DefsView_HeaderFile
#define _BRepGraph_DefsView_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraph_PCurveContext.hxx>
#include <BRepGraph_RepId.hxx>
#include <GeomAdaptor_TransformedCurve.hxx>
#include <GeomAdaptor_TransformedSurface.hxx>

class Adaptor3d_CurveOnSurface;

//! @brief Read-only view over topology definition nodes stored in BRepGraph.
//!
//! Provides const access to all topology kinds (Vertex, Edge, Wire, Face,
//! Shell, Solid, Compound, CompSolid, CoEdge) and assembly kinds (Product,
//! Occurrence) by zero-based definition index. Also exposes representation
//! counts and PCurve lookup methods.
//! Obtained via BRepGraph::Defs().
class BRepGraph::DefsView
{
public:
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

  //! Access solid definition by typed identifier.
  //! @param[in] theSolid typed solid definition identifier
  [[nodiscard]] Standard_EXPORT const BRepGraph_TopoNode::SolidDef& Solid(
    const BRepGraph_SolidId theSolid) const;

  //! Access shell definition by typed identifier.
  //! @param[in] theShell typed shell definition identifier
  [[nodiscard]] Standard_EXPORT const BRepGraph_TopoNode::ShellDef& Shell(
    const BRepGraph_ShellId theShell) const;

  //! Access face definition by typed identifier.
  //! @param[in] theFace typed face definition identifier
  [[nodiscard]] Standard_EXPORT const BRepGraph_TopoNode::FaceDef& Face(
    const BRepGraph_FaceId theFace) const;

  //! Access wire definition by typed identifier.
  //! @param[in] theWire typed wire definition identifier
  [[nodiscard]] Standard_EXPORT const BRepGraph_TopoNode::WireDef& Wire(
    const BRepGraph_WireId theWire) const;

  //! Access edge definition by typed identifier.
  //! @param[in] theEdge typed edge definition identifier
  [[nodiscard]] Standard_EXPORT const BRepGraph_TopoNode::EdgeDef& Edge(
    const BRepGraph_EdgeId theEdge) const;

  //! Access vertex definition by typed identifier.
  //! @param[in] theVertex typed vertex definition identifier
  [[nodiscard]] Standard_EXPORT const BRepGraph_TopoNode::VertexDef& Vertex(
    const BRepGraph_VertexId theVertex) const;

  //! Access compound definition by typed identifier.
  //! @param[in] theCompound typed compound definition identifier
  [[nodiscard]] Standard_EXPORT const BRepGraph_TopoNode::CompoundDef& Compound(
    const BRepGraph_CompoundId theCompound) const;

  //! Access compsolid definition by typed identifier.
  //! @param[in] theCompSolid typed compsolid definition identifier
  [[nodiscard]] Standard_EXPORT const BRepGraph_TopoNode::CompSolidDef& CompSolid(
    const BRepGraph_CompSolidId theCompSolid) const;

  //! Access coedge definition by typed identifier.
  //! @param[in] theCoEdge typed coedge definition identifier
  [[nodiscard]] Standard_EXPORT const BRepGraph_TopoNode::CoEdgeDef& CoEdge(
    const BRepGraph_CoEdgeId theCoEdge) const;

  //! Generic topology definition lookup by NodeId.
  //! @param[in] theId node identifier
  //! @return pointer to BaseDef or nullptr if invalid
  [[nodiscard]] Standard_EXPORT const BRepGraph_TopoNode::BaseDef* TopoDef(
    const BRepGraph_NodeId theId) const;

  //! Number of face definitions in a shell (via first usage).
  //! @param[in] theShell typed shell definition identifier
  [[nodiscard]] Standard_EXPORT int NbShellFaces(const BRepGraph_ShellId theShell) const;

  //! Access face definition NodeId belonging to a shell by index.
  //! @param[in] theShell typed shell definition identifier
  //! @param[in] theFaceIndex zero-based face index within the shell
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId ShellFaceDef(const BRepGraph_ShellId theShell,
                                                              const int theFaceIndex) const;

  //! Number of product definitions.
  Standard_EXPORT int NbProducts() const;

  //! Number of occurrence definitions.
  Standard_EXPORT int NbOccurrences() const;

  //! Number of active (non-removed) product definitions.
  Standard_EXPORT int NbActiveProducts() const;

  //! Number of active (non-removed) occurrence definitions.
  Standard_EXPORT int NbActiveOccurrences() const;

  //! Access product definition by typed identifier.
  //! @param[in] theProduct typed product definition identifier
  [[nodiscard]] Standard_EXPORT const BRepGraph_TopoNode::ProductDef& Product(
    const BRepGraph_ProductId theProduct) const;

  //! Access occurrence definition by typed identifier.
  //! @param[in] theOccurrence typed occurrence definition identifier
  [[nodiscard]] Standard_EXPORT const BRepGraph_TopoNode::OccurrenceDef& Occurrence(
    const BRepGraph_OccurrenceId theOccurrence) const;

  //! Return NodeIds of all root products (products that are not referenced by any occurrence).
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> RootProducts() const;

  //! True if the product is an assembly (has child occurrences, no ShapeRootId).
  //! @param[in] theProduct typed product definition identifier
  [[nodiscard]] Standard_EXPORT bool IsAssembly(const BRepGraph_ProductId theProduct) const;

  //! True if the product is a part (has a valid ShapeRootId).
  //! @param[in] theProduct typed product definition identifier
  [[nodiscard]] Standard_EXPORT bool IsPart(const BRepGraph_ProductId theProduct) const;

  //! Number of child occurrences of a product.
  //! @param[in] theProduct typed product definition identifier
  [[nodiscard]] Standard_EXPORT int NbComponents(const BRepGraph_ProductId theProduct) const;

  //! Return the i-th child occurrence NodeId of a product.
  //! @param[in] theProduct typed product definition identifier
  //! @param[in] theComponentIdx zero-based occurrence index within the product
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId Component(const BRepGraph_ProductId theProduct,
                                                           const int theComponentIdx) const;

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
  //! @param[in] theEdgeDef edge definition NodeId
  //! @param[in] theFaceDef face definition NodeId
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::CoEdgeEntity* FindPCurve(
    const BRepGraph_NodeId theEdgeDef,
    const BRepGraph_NodeId theFaceDef) const;

  //! Find the CoEdge for an edge/face/orientation triple (seam edge support).
  //! @param[in] theEdgeDef           edge definition NodeId
  //! @param[in] theFaceDef           face definition NodeId
  //! @param[in] theEdgeOrientation   edge orientation on the face
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::CoEdgeEntity* FindPCurve(
    const BRepGraph_NodeId   theEdgeDef,
    const BRepGraph_NodeId   theFaceDef,
    const TopAbs_Orientation theEdgeOrientation) const;

  //! Find the CoEdge for a given PCurve context.
  //! @param[in] theContext  composite key identifying edge, face and orientation
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::CoEdgeEntity* FindPCurve(
    const BRepGraph_PCurveContext& theContext) const;

private:
  friend class BRepGraph;
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

  explicit DefsView(const BRepGraph* theGraph)
      : myGraph(theGraph)
  {
  }

  const BRepGraph* myGraph;
};

inline BRepGraph::DefsView BRepGraph::Defs() const
{
  return DefsView(this);
}

#endif // _BRepGraph_DefsView_HeaderFile
