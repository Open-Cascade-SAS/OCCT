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
#include <BRepGraphInc_Entity.hxx>
#include <BRepGraph_HistoryRecord.hxx>
#include <BRepGraph_SubGraph.hxx>
#include <BRepGraph_UserAttribute.hxx>
#include <BRepGraphInc_Populate.hxx>

#include <Standard_DefineAlloc.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Trsf.hxx>
#include <Bnd_Box.hxx>
#include <gp_Pnt.hxx>

#include <NCollection_Vector.hxx>
#include <NCollection_DataMap.hxx>

#include <memory>

template <typename EntityT>
class BRepGraph_MutRef;
template <typename RefT>
class BRepGraph_MutRefEntry;

struct BRepGraph_Data;
class BRepGraph_Layer;
class NCollection_BaseAllocator;
class TCollection_AsciiString;

class BRepGraph_Builder;
class BRepGraph_History;
class BRepGraph_Analyze;
class BRepGraph_Mutator;

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
//!   seam edges use paired CoEdges with opposite Sense (Parasolid convention).
//! - **Lifecycle**: Build() populates from TopoDS_Shape; Builder().Mut*() guards
//!   provide RAII-scoped mutation with automatic cache invalidation and upward
//!   propagation.
//!
//! Per-occurrence data (orientation, location) lives on incidence refs.
//! Definition types are aliases to BRepGraphInc entity structs.
//!
//! ## Grouped View API
//! Related methods are grouped behind lightweight view objects.
//! Include the corresponding header (e.g. BRepGraph_TopoView.hxx) to use.
//!
//! ## Thread safety
//! All const query methods are thread-safe.
//! Build() is internally parallel when requested.
class BRepGraph
{
public:
  DEFINE_STANDARD_ALLOC

  BRepGraph(const BRepGraph&)            = delete;
  BRepGraph& operator=(const BRepGraph&) = delete;

  Standard_EXPORT BRepGraph();
  Standard_EXPORT explicit BRepGraph(const occ::handle<NCollection_BaseAllocator>& theAlloc);
  Standard_EXPORT ~BRepGraph();
  Standard_EXPORT            BRepGraph(BRepGraph&&) noexcept;
  Standard_EXPORT BRepGraph& operator=(BRepGraph&&) noexcept;

  //! Build the full graph from a TopoDS_Shape.
  Standard_EXPORT void Build(const TopoDS_Shape& theShape, const bool theParallel = false);

  //! Build the full graph with explicit post-pass control.
  Standard_EXPORT void Build(const TopoDS_Shape&                   theShape,
                             const bool                            theParallel,
                             const BRepGraphInc_Populate::Options& theOptions);

  //! Return true if the graph was successfully built.
  [[nodiscard]] Standard_EXPORT bool IsDone() const;

  //! Replace the internal allocator and re-create all storage.
  Standard_EXPORT void SetAllocator(const occ::handle<NCollection_BaseAllocator>& theAlloc);

  //! Return the current allocator.
  [[nodiscard]] Standard_EXPORT const occ::handle<NCollection_BaseAllocator>& Allocator() const;

public:
  //! Shared cache for edge/vertex shapes during multi-face reconstruction.
  using ReconstructCache = NCollection_DataMap<BRepGraph_NodeId, TopoDS_Shape>;

  //! @name Grouped View API
  class TopoView;
  class UIDsView;
  class AttrsView;
  class RefsView;
  class ShapesView;
  class BuilderView;
  class PathView;

  //! Access topology definitions and spatial adjacency queries.
  [[nodiscard]] Standard_EXPORT const TopoView& Topo() const;
  //! Access unique identifiers.
  [[nodiscard]] Standard_EXPORT const UIDsView& UIDs() const;
  //! Access topology path resolution queries.
  [[nodiscard]] Standard_EXPORT const PathView& Paths() const;
  //! Access user attributes.
  [[nodiscard]] Standard_EXPORT AttrsView& Attrs();
  //! Access reference entries and their UIDs.
  [[nodiscard]] Standard_EXPORT const RefsView& Refs() const;
  //! Access shape reconstruction.
  [[nodiscard]] Standard_EXPORT const ShapesView& Shapes() const;
  //! Access programmatic graph construction.
  [[nodiscard]] Standard_EXPORT BuilderView& Builder();

  //! Access history subsystem directly.
  [[nodiscard]] Standard_EXPORT BRepGraph_History&       History();
  [[nodiscard]] Standard_EXPORT const BRepGraph_History& History() const;

  //! Register a named layer. Replaces existing layer with same name.
  Standard_EXPORT void RegisterLayer(const occ::handle<BRepGraph_Layer>& theLayer);

