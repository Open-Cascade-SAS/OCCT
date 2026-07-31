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
#include <BRepGraph_CacheRegistry.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraphInc_Load.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_FlatDataMap.hxx>
#include <NCollection_FlatMap.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_LinearVector.hxx>
#include <NCollection_Map.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Standard_Assert.hxx>

#include <algorithm>
#include <utility>

using GeomPolicy = BRepGraph_Copy::GeomPolicy;
using MeshPolicy = BRepGraph_Copy::MeshPolicy;

namespace
{

//! Copy geometry handle according to the policy.
//! Copy: deep-clone via Geom_Geometry::Copy().
//! Share: return the same handle.
//! Drop: return null handle.
occ::handle<Geom_Surface> copySurface(const occ::handle<Geom_Surface>& theSurf,
                                      GeomPolicy                       thePolicy)
{
  if (theSurf.IsNull())
  {
    return theSurf;
  }
  switch (thePolicy)
  {
    case GeomPolicy::Copy:
      return occ::down_cast<Geom_Surface>(theSurf->Copy());
    case GeomPolicy::Share:
      return theSurf;
    case GeomPolicy::Drop:
      return occ::handle<Geom_Surface>();
  }
  return theSurf; // unreachable
}

occ::handle<Geom_Curve> copyCurve(const occ::handle<Geom_Curve>& theCrv, GeomPolicy thePolicy)
{
  if (theCrv.IsNull())
  {
    return theCrv;
  }
  switch (thePolicy)
  {
    case GeomPolicy::Copy:
      return occ::down_cast<Geom_Curve>(theCrv->Copy());
    case GeomPolicy::Share:
      return theCrv;
    case GeomPolicy::Drop:
      return occ::handle<Geom_Curve>();
  }
  return theCrv; // unreachable
}

occ::handle<Geom2d_Curve> copyPCurve(const occ::handle<Geom2d_Curve>& theCrv, GeomPolicy thePolicy)
{
  if (theCrv.IsNull())
  {
    return theCrv;
  }
  switch (thePolicy)
  {
    case GeomPolicy::Copy:
      return occ::down_cast<Geom2d_Curve>(theCrv->Copy());
    case GeomPolicy::Share:
      return theCrv;
    case GeomPolicy::Drop:
      return occ::handle<Geom2d_Curve>();
  }
  return theCrv; // unreachable
}

template <typename HandleT>
HandleT copyMeshHandle(const HandleT& theHandle, MeshPolicy thePolicy)
{
  if (theHandle.IsNull())
  {
    return theHandle;
  }
  switch (thePolicy)
  {
    case MeshPolicy::Copy:
      return theHandle->Copy();
    case MeshPolicy::Share:
      return theHandle;
    case MeshPolicy::Drop:
      return HandleT();
  }
  return theHandle; // unreachable
}

//=================================================================================================

//! Memoised copy context shared across all ensure* free functions.
//!
//! Source/result pointers that require friend access (incStorage, cacheRegistry, data)
//! are pre-extracted inside the friend methods Perform / CopyNode before any
//! ensure* function is called.
struct GraphCopyContext
{
  const BRepGraph&            Source;
  BRepGraph&                  Result;
  bool                        IsSelfCopy;
  GeomPolicy                  GeomPol;
  MeshPolicy                  MeshPol;
  const BRepGraphInc_Storage* SrcStorage = nullptr;
  BRepGraphInc_Storage*       DstStorage = nullptr;
  BRepGraph_Data*             DstData    = nullptr;

  NCollection_DataMap<BRepGraph_VertexId, BRepGraph_VertexId>               Vertices;
  NCollection_DataMap<BRepGraph_EdgeId, BRepGraph_EdgeId>                   Edges;
  NCollection_DataMap<BRepGraph_CoEdgeId, BRepGraph_CoEdgeId>               CoEdges;
  NCollection_DataMap<BRepGraph_WireId, BRepGraph_WireId>                   Wires;
  NCollection_DataMap<BRepGraph_FaceId, BRepGraph_FaceId>                   Faces;
  NCollection_DataMap<BRepGraph_ShellId, BRepGraph_ShellId>                 Shells;
  NCollection_DataMap<BRepGraph_SolidId, BRepGraph_SolidId>                 Solids;
  NCollection_DataMap<BRepGraph_CompoundId, BRepGraph_CompoundId>           Compounds;
  NCollection_DataMap<BRepGraph_CompSolidId, BRepGraph_CompSolidId>         CompSolids;
  NCollection_DataMap<BRepGraph_ProductId, BRepGraph_ProductId>             Products;
  NCollection_DataMap<BRepGraph_OccurrenceId, BRepGraph_OccurrenceId>       Occurrences;
  NCollection_DataMap<BRepGraph_OccurrenceRefId, BRepGraph_OccurrenceRefId> OccurrenceRefs;
  NCollection_FlatDataMap<BRepGraph_ItemId, BRepGraph_ItemId>               ItemRemap;

