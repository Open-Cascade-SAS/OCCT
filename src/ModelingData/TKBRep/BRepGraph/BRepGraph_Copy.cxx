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

#include <BRepGraph_Copy.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>

#include <BRepGraph_TransientCache.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_CacheView.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_TopoView.hxx>

#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_Map.hxx>

#include <utility>

namespace
{

//! Copy geometry handle if theCopyGeom is true, otherwise return same handle.
occ::handle<Geom_Surface> copySurface(const occ::handle<Geom_Surface>& theSurf, bool theCopyGeom)
{
  if (theSurf.IsNull() || !theCopyGeom)
    return theSurf;
  return occ::down_cast<Geom_Surface>(theSurf->Copy());
}

occ::handle<Geom_Curve> copyCurve(const occ::handle<Geom_Curve>& theCrv, bool theCopyGeom)
{
  if (theCrv.IsNull() || !theCopyGeom)
    return theCrv;
  return occ::down_cast<Geom_Curve>(theCrv->Copy());
}

occ::handle<Geom2d_Curve> copyPCurve(const occ::handle<Geom2d_Curve>& theCrv, bool theCopyGeom)
{
  if (theCrv.IsNull() || !theCopyGeom)
    return theCrv;
  return occ::down_cast<Geom2d_Curve>(theCrv->Copy());
}

template <typename TKeyId>
void transferFreshCacheValues(const BRepGraph& theSrcGraph,
                              const TKeyId     theSrcKey,
                              BRepGraph&       theDstGraph,
                              const TKeyId     theDstKey)
{
  for (auto aKindIt = theSrcGraph.Cache().CacheKindIter(theSrcKey); aKindIt.More(); aKindIt.Next())
  {
    const occ::handle<BRepGraph_CacheKind>  aKind  = aKindIt.Value();
    const occ::handle<BRepGraph_CacheValue> aValue = theSrcGraph.Cache().Get(theSrcKey, aKind);
    if (!aValue.IsNull())
    {
      theDstGraph.Cache().Set(theDstKey, aKind, aValue);
    }
  }
}

//! Deferred cache-transfer queue.
//!
//! Pairs are recorded only for entities that actually carry at least one fresh cache
//! value on the source graph; entities with no cache are skipped, so the queue size is
//! O(cached entities), not O(graph size).
//!
//! We defer instead of transferring eagerly because the destination graph's SubtreeGen
//! continues to advance during construction (every Mut/AddPCurve propagates via the
//! reverse index). A Set() during construction captures an intermediate SubtreeGen
//! that Get() later mismatches. Draining after all mutations means Set() captures the
//! final SubtreeGen and the cache survives the copy.
struct DeferredCacheTransfers
{
  using NodePair = std::pair<BRepGraph_NodeId, BRepGraph_NodeId>;
  using RefPair  = std::pair<BRepGraph_RefId, BRepGraph_RefId>;

  NCollection_DynamicArray<NodePair> NodePairs;
  NCollection_DynamicArray<RefPair>  RefPairs;

  template <typename TKeyId>
  static bool srcHasAnyCache(const BRepGraph& theSrc, const TKeyId theKey)
  {
    return theSrc.Cache().CacheKindIter(theKey).More();
  }

  void DeferNode(const BRepGraph& theSrc,
                 const BRepGraph_NodeId theSrcNode,
                 const BRepGraph_NodeId theDstNode)
  {
    if (srcHasAnyCache(theSrc, theSrcNode))
      NodePairs.Append({theSrcNode, theDstNode});
  }

  void DeferRef(const BRepGraph& theSrc,
                const BRepGraph_RefId theSrcRef,
                const BRepGraph_RefId theDstRef)
  {
    if (srcHasAnyCache(theSrc, theSrcRef))
      RefPairs.Append({theSrcRef, theDstRef});
  }

  void Drain(const BRepGraph& theSrc, BRepGraph& theDst) const
  {
    for (const auto& aPair : NodePairs)
      transferFreshCacheValues(theSrc, aPair.first, theDst, aPair.second);
    for (const auto& aPair : RefPairs)
      transferFreshCacheValues(theSrc, aPair.first, theDst, aPair.second);
  }
};

} // namespace

//=================================================================================================

void BRepGraph_Copy::reserveTransientCache(BRepGraph& theGraph)
{
  BRepGraphInc_Storage& aStorage                                          = theGraph.incStorage();
  int                   aCounts[BRepGraph_TransientCache::THE_KIND_COUNT] = {};
  aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Vertex)]               = aStorage.NbVertices();
  aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Edge)]                 = aStorage.NbEdges();
  aCounts[static_cast<int>(BRepGraph_NodeId::Kind::CoEdge)]               = aStorage.NbCoEdges();
  aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Wire)]                 = aStorage.NbWires();
  aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Face)]                 = aStorage.NbFaces();
  aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Shell)]                = aStorage.NbShells();
  aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Solid)]                = aStorage.NbSolids();
  aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Compound)]             = aStorage.NbCompounds();
  aCounts[static_cast<int>(BRepGraph_NodeId::Kind::CompSolid)]            = aStorage.NbCompSolids();
  aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Product)]              = aStorage.NbProducts();
  aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Occurrence)] = aStorage.NbOccurrences();
  int       aReservedKindCount   = BRepGraph_TransientCache::THE_DEFAULT_RESERVED_KIND_COUNT;
  const int aRegisteredKindCount = BRepGraph_CacheKindRegistry::NbRegistered();
  if (aRegisteredKindCount > aReservedKindCount)
  {
    aReservedKindCount = aRegisteredKindCount;
  }
  theGraph.transientCache().Reserve(aReservedKindCount, aCounts);
}

//=================================================================================================