  //! Find a layer by name. Returns null handle if not found.
  [[nodiscard]] Standard_EXPORT occ::handle<BRepGraph_Layer> FindLayer(
    const TCollection_AsciiString& theName) const;

  //! Remove a layer by name.
  Standard_EXPORT void UnregisterLayer(const TCollection_AsciiString& theName);

private:
  friend class BRepGraph_Builder;
  friend class BRepGraph_Analyze;
  friend class BRepGraph_Mutator;
  friend class BRepGraphAlgo_BndLib;
  friend class BRepGraphAlgo_Compact;
  friend class BRepGraphAlgo_Copy;
  friend class BRepGraphAlgo_Transform;
  friend class BRepGraphAlgo_UVBounds;
  template <typename>
  friend class BRepGraph_MutRef;
  template <typename>
  friend class BRepGraph_MutRefEntry;

  Standard_EXPORT int                            NbHistoryRecords() const;
  Standard_EXPORT const BRepGraph_HistoryRecord& HistoryRecord(const int theRecordIdx) const;
  Standard_EXPORT BRepGraph_NodeId FindOriginal(const BRepGraph_NodeId theModified) const;
  Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> FindDerived(
    const BRepGraph_NodeId theOriginal) const;

  Standard_EXPORT void RecordHistory(const TCollection_AsciiString&              theOpLabel,
                                     const BRepGraph_NodeId                      theOriginal,
                                     const NCollection_Vector<BRepGraph_NodeId>& theReplacements);

  std::unique_ptr<BRepGraph_Data> myData;

  //! Named layers (stored on BRepGraph, not BRepGraph_Data, to survive Compact swap).
  NCollection_DataMap<TCollection_AsciiString, occ::handle<BRepGraph_Layer>> myLayers;

  //! True if any registered layer has SubscribedKinds() != 0.
  //! Enables zero-cost skip of modification dispatch when no layer subscribes.
  bool myHasModificationSubscribers = false;

  //! Dispatch OnNodeRemoved to all registered layers.
  void dispatchLayerOnNodeRemoved(const BRepGraph_NodeId theNode,
                                  const BRepGraph_NodeId theReplacement);

  //! Rescan myLayers and update myHasModificationSubscribers flag.
  void updateModificationSubscriberFlag();

  //! Dispatch OnNodeModified to layers whose SubscribedKinds matches the node's kind.
  void dispatchNodeModified(const BRepGraph_NodeId theNode);

  //! Dispatch OnNodesModified to layers whose SubscribedKinds matches at least
  //! one kind in theModifiedKindsMask.
  void dispatchNodesModified(const NCollection_Vector<BRepGraph_NodeId>& theModifiedNodes,
                             const int                                   theModifiedKindsMask);

  //! Initialize cached view objects to point to this graph.
  void initViews();

  Standard_EXPORT void          invalidateSubgraphImpl(const BRepGraph_NodeId theNode);
  Standard_EXPORT BRepGraph_UID allocateUID(const BRepGraph_NodeId theNodeId);
  Standard_EXPORT BRepGraph_RefUID allocateRefUID(const BRepGraph_RefId theRefId);

  Standard_EXPORT BRepGraph_NodeCache* mutableCache(const BRepGraph_NodeId theNode);
  Standard_EXPORT void                 markModified(const BRepGraph_NodeId theNodeId);
  Standard_EXPORT void                 markRefModified(const BRepGraph_RefId theRefId);

  //! Optimized overload: skips ChangeTopoEntity() and mutableCache() dispatch
  //! when the caller already holds a mutable reference to the target entity.
  Standard_EXPORT void markModified(const BRepGraph_NodeId            theNodeId,
                                    BRepGraphInc::BaseEntity& theEntity);
  Standard_EXPORT void markRefModified(const BRepGraph_RefId            theRefId,
                                       BRepGraphInc::BaseRef& theRef);

  //! Mark a parent node as transitively modified (IsModified only, no MutationGen increment).
  //! Skips if already modified. Clears caches, dispatches events, and continues propagation.
  Standard_EXPORT void markParentModified(const BRepGraph_NodeId theParentId);

  //! Propagate IsModified upward through reverse indices without incrementing MutationGen.
  Standard_EXPORT void propagateModified(const BRepGraph_NodeId theNodeId);

  //! Generic topology definition lookup by NodeId (const).
  Standard_EXPORT const BRepGraphInc::BaseEntity* TopoEntity(const BRepGraph_NodeId theId) const;

  //! Generic mutable topology definition lookup by NodeId.
  Standard_EXPORT BRepGraphInc::BaseEntity* ChangeTopoEntity(const BRepGraph_NodeId theId);
};

// Included after BRepGraph is complete so the template body sees markModified().
#include <BRepGraph_MutRef.hxx>
#include "BRepGraph_MutRefEntry.hxx"

#endif // _BRepGraph_HeaderFile
