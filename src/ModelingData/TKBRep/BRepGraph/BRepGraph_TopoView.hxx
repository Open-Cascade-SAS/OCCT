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
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <GeomAdaptor_TransformedCurve.hxx>
#include <GeomAdaptor_TransformedSurface.hxx>
#include <TopLoc_Location.hxx>

class Adaptor3d_CurveOnSurface;

//! @brief Unified read-only view over topology definitions, adjacency, and representations.
//!
//! Provides topology definition lookup, representation lookup, read-only
//! adjacency queries, and assembly classification over the incidence-table
//! model stored in BRepGraph.
//! Obtained via BRepGraph::Topo().
//!
//! ## Soft-deletion convention
//! Per-kind count methods (Faces().Nb(), Edges().Nb(), etc.) return totals
//! including soft-removed nodes. Prefer per-kind NbActive() variants for
//! traversal and validation code that should ignore removed entities.
//! Definition accessors (Face, Edge, etc.) do not filter removed nodes - callers should check
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
    [[nodiscard]] Standard_EXPORT int Nb() const;
    [[nodiscard]] Standard_EXPORT int NbActive() const;

    [[nodiscard]] BRepGraph_FaceId StartId() const { return BRepGraph_FaceId::Start(); }

    [[nodiscard]] BRepGraph_FaceId EndId() const { return BRepGraph_FaceId(Nb()); }

    [[nodiscard]] Standard_EXPORT const BRepGraphInc::FaceDef& Definition(
      const BRepGraph_FaceId theFace) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_ShellId>& Shells(
      const BRepGraph_FaceId theFace) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_CompoundId>& Compounds(
      const BRepGraph_FaceId theFace) const;
    [[nodiscard]] Standard_EXPORT BRepGraph_SurfaceRepId
      SurfaceRepId(const BRepGraph_FaceId theFace) const;
    [[nodiscard]] Standard_EXPORT BRepGraph_TriangulationRepId
                                  ActiveTriangulationRepId(const BRepGraph_FaceId theFace) const;
    [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_FaceId> SameDomain(
      const BRepGraph_FaceId                        theFace,
      const occ::handle<NCollection_BaseAllocator>& theAllocator) const;
    [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_EdgeId> SharedEdges(
      const BRepGraph_FaceId                        theFaceA,
      const BRepGraph_FaceId                        theFaceB,
      const occ::handle<NCollection_BaseAllocator>& theAllocator) const;
    [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_FaceId> Adjacent(
      const BRepGraph_FaceId                        theFace,
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
    [[nodiscard]] Standard_EXPORT int Nb() const;
    [[nodiscard]] Standard_EXPORT int NbActive() const;

    [[nodiscard]] BRepGraph_EdgeId StartId() const { return BRepGraph_EdgeId::Start(); }

    [[nodiscard]] BRepGraph_EdgeId EndId() const { return BRepGraph_EdgeId(Nb()); }

    [[nodiscard]] Standard_EXPORT const BRepGraphInc::EdgeDef& Definition(
      const BRepGraph_EdgeId theEdge) const;
    [[nodiscard]] Standard_EXPORT int   NbFaces(const BRepGraph_EdgeId theEdge) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_WireId>& Wires(
      const BRepGraph_EdgeId theEdge) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_CoEdgeId>& CoEdges(
      const BRepGraph_EdgeId theEdge) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_FaceId>& Faces(
      const BRepGraph_EdgeId theEdge) const;
    [[nodiscard]] Standard_EXPORT BRepGraph_Curve3DRepId
                                  Curve3DRepId(const BRepGraph_EdgeId theEdge) const;
    [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_EdgeId> Adjacent(
      const BRepGraph_EdgeId                        theEdge,
      const occ::handle<NCollection_BaseAllocator>& theAllocator) const;
    [[nodiscard]] Standard_EXPORT bool  IsBoundary(const BRepGraph_EdgeId theEdge) const;
    [[nodiscard]] Standard_EXPORT bool  IsManifold(const BRepGraph_EdgeId theEdge) const;
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::CoEdgeDef* FindPCurve(
      const BRepGraph_EdgeId theEdge,
      const BRepGraph_FaceId theFace) const;
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::CoEdgeDef* FindPCurve(
      const BRepGraph_EdgeId   theEdge,
      const BRepGraph_FaceId   theFace,
      const TopAbs_Orientation theOrientation) const;

    //! Find the CoEdgeId for a given (edge, face) pair.
    //! @param[in] theEdge edge to look up
    //! @param[in] theFace face the edge belongs to
    //! @return CoEdgeId, or invalid if no coedge binds this edge to this face
    [[nodiscard]] Standard_EXPORT BRepGraph_CoEdgeId
      FindCoEdgeId(const BRepGraph_EdgeId theEdge, const BRepGraph_FaceId theFace) const;

    //! Find the CoEdgeId for a given (edge, face, orientation) triple.
    //! Useful for seam edges where two coedges share the same face.
    //! @param[in] theEdge        edge to look up
    //! @param[in] theFace        face the edge belongs to
    //! @param[in] theOrientation orientation to match (FORWARD or REVERSED)
    //! @return CoEdgeId, or invalid if no coedge matches
    [[nodiscard]] Standard_EXPORT BRepGraph_CoEdgeId
      FindCoEdgeId(const BRepGraph_EdgeId   theEdge,
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
    [[nodiscard]] Standard_EXPORT int Nb() const;
    [[nodiscard]] Standard_EXPORT int NbActive() const;

    [[nodiscard]] BRepGraph_VertexId StartId() const { return BRepGraph_VertexId::Start(); }

    [[nodiscard]] BRepGraph_VertexId EndId() const { return BRepGraph_VertexId(Nb()); }

    [[nodiscard]] Standard_EXPORT const BRepGraphInc::VertexDef& Definition(
      const BRepGraph_VertexId theVertex) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_EdgeId>& Edges(
      const BRepGraph_VertexId theVertex) const;

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
    [[nodiscard]] Standard_EXPORT int Nb() const;
    [[nodiscard]] Standard_EXPORT int NbActive() const;

    [[nodiscard]] BRepGraph_WireId StartId() const { return BRepGraph_WireId::Start(); }

    [[nodiscard]] BRepGraph_WireId EndId() const { return BRepGraph_WireId(Nb()); }

    [[nodiscard]] Standard_EXPORT const BRepGraphInc::WireDef& Definition(
      const BRepGraph_WireId theWire) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_FaceId>& Faces(
      const BRepGraph_WireId theWire) const;

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
    [[nodiscard]] Standard_EXPORT int Nb() const;
    [[nodiscard]] Standard_EXPORT int NbActive() const;

    [[nodiscard]] BRepGraph_ShellId StartId() const { return BRepGraph_ShellId::Start(); }

    [[nodiscard]] BRepGraph_ShellId EndId() const { return BRepGraph_ShellId(Nb()); }

    [[nodiscard]] Standard_EXPORT const BRepGraphInc::ShellDef& Definition(
      const BRepGraph_ShellId theShell) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_SolidId>& Solids(
      const BRepGraph_ShellId theShell) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_CompoundId>& Compounds(
      const BRepGraph_ShellId theShell) const;

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
    [[nodiscard]] Standard_EXPORT int Nb() const;
    [[nodiscard]] Standard_EXPORT int NbActive() const;

    [[nodiscard]] BRepGraph_SolidId StartId() const { return BRepGraph_SolidId::Start(); }

    [[nodiscard]] BRepGraph_SolidId EndId() const { return BRepGraph_SolidId(Nb()); }

    [[nodiscard]] Standard_EXPORT const BRepGraphInc::SolidDef& Definition(
      const BRepGraph_SolidId theSolid) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_CompSolidId>& CompSolids(
      const BRepGraph_SolidId theSolid) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_CompoundId>& Compounds(
      const BRepGraph_SolidId theSolid) const;

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
    [[nodiscard]] Standard_EXPORT int Nb() const;
    [[nodiscard]] Standard_EXPORT int NbActive() const;

    [[nodiscard]] BRepGraph_CoEdgeId StartId() const { return BRepGraph_CoEdgeId::Start(); }

    [[nodiscard]] BRepGraph_CoEdgeId EndId() const { return BRepGraph_CoEdgeId(Nb()); }

    [[nodiscard]] Standard_EXPORT const BRepGraphInc::CoEdgeDef& Definition(
      const BRepGraph_CoEdgeId theCoEdge) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_WireId>& Wires(
      const BRepGraph_CoEdgeId theCoEdge) const;
    [[nodiscard]] Standard_EXPORT BRepGraph_EdgeId Edge(const BRepGraph_CoEdgeId theCoEdge) const;
    [[nodiscard]] Standard_EXPORT BRepGraph_FaceId Face(const BRepGraph_CoEdgeId theCoEdge) const;
    [[nodiscard]] Standard_EXPORT BRepGraph_Curve2DRepId
      Curve2DRepId(const BRepGraph_CoEdgeId theCoEdge) const;
    [[nodiscard]] Standard_EXPORT BRepGraph_CoEdgeId
      SeamPair(const BRepGraph_CoEdgeId theCoEdge) const;

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
    [[nodiscard]] Standard_EXPORT int Nb() const;
    [[nodiscard]] Standard_EXPORT int NbActive() const;

    [[nodiscard]] BRepGraph_CompoundId StartId() const { return BRepGraph_CompoundId::Start(); }

    [[nodiscard]] BRepGraph_CompoundId EndId() const { return BRepGraph_CompoundId(Nb()); }

    [[nodiscard]] Standard_EXPORT const BRepGraphInc::CompoundDef& Definition(
      const BRepGraph_CompoundId theCompound) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_CompoundId>& ParentCompounds(
      const BRepGraph_CompoundId theCompound) const;

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
    [[nodiscard]] Standard_EXPORT int Nb() const;
    [[nodiscard]] Standard_EXPORT int NbActive() const;

    [[nodiscard]] BRepGraph_CompSolidId StartId() const { return BRepGraph_CompSolidId::Start(); }

    [[nodiscard]] BRepGraph_CompSolidId EndId() const { return BRepGraph_CompSolidId(Nb()); }

    [[nodiscard]] Standard_EXPORT const BRepGraphInc::CompSolidDef& Definition(
      const BRepGraph_CompSolidId theCompSolid) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_CompoundId>& Compounds(
      const BRepGraph_CompSolidId theCompSolid) const;

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
    [[nodiscard]] Standard_EXPORT int Nb() const;
    [[nodiscard]] Standard_EXPORT int NbActive() const;

    [[nodiscard]] BRepGraph_ProductId StartId() const { return BRepGraph_ProductId::Start(); }

    [[nodiscard]] BRepGraph_ProductId EndId() const { return BRepGraph_ProductId(Nb()); }

    [[nodiscard]] Standard_EXPORT const BRepGraphInc::ProductDef& Definition(
      const BRepGraph_ProductId theProduct) const;
    [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_OccurrenceId>& Instances(
      const BRepGraph_ProductId theProduct) const;
    [[nodiscard]] Standard_EXPORT BRepGraph_NodeId
      ShapeRoot(const BRepGraph_ProductId theProduct) const;

    //! True if the product is an assembly (has active child occurrences and no topology root).
    //! @param[in] theProduct typed product definition identifier
    [[nodiscard]] Standard_EXPORT bool IsAssembly(const BRepGraph_ProductId theProduct) const;

    //! True if the product is a part (has a valid topology root).
    //! @param[in] theProduct typed product definition identifier
    [[nodiscard]] Standard_EXPORT bool IsPart(const BRepGraph_ProductId theProduct) const;

    //! Return the topology root NodeId for a part product.
    //! For assemblies (no topology root) returns an invalid NodeId.
    //! @param[in] theProduct typed product definition identifier
    [[nodiscard]] Standard_EXPORT BRepGraph_NodeId
      ShapeRootNode(const BRepGraph_ProductId theProduct) const;

    //! Number of active child occurrences of a product.
    //! @param[in] theProduct typed product definition identifier
    [[nodiscard]] Standard_EXPORT int NbComponents(const BRepGraph_ProductId theProduct) const;

    //! Return the i-th active child occurrence identifier of a product.
    //! @param[in] theProduct typed product definition identifier
    //! @param[in] theComponentIdx zero-based active occurrence index within the product
    [[nodiscard]] Standard_EXPORT BRepGraph_OccurrenceId
      Component(const BRepGraph_ProductId theProduct, const int theComponentIdx) const;

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
    [[nodiscard]] Standard_EXPORT int Nb() const;
    [[nodiscard]] Standard_EXPORT int NbActive() const;

    [[nodiscard]] BRepGraph_OccurrenceId StartId() const { return BRepGraph_OccurrenceId::Start(); }

    [[nodiscard]] BRepGraph_OccurrenceId EndId() const { return BRepGraph_OccurrenceId(Nb()); }

    [[nodiscard]] Standard_EXPORT const BRepGraphInc::OccurrenceDef& Definition(
      const BRepGraph_OccurrenceId theOccurrence) const;
    [[nodiscard]] Standard_EXPORT BRepGraph_ProductId
      Product(const BRepGraph_OccurrenceId theOccurrence) const;
    [[nodiscard]] Standard_EXPORT BRepGraph_ProductId
      ParentProduct(const BRepGraph_OccurrenceId theOccurrence) const;
    //! Return the local placement of an occurrence (OccurrenceRef::LocalLocation).
    //! This is the placement relative to the parent product, not the global placement.
    //! For global placement, use ChildExplorer with cumulative location tracking.
    //! @param[in] theOccurrence typed occurrence identifier
    //! @return OccurrenceRef::LocalLocation, or identity if not found
    [[nodiscard]] Standard_EXPORT TopLoc_Location
      OccurrenceLocation(const BRepGraph_OccurrenceId theOccurrence) const;

  private:
    friend class TopoView;

    explicit OccurrenceOps(const BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    const BRepGraph* myGraph;
  };

  //! @brief Generic topology and assembly count / meta queries.
  class GenOps
  {
  public:
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::BaseDef* TopoEntity(
      const BRepGraph_NodeId theId) const;
    [[nodiscard]] Standard_EXPORT int  NbNodes() const;
    [[nodiscard]] Standard_EXPORT bool IsRemoved(const BRepGraph_NodeId theNode) const;

  private:
    friend class TopoView;

    explicit GenOps(const BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    const BRepGraph* myGraph;
  };

  //! @brief Analytic geometry representation queries.
  class GeometryOps
  {
  public:
    [[nodiscard]] Standard_EXPORT int NbSurfaces() const;
    [[nodiscard]] Standard_EXPORT int NbCurves3D() const;
    [[nodiscard]] Standard_EXPORT int NbCurves2D() const;

    [[nodiscard]] Standard_EXPORT int NbActiveSurfaces() const;
    [[nodiscard]] Standard_EXPORT int NbActiveCurves3D() const;
    [[nodiscard]] Standard_EXPORT int NbActiveCurves2D() const;

    [[nodiscard]] Standard_EXPORT const BRepGraphInc::SurfaceRep& SurfaceRep(
      const BRepGraph_SurfaceRepId theRep) const;
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::Curve3DRep& Curve3DRep(
      const BRepGraph_Curve3DRepId theRep) const;
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::Curve2DRep& Curve2DRep(
      const BRepGraph_Curve2DRepId theRep) const;

  private:
    friend class TopoView;

    explicit GeometryOps(const BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    const BRepGraph* myGraph;
  };

  //! Grouped face-oriented queries.
  [[nodiscard]] const FaceOps& Faces() const { return myFaces; }

  //! Grouped edge-oriented queries.
  [[nodiscard]] const EdgeOps& Edges() const { return myEdges; }

  //! Grouped vertex-oriented queries.
  [[nodiscard]] const VertexOps& Vertices() const { return myVertices; }

  //! Grouped wire-oriented queries.
  [[nodiscard]] const WireOps& Wires() const { return myWires; }

  //! Grouped shell-oriented queries.
  [[nodiscard]] const ShellOps& Shells() const { return myShells; }

  //! Grouped solid-oriented queries.
  [[nodiscard]] const SolidOps& Solids() const { return mySolids; }

  //! Grouped coedge-oriented queries.
  [[nodiscard]] const CoEdgeOps& CoEdges() const { return myCoEdges; }

  //! Grouped compound-oriented queries.
  [[nodiscard]] const CompoundOps& Compounds() const { return myCompounds; }

  //! Grouped comp-solid oriented queries.
  [[nodiscard]] const CompSolidOps& CompSolids() const { return myCompSolids; }

  //! Grouped product-oriented queries.
  [[nodiscard]] const ProductOps& Products() const { return myProducts; }

  //! Grouped occurrence-oriented queries.
  [[nodiscard]] const OccurrenceOps& Occurrences() const { return myOccurrences; }

  //! Grouped generic topology and assembly counts / meta queries.
  [[nodiscard]] const GenOps& Gen() const { return myGen; }

  //! Grouped analytic geometry representation queries.
  [[nodiscard]] const GeometryOps& Geometry() const { return myGeometry; }

  //! Representations use dense 0-based indexing. Iterate through grouped accessors:
  //! @code
  //!   for (int i = 0; i < aGraph.Topo().Geometry().NbSurfaces(); ++i)
  //!   {
  //!     const BRepGraphInc::SurfaceRep& aRep =
  //!       aGraph.Topo().Geometry().SurfaceRep(BRepGraph_SurfaceRepId(i));
  //!   }
  //! @endcode

private:
  friend class BRepGraph;
  friend struct BRepGraph_Data;
  friend class BRepGraph_Tool;

  explicit TopoView(const BRepGraph* theGraph)
      : myGraph(theGraph),
        myFaces(theGraph),
        myEdges(theGraph),
        myVertices(theGraph),
        myWires(theGraph),
        myShells(theGraph),
        mySolids(theGraph),
        myCoEdges(theGraph),
        myCompounds(theGraph),
        myCompSolids(theGraph),
        myProducts(theGraph),
        myOccurrences(theGraph),
        myGen(theGraph),
        myGeometry(theGraph)
  {
  }

  const BRepGraph* myGraph;
  FaceOps          myFaces;
  EdgeOps          myEdges;
  VertexOps        myVertices;
  WireOps          myWires;
  ShellOps         myShells;
  SolidOps         mySolids;
  CoEdgeOps        myCoEdges;
  CompoundOps      myCompounds;
  CompSolidOps     myCompSolids;
  ProductOps       myProducts;
  OccurrenceOps    myOccurrences;
  GenOps           myGen;
  GeometryOps      myGeometry;
};

#endif // _BRepGraph_TopoView_HeaderFile