  explicit GraphCopyContext(const BRepGraph& theSrc,
                            BRepGraph&       theDst,
                            bool             theIsSelfCopy,
                            GeomPolicy       theGeomPolicy,
                            MeshPolicy       theMeshPolicy)
      : Source(theSrc),
        Result(theDst),
        IsSelfCopy(theIsSelfCopy),
        GeomPol(theGeomPolicy),
        MeshPol(theMeshPolicy)
  {
  }
};

// Forward declarations - needed for mutual recursion between ensure* functions.
BRepGraph_VertexId     ensureVertex(GraphCopyContext& theCtx, BRepGraph_VertexId theSrcId);
BRepGraph_EdgeId       ensureEdge(GraphCopyContext& theCtx, BRepGraph_EdgeId theSrcId);
BRepGraph_WireId       ensureWire(GraphCopyContext& theCtx, BRepGraph_WireId theSrcId);
BRepGraph_FaceId       ensureFace(GraphCopyContext& theCtx, BRepGraph_FaceId theSrcId);
BRepGraph_ShellId      ensureShell(GraphCopyContext& theCtx, BRepGraph_ShellId theSrcId);
BRepGraph_SolidId      ensureSolid(GraphCopyContext& theCtx, BRepGraph_SolidId theSrcId);
BRepGraph_CompoundId   ensureCompound(GraphCopyContext& theCtx, BRepGraph_CompoundId theSrcId);
BRepGraph_CompSolidId  ensureCompSolid(GraphCopyContext& theCtx, BRepGraph_CompSolidId theSrcId);
BRepGraph_ProductId    ensureProduct(GraphCopyContext& theCtx, BRepGraph_ProductId theSrcId);
BRepGraph_OccurrenceId ensureOccurrence(GraphCopyContext& theCtx, BRepGraph_OccurrenceId theSrcId);
BRepGraph_OccurrenceRefId ensureOccurrenceRef(GraphCopyContext&         theCtx,
                                              BRepGraph_OccurrenceRefId theSrcRefId,
                                              BRepGraph_ProductId       theDstParentProductId);
void                      ensureNode(GraphCopyContext& theCtx, BRepGraph_NodeId theSrcNodeId);
BRepGraph_NodeId          mappedNode(const GraphCopyContext& theCtx, BRepGraph_NodeId theSrcId);

//=================================================================================================

template <typename SourceIdT, typename TargetIdT>
void bindItemRemap(NCollection_FlatDataMap<BRepGraph_ItemId, BRepGraph_ItemId>& theItemRemap,
                   const SourceIdT                                              theSource,
                   const TargetIdT                                              theTarget)
{
  if (theSource.IsValid() && theTarget.IsValid())
  {
    const BRepGraph_ItemId aSourceItem(theSource);
    const BRepGraph_ItemId aTargetItem(theTarget);
    if (theItemRemap.IsBound(aSourceItem))
    {
      theItemRemap.ChangeFind(aSourceItem) = aTargetItem;
    }
    else
    {
      theItemRemap.Bind(aSourceItem, aTargetItem);
    }
  }
}

//=================================================================================================

template <typename SourceIdT, typename TargetIdT>
void bindItemRemap(GraphCopyContext& theCtx, const SourceIdT theSource, const TargetIdT theTarget)
{
  bindItemRemap(theCtx.ItemRemap, theSource, theTarget);
}

//=================================================================================================

template <typename SourceIdT, typename TargetIdT>
void setRemovedLike(const BRepGraphInc_Storage& theSourceStorage,
                    BRepGraphInc_Storage&       theTargetStorage,
                    const SourceIdT             theSource,
                    const TargetIdT             theTarget)
{
  if (theSource.IsValid() && theTarget.IsValid())
  {
    theTargetStorage.SetRemoved(theTarget, theSourceStorage.IsRemoved(theSource));
  }
}

//=================================================================================================

template <typename SourceIdT, typename TargetIdT>
void bindTypedMap(NCollection_DataMap<SourceIdT, TargetIdT>& theTypedMap,
                  GraphCopyContext&                          theCtx,
                  const SourceIdT                            theSource,
                  const TargetIdT                            theTarget)
{
  theTypedMap.Bind(theSource, theTarget);
  bindItemRemap(theCtx, theSource, theTarget);
}

//=================================================================================================

template <BRepGraph_NodeId::Kind TheKind>
BRepGraph_NodeId::Typed<TheKind> mappedItem(
  const NCollection_FlatDataMap<BRepGraph_ItemId, BRepGraph_ItemId>& theItemRemap,
  const BRepGraph_NodeId::Typed<TheKind>                             theSource)
{
  if (!theSource.IsValid())
  {
    return BRepGraph_NodeId::Typed<TheKind>();
  }
  const BRepGraph_ItemId* aTarget = theItemRemap.Seek(BRepGraph_ItemId(theSource));
  if (aTarget == nullptr || !aTarget->IsNode())
  {
    return BRepGraph_NodeId::Typed<TheKind>();
  }
  return BRepGraph_NodeId::Typed<TheKind>::FromNodeId(aTarget->NodeId());
}

template <BRepGraph_RefId::Kind TheKind>
BRepGraph_RefId::Typed<TheKind> mappedItem(
  const NCollection_FlatDataMap<BRepGraph_ItemId, BRepGraph_ItemId>& theItemRemap,
  const BRepGraph_RefId::Typed<TheKind>                              theSource)
{
  if (!theSource.IsValid())
  {
    return BRepGraph_RefId::Typed<TheKind>();
  }
  const BRepGraph_ItemId* aTarget = theItemRemap.Seek(BRepGraph_ItemId(theSource));
  if (aTarget == nullptr || !aTarget->IsReference())
  {
    return BRepGraph_RefId::Typed<TheKind>();
  }
  return BRepGraph_RefId::Typed<TheKind>::FromRefId(aTarget->RefId());
}

//=================================================================================================

size_t itemCapacityUpperBound(const BRepGraphInc_Storage& theStorage)
{
  return static_cast<size_t>(theStorage.NbVertices()) + theStorage.NbEdges()
         + theStorage.NbCoEdges() + theStorage.NbWires() + theStorage.NbFaces()
         + theStorage.NbShells() + theStorage.NbSolids() + theStorage.NbCompounds()
         + theStorage.NbCompSolids() + theStorage.NbProducts() + theStorage.NbOccurrences()
         + theStorage.NbShellRefs() + theStorage.NbFaceRefs() + theStorage.NbWireRefs()
         + theStorage.NbVertexRefs() + theStorage.NbSolidRefs() + theStorage.NbChildRefs()
         + theStorage.NbOccurrenceRefs() + theStorage.NbFaceSurfaces() + theStorage.NbEdgeCurves3D()
         + theStorage.NbCoEdgeCurves2D() + theStorage.NbFaceTriangulations()
         + theStorage.NbEdgePolygons3D() + theStorage.NbCoEdgePolygons2D()
         + theStorage.NbCoEdgePolygonsOnTri();
}

//=================================================================================================

void ensureTriangulationRep(GraphCopyContext&                      theCtx,
                            const BRepGraph_FaceTriangulationRepId theSrcRepId,
                            const BRepGraph_FaceId                 theDstFaceId)
{
  if (theCtx.MeshPol == MeshPolicy::Drop
      || !theSrcRepId.IsValid(theCtx.SrcStorage->NbFaceTriangulations())
      || theCtx.SrcStorage->IsRemoved(theSrcRepId))
  {
    return;
  }

  theCtx.Result.Editor().Faces().SetPersistentTriangulation(
    theDstFaceId,
    copyMeshHandle(theCtx.SrcStorage->FaceTriangulationRep(theSrcRepId).Triangulation,
                   theCtx.MeshPol));
}

//=================================================================================================

void ensurePolygon3DRep(GraphCopyContext&                  theCtx,
                        const BRepGraph_EdgePolygon3DRepId theSrcRepId,
                        const BRepGraph_EdgeId             theDstEdgeId)
{
  if (theCtx.MeshPol == MeshPolicy::Drop
      || !theSrcRepId.IsValid(theCtx.SrcStorage->NbEdgePolygons3D())
      || theCtx.SrcStorage->IsRemoved(theSrcRepId))
  {
    return;
  }

  theCtx.Result.Editor().Edges().SetPersistentPolygon3D(
    theDstEdgeId,
    copyMeshHandle(theCtx.SrcStorage->EdgePolygon3DRep(theSrcRepId).Polygon, theCtx.MeshPol));
}

//=================================================================================================

void ensurePolygon2DRep(GraphCopyContext&                    theCtx,
                        const BRepGraph_CoEdgePolygon2DRepId theSrcRepId,
                        const BRepGraph_CoEdgeId             theDstCoEdgeId)
{
  if (theCtx.MeshPol == MeshPolicy::Drop
      || !theSrcRepId.IsValid(theCtx.SrcStorage->NbCoEdgePolygons2D())
      || theCtx.SrcStorage->IsRemoved(theSrcRepId))
  {
    return;
  }

  theCtx.Result.Editor().CoEdges().SetPersistentPolygon2D(
    theDstCoEdgeId,
    copyMeshHandle(theCtx.SrcStorage->CoEdgePolygon2DRep(theSrcRepId).Polygon, theCtx.MeshPol));
}

//=================================================================================================

void ensurePolygonOnTriRep(GraphCopyContext&                       theCtx,
                           const BRepGraph_CoEdgePolygonOnTriRepId theSrcRepId,
                           const BRepGraph_CoEdgeId                theDstCoEdgeId)
{
  if (theCtx.MeshPol == MeshPolicy::Drop
      || !theSrcRepId.IsValid(theCtx.SrcStorage->NbCoEdgePolygonsOnTri())
      || theCtx.SrcStorage->IsRemoved(theSrcRepId))
  {
    return;
  }

  theCtx.Result.Editor().CoEdges().SetPersistentPolygonOnTri(
    theDstCoEdgeId,
    copyMeshHandle(theCtx.SrcStorage->CoEdgePolygonOnTriRep(theSrcRepId).Polygon, theCtx.MeshPol));
}

//=================================================================================================

BRepGraph_NodeId mappedNode(const GraphCopyContext& theCtx, BRepGraph_NodeId theSrcId)
{
  using Kind = BRepGraph_NodeId::Kind;
  switch (theSrcId.NodeKind)
  {
    case Kind::Vertex: {
      const BRepGraph_VertexId* p = theCtx.Vertices.Seek(BRepGraph_VertexId(theSrcId.Index));
      return p != nullptr ? BRepGraph_NodeId(*p) : BRepGraph_NodeId();
    }
    case Kind::Edge: {
      const BRepGraph_EdgeId* p = theCtx.Edges.Seek(BRepGraph_EdgeId(theSrcId.Index));
      return p != nullptr ? BRepGraph_NodeId(*p) : BRepGraph_NodeId();
    }
    case Kind::CoEdge: {
      const BRepGraph_CoEdgeId* p = theCtx.CoEdges.Seek(BRepGraph_CoEdgeId(theSrcId.Index));
      return p != nullptr ? BRepGraph_NodeId(*p) : BRepGraph_NodeId();
    }
    case Kind::Wire: {
      const BRepGraph_WireId* p = theCtx.Wires.Seek(BRepGraph_WireId(theSrcId.Index));
      return p != nullptr ? BRepGraph_NodeId(*p) : BRepGraph_NodeId();
    }
    case Kind::Face: {
      const BRepGraph_FaceId* p = theCtx.Faces.Seek(BRepGraph_FaceId(theSrcId.Index));
      return p != nullptr ? BRepGraph_NodeId(*p) : BRepGraph_NodeId();
    }
    case Kind::Shell: {
      const BRepGraph_ShellId* p = theCtx.Shells.Seek(BRepGraph_ShellId(theSrcId.Index));
      return p != nullptr ? BRepGraph_NodeId(*p) : BRepGraph_NodeId();
    }
    case Kind::Solid: {
      const BRepGraph_SolidId* p = theCtx.Solids.Seek(BRepGraph_SolidId(theSrcId.Index));
      return p != nullptr ? BRepGraph_NodeId(*p) : BRepGraph_NodeId();
    }
    case Kind::Compound: {
      const BRepGraph_CompoundId* p = theCtx.Compounds.Seek(BRepGraph_CompoundId(theSrcId.Index));
      return p != nullptr ? BRepGraph_NodeId(*p) : BRepGraph_NodeId();
    }
    case Kind::CompSolid: {
      const BRepGraph_CompSolidId* p =
        theCtx.CompSolids.Seek(BRepGraph_CompSolidId(theSrcId.Index));
      return p != nullptr ? BRepGraph_NodeId(*p) : BRepGraph_NodeId();
    }
    case Kind::Product: {
      const BRepGraph_ProductId* p = theCtx.Products.Seek(BRepGraph_ProductId(theSrcId.Index));
      return p != nullptr ? BRepGraph_NodeId(*p) : BRepGraph_NodeId();
    }
    case Kind::Occurrence: {
      const BRepGraph_OccurrenceId* p =
        theCtx.Occurrences.Seek(BRepGraph_OccurrenceId(theSrcId.Index));
      return p != nullptr ? BRepGraph_NodeId(*p) : BRepGraph_NodeId();
    }
    default:
      return BRepGraph_NodeId();
  }
}

//=================================================================================================

BRepGraph_VertexId ensureVertex(GraphCopyContext& theCtx, BRepGraph_VertexId theSrcId)
{
  if (!theSrcId.IsValidIn(theCtx.Source.Topo().Vertices()))
  {
    return BRepGraph_VertexId();
  }
  // For self-copy, the mapping starts empty so Seek naturally returns null for unprocessed
  // vertices. For external copy, Seek returns non-null for vertices already in the target
  // graph (identity-mapped or previously processed). In both cases, returning the existing
  // mapping is the "already processed" fast path.
  const BRepGraph_VertexId* anExisting = theCtx.Vertices.Seek(theSrcId);
  if (anExisting != nullptr)
  {
    return *anExisting;
  }

  const BRepGraphInc::VertexDef& aVtx = theCtx.Source.Topo().Vertices().Definition(theSrcId);
  const BRepGraph_VertexId       aNewId =
    theCtx.Result.Editor().Vertices().Add(aVtx.Point, aVtx.Tolerance);
  theCtx.DstStorage->SetRemoved(aNewId, theCtx.SrcStorage->IsRemoved(theSrcId));
  bindTypedMap(theCtx.Vertices, theCtx, theSrcId, aNewId);
  return aNewId;
}

//=================================================================================================

BRepGraph_EdgeId ensureEdge(GraphCopyContext& theCtx, BRepGraph_EdgeId theSrcId)
{
  if (!theSrcId.IsValidIn(theCtx.Source.Topo().Edges()))
  {
    return BRepGraph_EdgeId();
  }
  const BRepGraph_EdgeId* anExisting = theCtx.Edges.Seek(theSrcId);
  if (anExisting != nullptr)
  {
    return *anExisting;
  }

  const BRepGraphInc::EdgeDef& anEdge = theCtx.Source.Topo().Edges().Definition(theSrcId);
  const BRepGraph_VertexRefId  aStartRefId =
    BRepGraph_Tool::Edge::StartVertexId(theCtx.Source, theSrcId);
  const BRepGraph_VertexRefId anEndRefId =
    BRepGraph_Tool::Edge::EndVertexId(theCtx.Source, theSrcId);
  const BRepGraph_VertexId aNewStart = ensureVertex(
    theCtx,
    aStartRefId.IsValid() ? theCtx.Source.Refs().Vertices().Entry(aStartRefId).ChildVertexId
                          : BRepGraph_VertexId());
  const BRepGraph_VertexId aNewEnd = ensureVertex(
    theCtx,
    anEndRefId.IsValid() ? theCtx.Source.Refs().Vertices().Entry(anEndRefId).ChildVertexId
                         : BRepGraph_VertexId());

  const occ::handle<Geom_Curve>& aSrcCurve = BRepGraph_Tool::Edge::Curve(theCtx.Source, theSrcId);
  occ::handle<Geom_Curve>        aCurve    = copyCurve(aSrcCurve, theCtx.GeomPol);

  const auto [aEdgeParamFirst, aEdgeParamLast] =
    BRepGraph_Tool::Edge::Range(theCtx.Source, theSrcId);

  const BRepGraph_EdgeId aNewId = theCtx.Result.Editor().Edges().Add(aNewStart,
                                                                     aNewEnd,
                                                                     aCurve,
                                                                     aEdgeParamFirst,
                                                                     aEdgeParamLast,
                                                                     anEdge.Tolerance);
  theCtx.DstStorage->SetRemoved(aNewId, theCtx.SrcStorage->IsRemoved(theSrcId));
  if (theCtx.MeshPol != MeshPolicy::Drop && anEdge.Polygon3DRepId.IsValid())
  {
    ensurePolygon3DRep(theCtx, anEdge.Polygon3DRepId, aNewId);
  }
  bindTypedMap(theCtx.Edges, theCtx, theSrcId, aNewId);
  const BRepGraphInc::EdgeDef& aNewEdge = theCtx.Result.Topo().Edges().Definition(aNewId);
  setRemovedLike(*theCtx.SrcStorage,
                 *theCtx.DstStorage,
                 anEdge.StartVertexRefId,
                 aNewEdge.StartVertexRefId);
  setRemovedLike(*theCtx.SrcStorage,
                 *theCtx.DstStorage,
                 anEdge.EndVertexRefId,
                 aNewEdge.EndVertexRefId);
  bindItemRemap(theCtx, anEdge.StartVertexRefId, aNewEdge.StartVertexRefId);
  bindItemRemap(theCtx, anEdge.EndVertexRefId, aNewEdge.EndVertexRefId);
  return aNewId;
}

//=================================================================================================

BRepGraph_WireId ensureWire(GraphCopyContext& theCtx, BRepGraph_WireId theSrcId)
{
  if (!theSrcId.IsValidIn(theCtx.Source.Topo().Wires()))
  {
    return BRepGraph_WireId();
  }
  const BRepGraph_WireId* anExisting = theCtx.Wires.Seek(theSrcId);
  if (anExisting != nullptr)
  {
    return *anExisting;
  }

  NCollection_LinearVector<BRepGraph_CoEdgeId> aNewCoEdgeIds;
  NCollection_LinearVector<BRepGraph_CoEdgeId> aSrcCoEdgeIds;
  for (BRepGraph_CoEdgesOfWire aCEIt(theCtx.Source, theSrcId); aCEIt.More(); aCEIt.Next())
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge =
      theCtx.Source.Topo().CoEdges().Definition(aCEIt.CurrentId());
    const BRepGraph_EdgeId   aNewEdgeId = ensureEdge(theCtx, aCoEdge.ChildEdgeId);
    const BRepGraph_CoEdgeId aNewCoEdgeId =
      theCtx.Result.Editor().CoEdges().Add(aNewEdgeId, aCoEdge.Orientation);
    theCtx.DstStorage->SetRemoved(aNewCoEdgeId, theCtx.SrcStorage->IsRemoved(aCEIt.CurrentId()));
    aNewCoEdgeIds.Append(aNewCoEdgeId);
    aSrcCoEdgeIds.Append(aCEIt.CurrentId());
  }
  const BRepGraph_WireId aNewId = theCtx.Result.Editor().Wires().Add(aNewCoEdgeIds.ToArray1());
  theCtx.DstStorage->SetRemoved(aNewId, theCtx.SrcStorage->IsRemoved(theSrcId));
  bindTypedMap(theCtx.Wires, theCtx, theSrcId, aNewId);
  for (size_t anIdx = 0; anIdx < aSrcCoEdgeIds.Size(); ++anIdx)
  {
    bindTypedMap(theCtx.CoEdges, theCtx, aSrcCoEdgeIds.Value(anIdx), aNewCoEdgeIds.Value(anIdx));
  }
  return aNewId;
}

