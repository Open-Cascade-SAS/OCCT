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
#include <BRepGraph_RefsView.hxx>

#include <BRepGraph_Builder.hxx>
#include <BRepGraphInc_ReverseIndex.hxx>

#include <NCollection_IncAllocator.hxx>
#include <NCollection_Map.hxx>
#include <Standard_ProgramError.hxx>

#include <shared_mutex>

//=================================================================================================

void BRepGraph::initViews()
{
  if (myData)
  {
    myData->myTopoView    = TopoView(this);
    myData->myUIDsView    = UIDsView(this);
    myData->myPathView    = PathView(this);
    myData->myAttrsView   = AttrsView(this);
    myData->myRefsView    = RefsView(this);
    myData->myShapesView  = ShapesView(this);
    myData->myBuilderView = BuilderView(this);
  }
}

//=================================================================================================

BRepGraph::BRepGraph()
    : myData(std::make_unique<BRepGraph_Data>())
{
  initViews();
}

//=================================================================================================

BRepGraph::BRepGraph(const occ::handle<NCollection_BaseAllocator>& theAlloc)
    : myData(std::make_unique<BRepGraph_Data>(theAlloc))
{
  initViews();
}

//=================================================================================================

BRepGraph::~BRepGraph() = default;

//=================================================================================================

const BRepGraph::TopoView& BRepGraph::Topo() const
{
  return myData->myTopoView;
}

//=================================================================================================

const BRepGraph::UIDsView& BRepGraph::UIDs() const
{
  return myData->myUIDsView;
}

//=================================================================================================

const BRepGraph::PathView& BRepGraph::Paths() const
{
  return myData->myPathView;
}

//=================================================================================================

BRepGraph::AttrsView& BRepGraph::Attrs()
{
  return myData->myAttrsView;
}

//=================================================================================================

const BRepGraph::RefsView& BRepGraph::Refs() const
{
  return myData->myRefsView;
}

//=================================================================================================

const BRepGraph::ShapesView& BRepGraph::Shapes() const
{
  return myData->myShapesView;
}

//=================================================================================================

BRepGraph::BuilderView& BRepGraph::Builder()
{
  return myData->myBuilderView;
}

//=================================================================================================

BRepGraph::BRepGraph(BRepGraph&& theOther) noexcept
    : myData(std::move(theOther.myData)),
      myLayers(std::move(theOther.myLayers)),
      myHasModificationSubscribers(theOther.myHasModificationSubscribers)
{
  // View objects store a back-pointer to the owning BRepGraph; after move,
  // they must point to the new owner (`this`), not the moved-from object.
  initViews();
}

//=================================================================================================

BRepGraph& BRepGraph::operator=(BRepGraph&& theOther) noexcept
{
  if (this != &theOther)
  {
    myData                       = std::move(theOther.myData);
    myLayers                     = std::move(theOther.myLayers);
    myHasModificationSubscribers = theOther.myHasModificationSubscribers;
    // View objects store a back-pointer to the owning BRepGraph; after move,
    // they must point to the new owner (`this`), not the moved-from object.
    initViews();
  }
  return *this;
}

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
  const size_t   aCounter    = myData->myNextUIDCounter.fetch_add(1, std::memory_order_relaxed);
  const uint32_t aGeneration = myData->myGeneration.load(std::memory_order_relaxed);
  BRepGraph_UID  aUID(theNodeId.NodeKind, aCounter, aGeneration);
  myData->myIncStorage.ChangeUIDs(theNodeId.NodeKind).Append(aUID);
  return aUID;
}

//=================================================================================================

BRepGraph_RefUID BRepGraph::allocateRefUID(const BRepGraph_RefId theRefId)
{
  const size_t        aCounter    = myData->myNextUIDCounter.fetch_add(1, std::memory_order_relaxed);
  const uint32_t      aGeneration = myData->myGeneration.load(std::memory_order_relaxed);
  const BRepGraph_RefUID aUID(theRefId.RefKind, aCounter, aGeneration);
  myData->myIncStorage.ChangeRefUIDs(theRefId.RefKind).Append(aUID);
  return aUID;
}

//=================================================================================================

