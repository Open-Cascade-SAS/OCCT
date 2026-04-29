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
#include <BRepGraph_DefsIterator.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraph_RefsView.hxx>

#include <BRepGraph_Builder.hxx>
#include <BRepGraphInc_ReverseIndex.hxx>
#include <BRepGraphInc_Storage.hxx>

#include <MathUtils_Random.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_Map.hxx>
#include <Standard_GUID.hxx>
#include <Standard_ProgramError.hxx>

#include <random>
#include <shared_mutex>

namespace
{

Standard_GUID generateRandomGUID()
{
  std::random_device         aRD;
  MathUtils::RandomGenerator aRNG(aRD());
  const uint64_t             aRand1 = aRNG.NextInt();
  const uint64_t             aRand2 = aRNG.NextInt();
  Standard_UUID              aUUID;
  aUUID.Data1 = static_cast<uint32_t>(aRand1);
  aUUID.Data2 = static_cast<uint16_t>(aRand1 >> 32);
  aUUID.Data3 = static_cast<uint16_t>(aRand1 >> 48);
  for (int i = 0; i < 8; ++i)
  {
    aUUID.Data4[i] = static_cast<uint8_t>(aRand2 >> (i * 8));
  }
  return Standard_GUID(aUUID);
}

} // namespace

//=================================================================================================

