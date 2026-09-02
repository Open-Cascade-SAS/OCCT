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
#include <BRepGraph_LayerHistory.hxx>
#include <BRepGraph_SupplementsView.hxx>
#include <BRepGraph_LayerSupplementRegistry.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <BRepGraphSupInc_Storage.hxx>
#include <MathUtils_Random.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_LinearVector.hxx>
#include <NCollection_Map.hxx>
#include <Standard_GUID.hxx>
#include <Standard_ProgramError.hxx>

#include <random>
#include <shared_mutex>
#include <type_traits>

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

BRepGraph::BRepGraph()
    : myData(std::make_unique<BRepGraph_Data>())
{
  myData->myIncStorage.SetGraphGUID(generateRandomGUID());
  initViewsAndRegistries();
  (void)myData->myLayerRegistry.Ensure<BRepGraph_LayerHistory>();
}

//=================================================================================================

BRepGraph::BRepGraph(const BRepGraphInc_Storage& theStorage,
                     const bool                  theToCloneMutableRepresentations)
    : myData(std::make_unique<BRepGraph_Data>(theStorage))
{
  if (theToCloneMutableRepresentations && !myData->myIncStorage.cloneMutableRepresentations())
  {
    throw Standard_ProgramError("BRepGraph: cannot isolate mutable representations");
  }
  initViewsAndRegistries();
  (void)myData->myLayerRegistry.Ensure<BRepGraph_LayerHistory>();
}

//=================================================================================================

BRepGraph::~BRepGraph()
{
  if (myData != nullptr)
  {
    detachExternalCacheRegistries();
    myData->myLayerRegistry.Detach();
    myData->myLayerSupplementRegistry.Detach();
    myData->myCacheRegistry.Detach();
  }
}

//=================================================================================================

bool BRepGraph::IsValid() const noexcept
{
  return myData != nullptr;
}

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

const BRepGraph::RefsView& BRepGraph::Refs() const
{
  return myData->myRefsView;
}

//=================================================================================================

