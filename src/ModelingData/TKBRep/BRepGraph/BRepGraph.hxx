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
#include <BRepGraph_RelEdge.hxx>
#include <BRepGraph_AttrRegistry.hxx>
#include <BRepGraph_CachedValue.hxx>
#include <BRepGraph_UserAttribute.hxx>
#include <BRepGraph_NodeCache.hxx>
#include <BRepGraph_TopoNode.hxx>
#include <BRepGraph_GeomNode.hxx>
#include <BRepGraph_HistoryRecord.hxx>
#include <BRepGraph_SubGraph.hxx>

#include <Standard_DefineAlloc.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Trsf.hxx>
#include <Bnd_Box.hxx>
#include <gp_Pnt.hxx>

#include <NCollection_Vector.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_BaseAllocator.hxx>

#include <shared_mutex>
#include <functional>
#include <utility>
#include <atomic>

//! @brief Bidirectional topology-geometry graph over TopoDS / BRep.
//!
//! BRepGraph is a **non-owning runtime index**.  It holds Handle<> refs
//! (incrementing refcount) to OCCT geometry but never deep-copies it.
//! TopoDS_Shape appears only at the Build() entry point and the
//! ReconstructShape() output.
//!
//! ## Typical lifecycle
//! @code
//!   BRepGraph aGraph(myPoolAllocator);
//!   aGraph.Build(myShape, /*parallel=*/ true);
//!
//!   Bnd_Box aBox = aGraph.BoundingBox(someNodeId);
//!   auto aSameDom = aGraph.SameDomainFaces(faceId);
//! @endcode
//!
//! ## Thread safety
//! All const query methods (including lazy cache Gets) are thread-safe.
//! Build() is internally parallel when requested.
//! ApplyModification() requires exclusive access.
class BRepGraph
{
public:
  DEFINE_STANDARD_ALLOC

  //! Non-copyable, non-movable (contains mutex and atomic members).
  BRepGraph(const BRepGraph&)            = delete;
  BRepGraph& operator=(const BRepGraph&) = delete;

  //! Default constructor -- uses CommonBaseAllocator.
  Standard_EXPORT BRepGraph();

  //! Construct with a caller-supplied pool allocator.
  Standard_EXPORT explicit BRepGraph(const Handle(NCollection_BaseAllocator)& theAlloc);

  //! Build the full graph from a TopoDS_Shape.
  //!
  //! @param theShape    Root shape (Solid, Shell, Compound, or any container).
  //! @param theParallel If true, face-level construction runs in parallel.
  Standard_EXPORT void Build(const TopoDS_Shape& theShape, bool theParallel = false);

  //! True after a successful Build().
  Standard_EXPORT bool IsDone() const;

  //! Node counts by kind.
  Standard_EXPORT int NbSolids() const;
  Standard_EXPORT int NbShells() const;
  Standard_EXPORT int NbFaces() const;
  Standard_EXPORT int NbWires() const;
  Standard_EXPORT int NbEdges() const;
  Standard_EXPORT int NbVertices() const;
  Standard_EXPORT int NbSurfaces() const;
  Standard_EXPORT int NbCurves() const;
  Standard_EXPORT int NbPCurves() const;

  //! Direct typed node access by per-kind index.
  Standard_EXPORT const BRepGraph_TopoNode::Solid& Solid(int theIdx) const;
  Standard_EXPORT const BRepGraph_TopoNode::Shell& Shell(int theIdx) const;
  Standard_EXPORT const BRepGraph_TopoNode::Face& Face(int theIdx) const;
  Standard_EXPORT const BRepGraph_TopoNode::Wire& Wire(int theIdx) const;
  Standard_EXPORT const BRepGraph_TopoNode::Edge& Edge(int theIdx) const;
  Standard_EXPORT const BRepGraph_TopoNode::Vertex& Vertex(int theIdx) const;
  Standard_EXPORT const BRepGraph_GeomNode::Surf& Surf(int theIdx) const;
  Standard_EXPORT const BRepGraph_GeomNode::Curve& Curve(int theIdx) const;
  Standard_EXPORT const BRepGraph_GeomNode::PCurve& PCurve(int theIdx) const;