void BRepGraph::initViews()
{
  if (myData)
  {
    myData->myTopoView   = TopoView(this);
    myData->myUIDsView   = UIDsView(this);
    myData->myCacheView  = CacheView(this);
    myData->myRefsView   = RefsView(this);
    myData->myShapesView = ShapesView(this);
    myData->myEditorView = EditorView(this);
    myData->myMeshView   = MeshView(this);
  }
  myLayerRegistry.SetOwningGraph(this);
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

BRepGraph::CacheView& BRepGraph::Cache()
{
  return myData->myCacheView;
}

//=================================================================================================

const BRepGraph::CacheView& BRepGraph::Cache() const
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

BRepGraph::EditorView& BRepGraph::Editor()
{
  return myData->myEditorView;
}

//=================================================================================================

const BRepGraph::EditorView& BRepGraph::Editor() const
{
  return myData->myEditorView;
}

//=================================================================================================

const BRepGraph::MeshView& BRepGraph::Mesh() const
{
  return myData->myMeshView;
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

BRepGraph_UID BRepGraph::allocateUID(const BRepGraph_NodeId theNodeId)
{
  // Load counter before append: if Append() throws, no counter is consumed.
  // Single-threaded precondition: UID allocation is only called from Editor
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
  const size_t           aCounter    = myData->myNextUIDCounter.load(std::memory_order_relaxed);
  const uint32_t         aGeneration = myData->myGeneration.load(std::memory_order_relaxed);
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

void BRepGraph::Clear()
{
  myData->myIncStorage.Clear();
  myData->myHistoryLog.Clear();
  myData->myCurrentShapes.Clear();
  myData->myRootProductIds.Clear();
  myTransientCache.Clear();
  {
    std::unique_lock<std::shared_mutex> aUIDLock(myData->myUIDToNodeIdMutex);
    myData->myUIDToNodeId.Clear();
    myData->myUIDToNodeIdDirty      = true;
    myData->myUIDToNodeIdGeneration = myData->myGeneration.load();
  }
  {
    std::unique_lock<std::shared_mutex> aRefUIDLock(myData->myRefUIDToRefIdMutex);
    myData->myRefUIDToRefId.Clear();
    myData->myRefUIDToRefIdDirty      = true;
    myData->myRefUIDToRefIdGeneration = myData->myGeneration.load();
  }
  ++myData->myGeneration;
  myData->myGraphGUID = generateRandomGUID();
  myData->myIsDone    = false;

  myLayerRegistry.ClearAll();
}

//=================================================================================================

bool BRepGraph::IsDone() const
{
  return myData->myIsDone;
}

//=================================================================================================

bool BRepGraph::ValidateReverseIndex() const
{
  return myData->myIncStorage.ValidateReverseIndex();
}

//=================================================================================================

const NCollection_DynamicArray<BRepGraph_ProductId>& BRepGraph::RootProductIds() const
{
  return myData->myRootProductIds;
}

//=================================================================================================

const BRepGraphInc::BaseDef* BRepGraph::topoEntity(const BRepGraph_NodeId theId) const
{
  if (!theId.IsValid())
  {
    return nullptr;
  }
  const BRepGraphInc_Storage& aStorage = myData->myIncStorage;
  switch (theId.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Solid: {
      const BRepGraph_SolidId anId(theId);
      return anId.IsValid(aStorage.NbSolids()) ? &aStorage.Solid(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      const BRepGraph_ShellId anId(theId);
      return anId.IsValid(aStorage.NbShells()) ? &aStorage.Shell(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Face: {
      const BRepGraph_FaceId anId(theId);
      return anId.IsValid(aStorage.NbFaces()) ? &aStorage.Face(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Wire: {
      const BRepGraph_WireId anId(theId);
      return anId.IsValid(aStorage.NbWires()) ? &aStorage.Wire(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Edge: {
      const BRepGraph_EdgeId anId(theId);
      return anId.IsValid(aStorage.NbEdges()) ? &aStorage.Edge(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::CoEdge: {
      const BRepGraph_CoEdgeId anId(theId);
      return anId.IsValid(aStorage.NbCoEdges()) ? &aStorage.CoEdge(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Vertex: {
      const BRepGraph_VertexId anId(theId);
      return anId.IsValid(aStorage.NbVertices()) ? &aStorage.Vertex(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Compound: {
      const BRepGraph_CompoundId anId(theId);
      return anId.IsValid(aStorage.NbCompounds()) ? &aStorage.Compound(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::CompSolid: {
      const BRepGraph_CompSolidId anId(theId);
      return anId.IsValid(aStorage.NbCompSolids()) ? &aStorage.CompSolid(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Product: {
      const BRepGraph_ProductId anId(theId);
      return anId.IsValid(aStorage.NbProducts()) ? &aStorage.Product(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Occurrence: {
      const BRepGraph_OccurrenceId anId(theId);
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
  {
    return nullptr;
  }
  BRepGraphInc_Storage& aStorage = myData->myIncStorage;
  switch (theId.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Solid: {
      const BRepGraph_SolidId anId(theId);
      return anId.IsValid(aStorage.NbSolids()) ? &aStorage.ChangeSolid(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      const BRepGraph_ShellId anId(theId);
      return anId.IsValid(aStorage.NbShells()) ? &aStorage.ChangeShell(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Face: {
      const BRepGraph_FaceId anId(theId);
      return anId.IsValid(aStorage.NbFaces()) ? &aStorage.ChangeFace(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Wire: {
      const BRepGraph_WireId anId(theId);
      return anId.IsValid(aStorage.NbWires()) ? &aStorage.ChangeWire(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Edge: {
      const BRepGraph_EdgeId anId(theId);
      return anId.IsValid(aStorage.NbEdges()) ? &aStorage.ChangeEdge(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::CoEdge: {
      const BRepGraph_CoEdgeId anId(theId);
      return anId.IsValid(aStorage.NbCoEdges()) ? &aStorage.ChangeCoEdge(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Vertex: {
      const BRepGraph_VertexId anId(theId);
      return anId.IsValid(aStorage.NbVertices()) ? &aStorage.ChangeVertex(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Compound: {
      const BRepGraph_CompoundId anId(theId);
      return anId.IsValid(aStorage.NbCompounds()) ? &aStorage.ChangeCompound(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::CompSolid: {
      const BRepGraph_CompSolidId anId(theId);
      return anId.IsValid(aStorage.NbCompSolids()) ? &aStorage.ChangeCompSolid(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Product: {
      const BRepGraph_ProductId anId(theId);
      return anId.IsValid(aStorage.NbProducts()) ? &aStorage.ChangeProduct(anId) : nullptr;
    }
    case BRepGraph_NodeId::Kind::Occurrence: {
      const BRepGraph_OccurrenceId anId(theId);
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
  {
    return;
  }

  using Kind                           = BRepGraph_NodeId::Kind;
  const BRepGraphInc_Storage& aStorage = myData->myIncStorage;

  // Bounds check: ensure the node index is within the entity vector.
  if (topoEntity(theNode) == nullptr)
  {
    return;
  }

  struct StackEntry
  {
    BRepGraph_NodeId Node;
    uint32_t         Depth;
  };

  const uint32_t aNbNodes = aStorage.NbSolids() + aStorage.NbShells() + aStorage.NbFaces()
                            + aStorage.NbWires() + aStorage.NbEdges() + aStorage.NbVertices()
                            + aStorage.NbCompounds() + aStorage.NbCompSolids()
                            + aStorage.NbProducts() + aStorage.NbOccurrences();
  const uint32_t                        aMaxDepth = aNbNodes > 0 ? aNbNodes : 1;
  occ::handle<NCollection_IncAllocator> anAlloc   = new NCollection_IncAllocator();
  NCollection_DynamicArray<StackEntry>  aStack(64, anAlloc);
  NCollection_Map<BRepGraph_NodeId>     aVisited(static_cast<size_t>(aNbNodes), anAlloc);
  aStack.Append({theNode, 0});

  const auto aPushChild = [&](const BRepGraph_NodeId theChild, const uint32_t theDepth) {
    if (!theChild.IsValid())
    {
      return;
    }
    aStack.Append({theChild, theDepth});
  };

  while (!aStack.IsEmpty())
  {
    const StackEntry aCurrent = aStack.Last();
    aStack.EraseLast();

    if (aCurrent.Depth > aMaxDepth || !aCurrent.Node.IsValid() || !aVisited.Add(aCurrent.Node))
    {
      continue;
    }

    // Bounds check: skip nodes whose index is outside the entity vector.
    if (topoEntity(aCurrent.Node) == nullptr)
    {
      continue;
    }

    // Increment OwnGen + SubtreeGen so generation-based cache freshness detects the change.
    BRepGraphInc::BaseDef* anEntity = changeTopoEntity(aCurrent.Node);
    if (anEntity != nullptr)
    {
      ++anEntity->OwnGen;
      ++anEntity->SubtreeGen;
    }

    const uint32_t aNextDepth = aCurrent.Depth + 1;

    switch (aCurrent.Node.NodeKind)
    {
      case Kind::Compound: {
        for (BRepGraph_DefsChildOfCompound aChildIt(*this, BRepGraph_CompoundId(aCurrent.Node));
             aChildIt.More();
             aChildIt.Next())
        {
          aPushChild(aChildIt.CurrentId(), aNextDepth);
        }
        break;
      }
      case Kind::CompSolid: {
        for (BRepGraph_DefsSolidOfCompSolid aChildIt(*this, BRepGraph_CompSolidId(aCurrent.Node));
             aChildIt.More();
             aChildIt.Next())
        {
          aPushChild(aChildIt.CurrentId(), aNextDepth);
        }
        break;
      }
      case Kind::Solid: {
        const BRepGraphInc::SolidDef& aSolidEnt = aStorage.Solid(BRepGraph_SolidId(aCurrent.Node));
        for (BRepGraph_DefsShellOfSolid aChildIt(*this, BRepGraph_SolidId(aCurrent.Node));
             aChildIt.More();
             aChildIt.Next())
        {
          aPushChild(aChildIt.CurrentId(), aNextDepth);
        }
        for (const BRepGraph_ChildRefId& aChildRefId : aSolidEnt.AuxChildRefIds)
        {
          aPushChild(aStorage.ChildRef(aChildRefId).ChildDefId, aNextDepth);
        }
        break;
      }
      case Kind::Shell: {
        const BRepGraphInc::ShellDef& aShellEnt = aStorage.Shell(BRepGraph_ShellId(aCurrent.Node));
        for (BRepGraph_DefsFaceOfShell aChildIt(*this, BRepGraph_ShellId(aCurrent.Node));
             aChildIt.More();
             aChildIt.Next())
        {
          aPushChild(aChildIt.CurrentId(), aNextDepth);
        }
        for (const BRepGraph_ChildRefId& aChildRefId : aShellEnt.AuxChildRefIds)
        {
          aPushChild(aStorage.ChildRef(aChildRefId).ChildDefId, aNextDepth);
        }
        break;
      }
      case Kind::Face: {
        for (BRepGraph_DefsWireOfFace aChildIt(*this, BRepGraph_FaceId(aCurrent.Node));
             aChildIt.More();
             aChildIt.Next())
        {
          aPushChild(aChildIt.CurrentId(), aNextDepth);
        }
        break;
      }
      case Kind::Wire: {
        for (BRepGraph_DefsEdgeOfWire aChildIt(*this, BRepGraph_WireId(aCurrent.Node));
             aChildIt.More();
             aChildIt.Next())
        {
          aPushChild(aChildIt.CurrentId(), aNextDepth);
        }
        break;
      }
      case Kind::Edge: {
        for (BRepGraph_DefsVertexOfEdge aChildIt(*this, BRepGraph_EdgeId(aCurrent.Node));
             aChildIt.More();
             aChildIt.Next())
        {
          aPushChild(aChildIt.CurrentId(), aNextDepth);
        }
        break;
      }
      case Kind::Product: {
        // Product children are occurrences.
        for (BRepGraph_RefsOccurrenceOfProduct aChildIt(*this, BRepGraph_ProductId(aCurrent.Node));
             aChildIt.More();
             aChildIt.Next())
        {
          aPushChild(aStorage.OccurrenceRef(aChildIt.CurrentId()).OccurrenceDefId, aNextDepth);
        }
        break;
      }
      case Kind::Occurrence: {
        const BRepGraphInc::OccurrenceDef& anOcc =
          aStorage.Occurrence(BRepGraph_OccurrenceId(aCurrent.Node));
        aPushChild(anOcc.ChildDefId, aNextDepth);
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
  {
    return;
  }

  BRepGraphInc::BaseDef* anEntity = changeTopoEntity(theNodeId);
  if (anEntity == nullptr)
  {
    return;
  }

  markModified(theNodeId, *anEntity);
}

//=================================================================================================

void BRepGraph::markModified(const BRepGraph_NodeId theNodeId,
                             BRepGraphInc::BaseDef& theEntity) noexcept
{
  ++theEntity.OwnGen;
  ++theEntity.SubtreeGen;
  const uint32_t aWave   = myData->myPropagationWave.fetch_add(1, std::memory_order_relaxed) + 1;
  theEntity.LastPropWave = aWave;

  // In deferred mode: accumulate for batch processing.
  if (myData->myDeferredMode.load(std::memory_order_relaxed))
  {
    myData->myDeferredModified.Append(theNodeId);
    return;
  }

  // Dispatch modification event for the directly mutated node.
  if (myLayerRegistry.HasModificationSubscribers())
  {
    myLayerRegistry.DispatchNodeModified(theNodeId);
  }

  // Propagate SubtreeGen upward to parents (mutex-free).
  propagateSubtreeGen(theNodeId);
}

//=================================================================================================

void BRepGraph::markRefModified(const BRepGraph_RefId theRefId) noexcept
{
  if (!theRefId.IsValid())
  {
    return;
  }

  BRepGraphInc::BaseRef& aRef = myData->myIncStorage.ChangeBaseRef(theRefId);
  markRefModified(theRefId, aRef);
}

//=================================================================================================

void BRepGraph::markRefModified(const BRepGraph_RefId  theRefId,
                                BRepGraphInc::BaseRef& theRef) noexcept
{
  ++theRef.OwnGen;
  myData->myPropagationWave.fetch_add(1, std::memory_order_relaxed);

  // Only dispatch modification events for active (non-removed) refs.
  if (!theRef.IsRemoved)
  {
    if (myData->myDeferredMode.load(std::memory_order_relaxed))
    {
      myData->myDeferredRefModified.Append(theRefId);
    }
    else if (myLayerRegistry.HasRefModificationSubscribers())
    {
      myLayerRegistry.DispatchRefModified(theRefId);
    }
  }

  if (!theRef.ParentId.IsValid())
  {
    return;
  }

  markParentSubtreeGen(theRef.ParentId);
}

//=================================================================================================

void BRepGraph::markParentSubtreeGen(const BRepGraph_NodeId theParentId) noexcept
{
  BRepGraphInc::BaseDef* aParent = changeTopoEntity(theParentId);
  if (aParent == nullptr)
  {
    return;
  }

  const uint32_t aWave = myData->myPropagationWave.load(std::memory_order_relaxed);

  // Re-visit guard: skip if this parent was already processed in the current
  // propagation wave. Prevents exponential blowup on diamond topologies.
  if (aParent->LastPropWave == aWave)
  {
    return;
  }
  aParent->LastPropWave = aWave;

  ++aParent->SubtreeGen; // ONLY SubtreeGen - not OwnGen.
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
      const NCollection_DynamicArray<BRepGraph_WireId>* aWires =
        aRevIdx.WiresOfEdge(BRepGraph_EdgeId(theNodeId));
      if (aWires != nullptr)
      {
        for (const BRepGraph_WireId& aWireId : *aWires)
        {
          markParentSubtreeGen(aWireId);
        }
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Wire: {
      const NCollection_DynamicArray<BRepGraph_FaceId>* aFaces =
        aRevIdx.FacesOfWire(BRepGraph_WireId(theNodeId));
      if (aFaces != nullptr)
      {
        for (const BRepGraph_FaceId& aFaceId : *aFaces)
        {
          markParentSubtreeGen(aFaceId);
        }
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Face: {
      const NCollection_DynamicArray<BRepGraph_ShellId>* aShells =
        aRevIdx.ShellsOfFace(BRepGraph_FaceId(theNodeId));
      if (aShells != nullptr)
      {
        for (const BRepGraph_ShellId& aShellId : *aShells)
        {
          markParentSubtreeGen(aShellId);
        }
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      const NCollection_DynamicArray<BRepGraph_SolidId>* aSolids =
        aRevIdx.SolidsOfShell(BRepGraph_ShellId(theNodeId));
      if (aSolids != nullptr)
      {
        for (const BRepGraph_SolidId& aSolidId : *aSolids)
        {
          markParentSubtreeGen(aSolidId);
        }
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Occurrence: {
      // Occurrence modifications propagate to the parent product.
      // Parent product is on the OccurrenceRef that links to this OccurrenceDef.
      const BRepGraph_OccurrenceId aThisOccId(theNodeId);
      for (BRepGraph_OccurrenceRefIterator aRefIt(*this); aRefIt.More(); aRefIt.Next())
      {
        const BRepGraphInc::OccurrenceRef& aRef = aRefIt.Current();
        if (aRef.OccurrenceDefId == aThisOccId)
        {
          markParentSubtreeGen(BRepGraph_ProductId::FromNodeId(aRef.ParentId));
          break;
        }
      }
      break;
    }
    default: {
      // Solid/Compound/CompSolid/Product: propagate to parent occurrences
      // that reference this node as ChildDefId.
      if (BRepGraph_NodeId::IsTopologyKind(theNodeId.NodeKind)
          || theNodeId.NodeKind == BRepGraph_NodeId::Kind::Product)
      {
        for (BRepGraph_OccurrenceIterator anOccIt(*this); anOccIt.More(); anOccIt.Next())
        {
          if (anOccIt.Current().ChildDefId == theNodeId)
          {
            markParentSubtreeGen(anOccIt.CurrentId());
          }
        }
      }
      break;
    }
  }
}

//=================================================================================================

void BRepGraph::markRepModified(const BRepGraph_RepId theRepId) noexcept
{
  if (!theRepId.IsValid())
  {
    return;
  }

  BRepGraphInc_Storage& aStorage = myData->myIncStorage;

  // Increment OwnGen on the representation.
  switch (theRepId.RepKind)
  {
    case BRepGraph_RepId::Kind::Surface: {
      const BRepGraph_SurfaceRepId aRepId(theRepId);
      if (aRepId.IsValid(aStorage.NbSurfaces()))
      {
        ++aStorage.ChangeSurfaceRep(aRepId).OwnGen;
      }
      break;
    }
    case BRepGraph_RepId::Kind::Curve3D: {
      const BRepGraph_Curve3DRepId aRepId(theRepId);
      if (aRepId.IsValid(aStorage.NbCurves3D()))
      {
        ++aStorage.ChangeCurve3DRep(aRepId).OwnGen;
      }
      break;
    }
    case BRepGraph_RepId::Kind::Curve2D: {
      const BRepGraph_Curve2DRepId aRepId(theRepId);
      if (aRepId.IsValid(aStorage.NbCurves2D()))
      {
        ++aStorage.ChangeCurve2DRep(aRepId).OwnGen;
      }
      break;
    }
    case BRepGraph_RepId::Kind::Triangulation: {
      const BRepGraph_TriangulationRepId aRepId(theRepId);
      if (aRepId.IsValid(aStorage.NbTriangulations()))
      {
        ++aStorage.ChangeTriangulationRep(aRepId).OwnGen;
      }
      break;
    }
    case BRepGraph_RepId::Kind::Polygon3D: {
      const BRepGraph_Polygon3DRepId aRepId(theRepId);
      if (aRepId.IsValid(aStorage.NbPolygons3D()))
      {
        ++aStorage.ChangePolygon3DRep(aRepId).OwnGen;
      }
      break;
    }
    case BRepGraph_RepId::Kind::Polygon2D: {
      const BRepGraph_Polygon2DRepId aRepId(theRepId);
      if (aRepId.IsValid(aStorage.NbPolygons2D()))
      {
        ++aStorage.ChangePolygon2DRep(aRepId).OwnGen;
      }
      break;
    }
    case BRepGraph_RepId::Kind::PolygonOnTri: {
      const BRepGraph_PolygonOnTriRepId aRepId(theRepId);
      if (aRepId.IsValid(aStorage.NbPolygonsOnTri()))
      {
        ++aStorage.ChangePolygonOnTriRep(aRepId).OwnGen;
      }
      break;
    }
    default:
      return;
  }

  // Propagate mutation to owning topology nodes.
  switch (theRepId.RepKind)
  {
    case BRepGraph_RepId::Kind::Surface: {
      const BRepGraph_SurfaceRepId aSurfaceRepId(theRepId);
      for (BRepGraph_FaceIterator aFaceIt(*this); aFaceIt.More(); aFaceIt.Next())
      {
        if (aFaceIt.Current().SurfaceRepId == aSurfaceRepId)
        {
          markModified(aFaceIt.CurrentId());
        }
      }
      break;
    }
    case BRepGraph_RepId::Kind::Curve3D: {
      const BRepGraph_Curve3DRepId aCurve3DRepId(theRepId);
      for (BRepGraph_EdgeIterator anEdgeIt(*this); anEdgeIt.More(); anEdgeIt.Next())
      {
        if (anEdgeIt.Current().Curve3DRepId == aCurve3DRepId)
        {
          markModified(anEdgeIt.CurrentId());
        }
      }
      break;
    }
    case BRepGraph_RepId::Kind::Curve2D: {
      const BRepGraph_Curve2DRepId aCurve2DRepId(theRepId);
      for (BRepGraph_CoEdgeIterator aCoEdgeIt(*this); aCoEdgeIt.More(); aCoEdgeIt.Next())
      {
        if (aCoEdgeIt.Current().Curve2DRepId == aCurve2DRepId)
        {
          markModified(aCoEdgeIt.CurrentId());
        }
      }
      break;
    }
    case BRepGraph_RepId::Kind::Triangulation: {
      const BRepGraph_TriangulationRepId aTriangulationRepId(theRepId);
      for (BRepGraph_FaceIterator aFaceIt(*this); aFaceIt.More(); aFaceIt.Next())
      {
        const BRepGraphInc::FaceDef& aFace   = aFaceIt.Current();
        const BRepGraph_FaceId       aFaceId = aFaceIt.CurrentId();
        bool                         aFound  = aFace.TriangulationRepId == aTriangulationRepId;
        if (!aFound)
        {
          const BRepGraph_MeshCache::FaceMeshEntry* aCached =
            myData->myMeshCache.FindFaceMesh(aFaceId);
          if (aCached != nullptr)
          {
            for (NCollection_DynamicArray<BRepGraph_TriangulationRepId>::Iterator aTriIt(
                   aCached->TriangulationRepIds);
                 aTriIt.More();
                 aTriIt.Next())
            {
              if (aTriIt.Value() == aTriangulationRepId)
              {
                aFound = true;
                break;
              }
            }
          }
        }
        if (aFound)
        {
          markModified(aFaceId);
        }
      }
      break;
    }
    case BRepGraph_RepId::Kind::Polygon3D: {
      const BRepGraph_Polygon3DRepId aPolygon3DRepId(theRepId);
      for (BRepGraph_EdgeIterator anEdgeIt(*this); anEdgeIt.More(); anEdgeIt.Next())
      {
        const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();
        bool                   aFound   = anEdgeIt.Current().Polygon3DRepId == aPolygon3DRepId;
        if (!aFound)
        {
          const BRepGraph_MeshCache::EdgeMeshEntry* aCached =
            myData->myMeshCache.FindEdgeMesh(anEdgeId);
          if (aCached != nullptr && aCached->Polygon3DRepId == aPolygon3DRepId)
          {
            aFound = true;
          }
        }
        if (aFound)
        {
          markModified(anEdgeId);
        }
      }
      break;
    }
    case BRepGraph_RepId::Kind::Polygon2D: {
      const BRepGraph_Polygon2DRepId aPolygon2DRepId(theRepId);
      for (BRepGraph_CoEdgeIterator aCoEdgeIt(*this); aCoEdgeIt.More(); aCoEdgeIt.Next())
      {
        const BRepGraph_CoEdgeId aCoEdgeId = aCoEdgeIt.CurrentId();
        bool                     aFound    = aCoEdgeIt.Current().Polygon2DRepId == aPolygon2DRepId;
        if (!aFound)
        {
          const BRepGraph_MeshCache::CoEdgeMeshEntry* aCached =
            myData->myMeshCache.FindCoEdgeMesh(aCoEdgeId);
          if (aCached != nullptr && aCached->Polygon2DRepId == aPolygon2DRepId)
          {
            aFound = true;
          }
        }
        if (aFound)
        {
          markModified(aCoEdgeId);
        }
      }
      break;
    }
    case BRepGraph_RepId::Kind::PolygonOnTri: {
      const BRepGraph_PolygonOnTriRepId aPolygonOnTriRepId(theRepId);
      for (BRepGraph_CoEdgeIterator aCoEdgeIt(*this); aCoEdgeIt.More(); aCoEdgeIt.Next())
      {
        const BRepGraph_CoEdgeId       aCoEdgeId = aCoEdgeIt.CurrentId();
        const BRepGraphInc::CoEdgeDef& aCoEdge   = aCoEdgeIt.Current();
        bool                           aFound    = aCoEdge.PolygonOnTriRepId == aPolygonOnTriRepId;
        if (!aFound)
        {
          const BRepGraph_MeshCache::CoEdgeMeshEntry* aCached =
            myData->myMeshCache.FindCoEdgeMesh(aCoEdgeId);
          if (aCached != nullptr)
          {
            for (NCollection_DynamicArray<BRepGraph_PolygonOnTriRepId>::Iterator aPolyIt(
                   aCached->PolygonOnTriRepIds);
                 aPolyIt.More();
                 aPolyIt.Next())
            {
              if (aPolyIt.Value() == aPolygonOnTriRepId)
              {
                aFound = true;
                break;
              }
            }
          }
        }
        if (aFound)
        {
          markModified(aCoEdgeId);
        }
      }
      break;
    }
    default:
      break;
  }
}

//=================================================================================================

void BRepGraph::SetAllocator(const occ::handle<NCollection_BaseAllocator>& theAlloc)
{
  Standard_ASSERT_VOID(!myData->myIsDone,
                       "SetAllocator: must be called before BRepGraph_Builder::Add() - "
                       "existing graph state will be lost");

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

//=================================================================================================

BRepGraph_RefTransientCache& BRepGraph::refTransientCache()
{
  return myRefTransientCache;
}

//=================================================================================================

const BRepGraph_RefTransientCache& BRepGraph::refTransientCache() const
{
  return myRefTransientCache;
}

//=================================================================================================

BRepGraph_MeshCacheStorage& BRepGraph::meshCache()
{
  return myData->myMeshCache;
}

//=================================================================================================

const BRepGraph_MeshCacheStorage& BRepGraph::meshCache() const
{
  return myData->myMeshCache;
}

//=================================================================================================

const BRepGraphInc::BaseRef* BRepGraph::refEntity(const BRepGraph_RefId theId) const
{
  if (!theId.IsValid())
  {
    return nullptr;
  }
  const BRepGraphInc_Storage& aStorage = myData->myIncStorage;
  switch (theId.RefKind)
  {
    case BRepGraph_RefId::Kind::Shell: {
      const BRepGraph_ShellRefId anId(theId);
      return anId.IsValid(aStorage.NbShellRefs()) ? &aStorage.ShellRef(anId) : nullptr;
    }
    case BRepGraph_RefId::Kind::Face: {
      const BRepGraph_FaceRefId anId(theId);
      return anId.IsValid(aStorage.NbFaceRefs()) ? &aStorage.FaceRef(anId) : nullptr;
    }
    case BRepGraph_RefId::Kind::Wire: {
      const BRepGraph_WireRefId anId(theId);
      return anId.IsValid(aStorage.NbWireRefs()) ? &aStorage.WireRef(anId) : nullptr;
    }
    case BRepGraph_RefId::Kind::CoEdge: {
      const BRepGraph_CoEdgeRefId anId(theId);
      return anId.IsValid(aStorage.NbCoEdgeRefs()) ? &aStorage.CoEdgeRef(anId) : nullptr;
    }
    case BRepGraph_RefId::Kind::Vertex: {
      const BRepGraph_VertexRefId anId(theId);
      return anId.IsValid(aStorage.NbVertexRefs()) ? &aStorage.VertexRef(anId) : nullptr;
    }
    case BRepGraph_RefId::Kind::Solid: {
      const BRepGraph_SolidRefId anId(theId);
      return anId.IsValid(aStorage.NbSolidRefs()) ? &aStorage.SolidRef(anId) : nullptr;
    }
    case BRepGraph_RefId::Kind::Child: {
      const BRepGraph_ChildRefId anId(theId);
      return anId.IsValid(aStorage.NbChildRefs()) ? &aStorage.ChildRef(anId) : nullptr;
    }
    case BRepGraph_RefId::Kind::Occurrence: {
      const BRepGraph_OccurrenceRefId anId(theId);
      return anId.IsValid(aStorage.NbOccurrenceRefs()) ? &aStorage.OccurrenceRef(anId) : nullptr;
    }
    default:
      return nullptr;
  }
}