BRepGraph BRepGraph_Copy::Perform(const BRepGraph& theGraph, const bool theCopyGeom)
{
  BRepGraph aResult;
  if (!theGraph.IsDone())
    return aResult;

  const BRepGraph::RefsView& aRefs = theGraph.Refs();
  DeferredCacheTransfers     aDeferred;

  // Bottom-up graph rebuild via EditorView.
  // Since this is a full copy, old index == new index (identity mapping).

  // Vertices.
  for (BRepGraph_FullVertexIterator aVertexIt(theGraph); aVertexIt.More(); aVertexIt.Next())
  {
    const BRepGraph_VertexId       aVertexId = aVertexIt.CurrentId();
    const BRepGraphInc::VertexDef& aVtx      = theGraph.Topo().Vertices().Definition(aVertexId);
    (void)aResult.Editor().Vertices().Add(aVtx.Point, aVtx.Tolerance);
    aDeferred.DeferNode(theGraph, aVertexId, aVertexId);
  }

  // Edges.
  for (BRepGraph_FullEdgeIterator anEdgeIt(theGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId       anEdgeId = anEdgeIt.CurrentId();
    const BRepGraphInc::EdgeDef& anEdge   = theGraph.Topo().Edges().Definition(anEdgeId);

    const occ::handle<Geom_Curve>& anEdgeSrcCurve = BRepGraph_Tool::Edge::Curve(theGraph, anEdgeId);
    occ::handle<Geom_Curve>        aCurve         = copyCurve(anEdgeSrcCurve, theCopyGeom);

    // Resolve vertex def ids via Tool helpers (identity mapping: old index == new index).
    const BRepGraph_VertexId aStartVtxId = BRepGraph_Tool::Edge::StartVertexId(theGraph, anEdgeId);
    const BRepGraph_VertexId anEndVtxId  = BRepGraph_Tool::Edge::EndVertexId(theGraph, anEdgeId);

    (void)aResult.Editor().Edges().Add(aStartVtxId,
                                       anEndVtxId,
                                       aCurve,
                                       anEdge.ParamFirst,
                                       anEdge.ParamLast,
                                       anEdge.Tolerance);

    {
      BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aNewEdge = aResult.Editor().Edges().Mut(anEdgeId);
      aNewEdge.Internal().IsDegenerate                   = anEdge.IsDegenerate;
      aNewEdge.Internal().SameParameter                  = anEdge.SameParameter;
      aNewEdge.Internal().SameRange                      = anEdge.SameRange;
      aNewEdge.MarkDirty();
    }
    aDeferred.DeferNode(theGraph, anEdgeId, anEdgeId);
  }

  // Wires.
  for (BRepGraph_FullWireIterator aWireIt(theGraph); aWireIt.More(); aWireIt.Next())
  {
    const BRepGraph_WireId aWireId = aWireIt.CurrentId();
    NCollection_DynamicArray<std::pair<BRepGraph_EdgeId, TopAbs_Orientation>> aWireEdges;
    for (BRepGraph_RefsCoEdgeOfWire aCEIt(theGraph, aWireId); aCEIt.More(); aCEIt.Next())
    {
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        theGraph.Topo().CoEdges().Definition(aRefs.CoEdges().Entry(aCEIt.CurrentId()).CoEdgeDefId);
      aWireEdges.Append(std::make_pair(aCoEdge.EdgeDefId, aCoEdge.Orientation));
    }
    (void)aResult.Editor().Wires().Add(aWireEdges);
    aDeferred.DeferNode(theGraph, aWireId, aWireId);
  }

  // Faces.
  for (BRepGraph_FullFaceIterator aFaceIt(theGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId       aFaceId = aFaceIt.CurrentId();
    const BRepGraphInc::FaceDef& aFace   = theGraph.Topo().Faces().Definition(aFaceId);

    const occ::handle<Geom_Surface>& aFaceSrcSurf =
      BRepGraph_Tool::Face::Surface(theGraph, aFaceId);
    occ::handle<Geom_Surface> aSurf = copySurface(aFaceSrcSurf, theCopyGeom);

    // Get outer/inner wire def NodeIds via typed iterator.
    BRepGraph_WireId                           anOuterWire;
    NCollection_DynamicArray<BRepGraph_WireId> anInnerWires;

    for (BRepGraph_RefsWireOfFace aWRIt(theGraph, aFaceId); aWRIt.More(); aWRIt.Next())
    {
      const BRepGraphInc::WireRef& aWR = aRefs.Wires().Entry(aWRIt.CurrentId());
      if (aWR.IsOuter)
      {
        anOuterWire = aWR.WireDefId;
      }
      else
      {
        anInnerWires.Append(aWR.WireDefId);
      }
    }

    (void)aResult.Editor().Faces().Add(aSurf, anOuterWire, anInnerWires, aFace.Tolerance);

    {
      BRepGraph_MutGuard<BRepGraphInc::FaceDef> aNewFace = aResult.Editor().Faces().Mut(aFaceId);
      aNewFace.Internal().NaturalRestriction             = aFace.NaturalRestriction;
      aNewFace.Internal().TriangulationRepId             = aFace.TriangulationRepId;
      aNewFace.MarkDirty();
    }
    // Copy cached mesh data if present.
    const BRepGraph_MeshCache::FaceMeshEntry* aCachedFace =
      theGraph.Mesh().Faces().CachedMesh(aFaceId);
    if (aCachedFace != nullptr)
    {
      BRepGraph_MeshCache::FaceMeshEntry& aNewEntry = aResult.meshCache().ChangeFaceMesh(aFaceId);
      aNewEntry                                     = *aCachedFace;
    }
    aDeferred.DeferNode(theGraph, aFaceId, aFaceId);
  }

  // PCurves via CoEdge data (after edges and faces are created).
  for (BRepGraph_FullEdgeIterator anEdgeIt(theGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId                              anEdgeId = anEdgeIt.CurrentId();
    const NCollection_DynamicArray<BRepGraph_CoEdgeId>& aCoEdgeIds =
      theGraph.Topo().Edges().CoEdges(anEdgeId);
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdgeIds)
    {
      const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(aCoEdgeId);
      if (!aCoEdge.Curve2DRepId.IsValid())
        continue;

      const occ::handle<Geom2d_Curve>& aCoEdgeSrcPC =
        BRepGraph_Tool::CoEdge::PCurve(theGraph, aCoEdgeId);
      occ::handle<Geom2d_Curve> aNewPC = copyPCurve(aCoEdgeSrcPC, theCopyGeom);
      aResult.Editor().CoEdges().AddPCurve(anEdgeId,
                                           aCoEdge.FaceDefId,
                                           aNewPC,
                                           aCoEdge.ParamFirst,
                                           aCoEdge.ParamLast,
                                           aCoEdge.Orientation);
    }
  }

  // Shells.
  for (BRepGraph_FullShellIterator aShellIt(theGraph); aShellIt.More(); aShellIt.Next())
  {
    const BRepGraph_ShellId aShellId    = aShellIt.CurrentId();
    BRepGraph_ShellId       aNewShellId = aResult.Editor().Shells().Add();
    aDeferred.DeferNode(theGraph, aShellId, aShellId);

    for (BRepGraph_RefsFaceOfShell aFRIt(theGraph, aShellId); aFRIt.More(); aFRIt.Next())
    {
      const BRepGraphInc::FaceRef& aFR = aRefs.Faces().Entry(aFRIt.CurrentId());
      const BRepGraph_FaceRefId    aNewFaceRefId =
        aResult.Editor().Shells().AddFace(aNewShellId, aFR.FaceDefId, aFR.Orientation);
      aDeferred.DeferRef(theGraph, aFRIt.CurrentId(), aNewFaceRefId);
    }
  }

  // Solids.
  for (BRepGraph_FullSolidIterator aSolidIt(theGraph); aSolidIt.More(); aSolidIt.Next())
  {
    const BRepGraph_SolidId aSolidId    = aSolidIt.CurrentId();
    BRepGraph_SolidId       aNewSolidId = aResult.Editor().Solids().Add();
    aDeferred.DeferNode(theGraph, aSolidId, aSolidId);

    for (BRepGraph_RefsShellOfSolid aSRIt(theGraph, aSolidId); aSRIt.More(); aSRIt.Next())
    {
      const BRepGraphInc::ShellRef& aSR = aRefs.Shells().Entry(aSRIt.CurrentId());
      const BRepGraph_ShellRefId    aNewShellRefId =
        aResult.Editor().Solids().AddShell(aNewSolidId, aSR.ShellDefId, aSR.Orientation);
      aDeferred.DeferRef(theGraph, aSRIt.CurrentId(), aNewShellRefId);
    }
  }

  // Compounds.
  for (BRepGraph_FullCompoundIterator aCompoundIt(theGraph); aCompoundIt.More(); aCompoundIt.Next())
  {
    const BRepGraph_CompoundId                 aCompoundId = aCompoundIt.CurrentId();
    NCollection_DynamicArray<BRepGraph_NodeId> aChildNodeIds;
    for (BRepGraph_RefsChildOfCompound aCRIt(theGraph, aCompoundId); aCRIt.More(); aCRIt.Next())
    {
      aChildNodeIds.Append(aRefs.Children().Entry(aCRIt.CurrentId()).ChildDefId);
    }
    (void)aResult.Editor().Compounds().Add(aChildNodeIds);
    aDeferred.DeferNode(theGraph, aCompoundId, aCompoundId);
  }

  // CompSolids.
  for (BRepGraph_FullCompSolidIterator aCompSolidIt(theGraph); aCompSolidIt.More();
       aCompSolidIt.Next())
  {
    const BRepGraph_CompSolidId                 aCompSolidId = aCompSolidIt.CurrentId();
    NCollection_DynamicArray<BRepGraph_SolidId> aSolidNodeIds;
    for (BRepGraph_RefsSolidOfCompSolid aSRIt(theGraph, aCompSolidId); aSRIt.More(); aSRIt.Next())
    {
      aSolidNodeIds.Append(aRefs.Solids().Entry(aSRIt.CurrentId()).SolidDefId);
    }
    (void)aResult.Editor().CompSolids().Add(aSolidNodeIds);
    aDeferred.DeferNode(theGraph, aCompSolidId, aCompSolidId);
  }

  // Products.
  for (BRepGraph_FullProductIterator aProductIt(theGraph); aProductIt.More(); aProductIt.Next())
  {
    const BRepGraph_ProductId       aProductId = aProductIt.CurrentId();
    const BRepGraphInc::ProductDef& aSrcProd   = theGraph.incStorage().Product(aProductId);
    const BRepGraph_ProductId       aNewId     = aResult.incStorage().AppendProduct();
    aResult.incStorage().ChangeProduct(aNewId).OccurrenceRefIds =
      aSrcProd.OccurrenceRefIds;
  }

  // Occurrences.
  for (BRepGraph_FullOccurrenceIterator anOccurrenceIt(theGraph); anOccurrenceIt.More();
       anOccurrenceIt.Next())
  {
    const BRepGraph_OccurrenceId       anOccurrenceId = anOccurrenceIt.CurrentId();
    const BRepGraphInc::OccurrenceDef& aSrcOcc = theGraph.incStorage().Occurrence(anOccurrenceId);
    const BRepGraph_OccurrenceId       aNewId  = aResult.incStorage().AppendOccurrence();
    aResult.incStorage().ChangeOccurrence(aNewId).ChildDefId =
      aSrcOcc.ChildDefId;
  }

  // OccurrenceRefs (carry LocalLocation, formerly stored as Placement on OccurrenceDef).
  for (BRepGraph_FullOccurrenceRefIterator aRefIt(theGraph); aRefIt.More(); aRefIt.Next())
  {
    const BRepGraph_OccurrenceRefId    aRefId  = aRefIt.CurrentId();
    const BRepGraphInc::OccurrenceRef& aSrcRef = theGraph.incStorage().OccurrenceRef(aRefId);
    const BRepGraph_OccurrenceRefId    aNewId  = aResult.incStorage().AppendOccurrenceRef();
    BRepGraphInc::OccurrenceRef&       aNewRef = aResult.incStorage().ChangeOccurrenceRef(aNewId);
    aNewRef.ParentId = aSrcRef.ParentId;
    aNewRef.IsRemoved                          = aSrcRef.IsRemoved;
    aNewRef.OccurrenceDefId = aSrcRef.OccurrenceDefId;
    aNewRef.LocalLocation                      = aSrcRef.LocalLocation;
  }

  // Phase 3: Transfer UIDs (identity mapping - direct vector copy).
  auto copyUIDs = [](const NCollection_DynamicArray<BRepGraph_UID>& theSrc,
                     NCollection_DynamicArray<BRepGraph_UID>&       theDst) {
    NCollection_DynamicArray<BRepGraph_UID>::Iterator aDstIt(theDst);
    NCollection_DynamicArray<BRepGraph_UID>::Iterator anSrcIt(theSrc);
    for (; anSrcIt.More() && aDstIt.More(); anSrcIt.Next(), aDstIt.Next())
    {
      if (anSrcIt.Value().IsValid())
        aDstIt.ChangeValue() = anSrcIt.Value();
    }
  };

  copyUIDs(theGraph.incStorage().UIDs(BRepGraph_NodeId::Kind::Vertex),
           aResult.incStorage().ChangeUIDs(BRepGraph_NodeId::Kind::Vertex));
  copyUIDs(theGraph.incStorage().UIDs(BRepGraph_NodeId::Kind::Edge),
           aResult.incStorage().ChangeUIDs(BRepGraph_NodeId::Kind::Edge));
  copyUIDs(theGraph.incStorage().UIDs(BRepGraph_NodeId::Kind::Wire),
           aResult.incStorage().ChangeUIDs(BRepGraph_NodeId::Kind::Wire));
  copyUIDs(theGraph.incStorage().UIDs(BRepGraph_NodeId::Kind::Face),
           aResult.incStorage().ChangeUIDs(BRepGraph_NodeId::Kind::Face));
  copyUIDs(theGraph.incStorage().UIDs(BRepGraph_NodeId::Kind::Shell),
           aResult.incStorage().ChangeUIDs(BRepGraph_NodeId::Kind::Shell));
  copyUIDs(theGraph.incStorage().UIDs(BRepGraph_NodeId::Kind::Solid),
           aResult.incStorage().ChangeUIDs(BRepGraph_NodeId::Kind::Solid));
  copyUIDs(theGraph.incStorage().UIDs(BRepGraph_NodeId::Kind::Compound),
           aResult.incStorage().ChangeUIDs(BRepGraph_NodeId::Kind::Compound));
  copyUIDs(theGraph.incStorage().UIDs(BRepGraph_NodeId::Kind::CompSolid),
           aResult.incStorage().ChangeUIDs(BRepGraph_NodeId::Kind::CompSolid));
  copyUIDs(theGraph.incStorage().UIDs(BRepGraph_NodeId::Kind::Product),
           aResult.incStorage().ChangeUIDs(BRepGraph_NodeId::Kind::Product));
  copyUIDs(theGraph.incStorage().UIDs(BRepGraph_NodeId::Kind::Occurrence),
           aResult.incStorage().ChangeUIDs(BRepGraph_NodeId::Kind::Occurrence));

  aResult.data()->myNextUIDCounter.store(
    theGraph.data()->myNextUIDCounter.load(std::memory_order_relaxed),
    std::memory_order_relaxed);
  aResult.data()->myGeneration.store(theGraph.data()->myGeneration.load(std::memory_order_relaxed),
                                     std::memory_order_relaxed);
  aResult.data()->myIsDone = true;

  // Build root product set: products not referenced as ChildDefId by any occurrence.
  {
    NCollection_Map<BRepGraph_ProductId> aReferencedProducts;
    for (BRepGraph_FullOccurrenceIterator anOccIt(aResult); anOccIt.More(); anOccIt.Next())
    {
      const BRepGraph_OccurrenceId       anOccId = anOccIt.CurrentId();
      const BRepGraphInc::OccurrenceDef& anOcc   = aResult.incStorage().Occurrence(anOccId);
      if (!anOcc.IsRemoved && anOcc.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Product)
      {
        const BRepGraph_ProductId aChildProductId =
          BRepGraph_ProductId::FromNodeId(anOcc.ChildDefId);
        if (aChildProductId.IsValidIn(aResult.Topo().Products()))
        {
          aReferencedProducts.Add(aChildProductId);
        }
      }
    }
    for (BRepGraph_FullProductIterator aProdIt(aResult); aProdIt.More(); aProdIt.Next())
    {
      const BRepGraph_ProductId       aProdId = aProdIt.CurrentId();
      const BRepGraphInc::ProductDef& aProd   = aResult.incStorage().Product(aProdId);
      if (!aProd.IsRemoved && !aReferencedProducts.Contains(aProdId))
      {
        aResult.data()->myRootProductIds.Append(aProdId);
      }
    }
  }

  // Pre-allocate transient cache for lock-free parallel access on the copied graph.
  reserveTransientCache(aResult);

  // Drain deferred cache transfers AFTER all mutations: Set captures the final SubtreeGen,
  // so subsequent Get calls match.
  aDeferred.Drain(theGraph, aResult);

  return aResult;
}

//=================================================================================================
// Internal helpers for CopyNode.
namespace
{

// Helper: collect all face definitions transitively reachable from a solid.
void collectFacesFromSolid(const BRepGraph&                         theGraph,
                           const BRepGraph_SolidId                  theSolid,
                           NCollection_IndexedMap<BRepGraph_FaceId>& theFaceSet)
{
  for (BRepGraph_RefsShellOfSolid aShellIt(theGraph, theSolid); aShellIt.More(); aShellIt.Next())
  {
    const BRepGraphInc::ShellRef& aSR = theGraph.Refs().Shells().Entry(aShellIt.CurrentId());
    for (BRepGraph_RefsFaceOfShell aFaceIt(theGraph, aSR.ShellDefId); aFaceIt.More();
         aFaceIt.Next())
    {
      const BRepGraphInc::FaceRef& aFR = theGraph.Refs().Faces().Entry(aFaceIt.CurrentId());
      theFaceSet.Add(aFR.FaceDefId);
    }
  }
}

// Helper: collect all face definitions from a shell.
void collectFacesFromShell(const BRepGraph&                         theGraph,
                           const BRepGraph_ShellId                  theShell,
                           NCollection_IndexedMap<BRepGraph_FaceId>& theFaceSet)
{
  for (BRepGraph_RefsFaceOfShell aFaceIt(theGraph, theShell); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraphInc::FaceRef& aFR = theGraph.Refs().Faces().Entry(aFaceIt.CurrentId());
    theFaceSet.Add(aFR.FaceDefId);
  }
}

} // anonymous namespace

//=================================================================================================

BRepGraph BRepGraph_Copy::CopyNode(const BRepGraph&       theGraph,
                                   const BRepGraph_NodeId theNodeId,
                                   const bool             theCopyGeom,
                                   const bool             theReserveCache)
{
  if (!theGraph.IsDone())
    return BRepGraph();

  using Kind = BRepGraph_NodeId::Kind;
  switch (theNodeId.NodeKind)
  {
    case Kind::Vertex:
      return copyVertex(theGraph, BRepGraph_VertexId(theNodeId.Index), theCopyGeom, theReserveCache);
    case Kind::Edge:
      return copyEdge(theGraph, BRepGraph_EdgeId(theNodeId.Index), theCopyGeom, theReserveCache);
    case Kind::Wire:
      return copyWire(theGraph, BRepGraph_WireId(theNodeId.Index), theCopyGeom, theReserveCache);
    case Kind::Face:
      return copyFace(theGraph, BRepGraph_FaceId(theNodeId.Index), theCopyGeom, theReserveCache);
    case Kind::Shell:
      return copyShell(theGraph, BRepGraph_ShellId(theNodeId.Index), theCopyGeom, theReserveCache);
    case Kind::Solid:
      return copySolid(theGraph, BRepGraph_SolidId(theNodeId.Index), theCopyGeom, theReserveCache);
    default:
      return BRepGraph();
  }
}

//=================================================================================================

[[nodiscard]] BRepGraph BRepGraph_Copy::copyVertex(const BRepGraph&   theGraph,
                                                   BRepGraph_VertexId theId,
                                                   bool               theCopyGeom,
                                                   bool               theReserveCache)
{
  (void)theCopyGeom;
  const BRepGraph_VertexId aVtxId = theId;
  if (!aVtxId.IsValidIn(theGraph.Topo().Vertices()))
    return BRepGraph();

  BRepGraph aResult;
  const BRepGraphInc::VertexDef& aVtx = theGraph.Topo().Vertices().Definition(aVtxId);
  (void)aResult.Editor().Vertices().Add(aVtx.Point, aVtx.Tolerance);
  aResult.data()->myIsDone = true;
  if (theReserveCache)
    reserveTransientCache(aResult);
  return aResult;
}

//=================================================================================================

[[nodiscard]] BRepGraph BRepGraph_Copy::copyEdge(const BRepGraph& theGraph,
                                                 BRepGraph_EdgeId theId,
                                                 bool             theCopyGeom,
                                                 bool             theReserveCache)
{
  const BRepGraph_EdgeId anEdgeId = theId;
  if (!anEdgeId.IsValidIn(theGraph.Topo().Edges()))
    return BRepGraph();

  const BRepGraphInc::EdgeDef& anEdge = theGraph.Topo().Edges().Definition(anEdgeId);
  BRepGraph aResult;
  DeferredCacheTransfers aDeferred;

  NCollection_DataMap<BRepGraph_VertexId, BRepGraph_VertexId> aVertexMap(2);
  const auto addVtx = [&](const BRepGraph_VertexId theOldId) -> BRepGraph_VertexId {
    if (!theOldId.IsValid(theGraph.Topo().Vertices().Nb()))
      return BRepGraph_VertexId();
    const BRepGraph_VertexId* aNew = aVertexMap.Seek(theOldId);
    if (aNew != nullptr)
      return *aNew;
    const BRepGraphInc::VertexDef& aVtx = theGraph.Topo().Vertices().Definition(theOldId);
    const BRepGraph_VertexId       aNewId(aVertexMap.Extent());
    (void)aResult.Editor().Vertices().Add(aVtx.Point, aVtx.Tolerance);
    aVertexMap.Bind(theOldId, aNewId);
    aDeferred.DeferNode(theGraph, theOldId, aNewId);
    return aNewId;
  };

  const BRepGraph_VertexId aNewStart =
    addVtx(BRepGraph_Tool::Edge::StartVertexId(theGraph, anEdgeId));
  const BRepGraph_VertexId aNewEnd =
    addVtx(BRepGraph_Tool::Edge::EndVertexId(theGraph, anEdgeId));

  const occ::handle<Geom_Curve>& aSrcCurve = BRepGraph_Tool::Edge::Curve(theGraph, anEdgeId);
  occ::handle<Geom_Curve>        aCurve     = copyCurve(aSrcCurve, theCopyGeom);
  const BRepGraph_EdgeId         aNewEdgeId(0);
  (void)aResult.Editor().Edges().Add(
    aNewStart, aNewEnd, aCurve, anEdge.ParamFirst, anEdge.ParamLast, anEdge.Tolerance);
  {
    BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aG = aResult.Editor().Edges().Mut(aNewEdgeId);
    aG.Internal().IsDegenerate  = anEdge.IsDegenerate;
    aG.Internal().SameParameter = anEdge.SameParameter;
    aG.Internal().SameRange     = anEdge.SameRange;
    aG.MarkDirty();
  }
  aDeferred.DeferNode(theGraph, anEdgeId, aNewEdgeId);
  aResult.data()->myIsDone = true;
  if (theReserveCache)
    reserveTransientCache(aResult);
  aDeferred.Drain(theGraph, aResult);
  return aResult;
}

//=================================================================================================

[[nodiscard]] BRepGraph BRepGraph_Copy::copyWire(const BRepGraph& theGraph,
                                                 BRepGraph_WireId theId,
                                                 bool             theCopyGeom,
                                                 bool             theReserveCache)
{
  const BRepGraph_WireId aWireId = theId;
  if (!aWireId.IsValidIn(theGraph.Topo().Wires()))
    return BRepGraph();

  BRepGraph aResult;
  DeferredCacheTransfers aDeferred;

  NCollection_IndexedMap<BRepGraph_VertexId> aVertexSet;
  NCollection_IndexedMap<BRepGraph_EdgeId>   anEdgeSet;

  for (BRepGraph_RefsCoEdgeOfWire aCEIt(theGraph, aWireId); aCEIt.More(); aCEIt.Next())
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge =
      theGraph.Topo().CoEdges().Definition(
        theGraph.Refs().CoEdges().Entry(aCEIt.CurrentId()).CoEdgeDefId);
    anEdgeSet.Add(aCoEdge.EdgeDefId);
    const BRepGraph_VertexId aStartVtx =
      BRepGraph_Tool::Edge::StartVertexId(theGraph, aCoEdge.EdgeDefId);
    if (aStartVtx.IsValid(theGraph.Topo().Vertices().Nb()))
      aVertexSet.Add(aStartVtx);
    const BRepGraph_VertexId anEndVtx =
      BRepGraph_Tool::Edge::EndVertexId(theGraph, aCoEdge.EdgeDefId);
    if (anEndVtx.IsValid(theGraph.Topo().Vertices().Nb()))
      aVertexSet.Add(anEndVtx);
  }

  NCollection_DataMap<BRepGraph_VertexId, BRepGraph_VertexId> aVertexMap(
    aVertexSet.Extent());
  for (int i = 1; i <= aVertexSet.Extent(); ++i)
  {
    const BRepGraph_VertexId      anOld = aVertexSet.FindKey(i);
    const BRepGraphInc::VertexDef& aVtx = theGraph.Topo().Vertices().Definition(anOld);
    const BRepGraph_VertexId       aNew(i - 1);
    (void)aResult.Editor().Vertices().Add(aVtx.Point, aVtx.Tolerance);
    aVertexMap.Bind(anOld, aNew);
    aDeferred.DeferNode(theGraph, anOld, aNew);
  }

  NCollection_DataMap<BRepGraph_EdgeId, BRepGraph_EdgeId> anEdgeMap(anEdgeSet.Extent());
  for (int i = 1; i <= anEdgeSet.Extent(); ++i)
  {
    const BRepGraph_EdgeId       anOld = anEdgeSet.FindKey(i);
    const BRepGraphInc::EdgeDef& anEdge = theGraph.Topo().Edges().Definition(anOld);
    const BRepGraph_VertexId*    aSt    =
      aVertexMap.Seek(BRepGraph_Tool::Edge::StartVertexId(theGraph, anOld));
    const BRepGraph_VertexId*    aEn    =
      aVertexMap.Seek(BRepGraph_Tool::Edge::EndVertexId(theGraph, anOld));
    const occ::handle<Geom_Curve>& aSrcC = BRepGraph_Tool::Edge::Curve(theGraph, anOld);
    occ::handle<Geom_Curve>        aC    = copyCurve(aSrcC, theCopyGeom);
    const BRepGraph_EdgeId         aNew(i - 1);
    (void)aResult.Editor().Edges().Add(aSt != nullptr ? *aSt : BRepGraph_VertexId(),
                                       aEn != nullptr ? *aEn : BRepGraph_VertexId(),
                                       aC,
                                       anEdge.ParamFirst,
                                       anEdge.ParamLast,
                                       anEdge.Tolerance);
    {
      BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aG = aResult.Editor().Edges().Mut(aNew);
      aG.Internal().IsDegenerate  = anEdge.IsDegenerate;
      aG.Internal().SameParameter = anEdge.SameParameter;
      aG.Internal().SameRange     = anEdge.SameRange;
      aG.MarkDirty();
    }
    anEdgeMap.Bind(anOld, aNew);
    aDeferred.DeferNode(theGraph, anOld, aNew);
  }

  NCollection_DynamicArray<std::pair<BRepGraph_EdgeId, TopAbs_Orientation>> aNewEntries;
  for (BRepGraph_RefsCoEdgeOfWire aCEIt(theGraph, aWireId); aCEIt.More(); aCEIt.Next())
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge =
      theGraph.Topo().CoEdges().Definition(
        theGraph.Refs().CoEdges().Entry(aCEIt.CurrentId()).CoEdgeDefId);
    const BRepGraph_EdgeId* aNewE = anEdgeMap.Seek(aCoEdge.EdgeDefId);
    if (aNewE == nullptr)
      continue;
    aNewEntries.Append(std::make_pair(*aNewE, aCoEdge.Orientation));
  }
  (void)aResult.Editor().Wires().Add(aNewEntries);
  aDeferred.DeferNode(theGraph, aWireId, BRepGraph_WireId::Start());
  aResult.data()->myIsDone = true;
  if (theReserveCache)
    reserveTransientCache(aResult);
  aDeferred.Drain(theGraph, aResult);
  return aResult;
}