//=================================================================================================

void ensurePCurvesForFace(GraphCopyContext& theCtx,
                          BRepGraph_FaceId  theSrcFaceId,
                          BRepGraph_FaceId  theNewFaceId)
{
  for (BRepGraph_RefsWireOfFace aWIt(theCtx.Source, theSrcFaceId); aWIt.More(); aWIt.Next())
  {
    const BRepGraph_WireId aSrcWireId =
      theCtx.Source.Refs().Wires().Entry(aWIt.CurrentId()).ChildWireId;
    for (BRepGraph_CoEdgesOfWire aCEIt(theCtx.Source, aSrcWireId); aCEIt.More(); aCEIt.Next())
    {
      const BRepGraph_CoEdgeId       aSrcCoEdgeId = aCEIt.CurrentId();
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        theCtx.Source.Topo().CoEdges().Definition(aSrcCoEdgeId);
      if (aCoEdge.FaceId != theSrcFaceId)
      {
        continue;
      }
      const BRepGraph_CoEdgeId* aNewCoEdge = theCtx.CoEdges.Seek(aSrcCoEdgeId);
      if (aNewCoEdge == nullptr)
      {
        continue;
      }

      if (aCoEdge.Curve2DRepId.IsValid()
          && aCoEdge.Curve2DRepId.IsValid(theCtx.SrcStorage->NbCoEdgeCurves2D())
          && !theCtx.SrcStorage->IsRemoved(aCoEdge.Curve2DRepId))
      {
        const BRepGraphInc::CoEdgeCurve2DRep& aSrcCurve2D =
          theCtx.SrcStorage->CoEdgeCurve2DRep(aCoEdge.Curve2DRepId);
        theCtx.Result.Editor().CoEdges().SetPCurve(*aNewCoEdge,
                                                   copyPCurve(aSrcCurve2D.Curve, theCtx.GeomPol),
                                                   aSrcCurve2D.ParamFirst,
                                                   aSrcCurve2D.ParamLast);
      }

      // Set FaceId first so SetPersistentPolygonOnTri can resolve the triangulation
      theCtx.Result.Editor().CoEdges().SetFaceId(*aNewCoEdge, theNewFaceId);

      if (theCtx.MeshPol != MeshPolicy::Drop && aCoEdge.Polygon2DRepId.IsValid())
      {
        ensurePolygon2DRep(theCtx, aCoEdge.Polygon2DRepId, *aNewCoEdge);
      }

      if (theCtx.MeshPol != MeshPolicy::Drop && aCoEdge.PolygonOnTriRepId.IsValid())
      {
        ensurePolygonOnTriRep(theCtx, aCoEdge.PolygonOnTriRepId, *aNewCoEdge);
      }
    }
  }
}

