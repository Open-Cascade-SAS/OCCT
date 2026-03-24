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
#include <BRepGraph_Layer.hxx>

#include <BRepGraph_Builder.hxx>
#include <BRepGraphInc_ReverseIndex.hxx>

#include <NCollection_IncAllocator.hxx>
#include <NCollection_Map.hxx>
#include <Standard_ProgramError.hxx>

#include <shared_mutex>

//=================================================================================================

BRepGraph::BRepGraph()
    : myData(std::make_unique<BRepGraph_Data>())
{
}

//=================================================================================================

BRepGraph::BRepGraph(const occ::handle<NCollection_BaseAllocator>& theAlloc)
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

void BRepGraph::Build(const TopoDS_Shape& theShape, const bool theParallel)
{
  BRepGraph_Builder::Perform(*this, theShape, theParallel);
}

//=================================================================================================

void BRepGraph::Build(const TopoDS_Shape&                   theShape,
                      const bool                            theParallel,
                      const BRepGraphInc_Populate::Options& theOptions)
{
  BRepGraph_Builder::Perform(*this, theShape, theParallel, theOptions);
}

//=================================================================================================

BRepGraph_UID BRepGraph::allocateUID(const BRepGraph_NodeId theNodeId)
{
  const size_t  aCounter = myData->myNextUIDCounter.fetch_add(1, std::memory_order_relaxed);
  BRepGraph_UID aUID(theNodeId.NodeKind, aCounter, myData->myGeneration);
  myData->myIncStorage.ChangeUIDs(theNodeId.NodeKind).Append(aUID);
  return aUID;
}

//=================================================================================================

BRepGraph_NodeCache* BRepGraph::mutableCache(const BRepGraph_NodeId theNode)
{
  BRepGraphInc_Storage& aStorage = myData->myIncStorage;
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Solid:
      return &aStorage.ChangeSolid(theNode.Index).Cache;
    case BRepGraph_NodeId::Kind::Shell:
      return &aStorage.ChangeShell(theNode.Index).Cache;
    case BRepGraph_NodeId::Kind::Face:
      return &aStorage.ChangeFace(theNode.Index).Cache;
    case BRepGraph_NodeId::Kind::Wire:
      return &aStorage.ChangeWire(theNode.Index).Cache;
    case BRepGraph_NodeId::Kind::Edge:
      return &aStorage.ChangeEdge(theNode.Index).Cache;
    case BRepGraph_NodeId::Kind::CoEdge:
      return &aStorage.ChangeCoEdge(theNode.Index).Cache;
    case BRepGraph_NodeId::Kind::Vertex:
      return &aStorage.ChangeVertex(theNode.Index).Cache;
    case BRepGraph_NodeId::Kind::Compound:
      return &aStorage.ChangeCompound(theNode.Index).Cache;
    case BRepGraph_NodeId::Kind::CompSolid:
      return &aStorage.ChangeCompSolid(theNode.Index).Cache;
    case BRepGraph_NodeId::Kind::Product:
      return &aStorage.ChangeProduct(theNode.Index).Cache;
    case BRepGraph_NodeId::Kind::Occurrence:
      return &aStorage.ChangeOccurrence(theNode.Index).Cache;
    default:
      return nullptr;
  }
}

//=================================================================================================

bool BRepGraph::IsDone() const
{
  return myData->myIsDone;
}

//=================================================================================================

const BRepGraph_TopoNode::BaseDef* BRepGraph::TopoDef(const BRepGraph_NodeId theId) const
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
    case BRepGraph_NodeId::Kind::CoEdge:
      return theId.Index < aStorage.NbCoEdges() ? &aStorage.CoEdge(theId.Index) : nullptr;
    case BRepGraph_NodeId::Kind::Vertex:
      return theId.Index < aStorage.NbVertices() ? &aStorage.Vertex(theId.Index) : nullptr;
    case BRepGraph_NodeId::Kind::Compound:
      return theId.Index < aStorage.NbCompounds() ? &aStorage.Compound(theId.Index) : nullptr;
    case BRepGraph_NodeId::Kind::CompSolid:
      return theId.Index < aStorage.NbCompSolids() ? &aStorage.CompSolid(theId.Index) : nullptr;
    case BRepGraph_NodeId::Kind::Product:
      return theId.Index < aStorage.NbProducts() ? &aStorage.Product(theId.Index) : nullptr;
    case BRepGraph_NodeId::Kind::Occurrence:
      return theId.Index < aStorage.NbOccurrences() ? &aStorage.Occurrence(theId.Index) : nullptr;
    default:
      return nullptr;
  }
}