//=================================================================================================

[[nodiscard]] BRepGraph BRepGraph_Copy::copyFace(const BRepGraph& theGraph,
                                                 BRepGraph_FaceId theId,
                                                 bool             theCopyGeom,
                                                 bool             theReserveCache)
{
  const BRepGraph_FaceId aFaceId = theId;
  if (!aFaceId.IsValidIn(theGraph.Topo().Faces()))
    return BRepGraph();
  BRepGraph aResult;
  const BRepGraph::RefsView&   aRefs    = theGraph.Refs();
  const BRepGraphInc::FaceDef& aFaceDef = theGraph.Topo().Faces().Definition(aFaceId);
  DeferredCacheTransfers       aDeferred;

  // Check that the face has at least one active wire via typed iterator.
  BRepGraph_RefsWireOfFace aHasWiresIt(theGraph, aFaceId);
  if (!aHasWiresIt.More())
    return aResult;

  // Use NCollection_IndexedMap to collect old indices in deterministic insertion order.
  NCollection_IndexedMap<BRepGraph_VertexId> aVertexSet;
  NCollection_IndexedMap<BRepGraph_EdgeId>   anEdgeSet;
  NCollection_IndexedMap<BRepGraph_WireId>   aWireSet;

  // Collect all edges/vertices/wires from the face's wire refs.
  for (BRepGraph_RefsWireOfFace aWireIt(theGraph, aFaceId); aWireIt.More(); aWireIt.Next())
  {
    const BRepGraphInc::WireRef& aWR = aRefs.Wires().Entry(aWireIt.CurrentId());
    aWireSet.Add(aWR.WireDefId);

    for (BRepGraph_RefsCoEdgeOfWire aCEIt(theGraph, aWR.WireDefId); aCEIt.More(); aCEIt.Next())
    {
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        theGraph.Topo().CoEdges().Definition(aRefs.CoEdges().Entry(aCEIt.CurrentId()).CoEdgeDefId);
      anEdgeSet.Add(aCoEdge.EdgeDefId);

      const BRepGraph_EdgeId   anEdgeId  = aCoEdge.EdgeDefId;
      const BRepGraph_VertexId aStartVtx = BRepGraph_Tool::Edge::StartVertexId(theGraph, anEdgeId);
      if (aStartVtx.IsValid(theGraph.Topo().Vertices().Nb()))
        aVertexSet.Add(aStartVtx);
      const BRepGraph_VertexId anEndVtx = BRepGraph_Tool::Edge::EndVertexId(theGraph, anEdgeId);
      if (anEndVtx.IsValid(theGraph.Topo().Vertices().Nb()))
        aVertexSet.Add(anEndVtx);
    }
  }

  // Build old->new index maps from the ordered sets.
  NCollection_DataMap<BRepGraph_VertexId, BRepGraph_VertexId> aVertexMap(aVertexSet.Extent());
  for (int anIdx = 1; anIdx <= aVertexSet.Extent(); ++anIdx)
    aVertexMap.Bind(aVertexSet.FindKey(anIdx), BRepGraph_VertexId(anIdx - 1));

  NCollection_DataMap<BRepGraph_EdgeId, BRepGraph_EdgeId> anEdgeMap(anEdgeSet.Extent());
  for (int anIdx = 1; anIdx <= anEdgeSet.Extent(); ++anIdx)
    anEdgeMap.Bind(anEdgeSet.FindKey(anIdx), BRepGraph_EdgeId(anIdx - 1));

  NCollection_DataMap<BRepGraph_WireId, BRepGraph_WireId> aWireMap(aWireSet.Extent());
  for (int anIdx = 1; anIdx <= aWireSet.Extent(); ++anIdx)
    aWireMap.Bind(aWireSet.FindKey(anIdx), BRepGraph_WireId(anIdx - 1));

  // Add vertices in deterministic order.
  for (int anIdx = 1; anIdx <= aVertexSet.Extent(); ++anIdx)
  {
    const BRepGraph_VertexId       anOldVtxId = aVertexSet.FindKey(anIdx);
    const BRepGraphInc::VertexDef& aVtx       = theGraph.Topo().Vertices().Definition(anOldVtxId);
    const BRepGraph_VertexId       aNewVtxId(anIdx - 1);
    (void)aResult.Editor().Vertices().Add(aVtx.Point, aVtx.Tolerance);
    aDeferred.DeferNode(theGraph, anOldVtxId, aNewVtxId);
  }

  // Add edges in deterministic order.
  for (int anIdx = 1; anIdx <= anEdgeSet.Extent(); ++anIdx)
  {
    const BRepGraph_EdgeId       anOldEdgeId = anEdgeSet.FindKey(anIdx);
    const BRepGraphInc::EdgeDef& anEdge      = theGraph.Topo().Edges().Definition(anOldEdgeId);

    BRepGraph_VertexId       aNewStart, aNewEnd;
    const BRepGraph_VertexId aStartVtx = BRepGraph_Tool::Edge::StartVertexId(theGraph, anOldEdgeId);
    if (aStartVtx.IsValid())
    {
      const BRepGraph_VertexId* aNewVtxId = aVertexMap.Seek(aStartVtx);
      if (aNewVtxId != nullptr)
        aNewStart = *aNewVtxId;
    }
    const BRepGraph_VertexId anEndVtx = BRepGraph_Tool::Edge::EndVertexId(theGraph, anOldEdgeId);
    if (anEndVtx.IsValid())
    {
      const BRepGraph_VertexId* aNewVtxId = aVertexMap.Seek(anEndVtx);
      if (aNewVtxId != nullptr)
        aNewEnd = *aNewVtxId;
    }

    const occ::handle<Geom_Curve>& anEdgeSrcCurve =
      BRepGraph_Tool::Edge::Curve(theGraph, anOldEdgeId);
    occ::handle<Geom_Curve> aCurve = copyCurve(anEdgeSrcCurve, theCopyGeom);

    const BRepGraph_EdgeId aNewEdgeId(anIdx - 1);
    (void)aResult.Editor().Edges().Add(aNewStart,
                                       aNewEnd,
                                       aCurve,
                                       anEdge.ParamFirst,
                                       anEdge.ParamLast,
                                       anEdge.Tolerance);

    {
      BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aNewEdge = aResult.Editor().Edges().Mut(aNewEdgeId);
      aNewEdge.Internal().IsDegenerate                   = anEdge.IsDegenerate;
      aNewEdge.Internal().SameParameter                  = anEdge.SameParameter;
      aNewEdge.Internal().SameRange                      = anEdge.SameRange;
      aNewEdge.MarkDirty();
    }
    aDeferred.DeferNode(theGraph, anOldEdgeId, aNewEdgeId);
  }

  // Add wires in deterministic order.
  for (int anIdx = 1; anIdx <= aWireSet.Extent(); ++anIdx)
  {
    const BRepGraph_WireId anOldWireId = aWireSet.FindKey(anIdx);
    NCollection_DynamicArray<std::pair<BRepGraph_EdgeId, TopAbs_Orientation>> aNewEntries;
    for (BRepGraph_RefsCoEdgeOfWire aCEIt(theGraph, anOldWireId); aCEIt.More(); aCEIt.Next())
    {
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        theGraph.Topo().CoEdges().Definition(aRefs.CoEdges().Entry(aCEIt.CurrentId()).CoEdgeDefId);
      const BRepGraph_EdgeId* aNewEdgeId = anEdgeMap.Seek(aCoEdge.EdgeDefId);
      if (aNewEdgeId == nullptr)
        continue;
      aNewEntries.Append(std::make_pair(*aNewEdgeId, aCoEdge.Orientation));
    }
    (void)aResult.Editor().Wires().Add(aNewEntries);
    aDeferred.DeferNode(theGraph, anOldWireId, BRepGraph_WireId(anIdx - 1));
  }

  // Add the face.
  const occ::handle<Geom_Surface>& aFaceSrcSurf = BRepGraph_Tool::Face::Surface(theGraph, aFaceId);
  occ::handle<Geom_Surface>        aSurf        = copySurface(aFaceSrcSurf, theCopyGeom);

  BRepGraph_WireId                           anOuterWire;
  NCollection_DynamicArray<BRepGraph_WireId> anInnerWires;

  for (BRepGraph_RefsWireOfFace aWRIt(theGraph, aFaceId); aWRIt.More(); aWRIt.Next())
  {
    const BRepGraphInc::WireRef& aWR        = aRefs.Wires().Entry(aWRIt.CurrentId());
    const BRepGraph_WireId*      aNewWireId = aWireMap.Seek(aWR.WireDefId);
    if (aNewWireId == nullptr)
      continue;
    if (aWR.IsOuter)
    {
      anOuterWire = *aNewWireId;
    }
    else
    {
      anInnerWires.Append(*aNewWireId);
    }
  }

  (void)aResult.Editor().Faces().Add(aSurf, anOuterWire, anInnerWires, aFaceDef.Tolerance);
  {
    BRepGraph_MutGuard<BRepGraphInc::FaceDef> aNewFace =
      aResult.Editor().Faces().Mut(BRepGraph_FaceId::Start());
    aNewFace.Internal().NaturalRestriction = aFaceDef.NaturalRestriction;
    aNewFace.Internal().TriangulationRepId = aFaceDef.TriangulationRepId;
    aNewFace.MarkDirty();
  }
  // Copy cached mesh data if present.
  const BRepGraph_MeshCache::FaceMeshEntry* aCachedFace =
    theGraph.Mesh().Faces().CachedMesh(aFaceId);
  if (aCachedFace != nullptr)
  {
    BRepGraph_MeshCache::FaceMeshEntry& aNewEntry =
      aResult.meshCache().ChangeFaceMesh(BRepGraph_FaceId::Start());
    aNewEntry = *aCachedFace;
  }
  aDeferred.DeferNode(theGraph, aFaceId, BRepGraph_FaceId::Start());

  // PCurves for edges in this face via CoEdge data.
  for (int anIdx = 1; anIdx <= anEdgeSet.Extent(); ++anIdx)
  {
    const BRepGraph_EdgeId                              anOldEdgeId = anEdgeSet.FindKey(anIdx);
    const BRepGraph_EdgeId                              aNewEdgeId(anIdx - 1);
    const NCollection_DynamicArray<BRepGraph_CoEdgeId>& aCoEdgeIds =
      theGraph.Topo().Edges().CoEdges(anOldEdgeId);
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdgeIds)
    {
      const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(aCoEdgeId);
      // Only copy CoEdges belonging to this face.
      if (aCoEdge.FaceDefId != aFaceId)
        continue;
      if (!aCoEdge.Curve2DRepId.IsValid())
        continue;

      const occ::handle<Geom2d_Curve>& aCoEdgeSrcPC =
        BRepGraph_Tool::CoEdge::PCurve(theGraph, aCoEdgeId);
      occ::handle<Geom2d_Curve> aNewPC = copyPCurve(aCoEdgeSrcPC, theCopyGeom);
      aResult.Editor().CoEdges().AddPCurve(aNewEdgeId,
                                           BRepGraph_FaceId::Start(),
                                           aNewPC,
                                           aCoEdge.ParamFirst,
                                           aCoEdge.ParamLast,
                                           aCoEdge.Orientation);
    }
  }

  aResult.data()->myIsDone = true;

  // Pre-allocate transient cache for lock-free parallel access on the copied graph.
  // Skip for short-lived temporary graphs where cache is never queried.
  if (theReserveCache)
  {
    reserveTransientCache(aResult);
  }

  // Drain deferred cache transfers AFTER all mutations: Set captures the final SubtreeGen.
  aDeferred.Drain(theGraph, aResult);

  return aResult;
}

