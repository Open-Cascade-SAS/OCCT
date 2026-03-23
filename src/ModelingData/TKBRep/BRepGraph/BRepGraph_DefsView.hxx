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
#include <GeomAdaptor_TransformedCurve.hxx>
#include <GeomAdaptor_TransformedSurface.hxx>

class Adaptor3d_CurveOnSurface;

//! Lightweight const view over topology definition nodes of a BRepGraph.
//! Obtained via BRepGraph::Defs().
class BRepGraph::DefsView
{
public:
  //! Number of solid definitions.
  Standard_EXPORT int NbSolids() const;

  //! Number of shell definitions.
  Standard_EXPORT int NbShells() const;

  //! Number of face definitions.
  Standard_EXPORT int NbFaces() const;

  //! Number of wire definitions.
  Standard_EXPORT int NbWires() const;

  //! Number of edge definitions.
  Standard_EXPORT int NbEdges() const;

  //! Number of vertex definitions.
  Standard_EXPORT int NbVertices() const;

  //! Number of compound definitions.
  Standard_EXPORT int NbCompounds() const;

  //! Number of compsolid definitions.
  Standard_EXPORT int NbCompSolids() const;

  //! Number of coedge definitions.
  Standard_EXPORT int NbCoEdges() const;

  //! Number of active (non-removed) definitions per kind.
  Standard_EXPORT int NbActiveVertices() const;
  Standard_EXPORT int NbActiveEdges() const;
  Standard_EXPORT int NbActiveWires() const;
  Standard_EXPORT int NbActiveFaces() const;
  Standard_EXPORT int NbActiveShells() const;
  Standard_EXPORT int NbActiveSolids() const;
  Standard_EXPORT int NbActiveCompounds() const;
  Standard_EXPORT int NbActiveCompSolids() const;

  //! Return cached face count for an edge — O(1).
  //! @param[in] theEdgeDefIdx zero-based edge definition index
  Standard_EXPORT int FaceCountOfEdge(int theEdgeDefIdx) const;

  //! Return coedge indices referencing the given edge (safe reference, never null).
  //! @param[in] theEdgeDefIdx zero-based edge definition index
  Standard_EXPORT const NCollection_Vector<int>& CoEdgesOfEdge(int theEdgeDefIdx) const;

  //! Access solid definition by index.
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT const BRepGraph_TopoNode::SolidDef& Solid(int theIdx) const;

  //! Access shell definition by index.
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT const BRepGraph_TopoNode::ShellDef& Shell(int theIdx) const;

  //! Access face definition by index.
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT const BRepGraph_TopoNode::FaceDef& Face(int theIdx) const;

  //! Access wire definition by index.
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT const BRepGraph_TopoNode::WireDef& Wire(int theIdx) const;

  //! Access edge definition by index.
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT const BRepGraph_TopoNode::EdgeDef& Edge(int theIdx) const;

  //! Access vertex definition by index.
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT const BRepGraph_TopoNode::VertexDef& Vertex(int theIdx) const;

  //! Access compound definition by index.
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT const BRepGraph_TopoNode::CompoundDef& Compound(int theIdx) const;

  //! Access compsolid definition by index.
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT const BRepGraph_TopoNode::CompSolidDef& CompSolid(int theIdx) const;

  //! Access coedge definition by index.
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT const BRepGraph_TopoNode::CoEdgeDef& CoEdge(int theIdx) const;

  //! Generic topology definition lookup by NodeId.
  //! @param[in] theId node identifier
  //! @return pointer to BaseDef or nullptr if invalid
  Standard_EXPORT const BRepGraph_TopoNode::BaseDef* TopoDef(BRepGraph_NodeId theId) const;

  //! Number of face definitions in a shell (via first usage).
  //! @param[in] theShellDefIdx zero-based shell definition index
  Standard_EXPORT int NbShellFaces(int theShellDefIdx) const;

  //! Access face definition NodeId belonging to a shell by index.
  //! @param[in] theShellDefIdx zero-based shell definition index
  //! @param[in] theFaceIdx zero-based face index within the shell
  Standard_EXPORT BRepGraph_NodeId ShellFaceDef(int theShellDefIdx, int theFaceIdx) const;

  //! Number of product definitions.
  Standard_EXPORT int NbProducts() const;

  //! Number of occurrence definitions.
  Standard_EXPORT int NbOccurrences() const;

  //! Number of active (non-removed) product definitions.
  Standard_EXPORT int NbActiveProducts() const;

