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
#include <BRepGraphInc_RepId.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Relations.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <GeomAdaptor_TransformedCurve.hxx>
#include <GeomAdaptor_TransformedSurface.hxx>
#include <NCollection_LinearVector.hxx>
#include <TopLoc_Location.hxx>

class Adaptor3d_CurveOnSurface;
class Geom_Surface;
class Geom_Curve;
class Geom2d_Curve;
class Poly_Triangulation;
class BRepGraph_FacesOfEdge;
class BRepGraph_WiresOfEdge;

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
//! Relations() is the single entry point for ordered topology relation containers.
//! Adjacency helpers return references only into existing relation storage.
//! Ref-owned parent links are exposed as reference-id containers; callers resolve
//! parent definitions through RefsView entries or typed iterators.
class BRepGraph::TopoView
{
public:
  //! @brief Face-oriented topology queries.
  class FaceOps
  {
  public:
    //! Return the total number of face definitions (including soft-removed).
    [[nodiscard]] Standard_EXPORT uint32_t Nb() const;

    //! Return the number of active (non-soft-removed) face definitions.
    [[nodiscard]] Standard_EXPORT uint32_t NbActive() const;

    //! Return the first valid face identifier for iteration.
    [[nodiscard]] BRepGraph_FaceId StartId() const { return BRepGraph_FaceId::Start(); }

    //! Return the past-the-end face identifier (one past the last valid id).
    [[nodiscard]] BRepGraph_FaceId EndId() const { return BRepGraph_FaceId(Nb()); }

    //! Return the definition struct for the given face.
    //! @param[in] theFace typed face identifier
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::FaceDef& Definition(
      const BRepGraph_FaceId theFace) const;

    //! Return the relation struct (adjacency lists) for the given face.
    //! @param[in] theFace typed face identifier
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::FaceRelations& Relations(
      const BRepGraph_FaceId theFace) const;

    //! Return the surface handle for the given face.
    //! May be null if the face has no surface representation.
    //! @param[in] theFace typed face identifier
    [[nodiscard]] Standard_EXPORT occ::handle<Geom_Surface> Surface(
      const BRepGraph_FaceId theFace) const;

    //! Return the active triangulation for the given face.
    //! Returns null if the face has no triangulation or it has been invalidated.
    //! @param[in] theFace typed face identifier
    [[nodiscard]] Standard_EXPORT occ::handle<Poly_Triangulation> ActiveTriangulation(
      const BRepGraph_FaceId theFace) const;

  private:
    friend class TopoView;

    explicit FaceOps(BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    BRepGraph* myGraph;
  };

  //! @brief Edge-oriented topology queries.
  class EdgeOps
  {
  public:
    //! Return the total number of edge definitions (including soft-removed).
    [[nodiscard]] Standard_EXPORT uint32_t Nb() const;

    //! Return the number of active (non-soft-removed) edge definitions.
    [[nodiscard]] Standard_EXPORT uint32_t NbActive() const;

    //! Return the first valid edge identifier for iteration.
    [[nodiscard]] BRepGraph_EdgeId StartId() const { return BRepGraph_EdgeId::Start(); }

    //! Return the past-the-end edge identifier (one past the last valid id).
    [[nodiscard]] BRepGraph_EdgeId EndId() const { return BRepGraph_EdgeId(Nb()); }

    //! Return the definition struct for the given edge.
    //! @param[in] theEdge typed edge identifier
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::EdgeDef& Definition(
      const BRepGraph_EdgeId theEdge) const;

    //! Return the relation struct (adjacency lists) for the given edge.
    //! @param[in] theEdge typed edge identifier
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::EdgeRelations& Relations(
      const BRepGraph_EdgeId theEdge) const;

    //! Return the number of active faces adjacent to the given edge through active coedges.
    //! @param[in] theEdge typed edge definition identifier
    //! @return active adjacent face count
    [[nodiscard]] Standard_EXPORT uint32_t NbFaces(const BRepGraph_EdgeId theEdge) const;

