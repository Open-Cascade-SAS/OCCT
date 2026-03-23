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
#include <BRepGraph_UsagesView.hxx>
#include <BRepGraph_AnalyzeView.hxx>

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

  const BRepGraph::DefsView     aDefs     = theGraph.Defs();
  const BRepGraph::RelEdgesView aRelEdges = theGraph.RelEdges();

  for (int anEdgeIdx = 0; anEdgeIdx < aDefs.NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = aDefs.Edge(anEdgeIdx);
    if (anEdge.IsDegenerate)
      continue;

    if (aRelEdges.FaceCountForEdge(anEdgeIdx) == 1)
      aResult.Append(anEdge.Id);
  }
  return aResult;
}

//==================================================================================================

NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>>
  BRepGraph_Analyze::MissingPCurves(const BRepGraph& theGraph)
{
  NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>> aResult;

  const BRepGraph::UsagesView aUsages = theGraph.Usages();
  const BRepGraph::DefsView   aDefs   = theGraph.Defs();

  for (int aFaceUsageIdx = 0; aFaceUsageIdx < aUsages.NbFaces(); ++aFaceUsageIdx)
  {
    const BRepGraph_TopoNode::FaceUsage& aFaceUsage = aUsages.Face(aFaceUsageIdx);
    const BRepGraph_NodeId               aFaceDefId = aFaceUsage.DefId;

    auto checkWire = [&](BRepGraph_UsageId theWireUsageId) {
      if (!theWireUsageId.IsValid())
        return;
      const BRepGraph_TopoNode::WireUsage& aWireUsage = aUsages.Wire(theWireUsageId.Index);
      const BRepGraph_TopoNode::WireDef&   aWireDef   = aDefs.Wire(aWireUsage.DefId.Index);
      for (int anEdgeIdx = 0; anEdgeIdx < aWireDef.OrderedEdges.Length(); ++anEdgeIdx)
      {
        const BRepGraph_NodeId             anEdgeDefId = aWireDef.OrderedEdges.Value(anEdgeIdx).EdgeDefId;
        const BRepGraph_TopoNode::EdgeDef& anEdge      = aDefs.Edge(anEdgeDefId.Index);

        if (anEdge.IsDegenerate)
          continue;

        const BRepGraph_TopoNode::EdgeDef::PCurveEntry* aPCurve = aDefs.FindPCurve(anEdgeDefId, aFaceDefId);
        if (aPCurve == nullptr)
          aResult.Append(std::make_pair(anEdgeDefId, aFaceDefId));
      }
    };

    checkWire(aFaceUsage.OuterWireUsage);
    for (int aWireIdx = 0; aWireIdx < aFaceUsage.InnerWireUsages.Length(); ++aWireIdx)
      checkWire(aFaceUsage.InnerWireUsages.Value(aWireIdx));
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
  for (int anEdgeIdx = 0; anEdgeIdx < theGraph.myData->myEdges.Defs.Length(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = theGraph.myData->myEdges.Defs.Value(anEdgeIdx);
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
      const double aTol = theGraph.myData->myEdges.Defs.Value(anEdges.Value(anIdx).Index).Tolerance;
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

  const BRepGraph::DefsView   aDefs   = theGraph.Defs();
  const BRepGraph::UsagesView aUsages = theGraph.Usages();

  for (int aWireDefIdx = 0; aWireDefIdx < aDefs.NbWires(); ++aWireDefIdx)
  {
    const BRepGraph_TopoNode::WireDef& aWire = aDefs.Wire(aWireDefIdx);

    // Wire with < 2 edges.
    if (aWire.OrderedEdges.Length() < 2)
    {
      aResult.Append(aWire.Id);
      continue;
    }

    // Outer wire that is not closed.
    if (!aWire.IsClosed)
    {
      // Check if any usage of this wire is the outer wire of its face.
      for (int aUsageIdx = 0; aUsageIdx < aWire.Usages.Length(); ++aUsageIdx)
      {
        const BRepGraph_UsageId& aWireUsageId = aWire.Usages.Value(aUsageIdx);
        const BRepGraph_TopoNode::WireUsage& aWireUsage =
          aUsages.Wire(aWireUsageId.Index);
        if (aWireUsage.OwnerFaceUsage.IsValid())
        {
          const BRepGraph_TopoNode::FaceUsage& aFaceUsage =
            aUsages.Face(aWireUsage.OwnerFaceUsage.Index);
          if (aFaceUsage.OuterWireUsage == aWireUsageId)
          {
            aResult.Append(aWire.Id);
            break;
          }
        }
      }
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Vector<BRepGraph_SubGraph> BRepGraph_Analyze::Decompose(const BRepGraph& theGraph)
{
  NCollection_Vector<BRepGraph_SubGraph> aResult;

  const BRepGraph::DefsView   aDefs   = theGraph.Defs();
  const BRepGraph::UsagesView aUsages = theGraph.Usages();

  // Collect wire, edge and vertex children from a face usage into a SubGraph.
  auto collectFaceChildren = [&](BRepGraph_SubGraph& theSub, int theFaceUsageIdx) {
    const BRepGraph_TopoNode::FaceUsage& aFaceUsage = aUsages.Face(theFaceUsageIdx);

    auto collectWire = [&](BRepGraph_UsageId theWireUsageId) {
      if (!theWireUsageId.IsValid())
        return;
      const BRepGraph_TopoNode::WireUsage& aWireUsage =
        aUsages.Wire(theWireUsageId.Index);
      theSub.myWireDefIndices.Append(aWireUsage.DefId.Index);
      theSub.myWireUsageIndices.Append(theWireUsageId.Index);
      const BRepGraph_TopoNode::WireDef& aWireDef =
        aDefs.Wire(aWireUsage.DefId.Index);
      for (int anEdgeIdx = 0; anEdgeIdx < aWireDef.OrderedEdges.Length(); ++anEdgeIdx)
      {
        const int anEdgeDefIdx = aWireDef.OrderedEdges.Value(anEdgeIdx).EdgeDefId.Index;
        theSub.myEdgeDefIndices.Append(anEdgeDefIdx);

        // Also add edge usage indices from WireUsage.EdgeUsages if available.
        if (anEdgeIdx < aWireUsage.EdgeUsages.Length())
        {
          const BRepGraph_UsageId& anEdgeUsageId = aWireUsage.EdgeUsages.Value(anEdgeIdx);
          theSub.myEdgeUsageIndices.Append(anEdgeUsageId.Index);

          // Get vertex usages from edge usage.
          const BRepGraph_TopoNode::EdgeUsage& anEdgeUsage =
            aUsages.Edge(anEdgeUsageId.Index);
          if (anEdgeUsage.StartVertexUsage.IsValid())
            theSub.myVertexUsageIndices.Append(anEdgeUsage.StartVertexUsage.Index);
          if (anEdgeUsage.EndVertexUsage.IsValid())
            theSub.myVertexUsageIndices.Append(anEdgeUsage.EndVertexUsage.Index);
        }

        const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aDefs.Edge(anEdgeDefIdx);
        if (anEdgeDef.StartVertexDefId.IsValid())
          theSub.myVertexDefIndices.Append(anEdgeDef.StartVertexDefId.Index);
        if (anEdgeDef.EndVertexDefId.IsValid())
          theSub.myVertexDefIndices.Append(anEdgeDef.EndVertexDefId.Index);
      }
    };

    collectWire(aFaceUsage.OuterWireUsage);
    for (int aWireIdx = 0; aWireIdx < aFaceUsage.InnerWireUsages.Length(); ++aWireIdx)
      collectWire(aFaceUsage.InnerWireUsages.Value(aWireIdx));
  };

  if (aUsages.NbSolids() > 0)
  {
    // Each solid usage -> one SubGraph.
    for (int aSolidUsageIdx = 0; aSolidUsageIdx < aUsages.NbSolids(); ++aSolidUsageIdx)
    {
      BRepGraph_SubGraph aSub;
      aSub.myParent = &theGraph;

      const BRepGraph_TopoNode::SolidUsage& aSolidUsage = aUsages.Solid(aSolidUsageIdx);
      aSub.mySolidDefIndices.Append(aSolidUsage.DefId.Index);
      aSub.mySolidUsageIndices.Append(aSolidUsageIdx);

      for (int aShellIter = 0; aShellIter < aSolidUsage.ShellUsages.Length(); ++aShellIter)
      {
        const BRepGraph_UsageId& aShellUsageId = aSolidUsage.ShellUsages.Value(aShellIter);
        const BRepGraph_TopoNode::ShellUsage& aShellUsage =
          aUsages.Shell(aShellUsageId.Index);
        aSub.myShellDefIndices.Append(aShellUsage.DefId.Index);
        aSub.myShellUsageIndices.Append(aShellUsageId.Index);

        for (int aFaceIter = 0; aFaceIter < aShellUsage.FaceUsages.Length(); ++aFaceIter)
        {
          const BRepGraph_UsageId& aFaceUsageId = aShellUsage.FaceUsages.Value(aFaceIter);
          const BRepGraph_TopoNode::FaceUsage& aFaceUsage =
            aUsages.Face(aFaceUsageId.Index);
          aSub.myFaceDefIndices.Append(aFaceUsage.DefId.Index);
          aSub.myFaceUsageIndices.Append(aFaceUsageId.Index);
          collectFaceChildren(aSub, aFaceUsageId.Index);
        }
      }

      aResult.Append(aSub);
    }
  }
  else if (aUsages.NbShells() > 0)
  {
    // No solids but shells exist: each shell usage -> one SubGraph.
    for (int aShellUsageIdx = 0; aShellUsageIdx < aUsages.NbShells(); ++aShellUsageIdx)
    {
      BRepGraph_SubGraph aSub;
      aSub.myParent = &theGraph;

      const BRepGraph_TopoNode::ShellUsage& aShellUsage = aUsages.Shell(aShellUsageIdx);
      aSub.myShellDefIndices.Append(aShellUsage.DefId.Index);
      aSub.myShellUsageIndices.Append(aShellUsageIdx);

      for (int aFaceIter = 0; aFaceIter < aShellUsage.FaceUsages.Length(); ++aFaceIter)
      {
        const BRepGraph_UsageId& aFaceUsageId = aShellUsage.FaceUsages.Value(aFaceIter);
        const BRepGraph_TopoNode::FaceUsage& aFaceUsage =
          aUsages.Face(aFaceUsageId.Index);
        aSub.myFaceDefIndices.Append(aFaceUsage.DefId.Index);
        aSub.myFaceUsageIndices.Append(aFaceUsageId.Index);
        collectFaceChildren(aSub, aFaceUsageId.Index);
      }

      aResult.Append(aSub);
    }
  }
  else
  {
    // No solids, no shells: each face usage -> one SubGraph.
    for (int aFaceUsageIdx = 0; aFaceUsageIdx < aUsages.NbFaces(); ++aFaceUsageIdx)
    {
      BRepGraph_SubGraph aSub;
      aSub.myParent = &theGraph;

      const BRepGraph_TopoNode::FaceUsage& aFaceUsage = aUsages.Face(aFaceUsageIdx);
      aSub.myFaceDefIndices.Append(aFaceUsage.DefId.Index);
      aSub.myFaceUsageIndices.Append(aFaceUsageIdx);
      collectFaceChildren(aSub, aFaceUsageIdx);

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
    NCollection_Vector<int> aQueue;
    aQueue.Append(aStartPos);
    aCompOfPos.ChangeValue(aStartPos) = aNbComps;

    for (int aQueueIter = 0; aQueueIter < aQueue.Length(); ++aQueueIter)
    {
      const int              aCurPos = aQueue.Value(aQueueIter);
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
          aQueue.Append(aNbrPos);
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

  const gp_Pnt& aStartA = theGraph.Defs().Vertex(aEdgeA.StartVertexDefId.Index).Point;
  const gp_Pnt& aEndA   = theGraph.Defs().Vertex(aEdgeA.EndVertexDefId.Index).Point;
  const gp_Pnt& aStartB = theGraph.Defs().Vertex(aEdgeB.StartVertexDefId.Index).Point;
  const gp_Pnt& aEndB   = theGraph.Defs().Vertex(aEdgeB.EndVertexDefId.Index).Point;

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

  const gp_Pnt& aStartA = theGraph.Defs().Vertex(aNodeA.StartVertexDefId.Index).Point;
  const gp_Pnt& aEndA   = theGraph.Defs().Vertex(aNodeA.EndVertexDefId.Index).Point;
  const gp_Pnt& aStartB = theGraph.Defs().Vertex(aNodeB.StartVertexDefId.Index).Point;
  const gp_Pnt& aEndB   = theGraph.Defs().Vertex(aNodeB.EndVertexDefId.Index).Point;

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

  const gp_Pnt& aStartA = theGraph.Defs().Vertex(aNodeA.StartVertexDefId.Index).Point;
  const gp_Pnt& aEndA   = theGraph.Defs().Vertex(aNodeA.EndVertexDefId.Index).Point;
  const gp_Pnt& aStartB = theGraph.Defs().Vertex(aNodeB.StartVertexDefId.Index).Point;
  const gp_Pnt& aEndB   = theGraph.Defs().Vertex(aNodeB.EndVertexDefId.Index).Point;

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
