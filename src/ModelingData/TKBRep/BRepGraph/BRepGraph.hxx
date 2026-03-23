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
#include <BRepGraph_RelEdge.hxx>
#include <BRepGraph_UID.hxx>
#include <BRepGraph_AttrRegistry.hxx>
#include <BRepGraph_CachedValue.hxx>
#include <BRepGraph_UserAttribute.hxx>
#include <BRepGraph_NodeCache.hxx>
#include <BRepGraph_TopoNode.hxx>
#include <BRepGraph_GeomNode.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_HistoryRecord.hxx>
#include <BRepGraph_SubGraph.hxx>

#include <Standard_DefineAlloc.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_TShape.hxx>
#include <gp_Trsf.hxx>
#include <Bnd_Box.hxx>
#include <gp_Pnt.hxx>

#include <NCollection_Vector.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_BaseAllocator.hxx>

#include <functional>
#include <atomic>
#include <shared_mutex>

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

  //! Build the full graph from a TopoDS_Shape.
  Standard_EXPORT void Build(const TopoDS_Shape& theShape, bool theParallel = false);

  Standard_EXPORT bool IsDone() const;

  // --- Definition counts ---

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

  // --- Usage counts ---

  Standard_EXPORT int NbSolidUsages() const;
  Standard_EXPORT int NbShellUsages() const;
  Standard_EXPORT int NbFaceUsages() const;
  Standard_EXPORT int NbWireUsages() const;
  Standard_EXPORT int NbEdgeUsages() const;
  Standard_EXPORT int NbVertexUsages() const;
  Standard_EXPORT int NbCompoundUsages() const;
  Standard_EXPORT int NbCompSolidUsages() const;

  // --- Definition access ---

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

  // --- Usage access ---

  Standard_EXPORT const BRepGraph_TopoNode::SolidUsage& SolidUsageNode(int theIdx) const;
  Standard_EXPORT const BRepGraph_TopoNode::ShellUsage& ShellUsageNode(int theIdx) const;
  Standard_EXPORT const BRepGraph_TopoNode::FaceUsage& FaceUsageNode(int theIdx) const;
  Standard_EXPORT const BRepGraph_TopoNode::WireUsage& WireUsageNode(int theIdx) const;
  Standard_EXPORT const BRepGraph_TopoNode::EdgeUsage& EdgeUsageNode(int theIdx) const;
  Standard_EXPORT const BRepGraph_TopoNode::VertexUsage& VertexUsageNode(int theIdx) const;
  Standard_EXPORT const BRepGraph_TopoNode::CompoundUsage& CompoundUsageNode(int theIdx) const;
  Standard_EXPORT const BRepGraph_TopoNode::CompSolidUsage& CompSolidUsageNode(int theIdx) const;

  // --- Navigation between layers ---

  //! All usages of a given definition.
  Standard_EXPORT const NCollection_Vector<BRepGraph_UsageId>& UsagesOf(
    BRepGraph_NodeId theDefId) const;

  //! Definition id for a given usage.
  Standard_EXPORT BRepGraph_NodeId DefOf(BRepGraph_UsageId theUsageId) const;

  //! Generic topology definition lookup by NodeId.
  Standard_EXPORT const BRepGraph_TopoNode::BaseDef* TopoDef(BRepGraph_NodeId theId) const;

  //! Total number of nodes in the graph (all kinds).
  Standard_EXPORT size_t NbNodes() const;

  // --- Opt-in UID system ---

  Standard_EXPORT void SetUIDEnabled(bool theVal);
  Standard_EXPORT bool IsUIDEnabled() const;
  Standard_EXPORT BRepGraph_UID UIDOf(BRepGraph_NodeId theNode) const;
  Standard_EXPORT BRepGraph_NodeId NodeIdFromUID(const BRepGraph_UID& theUID) const;
  Standard_EXPORT bool HasUID(const BRepGraph_UID& theUID) const;
  Standard_EXPORT uint32_t Generation() const;

  // --- RelEdge system ---

  Standard_EXPORT int AddRelEdge(BRepGraph_NodeId  theFrom,
                                 BRepGraph_NodeId  theTo,
                                 BRepGraph_RelKind theKind);

  Standard_EXPORT void RemoveRelEdges(BRepGraph_NodeId  theFrom,
                                      BRepGraph_NodeId  theTo,
                                      BRepGraph_RelKind theKind);

  Standard_EXPORT int NbRelEdgesFrom(BRepGraph_NodeId theNode) const;
  Standard_EXPORT int NbRelEdgesTo(BRepGraph_NodeId theNode) const;

  template <typename Func>
  void ForEachOutEdgeOfKind(BRepGraph_NodeId  theNodeId,
                            BRepGraph_RelKind theKind,
                            const Func&       theCallback) const
  {
    const NCollection_Vector<BRepGraph_RelEdge>* aEdges = myOutRelEdges.Seek(theNodeId);
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
    const NCollection_Vector<BRepGraph_RelEdge>* aEdges = myInRelEdges.Seek(theNodeId);
    if (aEdges == nullptr)
      return;
    for (int anIdx = 0; anIdx < aEdges->Length(); ++anIdx)
    {
      const BRepGraph_RelEdge& anEdge = aEdges->Value(anIdx);
      if (anEdge.Kind == theKind)
        theCallback(anEdge);
    }
  }

  // --- Topology-Geometry queries ---

  Standard_EXPORT BRepGraph_NodeId SurfaceOf(BRepGraph_NodeId theFaceDef) const;
  Standard_EXPORT const NCollection_Vector<BRepGraph_NodeId>& FacesOnSurface(
    BRepGraph_NodeId theSurf) const;
  Standard_EXPORT BRepGraph_NodeId CurveOf(BRepGraph_NodeId theEdgeDef) const;
  Standard_EXPORT const NCollection_Vector<BRepGraph_NodeId>& EdgesOnCurve(
    BRepGraph_NodeId theCurve) const;
  Standard_EXPORT BRepGraph_NodeId PCurveOf(BRepGraph_NodeId theEdgeDef,
                                            BRepGraph_NodeId theFaceDef) const;

  //! Returns the PCurve node id for the given edge/face/orientation triple.
  //! For seam edges two PCurve nodes share the same FaceDefId but differ in orientation;
  //! use this overload to distinguish FORWARD (C1) from REVERSED (C2).
  Standard_EXPORT BRepGraph_NodeId PCurveOf(BRepGraph_NodeId   theEdgeDef,
                                            BRepGraph_NodeId   theFaceDef,
                                            TopAbs_Orientation theEdgeOrientation) const;

  //! Accumulated global transform for a usage.
  Standard_EXPORT gp_Trsf GlobalTransform(BRepGraph_UsageId theUsage) const;

  //! Accumulated global transform for a definition (from first usage).
  Standard_EXPORT gp_Trsf GlobalTransform(BRepGraph_NodeId theDefId) const;

  Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> SameDomainFaces(
    BRepGraph_NodeId theFaceDef) const;

  // --- Edge adjacency queries ---

  //! Return all face definition NodeIds that reference this edge (via their wires).
  //! @param[in] theEdgeDef edge definition NodeId
  //! @return vector of face definition NodeIds
  Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> FacesOfEdge(
    BRepGraph_NodeId theEdgeDef) const;

  //! Return all edges shared between two faces.
  //! @param[in] theFaceA first face definition NodeId
  //! @param[in] theFaceB second face definition NodeId
  //! @return vector of edge definition NodeIds
  Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> SharedEdges(
    BRepGraph_NodeId theFaceA,
    BRepGraph_NodeId theFaceB) const;

  //! Return all faces adjacent to a face (sharing at least one edge).
  //! @param[in] theFaceDef face definition NodeId
  //! @return vector of adjacent face definition NodeIds
  Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> AdjacentFaces(
    BRepGraph_NodeId theFaceDef) const;

  // --- Cache ---

  Standard_EXPORT Bnd_Box BoundingBox(BRepGraph_NodeId theNode) const;
  Standard_EXPORT gp_Pnt Centroid(BRepGraph_NodeId theNode) const;
  Standard_EXPORT void Invalidate(BRepGraph_NodeId theNode);
  Standard_EXPORT void InvalidateSubgraph(BRepGraph_NodeId theNode);

  // --- User attributes ---

  Standard_EXPORT void SetUserAttribute(BRepGraph_NodeId             theNode,
                                        int                          theKey,
                                        const BRepGraph_UserAttrPtr& theAttr);
  Standard_EXPORT BRepGraph_UserAttrPtr GetUserAttribute(BRepGraph_NodeId theNode,
                                                         int              theKey) const;
  Standard_EXPORT bool RemoveUserAttribute(BRepGraph_NodeId theNode, int theKey);
  Standard_EXPORT void InvalidateUserAttribute(BRepGraph_NodeId theNode, int theKey);

  template <typename T>
  T UserAttributeValue(BRepGraph_NodeId          theNode,
                       int                       theKey,
                       const std::function<T()>& theComputer) const
  {
    BRepGraph_UserAttrPtr aBase = GetUserAttribute(theNode, theKey);
    if (!aBase)
      return T{};
    auto aTyped = std::dynamic_pointer_cast<BRepGraph_TypedAttribute<T>>(aBase);
    if (!aTyped)
      return T{};
    return aTyped->Get(theComputer);
  }

  // --- History ---

  Standard_EXPORT int NbHistoryRecords() const;
  Standard_EXPORT const BRepGraph_HistoryRecord& HistoryRecord(int theIdx) const;
  Standard_EXPORT BRepGraph_NodeId FindOriginal(BRepGraph_NodeId theModified) const;
  Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> FindDerived(
    BRepGraph_NodeId theOriginal) const;
  Standard_EXPORT void ApplyModification(
    BRepGraph_NodeId                                                                    theTarget,
    std::function<NCollection_Vector<BRepGraph_NodeId>(BRepGraph&, BRepGraph_NodeId)> theModifier,
    const TCollection_AsciiString&                                                      theOpLabel);

  // --- Shape access ---

  Standard_EXPORT TopoDS_Shape Shape(BRepGraph_NodeId theNode) const;
  Standard_EXPORT bool HasOriginalShape(BRepGraph_NodeId theNode) const;
  Standard_EXPORT const TopoDS_Shape& OriginalOf(BRepGraph_NodeId theNode) const;

  // --- Reconstruction ---

  Standard_EXPORT TopoDS_Shape ReconstructShape(BRepGraph_NodeId theRoot) const;
  Standard_EXPORT TopoDS_Shape ReconstructFace(int theFaceDefIdx) const;
  Standard_EXPORT TopoDS_Shape ReconstructFromUsage(BRepGraph_UsageId theRoot) const;

  // --- Mutation ---

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

  //! Split a single edge definition at a vertex and 3D-curve parameter.
  //! Creates two new EdgeDef slots, splits all PCurve nodes at the corresponding
  //! 2D parameter, and updates every wire that contained the original edge
  //! (inserting the two sub-edges in traversal order).
  //! The original EdgeDef index remains valid but is no longer referenced by any wire.
  //! @param theEdgeDef      edge to split (must not be degenerate)
  //! @param theSplitVertex  vertex definition at the split point (already in graph)
  //! @param theSplitParam   parameter on the 3D curve at the split point
  //! @param[out] theSubA    sub-edge: StartVertex → SplitVertex, [First, SplitParam]
  //! @param[out] theSubB    sub-edge: SplitVertex → EndVertex,   [SplitParam, Last]
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

  // --- Programmatic node addition ---

  //! Add a vertex definition to the graph.
  //! @param[in] thePoint     3D coordinates
  //! @param[in] theTolerance vertex tolerance
  //! @return NodeId of the new vertex definition
  Standard_EXPORT BRepGraph_NodeId AddVertexDef(const gp_Pnt& thePoint,
                                                double        theTolerance);

  //! Add an edge definition to the graph.
  //! @param[in] theStartVtx  start vertex def NodeId
  //! @param[in] theEndVtx    end vertex def NodeId
  //! @param[in] theCurve     3D curve (may be null for degenerate edges)
  //! @param[in] theFirst     first curve parameter
  //! @param[in] theLast      last curve parameter
  //! @param[in] theTolerance edge tolerance
  //! @return NodeId of the new edge definition
  Standard_EXPORT BRepGraph_NodeId AddEdgeDef(BRepGraph_NodeId          theStartVtx,
                                              BRepGraph_NodeId          theEndVtx,
                                              const Handle(Geom_Curve)& theCurve,
                                              double                    theFirst,
                                              double                    theLast,
                                              double                    theTolerance);

  //! Add a wire definition to the graph.
  //! @param[in] theEdges ordered edge entries
  //! @return NodeId of the new wire definition
  Standard_EXPORT BRepGraph_NodeId AddWireDef(
    const NCollection_Vector<BRepGraph_TopoNode::WireDef::EdgeEntry>& theEdges);

  //! Add a face definition to the graph.
  //! @param[in] theSurface    surface geometry
  //! @param[in] theOuterWire  outer wire def NodeId
  //! @param[in] theInnerWires inner wire def NodeIds
  //! @param[in] theTolerance  face tolerance
  //! @return NodeId of the new face definition
  Standard_EXPORT BRepGraph_NodeId AddFaceDef(const Handle(Geom_Surface)&              theSurface,
                                              BRepGraph_NodeId                         theOuterWire,
                                              const NCollection_Vector<BRepGraph_NodeId>& theInnerWires,
                                              double                                   theTolerance);

  //! Add an empty shell definition to the graph.
  //! @return NodeId of the new shell definition
  Standard_EXPORT BRepGraph_NodeId AddShellDef();

  //! Add an empty solid definition to the graph.
  //! @return NodeId of the new solid definition
  Standard_EXPORT BRepGraph_NodeId AddSolidDef();

  //! Link a face to a shell (creates FaceUsage under the ShellUsage).
  //! @param[in] theShellDef  shell definition NodeId
  //! @param[in] theFaceDef   face definition NodeId
  //! @param[in] theOri       orientation of the face in the shell
  //! @return UsageId of the created FaceUsage
  Standard_EXPORT BRepGraph_UsageId AddFaceToShell(BRepGraph_NodeId   theShellDef,
                                                   BRepGraph_NodeId   theFaceDef,
                                                   TopAbs_Orientation theOri = TopAbs_FORWARD);

  //! Link a shell to a solid (creates ShellUsage under the SolidUsage).
  //! @param[in] theSolidDef  solid definition NodeId
  //! @param[in] theShellDef  shell definition NodeId
  //! @param[in] theOri       orientation of the shell in the solid
  //! @return UsageId of the created ShellUsage
  Standard_EXPORT BRepGraph_UsageId AddShellToSolid(BRepGraph_NodeId   theSolidDef,
                                                    BRepGraph_NodeId   theShellDef,
                                                    TopAbs_Orientation theOri = TopAbs_FORWARD);

  //! Add a compound definition with child definitions.
  //! @param[in] theChildDefs child definition NodeIds
  //! @return NodeId of the new compound definition
  Standard_EXPORT BRepGraph_NodeId AddCompoundDef(
    const NCollection_Vector<BRepGraph_NodeId>& theChildDefs);

  //! Add a compsolid definition with child solid definitions.
  //! @param[in] theSolidDefs child solid definition NodeIds
  //! @return NodeId of the new compsolid definition
  Standard_EXPORT BRepGraph_NodeId AddCompSolidDef(
    const NCollection_Vector<BRepGraph_NodeId>& theSolidDefs);

  // --- Incremental build ---

  //! Append a shape to the existing graph without clearing.
  //! Uses existing deduplication maps to avoid re-registering shared entities.
  //! @param[in] theShape   shape to add
  //! @param[in] theParallel if true, per-face geometry extraction is parallel
  Standard_EXPORT void AppendShape(const TopoDS_Shape& theShape,
                                   bool                theParallel = false);

  // --- Soft removal ---

  //! Mark a node as removed (soft deletion).
  //! The node remains in storage but is skipped by iterators and queries.
  //! @param[in] theNode node to remove
  Standard_EXPORT void RemoveNode(BRepGraph_NodeId theNode);

  //! Mark a node and all its descendants as removed (cascading soft deletion).
  //! Walks the definition/usage hierarchy and marks all children as removed.
  //! @param[in] theNode root node to remove
  Standard_EXPORT void RemoveSubgraph(BRepGraph_NodeId theNode);

  //! Check if a node has been soft-removed.
  //! @param[in] theNode node to check
  //! @return true if the node was marked as removed
  Standard_EXPORT bool IsRemoved(BRepGraph_NodeId theNode) const;

  // --- Allocator ---

  Standard_EXPORT void SetAllocator(const Handle(NCollection_BaseAllocator)& theAlloc);
  Standard_EXPORT const Handle(NCollection_BaseAllocator)& Allocator() const;

  Standard_EXPORT void SetHistoryEnabled(bool theVal);
  Standard_EXPORT bool IsHistoryEnabled() const;