    //! Return an iterator over active wires that reference the given edge through active coedges.
    //! @param[in] theEdge typed edge definition identifier
    //! @return iterator positioned at the first active wire, or at end if none exists
    [[nodiscard]] Standard_EXPORT BRepGraph_WiresOfEdge
      WiresOf(const BRepGraph_EdgeId theEdge) const;

    //! Return an iterator over active wires from a stored edge-coedge relation index.
    //! @param[in] theEdge       typed edge definition identifier
    //! @param[in] theStartIndex zero-based index in EdgeRelations::CoEdgeIds to resume from
    //! @return iterator positioned at the first active wire at or after theStartIndex
    [[nodiscard]] Standard_EXPORT BRepGraph_WiresOfEdge WiresOf(const BRepGraph_EdgeId theEdge,
                                                                const uint32_t theStartIndex) const;

    //! Return an iterator over active faces adjacent to the given edge through active coedges.
    //! @param[in] theEdge typed edge definition identifier
    //! @return iterator positioned at the first active face, or at end if none exists
    [[nodiscard]] Standard_EXPORT BRepGraph_FacesOfEdge
      FacesOf(const BRepGraph_EdgeId theEdge) const;

    //! Return an iterator over active faces from a stored edge-coedge relation index.
    //! @param[in] theEdge       typed edge definition identifier
    //! @param[in] theStartIndex zero-based index in EdgeRelations::CoEdgeIds to resume from
    //! @return iterator positioned at the first active face at or after theStartIndex
    [[nodiscard]] Standard_EXPORT BRepGraph_FacesOfEdge FacesOf(const BRepGraph_EdgeId theEdge,
                                                                const uint32_t theStartIndex) const;

    //! Return the coedges that reference the given edge.
    //! @param[in] theEdge typed edge identifier
    [[nodiscard]] Standard_EXPORT const NCollection_LinearVector<BRepGraph_CoEdgeId>& CoEdges(
      const BRepGraph_EdgeId theEdge) const;

    //! Return the 3D curve handle for the given edge.
    //! May be null if the edge has no 3D curve representation.
    //! @param[in] theEdge typed edge identifier
    [[nodiscard]] Standard_EXPORT occ::handle<Geom_Curve> Curve3D(
      const BRepGraph_EdgeId theEdge) const;

  private:
    friend class TopoView;

    explicit EdgeOps(BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    BRepGraph* myGraph;
  };

  //! @brief Vertex-oriented topology queries.
  class VertexOps
  {
  public:
    //! Return the total number of vertex definitions (including soft-removed).
    [[nodiscard]] Standard_EXPORT uint32_t Nb() const;

    //! Return the number of active (non-soft-removed) vertex definitions.
    [[nodiscard]] Standard_EXPORT uint32_t NbActive() const;

    //! Return the first valid vertex identifier for iteration.
    [[nodiscard]] BRepGraph_VertexId StartId() const { return BRepGraph_VertexId::Start(); }

    //! Return the past-the-end vertex identifier (one past the last valid id).
    [[nodiscard]] BRepGraph_VertexId EndId() const { return BRepGraph_VertexId(Nb()); }

    //! Return the definition struct for the given vertex.
    //! @param[in] theVertex typed vertex identifier
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::VertexDef& Definition(
      const BRepGraph_VertexId theVertex) const;

    //! Return the relation struct (adjacency lists) for the given vertex.
    //! @param[in] theVertex typed vertex identifier
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::VertexRelations& Relations(
      const BRepGraph_VertexId theVertex) const;

    //! Return the edges incident to the given vertex.
    //! @param[in] theVertex typed vertex identifier
    [[nodiscard]] Standard_EXPORT const NCollection_LinearVector<BRepGraph_EdgeId>& Edges(
      const BRepGraph_VertexId theVertex) const;

  private:
    friend class TopoView;

    explicit VertexOps(BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    BRepGraph* myGraph;
  };

  //! @brief Wire-oriented topology queries.
  class WireOps
  {
  public:
    //! Return the total number of wire definitions (including soft-removed).
    [[nodiscard]] Standard_EXPORT uint32_t Nb() const;

