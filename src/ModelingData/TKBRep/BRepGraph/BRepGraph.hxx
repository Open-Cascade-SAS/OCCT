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

#ifndef _BRepGraph_HeaderFile
#define _BRepGraph_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_RefId.hxx>
#include <BRepGraph_RefUID.hxx>
#include <BRepGraphInc_RepId.hxx>
#include <BRepGraph_UID.hxx>
#include <BRepGraph_ItemId.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Trsf.hxx>
#include <Bnd_Box.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_LinearVector.hxx>

#include <memory>

template <typename T>
class BRepGraph_MutGuard;

struct BRepGraph_Data;
class BRepGraphInc_Storage;
class BRepGraph_CacheRegistry;
class BRepGraph_Layer;
class BRepGraph_LayerLock;
class BRepGraph_LayerRegistry;
class BRepGraph_CacheMesh;
class BRepGraph_Validate;
class BRepGraph_Deduplicate;
class BRepGraphODE;
class BRepGraphODE_Storage;
class NCollection_BaseAllocator;
class TCollection_AsciiString;

//! @brief Topology-geometry graph over TopoDS / BRep.
//!
//! Stores B-Rep topology as flat entity vectors (incidence-table model) with
//! integer cross-references, enabling cache-friendly traversal, relation-table
//! parent navigation, and parallel face-level geometry extraction.
//!
//! Key design concepts:
//! - **NodeId** (Kind + Index): lightweight typed address into per-kind vectors.
//! - **UID** (Kind + Counter): persistent identity surviving compaction/reorder.
//! - **RepId** (Kind + Index): separate geometry/mesh addressing (Surface,
//!   Curve3D, Curve2D, Triangulation, Polygon) decoupled from topology nodes.
//! - **CoEdge**: half-edge entity owning PCurve data for each edge-face binding;
//!   seam edges use paired CoEdges with opposite Orientation (Parasolid convention).
//! - **Lifecycle**: Shapes().Add() populates from TopoDS_Shape;
//!   Editor() is the single mutation entry point for both structural creation/removal
//!   (Add*, Remove*, Append*) and field-level RAII-scoped mutation (Mut*()) with
//!   automatic cache invalidation and upward SubtreeGen propagation.
//!
//! Per-occurrence data (orientation, location) lives on incidence refs.
//! Definition types are aliases to BRepGraphInc entity structs.
//!
//! ## Grouped View API
//! Related methods are grouped behind lightweight view objects.
//! Include the corresponding header (e.g. BRepGraph_TopoView.hxx) to use.
//!
//! ## Thread safety
//! Const query methods are safe for concurrent reads.
//! Concurrent reads during active mutation still require external synchronization.
//! Deferred invalidation (BRepGraph_DeferredScope) batches SubtreeGen propagation;
//! concurrent Editor().Mut*() calls during deferred mode still require external
//! serialization.
//! Shapes().Add() is internally parallel when requested.
//!
//! ## UID persistence
//! UIDs use monotonic counters (not vector indices), persisting across Compact()
//! and node removal. Only BRepGraph::Clear() resets counters (new generation).
//! See BRepGraph_UID.hxx for the serialization contract.
//!
//! ## Extension model
//! Extend via BRepGraph_Layer (persistent metadata / observers) or
//! BRepGraph_CacheRegistry (typed algorithm-computed transient cache services).
//! Direct storage extension is not supported.
//!
//! ## ID systems
//! Four ID types with different stability guarantees:
//! - **NodeId** (Kind + per-kind Index): fast graph-local address. NOT stable across Compact().
//!   Use for in-graph traversal and short-lived algorithm temporaries.
//! - **UID** (Kind + monotonic Counter): persistent identity surviving Compact() and node removal.
//!   Use for cross-session storage, history tracking, and external references.
//! - **RefId** (Kind + per-kind Index): same stability as NodeId, but addresses reference entries
//!   (Shell->Solid binding, Face->Shell binding, CoEdge->Wire binding) rather than defs.
//! - **RepId** (Kind + per-kind Index): addresses owner-scoped geometry/mesh representation slots
//!   (Surface, Curve3D, Curve2D, Triangulation, Polygon).
//!
//! ## Iterator guide
//! Choose the iterator that matches your traversal need:
//! - **BRepGraph_Iterator\<NodeType\>**: flat sequential scan of ALL definitions of one kind
//!   (e.g. every FaceDef, skipping removed). Use for bulk per-kind algorithms.
//! - **BRepGraph_DefsIterator / BRepGraph_RefsIterator**: single-level typed children of one
//!   parent (e.g. active shells of one solid, coedges of one wire). Zero allocation.
//!   Use when you have a specific parent and need its direct children.
//! - **BRepGraph_ChildExplorer**: depth-first downward walk from a root with accumulated
//!   location/orientation per step. Use when visiting descendants across multiple levels or
//!   when the global transform matters. Supports Recursive and DirectChildren modes.
//! - **BRepGraph_ParentExplorer**: upward walk via relation tables from a starting node.
//!   Use when tracing which shells/solids/compounds contain a given face or edge.
//! - **BRepGraph_RelatedIterator**: single-level semantic neighbors (adjacent faces, boundary
//!   edges, incident vertices). No structural descent; no location accumulation.
class BRepGraph
{
public:
  DEFINE_STANDARD_ALLOC

