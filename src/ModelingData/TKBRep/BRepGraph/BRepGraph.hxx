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
#include <BRepGraph_UsageId.hxx>
#include <BRepGraph_UID.hxx>
#include <BRepGraph_TopoNode.hxx>
#include <BRepGraph_GeomNode.hxx>
#include <BRepGraph_RelEdge.hxx>
#include <BRepGraph_HistoryRecord.hxx>
#include <BRepGraph_SubGraph.hxx>
#include <BRepGraph_UserAttribute.hxx>

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
class Geom_Surface;
class Geom_Curve;
class Geom2d_Curve;
class Poly_Triangulation;

class BRepGraph_Builder;
class BRepGraph_History;
class BRepGraph_Analyze;
class BRepGraph_Reconstruct;
class BRepGraph_Mutator;

//! @brief Bidirectional topology-geometry graph over TopoDS / BRep.
//!
//! Two-layer architecture: Definitions (one per unique TShape) hold intrinsic
//! data (geometry links, tolerances).  Usages (one per occurrence) hold
//! context-specific data (orientation, location, parent).
//!
//! ## Grouped View API
//! Related methods are grouped behind lightweight view objects returned by
//! value.  Each view is a zero-cost inner class holding a single pointer.
//! Include the corresponding header (e.g. BRepGraph_DefsView.hxx) to use.
//! Views are the primary public API for querying and mutating the graph.
//!
//! ## Thread safety
//! All const query methods (including lazy cache Gets) are thread-safe.
//! Build() is internally parallel when requested.
//! ApplyModification() requires exclusive access.
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

  //! Return true if the graph was successfully built.
  Standard_EXPORT bool IsDone() const;

  //! Return all usages of a given definition.
  Standard_EXPORT const NCollection_Vector<BRepGraph_UsageId>& UsagesOf(
    BRepGraph_NodeId theDefId) const;

  //! Return the definition NodeId for a given usage.
  Standard_EXPORT BRepGraph_NodeId DefOf(BRepGraph_UsageId theUsageId) const;

  //! Replace the internal allocator and re-create all storage with new allocator.
  Standard_EXPORT void SetAllocator(const Handle(NCollection_BaseAllocator)& theAlloc);

  //! Return the current allocator.
  Standard_EXPORT const Handle(NCollection_BaseAllocator)& Allocator() const;

  //! Enable or disable history recording.
  Standard_EXPORT void SetHistoryEnabled(bool theVal);

  //! Check if history recording is enabled.
  Standard_EXPORT bool IsHistoryEnabled() const;

  //! Apply a modification operation and record history.
  //! @param[in] theTarget node to modify
  //! @param[in] theModifier callback that performs the modification and returns replacements
  //! @param[in] theOpLabel label for the history record
  Standard_EXPORT void ApplyModification(
    BRepGraph_NodeId                                                                    theTarget,
    std::function<NCollection_Vector<BRepGraph_NodeId>(BRepGraph&, BRepGraph_NodeId)> theModifier,
    const TCollection_AsciiString&                                                      theOpLabel);

public:
  //! Shared cache for edge/vertex shapes during multi-face reconstruction.
  using ReconstructCache = NCollection_DataMap<BRepGraph_NodeId, TopoDS_Shape>;

  // -- Grouped View API (inner classes defined in separate headers) --
  class DefsView;
  class UsagesView;
  class GeomView;
  class UIDsView;
  class RelEdgesView;
  class SpatialView;
  class CacheView;
  class AttrsView;
  class ShapesView;
  class MutView;
  class BuilderView;
  class AnalyzeView;

  //! Access topology definitions. Include BRepGraph_DefsView.hxx to use.
  DefsView     Defs()     const;
  //! Access topology usages. Include BRepGraph_UsagesView.hxx to use.
  UsagesView   Usages()   const;
  //! Access geometry nodes and links. Include BRepGraph_GeomView.hxx to use.
  GeomView     Geom()     const;
  //! Access unique identifiers. Include BRepGraph_UIDsView.hxx to use.
  UIDsView     UIDs()     const;
  //! Access relation edges. Include BRepGraph_RelEdgesView.hxx to use.
  RelEdgesView RelEdges() const;
  //! Access spatial and adjacency queries. Include BRepGraph_SpatialView.hxx to use.
  SpatialView  Spatial()  const;
  //! Access cached spatial properties. Include BRepGraph_CacheView.hxx to use.
  CacheView    Cache()    const;
  //! Access user attributes. Include BRepGraph_AttrsView.hxx to use.
  AttrsView    Attrs();
  //! Access shape reconstruction. Include BRepGraph_ShapesView.hxx to use.
  ShapesView   Shapes()   const;
  //! Access mutable definitions and mutation operations. Include BRepGraph_MutView.hxx to use.
  MutView      Mut();
  //! Access programmatic graph construction. Include BRepGraph_BuilderView.hxx to use.
  BuilderView  Builder();
  //! Access analysis queries. Include BRepGraph_AnalyzeView.hxx to use.
  AnalyzeView  Analyze()  const;

  //! Access history subsystem directly.
  Standard_EXPORT BRepGraph_History&       History();
  //! Access history subsystem directly (const).
  Standard_EXPORT const BRepGraph_History& History() const;