//=================================================================================================

BRepGraph_FaceId ensureFace(GraphCopyContext& theCtx, BRepGraph_FaceId theSrcId)
{
  if (!theSrcId.IsValidIn(theCtx.Source.Topo().Faces()))
  {
    return BRepGraph_FaceId();
  }
  const BRepGraph_FaceId* anExisting = theCtx.Faces.Seek(theSrcId);
  if (anExisting != nullptr)
  {
    return *anExisting;
  }

  const BRepGraphInc::FaceDef& aFace = theCtx.Source.Topo().Faces().Definition(theSrcId);

  BRepGraph_WireId                           aFirstWire;
  NCollection_LinearVector<BRepGraph_WireId> aNextWires;
  for (BRepGraph_RefsWireOfFace aWRIt(theCtx.Source, theSrcId); aWRIt.More(); aWRIt.Next())
  {
    const BRepGraphInc::WireRef& aWR      = theCtx.Source.Refs().Wires().Entry(aWRIt.CurrentId());
    const BRepGraph_WireId       aNewWire = ensureWire(theCtx, aWR.ChildWireId);
    if (!aFirstWire.IsValid())
    {
      aFirstWire = aNewWire;
    }
    else
    {
      aNextWires.Append(aNewWire);
    }
  }

  const occ::handle<Geom_Surface>& aSrcSurf =
    BRepGraph_Tool::Face::Surface(theCtx.Source, theSrcId);
  occ::handle<Geom_Surface> aSurf = copySurface(aSrcSurf, theCtx.GeomPol);

  const BRepGraph_FaceId aNewId =
    theCtx.Result.Editor().Faces().Add(aSurf, aFirstWire, aNextWires.ToArray1(), aFace.Tolerance);
  theCtx.DstStorage->SetRemoved(aNewId, theCtx.SrcStorage->IsRemoved(theSrcId));
  if (theCtx.MeshPol != MeshPolicy::Drop && aFace.TriangulationRepId.IsValid())
  {
    ensureTriangulationRep(theCtx, aFace.TriangulationRepId, aNewId);
  }
  bindTypedMap(theCtx.Faces, theCtx, theSrcId, aNewId);
  const BRepGraphInc::FaceRelations& aSrcRel = theCtx.Source.Topo().Faces().Relations(theSrcId);
  const BRepGraphInc::FaceRelations& aDstRel = theCtx.Result.Topo().Faces().Relations(aNewId);
  const size_t aNbWireRefs = std::min(aSrcRel.WireRefIds.Size(), aDstRel.WireRefIds.Size());
  for (size_t anIdx = 0; anIdx < aNbWireRefs; ++anIdx)
  {
    theCtx.DstStorage->SetRemoved(aDstRel.WireRefIds.Value(anIdx),
                                  theCtx.SrcStorage->IsRemoved(aSrcRel.WireRefIds.Value(anIdx)));
    bindItemRemap(theCtx, aSrcRel.WireRefIds.Value(anIdx), aDstRel.WireRefIds.Value(anIdx));
  }

  // PCurves require both the face and its edges to exist in the result.
  ensurePCurvesForFace(theCtx, theSrcId, aNewId);
  return aNewId;
}

//=================================================================================================

BRepGraph_ShellId ensureShell(GraphCopyContext& theCtx, BRepGraph_ShellId theSrcId)
{
  if (!theSrcId.IsValidIn(theCtx.Source.Topo().Shells()))
  {
    return BRepGraph_ShellId();
  }
  const BRepGraph_ShellId* anExisting = theCtx.Shells.Seek(theSrcId);
  if (anExisting != nullptr)
  {
    return *anExisting;
  }

  const BRepGraph_ShellId aNewId = theCtx.Result.Editor().Shells().Add();
  theCtx.DstStorage->SetRemoved(aNewId, theCtx.SrcStorage->IsRemoved(theSrcId));
  bindTypedMap(theCtx.Shells, theCtx, theSrcId, aNewId);

  for (BRepGraph_RefsFaceOfShell aFRIt(theCtx.Source, theSrcId); aFRIt.More(); aFRIt.Next())
  {
    const BRepGraphInc::FaceRef& aFR      = theCtx.Source.Refs().Faces().Entry(aFRIt.CurrentId());
    const BRepGraph_FaceId       aNewFace = ensureFace(theCtx, aFR.ChildFaceId);
    const BRepGraph_FaceRefId    aNewRef =
      theCtx.Result.Editor().Shells().Append(aNewId, aNewFace, aFR.Orientation);
    theCtx.DstStorage->SetRemoved(aNewRef, theCtx.SrcStorage->IsRemoved(aFRIt.CurrentId()));
    bindItemRemap(theCtx, aFRIt.CurrentId(), aNewRef);
  }
  return aNewId;
}

//=================================================================================================

BRepGraph_SolidId ensureSolid(GraphCopyContext& theCtx, BRepGraph_SolidId theSrcId)
{
  if (!theSrcId.IsValidIn(theCtx.Source.Topo().Solids()))
  {
    return BRepGraph_SolidId();
  }
  const BRepGraph_SolidId* anExisting = theCtx.Solids.Seek(theSrcId);
  if (anExisting != nullptr)
  {
    return *anExisting;
  }

  const BRepGraph_SolidId aNewId = theCtx.Result.Editor().Solids().Add();
  theCtx.DstStorage->SetRemoved(aNewId, theCtx.SrcStorage->IsRemoved(theSrcId));
  bindTypedMap(theCtx.Solids, theCtx, theSrcId, aNewId);

  for (BRepGraph_RefsShellOfSolid aSRIt(theCtx.Source, theSrcId); aSRIt.More(); aSRIt.Next())
  {
    const BRepGraphInc::ShellRef& aSR = theCtx.Source.Refs().Shells().Entry(aSRIt.CurrentId());
    const BRepGraph_ShellId       aNewShell = ensureShell(theCtx, aSR.ChildShellId);
    const BRepGraph_ShellRefId    aNewRef =
      theCtx.Result.Editor().Solids().Append(aNewId, aNewShell, aSR.Orientation);
    theCtx.DstStorage->SetRemoved(aNewRef, theCtx.SrcStorage->IsRemoved(aSRIt.CurrentId()));
    bindItemRemap(theCtx, aSRIt.CurrentId(), aNewRef);
  }
  return aNewId;
}

//=================================================================================================

