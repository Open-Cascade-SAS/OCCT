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

void BRepGraph::Build(const TopoDS_Shape&                   theShape,
                      bool                                  theParallel,
                      const BRepGraphInc_Populate::Options& theOptions)
{
  BRepGraph_Builder::Perform(*this, theShape, theParallel, theOptions);
}

//=================================================================================================

BRepGraph_UID BRepGraph::allocateUID(BRepGraph_NodeId theNodeId)
{
  const size_t  aCounter = myData->myNextUIDCounter.fetch_add(1, std::memory_order_relaxed);
  BRepGraph_UID aUID(theNodeId.NodeKind, aCounter, myData->myGeneration);
  myData->myIncStorage.ChangeUIDs(theNodeId.NodeKind).Append(aUID);
  return aUID;
}

//=================================================================================================

BRepGraph_NodeCache* BRepGraph::mutableCache(BRepGraph_NodeId theNode)
{
  BRepGraphInc_Storage& aStorage = myData->myIncStorage;
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Solid:     return &aStorage.ChangeSolid(theNode.Index).Cache;
    case BRepGraph_NodeId::Kind::Shell:     return &aStorage.ChangeShell(theNode.Index).Cache;
    case BRepGraph_NodeId::Kind::Face:      return &aStorage.ChangeFace(theNode.Index).Cache;
    case BRepGraph_NodeId::Kind::Wire:      return &aStorage.ChangeWire(theNode.Index).Cache;
    case BRepGraph_NodeId::Kind::Edge:      return &aStorage.ChangeEdge(theNode.Index).Cache;
    case BRepGraph_NodeId::Kind::Vertex:    return &aStorage.ChangeVertex(theNode.Index).Cache;
    case BRepGraph_NodeId::Kind::Compound:  return &aStorage.ChangeCompound(theNode.Index).Cache;
    case BRepGraph_NodeId::Kind::CompSolid: return &aStorage.ChangeCompSolid(theNode.Index).Cache;
    default: return nullptr;
  }
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
  const BRepGraphInc_Storage& aStorage = myData->myIncStorage;
  switch (theId.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Solid:
      return theId.Index < aStorage.NbSolids() ? &aStorage.Solid(theId.Index) : nullptr;
    case BRepGraph_NodeId::Kind::Shell:
      return theId.Index < aStorage.NbShells() ? &aStorage.Shell(theId.Index) : nullptr;
    case BRepGraph_NodeId::Kind::Face:
      return theId.Index < aStorage.NbFaces() ? &aStorage.Face(theId.Index) : nullptr;
    case BRepGraph_NodeId::Kind::Wire:
      return theId.Index < aStorage.NbWires() ? &aStorage.Wire(theId.Index) : nullptr;
    case BRepGraph_NodeId::Kind::Edge:
      return theId.Index < aStorage.NbEdges() ? &aStorage.Edge(theId.Index) : nullptr;
    case BRepGraph_NodeId::Kind::Vertex:
      return theId.Index < aStorage.NbVertices() ? &aStorage.Vertex(theId.Index) : nullptr;
    case BRepGraph_NodeId::Kind::Compound:
      return theId.Index < aStorage.NbCompounds() ? &aStorage.Compound(theId.Index) : nullptr;
    case BRepGraph_NodeId::Kind::CompSolid:
      return theId.Index < aStorage.NbCompSolids() ? &aStorage.CompSolid(theId.Index) : nullptr;
    default: return nullptr;
  }
}

//=================================================================================================