  //! Generic topology node lookup by NodeId.
  //! Returns nullptr for invalid ids or geometry-kind ids.
  Standard_EXPORT const BRepGraph_TopoNode::Base* TopoNode(BRepGraph_NodeId theId) const;

  //! The graph's current generation number.
  Standard_EXPORT uint32_t Generation() const;

  //! Total number of nodes in the graph (all kinds).
  Standard_EXPORT size_t NbNodes() const;

  //! Resolve a UID to its typed NodeId.
  Standard_EXPORT BRepGraph_NodeId NodeIdFromUID(const BRepGraph_UID& theUID) const;

  //! Check whether a UID exists in this graph and is current-generation.
  Standard_EXPORT bool HasUID(const BRepGraph_UID& theUID) const;

  //! Number of relationship edges.
  Standard_EXPORT int NbRelEdges() const;

  //! Direct access to a relationship edge by index.
  Standard_EXPORT const BRepGraph_RelEdge& RelEdge(int theIdx) const;

  //! Outgoing edges from a node, filtered by relationship kind.
  Standard_EXPORT NCollection_Sequence<int> OutEdgesOfKind(BRepGraph_NodeId  theNode,
                                                           BRepGraph_RelKind theKind) const;

  //! Incoming edges to a node, filtered by relationship kind.
  Standard_EXPORT NCollection_Sequence<int> InEdgesOfKind(BRepGraph_NodeId  theNode,
                                                          BRepGraph_RelKind theKind) const;

  //! Iterates outgoing edges of the given kind, calling theCallback(edgeIndex) for each.
  //! Avoids allocation of a temporary sequence.
  //! @param[in] theNodeId  source node
  //! @param[in] theKind    relationship kind to filter
  //! @param[in] theCallback callable invoked with each matching rel-edge index
  template <typename Func>
  void ForEachOutEdgeOfKind(BRepGraph_NodeId  theNodeId,
                            BRepGraph_RelKind theKind,
                            const Func&       theCallback) const
  {
    const BRepGraph_TopoNode::Base* aBase = TopoNode(theNodeId);
    if (aBase == nullptr)
      return;
    for (int anIdx = 0; anIdx < aBase->OutRelEdgeIndices.Length(); ++anIdx)
    {
      const int anEdgeIdx = aBase->OutRelEdgeIndices.Value(anIdx);
      if (myRelEdges.Value(anEdgeIdx).Kind == theKind)
        theCallback(anEdgeIdx);
    }
  }

  //! Iterates incoming edges of the given kind, calling theCallback(edgeIndex) for each.
  //! Avoids allocation of a temporary sequence.
  //! @param[in] theNodeId  target node
  //! @param[in] theKind    relationship kind to filter
  //! @param[in] theCallback callable invoked with each matching rel-edge index
  template <typename Func>
  void ForEachInEdgeOfKind(BRepGraph_NodeId  theNodeId,
                           BRepGraph_RelKind theKind,
                           const Func&       theCallback) const
  {
    const BRepGraph_TopoNode::Base* aBase = TopoNode(theNodeId);
    if (aBase == nullptr)
      return;
    for (int anIdx = 0; anIdx < aBase->InRelEdgeIndices.Length(); ++anIdx)
    {
      const int anEdgeIdx = aBase->InRelEdgeIndices.Value(anIdx);
      if (myRelEdges.Value(anEdgeIdx).Kind == theKind)
        theCallback(anEdgeIdx);
    }
  }

  //! SurfNode for a given FaceNode.
  Standard_EXPORT BRepGraph_NodeId SurfaceOf(BRepGraph_NodeId theFace) const;

  //! All FaceNodes referencing a SurfNode.
  Standard_EXPORT const NCollection_Vector<BRepGraph_NodeId>& FacesOnSurface(
    BRepGraph_NodeId theSurf) const;

  //! CurveNode for a given EdgeNode (invalid id if degenerate).
  Standard_EXPORT BRepGraph_NodeId CurveOf(BRepGraph_NodeId theEdge) const;

  //! All EdgeNodes referencing a CurveNode.
  Standard_EXPORT const NCollection_Vector<BRepGraph_NodeId>& EdgesOnCurve(
    BRepGraph_NodeId theCurve) const;