//=================================================================================================

BRepGraph_TopoNode::BaseDef* BRepGraph::ChangeTopoDef(const BRepGraph_NodeId theId)
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
    case BRepGraph_NodeId::Kind::CoEdge:
      return theId.Index < aStorage.NbCoEdges() ? &aStorage.ChangeCoEdge(theId.Index) : nullptr;
    case BRepGraph_NodeId::Kind::Vertex:
      return theId.Index < aStorage.NbVertices() ? &aStorage.ChangeVertex(theId.Index) : nullptr;
    case BRepGraph_NodeId::Kind::Compound:
      return theId.Index < aStorage.NbCompounds() ? &aStorage.ChangeCompound(theId.Index) : nullptr;
    case BRepGraph_NodeId::Kind::CompSolid:
      return theId.Index < aStorage.NbCompSolids() ? &aStorage.ChangeCompSolid(theId.Index)
                                                   : nullptr;
    case BRepGraph_NodeId::Kind::Product:
      return theId.Index < aStorage.NbProducts() ? &aStorage.ChangeProduct(theId.Index) : nullptr;
    case BRepGraph_NodeId::Kind::Occurrence:
      return theId.Index < aStorage.NbOccurrences() ? &aStorage.ChangeOccurrence(theId.Index)
                                                    : nullptr;
    default:
      return nullptr;
  }
}

//=================================================================================================