//=================================================================================================

[[nodiscard]] BRepGraph BRepGraph_Copy::copyShell(const BRepGraph&  theGraph,
                                                  BRepGraph_ShellId theId,
                                                  bool              theCopyGeom,
                                                  bool              theReserveCache)
{
  const BRepGraph_ShellId aShellId = theId;
  if (!aShellId.IsValidIn(theGraph.Topo().Shells()))
    return BRepGraph();

  const BRepGraphInc::ShellDef& aShellDef = theGraph.Topo().Shells().Definition(aShellId);
  NCollection_IndexedMap<BRepGraph_FaceId> aFaceSet;
  collectFacesFromShell(theGraph, aShellId, aFaceSet);
  if (aFaceSet.IsEmpty())
    return BRepGraph();

  // Collect all edges/vertices/wires across all faces (deduplicated).
  NCollection_IndexedMap<BRepGraph_VertexId> aVertexSet;
  NCollection_IndexedMap<BRepGraph_EdgeId>   anEdgeSet;
  NCollection_IndexedMap<BRepGraph_WireId>   aWireSet;
  for (int iFace = 1; iFace <= aFaceSet.Extent(); ++iFace)
  {
    const BRepGraph_FaceId aFaceId = aFaceSet.FindKey(iFace);
    for (BRepGraph_RefsWireOfFace aWireIt(theGraph, aFaceId); aWireIt.More(); aWireIt.Next())
    {
      const BRepGraphInc::WireRef& aWR = theGraph.Refs().Wires().Entry(aWireIt.CurrentId());
      aWireSet.Add(aWR.WireDefId);
      for (BRepGraph_RefsCoEdgeOfWire aCEIt(theGraph, aWR.WireDefId); aCEIt.More();
           aCEIt.Next())
      {
        const BRepGraphInc::CoEdgeDef& aCE = theGraph.Topo().CoEdges().Definition(
          theGraph.Refs().CoEdges().Entry(aCEIt.CurrentId()).CoEdgeDefId);
        anEdgeSet.Add(aCE.EdgeDefId);
        const BRepGraph_VertexId aS = BRepGraph_Tool::Edge::StartVertexId(theGraph, aCE.EdgeDefId);
        if (aS.IsValid(theGraph.Topo().Vertices().Nb()))
          aVertexSet.Add(aS);
        const BRepGraph_VertexId aE = BRepGraph_Tool::Edge::EndVertexId(theGraph, aCE.EdgeDefId);
        if (aE.IsValid(theGraph.Topo().Vertices().Nb()))
          aVertexSet.Add(aE);
      }
    }
  }

  BRepGraph aResult;
  DeferredCacheTransfers aDeferred;

  NCollection_DataMap<BRepGraph_VertexId, BRepGraph_VertexId> aVertexMap(aVertexSet.Extent());
  for (int i = 1; i <= aVertexSet.Extent(); ++i)
  {
    const BRepGraph_VertexId      anOld = aVertexSet.FindKey(i);
    const BRepGraphInc::VertexDef& aVtx = theGraph.Topo().Vertices().Definition(anOld);
    const BRepGraph_VertexId       aNew(i - 1);
    (void)aResult.Editor().Vertices().Add(aVtx.Point, aVtx.Tolerance);
    aVertexMap.Bind(anOld, aNew);
    aDeferred.DeferNode(theGraph, anOld, aNew);
  }

  NCollection_DataMap<BRepGraph_EdgeId, BRepGraph_EdgeId> anEdgeMap(anEdgeSet.Extent());
  for (int i = 1; i <= anEdgeSet.Extent(); ++i)
  {
    const BRepGraph_EdgeId       anOld  = anEdgeSet.FindKey(i);
    const BRepGraphInc::EdgeDef& anEdge = theGraph.Topo().Edges().Definition(anOld);
    const BRepGraph_VertexId*    aSt    =
      aVertexMap.Seek(BRepGraph_Tool::Edge::StartVertexId(theGraph, anOld));
    const BRepGraph_VertexId*    aEn    =
      aVertexMap.Seek(BRepGraph_Tool::Edge::EndVertexId(theGraph, anOld));
    const occ::handle<Geom_Curve>& aSrcC = BRepGraph_Tool::Edge::Curve(theGraph, anOld);
    occ::handle<Geom_Curve>        aC    = copyCurve(aSrcC, theCopyGeom);
    const BRepGraph_EdgeId         aNew(i - 1);
    (void)aResult.Editor().Edges().Add(aSt != nullptr ? *aSt : BRepGraph_VertexId(),
                                       aEn != nullptr ? *aEn : BRepGraph_VertexId(),
                                       aC,
                                       anEdge.ParamFirst,
                                       anEdge.ParamLast,
                                       anEdge.Tolerance);
    {
      BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aG = aResult.Editor().Edges().Mut(aNew);
      aG.Internal().IsDegenerate  = anEdge.IsDegenerate;
      aG.Internal().SameParameter = anEdge.SameParameter;
      aG.Internal().SameRange     = anEdge.SameRange;
      aG.MarkDirty();
    }
    anEdgeMap.Bind(anOld, aNew);
    aDeferred.DeferNode(theGraph, anOld, aNew);
  }

  NCollection_DataMap<BRepGraph_WireId, BRepGraph_WireId> aWireMap(aWireSet.Extent());
  for (int i = 1; i <= aWireSet.Extent(); ++i)
  {
    const BRepGraph_WireId anOld = aWireSet.FindKey(i);
    NCollection_DynamicArray<std::pair<BRepGraph_EdgeId, TopAbs_Orientation>> aNewEntries;
    for (BRepGraph_RefsCoEdgeOfWire aCEIt(theGraph, anOld); aCEIt.More(); aCEIt.Next())
    {
      const BRepGraphInc::CoEdgeDef& aCE = theGraph.Topo().CoEdges().Definition(
        theGraph.Refs().CoEdges().Entry(aCEIt.CurrentId()).CoEdgeDefId);
      const BRepGraph_EdgeId* aNewE = anEdgeMap.Seek(aCE.EdgeDefId);
      if (aNewE == nullptr)
        continue;
      aNewEntries.Append(std::make_pair(*aNewE, aCE.Orientation));
    }
    (void)aResult.Editor().Wires().Add(aNewEntries);
    aWireMap.Bind(anOld, BRepGraph_WireId(i - 1));
    aDeferred.DeferNode(theGraph, anOld, BRepGraph_WireId(i - 1));
  }

  NCollection_DataMap<BRepGraph_FaceId, BRepGraph_FaceId> aFaceMap(aFaceSet.Extent());
  for (int i = 1; i <= aFaceSet.Extent(); ++i)
  {
    const BRepGraph_FaceId       anOld   = aFaceSet.FindKey(i);
    const BRepGraphInc::FaceDef& aFaceDef = theGraph.Topo().Faces().Definition(anOld);
    const occ::handle<Geom_Surface>& aSrcS = BRepGraph_Tool::Face::Surface(theGraph, anOld);
    occ::handle<Geom_Surface>        aS    = copySurface(aSrcS, theCopyGeom);

    BRepGraph_WireId anOuterWire;
    NCollection_DynamicArray<BRepGraph_WireId> anInnerWires;
    for (BRepGraph_RefsWireOfFace aWRIt(theGraph, anOld); aWRIt.More(); aWRIt.Next())
    {
      const BRepGraphInc::WireRef& aWR      = theGraph.Refs().Wires().Entry(aWRIt.CurrentId());
      const BRepGraph_WireId*      aNewWire = aWireMap.Seek(aWR.WireDefId);
      if (aNewWire == nullptr)
        continue;
      if (aWR.IsOuter)
        anOuterWire = *aNewWire;
      else
        anInnerWires.Append(*aNewWire);
    }
    const BRepGraph_FaceId aNew(i - 1);
    (void)aResult.Editor().Faces().Add(aS, anOuterWire, anInnerWires, aFaceDef.Tolerance);
    {
      BRepGraph_MutGuard<BRepGraphInc::FaceDef> aG = aResult.Editor().Faces().Mut(aNew);
      aG.Internal().NaturalRestriction = aFaceDef.NaturalRestriction;
      aG.MarkDirty();
    }
    aFaceMap.Bind(anOld, aNew);
    aDeferred.DeferNode(theGraph, anOld, aNew);

    // PCurves.
    for (int iE = 1; iE <= anEdgeSet.Extent(); ++iE)
    {
      const BRepGraph_EdgeId  anOldEdge = anEdgeSet.FindKey(iE);
      const BRepGraph_EdgeId  aNewEdge(iE - 1);
      const NCollection_DynamicArray<BRepGraph_CoEdgeId>& aCEIds =
        theGraph.Topo().Edges().CoEdges(anOldEdge);
      for (const BRepGraph_CoEdgeId& aCEId : aCEIds)
      {
        const BRepGraphInc::CoEdgeDef& aCE = theGraph.Topo().CoEdges().Definition(aCEId);
        if (aCE.FaceDefId != anOld)
          continue;
        if (!aCE.Curve2DRepId.IsValid())
          continue;
        const occ::handle<Geom2d_Curve>& aSrcPC = BRepGraph_Tool::CoEdge::PCurve(theGraph, aCEId);
        occ::handle<Geom2d_Curve>        aNewPC  = copyPCurve(aSrcPC, theCopyGeom);
        aResult.Editor().CoEdges().AddPCurve(
          aNewEdge, aNew, aNewPC, aCE.ParamFirst, aCE.ParamLast, aCE.Orientation);
      }
    }
  }

  // Add shell wrapping all faces (using the shell's own FaceRef entries for orientation).
  const BRepGraph_ShellId aNewShell = aResult.Editor().Shells().Add();
  for (BRepGraph_RefsFaceOfShell aFaceIt(theGraph, aShellId); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraphInc::FaceRef& aFR      = theGraph.Refs().Faces().Entry(aFaceIt.CurrentId());
    const BRepGraph_FaceId*      aNewFace = aFaceMap.Seek(aFR.FaceDefId);
    if (aNewFace == nullptr)
      continue;
    (void)aResult.Editor().Shells().AddFace(aNewShell, *aNewFace, aFR.Orientation);
  }
  {
    BRepGraph_MutGuard<BRepGraphInc::ShellDef> aG = aResult.Editor().Shells().Mut(aNewShell);
    aG.Internal().IsClosed = aShellDef.IsClosed;
    aG.MarkDirty();
  }
  aDeferred.DeferNode(theGraph, aShellId, aNewShell);
  aResult.data()->myIsDone = true;
  if (theReserveCache)
    reserveTransientCache(aResult);
  aDeferred.Drain(theGraph, aResult);
  return aResult;
}