  //! Number of active (non-removed) occurrence definitions.
  Standard_EXPORT int NbActiveOccurrences() const;

  //! Access product definition by index.
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT const BRepGraph_TopoNode::ProductDef& Product(int theIdx) const;

  //! Access occurrence definition by index.
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT const BRepGraph_TopoNode::OccurrenceDef& Occurrence(int theIdx) const;

  //! Return NodeIds of all root products (products that are not referenced by any occurrence).
  Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> RootProducts() const;

  //! True if the product at theProductIdx is an assembly (has child occurrences, no ShapeRootId).
  //! @param[in] theProductIdx zero-based product definition index
  Standard_EXPORT bool IsAssembly(int theProductIdx) const;

  //! True if the product at theProductIdx is a part (has a valid ShapeRootId).
  //! @param[in] theProductIdx zero-based product definition index
  Standard_EXPORT bool IsPart(int theProductIdx) const;

  //! Number of child occurrences of a product.
  //! @param[in] theProductIdx zero-based product definition index
  Standard_EXPORT int NbComponents(int theProductIdx) const;

  //! Return the i-th child occurrence NodeId of a product.
  //! @param[in] theProductIdx zero-based product definition index
  //! @param[in] theIdx zero-based occurrence index within the product
  Standard_EXPORT BRepGraph_NodeId Component(int theProductIdx, int theIdx) const;

  //! Total number of nodes in the graph (all topology + assembly kinds).
  Standard_EXPORT size_t NbNodes() const;

  // -- Representation count accessors --

  //! Number of surface representations.
  Standard_EXPORT int NbSurfaces() const;

  //! Number of 3D curve representations.
  Standard_EXPORT int NbCurves3D() const;

  //! Number of 2D curve (PCurve) representations.
  Standard_EXPORT int NbCurves2D() const;

  //! Number of triangulation representations.
  Standard_EXPORT int NbTriangulations() const;

  //! Number of 3D polygon representations.
  Standard_EXPORT int NbPolygons3D() const;

  //! Access surface representation by index.
  Standard_EXPORT const BRepGraphInc::SurfaceRep& SurfaceRep(int theIdx) const;

  //! Access 3D curve representation by index.
  Standard_EXPORT const BRepGraphInc::Curve3DRep& Curve3DRep(int theIdx) const;

  //! Access 2D curve representation by index.
  Standard_EXPORT const BRepGraphInc::Curve2DRep& Curve2DRep(int theIdx) const;

  //! Access triangulation representation by index.
  Standard_EXPORT const BRepGraphInc::TriangulationRep& TriangulationRep(int theIdx) const;

  //! Access 3D polygon representation by index.
  Standard_EXPORT const BRepGraphInc::Polygon3DRep& Polygon3DRep(int theIdx) const;

  //! Number of 2D polygon representations.
  Standard_EXPORT int NbPolygons2D() const;

  //! Number of polygon-on-triangulation representations.
  Standard_EXPORT int NbPolygonsOnTri() const;

  //! Access 2D polygon representation by index.
  Standard_EXPORT const BRepGraphInc::Polygon2DRep& Polygon2DRep(int theIdx) const;

  //! Access polygon-on-triangulation representation by index.
  Standard_EXPORT const BRepGraphInc::PolygonOnTriRep& PolygonOnTriRep(int theIdx) const;

  // -- Convenience geometry accessors (resolve rep index in one call) --

  //! Return the surface handle for a face, or null if no surface.
  //! @param[in] theFaceIdx zero-based face definition index
  Standard_EXPORT const occ::handle<Geom_Surface>& FaceSurface(int theFaceIdx) const;

  //! Return the 3D curve handle for an edge, or null if no curve.
  //! @param[in] theEdgeIdx zero-based edge definition index
  Standard_EXPORT const occ::handle<Geom_Curve>& EdgeCurve3D(int theEdgeIdx) const;

  //! Return the PCurve handle for a coedge, or null if no PCurve.
  //! @param[in] theCoEdgeIdx zero-based coedge definition index
  Standard_EXPORT const occ::handle<Geom2d_Curve>& CoEdgeCurve2D(int theCoEdgeIdx) const;

  //! Return the active triangulation handle for a face, or null if none.
  //! @param[in] theFaceIdx zero-based face definition index
  Standard_EXPORT const occ::handle<Poly_Triangulation>& FaceActiveTriangulation(int theFaceIdx) const;

