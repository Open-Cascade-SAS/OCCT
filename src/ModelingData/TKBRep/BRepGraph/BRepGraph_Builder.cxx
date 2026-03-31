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

#include <BRepGraph_Builder.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_Layer.hxx>
#include <BRepGraph_ParamLayer.hxx>
#include <BRepGraph_RegularityLayer.hxx>
#include <BRepGraph_TransientCache.hxx>
#include <BRepGraphInc_Populate.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <NCollection_IncAllocator.hxx>
#include <MathUtils_Random.hxx>
#include <Standard_GUID.hxx>
#include <TopAbs_ShapeEnum.hxx>

#include <cstring>
#include <random>
#include <shared_mutex>

namespace
{

//! Generate a random Standard_GUID using MathUtils::RandomGenerator
//! seeded with std::random_device for platform entropy.
static Standard_GUID generateRandomGUID()
{
  std::random_device         aRD;
  MathUtils::RandomGenerator aRNG(aRD());
  // Fill 16 bytes with random data, then construct Standard_UUID field by field.
  const uint64_t aRand1 = aRNG.NextInt();
  const uint64_t aRand2 = aRNG.NextInt();
  Standard_UUID  aUUID;
  aUUID.Data1 = static_cast<uint32_t>(aRand1);
  aUUID.Data2 = static_cast<uint16_t>(aRand1 >> 32);
  aUUID.Data3 = static_cast<uint16_t>(aRand1 >> 48);
  for (int i = 0; i < 8; ++i)
    aUUID.Data4[i] = static_cast<uint8_t>(aRand2 >> (i * 8));
  return Standard_GUID(aUUID);
}

//=================================================================================================

void appendFlattenedRootNodes(const BRepGraph&                      theGraph,
                              NCollection_Vector<BRepGraph_NodeId>& theRoots,
                              const TopoDS_Shape&                   theShape)
{
  if (theShape.IsNull())
    return;

  switch (theShape.ShapeType())
  {
    case TopAbs_COMPOUND:
    case TopAbs_COMPSOLID:
    case TopAbs_SOLID:
    case TopAbs_SHELL:
      for (TopoDS_Iterator aChildIt(theShape, false, false); aChildIt.More(); aChildIt.Next())
      {
        appendFlattenedRootNodes(theGraph, theRoots, aChildIt.Value());
      }
      return;
    case TopAbs_FACE:
    case TopAbs_WIRE:
    case TopAbs_EDGE:
    case TopAbs_VERTEX:
      break;
    default:
      return;
  }

  const BRepGraph_NodeId aNodeId = theGraph.Shapes().FindNode(theShape);
  if (!aNodeId.IsValid())
    return;

  for (int aRootIdx = 0; aRootIdx < theRoots.Length(); ++aRootIdx)
  {
    if (theRoots.Value(aRootIdx) == aNodeId)
      return;
  }
  theRoots.Append(aNodeId);
}

} // namespace

//=================================================================================================

