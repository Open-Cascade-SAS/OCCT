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

#include <BRepGraph_Cache.hxx>

#include <BRepGraph.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_CacheRegistry.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraph_RefsView.hxx>
#include <Standard_ProgramError.hxx>

#include <thread>

IMPLEMENT_STANDARD_RTTIEXT(BRepGraph_Cache, Standard_Transient)

//=================================================================================================

BRepGraph_Cache::BRepGraph_Cache() = default;

//=================================================================================================

void BRepGraph_Cache::NodeEntry::Reset() noexcept
{
  myNode       = BRepGraph_NodeId();
  myGeneration = 0;
  myKind       = GenKind::None;
}

//=================================================================================================

bool BRepGraph_Cache::NodeEntry::BindOwnGen(const BRepGraph_Cache& theCache,
                                            const BRepGraph_NodeId theNode) noexcept
{
  return bind(theCache, theNode, GenKind::Own);
}

//=================================================================================================

bool BRepGraph_Cache::NodeEntry::BindSubtreeGen(const BRepGraph_Cache& theCache,
                                                const BRepGraph_NodeId theNode) noexcept
{
  return bind(theCache, theNode, GenKind::Subtree);
}

//=================================================================================================

bool BRepGraph_Cache::NodeEntry::IsFreshOwn(const BRepGraph_Cache& theCache,
                                            const BRepGraph_NodeId theNode) const noexcept
{
  return isFresh(theCache, theNode, GenKind::Own);
}

//=================================================================================================

bool BRepGraph_Cache::NodeEntry::IsFreshSubtree(const BRepGraph_Cache& theCache,
                                                const BRepGraph_NodeId theNode) const noexcept
{
  return isFresh(theCache, theNode, GenKind::Subtree);
}

//=================================================================================================

bool BRepGraph_Cache::NodeEntry::IsFresh(const BRepGraph_Cache& theCache) const noexcept
{
  return isFresh(theCache, myNode, myKind);
}

//=================================================================================================

bool BRepGraph_Cache::NodeEntry::bind(const BRepGraph_Cache& theCache,
                                      const BRepGraph_NodeId theNode,
                                      const GenKind          theKind) noexcept
{
  uint32_t   aGeneration = 0;
  const bool isValid = theKind == GenKind::Own       ? theCache.NodeOwnGen(theNode, aGeneration)
                       : theKind == GenKind::Subtree ? theCache.NodeSubtreeGen(theNode, aGeneration)
                                                     : false;
  if (!isValid)
  {
    Reset();
    return false;
  }

  myNode       = theNode;
  myGeneration = aGeneration;
  myKind       = theKind;
  return true;
}

//=================================================================================================

bool BRepGraph_Cache::NodeEntry::isFresh(const BRepGraph_Cache& theCache,
                                         const BRepGraph_NodeId theNode,
                                         const GenKind          theKind) const noexcept
{
  if (myKind != theKind || myNode != theNode)
  {
    return false;
  }

  uint32_t   aGeneration = 0;
  const bool isValid = theKind == GenKind::Own       ? theCache.NodeOwnGen(theNode, aGeneration)
                       : theKind == GenKind::Subtree ? theCache.NodeSubtreeGen(theNode, aGeneration)
                                                     : false;
  return isValid && aGeneration == myGeneration;
}

//=================================================================================================

void BRepGraph_Cache::RefEntry::Reset() noexcept
{
  myRef        = BRepGraph_RefId();
  myGeneration = 0;
}

//=================================================================================================

bool BRepGraph_Cache::RefEntry::BindOwnGen(const BRepGraph_Cache& theCache,
                                           const BRepGraph_RefId  theRef) noexcept
{
  uint32_t aGeneration = 0;
  if (!theCache.RefOwnGen(theRef, aGeneration))
  {
    Reset();
    return false;
  }

  myRef        = theRef;
  myGeneration = aGeneration;
  return true;
}

//=================================================================================================

