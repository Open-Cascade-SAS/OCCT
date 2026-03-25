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
  BRepGraphInc_Storage& aStorage  = myData->myIncStorage;
  const int             anIdx    = theNode.Index;
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Solid:
      return &aStorage.ChangeSolid(BRepGraph_SolidId(anIdx)).Cache;
    case BRepGraph_NodeId::Kind::Shell:
      return &aStorage.ChangeShell(BRepGraph_ShellId(anIdx)).Cache;
    case BRepGraph_NodeId::Kind::Face:
      return &aStorage.ChangeFace(BRepGraph_FaceId(anIdx)).Cache;
    case BRepGraph_NodeId::Kind::Wire:
      return &aStorage.ChangeWire(BRepGraph_WireId(anIdx)).Cache;
    case BRepGraph_NodeId::Kind::Edge:
      return &aStorage.ChangeEdge(BRepGraph_EdgeId(anIdx)).Cache;
    case BRepGraph_NodeId::Kind::CoEdge:
      return &aStorage.ChangeCoEdge(BRepGraph_CoEdgeId(anIdx)).Cache;
    case BRepGraph_NodeId::Kind::Vertex:
      return &aStorage.ChangeVertex(BRepGraph_VertexId(anIdx)).Cache;
    case BRepGraph_NodeId::Kind::Compound:
      return &aStorage.ChangeCompound(BRepGraph_CompoundId(anIdx)).Cache;
    case BRepGraph_NodeId::Kind::CompSolid:
      return &aStorage.ChangeCompSolid(BRepGraph_CompSolidId(anIdx)).Cache;
    case BRepGraph_NodeId::Kind::Product:
      return &aStorage.ChangeProduct(BRepGraph_ProductId(anIdx)).Cache;
    case BRepGraph_NodeId::Kind::Occurrence:
      return &aStorage.ChangeOccurrence(BRepGraph_OccurrenceId(anIdx)).Cache;
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
  const int                   anIdx   = theId.Index;
  switch (theId.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Solid:
      return anIdx < aStorage.NbSolids() ? &aStorage.Solid(BRepGraph_SolidId(anIdx)) : nullptr;
    case BRepGraph_NodeId::Kind::Shell:
      return anIdx < aStorage.NbShells() ? &aStorage.Shell(BRepGraph_ShellId(anIdx)) : nullptr;
    case BRepGraph_NodeId::Kind::Face:
      return anIdx < aStorage.NbFaces() ? &aStorage.Face(BRepGraph_FaceId(anIdx)) : nullptr;
    case BRepGraph_NodeId::Kind::Wire:
      return anIdx < aStorage.NbWires() ? &aStorage.Wire(BRepGraph_WireId(anIdx)) : nullptr;
    case BRepGraph_NodeId::Kind::Edge:
      return anIdx < aStorage.NbEdges() ? &aStorage.Edge(BRepGraph_EdgeId(anIdx)) : nullptr;
    case BRepGraph_NodeId::Kind::CoEdge:
      return anIdx < aStorage.NbCoEdges() ? &aStorage.CoEdge(BRepGraph_CoEdgeId(anIdx)) : nullptr;
    case BRepGraph_NodeId::Kind::Vertex:
      return anIdx < aStorage.NbVertices() ? &aStorage.Vertex(BRepGraph_VertexId(anIdx)) : nullptr;
    case BRepGraph_NodeId::Kind::Compound:
      return anIdx < aStorage.NbCompounds() ? &aStorage.Compound(BRepGraph_CompoundId(anIdx))
                                            : nullptr;
    case BRepGraph_NodeId::Kind::CompSolid:
      return anIdx < aStorage.NbCompSolids() ? &aStorage.CompSolid(BRepGraph_CompSolidId(anIdx))
                                             : nullptr;
    case BRepGraph_NodeId::Kind::Product:
      return anIdx < aStorage.NbProducts() ? &aStorage.Product(BRepGraph_ProductId(anIdx))
                                           : nullptr;
    case BRepGraph_NodeId::Kind::Occurrence:
      return anIdx < aStorage.NbOccurrences()
               ? &aStorage.Occurrence(BRepGraph_OccurrenceId(anIdx))
               : nullptr;
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
  const int             anIdx    = theId.Index;
  switch (theId.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Solid:
      return anIdx < aStorage.NbSolids() ? &aStorage.ChangeSolid(BRepGraph_SolidId(anIdx))
                                         : nullptr;
    case BRepGraph_NodeId::Kind::Shell:
      return anIdx < aStorage.NbShells() ? &aStorage.ChangeShell(BRepGraph_ShellId(anIdx))
                                         : nullptr;
    case BRepGraph_NodeId::Kind::Face:
      return anIdx < aStorage.NbFaces() ? &aStorage.ChangeFace(BRepGraph_FaceId(anIdx)) : nullptr;
    case BRepGraph_NodeId::Kind::Wire:
      return anIdx < aStorage.NbWires() ? &aStorage.ChangeWire(BRepGraph_WireId(anIdx)) : nullptr;
    case BRepGraph_NodeId::Kind::Edge:
      return anIdx < aStorage.NbEdges() ? &aStorage.ChangeEdge(BRepGraph_EdgeId(anIdx)) : nullptr;
    case BRepGraph_NodeId::Kind::CoEdge:
      return anIdx < aStorage.NbCoEdges() ? &aStorage.ChangeCoEdge(BRepGraph_CoEdgeId(anIdx))
                                          : nullptr;
    case BRepGraph_NodeId::Kind::Vertex:
      return anIdx < aStorage.NbVertices() ? &aStorage.ChangeVertex(BRepGraph_VertexId(anIdx))
                                           : nullptr;
    case BRepGraph_NodeId::Kind::Compound:
      return anIdx < aStorage.NbCompounds()
               ? &aStorage.ChangeCompound(BRepGraph_CompoundId(anIdx))
               : nullptr;
    case BRepGraph_NodeId::Kind::CompSolid:
      return anIdx < aStorage.NbCompSolids()
               ? &aStorage.ChangeCompSolid(BRepGraph_CompSolidId(anIdx))
               : nullptr;
    case BRepGraph_NodeId::Kind::Product:
      return anIdx < aStorage.NbProducts() ? &aStorage.ChangeProduct(BRepGraph_ProductId(anIdx))
                                           : nullptr;
    case BRepGraph_NodeId::Kind::Occurrence:
      return anIdx < aStorage.NbOccurrences()
               ? &aStorage.ChangeOccurrence(BRepGraph_OccurrenceId(anIdx))
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
        const BRepGraphInc::CompoundEntity& aComp =
          aStorage.Compound(BRepGraph_CompoundId(aCurrent.Index));
        for (int i = 0; i < aComp.ChildRefs.Length(); ++i)
        {
          const BRepGraphInc::ChildRef& aRef = aComp.ChildRefs.Value(i);
          aStack.Append(aRef.ChildDefId);
        }
        break;
      }
      case Kind::CompSolid: {
        const BRepGraphInc::CompSolidEntity& aCS =
          aStorage.CompSolid(BRepGraph_CompSolidId(aCurrent.Index));
        for (int i = 0; i < aCS.SolidRefs.Length(); ++i)
          aStack.Append(aCS.SolidRefs.Value(i).SolidDefId);
        break;
      }
      case Kind::Solid: {
        const BRepGraph_TopoNode::SolidDef& aSolid =
          aStorage.Solid(BRepGraph_SolidId(aCurrent.Index));
        for (int i = 0; i < aSolid.ShellRefs.Length(); ++i)
          aStack.Append(aSolid.ShellRefs.Value(i).ShellDefId);
        for (int i = 0; i < aSolid.FreeChildRefs.Length(); ++i)
        {
          const BRepGraphInc::ChildRef& aRef = aSolid.FreeChildRefs.Value(i);
          aStack.Append(aRef.ChildDefId);
        }
        break;
      }
      case Kind::Shell: {
        const BRepGraph_TopoNode::ShellDef& aShell =
          aStorage.Shell(BRepGraph_ShellId(aCurrent.Index));
        for (int i = 0; i < aShell.FaceRefs.Length(); ++i)
          aStack.Append(aShell.FaceRefs.Value(i).FaceDefId);
        for (int i = 0; i < aShell.FreeChildRefs.Length(); ++i)
        {
          const BRepGraphInc::ChildRef& aRef = aShell.FreeChildRefs.Value(i);
          aStack.Append(aRef.ChildDefId);
        }
        break;
      }
      case Kind::Face: {
        const BRepGraph_TopoNode::FaceDef& aFace =
          aStorage.Face(BRepGraph_FaceId(aCurrent.Index));
        for (int i = 0; i < aFace.WireRefs.Length(); ++i)
          aStack.Append(aFace.WireRefs.Value(i).WireDefId);
        break;
      }
      case Kind::Wire: {
        const BRepGraph_TopoNode::WireDef& aWire =
          aStorage.Wire(BRepGraph_WireId(aCurrent.Index));
        for (int i = 0; i < aWire.CoEdgeRefs.Length(); ++i)
        {
          const BRepGraph_CoEdgeId aCoEdgeId = aWire.CoEdgeRefs.Value(i).CoEdgeDefId;
          if (aCoEdgeId.IsValid() && aCoEdgeId.Index < aStorage.NbCoEdges())
            aStack.Append(aStorage.CoEdge(aCoEdgeId).EdgeDefId);
        }
        break;
      }
      case Kind::Edge: {
        const BRepGraph_TopoNode::EdgeDef& anEdge =
          aStorage.Edge(BRepGraph_EdgeId(aCurrent.Index));
        if (anEdge.StartVertexDefId().IsValid())
          aStack.Append(anEdge.StartVertexDefId());
        if (anEdge.EndVertexDefId().IsValid())
          aStack.Append(anEdge.EndVertexDefId());
        break;
      }
      case Kind::Product: {
        const BRepGraph_TopoNode::ProductDef& aProd =
          aStorage.Product(BRepGraph_ProductId(aCurrent.Index));
        if (aProd.ShapeRootId.IsValid())
          aStack.Append(aProd.ShapeRootId);
        for (int i = 0; i < aProd.OccurrenceRefs.Length(); ++i)
          aStack.Append(aProd.OccurrenceRefs.Value(i).OccurrenceDefId);
        break;
      }
      case Kind::Occurrence: {
        const BRepGraph_TopoNode::OccurrenceDef& anOcc =
          aStorage.Occurrence(BRepGraph_OccurrenceId(aCurrent.Index));
        if (anOcc.ProductDefId.IsValid())
          aStack.Append(anOcc.ProductDefId);
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
      const NCollection_Vector<BRepGraph_WireId>* aWires = aRevIdx.WiresOfEdge(BRepGraph_EdgeId(theDefId.Index));
      if (aWires != nullptr)
        for (int i = 0; i < aWires->Length(); ++i)
          markParentModified(aWires->Value(i));
      break;
    }
    case BRepGraph_NodeId::Kind::Wire: {
      const NCollection_Vector<BRepGraph_FaceId>* aFaces = aRevIdx.FacesOfWire(BRepGraph_WireId(theDefId.Index));
      if (aFaces != nullptr)
        for (int i = 0; i < aFaces->Length(); ++i)
          markParentModified(aFaces->Value(i));
      break;
    }
    case BRepGraph_NodeId::Kind::Face: {
      const NCollection_Vector<BRepGraph_ShellId>* aShells = aRevIdx.ShellsOfFace(BRepGraph_FaceId(theDefId.Index));
      if (aShells != nullptr)
        for (int i = 0; i < aShells->Length(); ++i)
          markParentModified(aShells->Value(i));
      break;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      const NCollection_Vector<BRepGraph_SolidId>* aSolids = aRevIdx.SolidsOfShell(BRepGraph_ShellId(theDefId.Index));
      if (aSolids != nullptr)
        for (int i = 0; i < aSolids->Length(); ++i)
          markParentModified(aSolids->Value(i));
      break;
    }
    case BRepGraph_NodeId::Kind::Occurrence: {
      // Occurrence modifications propagate to the parent product.
      const BRepGraph_TopoNode::OccurrenceDef& anOccDef =
        myData->myIncStorage.Occurrence(BRepGraph_OccurrenceId(theDefId.Index));
      if (anOccDef.ParentProductDefId.IsValid())
        markParentModified(anOccDef.ParentProductDefId);
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
    const BRepGraph_TopoNode::EdgeDef& anEdge = myData->myIncStorage.Edge(BRepGraph_EdgeId(anEdgeIdx));
    if (anEdge.IsRemoved || !anEdge.IsModified)
      continue;

    const NCollection_Vector<BRepGraph_WireId>* aWires = aRevIdx.WiresOfEdge(BRepGraph_EdgeId(anEdgeIdx));
    if (aWires == nullptr)
      continue;

    for (int aWireIter = 0; aWireIter < aWires->Length(); ++aWireIter)
    {
      const BRepGraph_WireId         aWireId  = aWires->Value(aWireIter);
      BRepGraph_TopoNode::WireDef& aWireDef = myData->myIncStorage.ChangeWire(aWireId);
      if (aWireDef.IsModified)
        continue;
      aWireDef.IsModified = true;

      const NCollection_Vector<BRepGraph_FaceId>* aFaces = aRevIdx.FacesOfWire(aWireId);
      if (aFaces == nullptr)
        continue;

      for (int aFaceIter = 0; aFaceIter < aFaces->Length(); ++aFaceIter)
      {
        const BRepGraph_FaceId         aFaceId  = aFaces->Value(aFaceIter);
        BRepGraph_TopoNode::FaceDef& aFaceDef = myData->myIncStorage.ChangeFace(aFaceId);
        if (aFaceDef.IsModified)
          continue;
        aFaceDef.IsModified = true;

        const NCollection_Vector<BRepGraph_ShellId>* aShells = aRevIdx.ShellsOfFace(aFaceId);
        if (aShells == nullptr)
          continue;

        for (int aShellIter = 0; aShellIter < aShells->Length(); ++aShellIter)
        {
          const BRepGraph_ShellId         aShellId  = aShells->Value(aShellIter);
          BRepGraph_TopoNode::ShellDef& aShellDef = myData->myIncStorage.ChangeShell(aShellId);
          if (aShellDef.IsModified)
            continue;
          aShellDef.IsModified = true;

          const NCollection_Vector<BRepGraph_SolidId>* aSolids = aRevIdx.SolidsOfShell(aShellId);
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
    if (!myData->myIncStorage.Wire(BRepGraph_WireId(aWireIdx)).IsModified)
      continue;

    const NCollection_Vector<BRepGraph_FaceId>* aFaces = aRevIdx.FacesOfWire(BRepGraph_WireId(aWireIdx));
    if (aFaces == nullptr)
      continue;

    for (int aFaceIter = 0; aFaceIter < aFaces->Length(); ++aFaceIter)
    {
      const BRepGraph_FaceId         aFaceId  = aFaces->Value(aFaceIter);
      BRepGraph_TopoNode::FaceDef& aFaceDef = myData->myIncStorage.ChangeFace(aFaceId);
      if (aFaceDef.IsModified)
        continue;
      aFaceDef.IsModified = true;

      const NCollection_Vector<BRepGraph_ShellId>* aShells = aRevIdx.ShellsOfFace(aFaceId);
      if (aShells == nullptr)
        continue;

      for (int aShellIter = 0; aShellIter < aShells->Length(); ++aShellIter)
      {
        const BRepGraph_ShellId         aShellId2 = aShells->Value(aShellIter);
        BRepGraph_TopoNode::ShellDef& aShellDef = myData->myIncStorage.ChangeShell(aShellId2);
        if (aShellDef.IsModified)
          continue;
        aShellDef.IsModified = true;

        const NCollection_Vector<BRepGraph_SolidId>* aSolids = aRevIdx.SolidsOfShell(aShellId2);
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
    if (!myData->myIncStorage.Face(BRepGraph_FaceId(aFaceIdx)).IsModified)
      continue;

    const NCollection_Vector<BRepGraph_ShellId>* aShells = aRevIdx.ShellsOfFace(BRepGraph_FaceId(aFaceIdx));
    if (aShells == nullptr)
      continue;

    for (int aShellIter = 0; aShellIter < aShells->Length(); ++aShellIter)
    {
      const BRepGraph_ShellId         aShellId3 = aShells->Value(aShellIter);
      BRepGraph_TopoNode::ShellDef& aShellDef = myData->myIncStorage.ChangeShell(aShellId3);
      if (aShellDef.IsModified)
        continue;
      aShellDef.IsModified = true;

      const NCollection_Vector<BRepGraph_SolidId>* aSolids = aRevIdx.SolidsOfShell(aShellId3);
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
    if (!myData->myIncStorage.Shell(BRepGraph_ShellId(aShellIdx)).IsModified)
      continue;

    const NCollection_Vector<BRepGraph_SolidId>* aSolids = aRevIdx.SolidsOfShell(BRepGraph_ShellId(aShellIdx));
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
    const BRepGraphInc::VertexEntity& aVtx = myData->myIncStorage.Vertex(BRepGraph_VertexId(i));
    if (!aVtx.IsRemoved && aVtx.IsModified)
    {
      myData->myIncStorage.ChangeVertex(BRepGraph_VertexId(i)).Cache.InvalidateAll();
      if (aCollectModified)
      {
        aModifiedNodes.Append(BRepGraph_NodeId::Vertex(i));
        aModifiedKindsMask |= BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Vertex);
      }
    }
  }
  for (int i = 0; i < aNbEdges; ++i)
  {
    const BRepGraphInc::EdgeEntity& anEdge = myData->myIncStorage.Edge(BRepGraph_EdgeId(i));
    if (!anEdge.IsRemoved && anEdge.IsModified)
    {
      myData->myIncStorage.ChangeEdge(BRepGraph_EdgeId(i)).Cache.InvalidateAll();
      if (aCollectModified)
      {
        aModifiedNodes.Append(BRepGraph_NodeId::Edge(i));
        aModifiedKindsMask |= BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Edge);
      }
    }
  }
  for (int i = 0; i < aNbWires; ++i)
  {
    const BRepGraphInc::WireEntity& aWire = myData->myIncStorage.Wire(BRepGraph_WireId(i));
    if (!aWire.IsRemoved && aWire.IsModified)
    {
      myData->myIncStorage.ChangeWire(BRepGraph_WireId(i)).Cache.InvalidateAll();
      if (aCollectModified)
      {
        aModifiedNodes.Append(BRepGraph_NodeId::Wire(i));
        aModifiedKindsMask |= BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Wire);
      }
    }
  }
  for (int i = 0; i < aNbFaces; ++i)
  {
    const BRepGraphInc::FaceEntity& aFace = myData->myIncStorage.Face(BRepGraph_FaceId(i));
    if (!aFace.IsRemoved && aFace.IsModified)
    {
      myData->myIncStorage.ChangeFace(BRepGraph_FaceId(i)).Cache.InvalidateAll();
      if (aCollectModified)
      {
        aModifiedNodes.Append(BRepGraph_NodeId::Face(i));
        aModifiedKindsMask |= BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Face);
      }
    }
  }
  for (int i = 0; i < aNbShells; ++i)
  {
    const BRepGraphInc::ShellEntity& aShell = myData->myIncStorage.Shell(BRepGraph_ShellId(i));
    if (!aShell.IsRemoved && aShell.IsModified)
    {
      myData->myIncStorage.ChangeShell(BRepGraph_ShellId(i)).Cache.InvalidateAll();
      if (aCollectModified)
      {
        aModifiedNodes.Append(BRepGraph_NodeId::Shell(i));
        aModifiedKindsMask |= BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Shell);
      }
    }
  }
  for (int i = 0; i < aNbSolids; ++i)
  {
    const BRepGraphInc::SolidEntity& aSolid = myData->myIncStorage.Solid(BRepGraph_SolidId(i));
    if (!aSolid.IsRemoved && aSolid.IsModified)
    {
      myData->myIncStorage.ChangeSolid(BRepGraph_SolidId(i)).Cache.InvalidateAll();
      if (aCollectModified)
      {
        aModifiedNodes.Append(BRepGraph_NodeId::Solid(i));
        aModifiedKindsMask |= BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Solid);
      }
    }
  }
  for (int i = 0; i < aNbCompounds; ++i)
  {
    const BRepGraphInc::CompoundEntity& aComp = myData->myIncStorage.Compound(BRepGraph_CompoundId(i));
    if (!aComp.IsRemoved && aComp.IsModified)
    {
      myData->myIncStorage.ChangeCompound(BRepGraph_CompoundId(i)).Cache.InvalidateAll();
      if (aCollectModified)
      {
        aModifiedNodes.Append(BRepGraph_NodeId::Compound(i));
        aModifiedKindsMask |= BRepGraph_Layer::KindBit(BRepGraph_NodeId::Kind::Compound);
      }
    }
  }
  for (int i = 0; i < aNbCompSols; ++i)
  {
    const BRepGraphInc::CompSolidEntity& aCS = myData->myIncStorage.CompSolid(BRepGraph_CompSolidId(i));
    if (!aCS.IsRemoved && aCS.IsModified)
    {
      myData->myIncStorage.ChangeCompSolid(BRepGraph_CompSolidId(i)).Cache.InvalidateAll();
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

BRepGraph_MutRef<BRepGraph_TopoNode::EdgeDef> BRepGraph::MutEdge(const BRepGraph_EdgeId theEdge)
{
  return BRepGraph_MutRef<BRepGraph_TopoNode::EdgeDef>(
    this,
    &myData->myIncStorage.ChangeEdge(theEdge),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, theEdge.Index));
}

//=================================================================================================

BRepGraph_MutRef<BRepGraph_TopoNode::CoEdgeDef> BRepGraph::MutCoEdge(const BRepGraph_CoEdgeId theCoEdge)
{
  return BRepGraph_MutRef<BRepGraph_TopoNode::CoEdgeDef>(
    this,
    &myData->myIncStorage.ChangeCoEdge(theCoEdge),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::CoEdge, theCoEdge.Index));
}

//=================================================================================================

BRepGraph_Curve2DRepId BRepGraph::CreateCurve2DRep(const occ::handle<Geom2d_Curve>& theCurve2d)
{
  if (theCurve2d.IsNull())
    return BRepGraph_Curve2DRepId();
  BRepGraphInc::Curve2DRep& aRep  = myData->myIncStorage.AppendCurve2DRep();
  const int                 anIdx = myData->myIncStorage.NbCurves2D() - 1;
  aRep.Id                         = BRepGraph_RepId::Curve2D(anIdx);
  aRep.Curve                      = theCurve2d;
  return BRepGraph_Curve2DRepId(anIdx);
}

//=================================================================================================

BRepGraph_MutRef<BRepGraph_TopoNode::VertexDef> BRepGraph::MutVertex(const BRepGraph_VertexId theVertex)
{
  return BRepGraph_MutRef<BRepGraph_TopoNode::VertexDef>(
    this,
    &myData->myIncStorage.ChangeVertex(theVertex),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Vertex, theVertex.Index));
}