void BRepGraph_Builder::populateUIDs(BRepGraph& theGraph)
{
  BRepGraphInc_Storage& aStorage = theGraph.myData->myIncStorage;

  if (!aStorage.GetIsDone())
    return;

  for (int i = 0; i < aStorage.NbVertices(); ++i)
    theGraph.allocateUID(aStorage.Vertex(BRepGraph_VertexId(i)).Id);
  for (int i = 0; i < aStorage.NbEdges(); ++i)
    theGraph.allocateUID(aStorage.Edge(BRepGraph_EdgeId(i)).Id);
  for (int i = 0; i < aStorage.NbCoEdges(); ++i)
    theGraph.allocateUID(aStorage.CoEdge(BRepGraph_CoEdgeId(i)).Id);
  for (int i = 0; i < aStorage.NbWires(); ++i)
    theGraph.allocateUID(aStorage.Wire(BRepGraph_WireId(i)).Id);
  for (int i = 0; i < aStorage.NbFaces(); ++i)
    theGraph.allocateUID(aStorage.Face(BRepGraph_FaceId(i)).Id);
  for (int i = 0; i < aStorage.NbShells(); ++i)
    theGraph.allocateUID(aStorage.Shell(BRepGraph_ShellId(i)).Id);
  for (int i = 0; i < aStorage.NbSolids(); ++i)
    theGraph.allocateUID(aStorage.Solid(BRepGraph_SolidId(i)).Id);
  for (int i = 0; i < aStorage.NbCompounds(); ++i)
    theGraph.allocateUID(aStorage.Compound(BRepGraph_CompoundId(i)).Id);
  for (int i = 0; i < aStorage.NbCompSolids(); ++i)
    theGraph.allocateUID(aStorage.CompSolid(BRepGraph_CompSolidId(i)).Id);
  for (int i = 0; i < aStorage.NbProducts(); ++i)
    theGraph.allocateUID(aStorage.Product(BRepGraph_ProductId(i)).Id);
  for (int i = 0; i < aStorage.NbOccurrences(); ++i)
    theGraph.allocateUID(aStorage.Occurrence(BRepGraph_OccurrenceId(i)).Id);

  for (int i = 0; i < aStorage.NbShellRefs(); ++i)
    theGraph.allocateRefUID(BRepGraph_ShellRefId(i));
  for (int i = 0; i < aStorage.NbFaceRefs(); ++i)
    theGraph.allocateRefUID(BRepGraph_FaceRefId(i));
  for (int i = 0; i < aStorage.NbWireRefs(); ++i)
    theGraph.allocateRefUID(BRepGraph_WireRefId(i));
  for (int i = 0; i < aStorage.NbCoEdgeRefs(); ++i)
    theGraph.allocateRefUID(BRepGraph_CoEdgeRefId(i));
  for (int i = 0; i < aStorage.NbVertexRefs(); ++i)
    theGraph.allocateRefUID(BRepGraph_VertexRefId(i));
  for (int i = 0; i < aStorage.NbSolidRefs(); ++i)
    theGraph.allocateRefUID(BRepGraph_SolidRefId(i));
  for (int i = 0; i < aStorage.NbChildRefs(); ++i)
    theGraph.allocateRefUID(BRepGraph_ChildRefId(i));
}

//=================================================================================================

void BRepGraph_Builder::Perform(BRepGraph&          theGraph,
                                const TopoDS_Shape& theShape,
                                const bool          theParallel)
{
  Perform(theGraph, theShape, theParallel, BRepGraphInc_Populate::Options());
}

//=================================================================================================