void ensureNode(GraphCopyContext& theCtx, BRepGraph_NodeId theSrcNodeId)
{
  using Kind = BRepGraph_NodeId::Kind;
  switch (theSrcNodeId.NodeKind)
  {
    case Kind::Vertex:
      ensureVertex(theCtx, BRepGraph_VertexId(theSrcNodeId.Index));
      break;
    case Kind::Edge:
      ensureEdge(theCtx, BRepGraph_EdgeId(theSrcNodeId.Index));
      break;
    case Kind::CoEdge:
      break;
    case Kind::Wire:
      ensureWire(theCtx, BRepGraph_WireId(theSrcNodeId.Index));
      break;
    case Kind::Face:
      ensureFace(theCtx, BRepGraph_FaceId(theSrcNodeId.Index));
      break;
    case Kind::Shell:
      ensureShell(theCtx, BRepGraph_ShellId(theSrcNodeId.Index));
      break;
    case Kind::Solid:
      ensureSolid(theCtx, BRepGraph_SolidId(theSrcNodeId.Index));
      break;
    case Kind::Compound:
      ensureCompound(theCtx, BRepGraph_CompoundId(theSrcNodeId.Index));
      break;
    case Kind::CompSolid:
      ensureCompSolid(theCtx, BRepGraph_CompSolidId(theSrcNodeId.Index));
      break;
    case Kind::Product:
      ensureProduct(theCtx, BRepGraph_ProductId(theSrcNodeId.Index));
      break;
    case Kind::Occurrence:
      ensureOccurrence(theCtx, BRepGraph_OccurrenceId(theSrcNodeId.Index));
      break;
    default:
      break;
  }
}

//=================================================================================================

BRepGraph_CompoundId ensureCompound(GraphCopyContext& theCtx, BRepGraph_CompoundId theSrcId)
{
  if (!theSrcId.IsValidIn(theCtx.Source.Topo().Compounds()))
  {
    return BRepGraph_CompoundId();
  }
  const BRepGraph_CompoundId* anExisting = theCtx.Compounds.Seek(theSrcId);
  if (anExisting != nullptr)
  {
    return *anExisting;
  }

  // Pre-allocate an empty compound and bind it BEFORE recursing into children so a
  // self-referencing compound chain (A->B->A or A->A) terminates instead of recursing
  // infinitely. Children are appended one at a time after recursion resolves them.
  const NCollection_Array1<BRepGraph_NodeId> aEmpty;
  const BRepGraph_CompoundId aNewId = theCtx.Result.Editor().Compounds().Add(aEmpty);
  theCtx.DstStorage->SetRemoved(aNewId, theCtx.SrcStorage->IsRemoved(theSrcId));
  bindTypedMap(theCtx.Compounds, theCtx, theSrcId, aNewId);

  for (BRepGraph_RefsChildOfCompound aCRIt(theCtx.Source, theSrcId); aCRIt.More(); aCRIt.Next())
  {
    const BRepGraphInc::ChildRef& aChildRef =
      theCtx.Source.Refs().Children().Entry(aCRIt.CurrentId());
    const BRepGraph_NodeId aSrcChild = aChildRef.ChildNodeId;
    ensureNode(theCtx, aSrcChild);
    const BRepGraph_NodeId aMapped = mappedNode(theCtx, aSrcChild);
    if (aMapped.IsValid())
    {
      const BRepGraph_ChildRefId aNewChildRef =
        theCtx.Result.Editor().Compounds().Append(aNewId, aMapped, aChildRef.Orientation);
      Standard_ASSERT_RAISE(aNewChildRef.IsValid(),
                            "BRepGraph_Copy: failed to copy compound child ref");
      theCtx.DstStorage->SetRemoved(aNewChildRef, theCtx.SrcStorage->IsRemoved(aCRIt.CurrentId()));
      bindItemRemap(theCtx, aCRIt.CurrentId(), aNewChildRef);
    }
  }
  return aNewId;
}

//=================================================================================================

BRepGraph_CompSolidId ensureCompSolid(GraphCopyContext& theCtx, BRepGraph_CompSolidId theSrcId)
{
  if (!theSrcId.IsValidIn(theCtx.Source.Topo().CompSolids()))
  {
    return BRepGraph_CompSolidId();
  }
  const BRepGraph_CompSolidId* anExisting = theCtx.CompSolids.Seek(theSrcId);
  if (anExisting != nullptr)
  {
    return *anExisting;
  }

  NCollection_LinearVector<BRepGraph_SolidId> aSolidIds;
  for (BRepGraph_RefsSolidOfCompSolid aSRIt(theCtx.Source, theSrcId); aSRIt.More(); aSRIt.Next())
  {
    const BRepGraph_SolidId aSrcSolid =
      theCtx.Source.Refs().Solids().Entry(aSRIt.CurrentId()).ChildSolidId;
    aSolidIds.Append(ensureSolid(theCtx, aSrcSolid));
  }

  const BRepGraph_CompSolidId aNewId =
    theCtx.Result.Editor().CompSolids().Add(aSolidIds.ToArray1());
  theCtx.DstStorage->SetRemoved(aNewId, theCtx.SrcStorage->IsRemoved(theSrcId));
  bindTypedMap(theCtx.CompSolids, theCtx, theSrcId, aNewId);
  const BRepGraphInc::CompSolidRelations& aSrcRel =
    theCtx.Source.Topo().CompSolids().Relations(theSrcId);
  const BRepGraphInc::CompSolidRelations& aDstRel =
    theCtx.Result.Topo().CompSolids().Relations(aNewId);
  const size_t aNbSolidRefs = std::min(aSrcRel.SolidRefIds.Size(), aDstRel.SolidRefIds.Size());
  for (size_t anIdx = 0; anIdx < aNbSolidRefs; ++anIdx)
  {
    theCtx.DstStorage->SetRemoved(aDstRel.SolidRefIds.Value(anIdx),
                                  theCtx.SrcStorage->IsRemoved(aSrcRel.SolidRefIds.Value(anIdx)));
    bindItemRemap(theCtx, aSrcRel.SolidRefIds.Value(anIdx), aDstRel.SolidRefIds.Value(anIdx));
  }
  return aNewId;
}

//=================================================================================================

BRepGraph_OccurrenceRefId ensureOccurrenceRef(GraphCopyContext&         theCtx,
                                              BRepGraph_OccurrenceRefId theSrcRefId,
                                              BRepGraph_ProductId       theDstParentProductId)
{
  const BRepGraph_OccurrenceRefId* anExisting = theCtx.OccurrenceRefs.Seek(theSrcRefId);
  if (anExisting != nullptr)
  {
    return *anExisting;
  }

  const BRepGraphInc::OccurrenceRef& aSrcRef = theCtx.SrcStorage->OccurrenceRef(theSrcRefId);
  BRepGraph_OccurrenceId             aNewOccurrenceId;
  if (aSrcRef.ChildOccurrenceId.IsValid())
  {
    aNewOccurrenceId = ensureOccurrence(theCtx, aSrcRef.ChildOccurrenceId);
  }
  const BRepGraph_OccurrenceRefId aNewId =
    theCtx.DstStorage->AttachOccurrenceToProduct(theDstParentProductId,
                                                 aNewOccurrenceId,
                                                 aSrcRef.LocalLocation);
  theCtx.OccurrenceRefs.Bind(theSrcRefId, aNewId);
  bindItemRemap(theCtx, theSrcRefId, aNewId);
  theCtx.DstStorage->SetRemoved(aNewId, theCtx.SrcStorage->IsRemoved(theSrcRefId));
  return aNewId;
}

//=================================================================================================

BRepGraph_OccurrenceId ensureOccurrence(GraphCopyContext& theCtx, BRepGraph_OccurrenceId theSrcId)
{
  const BRepGraph_OccurrenceId* anExisting = theCtx.Occurrences.Seek(theSrcId);
  if (anExisting != nullptr)
  {
    return *anExisting;
  }

  const BRepGraphInc::OccurrenceDef& aSrcOcc = theCtx.SrcStorage->Occurrence(theSrcId);
  const BRepGraph_OccurrenceId       aNewId  = theCtx.DstStorage->AppendOccurrence();
  bindTypedMap(theCtx.Occurrences, theCtx, theSrcId, aNewId);

  BRepGraphInc::OccurrenceDef& aNewOcc = theCtx.DstStorage->ChangeOccurrence(aNewId);
  theCtx.DstStorage->SetRemoved(aNewId, theCtx.SrcStorage->IsRemoved(theSrcId));
  if (aSrcOcc.ChildNodeId.IsValid())
  {
    ensureNode(theCtx, aSrcOcc.ChildNodeId);
    aNewOcc.ChildNodeId = mappedNode(theCtx, aSrcOcc.ChildNodeId);
  }
  return aNewId;
}

//=================================================================================================

BRepGraph_ProductId ensureProduct(GraphCopyContext& theCtx, BRepGraph_ProductId theSrcId)
{
  const BRepGraph_ProductId* anExisting = theCtx.Products.Seek(theSrcId);
  if (anExisting != nullptr)
  {
    return *anExisting;
  }

  // Bind before iterating refs so re-entrant calls (assembly cycles) short-circuit.
  const BRepGraph_ProductId aNewId = theCtx.DstStorage->AppendProduct();
  theCtx.DstStorage->SetRemoved(aNewId, theCtx.SrcStorage->IsRemoved(theSrcId));
  bindTypedMap(theCtx.Products, theCtx, theSrcId, aNewId);

  for (const BRepGraph_OccurrenceRefId& aSrcRefId :
       theCtx.SrcStorage->ProductRelations(theSrcId).OccurrenceRefIds)
  {
    const BRepGraph_OccurrenceRefId aNewRefId = ensureOccurrenceRef(theCtx, aSrcRefId, aNewId);
    Standard_ASSERT_RAISE(aNewRefId.IsValid(), "BRepGraph_Copy: failed to copy occurrence ref");
  }
  return aNewId;
}

