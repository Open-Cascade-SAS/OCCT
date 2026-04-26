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

//! Memoised copy context shared across all ensure* free functions.
//!
//! Source/result pointers that require friend access (incStorage, meshCache, data)
//! are pre-extracted inside the friend methods Perform / CopyNode before any
//! ensure* function is called.
struct CopyContext
{
  const BRepGraph&            Source;
  BRepGraph                   Result;
  bool                        CopyGeom;
  bool                        ReserveCache;
  DeferredCacheTransfers      Deferred;
  const BRepGraphInc_Storage* SrcStorage = nullptr;
  BRepGraphInc_Storage*       DstStorage = nullptr;
  BRepGraph_MeshCacheStorage* DstMesh    = nullptr;
  BRepGraph_Data*             DstData    = nullptr;

  NCollection_DataMap<BRepGraph_VertexId,        BRepGraph_VertexId>        Vertices;
  NCollection_DataMap<BRepGraph_EdgeId,          BRepGraph_EdgeId>          Edges;
  NCollection_DataMap<BRepGraph_WireId,          BRepGraph_WireId>          Wires;
  NCollection_DataMap<BRepGraph_FaceId,          BRepGraph_FaceId>          Faces;
  NCollection_DataMap<BRepGraph_ShellId,         BRepGraph_ShellId>         Shells;
  NCollection_DataMap<BRepGraph_SolidId,         BRepGraph_SolidId>         Solids;
  NCollection_DataMap<BRepGraph_CompoundId,      BRepGraph_CompoundId>      Compounds;
  NCollection_DataMap<BRepGraph_CompSolidId,     BRepGraph_CompSolidId>     CompSolids;
  NCollection_DataMap<BRepGraph_ProductId,       BRepGraph_ProductId>       Products;
  NCollection_DataMap<BRepGraph_OccurrenceId,    BRepGraph_OccurrenceId>    Occurrences;
  NCollection_DataMap<BRepGraph_OccurrenceRefId, BRepGraph_OccurrenceRefId> OccurrenceRefs;

  explicit CopyContext(const BRepGraph& theSrc, bool theCopyGeom, bool theReserveCache)
      : Source(theSrc), CopyGeom(theCopyGeom), ReserveCache(theReserveCache)
  {
  }
};

// Forward declarations — needed for mutual recursion between ensure* functions.
BRepGraph_VertexId        ensureVertex       (CopyContext& ctx, BRepGraph_VertexId srcId);
BRepGraph_EdgeId          ensureEdge         (CopyContext& ctx, BRepGraph_EdgeId srcId);
BRepGraph_WireId          ensureWire         (CopyContext& ctx, BRepGraph_WireId srcId);
BRepGraph_FaceId          ensureFace         (CopyContext& ctx, BRepGraph_FaceId srcId);
BRepGraph_ShellId         ensureShell        (CopyContext& ctx, BRepGraph_ShellId srcId);
BRepGraph_SolidId         ensureSolid        (CopyContext& ctx, BRepGraph_SolidId srcId);
BRepGraph_CompoundId      ensureCompound     (CopyContext& ctx, BRepGraph_CompoundId srcId);
BRepGraph_CompSolidId     ensureCompSolid    (CopyContext& ctx, BRepGraph_CompSolidId srcId);
BRepGraph_ProductId       ensureProduct      (CopyContext& ctx, BRepGraph_ProductId srcId);
BRepGraph_OccurrenceId    ensureOccurrence   (CopyContext& ctx, BRepGraph_OccurrenceId srcId);
BRepGraph_OccurrenceRefId ensureOccurrenceRef(CopyContext& ctx, BRepGraph_OccurrenceRefId srcRefId);
void                      ensureNode         (CopyContext& ctx, BRepGraph_NodeId srcNodeId);
BRepGraph_NodeId          mappedNode         (const CopyContext& ctx, BRepGraph_NodeId srcId);

//=================================================================================================