//=================================================================================================

BRepGraph_MutRef<BRepGraph_TopoNode::WireDef> BRepGraph::MutWire(const BRepGraph_WireId theWire)
{
  return BRepGraph_MutRef<BRepGraph_TopoNode::WireDef>(
    this,
    &myData->myIncStorage.ChangeWire(theWire),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Wire, theWire.Index));
}

//=================================================================================================

BRepGraph_MutRef<BRepGraph_TopoNode::FaceDef> BRepGraph::MutFace(const BRepGraph_FaceId theFace)
{
  return BRepGraph_MutRef<BRepGraph_TopoNode::FaceDef>(
    this,
    &myData->myIncStorage.ChangeFace(theFace),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, theFace.Index));
}

//=================================================================================================

BRepGraph_MutRef<BRepGraph_TopoNode::ShellDef> BRepGraph::MutShell(const BRepGraph_ShellId theShell)
{
  return BRepGraph_MutRef<BRepGraph_TopoNode::ShellDef>(
    this,
    &myData->myIncStorage.ChangeShell(theShell),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Shell, theShell.Index));
}

//=================================================================================================

BRepGraph_MutRef<BRepGraph_TopoNode::SolidDef> BRepGraph::MutSolid(const BRepGraph_SolidId theSolid)
{
  return BRepGraph_MutRef<BRepGraph_TopoNode::SolidDef>(
    this,
    &myData->myIncStorage.ChangeSolid(theSolid),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, theSolid.Index));
}