    //! Return the number of active (non-soft-removed) wire definitions.
    [[nodiscard]] Standard_EXPORT uint32_t NbActive() const;

    //! Return the first valid wire identifier for iteration.
    [[nodiscard]] BRepGraph_WireId StartId() const { return BRepGraph_WireId::Start(); }

    //! Return the past-the-end wire identifier (one past the last valid id).
    [[nodiscard]] BRepGraph_WireId EndId() const { return BRepGraph_WireId(Nb()); }

    //! Return the definition struct for the given wire.
    //! @param[in] theWire typed wire identifier
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::WireDef& Definition(
      const BRepGraph_WireId theWire) const;

    //! Return the relation struct (adjacency lists) for the given wire.
    //! @param[in] theWire typed wire identifier
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::WireRelations& Relations(
      const BRepGraph_WireId theWire) const;

  private:
    friend class TopoView;

    explicit WireOps(BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    BRepGraph* myGraph;
  };

  //! @brief Shell-oriented topology queries.
  class ShellOps
  {
  public:
    //! Return the total number of shell definitions (including soft-removed).
    [[nodiscard]] Standard_EXPORT uint32_t Nb() const;

    //! Return the number of active (non-soft-removed) shell definitions.
    [[nodiscard]] Standard_EXPORT uint32_t NbActive() const;

    //! Return the first valid shell identifier for iteration.
    [[nodiscard]] BRepGraph_ShellId StartId() const { return BRepGraph_ShellId::Start(); }

    //! Return the past-the-end shell identifier (one past the last valid id).
    [[nodiscard]] BRepGraph_ShellId EndId() const { return BRepGraph_ShellId(Nb()); }

    //! Return the definition struct for the given shell.
    //! @param[in] theShell typed shell identifier
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::ShellDef& Definition(
      const BRepGraph_ShellId theShell) const;

    //! Return the relation struct (adjacency lists) for the given shell.
    //! @param[in] theShell typed shell identifier
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::ShellRelations& Relations(
      const BRepGraph_ShellId theShell) const;

  private:
    friend class TopoView;

    explicit ShellOps(BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    BRepGraph* myGraph;
  };

  //! @brief Solid-oriented topology queries.
  class SolidOps
  {
  public:
    //! Return the total number of solid definitions (including soft-removed).
    [[nodiscard]] Standard_EXPORT uint32_t Nb() const;

    //! Return the number of active (non-soft-removed) solid definitions.
    [[nodiscard]] Standard_EXPORT uint32_t NbActive() const;

    //! Return the first valid solid identifier for iteration.
    [[nodiscard]] BRepGraph_SolidId StartId() const { return BRepGraph_SolidId::Start(); }

    //! Return the past-the-end solid identifier (one past the last valid id).
    [[nodiscard]] BRepGraph_SolidId EndId() const { return BRepGraph_SolidId(Nb()); }

    //! Return the definition struct for the given solid.
    //! @param[in] theSolid typed solid identifier
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::SolidDef& Definition(
      const BRepGraph_SolidId theSolid) const;

    //! Return the relation struct (adjacency lists) for the given solid.
    //! @param[in] theSolid typed solid identifier
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::SolidRelations& Relations(
      const BRepGraph_SolidId theSolid) const;

  private:
    friend class TopoView;

    explicit SolidOps(BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    BRepGraph* myGraph;
  };

  //! @brief Coedge-oriented topology and representation queries.
  class CoEdgeOps
  {
  public:
    //! Return the total number of coedge definitions (including soft-removed).
    [[nodiscard]] Standard_EXPORT uint32_t Nb() const;

    //! Return the number of active (non-soft-removed) coedge definitions.
    [[nodiscard]] Standard_EXPORT uint32_t NbActive() const;

    //! Return the first valid coedge identifier for iteration.
    [[nodiscard]] BRepGraph_CoEdgeId StartId() const { return BRepGraph_CoEdgeId::Start(); }

    //! Return the past-the-end coedge identifier (one past the last valid id).
    [[nodiscard]] BRepGraph_CoEdgeId EndId() const { return BRepGraph_CoEdgeId(Nb()); }