BRepGraph_NodeId mappedNode(const CopyContext& ctx, BRepGraph_NodeId srcId)
{
  using Kind = BRepGraph_NodeId::Kind;
  switch (srcId.NodeKind)
  {
    case Kind::Vertex:
    {
      const BRepGraph_VertexId* p = ctx.Vertices.Seek(BRepGraph_VertexId(srcId.Index));
      return p != nullptr ? BRepGraph_NodeId(*p) : BRepGraph_NodeId();
    }
    case Kind::Edge:
    {
      const BRepGraph_EdgeId* p = ctx.Edges.Seek(BRepGraph_EdgeId(srcId.Index));
      return p != nullptr ? BRepGraph_NodeId(*p) : BRepGraph_NodeId();
    }
    case Kind::Wire:
    {
      const BRepGraph_WireId* p = ctx.Wires.Seek(BRepGraph_WireId(srcId.Index));
      return p != nullptr ? BRepGraph_NodeId(*p) : BRepGraph_NodeId();
    }
    case Kind::Face:
    {
      const BRepGraph_FaceId* p = ctx.Faces.Seek(BRepGraph_FaceId(srcId.Index));
      return p != nullptr ? BRepGraph_NodeId(*p) : BRepGraph_NodeId();
    }
    case Kind::Shell:
    {
      const BRepGraph_ShellId* p = ctx.Shells.Seek(BRepGraph_ShellId(srcId.Index));
      return p != nullptr ? BRepGraph_NodeId(*p) : BRepGraph_NodeId();
    }
    case Kind::Solid:
    {
      const BRepGraph_SolidId* p = ctx.Solids.Seek(BRepGraph_SolidId(srcId.Index));
      return p != nullptr ? BRepGraph_NodeId(*p) : BRepGraph_NodeId();
    }
    case Kind::Compound:
    {
      const BRepGraph_CompoundId* p = ctx.Compounds.Seek(BRepGraph_CompoundId(srcId.Index));
      return p != nullptr ? BRepGraph_NodeId(*p) : BRepGraph_NodeId();
    }
    case Kind::CompSolid:
    {
      const BRepGraph_CompSolidId* p = ctx.CompSolids.Seek(BRepGraph_CompSolidId(srcId.Index));
      return p != nullptr ? BRepGraph_NodeId(*p) : BRepGraph_NodeId();
    }
    case Kind::Product:
    {
      const BRepGraph_ProductId* p = ctx.Products.Seek(BRepGraph_ProductId(srcId.Index));
      return p != nullptr ? BRepGraph_NodeId(*p) : BRepGraph_NodeId();
    }
    case Kind::Occurrence:
    {
      const BRepGraph_OccurrenceId* p =
        ctx.Occurrences.Seek(BRepGraph_OccurrenceId(srcId.Index));
      return p != nullptr ? BRepGraph_NodeId(*p) : BRepGraph_NodeId();
    }
    default:
      return BRepGraph_NodeId();
  }
}

//=================================================================================================

BRepGraph_VertexId ensureVertex(CopyContext& ctx, BRepGraph_VertexId srcId)
{
  if (!srcId.IsValidIn(ctx.Source.Topo().Vertices()))
    return BRepGraph_VertexId();
  const BRepGraph_VertexId* anExisting = ctx.Vertices.Seek(srcId);
  if (anExisting != nullptr)
    return *anExisting;

  const BRepGraphInc::VertexDef& aVtx = ctx.Source.Topo().Vertices().Definition(srcId);
  const BRepGraph_VertexId       aNewId =
    ctx.Result.Editor().Vertices().Add(aVtx.Point, aVtx.Tolerance);
  ctx.Vertices.Bind(srcId, aNewId);
  ctx.Deferred.DeferNode(ctx.Source, srcId, aNewId);
  return aNewId;
}

//=================================================================================================

