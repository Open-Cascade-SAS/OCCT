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

struct BRepGraph_Data;
class NCollection_BaseAllocator;
class TCollection_AsciiString;

class BRepGraph_Builder;
class BRepGraph_History;
class BRepGraph_Analyze;
class BRepGraph_Mutator;

//! @brief Topology-geometry graph over TopoDS / BRep.
//!
//! Definition types are aliases to incidence entity types.
//! Per-occurrence data (orientation, location) lives on incidence refs.
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
  Standard_EXPORT explicit BRepGraph(const Handle(NCollection_BaseAllocator)& theAlloc);
  Standard_EXPORT ~BRepGraph();
  Standard_EXPORT BRepGraph(BRepGraph&&) noexcept;
  Standard_EXPORT BRepGraph& operator=(BRepGraph&&) noexcept;

  //! Build the full graph from a TopoDS_Shape.
  Standard_EXPORT void Build(const TopoDS_Shape& theShape, bool theParallel = false);

  //! Build the full graph with explicit post-pass control.
  Standard_EXPORT void Build(const TopoDS_Shape& theShape,
                             bool                                     theParallel,
                             const BRepGraphInc_Populate::Options&    theOptions);

  //! Return true if the graph was successfully built.
  Standard_EXPORT bool IsDone() const;

  //! Replace the internal allocator and re-create all storage.
  Standard_EXPORT void SetAllocator(const Handle(NCollection_BaseAllocator)& theAlloc);

  //! Return the current allocator.
  Standard_EXPORT const Handle(NCollection_BaseAllocator)& Allocator() const;

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
  Standard_EXPORT void SetHistoryEnabled(bool theVal);

  //! Check if history recording is enabled.
  Standard_EXPORT bool IsHistoryEnabled() const;

  //! Apply a modification operation and record history.
  Standard_EXPORT void ApplyModification(
    BRepGraph_NodeId                                                                    theTarget,
    std::function<NCollection_Vector<BRepGraph_NodeId>(BRepGraph&, BRepGraph_NodeId)> theModifier,
    const TCollection_AsciiString&                                                      theOpLabel);

public:
  //! Shared cache for edge/vertex shapes during multi-face reconstruction.
  using ReconstructCache = NCollection_DataMap<BRepGraph_NodeId, TopoDS_Shape>;

  // -- Grouped View API --
  class DefsView;
  class UIDsView;
  class RelEdgesView;
  class SpatialView;
  class CacheView;
  class AttrsView;
  class ShapesView;
  class MutView;
  class BuilderView;
  class AnalyzeView;

  //! Access topology definitions.
  DefsView     Defs()     const;
  //! Access unique identifiers.
  UIDsView     UIDs()     const;
  //! Access relation edges.
  RelEdgesView RelEdges() const;
  //! Access spatial and adjacency queries.
  SpatialView  Spatial()  const;
  //! Access cached spatial properties.
  CacheView    Cache()    const;
  //! Access user attributes.
  AttrsView    Attrs();
  //! Access shape reconstruction.
  ShapesView   Shapes()   const;
  //! Access mutable definitions and mutation operations.
  MutView      Mut();
  //! Access programmatic graph construction.
  BuilderView  Builder();
  //! Access analysis queries.
  AnalyzeView  Analyze()  const;

  //! Access history subsystem directly.
  Standard_EXPORT BRepGraph_History&       History();
  Standard_EXPORT const BRepGraph_History& History() const;

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

  Standard_EXPORT int NbHistoryRecords() const;
  Standard_EXPORT const BRepGraph_HistoryRecord& HistoryRecord(int theIdx) const;
  Standard_EXPORT BRepGraph_NodeId FindOriginal(BRepGraph_NodeId theModified) const;
  Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> FindDerived(
    BRepGraph_NodeId theOriginal) const;

  Standard_EXPORT void RecordHistory(const TCollection_AsciiString&                theOpLabel,
                                     BRepGraph_NodeId                              theOriginal,
                                     const NCollection_Vector<BRepGraph_NodeId>& theReplacements);

  std::unique_ptr<BRepGraph_Data> myData;

  Standard_EXPORT void invalidateSubgraphImpl(BRepGraph_NodeId theNode);
  Standard_EXPORT BRepGraph_UID allocateUID(BRepGraph_NodeId theNodeId);

  Standard_EXPORT BRepGraph_NodeCache* mutableCache(BRepGraph_NodeId theNode);
  Standard_EXPORT void markModified(BRepGraph_NodeId theDefId);

  //! Generic topology definition lookup by NodeId.
  Standard_EXPORT const BRepGraph_TopoNode::BaseDef* TopoDef(BRepGraph_NodeId theId) const;

  template <typename Func>
  auto dispatchDef(BRepGraph_NodeId theNode, Func&& theFunc) const
    -> decltype(theFunc(std::declval<const NCollection_Vector<BRepGraph_TopoNode::SolidDef>&>(), 0));

  template <typename Func>
  auto dispatchDef(BRepGraph_NodeId theNode, Func&& theFunc)
    -> decltype(theFunc(std::declval<NCollection_Vector<BRepGraph_TopoNode::SolidDef>&>(), 0));
};

#endif // _BRepGraph_HeaderFile