void BRepGraph::invalidateSubgraphImpl(const BRepGraph_NodeId theNode)
{
  using Kind                           = BRepGraph_NodeId::Kind;
  const BRepGraphInc_Storage& aStorage = myData->myIncStorage;

  const int aNbNodes = aStorage.NbSolids() + aStorage.NbShells() + aStorage.NbFaces()
                       + aStorage.NbWires() + aStorage.NbEdges() + aStorage.NbVertices()
                       + aStorage.NbCompounds() + aStorage.NbCompSolids() + aStorage.NbProducts()
                       + aStorage.NbOccurrences();
  occ::handle<NCollection_IncAllocator> anAlloc = new NCollection_IncAllocator();
  NCollection_Vector<BRepGraph_NodeId>  aStack(64, anAlloc);
  NCollection_Map<BRepGraph_NodeId>     aVisited(aNbNodes, anAlloc);
  aStack.Append(theNode);

  while (!aStack.IsEmpty())
  {
    const BRepGraph_NodeId aCurrent = aStack.Last();
    aStack.EraseLast();

    if (!aCurrent.IsValid() || !aVisited.Add(aCurrent))
      continue;

    BRepGraph_NodeCache* aCache = mutableCache(aCurrent);
    if (aCache != nullptr)
      aCache->InvalidateAll();

    switch (aCurrent.NodeKind)
    {
      case Kind::Compound: {
        const BRepGraphInc::CompoundEntity& aComp = aStorage.Compound(aCurrent.Index);
        for (int i = 0; i < aComp.ChildRefs.Length(); ++i)
        {
          const BRepGraphInc::ChildRef& aRef = aComp.ChildRefs.Value(i);
          aStack.Append(BRepGraph_NodeId(aRef.Kind, aRef.ChildIdx));
        }
        break;
      }
      case Kind::CompSolid: {
        const BRepGraphInc::CompSolidEntity& aCS = aStorage.CompSolid(aCurrent.Index);
        for (int i = 0; i < aCS.SolidRefs.Length(); ++i)
          aStack.Append(BRepGraph_NodeId::Solid(aCS.SolidRefs.Value(i).SolidIdx));
        break;
      }
      case Kind::Solid: {
        const BRepGraph_TopoNode::SolidDef& aSolid = aStorage.Solid(aCurrent.Index);
        for (int i = 0; i < aSolid.ShellRefs.Length(); ++i)
          aStack.Append(BRepGraph_NodeId::Shell(aSolid.ShellRefs.Value(i).ShellIdx));
        for (int i = 0; i < aSolid.FreeChildRefs.Length(); ++i)
        {
          const BRepGraphInc::ChildRef& aRef = aSolid.FreeChildRefs.Value(i);
          aStack.Append(BRepGraph_NodeId(aRef.Kind, aRef.ChildIdx));
        }
        break;
      }
      case Kind::Shell: {
        const BRepGraph_TopoNode::ShellDef& aShell = aStorage.Shell(aCurrent.Index);
        for (int i = 0; i < aShell.FaceRefs.Length(); ++i)
          aStack.Append(BRepGraph_NodeId::Face(aShell.FaceRefs.Value(i).FaceIdx));
        for (int i = 0; i < aShell.FreeChildRefs.Length(); ++i)
        {
          const BRepGraphInc::ChildRef& aRef = aShell.FreeChildRefs.Value(i);
          aStack.Append(BRepGraph_NodeId(aRef.Kind, aRef.ChildIdx));
        }
        break;
      }
      case Kind::Face: {
        const BRepGraph_TopoNode::FaceDef& aFace = aStorage.Face(aCurrent.Index);
        for (int i = 0; i < aFace.WireRefs.Length(); ++i)
          aStack.Append(BRepGraph_NodeId::Wire(aFace.WireRefs.Value(i).WireIdx));
        break;
      }
      case Kind::Wire: {
        const BRepGraph_TopoNode::WireDef& aWire = aStorage.Wire(aCurrent.Index);
        for (int i = 0; i < aWire.CoEdgeRefs.Length(); ++i)
        {
          const int aCoEdgeIdx = aWire.CoEdgeRefs.Value(i).CoEdgeIdx;
          if (aCoEdgeIdx >= 0 && aCoEdgeIdx < aStorage.NbCoEdges())
            aStack.Append(BRepGraph_NodeId::Edge(aStorage.CoEdge(aCoEdgeIdx).EdgeIdx));
        }
        break;
      }
      case Kind::Edge: {
        const BRepGraph_TopoNode::EdgeDef& anEdge = aStorage.Edge(aCurrent.Index);
        if (anEdge.StartVertexDefId().IsValid())
          aStack.Append(anEdge.StartVertexDefId());
        if (anEdge.EndVertexDefId().IsValid())
          aStack.Append(anEdge.EndVertexDefId());
        break;
      }
      case Kind::Product: {
        const BRepGraph_TopoNode::ProductDef& aProd = aStorage.Product(aCurrent.Index);
        if (aProd.ShapeRootId.IsValid())
          aStack.Append(aProd.ShapeRootId);
        for (int i = 0; i < aProd.OccurrenceRefs.Length(); ++i)
          aStack.Append(BRepGraph_NodeId::Occurrence(aProd.OccurrenceRefs.Value(i).OccurrenceIdx));
        break;
      }
      case Kind::Occurrence: {
        const BRepGraph_TopoNode::OccurrenceDef& anOcc = aStorage.Occurrence(aCurrent.Index);
        if (anOcc.ProductIdx >= 0)
          aStack.Append(BRepGraph_NodeId::Product(anOcc.ProductIdx));
        break;
      }
      default:
        break;
    }
  }
}

//=================================================================================================

void BRepGraph::markModified(const BRepGraph_NodeId theDefId)
{
  if (!theDefId.IsValid())
    return;

  BRepGraph_TopoNode::BaseDef* aDef = ChangeTopoDef(theDefId);
  if (aDef == nullptr)
    return;

  aDef->IsModified = true;
  ++aDef->MutationGen; // Track direct mutations even in deferred mode.

  // In deferred mode: skip mutex and upward propagation.
  if (myData->myDeferredMode)
    return;

  {
    std::unique_lock<std::shared_mutex> aWriteLock(myData->myCurrentShapesMutex);
    myData->myCurrentShapes.UnBind(theDefId);
  }

  // Invalidate UserAttribute caches (UVBounds, BndLib, etc.).
  BRepGraph_NodeCache* aCache = mutableCache(theDefId);
  if (aCache != nullptr)
    aCache->InvalidateAll();

  // Dispatch modification event for the directly mutated node.
  if (myHasModificationSubscribers)
    dispatchNodeModified(theDefId);

  // Propagate IsModified upward without incrementing MutationGen on parents.
  propagateModified(theDefId);
}

//=================================================================================================

