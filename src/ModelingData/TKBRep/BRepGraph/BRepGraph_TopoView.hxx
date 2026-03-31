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
//! Product and Occurrence accessors in this view expose raw definition storage
//! only. For assembly-aware classification, child-occurrence traversal, and
//! path-based placement/orientation queries, use PathView.
//! Obtained via BRepGraph::Topo().
//!
//! ## Soft-deletion convention
//! Count methods (NbFaces, NbEdges, etc.) return totals including soft-removed
//! nodes. Prefer NbActive* variants for traversal and validation code that
//! should ignore removed entities. Definition accessors (Face, Edge, etc.) do
//! not filter removed nodes - callers should check
//! IsRemoved() if needed.
//!
//! ## TopoView vs RefsView naming
//! TopoView accessors take definition IDs (BRepGraph_FaceId, BRepGraph_ShellId, etc.)
//! and return definition structs (FaceDef, ShellDef). RefsView accessors take
//! reference IDs (BRepGraph_FaceRefId, BRepGraph_ShellRefId) and return
//! reference-entry structs carrying per-use orientation and location.
//!
//! Reverse-index accessors return const references to internal vectors. The
//! reference itself is always valid; the returned vector may be empty when the
//! queried entity has no parents of that kind.
class BRepGraph::TopoView
{
public:
  //! @brief Face-oriented topology queries.
  class FaceOps
  {
  public:
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::FaceDef& Definition(
      const BRepGraph_FaceId theFace) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_ShellId>& Shells(
      const BRepGraph_FaceId theFace) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_CompoundId>& Compounds(
      const BRepGraph_FaceId theFace) const;
    [[nodiscard]] Standard_EXPORT BRepGraph_SurfaceRepId SurfaceRepId(
      const BRepGraph_FaceId theFace) const;
    [[nodiscard]] Standard_EXPORT BRepGraph_TriangulationRepId ActiveTriangulationRepId(
      const BRepGraph_FaceId theFace) const;
    [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_FaceId> SameDomain(
      const BRepGraph_FaceId                          theFace,
      const occ::handle<NCollection_BaseAllocator>& theAllocator) const;
    [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_EdgeId> SharedEdges(
      const BRepGraph_FaceId                          theFaceA,
      const BRepGraph_FaceId                          theFaceB,
      const occ::handle<NCollection_BaseAllocator>& theAllocator) const;
    [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_FaceId> Adjacent(
      const BRepGraph_FaceId                          theFace,
      const occ::handle<NCollection_BaseAllocator>& theAllocator) const;
    [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_EdgeId> Edges(
      const BRepGraph_FaceId                          theFace,
      const occ::handle<NCollection_BaseAllocator>& theAllocator) const;
    [[nodiscard]] Standard_EXPORT BRepGraph_WireId OuterWire(const BRepGraph_FaceId theFace) const;

  private:
    friend class TopoView;

    explicit FaceOps(const BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    const BRepGraph* myGraph;
  };

  //! @brief Edge-oriented topology queries.
  class EdgeOps
  {
  public:
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::EdgeDef& Definition(
      const BRepGraph_EdgeId theEdge) const;
    [[nodiscard]] Standard_EXPORT int NbFaces(const BRepGraph_EdgeId theEdge) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_WireId>& Wires(
      const BRepGraph_EdgeId theEdge) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_CoEdgeId>& CoEdges(
      const BRepGraph_EdgeId theEdge) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_FaceId>& Faces(
      const BRepGraph_EdgeId theEdge) const;
    [[nodiscard]] Standard_EXPORT BRepGraph_Curve3DRepId Curve3DRepId(
      const BRepGraph_EdgeId theEdge) const;
    [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_VertexId> Vertices(
      const BRepGraph_EdgeId                          theEdge,
      const occ::handle<NCollection_BaseAllocator>& theAllocator) const;
    [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_EdgeId> Adjacent(
      const BRepGraph_EdgeId                          theEdge,
      const occ::handle<NCollection_BaseAllocator>& theAllocator) const;
    [[nodiscard]] Standard_EXPORT bool IsBoundary(const BRepGraph_EdgeId theEdge) const;
    [[nodiscard]] Standard_EXPORT bool IsManifold(const BRepGraph_EdgeId theEdge) const;
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::CoEdgeDef* FindPCurve(
      const BRepGraph_EdgeId theEdge,
      const BRepGraph_FaceId theFace) const;
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::CoEdgeDef* FindPCurve(
      const BRepGraph_EdgeId   theEdge,
      const BRepGraph_FaceId   theFace,
      const TopAbs_Orientation theOrientation) const;

  private:
    friend class TopoView;

    explicit EdgeOps(const BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    const BRepGraph* myGraph;
  };

  //! @brief Vertex-oriented topology queries.
  class VertexOps
  {
  public:
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::VertexDef& Definition(
      const BRepGraph_VertexId theVertex) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_EdgeId>& Edges(
      const BRepGraph_VertexId theVertex) const;
    [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_FaceId> Faces(
      const BRepGraph_VertexId                        theVertex,
      const occ::handle<NCollection_BaseAllocator>& theAllocator) const;

  private:
    friend class TopoView;

    explicit VertexOps(const BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    const BRepGraph* myGraph;
  };

  //! @brief Wire-oriented topology queries.
  class WireOps
  {
  public:
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::WireDef& Definition(
      const BRepGraph_WireId theWire) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_FaceId>& Faces(
      const BRepGraph_WireId theWire) const;
    [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_CoEdgeId> CoEdges(
      const BRepGraph_WireId                          theWire,
      const occ::handle<NCollection_BaseAllocator>& theAllocator) const;
    [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_EdgeId> Edges(
      const BRepGraph_WireId                          theWire,
      const occ::handle<NCollection_BaseAllocator>& theAllocator) const;

  private:
    friend class TopoView;

    explicit WireOps(const BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    const BRepGraph* myGraph;
  };

  //! @brief Shell-oriented topology queries.
  class ShellOps
  {
  public:
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::ShellDef& Definition(
      const BRepGraph_ShellId theShell) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_SolidId>& Solids(
      const BRepGraph_ShellId theShell) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_CompoundId>& Compounds(
      const BRepGraph_ShellId theShell) const;
    [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_FaceId> Faces(
      const BRepGraph_ShellId                         theShell,
      const occ::handle<NCollection_BaseAllocator>& theAllocator) const;
    [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> FreeChildren(
      const BRepGraph_ShellId                         theShell,
      const occ::handle<NCollection_BaseAllocator>& theAllocator) const;

  private:
    friend class TopoView;

    explicit ShellOps(const BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    const BRepGraph* myGraph;
  };

  //! @brief Solid-oriented topology queries.
  class SolidOps
  {
  public:
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::SolidDef& Definition(
      const BRepGraph_SolidId theSolid) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_CompSolidId>& CompSolids(
      const BRepGraph_SolidId theSolid) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_CompoundId>& Compounds(
      const BRepGraph_SolidId theSolid) const;
    [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_ShellId> Shells(
      const BRepGraph_SolidId                         theSolid,
      const occ::handle<NCollection_BaseAllocator>& theAllocator) const;
    [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> FreeChildren(
      const BRepGraph_SolidId                         theSolid,
      const occ::handle<NCollection_BaseAllocator>& theAllocator) const;

  private:
    friend class TopoView;

    explicit SolidOps(const BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    const BRepGraph* myGraph;
  };

  //! @brief Coedge-oriented topology and representation queries.
  class CoEdgeOps
  {
  public:
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::CoEdgeDef& Definition(
      const BRepGraph_CoEdgeId theCoEdge) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_WireId>& Wires(
      const BRepGraph_CoEdgeId theCoEdge) const;
    [[nodiscard]] Standard_EXPORT BRepGraph_EdgeId Edge(const BRepGraph_CoEdgeId theCoEdge) const;
    [[nodiscard]] Standard_EXPORT BRepGraph_FaceId Face(const BRepGraph_CoEdgeId theCoEdge) const;
    [[nodiscard]] Standard_EXPORT BRepGraph_Curve2DRepId Curve2DRepId(
      const BRepGraph_CoEdgeId theCoEdge) const;
    [[nodiscard]] Standard_EXPORT BRepGraph_CoEdgeId SeamPair(
      const BRepGraph_CoEdgeId theCoEdge) const;

  private:
    friend class TopoView;

    explicit CoEdgeOps(const BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    const BRepGraph* myGraph;
  };

  //! @brief Compound-oriented topology queries.
  class CompoundOps
  {
  public:
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::CompoundDef& Definition(
      const BRepGraph_CompoundId theCompound) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_CompoundId>& ParentCompounds(
      const BRepGraph_CompoundId theCompound) const;
    [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> Children(
      const BRepGraph_CompoundId                      theCompound,
      const occ::handle<NCollection_BaseAllocator>& theAllocator) const;

  private:
    friend class TopoView;

    explicit CompoundOps(const BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    const BRepGraph* myGraph;
  };

  //! @brief Comp-solid oriented topology queries.
  class CompSolidOps
  {
  public:
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::CompSolidDef& Definition(
      const BRepGraph_CompSolidId theCompSolid) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_CompoundId>& Compounds(
      const BRepGraph_CompSolidId theCompSolid) const;
    [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_SolidId> Solids(
      const BRepGraph_CompSolidId                     theCompSolid,
      const occ::handle<NCollection_BaseAllocator>& theAllocator) const;

  private:
    friend class TopoView;

    explicit CompSolidOps(const BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    const BRepGraph* myGraph;
  };

  //! @brief Product-oriented raw assembly queries.
  class ProductOps
  {
  public:
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::ProductDef& Definition(
      const BRepGraph_ProductId theProduct) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_OccurrenceId>& Instances(
      const BRepGraph_ProductId theProduct) const;
    [[nodiscard]] Standard_EXPORT BRepGraph_NodeId ShapeRoot(
      const BRepGraph_ProductId theProduct) const;
    [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_OccurrenceId> Occurrences(
      const BRepGraph_ProductId                       theProduct,
      const occ::handle<NCollection_BaseAllocator>& theAllocator) const;

  private:
    friend class TopoView;

    explicit ProductOps(const BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    const BRepGraph* myGraph;
  };

  //! @brief Occurrence-oriented raw assembly queries.
  class OccurrenceOps
  {
  public:
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::OccurrenceDef& Definition(
      const BRepGraph_OccurrenceId theOccurrence) const;
    [[nodiscard]] Standard_EXPORT BRepGraph_ProductId Product(
      const BRepGraph_OccurrenceId theOccurrence) const;
    [[nodiscard]] Standard_EXPORT BRepGraph_ProductId ParentProduct(
      const BRepGraph_OccurrenceId theOccurrence) const;
    [[nodiscard]] Standard_EXPORT BRepGraph_OccurrenceId ParentOccurrence(
      const BRepGraph_OccurrenceId theOccurrence) const;

  private:
    friend class TopoView;

    explicit OccurrenceOps(const BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    const BRepGraph* myGraph;
  };

  //! Grouped face-oriented queries.
  [[nodiscard]] FaceOps Faces() const { return FaceOps(myGraph); }

  //! Grouped edge-oriented queries.
  [[nodiscard]] EdgeOps Edges() const { return EdgeOps(myGraph); }

  //! Grouped vertex-oriented queries.
  [[nodiscard]] VertexOps Vertices() const { return VertexOps(myGraph); }

  //! Grouped wire-oriented queries.
  [[nodiscard]] WireOps Wires() const { return WireOps(myGraph); }

  //! Grouped shell-oriented queries.
  [[nodiscard]] ShellOps Shells() const { return ShellOps(myGraph); }

  //! Grouped solid-oriented queries.
  [[nodiscard]] SolidOps Solids() const { return SolidOps(myGraph); }

  //! Grouped coedge-oriented queries.
  [[nodiscard]] CoEdgeOps CoEdges() const { return CoEdgeOps(myGraph); }

  //! Grouped compound-oriented queries.
  [[nodiscard]] CompoundOps Compounds() const { return CompoundOps(myGraph); }

  //! Grouped comp-solid oriented queries.
  [[nodiscard]] CompSolidOps CompSolids() const { return CompSolidOps(myGraph); }

  //! Grouped product-oriented queries.
  [[nodiscard]] ProductOps Products() const { return ProductOps(myGraph); }

  //! Grouped occurrence-oriented queries.
  [[nodiscard]] OccurrenceOps Occurrences() const { return OccurrenceOps(myGraph); }

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

  //! Generic node-definition lookup by NodeId (topology or assembly).
  //! @param[in] theId node identifier
  //! @return pointer to BaseDef or nullptr if invalid
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::BaseDef* TopoEntity(
    const BRepGraph_NodeId theId) const;

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
  //! Use PathView for part-vs-assembly classification, root-product discovery,
  //! active child-occurrence traversal, and placement/path queries.

  //! Number of product definitions.
  [[nodiscard]] Standard_EXPORT int NbProducts() const;

  //! Number of occurrence definitions.
  [[nodiscard]] Standard_EXPORT int NbOccurrences() const;

  //! Number of active (non-removed) product definitions.
  [[nodiscard]] Standard_EXPORT int NbActiveProducts() const;

  //! Number of active (non-removed) occurrence definitions.
  [[nodiscard]] Standard_EXPORT int NbActiveOccurrences() const;

private:
  friend class BRepGraph;
  friend struct BRepGraph_Data;
  friend class BRepGraph_Tool;
  friend class BRepGraph_Analyze;

  explicit TopoView(const BRepGraph* theGraph)
      : myGraph(theGraph)
  {
  }

  const BRepGraph* myGraph;
};

#endif // _BRepGraph_TopoView_HeaderFile
