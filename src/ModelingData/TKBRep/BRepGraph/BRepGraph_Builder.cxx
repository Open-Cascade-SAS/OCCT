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
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_Layer.hxx>
#include <BRepGraph_LayerParam.hxx>
#include <BRepGraph_LayerRegularity.hxx>
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
  const bool isValid = theGraph.Editor().ValidateMutationBoundary();
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

  for (BRepGraph_FullVertexIterator aVertexIt(theGraph); aVertexIt.More(); aVertexIt.Next())
    theGraph.allocateUID(aVertexIt.CurrentId());
  for (BRepGraph_FullEdgeIterator anEdgeIt(theGraph); anEdgeIt.More(); anEdgeIt.Next())
    theGraph.allocateUID(anEdgeIt.CurrentId());
  for (BRepGraph_FullCoEdgeIterator aCoEdgeIt(theGraph); aCoEdgeIt.More(); aCoEdgeIt.Next())
    theGraph.allocateUID(aCoEdgeIt.CurrentId());
  for (BRepGraph_FullWireIterator aWireIt(theGraph); aWireIt.More(); aWireIt.Next())
    theGraph.allocateUID(aWireIt.CurrentId());
  for (BRepGraph_FullFaceIterator aFaceIt(theGraph); aFaceIt.More(); aFaceIt.Next())
    theGraph.allocateUID(aFaceIt.CurrentId());
  for (BRepGraph_FullShellIterator aShellIt(theGraph); aShellIt.More(); aShellIt.Next())
    theGraph.allocateUID(aShellIt.CurrentId());
  for (BRepGraph_FullSolidIterator aSolidIt(theGraph); aSolidIt.More(); aSolidIt.Next())
    theGraph.allocateUID(aSolidIt.CurrentId());
  for (BRepGraph_FullCompoundIterator aCompoundIt(theGraph); aCompoundIt.More();
       aCompoundIt.Next())
    theGraph.allocateUID(aCompoundIt.CurrentId());
  for (BRepGraph_FullCompSolidIterator aCompSolidIt(theGraph); aCompSolidIt.More();
       aCompSolidIt.Next())
    theGraph.allocateUID(aCompSolidIt.CurrentId());
  for (BRepGraph_FullProductIterator aProductIt(theGraph); aProductIt.More();
       aProductIt.Next())
    theGraph.allocateUID(aProductIt.CurrentId());
  for (BRepGraph_FullOccurrenceIterator anOccurrenceIt(theGraph); anOccurrenceIt.More();
       anOccurrenceIt.Next())
    theGraph.allocateUID(anOccurrenceIt.CurrentId());

  for (BRepGraph_FullShellRefIterator aShellRefIt(theGraph); aShellRefIt.More(); aShellRefIt.Next())
    theGraph.allocateRefUID(aShellRefIt.CurrentId());
  for (BRepGraph_FullFaceRefIterator aFaceRefIt(theGraph); aFaceRefIt.More(); aFaceRefIt.Next())
    theGraph.allocateRefUID(aFaceRefIt.CurrentId());
  for (BRepGraph_FullWireRefIterator aWireRefIt(theGraph); aWireRefIt.More(); aWireRefIt.Next())
    theGraph.allocateRefUID(aWireRefIt.CurrentId());
  for (BRepGraph_FullCoEdgeRefIterator aCoEdgeRefIt(theGraph); aCoEdgeRefIt.More();
       aCoEdgeRefIt.Next())
    theGraph.allocateRefUID(aCoEdgeRefIt.CurrentId());
  for (BRepGraph_FullVertexRefIterator aVertexRefIt(theGraph); aVertexRefIt.More();
       aVertexRefIt.Next())
    theGraph.allocateRefUID(aVertexRefIt.CurrentId());
  for (BRepGraph_FullSolidRefIterator aSolidRefIt(theGraph); aSolidRefIt.More(); aSolidRefIt.Next())
    theGraph.allocateRefUID(aSolidRefIt.CurrentId());
  for (BRepGraph_FullChildRefIterator aChildRefIt(theGraph); aChildRefIt.More(); aChildRefIt.Next())
    theGraph.allocateRefUID(aChildRefIt.CurrentId());
}

//=================================================================================================

void BRepGraph_Builder::Perform(BRepGraph&          theGraph,
                                const TopoDS_Shape& theShape,
                                const bool          theParallel)
{
  Perform(theGraph, theShape, theParallel, BuildOptions());
}

//=================================================================================================

