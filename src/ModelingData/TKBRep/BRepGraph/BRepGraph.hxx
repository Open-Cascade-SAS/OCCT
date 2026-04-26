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
#include <BRepGraph_RepId.hxx>
#include <BRepGraph_UID.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <BRepGraph_HistoryRecord.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_RefTransientCache.hxx>
#include <BRepGraph_TransientCache.hxx>

#include <Standard_DefineAlloc.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Trsf.hxx>
#include <Bnd_Box.hxx>
#include <gp_Pnt.hxx>

#include <NCollection_DynamicArray.hxx>
#include <NCollection_DataMap.hxx>

#include <memory>

template <typename T>
class BRepGraph_MutGuard;

struct BRepGraph_Data;
class BRepGraphInc_Storage;
class BRepGraph_Layer;
class BRepGraph_MeshCacheStorage;
class NCollection_BaseAllocator;
class TCollection_AsciiString;

class BRepGraph_Builder;
class BRepGraph_History;

//! @brief Topology-geometry graph over TopoDS / BRep.
//!
//! Stores B-Rep topology as flat entity vectors (incidence-table model) with
//! integer cross-references, enabling cache-friendly traversal, O(1) upward
//! navigation via reverse indices, and parallel face-level geometry extraction.
//!
//! Key design concepts:
//! - **NodeId** (Kind + Index): lightweight typed address into per-kind vectors.
//! - **UID** (Kind + Counter): persistent identity surviving compaction/reorder.
//! - **RepId** (Kind + Index): separate geometry/mesh addressing (Surface,
//!   Curve3D, Curve2D, Triangulation, Polygon) decoupled from topology nodes.
//! - **CoEdge**: half-edge entity owning PCurve data for each edge-face binding;
//!   seam edges use paired CoEdges with opposite Orientation (Parasolid convention).
//! - **Lifecycle**: BRepGraph_Builder::Add() populates from TopoDS_Shape;
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
//! BRepGraph_Builder::Add() is internally parallel when requested.
//!
//! ## UID persistence
//! UIDs use monotonic counters (not vector indices), persisting across Compact()
//! and node removal. Only BRepGraph::Clear() resets counters (new generation).
//! See BRepGraph_UID.hxx for the serialization contract.
//!
//! ## Extension model
//! Extend via BRepGraph_Layer (per-node attributes) or BRepGraph_TransientCache
//! (algorithm-computed caches). Direct storage extension is not supported.
//!
//! ## ID systems
//! Four ID types with different stability guarantees:
//! - **NodeId** (Kind + per-kind Index): fast graph-local address. NOT stable across Compact().
//!   Use for in-graph traversal and short-lived algorithm temporaries.
//! - **UID** (Kind + monotonic Counter): persistent identity surviving Compact() and node removal.
//!   Use for cross-session storage, history tracking, and external references.
//! - **RefId** (Kind + per-kind Index): same stability as NodeId, but addresses reference entries
//!   (Shell->Solid binding, Face->Shell binding, CoEdge->Wire binding) rather than defs.
//! - **RepId** (Kind + per-kind Index): addresses geometry/mesh representation objects (Surface,
//!   Curve3D, Curve2D, Triangulation, Polygon) independently of topology nodes.
//!
//! ## Iterator guide
//! Choose the iterator that matches your traversal need:
//! - **BRepGraph_Iterator\<NodeType\>**: flat sequential scan of ALL definitions of one kind
//!   (e.g. every FaceDef, skipping removed). Use for bulk per-kind algorithms.
//! - **BRepGraph_DefsIterator / BRepGraph_RefsIterator**: single-level typed children of one
//!   parent (e.g. active shells of one solid, coedge refs of one wire). Zero allocation.
//!   Use when you have a specific parent and need its direct children.
//! - **BRepGraph_ChildExplorer**: depth-first downward walk from a root with accumulated
//!   location/orientation per step. Use when visiting descendants across multiple levels or
//!   when the global transform matters. Supports Recursive and DirectChildren modes.
//! - **BRepGraph_ParentExplorer**: upward walk via reverse indices from a starting node.
//!   Use when tracing which shells/solids/compounds contain a given face or edge.
//! - **BRepGraph_RelatedIterator**: single-level semantic neighbors (adjacent faces, boundary
//!   edges, incident vertices). No structural descent; no location accumulation.
//! - **BRepGraph_WireExplorer**: ordered edge traversal within a single wire, following
//!   connectivity order (graph equivalent of BRepTools_WireExplorer).
class BRepGraph
{
public:
  DEFINE_STANDARD_ALLOC

  BRepGraph(const BRepGraph&)            = delete;
  BRepGraph& operator=(const BRepGraph&) = delete;