void BRepGraph::markModified(const BRepGraph_NodeId theDefId, BRepGraph_TopoNode::BaseDef& theDef)
{
  theDef.IsModified = true;
  ++theDef.MutationGen; // Track direct mutations even in deferred mode.

  // In deferred mode: skip mutex and upward propagation.
  if (myData->myDeferredMode)
    return;

  {
    std::unique_lock<std::shared_mutex> aWriteLock(myData->myCurrentShapesMutex);
    myData->myCurrentShapes.UnBind(theDefId);
  }

  // Invalidate UserAttribute caches (UVBounds, BndLib, etc.).
  theDef.Cache.InvalidateAll();

  // Dispatch modification event for the directly mutated node.
  if (myHasModificationSubscribers)
    dispatchNodeModified(theDefId);

  // Propagate IsModified upward without incrementing MutationGen on parents.
  propagateModified(theDefId);
}

//=================================================================================================

void BRepGraph::markParentModified(const BRepGraph_NodeId theParentId)
{
  BRepGraph_TopoNode::BaseDef* aParent = ChangeTopoDef(theParentId);
  if (aParent == nullptr || aParent->IsModified)
    return;

  aParent->IsModified = true;

  {
    std::unique_lock<std::shared_mutex> aWriteLock(myData->myCurrentShapesMutex);
    myData->myCurrentShapes.UnBind(theParentId);
  }

  BRepGraph_NodeCache* aCache = mutableCache(theParentId);
  if (aCache != nullptr)
    aCache->InvalidateAll();

  if (myHasModificationSubscribers)
    dispatchNodeModified(theParentId);

  propagateModified(theParentId);
}

//=================================================================================================

void BRepGraph::propagateModified(const BRepGraph_NodeId theDefId)
{
  const BRepGraphInc_ReverseIndex& aRevIdx = myData->myIncStorage.ReverseIndex();
  switch (theDefId.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      // Vertex modifications don't propagate.
      break;
    case BRepGraph_NodeId::Kind::Edge: {
      const NCollection_Vector<int>* aWires = aRevIdx.WiresOfEdge(theDefId.Index);
      if (aWires != nullptr)
        for (int i = 0; i < aWires->Length(); ++i)
          markParentModified(BRepGraph_NodeId::Wire(aWires->Value(i)));
      break;
    }
    case BRepGraph_NodeId::Kind::Wire: {
      const NCollection_Vector<int>* aFaces = aRevIdx.FacesOfWire(theDefId.Index);
      if (aFaces != nullptr)
        for (int i = 0; i < aFaces->Length(); ++i)
          markParentModified(BRepGraph_NodeId::Face(aFaces->Value(i)));
      break;
    }
    case BRepGraph_NodeId::Kind::Face: {
      const NCollection_Vector<int>* aShells = aRevIdx.ShellsOfFace(theDefId.Index);
      if (aShells != nullptr)
        for (int i = 0; i < aShells->Length(); ++i)
          markParentModified(BRepGraph_NodeId::Shell(aShells->Value(i)));
      break;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      const NCollection_Vector<int>* aSolids = aRevIdx.SolidsOfShell(theDefId.Index);
      if (aSolids != nullptr)
        for (int i = 0; i < aSolids->Length(); ++i)
          markParentModified(BRepGraph_NodeId::Solid(aSolids->Value(i)));
      break;
    }
    case BRepGraph_NodeId::Kind::Occurrence: {
      // Occurrence modifications propagate to the parent product.
      const BRepGraph_TopoNode::OccurrenceDef& anOccDef =
        myData->myIncStorage.Occurrence(theDefId.Index);
      if (anOccDef.ParentProductIdx >= 0)
        markParentModified(BRepGraph_NodeId::Product(anOccDef.ParentProductIdx));
      break;
    }
    default:
      // Solid/Compound/CompSolid/Product modifications don't propagate further.
      break;
  }
}

//=================================================================================================

int BRepGraph::NbHistoryRecords() const
{
  return myData->myHistoryLog.NbRecords();
}

const BRepGraph_HistoryRecord& BRepGraph::HistoryRecord(const int theRecordIdx) const
{
  return myData->myHistoryLog.Record(theRecordIdx);
}

BRepGraph_NodeId BRepGraph::FindOriginal(const BRepGraph_NodeId theModified) const
{
  return myData->myHistoryLog.FindOriginal(theModified);
}