  //! PCurveNode for a specific (Edge, Face) pair.
  Standard_EXPORT BRepGraph_NodeId PCurveOf(BRepGraph_NodeId theEdge,
                                            BRepGraph_NodeId theFace) const;

  //! Accumulated global transform for any topology node.
  Standard_EXPORT gp_Trsf GlobalTransform(BRepGraph_NodeId theNode) const;

  //! All FaceNodes sharing the same Geom_Surface handle as theFace,
  //! excluding theFace itself.
  Standard_EXPORT NCollection_Sequence<BRepGraph_NodeId> SameDomainFaces(
    BRepGraph_NodeId theFace) const;

  //! Bounding box (lazy-computed, thread-safe).
  Standard_EXPORT Bnd_Box BoundingBox(BRepGraph_NodeId theNode) const;

  //! Centroid (lazy-computed, thread-safe).
  Standard_EXPORT gp_Pnt Centroid(BRepGraph_NodeId theNode) const;

  //! Surface area -- meaningful for FaceNodes only.
  Standard_EXPORT double Area(BRepGraph_NodeId theFace) const;

  //! Curve length -- meaningful for EdgeNodes only.
  Standard_EXPORT double Length(BRepGraph_NodeId theEdge) const;

  //! Invalidate all cached data for a single node.
  Standard_EXPORT void Invalidate(BRepGraph_NodeId theNode);

  //! Invalidate theNode and all descendants via Contains edges.
  Standard_EXPORT void InvalidateSubgraph(BRepGraph_NodeId theNode);

  //! Attach a user-defined attribute to a topology node.
  Standard_EXPORT void SetUserAttribute(BRepGraph_NodeId             theNode,
                                        int                          theKey,
                                        const BRepGraph_UserAttrPtr& theAttr);

  //! Retrieve a user attribute by node and key.
  Standard_EXPORT BRepGraph_UserAttrPtr GetUserAttribute(BRepGraph_NodeId theNode,
                                                         int              theKey) const;

  //! Remove a user attribute from a node.
  Standard_EXPORT bool RemoveUserAttribute(BRepGraph_NodeId theNode, int theKey);

  //! Invalidate a specific user attribute on a node.
  Standard_EXPORT void InvalidateUserAttribute(BRepGraph_NodeId theNode, int theKey);

  //! Convenience: get typed attribute value with lazy computation.
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

  //! Number of history records.
  Standard_EXPORT int NbHistoryRecords() const;

  //! Direct access to a history record by index.
  Standard_EXPORT const BRepGraph_HistoryRecord& History(int theIdx) const;

  //! Walk history backwards from a modified node to its original.
  Standard_EXPORT BRepGraph_NodeId FindOriginal(BRepGraph_NodeId theModified) const;

  //! Walk history forwards from an original node to all derived nodes.
  Standard_EXPORT NCollection_Sequence<BRepGraph_NodeId> FindDerived(
    BRepGraph_NodeId theOriginal) const;

  //! Apply a mutation and record a HistoryRecord.
  //! NOT thread-safe -- caller ensures exclusive access.
  Standard_EXPORT void ApplyModification(
    BRepGraph_NodeId                                                                    theTarget,
    std::function<NCollection_Sequence<BRepGraph_NodeId>(BRepGraph&, BRepGraph_NodeId)> theModifier,
    const TCollection_AsciiString&                                                      theOpLabel);

  //! Rebuild a TopoDS_Shape tree from current graph state.
  //! @param theRoot  Must be Solid, Shell, or Face kind.
  Standard_EXPORT TopoDS_Shape ReconstructShape(BRepGraph_NodeId theRoot) const;

  //! Split into connected components (non-owning SubGraph views).
  Standard_EXPORT NCollection_Sequence<BRepGraph_SubGraph> Decompose() const;

  //! Parallel iteration over FaceNode indices in a SubGraph.
  Standard_EXPORT void ParallelForEachFace(const BRepGraph_SubGraph&               theSub,
                                           const std::function<void(int faceIdx)>& theLambda) const;

  //! Parallel iteration over EdgeNode indices in a SubGraph.
  Standard_EXPORT void ParallelForEachEdge(const BRepGraph_SubGraph&               theSub,
                                           const std::function<void(int edgeIdx)>& theLambda) const;

