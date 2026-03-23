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
#include <BRepGraph_MutView.hxx>
#include <BRepGraph_UsagesView.hxx>

#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_IndexedMap.hxx>

namespace
{

//! Copy geometry handle if theCopyGeom is true, otherwise return same handle.
Handle(Geom_Surface) copySurface(const Handle(Geom_Surface)& theSurf, bool theCopyGeom)
{
  if (theSurf.IsNull() || !theCopyGeom)
    return theSurf;
  return Handle(Geom_Surface)::DownCast(theSurf->Copy());
}

Handle(Geom_Curve) copyCurve(const Handle(Geom_Curve)& theCrv, bool theCopyGeom)
{
  if (theCrv.IsNull() || !theCopyGeom)
    return theCrv;
  return Handle(Geom_Curve)::DownCast(theCrv->Copy());
}

Handle(Geom2d_Curve) copyPCurve(const Handle(Geom2d_Curve)& theCrv, bool theCopyGeom)
{
  if (theCrv.IsNull() || !theCopyGeom)
    return theCrv;
  return Handle(Geom2d_Curve)::DownCast(theCrv->Copy());
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
    BRepGraph_UserAttrPtr anAttr = theSrc.GetUserAttribute(aKeys.Value(anIdx));
    if (anAttr)
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
    transferUserAttributes(aVtx.Cache, aResult.Mut().VertexDef(anIdx).Cache);
  }

  // Edges.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbEdges(); ++anIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = theGraph.Defs().Edge(anIdx);

    Handle(Geom_Curve) aCurve = copyCurve(anEdge.Curve3d, theCopyGeom);

    aResult.Builder().AddEdgeDef(anEdge.StartVertexDefId,
                                 anEdge.EndVertexDefId,
                                 aCurve,
                                 anEdge.ParamFirst,
                                 anEdge.ParamLast,
                                 anEdge.Tolerance);

