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
#include <BRepGraph_UID.hxx>
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
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_BaseAllocator.hxx>

#include <functional>
#include <utility>
#include <atomic>

class BRepGraph_Builder;
class BRepGraph_History;
class BRepGraph_Analyze;

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

  //! Total number of nodes in the graph (all kinds).
  Standard_EXPORT size_t NbNodes() const;

  // --- Opt-in UID system ---

  //! Enable/disable UID tracking. When disabled (default), UID allocation
  //! is a no-op during Build() for zero overhead.
  Standard_EXPORT void SetUIDEnabled(bool theVal);

  //! Query UID tracking mode.
  Standard_EXPORT bool IsUIDEnabled() const;

  //! UID of a node. Returns invalid UID if UIDs are disabled or node is invalid.
  Standard_EXPORT BRepGraph_UID UIDOf(BRepGraph_NodeId theNode) const;

  //! Resolve a UID to its typed NodeId.
  Standard_EXPORT BRepGraph_NodeId NodeIdFromUID(const BRepGraph_UID& theUID) const;

  //! Check whether a UID exists in this graph and is current-generation.
  Standard_EXPORT bool HasUID(const BRepGraph_UID& theUID) const;

  //! The graph's current generation number.
  Standard_EXPORT uint32_t Generation() const;

  // --- RelEdge system (map-based, for dynamic edges only) ---

  //! Add a relationship edge to the graph.
  //! @param[in] theFrom   source node
  //! @param[in] theTo     target node
  //! @param[in] theKind   relationship kind
  //! @return index of the new edge in the per-source vector
  Standard_EXPORT int AddRelEdge(BRepGraph_NodeId  theFrom,
                                 BRepGraph_NodeId  theTo,
                                 BRepGraph_RelKind theKind);

  //! Remove relationship edges of a given kind between two nodes.
  Standard_EXPORT void RemoveRelEdges(BRepGraph_NodeId  theFrom,
                                      BRepGraph_NodeId  theTo,
                                      BRepGraph_RelKind theKind);

  //! Number of outgoing dynamic edges from a node.
  Standard_EXPORT int NbRelEdgesFrom(BRepGraph_NodeId theNode) const;

  //! Number of incoming dynamic edges to a node.
  Standard_EXPORT int NbRelEdgesTo(BRepGraph_NodeId theNode) const;

  //! Iterates outgoing edges of the given kind, calling theCallback(const BRepGraph_RelEdge&).
  //! Avoids allocation of a temporary sequence.
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

  //! Iterates incoming edges of the given kind, calling theCallback(const BRepGraph_RelEdge&).
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

  // --- Topology-Geometry queries (direct field access, no RelEdges needed) ---

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
  Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> SameDomainFaces(
    BRepGraph_NodeId theFace) const;

  // --- Cache ---

  //! Bounding box (lazy-computed, thread-safe).
  Standard_EXPORT Bnd_Box BoundingBox(BRepGraph_NodeId theNode) const;

  //! Centroid (lazy-computed, thread-safe).
  Standard_EXPORT gp_Pnt Centroid(BRepGraph_NodeId theNode) const;

  //! Invalidate all cached data for a single node.
  Standard_EXPORT void Invalidate(BRepGraph_NodeId theNode);

  //! Invalidate theNode and all descendants via containment hierarchy.
  Standard_EXPORT void InvalidateSubgraph(BRepGraph_NodeId theNode);

  // --- User attributes ---

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

  // --- History ---

  //! Number of history records.
  Standard_EXPORT int NbHistoryRecords() const;

  //! Direct access to a history record by index.
  Standard_EXPORT const BRepGraph_HistoryRecord& HistoryRecord(int theIdx) const;

  //! Walk history backwards from a modified node to its original.
  Standard_EXPORT BRepGraph_NodeId FindOriginal(BRepGraph_NodeId theModified) const;

  //! Walk history forwards from an original node to all derived nodes.
  Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> FindDerived(
    BRepGraph_NodeId theOriginal) const;

  //! Apply a mutation and record a HistoryRecord.
  //! NOT thread-safe -- caller ensures exclusive access.
  Standard_EXPORT void ApplyModification(
    BRepGraph_NodeId                                                                    theTarget,
    std::function<NCollection_Vector<BRepGraph_NodeId>(BRepGraph&, BRepGraph_NodeId)> theModifier,
    const TCollection_AsciiString&                                                      theOpLabel);

  // --- Shape access ---

  //! Returns the current TopoDS_Shape for a node.
  //! If unmodified and an original exists, returns it.
  //! If modified or no original exists, reconstructs from graph state and caches.
  Standard_EXPORT TopoDS_Shape Shape(BRepGraph_NodeId theNode) const;

  //! True if an original shape was stored during Build().
  Standard_EXPORT bool HasOriginalShape(BRepGraph_NodeId theNode) const;

  //! Returns the original shape from Build(). Throws if none.
  Standard_EXPORT const TopoDS_Shape& OriginalOf(BRepGraph_NodeId theNode) const;

  // --- Reconstruction ---

  //! Rebuild a TopoDS_Shape tree from current graph state.
  //! Supports all topology kinds: Solid, Shell, Face, Wire, Edge, Vertex.
  Standard_EXPORT TopoDS_Shape ReconstructShape(BRepGraph_NodeId theRoot) const;

  //! Reconstruct a TopoDS_Face by rebuilding its wires from current graph state.
  //! This picks up any edge replacements done via ReplaceEdgeInWire().
  Standard_EXPORT TopoDS_Shape ReconstructFace(int theFaceIdx) const;

  // --- Mutation ---

  //! Mutable access to an Edge node.
  Standard_EXPORT BRepGraph_TopoNode::Edge& MutableEdge(int theIdx);

  //! Mutable access to a Wire node.
  Standard_EXPORT BRepGraph_TopoNode::Wire& MutableWire(int theIdx);

  //! Mutable access to a Vertex node.
  Standard_EXPORT BRepGraph_TopoNode::Vertex& MutableVertex(int theIdx);

  //! Create and register a new PCurve node, linking it to edge and face.
  Standard_EXPORT BRepGraph_NodeId AddPCurveToEdge(BRepGraph_NodeId            theEdge,
                                                   BRepGraph_NodeId            theFace,
                                                   const Handle(Geom2d_Curve)& theCurve2d,
                                                   double                      theFirst,
                                                   double                      theLast);

  //! Replace all references to theOldEdge with theNewEdge in a wire's OrderedEdges.
  Standard_EXPORT void ReplaceEdgeInWire(int              theWireIdx,
                                         BRepGraph_NodeId theOldEdge,
                                         BRepGraph_NodeId theNewEdge,
                                         bool             theReversed);

  //! Number of distinct faces referencing an edge (via pcurves / wire membership).
  Standard_EXPORT int FaceCountForEdge(int theEdgeIdx) const;

  //! Wire indices that contain a given edge.
  Standard_EXPORT const NCollection_Vector<int>& WiresOfEdge(int theEdgeIdx) const;

  //! Record a history entry: theOriginal was replaced by theReplacements.
  Standard_EXPORT void RecordHistory(const TCollection_AsciiString&                theOpLabel,
                                     BRepGraph_NodeId                              theOriginal,
                                     const NCollection_Vector<BRepGraph_NodeId>& theReplacements);

  // --- Allocator ---

  //! Set the pool allocator before Build().
  Standard_EXPORT void SetAllocator(const Handle(NCollection_BaseAllocator)& theAlloc);

  //! Allocator access.
  Standard_EXPORT const Handle(NCollection_BaseAllocator)& Allocator() const;

  //! Enable/disable history recording. When disabled, RecordHistory() is a no-op.
  Standard_EXPORT void SetHistoryEnabled(bool theVal);

  //! Query history recording mode.
  Standard_EXPORT bool IsHistoryEnabled() const;

