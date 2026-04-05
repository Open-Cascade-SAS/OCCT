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
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_CacheView.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_TopoView.hxx>

#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_IndexedMap.hxx>

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
  for (auto aKindIt = theSrcGraph.Cache().CacheKindIter(theSrcKey);
       aKindIt.More();
       aKindIt.Next())
  {
    const occ::handle<BRepGraph_CacheKind>  aKind  = aKindIt.Value();
    const occ::handle<BRepGraph_CacheValue> aValue = theSrcGraph.Cache().Get(theSrcKey, aKind);
    if (!aValue.IsNull())
    {
      theDstGraph.Cache().Set(theDstKey, aKind, aValue);
    }
  }
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

  // Bottom-up graph rebuild via BuilderView.
  // Since this is a full copy, old index == new index (identity mapping).

  // Vertices.
  const int aNbVertices = theGraph.Topo().Vertices().Nb();
  for (BRepGraph_VertexId aVertexId(0); aVertexId.IsValid(aNbVertices); ++aVertexId)
  {
    const BRepGraphInc::VertexDef& aVtx = theGraph.Topo().Vertices().Definition(aVertexId);
    (void)aResult.Builder().AddVertex(aVtx.Point, aVtx.Tolerance);
    transferFreshCacheValues(theGraph, aVertexId, aResult, aVertexId);
  }

  // Edges.
  const int aNbEdges = theGraph.Topo().Edges().Nb();
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const BRepGraphInc::EdgeDef& anEdge = theGraph.Topo().Edges().Definition(anEdgeId);

    const occ::handle<Geom_Curve>& anEdgeSrcCurve = BRepGraph_Tool::Edge::Curve(theGraph, anEdgeId);
    occ::handle<Geom_Curve>        aCurve         = copyCurve(anEdgeSrcCurve, theCopyGeom);

    // Resolve vertex def ids through storage ref entries for the full copy
    // (identity mapping: old index == new index).
    const BRepGraph_VertexId aStartVtxId =
      anEdge.StartVertexRefId.IsValid()
        ? aRefs.Vertices().Entry(anEdge.StartVertexRefId).VertexDefId
        : BRepGraph_VertexId();
    const BRepGraph_VertexId anEndVtxId =
      anEdge.EndVertexRefId.IsValid() ? aRefs.Vertices().Entry(anEdge.EndVertexRefId).VertexDefId
                                      : BRepGraph_VertexId();

    (void)aResult.Builder().AddEdge(aStartVtxId,
                                    anEndVtxId,
                                    aCurve,
                                    anEdge.ParamFirst,
                                    anEdge.ParamLast,
                                    anEdge.Tolerance);

    {
      BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aNewEdge = aResult.Builder().MutEdge(anEdgeId);
      aNewEdge->IsDegenerate                             = anEdge.IsDegenerate;
      aNewEdge->SameParameter                            = anEdge.SameParameter;
      aNewEdge->SameRange                                = anEdge.SameRange;
    }
    transferFreshCacheValues(theGraph, anEdgeId, aResult, anEdgeId);
  }

  // Wires.
  const int aNbWires = theGraph.Topo().Wires().Nb();
  for (BRepGraph_WireId aWireId(0); aWireId.IsValid(aNbWires); ++aWireId)
  {
    const BRepGraphInc::WireDef& aWire = theGraph.Topo().Wires().Definition(aWireId);
    NCollection_Vector<std::pair<BRepGraph_EdgeId, TopAbs_Orientation>> aWireEdges;
    for (const BRepGraph_CoEdgeRefId& aRefId : aWire.CoEdgeRefIds)
    {
      const BRepGraphInc::CoEdgeRef& aCR = aRefs.CoEdges().Entry(aRefId);
      if (aCR.IsRemoved || !aCR.CoEdgeDefId.IsValid(theGraph.Topo().CoEdges().Nb()))
        continue;
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        theGraph.Topo().CoEdges().Definition(aCR.CoEdgeDefId);
      if (!aCoEdge.EdgeDefId.IsValid(theGraph.Topo().Edges().Nb()))
        continue;
      aWireEdges.Append(std::make_pair(aCoEdge.EdgeDefId, aCoEdge.Orientation));
    }
    (void)aResult.Builder().AddWire(aWireEdges);
    transferFreshCacheValues(theGraph, aWireId, aResult, aWireId);
  }

  // Faces.
  const int aNbFaces = theGraph.Topo().Faces().Nb();
  for (BRepGraph_FaceId aFaceId(0); aFaceId.IsValid(aNbFaces); ++aFaceId)
  {
    const BRepGraphInc::FaceDef& aFace = theGraph.Topo().Faces().Definition(aFaceId);

    const occ::handle<Geom_Surface>& aFaceSrcSurf =
      BRepGraph_Tool::Face::Surface(theGraph, aFaceId);
    occ::handle<Geom_Surface> aSurf = copySurface(aFaceSrcSurf, theCopyGeom);

    // Get outer/inner wire def NodeIds from incidence refs.
    BRepGraph_WireId                     anOuterWire;
    NCollection_Vector<BRepGraph_WireId> anInnerWires;

    for (const BRepGraph_WireRefId& aRefId : aFace.WireRefIds)
    {
      const BRepGraphInc::WireRef& aWR = aRefs.Wires().Entry(aRefId);
      if (aWR.IsRemoved || !aWR.WireDefId.IsValid(theGraph.Topo().Wires().Nb()))
        continue;
      const BRepGraph_WireId aWireDefId = aWR.WireDefId;
      if (aWR.IsOuter)
      {
        anOuterWire = aWireDefId;
      }
      else
      {
        anInnerWires.Append(aWireDefId);
      }
    }

    (void)aResult.Builder().AddFace(aSurf, anOuterWire, anInnerWires, aFace.Tolerance);

    {
      BRepGraph_MutGuard<BRepGraphInc::FaceDef> aNewFace = aResult.Builder().MutFace(aFaceId);
      aNewFace->NaturalRestriction                       = aFace.NaturalRestriction;
      aNewFace->TriangulationRepIds                      = aFace.TriangulationRepIds;
      aNewFace->ActiveTriangulationIndex                 = aFace.ActiveTriangulationIndex;
    }
    transferFreshCacheValues(theGraph, aFaceId, aResult, aFaceId);
  }

  // PCurves via CoEdge data (after edges and faces are created).
  for (BRepGraph_EdgeId anEdgeId(0); anEdgeId.IsValid(aNbEdges); ++anEdgeId)
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIds =
      theGraph.Topo().Edges().CoEdges(anEdgeId);
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdgeIds)
    {
      const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(aCoEdgeId);
      if (!aCoEdge.Curve2DRepId.IsValid())
        continue;

      const occ::handle<Geom2d_Curve>& aCoEdgeSrcPC =
        BRepGraph_Tool::CoEdge::PCurve(theGraph, aCoEdgeId);
      occ::handle<Geom2d_Curve> aNewPC = copyPCurve(aCoEdgeSrcPC, theCopyGeom);
      aResult.Builder().AddPCurveToEdge(anEdgeId,
                                        aCoEdge.FaceDefId,
                                        aNewPC,
                                        aCoEdge.ParamFirst,
                                        aCoEdge.ParamLast,
                                        aCoEdge.Orientation);
    }
  }

  // Shells.
  const int aNbShells = theGraph.Topo().Shells().Nb();
  for (BRepGraph_ShellId aShellId(0); aShellId.IsValid(aNbShells); ++aShellId)
  {
    BRepGraph_ShellId aNewShellId = aResult.Builder().AddShell();

    const BRepGraphInc::ShellDef& aShell = theGraph.Topo().Shells().Definition(aShellId);
    transferFreshCacheValues(theGraph, aShellId, aResult, aShellId);

    for (const BRepGraph_FaceRefId& aRefId : aShell.FaceRefIds)
    {
      const BRepGraphInc::FaceRef& aFR = aRefs.Faces().Entry(aRefId);
      if (aFR.IsRemoved || !aFR.FaceDefId.IsValid(theGraph.Topo().Faces().Nb()))
        continue;
      const BRepGraph_FaceId aFaceDefId = aFR.FaceDefId;
      const BRepGraph_FaceRefId aNewFaceRefId =
        aResult.Builder().AddFaceToShell(aNewShellId, aFaceDefId, aFR.Orientation);
      transferFreshCacheValues(theGraph, aRefId, aResult, aNewFaceRefId);
    }
  }

  // Solids.
  const int aNbSolids = theGraph.Topo().Solids().Nb();
  for (BRepGraph_SolidId aSolidId(0); aSolidId.IsValid(aNbSolids); ++aSolidId)
  {
    BRepGraph_SolidId aNewSolidId = aResult.Builder().AddSolid();

    const BRepGraphInc::SolidDef& aSolid = theGraph.Topo().Solids().Definition(aSolidId);
    transferFreshCacheValues(theGraph, aSolidId, aResult, aSolidId);

    for (const BRepGraph_ShellRefId& aRefId : aSolid.ShellRefIds)
    {
      const BRepGraphInc::ShellRef& aSR = aRefs.Shells().Entry(aRefId);
      if (aSR.IsRemoved || !aSR.ShellDefId.IsValid(theGraph.Topo().Shells().Nb()))
        continue;
      const BRepGraph_ShellId aShellDefId = aSR.ShellDefId;
      const BRepGraph_ShellRefId aNewShellRefId =
        aResult.Builder().AddShellToSolid(aNewSolidId, aShellDefId, aSR.Orientation);
      transferFreshCacheValues(theGraph, aRefId, aResult, aNewShellRefId);
    }
  }

  // Compounds.
  const int aNbCompounds = theGraph.Topo().Compounds().Nb();
  for (BRepGraph_CompoundId aCompoundId(0); aCompoundId.IsValid(aNbCompounds); ++aCompoundId)
  {
    const BRepGraphInc::CompoundDef& aComp = theGraph.Topo().Compounds().Definition(aCompoundId);
    NCollection_Vector<BRepGraph_NodeId> aChildNodeIds;
    for (const BRepGraph_ChildRefId& aRefId : aComp.ChildRefIds)
    {
      const BRepGraphInc::ChildRef& aCR = aRefs.Children().Entry(aRefId);
      if (aCR.IsRemoved || !aCR.ChildDefId.IsValid())
        continue;
      aChildNodeIds.Append(aCR.ChildDefId);
    }
    (void)aResult.Builder().AddCompound(aChildNodeIds);
    transferFreshCacheValues(theGraph, aCompoundId, aResult, aCompoundId);
  }

  // CompSolids.
  const int aNbCompSolids = theGraph.Topo().CompSolids().Nb();
  for (BRepGraph_CompSolidId aCompSolidId(0); aCompSolidId.IsValid(aNbCompSolids); ++aCompSolidId)
  {
    const BRepGraphInc::CompSolidDef& aCS = theGraph.Topo().CompSolids().Definition(aCompSolidId);
    NCollection_Vector<BRepGraph_SolidId> aSolidNodeIds;
    for (const BRepGraph_SolidRefId& aRefId : aCS.SolidRefIds)
    {
      const BRepGraphInc::SolidRef& aSR = aRefs.Solids().Entry(aRefId);
      if (aSR.IsRemoved || !aSR.SolidDefId.IsValid(theGraph.Topo().Solids().Nb()))
        continue;
      aSolidNodeIds.Append(aSR.SolidDefId);
    }
    (void)aResult.Builder().AddCompSolid(aSolidNodeIds);
    transferFreshCacheValues(theGraph, aCompSolidId, aResult, aCompSolidId);
  }

  // Products.
  const int aNbProducts = theGraph.incStorage().NbProducts();
  for (BRepGraph_ProductId aProductId(0); aProductId.IsValid(aNbProducts); ++aProductId)
  {
    const BRepGraphInc::ProductDef& aSrcProd = theGraph.incStorage().Product(aProductId);
    BRepGraphInc::ProductDef&       aNewProd = aResult.incStorage().AppendProduct();
    aNewProd.Id                              = aProductId;
    aNewProd.ShapeRootId                     = aSrcProd.ShapeRootId;
    aNewProd.RootOrientation                 = aSrcProd.RootOrientation;
    aNewProd.RootLocation                    = aSrcProd.RootLocation;
    aNewProd.OccurrenceRefIds                = aSrcProd.OccurrenceRefIds;
  }

  // Occurrences.
  const int aNbOccurrences = theGraph.incStorage().NbOccurrences();
  for (BRepGraph_OccurrenceId anOccurrenceId(0); anOccurrenceId.IsValid(aNbOccurrences);
       ++anOccurrenceId)
  {
    const BRepGraphInc::OccurrenceDef& aSrcOcc = theGraph.incStorage().Occurrence(anOccurrenceId);
    BRepGraphInc::OccurrenceDef&       aNewOcc = aResult.incStorage().AppendOccurrence();
    aNewOcc.Id                                 = anOccurrenceId;
    aNewOcc.ProductDefId                       = aSrcOcc.ProductDefId;
    aNewOcc.ParentProductDefId                 = aSrcOcc.ParentProductDefId;
    aNewOcc.ParentOccurrenceDefId              = aSrcOcc.ParentOccurrenceDefId;
    aNewOcc.Placement                          = aSrcOcc.Placement;
  }

  // Phase 3: Transfer UIDs (identity mapping - direct vector copy).
  auto copyUIDs = [](const NCollection_Vector<BRepGraph_UID>& theSrc,
                     NCollection_Vector<BRepGraph_UID>&       theDst) {
    const int aNbToCopy = std::min(theSrc.Length(), theDst.Length());
    for (int anIdx = 0; anIdx < aNbToCopy; ++anIdx)
    {
      if (theSrc.Value(anIdx).IsValid())
        theDst.ChangeValue(anIdx) = theSrc.Value(anIdx);
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

  // Pre-allocate transient cache for lock-free parallel access on the copied graph.
  reserveTransientCache(aResult);

  return aResult;
}

//=================================================================================================

BRepGraph BRepGraph_Copy::CopyFace(const BRepGraph&       theGraph,
                                   const BRepGraph_FaceId theFace,
                                   const bool             theCopyGeom,
                                   const bool             theReserveCache)
{
  BRepGraph aResult;
  if (!theGraph.IsDone() || theFace.Index < 0 || theFace.Index >= theGraph.Topo().Faces().Nb())
    return aResult;

  const BRepGraph::RefsView&   aRefs    = theGraph.Refs();
  const BRepGraphInc::FaceDef& aFaceDef = theGraph.Topo().Faces().Definition(theFace);
  bool                         hasWires = false;
  for (const BRepGraph_WireRefId& aRefId : aFaceDef.WireRefIds)
  {
    const BRepGraphInc::WireRef& aWR = aRefs.Wires().Entry(aRefId);
    if (!aWR.IsRemoved && aWR.WireDefId.IsValid(theGraph.Topo().Wires().Nb()))
    {
      hasWires = true;
      break;
    }
  }
  if (!hasWires)
    return aResult;

  // Use NCollection_IndexedMap to collect old indices in deterministic insertion order.
  NCollection_IndexedMap<int> aVertexSet, anEdgeSet, aWireSet;

  // Collect all edges/vertices/wires from the face's wire refs.
  for (const BRepGraph_WireRefId& aWireRefId : aFaceDef.WireRefIds)
  {
    const BRepGraphInc::WireRef& aWR = aRefs.Wires().Entry(aWireRefId);
    if (aWR.IsRemoved || !aWR.WireDefId.IsValid(theGraph.Topo().Wires().Nb()))
      continue;
    const int aWireDefIdx = aWR.WireDefId.Index;
    aWireSet.Add(aWireDefIdx);

    const BRepGraphInc::WireDef& aWireDef = theGraph.Topo().Wires().Definition(aWR.WireDefId);
    for (const BRepGraph_CoEdgeRefId& aCoEdgeRefId : aWireDef.CoEdgeRefIds)
    {
      const BRepGraphInc::CoEdgeRef& aCR = aRefs.CoEdges().Entry(aCoEdgeRefId);
      if (aCR.IsRemoved || !aCR.CoEdgeDefId.IsValid(theGraph.Topo().CoEdges().Nb()))
        continue;
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        theGraph.Topo().CoEdges().Definition(aCR.CoEdgeDefId);
      if (!aCoEdge.EdgeDefId.IsValid(theGraph.Topo().Edges().Nb()))
        continue;
      anEdgeSet.Add(aCoEdge.EdgeDefId.Index);

      const BRepGraphInc::EdgeDef& anEdgeDef =
        theGraph.Topo().Edges().Definition(aCoEdge.EdgeDefId);
      if (anEdgeDef.StartVertexRefId.IsValid())
      {
        const BRepGraph_VertexId aStartVtx =
          aRefs.Vertices().Entry(anEdgeDef.StartVertexRefId).VertexDefId;
        if (aStartVtx.IsValid(theGraph.Topo().Vertices().Nb()))
          aVertexSet.Add(aStartVtx.Index);
      }
      if (anEdgeDef.EndVertexRefId.IsValid())
      {
        const BRepGraph_VertexId anEndVtx =
          aRefs.Vertices().Entry(anEdgeDef.EndVertexRefId).VertexDefId;
        if (anEndVtx.IsValid(theGraph.Topo().Vertices().Nb()))
          aVertexSet.Add(anEndVtx.Index);
      }
    }
  }

  // Build old->new index maps from the ordered sets.
  NCollection_DataMap<int, int> aVertexMap(aVertexSet.Extent());
  for (int anIdx = 1; anIdx <= aVertexSet.Extent(); ++anIdx)
    aVertexMap.Bind(aVertexSet.FindKey(anIdx), anIdx - 1);

  NCollection_DataMap<int, int> anEdgeMap(anEdgeSet.Extent());
  for (int anIdx = 1; anIdx <= anEdgeSet.Extent(); ++anIdx)
    anEdgeMap.Bind(anEdgeSet.FindKey(anIdx), anIdx - 1);

  NCollection_DataMap<int, int> aWireMap(aWireSet.Extent());
  for (int anIdx = 1; anIdx <= aWireSet.Extent(); ++anIdx)
    aWireMap.Bind(aWireSet.FindKey(anIdx), anIdx - 1);

  // Add vertices in deterministic order.
  for (int anIdx = 1; anIdx <= aVertexSet.Extent(); ++anIdx)
  {
    const int                      anOldIdx = aVertexSet.FindKey(anIdx);
    const BRepGraphInc::VertexDef& aVtx =
      theGraph.Topo().Vertices().Definition(BRepGraph_VertexId(anOldIdx));
    (void)aResult.Builder().AddVertex(aVtx.Point, aVtx.Tolerance);
    transferFreshCacheValues(theGraph,
                             BRepGraph_VertexId(anOldIdx),
                             aResult,
                             BRepGraph_VertexId(anIdx - 1));
  }

  // Add edges in deterministic order.
  for (int anIdx = 1; anIdx <= anEdgeSet.Extent(); ++anIdx)
  {
    const int                    anOldIdx = anEdgeSet.FindKey(anIdx);
    const BRepGraphInc::EdgeDef& anEdge =
      theGraph.Topo().Edges().Definition(BRepGraph_EdgeId(anOldIdx));

    BRepGraph_VertexId aNewStart, aNewEnd;
    if (anEdge.StartVertexRefId.IsValid())
    {
      const BRepGraph_VertexId aStartVtx =
        aRefs.Vertices().Entry(anEdge.StartVertexRefId).VertexDefId;
      if (aStartVtx.IsValid())
      {
        const int* aNewVtxIdx = aVertexMap.Seek(aStartVtx.Index);
        if (aNewVtxIdx != nullptr)
          aNewStart = BRepGraph_VertexId(*aNewVtxIdx);
      }
    }
    if (anEdge.EndVertexRefId.IsValid())
    {
      const BRepGraph_VertexId anEndVtx = aRefs.Vertices().Entry(anEdge.EndVertexRefId).VertexDefId;
      if (anEndVtx.IsValid())
      {
        const int* aNewVtxIdx = aVertexMap.Seek(anEndVtx.Index);
        if (aNewVtxIdx != nullptr)
          aNewEnd = BRepGraph_VertexId(*aNewVtxIdx);
      }
    }

    const occ::handle<Geom_Curve>& anEdgeSrcCurve =
      BRepGraph_Tool::Edge::Curve(theGraph, BRepGraph_EdgeId(anOldIdx));
    occ::handle<Geom_Curve> aCurve = copyCurve(anEdgeSrcCurve, theCopyGeom);

    const int aNewEdgeIdx = anIdx - 1;
    (void)aResult.Builder()
      .AddEdge(aNewStart, aNewEnd, aCurve, anEdge.ParamFirst, anEdge.ParamLast, anEdge.Tolerance);

    {
      BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aNewEdge =
        aResult.Builder().MutEdge(BRepGraph_EdgeId(aNewEdgeIdx));
      aNewEdge->IsDegenerate  = anEdge.IsDegenerate;
      aNewEdge->SameParameter = anEdge.SameParameter;
      aNewEdge->SameRange     = anEdge.SameRange;
    }
    transferFreshCacheValues(theGraph,
                             BRepGraph_EdgeId(anOldIdx),
                             aResult,
                             BRepGraph_EdgeId(aNewEdgeIdx));
  }

  // Add wires in deterministic order.
  for (int anIdx = 1; anIdx <= aWireSet.Extent(); ++anIdx)
  {
    const int                    anOldIdx = aWireSet.FindKey(anIdx);
    const BRepGraphInc::WireDef& aWire =
      theGraph.Topo().Wires().Definition(BRepGraph_WireId(anOldIdx));
    NCollection_Vector<std::pair<BRepGraph_EdgeId, TopAbs_Orientation>> aNewEntries;
    for (const BRepGraph_CoEdgeRefId& aCoEdgeRefId : aWire.CoEdgeRefIds)
    {
      const BRepGraphInc::CoEdgeRef& aCR = aRefs.CoEdges().Entry(aCoEdgeRefId);
      if (aCR.IsRemoved || !aCR.CoEdgeDefId.IsValid(theGraph.Topo().CoEdges().Nb()))
        continue;
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        theGraph.Topo().CoEdges().Definition(aCR.CoEdgeDefId);
      const int* aNewEdgeIdx = anEdgeMap.Seek(aCoEdge.EdgeDefId.Index);
      if (aNewEdgeIdx == nullptr)
        continue;
      aNewEntries.Append(std::make_pair(BRepGraph_EdgeId(*aNewEdgeIdx), aCoEdge.Orientation));
    }
    (void)aResult.Builder().AddWire(aNewEntries);
    {
      const int anOldWireIdx = aWireSet.FindKey(anIdx);
      transferFreshCacheValues(theGraph,
                               BRepGraph_WireId(anOldWireIdx),
                               aResult,
                               BRepGraph_WireId(anIdx - 1));
    }
  }

  // Add the face.
  const occ::handle<Geom_Surface>& aFaceSrcSurf = BRepGraph_Tool::Face::Surface(theGraph, theFace);
  occ::handle<Geom_Surface>        aSurf        = copySurface(aFaceSrcSurf, theCopyGeom);

  BRepGraph_WireId                     anOuterWire;
  NCollection_Vector<BRepGraph_WireId> anInnerWires;

  for (const BRepGraph_WireRefId& aRefId : aFaceDef.WireRefIds)
  {
    const BRepGraphInc::WireRef& aWR = aRefs.Wires().Entry(aRefId);
    if (aWR.IsRemoved || !aWR.WireDefId.IsValid(theGraph.Topo().Wires().Nb()))
      continue;
    const int* aNewWireIdx = aWireMap.Seek(aWR.WireDefId.Index);
    if (aNewWireIdx == nullptr)
      continue;
    if (aWR.IsOuter)
    {
      anOuterWire = BRepGraph_WireId(*aNewWireIdx);
    }
    else
    {
      anInnerWires.Append(BRepGraph_WireId(*aNewWireIdx));
    }
  }

  (void)aResult.Builder().AddFace(aSurf, anOuterWire, anInnerWires, aFaceDef.Tolerance);
  {
    BRepGraph_MutGuard<BRepGraphInc::FaceDef> aNewFace =
      aResult.Builder().MutFace(BRepGraph_FaceId(0));
    aNewFace->NaturalRestriction       = aFaceDef.NaturalRestriction;
    aNewFace->TriangulationRepIds      = aFaceDef.TriangulationRepIds;
    aNewFace->ActiveTriangulationIndex = aFaceDef.ActiveTriangulationIndex;
  }
  transferFreshCacheValues(theGraph, theFace, aResult, BRepGraph_FaceId(0));

  // PCurves for edges in this face via CoEdge data.
  for (int anIdx = 1; anIdx <= anEdgeSet.Extent(); ++anIdx)
  {
    const int                                     anOldEdgeIdx = anEdgeSet.FindKey(anIdx);
    const int                                     aNewEdgeIdx  = anIdx - 1;
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIds =
      theGraph.Topo().Edges().CoEdges(BRepGraph_EdgeId(anOldEdgeIdx));
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdgeIds)
    {
      const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(aCoEdgeId);
      // Only copy CoEdges belonging to this face.
      if (aCoEdge.FaceDefId.Index != theFace.Index)
        continue;
      if (!aCoEdge.Curve2DRepId.IsValid())
        continue;

      const occ::handle<Geom2d_Curve>& aCoEdgeSrcPC =
        BRepGraph_Tool::CoEdge::PCurve(theGraph, aCoEdgeId);
      occ::handle<Geom2d_Curve> aNewPC = copyPCurve(aCoEdgeSrcPC, theCopyGeom);
      aResult.Builder().AddPCurveToEdge(BRepGraph_EdgeId(aNewEdgeIdx),
                                        BRepGraph_FaceId(0),
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

  return aResult;
}
