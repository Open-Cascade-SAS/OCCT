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
#include <BRepGraphInc_ReverseIndex.hxx>

#include <NCollection_Map.hxx>
#include <Standard_ProgramError.hxx>

#include <shared_mutex>

//=================================================================================================
// dispatchDef template implementations.
// Accesses incidence storage entity vectors directly.

template <typename Func>
auto BRepGraph::dispatchDef(BRepGraph_NodeId theNode, Func&& theFunc) const
  -> decltype(theFunc(std::declval<const NCollection_Vector<BRepGraph_TopoNode::SolidDef>&>(), 0))
{
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Solid:     return theFunc(myData->myIncStorage.Solids, theNode.Index);
    case BRepGraph_NodeId::Kind::Shell:     return theFunc(myData->myIncStorage.Shells, theNode.Index);
    case BRepGraph_NodeId::Kind::Face:      return theFunc(myData->myIncStorage.Faces, theNode.Index);
    case BRepGraph_NodeId::Kind::Wire:      return theFunc(myData->myIncStorage.Wires, theNode.Index);
    case BRepGraph_NodeId::Kind::Edge:      return theFunc(myData->myIncStorage.Edges, theNode.Index);
    case BRepGraph_NodeId::Kind::Vertex:    return theFunc(myData->myIncStorage.Vertices, theNode.Index);
    case BRepGraph_NodeId::Kind::Compound:  return theFunc(myData->myIncStorage.Compounds, theNode.Index);
    case BRepGraph_NodeId::Kind::CompSolid: return theFunc(myData->myIncStorage.CompSolids, theNode.Index);
    default: return decltype(theFunc(myData->myIncStorage.Solids, 0)){};
  }
}