  //! All (Edge, Face) pairs missing a PCurveNode.
  Standard_EXPORT NCollection_Sequence<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>>
                  DetectMissingPCurves() const;

  //! Nodes with tolerance conflicts across shared geometry.
  Standard_EXPORT NCollection_Sequence<BRepGraph_NodeId> DetectToleranceConflicts(
    double theThreshold) const;

  //! WireNodes with < 2 edges or non-closed outer wires.
  Standard_EXPORT NCollection_Sequence<BRepGraph_NodeId> DetectDegenerateWires() const;

  //! Detect free (boundary) edges: edges referenced by exactly one face.
  //! Degenerate edges are excluded.
  Standard_EXPORT NCollection_Sequence<BRepGraph_NodeId> FreeEdges() const;

  //! Reconstruct a TopoDS_Face by rebuilding its wires from current graph state.
  //! This picks up any edge replacements done via ReplaceEdgeInWire().
  //! @param[in] theFaceIdx  index into the graph's face vector
  Standard_EXPORT TopoDS_Shape ReconstructFace(int theFaceIdx) const;

  // --- Generic graph mutation APIs ---

  //! Add a relationship edge to the graph.
  //! @param[in] theFrom   source node
  //! @param[in] theTo     target node
  //! @param[in] theKind   relationship kind
  //! @return index of the new edge
  Standard_EXPORT int AddRelEdge(BRepGraph_NodeId  theFrom,
                                 BRepGraph_NodeId  theTo,
                                 BRepGraph_RelKind theKind);

  //! Remove relationship edges of a given kind between two nodes.
  //! @param[in] theFrom   source node
  //! @param[in] theTo     target node
  //! @param[in] theKind   relationship kind to remove
  Standard_EXPORT void RemoveRelEdges(BRepGraph_NodeId  theFrom,
                                      BRepGraph_NodeId  theTo,
                                      BRepGraph_RelKind theKind);

  //! Mutable access to an Edge node.
  //! @param[in] theIdx  edge index
  Standard_EXPORT BRepGraph_TopoNode::Edge& MutableEdge(int theIdx);

  //! Mutable access to a Wire node.
  //! @param[in] theIdx  wire index
  Standard_EXPORT BRepGraph_TopoNode::Wire& MutableWire(int theIdx);

  //! Mutable access to a Vertex node.
  //! @param[in] theIdx  vertex index
  Standard_EXPORT BRepGraph_TopoNode::Vertex& MutableVertex(int theIdx);

  //! Create and register a new PCurve node, linking it to edge and face.
  //! Also adds a PCurveEntry to the edge's PCurves list.
  //! @param[in] theEdge    edge node id
  //! @param[in] theFace    face node id
  //! @param[in] theCurve2d 2D curve handle
  //! @param[in] theFirst   first parameter
  //! @param[in] theLast    last parameter
  //! @return node id of the new PCurve node
  Standard_EXPORT BRepGraph_NodeId AddPCurveToEdge(BRepGraph_NodeId            theEdge,
                                                   BRepGraph_NodeId            theFace,
                                                   const Handle(Geom2d_Curve)& theCurve2d,
                                                   double                      theFirst,
                                                   double                      theLast);

  //! Replace all references to theOldEdge with theNewEdge in a wire's OrderedEdges.
  //! @param[in] theWireIdx   wire index
  //! @param[in] theOldEdge   edge node id to replace
  //! @param[in] theNewEdge   replacement edge node id
  //! @param[in] theReversed  if true, flip orientation of the replacement
  Standard_EXPORT void ReplaceEdgeInWire(int              theWireIdx,
                                         BRepGraph_NodeId theOldEdge,
                                         BRepGraph_NodeId theNewEdge,
                                         bool             theReversed);

  //! Number of distinct faces referencing an edge (via pcurves / wire membership).
  //! @param[in] theEdgeIdx  edge index
  //! @return face count
  Standard_EXPORT int FaceCountForEdge(int theEdgeIdx) const;

  //! Wire indices that contain a given edge.
  //! @param[in] theEdgeIdx edge index
  //! @return sequence of wire indices (empty if none)
  Standard_EXPORT const NCollection_Vector<int>& WiresOfEdge(int theEdgeIdx) const;