    //! Return the definition struct for the given coedge.
    //! @param[in] theCoEdge typed coedge identifier
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::CoEdgeDef& Definition(
      const BRepGraph_CoEdgeId theCoEdge) const;

    //! Return the parent edge of the given coedge.
    //! @param[in] theCoEdge typed coedge identifier
    [[nodiscard]] Standard_EXPORT BRepGraph_EdgeId Edge(const BRepGraph_CoEdgeId theCoEdge) const;

    //! Return the face that owns the given coedge.
    //! @param[in] theCoEdge typed coedge identifier
    [[nodiscard]] Standard_EXPORT BRepGraph_FaceId Face(const BRepGraph_CoEdgeId theCoEdge) const;

    //! Return the wire that owns the given coedge.
    //! @param[in] theCoEdge typed coedge identifier
    [[nodiscard]] Standard_EXPORT BRepGraph_WireId Wire(const BRepGraph_CoEdgeId theCoEdge) const;

    //! Return the 2D PCurve handle for the given coedge.
    //! May be null if the coedge has no PCurve representation.
    //! @param[in] theCoEdge typed coedge identifier
    [[nodiscard]] Standard_EXPORT occ::handle<Geom2d_Curve> Curve2D(
      const BRepGraph_CoEdgeId theCoEdge) const;

  private:
    friend class TopoView;

    explicit CoEdgeOps(BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    BRepGraph* myGraph;
  };

  //! @brief Compound-oriented topology queries.
  class CompoundOps
  {
  public:
    //! Return the total number of compound definitions (including soft-removed).
    [[nodiscard]] Standard_EXPORT uint32_t Nb() const;

    //! Return the number of active (non-soft-removed) compound definitions.
    [[nodiscard]] Standard_EXPORT uint32_t NbActive() const;

    //! Return the first valid compound identifier for iteration.
    [[nodiscard]] BRepGraph_CompoundId StartId() const { return BRepGraph_CompoundId::Start(); }

    //! Return the past-the-end compound identifier (one past the last valid id).
    [[nodiscard]] BRepGraph_CompoundId EndId() const { return BRepGraph_CompoundId(Nb()); }

    //! Return the definition struct for the given compound.
    //! @param[in] theCompound typed compound identifier
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::CompoundDef& Definition(
      const BRepGraph_CompoundId theCompound) const;

    //! Return the relation struct (child references) for the given compound.
    //! @param[in] theCompound typed compound identifier
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::CompoundRelations& Relations(
      const BRepGraph_CompoundId theCompound) const;

  private:
    friend class TopoView;

    explicit CompoundOps(BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    BRepGraph* myGraph;
  };

  //! @brief Comp-solid oriented topology queries.
  class CompSolidOps
  {
  public:
    //! Return the total number of comp-solid definitions (including soft-removed).
    [[nodiscard]] Standard_EXPORT uint32_t Nb() const;

    //! Return the number of active (non-soft-removed) comp-solid definitions.
    [[nodiscard]] Standard_EXPORT uint32_t NbActive() const;

    //! Return the first valid comp-solid identifier for iteration.
    [[nodiscard]] BRepGraph_CompSolidId StartId() const { return BRepGraph_CompSolidId::Start(); }

    //! Return the past-the-end comp-solid identifier (one past the last valid id).
    [[nodiscard]] BRepGraph_CompSolidId EndId() const { return BRepGraph_CompSolidId(Nb()); }

    //! Return the definition struct for the given comp-solid.
    //! @param[in] theCompSolid typed comp-solid identifier
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::CompSolidDef& Definition(
      const BRepGraph_CompSolidId theCompSolid) const;

    //! Return the relation struct (child solids) for the given comp-solid.
    //! @param[in] theCompSolid typed comp-solid identifier
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::CompSolidRelations& Relations(
      const BRepGraph_CompSolidId theCompSolid) const;

  private:
    friend class TopoView;

    explicit CompSolidOps(BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    BRepGraph* myGraph;
  };

  //! @brief Product-oriented raw assembly queries.
  class ProductOps
  {
  public:
    //! Return the total number of product definitions (including soft-removed).
    [[nodiscard]] Standard_EXPORT uint32_t Nb() const;