  //! Copying is intentionally disabled: BRepGraph is the unique owner of graph data.
  BRepGraph(const BRepGraph&) = delete;
  //! Copying is intentionally disabled: BRepGraph is the unique owner of graph data.
  BRepGraph& operator=(const BRepGraph&) = delete;

  //! Default constructor. Creates an empty graph with default allocator.
  Standard_EXPORT BRepGraph();
  //! Destructor.
  Standard_EXPORT ~BRepGraph();
  //! Move constructor.
  Standard_EXPORT BRepGraph(BRepGraph&&) noexcept;
  //! Move assignment operator.
  Standard_EXPORT BRepGraph& operator=(BRepGraph&&) noexcept;

  //! Reset the graph to an empty state. Increments generation and regenerates the graph GUID.
  Standard_EXPORT void Clear();

  //! Return true when the graph contains no topology definitions.
  [[nodiscard]] Standard_EXPORT bool IsEmpty() const;

  //! Verify relation consistency against entity / reference-entry tables.
  //! Intended for debug builds and regression tests of incremental mutation paths.
  //! @return true when every stored relation matches its endpoints.
  [[nodiscard]] Standard_EXPORT bool ValidateRelations() const;

  //! Return root product identifiers (products not referenced by any active occurrence).
  //! Maintained incrementally by Editor/EditorView mutations.
  //! Returns empty vector if the graph has not been built.
  [[nodiscard]] Standard_EXPORT const NCollection_LinearVector<BRepGraph_ProductId>&
                                      RootProductIds() const;

  //! Return the current allocator.
  [[nodiscard]] Standard_EXPORT const occ::handle<NCollection_BaseAllocator>& Allocator() const;

  //! Return true when this wrapper references graph data.
  [[nodiscard]] Standard_EXPORT bool IsValid() const noexcept;

  //! Return true when this wrapper does not reference graph data.
  [[nodiscard]] bool IsNull() const noexcept { return !IsValid(); }

  class TopoView;
  class UIDsView;
  class RefsView;
  class ShapesView;
  class EditorView;
  class MeshView;

  //! Access topology definitions, representation access, adjacency queries,
  //! raw Product/Occurrence definition storage, and assembly classification.
  [[nodiscard]] Standard_EXPORT const TopoView& Topo() const;
  //! Access unique identifiers.
  [[nodiscard]] Standard_EXPORT const UIDsView& UIDs() const;
  //! Access reference entries and their UIDs.
  [[nodiscard]] Standard_EXPORT const RefsView& Refs() const;
  //! Access cached and fresh shape reconstruction.
  [[nodiscard]] Standard_EXPORT ShapesView& Shapes();
  //! Access shape ingestion, cached shape reconstruction and fresh shape reconstruction.
  [[nodiscard]] Standard_EXPORT const ShapesView& Shapes() const;
  //! Access programmatic graph construction and mutation.
  [[nodiscard]] Standard_EXPORT EditorView& Editor();
  //! Const access to editor-specific state queries.
  //! Exposes IsDeferredMode() and ValidateMutationBoundary() on a const graph.
  //! All structural mutation methods require the non-const Editor() overload.
  [[nodiscard]] Standard_EXPORT const EditorView& Editor() const;
  //! Access mesh data with explicit Cache()/Persistent() sub-views and Editor() for cache
  //! mutations. Persistent rep creation lives on Editor().Edges(), Editor().CoEdges(),
  //! Editor().Faces() (since reps back the topology defs).
  //! @return read-only mesh view
  [[nodiscard]] Standard_EXPORT const MeshView& Mesh() const;
  //! Non-const access to mesh view (required to call Editor() sub-view for cache mutations).
  //! @return mutable mesh view
  [[nodiscard]] Standard_EXPORT MeshView& Mesh();

  //! Access registered graph layers.
  //! @return layer registry for managing attribute layers
  [[nodiscard]] Standard_EXPORT BRepGraph_LayerRegistry& LayerRegistry();
  //! Access registered graph layers (const).
  //! @return layer registry for managing attribute layers
  [[nodiscard]] Standard_EXPORT const BRepGraph_LayerRegistry& LayerRegistry() const;