private:
  friend class BRepGraph_Builder;
  friend class BRepGraph_History;
  friend class BRepGraph_Analyze;

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

  //! Map-based RelEdge storage: source node -> outgoing dynamic edges.
  //! Zero overhead for nodes without dynamic edges.
  NCollection_DataMap<BRepGraph_NodeId,
                      NCollection_Vector<BRepGraph_RelEdge>,
                      BRepGraph_NodeId::Hasher> myOutRelEdges;

  //! Target node -> incoming dynamic edges (for reverse lookups).
  NCollection_DataMap<BRepGraph_NodeId,
                      NCollection_Vector<BRepGraph_RelEdge>,
                      BRepGraph_NodeId::Hasher> myInRelEdges;

  //! Geometry deduplication registries (key = Handle::get() raw pointer).
  NCollection_IndexedDataMap<void*, int> mySurfRegistry;
  NCollection_IndexedDataMap<void*, int> myCurveRegistry;

  //! Shape -> NodeId reverse lookup.
  NCollection_DataMap<void*, BRepGraph_NodeId> myTShapeToNodeId;

  //! Opt-in UID system.
  bool                myUIDEnabled = false;
  std::atomic<size_t> myNextUIDCounter{0};
  uint32_t            myGeneration{0};

  NCollection_DataMap<BRepGraph_NodeId, BRepGraph_UID, BRepGraph_NodeId::Hasher> myNodeToUID;
  NCollection_DataMap<BRepGraph_UID, BRepGraph_NodeId, BRepGraph_UID::Hasher>    myUIDToNodeId;

  //! Reverse index: edge index -> wire indices containing that edge.
  NCollection_DataMap<int, NCollection_Vector<int>> myEdgeToWires;

  //! History log.
  NCollection_Vector<BRepGraph_HistoryRecord> myHistory;

  //! History reverse maps for O(1) lookups.
  NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId, BRepGraph_NodeId::Hasher>
    myDerivedToOriginal;
  NCollection_DataMap<BRepGraph_NodeId,
                      NCollection_Vector<BRepGraph_NodeId>,
                      BRepGraph_NodeId::Hasher>
    myOriginalToDerived;

  bool myIsDone;
  bool myIsHistoryEnabled = true;

  //! Internal build helpers.
  BRepGraph_NodeId registerSurface(const Handle(Geom_Surface)&       theSurf,
                                   const Handle(Poly_Triangulation)& theTri);

  BRepGraph_NodeId registerCurve(const Handle(Geom_Curve)& theCrv);

  BRepGraph_NodeId createPCurveNode(const Handle(Geom2d_Curve)& theCrv2d,
                                    BRepGraph_NodeId            theEdge,
                                    BRepGraph_NodeId            theFace,
                                    double                      theFirst,
                                    double                      theLast);

  void invalidateSubgraphImpl(BRepGraph_NodeId theNode);

  BRepGraph_UID allocateUID(BRepGraph_NodeId theNodeId);

  BRepGraph_NodeCache* mutableCache(BRepGraph_NodeId theNode);

  //! Propagate IsModified up the Parent chain and invalidate myCurrentShapes.
  void markModified(BRepGraph_NodeId theNode);

  //! Reconstruct a TopoDS_Shape from graph fields only (no cache/original lookup).
  TopoDS_Shape reconstructNode(BRepGraph_NodeId theNode) const;

  //! Shapes from Build() — read-only after build.
  //! Empty if graph was constructed algorithmically.
  NCollection_DataMap<BRepGraph_NodeId, TopoDS_Shape,
                      BRepGraph_NodeId::Hasher> myOriginalShapes;

  //! Lazily-cached current shapes.
  //! Populated on first Shape() call, invalidated when IsModified is set.
  mutable NCollection_DataMap<BRepGraph_NodeId, TopoDS_Shape,
                              BRepGraph_NodeId::Hasher> myCurrentShapes;
};

#endif // _BRepGraph_HeaderFile