  //! Record a history entry: theOriginal was replaced by theReplacements.
  //! @param[in] theOpLabel       operation label
  //! @param[in] theOriginal      original node
  //! @param[in] theReplacements  replacement nodes
  Standard_EXPORT void RecordHistory(const TCollection_AsciiString&                theOpLabel,
                                     BRepGraph_NodeId                              theOriginal,
                                     const NCollection_Sequence<BRepGraph_NodeId>& theReplacements);

  //! Set the pool allocator before Build().
  //! Must be called before Build() to take effect on internal collections.
  //! @param[in] theAlloc allocator to use (null => CommonBaseAllocator)
  Standard_EXPORT void SetAllocator(const Handle(NCollection_BaseAllocator)& theAlloc);

  //! Allocator access.
  Standard_EXPORT const Handle(NCollection_BaseAllocator)& Allocator() const;

  //! Enable/disable history recording. When disabled, RecordHistory() is a no-op.
  //! Default: true.
  Standard_EXPORT void SetHistoryEnabled(bool theVal);

  //! Query history recording mode.
  Standard_EXPORT bool IsHistoryEnabled() const;

private:
  Handle(NCollection_BaseAllocator) myAllocator;

  //! Per-kind node vectors.
  NCollection_Vector<BRepGraph_TopoNode::Solid>  mySolids;
  NCollection_Vector<BRepGraph_TopoNode::Shell>  myShells;
  NCollection_Vector<BRepGraph_TopoNode::Face>   myFaces;
  NCollection_Vector<BRepGraph_TopoNode::Wire>   myWires;
  NCollection_Vector<BRepGraph_TopoNode::Edge>   myEdges;
  NCollection_Vector<BRepGraph_TopoNode::Vertex> myVertices;

  NCollection_Vector<BRepGraph_GeomNode::Surf>   mySurfaces;
  NCollection_Vector<BRepGraph_GeomNode::Curve>  myCurves;
  NCollection_Vector<BRepGraph_GeomNode::PCurve> myPCurves;

  //! Flat relationship edge array.
  NCollection_Vector<BRepGraph_RelEdge> myRelEdges;

  //! Geometry deduplication registries (key = Handle::get() raw pointer).
  NCollection_IndexedDataMap<void*, int> mySurfRegistry;
  NCollection_IndexedDataMap<void*, int> myCurveRegistry;

  mutable std::shared_mutex mySurfRegistryMutex;
  mutable std::shared_mutex myCurveRegistryMutex;

  //! Shape -> NodeId reverse lookup.
  NCollection_DataMap<void*, BRepGraph_NodeId> myTShapeToNodeId;

  //! UID system.
  std::atomic<size_t> myNextUIDCounter{0};
  uint32_t            myGeneration{0};

  NCollection_DataMap<BRepGraph_UID, BRepGraph_NodeId, BRepGraph_UID::Hasher> myUIDToNodeId;

  //! Reverse index: edge index -> wire indices containing that edge.
  NCollection_DataMap<int, NCollection_Vector<int>> myEdgeToWires;

  //! History log.
  NCollection_Vector<BRepGraph_HistoryRecord> myHistory;

  bool myIsDone;
  bool myIsHistoryEnabled = true;

  //! True during Phase 3 of Build() -- allows mutex bypass in registration.
  std::atomic<bool> myIsBuilding{false};

  //! Internal build helpers.
  BRepGraph_NodeId registerSurface(const Handle(Geom_Surface)&       theSurf,
                                   const Handle(Poly_Triangulation)& theTri);

  BRepGraph_NodeId registerCurve(const Handle(Geom_Curve)& theCrv);

  BRepGraph_NodeId createPCurveNode(const Handle(Geom2d_Curve)& theCrv2d,
                                    BRepGraph_NodeId            theEdge,
                                    BRepGraph_NodeId            theFace,
                                    double                      theFirst,
                                    double                      theLast);

  int addRelEdge(const BRepGraph_RelEdge& theEdge);

  void buildSameDomainEdges();

  void invalidateSubgraphImpl(BRepGraph_NodeId theNode);

  BRepGraph_UID allocateUID(BRepGraph_NodeId theNodeId);

  BRepGraph_NodeCache* mutableCache(BRepGraph_NodeId theNode);
};

#endif // _BRepGraph_HeaderFile