private:
  friend class BRepGraph_Builder;
  friend class BRepGraph_History;
  friend class BRepGraph_Analyze;
  friend class BRepGraph_Reconstruct;
  friend class BRepGraph_Mutator;

  Handle(NCollection_BaseAllocator) myAllocator;

  //! Definition vectors (indexed by BRepGraph_NodeId.Index).
  NCollection_Vector<BRepGraph_TopoNode::SolidDef>    mySolidDefs;
  NCollection_Vector<BRepGraph_TopoNode::ShellDef>    myShellDefs;
  NCollection_Vector<BRepGraph_TopoNode::FaceDef>     myFaceDefs;
  NCollection_Vector<BRepGraph_TopoNode::WireDef>     myWireDefs;
  NCollection_Vector<BRepGraph_TopoNode::EdgeDef>     myEdgeDefs;
  NCollection_Vector<BRepGraph_TopoNode::VertexDef>   myVertexDefs;
  NCollection_Vector<BRepGraph_TopoNode::CompoundDef>  myCompoundDefs;
  NCollection_Vector<BRepGraph_TopoNode::CompSolidDef> myCompSolidDefs;

  //! Usage vectors (indexed by BRepGraph_UsageId.Index).
  NCollection_Vector<BRepGraph_TopoNode::SolidUsage>    mySolidUsages;
  NCollection_Vector<BRepGraph_TopoNode::ShellUsage>    myShellUsages;
  NCollection_Vector<BRepGraph_TopoNode::FaceUsage>     myFaceUsages;
  NCollection_Vector<BRepGraph_TopoNode::WireUsage>     myWireUsages;
  NCollection_Vector<BRepGraph_TopoNode::EdgeUsage>     myEdgeUsages;
  NCollection_Vector<BRepGraph_TopoNode::VertexUsage>   myVertexUsages;
  NCollection_Vector<BRepGraph_TopoNode::CompoundUsage>  myCompoundUsages;
  NCollection_Vector<BRepGraph_TopoNode::CompSolidUsage> myCompSolidUsages;

  //! Geometry node vectors.
  NCollection_Vector<BRepGraph_GeomNode::Surf>   mySurfaces;
  NCollection_Vector<BRepGraph_GeomNode::Curve>  myCurves;
  NCollection_Vector<BRepGraph_GeomNode::PCurve> myPCurves;

  //! Map-based RelEdge storage.
  NCollection_DataMap<BRepGraph_NodeId,
                      NCollection_Vector<BRepGraph_RelEdge>> myOutRelEdges;
  NCollection_DataMap<BRepGraph_NodeId,
                      NCollection_Vector<BRepGraph_RelEdge>> myInRelEdges;

  //! Geometry deduplication registries.
  NCollection_IndexedDataMap<const Geom_Surface*, int> mySurfRegistry;
  NCollection_IndexedDataMap<const Geom_Curve*, int>   myCurveRegistry;

  //! TShape -> Definition NodeId reverse lookup.
  NCollection_DataMap<const TopoDS_TShape*, BRepGraph_NodeId> myTShapeToDefId;

  //! Opt-in UID system.
  bool                myUIDEnabled = false;
  std::atomic<size_t> myNextUIDCounter{0};
  uint32_t            myGeneration{0};

  NCollection_DataMap<BRepGraph_NodeId, BRepGraph_UID> myNodeToUID;
  NCollection_DataMap<BRepGraph_UID, BRepGraph_NodeId> myUIDToNodeId;

  //! Reverse index: edge def index -> wire def indices containing that edge.
  NCollection_DataMap<int, NCollection_Vector<int>> myEdgeToWires;

  //! History subsystem (records, reverse maps, enable/disable toggle).
  BRepGraph_History myHistoryLog;

  bool myIsDone;

  //! Internal build helpers.
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