  //! Return the 3D polygon handle for an edge, or null if no polygon.
  //! @param[in] theEdgeIdx zero-based edge definition index
  Standard_EXPORT const occ::handle<Poly_Polygon3D>& EdgePolygon3D(int theEdgeIdx) const;

  // -- Geometry query methods --

  //! Find the CoEdge for an edge on a given face, or nullptr if none exists.
  //! @param[in] theEdgeDef edge definition NodeId
  //! @param[in] theFaceDef face definition NodeId
  Standard_EXPORT const BRepGraphInc::CoEdgeEntity* FindPCurve(
    BRepGraph_NodeId theEdgeDef,
    BRepGraph_NodeId theFaceDef) const;

  //! Find the CoEdge for an edge/face/orientation triple (seam edge support).
  //! @param[in] theEdgeDef           edge definition NodeId
  //! @param[in] theFaceDef           face definition NodeId
  //! @param[in] theEdgeOrientation   edge orientation on the face
  Standard_EXPORT const BRepGraphInc::CoEdgeEntity* FindPCurve(
    BRepGraph_NodeId   theEdgeDef,
    BRepGraph_NodeId   theFaceDef,
    TopAbs_Orientation theEdgeOrientation) const;

  //! Find the CoEdge for a given PCurve context.
  //! @param[in] theContext  composite key identifying edge, face and orientation
  Standard_EXPORT const BRepGraphInc::CoEdgeEntity* FindPCurve(
    const BRepGraph_PCurveContext& theContext) const;

  //! Build a GeomAdaptor_TransformedCurve for an edge definition.
  //! @param[in] theEdgeDef edge definition NodeId
  Standard_EXPORT GeomAdaptor_TransformedCurve CurveAdaptor(BRepGraph_NodeId theEdgeDef) const;

  //! Build a GeomAdaptor_TransformedCurve for a specific edge node.
  //! @param[in] theEdgeDef   edge definition NodeId
  //! @param[in] theEdgeNode  edge node id (reserved for per-node transform)
  Standard_EXPORT GeomAdaptor_TransformedCurve CurveAdaptor(BRepGraph_NodeId  theEdgeDef,
                                                            BRepGraph_NodeId  theEdgeNode) const;

  //! Build a curve-on-surface adaptor from edge's inline PCurve on a face.
  //! @param[in] theEdgeDef edge definition NodeId
  //! @param[in] theFaceDef face definition NodeId
  Standard_EXPORT occ::handle<Adaptor3d_CurveOnSurface>
    CurveOnSurfaceAdaptor(BRepGraph_NodeId theEdgeDef,
                          BRepGraph_NodeId theFaceDef) const;

  //! Overload with explicit edge orientation for seam edges.
  //! @param[in] theEdgeDef           edge definition NodeId
  //! @param[in] theFaceDef           face definition NodeId
  //! @param[in] theEdgeOrientation   edge orientation on the face
  Standard_EXPORT occ::handle<Adaptor3d_CurveOnSurface>
    CurveOnSurfaceAdaptor(BRepGraph_NodeId   theEdgeDef,
                          BRepGraph_NodeId   theFaceDef,
                          TopAbs_Orientation theEdgeOrientation) const;

  //! Build a GeomAdaptor_TransformedSurface for a face definition.
  //! @param[in] theFaceDef face definition NodeId
  Standard_EXPORT GeomAdaptor_TransformedSurface SurfaceAdaptor(BRepGraph_NodeId theFaceDef) const;

  //! Build a GeomAdaptor_TransformedSurface with explicit UV bounds.
  //! @param[in] theFaceDef face definition NodeId
  //! @param[in] theUFirst  minimum U parameter
  //! @param[in] theULast   maximum U parameter
  //! @param[in] theVFirst  minimum V parameter
  //! @param[in] theVLast   maximum V parameter
  Standard_EXPORT GeomAdaptor_TransformedSurface SurfaceAdaptor(BRepGraph_NodeId theFaceDef,
                                                                double           theUFirst,
                                                                double           theULast,
                                                                double           theVFirst,
                                                                double           theVLast) const;

private:
  friend class BRepGraph;
  explicit DefsView(const BRepGraph* theGraph) : myGraph(theGraph) {}
  const BRepGraph* myGraph;
};

inline BRepGraph::DefsView BRepGraph::Defs() const { return DefsView(this); }

#endif // _BRepGraph_DefsView_HeaderFile
