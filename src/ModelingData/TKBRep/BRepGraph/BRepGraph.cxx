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
#include <BRepGraphInc_Storage.hxx>
#include <MathUtils_Random.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_LinearVector.hxx>
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

BRepGraph::BRepGraph()
    : myData(std::make_unique<BRepGraph_Data>())
{
  initViewsAndRegistries();
  (void)myData->myLayerRegistry.Ensure<BRepGraph_LayerHistory>();
}

//=================================================================================================

BRepGraph::~BRepGraph()
{
  if (myData != nullptr)
  {
    myData->myLayerRegistry.Detach();
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

BRepGraph::BRepGraph(BRepGraph&& theOther) noexcept
    : myData(std::move(theOther.myData))
{
  initViewsAndRegistries();
}

//=================================================================================================

BRepGraph& BRepGraph::operator=(BRepGraph&& theOther) noexcept
{
  if (this != &theOther)
  {
    if (myData != nullptr)
    {
      myData->myLayerRegistry.Detach();
      myData->myCacheRegistry.Detach();
    }
    myData = std::move(theOther.myData);
    initViewsAndRegistries();
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

//=================================================================================================

void BRepGraph::Clear()
{
  Standard_ASSERT_RAISE(!myData->myIncStorage.HasAnyGuard(),
                        "BRepGraph::Clear(): guards still active");
  myData->myIncStorage.Clear();
  myData->myCacheRegistry.ClearAll();
  myData->myIncStorage.IncrementGeneration();
  myData->myIncStorage.SetGraphGUID(generateRandomGUID());

  myData->myLayerRegistry.ClearAll();
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
  switch (NodeKind)
  {
    case Kind::Vertex:
      return aS.IsRemoved(BRepGraph_VertexId(*this));
    case Kind::Edge:
      return aS.IsRemoved(BRepGraph_EdgeId(*this));
    case Kind::CoEdge:
      return aS.IsRemoved(BRepGraph_CoEdgeId(*this));
    case Kind::Wire:
      return aS.IsRemoved(BRepGraph_WireId(*this));
    case Kind::Face:
      return aS.IsRemoved(BRepGraph_FaceId(*this));
    case Kind::Shell:
      return aS.IsRemoved(BRepGraph_ShellId(*this));
    case Kind::Solid:
      return aS.IsRemoved(BRepGraph_SolidId(*this));
    case Kind::Compound:
      return aS.IsRemoved(BRepGraph_CompoundId(*this));
    case Kind::CompSolid:
      return aS.IsRemoved(BRepGraph_CompSolidId(*this));
    case Kind::Product:
      return aS.IsRemoved(BRepGraph_ProductId(*this));
    case Kind::Occurrence:
      return aS.IsRemoved(BRepGraph_OccurrenceId(*this));
  }
  return false;
}

//=================================================================================================

bool BRepGraph_NodeId::IsOwned(const BRepGraph& theGraph) const
{
  if (!IsValid())
  {
    return false;
  }
  const BRepGraphInc_Storage& aS = theGraph.myData->myIncStorage;
  switch (NodeKind)
  {
    case Kind::Vertex:
      return aS.IsOwned(BRepGraph_VertexId(*this));
    case Kind::Edge:
      return aS.IsOwned(BRepGraph_EdgeId(*this));
    case Kind::CoEdge:
      return aS.IsOwned(BRepGraph_CoEdgeId(*this));
    case Kind::Wire:
      return aS.IsOwned(BRepGraph_WireId(*this));
    case Kind::Face:
      return aS.IsOwned(BRepGraph_FaceId(*this));
    case Kind::Shell:
      return aS.IsOwned(BRepGraph_ShellId(*this));
    case Kind::Solid:
      return aS.IsOwned(BRepGraph_SolidId(*this));
    case Kind::Compound:
      return aS.IsOwned(BRepGraph_CompoundId(*this));
    case Kind::CompSolid:
      return aS.IsOwned(BRepGraph_CompSolidId(*this));
    case Kind::Product:
      return aS.IsOwned(BRepGraph_ProductId(*this));
    case Kind::Occurrence:
      return aS.IsOwned(BRepGraph_OccurrenceId(*this));
  }
  return false;
}

//=================================================================================================

bool BRepGraph_RefId::IsRemoved(const BRepGraph& theGraph) const
{
  if (!IsValid())
  {
    return false;
  }
  const BRepGraphInc_Storage& aS = theGraph.myData->myIncStorage;
  switch (RefKind)
  {
    case Kind::Shell:
      return aS.IsRemoved(BRepGraph_ShellRefId(*this));
    case Kind::Face:
      return aS.IsRemoved(BRepGraph_FaceRefId(*this));
    case Kind::Wire:
      return aS.IsRemoved(BRepGraph_WireRefId(*this));
    case Kind::Vertex:
      return aS.IsRemoved(BRepGraph_VertexRefId(*this));
    case Kind::Solid:
      return aS.IsRemoved(BRepGraph_SolidRefId(*this));
    case Kind::Child:
      return aS.IsRemoved(BRepGraph_ChildRefId(*this));
    case Kind::Occurrence:
      return aS.IsRemoved(BRepGraph_OccurrenceRefId(*this));
  }
  return false;
}

//=================================================================================================

bool BRepGraph_RepId::IsRemoved(const BRepGraph& theGraph) const
{
  if (!IsValid())
  {
    return false;
  }
  const BRepGraphInc_Storage& aS = theGraph.myData->myIncStorage;
  switch (RepKind)
  {
    case Kind::EdgeCurve3D:
      return aS.IsRemoved(BRepGraph_EdgeCurve3DRepId(Index));
    case Kind::EdgePolygon3D:
      return aS.IsRemoved(BRepGraph_EdgePolygon3DRepId(Index));
    case Kind::CoEdgeCurve2D:
      return aS.IsRemoved(BRepGraph_CoEdgeCurve2DRepId(Index));
    case Kind::CoEdgePolygon2D:
      return aS.IsRemoved(BRepGraph_CoEdgePolygon2DRepId(Index));
    case Kind::CoEdgePolygonOnTri:
      return aS.IsRemoved(BRepGraph_CoEdgePolygonOnTriRepId(Index));
    case Kind::FaceSurface:
      return aS.IsRemoved(BRepGraph_FaceSurfaceRepId(Index));
    case Kind::FaceTriangulation:
      return aS.IsRemoved(BRepGraph_FaceTriangulationRepId(Index));
  }
  return false;
}

//=================================================================================================

bool BRepGraph_RefId::IsOwned(const BRepGraph& theGraph) const
{
  if (!IsValid())
  {
    return false;
  }
  const BRepGraphInc_Storage& aS = theGraph.myData->myIncStorage;
  switch (RefKind)
  {
    case Kind::Shell:
      return aS.IsOwned(BRepGraph_ShellRefId(*this));
    case Kind::Face:
      return aS.IsOwned(BRepGraph_FaceRefId(*this));
    case Kind::Wire:
      return aS.IsOwned(BRepGraph_WireRefId(*this));
    case Kind::Vertex:
      return aS.IsOwned(BRepGraph_VertexRefId(*this));
    case Kind::Solid:
      return aS.IsOwned(BRepGraph_SolidRefId(*this));
    case Kind::Child:
      return aS.IsOwned(BRepGraph_ChildRefId(*this));
    case Kind::Occurrence:
      return aS.IsOwned(BRepGraph_OccurrenceRefId(*this));
  }
  return false;
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

  const BRepGraphInc_Storage& aStorage = myData->myIncStorage;
  switch (theRefId.RefKind)
  {
    case BRepGraph_RefId::Kind::Vertex: {
      const BRepGraph_VertexRefId aRefId(theRefId);
      if (!aRefId.IsValid(aStorage.NbVertexRefs()))
      {
        break;
      }
      const BRepGraph_VertexId aVertexId = aStorage.VertexRef(aRefId).ChildVertexId;
      if (!aVertexId.IsValid(aStorage.NbVertices()))
      {
        break;
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
      break;
    }
    case BRepGraph_RefId::Kind::Wire: {
      const BRepGraph_WireRefId aRefId(theRefId);
      if (!aRefId.IsValid(aStorage.NbWireRefs()))
      {
        break;
      }
      const BRepGraph_FaceId aFaceId = aStorage.WireRef(aRefId).ParentFaceId;
      if (aFaceId.IsValid(aStorage.NbFaces()) && !aFaceId.IsRemoved(*this))
      {
        markModified(aFaceId);
      }
      break;
    }
    case BRepGraph_RefId::Kind::Face: {
      const BRepGraph_FaceRefId aRefId(theRefId);
      if (!aRefId.IsValid(aStorage.NbFaceRefs()))
      {
        break;
      }
      const BRepGraph_ShellId aShellId = aStorage.FaceRef(aRefId).ParentShellId;
      if (aShellId.IsValid(aStorage.NbShells()) && !aShellId.IsRemoved(*this))
      {
        markModified(aShellId);
      }
      break;
    }
    case BRepGraph_RefId::Kind::Shell: {
      const BRepGraph_ShellRefId aRefId(theRefId);
      if (!aRefId.IsValid(aStorage.NbShellRefs()))
      {
        break;
      }
      const BRepGraph_SolidId aSolidId = aStorage.ShellRef(aRefId).ParentSolidId;
      if (aSolidId.IsValid(aStorage.NbSolids()) && !aSolidId.IsRemoved(*this))
      {
        markModified(aSolidId);
      }
      break;
    }
    case BRepGraph_RefId::Kind::Solid: {
      const BRepGraph_SolidRefId aRefId(theRefId);
      if (!aRefId.IsValid(aStorage.NbSolidRefs()))
      {
        break;
      }
      const BRepGraph_CompSolidId aCompSolidId = aStorage.SolidRef(aRefId).ParentCompSolidId;
      if (aCompSolidId.IsValid(aStorage.NbCompSolids()) && !aCompSolidId.IsRemoved(*this))
      {
        markModified(aCompSolidId);
      }
      break;
    }
    case BRepGraph_RefId::Kind::Child: {
      const BRepGraph_ChildRefId aRefId(theRefId);
      if (!aRefId.IsValid(aStorage.NbChildRefs()))
      {
        break;
      }
      const BRepGraph_CompoundId aCompoundId = aStorage.ChildRef(aRefId).ParentCompoundId;
      if (aCompoundId.IsValid(aStorage.NbCompounds()) && !aCompoundId.IsRemoved(*this))
      {
        markModified(aCompoundId);
      }
      break;
    }
    case BRepGraph_RefId::Kind::Occurrence: {
      const BRepGraph_OccurrenceRefId aRefId(theRefId);
      if (!aRefId.IsValid(aStorage.NbOccurrenceRefs()))
      {
        break;
      }
      const BRepGraph_ProductId aProductId = aStorage.OccurrenceRef(aRefId).ParentProductId;
      if (aProductId.IsValid(aStorage.NbProducts()) && !aProductId.IsRemoved(*this))
      {
        markModified(aProductId);
      }
      break;
    }
    default:
      break;
  }
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

void BRepGraph::initViewsAndRegistries() noexcept
{
  if (myData == nullptr)
  {
    return;
  }
  myData->myTopoView   = TopoView(this);
  myData->myUIDsView   = UIDsView(this);
  myData->myRefsView   = RefsView(this);
  myData->myShapesView = ShapesView(this);
  myData->myEditorView = EditorView(this);
  myData->myMeshView   = MeshView(this);
  myData->myLayerRegistry.Attach(this);
  myData->myCacheRegistry.Attach(this);
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
