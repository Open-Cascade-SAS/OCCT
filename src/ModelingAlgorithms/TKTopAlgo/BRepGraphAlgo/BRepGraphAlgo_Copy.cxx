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

#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_MutRef.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraphInc_IncidenceRef.hxx>

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

//! Transfer user attributes from source node cache to destination node cache.
//! Attributes are shared (same shared_ptr) - caller must deep-copy if needed.
void transferUserAttributes(const BRepGraph_NodeCache& theSrc, BRepGraph_NodeCache& theDst)
{
  if (!theSrc.HasUserAttributes())
    return;
  NCollection_Vector<int> aKeys = theSrc.UserAttributeKeys();
  for (int anIdx = 0; anIdx < aKeys.Length(); ++anIdx)
  {
    occ::handle<BRepGraph_UserAttribute> anAttr = theSrc.GetUserAttribute(aKeys.Value(anIdx));
    if (!anAttr.IsNull())
      theDst.SetUserAttribute(aKeys.Value(anIdx), anAttr);
  }
}

} // namespace

//=================================================================================================

BRepGraph BRepGraphAlgo_Copy::Perform(const BRepGraph& theGraph, bool theCopyGeom)
{
  BRepGraph aResult;
  if (!theGraph.IsDone())
    return aResult;

  // Bottom-up graph rebuild via BuilderView.
  // Since this is a full copy, old index == new index (identity mapping).

  // Vertices.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbVertices(); ++anIdx)
  {
    const BRepGraph_TopoNode::VertexDef& aVtx = theGraph.Defs().Vertex(BRepGraph_VertexId(anIdx));
    (void)aResult.Builder().AddVertexDef(aVtx.Point, aVtx.Tolerance);
    transferUserAttributes(aVtx.Cache, aResult.MutVertex(BRepGraph_VertexId(anIdx))->Cache);
  }

  // Edges.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbEdges(); ++anIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = theGraph.Defs().Edge(BRepGraph_EdgeId(anIdx));

    const occ::handle<Geom_Curve>& anEdgeSrcCurve =
      BRepGraph_Tool::Edge::Curve(theGraph, BRepGraph_EdgeId(anIdx));
    occ::handle<Geom_Curve> aCurve = copyCurve(anEdgeSrcCurve, theCopyGeom);

    (void)aResult.Builder().AddEdgeDef(anEdge.StartVertexDefId(),
                                 anEdge.EndVertexDefId(),
                                 aCurve,
                                 anEdge.ParamFirst,
                                 anEdge.ParamLast,
                                 anEdge.Tolerance);

    BRepGraph_MutRef<BRepGraph_TopoNode::EdgeDef> aNewEdge =
      aResult.MutEdge(BRepGraph_EdgeId(anIdx));
    aNewEdge->IsDegenerate  = anEdge.IsDegenerate;
    aNewEdge->SameParameter = anEdge.SameParameter;
    aNewEdge->SameRange     = anEdge.SameRange;
    transferUserAttributes(anEdge.Cache, aNewEdge->Cache);
  }

  // Wires.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbWires(); ++anIdx)
  {
    const BRepGraph_TopoNode::WireDef& aWire = theGraph.Defs().Wire(BRepGraph_WireId(anIdx));
    NCollection_Vector<std::pair<BRepGraph_NodeId, TopAbs_Orientation>> aWireEdges;
    for (int aCoEdgeIter = 0; aCoEdgeIter < aWire.CoEdgeRefs.Length(); ++aCoEdgeIter)
    {
      const BRepGraphInc::CoEdgeRef&       aCR     = aWire.CoEdgeRefs.Value(aCoEdgeIter);
      const BRepGraph_TopoNode::CoEdgeDef& aCoEdge = theGraph.Defs().CoEdge(aCR.CoEdgeDefId);
      aWireEdges.Append(std::make_pair(aCoEdge.EdgeDefId, aCoEdge.Sense));
    }
    (void)aResult.Builder().AddWireDef(aWireEdges);
    transferUserAttributes(aWire.Cache, aResult.MutWire(BRepGraph_WireId(anIdx))->Cache);
  }

  // Faces.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbFaces(); ++anIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFace = theGraph.Defs().Face(BRepGraph_FaceId(anIdx));

    const occ::handle<Geom_Surface>& aFaceSrcSurf =
      BRepGraph_Tool::Face::Surface(theGraph, BRepGraph_FaceId(anIdx));
    occ::handle<Geom_Surface> aSurf = copySurface(aFaceSrcSurf, theCopyGeom);

    // Get outer/inner wire def NodeIds from incidence refs.
    BRepGraph_NodeId                     anOuterWire;
    NCollection_Vector<BRepGraph_NodeId> anInnerWires;

    for (int aWireIdx = 0; aWireIdx < aFace.WireRefs.Length(); ++aWireIdx)
    {
      const BRepGraphInc::WireRef& aWR        = aFace.WireRefs.Value(aWireIdx);
      const BRepGraph_NodeId       aWireDefId = aWR.WireDefId;
      if (aWR.IsOuter)
      {
        anOuterWire = aWireDefId;
      }
      else
      {
        anInnerWires.Append(aWireDefId);
      }
    }

    (void)aResult.Builder().AddFaceDef(aSurf, anOuterWire, anInnerWires, aFace.Tolerance);

    BRepGraph_MutRef<BRepGraph_TopoNode::FaceDef> aNewFace =
      aResult.MutFace(BRepGraph_FaceId(anIdx));
    aNewFace->NaturalRestriction       = aFace.NaturalRestriction;
    aNewFace->TriangulationRepIds      = aFace.TriangulationRepIds;
    aNewFace->ActiveTriangulationIndex = aFace.ActiveTriangulationIndex;
    transferUserAttributes(aFace.Cache, aNewFace->Cache);
  }

  // PCurves via CoEdge data (after edges and faces are created).
  const BRepGraphInc_ReverseIndex& aRevIdx = theGraph.myData->myIncStorage.ReverseIndex();
  for (int anIdx = 0; anIdx < theGraph.Defs().NbEdges(); ++anIdx)
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>* aCoEdgeIdxs =
      aRevIdx.CoEdgesOfEdge(BRepGraph_EdgeId(anIdx));
    if (aCoEdgeIdxs == nullptr)
      continue;
    for (int aCEIter = 0; aCEIter < aCoEdgeIdxs->Length(); ++aCEIter)
    {
      const BRepGraph_CoEdgeId             aCoEdgeId = aCoEdgeIdxs->Value(aCEIter);
      const BRepGraph_TopoNode::CoEdgeDef& aCoEdge   = theGraph.Defs().CoEdge(aCoEdgeId);
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
  for (int anIdx = 0; anIdx < theGraph.Defs().NbShells(); ++anIdx)
  {
    BRepGraph_NodeId aNewShellId = aResult.Builder().AddShellDef();

    const BRepGraph_TopoNode::ShellDef& aShell = theGraph.Defs().Shell(BRepGraph_ShellId(anIdx));
    transferUserAttributes(aShell.Cache, aResult.MutShell(BRepGraph_ShellId(anIdx))->Cache);

    for (int aFaceRefIdx = 0; aFaceRefIdx < aShell.FaceRefs.Length(); ++aFaceRefIdx)
    {
      const BRepGraphInc::FaceRef& aFR        = aShell.FaceRefs.Value(aFaceRefIdx);
      const BRepGraph_NodeId       aFaceDefId = aFR.FaceDefId;
      aResult.Builder().AddFaceToShell(aNewShellId, aFaceDefId, aFR.Orientation);
    }
  }

  // Solids.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbSolids(); ++anIdx)
  {
    BRepGraph_NodeId aNewSolidId = aResult.Builder().AddSolidDef();

    const BRepGraph_TopoNode::SolidDef& aSolid = theGraph.Defs().Solid(BRepGraph_SolidId(anIdx));
    transferUserAttributes(aSolid.Cache, aResult.MutSolid(BRepGraph_SolidId(anIdx))->Cache);

    for (int aShellRefIdx = 0; aShellRefIdx < aSolid.ShellRefs.Length(); ++aShellRefIdx)
    {
      const BRepGraphInc::ShellRef& aSR         = aSolid.ShellRefs.Value(aShellRefIdx);
      const BRepGraph_NodeId        aShellDefId = aSR.ShellDefId;
      aResult.Builder().AddShellToSolid(aNewSolidId, aShellDefId, aSR.Orientation);
    }
  }

  // Compounds.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbCompounds(); ++anIdx)
  {
    const BRepGraph_TopoNode::CompoundDef& aComp =
      theGraph.Defs().Compound(BRepGraph_CompoundId(anIdx));
    NCollection_Vector<BRepGraph_NodeId> aChildNodeIds;
    for (int aCI = 0; aCI < aComp.ChildRefs.Length(); ++aCI)
    {
      const BRepGraphInc::ChildRef& aCR = aComp.ChildRefs.Value(aCI);
      aChildNodeIds.Append(aCR.ChildDefId);
    }
    (void)aResult.Builder().AddCompoundDef(aChildNodeIds);
    transferUserAttributes(aComp.Cache, aResult.MutCompound(BRepGraph_CompoundId(anIdx))->Cache);
  }

  // CompSolids.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbCompSolids(); ++anIdx)
  {
    const BRepGraph_TopoNode::CompSolidDef& aCS =
      theGraph.Defs().CompSolid(BRepGraph_CompSolidId(anIdx));
    NCollection_Vector<BRepGraph_NodeId> aSolidNodeIds;
    for (int aSI = 0; aSI < aCS.SolidRefs.Length(); ++aSI)
      aSolidNodeIds.Append(aCS.SolidRefs.Value(aSI).SolidDefId);
    (void)aResult.Builder().AddCompSolidDef(aSolidNodeIds);
    transferUserAttributes(aCS.Cache, aResult.MutCompSolid(BRepGraph_CompSolidId(anIdx))->Cache);
  }

  // Products.
  for (int anIdx = 0; anIdx < theGraph.myData->myIncStorage.NbProducts(); ++anIdx)
  {
    const BRepGraphInc::ProductEntity& aSrcProd =
      theGraph.myData->myIncStorage.Product(BRepGraph_ProductId(anIdx));
    BRepGraphInc::ProductEntity& aNewProd = aResult.myData->myIncStorage.AppendProduct();
    aNewProd.Id                           = BRepGraph_NodeId::Product(anIdx);
    aNewProd.ShapeRootId                  = aSrcProd.ShapeRootId;
    aNewProd.RootOrientation              = aSrcProd.RootOrientation;
    aNewProd.RootLocation                 = aSrcProd.RootLocation;
    aNewProd.OccurrenceRefs               = aSrcProd.OccurrenceRefs;
    aResult.allocateUID(aNewProd.Id);
  }

  // Occurrences.
  for (int anIdx = 0; anIdx < theGraph.myData->myIncStorage.NbOccurrences(); ++anIdx)
  {
    const BRepGraphInc::OccurrenceEntity& aSrcOcc =
      theGraph.myData->myIncStorage.Occurrence(BRepGraph_OccurrenceId(anIdx));
    BRepGraphInc::OccurrenceEntity& aNewOcc = aResult.myData->myIncStorage.AppendOccurrence();
    aNewOcc.Id                              = BRepGraph_NodeId::Occurrence(anIdx);
    aNewOcc.ProductDefId                    = aSrcOcc.ProductDefId;
    aNewOcc.ParentProductDefId              = aSrcOcc.ParentProductDefId;
    aNewOcc.ParentOccurrenceDefId           = aSrcOcc.ParentOccurrenceDefId;
    aNewOcc.Placement                       = aSrcOcc.Placement;
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
  aResult.myData->myGeneration.store(
    theGraph.myData->myGeneration.load(std::memory_order_relaxed),
    std::memory_order_relaxed);
  aResult.myData->myIsDone     = true;

  return aResult;
}

//=================================================================================================

BRepGraph BRepGraphAlgo_Copy::CopyFace(const BRepGraph&       theGraph,
                                       const BRepGraph_FaceId theFace,
                                       const bool             theCopyGeom)
{
  BRepGraph aResult;
  if (!theGraph.IsDone() || theFace.Index < 0 || theFace.Index >= theGraph.Defs().NbFaces())
    return aResult;

  const BRepGraph_TopoNode::FaceDef& aFaceDef = theGraph.Defs().Face(theFace);
  if (aFaceDef.WireRefs.IsEmpty())
    return aResult;

  // Use NCollection_IndexedMap to collect old indices in deterministic insertion order.
  NCollection_IndexedMap<int> aVertexSet, anEdgeSet, aWireSet;

  // Collect all edges/vertices/wires from the face's wire refs.
  for (int aWireRefIdx = 0; aWireRefIdx < aFaceDef.WireRefs.Length(); ++aWireRefIdx)
  {
    const BRepGraphInc::WireRef& aWR         = aFaceDef.WireRefs.Value(aWireRefIdx);
    const int                    aWireDefIdx = aWR.WireDefId.Index;
    aWireSet.Add(aWireDefIdx);

    const BRepGraph_TopoNode::WireDef& aWireDef =
      theGraph.Defs().Wire(BRepGraph_WireId(aWireDefIdx));
    for (int aCoEdgeIter = 0; aCoEdgeIter < aWireDef.CoEdgeRefs.Length(); ++aCoEdgeIter)
    {
      const BRepGraphInc::CoEdgeRef&       aCR     = aWireDef.CoEdgeRefs.Value(aCoEdgeIter);
      const BRepGraph_TopoNode::CoEdgeDef& aCoEdge = theGraph.Defs().CoEdge(aCR.CoEdgeDefId);
      if (!aCoEdge.EdgeDefId.IsValid(theGraph.Defs().NbEdges()))
        continue;
      anEdgeSet.Add(aCoEdge.EdgeDefId.Index);

      const BRepGraph_TopoNode::EdgeDef& anEdgeDef = theGraph.Defs().Edge(aCoEdge.EdgeDefId);
      if (anEdgeDef.StartVertex.VertexDefId.IsValid(theGraph.Defs().NbVertices()))
        aVertexSet.Add(anEdgeDef.StartVertex.VertexDefId.Index);
      if (anEdgeDef.EndVertex.VertexDefId.IsValid(theGraph.Defs().NbVertices()))
        aVertexSet.Add(anEdgeDef.EndVertex.VertexDefId.Index);
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
    const int                            anOldIdx = aVertexSet.FindKey(anIdx);
    const BRepGraph_TopoNode::VertexDef& aVtx =
      theGraph.Defs().Vertex(BRepGraph_VertexId(anOldIdx));
    (void)aResult.Builder().AddVertexDef(aVtx.Point, aVtx.Tolerance);
    transferUserAttributes(aVtx.Cache, aResult.MutVertex(BRepGraph_VertexId(anIdx - 1))->Cache);
  }

  // Add edges in deterministic order.
  for (int anIdx = 1; anIdx <= anEdgeSet.Extent(); ++anIdx)
  {
    const int                          anOldIdx = anEdgeSet.FindKey(anIdx);
    const BRepGraph_TopoNode::EdgeDef& anEdge   = theGraph.Defs().Edge(BRepGraph_EdgeId(anOldIdx));

    BRepGraph_NodeId aNewStart, aNewEnd;
    if (anEdge.StartVertex.VertexDefId.IsValid())
    {
      const int* aNewVtxIdx = aVertexMap.Seek(anEdge.StartVertex.VertexDefId.Index);
      if (aNewVtxIdx != nullptr)
        aNewStart = BRepGraph_NodeId::Vertex(*aNewVtxIdx);
    }
    if (anEdge.EndVertex.VertexDefId.IsValid())
    {
      const int* aNewVtxIdx = aVertexMap.Seek(anEdge.EndVertex.VertexDefId.Index);
      if (aNewVtxIdx != nullptr)
        aNewEnd = BRepGraph_NodeId::Vertex(*aNewVtxIdx);
    }

    const occ::handle<Geom_Curve>& anEdgeSrcCurve2 =
      BRepGraph_Tool::Edge::Curve(theGraph, BRepGraph_EdgeId(anOldIdx));
    occ::handle<Geom_Curve> aCurve = copyCurve(anEdgeSrcCurve2, theCopyGeom);

    const int aNewEdgeIdx = anIdx - 1;
    (void)aResult.Builder().AddEdgeDef(aNewStart,
                                       aNewEnd,
                                       aCurve,
                                       anEdge.ParamFirst,
                                       anEdge.ParamLast,
                                       anEdge.Tolerance);

    BRepGraph_MutRef<BRepGraph_TopoNode::EdgeDef> aNewEdge =
      aResult.MutEdge(BRepGraph_EdgeId(aNewEdgeIdx));
    aNewEdge->IsDegenerate  = anEdge.IsDegenerate;
    aNewEdge->SameParameter = anEdge.SameParameter;
    aNewEdge->SameRange     = anEdge.SameRange;
    transferUserAttributes(anEdge.Cache, aNewEdge->Cache);
  }

  // Add wires in deterministic order.
  for (int anIdx = 1; anIdx <= aWireSet.Extent(); ++anIdx)
  {
    const int                          anOldIdx = aWireSet.FindKey(anIdx);
    const BRepGraph_TopoNode::WireDef& aWire    = theGraph.Defs().Wire(BRepGraph_WireId(anOldIdx));
    NCollection_Vector<std::pair<BRepGraph_NodeId, TopAbs_Orientation>> aNewEntries;
    for (int aCoEdgeIter = 0; aCoEdgeIter < aWire.CoEdgeRefs.Length(); ++aCoEdgeIter)
    {
      const BRepGraphInc::CoEdgeRef&       aCR         = aWire.CoEdgeRefs.Value(aCoEdgeIter);
      const BRepGraph_TopoNode::CoEdgeDef& aCoEdge     = theGraph.Defs().CoEdge(aCR.CoEdgeDefId);
      const int*                           aNewEdgeIdx = anEdgeMap.Seek(aCoEdge.EdgeDefId.Index);
      if (aNewEdgeIdx == nullptr)
        continue;
      aNewEntries.Append(std::make_pair(BRepGraph_NodeId::Edge(*aNewEdgeIdx), aCoEdge.Sense));
    }
    (void)aResult.Builder().AddWireDef(aNewEntries);
    transferUserAttributes(aWire.Cache, aResult.MutWire(BRepGraph_WireId(anIdx - 1))->Cache);
  }

  // Add the face.
  const occ::handle<Geom_Surface>& aFaceSrcSurf2 = BRepGraph_Tool::Face::Surface(theGraph, theFace);
  occ::handle<Geom_Surface>        aSurf         = copySurface(aFaceSrcSurf2, theCopyGeom);

  BRepGraph_NodeId                     anOuterWire;
  NCollection_Vector<BRepGraph_NodeId> anInnerWires;

  for (int aWireRefIdx = 0; aWireRefIdx < aFaceDef.WireRefs.Length(); ++aWireRefIdx)
  {
    const BRepGraphInc::WireRef& aWR         = aFaceDef.WireRefs.Value(aWireRefIdx);
    const int*                   aNewWireIdx = aWireMap.Seek(aWR.WireDefId.Index);
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

  (void)aResult.Builder().AddFaceDef(aSurf, anOuterWire, anInnerWires, aFaceDef.Tolerance);
  BRepGraph_MutRef<BRepGraph_TopoNode::FaceDef> aNewFace = aResult.MutFace(BRepGraph_FaceId(0));
  aNewFace->NaturalRestriction                           = aFaceDef.NaturalRestriction;
  aNewFace->TriangulationRepIds                          = aFaceDef.TriangulationRepIds;
  aNewFace->ActiveTriangulationIndex                     = aFaceDef.ActiveTriangulationIndex;
  transferUserAttributes(aFaceDef.Cache, aNewFace->Cache);

  // PCurves for edges in this face via CoEdge data.
  const BRepGraphInc_ReverseIndex& aFaceRevIdx = theGraph.myData->myIncStorage.ReverseIndex();
  for (int anIdx = 1; anIdx <= anEdgeSet.Extent(); ++anIdx)
  {
    const int                                     anOldEdgeIdx = anEdgeSet.FindKey(anIdx);
    const int                                     aNewEdgeIdx  = anIdx - 1;
    const NCollection_Vector<BRepGraph_CoEdgeId>* aCoEdgeIdxs =
      aFaceRevIdx.CoEdgesOfEdge(BRepGraph_EdgeId(anOldEdgeIdx));
    if (aCoEdgeIdxs == nullptr)
      continue;
    for (int aCEIter = 0; aCEIter < aCoEdgeIdxs->Length(); ++aCEIter)
    {
      const BRepGraph_CoEdgeId             aCoEdgeId = aCoEdgeIdxs->Value(aCEIter);
      const BRepGraph_TopoNode::CoEdgeDef& aCoEdge   = theGraph.Defs().CoEdge(aCoEdgeId);
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

  return aResult;
}