    //! Return the number of active (non-soft-removed) product definitions.
    [[nodiscard]] Standard_EXPORT uint32_t NbActive() const;

    //! Return the first valid product identifier for iteration.
    [[nodiscard]] BRepGraph_ProductId StartId() const { return BRepGraph_ProductId::Start(); }

    //! Return the past-the-end product identifier (one past the last valid id).
    [[nodiscard]] BRepGraph_ProductId EndId() const { return BRepGraph_ProductId(Nb()); }

    //! Return the definition struct for the given product.
    //! @param[in] theProduct typed product definition identifier
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::ProductDef& Definition(
      const BRepGraph_ProductId theProduct) const;

    //! Return the relation struct (occurrences, shape root) for the given product.
    //! @param[in] theProduct typed product definition identifier
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::ProductRelations& Relations(
      const BRepGraph_ProductId theProduct) const;

    //! Return the topology root NodeId for the given product.
    //! For assemblies (no topology root) returns an invalid NodeId.
    //! @param[in] theProduct typed product definition identifier
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
    [[nodiscard]] Standard_EXPORT uint32_t NbComponents(const BRepGraph_ProductId theProduct) const;

    //! Return the i-th active child occurrence identifier of a product.
    //! @param[in] theProduct typed product definition identifier
    //! @param[in] theComponentIdx zero-based active occurrence index within the product
    [[nodiscard]] Standard_EXPORT BRepGraph_OccurrenceId
      Component(const BRepGraph_ProductId theProduct, const int theComponentIdx) const;

  private:
    friend class TopoView;

    explicit ProductOps(BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    BRepGraph* myGraph;
  };

  //! @brief Occurrence-oriented raw assembly queries.
  class OccurrenceOps
  {
  public:
    //! Return the total number of occurrence definitions (including soft-removed).
    [[nodiscard]] Standard_EXPORT uint32_t Nb() const;

    //! Return the number of active (non-soft-removed) occurrence definitions.
    [[nodiscard]] Standard_EXPORT uint32_t NbActive() const;

    //! Return the first valid occurrence identifier for iteration.
    [[nodiscard]] BRepGraph_OccurrenceId StartId() const { return BRepGraph_OccurrenceId::Start(); }

    //! Return the past-the-end occurrence identifier (one past the last valid id).
    [[nodiscard]] BRepGraph_OccurrenceId EndId() const { return BRepGraph_OccurrenceId(Nb()); }

    //! Return the definition struct for the given occurrence.
    //! @param[in] theOccurrence typed occurrence identifier
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::OccurrenceDef& Definition(
      const BRepGraph_OccurrenceId theOccurrence) const;

    //! Return the relation struct (parent, placement) for the given occurrence.
    //! @param[in] theOccurrence typed occurrence identifier
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::OccurrenceRelations& Relations(
      const BRepGraph_OccurrenceId theOccurrence) const;

    //! Return the product that this occurrence instantiates.
    //! @param[in] theOccurrence typed occurrence identifier
    [[nodiscard]] Standard_EXPORT BRepGraph_ProductId
      Product(const BRepGraph_OccurrenceId theOccurrence) const;

    //! Return the parent product that owns this occurrence.
    //! @param[in] theOccurrence typed occurrence identifier
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

    explicit OccurrenceOps(BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    BRepGraph* myGraph;
  };

  //! @brief Generic topology and assembly count / meta queries.
  class GenOps
  {
  public:
    //! Return the base definition pointer for any topology node (polymorphic).
    //! Returns null if the node id is invalid, out of range, or soft-removed.
    //! @param[in] theId node identifier (any kind)
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::BaseDef* TopoEntity(
      const BRepGraph_NodeId theId) const;

    //! Return the compound (child) reference identifiers that point to the given node.
    //! @param[in] theChild node identifier
    [[nodiscard]] Standard_EXPORT const NCollection_LinearVector<BRepGraph_ChildRefId>&
                                        CompoundRefIds(const BRepGraph_NodeId theChild) const;