BRepGraph::ShapesView& BRepGraph::Shapes()
{
  return myData->myShapesView;
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

BRepGraph::MeshView& BRepGraph::Mesh()
{
  return myData->myMeshView;
}

//=================================================================================================

BRepGraph::SupplementsView& BRepGraph::Supplements()
{
  return myData->mySupplementsView;
}

//=================================================================================================

const BRepGraph::SupplementsView& BRepGraph::Supplements() const
{
  return myData->mySupplementsView;
}

//=================================================================================================

BRepGraph::BRepGraph(BRepGraph&& theOther) noexcept
    : myData(std::move(theOther.myData))
{
  initViewsAndRegistries(true);
}

//=================================================================================================

BRepGraph& BRepGraph::operator=(BRepGraph&& theOther) noexcept
{
  if (this != &theOther)
  {
    if (myData != nullptr)
    {
      detachExternalCacheRegistries();
      myData->myLayerRegistry.Detach();
      myData->myLayerSupplementRegistry.Detach();
      myData->myCacheRegistry.Detach();
    }
    myData = std::move(theOther.myData);
    initViewsAndRegistries(true);
  }
  return *this;
}

//=================================================================================================

BRepGraph_UID BRepGraph::allocateUID(const BRepGraph_NodeId theNodeId)
{
  return myData->myIncStorage.AllocateNodeUID(theNodeId);
}

//=================================================================================================

BRepGraph_RefUID BRepGraph::allocateRefUID(const BRepGraph_RefId theRefId)
{
  return myData->myIncStorage.AllocateRefUID(theRefId);
}

//=================================================================================================

bool BRepGraph::RaiseNextNodeUIDCounter(const BRepGraph_NodeId::Kind theKind,
                                        const uint32_t               theNextCounter)
{
  if (!BRepGraph_NodeId::IsValidKind(theKind) || theNextCounter == 0)
  {
    return false;
  }

  const uint32_t aCurrentCounter = myData->myIncStorage.NextNodeUIDCounter(theKind);
  if (theNextCounter < aCurrentCounter)
  {
    return false;
  }
  if (theNextCounter == aCurrentCounter)
  {
    return true;
  }

  myData->myIncStorage.SetNextNodeUIDCounter(theKind, theNextCounter);
  return true;
}

//=================================================================================================

bool BRepGraph::SetNextNodeUIDCounter(const BRepGraph_NodeId::Kind theKind,
                                      const uint32_t               theNextCounter)
{
  if (!BRepGraph_NodeId::IsValidKind(theKind) || theNextCounter == 0)
  {
    return false;
  }
  const uint32_t aCurrentCounter = NextNodeUIDCounter(theKind);
  if (theNextCounter >= aCurrentCounter)
  {
    return RaiseNextNodeUIDCounter(theKind, theNextCounter);
  }
  for (uint32_t aCounter = theNextCounter; aCounter < aCurrentCounter; ++aCounter)
  {
    if (UIDs().NodeIdFrom(BRepGraph_UID(theKind, aCounter)).IsValid())
    {
      return false;
    }
  }
  myData->myIncStorage.SetNextNodeUIDCounter(theKind, theNextCounter);
  return true;
}

//=================================================================================================

bool BRepGraph::RaiseNextRefUIDCounter(const BRepGraph_RefId::Kind theKind,
                                       const uint32_t              theNextCounter)
{
  if (!BRepGraph_RefId::IsValidKind(theKind) || theNextCounter == 0)
  {
    return false;
  }

  const uint32_t aCurrentCounter = myData->myIncStorage.NextRefUIDCounter(theKind);
  if (theNextCounter < aCurrentCounter)
  {
    return false;
  }
  if (theNextCounter == aCurrentCounter)
  {
    return true;
  }

  myData->myIncStorage.SetNextRefUIDCounter(theKind, theNextCounter);
  return true;
}

//=================================================================================================

bool BRepGraph::SetNextRefUIDCounter(const BRepGraph_RefId::Kind theKind,
                                     const uint32_t              theNextCounter)
{
  if (!BRepGraph_RefId::IsValidKind(theKind) || theNextCounter == 0)
  {
    return false;
  }
  const uint32_t aCurrentCounter = NextRefUIDCounter(theKind);
  if (theNextCounter >= aCurrentCounter)
  {
    return RaiseNextRefUIDCounter(theKind, theNextCounter);
  }
  for (uint32_t aCounter = theNextCounter; aCounter < aCurrentCounter; ++aCounter)
  {
    if (UIDs().RefIdFrom(BRepGraph_RefUID(theKind, aCounter)).IsValid())
    {
      return false;
    }
  }
  myData->myIncStorage.SetNextRefUIDCounter(theKind, theNextCounter);
  return true;
}

//=================================================================================================

uint32_t BRepGraph::NextNodeUIDCounter(const BRepGraph_NodeId::Kind theKind) const
{
  return myData->myIncStorage.NextNodeUIDCounter(theKind);
}

//=================================================================================================

uint32_t BRepGraph::NextRefUIDCounter(const BRepGraph_RefId::Kind theKind) const
{
  return myData->myIncStorage.NextRefUIDCounter(theKind);
}

//=================================================================================================

void BRepGraph::Clear()
{
  Standard_ASSERT_RAISE(!myData->myIncStorage.HasAnyGuard(),
                        "BRepGraph::Clear(): guards still active");
  Standard_ASSERT_RAISE(!myData->myIncStorage.DeferredMode(),
                        "BRepGraph::Clear(): deferred invalidation mode is active");
  myData->myIncStorage.Clear();
  myData->mySupplementStorage.Clear();
  myData->myCacheRegistry.ClearAll();
  clearExternalCacheRegistries();
  myData->myIncStorage.IncrementGeneration();
  myData->myIncStorage.SetGraphGUID(generateRandomGUID());

  myData->myLayerRegistry.ClearAll();
  myData->myLayerSupplementRegistry.ClearAll();
}

//=================================================================================================

bool BRepGraph::IsEmpty() const
{
  return myData->myIncStorage.IsEmpty();
}

//=================================================================================================

bool BRepGraph::ValidateRelations() const
{
  return myData->myIncStorage.ValidateRelations();
}

//=================================================================================================

const NCollection_LinearVector<BRepGraph_ProductId>& BRepGraph::RootProductIds() const
{
  return myData->myIncStorage.RootProductIds();
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

bool BRepGraph_NodeId::IsRemoved(const BRepGraph& theGraph) const
{
  if (!IsValid())
  {
    return false;
  }
  const BRepGraphInc_Storage& aS = theGraph.myData->myIncStorage;
  return BRepGraph_NodeId::Visit(*this,
                                 [&aS](const auto theTypedId) { return aS.IsRemoved(theTypedId); });
}

//=================================================================================================

bool BRepGraph_NodeId::IsOwned(const BRepGraph& theGraph) const
{
  if (!IsValid())
  {
    return false;
  }
  const BRepGraphInc_Storage& aS = theGraph.myData->myIncStorage;
  return BRepGraph_NodeId::Visit(*this,
                                 [&aS](const auto theTypedId) { return aS.IsOwned(theTypedId); });
}

//=================================================================================================

bool BRepGraph_RefId::IsRemoved(const BRepGraph& theGraph) const
{
  if (!IsValid())
  {
    return false;
  }
  const BRepGraphInc_Storage& aS = theGraph.myData->myIncStorage;
  return BRepGraph_RefId::Visit(*this,
                                [&aS](const auto theTypedId) { return aS.IsRemoved(theTypedId); });
}

//=================================================================================================

bool BRepGraph_RepId::IsRemoved(const BRepGraph& theGraph) const
{
  if (!IsValid())
  {
    return false;
  }
  const BRepGraphInc_Storage& aS = theGraph.myData->myIncStorage;
  return BRepGraph_RepId::Visit(*this,
                                [&aS](const auto theTypedId) { return aS.IsRemoved(theTypedId); });
}

//=================================================================================================

bool BRepGraph_RefId::IsOwned(const BRepGraph& theGraph) const
{
  if (!IsValid())
  {
    return false;
  }
  const BRepGraphInc_Storage& aS = theGraph.myData->myIncStorage;
  return BRepGraph_RefId::Visit(*this,
                                [&aS](const auto theTypedId) { return aS.IsOwned(theTypedId); });
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

  const uint32_t aNbNodes  = aStorage.NbSolids() + aStorage.NbShells() + aStorage.NbFaces()
                             + aStorage.NbWires() + aStorage.NbEdges() + aStorage.NbVertices()
                             + aStorage.NbCompounds() + aStorage.NbCompSolids()
                             + aStorage.NbProducts() + aStorage.NbOccurrences();
  const uint32_t aMaxDepth = aNbNodes > 0 ? aNbNodes : 1;
  occ::handle<NCollection_IncAllocator> anAlloc = new NCollection_IncAllocator();
  NCollection_LinearVector<StackEntry>  aStack(64);
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

    // Increment OwnGen + SubtreeGen so generation-based cache freshness detects the change.
    BRepGraphInc::BaseDef* anEntity = changeTopoEntity(aCurrent.Node);
    if (anEntity == nullptr)
    {
      continue;
    }
    ++anEntity->OwnGen;
    ++anEntity->SubtreeGen;

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
        for (BRepGraph_DefsShellOfSolid aChildIt(*this, BRepGraph_SolidId(aCurrent.Node));
             aChildIt.More();
             aChildIt.Next())
        {
          aPushChild(aChildIt.CurrentId(), aNextDepth);
        }
        break;
      }
      case Kind::Shell: {
        for (BRepGraph_DefsFaceOfShell aChildIt(*this, BRepGraph_ShellId(aCurrent.Node));
             aChildIt.More();
             aChildIt.Next())
        {
          aPushChild(aChildIt.CurrentId(), aNextDepth);
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
          aPushChild(aStorage.OccurrenceRef(aChildIt.CurrentId()).ChildOccurrenceId, aNextDepth);
        }
        break;
      }
      case Kind::Occurrence: {
        const BRepGraphInc::OccurrenceDef& anOcc =
          aStorage.Occurrence(BRepGraph_OccurrenceId(aCurrent.Node));
        aPushChild(anOcc.ChildNodeId, aNextDepth);
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
  const uint32_t aWave   = myData->myIncStorage.AdvancePropagationWave();
  theEntity.LastPropWave = aWave;

  // In deferred mode: accumulate for batch processing.
  if (myData->myIncStorage.DeferredMode())
  {
    myData->myIncStorage.ChangeDeferredModified().Append(theNodeId);
    return;
  }

  // Dispatch modification event for the directly mutated node.
  if (myData->myLayerRegistry.HasModificationSubscribers())
  {
    myData->myLayerRegistry.DispatchNodeModified(theNodeId);
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

  // Only dispatch modification events for active (non-removed) refs.
  if (!theRefId.IsRemoved(*this))
  {
    if (myData->myIncStorage.DeferredMode())
    {
      myData->myIncStorage.ChangeDeferredRefModified().Append(theRefId);
    }
    else if (myData->myLayerRegistry.HasRefModificationSubscribers())
    {
      myData->myLayerRegistry.DispatchRefModified(theRefId);
    }
  }

  const BRepGraphInc_Storage& aStorage   = myData->myIncStorage;
  const auto                  aPropagate = [this, &aStorage](const auto theTypedRef) noexcept {
    using RefId = std::remove_cv_t<decltype(theTypedRef)>;
    if constexpr (std::is_same_v<RefId, BRepGraph_VertexRefId>)
    {
      const BRepGraph_VertexRefId aRefId = theTypedRef;
      if (!aRefId.IsValid(aStorage.NbVertexRefs()))
      {
        return;
      }
      const BRepGraph_VertexId aVertexId = aStorage.VertexRef(aRefId).ChildVertexId;
      if (!aVertexId.IsValid(aStorage.NbVertices()))
      {
        return;
      }
      for (const BRepGraph_EdgeId& anEdgeId : aStorage.VertexRelations(aVertexId).EdgeIds)
      {
        if (!anEdgeId.IsValid(aStorage.NbEdges()))
        {
          continue;
        }
        const BRepGraphInc::EdgeDef& anEdge = aStorage.Edge(anEdgeId);
        if (!anEdgeId.IsRemoved(*this)
            && (anEdge.StartVertexRefId == aRefId || anEdge.EndVertexRefId == aRefId))
        {
          markModified(anEdgeId);
        }
      }
    }
    else if constexpr (std::is_same_v<RefId, BRepGraph_WireRefId>)
    {
      const BRepGraph_WireRefId aRefId = theTypedRef;
      if (!aRefId.IsValid(aStorage.NbWireRefs()))
      {
        return;
      }
      const BRepGraph_FaceId aFaceId = aStorage.WireRef(aRefId).ParentFaceId;
      if (aFaceId.IsValid(aStorage.NbFaces()) && !aFaceId.IsRemoved(*this))
      {
        markModified(aFaceId);
      }
    }
    else if constexpr (std::is_same_v<RefId, BRepGraph_FaceRefId>)
    {
      const BRepGraph_FaceRefId aRefId = theTypedRef;
      if (!aRefId.IsValid(aStorage.NbFaceRefs()))
      {
        return;
      }
      const BRepGraph_ShellId aShellId = aStorage.FaceRef(aRefId).ParentShellId;
      if (aShellId.IsValid(aStorage.NbShells()) && !aShellId.IsRemoved(*this))
      {
        markModified(aShellId);
      }
    }
    else if constexpr (std::is_same_v<RefId, BRepGraph_ShellRefId>)
    {
      const BRepGraph_ShellRefId aRefId = theTypedRef;
      if (!aRefId.IsValid(aStorage.NbShellRefs()))
      {
        return;
      }
      const BRepGraph_SolidId aSolidId = aStorage.ShellRef(aRefId).ParentSolidId;
      if (aSolidId.IsValid(aStorage.NbSolids()) && !aSolidId.IsRemoved(*this))
      {
        markModified(aSolidId);
      }
    }
    else if constexpr (std::is_same_v<RefId, BRepGraph_SolidRefId>)
    {
      const BRepGraph_SolidRefId aRefId = theTypedRef;
      if (!aRefId.IsValid(aStorage.NbSolidRefs()))
      {
        return;
      }
      const BRepGraph_CompSolidId aCompSolidId = aStorage.SolidRef(aRefId).ParentCompSolidId;
      if (aCompSolidId.IsValid(aStorage.NbCompSolids()) && !aCompSolidId.IsRemoved(*this))
      {
        markModified(aCompSolidId);
      }
    }
    else if constexpr (std::is_same_v<RefId, BRepGraph_ChildRefId>)
    {
      const BRepGraph_ChildRefId aRefId = theTypedRef;
      if (!aRefId.IsValid(aStorage.NbChildRefs()))
      {
        return;
      }
      const BRepGraph_CompoundId aCompoundId = aStorage.ChildRef(aRefId).ParentCompoundId;
      if (aCompoundId.IsValid(aStorage.NbCompounds()) && !aCompoundId.IsRemoved(*this))
      {
        markModified(aCompoundId);
      }
    }
    else if constexpr (std::is_same_v<RefId, BRepGraph_OccurrenceRefId>)
    {
      const BRepGraph_OccurrenceRefId aRefId = theTypedRef;
      if (!aRefId.IsValid(aStorage.NbOccurrenceRefs()))
      {
        return;
      }
      const BRepGraph_ProductId aProductId = aStorage.OccurrenceRef(aRefId).ParentProductId;
      if (aProductId.IsValid(aStorage.NbProducts()) && !aProductId.IsRemoved(*this))
      {
        markModified(aProductId);
      }
    }
  };
  BRepGraph_RefId::Visit(theRefId, aPropagate);
}

//=================================================================================================

void BRepGraph::markParentSubtreeGen(const BRepGraph_NodeId theParentId) noexcept
{
  BRepGraphInc::BaseDef* aParent = changeTopoEntity(theParentId);
  if (aParent == nullptr)
  {
    return;
  }

  const uint32_t aWave = myData->myIncStorage.PropagationWave();

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
  const BRepGraphInc_Storage& aStorage = myData->myIncStorage;
  if (BRepGraph_NodeId::IsTopologyKind(theNodeId.NodeKind)
      || theNodeId.NodeKind == BRepGraph_NodeId::Kind::Product)
  {
    for (const BRepGraph_ChildRefId& aRefId : aStorage.CompoundRefsOfNode(theNodeId))
    {
      if (!aRefId.IsValid(aStorage.NbChildRefs()) || aStorage.IsRemoved(aRefId))
      {
        continue;
      }
      const BRepGraphInc::ChildRef& aRef = aStorage.ChildRef(aRefId);
      if (aRef.ChildNodeId == theNodeId && aRef.ParentCompoundId.IsValid(aStorage.NbCompounds())
          && !aStorage.IsRemoved(aRef.ParentCompoundId))
      {
        markParentSubtreeGen(aRef.ParentCompoundId);
      }
    }
  }
  switch (theNodeId.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex: {
      for (const BRepGraph_EdgeId& anEdgeId :
           aStorage.VertexRelations(BRepGraph_VertexId(theNodeId)).EdgeIds)
      {
        markParentSubtreeGen(anEdgeId);
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Edge: {
      for (const BRepGraph_CoEdgeId& aCoEdgeId :
           aStorage.EdgeRelations(BRepGraph_EdgeId(theNodeId)).CoEdgeIds)
      {
        if (!aCoEdgeId.IsValid(aStorage.NbCoEdges()) || aStorage.IsRemoved(aCoEdgeId))
        {
          continue;
        }
        const BRepGraph_WireId aWireId = aStorage.CoEdge(aCoEdgeId).ParentWireId;
        if (aWireId.IsValid(aStorage.NbWires()) && !aStorage.IsRemoved(aWireId))
        {
          markParentSubtreeGen(aWireId);
        }
      }
      break;
    }
    case BRepGraph_NodeId::Kind::CoEdge: {
      const BRepGraph_CoEdgeId aCoEdgeId(theNodeId);
      if (aCoEdgeId.IsValid(aStorage.NbCoEdges()) && !aStorage.IsRemoved(aCoEdgeId))
      {
        const BRepGraph_WireId aWireId = aStorage.CoEdge(aCoEdgeId).ParentWireId;
        if (aWireId.IsValid(aStorage.NbWires()))
        {
          markParentSubtreeGen(aWireId);
        }
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Wire: {
      for (const BRepGraph_WireRefId& aRefId :
           aStorage.WireRelations(BRepGraph_WireId(theNodeId)).ParentWireRefIds)
      {
        if (aRefId.IsValid(aStorage.NbWireRefs()) && !aStorage.IsRemoved(aRefId))
        {
          markParentSubtreeGen(aStorage.WireRef(aRefId).ParentFaceId);
        }
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Face: {
      for (const BRepGraph_FaceRefId& aRefId :
           aStorage.FaceRelations(BRepGraph_FaceId(theNodeId)).ParentFaceRefIds)
      {
        if (aRefId.IsValid(aStorage.NbFaceRefs()) && !aStorage.IsRemoved(aRefId))
        {
          markParentSubtreeGen(aStorage.FaceRef(aRefId).ParentShellId);
        }
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      for (const BRepGraph_ShellRefId& aRefId :
           aStorage.ShellRelations(BRepGraph_ShellId(theNodeId)).ParentShellRefIds)
      {
        if (aRefId.IsValid(aStorage.NbShellRefs()) && !aStorage.IsRemoved(aRefId))
        {
          markParentSubtreeGen(aStorage.ShellRef(aRefId).ParentSolidId);
        }
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Occurrence: {
      const BRepGraph_OccurrenceId aThisOccId(theNodeId);
      for (const BRepGraph_OccurrenceRefId& aRefId :
           aStorage.OccurrenceRelations(aThisOccId).ParentOccurrenceRefIds)
      {
        if (!aRefId.IsValid(aStorage.NbOccurrenceRefs()) || aStorage.IsRemoved(aRefId))
        {
          continue;
        }
        const BRepGraphInc::OccurrenceRef& aRef = aStorage.OccurrenceRef(aRefId);
        if (aRef.ParentProductId.IsValid(aStorage.NbProducts())
            && !aStorage.IsRemoved(aRef.ParentProductId))
        {
          markParentSubtreeGen(aRef.ParentProductId);
        }
      }
      break;
    }
    default: {
      // Solid/Compound/CompSolid/Product: propagate to parent occurrences
      // that reference this node as ChildNodeId.
      if (BRepGraph_NodeId::IsTopologyKind(theNodeId.NodeKind)
          || theNodeId.NodeKind == BRepGraph_NodeId::Kind::Product)
      {
        for (const BRepGraph_OccurrenceRefId& aRefId : aStorage.OccurrenceRefsOfNode(theNodeId))
        {
          if (!aRefId.IsValid(aStorage.NbOccurrenceRefs()) || aStorage.IsRemoved(aRefId))
          {
            continue;
          }
          const BRepGraph_OccurrenceId anOccurrenceId =
            aStorage.OccurrenceRef(aRefId).ChildOccurrenceId;
          if (anOccurrenceId.IsValid(aStorage.NbOccurrences())
              && !aStorage.IsRemoved(anOccurrenceId))
          {
            markParentSubtreeGen(anOccurrenceId);
          }
        }
      }
      break;
    }
  }
}

//=================================================================================================

const occ::handle<NCollection_BaseAllocator>& BRepGraph::Allocator() const
{
  return myData->myIncStorage.Allocator();
}

//=================================================================================================

BRepGraph_LayerRegistry& BRepGraph::LayerRegistry()
{
  return myData->myLayerRegistry;
}

//=================================================================================================

const BRepGraph_LayerRegistry& BRepGraph::LayerRegistry() const
{
  return myData->myLayerRegistry;
}

//=================================================================================================

BRepGraph_LayerSupplementRegistry& BRepGraph::LayerSupplementRegistry()
{
  return myData->myLayerSupplementRegistry;
}

//=================================================================================================

const BRepGraph_LayerSupplementRegistry& BRepGraph::LayerSupplementRegistry() const
{
  return myData->myLayerSupplementRegistry;
}

//=================================================================================================

BRepGraph_CacheRegistry& BRepGraph::CacheRegistry()
{
  return myData->myCacheRegistry;
}

//=================================================================================================

const BRepGraph_CacheRegistry& BRepGraph::CacheRegistry() const
{
  return myData->myCacheRegistry;
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

BRepGraphSupInc_Storage& BRepGraph::supplementStorage()
{
  return myData->mySupplementStorage;
}

//=================================================================================================

const BRepGraphSupInc_Storage& BRepGraph::supplementStorage() const
{
  return myData->mySupplementStorage;
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
  return myData->myLayerRegistry;
}

//=================================================================================================

const BRepGraph_LayerRegistry& BRepGraph::layerRegistry() const
{
  return myData->myLayerRegistry;
}

//=================================================================================================

BRepGraph_LayerSupplementRegistry& BRepGraph::layerSupplementRegistry()
{
  return myData->myLayerSupplementRegistry;
}

//=================================================================================================

const BRepGraph_LayerSupplementRegistry& BRepGraph::layerSupplementRegistry() const
{
  return myData->myLayerSupplementRegistry;
}

//=================================================================================================

BRepGraph_CacheRegistry& BRepGraph::cacheRegistry()
{
  return myData->myCacheRegistry;
}

//=================================================================================================

const BRepGraph_CacheRegistry& BRepGraph::cacheRegistry() const
{
  return myData->myCacheRegistry;
}

//=================================================================================================

void BRepGraph::registerExternalCacheRegistry(BRepGraph_CacheRegistry* theRegistry)
{
  if (theRegistry == nullptr)
  {
    return;
  }
  for (BRepGraph_CacheRegistry* aRegistry : myData->myExternalCacheRegistries)
  {
    if (aRegistry == theRegistry)
    {
      return;
    }
  }
  myData->myExternalCacheRegistries.Append(theRegistry);
}

//=================================================================================================

void BRepGraph::unregisterExternalCacheRegistry(BRepGraph_CacheRegistry* theRegistry) noexcept
{
  if (theRegistry == nullptr || myData == nullptr)
  {
    return;
  }
  for (size_t anIndex = 0; anIndex < myData->myExternalCacheRegistries.Size(); ++anIndex)
  {
    if (myData->myExternalCacheRegistries.Value(anIndex) == theRegistry)
    {
      myData->myExternalCacheRegistries.Erase(anIndex);
      return;
    }
  }
}

//=================================================================================================

void BRepGraph::replaceExternalCacheRegistry(BRepGraph_CacheRegistry* theOldRegistry,
                                             BRepGraph_CacheRegistry* theNewRegistry) noexcept
{
  if (theOldRegistry == nullptr || theNewRegistry == nullptr || myData == nullptr)
  {
    return;
  }
  for (size_t anIndex = 0; anIndex < myData->myExternalCacheRegistries.Size(); ++anIndex)
  {
    if (myData->myExternalCacheRegistries.Value(anIndex) == theOldRegistry)
    {
      myData->myExternalCacheRegistries.ChangeValue(anIndex) = theNewRegistry;
      return;
    }
  }
}

//=================================================================================================

void BRepGraph::detachExternalCacheRegistries() noexcept
{
  if (myData == nullptr)
  {
    return;
  }
  while (!myData->myExternalCacheRegistries.IsEmpty())
  {
    BRepGraph_CacheRegistry* aRegistry = myData->myExternalCacheRegistries.Last();
    myData->myExternalCacheRegistries.EraseLast();
    if (aRegistry != nullptr)
    {
      aRegistry->detachGraphFromOwner(this);
    }
  }
}

//=================================================================================================

void BRepGraph::clearExternalCacheRegistries() noexcept
{
  if (myData == nullptr)
  {
    return;
  }
  for (BRepGraph_CacheRegistry* aRegistry : myData->myExternalCacheRegistries)
  {
    if (aRegistry != nullptr)
    {
      aRegistry->ClearAll();
    }
  }
}

//=================================================================================================

void BRepGraph::initViewsAndRegistries(const bool theIsOwnerRelocation) noexcept
{
  if (myData == nullptr)
  {
    return;
  }
  myData->myTopoView        = TopoView(this);
  myData->myUIDsView        = UIDsView(this);
  myData->myRefsView        = RefsView(this);
  myData->myShapesView      = ShapesView(this);
  myData->myEditorView      = EditorView(this);
  myData->myMeshView        = MeshView(this);
  myData->mySupplementsView = SupplementsView(this);
  if (theIsOwnerRelocation)
  {
    myData->myLayerRegistry.Relocate(this);
    myData->myLayerSupplementRegistry.Relocate(this);
    myData->myCacheRegistry.rebindGraph(this);
  }
  else
  {
    myData->myLayerRegistry.Attach(this);
    myData->myLayerSupplementRegistry.Attach(this);
    myData->myCacheRegistry.attachGraph(this);
  }
  for (BRepGraph_CacheRegistry* aRegistry : myData->myExternalCacheRegistries)
  {
    if (aRegistry != nullptr)
    {
      aRegistry->rebindGraph(this);
    }
  }
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