NCollection_Vector<BRepGraph_NodeId> BRepGraph::FindDerived(
  const BRepGraph_NodeId theOriginal) const
{
  return myData->myHistoryLog.FindDerived(theOriginal);
}

//=================================================================================================

void BRepGraph::ApplyModification(
  const BRepGraph_NodeId                                                            theTarget,
  std::function<NCollection_Vector<BRepGraph_NodeId>(BRepGraph&, BRepGraph_NodeId)> theModifier,
  const TCollection_AsciiString&                                                    theOpLabel)
{
  NCollection_Vector<BRepGraph_NodeId> aReplacements = theModifier(*this, theTarget);

  myData->myHistoryLog.Record(theOpLabel, theTarget, aReplacements);

  invalidateSubgraphImpl(theTarget);
}

//=================================================================================================

void BRepGraph::RecordHistory(const TCollection_AsciiString&              theOpLabel,
                              const BRepGraph_NodeId                      theOriginal,
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
  // set on mutated entities - reconstruction will recompute as needed.
  {
    std::unique_lock<std::shared_mutex> aWriteLock(myData->myCurrentShapesMutex);
    myData->myCurrentShapes.Clear();
  }

  // Propagate IsModified upward for all modified entities.
  // Single iterative pass per kind: Edge->Wire->Face->Shell->Solid.
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
      const int                    aWireIdx = aWires->Value(aWireIter);
      BRepGraph_TopoNode::WireDef& aWireDef = myData->myIncStorage.ChangeWire(aWireIdx);
      if (aWireDef.IsModified)
        continue;
      aWireDef.IsModified = true;

      const NCollection_Vector<int>* aFaces = aRevIdx.FacesOfWire(aWireIdx);
      if (aFaces == nullptr)
        continue;

      for (int aFaceIter = 0; aFaceIter < aFaces->Length(); ++aFaceIter)
      {
        const int                    aFaceIdx = aFaces->Value(aFaceIter);
        BRepGraph_TopoNode::FaceDef& aFaceDef = myData->myIncStorage.ChangeFace(aFaceIdx);
        if (aFaceDef.IsModified)
          continue;
        aFaceDef.IsModified = true;

        const NCollection_Vector<int>* aShells = aRevIdx.ShellsOfFace(aFaceIdx);
        if (aShells == nullptr)
          continue;

        for (int aShellIter = 0; aShellIter < aShells->Length(); ++aShellIter)
        {
          const int                     aShellIdx = aShells->Value(aShellIter);
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
      const int                    aFaceIdx = aFaces->Value(aFaceIter);
      BRepGraph_TopoNode::FaceDef& aFaceDef = myData->myIncStorage.ChangeFace(aFaceIdx);
      if (aFaceDef.IsModified)
        continue;
      aFaceDef.IsModified = true;

      const NCollection_Vector<int>* aShells = aRevIdx.ShellsOfFace(aFaceIdx);
      if (aShells == nullptr)
        continue;

      for (int aShellIter = 0; aShellIter < aShells->Length(); ++aShellIter)
      {
        const int                     aShellIdx = aShells->Value(aShellIter);
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
      const int                     aShellIdx = aShells->Value(aShellIter);
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

  // Invalidate UserAttribute caches (UVBounds, BndLib, etc.) for all modified entities.
  // When modification subscribers exist, the collection of modified NodeIds piggybacks
  // on this existing per-kind iteration to avoid a separate sweep pass.
  const int                            aNbVertices      = myData->myIncStorage.NbVertices();
  const int                            aNbSolids        = myData->myIncStorage.NbSolids();
  const int                            aNbCompounds     = myData->myIncStorage.NbCompounds();
  const int                            aNbCompSols      = myData->myIncStorage.NbCompSolids();
  const bool                           aCollectModified = myHasModificationSubscribers;
  NCollection_Vector<BRepGraph_NodeId> aModifiedNodes;
  int                                  aModifiedKindsMask = 0;

  for (int i = 0; i < aNbVertices; ++i)
  {
    const BRepGraphInc::VertexEntity& aVtx = myData->myIncStorage.Vertex(i);
    if (!aVtx.IsRemoved && aVtx.IsModified)
    {
      myData->myIncStorage.ChangeVertex(i).Cache.InvalidateAll();
      if (aCollectModified)
      {
        aModifiedNodes.Append(BRepGraph_NodeId::Vertex(i));
        aModifiedKindsMask |= BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Vertex);
      }
    }
  }
  for (int i = 0; i < aNbEdges; ++i)
  {
    const BRepGraphInc::EdgeEntity& anEdge = myData->myIncStorage.Edge(i);
    if (!anEdge.IsRemoved && anEdge.IsModified)
    {
      myData->myIncStorage.ChangeEdge(i).Cache.InvalidateAll();
      if (aCollectModified)
      {
        aModifiedNodes.Append(BRepGraph_NodeId::Edge(i));
        aModifiedKindsMask |= BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Edge);
      }
    }
  }
  for (int i = 0; i < aNbWires; ++i)
  {
    const BRepGraphInc::WireEntity& aWire = myData->myIncStorage.Wire(i);
    if (!aWire.IsRemoved && aWire.IsModified)
    {
      myData->myIncStorage.ChangeWire(i).Cache.InvalidateAll();
      if (aCollectModified)
      {
        aModifiedNodes.Append(BRepGraph_NodeId::Wire(i));
        aModifiedKindsMask |= BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Wire);
      }
    }
  }
  for (int i = 0; i < aNbFaces; ++i)
  {
    const BRepGraphInc::FaceEntity& aFace = myData->myIncStorage.Face(i);
    if (!aFace.IsRemoved && aFace.IsModified)
    {
      myData->myIncStorage.ChangeFace(i).Cache.InvalidateAll();
      if (aCollectModified)
      {
        aModifiedNodes.Append(BRepGraph_NodeId::Face(i));
        aModifiedKindsMask |= BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Face);
      }
    }
  }
  for (int i = 0; i < aNbShells; ++i)
  {
    const BRepGraphInc::ShellEntity& aShell = myData->myIncStorage.Shell(i);
    if (!aShell.IsRemoved && aShell.IsModified)
    {
      myData->myIncStorage.ChangeShell(i).Cache.InvalidateAll();
      if (aCollectModified)
      {
        aModifiedNodes.Append(BRepGraph_NodeId::Shell(i));
        aModifiedKindsMask |= BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Shell);
      }
    }
  }
  for (int i = 0; i < aNbSolids; ++i)
  {
    const BRepGraphInc::SolidEntity& aSolid = myData->myIncStorage.Solid(i);
    if (!aSolid.IsRemoved && aSolid.IsModified)
    {
      myData->myIncStorage.ChangeSolid(i).Cache.InvalidateAll();
      if (aCollectModified)
      {
        aModifiedNodes.Append(BRepGraph_NodeId::Solid(i));
        aModifiedKindsMask |= BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Solid);
      }
    }
  }
  for (int i = 0; i < aNbCompounds; ++i)
  {
    const BRepGraphInc::CompoundEntity& aComp = myData->myIncStorage.Compound(i);
    if (!aComp.IsRemoved && aComp.IsModified)
    {
      myData->myIncStorage.ChangeCompound(i).Cache.InvalidateAll();
      if (aCollectModified)
      {
        aModifiedNodes.Append(BRepGraph_NodeId::Compound(i));
        aModifiedKindsMask |= BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Compound);
      }
    }
  }
  for (int i = 0; i < aNbCompSols; ++i)
  {
    const BRepGraphInc::CompSolidEntity& aCS = myData->myIncStorage.CompSolid(i);
    if (!aCS.IsRemoved && aCS.IsModified)
    {
      myData->myIncStorage.ChangeCompSolid(i).Cache.InvalidateAll();
      if (aCollectModified)
      {
        aModifiedNodes.Append(BRepGraph_NodeId::CompSolid(i));
        aModifiedKindsMask |= BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::CompSolid);
      }
    }
  }

  // Dispatch batch modification event to subscribing layers.
  if (aCollectModified && !aModifiedNodes.IsEmpty())
    dispatchNodesModified(aModifiedNodes, aModifiedKindsMask);
}