    //! Return the occurrence reference identifiers that point to the given node.
    //! @param[in] theChild node identifier
    [[nodiscard]] Standard_EXPORT const NCollection_LinearVector<BRepGraph_OccurrenceRefId>&
                                        OccurrenceRefIds(const BRepGraph_NodeId theChild) const;

    //! True if the node has at least one compound parent.
    //! @param[in] theNode node identifier
    [[nodiscard]] Standard_EXPORT bool HasCompoundParents(const BRepGraph_NodeId theNode) const;

    //! True if the node has at least one occurrence parent.
    //! @param[in] theNode node identifier
    [[nodiscard]] Standard_EXPORT bool HasOccurrenceParents(const BRepGraph_NodeId theNode) const;

    //! Return the total number of nodes across all topology kinds (including soft-removed).
    [[nodiscard]] Standard_EXPORT uint32_t NbNodes() const;

    //! Return the number of node definitions of the specified kind (including soft-removed).
    [[nodiscard]] Standard_EXPORT uint32_t Nb(const BRepGraph_NodeId::Kind theKind) const;

    //! Return true if the node id kind and index are within storage bounds.
    [[nodiscard]] Standard_EXPORT bool IsValid(const BRepGraph_NodeId theNode) const;

    //! Return true if the node id is valid and not soft-removed.
    [[nodiscard]] Standard_EXPORT bool IsActive(const BRepGraph_NodeId theNode) const;

    //! Return true if the given node is invalid or has been soft-removed.
    //! @param[in] theNode node identifier
    [[nodiscard]] Standard_EXPORT bool IsRemoved(const BRepGraph_NodeId theNode) const;

  private:
    friend class TopoView;

    explicit GenOps(BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    BRepGraph* myGraph;
  };

  //! @brief Analytic geometry representation queries.
  class GeometryOps
  {
  public:
    //! Return the total number of face surface representations (including soft-removed).
    [[nodiscard]] Standard_EXPORT uint32_t NbFaceSurfaces() const;

    //! Return the total number of edge 3D curve representations (including soft-removed).
    [[nodiscard]] Standard_EXPORT uint32_t NbEdgeCurves3D() const;

    //! Return the total number of coedge 2D PCurve representations (including soft-removed).
    [[nodiscard]] Standard_EXPORT uint32_t NbCoEdgeCurves2D() const;

    //! Return the number of active (non-soft-removed) face surface representations.
    [[nodiscard]] Standard_EXPORT uint32_t NbActiveFaceSurfaces() const;

    //! Return the number of active (non-soft-removed) edge 3D curve representations.
    [[nodiscard]] Standard_EXPORT uint32_t NbActiveEdgeCurves3D() const;

    //! Return the number of active (non-soft-removed) coedge 2D PCurve representations.
    [[nodiscard]] Standard_EXPORT uint32_t NbActiveCoEdgeCurves2D() const;

  private:
    friend class TopoView;

    explicit GeometryOps(BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    BRepGraph* myGraph;
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
  //!   for (BRepGraph_FaceId aFId = aGraph.Topo().Faces().StartId();
  //!        aFId < aGraph.Topo().Faces().EndId(); aFId = aFId.Next())
  //!   {
  //!     occ::handle<Geom_Surface> aSurf = aGraph.Topo().Faces().Surface(aFId);
  //!   }
  //! @endcode

private:
  friend class BRepGraph;
  friend struct BRepGraph_Data;
  friend class BRepGraph_Tool;

  explicit TopoView(BRepGraph* theGraph)
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

  BRepGraph*    myGraph;
  FaceOps       myFaces;
  EdgeOps       myEdges;
  VertexOps     myVertices;
  WireOps       myWires;
  ShellOps      myShells;
  SolidOps      mySolids;
  CoEdgeOps     myCoEdges;
  CompoundOps   myCompounds;
  CompSolidOps  myCompSolids;
  ProductOps    myProducts;
  OccurrenceOps myOccurrences;
  GenOps        myGen;
  GeometryOps   myGeometry;
};

#endif // _BRepGraph_TopoView_HeaderFile
