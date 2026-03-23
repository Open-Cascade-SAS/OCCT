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

#include <BRepGraph.hxx>
#include <BRepGraph_Data.hxx>

#include <BRepGraph_Builder.hxx>

#include <NCollection_Map.hxx>
#include <Standard_ProgramError.hxx>

#include <shared_mutex>

//=================================================================================================
// dispatchDef template implementations (only used in this TU).

template <typename Func>
auto BRepGraph::dispatchDef(BRepGraph_NodeId theNode, Func&& theFunc) const
  -> decltype(theFunc(std::declval<const NCollection_Vector<BRepGraph_TopoNode::SolidDef>&>(), 0))
{
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Solid:     return theFunc(myData->mySolidDefs, theNode.Index);
    case BRepGraph_NodeId::Kind::Shell:     return theFunc(myData->myShellDefs, theNode.Index);
    case BRepGraph_NodeId::Kind::Face:      return theFunc(myData->myFaceDefs, theNode.Index);
    case BRepGraph_NodeId::Kind::Wire:      return theFunc(myData->myWireDefs, theNode.Index);
    case BRepGraph_NodeId::Kind::Edge:      return theFunc(myData->myEdgeDefs, theNode.Index);
    case BRepGraph_NodeId::Kind::Vertex:    return theFunc(myData->myVertexDefs, theNode.Index);
    case BRepGraph_NodeId::Kind::Compound:  return theFunc(myData->myCompoundDefs, theNode.Index);
    case BRepGraph_NodeId::Kind::CompSolid: return theFunc(myData->myCompSolidDefs, theNode.Index);
    default: return decltype(theFunc(myData->mySolidDefs, 0)){};
  }
}

template <typename Func>
auto BRepGraph::dispatchDef(BRepGraph_NodeId theNode, Func&& theFunc)
  -> decltype(theFunc(std::declval<NCollection_Vector<BRepGraph_TopoNode::SolidDef>&>(), 0))
{
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Solid:     return theFunc(myData->mySolidDefs, theNode.Index);
    case BRepGraph_NodeId::Kind::Shell:     return theFunc(myData->myShellDefs, theNode.Index);
    case BRepGraph_NodeId::Kind::Face:      return theFunc(myData->myFaceDefs, theNode.Index);
    case BRepGraph_NodeId::Kind::Wire:      return theFunc(myData->myWireDefs, theNode.Index);
    case BRepGraph_NodeId::Kind::Edge:      return theFunc(myData->myEdgeDefs, theNode.Index);
    case BRepGraph_NodeId::Kind::Vertex:    return theFunc(myData->myVertexDefs, theNode.Index);
    case BRepGraph_NodeId::Kind::Compound:  return theFunc(myData->myCompoundDefs, theNode.Index);
    case BRepGraph_NodeId::Kind::CompSolid: return theFunc(myData->myCompSolidDefs, theNode.Index);
    default: return decltype(theFunc(myData->mySolidDefs, 0)){};
  }
}

//=================================================================================================

BRepGraph::BRepGraph()
    : myData(std::make_unique<BRepGraph_Data>())
{
}

//=================================================================================================

BRepGraph::BRepGraph(const Handle(NCollection_BaseAllocator)& theAlloc)
    : myData(std::make_unique<BRepGraph_Data>(theAlloc))
{
}

//=================================================================================================

BRepGraph::~BRepGraph() = default;

//=================================================================================================

BRepGraph::BRepGraph(BRepGraph&&) noexcept = default;

//=================================================================================================

BRepGraph& BRepGraph::operator=(BRepGraph&&) noexcept = default;

//=================================================================================================

