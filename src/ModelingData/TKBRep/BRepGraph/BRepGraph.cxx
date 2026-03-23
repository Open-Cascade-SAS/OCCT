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
// dispatchDef template implementations.
// Defs is a reference to incidence storage, so this accesses incidence data directly.

template <typename Func>
auto BRepGraph::dispatchDef(BRepGraph_NodeId theNode, Func&& theFunc) const
  -> decltype(theFunc(std::declval<const NCollection_Vector<BRepGraph_TopoNode::SolidDef>&>(), 0))
{
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Solid:     return theFunc(myData->mySolids.Defs, theNode.Index);
    case BRepGraph_NodeId::Kind::Shell:     return theFunc(myData->myShells.Defs, theNode.Index);
    case BRepGraph_NodeId::Kind::Face:      return theFunc(myData->myFaces.Defs, theNode.Index);
    case BRepGraph_NodeId::Kind::Wire:      return theFunc(myData->myWires.Defs, theNode.Index);
    case BRepGraph_NodeId::Kind::Edge:      return theFunc(myData->myEdges.Defs, theNode.Index);
    case BRepGraph_NodeId::Kind::Vertex:    return theFunc(myData->myVertices.Defs, theNode.Index);
    case BRepGraph_NodeId::Kind::Compound:  return theFunc(myData->myCompounds.Defs, theNode.Index);
    case BRepGraph_NodeId::Kind::CompSolid: return theFunc(myData->myCompSolids.Defs, theNode.Index);
    default: return decltype(theFunc(myData->mySolids.Defs, 0)){};
  }
}