BRepGraph_EdgeId ensureEdge(CopyContext& ctx, BRepGraph_EdgeId srcId)
{
  if (!srcId.IsValidIn(ctx.Source.Topo().Edges()))
    return BRepGraph_EdgeId();
  const BRepGraph_EdgeId* anExisting = ctx.Edges.Seek(srcId);
  if (anExisting != nullptr)
    return *anExisting;

  const BRepGraphInc::EdgeDef& anEdge = ctx.Source.Topo().Edges().Definition(srcId);
  const BRepGraph_VertexId     aNewStart =
    ensureVertex(ctx, BRepGraph_Tool::Edge::StartVertexId(ctx.Source, srcId));
  const BRepGraph_VertexId aNewEnd =
    ensureVertex(ctx, BRepGraph_Tool::Edge::EndVertexId(ctx.Source, srcId));

  const occ::handle<Geom_Curve>& aSrcCurve = BRepGraph_Tool::Edge::Curve(ctx.Source, srcId);
  occ::handle<Geom_Curve>        aCurve     = copyCurve(aSrcCurve, ctx.CopyGeom);

  const BRepGraph_EdgeId aNewId = ctx.Result.Editor().Edges().Add(
    aNewStart, aNewEnd, aCurve, anEdge.ParamFirst, anEdge.ParamLast, anEdge.Tolerance);
  {
    BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aG = ctx.Result.Editor().Edges().Mut(aNewId);
    aG.Internal().IsDegenerate  = anEdge.IsDegenerate;
    aG.Internal().SameParameter = anEdge.SameParameter;
    aG.Internal().SameRange     = anEdge.SameRange;
    aG.MarkDirty();
  }
  ctx.Edges.Bind(srcId, aNewId);
  ctx.Deferred.DeferNode(ctx.Source, srcId, aNewId);
  return aNewId;
}

//=================================================================================================

BRepGraph_WireId ensureWire(CopyContext& ctx, BRepGraph_WireId srcId)
{
  if (!srcId.IsValidIn(ctx.Source.Topo().Wires()))
    return BRepGraph_WireId();
  const BRepGraph_WireId* anExisting = ctx.Wires.Seek(srcId);
  if (anExisting != nullptr)
    return *anExisting;

  NCollection_DynamicArray<std::pair<BRepGraph_EdgeId, TopAbs_Orientation>> aWireEdges;
  for (BRepGraph_RefsCoEdgeOfWire aCEIt(ctx.Source, srcId); aCEIt.More(); aCEIt.Next())
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = ctx.Source.Topo().CoEdges().Definition(
      ctx.Source.Refs().CoEdges().Entry(aCEIt.CurrentId()).CoEdgeDefId);
    const BRepGraph_EdgeId aNewEdgeId = ensureEdge(ctx, aCoEdge.EdgeDefId);
    aWireEdges.Append(std::make_pair(aNewEdgeId, aCoEdge.Orientation));
  }
  const BRepGraph_WireId aNewId = ctx.Result.Editor().Wires().Add(aWireEdges);
  ctx.Wires.Bind(srcId, aNewId);
  ctx.Deferred.DeferNode(ctx.Source, srcId, aNewId);
  return aNewId;
}

//=================================================================================================

void ensurePCurvesForFace(CopyContext& ctx, BRepGraph_FaceId srcFaceId, BRepGraph_FaceId newFaceId)
{
  for (BRepGraph_RefsWireOfFace aWIt(ctx.Source, srcFaceId); aWIt.More(); aWIt.Next())
  {
    const BRepGraph_WireId aSrcWireId =
      ctx.Source.Refs().Wires().Entry(aWIt.CurrentId()).WireDefId;
    for (BRepGraph_RefsCoEdgeOfWire aCEIt(ctx.Source, aSrcWireId); aCEIt.More(); aCEIt.Next())
    {
      const BRepGraph_CoEdgeId aSrcCoEdgeId =
        ctx.Source.Refs().CoEdges().Entry(aCEIt.CurrentId()).CoEdgeDefId;
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        ctx.Source.Topo().CoEdges().Definition(aSrcCoEdgeId);
      if (!aCoEdge.Curve2DRepId.IsValid())
        continue;
      const BRepGraph_EdgeId* aNewEdge = ctx.Edges.Seek(aCoEdge.EdgeDefId);
      if (aNewEdge == nullptr)
        continue;
      const occ::handle<Geom2d_Curve>& aSrcPC =
        BRepGraph_Tool::CoEdge::PCurve(ctx.Source, aSrcCoEdgeId);
      occ::handle<Geom2d_Curve> aNewPC = copyPCurve(aSrcPC, ctx.CopyGeom);
      ctx.Result.Editor().CoEdges().AddPCurve(
        *aNewEdge, newFaceId, aNewPC, aCoEdge.ParamFirst, aCoEdge.ParamLast, aCoEdge.Orientation);
    }
  }
}

