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
#include <BRepGraph_MutView.hxx>
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
//! Attributes are shared (same shared_ptr) — caller must deep-copy if needed.
void transferUserAttributes(const BRepGraph_NodeCache& theSrc,
                            BRepGraph_NodeCache&       theDst)
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

BRepGraph BRepGraphAlgo_Copy::Perform(const BRepGraph& theGraph,
                                      bool             theCopyGeom)
{
  BRepGraph aResult;
  if (!theGraph.IsDone())
    return aResult;

  // Bottom-up graph rebuild via BuilderView.
  // Since this is a full copy, old index == new index (identity mapping).

  // Vertices.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbVertices(); ++anIdx)
  {
    const BRepGraph_TopoNode::VertexDef& aVtx = theGraph.Defs().Vertex(anIdx);
    aResult.Builder().AddVertexDef(aVtx.Point, aVtx.Tolerance);
    transferUserAttributes(aVtx.Cache, aResult.Mut().VertexDef(anIdx)->Cache);
  }

  // Edges.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbEdges(); ++anIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = theGraph.Defs().Edge(anIdx);

    occ::handle<Geom_Curve> aCurve = copyCurve(anEdge.Curve3d, theCopyGeom);

    aResult.Builder().AddEdgeDef(anEdge.StartVertexDefId(),
                                 anEdge.EndVertexDefId(),
                                 aCurve,
                                 anEdge.ParamFirst,
                                 anEdge.ParamLast,
                                 anEdge.Tolerance);

    BRepGraph_MutRef<BRepGraph_TopoNode::EdgeDef> aNewEdge = aResult.MutEdge(anIdx);
    aNewEdge->IsDegenerate  = anEdge.IsDegenerate;
    aNewEdge->SameParameter = anEdge.SameParameter;
    aNewEdge->SameRange     = anEdge.SameRange;
    transferUserAttributes(anEdge.Cache, aNewEdge->Cache);
  }

  // Wires.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbWires(); ++anIdx)
  {
    const BRepGraph_TopoNode::WireDef& aWire = theGraph.Defs().Wire(anIdx);
    NCollection_Vector<std::pair<BRepGraph_NodeId, TopAbs_Orientation>> aWireEdges;
    for (int aCoEdgeIter = 0; aCoEdgeIter < aWire.CoEdgeRefs.Length(); ++aCoEdgeIter)
    {
      const BRepGraphInc::CoEdgeRef& aCR = aWire.CoEdgeRefs.Value(aCoEdgeIter);
      const BRepGraph_TopoNode::CoEdgeDef& aCoEdge = theGraph.Defs().CoEdge(aCR.CoEdgeIdx);
      aWireEdges.Append(std::make_pair(BRepGraph_NodeId::Edge(aCoEdge.EdgeIdx), aCoEdge.Sense));
    }
    aResult.Builder().AddWireDef(aWireEdges);
    transferUserAttributes(aWire.Cache, aResult.Mut().WireDef(anIdx)->Cache);
  }

  // Faces.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbFaces(); ++anIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFace = theGraph.Defs().Face(anIdx);

    occ::handle<Geom_Surface> aSurf = copySurface(aFace.Surface, theCopyGeom);

    // Get outer/inner wire def NodeIds from incidence refs.
    BRepGraph_NodeId                     anOuterWire;
    NCollection_Vector<BRepGraph_NodeId> anInnerWires;

    for (int aWireIdx = 0; aWireIdx < aFace.WireRefs.Length(); ++aWireIdx)
    {
      const BRepGraphInc::WireRef& aWR = aFace.WireRefs.Value(aWireIdx);
      const BRepGraph_NodeId aWireDefId = BRepGraph_NodeId::Wire(aWR.WireIdx);
      if (aWR.IsOuter)
      {
        anOuterWire = aWireDefId;
      }
      else
      {
        anInnerWires.Append(aWireDefId);
      }
    }

    aResult.Builder().AddFaceDef(aSurf, anOuterWire, anInnerWires, aFace.Tolerance);

    BRepGraph_MutRef<BRepGraph_TopoNode::FaceDef> aNewFace = aResult.MutFace(anIdx);
    aNewFace->NaturalRestriction          = aFace.NaturalRestriction;
    aNewFace->Triangulations              = aFace.Triangulations;
    aNewFace->ActiveTriangulationIndex    = aFace.ActiveTriangulationIndex;
    transferUserAttributes(aFace.Cache, aNewFace->Cache);
  }

  // PCurves (after edges and faces are created).
  for (int anIdx = 0; anIdx < theGraph.Defs().NbEdges(); ++anIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = theGraph.Defs().Edge(anIdx);
    for (int aPCIdx = 0; aPCIdx < anEdge.PCurves.Length(); ++aPCIdx)
    {
      const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry = anEdge.PCurves.Value(aPCIdx);
      if (aPCEntry.Curve2d.IsNull())
        continue;

      occ::handle<Geom2d_Curve> aNewPC = copyPCurve(aPCEntry.Curve2d, theCopyGeom);
      aResult.Mut().AddPCurveToEdge(BRepGraph_NodeId::Edge(anIdx),
                                    aPCEntry.FaceDefId,
                                    aNewPC,
                                    aPCEntry.ParamFirst,
                                    aPCEntry.ParamLast,
                                    aPCEntry.EdgeOrientation);
    }
  }

  // Shells.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbShells(); ++anIdx)
  {
    BRepGraph_NodeId aNewShellId = aResult.Builder().AddShellDef();

    const BRepGraph_TopoNode::ShellDef& aShell = theGraph.Defs().Shell(anIdx);
    transferUserAttributes(aShell.Cache, aResult.Mut().ShellDef(anIdx)->Cache);

    for (int aFaceRefIdx = 0; aFaceRefIdx < aShell.FaceRefs.Length(); ++aFaceRefIdx)
    {
      const BRepGraphInc::FaceRef& aFR = aShell.FaceRefs.Value(aFaceRefIdx);
      const BRepGraph_NodeId aFaceDefId = BRepGraph_NodeId::Face(aFR.FaceIdx);
      aResult.Builder().AddFaceToShell(aNewShellId, aFaceDefId, aFR.Orientation);
    }
  }

  // Solids.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbSolids(); ++anIdx)
  {
    BRepGraph_NodeId aNewSolidId = aResult.Builder().AddSolidDef();

    const BRepGraph_TopoNode::SolidDef& aSolid = theGraph.Defs().Solid(anIdx);
    transferUserAttributes(aSolid.Cache, aResult.Mut().SolidDef(anIdx)->Cache);

    for (int aShellRefIdx = 0; aShellRefIdx < aSolid.ShellRefs.Length(); ++aShellRefIdx)
    {
      const BRepGraphInc::ShellRef& aSR = aSolid.ShellRefs.Value(aShellRefIdx);
      const BRepGraph_NodeId aShellDefId = BRepGraph_NodeId::Shell(aSR.ShellIdx);
      aResult.Builder().AddShellToSolid(aNewSolidId, aShellDefId, aSR.Orientation);
    }
  }

  // Compounds.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbCompounds(); ++anIdx)
  {
    const BRepGraph_TopoNode::CompoundDef& aComp = theGraph.Defs().Compound(anIdx);
    NCollection_Vector<BRepGraph_NodeId> aChildNodeIds;
    for (int aCI = 0; aCI < aComp.ChildRefs.Length(); ++aCI)
    {
      const BRepGraphInc::ChildRef& aCR = aComp.ChildRefs.Value(aCI);
      aChildNodeIds.Append(BRepGraph_NodeId(static_cast<BRepGraph_NodeId::Kind>(aCR.Kind), aCR.ChildIdx));
    }
    aResult.Builder().AddCompoundDef(aChildNodeIds);
    transferUserAttributes(aComp.Cache, aResult.Mut().CompoundDef(anIdx)->Cache);
  }

  // CompSolids.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbCompSolids(); ++anIdx)
  {
    const BRepGraph_TopoNode::CompSolidDef& aCS = theGraph.Defs().CompSolid(anIdx);
    NCollection_Vector<BRepGraph_NodeId> aSolidNodeIds;
    for (int aSI = 0; aSI < aCS.SolidRefs.Length(); ++aSI)
      aSolidNodeIds.Append(BRepGraph_NodeId::Solid(aCS.SolidRefs.Value(aSI).SolidIdx));
    aResult.Builder().AddCompSolidDef(aSolidNodeIds);
    transferUserAttributes(aCS.Cache, aResult.Mut().CompSolidDef(anIdx)->Cache);
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

  copyUIDs(theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Vertex),    aResult.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Vertex));
  copyUIDs(theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Edge),      aResult.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Edge));
  copyUIDs(theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Wire),      aResult.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Wire));
  copyUIDs(theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Face),      aResult.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Face));
  copyUIDs(theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Shell),     aResult.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Shell));
  copyUIDs(theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Solid),     aResult.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Solid));
  copyUIDs(theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Compound),  aResult.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Compound));
  copyUIDs(theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::CompSolid), aResult.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::CompSolid));

  aResult.myData->myNextUIDCounter.store(
    theGraph.myData->myNextUIDCounter.load(std::memory_order_relaxed),
    std::memory_order_relaxed);
  aResult.myData->myGeneration = theGraph.myData->myGeneration;
  aResult.myData->myIsDone = true;

  return aResult;
}