//=================================================================================================

void BRepGraph::SetAllocator(const occ::handle<NCollection_BaseAllocator>& theAlloc)
{
  Standard_ASSERT_VOID(
    !myData->myIsDone,
    "SetAllocator: must be called before Build() - existing graph state will be lost");

  myData->myAllocator =
    !theAlloc.IsNull() ? theAlloc : NCollection_BaseAllocator::CommonBaseAllocator();

  // Recreate the entire data object with the new allocator.
  myData = std::make_unique<BRepGraph_Data>(myData->myAllocator);
}

const occ::handle<NCollection_BaseAllocator>& BRepGraph::Allocator() const
{
  return myData->myAllocator;
}

void BRepGraph::SetHistoryEnabled(const bool theVal)
{
  myData->myHistoryLog.SetEnabled(theVal);
}

bool BRepGraph::IsHistoryEnabled() const
{
  return myData->myHistoryLog.IsEnabled();
}

BRepGraph_History& BRepGraph::History()
{
  return myData->myHistoryLog;
}

const BRepGraph_History& BRepGraph::History() const
{
  return myData->myHistoryLog;
}

//=================================================================================================

void BRepGraph::RegisterLayer(const occ::handle<BRepGraph_Layer>& theLayer)
{
  if (!theLayer.IsNull())
  {
    myLayers.Bind(theLayer->Name(), theLayer);
    updateModificationSubscriberFlag();
  }
}