//=================================================================================================

BRepGraph_FaceId ensureFace(CopyContext& ctx, BRepGraph_FaceId srcId)
{
  if (!srcId.IsValidIn(ctx.Source.Topo().Faces()))
    return BRepGraph_FaceId();
  const BRepGraph_FaceId* anExisting = ctx.Faces.Seek(srcId);
  if (anExisting != nullptr)
    return *anExisting;

  const BRepGraphInc::FaceDef& aFace = ctx.Source.Topo().Faces().Definition(srcId);

  BRepGraph_WireId                           anOuterWire;
  NCollection_DynamicArray<BRepGraph_WireId> anInnerWires;
  for (BRepGraph_RefsWireOfFace aWRIt(ctx.Source, srcId); aWRIt.More(); aWRIt.Next())
  {
    const BRepGraphInc::WireRef& aWR     = ctx.Source.Refs().Wires().Entry(aWRIt.CurrentId());
    const BRepGraph_WireId       aNewWire = ensureWire(ctx, aWR.WireDefId);
    if (aWR.IsOuter)
      anOuterWire = aNewWire;
    else
      anInnerWires.Append(aNewWire);
  }

  const occ::handle<Geom_Surface>& aSrcSurf = BRepGraph_Tool::Face::Surface(ctx.Source, srcId);
  occ::handle<Geom_Surface>        aSurf    = copySurface(aSrcSurf, ctx.CopyGeom);

  const BRepGraph_FaceId aNewId =
    ctx.Result.Editor().Faces().Add(aSurf, anOuterWire, anInnerWires, aFace.Tolerance);
  {
    BRepGraph_MutGuard<BRepGraphInc::FaceDef> aG = ctx.Result.Editor().Faces().Mut(aNewId);
    aG.Internal().NaturalRestriction = aFace.NaturalRestriction;
    aG.Internal().TriangulationRepId = aFace.TriangulationRepId;
    aG.MarkDirty();
  }

  const BRepGraph_MeshCache::FaceMeshEntry* aCached = ctx.Source.Mesh().Faces().CachedMesh(srcId);
  if (aCached != nullptr)
    ctx.DstMesh->ChangeFaceMesh(aNewId) = *aCached;

  ctx.Faces.Bind(srcId, aNewId);
  ctx.Deferred.DeferNode(ctx.Source, srcId, aNewId);

  // PCurves require both the face and its edges to exist in the result.
  ensurePCurvesForFace(ctx, srcId, aNewId);
  return aNewId;
}

//=================================================================================================

BRepGraph_ShellId ensureShell(CopyContext& ctx, BRepGraph_ShellId srcId)
{
  if (!srcId.IsValidIn(ctx.Source.Topo().Shells()))
    return BRepGraph_ShellId();
  const BRepGraph_ShellId* anExisting = ctx.Shells.Seek(srcId);
  if (anExisting != nullptr)
    return *anExisting;

  const BRepGraphInc::ShellDef& aShellDef = ctx.Source.Topo().Shells().Definition(srcId);
  const BRepGraph_ShellId       aNewId    = ctx.Result.Editor().Shells().Add();
  {
    BRepGraph_MutGuard<BRepGraphInc::ShellDef> aG = ctx.Result.Editor().Shells().Mut(aNewId);
    aG.Internal().IsClosed = aShellDef.IsClosed;
    aG.MarkDirty();
  }
  ctx.Shells.Bind(srcId, aNewId);
  ctx.Deferred.DeferNode(ctx.Source, srcId, aNewId);

  for (BRepGraph_RefsFaceOfShell aFRIt(ctx.Source, srcId); aFRIt.More(); aFRIt.Next())
  {
    const BRepGraphInc::FaceRef& aFR     = ctx.Source.Refs().Faces().Entry(aFRIt.CurrentId());
    const BRepGraph_FaceId       aNewFace = ensureFace(ctx, aFR.FaceDefId);
    const BRepGraph_FaceRefId    aNewFaceRefId =
      ctx.Result.Editor().Shells().AddFace(aNewId, aNewFace, aFR.Orientation);
    ctx.Deferred.DeferRef(ctx.Source, aFRIt.CurrentId(), aNewFaceRefId);
  }
  return aNewId;
}