BRepGraph_TopoNode::BaseDef* BRepGraph::ChangeTopoDef(BRepGraph_NodeId theId)
{
  if (!theId.IsValid())
    return nullptr;
  BRepGraphInc_Storage& aStorage = myData->myIncStorage;
  switch (theId.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Solid:
      return theId.Index < aStorage.NbSolids() ? &aStorage.ChangeSolid(theId.Index) : nullptr;
    case BRepGraph_NodeId::Kind::Shell:
      return theId.Index < aStorage.NbShells() ? &aStorage.ChangeShell(theId.Index) : nullptr;
    case BRepGraph_NodeId::Kind::Face:
      return theId.Index < aStorage.NbFaces() ? &aStorage.ChangeFace(theId.Index) : nullptr;
    case BRepGraph_NodeId::Kind::Wire:
      return theId.Index < aStorage.NbWires() ? &aStorage.ChangeWire(theId.Index) : nullptr;
    case BRepGraph_NodeId::Kind::Edge:
      return theId.Index < aStorage.NbEdges() ? &aStorage.ChangeEdge(theId.Index) : nullptr;
    case BRepGraph_NodeId::Kind::Vertex:
      return theId.Index < aStorage.NbVertices() ? &aStorage.ChangeVertex(theId.Index) : nullptr;
    case BRepGraph_NodeId::Kind::Compound:
      return theId.Index < aStorage.NbCompounds() ? &aStorage.ChangeCompound(theId.Index) : nullptr;
    case BRepGraph_NodeId::Kind::CompSolid:
      return theId.Index < aStorage.NbCompSolids() ? &aStorage.ChangeCompSolid(theId.Index) : nullptr;
    default: return nullptr;
  }
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
      const BRepGraph_TopoNode::SolidDef& aSolidDef = myData->myIncStorage.Solid(theNode.Index);
      for (int s = 0; s < aSolidDef.ShellRefs.Length(); ++s)
        invalidateSubgraphImpl(BRepGraph_NodeId::Shell(aSolidDef.ShellRefs.Value(s).ShellIdx));
      break;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      const BRepGraph_TopoNode::ShellDef& aShellDef = myData->myIncStorage.Shell(theNode.Index);
      for (int f = 0; f < aShellDef.FaceRefs.Length(); ++f)
        invalidateSubgraphImpl(BRepGraph_NodeId::Face(aShellDef.FaceRefs.Value(f).FaceIdx));
      break;
    }
    case BRepGraph_NodeId::Kind::Face: {
      const BRepGraph_TopoNode::FaceDef& aFaceDef = myData->myIncStorage.Face(theNode.Index);
      for (int w = 0; w < aFaceDef.WireRefs.Length(); ++w)
        invalidateSubgraphImpl(BRepGraph_NodeId::Wire(aFaceDef.WireRefs.Value(w).WireIdx));
      break;
    }
    case BRepGraph_NodeId::Kind::Wire: {
      const BRepGraph_TopoNode::WireDef& aWireDef = myData->myIncStorage.Wire(theNode.Index);
      for (int e = 0; e < aWireDef.EdgeRefs.Length(); ++e)
        invalidateSubgraphImpl(BRepGraph_NodeId::Edge(aWireDef.EdgeRefs.Value(e).EdgeIdx));
      break;
    }
    case BRepGraph_NodeId::Kind::Edge: {
      const BRepGraph_TopoNode::EdgeDef& anEdgeDef = myData->myIncStorage.Edge(theNode.Index);
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

  BRepGraph_TopoNode::BaseDef* aDef = ChangeTopoDef(theDefId);
  if (aDef == nullptr)
    return;

  aDef->IsModified = true;

  // In deferred mode: skip mutex and upward propagation.
  if (myData->myDeferredMode)
    return;

  {
    std::unique_lock<std::shared_mutex> aWriteLock(myData->myCurrentShapesMutex);
    myData->myCurrentShapes.UnBind(theDefId);
  }

  // Propagate upward via reverse indices.
  const BRepGraphInc_ReverseIndex& aRevIdx = myData->myIncStorage.ReverseIndex();
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
  return myData->myHistoryLog.FindDerived(theOriginal);
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

void BRepGraph::BeginDeferredInvalidation()
{
  myData->myDeferredMode = true;
}

//=================================================================================================

void BRepGraph::EndDeferredInvalidation()
{
  if (!myData->myDeferredMode)
    return;

  myData->myDeferredMode = false;

  // Bulk-clear all cached shapes. Safe because IsModified flags are already
  // set on mutated entities — reconstruction will recompute as needed.
  {
    std::unique_lock<std::shared_mutex> aWriteLock(myData->myCurrentShapesMutex);
    myData->myCurrentShapes.Clear();
  }

  // Propagate IsModified upward for all modified entities.
  // Single iterative pass per kind: Edge→Wire→Face→Shell→Solid.
  // Skips already-propagated nodes: O(modified) not O(total).
  const BRepGraphInc_ReverseIndex& aRevIdx  = myData->myIncStorage.ReverseIndex();
  const int                        aNbEdges = myData->myIncStorage.NbEdges();
  for (int anEdgeIdx = 0; anEdgeIdx < aNbEdges; ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = myData->myIncStorage.Edge(anEdgeIdx);
    if (anEdge.IsRemoved || !anEdge.IsModified)
      continue;

    const NCollection_Vector<int>* aWires = aRevIdx.WiresOfEdge(anEdgeIdx);
    if (aWires == nullptr)
      continue;

    for (int aWireIter = 0; aWireIter < aWires->Length(); ++aWireIter)
    {
      const int aWireIdx = aWires->Value(aWireIter);
      BRepGraph_TopoNode::WireDef& aWireDef = myData->myIncStorage.ChangeWire(aWireIdx);
      if (aWireDef.IsModified)
        continue;
      aWireDef.IsModified = true;

      const NCollection_Vector<int>* aFaces = aRevIdx.FacesOfWire(aWireIdx);
      if (aFaces == nullptr)
        continue;

      for (int aFaceIter = 0; aFaceIter < aFaces->Length(); ++aFaceIter)
      {
        const int aFaceIdx = aFaces->Value(aFaceIter);
        BRepGraph_TopoNode::FaceDef& aFaceDef = myData->myIncStorage.ChangeFace(aFaceIdx);
        if (aFaceDef.IsModified)
          continue;
        aFaceDef.IsModified = true;

        const NCollection_Vector<int>* aShells = aRevIdx.ShellsOfFace(aFaceIdx);
        if (aShells == nullptr)
          continue;

        for (int aShellIter = 0; aShellIter < aShells->Length(); ++aShellIter)
        {
          const int aShellIdx = aShells->Value(aShellIter);
          BRepGraph_TopoNode::ShellDef& aShellDef = myData->myIncStorage.ChangeShell(aShellIdx);
          if (aShellDef.IsModified)
            continue;
          aShellDef.IsModified = true;

          const NCollection_Vector<int>* aSolids = aRevIdx.SolidsOfShell(aShellIdx);
          if (aSolids == nullptr)
            continue;

          for (int aSolidIter = 0; aSolidIter < aSolids->Length(); ++aSolidIter)
          {
            myData->myIncStorage.ChangeSolid(aSolids->Value(aSolidIter)).IsModified = true;
          }
        }
      }
    }
  }

  // Propagate for directly modified wires (not reached via edges above).
  const int aNbWires = myData->myIncStorage.NbWires();
  for (int aWireIdx = 0; aWireIdx < aNbWires; ++aWireIdx)
  {
    if (!myData->myIncStorage.Wire(aWireIdx).IsModified)
      continue;

    const NCollection_Vector<int>* aFaces = aRevIdx.FacesOfWire(aWireIdx);
    if (aFaces == nullptr)
      continue;

    for (int aFaceIter = 0; aFaceIter < aFaces->Length(); ++aFaceIter)
    {
      const int aFaceIdx = aFaces->Value(aFaceIter);
      auto& aFaceDef = myData->myIncStorage.ChangeFace(aFaceIdx);
      if (aFaceDef.IsModified)
        continue;
      aFaceDef.IsModified = true;

      const NCollection_Vector<int>* aShells = aRevIdx.ShellsOfFace(aFaceIdx);
      if (aShells == nullptr)
        continue;

      for (int aShellIter = 0; aShellIter < aShells->Length(); ++aShellIter)
      {
        const int aShellIdx = aShells->Value(aShellIter);
        auto& aShellDef = myData->myIncStorage.ChangeShell(aShellIdx);
        if (aShellDef.IsModified)
          continue;
        aShellDef.IsModified = true;

        const NCollection_Vector<int>* aSolids = aRevIdx.SolidsOfShell(aShellIdx);
        if (aSolids == nullptr)
          continue;

        for (int aSolidIter = 0; aSolidIter < aSolids->Length(); ++aSolidIter)
        {
          myData->myIncStorage.ChangeSolid(aSolids->Value(aSolidIter)).IsModified = true;
        }
      }
    }
  }

  // Propagate for directly modified faces (not reached via wires above).
  const int aNbFaces = myData->myIncStorage.NbFaces();
  for (int aFaceIdx = 0; aFaceIdx < aNbFaces; ++aFaceIdx)
  {
    if (!myData->myIncStorage.Face(aFaceIdx).IsModified)
      continue;

    const NCollection_Vector<int>* aShells = aRevIdx.ShellsOfFace(aFaceIdx);
    if (aShells == nullptr)
      continue;

    for (int aShellIter = 0; aShellIter < aShells->Length(); ++aShellIter)
    {
      const int aShellIdx = aShells->Value(aShellIter);
      auto& aShellDef = myData->myIncStorage.ChangeShell(aShellIdx);
      if (aShellDef.IsModified)
        continue;
      aShellDef.IsModified = true;

      const NCollection_Vector<int>* aSolids = aRevIdx.SolidsOfShell(aShellIdx);
      if (aSolids == nullptr)
        continue;

      for (int aSolidIter = 0; aSolidIter < aSolids->Length(); ++aSolidIter)
      {
        myData->myIncStorage.ChangeSolid(aSolids->Value(aSolidIter)).IsModified = true;
      }
    }
  }

  // Propagate for directly modified shells (not reached via faces above).
  const int aNbShells = myData->myIncStorage.NbShells();
  for (int aShellIdx = 0; aShellIdx < aNbShells; ++aShellIdx)
  {
    if (!myData->myIncStorage.Shell(aShellIdx).IsModified)
      continue;

    const NCollection_Vector<int>* aSolids = aRevIdx.SolidsOfShell(aShellIdx);
    if (aSolids == nullptr)
      continue;

    for (int aSolidIter = 0; aSolidIter < aSolids->Length(); ++aSolidIter)
    {
      myData->myIncStorage.ChangeSolid(aSolids->Value(aSolidIter)).IsModified = true;
    }
  }
}

//=================================================================================================

void BRepGraph::SetAllocator(const Handle(NCollection_BaseAllocator)& theAlloc)
{
  Standard_ASSERT_VOID(!myData->myIsDone,
                       "SetAllocator: must be called before Build() — existing graph state will be lost");

  myData->myAllocator = !theAlloc.IsNull() ? theAlloc : NCollection_BaseAllocator::CommonBaseAllocator();

  // Recreate the entire data object with the new allocator.
  myData = std::make_unique<BRepGraph_Data>(myData->myAllocator);
}

const Handle(NCollection_BaseAllocator)& BRepGraph::Allocator() const { return myData->myAllocator; }
void BRepGraph::SetHistoryEnabled(bool theVal) { myData->myHistoryLog.SetEnabled(theVal); }
bool BRepGraph::IsHistoryEnabled() const { return myData->myHistoryLog.IsEnabled(); }

BRepGraph_History& BRepGraph::History() { return myData->myHistoryLog; }
const BRepGraph_History& BRepGraph::History() const { return myData->myHistoryLog; }