  //! Access registered graph cache services.
  //! @return cache registry for managing typed transient cache services
  [[nodiscard]] Standard_EXPORT BRepGraph_CacheRegistry& CacheRegistry();
  //! Access registered graph cache services (const).
  //! @return cache registry for managing typed transient cache services
  [[nodiscard]] Standard_EXPORT const BRepGraph_CacheRegistry& CacheRegistry() const;

private:
  friend class BRepGraph_Cache;
  friend class BRepGraph_CacheRegistry;
  friend class BRepGraph_Compact;
  friend class BRepGraph_Copy;
  friend class BRepGraph_Deduplicate;
  friend class BRepGraph_Layer;
  friend class BRepGraph_LayerLock;
  friend class BRepGraph_LayerRegistry;
  friend class BRepGraph_Tool;
  friend class BRepGraph_Transform;
  friend class BRepGraph_Validate;
  friend class BRepGraphInc_Populate;
  friend class BRepGraphInc_Reconstruct;
  friend class BRepGraphODE;
  friend class BRepGraphODE_Storage;
  template <typename>
  friend class BRepGraph_MutGuard;

  friend struct BRepGraph_NodeId;
  friend struct BRepGraph_RefId;
  friend struct BRepGraph_RepId;

  //! Access the underlying storage.
  [[nodiscard]] Standard_EXPORT BRepGraphInc_Storage&       incStorage();
  [[nodiscard]] Standard_EXPORT const BRepGraphInc_Storage& incStorage() const;

  //! Access the graph data structure.
  [[nodiscard]] Standard_EXPORT BRepGraph_Data*       data();
  [[nodiscard]] Standard_EXPORT const BRepGraph_Data* data() const;

  //! Bind graph-owned views and registries to this owner.
  Standard_EXPORT void initViewsAndRegistries() noexcept;

  //! Access the layer registry.
  [[nodiscard]] Standard_EXPORT BRepGraph_LayerRegistry&       layerRegistry();
  [[nodiscard]] Standard_EXPORT const BRepGraph_LayerRegistry& layerRegistry() const;

  //! Access the cache registry.
  [[nodiscard]] Standard_EXPORT BRepGraph_CacheRegistry&       cacheRegistry();
  [[nodiscard]] Standard_EXPORT const BRepGraph_CacheRegistry& cacheRegistry() const;

  //! Generic reference lookup by RefId (const).
  //! Returns nullptr if the RefId is invalid or out of range.
  Standard_EXPORT const BRepGraphInc::BaseRef* refEntity(const BRepGraph_RefId theId) const;

  //! Invalidate reconstructed shapes and dependent caches below a node.
  //! @param[in] theNode root node of the invalidated subgraph
  Standard_EXPORT void invalidateSubgraphImpl(const BRepGraph_NodeId theNode);

  //! Allocate and attach a persistent definition UID for a freshly appended node.
  //! @param[in] theNodeId node slot receiving the UID
  //! @return allocated definition UID
  Standard_EXPORT BRepGraph_UID allocateUID(const BRepGraph_NodeId theNodeId);

  //! Allocate and attach a persistent reference UID for a freshly appended reference.
  //! @param[in] theRefId reference slot receiving the UID
  //! @return allocated reference UID
  Standard_EXPORT BRepGraph_RefUID allocateRefUID(const BRepGraph_RefId theRefId);

  //! Mark a topology definition as modified and propagate cache invalidation.
  //! @param[in] theNodeId modified topology definition
  Standard_EXPORT void markModified(const BRepGraph_NodeId theNodeId) noexcept;

  //! Mark a reference entry as modified and propagate cache invalidation.
  //! @param[in] theRefId modified reference entry
  Standard_EXPORT void markRefModified(const BRepGraph_RefId theRefId) noexcept;

  //! Optimized overload: skips changeTopoEntity() dispatch
  //! when the caller already holds a mutable reference to the target entity.
  Standard_EXPORT void markModified(const BRepGraph_NodeId theNodeId,
                                    BRepGraphInc::BaseDef& theEntity) noexcept;
  //! Increment SubtreeGen on a parent node (NOT OwnGen - parent's own data didn't change).
  //! Uses wave guard to prevent exponential blowup on diamond topologies.
  //! Mutex-free: no shape cache clear, no dispatch.
  Standard_EXPORT void markParentSubtreeGen(const BRepGraph_NodeId theParentId) noexcept;

  //! Propagate SubtreeGen upward through relation tables via markParentSubtreeGen().
  Standard_EXPORT void propagateSubtreeGen(const BRepGraph_NodeId theNodeId) noexcept;

  //! Generic topology definition lookup by NodeId (const).
  Standard_EXPORT const BRepGraphInc::BaseDef* topoEntity(const BRepGraph_NodeId theId) const;

  //! Generic mutable topology definition lookup by NodeId.
  Standard_EXPORT BRepGraphInc::BaseDef* changeTopoEntity(const BRepGraph_NodeId theId);

  // Fields at the bottom (OCCT style)
  std::unique_ptr<BRepGraph_Data> myData;
};

// Included after BRepGraph is complete so the template body sees markModified().
#include <BRepGraph_MutGuard.hxx>

#endif // _BRepGraph_HeaderFile
