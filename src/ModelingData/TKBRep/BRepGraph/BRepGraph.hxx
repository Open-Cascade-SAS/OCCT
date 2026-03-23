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

//! Hasher for raw pointer keys (hashes by address).  Used internally by BRepGraph
//! for typed-pointer map keys (replaces void* for type safety).
template<typename T>
struct BRepGraph_PtrHasher
{
  size_t operator()(const T* thePtr) const noexcept
  { return std::hash<const void*>{}(static_cast<const void*>(thePtr)); }
  bool operator()(const T* theA, const T* theB) const noexcept
  { return theA == theB; }
};

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

  //! Accumulated global transform for a usage.
  Standard_EXPORT gp_Trsf GlobalTransform(BRepGraph_UsageId theUsage) const;

  //! Accumulated global transform for a definition (from first usage).
  Standard_EXPORT gp_Trsf GlobalTransform(BRepGraph_NodeId theDefId) const;

  Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> SameDomainFaces(
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

  Standard_EXPORT BRepGraph_NodeId AddPCurveToEdge(BRepGraph_NodeId            theEdgeDef,
                                                   BRepGraph_NodeId            theFaceDef,
                                                   const Handle(Geom2d_Curve)& theCurve2d,
                                                   double                      theFirst,
                                                   double                      theLast);

  Standard_EXPORT void ReplaceEdgeInWire(int              theWireDefIdx,
                                         BRepGraph_NodeId theOldEdgeDef,
                                         BRepGraph_NodeId theNewEdgeDef,
                                         bool             theReversed);

  Standard_EXPORT int FaceCountForEdge(int theEdgeDefIdx) const;
  Standard_EXPORT const NCollection_Vector<int>& WiresOfEdge(int theEdgeDefIdx) const;

  Standard_EXPORT void RecordHistory(const TCollection_AsciiString&                theOpLabel,
                                     BRepGraph_NodeId                              theOriginal,
                                     const NCollection_Vector<BRepGraph_NodeId>& theReplacements);

  // --- Allocator ---

  Standard_EXPORT void SetAllocator(const Handle(NCollection_BaseAllocator)& theAlloc);
  Standard_EXPORT const Handle(NCollection_BaseAllocator)& Allocator() const;

  Standard_EXPORT void SetHistoryEnabled(bool theVal);
  Standard_EXPORT bool IsHistoryEnabled() const;

private:
  friend class BRepGraph_Builder;
  friend class BRepGraph_History;
  friend class BRepGraph_Analyze;

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
                      NCollection_Vector<BRepGraph_RelEdge>,
                      BRepGraph_NodeId::Hasher> myOutRelEdges;
  NCollection_DataMap<BRepGraph_NodeId,
                      NCollection_Vector<BRepGraph_RelEdge>,
                      BRepGraph_NodeId::Hasher> myInRelEdges;

  //! Geometry deduplication registries.
  NCollection_IndexedDataMap<const Geom_Surface*,
                             int,
                             BRepGraph_PtrHasher<Geom_Surface>> mySurfRegistry;
  NCollection_IndexedDataMap<const Geom_Curve*,
                             int,
                             BRepGraph_PtrHasher<Geom_Curve>>   myCurveRegistry;

  //! TShape -> Definition NodeId reverse lookup.
  NCollection_DataMap<const TopoDS_TShape*,
                      BRepGraph_NodeId,
                      BRepGraph_PtrHasher<TopoDS_TShape>>        myTShapeToDefId;

  //! Opt-in UID system.
  bool                myUIDEnabled = false;
  std::atomic<size_t> myNextUIDCounter{0};
  uint32_t            myGeneration{0};

  NCollection_DataMap<BRepGraph_NodeId, BRepGraph_UID, BRepGraph_NodeId::Hasher> myNodeToUID;
  NCollection_DataMap<BRepGraph_UID, BRepGraph_NodeId, BRepGraph_UID::Hasher>    myUIDToNodeId;

  //! Reverse index: edge def index -> wire def indices containing that edge.
  NCollection_DataMap<int, NCollection_Vector<int>> myEdgeToWires;

  //! History log.
  NCollection_Vector<BRepGraph_HistoryRecord> myHistory;

  //! History reverse maps.
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
                                    BRepGraph_NodeId            theEdgeDef,
                                    BRepGraph_NodeId            theFaceDef,
                                    double                      theFirst,
                                    double                      theLast);

  void invalidateSubgraphImpl(BRepGraph_NodeId theNode);
  BRepGraph_UID allocateUID(BRepGraph_NodeId theNodeId);
  BRepGraph_NodeCache* mutableCache(BRepGraph_NodeId theNode);
  void markModified(BRepGraph_NodeId theDefId);
  //! Shared cache for edge/vertex shapes during multi-face reconstruction.
  using ReconstructCache = NCollection_DataMap<BRepGraph_NodeId, TopoDS_Shape,
                                               BRepGraph_NodeId::Hasher>;

  TopoDS_Shape reconstructNode(BRepGraph_NodeId theNode) const;
  TopoDS_Shape reconstructFaceWithCache(BRepGraph_NodeId theNode,
                                        ReconstructCache& theEdgeCache) const;
  TopoDS_Shape reconstructUsage(BRepGraph_UsageId theUsage) const;

  //! Shapes from Build().
  NCollection_DataMap<BRepGraph_NodeId, TopoDS_Shape,
                      BRepGraph_NodeId::Hasher> myOriginalShapes;

  mutable NCollection_DataMap<BRepGraph_NodeId, TopoDS_Shape,
                              BRepGraph_NodeId::Hasher> myCurrentShapes;
  mutable std::shared_mutex myCurrentShapesMutex;
};

#endif // _BRepGraph_HeaderFile