//=================================================================================================

//! Build counts for identity copy, adjusting representation counts for drop policies.
static BRepGraphInc_Load::Counts countsForIdentityCopy(const BRepGraphInc_Storage& theSrc,
                                                       GeomPolicy                  theGeomPol,
                                                       MeshPolicy                  theMeshPol)
{
  BRepGraphInc_Load::Counts aCounts = theSrc.Counts();
  if (theGeomPol == GeomPolicy::Drop)
  {
    aCounts.NbFaceSurfaceReps   = 0;
    aCounts.NbEdgeCurve3DReps   = 0;
    aCounts.NbCoEdgeCurve2DReps = 0;
  }
  if (theMeshPol == MeshPolicy::Drop)
  {
    aCounts.NbFaceTriangulationReps  = 0;
    aCounts.NbEdgePolygon3DReps      = 0;
    aCounts.NbCoEdgePolygon2DReps    = 0;
    aCounts.NbCoEdgePolygonOnTriReps = 0;
  }
  return aCounts;
}

//=================================================================================================

//! Copy all topology definitions slot-for-slot from source to destination.
static void copyTopologyDefinitionsIdentity(const BRepGraphInc_Storage& theSrc,
                                            BRepGraphInc_Storage&       theDst,
                                            GeomPolicy                  theGeomPol,
                                            MeshPolicy                  theMeshPol)
{
  // Copy definition structs.
  for (BRepGraph_VertexId anId(0); anId.IsValid(theSrc.NbVertices()); ++anId)
  {
    theDst.ChangeVertex(anId) = theSrc.Vertex(anId);
  }
  for (BRepGraph_EdgeId anId(0); anId.IsValid(theSrc.NbEdges()); ++anId)
  {
    theDst.ChangeEdge(anId) = theSrc.Edge(anId);
  }
  for (BRepGraph_CoEdgeId anId(0); anId.IsValid(theSrc.NbCoEdges()); ++anId)
  {
    theDst.ChangeCoEdge(anId) = theSrc.CoEdge(anId);
  }
  for (BRepGraph_WireId anId(0); anId.IsValid(theSrc.NbWires()); ++anId)
  {
    theDst.ChangeWire(anId) = theSrc.Wire(anId);
  }
  for (BRepGraph_FaceId anId(0); anId.IsValid(theSrc.NbFaces()); ++anId)
  {
    BRepGraphInc::FaceDef& aDstFace = theDst.ChangeFace(anId);
    aDstFace                        = theSrc.Face(anId);
    // Clear surface rep id if geometry is dropped.
    if (theGeomPol == GeomPolicy::Drop)
    {
      aDstFace.SurfaceRepId = BRepGraph_FaceSurfaceRepId();
    }
    if (theMeshPol == MeshPolicy::Drop)
    {
      aDstFace.TriangulationRepId = BRepGraph_FaceTriangulationRepId();
    }
  }
  for (BRepGraph_ShellId anId(0); anId.IsValid(theSrc.NbShells()); ++anId)
  {
    theDst.ChangeShell(anId) = theSrc.Shell(anId);
  }
  for (BRepGraph_SolidId anId(0); anId.IsValid(theSrc.NbSolids()); ++anId)
  {
    theDst.ChangeSolid(anId) = theSrc.Solid(anId);
  }
  for (BRepGraph_CompoundId anId(0); anId.IsValid(theSrc.NbCompounds()); ++anId)
  {
    theDst.ChangeCompound(anId) = theSrc.Compound(anId);
  }
  for (BRepGraph_CompSolidId anId(0); anId.IsValid(theSrc.NbCompSolids()); ++anId)
  {
    theDst.ChangeCompSolid(anId) = theSrc.CompSolid(anId);
  }
  for (BRepGraph_ProductId anId(0); anId.IsValid(theSrc.NbProducts()); ++anId)
  {
    theDst.ChangeProduct(anId) = theSrc.Product(anId);
  }
  for (BRepGraph_OccurrenceId anId(0); anId.IsValid(theSrc.NbOccurrences()); ++anId)
  {
    theDst.ChangeOccurrence(anId) = theSrc.Occurrence(anId);
  }

  // Copy coedge rep ids with policy filtering.
  if (theGeomPol == GeomPolicy::Drop || theMeshPol == MeshPolicy::Drop)
  {
    for (BRepGraph_CoEdgeId anId(0); anId.IsValid(theSrc.NbCoEdges()); ++anId)
    {
      BRepGraphInc::CoEdgeDef& aCoEdge = theDst.ChangeCoEdge(anId);
      if (theGeomPol == GeomPolicy::Drop)
      {
        aCoEdge.Curve2DRepId = BRepGraph_CoEdgeCurve2DRepId();
      }
      if (theMeshPol == MeshPolicy::Drop)
      {
        aCoEdge.Polygon2DRepId    = BRepGraph_CoEdgePolygon2DRepId();
        aCoEdge.PolygonOnTriRepId = BRepGraph_CoEdgePolygonOnTriRepId();
      }
    }
    for (BRepGraph_EdgeId anId(0); anId.IsValid(theSrc.NbEdges()); ++anId)
    {
      if (theGeomPol == GeomPolicy::Drop)
      {
        theDst.ChangeEdge(anId).Curve3DRepId = BRepGraph_EdgeCurve3DRepId();
      }
      if (theMeshPol == MeshPolicy::Drop)
      {
        theDst.ChangeEdge(anId).Polygon3DRepId = BRepGraph_EdgePolygon3DRepId();
      }
    }
  }
}

//=================================================================================================

//! Copy all reference stores slot-for-slot from source to destination.
static void copyReferencesIdentity(const BRepGraphInc_Storage& theSrc, BRepGraphInc_Storage& theDst)
{
  for (BRepGraph_ShellRefId anId(0); anId.IsValid(theSrc.NbShellRefs()); ++anId)
  {
    theDst.ChangeShellRef(anId) = theSrc.ShellRef(anId);
  }
  for (BRepGraph_FaceRefId anId(0); anId.IsValid(theSrc.NbFaceRefs()); ++anId)
  {
    theDst.ChangeFaceRef(anId) = theSrc.FaceRef(anId);
  }
  for (BRepGraph_WireRefId anId(0); anId.IsValid(theSrc.NbWireRefs()); ++anId)
  {
    theDst.ChangeWireRef(anId) = theSrc.WireRef(anId);
  }
  for (BRepGraph_VertexRefId anId(0); anId.IsValid(theSrc.NbVertexRefs()); ++anId)
  {
    theDst.ChangeVertexRef(anId) = theSrc.VertexRef(anId);
  }
  for (BRepGraph_SolidRefId anId(0); anId.IsValid(theSrc.NbSolidRefs()); ++anId)
  {
    theDst.ChangeSolidRef(anId) = theSrc.SolidRef(anId);
  }
  for (BRepGraph_ChildRefId anId(0); anId.IsValid(theSrc.NbChildRefs()); ++anId)
  {
    theDst.ChangeChildRef(anId) = theSrc.ChildRef(anId);
  }
  for (BRepGraph_OccurrenceRefId anId(0); anId.IsValid(theSrc.NbOccurrenceRefs()); ++anId)
  {
    theDst.ChangeOccurrenceRef(anId) = theSrc.OccurrenceRef(anId);
  }
}

//=================================================================================================