bool BRepGraph_Cache::RefEntry::IsFreshOwn(const BRepGraph_Cache& theCache,
                                           const BRepGraph_RefId  theRef) const noexcept
{
  if (!myRef.IsValid() || myRef != theRef)
  {
    return false;
  }

  uint32_t aGeneration = 0;
  return theCache.RefOwnGen(theRef, aGeneration) && aGeneration == myGeneration;
}

//=================================================================================================

bool BRepGraph_Cache::RefEntry::IsFresh(const BRepGraph_Cache& theCache) const noexcept
{
  return IsFreshOwn(theCache, myRef);
}

//=================================================================================================

void BRepGraph_Cache::ItemEntry::Reset() noexcept
{
  myItem       = BRepGraph_ItemId();
  myGeneration = 0;
}

//=================================================================================================

bool BRepGraph_Cache::ItemEntry::BindOwnGen(const BRepGraph_Cache& theCache,
                                            const BRepGraph_ItemId theItem) noexcept
{
  uint32_t aGeneration = 0;
  if (!theCache.ItemOwnGen(theItem, aGeneration))
  {
    Reset();
    return false;
  }

  myItem       = theItem;
  myGeneration = aGeneration;
  return true;
}

//=================================================================================================

bool BRepGraph_Cache::ItemEntry::IsFreshOwn(const BRepGraph_Cache& theCache,
                                            const BRepGraph_ItemId theItem) const noexcept
{
  if (!myItem.IsValid() || myItem != theItem)
  {
    return false;
  }

  uint32_t aGeneration = 0;
  return theCache.ItemOwnGen(theItem, aGeneration) && aGeneration == myGeneration;
}

//=================================================================================================

bool BRepGraph_Cache::ItemEntry::IsFresh(const BRepGraph_Cache& theCache) const noexcept
{
  return IsFreshOwn(theCache, myItem);
}

//=================================================================================================

void BRepGraph_Cache::Clear() noexcept {}

//=================================================================================================

void BRepGraph_Cache::CopyFreshTo(const BRepGraph_CopyRemap&) const {}

//=================================================================================================

const BRepGraph& BRepGraph_Cache::Graph() const
{
  const BRepGraph* aGraph = AttachedGraph();
  if (aGraph == nullptr)
  {
    throw Standard_ProgramError("BRepGraph_Cache: cache is detached from graph");
  }
  return *aGraph;
}

//=================================================================================================

BRepGraph* BRepGraph_Cache::AttachedGraph() const noexcept
{
  LifecycleState aState = myLifecycleState.load(std::memory_order_acquire);
  while (aState == LifecycleState::Attaching)
  {
    std::this_thread::yield();
    aState = myLifecycleState.load(std::memory_order_acquire);
  }
  if (aState == LifecycleState::Detached)
  {
    return nullptr;
  }
  BRepGraph_CacheRegistry* aRegistry = myRegistry.load(std::memory_order_acquire);
  return aRegistry == nullptr ? nullptr : aRegistry->AttachedGraph();
}

//=================================================================================================

bool BRepGraph_Cache::IsAttached() const noexcept
{
  return AttachedGraph() != nullptr;
}

//=================================================================================================

void BRepGraph_Cache::OnAttached() noexcept {}

//=================================================================================================

void BRepGraph_Cache::OnDetached() noexcept {}

//=================================================================================================

bool BRepGraph_Cache::NodeSubtreeGen(const BRepGraph_NodeId theNode,
                                     uint32_t&              theGen) const noexcept
{
  const BRepGraph* aGraph = AttachedGraph();
  if (aGraph == nullptr)
  {
    return false;
  }
  const BRepGraphInc::BaseDef* aDef = aGraph->topoEntity(theNode);
  if (aDef == nullptr || theNode.IsRemoved(*aGraph))
  {
    return false;
  }
  theGen = aDef->SubtreeGen;
  return true;
}

//=================================================================================================

bool BRepGraph_Cache::NodeOwnGen(const BRepGraph_NodeId theNode, uint32_t& theGen) const noexcept
{
  const BRepGraph* aGraph = AttachedGraph();
  if (aGraph == nullptr)
  {
    return false;
  }
  const BRepGraphInc::BaseDef* aDef = aGraph->topoEntity(theNode);
  if (aDef == nullptr || theNode.IsRemoved(*aGraph))
  {
    return false;
  }
  theGen = aDef->OwnGen;
  return true;
}