  //! Default constructor. Creates an empty graph with default allocator.
  Standard_EXPORT BRepGraph();
  //! Construct with a custom allocator for internal collections.
  //! @param[in] theAlloc allocator for internal collections (null uses CommonBaseAllocator)
  Standard_EXPORT explicit BRepGraph(const occ::handle<NCollection_BaseAllocator>& theAlloc);
  //! Destructor.
  Standard_EXPORT ~BRepGraph();
  //! Move constructor.
  Standard_EXPORT BRepGraph(BRepGraph&&) noexcept;
  //! Move assignment operator.
  Standard_EXPORT BRepGraph& operator=(BRepGraph&&) noexcept;

  //! Reset the graph to an empty state. Clears storage, history, root products,
  //! transient caches, and notifies all registered layers. Increments generation
  //! and regenerates the graph GUID. After Clear(), IsDone() returns false; pass
  //! the graph to BRepGraph_Builder::Add() to repopulate.
  Standard_EXPORT void Clear();

  //! Return true if the graph was successfully built.
  [[nodiscard]] Standard_EXPORT bool IsDone() const;

  //! Return root product identifiers (products not referenced by any active occurrence).
  //! Maintained incrementally by Editor/EditorView mutations.
  //! Returns empty vector if the graph has not been built.
  [[nodiscard]] Standard_EXPORT const NCollection_DynamicArray<BRepGraph_ProductId>&
                                      RootProductIds() const;

  //! Replace the internal allocator and re-create all storage.
  Standard_EXPORT void SetAllocator(const occ::handle<NCollection_BaseAllocator>& theAlloc);

  //! Return the current allocator.
  [[nodiscard]] Standard_EXPORT const occ::handle<NCollection_BaseAllocator>& Allocator() const;

public:
  //! Shared cache for edge/vertex shapes during multi-face reconstruction.
  using ReconstructCache = NCollection_DataMap<BRepGraph_NodeId, TopoDS_Shape>;

  class TopoView;
  class UIDsView;
  class CacheView;
  class RefsView;
  class ShapesView;
  class EditorView;
  class MeshView;

  //! Access topology definitions, representation access, adjacency queries,
  //! raw Product/Occurrence definition storage, and assembly classification.
  [[nodiscard]] Standard_EXPORT const TopoView& Topo() const;
  //! Access unique identifiers.
  [[nodiscard]] Standard_EXPORT const UIDsView& UIDs() const;
  //! Access transient cache values through the stable grouped-view API.
  //! This is the only public cache interface.
  [[nodiscard]] Standard_EXPORT CacheView& Cache();
  //! Access transient cache values (const, read-only Get/Has/CacheKinds).
  //! This is the only public cache interface.
  [[nodiscard]] Standard_EXPORT const CacheView& Cache() const;
  //! Access reference entries and their UIDs.
  [[nodiscard]] Standard_EXPORT const RefsView& Refs() const;
  //! Access cached and fresh shape reconstruction.
  [[nodiscard]] Standard_EXPORT const ShapesView& Shapes() const;
  //! Access programmatic graph construction and mutation.
  [[nodiscard]] Standard_EXPORT EditorView& Editor();
  //! Const access to editor-specific state queries.
  //! Exposes IsDeferredMode() and ValidateMutationBoundary() on a const graph.
  //! All structural mutation methods require the non-const Editor() overload.
  [[nodiscard]] Standard_EXPORT const EditorView& Editor() const;
  //! Access mesh data with cache-first, persistent-fallback priority.
  //! For mesh cache writes and rep creation, use BRepGraph_Tool::Mesh.
  [[nodiscard]] Standard_EXPORT const MeshView& Mesh() const;

  //! Access history subsystem directly.
  //! History is returned directly rather than through a lightweight view
  //! because it is already a self-contained query and recording subsystem
  //! with no per-view cached state.
  //! @return history subsystem for tracking modifications
  [[nodiscard]] Standard_EXPORT BRepGraph_History& History();
  //! Access history subsystem directly (const).
  //! @return history subsystem for tracking modifications
  [[nodiscard]] Standard_EXPORT const BRepGraph_History& History() const;

  //! Access registered graph layers.
  //! @return layer registry for managing attribute layers
  [[nodiscard]] Standard_EXPORT BRepGraph_LayerRegistry& LayerRegistry();
  //! Access registered graph layers (const).
  //! @return layer registry for managing attribute layers
  [[nodiscard]] Standard_EXPORT const BRepGraph_LayerRegistry& LayerRegistry() const;

private:
  friend class BRepGraph_Builder;
  friend class BRepGraph_Compact;
  friend class BRepGraph_Copy;
  friend class BRepGraph_Tool;
  friend class BRepGraph_Transform;
  template <typename>
  friend class BRepGraph_MutGuard;