void BRepGraph_Builder::Perform(BRepGraph&                            theGraph,
                                const TopoDS_Shape&                   theShape,
                                const bool                            theParallel,
                                const BRepGraphInc_Populate::Options& theOptions)
{
  theGraph.myData->myIncStorage.Clear();
  theGraph.myData->myHistoryLog.Clear();
  theGraph.myData->myCurrentShapes.Clear();
  theGraph.myData->myRootNodeIds.Clear();
  theGraph.myTransientCache.Clear();
  {
    std::unique_lock<std::shared_mutex> aUIDLock(theGraph.myData->myUIDToNodeIdMutex);
    theGraph.myData->myUIDToNodeId.Clear();
    theGraph.myData->myUIDToNodeIdDirty      = true;
    theGraph.myData->myUIDToNodeIdGeneration = theGraph.myData->myGeneration.load();
  }
  {
    std::unique_lock<std::shared_mutex> aRefUIDLock(theGraph.myData->myRefUIDToRefIdMutex);
    theGraph.myData->myRefUIDToRefId.Clear();
    theGraph.myData->myRefUIDToRefIdDirty      = true;
    theGraph.myData->myRefUIDToRefIdGeneration = theGraph.myData->myGeneration.load();
  }
  ++theGraph.myData->myGeneration;
  theGraph.myData->myGraphGUID = generateRandomGUID();
  theGraph.myData->myIsDone    = false;

  // Notify registered layers that graph data is being cleared.
  theGraph.myLayerRegistry.ClearAll();

  if (theShape.IsNull())
    return;

  // Temporary allocator for populate scratch data, discarded after build.
  occ::handle<NCollection_IncAllocator> aTmpAlloc = new NCollection_IncAllocator;
  const occ::handle<BRepGraph_ParamLayer> aParamLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_ParamLayer>();
  const occ::handle<BRepGraph_RegularityLayer> aRegularityLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_RegularityLayer>();

  BRepGraphInc_Populate::Perform(theGraph.myData->myIncStorage,
                                 theShape,
                                 theParallel,
                                 theOptions,
                                 aParamLayer.get(),
                                 aRegularityLayer.get(),
                                 aTmpAlloc);
  if (!theGraph.myData->myIncStorage.GetIsDone())
  {
    theGraph.myData->myIncStorage.Clear();
    return;
  }

  populateUIDs(theGraph);

  // Auto-create a single root Product pointing to the top-level topology node.
  // aTopologyRoot defaults to invalid (Index = -1); set only if topology exists.
  {
    BRepGraphInc_Storage& aStorage = theGraph.myData->myIncStorage;
    BRepGraph_NodeId      aTopologyRoot; // default: invalid (Index = -1)
    switch (theShape.ShapeType())
    {
      case TopAbs_COMPOUND:
        if (aStorage.NbCompounds() > 0)
          aTopologyRoot = BRepGraph_CompoundId(0);
        break;
      case TopAbs_COMPSOLID:
        if (aStorage.NbCompSolids() > 0)
          aTopologyRoot = BRepGraph_CompSolidId(0);
        break;
      case TopAbs_SOLID:
        if (aStorage.NbSolids() > 0)
          aTopologyRoot = BRepGraph_SolidId(0);
        break;
      case TopAbs_SHELL:
        if (aStorage.NbShells() > 0)
          aTopologyRoot = BRepGraph_ShellId(0);
        break;
      case TopAbs_FACE:
        if (aStorage.NbFaces() > 0)
          aTopologyRoot = BRepGraph_FaceId(0);
        break;
      case TopAbs_WIRE:
        if (aStorage.NbWires() > 0)
          aTopologyRoot = BRepGraph_WireId(0);
        break;
      case TopAbs_EDGE:
        if (aStorage.NbEdges() > 0)
          aTopologyRoot = BRepGraph_EdgeId(0);
        break;
      case TopAbs_VERTEX:
        if (aStorage.NbVertices() > 0)
          aTopologyRoot = BRepGraph_VertexId(0);
        break;
      default:
        break;
    }

    if (aTopologyRoot.IsValid())
      theGraph.myData->myRootNodeIds.Append(aTopologyRoot);

    BRepGraphInc::ProductDef& aProduct    = aStorage.AppendProduct();
    const int                 aProductIdx = aStorage.NbProducts() - 1;
    aProduct.Id                           = BRepGraph_ProductId(aProductIdx);
    aProduct.ShapeRootId                  = aTopologyRoot; // invalid if no topology matched
    aProduct.RootOrientation              = theShape.Orientation();
    aProduct.RootLocation                 = theShape.Location();
    theGraph.allocateUID(aProduct.Id);
  }

  theGraph.myData->myIsDone = true;

  // Pre-allocate transient cache for lock-free parallel access.
  // Entity counts are now final - Reserve() sizes dense vectors so that
  // Get()/Set() skip the mutex for in-range indices.
  {
    BRepGraphInc_Storage& aStorage = theGraph.myData->myIncStorage;
    int aCounts[BRepGraph_TransientCache::THE_KIND_COUNT] = {};
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Vertex)]    = aStorage.NbVertices();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Edge)]      = aStorage.NbEdges();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::CoEdge)]    = aStorage.NbCoEdges();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Wire)]      = aStorage.NbWires();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Face)]      = aStorage.NbFaces();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Shell)]     = aStorage.NbShells();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Solid)]     = aStorage.NbSolids();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Compound)]  = aStorage.NbCompounds();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::CompSolid)] = aStorage.NbCompSolids();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Product)]    = aStorage.NbProducts();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Occurrence)] = aStorage.NbOccurrences();
    int aReservedKindCount = BRepGraph_TransientCache::THE_DEFAULT_RESERVED_KIND_COUNT;
    const int aRegisteredKindCount = BRepGraph_CacheKindRegistry::NbRegistered();
    if (aRegisteredKindCount > aReservedKindCount)
    {
      aReservedKindCount = aRegisteredKindCount;
    }
    theGraph.myTransientCache.Reserve(aReservedKindCount, aCounts);
  }
}