void BRepGraph::Build(const TopoDS_Shape& theShape, bool theParallel)
{
  BRepGraph_Builder::Perform(*this, theShape, theParallel);
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::registerSurface(const Handle(Geom_Surface)&       theSurf,
                                            const Handle(Poly_Triangulation)& theTri,
                                            const TopLoc_Location&            theLoc)
{
  if (theSurf.IsNull())
    return BRepGraph_NodeId();

  const Geom_Surface* aKey       = theSurf.get();
  const int*          anExisting = myData->mySurfRegistry.Seek(aKey);
  if (anExisting != nullptr)
    return BRepGraph_NodeId(BRepGraph_NodeId::Kind::Surface, *anExisting);

  BRepGraph_GeomNode::Surf& aSurfNode = myData->mySurfaces.Appended();
  const int                 aSurfIdx  = myData->mySurfaces.Length() - 1;
  aSurfNode.Id                        = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Surface, aSurfIdx);
  aSurfNode.Surface                   = theSurf;
  aSurfNode.Triangulation             = theTri;
  aSurfNode.SurfaceLocation           = theLoc;
  aSurfNode.IsMultiLocated            = false;
  allocateUID(aSurfNode.Id);

  myData->mySurfRegistry.Add(aKey, aSurfIdx);
  return aSurfNode.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::registerCurve(const Handle(Geom_Curve)& theCrv,
                                          const TopLoc_Location&    theLoc)
{
  if (theCrv.IsNull())
    return BRepGraph_NodeId();

  const Geom_Curve* aKey       = theCrv.get();
  const int*        anExisting = myData->myCurveRegistry.Seek(aKey);
  if (anExisting != nullptr)
    return BRepGraph_NodeId(BRepGraph_NodeId::Kind::Curve, *anExisting);

  BRepGraph_GeomNode::Curve& aCurveNode = myData->myCurves.Appended();
  const int                  aCurveIdx  = myData->myCurves.Length() - 1;
  aCurveNode.Id                         = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Curve, aCurveIdx);
  aCurveNode.CurveGeom                  = theCrv;
  aCurveNode.CurveLocation              = theLoc;
  aCurveNode.IsMultiLocated             = false;
  allocateUID(aCurveNode.Id);

  myData->myCurveRegistry.Add(aKey, aCurveIdx);
  return aCurveNode.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::createPCurveNode(const Handle(Geom2d_Curve)& theCrv2d,
                                             BRepGraph_NodeId            theEdgeDef,
                                             BRepGraph_NodeId            theFaceDef,
                                             double                      theFirst,
                                             double                      theLast,
                                             GeomAbs_Shape               theContinuity)
{
  BRepGraph_GeomNode::PCurve& aNode = myData->myPCurves.Appended();
  const int                   aIdx  = myData->myPCurves.Length() - 1;
  aNode.Id                          = BRepGraph_NodeId(BRepGraph_NodeId::Kind::PCurve, aIdx);
  aNode.Curve2d                     = theCrv2d;
  aNode.EdgeContext                 = theEdgeDef;
  aNode.FaceContext                 = theFaceDef;
  aNode.ParamFirst                  = theFirst;
  aNode.ParamLast                   = theLast;
  aNode.Continuity                  = theContinuity;
  allocateUID(aNode.Id);

  return aNode.Id;
}

//=================================================================================================

BRepGraph_UID BRepGraph::allocateUID(BRepGraph_NodeId theNodeId)
{
  const size_t  aCounter = myData->myNextUIDCounter.fetch_add(1, std::memory_order_relaxed);
  BRepGraph_UID aUID(theNodeId.NodeKind, aCounter, myData->myGeneration);

  // Append to per-kind forward vector (O(1) amortized, no hashing).
  switch (theNodeId.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Solid:     myData->mySolidUIDs.Append(aUID);     break;
    case BRepGraph_NodeId::Kind::Shell:     myData->myShellUIDs.Append(aUID);     break;
    case BRepGraph_NodeId::Kind::Face:      myData->myFaceUIDs.Append(aUID);      break;
    case BRepGraph_NodeId::Kind::Wire:      myData->myWireUIDs.Append(aUID);      break;
    case BRepGraph_NodeId::Kind::Edge:      myData->myEdgeUIDs.Append(aUID);      break;
    case BRepGraph_NodeId::Kind::Vertex:    myData->myVertexUIDs.Append(aUID);    break;
    case BRepGraph_NodeId::Kind::Compound:  myData->myCompoundUIDs.Append(aUID);  break;
    case BRepGraph_NodeId::Kind::CompSolid: myData->myCompSolidUIDs.Append(aUID); break;
    case BRepGraph_NodeId::Kind::Surface:   myData->mySurfaceUIDs.Append(aUID);   break;
    case BRepGraph_NodeId::Kind::Curve:     myData->myCurveUIDs.Append(aUID);     break;
    case BRepGraph_NodeId::Kind::PCurve:    myData->myPCurveUIDs.Append(aUID);    break;
    default: break;
  }

  return aUID;
}

//=================================================================================================

BRepGraph_NodeCache* BRepGraph::mutableCache(BRepGraph_NodeId theNode)
{
  return dispatchDef(theNode, [](auto& theVec, int theIdx) -> BRepGraph_NodeCache* {
    return &theVec.ChangeValue(theIdx).Cache;
  });
}

//=================================================================================================

bool BRepGraph::IsDone() const
{
  return myData->myIsDone;
}

//=================================================================================================

const NCollection_Vector<BRepGraph_UsageId>& BRepGraph::UsagesOf(BRepGraph_NodeId theDefId) const
{
  static const NCollection_Vector<BRepGraph_UsageId> THE_EMPTY;
  const BRepGraph_TopoNode::BaseDef* aDef = TopoDef(theDefId);
  if (aDef == nullptr)
    return THE_EMPTY;
  return aDef->Usages;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::DefOf(BRepGraph_UsageId theUsageId) const
{
  if (!theUsageId.IsValid())
    return BRepGraph_NodeId();

  switch (theUsageId.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Solid:     return myData->mySolidUsages.Value(theUsageId.Index).DefId;
    case BRepGraph_NodeId::Kind::Shell:     return myData->myShellUsages.Value(theUsageId.Index).DefId;
    case BRepGraph_NodeId::Kind::Face:      return myData->myFaceUsages.Value(theUsageId.Index).DefId;
    case BRepGraph_NodeId::Kind::Wire:      return myData->myWireUsages.Value(theUsageId.Index).DefId;
    case BRepGraph_NodeId::Kind::Edge:      return myData->myEdgeUsages.Value(theUsageId.Index).DefId;
    case BRepGraph_NodeId::Kind::Vertex:    return myData->myVertexUsages.Value(theUsageId.Index).DefId;
    case BRepGraph_NodeId::Kind::Compound:  return myData->myCompoundUsages.Value(theUsageId.Index).DefId;
    case BRepGraph_NodeId::Kind::CompSolid: return myData->myCompSolidUsages.Value(theUsageId.Index).DefId;
    default: return BRepGraph_NodeId();
  }
}

//=================================================================================================

const BRepGraph_TopoNode::BaseDef* BRepGraph::TopoDef(BRepGraph_NodeId theId) const
{
  if (!theId.IsValid())
    return nullptr;

  return dispatchDef(theId,
    [](const auto& theVec, int theIdx) -> const BRepGraph_TopoNode::BaseDef* {
      return theIdx < theVec.Length()
               ? static_cast<const BRepGraph_TopoNode::BaseDef*>(&theVec.Value(theIdx))
               : nullptr;
    });
}

//=================================================================================================

void BRepGraph::invalidateSubgraphImpl(BRepGraph_NodeId theNode)
{
  BRepGraph_NodeCache* aCache = mutableCache(theNode);
  if (aCache != nullptr)
    aCache->InvalidateAll();

  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Solid: {
      const BRepGraph_TopoNode::SolidDef& aSolidDef = myData->mySolidDefs.Value(theNode.Index);
      for (int aUsIdx = 0; aUsIdx < aSolidDef.Usages.Length(); ++aUsIdx)
      {
        const BRepGraph_TopoNode::SolidUsage& aUsage =
          myData->mySolidUsages.Value(aSolidDef.Usages.Value(aUsIdx).Index);
        for (int aShellIter = 0; aShellIter < aUsage.ShellUsages.Length(); ++aShellIter)
        {
          BRepGraph_NodeId aShellDefId = DefOf(aUsage.ShellUsages.Value(aShellIter));
          invalidateSubgraphImpl(aShellDefId);
        }
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      const BRepGraph_TopoNode::ShellDef& aShellDef = myData->myShellDefs.Value(theNode.Index);
      for (int aUsIdx = 0; aUsIdx < aShellDef.Usages.Length(); ++aUsIdx)
      {
        const BRepGraph_TopoNode::ShellUsage& aUsage =
          myData->myShellUsages.Value(aShellDef.Usages.Value(aUsIdx).Index);
        for (int aFaceIter = 0; aFaceIter < aUsage.FaceUsages.Length(); ++aFaceIter)
        {
          BRepGraph_NodeId aFaceDefId = DefOf(aUsage.FaceUsages.Value(aFaceIter));
          invalidateSubgraphImpl(aFaceDefId);
        }
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Face: {
      const BRepGraph_TopoNode::FaceDef& aFaceDef = myData->myFaceDefs.Value(theNode.Index);
      for (int aUsIdx = 0; aUsIdx < aFaceDef.Usages.Length(); ++aUsIdx)
      {
        const BRepGraph_TopoNode::FaceUsage& aUsage =
          myData->myFaceUsages.Value(aFaceDef.Usages.Value(aUsIdx).Index);
        if (aUsage.OuterWireUsage.IsValid())
          invalidateSubgraphImpl(DefOf(aUsage.OuterWireUsage));
        for (int aWireIter = 0; aWireIter < aUsage.InnerWireUsages.Length(); ++aWireIter)
          invalidateSubgraphImpl(DefOf(aUsage.InnerWireUsages.Value(aWireIter)));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Wire: {
      const BRepGraph_TopoNode::WireDef& aWireDef = myData->myWireDefs.Value(theNode.Index);
      for (int anEdgeIdx = 0; anEdgeIdx < aWireDef.OrderedEdges.Length(); ++anEdgeIdx)
        invalidateSubgraphImpl(aWireDef.OrderedEdges.Value(anEdgeIdx).EdgeDefId);
      break;
    }
    case BRepGraph_NodeId::Kind::Edge: {
      const BRepGraph_TopoNode::EdgeDef& anEdgeDef = myData->myEdgeDefs.Value(theNode.Index);
      if (anEdgeDef.StartVertexDefId.IsValid())
      {
        BRepGraph_NodeCache* aVtxCache = mutableCache(anEdgeDef.StartVertexDefId);
        if (aVtxCache != nullptr)
          aVtxCache->InvalidateAll();
      }
      if (anEdgeDef.EndVertexDefId.IsValid())
      {
        BRepGraph_NodeCache* aVtxCache = mutableCache(anEdgeDef.EndVertexDefId);
        if (aVtxCache != nullptr)
          aVtxCache->InvalidateAll();
      }
      break;
    }
    default:
      break;
  }
}

//=================================================================================================

void BRepGraph::markModified(BRepGraph_NodeId theDefId)
{
  if (!theDefId.IsValid())
    return;

  const BRepGraph_TopoNode::BaseDef* aDef = TopoDef(theDefId);
  if (aDef == nullptr)
    return;

  const_cast<BRepGraph_TopoNode::BaseDef*>(aDef)->IsModified = true;
  {
    std::unique_lock<std::shared_mutex> aWriteLock(myData->myCurrentShapesMutex);
    myData->myCurrentShapes.UnBind(theDefId);
  }

  // Propagate upward via usages' parent chain.
  for (int aUsIdx = 0; aUsIdx < aDef->Usages.Length(); ++aUsIdx)
  {
    BRepGraph_UsageId aParent;
    switch (theDefId.NodeKind)
    {
      case BRepGraph_NodeId::Kind::Edge: {
        const NCollection_Vector<int>* aWires = myData->myEdgeToWires.Seek(theDefId.Index);
        if (aWires != nullptr)
        {
          for (int aWIdx = 0; aWIdx < aWires->Length(); ++aWIdx)
            markModified(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Wire, aWires->Value(aWIdx)));
        }
        return;
      }
      case BRepGraph_NodeId::Kind::Vertex: {
        return;
      }
      default: {
        const BRepGraph_UsageId& aUsageId = aDef->Usages.Value(aUsIdx);
        switch (aUsageId.NodeKind)
        {
          case BRepGraph_NodeId::Kind::Solid:  aParent = myData->mySolidUsages.Value(aUsageId.Index).ParentUsage; break;
          case BRepGraph_NodeId::Kind::Shell:  aParent = myData->myShellUsages.Value(aUsageId.Index).ParentUsage; break;
          case BRepGraph_NodeId::Kind::Face:   aParent = myData->myFaceUsages.Value(aUsageId.Index).ParentUsage; break;
          case BRepGraph_NodeId::Kind::Wire: {
            const BRepGraph_TopoNode::WireUsage& aWu = myData->myWireUsages.Value(aUsageId.Index);
            if (aWu.OwnerFaceUsage.IsValid())
              markModified(DefOf(aWu.OwnerFaceUsage));
            continue;
          }
          default: continue;
        }
        if (aParent.IsValid())
          markModified(DefOf(aParent));
      }
    }
  }
}

//=================================================================================================

int BRepGraph::NbHistoryRecords() const { return myData->myHistoryLog.NbRecords(); }

const BRepGraph_HistoryRecord& BRepGraph::HistoryRecord(int theIdx) const
{ return myData->myHistoryLog.Record(theIdx); }

BRepGraph_NodeId BRepGraph::FindOriginal(BRepGraph_NodeId theModified) const
{ return myData->myHistoryLog.FindOriginal(theModified); }

NCollection_Vector<BRepGraph_NodeId> BRepGraph::FindDerived(BRepGraph_NodeId theOriginal) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;
  const NCollection_Vector<BRepGraph_NodeId>* aDerived =
    myData->myHistoryLog.myOriginalToDerived.Seek(theOriginal);
  if (aDerived == nullptr)
    return aResult;

  for (int anIdx = 0; anIdx < aDerived->Length(); ++anIdx)
  {
    const BRepGraph_NodeId& aDerivedId = aDerived->Value(anIdx);
    aResult.Append(aDerivedId);
    NCollection_Vector<BRepGraph_NodeId> aFurther = FindDerived(aDerivedId);
    for (int aFurtherIter = 0; aFurtherIter < aFurther.Length(); ++aFurtherIter)
      aResult.Append(aFurther.Value(aFurtherIter));
  }
  return aResult;
}

//=================================================================================================

void BRepGraph::ApplyModification(
  BRepGraph_NodeId                                                                  theTarget,
  std::function<NCollection_Vector<BRepGraph_NodeId>(BRepGraph&, BRepGraph_NodeId)> theModifier,
  const TCollection_AsciiString&                                                    theOpLabel)
{
  NCollection_Vector<BRepGraph_NodeId> aReplacements = theModifier(*this, theTarget);

  myData->myHistoryLog.Record(theOpLabel, theTarget, aReplacements);

  invalidateSubgraphImpl(theTarget);
}

//=================================================================================================

void BRepGraph::RecordHistory(const TCollection_AsciiString&              theOpLabel,
                              BRepGraph_NodeId                            theOriginal,
                              const NCollection_Vector<BRepGraph_NodeId>& theReplacements)
{
  myData->myHistoryLog.Record(theOpLabel, theOriginal, theReplacements);
}

//=================================================================================================

void BRepGraph::SetAllocator(const Handle(NCollection_BaseAllocator)& theAlloc)
{
  myData->myAllocator = !theAlloc.IsNull() ? theAlloc : NCollection_BaseAllocator::CommonBaseAllocator();

  myData->mySolidDefs       = NCollection_Vector<BRepGraph_TopoNode::SolidDef>(16, myData->myAllocator);
  myData->myShellDefs       = NCollection_Vector<BRepGraph_TopoNode::ShellDef>(16, myData->myAllocator);
  myData->myFaceDefs        = NCollection_Vector<BRepGraph_TopoNode::FaceDef>(128, myData->myAllocator);
  myData->myWireDefs        = NCollection_Vector<BRepGraph_TopoNode::WireDef>(128, myData->myAllocator);
  myData->myEdgeDefs        = NCollection_Vector<BRepGraph_TopoNode::EdgeDef>(256, myData->myAllocator);
  myData->myVertexDefs      = NCollection_Vector<BRepGraph_TopoNode::VertexDef>(256, myData->myAllocator);
  myData->myCompoundDefs    = NCollection_Vector<BRepGraph_TopoNode::CompoundDef>(8, myData->myAllocator);
  myData->myCompSolidDefs   = NCollection_Vector<BRepGraph_TopoNode::CompSolidDef>(8, myData->myAllocator);
  myData->mySolidUsages     = NCollection_Vector<BRepGraph_TopoNode::SolidUsage>(16, myData->myAllocator);
  myData->myShellUsages     = NCollection_Vector<BRepGraph_TopoNode::ShellUsage>(16, myData->myAllocator);
  myData->myFaceUsages      = NCollection_Vector<BRepGraph_TopoNode::FaceUsage>(128, myData->myAllocator);
  myData->myWireUsages      = NCollection_Vector<BRepGraph_TopoNode::WireUsage>(128, myData->myAllocator);
  myData->myEdgeUsages      = NCollection_Vector<BRepGraph_TopoNode::EdgeUsage>(256, myData->myAllocator);
  myData->myVertexUsages    = NCollection_Vector<BRepGraph_TopoNode::VertexUsage>(256, myData->myAllocator);
  myData->myCompoundUsages  = NCollection_Vector<BRepGraph_TopoNode::CompoundUsage>(8, myData->myAllocator);
  myData->myCompSolidUsages = NCollection_Vector<BRepGraph_TopoNode::CompSolidUsage>(8, myData->myAllocator);
  myData->mySurfaces       = NCollection_Vector<BRepGraph_GeomNode::Surf>(64, myData->myAllocator);
  myData->myCurves         = NCollection_Vector<BRepGraph_GeomNode::Curve>(64, myData->myAllocator);
  myData->myPCurves        = NCollection_Vector<BRepGraph_GeomNode::PCurve>(128, myData->myAllocator);
  myData->mySurfRegistry   = NCollection_IndexedDataMap<const Geom_Surface*, int>(100, myData->myAllocator);
  myData->myCurveRegistry  = NCollection_IndexedDataMap<const Geom_Curve*, int>(100, myData->myAllocator);
  myData->myTShapeToDefId  = NCollection_DataMap<const TopoDS_TShape*, BRepGraph_NodeId>(100, myData->myAllocator);
  myData->mySolidUIDs      = NCollection_Vector<BRepGraph_UID>(16, myData->myAllocator);
  myData->myShellUIDs      = NCollection_Vector<BRepGraph_UID>(16, myData->myAllocator);
  myData->myFaceUIDs       = NCollection_Vector<BRepGraph_UID>(128, myData->myAllocator);
  myData->myWireUIDs       = NCollection_Vector<BRepGraph_UID>(128, myData->myAllocator);
  myData->myEdgeUIDs       = NCollection_Vector<BRepGraph_UID>(256, myData->myAllocator);
  myData->myVertexUIDs     = NCollection_Vector<BRepGraph_UID>(256, myData->myAllocator);
  myData->myCompoundUIDs   = NCollection_Vector<BRepGraph_UID>(16, myData->myAllocator);
  myData->myCompSolidUIDs  = NCollection_Vector<BRepGraph_UID>(16, myData->myAllocator);
  myData->mySurfaceUIDs    = NCollection_Vector<BRepGraph_UID>(64, myData->myAllocator);
  myData->myCurveUIDs      = NCollection_Vector<BRepGraph_UID>(64, myData->myAllocator);
  myData->myPCurveUIDs     = NCollection_Vector<BRepGraph_UID>(128, myData->myAllocator);
  myData->myEdgeToWires = NCollection_DataMap<int, NCollection_Vector<int>>(100, myData->myAllocator);
}

const Handle(NCollection_BaseAllocator)& BRepGraph::Allocator() const { return myData->myAllocator; }
void BRepGraph::SetHistoryEnabled(bool theVal) { myData->myHistoryLog.SetEnabled(theVal); }
bool BRepGraph::IsHistoryEnabled() const { return myData->myHistoryLog.IsEnabled(); }

BRepGraph_History& BRepGraph::History() { return myData->myHistoryLog; }
const BRepGraph_History& BRepGraph::History() const { return myData->myHistoryLog; }