  //! @{

  //! Access the underlying storage.
  [[nodiscard]] Standard_EXPORT BRepGraphInc_Storage&       incStorage();
  [[nodiscard]] Standard_EXPORT const BRepGraphInc_Storage& incStorage() const;

  //! Access the graph data structure.
  [[nodiscard]] Standard_EXPORT BRepGraph_Data*       data();
  [[nodiscard]] Standard_EXPORT const BRepGraph_Data* data() const;

  //! Access the layer registry.
  [[nodiscard]] Standard_EXPORT BRepGraph_LayerRegistry&       layerRegistry();
  [[nodiscard]] Standard_EXPORT const BRepGraph_LayerRegistry& layerRegistry() const;

  //! Access the raw transient cache for friend algorithms and builders.
  [[nodiscard]] Standard_EXPORT BRepGraph_TransientCache&       transientCache();
  [[nodiscard]] Standard_EXPORT const BRepGraph_TransientCache& transientCache() const;

  //! Access the raw reference transient cache.
  [[nodiscard]] Standard_EXPORT BRepGraph_RefTransientCache&       refTransientCache();
  [[nodiscard]] Standard_EXPORT const BRepGraph_RefTransientCache& refTransientCache() const;

  //! Access the mesh cache storage.
  [[nodiscard]] Standard_EXPORT BRepGraph_MeshCacheStorage&       meshCache();
  [[nodiscard]] Standard_EXPORT const BRepGraph_MeshCacheStorage& meshCache() const;

  //! Generic reference lookup by RefId (const).
  //! Returns nullptr if the RefId is invalid or out of range.
  Standard_EXPORT const BRepGraphInc::BaseRef* refEntity(const BRepGraph_RefId theId) const;

  //! @}

  Standard_EXPORT void             invalidateSubgraphImpl(const BRepGraph_NodeId theNode);
  Standard_EXPORT BRepGraph_UID    allocateUID(const BRepGraph_NodeId theNodeId);
  Standard_EXPORT BRepGraph_RefUID allocateRefUID(const BRepGraph_RefId theRefId);

  Standard_EXPORT void markModified(const BRepGraph_NodeId theNodeId) noexcept;
  Standard_EXPORT void markRefModified(const BRepGraph_RefId theRefId) noexcept;

  //! Optimized overload: skips changeTopoEntity() dispatch
  //! when the caller already holds a mutable reference to the target entity.
  Standard_EXPORT void markModified(const BRepGraph_NodeId theNodeId,
                                    BRepGraphInc::BaseDef& theEntity) noexcept;
  Standard_EXPORT void markRefModified(const BRepGraph_RefId  theRefId,
                                       BRepGraphInc::BaseRef& theRef) noexcept;

  //! Increment SubtreeGen on a parent node (NOT OwnGen - parent's own data didn't change).
  //! Uses wave guard to prevent exponential blowup on diamond topologies.
  //! Mutex-free: no shape cache clear, no dispatch.
  Standard_EXPORT void markParentSubtreeGen(const BRepGraph_NodeId theParentId) noexcept;

  //! Propagate SubtreeGen upward through reverse indices via markParentSubtreeGen().
  Standard_EXPORT void propagateSubtreeGen(const BRepGraph_NodeId theNodeId) noexcept;

  //! Increment OwnGen on a representation and propagate mutation
  //! to the owning topology node(s).
  Standard_EXPORT void markRepModified(const BRepGraph_RepId theRepId) noexcept;

  //! Generic topology definition lookup by NodeId (const).
  Standard_EXPORT const BRepGraphInc::BaseDef* topoEntity(const BRepGraph_NodeId theId) const;

  //! Generic mutable topology definition lookup by NodeId.
  Standard_EXPORT BRepGraphInc::BaseDef* changeTopoEntity(const BRepGraph_NodeId theId);

  //! Initialize cached view objects to point to this graph.
  Standard_EXPORT void initViews();

  // Fields at the bottom (OCCT style)
  std::unique_ptr<BRepGraph_Data> myData;

  //! Registered layers are stored on BRepGraph, not BRepGraph_Data, to survive Compact swap.
  BRepGraph_LayerRegistry     myLayerRegistry;
  BRepGraph_TransientCache    myTransientCache; //!< Transient algorithm caches (BndBox, UVBounds)
  BRepGraph_RefTransientCache myRefTransientCache; //!< Transient per-reference caches
};

// Included after BRepGraph is complete so the template body sees markModified().
#include <BRepGraph_MutGuard.hxx>

#endif // _BRepGraph_HeaderFile