template <typename Func>
auto BRepGraph::dispatchDef(BRepGraph_NodeId theNode, Func&& theFunc)
  -> decltype(theFunc(std::declval<NCollection_Vector<BRepGraph_TopoNode::SolidDef>&>(), 0))
{
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Solid:     return theFunc(myData->mySolids.Defs, theNode.Index);
    case BRepGraph_NodeId::Kind::Shell:     return theFunc(myData->myShells.Defs, theNode.Index);
    case BRepGraph_NodeId::Kind::Face:      return theFunc(myData->myFaces.Defs, theNode.Index);
    case BRepGraph_NodeId::Kind::Wire:      return theFunc(myData->myWires.Defs, theNode.Index);
    case BRepGraph_NodeId::Kind::Edge:      return theFunc(myData->myEdges.Defs, theNode.Index);
    case BRepGraph_NodeId::Kind::Vertex:    return theFunc(myData->myVertices.Defs, theNode.Index);
    case BRepGraph_NodeId::Kind::Compound:  return theFunc(myData->myCompounds.Defs, theNode.Index);
    case BRepGraph_NodeId::Kind::CompSolid: return theFunc(myData->myCompSolids.Defs, theNode.Index);
    default: return decltype(theFunc(myData->mySolids.Defs, 0)){};
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

BRepGraph_UID BRepGraph::allocateUID(BRepGraph_NodeId theNodeId)
{
  const size_t  aCounter = myData->myNextUIDCounter.fetch_add(1, std::memory_order_relaxed);
  BRepGraph_UID aUID(theNodeId.NodeKind, aCounter, myData->myGeneration);

  // Append to per-kind forward vector (O(1) amortized, no hashing).
  switch (theNodeId.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Solid:     myData->mySolids.UIDs.Append(aUID);     break;
    case BRepGraph_NodeId::Kind::Shell:     myData->myShells.UIDs.Append(aUID);     break;
    case BRepGraph_NodeId::Kind::Face:      myData->myFaces.UIDs.Append(aUID);      break;
    case BRepGraph_NodeId::Kind::Wire:      myData->myWires.UIDs.Append(aUID);      break;
    case BRepGraph_NodeId::Kind::Edge:      myData->myEdges.UIDs.Append(aUID);      break;
    case BRepGraph_NodeId::Kind::Vertex:    myData->myVertices.UIDs.Append(aUID);    break;
    case BRepGraph_NodeId::Kind::Compound:  myData->myCompounds.UIDs.Append(aUID);  break;
    case BRepGraph_NodeId::Kind::CompSolid: myData->myCompSolids.UIDs.Append(aUID); break;
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
    case BRepGraph_NodeId::Kind::Solid:     return myData->mySolids.Usages.Value(theUsageId.Index).DefId;
    case BRepGraph_NodeId::Kind::Shell:     return myData->myShells.Usages.Value(theUsageId.Index).DefId;
    case BRepGraph_NodeId::Kind::Face:      return myData->myFaces.Usages.Value(theUsageId.Index).DefId;
    case BRepGraph_NodeId::Kind::Wire:      return myData->myWires.Usages.Value(theUsageId.Index).DefId;
    case BRepGraph_NodeId::Kind::Edge:      return myData->myEdges.Usages.Value(theUsageId.Index).DefId;
    case BRepGraph_NodeId::Kind::Vertex:    return myData->myVertices.Usages.Value(theUsageId.Index).DefId;
    case BRepGraph_NodeId::Kind::Compound:  return myData->myCompounds.Usages.Value(theUsageId.Index).DefId;
    case BRepGraph_NodeId::Kind::CompSolid: return myData->myCompSolids.Usages.Value(theUsageId.Index).DefId;
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
      const BRepGraph_TopoNode::SolidDef& aSolidDef = myData->mySolids.Defs.Value(theNode.Index);
      for (int aUsIdx = 0; aUsIdx < aSolidDef.Usages.Length(); ++aUsIdx)
      {
        const BRepGraph_TopoNode::SolidUsage& aUsage =
          myData->mySolids.Usages.Value(aSolidDef.Usages.Value(aUsIdx).Index);
        for (int aShellIter = 0; aShellIter < aUsage.ShellUsages.Length(); ++aShellIter)
        {
          BRepGraph_NodeId aShellDefId = DefOf(aUsage.ShellUsages.Value(aShellIter));
          invalidateSubgraphImpl(aShellDefId);
        }
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      const BRepGraph_TopoNode::ShellDef& aShellDef = myData->myShells.Defs.Value(theNode.Index);
      for (int aUsIdx = 0; aUsIdx < aShellDef.Usages.Length(); ++aUsIdx)
      {
        const BRepGraph_TopoNode::ShellUsage& aUsage =
          myData->myShells.Usages.Value(aShellDef.Usages.Value(aUsIdx).Index);
        for (int aFaceIter = 0; aFaceIter < aUsage.FaceUsages.Length(); ++aFaceIter)
        {
          BRepGraph_NodeId aFaceDefId = DefOf(aUsage.FaceUsages.Value(aFaceIter));
          invalidateSubgraphImpl(aFaceDefId);
        }
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Face: {
      const BRepGraph_TopoNode::FaceDef& aFaceDef = myData->myFaces.Defs.Value(theNode.Index);
      for (int aUsIdx = 0; aUsIdx < aFaceDef.Usages.Length(); ++aUsIdx)
      {
        const BRepGraph_TopoNode::FaceUsage& aUsage =
          myData->myFaces.Usages.Value(aFaceDef.Usages.Value(aUsIdx).Index);
        if (aUsage.OuterWireUsage.IsValid())
          invalidateSubgraphImpl(DefOf(aUsage.OuterWireUsage));
        for (int aWireIter = 0; aWireIter < aUsage.InnerWireUsages.Length(); ++aWireIter)
          invalidateSubgraphImpl(DefOf(aUsage.InnerWireUsages.Value(aWireIter)));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Wire: {
      const BRepGraph_TopoNode::WireDef& aWireDef = myData->myWires.Defs.Value(theNode.Index);
      if (!aWireDef.Usages.IsEmpty())
      {
        const BRepGraph_TopoNode::WireUsage& aWireUsage =
          myData->myWires.Usages.Value(aWireDef.Usages.Value(0).Index);
        for (int anEdgeIdx = 0; anEdgeIdx < aWireUsage.EdgeUsages.Length(); ++anEdgeIdx)
          invalidateSubgraphImpl(DefOf(aWireUsage.EdgeUsages.Value(anEdgeIdx)));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Edge: {
      const BRepGraph_TopoNode::EdgeDef& anEdgeDef = myData->myEdges.Defs.Value(theNode.Index);
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
          case BRepGraph_NodeId::Kind::Solid:  aParent = myData->mySolids.Usages.Value(aUsageId.Index).ParentUsage; break;
          case BRepGraph_NodeId::Kind::Shell:  aParent = myData->myShells.Usages.Value(aUsageId.Index).ParentUsage; break;
          case BRepGraph_NodeId::Kind::Face:   aParent = myData->myFaces.Usages.Value(aUsageId.Index).ParentUsage; break;
          case BRepGraph_NodeId::Kind::Wire: {
            const BRepGraph_TopoNode::WireUsage& aWu = myData->myWires.Usages.Value(aUsageId.Index);
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

  // Recreate the entire data object with the new allocator.
  // TopoKindData has reference members that can't be reassigned.
  myData = std::make_unique<BRepGraph_Data>(myData->myAllocator);
}

const Handle(NCollection_BaseAllocator)& BRepGraph::Allocator() const { return myData->myAllocator; }
void BRepGraph::SetHistoryEnabled(bool theVal) { myData->myHistoryLog.SetEnabled(theVal); }
bool BRepGraph::IsHistoryEnabled() const { return myData->myHistoryLog.IsEnabled(); }

BRepGraph_History& BRepGraph::History() { return myData->myHistoryLog; }
const BRepGraph_History& BRepGraph::History() const { return myData->myHistoryLog; }