void BRepGraph_Builder::Perform(BRepGraph&                      theGraph,
                                const TopoDS_Shape&             theShape,
                                const bool                      theParallel,
                                const BuildOptions&             theOptions)
{
  theGraph.myData->myIncStorage.Clear();
  theGraph.myData->myHistoryLog.Clear();
  theGraph.myData->myCurrentShapes.Clear();
  theGraph.myData->myRootProductIds.Clear();
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
  const occ::handle<BRepGraph_LayerParam> aParamLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_LayerParam>();
  const occ::handle<BRepGraph_LayerRegularity> aRegularityLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_LayerRegularity>();

  BRepGraphInc_Populate::Perform(theGraph.myData->myIncStorage,
                                 theShape,
                                 theParallel,
                                 theOptions.Populate,
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
          aTopologyRoot = BRepGraph_CompoundId::Start();
        break;
      case TopAbs_COMPSOLID:
        if (aStorage.NbCompSolids() > 0)
          aTopologyRoot = BRepGraph_CompSolidId::Start();
        break;
      case TopAbs_SOLID:
        if (aStorage.NbSolids() > 0)
          aTopologyRoot = BRepGraph_SolidId::Start();
        break;
      case TopAbs_SHELL:
        if (aStorage.NbShells() > 0)
          aTopologyRoot = BRepGraph_ShellId::Start();
        break;
      case TopAbs_FACE:
        if (aStorage.NbFaces() > 0)
          aTopologyRoot = BRepGraph_FaceId::Start();
        break;
      case TopAbs_WIRE:
        if (aStorage.NbWires() > 0)
          aTopologyRoot = BRepGraph_WireId::Start();
        break;
      case TopAbs_EDGE:
        if (aStorage.NbEdges() > 0)
          aTopologyRoot = BRepGraph_EdgeId::Start();
        break;
      case TopAbs_VERTEX:
        if (aStorage.NbVertices() > 0)
          aTopologyRoot = BRepGraph_VertexId::Start();
        break;
      default:
        break;
    }

    // Auto-create a single root Product pointing to the top-level topology node
    // via an occurrence. Skipped when CreateAutoProduct is false (e.g. XCAF builder
    // manages Products itself).
    if (theOptions.CreateAutoProduct)
    {
      const BRepGraph_ProductId aProductId = aStorage.AppendProduct();
      BRepGraphInc::ProductDef& aProduct   = aStorage.ChangeProduct(aProductId);
      theGraph.allocateUID(aProductId);

      // Link the product to its shape root via an occurrence + occurrence ref.
      if (aTopologyRoot.IsValid())
      {
        const BRepGraph_OccurrenceId anOccId = aStorage.AppendOccurrence();
        BRepGraphInc::OccurrenceDef& anOccDef = aStorage.ChangeOccurrence(anOccId);
        anOccDef.ChildDefId                   = aTopologyRoot;
        theGraph.allocateUID(anOccId);

        const BRepGraph_OccurrenceRefId anOccRefId = aStorage.AppendOccurrenceRef();
        BRepGraphInc::OccurrenceRef&    anOccRef   = aStorage.ChangeOccurrenceRef(anOccRefId);
        anOccRef.ParentId                          = BRepGraph_NodeId(aProductId);
        anOccRef.OccurrenceDefId                   = anOccId;
        anOccRef.LocalLocation                     = theShape.Location();
        theGraph.allocateRefUID(anOccRefId);
        aProduct.OccurrenceRefIds.Append(anOccRefId);
      }

      theGraph.myData->myRootProductIds.Append(aProductId);
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

  assertMutationBoundary(theGraph, "Perform: post-build mutation boundary inconsistency");
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
  const occ::handle<BRepGraph_LayerParam> aParamLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_LayerParam>();
  const occ::handle<BRepGraph_LayerRegularity> aRegularityLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_LayerRegularity>();
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
  const occ::handle<BRepGraph_LayerParam> aParamLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_LayerParam>();
  const occ::handle<BRepGraph_LayerRegularity> aRegularityLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_LayerRegularity>();
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

  for (BRepGraph_FullVertexIterator aVertexIt(theGraph, BRepGraph_VertexId(theOldVtx));
       aVertexIt.More();
       aVertexIt.Next())
    theGraph.allocateUID(aVertexIt.CurrentId());
  for (BRepGraph_FullEdgeIterator anEdgeIt(theGraph, BRepGraph_EdgeId(theOldEdge));
       anEdgeIt.More();
       anEdgeIt.Next())
    theGraph.allocateUID(anEdgeIt.CurrentId());
  for (BRepGraph_FullCoEdgeIterator aCoEdgeIt(theGraph, BRepGraph_CoEdgeId(theOldCoEdge));
       aCoEdgeIt.More();
       aCoEdgeIt.Next())
    theGraph.allocateUID(aCoEdgeIt.CurrentId());
  for (BRepGraph_FullWireIterator aWireIt(theGraph, BRepGraph_WireId(theOldWire));
       aWireIt.More();
       aWireIt.Next())
    theGraph.allocateUID(aWireIt.CurrentId());
  for (BRepGraph_FullFaceIterator aFaceIt(theGraph, BRepGraph_FaceId(theOldFace));
       aFaceIt.More();
       aFaceIt.Next())
    theGraph.allocateUID(aFaceIt.CurrentId());
  for (BRepGraph_FullShellIterator aShellIt(theGraph, BRepGraph_ShellId(theOldShell));
       aShellIt.More();
       aShellIt.Next())
    theGraph.allocateUID(aShellIt.CurrentId());
  for (BRepGraph_FullSolidIterator aSolidIt(theGraph, BRepGraph_SolidId(theOldSolid));
       aSolidIt.More();
       aSolidIt.Next())
    theGraph.allocateUID(aSolidIt.CurrentId());
  for (BRepGraph_FullCompoundIterator aCompoundIt(theGraph, BRepGraph_CompoundId(theOldComp));
       aCompoundIt.More();
       aCompoundIt.Next())
    theGraph.allocateUID(aCompoundIt.CurrentId());
  for (BRepGraph_FullCompSolidIterator aCompSolidIt(theGraph, BRepGraph_CompSolidId(theOldCS));
       aCompSolidIt.More();
       aCompSolidIt.Next())
    theGraph.allocateUID(aCompSolidIt.CurrentId());
  for (BRepGraph_FullProductIterator aProductIt(theGraph, BRepGraph_ProductId(theOldProduct));
       aProductIt.More();
       aProductIt.Next())
    theGraph.allocateUID(aProductIt.CurrentId());
  for (BRepGraph_FullOccurrenceIterator anOccurrenceIt(theGraph,
                                                       BRepGraph_OccurrenceId(theOldOccurrence));
       anOccurrenceIt.More();
       anOccurrenceIt.Next())
    theGraph.allocateUID(anOccurrenceIt.CurrentId());

  for (BRepGraph_FullShellRefIterator aShellRefIt(theGraph, BRepGraph_ShellRefId(theOldShellRef));
       aShellRefIt.More();
       aShellRefIt.Next())
    theGraph.allocateRefUID(aShellRefIt.CurrentId());
  for (BRepGraph_FullFaceRefIterator aFaceRefIt(theGraph, BRepGraph_FaceRefId(theOldFaceRef));
       aFaceRefIt.More();
       aFaceRefIt.Next())
    theGraph.allocateRefUID(aFaceRefIt.CurrentId());
  for (BRepGraph_FullWireRefIterator aWireRefIt(theGraph, BRepGraph_WireRefId(theOldWireRef));
       aWireRefIt.More();
       aWireRefIt.Next())
    theGraph.allocateRefUID(aWireRefIt.CurrentId());
  for (BRepGraph_FullCoEdgeRefIterator aCoEdgeRefIt(theGraph,
                                                    BRepGraph_CoEdgeRefId(theOldCoEdgeRef));
       aCoEdgeRefIt.More();
       aCoEdgeRefIt.Next())
    theGraph.allocateRefUID(aCoEdgeRefIt.CurrentId());
  for (BRepGraph_FullVertexRefIterator aVertexRefIt(theGraph,
                                                    BRepGraph_VertexRefId(theOldVertexRef));
       aVertexRefIt.More();
       aVertexRefIt.Next())
    theGraph.allocateRefUID(aVertexRefIt.CurrentId());
  for (BRepGraph_FullSolidRefIterator aSolidRefIt(theGraph, BRepGraph_SolidRefId(theOldSolidRef));
       aSolidRefIt.More();
       aSolidRefIt.Next())
    theGraph.allocateRefUID(aSolidRefIt.CurrentId());
  for (BRepGraph_FullChildRefIterator aChildRefIt(theGraph, BRepGraph_ChildRefId(theOldChildRef));
       aChildRefIt.More();
       aChildRefIt.Next())
    theGraph.allocateRefUID(aChildRefIt.CurrentId());
}