//! Copy persistent representation stores with geometry/mesh policy.
static void copyRepresentationsIdentity(const BRepGraphInc_Storage& theSrc,
                                        BRepGraphInc_Storage&       theDst,
                                        GeomPolicy                  theGeomPol,
                                        MeshPolicy                  theMeshPol)
{
  if (theGeomPol != GeomPolicy::Drop)
  {
    // Face surfaces.
    for (BRepGraph_FaceSurfaceRepId anId(0); anId.IsValid(theSrc.NbFaceSurfaces()); ++anId)
    {
      BRepGraphInc::FaceSurfaceRep& aDst = theDst.ChangeFaceSurfaceRep(anId);
      aDst                               = theSrc.FaceSurfaceRep(anId);
      if (theGeomPol == GeomPolicy::Copy && !aDst.Surface.IsNull())
      {
        aDst.Surface = occ::down_cast<Geom_Surface>(aDst.Surface->Copy());
      }
    }

    // Edge 3D curves.
    for (BRepGraph_EdgeCurve3DRepId anId(0); anId.IsValid(theSrc.NbEdgeCurves3D()); ++anId)
    {
      BRepGraphInc::EdgeCurve3DRep& aDst = theDst.ChangeEdgeCurve3DRep(anId);
      aDst                               = theSrc.EdgeCurve3DRep(anId);
      if (theGeomPol == GeomPolicy::Copy && !aDst.Curve.IsNull())
      {
        aDst.Curve = occ::down_cast<Geom_Curve>(aDst.Curve->Copy());
      }
    }

    // Coedge 2D curves.
    for (BRepGraph_CoEdgeCurve2DRepId anId(0); anId.IsValid(theSrc.NbCoEdgeCurves2D()); ++anId)
    {
      BRepGraphInc::CoEdgeCurve2DRep& aDst = theDst.ChangeCoEdgeCurve2DRep(anId);
      aDst                                 = theSrc.CoEdgeCurve2DRep(anId);
      if (theGeomPol == GeomPolicy::Copy && !aDst.Curve.IsNull())
      {
        aDst.Curve = occ::down_cast<Geom2d_Curve>(aDst.Curve->Copy());
      }
    }
  }

  if (theMeshPol != MeshPolicy::Drop)
  {
    // Face triangulations.
    for (BRepGraph_FaceTriangulationRepId anId(0); anId.IsValid(theSrc.NbFaceTriangulations());
         ++anId)
    {
      BRepGraphInc::FaceTriangulationRep& aDst = theDst.ChangeFaceTriangulationRep(anId);
      aDst                                     = theSrc.FaceTriangulationRep(anId);
      if (theMeshPol == MeshPolicy::Copy && !aDst.Triangulation.IsNull())
      {
        aDst.Triangulation = aDst.Triangulation->Copy();
      }
    }

    // Edge 3D polygons.
    for (BRepGraph_EdgePolygon3DRepId anId(0); anId.IsValid(theSrc.NbEdgePolygons3D()); ++anId)
    {
      BRepGraphInc::EdgePolygon3DRep& aDst = theDst.ChangeEdgePolygon3DRep(anId);
      aDst                                 = theSrc.EdgePolygon3DRep(anId);
      if (theMeshPol == MeshPolicy::Copy && !aDst.Polygon.IsNull())
      {
        aDst.Polygon = aDst.Polygon->Copy();
      }
    }

    // Coedge 2D polygons.
    for (BRepGraph_CoEdgePolygon2DRepId anId(0); anId.IsValid(theSrc.NbCoEdgePolygons2D()); ++anId)
    {
      BRepGraphInc::CoEdgePolygon2DRep& aDst = theDst.ChangeCoEdgePolygon2DRep(anId);
      aDst                                   = theSrc.CoEdgePolygon2DRep(anId);
      if (theMeshPol == MeshPolicy::Copy && !aDst.Polygon.IsNull())
      {
        aDst.Polygon = aDst.Polygon->Copy();
      }
    }

    // Coedge polygon-on-triangulation.
    for (BRepGraph_CoEdgePolygonOnTriRepId anId(0); anId.IsValid(theSrc.NbCoEdgePolygonsOnTri());
         ++anId)
    {
      BRepGraphInc::CoEdgePolygonOnTriRep& aDst = theDst.ChangeCoEdgePolygonOnTriRep(anId);
      aDst                                      = theSrc.CoEdgePolygonOnTriRep(anId);
      if (theMeshPol == MeshPolicy::Copy && !aDst.Polygon.IsNull())
      {
        aDst.Polygon = aDst.Polygon->Copy();
      }
    }
  }
}

//=================================================================================================

//! Copy root product ids from source to destination.
static void copyRootProductsIdentity(const BRepGraphInc_Storage& theSrc,
                                     BRepGraphInc_Storage&       theDst)
{
  theDst.ChangeRootProductIds().Clear();
  for (const BRepGraph_ProductId& aRootId : theSrc.RootProductIds())
  {
    theDst.ChangeRootProductIds().Append(aRootId);
  }
}

//=================================================================================================

//! Copy UID counters, generation, and graph GUID from source to destination.
static void copyUIDAndGraphStateIdentity(const BRepGraphInc_Storage& theSrc,
                                         BRepGraphInc_Storage&       theDst)
{
  constexpr BRepGraph_NodeId::Kind aNodeKinds[] = {BRepGraph_NodeId::Kind::Vertex,
                                                   BRepGraph_NodeId::Kind::Edge,
                                                   BRepGraph_NodeId::Kind::CoEdge,
                                                   BRepGraph_NodeId::Kind::Wire,
                                                   BRepGraph_NodeId::Kind::Face,
                                                   BRepGraph_NodeId::Kind::Shell,
                                                   BRepGraph_NodeId::Kind::Solid,
                                                   BRepGraph_NodeId::Kind::Compound,
                                                   BRepGraph_NodeId::Kind::CompSolid,
                                                   BRepGraph_NodeId::Kind::Product,
                                                   BRepGraph_NodeId::Kind::Occurrence};
  for (const auto aKind : aNodeKinds)
  {
    theDst.SetNextNodeUIDCounter(aKind, theSrc.NextNodeUIDCounter(aKind));
  }

  constexpr BRepGraph_RefId::Kind aRefKinds[] = {BRepGraph_RefId::Kind::Shell,
                                                 BRepGraph_RefId::Kind::Face,
                                                 BRepGraph_RefId::Kind::Wire,
                                                 BRepGraph_RefId::Kind::Vertex,
                                                 BRepGraph_RefId::Kind::Solid,
                                                 BRepGraph_RefId::Kind::Child,
                                                 BRepGraph_RefId::Kind::Occurrence};
  for (const auto aKind : aRefKinds)
  {
    theDst.SetNextRefUIDCounter(aKind, theSrc.NextRefUIDCounter(aKind));
  }

  theDst.SetGeneration(theSrc.Generation());
  theDst.SetGraphGUID(theSrc.GraphGUID());
}

//=================================================================================================

//! Copy removed flags and active counts from source to destination.
static void copyFlagsAndActiveCountsIdentity(const BRepGraphInc_Storage& theSrc,
                                             BRepGraphInc_Storage&       theDst)
{
  // Bulk-copy all removed flags in a single pass per kind (vs per-slot loops).
  theDst.CopyRemovedFlagsFrom(theSrc);

  // Recount active counts now that removed flags are correct.
  theDst.RecountActiveCounts();
}

//=================================================================================================

//! Fast identity copy: source and target ids match.
//! Used for Perform(source, emptyTarget, ...) to avoid editor API overhead.
static bool copyFullGraphIdentity(const BRepGraphInc_Storage& aSrc,
                                  BRepGraphInc_Storage&       aDst,
                                  const BRepGraph&            theSourceGraph,
                                  BRepGraph&                  theTargetGraph,
                                  GeomPolicy                  theGeomPolicy,
                                  MeshPolicy                  theMeshPolicy,
                                  BRepGraph_Copy::CachePolicy theCachePolicy)
{
  const BRepGraphInc_Load::Counts aCounts =
    countsForIdentityCopy(aSrc, theGeomPolicy, theMeshPolicy);
  aDst.PrepareForLoad(aCounts);

  copyTopologyDefinitionsIdentity(aSrc, aDst, theGeomPolicy, theMeshPolicy);
  copyReferencesIdentity(aSrc, aDst);
  copyRepresentationsIdentity(aSrc, aDst, theGeomPolicy, theMeshPolicy);
  copyRootProductsIdentity(aSrc, aDst);
  copyUIDAndGraphStateIdentity(aSrc, aDst);
  copyFlagsAndActiveCountsIdentity(aSrc, aDst);

  aDst.CopyDerivedRelationsFrom(aSrc);
  aDst.MarkUIDReverseIndexesDirty();
  aDst.CopyShapeBindingsFrom(aSrc);

  theSourceGraph.LayerRegistry().CopyLayersTo(theTargetGraph,
                                              BRepGraph_CopyRemap::MappingKind::Identity,
                                              BRepGraph_CopyRemap::Mode::Copy);
  if (theCachePolicy == BRepGraph_Copy::CachePolicy::CopyFresh)
  {
    theSourceGraph.CacheRegistry().CopyFreshCachesTo(theTargetGraph,
                                                     BRepGraph_CopyRemap::MappingKind::Identity,
                                                     BRepGraph_CopyRemap::Mode::Copy);
  }
  return true;
}

} // namespace

//=================================================================================================