BRepGraph_NodeCache* BRepGraph::mutableCache(const BRepGraph_NodeId theNode)
{
  BRepGraphInc_Storage& aStorage = myData->myIncStorage;
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
  switch (theId.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Solid: {
      const BRepGraph_SolidId anId(theId.Index);
      return anId.IsValid(aStorage.NbSolids()) ? &aStorage.Solid(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      const BRepGraph_ShellId anId(theId.Index);
      return anId.IsValid(aStorage.NbShells()) ? &aStorage.Shell(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Face: {
      const BRepGraph_FaceId anId(theId.Index);
      return anId.IsValid(aStorage.NbFaces()) ? &aStorage.Face(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Wire: {
      const BRepGraph_WireId anId(theId.Index);
      return anId.IsValid(aStorage.NbWires()) ? &aStorage.Wire(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Edge: {
      const BRepGraph_EdgeId anId(theId.Index);
      return anId.IsValid(aStorage.NbEdges()) ? &aStorage.Edge(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::CoEdge: {
      const BRepGraph_CoEdgeId anId(theId.Index);
      return anId.IsValid(aStorage.NbCoEdges()) ? &aStorage.CoEdge(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Vertex: {
      const BRepGraph_VertexId anId(theId.Index);
      return anId.IsValid(aStorage.NbVertices()) ? &aStorage.Vertex(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Compound: {
      const BRepGraph_CompoundId anId(theId.Index);
      return anId.IsValid(aStorage.NbCompounds()) ? &aStorage.Compound(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::CompSolid: {
      const BRepGraph_CompSolidId anId(theId.Index);
      return anId.IsValid(aStorage.NbCompSolids()) ? &aStorage.CompSolid(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Product: {
      const BRepGraph_ProductId anId(theId.Index);
      return anId.IsValid(aStorage.NbProducts()) ? &aStorage.Product(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Occurrence: {
      const BRepGraph_OccurrenceId anId(theId.Index);
      return anId.IsValid(aStorage.NbOccurrences()) ? &aStorage.Occurrence(anId) : nullptr;
    }
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
    case BRepGraph_NodeId::Kind::Solid: {
      const BRepGraph_SolidId anId(theId.Index);
      return anId.IsValid(aStorage.NbSolids()) ? &aStorage.ChangeSolid(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      const BRepGraph_ShellId anId(theId.Index);
      return anId.IsValid(aStorage.NbShells()) ? &aStorage.ChangeShell(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Face: {
      const BRepGraph_FaceId anId(theId.Index);
      return anId.IsValid(aStorage.NbFaces()) ? &aStorage.ChangeFace(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Wire: {
      const BRepGraph_WireId anId(theId.Index);
      return anId.IsValid(aStorage.NbWires()) ? &aStorage.ChangeWire(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Edge: {
      const BRepGraph_EdgeId anId(theId.Index);
      return anId.IsValid(aStorage.NbEdges()) ? &aStorage.ChangeEdge(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::CoEdge: {
      const BRepGraph_CoEdgeId anId(theId.Index);
      return anId.IsValid(aStorage.NbCoEdges()) ? &aStorage.ChangeCoEdge(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Vertex: {
      const BRepGraph_VertexId anId(theId.Index);
      return anId.IsValid(aStorage.NbVertices()) ? &aStorage.ChangeVertex(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Compound: {
      const BRepGraph_CompoundId anId(theId.Index);
      return anId.IsValid(aStorage.NbCompounds()) ? &aStorage.ChangeCompound(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::CompSolid: {
      const BRepGraph_CompSolidId anId(theId.Index);
      return anId.IsValid(aStorage.NbCompSolids()) ? &aStorage.ChangeCompSolid(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Product: {
      const BRepGraph_ProductId anId(theId.Index);
      return anId.IsValid(aStorage.NbProducts()) ? &aStorage.ChangeProduct(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Occurrence: {
      const BRepGraph_OccurrenceId anId(theId.Index);
      return anId.IsValid(aStorage.NbOccurrences()) ? &aStorage.ChangeOccurrence(anId) : nullptr;
    }
    default:
      return nullptr;
  }
}

//=================================================================================================

void BRepGraph::invalidateSubgraphImpl(const BRepGraph_NodeId theNode)
{
  if (!theNode.IsValid())
    return;

  using Kind                           = BRepGraph_NodeId::Kind;
  const BRepGraphInc_Storage& aStorage = myData->myIncStorage;

  // Bounds check: ensure the node index is within the entity vector.
  if (TopoDef(theNode) == nullptr)
    return;

  struct StackEntry
  {
    BRepGraph_NodeId Node;
    int              Depth;
  };

  const int aNbNodes = aStorage.NbSolids() + aStorage.NbShells() + aStorage.NbFaces()
                       + aStorage.NbWires() + aStorage.NbEdges() + aStorage.NbVertices()
                       + aStorage.NbCompounds() + aStorage.NbCompSolids() + aStorage.NbProducts()
                       + aStorage.NbOccurrences();
  const int                             aMaxDepth = aNbNodes > 0 ? aNbNodes : 1;
  occ::handle<NCollection_IncAllocator> anAlloc   = new NCollection_IncAllocator();
  NCollection_Vector<StackEntry>        aStack(64, anAlloc);
  NCollection_Map<BRepGraph_NodeId>     aVisited(aNbNodes, anAlloc);
  aStack.Append({theNode, 0});

  const auto aPushChild = [&](const BRepGraph_NodeId theChild, const int theDepth) {
    if (!theChild.IsValid())
      return;
    aStack.Append({theChild, theDepth});
  };

  while (!aStack.IsEmpty())
  {
    const StackEntry aCurrent = aStack.Last();
    aStack.EraseLast();

    if (aCurrent.Depth > aMaxDepth || !aCurrent.Node.IsValid() || !aVisited.Add(aCurrent.Node))
      continue;

    // Bounds check: skip nodes whose index is outside the entity vector.
    if (TopoDef(aCurrent.Node) == nullptr)
      continue;

    BRepGraph_NodeCache* aCache = mutableCache(aCurrent.Node);
    if (aCache != nullptr)
      aCache->InvalidateAll();

    const int aNextDepth = aCurrent.Depth + 1;

    switch (aCurrent.Node.NodeKind)
    {
      case Kind::Compound: {
        const BRepGraphInc::CompoundEntity& aCompEnt =
          aStorage.Compound(BRepGraph_CompoundId(aCurrent.Node.Index));
        for (int i = 0; i < aCompEnt.ChildRefIds.Length(); ++i)
        {
          const BRepGraphInc::ChildRefEntry& aRef = aStorage.ChildRefEntry(aCompEnt.ChildRefIds.Value(i));
          if (!aRef.IsRemoved)
            aPushChild(aRef.ChildDefId, aNextDepth);
        }
        break;
      }
      case Kind::CompSolid: {
        const BRepGraphInc::CompSolidEntity& aCSolEnt =
          aStorage.CompSolid(BRepGraph_CompSolidId(aCurrent.Node.Index));
        for (int i = 0; i < aCSolEnt.SolidRefIds.Length(); ++i)
        {
          const BRepGraphInc::SolidRefEntry& aRef = aStorage.SolidRefEntry(aCSolEnt.SolidRefIds.Value(i));
          if (!aRef.IsRemoved)
            aPushChild(aRef.SolidDefId, aNextDepth);
        }
        break;
      }
      case Kind::Solid: {
        const BRepGraphInc::SolidEntity& aSolidEnt =
          aStorage.Solid(BRepGraph_SolidId(aCurrent.Node.Index));
        for (int i = 0; i < aSolidEnt.ShellRefIds.Length(); ++i)
        {
          const BRepGraphInc::ShellRefEntry& aRef =
            aStorage.ShellRefEntry(aSolidEnt.ShellRefIds.Value(i));
          if (!aRef.IsRemoved)
            aPushChild(aRef.ShellDefId, aNextDepth);
        }
        for (int i = 0; i < aSolidEnt.FreeChildRefIds.Length(); ++i)
          aPushChild(aStorage.ChildRefEntry(aSolidEnt.FreeChildRefIds.Value(i)).ChildDefId,
                    aNextDepth);
        break;
      }
      case Kind::Shell: {
        const BRepGraphInc::ShellEntity& aShellEnt =
          aStorage.Shell(BRepGraph_ShellId(aCurrent.Node.Index));
        for (int i = 0; i < aShellEnt.FaceRefIds.Length(); ++i)
        {
          const BRepGraphInc::FaceRefEntry& aRef =
            aStorage.FaceRefEntry(aShellEnt.FaceRefIds.Value(i));
          if (!aRef.IsRemoved)
            aPushChild(aRef.FaceDefId, aNextDepth);
        }
        for (int i = 0; i < aShellEnt.FreeChildRefIds.Length(); ++i)
          aPushChild(aStorage.ChildRefEntry(aShellEnt.FreeChildRefIds.Value(i)).ChildDefId,
                    aNextDepth);
        break;
      }
      case Kind::Face: {
        const BRepGraphInc::FaceEntity& aFaceEnt =
          aStorage.Face(BRepGraph_FaceId(aCurrent.Node.Index));
        for (int i = 0; i < aFaceEnt.WireRefIds.Length(); ++i)
        {
          const BRepGraphInc::WireRefEntry& aRef = aStorage.WireRefEntry(aFaceEnt.WireRefIds.Value(i));
          if (!aRef.IsRemoved)
            aPushChild(aRef.WireDefId, aNextDepth);
        }
        break;
      }
      case Kind::Wire: {
        const BRepGraphInc::WireEntity& aWireEnt =
          aStorage.Wire(BRepGraph_WireId(aCurrent.Node.Index));
        for (int i = 0; i < aWireEnt.CoEdgeRefIds.Length(); ++i)
        {
          const BRepGraphInc::CoEdgeRefEntry& aRef =
            aStorage.CoEdgeRefEntry(aWireEnt.CoEdgeRefIds.Value(i));
          if (aRef.IsRemoved)
            continue;
          const BRepGraph_CoEdgeId aCoEdgeId = aRef.CoEdgeDefId;
          if (aCoEdgeId.IsValid() && aCoEdgeId.Index < aStorage.NbCoEdges())
            aPushChild(aStorage.CoEdge(aCoEdgeId).EdgeDefId, aNextDepth);
        }
        break;
      }
      case Kind::Edge: {
        const BRepGraphInc::EdgeEntity& anEdge =
          aStorage.Edge(BRepGraph_EdgeId(aCurrent.Node.Index));
        if (anEdge.StartVertexRefId.IsValid())
        {
          const BRepGraph_VertexId aStartVtx =
            aStorage.VertexRefEntry(anEdge.StartVertexRefId).VertexDefId;
          if (aStartVtx.IsValid())
            aPushChild(aStartVtx, aNextDepth);
        }
        if (anEdge.EndVertexRefId.IsValid())
        {
          const BRepGraph_VertexId anEndVtx =
            aStorage.VertexRefEntry(anEdge.EndVertexRefId).VertexDefId;
          if (anEndVtx.IsValid())
            aPushChild(anEndVtx, aNextDepth);
        }
        for (int i = 0; i < anEdge.InternalVertexRefIds.Length(); ++i)
          aPushChild(aStorage.VertexRefEntry(anEdge.InternalVertexRefIds.Value(i)).VertexDefId,
                    aNextDepth);
        break;
      }
      case Kind::Product: {
        const BRepGraph_TopoNode::ProductDef& aProd =
          aStorage.Product(BRepGraph_ProductId(aCurrent.Node.Index));
        aPushChild(aProd.ShapeRootId, aNextDepth);
        for (int i = 0; i < aProd.OccurrenceRefIds.Length(); ++i)
          aPushChild(
            aStorage.OccurrenceRefEntry(aProd.OccurrenceRefIds.Value(i)).OccurrenceDefId,
            aNextDepth);
        break;
      }
      case Kind::Occurrence: {
        const BRepGraph_TopoNode::OccurrenceDef& anOcc =
          aStorage.Occurrence(BRepGraph_OccurrenceId(aCurrent.Node.Index));
        aPushChild(anOcc.ProductDefId, aNextDepth);
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

void BRepGraph::markRefModified(const BRepGraph_RefId theRefId)
{
  if (!theRefId.IsValid())
    return;

  BRepGraphInc::BaseRef& aRef = myData->myIncStorage.ChangeBaseRefEntry(theRefId);
  markRefModified(theRefId, aRef);
}

//=================================================================================================

void BRepGraph::markRefModified(const BRepGraph_RefId /*theRefId*/, BRepGraphInc::BaseRef& theRef)
{
  ++theRef.MutationGen;

  if (!theRef.ParentId.IsValid())
    return;

  markParentModified(theRef.ParentId);
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
      const NCollection_Vector<BRepGraph_WireId>* aWires =
        aRevIdx.WiresOfEdge(BRepGraph_EdgeId(theDefId.Index));
      if (aWires != nullptr)
        for (int i = 0; i < aWires->Length(); ++i)
          markParentModified(aWires->Value(i));
      break;
    }
    case BRepGraph_NodeId::Kind::Wire: {
      const NCollection_Vector<BRepGraph_FaceId>* aFaces =
        aRevIdx.FacesOfWire(BRepGraph_WireId(theDefId.Index));
      if (aFaces != nullptr)
        for (int i = 0; i < aFaces->Length(); ++i)
          markParentModified(aFaces->Value(i));
      break;
    }
    case BRepGraph_NodeId::Kind::Face: {
      const NCollection_Vector<BRepGraph_ShellId>* aShells =
        aRevIdx.ShellsOfFace(BRepGraph_FaceId(theDefId.Index));
      if (aShells != nullptr)
        for (int i = 0; i < aShells->Length(); ++i)
          markParentModified(aShells->Value(i));
      break;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      const NCollection_Vector<BRepGraph_SolidId>* aSolids =
        aRevIdx.SolidsOfShell(BRepGraph_ShellId(theDefId.Index));
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

void BRepGraph::RecordHistory(const TCollection_AsciiString&              theOpLabel,
                              const BRepGraph_NodeId                      theOriginal,
                              const NCollection_Vector<BRepGraph_NodeId>& theReplacements)
{
  myData->myHistoryLog.Record(theOpLabel, theOriginal, theReplacements);
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
  initViews();
}

const occ::handle<NCollection_BaseAllocator>& BRepGraph::Allocator() const
{
  return myData->myAllocator;
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