//=================================================================================================

void BRepGraph_Builder::AppendFlattened(BRepGraph&          theGraph,
                                        const TopoDS_Shape& theShape,
                                        const bool          theParallel)
{
  if (theShape.IsNull())
    return;

  // Snapshot entity counts before append to allocate UIDs only for new entities.
  BRepGraphInc_Storage& aStorage        = theGraph.myData->myIncStorage;
  const int             anOldVtx        = aStorage.NbVertices();
  const int             anOldEdge       = aStorage.NbEdges();
  const int             anOldCoEdge     = aStorage.NbCoEdges();
  const int             anOldWire       = aStorage.NbWires();
  const int             anOldFace       = aStorage.NbFaces();
  const int             anOldShell      = aStorage.NbShells();
  const int             anOldSolid      = aStorage.NbSolids();
  const int             anOldComp       = aStorage.NbCompounds();
  const int             anOldCS         = aStorage.NbCompSolids();
  const int             anOldProduct    = aStorage.NbProducts();
  const int             anOldOccurrence = aStorage.NbOccurrences();
  const int             anOldShellRef   = aStorage.NbShellRefs();
  const int             anOldFaceRef    = aStorage.NbFaceRefs();
  const int             anOldWireRef    = aStorage.NbWireRefs();
  const int             anOldCoEdgeRef  = aStorage.NbCoEdgeRefs();
  const int             anOldVertexRef  = aStorage.NbVertexRefs();
  const int             anOldSolidRef   = aStorage.NbSolidRefs();
  const int             anOldChildRef   = aStorage.NbChildRefs();

  occ::handle<NCollection_IncAllocator> aTmpAlloc = new NCollection_IncAllocator;
  const occ::handle<BRepGraph_ParamLayer> aParamLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_ParamLayer>();
  const occ::handle<BRepGraph_RegularityLayer> aRegularityLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_RegularityLayer>();
  BRepGraphInc_Populate::AppendFlattened(aStorage,
                                         theShape,
                                         theParallel,
                                         BRepGraphInc_Populate::Options(),
                                         aParamLayer.get(),
                                         aRegularityLayer.get(),
                                         aTmpAlloc);

  if (!aStorage.GetIsDone())
    return;

  theGraph.myData->myCurrentShapes.Clear();

  populateUIDsIncremental(theGraph,
                          anOldVtx,
                          anOldEdge,
                          anOldCoEdge,
                          anOldWire,
                          anOldFace,
                          anOldShell,
                          anOldSolid,
                          anOldComp,
                          anOldCS,
                          anOldProduct,
                          anOldOccurrence,
                          anOldShellRef,
                          anOldFaceRef,
                          anOldWireRef,
                          anOldCoEdgeRef,
                          anOldVertexRef,
                          anOldSolidRef,
                          anOldChildRef);

  appendFlattenedRootNodes(theGraph, theGraph.myData->myRootNodeIds, theShape);

  theGraph.myData->myIsDone = true;
}

//=================================================================================================

