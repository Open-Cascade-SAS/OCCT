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
#include <BRepGraph_BuilderView.hxx>
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

static void assertMutationBoundary(BRepGraph& theGraph, const char* theContext)
{
  (void)theContext;
  const bool isValid = theGraph.Builder().ValidateMutationBoundary();
  Standard_ASSERT_VOID(isValid, theContext);
  (void)isValid;
}

} // namespace

//=================================================================================================

void BRepGraph_Builder::populateUIDs(BRepGraph& theGraph)
{
  BRepGraphInc_Storage& aStorage = theGraph.myData->myIncStorage;

  if (!aStorage.GetIsDone())
    return;

  const int aNbVertices = aStorage.NbVertices();
  for (BRepGraph_VertexId aVertexId(0); aVertexId.IsValid(aNbVertices); ++aVertexId)
    theGraph.allocateUID(aStorage.Vertex(aVertexId).Id);
  const int aNbEdges = aStorage.NbEdges();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
    theGraph.allocateUID(aStorage.Edge(anEdgeId).Id);
  const int aNbCoEdges = aStorage.NbCoEdges();
  for (BRepGraph_CoEdgeId aCoEdgeId(0); aCoEdgeId.IsValid(aNbCoEdges); ++aCoEdgeId)
    theGraph.allocateUID(aStorage.CoEdge(aCoEdgeId).Id);
  const int aNbWires = aStorage.NbWires();
  for (BRepGraph_WireId aWireId(0); aWireId.IsValid(aNbWires); ++aWireId)
    theGraph.allocateUID(aStorage.Wire(aWireId).Id);
  const int aNbFaces = aStorage.NbFaces();
  for (BRepGraph_FaceId aFaceId(0); aFaceId.IsValid(aNbFaces); ++aFaceId)
    theGraph.allocateUID(aStorage.Face(aFaceId).Id);
  const int aNbShells = aStorage.NbShells();
  for (BRepGraph_ShellId aShellId(0); aShellId.IsValid(aNbShells); ++aShellId)
    theGraph.allocateUID(aStorage.Shell(aShellId).Id);
  const int aNbSolids = aStorage.NbSolids();
  for (BRepGraph_SolidId aSolidId(0); aSolidId.IsValid(aNbSolids); ++aSolidId)
    theGraph.allocateUID(aStorage.Solid(aSolidId).Id);
  const int aNbCompounds = aStorage.NbCompounds();
  for (BRepGraph_CompoundId aCompoundId(0); aCompoundId.IsValid(aNbCompounds); ++aCompoundId)
    theGraph.allocateUID(aStorage.Compound(aCompoundId).Id);
  const int aNbCompSolids = aStorage.NbCompSolids();
  for (BRepGraph_CompSolidId aCompSolidId(0); aCompSolidId.IsValid(aNbCompSolids); ++aCompSolidId)
    theGraph.allocateUID(aStorage.CompSolid(aCompSolidId).Id);
  const int aNbProducts = aStorage.NbProducts();
  for (BRepGraph_ProductId aProductId(0); aProductId.IsValid(aNbProducts); ++aProductId)
    theGraph.allocateUID(aStorage.Product(aProductId).Id);
  const int aNbOccurrences = aStorage.NbOccurrences();
  for (BRepGraph_OccurrenceId anOccurrenceId(0); anOccurrenceId.IsValid(aNbOccurrences);
       ++anOccurrenceId)
    theGraph.allocateUID(aStorage.Occurrence(anOccurrenceId).Id);

  const int aNbShellRefs = aStorage.NbShellRefs();
  for (BRepGraph_ShellRefId aShellRefId(0); aShellRefId.IsValid(aNbShellRefs); ++aShellRefId)
    theGraph.allocateRefUID(aShellRefId);
  const int aNbFaceRefs = aStorage.NbFaceRefs();
  for (BRepGraph_FaceRefId aFaceRefId(0); aFaceRefId.IsValid(aNbFaceRefs); ++aFaceRefId)
    theGraph.allocateRefUID(aFaceRefId);
  const int aNbWireRefs = aStorage.NbWireRefs();
  for (BRepGraph_WireRefId aWireRefId(0); aWireRefId.IsValid(aNbWireRefs); ++aWireRefId)
    theGraph.allocateRefUID(aWireRefId);
  const int aNbCoEdgeRefs = aStorage.NbCoEdgeRefs();
  for (BRepGraph_CoEdgeRefId aCoEdgeRefId(0); aCoEdgeRefId.IsValid(aNbCoEdgeRefs); ++aCoEdgeRefId)
    theGraph.allocateRefUID(aCoEdgeRefId);
  const int aNbVertexRefs = aStorage.NbVertexRefs();
  for (BRepGraph_VertexRefId aVertexRefId(0); aVertexRefId.IsValid(aNbVertexRefs); ++aVertexRefId)
    theGraph.allocateRefUID(aVertexRefId);
  const int aNbSolidRefs = aStorage.NbSolidRefs();
  for (BRepGraph_SolidRefId aSolidRefId(0); aSolidRefId.IsValid(aNbSolidRefs); ++aSolidRefId)
    theGraph.allocateRefUID(aSolidRefId);
  const int aNbChildRefs = aStorage.NbChildRefs();
  for (BRepGraph_ChildRefId aChildRefId(0); aChildRefId.IsValid(aNbChildRefs); ++aChildRefId)
    theGraph.allocateRefUID(aChildRefId);
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
  occ::handle<NCollection_IncAllocator>   aTmpAlloc = new NCollection_IncAllocator;
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

  // Determine the top-level topology root node.
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

    // Auto-create a single root Product pointing to the top-level topology node.
    // Skipped when CreateAutoProduct is false (e.g. XCAF builder manages Products itself).
    if (theOptions.CreateAutoProduct)
    {
      BRepGraphInc::ProductDef& aProduct    = aStorage.AppendProduct();
      const int                 aProductIdx = aStorage.NbProducts() - 1;
      aProduct.Id                           = BRepGraph_ProductId(aProductIdx);
      aProduct.ShapeRootId                  = aTopologyRoot; // invalid if no topology matched
      aProduct.RootOrientation              = theShape.Orientation();
      aProduct.RootLocation                 = theShape.Location();
      theGraph.allocateUID(aProduct.Id);
    }
  }

  theGraph.myData->myIsDone = true;

  // Pre-allocate transient cache for lock-free parallel access.
  // Entity counts are now final - Reserve() sizes dense vectors so that
  // Get()/Set() skip the mutex for in-range indices.
  {
    BRepGraphInc_Storage& aStorage = theGraph.myData->myIncStorage;
    int                   aCounts[BRepGraph_TransientCache::THE_KIND_COUNT] = {};
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Vertex)]               = aStorage.NbVertices();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Edge)]                 = aStorage.NbEdges();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::CoEdge)]               = aStorage.NbCoEdges();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Wire)]                 = aStorage.NbWires();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Face)]                 = aStorage.NbFaces();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Shell)]                = aStorage.NbShells();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Solid)]                = aStorage.NbSolids();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Compound)]   = aStorage.NbCompounds();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::CompSolid)]  = aStorage.NbCompSolids();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Product)]    = aStorage.NbProducts();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Occurrence)] = aStorage.NbOccurrences();
    int       aReservedKindCount   = BRepGraph_TransientCache::THE_DEFAULT_RESERVED_KIND_COUNT;
    const int aRegisteredKindCount = BRepGraph_CacheKindRegistry::NbRegistered();
    if (aRegisteredKindCount > aReservedKindCount)
    {
      aReservedKindCount = aRegisteredKindCount;
    }
    theGraph.myTransientCache.Reserve(aReservedKindCount, aCounts);
  }

  assertMutationBoundary(theGraph, "Build: post-build mutation boundary inconsistency");
}