template <typename Func>
auto BRepGraph::dispatchDef(BRepGraph_NodeId theNode, Func&& theFunc)
  -> decltype(theFunc(std::declval<NCollection_Vector<BRepGraph_TopoNode::SolidDef>&>(), 0))
{
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Solid:     return theFunc(myData->myIncStorage.Solids, theNode.Index);
    case BRepGraph_NodeId::Kind::Shell:     return theFunc(myData->myIncStorage.Shells, theNode.Index);
    case BRepGraph_NodeId::Kind::Face:      return theFunc(myData->myIncStorage.Faces, theNode.Index);
    case BRepGraph_NodeId::Kind::Wire:      return theFunc(myData->myIncStorage.Wires, theNode.Index);
    case BRepGraph_NodeId::Kind::Edge:      return theFunc(myData->myIncStorage.Edges, theNode.Index);
    case BRepGraph_NodeId::Kind::Vertex:    return theFunc(myData->myIncStorage.Vertices, theNode.Index);
    case BRepGraph_NodeId::Kind::Compound:  return theFunc(myData->myIncStorage.Compounds, theNode.Index);
    case BRepGraph_NodeId::Kind::CompSolid: return theFunc(myData->myIncStorage.CompSolids, theNode.Index);
    default: return decltype(theFunc(myData->myIncStorage.Solids, 0)){};
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

  // Append to per-kind forward vector in Storage (O(1) amortized).
  BRepGraphInc_Storage& aStorage = myData->myIncStorage;
  switch (theNodeId.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Solid:     aStorage.SolidUIDs.Append(aUID);     break;
    case BRepGraph_NodeId::Kind::Shell:     aStorage.ShellUIDs.Append(aUID);     break;
    case BRepGraph_NodeId::Kind::Face:      aStorage.FaceUIDs.Append(aUID);      break;
    case BRepGraph_NodeId::Kind::Wire:      aStorage.WireUIDs.Append(aUID);      break;
    case BRepGraph_NodeId::Kind::Edge:      aStorage.EdgeUIDs.Append(aUID);      break;
    case BRepGraph_NodeId::Kind::Vertex:    aStorage.VertexUIDs.Append(aUID);    break;
    case BRepGraph_NodeId::Kind::Compound:  aStorage.CompoundUIDs.Append(aUID);  break;
    case BRepGraph_NodeId::Kind::CompSolid: aStorage.CompSolidUIDs.Append(aUID); break;
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

  // Forward traversal via incidence refs.
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Solid: {
      const BRepGraph_TopoNode::SolidDef& aSolidDef = myData->myIncStorage.Solids.Value(theNode.Index);
      for (int s = 0; s < aSolidDef.ShellRefs.Length(); ++s)
        invalidateSubgraphImpl(BRepGraph_NodeId::Shell(aSolidDef.ShellRefs.Value(s).ShellIdx));
      break;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      const BRepGraph_TopoNode::ShellDef& aShellDef = myData->myIncStorage.Shells.Value(theNode.Index);
      for (int f = 0; f < aShellDef.FaceRefs.Length(); ++f)
        invalidateSubgraphImpl(BRepGraph_NodeId::Face(aShellDef.FaceRefs.Value(f).FaceIdx));
      break;
    }
    case BRepGraph_NodeId::Kind::Face: {
      const BRepGraph_TopoNode::FaceDef& aFaceDef = myData->myIncStorage.Faces.Value(theNode.Index);
      for (int w = 0; w < aFaceDef.WireRefs.Length(); ++w)
        invalidateSubgraphImpl(BRepGraph_NodeId::Wire(aFaceDef.WireRefs.Value(w).WireIdx));
      break;
    }
    case BRepGraph_NodeId::Kind::Wire: {
      const BRepGraph_TopoNode::WireDef& aWireDef = myData->myIncStorage.Wires.Value(theNode.Index);
      for (int e = 0; e < aWireDef.EdgeRefs.Length(); ++e)
        invalidateSubgraphImpl(BRepGraph_NodeId::Edge(aWireDef.EdgeRefs.Value(e).EdgeIdx));
      break;
    }
    case BRepGraph_NodeId::Kind::Edge: {
      const BRepGraph_TopoNode::EdgeDef& anEdgeDef = myData->myIncStorage.Edges.Value(theNode.Index);
      if (anEdgeDef.StartVertexDefId().IsValid())
      {
        BRepGraph_NodeCache* aVtxCache = mutableCache(anEdgeDef.StartVertexDefId());
        if (aVtxCache != nullptr)
          aVtxCache->InvalidateAll();
      }
      if (anEdgeDef.EndVertexDefId().IsValid())
      {
        BRepGraph_NodeCache* aVtxCache = mutableCache(anEdgeDef.EndVertexDefId());
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

  // Propagate upward via reverse indices.
  const BRepGraphInc_ReverseIndex& aRevIdx = myData->myIncStorage.ReverseIdx;
  switch (theDefId.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      // Vertex modifications don't propagate.
      break;
    case BRepGraph_NodeId::Kind::Edge: {
      // Edge -> Wire (via incidence reverse index).
      const NCollection_Vector<int>* aWires = aRevIdx.WiresOfEdge(theDefId.Index);
      if (aWires != nullptr)
        for (int i = 0; i < aWires->Length(); ++i)
          markModified(BRepGraph_NodeId::Wire(aWires->Value(i)));
      break;
    }
    case BRepGraph_NodeId::Kind::Wire: {
      // Wire -> Face (via wire-to-face reverse index).
      const NCollection_Vector<int>* aFaces = aRevIdx.FacesOfWire(theDefId.Index);
      if (aFaces != nullptr)
        for (int i = 0; i < aFaces->Length(); ++i)
          markModified(BRepGraph_NodeId::Face(aFaces->Value(i)));
      break;
    }
    case BRepGraph_NodeId::Kind::Face: {
      // Face -> Shell (via face-to-shell reverse index).
      const NCollection_Vector<int>* aShells = aRevIdx.ShellsOfFace(theDefId.Index);
      if (aShells != nullptr)
        for (int i = 0; i < aShells->Length(); ++i)
          markModified(BRepGraph_NodeId::Shell(aShells->Value(i)));
      break;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      // Shell -> Solid (via shell-to-solid reverse index).
      const NCollection_Vector<int>* aSolids = aRevIdx.SolidsOfShell(theDefId.Index);
      if (aSolids != nullptr)
        for (int i = 0; i < aSolids->Length(); ++i)
          markModified(BRepGraph_NodeId::Solid(aSolids->Value(i)));
      break;
    }
    default:
      // Solid/Compound/CompSolid modifications don't propagate further.
      break;
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
  myData = std::make_unique<BRepGraph_Data>(myData->myAllocator);
}

const Handle(NCollection_BaseAllocator)& BRepGraph::Allocator() const { return myData->myAllocator; }
void BRepGraph::SetHistoryEnabled(bool theVal) { myData->myHistoryLog.SetEnabled(theVal); }
bool BRepGraph::IsHistoryEnabled() const { return myData->myHistoryLog.IsEnabled(); }

BRepGraph_History& BRepGraph::History() { return myData->myHistoryLog; }
const BRepGraph_History& BRepGraph::History() const { return myData->myHistoryLog; }