//=================================================================================================

bool BRepGraph_Cache::RefOwnGen(const BRepGraph_RefId theRef, uint32_t& theGen) const noexcept
{
  const BRepGraph* aGraph = AttachedGraph();
  if (aGraph == nullptr)
  {
    return false;
  }
  if (theRef.IsRemoved(*aGraph))
  {
    return false;
  }
  // Reference freshness uses the parent node OwnGen.
  const BRepGraphInc_Storage& aStorage = aGraph->myData->myIncStorage;
  BRepGraph_NodeId            aParent;
  switch (theRef.RefKind)
  {
    case BRepGraph_RefId::Kind::Vertex: {
      const BRepGraph_VertexRefId aRefId(theRef);
      if (!aRefId.IsValid(aStorage.NbVertexRefs()))
      {
        return false;
      }
      // VertexRef parent is the edge referencing it - find via relations.
      const BRepGraph_VertexId aVtxId = aStorage.VertexRef(aRefId).ChildVertexId;
      if (!aVtxId.IsValid(aStorage.NbVertices()))
      {
        return false;
      }
      for (const BRepGraph_EdgeId& anEdgeId : aStorage.VertexRelations(aVtxId).EdgeIds)
      {
        if (anEdgeId.IsValid(aStorage.NbEdges()))
        {
          const BRepGraphInc::EdgeDef& anEdge = aStorage.Edge(anEdgeId);
          if (anEdge.StartVertexRefId == aRefId || anEdge.EndVertexRefId == aRefId)
          {
            aParent = anEdgeId;
            break;
          }
        }
      }
      break;
    }
    case BRepGraph_RefId::Kind::Wire: {
      const BRepGraph_WireRefId aRefId(theRef);
      if (!aRefId.IsValid(aStorage.NbWireRefs()))
      {
        return false;
      }
      aParent = aStorage.WireRef(aRefId).ParentFaceId;
      break;
    }
    case BRepGraph_RefId::Kind::Face: {
      const BRepGraph_FaceRefId aRefId(theRef);
      if (!aRefId.IsValid(aStorage.NbFaceRefs()))
      {
        return false;
      }
      aParent = aStorage.FaceRef(aRefId).ParentShellId;
      break;
    }
    case BRepGraph_RefId::Kind::Shell: {
      const BRepGraph_ShellRefId aRefId(theRef);
      if (!aRefId.IsValid(aStorage.NbShellRefs()))
      {
        return false;
      }
      aParent = aStorage.ShellRef(aRefId).ParentSolidId;
      break;
    }
    case BRepGraph_RefId::Kind::Solid: {
      const BRepGraph_SolidRefId aRefId(theRef);
      if (!aRefId.IsValid(aStorage.NbSolidRefs()))
      {
        return false;
      }
      aParent = aStorage.SolidRef(aRefId).ParentCompSolidId;
      break;
    }
    case BRepGraph_RefId::Kind::Child: {
      const BRepGraph_ChildRefId aRefId(theRef);
      if (!aRefId.IsValid(aStorage.NbChildRefs()))
      {
        return false;
      }
      aParent = aStorage.ChildRef(aRefId).ParentCompoundId;
      break;
    }
    case BRepGraph_RefId::Kind::Occurrence: {
      const BRepGraph_OccurrenceRefId aRefId(theRef);
      if (!aRefId.IsValid(aStorage.NbOccurrenceRefs()))
      {
        return false;
      }
      aParent = aStorage.OccurrenceRef(aRefId).ParentProductId;
      break;
    }
    default:
      return false;
  }
  if (!aParent.IsValid())
  {
    return false;
  }
  const BRepGraphInc::BaseDef* aDef = aGraph->topoEntity(aParent);
  if (aDef == nullptr)
  {
    return false;
  }
  theGen = aDef->OwnGen;
  return true;
}