void BRepGraph_Builder::populateUIDsIncremental(BRepGraph& theGraph,
                                                const int  theOldVtx,
                                                const int  theOldEdge,
                                                const int  theOldCoEdge,
                                                const int  theOldWire,
                                                const int  theOldFace,
                                                const int  theOldShell,
                                                const int  theOldSolid,
                                                const int  theOldComp,
                                                const int  theOldCS,
                                                const int  theOldProduct,
                                                const int  theOldOccurrence,
                                                const int  theOldShellRef,
                                                const int  theOldFaceRef,
                                                const int  theOldWireRef,
                                                const int  theOldCoEdgeRef,
                                                const int  theOldVertexRef,
                                                const int  theOldSolidRef,
                                                const int  theOldChildRef)
{
  BRepGraphInc_Storage& aStorage = theGraph.myData->myIncStorage;

  if (!aStorage.GetIsDone())
    return;

  for (int i = theOldVtx; i < aStorage.NbVertices(); ++i)
    theGraph.allocateUID(aStorage.Vertex(BRepGraph_VertexId(i)).Id);
  for (int i = theOldEdge; i < aStorage.NbEdges(); ++i)
    theGraph.allocateUID(aStorage.Edge(BRepGraph_EdgeId(i)).Id);
  for (int i = theOldCoEdge; i < aStorage.NbCoEdges(); ++i)
    theGraph.allocateUID(aStorage.CoEdge(BRepGraph_CoEdgeId(i)).Id);
  for (int i = theOldWire; i < aStorage.NbWires(); ++i)
    theGraph.allocateUID(aStorage.Wire(BRepGraph_WireId(i)).Id);
  for (int i = theOldFace; i < aStorage.NbFaces(); ++i)
    theGraph.allocateUID(aStorage.Face(BRepGraph_FaceId(i)).Id);
  for (int i = theOldShell; i < aStorage.NbShells(); ++i)
    theGraph.allocateUID(aStorage.Shell(BRepGraph_ShellId(i)).Id);
  for (int i = theOldSolid; i < aStorage.NbSolids(); ++i)
    theGraph.allocateUID(aStorage.Solid(BRepGraph_SolidId(i)).Id);
  for (int i = theOldComp; i < aStorage.NbCompounds(); ++i)
    theGraph.allocateUID(aStorage.Compound(BRepGraph_CompoundId(i)).Id);
  for (int i = theOldCS; i < aStorage.NbCompSolids(); ++i)
    theGraph.allocateUID(aStorage.CompSolid(BRepGraph_CompSolidId(i)).Id);
  for (int i = theOldProduct; i < aStorage.NbProducts(); ++i)
    theGraph.allocateUID(aStorage.Product(BRepGraph_ProductId(i)).Id);
  for (int i = theOldOccurrence; i < aStorage.NbOccurrences(); ++i)
    theGraph.allocateUID(aStorage.Occurrence(BRepGraph_OccurrenceId(i)).Id);

  for (int i = theOldShellRef; i < aStorage.NbShellRefs(); ++i)
    theGraph.allocateRefUID(BRepGraph_ShellRefId(i));
  for (int i = theOldFaceRef; i < aStorage.NbFaceRefs(); ++i)
    theGraph.allocateRefUID(BRepGraph_FaceRefId(i));
  for (int i = theOldWireRef; i < aStorage.NbWireRefs(); ++i)
    theGraph.allocateRefUID(BRepGraph_WireRefId(i));
  for (int i = theOldCoEdgeRef; i < aStorage.NbCoEdgeRefs(); ++i)
    theGraph.allocateRefUID(BRepGraph_CoEdgeRefId(i));
  for (int i = theOldVertexRef; i < aStorage.NbVertexRefs(); ++i)
    theGraph.allocateRefUID(BRepGraph_VertexRefId(i));
  for (int i = theOldSolidRef; i < aStorage.NbSolidRefs(); ++i)
    theGraph.allocateRefUID(BRepGraph_SolidRefId(i));
  for (int i = theOldChildRef; i < aStorage.NbChildRefs(); ++i)
    theGraph.allocateRefUID(BRepGraph_ChildRefId(i));
}
