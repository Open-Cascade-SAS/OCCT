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

  //! Enable or disable UID generation for newly created nodes.
  Standard_EXPORT void SetUIDEnabled(bool theVal);

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

  // -- Flat methods (accessible through views, friend classes, and nested views) --

  Standard_EXPORT int NbSolidDefs() const;
  Standard_EXPORT int NbShellDefs() const;
  Standard_EXPORT int NbFaceDefs() const;
  Standard_EXPORT int NbWireDefs() const;
  Standard_EXPORT int NbEdgeDefs() const;
  Standard_EXPORT int NbVertexDefs() const;
  Standard_EXPORT int NbCompoundDefs() const;
  Standard_EXPORT int NbCompSolidDefs() const;

  Standard_EXPORT int NbSurfaces() const;
  Standard_EXPORT int NbCurves() const;
  Standard_EXPORT int NbPCurves() const;

  Standard_EXPORT int NbSolidUsages() const;
  Standard_EXPORT int NbShellUsages() const;
  Standard_EXPORT int NbFaceUsages() const;
  Standard_EXPORT int NbWireUsages() const;
  Standard_EXPORT int NbEdgeUsages() const;
  Standard_EXPORT int NbVertexUsages() const;
  Standard_EXPORT int NbCompoundUsages() const;
  Standard_EXPORT int NbCompSolidUsages() const;

  Standard_EXPORT const BRepGraph_TopoNode::SolidDef& SolidDefinition(int theIdx) const;
  Standard_EXPORT const BRepGraph_TopoNode::ShellDef& ShellDefinition(int theIdx) const;
  Standard_EXPORT const BRepGraph_TopoNode::FaceDef& FaceDefinition(int theIdx) const;
  Standard_EXPORT const BRepGraph_TopoNode::WireDef& WireDefinition(int theIdx) const;
  Standard_EXPORT const BRepGraph_TopoNode::EdgeDef& EdgeDefinition(int theIdx) const;
  Standard_EXPORT const BRepGraph_TopoNode::VertexDef& VertexDefinition(int theIdx) const;
  Standard_EXPORT const BRepGraph_TopoNode::CompoundDef& CompoundDefinition(int theIdx) const;
  Standard_EXPORT const BRepGraph_TopoNode::CompSolidDef& CompSolidDefinition(int theIdx) const;

  Standard_EXPORT const BRepGraph_GeomNode::Surf& SurfNode(int theIdx) const;
  Standard_EXPORT const BRepGraph_GeomNode::Curve& CurveNode(int theIdx) const;
  Standard_EXPORT const BRepGraph_GeomNode::PCurve& PCurveNode(int theIdx) const;

  Standard_EXPORT const BRepGraph_TopoNode::SolidUsage& SolidUsageNode(int theIdx) const;
  Standard_EXPORT const BRepGraph_TopoNode::ShellUsage& ShellUsageNode(int theIdx) const;
  Standard_EXPORT const BRepGraph_TopoNode::FaceUsage& FaceUsageNode(int theIdx) const;
  Standard_EXPORT const BRepGraph_TopoNode::WireUsage& WireUsageNode(int theIdx) const;
  Standard_EXPORT const BRepGraph_TopoNode::EdgeUsage& EdgeUsageNode(int theIdx) const;
  Standard_EXPORT const BRepGraph_TopoNode::VertexUsage& VertexUsageNode(int theIdx) const;
  Standard_EXPORT const BRepGraph_TopoNode::CompoundUsage& CompoundUsageNode(int theIdx) const;
  Standard_EXPORT const BRepGraph_TopoNode::CompSolidUsage& CompSolidUsageNode(int theIdx) const;

  Standard_EXPORT const BRepGraph_TopoNode::BaseDef* TopoDef(BRepGraph_NodeId theId) const;
  Standard_EXPORT size_t NbNodes() const;

  Standard_EXPORT bool IsUIDEnabled() const;
  Standard_EXPORT BRepGraph_UID UIDOf(BRepGraph_NodeId theNode) const;
  Standard_EXPORT BRepGraph_NodeId NodeIdFromUID(const BRepGraph_UID& theUID) const;
  Standard_EXPORT bool HasUID(const BRepGraph_UID& theUID) const;
  Standard_EXPORT uint32_t Generation() const;

  Standard_EXPORT int AddRelEdge(BRepGraph_NodeId  theFrom,
                                 BRepGraph_NodeId  theTo,
                                 BRepGraph_RelKind theKind);
  Standard_EXPORT void RemoveRelEdges(BRepGraph_NodeId  theFrom,
                                      BRepGraph_NodeId  theTo,
                                      BRepGraph_RelKind theKind);
  Standard_EXPORT int NbRelEdgesFrom(BRepGraph_NodeId theNode) const;
  Standard_EXPORT int NbRelEdgesTo(BRepGraph_NodeId theNode) const;
  Standard_EXPORT const NCollection_Vector<BRepGraph_RelEdge>* OutRelEdgesOf(
    BRepGraph_NodeId theNode) const;
  Standard_EXPORT const NCollection_Vector<BRepGraph_RelEdge>* InRelEdgesOf(
    BRepGraph_NodeId theNode) const;

  template <typename Func>
  void ForEachOutEdgeOfKind(BRepGraph_NodeId  theNodeId,
                            BRepGraph_RelKind theKind,
                            const Func&       theCallback) const
  {
    const NCollection_Vector<BRepGraph_RelEdge>* aEdges = OutRelEdgesOf(theNodeId);
    if (aEdges == nullptr)
      return;
    for (int anIdx = 0; anIdx < aEdges->Length(); ++anIdx)
    {
      const BRepGraph_RelEdge& anEdge = aEdges->Value(anIdx);
      if (anEdge.Kind == theKind)
        theCallback(anEdge);
    }
  }

  template <typename Func>
  void ForEachInEdgeOfKind(BRepGraph_NodeId  theNodeId,
                           BRepGraph_RelKind theKind,
                           const Func&       theCallback) const
  {
    const NCollection_Vector<BRepGraph_RelEdge>* aEdges = InRelEdgesOf(theNodeId);
    if (aEdges == nullptr)
      return;
    for (int anIdx = 0; anIdx < aEdges->Length(); ++anIdx)
    {
      const BRepGraph_RelEdge& anEdge = aEdges->Value(anIdx);
      if (anEdge.Kind == theKind)
        theCallback(anEdge);
    }
  }

  Standard_EXPORT BRepGraph_NodeId SurfaceOf(BRepGraph_NodeId theFaceDef) const;
  Standard_EXPORT const NCollection_Vector<BRepGraph_NodeId>& FacesOnSurface(
    BRepGraph_NodeId theSurf) const;
  Standard_EXPORT BRepGraph_NodeId CurveOf(BRepGraph_NodeId theEdgeDef) const;
  Standard_EXPORT const NCollection_Vector<BRepGraph_NodeId>& EdgesOnCurve(
    BRepGraph_NodeId theCurve) const;
  Standard_EXPORT BRepGraph_NodeId PCurveOf(BRepGraph_NodeId theEdgeDef,
                                            BRepGraph_NodeId theFaceDef) const;
  Standard_EXPORT BRepGraph_NodeId PCurveOf(BRepGraph_NodeId   theEdgeDef,
                                            BRepGraph_NodeId   theFaceDef,
                                            TopAbs_Orientation theEdgeOrientation) const;

  Standard_EXPORT gp_Trsf GlobalTransform(BRepGraph_UsageId theUsage) const;
  Standard_EXPORT gp_Trsf GlobalTransform(BRepGraph_NodeId theDefId) const;

  Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> SameDomainFaces(
    BRepGraph_NodeId theFaceDef) const;
  Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> FacesOfEdge(
    BRepGraph_NodeId theEdgeDef) const;
  Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> SharedEdges(
    BRepGraph_NodeId theFaceA,
    BRepGraph_NodeId theFaceB) const;
  Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> AdjacentFaces(
    BRepGraph_NodeId theFaceDef) const;

  Standard_EXPORT Bnd_Box BoundingBox(BRepGraph_NodeId theNode) const;
  Standard_EXPORT gp_Pnt Centroid(BRepGraph_NodeId theNode) const;
  Standard_EXPORT void Invalidate(BRepGraph_NodeId theNode);
  Standard_EXPORT void InvalidateSubgraph(BRepGraph_NodeId theNode);

  Standard_EXPORT void SetUserAttribute(BRepGraph_NodeId             theNode,
                                        int                          theKey,
                                        const BRepGraph_UserAttrPtr& theAttr);
  Standard_EXPORT BRepGraph_UserAttrPtr GetUserAttribute(BRepGraph_NodeId theNode,
                                                         int              theKey) const;
  Standard_EXPORT bool RemoveUserAttribute(BRepGraph_NodeId theNode, int theKey);
  Standard_EXPORT void InvalidateUserAttribute(BRepGraph_NodeId theNode, int theKey);

  Standard_EXPORT int NbHistoryRecords() const;
  Standard_EXPORT const BRepGraph_HistoryRecord& HistoryRecord(int theIdx) const;
  Standard_EXPORT BRepGraph_NodeId FindOriginal(BRepGraph_NodeId theModified) const;
  Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> FindDerived(
    BRepGraph_NodeId theOriginal) const;

  Standard_EXPORT TopoDS_Shape Shape(BRepGraph_NodeId theNode) const;
  Standard_EXPORT bool HasOriginalShape(BRepGraph_NodeId theNode) const;
  Standard_EXPORT const TopoDS_Shape& OriginalOf(BRepGraph_NodeId theNode) const;
  Standard_EXPORT TopoDS_Shape ReconstructShape(BRepGraph_NodeId theRoot) const;
  Standard_EXPORT TopoDS_Shape ReconstructFace(int theFaceDefIdx) const;
  Standard_EXPORT TopoDS_Shape ReconstructFromUsage(BRepGraph_UsageId theRoot) const;

  Standard_EXPORT BRepGraph_TopoNode::EdgeDef& MutableEdgeDefinition(int theIdx);
  Standard_EXPORT BRepGraph_TopoNode::WireDef& MutableWireDefinition(int theIdx);
  Standard_EXPORT BRepGraph_TopoNode::VertexDef& MutableVertexDefinition(int theIdx);
  Standard_EXPORT BRepGraph_TopoNode::FaceDef& MutableFaceDefinition(int theIdx);
  Standard_EXPORT BRepGraph_TopoNode::ShellDef& MutableShellDefinition(int theIdx);
  Standard_EXPORT BRepGraph_TopoNode::SolidDef& MutableSolidDefinition(int theIdx);
  Standard_EXPORT BRepGraph_TopoNode::CompoundDef& MutableCompoundDefinition(int theIdx);
  Standard_EXPORT BRepGraph_TopoNode::CompSolidDef& MutableCompSolidDefinition(int theIdx);

  Standard_EXPORT BRepGraph_NodeId AddPCurveToEdge(
    BRepGraph_NodeId            theEdgeDef,
    BRepGraph_NodeId            theFaceDef,
    const Handle(Geom2d_Curve)& theCurve2d,
    double                      theFirst,
    double                      theLast,
    TopAbs_Orientation          theEdgeOrientation = TopAbs_FORWARD);
  Standard_EXPORT void ReplaceEdgeInWire(int              theWireDefIdx,
                                         BRepGraph_NodeId theOldEdgeDef,
                                         BRepGraph_NodeId theNewEdgeDef,
                                         bool             theReversed);
  Standard_EXPORT void SplitEdge(BRepGraph_NodeId  theEdgeDef,
                                  BRepGraph_NodeId  theSplitVertex,
                                  double            theSplitParam,
                                  BRepGraph_NodeId& theSubA,
                                  BRepGraph_NodeId& theSubB);

  Standard_EXPORT int FaceCountForEdge(int theEdgeDefIdx) const;
  Standard_EXPORT const NCollection_Vector<int>& WiresOfEdge(int theEdgeDefIdx) const;

  Standard_EXPORT void RecordHistory(const TCollection_AsciiString&                theOpLabel,
                                     BRepGraph_NodeId                              theOriginal,
                                     const NCollection_Vector<BRepGraph_NodeId>& theReplacements);

  Standard_EXPORT BRepGraph_NodeId AddVertexDef(const gp_Pnt& thePoint,
                                                double        theTolerance);
  Standard_EXPORT BRepGraph_NodeId AddEdgeDef(BRepGraph_NodeId          theStartVtx,
                                              BRepGraph_NodeId          theEndVtx,
                                              const Handle(Geom_Curve)& theCurve,
                                              double                    theFirst,
                                              double                    theLast,
                                              double                    theTolerance);
  Standard_EXPORT BRepGraph_NodeId AddWireDef(
    const NCollection_Vector<BRepGraph_TopoNode::WireDef::EdgeEntry>& theEdges);
  Standard_EXPORT BRepGraph_NodeId AddFaceDef(const Handle(Geom_Surface)&              theSurface,
                                              BRepGraph_NodeId                         theOuterWire,
                                              const NCollection_Vector<BRepGraph_NodeId>& theInnerWires,
                                              double                                   theTolerance);
  Standard_EXPORT BRepGraph_NodeId AddShellDef();
  Standard_EXPORT BRepGraph_NodeId AddSolidDef();
  Standard_EXPORT BRepGraph_UsageId AddFaceToShell(BRepGraph_NodeId   theShellDef,
                                                   BRepGraph_NodeId   theFaceDef,
                                                   TopAbs_Orientation theOri = TopAbs_FORWARD);
  Standard_EXPORT BRepGraph_UsageId AddShellToSolid(BRepGraph_NodeId   theSolidDef,
                                                    BRepGraph_NodeId   theShellDef,
                                                    TopAbs_Orientation theOri = TopAbs_FORWARD);
  Standard_EXPORT BRepGraph_NodeId AddCompoundDef(
    const NCollection_Vector<BRepGraph_NodeId>& theChildDefs);
  Standard_EXPORT BRepGraph_NodeId AddCompSolidDef(
    const NCollection_Vector<BRepGraph_NodeId>& theSolidDefs);
  Standard_EXPORT void AppendShape(const TopoDS_Shape& theShape,
                                   bool                theParallel = false);
  Standard_EXPORT void RemoveNode(BRepGraph_NodeId theNode);
  Standard_EXPORT void RemoveSubgraph(BRepGraph_NodeId theNode);
  Standard_EXPORT bool IsRemoved(BRepGraph_NodeId theNode) const;

  // -- Internal build and utility helpers --

  std::unique_ptr<BRepGraph_Data> myData;

  BRepGraph_NodeId registerSurface(const Handle(Geom_Surface)&       theSurf,
                                   const Handle(Poly_Triangulation)& theTri);
  BRepGraph_NodeId registerCurve(const Handle(Geom_Curve)& theCrv);
  BRepGraph_NodeId createPCurveNode(const Handle(Geom2d_Curve)& theCrv2d,
                                    BRepGraph_NodeId            theEdgeDef,
                                    BRepGraph_NodeId            theFaceDef,
                                    double                      theFirst,
                                    double                      theLast);

  void collectVertexPoints(BRepGraph_NodeId theNode, Bnd_Box& theBox) const;
  void invalidateSubgraphImpl(BRepGraph_NodeId theNode);
  BRepGraph_UID allocateUID(BRepGraph_NodeId theNodeId);
  BRepGraph_NodeCache* mutableCache(BRepGraph_NodeId theNode);
  void markModified(BRepGraph_NodeId theDefId);

  //! Dispatch a callback on the def vector matching theNode.Kind.
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