//=================================================================================================

bool BRepGraph_Cache::ItemOwnGen(const BRepGraph_ItemId theItem, uint32_t& theGen) const noexcept
{
  if (!theItem.IsValid())
  {
    return false;
  }

  switch (theItem.ItemDomain())
  {
    case BRepGraph_ItemId::Domain::Node:
      return NodeOwnGen(theItem.NodeId(), theGen);
    case BRepGraph_ItemId::Domain::Reference:
      return RefOwnGen(theItem.RefId(), theGen);
    case BRepGraph_ItemId::Domain::None:
      return false;
  }
  return false;
}

//=================================================================================================

bool BRepGraph_Cache::ResolveActiveRefChild(const BRepGraph_RefId theRef,
                                            BRepGraph_NodeId&     theNode) const noexcept
{
  theNode                 = BRepGraph_NodeId();
  const BRepGraph* aGraph = AttachedGraph();
  if (aGraph == nullptr)
  {
    return false;
  }

  const BRepGraphInc::BaseRef* aRef = aGraph->refEntity(theRef);
  if (aRef == nullptr || theRef.IsRemoved(*aGraph))
  {
    return false;
  }

  const BRepGraph_NodeId       aNode = aGraph->Refs().Gen().ChildNode(theRef);
  const BRepGraphInc::BaseDef* aDef  = aGraph->topoEntity(aNode);
  if (aDef == nullptr || aNode.IsRemoved(*aGraph))
  {
    return false;
  }

  theNode = aNode;
  return true;
}

//=================================================================================================

bool BRepGraph_Cache::ResolveActiveFaceRef(const BRepGraph_FaceRefId theRef,
                                           BRepGraph_FaceId&         theFace) const noexcept
{
  BRepGraph_NodeId aNode;
  if (!ResolveActiveRefChild(BRepGraph_RefId(theRef), aNode)
      || aNode.NodeKind != BRepGraph_NodeId::Kind::Face)
  {
    theFace = BRepGraph_FaceId();
    return false;
  }

  theFace = BRepGraph_FaceId::FromNodeId(aNode);
  return true;
}

//=================================================================================================

void BRepGraph_Cache::attachGraph(BRepGraph_CacheRegistry* theRegistry) noexcept
{
  myRegistry.store(theRegistry, std::memory_order_release);
}

//=================================================================================================

void BRepGraph_Cache::beginAttach() noexcept
{
  myWasCleared.store(false, std::memory_order_release);
  myLifecycleState.store(LifecycleState::Attaching, std::memory_order_release);
}

//=================================================================================================

void BRepGraph_Cache::notifyAttached() noexcept
{
  LifecycleState anExpectedState = LifecycleState::Attaching;
  if (!myLifecycleState.compare_exchange_strong(anExpectedState,
                                                LifecycleState::Attached,
                                                std::memory_order_acq_rel))
  {
    return;
  }
  OnAttached();
}

//=================================================================================================

void BRepGraph_Cache::rebindRegistry(BRepGraph_CacheRegistry* theRegistry) noexcept
{
  myRegistry.store(theRegistry, std::memory_order_release);
}

//=================================================================================================

void BRepGraph_Cache::detachGraph() noexcept
{
  if (myWasCleared.exchange(true, std::memory_order_acq_rel))
  {
    return;
  }
  myLifecycleState.store(LifecycleState::Detaching, std::memory_order_release);
  Clear();
  OnDetached();
  LifecycleState aState = LifecycleState::Detaching;
  myLifecycleState.compare_exchange_strong(aState,
                                           LifecycleState::Detached,
                                           std::memory_order_acq_rel);
}

//=================================================================================================

void BRepGraph_Cache::clearForRelease() noexcept
{
  if (!myWasCleared.exchange(true, std::memory_order_acq_rel))
  {
    Clear();
  }
}

//=================================================================================================

void BRepGraph_Cache::releaseRegistry() noexcept
{
  myLifecycleState.store(LifecycleState::Detached, std::memory_order_release);
  myRegistry.store(nullptr, std::memory_order_release);
}