private:
  friend class BRepGraph_Builder;
  friend class BRepGraph_History;
  friend class BRepGraph_Analyze;
  friend class BRepGraph_Reconstruct;
  friend class BRepGraph_Mutator;
  friend class BRepGraph_BackRefManager;
  friend class BRepGraphAlgo_BndLib;
  friend class BRepGraphAlgo_Compact;
  friend class BRepGraphAlgo_UVBounds;

  // -- History flat methods (used internally by History subsystem) --

  Standard_EXPORT int NbHistoryRecords() const;
  Standard_EXPORT const BRepGraph_HistoryRecord& HistoryRecord(int theIdx) const;
  Standard_EXPORT BRepGraph_NodeId FindOriginal(BRepGraph_NodeId theModified) const;
  Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> FindDerived(
    BRepGraph_NodeId theOriginal) const;

  Standard_EXPORT void RecordHistory(const TCollection_AsciiString&                theOpLabel,
                                     BRepGraph_NodeId                              theOriginal,
                                     const NCollection_Vector<BRepGraph_NodeId>& theReplacements);

  // -- Internal storage --

  std::unique_ptr<BRepGraph_Data> myData;

  // -- Internal build and utility helpers (called by views and friend classes) --

  Standard_EXPORT BRepGraph_NodeId registerSurface(const Handle(Geom_Surface)&       theSurf,
                                                   const Handle(Poly_Triangulation)& theTri,
                                                   const TopLoc_Location&            theLoc = TopLoc_Location());
  Standard_EXPORT BRepGraph_NodeId registerCurve(const Handle(Geom_Curve)&  theCrv,
                                                 const TopLoc_Location&     theLoc = TopLoc_Location());
  Standard_EXPORT BRepGraph_NodeId createPCurveNode(const Handle(Geom2d_Curve)& theCrv2d,
                                                    BRepGraph_NodeId            theEdgeDef,
                                                    BRepGraph_NodeId            theFaceDef,
                                                    double                      theFirst,
                                                    double                      theLast,
                                                    GeomAbs_Shape               theContinuity = GeomAbs_C0);

  Standard_EXPORT void invalidateSubgraphImpl(BRepGraph_NodeId theNode);
  Standard_EXPORT BRepGraph_UID allocateUID(BRepGraph_NodeId theNodeId);

  Standard_EXPORT BRepGraph_NodeCache* mutableCache(BRepGraph_NodeId theNode);
  Standard_EXPORT void markModified(BRepGraph_NodeId theDefId);

  //! Generic topology definition lookup by NodeId (internal helper).
  Standard_EXPORT const BRepGraph_TopoNode::BaseDef* TopoDef(BRepGraph_NodeId theId) const;

  //! Dispatch a callback on the def vector matching theNode.NodeKind.
  //! Defined in BRepGraph.cxx (only used internally).
  template <typename Func>
  auto dispatchDef(BRepGraph_NodeId theNode, Func&& theFunc) const
    -> decltype(theFunc(std::declval<const NCollection_Vector<BRepGraph_TopoNode::SolidDef>&>(), 0));

  //! Non-const overload of dispatchDef for mutable access.
  template <typename Func>
  auto dispatchDef(BRepGraph_NodeId theNode, Func&& theFunc)
    -> decltype(theFunc(std::declval<NCollection_Vector<BRepGraph_TopoNode::SolidDef>&>(), 0));
};

#endif // _BRepGraph_HeaderFile
