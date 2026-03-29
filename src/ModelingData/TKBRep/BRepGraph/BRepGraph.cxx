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
    myData->myCacheView   = CacheView(this);
    myData->myRefsView    = RefsView(this);
    myData->myShapesView  = ShapesView(this);
    myData->myBuilderView = BuilderView(this);
  }
}

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

BRepGraph::CacheView& BRepGraph::Cache()
{
  return myData->myCacheView;
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

const BRepGraph::BuilderView& BRepGraph::Builder() const
{
  return myData->myBuilderView;
}

//=================================================================================================

BRepGraph::BRepGraph(BRepGraph&& theOther) noexcept
    : myData(std::move(theOther.myData)),
      myLayerRegistry(std::move(theOther.myLayerRegistry)),
      myTransientCache(std::move(theOther.myTransientCache))
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
    myData           = std::move(theOther.myData);
    myLayerRegistry  = std::move(theOther.myLayerRegistry);
    myTransientCache = std::move(theOther.myTransientCache);
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
  // Load counter before append: if Append() throws, no counter is consumed.
  // Single-threaded precondition: UID allocation is only called from Builder
  // methods which are externally serialized, so load-then-increment is safe.
  const size_t   aCounter    = myData->myNextUIDCounter.load(std::memory_order_relaxed);
  const uint32_t aGeneration = myData->myGeneration.load(std::memory_order_relaxed);
  BRepGraph_UID  aUID(theNodeId.NodeKind, aCounter, aGeneration);
  myData->myIncStorage.ChangeUIDs(theNodeId.NodeKind).Append(aUID);
  {
    std::unique_lock<std::shared_mutex> aLock(myData->myUIDToNodeIdMutex);
    if (myData->myUIDToNodeIdGeneration != aGeneration)
    {
      myData->myUIDToNodeId.Clear();
      myData->myUIDToNodeIdGeneration = aGeneration;
      myData->myUIDToNodeIdDirty      = false;
    }
    if (!myData->myUIDToNodeIdDirty)
    {
      myData->myUIDToNodeId.Bind(aUID, theNodeId);
    }
  }
  myData->myNextUIDCounter.fetch_add(1, std::memory_order_relaxed);
  return aUID;
}

//=================================================================================================

BRepGraph_RefUID BRepGraph::allocateRefUID(const BRepGraph_RefId theRefId)
{
  // Load counter before append: if Append() throws, no counter is consumed.
  // Single-threaded precondition: see allocateUID() comment.
  const size_t   aCounter    = myData->myNextUIDCounter.load(std::memory_order_relaxed);
  const uint32_t aGeneration = myData->myGeneration.load(std::memory_order_relaxed);
  const BRepGraph_RefUID aUID(theRefId.RefKind, aCounter, aGeneration);
  myData->myIncStorage.ChangeRefUIDs(theRefId.RefKind).Append(aUID);
  {
    std::unique_lock<std::shared_mutex> aLock(myData->myRefUIDToRefIdMutex);
    if (myData->myRefUIDToRefIdGeneration != aGeneration)
    {
      myData->myRefUIDToRefId.Clear();
      myData->myRefUIDToRefIdGeneration = aGeneration;
      myData->myRefUIDToRefIdDirty      = false;
    }
    if (!myData->myRefUIDToRefIdDirty)
    {
      myData->myRefUIDToRefId.Bind(aUID, theRefId);
    }
  }
  myData->myNextUIDCounter.fetch_add(1, std::memory_order_relaxed);
  return aUID;
}

//=================================================================================================

// mutableCache() removed — NodeCache no longer exists in BaseDef.
// Transient caches now live in BRepGraph_TransientCache.

//=================================================================================================

bool BRepGraph::IsDone() const
{
  return myData->myIsDone;
}

//=================================================================================================

const BRepGraphInc::BaseDef* BRepGraph::topoEntity(const BRepGraph_NodeId theId) const
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