//=================================================================================================

occ::handle<BRepGraph_Layer> BRepGraph::FindLayer(const TCollection_AsciiString& theName) const
{
  const occ::handle<BRepGraph_Layer>* aPtr = myLayers.Seek(theName);
  return aPtr != nullptr ? *aPtr : occ::handle<BRepGraph_Layer>();
}

//=================================================================================================

void BRepGraph::UnregisterLayer(const TCollection_AsciiString& theName)
{
  myLayers.UnBind(theName);
  updateModificationSubscriberFlag();
}

//=================================================================================================

void BRepGraph::dispatchLayerOnNodeRemoved(const BRepGraph_NodeId theNode,
                                           const BRepGraph_NodeId theReplacement)
{
  for (NCollection_DataMap<TCollection_AsciiString, occ::handle<BRepGraph_Layer>>::Iterator anIter(
         myLayers);
       anIter.More();
       anIter.Next())
  {
    anIter.Value()->OnNodeRemoved(theNode, theReplacement);
  }
}

//=================================================================================================

void BRepGraph::updateModificationSubscriberFlag()
{
  myHasModificationSubscribers = false;
  for (NCollection_DataMap<TCollection_AsciiString, occ::handle<BRepGraph_Layer>>::Iterator anIter(
         myLayers);
       anIter.More();
       anIter.Next())
  {
    if (anIter.Value()->SubscribedKinds() != 0)
    {
      myHasModificationSubscribers = true;
      return;
    }
  }
}

//=================================================================================================

void BRepGraph::dispatchNodeModified(const BRepGraph_NodeId theNode)
{
  const int aKindBit = BRepGraph_Layer::KindBit(theNode.NodeKind);
  for (NCollection_DataMap<TCollection_AsciiString, occ::handle<BRepGraph_Layer>>::Iterator anIter(
         myLayers);
       anIter.More();
       anIter.Next())
  {
    if ((anIter.Value()->SubscribedKinds() & aKindBit) != 0)
      anIter.Value()->OnNodeModified(theNode);
  }
}

//=================================================================================================

void BRepGraph::dispatchNodesModified(const NCollection_Vector<BRepGraph_NodeId>& theModifiedNodes,
                                      const int theModifiedKindsMask)
{
  for (NCollection_DataMap<TCollection_AsciiString, occ::handle<BRepGraph_Layer>>::Iterator anIter(
         myLayers);
       anIter.More();
       anIter.Next())
  {
    if ((anIter.Value()->SubscribedKinds() & theModifiedKindsMask) != 0)
      anIter.Value()->OnNodesModified(theModifiedNodes);
  }
}

//=================================================================================================

BRepGraph_MutRef<BRepGraph_TopoNode::EdgeDef> BRepGraph::MutEdge(const int theEdgeIdx)
{
  return BRepGraph_MutRef<BRepGraph_TopoNode::EdgeDef>(
    this,
    &myData->myIncStorage.ChangeEdge(theEdgeIdx),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, theEdgeIdx));
}

//=================================================================================================