//=================================================================================================

void BRepGraph_Builder::AppendFlattened(BRepGraph&                            theGraph,
                                        const TopoDS_Shape&                   theShape,
                                        const bool                            theParallel,
                                        const BRepGraphInc_Populate::Options& theOptions)
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

  occ::handle<NCollection_IncAllocator>   aTmpAlloc = new NCollection_IncAllocator;
  const occ::handle<BRepGraph_ParamLayer> aParamLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_ParamLayer>();
  const occ::handle<BRepGraph_RegularityLayer> aRegularityLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_RegularityLayer>();
  NCollection_Vector<BRepGraph_NodeId> aAppendedRoots(8, theGraph.Allocator());
  BRepGraphInc_Populate::AppendFlattened(aStorage,
                                         theShape,
                                         theParallel,
                                         aAppendedRoots,
                                         theOptions,
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

  for (const BRepGraph_NodeId& aRootNode : aAppendedRoots)
  {
    theGraph.myData->myRootNodeIds.Append(aRootNode);
  }

  theGraph.myData->myIsDone = true;

  assertMutationBoundary(theGraph, "AppendFlattened: post-append mutation boundary inconsistency");
}

//=================================================================================================

void BRepGraph_Builder::AppendFull(BRepGraph&                            theGraph,
                                   const TopoDS_Shape&                   theShape,
                                   const bool                            theParallel,
                                   const BRepGraphInc_Populate::Options& theOptions)
{
  if (theShape.IsNull())
    return;

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

  occ::handle<NCollection_IncAllocator>   aTmpAlloc = new NCollection_IncAllocator;
  const occ::handle<BRepGraph_ParamLayer> aParamLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_ParamLayer>();
  const occ::handle<BRepGraph_RegularityLayer> aRegularityLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_RegularityLayer>();
  BRepGraphInc_Populate::Append(aStorage,
                                theShape,
                                theParallel,
                                theOptions,
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

  theGraph.myData->myIsDone = true;

  assertMutationBoundary(theGraph, "AppendFull: post-append mutation boundary inconsistency");
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

  const int aNbVertices = aStorage.NbVertices();
  for (BRepGraph_VertexId aVertexId(theOldVtx); aVertexId.IsValid(aNbVertices); ++aVertexId)
    theGraph.allocateUID(aStorage.Vertex(aVertexId).Id);
  const int aNbEdges = aStorage.NbEdges();
  for (BRepGraph_EdgeId anEdgeId(theOldEdge); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
    theGraph.allocateUID(aStorage.Edge(anEdgeId).Id);
  const int aNbCoEdges = aStorage.NbCoEdges();
  for (BRepGraph_CoEdgeId aCoEdgeId(theOldCoEdge); aCoEdgeId.IsValid(aNbCoEdges); ++aCoEdgeId)
    theGraph.allocateUID(aStorage.CoEdge(aCoEdgeId).Id);
  const int aNbWires = aStorage.NbWires();
  for (BRepGraph_WireId aWireId(theOldWire); aWireId.IsValid(aNbWires); ++aWireId)
    theGraph.allocateUID(aStorage.Wire(aWireId).Id);
  const int aNbFaces = aStorage.NbFaces();
  for (BRepGraph_FaceId aFaceId(theOldFace); aFaceId.IsValid(aNbFaces); ++aFaceId)
    theGraph.allocateUID(aStorage.Face(aFaceId).Id);
  const int aNbShells = aStorage.NbShells();
  for (BRepGraph_ShellId aShellId(theOldShell); aShellId.IsValid(aNbShells); ++aShellId)
    theGraph.allocateUID(aStorage.Shell(aShellId).Id);
  const int aNbSolids = aStorage.NbSolids();
  for (BRepGraph_SolidId aSolidId(theOldSolid); aSolidId.IsValid(aNbSolids); ++aSolidId)
    theGraph.allocateUID(aStorage.Solid(aSolidId).Id);
  const int aNbCompounds = aStorage.NbCompounds();
  for (BRepGraph_CompoundId aCompoundId(theOldComp); aCompoundId.IsValid(aNbCompounds);
       ++aCompoundId)
    theGraph.allocateUID(aStorage.Compound(aCompoundId).Id);
  const int aNbCompSolids = aStorage.NbCompSolids();
  for (BRepGraph_CompSolidId aCompSolidId(theOldCS); aCompSolidId.IsValid(aNbCompSolids);
       ++aCompSolidId)
    theGraph.allocateUID(aStorage.CompSolid(aCompSolidId).Id);
  const int aNbProducts = aStorage.NbProducts();
  for (BRepGraph_ProductId aProductId(theOldProduct); aProductId.IsValid(aNbProducts); ++aProductId)
    theGraph.allocateUID(aStorage.Product(aProductId).Id);
  const int aNbOccurrences = aStorage.NbOccurrences();
  for (BRepGraph_OccurrenceId anOccurrenceId(theOldOccurrence);
       anOccurrenceId.IsValid(aNbOccurrences);
       ++anOccurrenceId)
    theGraph.allocateUID(aStorage.Occurrence(anOccurrenceId).Id);

  const int aNbShellRefs = aStorage.NbShellRefs();
  for (BRepGraph_ShellRefId aShellRefId(theOldShellRef); aShellRefId.IsValid(aNbShellRefs);
       ++aShellRefId)
    theGraph.allocateRefUID(aShellRefId);
  const int aNbFaceRefs = aStorage.NbFaceRefs();
  for (BRepGraph_FaceRefId aFaceRefId(theOldFaceRef); aFaceRefId.IsValid(aNbFaceRefs); ++aFaceRefId)
    theGraph.allocateRefUID(aFaceRefId);
  const int aNbWireRefs = aStorage.NbWireRefs();
  for (BRepGraph_WireRefId aWireRefId(theOldWireRef); aWireRefId.IsValid(aNbWireRefs); ++aWireRefId)
    theGraph.allocateRefUID(aWireRefId);
  const int aNbCoEdgeRefs = aStorage.NbCoEdgeRefs();
  for (BRepGraph_CoEdgeRefId aCoEdgeRefId(theOldCoEdgeRef); aCoEdgeRefId.IsValid(aNbCoEdgeRefs);
       ++aCoEdgeRefId)
    theGraph.allocateRefUID(aCoEdgeRefId);
  const int aNbVertexRefs = aStorage.NbVertexRefs();
  for (BRepGraph_VertexRefId aVertexRefId(theOldVertexRef); aVertexRefId.IsValid(aNbVertexRefs);
       ++aVertexRefId)
    theGraph.allocateRefUID(aVertexRefId);
  const int aNbSolidRefs = aStorage.NbSolidRefs();
  for (BRepGraph_SolidRefId aSolidRefId(theOldSolidRef); aSolidRefId.IsValid(aNbSolidRefs);
       ++aSolidRefId)
    theGraph.allocateRefUID(aSolidRefId);
  const int aNbChildRefs = aStorage.NbChildRefs();
  for (BRepGraph_ChildRefId aChildRefId(theOldChildRef); aChildRefId.IsValid(aNbChildRefs);
       ++aChildRefId)
    theGraph.allocateRefUID(aChildRefId);
}
