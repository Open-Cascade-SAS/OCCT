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
#include <BRepGraph_UID.hxx>
#include <BRepGraph_TopoNode.hxx>
#include <BRepGraph_RelEdge.hxx>
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

#include <functional>
#include <memory>
#include <utility>

template <typename DefT>
class BRepGraph_MutRef;

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
//! - **Lifecycle**: Build() populates from TopoDS_Shape; Mut*() guards provide
//!   RAII-scoped mutation with automatic cache invalidation and upward propagation.
//!
//! Per-occurrence data (orientation, location) lives on incidence refs.
//! Definition types are aliases to BRepGraphInc entity structs.
//!
//! ## Grouped View API
//! Related methods are grouped behind lightweight view objects.
//! Include the corresponding header (e.g. BRepGraph_DefsView.hxx) to use.
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
  Standard_EXPORT bool IsDone() const;

  //! Replace the internal allocator and re-create all storage.
  Standard_EXPORT void SetAllocator(const occ::handle<NCollection_BaseAllocator>& theAlloc);

  //! Return the current allocator.
  Standard_EXPORT const occ::handle<NCollection_BaseAllocator>& Allocator() const;

  //! Begin deferred invalidation mode.
  //! While active, markModified() only sets IsModified flags on entities
  //! without acquiring the shape-cache mutex or propagating upward.
  //! Call EndDeferredInvalidation() to batch-flush all accumulated changes.
  //! Intended for parallel mutation loops (SameParameter, Sewing processEdges).
  Standard_EXPORT void BeginDeferredInvalidation();

  //! End deferred invalidation mode and batch-flush:
  //! clears the entire shape cache and propagates IsModified upward
  //! for all modified entities in a single pass.
  Standard_EXPORT void EndDeferredInvalidation();

  //! Enable or disable history recording.
  Standard_EXPORT void SetHistoryEnabled(const bool theVal);

  //! Check if history recording is enabled.
  Standard_EXPORT bool IsHistoryEnabled() const;

  //! Apply a modification operation and record history.
  Standard_EXPORT void ApplyModification(
    const BRepGraph_NodeId                                                            theTarget,
    std::function<NCollection_Vector<BRepGraph_NodeId>(BRepGraph&, BRepGraph_NodeId)> theModifier,
    const TCollection_AsciiString&                                                    theOpLabel);

public:
  //! Shared cache for edge/vertex shapes during multi-face reconstruction.
  using ReconstructCache = NCollection_DataMap<BRepGraph_NodeId, TopoDS_Shape>;

  //! @name Grouped View API
  class DefsView;
  class UIDsView;
  class RelEdgesView;
  class SpatialView;
  class AttrsView;
  class ShapesView;
  class MutView;
  class BuilderView;
  class AnalyzeView;

  //! Access topology definitions.
  DefsView Defs() const;
  //! Access unique identifiers.
  UIDsView UIDs() const;
  //! Access relation edges.
  RelEdgesView RelEdges() const;
  //! Access spatial and adjacency queries.
  SpatialView Spatial() const;
  //! Access user attributes.
  AttrsView Attrs();
  //! Access shape reconstruction.
  ShapesView Shapes() const;
  //! Access mutable definitions and mutation operations.
  MutView Mut();

  //! @name Scoped mutable definition guards (RAII).
  //! Return a BRepGraph_MutRef that defers markModified() to scope exit.
  //! Use when modifying multiple fields on the same entity.

  //! Return scoped mutable edge definition guard.
  //! @param[in] theEdgeIdx zero-based edge definition index
  Standard_EXPORT BRepGraph_MutRef<BRepGraph_TopoNode::EdgeDef> MutEdge(const int theEdgeIdx);

  //! Return scoped mutable vertex definition guard.
  //! @param[in] theVertexIdx zero-based vertex definition index
  Standard_EXPORT BRepGraph_MutRef<BRepGraph_TopoNode::VertexDef> MutVertex(const int theVertexIdx);

  //! Return scoped mutable wire definition guard.
  //! @param[in] theWireIdx zero-based wire definition index
  Standard_EXPORT BRepGraph_MutRef<BRepGraph_TopoNode::WireDef> MutWire(const int theWireIdx);

  //! Return scoped mutable face definition guard.
  //! @param[in] theFaceIdx zero-based face definition index
  Standard_EXPORT BRepGraph_MutRef<BRepGraph_TopoNode::FaceDef> MutFace(const int theFaceIdx);

  //! Return scoped mutable shell definition guard.
  //! @param[in] theShellIdx zero-based shell definition index
  Standard_EXPORT BRepGraph_MutRef<BRepGraph_TopoNode::ShellDef> MutShell(const int theShellIdx);

  //! Return scoped mutable solid definition guard.
  //! @param[in] theSolidIdx zero-based solid definition index
  Standard_EXPORT BRepGraph_MutRef<BRepGraph_TopoNode::SolidDef> MutSolid(const int theSolidIdx);

  //! Return scoped mutable compound definition guard.
  //! @param[in] theCompoundIdx zero-based compound definition index
  Standard_EXPORT BRepGraph_MutRef<BRepGraph_TopoNode::CompoundDef> MutCompound(
    const int theCompoundIdx);

  //! Return scoped mutable coedge definition guard.
  //! @param[in] theCoEdgeIdx zero-based coedge definition index
  Standard_EXPORT BRepGraph_MutRef<BRepGraph_TopoNode::CoEdgeDef> MutCoEdge(const int theCoEdgeIdx);

  //! Create a new Curve2DRep in storage and return its index.
  //! Use this when assigning a new PCurve to an existing CoEdge entity
  //! via MutCoEdge().
  //! @param[in] theCurve2d the 2D parametric curve handle
  //! @return index into Curve2DRep storage, or -1 if the curve is null
  Standard_EXPORT int CreateCurve2DRep(const occ::handle<Geom2d_Curve>& theCurve2d);

  //! Return scoped mutable comp-solid definition guard.
  //! @param[in] theCompSolidIdx zero-based comp-solid definition index
  Standard_EXPORT BRepGraph_MutRef<BRepGraph_TopoNode::CompSolidDef> MutCompSolid(
    const int theCompSolidIdx);

  //! Return scoped mutable product definition guard.
  //! @param[in] theProductIdx zero-based product definition index
  Standard_EXPORT BRepGraph_MutRef<BRepGraph_TopoNode::ProductDef> MutProduct(
    const int theProductIdx);

  //! Return scoped mutable occurrence definition guard.
  //! @param[in] theOccurrenceIdx zero-based occurrence definition index
  Standard_EXPORT BRepGraph_MutRef<BRepGraph_TopoNode::OccurrenceDef> MutOccurrence(
    const int theOccurrenceIdx);

  //! Access programmatic graph construction.
  BuilderView Builder();
  //! Access analysis queries.
  AnalyzeView Analyze() const;

  //! Access history subsystem directly.
  Standard_EXPORT BRepGraph_History&       History();
  Standard_EXPORT const BRepGraph_History& History() const;

  //! Register a named layer. Replaces existing layer with same name.
  Standard_EXPORT void RegisterLayer(const occ::handle<BRepGraph_Layer>& theLayer);

  //! Find a layer by name. Returns null handle if not found.
  Standard_EXPORT occ::handle<BRepGraph_Layer> FindLayer(
    const TCollection_AsciiString& theName) const;

  //! Remove a layer by name.
  Standard_EXPORT void UnregisterLayer(const TCollection_AsciiString& theName);

