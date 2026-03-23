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

#include <BRepGraphAlgo_Deduplicate.hxx>

#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_GeomView.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_MutView.hxx>

#include <GeomHash_CurveHasher.hxx>
#include <GeomHash_SurfaceHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_Vector.hxx>
#include <BRepGraph_PCurveContext.hxx>

//=================================================================================================

BRepGraphAlgo_Deduplicate::Result BRepGraphAlgo_Deduplicate::Perform(BRepGraph& theGraph)
{
  return Perform(theGraph, Options());
}

//=================================================================================================

BRepGraphAlgo_Deduplicate::Result BRepGraphAlgo_Deduplicate::Perform(BRepGraph&     theGraph,
                                                                     const Options& theOptions)
{
  Result aResult;
  if (!theGraph.IsDone())
  {
    return aResult;
  }

  const bool wasHistoryEnabled = theGraph.IsHistoryEnabled();
  theGraph.SetHistoryEnabled(theOptions.HistoryMode);

  GeomHash_SurfaceHasher aSurfHasher(theOptions.CompTolerance, theOptions.HashTolerance);
  GeomHash_CurveHasher   aCurveHasher(theOptions.CompTolerance, theOptions.HashTolerance);

  const occ::handle<NCollection_IncAllocator> aTmpAlloc = new NCollection_IncAllocator();

  NCollection_DataMap<occ::handle<Geom_Surface>, int, GeomHash_SurfaceHasher> aSurfToCanonical(
    aSurfHasher,
    std::max(1, theGraph.Geom().NbSurfaces() * 2),
    aTmpAlloc);
  NCollection_DataMap<occ::handle<Geom_Curve>, int, GeomHash_CurveHasher> aCurveToCanonical(
    aCurveHasher,
    std::max(1, theGraph.Geom().NbCurves() * 2),
    aTmpAlloc);
  NCollection_DataMap<int, int> aCanonicalBySurf(std::max(1, theGraph.Geom().NbSurfaces() * 2),
                                                 aTmpAlloc);
  NCollection_DataMap<int, int> aCanonicalByCurve(std::max(1, theGraph.Geom().NbCurves() * 2),
                                                  aTmpAlloc);
  NCollection_DataMap<int, int> aCanonicalByPCurve(std::max(1, theGraph.Geom().NbPCurves() * 2),
                                                   aTmpAlloc);

  NCollection_DataMap<BRepGraph_PCurveContext, NCollection_Vector<int>> aPCurveNodesByContext(
    std::max(1, theGraph.Defs().NbEdges() * 2),
    aTmpAlloc);

  for (int aSurfIter = 0; aSurfIter < theGraph.Geom().NbSurfaces(); ++aSurfIter)
  {
    const occ::handle<Geom_Surface>& aSurf = theGraph.Geom().Surface(aSurfIter).Surface;
    if (aSurf.IsNull())
    {
      continue;
    }

    const int* aCanonicalIdx = aSurfToCanonical.Seek(aSurf);
    if (aCanonicalIdx == nullptr)
    {
      aSurfToCanonical.Bind(aSurf, aSurfIter);
    }
    else if (*aCanonicalIdx != aSurfIter)
    {
      aCanonicalBySurf.Bind(aSurfIter, *aCanonicalIdx);
    }
  }

  for (int aCurveIter = 0; aCurveIter < theGraph.Geom().NbCurves(); ++aCurveIter)
  {
    const occ::handle<Geom_Curve>& aCurve = theGraph.Geom().Curve(aCurveIter).CurveGeom;
    if (aCurve.IsNull())
    {
      continue;
    }

    const int* aCanonicalIdx = aCurveToCanonical.Seek(aCurve);
    if (aCanonicalIdx == nullptr)
    {
      aCurveToCanonical.Bind(aCurve, aCurveIter);
    }
    else if (*aCanonicalIdx != aCurveIter)
    {
      aCanonicalByCurve.Bind(aCurveIter, *aCanonicalIdx);
    }
  }

  for (int anEdgeIdx = 0; anEdgeIdx < theGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = theGraph.Defs().Edge(anEdgeIdx);
    for (int aPCIter = 0; aPCIter < anEdgeDef.PCurves.Length(); ++aPCIter)
    {
      const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry = anEdgeDef.PCurves.Value(aPCIter);
      if (!aPCEntry.PCurveNodeId.IsValid())
      {
        continue;
      }

      const BRepGraph_GeomNode::PCurve& aPCNode =
        theGraph.Geom().PCurve(aPCEntry.PCurveNodeId.Index);
      if (aPCNode.Curve2d.IsNull())
      {
        continue;
      }

      const BRepGraph_PCurveContext aCtx(anEdgeIdx,
                                        aPCEntry.FaceDefId.Index,
                                        aPCEntry.EdgeOrientation);

      aPCurveNodesByContext.TryBind(aCtx, NCollection_Vector<int>());
      aPCurveNodesByContext.ChangeFind(aCtx).Append(aPCEntry.PCurveNodeId.Index);
    }
  }

  for (NCollection_DataMap<BRepGraph_PCurveContext, NCollection_Vector<int>>::Iterator aCtxIter(
         aPCurveNodesByContext);
       aCtxIter.More();
       aCtxIter.Next())
  {
    const NCollection_Vector<int>& aPCNodes = aCtxIter.Value();
    for (int aBaseIdx = 0; aBaseIdx < aPCNodes.Length(); ++aBaseIdx)
    {
      const int aCanonNodeIdx = aPCNodes.Value(aBaseIdx);
      if (aCanonicalByPCurve.IsBound(aCanonNodeIdx))
      {
        continue;
      }

      for (int aCandIdx = aBaseIdx + 1; aCandIdx < aPCNodes.Length(); ++aCandIdx)
      {
        const int aCandNodeIdx = aPCNodes.Value(aCandIdx);
        if (aCanonicalByPCurve.IsBound(aCandNodeIdx))
        {
          continue;
        }

        const BRepGraph_GeomNode::PCurve& aCandPC = theGraph.Geom().PCurve(aCandNodeIdx);
        if (aCandPC.Curve2d.IsNull())
        {
          continue;
        }

        // Context-strict policy: within the same (edge, face, orientation) context,
        // all PCurves are assumed geometrically identical (e.g. from AddPCurveToEdge
        // adding a copy). The first entry becomes canonical without deep geometry
        // comparison. If non-identical PCurves can appear in the same context,
        // add Geom2dHash_CurveHasher comparison here.
        aCanonicalByPCurve.Bind(aCandNodeIdx, aCanonNodeIdx);
      }
    }
  }

  aResult.NbCanonicalSurfaces = theGraph.Geom().NbSurfaces() - aCanonicalBySurf.Size();
  aResult.NbCanonicalCurves   = theGraph.Geom().NbCurves() - aCanonicalByCurve.Size();
  aResult.NbCanonicalPCurves  = theGraph.Geom().NbPCurves() - aCanonicalByPCurve.Size();

  if (theOptions.AnalyzeOnly)
  {
    theGraph.SetHistoryEnabled(wasHistoryEnabled);
    return aResult;
  }

  NCollection_Map<int> aRecordedSurfaceHistory(std::max(1, aCanonicalBySurf.Size() * 2), aTmpAlloc);
  NCollection_Map<int> aRecordedCurveHistory(std::max(1, aCanonicalByCurve.Size() * 2), aTmpAlloc);
  NCollection_Map<int> aRecordedPCurveHistory(std::max(1, aCanonicalByPCurve.Size() * 2),
                                              aTmpAlloc);

  for (int aFaceIdx = 0; aFaceIdx < theGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    BRepGraph_TopoNode::FaceDef& aFaceDef = theGraph.Mut().FaceDef(aFaceIdx);
    if (!aFaceDef.SurfNodeId.IsValid())
    {
      continue;
    }

    const int* aCanonSurf = aCanonicalBySurf.Seek(aFaceDef.SurfNodeId.Index);
    if (aCanonSurf == nullptr)
    {
      continue;
    }

    const BRepGraph_NodeId anOldSurfId = aFaceDef.SurfNodeId;
    aFaceDef.SurfNodeId = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Surface, *aCanonSurf);
    ++aResult.NbSurfaceRewrites;
    aResult.AffectedFaceDefs.Append(aFaceDef.Id);

    // Fix back-references: remove from old surface, add to canonical surface.
    {
      NCollection_Vector<BRepGraph_NodeId>& anOldUsers =
        theGraph.Mut().SurfNode(anOldSurfId.Index).FaceDefUsers;
      for (int anIdx = anOldUsers.Length() - 1; anIdx >= 0; --anIdx)
      {
        if (anOldUsers.Value(anIdx) == aFaceDef.Id)
        {
          if (anIdx < anOldUsers.Length() - 1)
            anOldUsers.ChangeValue(anIdx) = anOldUsers.Value(anOldUsers.Length() - 1);
          anOldUsers.EraseLast();
          break;
        }
      }
      theGraph.Mut().SurfNode(*aCanonSurf).FaceDefUsers.Append(aFaceDef.Id);
    }

    if (!aRecordedSurfaceHistory.Contains(anOldSurfId.Index))
    {
      NCollection_Vector<BRepGraph_NodeId> aRepl;
      aRepl.Append(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Surface, *aCanonSurf));
      theGraph.History().Record(TCollection_AsciiString("Dedup:CanonicalizeSurface"),
                                anOldSurfId,
                                aRepl);
      aRecordedSurfaceHistory.Add(anOldSurfId.Index);
      ++aResult.NbHistoryRecords;
    }
  }

  for (int anEdgeIdx = 0; anEdgeIdx < theGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    BRepGraph_TopoNode::EdgeDef& anEdgeDef = theGraph.Mut().EdgeDef(anEdgeIdx);
    if (anEdgeDef.CurveNodeId.IsValid())
    {
      const int* aCanonCurve = aCanonicalByCurve.Seek(anEdgeDef.CurveNodeId.Index);
      if (aCanonCurve != nullptr)
      {
        const BRepGraph_NodeId anOldCurveId = anEdgeDef.CurveNodeId;
        anEdgeDef.CurveNodeId = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Curve, *aCanonCurve);
        ++aResult.NbCurveRewrites;
        aResult.AffectedEdgeDefs.Append(anEdgeDef.Id);

        // Fix back-references: remove from old curve, add to canonical curve.
        {
          NCollection_Vector<BRepGraph_NodeId>& anOldUsers =
            theGraph.Mut().CurveNode(anOldCurveId.Index).EdgeDefUsers;
          for (int anIdx = anOldUsers.Length() - 1; anIdx >= 0; --anIdx)
          {
            if (anOldUsers.Value(anIdx) == anEdgeDef.Id)
            {
              if (anIdx < anOldUsers.Length() - 1)
                anOldUsers.ChangeValue(anIdx) = anOldUsers.Value(anOldUsers.Length() - 1);
              anOldUsers.EraseLast();
              break;
            }
          }
          theGraph.Mut().CurveNode(*aCanonCurve).EdgeDefUsers.Append(anEdgeDef.Id);
        }

        if (!aRecordedCurveHistory.Contains(anOldCurveId.Index))
        {
          NCollection_Vector<BRepGraph_NodeId> aRepl;
          aRepl.Append(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Curve, *aCanonCurve));
          theGraph.History().Record(TCollection_AsciiString("Dedup:CanonicalizeCurve"),
                                    anOldCurveId,
                                    aRepl);
          aRecordedCurveHistory.Add(anOldCurveId.Index);
          ++aResult.NbHistoryRecords;
        }
      }
    }

    for (int aPCIter = 0; aPCIter < anEdgeDef.PCurves.Length(); ++aPCIter)
    {
      BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry = anEdgeDef.PCurves.ChangeValue(aPCIter);
      if (!aPCEntry.PCurveNodeId.IsValid())
      {
        continue;
      }

      const int* aCanonPCurve = aCanonicalByPCurve.Seek(aPCEntry.PCurveNodeId.Index);
      if (aCanonPCurve == nullptr)
      {
        continue;
      }

      const BRepGraph_NodeId anOldPCurveId = aPCEntry.PCurveNodeId;
      aPCEntry.PCurveNodeId = BRepGraph_NodeId(BRepGraph_NodeId::Kind::PCurve, *aCanonPCurve);
      ++aResult.NbPCurveRewrites;

      if (!aRecordedPCurveHistory.Contains(anOldPCurveId.Index))
      {
        NCollection_Vector<BRepGraph_NodeId> aRepl;
        aRepl.Append(BRepGraph_NodeId(BRepGraph_NodeId::Kind::PCurve, *aCanonPCurve));
        theGraph.History().Record(TCollection_AsciiString("Dedup:CanonicalizePCurve"),
                                  anOldPCurveId,
                                  aRepl);
        aRecordedPCurveHistory.Add(anOldPCurveId.Index);
        ++aResult.NbHistoryRecords;
      }
    }
  }

  // Nullify orphaned geometry handles to free memory.
  for (int aSurfIdx = 0; aSurfIdx < theGraph.Geom().NbSurfaces(); ++aSurfIdx)
  {
    BRepGraph_GeomNode::Surf& aSurfNode = theGraph.Mut().SurfNode(aSurfIdx);
    if (aSurfNode.FaceDefUsers.IsEmpty() && !aSurfNode.Surface.IsNull())
    {
      aSurfNode.Surface.Nullify();
      aSurfNode.Triangulation.Nullify();
      ++aResult.NbNullifiedSurfaces;

      NCollection_Vector<BRepGraph_NodeId> aEmptyRepl;
      theGraph.History().Record(TCollection_AsciiString("Dedup:NullifyOrphanSurface"),
                                aSurfNode.Id,
                                aEmptyRepl);
      ++aResult.NbHistoryRecords;
    }
  }

  for (int aCurveIdx = 0; aCurveIdx < theGraph.Geom().NbCurves(); ++aCurveIdx)
  {
    BRepGraph_GeomNode::Curve& aCurveNode = theGraph.Mut().CurveNode(aCurveIdx);
    if (aCurveNode.EdgeDefUsers.IsEmpty() && !aCurveNode.CurveGeom.IsNull())
    {
      aCurveNode.CurveGeom.Nullify();
      ++aResult.NbNullifiedCurves;

      NCollection_Vector<BRepGraph_NodeId> aEmptyRepl;
      theGraph.History().Record(TCollection_AsciiString("Dedup:NullifyOrphanCurve"),
                                aCurveNode.Id,
                                aEmptyRepl);
      ++aResult.NbHistoryRecords;
    }
  }

  {
    NCollection_Map<int> aReferencedPCurves(std::max(1, theGraph.Geom().NbPCurves() * 2), aTmpAlloc);
    for (int anEdgeIdx = 0; anEdgeIdx < theGraph.Defs().NbEdges(); ++anEdgeIdx)
    {
      const BRepGraph_TopoNode::EdgeDef& anEdgeDef = theGraph.Defs().Edge(anEdgeIdx);
      for (int aPCIter = 0; aPCIter < anEdgeDef.PCurves.Length(); ++aPCIter)
      {
        const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry =
          anEdgeDef.PCurves.Value(aPCIter);
        if (aPCEntry.PCurveNodeId.IsValid())
        {
          aReferencedPCurves.Add(aPCEntry.PCurveNodeId.Index);
        }
      }
    }

    for (int aPCIdx = 0; aPCIdx < theGraph.Geom().NbPCurves(); ++aPCIdx)
    {
      if (aReferencedPCurves.Contains(aPCIdx))
      {
        continue;
      }

      BRepGraph_GeomNode::PCurve& aPCNode = theGraph.Mut().PCurveNode(aPCIdx);
      if (!aPCNode.Curve2d.IsNull())
      {
        aPCNode.Curve2d.Nullify();
        ++aResult.NbNullifiedPCurves;

        NCollection_Vector<BRepGraph_NodeId> aEmptyRepl;
        theGraph.History().Record(TCollection_AsciiString("Dedup:NullifyOrphanPCurve"),
                                  aPCNode.Id,
                                  aEmptyRepl);
        ++aResult.NbHistoryRecords;
      }
    }
  }

  // Definition merge phase is reserved for the next implementation step.
  aResult.IsDefMergeApplied = false;
  (void)theOptions.MergeDefsWhenSafe;

  theGraph.SetHistoryEnabled(wasHistoryEnabled);
  return aResult;
}