    BRepGraph_TopoNode::EdgeDef& aNewEdge = aResult.Mut().EdgeDef(anIdx);
    aNewEdge.IsDegenerate  = anEdge.IsDegenerate;
    aNewEdge.SameParameter = anEdge.SameParameter;
    aNewEdge.SameRange     = anEdge.SameRange;
    transferUserAttributes(anEdge.Cache, aNewEdge.Cache);
  }

  // Wires.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbWires(); ++anIdx)
  {
    const BRepGraph_TopoNode::WireDef& aWire = theGraph.Defs().Wire(anIdx);
    aResult.Builder().AddWireDef(aWire.OrderedEdges);
    transferUserAttributes(aWire.Cache, aResult.Mut().WireDef(anIdx).Cache);
  }

  // Faces.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbFaces(); ++anIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFace = theGraph.Defs().Face(anIdx);

    Handle(Geom_Surface) aSurf = copySurface(aFace.Surface, theCopyGeom);

    // Get outer/inner wire def NodeIds from the first usage.
    BRepGraph_NodeId                     anOuterWire;
    NCollection_Vector<BRepGraph_NodeId> anInnerWires;

    if (!aFace.Usages.IsEmpty())
    {
      const BRepGraph_UsageId& aFirstUsage = aFace.Usages.Value(0);
      if (aFirstUsage.IsValid() && aFirstUsage.Index < theGraph.Usages().NbFaces())
      {
        const BRepGraph_TopoNode::FaceUsage& aFaceUs = theGraph.Usages().Face(aFirstUsage.Index);
        if (aFaceUs.OuterWireUsage.IsValid()
            && aFaceUs.OuterWireUsage.Index < theGraph.Usages().NbWires())
        {
          anOuterWire = theGraph.Usages().Wire(aFaceUs.OuterWireUsage.Index).DefId;
        }
        for (int aWireIdx = 0; aWireIdx < aFaceUs.InnerWireUsages.Length(); ++aWireIdx)
        {
          const BRepGraph_UsageId& aInnerUsId = aFaceUs.InnerWireUsages.Value(aWireIdx);
          if (aInnerUsId.IsValid() && aInnerUsId.Index < theGraph.Usages().NbWires())
          {
            anInnerWires.Append(theGraph.Usages().Wire(aInnerUsId.Index).DefId);
          }
        }
      }
    }

    aResult.Builder().AddFaceDef(aSurf, anOuterWire, anInnerWires, aFace.Tolerance);

    BRepGraph_TopoNode::FaceDef& aNewFace = aResult.Mut().FaceDef(anIdx);
    aNewFace.NaturalRestriction          = aFace.NaturalRestriction;
    aNewFace.Triangulations              = aFace.Triangulations;
    aNewFace.ActiveTriangulationIndex    = aFace.ActiveTriangulationIndex;
    transferUserAttributes(aFace.Cache, aNewFace.Cache);
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

      Handle(Geom2d_Curve) aNewPC = copyPCurve(aPCEntry.Curve2d, theCopyGeom);
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
    transferUserAttributes(aShell.Cache, aResult.Mut().ShellDef(anIdx).Cache);

    if (!aShell.Usages.IsEmpty())
    {
      const BRepGraph_UsageId& aFirstUsage = aShell.Usages.Value(0);
      if (aFirstUsage.IsValid() && aFirstUsage.Index < theGraph.Usages().NbShells())
      {
        const BRepGraph_TopoNode::ShellUsage& aShellUs =
          theGraph.Usages().Shell(aFirstUsage.Index);
        for (int aFaceUsIdx = 0; aFaceUsIdx < aShellUs.FaceUsages.Length(); ++aFaceUsIdx)
        {
          const BRepGraph_UsageId& aFaceUsId = aShellUs.FaceUsages.Value(aFaceUsIdx);
          if (!aFaceUsId.IsValid() || aFaceUsId.Index >= theGraph.Usages().NbFaces())
            continue;
          const BRepGraph_TopoNode::FaceUsage& aFaceUs = theGraph.Usages().Face(aFaceUsId.Index);
          if (!aFaceUs.DefId.IsValid())
            continue;
          aResult.Builder().AddFaceToShell(aNewShellId, aFaceUs.DefId, aFaceUs.Orientation);
        }
      }
    }
  }

  // Solids.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbSolids(); ++anIdx)
  {
    BRepGraph_NodeId aNewSolidId = aResult.Builder().AddSolidDef();

    const BRepGraph_TopoNode::SolidDef& aSolid = theGraph.Defs().Solid(anIdx);
    transferUserAttributes(aSolid.Cache, aResult.Mut().SolidDef(anIdx).Cache);

    if (!aSolid.Usages.IsEmpty())
    {
      const BRepGraph_UsageId& aFirstUsage = aSolid.Usages.Value(0);
      if (aFirstUsage.IsValid() && aFirstUsage.Index < theGraph.Usages().NbSolids())
      {
        const BRepGraph_TopoNode::SolidUsage& aSolidUs =
          theGraph.Usages().Solid(aFirstUsage.Index);
        for (int aShellUsIdx = 0; aShellUsIdx < aSolidUs.ShellUsages.Length(); ++aShellUsIdx)
        {
          const BRepGraph_UsageId& aShellUsId = aSolidUs.ShellUsages.Value(aShellUsIdx);
          if (!aShellUsId.IsValid() || aShellUsId.Index >= theGraph.Usages().NbShells())
            continue;
          const BRepGraph_TopoNode::ShellUsage& aShellUs =
            theGraph.Usages().Shell(aShellUsId.Index);
          if (!aShellUs.DefId.IsValid())
            continue;
          aResult.Builder().AddShellToSolid(aNewSolidId, aShellUs.DefId, aShellUs.Orientation);
        }
      }
    }
  }

  // Compounds.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbCompounds(); ++anIdx)
  {
    const BRepGraph_TopoNode::CompoundDef& aComp = theGraph.Defs().Compound(anIdx);
    aResult.Builder().AddCompoundDef(aComp.ChildDefIds);
    transferUserAttributes(aComp.Cache, aResult.Mut().CompoundDef(anIdx).Cache);
  }

  // CompSolids.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbCompSolids(); ++anIdx)
  {
    const BRepGraph_TopoNode::CompSolidDef& aCS = theGraph.Defs().CompSolid(anIdx);
    aResult.Builder().AddCompSolidDef(aCS.SolidDefIds);
    transferUserAttributes(aCS.Cache, aResult.Mut().CompSolidDef(anIdx).Cache);
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

  copyUIDs(theGraph.myData->myVertices.UIDs,    aResult.myData->myVertices.UIDs);
  copyUIDs(theGraph.myData->myEdges.UIDs,       aResult.myData->myEdges.UIDs);
  copyUIDs(theGraph.myData->myWires.UIDs,       aResult.myData->myWires.UIDs);
  copyUIDs(theGraph.myData->myFaces.UIDs,       aResult.myData->myFaces.UIDs);
  copyUIDs(theGraph.myData->myShells.UIDs,      aResult.myData->myShells.UIDs);
  copyUIDs(theGraph.myData->mySolids.UIDs,      aResult.myData->mySolids.UIDs);
  copyUIDs(theGraph.myData->myCompounds.UIDs,   aResult.myData->myCompounds.UIDs);
  copyUIDs(theGraph.myData->myCompSolids.UIDs,  aResult.myData->myCompSolids.UIDs);

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
  if (aFaceDef.Usages.IsEmpty())
    return aResult;

  const int aFaceUsageIdx = aFaceDef.Usages.First().Index;
  if (aFaceUsageIdx >= theGraph.Usages().NbFaces())
    return aResult;
  const BRepGraph_TopoNode::FaceUsage& aFaceUsage = theGraph.Usages().Face(aFaceUsageIdx);

  // Use NCollection_IndexedMap to collect old indices in deterministic insertion order.
  NCollection_IndexedMap<int> aVertexSet, anEdgeSet, aWireSet;

  // Collect all edges/vertices/wires from the face's wires.
  auto collectFromWire = [&](BRepGraph_UsageId theWireUsageId) {
    if (!theWireUsageId.IsValid() || theWireUsageId.Index >= theGraph.Usages().NbWires())
      return;
    const BRepGraph_TopoNode::WireUsage& aWireUs = theGraph.Usages().Wire(theWireUsageId.Index);
    const int aWireDefIdx = aWireUs.DefId.Index;
    aWireSet.Add(aWireDefIdx);

    if (aWireDefIdx >= theGraph.Defs().NbWires())
      return;
    const BRepGraph_TopoNode::WireDef& aWireDef = theGraph.Defs().Wire(aWireDefIdx);
    for (int anEdgeIter = 0; anEdgeIter < aWireDef.OrderedEdges.Length(); ++anEdgeIter)
    {
      const BRepGraph_TopoNode::WireDef::EdgeEntry& anEntry =
        aWireDef.OrderedEdges.Value(anEdgeIter);
      if (!anEntry.EdgeDefId.IsValid() || anEntry.EdgeDefId.Index >= theGraph.Defs().NbEdges())
        continue;
      anEdgeSet.Add(anEntry.EdgeDefId.Index);

      const BRepGraph_TopoNode::EdgeDef& anEdgeDef = theGraph.Defs().Edge(anEntry.EdgeDefId.Index);
      if (anEdgeDef.StartVertexDefId.IsValid()
          && anEdgeDef.StartVertexDefId.Index < theGraph.Defs().NbVertices())
        aVertexSet.Add(anEdgeDef.StartVertexDefId.Index);
      if (anEdgeDef.EndVertexDefId.IsValid()
          && anEdgeDef.EndVertexDefId.Index < theGraph.Defs().NbVertices())
        aVertexSet.Add(anEdgeDef.EndVertexDefId.Index);
    }
  };

  collectFromWire(aFaceUsage.OuterWireUsage);
  for (int aWireIdx = 0; aWireIdx < aFaceUsage.InnerWireUsages.Length(); ++aWireIdx)
    collectFromWire(aFaceUsage.InnerWireUsages.Value(aWireIdx));

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
    transferUserAttributes(aVtx.Cache, aResult.Mut().VertexDef(anIdx - 1).Cache);
  }

  // Add edges in deterministic order.
  for (int anIdx = 1; anIdx <= anEdgeSet.Extent(); ++anIdx)
  {
    const int anOldIdx = anEdgeSet.FindKey(anIdx);
    const BRepGraph_TopoNode::EdgeDef& anEdge = theGraph.Defs().Edge(anOldIdx);

    BRepGraph_NodeId aNewStart, aNewEnd;
    if (anEdge.StartVertexDefId.IsValid())
    {
      const int* aNewVtxIdx = aVertexMap.Seek(anEdge.StartVertexDefId.Index);
      if (aNewVtxIdx != nullptr)
        aNewStart = BRepGraph_NodeId::Vertex(*aNewVtxIdx);
    }
    if (anEdge.EndVertexDefId.IsValid())
    {
      const int* aNewVtxIdx = aVertexMap.Seek(anEdge.EndVertexDefId.Index);
      if (aNewVtxIdx != nullptr)
        aNewEnd = BRepGraph_NodeId::Vertex(*aNewVtxIdx);
    }

    Handle(Geom_Curve) aCurve = copyCurve(anEdge.Curve3d, theCopyGeom);

    const int aNewEdgeIdx = anIdx - 1;
    aResult.Builder().AddEdgeDef(aNewStart, aNewEnd, aCurve,
                                 anEdge.ParamFirst, anEdge.ParamLast, anEdge.Tolerance);

    BRepGraph_TopoNode::EdgeDef& aNewEdge = aResult.Mut().EdgeDef(aNewEdgeIdx);
    aNewEdge.IsDegenerate  = anEdge.IsDegenerate;
    aNewEdge.SameParameter = anEdge.SameParameter;
    aNewEdge.SameRange     = anEdge.SameRange;
    transferUserAttributes(anEdge.Cache, aNewEdge.Cache);
  }

  // Add wires in deterministic order.
  for (int anIdx = 1; anIdx <= aWireSet.Extent(); ++anIdx)
  {
    const int anOldIdx = aWireSet.FindKey(anIdx);
    const BRepGraph_TopoNode::WireDef& aWire = theGraph.Defs().Wire(anOldIdx);
    NCollection_Vector<BRepGraph_TopoNode::WireDef::EdgeEntry> aNewEntries;
    for (int anEntryIdx = 0; anEntryIdx < aWire.OrderedEdges.Length(); ++anEntryIdx)
    {
      const BRepGraph_TopoNode::WireDef::EdgeEntry& anOldEntry =
        aWire.OrderedEdges.Value(anEntryIdx);
      const int* aNewEdgeIdx = anEdgeMap.Seek(anOldEntry.EdgeDefId.Index);
      if (aNewEdgeIdx == nullptr)
        continue;
      BRepGraph_TopoNode::WireDef::EdgeEntry aNewEntry;
      aNewEntry.EdgeDefId         = BRepGraph_NodeId::Edge(*aNewEdgeIdx);
      aNewEntry.OrientationInWire = anOldEntry.OrientationInWire;
      aNewEntries.Append(aNewEntry);
    }
    aResult.Builder().AddWireDef(aNewEntries);
    transferUserAttributes(aWire.Cache, aResult.Mut().WireDef(anIdx - 1).Cache);
  }

  // Add the face.
  Handle(Geom_Surface) aSurf = copySurface(aFaceDef.Surface, theCopyGeom);

  BRepGraph_NodeId anOuterWire;
  NCollection_Vector<BRepGraph_NodeId> anInnerWires;

  if (aFaceUsage.OuterWireUsage.IsValid()
      && aFaceUsage.OuterWireUsage.Index < theGraph.Usages().NbWires())
  {
    const int anOldWireDefIdx =
      theGraph.Usages().Wire(aFaceUsage.OuterWireUsage.Index).DefId.Index;
    const int* aNewWireIdx = aWireMap.Seek(anOldWireDefIdx);
    if (aNewWireIdx != nullptr)
      anOuterWire = BRepGraph_NodeId::Wire(*aNewWireIdx);
  }
  for (int aWireIdx = 0; aWireIdx < aFaceUsage.InnerWireUsages.Length(); ++aWireIdx)
  {
    const BRepGraph_UsageId& aInnerUsId = aFaceUsage.InnerWireUsages.Value(aWireIdx);
    if (!aInnerUsId.IsValid() || aInnerUsId.Index >= theGraph.Usages().NbWires())
      continue;
    const int anOldWireDefIdx = theGraph.Usages().Wire(aInnerUsId.Index).DefId.Index;
    const int* aNewWireIdx = aWireMap.Seek(anOldWireDefIdx);
    if (aNewWireIdx != nullptr)
      anInnerWires.Append(BRepGraph_NodeId::Wire(*aNewWireIdx));
  }

  aResult.Builder().AddFaceDef(aSurf, anOuterWire, anInnerWires, aFaceDef.Tolerance);
  BRepGraph_TopoNode::FaceDef& aNewFace = aResult.Mut().FaceDef(0);
  aNewFace.NaturalRestriction       = aFaceDef.NaturalRestriction;
  aNewFace.Triangulations           = aFaceDef.Triangulations;
  aNewFace.ActiveTriangulationIndex = aFaceDef.ActiveTriangulationIndex;
  transferUserAttributes(aFaceDef.Cache, aNewFace.Cache);

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

      Handle(Geom2d_Curve) aNewPC = copyPCurve(aPCEntry.Curve2d, theCopyGeom);
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
