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

#include <BRepGraph_Analyze.hxx>

#include <BRepGraph.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_RelEdgesView.hxx>
#include <BRepGraph_AnalyzeView.hxx>
#include <BRepGraphInc_IncidenceRef.hxx>

#include <Geom_Curve.hxx>

#include <ExtremaPC_Curve.hxx>
#include <GCPnts_UniformAbscissa.hxx>
#include <GeomAdaptor_TransformedCurve.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_LocalArray.hxx>
#include <NCollection_Map.hxx>
#include <OSD_Parallel.hxx>
#include <Precision.hxx>

#include <functional>

//==================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph_Analyze::FreeEdges(const BRepGraph& theGraph)
{
  NCollection_Vector<BRepGraph_NodeId> aResult;

  const BRepGraph::DefsView aDefs = theGraph.Defs();

  for (int anEdgeIdx = 0; anEdgeIdx < aDefs.NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = aDefs.Edge(anEdgeIdx);
    if (anEdge.IsRemoved || anEdge.IsDegenerate)
      continue;

    if (aDefs.FaceCountOfEdge(anEdgeIdx) == 1)
      aResult.Append(anEdge.Id);
  }
  return aResult;
}

//==================================================================================================

NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>>
  BRepGraph_Analyze::MissingPCurves(const BRepGraph& theGraph)
{
  NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>> aResult;

  const BRepGraph::DefsView aDefs = theGraph.Defs();

  for (int aFaceDefIdx = 0; aFaceDefIdx < aDefs.NbFaces(); ++aFaceDefIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFaceDef = aDefs.Face(aFaceDefIdx);
    const BRepGraph_NodeId             aFaceDefId = aFaceDef.Id;

    for (int aWireRefIdx = 0; aWireRefIdx < aFaceDef.WireRefs.Length(); ++aWireRefIdx)
    {
      const int aWireDefIdx = aFaceDef.WireRefs.Value(aWireRefIdx).WireIdx;
      const BRepGraph_TopoNode::WireDef& aWireDef = aDefs.Wire(aWireDefIdx);
      for (int anEdgeIdx = 0; anEdgeIdx < aWireDef.EdgeRefs.Length(); ++anEdgeIdx)
      {
        const int                        anEdgeDefIdx = aWireDef.EdgeRefs.Value(anEdgeIdx).EdgeIdx;
        const BRepGraph_TopoNode::EdgeDef& anEdge     = aDefs.Edge(anEdgeDefIdx);
        const BRepGraph_NodeId             anEdgeDefId = anEdge.Id;

        if (anEdge.IsDegenerate)
          continue;

        const BRepGraph_TopoNode::EdgeDef::PCurveEntry* aPCurve = aDefs.FindPCurve(anEdgeDefId, aFaceDefId);
        if (aPCurve == nullptr)
          aResult.Append(std::make_pair(anEdgeDefId, aFaceDefId));
      }
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph_Analyze::ToleranceConflicts(
  const BRepGraph& theGraph,
  double           theThreshold)
{
  NCollection_Vector<BRepGraph_NodeId> aResult;

  // Build temporary curve pointer -> edges map by scanning edge defs
  NCollection_DataMap<const Geom_Curve*, NCollection_Vector<BRepGraph_NodeId>> aCurveToEdges;
  for (int anEdgeIdx = 0; anEdgeIdx < theGraph.myData->myIncStorage.NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = theGraph.myData->myIncStorage.Edge(anEdgeIdx);
    if (anEdgeDef.Curve3d.IsNull())
      continue;
    const Geom_Curve* aCurveKey = anEdgeDef.Curve3d.get();
    aCurveToEdges.TryBind(aCurveKey, NCollection_Vector<BRepGraph_NodeId>());
    aCurveToEdges.ChangeFind(aCurveKey).Append(anEdgeDef.Id);
  }

  for (NCollection_DataMap<const Geom_Curve*, NCollection_Vector<BRepGraph_NodeId>>::Iterator anIter(aCurveToEdges);
       anIter.More(); anIter.Next())
  {
    const NCollection_Vector<BRepGraph_NodeId>& anEdges = anIter.Value();
    if (anEdges.Length() <= 1)
      continue;

    double aMinTol = 1.0e100;
    double aMaxTol = -1.0;
    for (int anIdx = 0; anIdx < anEdges.Length(); ++anIdx)
    {
      const double aTol = theGraph.myData->myIncStorage.Edge(anEdges.Value(anIdx).Index).Tolerance;
      if (aTol < aMinTol) aMinTol = aTol;
      if (aTol > aMaxTol) aMaxTol = aTol;
    }

    if (aMaxTol - aMinTol > theThreshold)
    {
      for (int anIdx = 0; anIdx < anEdges.Length(); ++anIdx)
        aResult.Append(anEdges.Value(anIdx));
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph_Analyze::DegenerateWires(const BRepGraph& theGraph)
{
  NCollection_Vector<BRepGraph_NodeId> aResult;

  const BRepGraph::DefsView aDefs = theGraph.Defs();

  for (int aWireDefIdx = 0; aWireDefIdx < aDefs.NbWires(); ++aWireDefIdx)
  {
    const BRepGraph_TopoNode::WireDef& aWire = aDefs.Wire(aWireDefIdx);

    // Wire with < 2 edges (check via incidence EdgeRefs).
    const int aNbEdgesInWire = aWire.EdgeRefs.Length();
    if (aNbEdgesInWire < 2)
    {
      aResult.Append(aWire.Id);
      continue;
    }

    // Outer wire that is not closed (check via incidence IsClosed).
    const bool aIsClosed = aWire.IsClosed;
    if (!aIsClosed)
    {
      // Check if this wire is the outer wire of any face (via reverse index).
      auto isOuterWireOfAnyFace = [&]() -> bool {
        const NCollection_Vector<int>* aFaces =
          theGraph.myData->myIncStorage.ReverseIndex().FacesOfWire(aWireDefIdx);
        if (aFaces == nullptr)
          return false;
        for (int aFIdx = 0; aFIdx < aFaces->Length(); ++aFIdx)
        {
          const BRepGraph_TopoNode::FaceDef& aFaceDef = aDefs.Face(aFaces->Value(aFIdx));
          for (int aWRIdx = 0; aWRIdx < aFaceDef.WireRefs.Length(); ++aWRIdx)
          {
            const BRepGraphInc::WireRef& aWR = aFaceDef.WireRefs.Value(aWRIdx);
            if (aWR.WireIdx == aWireDefIdx && aWR.IsOuter)
              return true;
          }
        }
        return false;
      };
      if (isOuterWireOfAnyFace())
        aResult.Append(aWire.Id);
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Vector<BRepGraph_SubGraph> BRepGraph_Analyze::Decompose(const BRepGraph& theGraph)
{
  NCollection_Vector<BRepGraph_SubGraph> aResult;

  const BRepGraph::DefsView aDefs = theGraph.Defs();

  // Collect wire, edge and vertex children from a face def into a SubGraph.
  auto collectFaceChildren = [&](BRepGraph_SubGraph& theSub, int theFaceDefIdx) {
    const BRepGraph_TopoNode::FaceDef& aFaceDef = aDefs.Face(theFaceDefIdx);

    for (int aWireRefIdx = 0; aWireRefIdx < aFaceDef.WireRefs.Length(); ++aWireRefIdx)
    {
      const int aWireDefIdx = aFaceDef.WireRefs.Value(aWireRefIdx).WireIdx;
      const BRepGraph_TopoNode::WireDef& aWireDef = aDefs.Wire(aWireDefIdx);
      theSub.myWireDefIndices.Append(aWireDefIdx);

      for (int anEdgeIdx = 0; anEdgeIdx < aWireDef.EdgeRefs.Length(); ++anEdgeIdx)
      {
        const int anEdgeDefIdx = aWireDef.EdgeRefs.Value(anEdgeIdx).EdgeIdx;
        theSub.myEdgeDefIndices.Append(anEdgeDefIdx);

        const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aDefs.Edge(anEdgeDefIdx);
        if (anEdgeDef.StartVertexIdx >= 0)
          theSub.myVertexDefIndices.Append(anEdgeDef.StartVertexIdx);
        if (anEdgeDef.EndVertexIdx >= 0)
          theSub.myVertexDefIndices.Append(anEdgeDef.EndVertexIdx);
      }
    }
  };

  if (aDefs.NbSolids() > 0)
  {
    // Each solid def -> one SubGraph.
    for (int aSolidDefIdx = 0; aSolidDefIdx < aDefs.NbSolids(); ++aSolidDefIdx)
    {
      BRepGraph_SubGraph aSub;
      aSub.myParent = &theGraph;

      const BRepGraph_TopoNode::SolidDef& aSolidDef = aDefs.Solid(aSolidDefIdx);
      aSub.mySolidDefIndices.Append(aSolidDefIdx);

      for (int aShellIter = 0; aShellIter < aSolidDef.ShellRefs.Length(); ++aShellIter)
      {
        const int aShellDefIdx = aSolidDef.ShellRefs.Value(aShellIter).ShellIdx;
        const BRepGraph_TopoNode::ShellDef& aShellDef = aDefs.Shell(aShellDefIdx);
        aSub.myShellDefIndices.Append(aShellDefIdx);

        for (int aFaceIter = 0; aFaceIter < aShellDef.FaceRefs.Length(); ++aFaceIter)
        {
          const int aFaceDefIdx = aShellDef.FaceRefs.Value(aFaceIter).FaceIdx;
          aSub.myFaceDefIndices.Append(aFaceDefIdx);
          collectFaceChildren(aSub, aFaceDefIdx);
        }
      }

      aResult.Append(aSub);
    }
  }
  else if (aDefs.NbShells() > 0)
  {
    // No solids but shells exist: each shell def -> one SubGraph.
    for (int aShellDefIdx = 0; aShellDefIdx < aDefs.NbShells(); ++aShellDefIdx)
    {
      BRepGraph_SubGraph aSub;
      aSub.myParent = &theGraph;

      const BRepGraph_TopoNode::ShellDef& aShellDef = aDefs.Shell(aShellDefIdx);
      aSub.myShellDefIndices.Append(aShellDefIdx);

      for (int aFaceIter = 0; aFaceIter < aShellDef.FaceRefs.Length(); ++aFaceIter)
      {
        const int aFaceDefIdx = aShellDef.FaceRefs.Value(aFaceIter).FaceIdx;
        aSub.myFaceDefIndices.Append(aFaceDefIdx);
        collectFaceChildren(aSub, aFaceDefIdx);
      }

      aResult.Append(aSub);
    }
  }
  else
  {
    // No solids, no shells: each face def -> one SubGraph.
    for (int aFaceDefIdx = 0; aFaceDefIdx < aDefs.NbFaces(); ++aFaceDefIdx)
    {
      BRepGraph_SubGraph aSub;
      aSub.myParent = &theGraph;

      aSub.myFaceDefIndices.Append(aFaceDefIdx);
      collectFaceChildren(aSub, aFaceDefIdx);

      aResult.Append(aSub);
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Vector<NCollection_Vector<BRepGraph_NodeId>> BRepGraph_Analyze::RelationClusters(
  const BRepGraph&                        theGraph,
  const NCollection_Array1<BRepGraph_NodeId>& theNodes,
  BRepGraph_RelEdge::Kind                theKind)
{
  NCollection_Vector<NCollection_Vector<BRepGraph_NodeId>> aClusters;
  if (theNodes.IsEmpty())
  {
    return aClusters;
  }

  NCollection_DataMap<int, int> aNodePosByEdgeIdx(theNodes.Length(), theGraph.Allocator());
  for (int aNodeIter = theNodes.Lower(); aNodeIter <= theNodes.Upper(); ++aNodeIter)
  {
    const BRepGraph_NodeId aNodeId = theNodes.Value(aNodeIter);
    aNodePosByEdgeIdx.Bind(aNodeId.Index, aNodeIter - theNodes.Lower() + 1);
  }

  NCollection_Array1<int> aCompOfPos(1, theNodes.Length());
  for (int aPosIter = aCompOfPos.Lower(); aPosIter <= aCompOfPos.Upper(); ++aPosIter)
  {
    aCompOfPos.ChangeValue(aPosIter) = 0;
  }

  int aNbComps = 0;
  for (int aStartPos = aCompOfPos.Lower(); aStartPos <= aCompOfPos.Upper(); ++aStartPos)
  {
    if (aCompOfPos.Value(aStartPos) != 0)
    {
      continue;
    }

    ++aNbComps;
    NCollection_Array1<int> aQueue(0, theNodes.Length() - 1);
    int aQHead = 0;
    int aQTail = 0;
    aQueue.SetValue(aQTail++, aStartPos);
    aCompOfPos.ChangeValue(aStartPos) = aNbComps;

    while (aQHead < aQTail)
    {
      const int              aCurPos = aQueue.Value(aQHead++);
      const BRepGraph_NodeId aCurId  = theNodes.Value(theNodes.Lower() + aCurPos - 1);

      theGraph.RelEdges().ForEachOutOfKind(
        aCurId,
        theKind,
        [&](const BRepGraph_RelEdge& theRelEdge) {
          const int* aNbrPosPtr = aNodePosByEdgeIdx.Seek(theRelEdge.Target.Index);
          if (aNbrPosPtr == nullptr)
          {
            return;
          }

          const int aNbrPos = *aNbrPosPtr;
          if (aCompOfPos.Value(aNbrPos) != 0)
          {
            return;
          }

          aCompOfPos.ChangeValue(aNbrPos) = aNbComps;
          aQueue.SetValue(aQTail++, aNbrPos);
        });
    }
  }

  NCollection_Array1<NCollection_Vector<BRepGraph_NodeId>> aCompNodes(1, aNbComps);
  for (int aPosIter = aCompOfPos.Lower(); aPosIter <= aCompOfPos.Upper(); ++aPosIter)
  {
    const BRepGraph_NodeId aNodeId = theNodes.Value(theNodes.Lower() + aPosIter - 1);
    aCompNodes.ChangeValue(aCompOfPos.Value(aPosIter)).Append(aNodeId);
  }

  for (int aCompIter = aCompNodes.Lower(); aCompIter <= aCompNodes.Upper(); ++aCompIter)
  {
    aClusters.Append(aCompNodes.Value(aCompIter));
  }

  return aClusters;
}

//==================================================================================================

double BRepGraph_Analyze::EdgeEndpointPairScore(const BRepGraph& theGraph,
                                                BRepGraph_NodeId  theEdgeA,
                                                BRepGraph_NodeId  theEdgeB)
{
  const BRepGraph_TopoNode::EdgeDef& aEdgeA = theGraph.Defs().Edge(theEdgeA.Index);
  const BRepGraph_TopoNode::EdgeDef& aEdgeB = theGraph.Defs().Edge(theEdgeB.Index);

  const gp_Pnt& aStartA = theGraph.Defs().Vertex(aEdgeA.StartVertexIdx).Point;
  const gp_Pnt& aEndA   = theGraph.Defs().Vertex(aEdgeA.EndVertexIdx).Point;
  const gp_Pnt& aStartB = theGraph.Defs().Vertex(aEdgeB.StartVertexIdx).Point;
  const gp_Pnt& aEndB   = theGraph.Defs().Vertex(aEdgeB.EndVertexIdx).Point;

  return std::min(aStartA.Distance(aStartB) + aEndA.Distance(aEndB),
                  aStartA.Distance(aEndB) + aEndA.Distance(aStartB));
}

//==================================================================================================

bool BRepGraph_Analyze::AreEdgesCompatibleSampled(const BRepGraph& theGraph,
                                                  BRepGraph_NodeId  theEdgeA,
                                                  BRepGraph_NodeId  theEdgeB,
                                                  double            theTolerance,
                                                  int               theNbSamples,
                                                  double            theMaxChordRatio,
                                                  double            theHighConfidenceRatio)
{
  const BRepGraph_TopoNode::EdgeDef& aNodeA = theGraph.Defs().Edge(theEdgeA.Index);
  const BRepGraph_TopoNode::EdgeDef& aNodeB = theGraph.Defs().Edge(theEdgeB.Index);

  if (aNodeA.IsDegenerate || aNodeB.IsDegenerate)
  {
    return false;
  }
  if (aNodeA.Curve3d.IsNull() || aNodeB.Curve3d.IsNull())
  {
    return false;
  }

  const gp_Pnt& aStartA = theGraph.Defs().Vertex(aNodeA.StartVertexIdx).Point;
  const gp_Pnt& aEndA   = theGraph.Defs().Vertex(aNodeA.EndVertexIdx).Point;
  const gp_Pnt& aStartB = theGraph.Defs().Vertex(aNodeB.StartVertexIdx).Point;
  const gp_Pnt& aEndB   = theGraph.Defs().Vertex(aNodeB.EndVertexIdx).Point;

  const bool isSameDir =
    aStartA.Distance(aStartB) <= theTolerance && aEndA.Distance(aEndB) <= theTolerance;
  const bool isReversed =
    aStartA.Distance(aEndB) <= theTolerance && aEndA.Distance(aStartB) <= theTolerance;
  if (!isSameDir && !isReversed)
  {
    return false;
  }

  const double aChordA = aStartA.Distance(aEndA);
  const double aChordB = aStartB.Distance(aEndB);
  if (aChordA > Precision::Confusion() && aChordB > Precision::Confusion())
  {
    const double aChordRatio = aChordA > aChordB ? aChordA / aChordB : aChordB / aChordA;
    if (aChordRatio > theMaxChordRatio)
    {
      return false;
    }
  }

  GeomAdaptor_TransformedCurve aCurveA = theGraph.Defs().CurveAdaptor(theEdgeA);
  GeomAdaptor_TransformedCurve aCurveB = theGraph.Defs().CurveAdaptor(theEdgeB);

  GCPnts_UniformAbscissa aSamplerA(aCurveA, theNbSamples);
  if (!aSamplerA.IsDone() || aSamplerA.NbPoints() < 2)
  {
    return false;
  }

  NCollection_LocalArray<gp_Pnt, 8> aSamplePtsABuf(aSamplerA.NbPoints());
  NCollection_Array1<gp_Pnt>        aSamplePtsA(aSamplePtsABuf[0], 1, aSamplerA.NbPoints());
  for (int aSmpIter = 1; aSmpIter <= aSamplerA.NbPoints(); ++aSmpIter)
  {
    aSamplePtsA.SetValue(aSmpIter, aCurveA.EvalD0(aSamplerA.Parameter(aSmpIter)));
  }

  ExtremaPC_Curve anExtPCRevA(aCurveA);
  return AreEdgesCompatibleSampled(theGraph,
                                   theEdgeA,
                                   theEdgeB,
                                   aSamplePtsA,
                                   anExtPCRevA,
                                   aChordA,
                                   theTolerance,
                                   theNbSamples,
                                   theMaxChordRatio,
                                   theHighConfidenceRatio);
}

//==================================================================================================

bool BRepGraph_Analyze::AreEdgesCompatibleSampled(const BRepGraph&            theGraph,
                                                  BRepGraph_NodeId             theEdgeA,
                                                  BRepGraph_NodeId             theEdgeB,
                                                  const NCollection_Array1<gp_Pnt>& theSamplePtsA,
                                                  const ExtremaPC_Curve&       theExtPCRevA,
                                                  double                       theChordA,
                                                  double                       theTolerance,
                                                  int                          theNbSamples,
                                                  double                       theMaxChordRatio,
                                                  double                       theHighConfidenceRatio)
{
  const BRepGraph_TopoNode::EdgeDef& aNodeA = theGraph.Defs().Edge(theEdgeA.Index);
  const BRepGraph_TopoNode::EdgeDef& aNodeB = theGraph.Defs().Edge(theEdgeB.Index);

  if (aNodeA.IsDegenerate || aNodeB.IsDegenerate)
  {
    return false;
  }

  const gp_Pnt& aStartA = theGraph.Defs().Vertex(aNodeA.StartVertexIdx).Point;
  const gp_Pnt& aEndA   = theGraph.Defs().Vertex(aNodeA.EndVertexIdx).Point;
  const gp_Pnt& aStartB = theGraph.Defs().Vertex(aNodeB.StartVertexIdx).Point;
  const gp_Pnt& aEndB   = theGraph.Defs().Vertex(aNodeB.EndVertexIdx).Point;

  const bool isSameDir =
    aStartA.Distance(aStartB) <= theTolerance && aEndA.Distance(aEndB) <= theTolerance;
  const bool isReversed =
    aStartA.Distance(aEndB) <= theTolerance && aEndA.Distance(aStartB) <= theTolerance;
  if (!isSameDir && !isReversed)
  {
    return false;
  }

  const double aChordB = aStartB.Distance(aEndB);
  if (theChordA > Precision::Confusion() && aChordB > Precision::Confusion())
  {
    const double aChordRatio = theChordA > aChordB ? theChordA / aChordB : aChordB / theChordA;
    if (aChordRatio > theMaxChordRatio)
    {
      return false;
    }
  }

  GeomAdaptor_TransformedCurve aCurveB = theGraph.Defs().CurveAdaptor(theEdgeB);
  ExtremaPC_Curve              anExtPCB(aCurveB);

  const double aTolSq = theTolerance * theTolerance;
  double       aMaxFwdDistSq = 0.0;
  for (int aSampleIter = theSamplePtsA.Lower(); aSampleIter <= theSamplePtsA.Upper(); ++aSampleIter)
  {
    const gp_Pnt& aPntA = theSamplePtsA.Value(aSampleIter);
    const ExtremaPC::Result& aRes =
      anExtPCB.Perform(aPntA, Precision::Confusion(), ExtremaPC::SearchMode::Min);
    if (!aRes.IsDone() || aRes.NbExt() == 0)
    {
      return false;
    }

    const double aMinSqDist = aRes.MinSquareDistance();
    if (aMinSqDist > aTolSq)
    {
      return false;
    }
    if (aMinSqDist > aMaxFwdDistSq)
    {
      aMaxFwdDistSq = aMinSqDist;
    }
  }

  if (aMaxFwdDistSq < theHighConfidenceRatio * aTolSq)
  {
    return true;
  }

  GCPnts_UniformAbscissa aSamplerB(aCurveB, theNbSamples);
  if (!aSamplerB.IsDone() || aSamplerB.NbPoints() < 2)
  {
    return false;
  }

  for (int aSampleIter = 1; aSampleIter <= aSamplerB.NbPoints(); ++aSampleIter)
  {
    const gp_Pnt aPntB = aCurveB.EvalD0(aSamplerB.Parameter(aSampleIter));
    const ExtremaPC::Result& aRes =
      theExtPCRevA.Perform(aPntB, Precision::Confusion(), ExtremaPC::SearchMode::Min);
    if (!aRes.IsDone() || aRes.NbExt() == 0)
    {
      return false;
    }
    if (aRes.MinSquareDistance() > aTolSq)
    {
      return false;
    }
  }

  return true;
}

//==================================================================================================

void BRepGraph_Analyze::ParallelForEachFace(
  const BRepGraph&                        theGraph,
  const BRepGraph_SubGraph&               theSub,
  const std::function<void(int faceIdx)>& theLambda)
{
  (void)theGraph;
  const NCollection_Vector<int>& aIndices = theSub.FaceDefIndices();
  OSD_Parallel::For(
    0,
    aIndices.Length(),
    [&](int anIdx) { theLambda(aIndices.Value(anIdx)); },
    false);
}

//==================================================================================================

void BRepGraph_Analyze::ParallelForEachEdge(
  const BRepGraph&                        theGraph,
  const BRepGraph_SubGraph&               theSub,
  const std::function<void(int edgeIdx)>& theLambda)
{
  (void)theGraph;
  const NCollection_Vector<int>& aIndices = theSub.EdgeDefIndices();
  OSD_Parallel::For(
    0,
    aIndices.Length(),
    [&](int anIdx) { theLambda(aIndices.Value(anIdx)); },
    false);
}

// ============================================================
// AnalyzeView implementations
// ============================================================

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::AnalyzeView::FreeEdges() const
{ return BRepGraph_Analyze::FreeEdges(*myGraph); }

//=================================================================================================

NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>>
  BRepGraph::AnalyzeView::MissingPCurves() const
{ return BRepGraph_Analyze::MissingPCurves(*myGraph); }

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId>
  BRepGraph::AnalyzeView::ToleranceConflicts(double theThreshold) const
{ return BRepGraph_Analyze::ToleranceConflicts(*myGraph, theThreshold); }

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::AnalyzeView::DegenerateWires() const
{ return BRepGraph_Analyze::DegenerateWires(*myGraph); }

//=================================================================================================

NCollection_Vector<BRepGraph_SubGraph> BRepGraph::AnalyzeView::Decompose() const
{ return BRepGraph_Analyze::Decompose(*myGraph); }

//=================================================================================================

void BRepGraph::AnalyzeView::ParallelForEachFace(
  const BRepGraph_SubGraph&               theSub,
  const std::function<void(int faceIdx)>& theLambda) const
{ BRepGraph_Analyze::ParallelForEachFace(*myGraph, theSub, theLambda); }

//=================================================================================================

void BRepGraph::AnalyzeView::ParallelForEachEdge(
  const BRepGraph_SubGraph&               theSub,
  const std::function<void(int edgeIdx)>& theLambda) const
{ BRepGraph_Analyze::ParallelForEachEdge(*myGraph, theSub, theLambda); }