public:
  //! Shared cache for edge/vertex shapes during multi-face reconstruction.
  using ReconstructCache = NCollection_DataMap<BRepGraph_NodeId, TopoDS_Shape>;
private:

  //! Shapes from Build().
  NCollection_DataMap<BRepGraph_NodeId, TopoDS_Shape> myOriginalShapes;

  mutable NCollection_DataMap<BRepGraph_NodeId, TopoDS_Shape> myCurrentShapes;
  mutable std::shared_mutex myCurrentShapesMutex;

  //! Dispatch a callback on the def vector matching theNode.Kind.
  //! Returns the callback result, or a default-constructed value for unhandled kinds.
  //! Usage: `auto result = dispatchDef(nodeId, [](auto& defVec, int idx) { ... });`
  template <typename Func>
  auto dispatchDef(BRepGraph_NodeId theNode, Func&& theFunc) const
    -> decltype(theFunc(mySolidDefs, 0))
  {
    switch (theNode.Kind)
    {
      case BRepGraph_NodeKind::Solid:     return theFunc(mySolidDefs, theNode.Index);
      case BRepGraph_NodeKind::Shell:     return theFunc(myShellDefs, theNode.Index);
      case BRepGraph_NodeKind::Face:      return theFunc(myFaceDefs, theNode.Index);
      case BRepGraph_NodeKind::Wire:      return theFunc(myWireDefs, theNode.Index);
      case BRepGraph_NodeKind::Edge:      return theFunc(myEdgeDefs, theNode.Index);
      case BRepGraph_NodeKind::Vertex:    return theFunc(myVertexDefs, theNode.Index);
      case BRepGraph_NodeKind::Compound:  return theFunc(myCompoundDefs, theNode.Index);
      case BRepGraph_NodeKind::CompSolid: return theFunc(myCompSolidDefs, theNode.Index);
      default: return decltype(theFunc(mySolidDefs, 0)){};
    }
  }

  //! Non-const overload of dispatchDef for mutable access.
  template <typename Func>
  auto dispatchDef(BRepGraph_NodeId theNode, Func&& theFunc)
    -> decltype(theFunc(mySolidDefs, 0))
  {
    switch (theNode.Kind)
    {
      case BRepGraph_NodeKind::Solid:     return theFunc(mySolidDefs, theNode.Index);
      case BRepGraph_NodeKind::Shell:     return theFunc(myShellDefs, theNode.Index);
      case BRepGraph_NodeKind::Face:      return theFunc(myFaceDefs, theNode.Index);
      case BRepGraph_NodeKind::Wire:      return theFunc(myWireDefs, theNode.Index);
      case BRepGraph_NodeKind::Edge:      return theFunc(myEdgeDefs, theNode.Index);
      case BRepGraph_NodeKind::Vertex:    return theFunc(myVertexDefs, theNode.Index);
      case BRepGraph_NodeKind::Compound:  return theFunc(myCompoundDefs, theNode.Index);
      case BRepGraph_NodeKind::CompSolid: return theFunc(myCompSolidDefs, theNode.Index);
      default: return decltype(theFunc(mySolidDefs, 0)){};
    }
  }
};

#endif // _BRepGraph_HeaderFile