bool BRepGraph_Copy::Perform(const BRepGraph& theSourceGraph,
                             BRepGraph&       theTargetGraph,
                             GeomPolicy       theGeomPolicy,
                             MeshPolicy       theMeshPolicy,
                             CachePolicy      theCachePolicy)
{
  if (&theSourceGraph == &theTargetGraph)
  {
    return true; // self-copy: identity no-op
  }

  if (theSourceGraph.IsEmpty())
  {
    return false;
  }

  // Non-empty target: use explicit mapping via GraphCopyContext (same as CopyNode).
  // IDs in theTargetGraph will differ from theSourceGraph.
  if (!theTargetGraph.IsEmpty())
  {
    GraphCopyContext aCtx(theSourceGraph, theTargetGraph, false, theGeomPolicy, theMeshPolicy);
    aCtx.SrcStorage = &theSourceGraph.incStorage();
    aCtx.DstStorage = &theTargetGraph.incStorage();
    aCtx.DstData    = theTargetGraph.data();
    aCtx.ItemRemap.Reserve(itemCapacityUpperBound(theSourceGraph.incStorage()));

    // Copy all topological entities bottom-up.
    for (BRepGraph_FullVertexIterator aVIt(theSourceGraph); aVIt.More(); aVIt.Next())
    {
      ensureVertex(aCtx, aVIt.CurrentId());
    }
    for (BRepGraph_FullEdgeIterator aEIt(theSourceGraph); aEIt.More(); aEIt.Next())
    {
      ensureEdge(aCtx, aEIt.CurrentId());
    }
    for (BRepGraph_FullWireIterator aWIt(theSourceGraph); aWIt.More(); aWIt.Next())
    {
      ensureWire(aCtx, aWIt.CurrentId());
    }
    for (BRepGraph_FullFaceIterator aFIt(theSourceGraph); aFIt.More(); aFIt.Next())
    {
      ensureFace(aCtx, aFIt.CurrentId());
    }
    for (BRepGraph_FullShellIterator aSIt(theSourceGraph); aSIt.More(); aSIt.Next())
    {
      ensureShell(aCtx, aSIt.CurrentId());
    }
    for (BRepGraph_FullSolidIterator aSoIt(theSourceGraph); aSoIt.More(); aSoIt.Next())
    {
      ensureSolid(aCtx, aSoIt.CurrentId());
    }
    for (BRepGraph_FullCompoundIterator aCIt(theSourceGraph); aCIt.More(); aCIt.Next())
    {
      ensureCompound(aCtx, aCIt.CurrentId());
    }
    for (BRepGraph_FullCompSolidIterator aCSIt(theSourceGraph); aCSIt.More(); aCSIt.Next())
    {
      ensureCompSolid(aCtx, aCSIt.CurrentId());
    }
    for (BRepGraph_FullProductIterator aPIt(theSourceGraph); aPIt.More(); aPIt.Next())
    {
      ensureProduct(aCtx, aPIt.CurrentId());
    }
    for (BRepGraph_FullOccurrenceIterator aOIt(theSourceGraph); aOIt.More(); aOIt.Next())
    {
      ensureOccurrence(aCtx, aOIt.CurrentId());
    }

    // Build root product list.
    if (!aCtx.Products.IsEmpty())
    {
      NCollection_FlatMap<BRepGraph_ProductId> aReferencedProducts;
      for (BRepGraph_FullOccurrenceIterator anOccIt(theTargetGraph); anOccIt.More(); anOccIt.Next())
      {
        const BRepGraph_OccurrenceId       anOccId = anOccIt.CurrentId();
        const BRepGraphInc::OccurrenceDef& anOcc   = aCtx.DstStorage->Occurrence(anOccId);
        if (!aCtx.DstStorage->IsRemoved(anOccId)
            && anOcc.ChildNodeId.NodeKind == BRepGraph_NodeId::Kind::Product)
        {
          const BRepGraph_ProductId aChildProdId =
            BRepGraph_ProductId::FromNodeId(anOcc.ChildNodeId);
          if (aChildProdId.IsValidIn(theTargetGraph.Topo().Products()))
          {
            aReferencedProducts.Add(aChildProdId);
          }
        }
      }
      for (BRepGraph_FullProductIterator aProdIt(theTargetGraph); aProdIt.More(); aProdIt.Next())
      {
        const BRepGraph_ProductId aProdId = aProdIt.CurrentId();
        if (!aCtx.DstStorage->IsRemoved(aProdId) && !aReferencedProducts.Contains(aProdId))
        {
          aCtx.DstStorage->ChangeRootProductIds().Append(aProdId);
        }
      }
    }

    aCtx.DstStorage->MarkUIDReverseIndexesDirty();
    aCtx.DstStorage->RebuildDerivedRelationsPreservingActiveCounts();
    theSourceGraph.LayerRegistry().CopyLayersTo(theTargetGraph,
                                                aCtx.ItemRemap,
                                                BRepGraph_CopyRemap::Mode::Copy);
    if (theCachePolicy == CachePolicy::CopyFresh)
    {
      theSourceGraph.CacheRegistry().CopyFreshCachesTo(theTargetGraph,
                                                       aCtx.ItemRemap,
                                                       BRepGraph_CopyRemap::Mode::Copy);
    }
    return true;
  }

  // Empty target: fast identity copy using PrepareForLoad() instead of editor APIs.
  return copyFullGraphIdentity(theSourceGraph.incStorage(),
                               theTargetGraph.incStorage(),
                               theSourceGraph,
                               theTargetGraph,
                               theGeomPolicy,
                               theMeshPolicy,
                               theCachePolicy);
}

//=================================================================================================

BRepGraph_NodeId BRepGraph_Copy::CopyNode(const BRepGraph&       theSourceGraph,
                                          BRepGraph&             theTargetGraph,
                                          const BRepGraph_NodeId theNodeId,
                                          GeomPolicy             theGeomPolicy,
                                          MeshPolicy             theMeshPolicy,
                                          CachePolicy            theCachePolicy)
{
  if (theSourceGraph.IsEmpty())
  {
    return BRepGraph_NodeId();
  }

  const bool isSelfCopy = (&theSourceGraph == &theTargetGraph);

  GraphCopyContext theCtx(theSourceGraph, theTargetGraph, isSelfCopy, theGeomPolicy, theMeshPolicy);
  theCtx.SrcStorage = &theSourceGraph.incStorage();
  theCtx.DstStorage = &theTargetGraph.incStorage();
  theCtx.DstData    = theTargetGraph.data();
  theCtx.ItemRemap.Reserve(itemCapacityUpperBound(theSourceGraph.incStorage()));

  ensureNode(theCtx, theNodeId);

  // Build root product list when product entities were copied.
  if (!theCtx.Products.IsEmpty())
  {
    NCollection_FlatMap<BRepGraph_ProductId> aReferencedProducts;
    for (BRepGraph_FullOccurrenceIterator anOccIt(theTargetGraph); anOccIt.More(); anOccIt.Next())
    {
      const BRepGraph_OccurrenceId       anOccId = anOccIt.CurrentId();
      const BRepGraphInc::OccurrenceDef& anOcc   = theCtx.DstStorage->Occurrence(anOccId);
      if (!theCtx.DstStorage->IsRemoved(anOccId)
          && anOcc.ChildNodeId.NodeKind == BRepGraph_NodeId::Kind::Product)
      {
        const BRepGraph_ProductId aChildProdId = BRepGraph_ProductId::FromNodeId(anOcc.ChildNodeId);
        if (aChildProdId.IsValidIn(theTargetGraph.Topo().Products()))
        {
          aReferencedProducts.Add(aChildProdId);
        }
      }
    }
    for (BRepGraph_FullProductIterator aProdIt(theTargetGraph); aProdIt.More(); aProdIt.Next())
    {
      const BRepGraph_ProductId aProdId = aProdIt.CurrentId();
      if (!theCtx.DstStorage->IsRemoved(aProdId) && !aReferencedProducts.Contains(aProdId))
      {
        theCtx.DstStorage->ChangeRootProductIds().Append(aProdId);
      }
    }
  }

  theCtx.DstStorage->MarkUIDReverseIndexesDirty();

  theCtx.DstStorage->RebuildDerivedRelationsPreservingActiveCounts();
  theSourceGraph.LayerRegistry().CopyLayersTo(theTargetGraph,
                                              theCtx.ItemRemap,
                                              BRepGraph_CopyRemap::Mode::Copy);
  if (theCachePolicy == CachePolicy::CopyFresh && !isSelfCopy)
  {
    theSourceGraph.CacheRegistry().CopyFreshCachesTo(theTargetGraph,
                                                     theCtx.ItemRemap,
                                                     BRepGraph_CopyRemap::Mode::Copy);
  }

  return mappedNode(theCtx, theNodeId);
}