private:
  friend class BRepGraph_Builder;
  friend class BRepGraph_History;
  friend class BRepGraph_Analyze;
  friend class BRepGraph_Mutator;
  friend class BRepGraph_BackRefManager;
  friend class BRepGraphAlgo_BndLib;
  friend class BRepGraphAlgo_Compact;
  friend class BRepGraphAlgo_Copy;
  friend class BRepGraphAlgo_Transform;
  friend class BRepGraphAlgo_UVBounds;
  friend class BRepGraph_MutationGuard;
  template <typename>
  friend class BRepGraph_MutRef;

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

  Standard_EXPORT void          invalidateSubgraphImpl(const BRepGraph_NodeId theNode);
  Standard_EXPORT BRepGraph_UID allocateUID(const BRepGraph_NodeId theNodeId);

  Standard_EXPORT BRepGraph_NodeCache* mutableCache(const BRepGraph_NodeId theNode);
  Standard_EXPORT void                 markModified(const BRepGraph_NodeId theDefId);

  //! Optimized overload: skips ChangeTopoDef() and mutableCache() dispatch
  //! when the caller already holds a mutable reference to the definition.
  Standard_EXPORT void markModified(const BRepGraph_NodeId       theDefId,
                                    BRepGraph_TopoNode::BaseDef& theDef);

  //! Mark a parent node as transitively modified (IsModified only, no MutationGen increment).
  //! Skips if already modified. Clears caches, dispatches events, and continues propagation.
  Standard_EXPORT void markParentModified(const BRepGraph_NodeId theParentId);

  //! Propagate IsModified upward through reverse indices without incrementing MutationGen.
  Standard_EXPORT void propagateModified(const BRepGraph_NodeId theDefId);

  //! Generic topology definition lookup by NodeId (const).
  Standard_EXPORT const BRepGraph_TopoNode::BaseDef* TopoDef(const BRepGraph_NodeId theId) const;

  //! Generic mutable topology definition lookup by NodeId.
  Standard_EXPORT BRepGraph_TopoNode::BaseDef* ChangeTopoDef(const BRepGraph_NodeId theId);

  template <typename Func>
  auto dispatchDef(const BRepGraph_NodeId theNode, Func&& theFunc) const
    -> decltype(theFunc(std::declval<const NCollection_Vector<BRepGraph_TopoNode::SolidDef>&>(),
                        0));

  template <typename Func>
  auto dispatchDef(const BRepGraph_NodeId theNode, Func&& theFunc)
    -> decltype(theFunc(std::declval<NCollection_Vector<BRepGraph_TopoNode::SolidDef>&>(), 0));
};

// Included after BRepGraph is complete so the template body sees markModified().
#include <BRepGraph_MutRef.hxx>

#endif // _BRepGraph_HeaderFile
