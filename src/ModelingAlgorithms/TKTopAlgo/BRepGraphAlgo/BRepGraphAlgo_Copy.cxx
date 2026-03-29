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

#include <BRepGraphAlgo_Copy.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>

#include <BRepGraph_TransientCache.hxx>
#include <BRepGraph_BuilderView.hxx>
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

//! Transfer transient cache values from source graph/node to destination graph/node.
//! Cache values are shared (same handle) - caller must deep-copy if needed.
//! Uses OwnGen=0 for destination (fresh entities from copy always start at gen 0).
void transferCacheValues(const BRepGraph&       theSrcGraph,
                         const BRepGraph_NodeId theSrcNode,
                         BRepGraph&             theDstGraph,
                         const BRepGraph_NodeId theDstNode)
{
  const BRepGraph_TransientCache& aSrcCache = theSrcGraph.TransientCache();
  if (!aSrcCache.HasCacheValues(theSrcNode))
    return;
  theDstGraph.TransientCache().TransferCacheValues(aSrcCache, theSrcNode, theDstNode, 0);
}

} // namespace

//=================================================================================================

BRepGraph BRepGraphAlgo_Copy::Perform(const BRepGraph& theGraph, bool theCopyGeom)
{
  BRepGraph aResult;
  if (!theGraph.IsDone())
    return aResult;

  const BRepGraph::RefsView& aRefs = theGraph.Refs();

  // Bottom-up graph rebuild via BuilderView.
  // Since this is a full copy, old index == new index (identity mapping).

  // Vertices.
  for (int anIdx = 0; anIdx < theGraph.Topo().NbVertices(); ++anIdx)
  {
    const BRepGraphInc::VertexDef& aVtx = theGraph.Topo().Vertex(BRepGraph_VertexId(anIdx));
    (void)aResult.Builder().AddVertex(aVtx.Point, aVtx.Tolerance);
    transferCacheValues(theGraph, BRepGraph_NodeId::Vertex(anIdx),
                        aResult, BRepGraph_NodeId::Vertex(anIdx));
  }

  // Edges.
  for (int anIdx = 0; anIdx < theGraph.Topo().NbEdges(); ++anIdx)
  {
    const BRepGraphInc::EdgeDef& anEdge = theGraph.Topo().Edge(BRepGraph_EdgeId(anIdx));

    const occ::handle<Geom_Curve>& anEdgeSrcCurve =
      BRepGraph_Tool::Edge::Curve(theGraph, BRepGraph_EdgeId(anIdx));
    occ::handle<Geom_Curve> aCurve = copyCurve(anEdgeSrcCurve, theCopyGeom);

    // Resolve vertex def ids through storage ref entries for the full copy
    // (identity mapping: old index == new index).
    const BRepGraph_NodeId aStartVtxId =
      anEdge.StartVertexRefId.IsValid()
        ? BRepGraph_NodeId::Vertex(aRefs.Vertex(anEdge.StartVertexRefId).VertexDefId.Index)
        : BRepGraph_NodeId();
    const BRepGraph_NodeId anEndVtxId =
      anEdge.EndVertexRefId.IsValid()
        ? BRepGraph_NodeId::Vertex(aRefs.Vertex(anEdge.EndVertexRefId).VertexDefId.Index)
        : BRepGraph_NodeId();

    (void)aResult.Builder().AddEdge(aStartVtxId,
                                    anEndVtxId,
                                    aCurve,
                                    anEdge.ParamFirst,
                                    anEdge.ParamLast,
                                    anEdge.Tolerance);

    BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aNewEdge =
      aResult.Builder().MutEdge(BRepGraph_EdgeId(anIdx));
    aNewEdge->IsDegenerate  = anEdge.IsDegenerate;
    aNewEdge->SameParameter = anEdge.SameParameter;
    aNewEdge->SameRange     = anEdge.SameRange;
    transferCacheValues(theGraph, BRepGraph_NodeId::Edge(anIdx),
                        aResult, BRepGraph_NodeId::Edge(anIdx));
  }

  // Wires.
  for (int anIdx = 0; anIdx < theGraph.Topo().NbWires(); ++anIdx)
  {
    const BRepGraphInc::WireDef& aWire = theGraph.Topo().Wire(BRepGraph_WireId(anIdx));
    NCollection_Vector<std::pair<BRepGraph_NodeId, TopAbs_Orientation>> aWireEdges;
    for (int aCoEdgeIter = 0; aCoEdgeIter < aWire.CoEdgeRefIds.Length(); ++aCoEdgeIter)
    {
      const BRepGraph_CoEdgeRefId    aRefId = aWire.CoEdgeRefIds.Value(aCoEdgeIter);
      const BRepGraphInc::CoEdgeRef& aCR    = aRefs.CoEdge(aRefId);
      if (aCR.IsRemoved || !aCR.CoEdgeDefId.IsValid(theGraph.Topo().NbCoEdges()))
        continue;
      const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdge(aCR.CoEdgeDefId);
      if (!aCoEdge.EdgeDefId.IsValid(theGraph.Topo().NbEdges()))
        continue;
      aWireEdges.Append(std::make_pair(aCoEdge.EdgeDefId, aCoEdge.Sense));
    }
    (void)aResult.Builder().AddWire(aWireEdges);
    transferCacheValues(theGraph, BRepGraph_NodeId::Wire(anIdx),
                        aResult, BRepGraph_NodeId::Wire(anIdx));
  }

  // Faces.
  for (int anIdx = 0; anIdx < theGraph.Topo().NbFaces(); ++anIdx)
  {
    const BRepGraphInc::FaceDef& aFace = theGraph.Topo().Face(BRepGraph_FaceId(anIdx));

    const occ::handle<Geom_Surface>& aFaceSrcSurf =
      BRepGraph_Tool::Face::Surface(theGraph, BRepGraph_FaceId(anIdx));
    occ::handle<Geom_Surface> aSurf = copySurface(aFaceSrcSurf, theCopyGeom);

    // Get outer/inner wire def NodeIds from incidence refs.
    BRepGraph_NodeId                     anOuterWire;
    NCollection_Vector<BRepGraph_NodeId> anInnerWires;

    for (int aWireIdx = 0; aWireIdx < aFace.WireRefIds.Length(); ++aWireIdx)
    {
      const BRepGraph_WireRefId    aRefId = aFace.WireRefIds.Value(aWireIdx);
      const BRepGraphInc::WireRef& aWR    = aRefs.Wire(aRefId);
      if (aWR.IsRemoved || !aWR.WireDefId.IsValid(theGraph.Topo().NbWires()))
        continue;
      const BRepGraph_NodeId aWireDefId = aWR.WireDefId;
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

    BRepGraph_MutGuard<BRepGraphInc::FaceDef> aNewFace =
      aResult.Builder().MutFace(BRepGraph_FaceId(anIdx));
    aNewFace->NaturalRestriction       = aFace.NaturalRestriction;
    aNewFace->TriangulationRepIds      = aFace.TriangulationRepIds;
    aNewFace->ActiveTriangulationIndex = aFace.ActiveTriangulationIndex;
    transferCacheValues(theGraph, BRepGraph_NodeId::Face(anIdx),
                        aResult, BRepGraph_NodeId::Face(anIdx));
  }

  // PCurves via CoEdge data (after edges and faces are created).
  for (int anIdx = 0; anIdx < theGraph.Topo().NbEdges(); ++anIdx)
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIds =
      theGraph.Topo().CoEdgesOfEdge(BRepGraph_EdgeId(anIdx));
    for (int aCEIter = 0; aCEIter < aCoEdgeIds.Length(); ++aCEIter)
    {
      const BRepGraph_CoEdgeId       aCoEdgeId = aCoEdgeIds.Value(aCEIter);
      const BRepGraphInc::CoEdgeDef& aCoEdge   = theGraph.Topo().CoEdge(aCoEdgeId);
      if (!aCoEdge.Curve2DRepId.IsValid())
        continue;

      const occ::handle<Geom2d_Curve>& aCoEdgeSrcPC =
        BRepGraph_Tool::CoEdge::PCurve(theGraph, aCoEdgeId);
      occ::handle<Geom2d_Curve> aNewPC = copyPCurve(aCoEdgeSrcPC, theCopyGeom);
      aResult.Builder().AddPCurveToEdge(BRepGraph_NodeId::Edge(anIdx),
                                        aCoEdge.FaceDefId,
                                        aNewPC,
                                        aCoEdge.ParamFirst,
                                        aCoEdge.ParamLast,
                                        aCoEdge.Sense);
    }
  }

  // Shells.
  for (int anIdx = 0; anIdx < theGraph.Topo().NbShells(); ++anIdx)
  {
    BRepGraph_NodeId aNewShellId = aResult.Builder().AddShell();

    const BRepGraphInc::ShellDef& aShell = theGraph.Topo().Shell(BRepGraph_ShellId(anIdx));
    transferCacheValues(theGraph, BRepGraph_NodeId::Shell(anIdx),
                        aResult, BRepGraph_NodeId::Shell(anIdx));

    for (int aFaceRefIdx = 0; aFaceRefIdx < aShell.FaceRefIds.Length(); ++aFaceRefIdx)
    {
      const BRepGraph_FaceRefId    aRefId = aShell.FaceRefIds.Value(aFaceRefIdx);
      const BRepGraphInc::FaceRef& aFR    = aRefs.Face(aRefId);
      if (aFR.IsRemoved || !aFR.FaceDefId.IsValid(theGraph.Topo().NbFaces()))
        continue;
      const BRepGraph_NodeId aFaceDefId = aFR.FaceDefId;
      aResult.Builder().AddFaceToShell(aNewShellId, aFaceDefId, aFR.Orientation);
    }
  }

  // Solids.
  for (int anIdx = 0; anIdx < theGraph.Topo().NbSolids(); ++anIdx)
  {
    BRepGraph_NodeId aNewSolidId = aResult.Builder().AddSolid();

    const BRepGraphInc::SolidDef& aSolid = theGraph.Topo().Solid(BRepGraph_SolidId(anIdx));
    transferCacheValues(theGraph, BRepGraph_NodeId::Solid(anIdx),
                        aResult, BRepGraph_NodeId::Solid(anIdx));

    for (int aShellRefIdx = 0; aShellRefIdx < aSolid.ShellRefIds.Length(); ++aShellRefIdx)
    {
      const BRepGraph_ShellRefId    aRefId = aSolid.ShellRefIds.Value(aShellRefIdx);
      const BRepGraphInc::ShellRef& aSR    = aRefs.Shell(aRefId);
      if (aSR.IsRemoved || !aSR.ShellDefId.IsValid(theGraph.Topo().NbShells()))
        continue;
      const BRepGraph_NodeId aShellDefId = aSR.ShellDefId;
      aResult.Builder().AddShellToSolid(aNewSolidId, aShellDefId, aSR.Orientation);
    }
  }

  // Compounds.
  for (int anIdx = 0; anIdx < theGraph.Topo().NbCompounds(); ++anIdx)
  {
    const BRepGraphInc::CompoundDef& aComp = theGraph.Topo().Compound(BRepGraph_CompoundId(anIdx));
    NCollection_Vector<BRepGraph_NodeId> aChildNodeIds;
    for (int aCI = 0; aCI < aComp.ChildRefIds.Length(); ++aCI)
    {
      const BRepGraph_ChildRefId    aRefId = aComp.ChildRefIds.Value(aCI);
      const BRepGraphInc::ChildRef& aCR    = aRefs.Child(aRefId);
      if (aCR.IsRemoved || !aCR.ChildDefId.IsValid())
        continue;
      aChildNodeIds.Append(aCR.ChildDefId);
    }
    (void)aResult.Builder().AddCompound(aChildNodeIds);
    transferCacheValues(theGraph, BRepGraph_NodeId::Compound(anIdx),
                        aResult, BRepGraph_NodeId::Compound(anIdx));
  }

  // CompSolids.
  for (int anIdx = 0; anIdx < theGraph.Topo().NbCompSolids(); ++anIdx)
  {
    const BRepGraphInc::CompSolidDef& aCS = theGraph.Topo().CompSolid(BRepGraph_CompSolidId(anIdx));
    NCollection_Vector<BRepGraph_NodeId> aSolidNodeIds;
    for (int aSI = 0; aSI < aCS.SolidRefIds.Length(); ++aSI)
    {
      const BRepGraph_SolidRefId    aRefId = aCS.SolidRefIds.Value(aSI);
      const BRepGraphInc::SolidRef& aSR    = aRefs.Solid(aRefId);
      if (aSR.IsRemoved || !aSR.SolidDefId.IsValid(theGraph.Topo().NbSolids()))
        continue;
      aSolidNodeIds.Append(aSR.SolidDefId);
    }
    (void)aResult.Builder().AddCompSolid(aSolidNodeIds);
    transferCacheValues(theGraph, BRepGraph_NodeId::CompSolid(anIdx),
                        aResult, BRepGraph_NodeId::CompSolid(anIdx));
  }

  // Products.
  for (int anIdx = 0; anIdx < theGraph.myData->myIncStorage.NbProducts(); ++anIdx)
  {
    const BRepGraphInc::ProductDef& aSrcProd =
      theGraph.myData->myIncStorage.Product(BRepGraph_ProductId(anIdx));
    BRepGraphInc::ProductDef& aNewProd = aResult.myData->myIncStorage.AppendProduct();
    aNewProd.Id                        = BRepGraph_NodeId::Product(anIdx);
    aNewProd.ShapeRootId               = aSrcProd.ShapeRootId;
    aNewProd.RootOrientation           = aSrcProd.RootOrientation;
    aNewProd.RootLocation              = aSrcProd.RootLocation;
    aNewProd.OccurrenceRefIds          = aSrcProd.OccurrenceRefIds;
    aResult.allocateUID(aNewProd.Id);
  }

  // Occurrences.
  for (int anIdx = 0; anIdx < theGraph.myData->myIncStorage.NbOccurrences(); ++anIdx)
  {
    const BRepGraphInc::OccurrenceDef& aSrcOcc =
      theGraph.myData->myIncStorage.Occurrence(BRepGraph_OccurrenceId(anIdx));
    BRepGraphInc::OccurrenceDef& aNewOcc = aResult.myData->myIncStorage.AppendOccurrence();
    aNewOcc.Id                           = BRepGraph_NodeId::Occurrence(anIdx);
    aNewOcc.ProductDefId                 = aSrcOcc.ProductDefId;
    aNewOcc.ParentProductDefId           = aSrcOcc.ParentProductDefId;
    aNewOcc.ParentOccurrenceDefId        = aSrcOcc.ParentOccurrenceDefId;
    aNewOcc.Placement                    = aSrcOcc.Placement;
    aResult.allocateUID(aNewOcc.Id);
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

  copyUIDs(theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Vertex),
           aResult.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Vertex));
  copyUIDs(theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Edge),
           aResult.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Edge));
  copyUIDs(theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Wire),
           aResult.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Wire));
  copyUIDs(theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Face),
           aResult.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Face));
  copyUIDs(theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Shell),
           aResult.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Shell));
  copyUIDs(theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Solid),
           aResult.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Solid));
  copyUIDs(theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Compound),
           aResult.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Compound));
  copyUIDs(theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::CompSolid),
           aResult.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::CompSolid));
  copyUIDs(theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Product),
           aResult.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Product));
  copyUIDs(theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Occurrence),
           aResult.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Occurrence));

  aResult.myData->myNextUIDCounter.store(
    theGraph.myData->myNextUIDCounter.load(std::memory_order_relaxed),
    std::memory_order_relaxed);
  aResult.myData->myGeneration.store(theGraph.myData->myGeneration.load(std::memory_order_relaxed),
                                     std::memory_order_relaxed);
  aResult.myData->myIsDone = true;

  // Pre-allocate transient cache for lock-free parallel access on the copied graph.
  {
    BRepGraphInc_Storage& aStorage = aResult.myData->myIncStorage;
    int aCounts[BRepGraph_TransientCache::THE_KIND_COUNT] = {};
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Vertex)]     = aStorage.NbVertices();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Edge)]       = aStorage.NbEdges();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::CoEdge)]     = aStorage.NbCoEdges();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Wire)]       = aStorage.NbWires();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Face)]       = aStorage.NbFaces();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Shell)]      = aStorage.NbShells();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Solid)]      = aStorage.NbSolids();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Compound)]   = aStorage.NbCompounds();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::CompSolid)]  = aStorage.NbCompSolids();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Product)]    = aStorage.NbProducts();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Occurrence)] = aStorage.NbOccurrences();
    int aReservedKindCount = BRepGraph_TransientCache::THE_DEFAULT_RESERVED_KIND_COUNT;
    const int aRegisteredKindCount = BRepGraph_CacheKindRegistry::NbRegistered();
    if (aRegisteredKindCount > aReservedKindCount)
    {
      aReservedKindCount = aRegisteredKindCount;
    }
    aResult.myTransientCache.Reserve(aReservedKindCount, aCounts);
  }

  return aResult;
}