BRepGraphInc::BaseDef* BRepGraph::changeTopoEntity(const BRepGraph_NodeId theId)
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
  if (topoEntity(theNode) == nullptr)
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
    if (topoEntity(aCurrent.Node) == nullptr)
      continue;

    // Increment OwnGen + SubtreeGen so generation-based cache freshness detects the change.
    BRepGraphInc::BaseDef* anEntity = changeTopoEntity(aCurrent.Node);
    if (anEntity != nullptr)
    {
      ++anEntity->OwnGen;
      ++anEntity->SubtreeGen;
    }

    const int aNextDepth = aCurrent.Depth + 1;

    switch (aCurrent.Node.NodeKind)
    {
      case Kind::Compound: {
        const BRepGraphInc::CompoundDef& aCompEnt =
          aStorage.Compound(BRepGraph_CompoundId(aCurrent.Node.Index));
        for (int i = 0; i < aCompEnt.ChildRefIds.Length(); ++i)
        {
          const BRepGraphInc::ChildRef& aRef = aStorage.ChildRef(aCompEnt.ChildRefIds.Value(i));
          if (!aRef.IsRemoved)
            aPushChild(aRef.ChildDefId, aNextDepth);
        }
        break;
      }
      case Kind::CompSolid: {
        const BRepGraphInc::CompSolidDef& aCSolEnt =
          aStorage.CompSolid(BRepGraph_CompSolidId(aCurrent.Node.Index));
        for (int i = 0; i < aCSolEnt.SolidRefIds.Length(); ++i)
        {
          const BRepGraphInc::SolidRef& aRef = aStorage.SolidRef(aCSolEnt.SolidRefIds.Value(i));
          if (!aRef.IsRemoved)
            aPushChild(aRef.SolidDefId, aNextDepth);
        }
        break;
      }
      case Kind::Solid: {
        const BRepGraphInc::SolidDef& aSolidEnt =
          aStorage.Solid(BRepGraph_SolidId(aCurrent.Node.Index));
        for (int i = 0; i < aSolidEnt.ShellRefIds.Length(); ++i)
        {
          const BRepGraphInc::ShellRef& aRef = aStorage.ShellRef(aSolidEnt.ShellRefIds.Value(i));
          if (!aRef.IsRemoved)
            aPushChild(aRef.ShellDefId, aNextDepth);
        }
        for (int i = 0; i < aSolidEnt.FreeChildRefIds.Length(); ++i)
          aPushChild(aStorage.ChildRef(aSolidEnt.FreeChildRefIds.Value(i)).ChildDefId, aNextDepth);
        break;
      }
      case Kind::Shell: {
        const BRepGraphInc::ShellDef& aShellEnt =
          aStorage.Shell(BRepGraph_ShellId(aCurrent.Node.Index));
        for (int i = 0; i < aShellEnt.FaceRefIds.Length(); ++i)
        {
          const BRepGraphInc::FaceRef& aRef = aStorage.FaceRef(aShellEnt.FaceRefIds.Value(i));
          if (!aRef.IsRemoved)
            aPushChild(aRef.FaceDefId, aNextDepth);
        }
        for (int i = 0; i < aShellEnt.FreeChildRefIds.Length(); ++i)
          aPushChild(aStorage.ChildRef(aShellEnt.FreeChildRefIds.Value(i)).ChildDefId, aNextDepth);
        break;
      }
      case Kind::Face: {
        const BRepGraphInc::FaceDef& aFaceEnt =
          aStorage.Face(BRepGraph_FaceId(aCurrent.Node.Index));
        for (int i = 0; i < aFaceEnt.WireRefIds.Length(); ++i)
        {
          const BRepGraphInc::WireRef& aRef = aStorage.WireRef(aFaceEnt.WireRefIds.Value(i));
          if (!aRef.IsRemoved)
            aPushChild(aRef.WireDefId, aNextDepth);
        }
        break;
      }
      case Kind::Wire: {
        const BRepGraphInc::WireDef& aWireEnt =
          aStorage.Wire(BRepGraph_WireId(aCurrent.Node.Index));
        for (int i = 0; i < aWireEnt.CoEdgeRefIds.Length(); ++i)
        {
          const BRepGraphInc::CoEdgeRef& aRef = aStorage.CoEdgeRef(aWireEnt.CoEdgeRefIds.Value(i));
          if (aRef.IsRemoved)
            continue;
          const BRepGraph_CoEdgeId aCoEdgeId = aRef.CoEdgeDefId;
          if (aCoEdgeId.IsValid() && aCoEdgeId.Index < aStorage.NbCoEdges())
            aPushChild(aStorage.CoEdge(aCoEdgeId).EdgeDefId, aNextDepth);
        }
        break;
      }
      case Kind::Edge: {
        const BRepGraphInc::EdgeDef& anEdge = aStorage.Edge(BRepGraph_EdgeId(aCurrent.Node.Index));
        if (anEdge.StartVertexRefId.IsValid())
        {
          const BRepGraph_VertexId aStartVtx =
            aStorage.VertexRef(anEdge.StartVertexRefId).VertexDefId;
          if (aStartVtx.IsValid())
            aPushChild(aStartVtx, aNextDepth);
        }
        if (anEdge.EndVertexRefId.IsValid())
        {
          const BRepGraph_VertexId anEndVtx = aStorage.VertexRef(anEdge.EndVertexRefId).VertexDefId;
          if (anEndVtx.IsValid())
            aPushChild(anEndVtx, aNextDepth);
        }
        for (int i = 0; i < anEdge.InternalVertexRefIds.Length(); ++i)
          aPushChild(aStorage.VertexRef(anEdge.InternalVertexRefIds.Value(i)).VertexDefId,
                     aNextDepth);
        break;
      }
      case Kind::Product: {
        const BRepGraphInc::ProductDef& aProd =
          aStorage.Product(BRepGraph_ProductId(aCurrent.Node.Index));
        aPushChild(aProd.ShapeRootId, aNextDepth);
        for (int i = 0; i < aProd.OccurrenceRefIds.Length(); ++i)
          aPushChild(aStorage.OccurrenceRef(aProd.OccurrenceRefIds.Value(i)).OccurrenceDefId,
                     aNextDepth);
        break;
      }
      case Kind::Occurrence: {
        const BRepGraphInc::OccurrenceDef& anOcc =
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

void BRepGraph::markModified(const BRepGraph_NodeId theNodeId) noexcept
{
  if (!theNodeId.IsValid())
    return;

  BRepGraphInc::BaseDef* anEntity = changeTopoEntity(theNodeId);
  if (anEntity == nullptr)
    return;

  markModified(theNodeId, *anEntity);
}

//=================================================================================================

void BRepGraph::markModified(const BRepGraph_NodeId theNodeId,
                             BRepGraphInc::BaseDef& theEntity) noexcept
{
  ++theEntity.OwnGen;
  ++theEntity.SubtreeGen;
  const uint32_t aWave = myData->myPropagationWave.fetch_add(1, std::memory_order_relaxed) + 1;
  theEntity.LastPropWave = aWave;

  // In deferred mode: accumulate for batch processing.
  if (myData->myDeferredMode.load(std::memory_order_relaxed))
  {
    myData->myDeferredModified.Append(theNodeId);
    return;
  }

  // Dispatch modification event for the directly mutated node.
  if (myLayerRegistry.HasModificationSubscribers())
    myLayerRegistry.DispatchNodeModified(theNodeId);

  // Propagate SubtreeGen upward to parents (mutex-free).
  propagateSubtreeGen(theNodeId);
}

//=================================================================================================

void BRepGraph::markRefModified(const BRepGraph_RefId theRefId) noexcept
{
  if (!theRefId.IsValid())
    return;

  BRepGraphInc::BaseRef& aRef = myData->myIncStorage.ChangeBaseRef(theRefId);
  markRefModified(theRefId, aRef);
}

//=================================================================================================

void BRepGraph::markRefModified(const BRepGraph_RefId /*theRefId*/,
                                BRepGraphInc::BaseRef& theRef) noexcept
{
  ++theRef.OwnGen;
  myData->myPropagationWave.fetch_add(1, std::memory_order_relaxed);

  if (!theRef.ParentId.IsValid())
    return;

  markParentSubtreeGen(theRef.ParentId);
}

//=================================================================================================

void BRepGraph::markParentSubtreeGen(const BRepGraph_NodeId theParentId) noexcept
{
  BRepGraphInc::BaseDef* aParent = changeTopoEntity(theParentId);
  if (aParent == nullptr)
    return;

  const uint32_t aWave = myData->myPropagationWave.load(std::memory_order_relaxed);

  // Re-visit guard: skip if this parent was already processed in the current
  // propagation wave. Prevents exponential blowup on diamond topologies.
  if (aParent->LastPropWave == aWave)
    return;
  aParent->LastPropWave = aWave;

  ++aParent->SubtreeGen; // ONLY SubtreeGen — not OwnGen.
  // NO mutex, NO shape cache UnBind, NO dispatch.
  propagateSubtreeGen(theParentId);
}

//=================================================================================================

void BRepGraph::propagateSubtreeGen(const BRepGraph_NodeId theNodeId) noexcept
{
  const BRepGraphInc_ReverseIndex& aRevIdx = myData->myIncStorage.ReverseIndex();
  switch (theNodeId.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      // Vertex modifications don't propagate.
      break;
    case BRepGraph_NodeId::Kind::Edge: {
      const NCollection_Vector<BRepGraph_WireId>* aWires =
        aRevIdx.WiresOfEdge(BRepGraph_EdgeId(theNodeId.Index));
      if (aWires != nullptr)
        for (int i = 0; i < aWires->Length(); ++i)
          markParentSubtreeGen(aWires->Value(i));
      break;
    }
    case BRepGraph_NodeId::Kind::Wire: {
      const NCollection_Vector<BRepGraph_FaceId>* aFaces =
        aRevIdx.FacesOfWire(BRepGraph_WireId(theNodeId.Index));
      if (aFaces != nullptr)
        for (int i = 0; i < aFaces->Length(); ++i)
          markParentSubtreeGen(aFaces->Value(i));
      break;
    }
    case BRepGraph_NodeId::Kind::Face: {
      const NCollection_Vector<BRepGraph_ShellId>* aShells =
        aRevIdx.ShellsOfFace(BRepGraph_FaceId(theNodeId.Index));
      if (aShells != nullptr)
        for (int i = 0; i < aShells->Length(); ++i)
          markParentSubtreeGen(aShells->Value(i));
      break;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      const NCollection_Vector<BRepGraph_SolidId>* aSolids =
        aRevIdx.SolidsOfShell(BRepGraph_ShellId(theNodeId.Index));
      if (aSolids != nullptr)
        for (int i = 0; i < aSolids->Length(); ++i)
          markParentSubtreeGen(aSolids->Value(i));
      break;
    }
    case BRepGraph_NodeId::Kind::Occurrence: {
      // Occurrence modifications propagate to the parent product.
      const BRepGraphInc::OccurrenceDef& anOccDef =
        myData->myIncStorage.Occurrence(BRepGraph_OccurrenceId(theNodeId.Index));
      if (anOccDef.ParentProductDefId.IsValid())
        markParentSubtreeGen(anOccDef.ParentProductDefId);
      break;
    }
    default:
      // Solid/Compound/CompSolid/Product modifications don't propagate further.
      break;
  }
}

//=================================================================================================

void BRepGraph::markRepModified(const BRepGraph_RepId theRepId) noexcept
{
  if (!theRepId.IsValid())
    return;

  BRepGraphInc_Storage&   aStorage = myData->myIncStorage;
  const BRepGraph_RepId::Kind aKind = theRepId.RepKind;
  const int               anIdx    = theRepId.Index;

  // Increment OwnGen on the representation.
  switch (aKind)
  {
    case BRepGraph_RepId::Kind::Surface:
      if (anIdx < aStorage.NbSurfaces())
        ++aStorage.ChangeSurfaceRep(BRepGraph_SurfaceRepId(anIdx)).OwnGen;
      break;
    case BRepGraph_RepId::Kind::Curve3D:
      if (anIdx < aStorage.NbCurves3D())
        ++aStorage.ChangeCurve3DRep(BRepGraph_Curve3DRepId(anIdx)).OwnGen;
      break;
    case BRepGraph_RepId::Kind::Curve2D:
      if (anIdx < aStorage.NbCurves2D())
        ++aStorage.ChangeCurve2DRep(BRepGraph_Curve2DRepId(anIdx)).OwnGen;
      break;
    case BRepGraph_RepId::Kind::Triangulation:
      if (anIdx < aStorage.NbTriangulations())
        ++aStorage.ChangeTriangulationRep(BRepGraph_TriangulationRepId(anIdx)).OwnGen;
      break;
    case BRepGraph_RepId::Kind::Polygon3D:
      if (anIdx < aStorage.NbPolygons3D())
        ++aStorage.ChangePolygon3DRep(BRepGraph_Polygon3DRepId(anIdx)).OwnGen;
      break;
    default:
      return;
  }

  // Propagate mutation to owning topology nodes.
  switch (aKind)
  {
    case BRepGraph_RepId::Kind::Surface:
      for (int i = 0; i < aStorage.NbFaces(); ++i)
        if (aStorage.Face(BRepGraph_FaceId(i)).SurfaceRepId.Index == anIdx)
          markModified(BRepGraph_NodeId::Face(i));
      break;
    case BRepGraph_RepId::Kind::Curve3D:
      for (int i = 0; i < aStorage.NbEdges(); ++i)
        if (aStorage.Edge(BRepGraph_EdgeId(i)).Curve3DRepId.Index == anIdx)
          markModified(BRepGraph_NodeId::Edge(i));
      break;
    case BRepGraph_RepId::Kind::Curve2D:
      for (int i = 0; i < aStorage.NbCoEdges(); ++i)
        if (aStorage.CoEdge(BRepGraph_CoEdgeId(i)).Curve2DRepId.Index == anIdx)
          markModified(BRepGraph_NodeId::CoEdge(i));
      break;
    case BRepGraph_RepId::Kind::Triangulation:
      for (int i = 0; i < aStorage.NbFaces(); ++i)
      {
        const BRepGraphInc::FaceDef& aFace = aStorage.Face(BRepGraph_FaceId(i));
        for (int j = 0; j < aFace.TriangulationRepIds.Length(); ++j)
          if (aFace.TriangulationRepIds.Value(j).Index == anIdx)
          {
            markModified(BRepGraph_NodeId::Face(i));
            break;
          }
      }
      break;
    case BRepGraph_RepId::Kind::Polygon3D:
      for (int i = 0; i < aStorage.NbEdges(); ++i)
        if (aStorage.Edge(BRepGraph_EdgeId(i)).Polygon3DRepId.Index == anIdx)
          markModified(BRepGraph_NodeId::Edge(i));
      break;
    default:
      break;
  }
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

//=================================================================================================

const BRepGraph_History& BRepGraph::History() const
{
  return myData->myHistoryLog;
}

//=================================================================================================

BRepGraph_LayerRegistry& BRepGraph::LayerRegistry()
{
  return myLayerRegistry;
}

//=================================================================================================

const BRepGraph_LayerRegistry& BRepGraph::LayerRegistry() const
{
  return myLayerRegistry;
}

//=================================================================================================

BRepGraphInc_Storage& BRepGraph::incStorage()
{
  return myData->myIncStorage;
}

//=================================================================================================

const BRepGraphInc_Storage& BRepGraph::incStorage() const
{
  return myData->myIncStorage;
}

//=================================================================================================

BRepGraph_Data* BRepGraph::data()
{
  return myData.get();
}

//=================================================================================================

const BRepGraph_Data* BRepGraph::data() const
{
  return myData.get();
}

//=================================================================================================

BRepGraph_LayerRegistry& BRepGraph::layerRegistry()
{
  return myLayerRegistry;
}

//=================================================================================================

const BRepGraph_LayerRegistry& BRepGraph::layerRegistry() const
{
  return myLayerRegistry;
}

//=================================================================================================

BRepGraph_TransientCache& BRepGraph::transientCache()
{
  return myTransientCache;
}

//=================================================================================================

const BRepGraph_TransientCache& BRepGraph::transientCache() const
{
  return myTransientCache;
}