//=================================================================================================

BRepGraph_SolidId ensureSolid(CopyContext& ctx, BRepGraph_SolidId srcId)
{
  if (!srcId.IsValidIn(ctx.Source.Topo().Solids()))
    return BRepGraph_SolidId();
  const BRepGraph_SolidId* anExisting = ctx.Solids.Seek(srcId);
  if (anExisting != nullptr)
    return *anExisting;

  const BRepGraph_SolidId aNewId = ctx.Result.Editor().Solids().Add();
  ctx.Solids.Bind(srcId, aNewId);
  ctx.Deferred.DeferNode(ctx.Source, srcId, aNewId);

  for (BRepGraph_RefsShellOfSolid aSRIt(ctx.Source, srcId); aSRIt.More(); aSRIt.Next())
  {
    const BRepGraphInc::ShellRef& aSR      = ctx.Source.Refs().Shells().Entry(aSRIt.CurrentId());
    const BRepGraph_ShellId       aNewShell = ensureShell(ctx, aSR.ShellDefId);
    const BRepGraph_ShellRefId    aNewShellRefId =
      ctx.Result.Editor().Solids().AddShell(aNewId, aNewShell, aSR.Orientation);
    ctx.Deferred.DeferRef(ctx.Source, aSRIt.CurrentId(), aNewShellRefId);
  }
  return aNewId;
}

//=================================================================================================

void ensureNode(CopyContext& ctx, BRepGraph_NodeId srcNodeId)
{
  using Kind = BRepGraph_NodeId::Kind;
  switch (srcNodeId.NodeKind)
  {
    case Kind::Vertex:
      ensureVertex(ctx, BRepGraph_VertexId(srcNodeId.Index));
      break;
    case Kind::Edge:
      ensureEdge(ctx, BRepGraph_EdgeId(srcNodeId.Index));
      break;
    case Kind::Wire:
      ensureWire(ctx, BRepGraph_WireId(srcNodeId.Index));
      break;
    case Kind::Face:
      ensureFace(ctx, BRepGraph_FaceId(srcNodeId.Index));
      break;
    case Kind::Shell:
      ensureShell(ctx, BRepGraph_ShellId(srcNodeId.Index));
      break;
    case Kind::Solid:
      ensureSolid(ctx, BRepGraph_SolidId(srcNodeId.Index));
      break;
    case Kind::Compound:
      ensureCompound(ctx, BRepGraph_CompoundId(srcNodeId.Index));
      break;
    case Kind::CompSolid:
      ensureCompSolid(ctx, BRepGraph_CompSolidId(srcNodeId.Index));
      break;
    case Kind::Product:
      ensureProduct(ctx, BRepGraph_ProductId(srcNodeId.Index));
      break;
    case Kind::Occurrence:
      ensureOccurrence(ctx, BRepGraph_OccurrenceId(srcNodeId.Index));
      break;
    default:
      break;
  }
}

//=================================================================================================

BRepGraph_CompoundId ensureCompound(CopyContext& ctx, BRepGraph_CompoundId srcId)
{
  if (!srcId.IsValidIn(ctx.Source.Topo().Compounds()))
    return BRepGraph_CompoundId();
  const BRepGraph_CompoundId* anExisting = ctx.Compounds.Seek(srcId);
  if (anExisting != nullptr)
    return *anExisting;

  // Bind a placeholder before recursing so cycles in the ref graph do not loop.
  // The actual value is replaced after Add() returns the real ID.
  NCollection_DynamicArray<BRepGraph_NodeId> aNewChildIds;
  for (BRepGraph_RefsChildOfCompound aCRIt(ctx.Source, srcId); aCRIt.More(); aCRIt.Next())
  {
    const BRepGraph_NodeId aSrcChild =
      ctx.Source.Refs().Children().Entry(aCRIt.CurrentId()).ChildDefId;
    ensureNode(ctx, aSrcChild);
    const BRepGraph_NodeId aMapped = mappedNode(ctx, aSrcChild);
    if (aMapped.IsValid())
      aNewChildIds.Append(aMapped);
  }

  const BRepGraph_CompoundId aNewId = ctx.Result.Editor().Compounds().Add(aNewChildIds);
  ctx.Compounds.Bind(srcId, aNewId);
  ctx.Deferred.DeferNode(ctx.Source, srcId, aNewId);
  return aNewId;
}