//=================================================================================================

BRepGraph BRepGraphAlgo_Copy::CopyFace(const BRepGraph&       theGraph,
                                       const BRepGraph_FaceId theFace,
                                       const bool             theCopyGeom)
{
  BRepGraph aResult;
  if (!theGraph.IsDone() || theFace.Index < 0 || theFace.Index >= theGraph.Topo().NbFaces())
    return aResult;

  const BRepGraph::RefsView&   aRefs    = theGraph.Refs();
  const BRepGraphInc::FaceDef& aFaceDef = theGraph.Topo().Face(theFace);
  bool                         hasWires = false;
  for (int aWireRefIdx = 0; aWireRefIdx < aFaceDef.WireRefIds.Length(); ++aWireRefIdx)
  {
    const BRepGraph_WireRefId    aRefId = aFaceDef.WireRefIds.Value(aWireRefIdx);
    const BRepGraphInc::WireRef& aWR    = aRefs.Wire(aRefId);
    if (!aWR.IsRemoved && aWR.WireDefId.IsValid(theGraph.Topo().NbWires()))
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
  for (int aWireRefIdx = 0; aWireRefIdx < aFaceDef.WireRefIds.Length(); ++aWireRefIdx)
  {
    const BRepGraph_WireRefId    aWireRefId = aFaceDef.WireRefIds.Value(aWireRefIdx);
    const BRepGraphInc::WireRef& aWR        = aRefs.Wire(aWireRefId);
    if (aWR.IsRemoved || !aWR.WireDefId.IsValid(theGraph.Topo().NbWires()))
      continue;
    const int aWireDefIdx = aWR.WireDefId.Index;
    aWireSet.Add(aWireDefIdx);

    const BRepGraphInc::WireDef& aWireDef = theGraph.Topo().Wire(aWR.WireDefId);
    for (int aCoEdgeIter = 0; aCoEdgeIter < aWireDef.CoEdgeRefIds.Length(); ++aCoEdgeIter)
    {
      const BRepGraph_CoEdgeRefId    aCoEdgeRefId = aWireDef.CoEdgeRefIds.Value(aCoEdgeIter);
      const BRepGraphInc::CoEdgeRef& aCR          = aRefs.CoEdge(aCoEdgeRefId);
      if (aCR.IsRemoved || !aCR.CoEdgeDefId.IsValid(theGraph.Topo().NbCoEdges()))
        continue;
      const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdge(aCR.CoEdgeDefId);
      if (!aCoEdge.EdgeDefId.IsValid(theGraph.Topo().NbEdges()))
        continue;
      anEdgeSet.Add(aCoEdge.EdgeDefId.Index);

      const BRepGraphInc::EdgeDef& anEdgeDef = theGraph.Topo().Edge(aCoEdge.EdgeDefId);
      if (anEdgeDef.StartVertexRefId.IsValid())
      {
        const BRepGraph_VertexId aStartVtx = aRefs.Vertex(anEdgeDef.StartVertexRefId).VertexDefId;
        if (aStartVtx.IsValid(theGraph.Topo().NbVertices()))
          aVertexSet.Add(aStartVtx.Index);
      }
      if (anEdgeDef.EndVertexRefId.IsValid())
      {
        const BRepGraph_VertexId anEndVtx = aRefs.Vertex(anEdgeDef.EndVertexRefId).VertexDefId;
        if (anEndVtx.IsValid(theGraph.Topo().NbVertices()))
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
    const BRepGraphInc::VertexDef& aVtx     = theGraph.Topo().Vertex(BRepGraph_VertexId(anOldIdx));
    (void)aResult.Builder().AddVertex(aVtx.Point, aVtx.Tolerance);
    transferCacheValues(theGraph, BRepGraph_NodeId::Vertex(anOldIdx),
                        aResult, BRepGraph_NodeId::Vertex(anIdx - 1));
  }

  // Add edges in deterministic order.
  for (int anIdx = 1; anIdx <= anEdgeSet.Extent(); ++anIdx)
  {
    const int                    anOldIdx = anEdgeSet.FindKey(anIdx);
    const BRepGraphInc::EdgeDef& anEdge   = theGraph.Topo().Edge(BRepGraph_EdgeId(anOldIdx));

    BRepGraph_NodeId aNewStart, aNewEnd;
    if (anEdge.StartVertexRefId.IsValid())
    {
      const BRepGraph_VertexId aStartVtx = aRefs.Vertex(anEdge.StartVertexRefId).VertexDefId;
      if (aStartVtx.IsValid())
      {
        const int* aNewVtxIdx = aVertexMap.Seek(aStartVtx.Index);
        if (aNewVtxIdx != nullptr)
          aNewStart = BRepGraph_NodeId::Vertex(*aNewVtxIdx);
      }
    }
    if (anEdge.EndVertexRefId.IsValid())
    {
      const BRepGraph_VertexId anEndVtx = aRefs.Vertex(anEdge.EndVertexRefId).VertexDefId;
      if (anEndVtx.IsValid())
      {
        const int* aNewVtxIdx = aVertexMap.Seek(anEndVtx.Index);
        if (aNewVtxIdx != nullptr)
          aNewEnd = BRepGraph_NodeId::Vertex(*aNewVtxIdx);
      }
    }

    const occ::handle<Geom_Curve>& anEdgeSrcCurve2 =
      BRepGraph_Tool::Edge::Curve(theGraph, BRepGraph_EdgeId(anOldIdx));
    occ::handle<Geom_Curve> aCurve = copyCurve(anEdgeSrcCurve2, theCopyGeom);

    const int aNewEdgeIdx = anIdx - 1;
    (void)aResult.Builder()
      .AddEdge(aNewStart, aNewEnd, aCurve, anEdge.ParamFirst, anEdge.ParamLast, anEdge.Tolerance);

    BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aNewEdge =
      aResult.Builder().MutEdge(BRepGraph_EdgeId(aNewEdgeIdx));
    aNewEdge->IsDegenerate  = anEdge.IsDegenerate;
    aNewEdge->SameParameter = anEdge.SameParameter;
    aNewEdge->SameRange     = anEdge.SameRange;
    transferCacheValues(theGraph, BRepGraph_NodeId::Edge(anOldIdx),
                        aResult, BRepGraph_NodeId::Edge(aNewEdgeIdx));
  }

  // Add wires in deterministic order.
  for (int anIdx = 1; anIdx <= aWireSet.Extent(); ++anIdx)
  {
    const int                    anOldIdx = aWireSet.FindKey(anIdx);
    const BRepGraphInc::WireDef& aWire    = theGraph.Topo().Wire(BRepGraph_WireId(anOldIdx));
    NCollection_Vector<std::pair<BRepGraph_NodeId, TopAbs_Orientation>> aNewEntries;
    for (int aCoEdgeIter = 0; aCoEdgeIter < aWire.CoEdgeRefIds.Length(); ++aCoEdgeIter)
    {
      const BRepGraph_CoEdgeRefId    aCoEdgeRefId = aWire.CoEdgeRefIds.Value(aCoEdgeIter);
      const BRepGraphInc::CoEdgeRef& aCR          = aRefs.CoEdge(aCoEdgeRefId);
      if (aCR.IsRemoved || !aCR.CoEdgeDefId.IsValid(theGraph.Topo().NbCoEdges()))
        continue;
      const BRepGraphInc::CoEdgeDef& aCoEdge     = theGraph.Topo().CoEdge(aCR.CoEdgeDefId);
      const int*                     aNewEdgeIdx = anEdgeMap.Seek(aCoEdge.EdgeDefId.Index);
      if (aNewEdgeIdx == nullptr)
        continue;
      aNewEntries.Append(std::make_pair(BRepGraph_NodeId::Edge(*aNewEdgeIdx), aCoEdge.Sense));
    }
    (void)aResult.Builder().AddWire(aNewEntries);
    {
      const int anOldWireIdx = aWireSet.FindKey(anIdx);
      transferCacheValues(theGraph, BRepGraph_NodeId::Wire(anOldWireIdx),
                          aResult, BRepGraph_NodeId::Wire(anIdx - 1));
    }
  }

  // Add the face.
  const occ::handle<Geom_Surface>& aFaceSrcSurf2 = BRepGraph_Tool::Face::Surface(theGraph, theFace);
  occ::handle<Geom_Surface>        aSurf         = copySurface(aFaceSrcSurf2, theCopyGeom);

  BRepGraph_NodeId                     anOuterWire;
  NCollection_Vector<BRepGraph_NodeId> anInnerWires;

  for (int aWireRefIdx = 0; aWireRefIdx < aFaceDef.WireRefIds.Length(); ++aWireRefIdx)
  {
    const BRepGraph_WireRefId    aRefId = aFaceDef.WireRefIds.Value(aWireRefIdx);
    const BRepGraphInc::WireRef& aWR    = aRefs.Wire(aRefId);
    if (aWR.IsRemoved || !aWR.WireDefId.IsValid(theGraph.Topo().NbWires()))
      continue;
    const int* aNewWireIdx = aWireMap.Seek(aWR.WireDefId.Index);
    if (aNewWireIdx == nullptr)
      continue;
    if (aWR.IsOuter)
    {
      anOuterWire = BRepGraph_NodeId::Wire(*aNewWireIdx);
    }
    else
    {
      anInnerWires.Append(BRepGraph_NodeId::Wire(*aNewWireIdx));
    }
  }

  (void)aResult.Builder().AddFace(aSurf, anOuterWire, anInnerWires, aFaceDef.Tolerance);
  BRepGraph_MutGuard<BRepGraphInc::FaceDef> aNewFace = aResult.Builder().MutFace(BRepGraph_FaceId(0));
  aNewFace->NaturalRestriction                     = aFaceDef.NaturalRestriction;
  aNewFace->TriangulationRepIds                    = aFaceDef.TriangulationRepIds;
  aNewFace->ActiveTriangulationIndex               = aFaceDef.ActiveTriangulationIndex;
  transferCacheValues(theGraph, theFace, aResult, BRepGraph_NodeId::Face(0));

  // PCurves for edges in this face via CoEdge data.
  for (int anIdx = 1; anIdx <= anEdgeSet.Extent(); ++anIdx)
  {
    const int                                     anOldEdgeIdx = anEdgeSet.FindKey(anIdx);
    const int                                     aNewEdgeIdx  = anIdx - 1;
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIds =
      theGraph.Topo().CoEdgesOfEdge(BRepGraph_EdgeId(anOldEdgeIdx));
    for (int aCEIter = 0; aCEIter < aCoEdgeIds.Length(); ++aCEIter)
    {
      const BRepGraph_CoEdgeId       aCoEdgeId = aCoEdgeIds.Value(aCEIter);
      const BRepGraphInc::CoEdgeDef& aCoEdge   = theGraph.Topo().CoEdge(aCoEdgeId);
      // Only copy CoEdges belonging to this face.
      if (aCoEdge.FaceDefId.Index != theFace.Index)
        continue;
      if (!aCoEdge.Curve2DRepId.IsValid())
        continue;

      const occ::handle<Geom2d_Curve>& aCoEdgeSrcPC2 =
        BRepGraph_Tool::CoEdge::PCurve(theGraph, aCoEdgeId);
      occ::handle<Geom2d_Curve> aNewPC = copyPCurve(aCoEdgeSrcPC2, theCopyGeom);
      aResult.Builder().AddPCurveToEdge(BRepGraph_NodeId::Edge(aNewEdgeIdx),
                                        BRepGraph_NodeId::Face(0),
                                        aNewPC,
                                        aCoEdge.ParamFirst,
                                        aCoEdge.ParamLast,
                                        aCoEdge.Sense);
    }
  }

  aResult.myData->myIsDone = true;

  // Pre-allocate transient cache for lock-free parallel access on the copied graph.
  {
    BRepGraphInc_Storage& aStorage = aResult.myData->myIncStorage;
    int aCounts[BRepGraph_TransientCache::THE_KIND_COUNT] = {};
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Vertex)]     = aStorage.NbVertices();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Edge)]       = aStorage.NbEdges();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::CoEdge)]     = aStorage.NbCoEdges();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Wire)]       = aStorage.NbWires();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Face)]       = aStorage.NbFaces();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Shell)]      = aStorage.NbShells();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Solid)]      = aStorage.NbSolids();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Compound)]   = aStorage.NbCompounds();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::CompSolid)]  = aStorage.NbCompSolids();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Product)]    = aStorage.NbProducts();
    aCounts[static_cast<int>(BRepGraph_NodeId::Kind::Occurrence)] = aStorage.NbOccurrences();
    int aReservedKindCount = BRepGraph_TransientCache::THE_DEFAULT_RESERVED_KIND_COUNT;
    const int aRegisteredKindCount = BRepGraph_CacheKindRegistry::NbRegistered();
    if (aRegisteredKindCount > aReservedKindCount)
    {
      aReservedKindCount = aRegisteredKindCount;
    }
    aResult.myTransientCache.Reserve(aReservedKindCount, aCounts);
  }

  return aResult;
}