//=================================================================================================

BRepGraph_MutRef<BRepGraph_TopoNode::CompoundDef> BRepGraph::MutCompound(const BRepGraph_CompoundId theCompound)
{
  return BRepGraph_MutRef<BRepGraph_TopoNode::CompoundDef>(
    this,
    &myData->myIncStorage.ChangeCompound(theCompound),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Compound, theCompound.Index));
}

//=================================================================================================

BRepGraph_MutRef<BRepGraph_TopoNode::CompSolidDef> BRepGraph::MutCompSolid(
  const BRepGraph_CompSolidId theCompSolid)
{
  return BRepGraph_MutRef<BRepGraph_TopoNode::CompSolidDef>(
    this,
    &myData->myIncStorage.ChangeCompSolid(theCompSolid),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::CompSolid, theCompSolid.Index));
}

//=================================================================================================

BRepGraph_MutRef<BRepGraph_TopoNode::ProductDef> BRepGraph::MutProduct(const BRepGraph_ProductId theProduct)
{
  return BRepGraph_MutRef<BRepGraph_TopoNode::ProductDef>(
    this,
    &myData->myIncStorage.ChangeProduct(theProduct),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Product, theProduct.Index));
}

//=================================================================================================

BRepGraph_MutRef<BRepGraph_TopoNode::OccurrenceDef> BRepGraph::MutOccurrence(
  const BRepGraph_OccurrenceId theOccurrence)
{
  return BRepGraph_MutRef<BRepGraph_TopoNode::OccurrenceDef>(
    this,
    &myData->myIncStorage.ChangeOccurrence(theOccurrence),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Occurrence, theOccurrence.Index));
}