//=================================================================================================

[[nodiscard]] BRepGraph BRepGraph_Copy::copySolid(const BRepGraph&  theGraph,
                                                  BRepGraph_SolidId theId,
                                                  bool              theCopyGeom,
                                                  bool              theReserveCache)
{
  const BRepGraph_SolidId aSolidId = theId;
  if (!aSolidId.IsValidIn(theGraph.Topo().Solids()))
    return BRepGraph();

  // Collect ordered face sets per shell.
  NCollection_IndexedMap<BRepGraph_ShellId> aShellSet;
  for (BRepGraph_RefsShellOfSolid aShellIt(theGraph, aSolidId); aShellIt.More();
       aShellIt.Next())
  {
    const BRepGraphInc::ShellRef& aSR = theGraph.Refs().Shells().Entry(aShellIt.CurrentId());
    aShellSet.Add(aSR.ShellDefId);
  }
  if (aShellSet.IsEmpty())
    return BRepGraph();

  NCollection_IndexedMap<BRepGraph_FaceId> aFaceSet;
  collectFacesFromSolid(theGraph, aSolidId, aFaceSet);

  // Collect all edges/vertices/wires.
  NCollection_IndexedMap<BRepGraph_VertexId> aVertexSet;
  NCollection_IndexedMap<BRepGraph_EdgeId>   anEdgeSet;
  NCollection_IndexedMap<BRepGraph_WireId>   aWireSet;
  for (int iFace = 1; iFace <= aFaceSet.Extent(); ++iFace)
  {
    const BRepGraph_FaceId aFaceId = aFaceSet.FindKey(iFace);
    for (BRepGraph_RefsWireOfFace aWireIt(theGraph, aFaceId); aWireIt.More(); aWireIt.Next())
    {
      const BRepGraphInc::WireRef& aWR = theGraph.Refs().Wires().Entry(aWireIt.CurrentId());
      aWireSet.Add(aWR.WireDefId);
      for (BRepGraph_RefsCoEdgeOfWire aCEIt(theGraph, aWR.WireDefId); aCEIt.More();
           aCEIt.Next())
      {
        const BRepGraphInc::CoEdgeDef& aCE = theGraph.Topo().CoEdges().Definition(
          theGraph.Refs().CoEdges().Entry(aCEIt.CurrentId()).CoEdgeDefId);
        anEdgeSet.Add(aCE.EdgeDefId);
        const BRepGraph_VertexId aS =
          BRepGraph_Tool::Edge::StartVertexId(theGraph, aCE.EdgeDefId);
        if (aS.IsValid(theGraph.Topo().Vertices().Nb()))
          aVertexSet.Add(aS);
        const BRepGraph_VertexId aE =
          BRepGraph_Tool::Edge::EndVertexId(theGraph, aCE.EdgeDefId);
        if (aE.IsValid(theGraph.Topo().Vertices().Nb()))
          aVertexSet.Add(aE);
      }
    }
  }

  BRepGraph aResult;
  DeferredCacheTransfers aDeferred;

  NCollection_DataMap<BRepGraph_VertexId, BRepGraph_VertexId> aVertexMap(aVertexSet.Extent());
  for (int i = 1; i <= aVertexSet.Extent(); ++i)
  {
    const BRepGraph_VertexId      anOld = aVertexSet.FindKey(i);
    const BRepGraphInc::VertexDef& aVtx = theGraph.Topo().Vertices().Definition(anOld);
    const BRepGraph_VertexId       aNew(i - 1);
    (void)aResult.Editor().Vertices().Add(aVtx.Point, aVtx.Tolerance);
    aVertexMap.Bind(anOld, aNew);
    aDeferred.DeferNode(theGraph, anOld, aNew);
  }

  NCollection_DataMap<BRepGraph_EdgeId, BRepGraph_EdgeId> anEdgeMap(anEdgeSet.Extent());
  for (int i = 1; i <= anEdgeSet.Extent(); ++i)
  {
    const BRepGraph_EdgeId       anOld  = anEdgeSet.FindKey(i);
    const BRepGraphInc::EdgeDef& anEdge = theGraph.Topo().Edges().Definition(anOld);
    const BRepGraph_VertexId*    aSt    =
      aVertexMap.Seek(BRepGraph_Tool::Edge::StartVertexId(theGraph, anOld));
    const BRepGraph_VertexId*    aEn    =
      aVertexMap.Seek(BRepGraph_Tool::Edge::EndVertexId(theGraph, anOld));
    const occ::handle<Geom_Curve>& aSrcC = BRepGraph_Tool::Edge::Curve(theGraph, anOld);
    occ::handle<Geom_Curve>        aC    = copyCurve(aSrcC, theCopyGeom);
    const BRepGraph_EdgeId         aNew(i - 1);
    (void)aResult.Editor().Edges().Add(aSt != nullptr ? *aSt : BRepGraph_VertexId(),
                                       aEn != nullptr ? *aEn : BRepGraph_VertexId(),
                                       aC,
                                       anEdge.ParamFirst,
                                       anEdge.ParamLast,
                                       anEdge.Tolerance);
    {
      BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aG = aResult.Editor().Edges().Mut(aNew);
      aG.Internal().IsDegenerate  = anEdge.IsDegenerate;
      aG.Internal().SameParameter = anEdge.SameParameter;
      aG.Internal().SameRange     = anEdge.SameRange;
      aG.MarkDirty();
    }
    anEdgeMap.Bind(anOld, aNew);
    aDeferred.DeferNode(theGraph, anOld, aNew);
  }

  NCollection_DataMap<BRepGraph_WireId, BRepGraph_WireId> aWireMap(aWireSet.Extent());
  for (int i = 1; i <= aWireSet.Extent(); ++i)
  {
    const BRepGraph_WireId anOld = aWireSet.FindKey(i);
    NCollection_DynamicArray<std::pair<BRepGraph_EdgeId, TopAbs_Orientation>> aNewEntries;
    for (BRepGraph_RefsCoEdgeOfWire aCEIt(theGraph, anOld); aCEIt.More(); aCEIt.Next())
    {
      const BRepGraphInc::CoEdgeDef& aCE = theGraph.Topo().CoEdges().Definition(
        theGraph.Refs().CoEdges().Entry(aCEIt.CurrentId()).CoEdgeDefId);
      const BRepGraph_EdgeId* aNewE = anEdgeMap.Seek(aCE.EdgeDefId);
      if (aNewE == nullptr)
        continue;
      aNewEntries.Append(std::make_pair(*aNewE, aCE.Orientation));
    }
    (void)aResult.Editor().Wires().Add(aNewEntries);
    aWireMap.Bind(anOld, BRepGraph_WireId(i - 1));
    aDeferred.DeferNode(theGraph, anOld, BRepGraph_WireId(i - 1));
  }

  NCollection_DataMap<BRepGraph_FaceId, BRepGraph_FaceId> aFaceMap(aFaceSet.Extent());
  for (int i = 1; i <= aFaceSet.Extent(); ++i)
  {
    const BRepGraph_FaceId       anOld    = aFaceSet.FindKey(i);
    const BRepGraphInc::FaceDef& aFaceDef = theGraph.Topo().Faces().Definition(anOld);
    const occ::handle<Geom_Surface>& aSrcS = BRepGraph_Tool::Face::Surface(theGraph, anOld);
    occ::handle<Geom_Surface>        aS    = copySurface(aSrcS, theCopyGeom);

    BRepGraph_WireId anOuterWire;
    NCollection_DynamicArray<BRepGraph_WireId> anInnerWires;
    for (BRepGraph_RefsWireOfFace aWRIt(theGraph, anOld); aWRIt.More(); aWRIt.Next())
    {
      const BRepGraphInc::WireRef& aWR      = theGraph.Refs().Wires().Entry(aWRIt.CurrentId());
      const BRepGraph_WireId*      aNewWire = aWireMap.Seek(aWR.WireDefId);
      if (aNewWire == nullptr)
        continue;
      if (aWR.IsOuter)
        anOuterWire = *aNewWire;
      else
        anInnerWires.Append(*aNewWire);
    }
    const BRepGraph_FaceId aNew(i - 1);
    (void)aResult.Editor().Faces().Add(aS, anOuterWire, anInnerWires, aFaceDef.Tolerance);
    {
      BRepGraph_MutGuard<BRepGraphInc::FaceDef> aG = aResult.Editor().Faces().Mut(aNew);
      aG.Internal().NaturalRestriction = aFaceDef.NaturalRestriction;
      aG.MarkDirty();
    }
    aFaceMap.Bind(anOld, aNew);
    aDeferred.DeferNode(theGraph, anOld, aNew);

    // PCurves.
    for (int iE = 1; iE <= anEdgeSet.Extent(); ++iE)
    {
      const BRepGraph_EdgeId anOldEdge = anEdgeSet.FindKey(iE);
      const BRepGraph_EdgeId aNewEdge(iE - 1);
      const NCollection_DynamicArray<BRepGraph_CoEdgeId>& aCEIds =
        theGraph.Topo().Edges().CoEdges(anOldEdge);
      for (const BRepGraph_CoEdgeId& aCEId : aCEIds)
      {
        const BRepGraphInc::CoEdgeDef& aCE = theGraph.Topo().CoEdges().Definition(aCEId);
        if (aCE.FaceDefId != anOld)
          continue;
        if (!aCE.Curve2DRepId.IsValid())
          continue;
        const occ::handle<Geom2d_Curve>& aSrcPC = BRepGraph_Tool::CoEdge::PCurve(theGraph, aCEId);
        occ::handle<Geom2d_Curve>        aNewPC  = copyPCurve(aSrcPC, theCopyGeom);
        aResult.Editor().CoEdges().AddPCurve(
          aNewEdge, aNew, aNewPC, aCE.ParamFirst, aCE.ParamLast, aCE.Orientation);
      }
    }
  }

  // Add shells wrapping their faces, then the solid.
  NCollection_DataMap<BRepGraph_ShellId, BRepGraph_ShellId> aShellMap(aShellSet.Extent());
  for (int iShell = 1; iShell <= aShellSet.Extent(); ++iShell)
  {
    const BRepGraph_ShellId       anOldShell = aShellSet.FindKey(iShell);
    const BRepGraphInc::ShellDef& aShellDef  = theGraph.Topo().Shells().Definition(anOldShell);
    const BRepGraph_ShellId       aNewShell  = aResult.Editor().Shells().Add();

    for (BRepGraph_RefsFaceOfShell aFaceIt(theGraph, anOldShell); aFaceIt.More(); aFaceIt.Next())
    {
      const BRepGraphInc::FaceRef& aFR    = theGraph.Refs().Faces().Entry(aFaceIt.CurrentId());
      const BRepGraph_FaceId*      aNewFace = aFaceMap.Seek(aFR.FaceDefId);
      if (aNewFace == nullptr)
        continue;
      (void)aResult.Editor().Shells().AddFace(aNewShell, *aNewFace, aFR.Orientation);
    }
    {
      BRepGraph_MutGuard<BRepGraphInc::ShellDef> aG =
        aResult.Editor().Shells().Mut(aNewShell);
      aG.Internal().IsClosed = aShellDef.IsClosed;
      aG.MarkDirty();
    }
    aShellMap.Bind(anOldShell, aNewShell);
    aDeferred.DeferNode(theGraph, anOldShell, aNewShell);
  }

  const BRepGraph_SolidId aNewSolid = aResult.Editor().Solids().Add();
  for (BRepGraph_RefsShellOfSolid aShellIt(theGraph, aSolidId); aShellIt.More();
       aShellIt.Next())
  {
    const BRepGraphInc::ShellRef& aSR       = theGraph.Refs().Shells().Entry(aShellIt.CurrentId());
    const BRepGraph_ShellId*      aNewShell = aShellMap.Seek(aSR.ShellDefId);
    if (aNewShell == nullptr)
      continue;
    (void)aResult.Editor().Solids().AddShell(aNewSolid, *aNewShell, aSR.Orientation);
  }
  aDeferred.DeferNode(theGraph, aSolidId, aNewSolid);
  aResult.data()->myIsDone = true;
  if (theReserveCache)
    reserveTransientCache(aResult);
  aDeferred.Drain(theGraph, aResult);
  return aResult;
}