//=================================================================================================

BRepGraph BRepGraphAlgo_Copy::CopyFace(const BRepGraph& theGraph,
                                       int              theFaceIdx,
                                       bool             theCopyGeom)
{
  BRepGraph aResult;
  if (!theGraph.IsDone() || theFaceIdx < 0 || theFaceIdx >= theGraph.Defs().NbFaces())
    return aResult;

  const BRepGraph_TopoNode::FaceDef& aFaceDef = theGraph.Defs().Face(theFaceIdx);
  if (aFaceDef.WireRefs.IsEmpty())
    return aResult;

  // Use NCollection_IndexedMap to collect old indices in deterministic insertion order.
  NCollection_IndexedMap<int> aVertexSet, anEdgeSet, aWireSet;

  // Collect all edges/vertices/wires from the face's wire refs.
  for (int aWireRefIdx = 0; aWireRefIdx < aFaceDef.WireRefs.Length(); ++aWireRefIdx)
  {
    const BRepGraphInc::WireRef& aWR = aFaceDef.WireRefs.Value(aWireRefIdx);
    const int aWireDefIdx = aWR.WireIdx;
    aWireSet.Add(aWireDefIdx);

    const BRepGraph_TopoNode::WireDef& aWireDef = theGraph.Defs().Wire(aWireDefIdx);
    for (int aCoEdgeIter = 0; aCoEdgeIter < aWireDef.CoEdgeRefs.Length(); ++aCoEdgeIter)
    {
      const BRepGraphInc::CoEdgeRef& aCR = aWireDef.CoEdgeRefs.Value(aCoEdgeIter);
      const BRepGraph_TopoNode::CoEdgeDef& aCoEdge = theGraph.Defs().CoEdge(aCR.CoEdgeIdx);
      if (aCoEdge.EdgeIdx < 0 || aCoEdge.EdgeIdx >= theGraph.Defs().NbEdges())
        continue;
      anEdgeSet.Add(aCoEdge.EdgeIdx);

      const BRepGraph_TopoNode::EdgeDef& anEdgeDef = theGraph.Defs().Edge(aCoEdge.EdgeIdx);
      if (anEdgeDef.StartVertexIdx >= 0
          && anEdgeDef.StartVertexIdx < theGraph.Defs().NbVertices())
        aVertexSet.Add(anEdgeDef.StartVertexIdx);
      if (anEdgeDef.EndVertexIdx >= 0
          && anEdgeDef.EndVertexIdx < theGraph.Defs().NbVertices())
        aVertexSet.Add(anEdgeDef.EndVertexIdx);
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
    const int anOldIdx = aVertexSet.FindKey(anIdx);
    const BRepGraph_TopoNode::VertexDef& aVtx = theGraph.Defs().Vertex(anOldIdx);
    aResult.Builder().AddVertexDef(aVtx.Point, aVtx.Tolerance);
    transferUserAttributes(aVtx.Cache, aResult.Mut().VertexDef(anIdx - 1)->Cache);
  }

  // Add edges in deterministic order.
  for (int anIdx = 1; anIdx <= anEdgeSet.Extent(); ++anIdx)
  {
    const int anOldIdx = anEdgeSet.FindKey(anIdx);
    const BRepGraph_TopoNode::EdgeDef& anEdge = theGraph.Defs().Edge(anOldIdx);

    BRepGraph_NodeId aNewStart, aNewEnd;
    if (anEdge.StartVertexIdx >= 0)
    {
      const int* aNewVtxIdx = aVertexMap.Seek(anEdge.StartVertexIdx);
      if (aNewVtxIdx != nullptr)
        aNewStart = BRepGraph_NodeId::Vertex(*aNewVtxIdx);
    }
    if (anEdge.EndVertexIdx >= 0)
    {
      const int* aNewVtxIdx = aVertexMap.Seek(anEdge.EndVertexIdx);
      if (aNewVtxIdx != nullptr)
        aNewEnd = BRepGraph_NodeId::Vertex(*aNewVtxIdx);
    }

    occ::handle<Geom_Curve> aCurve = copyCurve(anEdge.Curve3d, theCopyGeom);

    const int aNewEdgeIdx = anIdx - 1;
    aResult.Builder().AddEdgeDef(aNewStart, aNewEnd, aCurve,
                                 anEdge.ParamFirst, anEdge.ParamLast, anEdge.Tolerance);

    BRepGraph_MutRef<BRepGraph_TopoNode::EdgeDef> aNewEdge = aResult.MutEdge(aNewEdgeIdx);
    aNewEdge->IsDegenerate  = anEdge.IsDegenerate;
    aNewEdge->SameParameter = anEdge.SameParameter;
    aNewEdge->SameRange     = anEdge.SameRange;
    transferUserAttributes(anEdge.Cache, aNewEdge->Cache);
  }

  // Add wires in deterministic order.
  for (int anIdx = 1; anIdx <= aWireSet.Extent(); ++anIdx)
  {
    const int anOldIdx = aWireSet.FindKey(anIdx);
    const BRepGraph_TopoNode::WireDef& aWire = theGraph.Defs().Wire(anOldIdx);
    NCollection_Vector<std::pair<BRepGraph_NodeId, TopAbs_Orientation>> aNewEntries;
    for (int aCoEdgeIter = 0; aCoEdgeIter < aWire.CoEdgeRefs.Length(); ++aCoEdgeIter)
    {
      const BRepGraphInc::CoEdgeRef& aCR = aWire.CoEdgeRefs.Value(aCoEdgeIter);
      const BRepGraph_TopoNode::CoEdgeDef& aCoEdge = theGraph.Defs().CoEdge(aCR.CoEdgeIdx);
      const int* aNewEdgeIdx = anEdgeMap.Seek(aCoEdge.EdgeIdx);
      if (aNewEdgeIdx == nullptr)
        continue;
      aNewEntries.Append(
        std::make_pair(BRepGraph_NodeId::Edge(*aNewEdgeIdx), aCoEdge.Sense));
    }
    aResult.Builder().AddWireDef(aNewEntries);
    transferUserAttributes(aWire.Cache, aResult.Mut().WireDef(anIdx - 1)->Cache);
  }

  // Add the face.
  occ::handle<Geom_Surface> aSurf = copySurface(aFaceDef.Surface, theCopyGeom);

  BRepGraph_NodeId anOuterWire;
  NCollection_Vector<BRepGraph_NodeId> anInnerWires;

  for (int aWireRefIdx = 0; aWireRefIdx < aFaceDef.WireRefs.Length(); ++aWireRefIdx)
  {
    const BRepGraphInc::WireRef& aWR = aFaceDef.WireRefs.Value(aWireRefIdx);
    const int* aNewWireIdx = aWireMap.Seek(aWR.WireIdx);
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

  aResult.Builder().AddFaceDef(aSurf, anOuterWire, anInnerWires, aFaceDef.Tolerance);
  BRepGraph_MutRef<BRepGraph_TopoNode::FaceDef> aNewFace = aResult.MutFace(0);
  aNewFace->NaturalRestriction       = aFaceDef.NaturalRestriction;
  aNewFace->Triangulations           = aFaceDef.Triangulations;
  aNewFace->ActiveTriangulationIndex = aFaceDef.ActiveTriangulationIndex;
  transferUserAttributes(aFaceDef.Cache, aNewFace->Cache);

  // PCurves for edges in this face.
  for (int anIdx = 1; anIdx <= anEdgeSet.Extent(); ++anIdx)
  {
    const int anOldEdgeIdx = anEdgeSet.FindKey(anIdx);
    const int aNewEdgeIdx  = anIdx - 1;
    const BRepGraph_TopoNode::EdgeDef& anEdge = theGraph.Defs().Edge(anOldEdgeIdx);
    for (int aPCIdx = 0; aPCIdx < anEdge.PCurves.Length(); ++aPCIdx)
    {
      const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry = anEdge.PCurves.Value(aPCIdx);
      // Only copy PCurves belonging to this face.
      if (aPCEntry.FaceDefId.Index != theFaceIdx)
        continue;
      if (aPCEntry.Curve2d.IsNull())
        continue;

      occ::handle<Geom2d_Curve> aNewPC = copyPCurve(aPCEntry.Curve2d, theCopyGeom);
      aResult.Mut().AddPCurveToEdge(BRepGraph_NodeId::Edge(aNewEdgeIdx),
                                    BRepGraph_NodeId::Face(0),
                                    aNewPC,
                                    aPCEntry.ParamFirst,
                                    aPCEntry.ParamLast,
                                    aPCEntry.EdgeOrientation);
    }
  }

  aResult.myData->myIsDone = true;

  return aResult;
}