BRepGraph_MutRef<BRepGraph_TopoNode::CoEdgeDef> BRepGraph::MutCoEdge(const int theCoEdgeIdx)
{
  return BRepGraph_MutRef<BRepGraph_TopoNode::CoEdgeDef>(
    this,
    &myData->myIncStorage.ChangeCoEdge(theCoEdgeIdx),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::CoEdge, theCoEdgeIdx));
}

//=================================================================================================

int BRepGraph::CreateCurve2DRep(const occ::handle<Geom2d_Curve>& theCurve2d)
{
  if (theCurve2d.IsNull())
    return -1;
  BRepGraphInc::Curve2DRep& aRep  = myData->myIncStorage.AppendCurve2DRep();
  const int                 anIdx = myData->myIncStorage.NbCurves2D() - 1;
  aRep.Id                         = BRepGraph_RepId::Curve2D(anIdx);
  aRep.Curve                      = theCurve2d;
  return anIdx;
}

//=================================================================================================

BRepGraph_MutRef<BRepGraph_TopoNode::VertexDef> BRepGraph::MutVertex(const int theVertexIdx)
{
  return BRepGraph_MutRef<BRepGraph_TopoNode::VertexDef>(
    this,
    &myData->myIncStorage.ChangeVertex(theVertexIdx),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Vertex, theVertexIdx));
}

//=================================================================================================

BRepGraph_MutRef<BRepGraph_TopoNode::WireDef> BRepGraph::MutWire(const int theWireIdx)
{
  return BRepGraph_MutRef<BRepGraph_TopoNode::WireDef>(
    this,
    &myData->myIncStorage.ChangeWire(theWireIdx),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Wire, theWireIdx));
}

//=================================================================================================

BRepGraph_MutRef<BRepGraph_TopoNode::FaceDef> BRepGraph::MutFace(const int theFaceIdx)
{
  return BRepGraph_MutRef<BRepGraph_TopoNode::FaceDef>(
    this,
    &myData->myIncStorage.ChangeFace(theFaceIdx),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, theFaceIdx));
}

//=================================================================================================

BRepGraph_MutRef<BRepGraph_TopoNode::ShellDef> BRepGraph::MutShell(const int theShellIdx)
{
  return BRepGraph_MutRef<BRepGraph_TopoNode::ShellDef>(
    this,
    &myData->myIncStorage.ChangeShell(theShellIdx),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Shell, theShellIdx));
}

//=================================================================================================

BRepGraph_MutRef<BRepGraph_TopoNode::SolidDef> BRepGraph::MutSolid(const int theSolidIdx)
{
  return BRepGraph_MutRef<BRepGraph_TopoNode::SolidDef>(
    this,
    &myData->myIncStorage.ChangeSolid(theSolidIdx),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, theSolidIdx));
}

//=================================================================================================

BRepGraph_MutRef<BRepGraph_TopoNode::CompoundDef> BRepGraph::MutCompound(const int theCompoundIdx)
{
  return BRepGraph_MutRef<BRepGraph_TopoNode::CompoundDef>(
    this,
    &myData->myIncStorage.ChangeCompound(theCompoundIdx),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Compound, theCompoundIdx));
}

//=================================================================================================

BRepGraph_MutRef<BRepGraph_TopoNode::CompSolidDef> BRepGraph::MutCompSolid(
  const int theCompSolidIdx)
{
  return BRepGraph_MutRef<BRepGraph_TopoNode::CompSolidDef>(
    this,
    &myData->myIncStorage.ChangeCompSolid(theCompSolidIdx),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::CompSolid, theCompSolidIdx));
}

//=================================================================================================

BRepGraph_MutRef<BRepGraph_TopoNode::ProductDef> BRepGraph::MutProduct(const int theProductIdx)
{
  return BRepGraph_MutRef<BRepGraph_TopoNode::ProductDef>(
    this,
    &myData->myIncStorage.ChangeProduct(theProductIdx),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Product, theProductIdx));
}

//=================================================================================================

BRepGraph_MutRef<BRepGraph_TopoNode::OccurrenceDef> BRepGraph::MutOccurrence(
  const int theOccurrenceIdx)
{
  return BRepGraph_MutRef<BRepGraph_TopoNode::OccurrenceDef>(
    this,
    &myData->myIncStorage.ChangeOccurrence(theOccurrenceIdx),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Occurrence, theOccurrenceIdx));
}