//=================================================================================================

BRepGraph_CompSolidId ensureCompSolid(CopyContext& ctx, BRepGraph_CompSolidId srcId)
{
  if (!srcId.IsValidIn(ctx.Source.Topo().CompSolids()))
    return BRepGraph_CompSolidId();
  const BRepGraph_CompSolidId* anExisting = ctx.CompSolids.Seek(srcId);
  if (anExisting != nullptr)
    return *anExisting;

  NCollection_DynamicArray<BRepGraph_SolidId> aSolidIds;
  for (BRepGraph_RefsSolidOfCompSolid aSRIt(ctx.Source, srcId); aSRIt.More(); aSRIt.Next())
  {
    const BRepGraph_SolidId aSrcSolid =
      ctx.Source.Refs().Solids().Entry(aSRIt.CurrentId()).SolidDefId;
    aSolidIds.Append(ensureSolid(ctx, aSrcSolid));
  }

  const BRepGraph_CompSolidId aNewId = ctx.Result.Editor().CompSolids().Add(aSolidIds);
  ctx.CompSolids.Bind(srcId, aNewId);
  ctx.Deferred.DeferNode(ctx.Source, srcId, aNewId);
  return aNewId;
}

//=================================================================================================

BRepGraph_OccurrenceRefId ensureOccurrenceRef(CopyContext& ctx, BRepGraph_OccurrenceRefId srcRefId)
{
  const BRepGraph_OccurrenceRefId* anExisting = ctx.OccurrenceRefs.Seek(srcRefId);
  if (anExisting != nullptr)
    return *anExisting;

  const BRepGraphInc::OccurrenceRef& aSrcRef = ctx.SrcStorage->OccurrenceRef(srcRefId);
  const BRepGraph_OccurrenceRefId    aNewId  = ctx.DstStorage->AppendOccurrenceRef();
  ctx.OccurrenceRefs.Bind(srcRefId, aNewId);

  BRepGraphInc::OccurrenceRef& aNewRef = ctx.DstStorage->ChangeOccurrenceRef(aNewId);
  aNewRef.IsRemoved     = aSrcRef.IsRemoved;
  aNewRef.LocalLocation = aSrcRef.LocalLocation;
  if (aSrcRef.OccurrenceDefId.IsValid())
    aNewRef.OccurrenceDefId = ensureOccurrence(ctx, aSrcRef.OccurrenceDefId);
  // ParentId is set by the caller (ensureProduct) after the ref is created.
  return aNewId;
}

//=================================================================================================

BRepGraph_OccurrenceId ensureOccurrence(CopyContext& ctx, BRepGraph_OccurrenceId srcId)
{
  const BRepGraph_OccurrenceId* anExisting = ctx.Occurrences.Seek(srcId);
  if (anExisting != nullptr)
    return *anExisting;

  const BRepGraphInc::OccurrenceDef& aSrcOcc = ctx.SrcStorage->Occurrence(srcId);
  const BRepGraph_OccurrenceId       aNewId  = ctx.DstStorage->AppendOccurrence();
  ctx.Occurrences.Bind(srcId, aNewId);

  BRepGraphInc::OccurrenceDef& aNewOcc = ctx.DstStorage->ChangeOccurrence(aNewId);
  aNewOcc.IsRemoved = aSrcOcc.IsRemoved;
  if (aSrcOcc.ChildDefId.IsValid())
  {
    ensureNode(ctx, aSrcOcc.ChildDefId);
    aNewOcc.ChildDefId = mappedNode(ctx, aSrcOcc.ChildDefId);
  }
  return aNewId;
}

//=================================================================================================

BRepGraph_ProductId ensureProduct(CopyContext& ctx, BRepGraph_ProductId srcId)
{
  const BRepGraph_ProductId* anExisting = ctx.Products.Seek(srcId);
  if (anExisting != nullptr)
    return *anExisting;

  // Bind before iterating refs so re-entrant calls (assembly cycles) short-circuit.
  const BRepGraph_ProductId aNewId = ctx.DstStorage->AppendProduct();
  ctx.Products.Bind(srcId, aNewId);

  const BRepGraphInc::ProductDef& aSrcProd = ctx.SrcStorage->Product(srcId);
  for (const BRepGraph_OccurrenceRefId& aSrcRefId : aSrcProd.OccurrenceRefIds)
  {
    const BRepGraph_OccurrenceRefId aNewRefId = ensureOccurrenceRef(ctx, aSrcRefId);
    ctx.DstStorage->ChangeProduct(aNewId).OccurrenceRefIds.Append(aNewRefId);
    // Set ParentId: the owning product is the parent of this occurrence reference.
    ctx.DstStorage->ChangeOccurrenceRef(aNewRefId).ParentId = BRepGraph_NodeId(aNewId);
  }
  return aNewId;
}

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

BRepGraph BRepGraph_Copy::CopyNode(const BRepGraph&       theGraph,
                                   const BRepGraph_NodeId theNodeId,
                                   const bool             theCopyGeom,
                                   const bool             theReserveCache)
{
  if (!theGraph.IsDone())
    return BRepGraph();

  CopyContext ctx(theGraph, theCopyGeom, theReserveCache);
  ctx.SrcStorage = &theGraph.incStorage();
  ctx.DstStorage = &ctx.Result.incStorage();
  ctx.DstMesh    = &ctx.Result.meshCache();
  ctx.DstData    = ctx.Result.data();

  ensureNode(ctx, theNodeId);

  // Post-pass: remap OccurrenceRef.ParentId for any assembly entities that were copied.
  // ensureProduct sets ParentId inline, but this guard handles other edge cases.
  if (!ctx.OccurrenceRefs.IsEmpty())
  {
    for (BRepGraph_FullOccurrenceRefIterator aRefIt(theGraph); aRefIt.More(); aRefIt.Next())
    {
      const BRepGraph_OccurrenceRefId  aSrcRefId  = aRefIt.CurrentId();
      const BRepGraph_OccurrenceRefId* aDstRefPtr = ctx.OccurrenceRefs.Seek(aSrcRefId);
      if (aDstRefPtr == nullptr)
        continue;
      const BRepGraphInc::OccurrenceRef& aSrcRef = ctx.SrcStorage->OccurrenceRef(aSrcRefId);
      if (!aSrcRef.ParentId.IsValid())
        continue;
      const BRepGraph_NodeId aMapped = mappedNode(ctx, aSrcRef.ParentId);
      if (aMapped.IsValid())
        ctx.DstStorage->ChangeOccurrenceRef(*aDstRefPtr).ParentId = aMapped;
    }
  }

  // Build root product list when product entities were copied.
  if (!ctx.Products.IsEmpty())
  {
    NCollection_Map<BRepGraph_ProductId> aReferencedProducts;
    for (BRepGraph_FullOccurrenceIterator anOccIt(ctx.Result); anOccIt.More(); anOccIt.Next())
    {
      const BRepGraph_OccurrenceId       anOccId = anOccIt.CurrentId();
      const BRepGraphInc::OccurrenceDef& anOcc   = ctx.DstStorage->Occurrence(anOccId);
      if (!anOcc.IsRemoved && anOcc.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Product)
      {
        const BRepGraph_ProductId aChildProdId = BRepGraph_ProductId::FromNodeId(anOcc.ChildDefId);
        if (aChildProdId.IsValidIn(ctx.Result.Topo().Products()))
          aReferencedProducts.Add(aChildProdId);
      }
    }
    for (BRepGraph_FullProductIterator aProdIt(ctx.Result); aProdIt.More(); aProdIt.Next())
    {
      const BRepGraph_ProductId       aProdId = aProdIt.CurrentId();
      const BRepGraphInc::ProductDef& aProd   = ctx.DstStorage->Product(aProdId);
      if (!aProd.IsRemoved && !aReferencedProducts.Contains(aProdId))
        ctx.DstData->myRootProductIds.Append(aProdId);
    }
  }

  ctx.DstData->myIsDone = true;
  if (ctx.ReserveCache)
    reserveTransientCache(ctx.Result);
  ctx.Deferred.Drain(theGraph, ctx.Result);
  return std::move(ctx.Result);
}
