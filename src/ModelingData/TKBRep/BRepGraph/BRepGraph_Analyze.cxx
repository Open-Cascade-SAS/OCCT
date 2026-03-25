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
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_AnalyzeView.hxx>
#include <BRepGraphInc_IncidenceRef.hxx>

#include <Geom_Curve.hxx>

#include <ExtremaPC_Curve.hxx>
#include <GCPnts_UniformAbscissa.hxx>
#include <GeomAdaptor_TransformedCurve.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_LocalArray.hxx>
#include <OSD_Parallel.hxx>
#include <Precision.hxx>

#include <functional>

//==================================================================================================

NCollection_Vector<BRepGraph_EdgeId> BRepGraph_Analyze::FreeEdges(const BRepGraph& theGraph)
{
  NCollection_Vector<BRepGraph_EdgeId> aResult;

  const BRepGraph::DefsView aDefs = theGraph.Defs();

  for (int anEdgeIdx = 0; anEdgeIdx < aDefs.NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_EdgeId             anEdgeId(anEdgeIdx);
    const BRepGraph_TopoNode::EdgeDef& anEdge = aDefs.Edge(anEdgeId);
    if (anEdge.IsRemoved || anEdge.IsDegenerate)
      continue;

    if (aDefs.FaceCountOfEdge(anEdgeId) == 1)
      aResult.Append(anEdgeId);
  }
  return aResult;
}

//==================================================================================================

NCollection_Vector<std::pair<BRepGraph_EdgeId, BRepGraph_FaceId>> BRepGraph_Analyze::MissingPCurves(
  const BRepGraph& theGraph)
{
  NCollection_Vector<std::pair<BRepGraph_EdgeId, BRepGraph_FaceId>> aResult;

  const BRepGraph::DefsView aDefs = theGraph.Defs();

  for (int aFaceDefIdx = 0; aFaceDefIdx < aDefs.NbFaces(); ++aFaceDefIdx)
  {
    const BRepGraph_FaceId             aFaceId(aFaceDefIdx);
    const BRepGraph_TopoNode::FaceDef& aFaceDef   = aDefs.Face(aFaceId);
    const BRepGraph_NodeId             aFaceDefId = aFaceDef.Id;

    for (int aWireRefIdx = 0; aWireRefIdx < aFaceDef.WireRefs.Length(); ++aWireRefIdx)
    {
      const BRepGraph_WireId aWireDefId           = aFaceDef.WireRefs.Value(aWireRefIdx).WireDefId;
      const BRepGraph_TopoNode::WireDef& aWireDef = aDefs.Wire(aWireDefId);
      for (int aCoEdgeIdx = 0; aCoEdgeIdx < aWireDef.CoEdgeRefs.Length(); ++aCoEdgeIdx)
      {
        const BRepGraphInc::CoEdgeEntity& aCoEdge = theGraph.myData->myIncStorage.CoEdge(
          BRepGraph_CoEdgeId(aWireDef.CoEdgeRefs.Value(aCoEdgeIdx).CoEdgeDefId));
        const BRepGraph_EdgeId             anEdgeDefId = aCoEdge.EdgeDefId;
        const BRepGraph_TopoNode::EdgeDef& anEdge      = aDefs.Edge(anEdgeDefId);

        if (anEdge.IsDegenerate)
          continue;

        const BRepGraphInc::CoEdgeEntity* aPCurve = aDefs.FindPCurve(anEdge.Id, aFaceDefId);
        if (aPCurve == nullptr)
          aResult.Append(std::make_pair(anEdgeDefId, aFaceId));
      }
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Vector<BRepGraph_EdgeId> BRepGraph_Analyze::ToleranceConflicts(
  const BRepGraph& theGraph,
  const double     theThreshold)
{
  NCollection_Vector<BRepGraph_EdgeId> aResult;

  // Build temporary curve pointer -> edges map by scanning edge defs
  NCollection_DataMap<const Geom_Curve*, NCollection_Vector<BRepGraph_EdgeId>> aCurveToEdges;
  for (int anEdgeIdx = 0; anEdgeIdx < theGraph.myData->myIncStorage.NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_EdgeId             anEdgeId(anEdgeIdx);
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = theGraph.myData->myIncStorage.Edge(anEdgeId);
    if (!anEdgeDef.Curve3DRepId.IsValid())
      continue;
    const occ::handle<Geom_Curve>& aCurve3d =
      theGraph.myData->myIncStorage.Curve3DRep(anEdgeDef.Curve3DRepId).Curve;
    if (aCurve3d.IsNull())
      continue;
    const Geom_Curve* aCurveKey = aCurve3d.get();
    aCurveToEdges.TryBind(aCurveKey, NCollection_Vector<BRepGraph_EdgeId>());
    aCurveToEdges.ChangeFind(aCurveKey).Append(anEdgeId);
  }

  for (NCollection_DataMap<const Geom_Curve*, NCollection_Vector<BRepGraph_EdgeId>>::Iterator
         anIter(aCurveToEdges);
       anIter.More();
       anIter.Next())
  {
    const NCollection_Vector<BRepGraph_EdgeId>& anEdges = anIter.Value();
    if (anEdges.Length() <= 1)
      continue;

    double aMinTol = 1.0e100;
    double aMaxTol = -1.0;
    for (int anIdx = 0; anIdx < anEdges.Length(); ++anIdx)
    {
      const BRepGraph_EdgeId anEdgeDefId = anEdges.Value(anIdx);
      const double           aTol = theGraph.myData->myIncStorage.Edge(anEdgeDefId).Tolerance;
      if (aTol < aMinTol)
        aMinTol = aTol;
      if (aTol > aMaxTol)
        aMaxTol = aTol;
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

NCollection_Vector<BRepGraph_WireId> BRepGraph_Analyze::DegenerateWires(const BRepGraph& theGraph)
{
  NCollection_Vector<BRepGraph_WireId> aResult;

  const BRepGraph::DefsView aDefs = theGraph.Defs();

  for (int aWireDefIdx = 0; aWireDefIdx < aDefs.NbWires(); ++aWireDefIdx)
  {
    const BRepGraph_WireId             aWireId(aWireDefIdx);
    const BRepGraph_TopoNode::WireDef& aWire = aDefs.Wire(aWireId);

    // Wire with < 2 edges (check via incidence CoEdgeRefs).
    const int aNbEdgesInWire = aWire.CoEdgeRefs.Length();
    if (aNbEdgesInWire < 2)
    {
      aResult.Append(aWireId);
      continue;
    }

    // Outer wire that is not closed (check via incidence IsClosed).
    const bool aIsClosed = aWire.IsClosed;
    if (!aIsClosed)
    {
      // Check if this wire is the outer wire of any face (via reverse index).
      auto isOuterWireOfAnyFace = [&]() -> bool {
        const NCollection_Vector<BRepGraph_FaceId>* aFaces =
          theGraph.myData->myIncStorage.ReverseIndex().FacesOfWire(aWireId);
        if (aFaces == nullptr)
          return false;
        for (int aFIdx = 0; aFIdx < aFaces->Length(); ++aFIdx)
        {
          const BRepGraph_TopoNode::FaceDef& aFaceDef = aDefs.Face(aFaces->Value(aFIdx));
          for (int aWRIdx = 0; aWRIdx < aFaceDef.WireRefs.Length(); ++aWRIdx)
          {
            const BRepGraphInc::WireRef& aWR = aFaceDef.WireRefs.Value(aWRIdx);
            if (aWR.WireDefId.Index == aWireDefIdx && aWR.IsOuter)
              return true;
          }
        }
        return false;
      };
      if (isOuterWireOfAnyFace())
        aResult.Append(aWireId);
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
  auto collectFaceChildren = [&](BRepGraph_SubGraph& theSub, const BRepGraph_FaceId theFaceDefId) {
    const BRepGraph_TopoNode::FaceDef& aFaceDef = aDefs.Face(theFaceDefId);

    for (int aWireRefIdx = 0; aWireRefIdx < aFaceDef.WireRefs.Length(); ++aWireRefIdx)
    {
      const BRepGraph_WireId aWireDefId           = aFaceDef.WireRefs.Value(aWireRefIdx).WireDefId;
      const BRepGraph_TopoNode::WireDef& aWireDef = aDefs.Wire(aWireDefId);
      theSub.myWireDefIds.Append(aWireDefId);

      for (int aCoEdgeIdx = 0; aCoEdgeIdx < aWireDef.CoEdgeRefs.Length(); ++aCoEdgeIdx)
      {
        const BRepGraphInc::CoEdgeEntity& aCoEdge = theGraph.myData->myIncStorage.CoEdge(
          BRepGraph_CoEdgeId(aWireDef.CoEdgeRefs.Value(aCoEdgeIdx).CoEdgeDefId));
        const BRepGraph_EdgeId anEdgeDefId = aCoEdge.EdgeDefId;
        theSub.myEdgeDefIds.Append(anEdgeDefId);

        const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aDefs.Edge(anEdgeDefId);
        if (anEdgeDef.StartVertex.VertexDefId.IsValid())
          theSub.myVertexDefIds.Append(anEdgeDef.StartVertex.VertexDefId);
        if (anEdgeDef.EndVertex.VertexDefId.IsValid())
          theSub.myVertexDefIds.Append(anEdgeDef.EndVertex.VertexDefId);
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

      const BRepGraph_TopoNode::SolidDef& aSolidDef = aDefs.Solid(BRepGraph_SolidId(aSolidDefIdx));
      aSub.mySolidDefIds.Append(BRepGraph_SolidId(aSolidDefIdx));

      for (int aShellIter = 0; aShellIter < aSolidDef.ShellRefs.Length(); ++aShellIter)
      {
        const BRepGraph_ShellId aShellDefId = aSolidDef.ShellRefs.Value(aShellIter).ShellDefId;
        const BRepGraph_TopoNode::ShellDef& aShellDef = aDefs.Shell(aShellDefId);
        aSub.myShellDefIds.Append(aShellDefId);

        for (int aFaceIter = 0; aFaceIter < aShellDef.FaceRefs.Length(); ++aFaceIter)
        {
          const BRepGraph_FaceId aFaceDefId = aShellDef.FaceRefs.Value(aFaceIter).FaceDefId;
          aSub.myFaceDefIds.Append(aFaceDefId);
          collectFaceChildren(aSub, aFaceDefId);
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

      const BRepGraph_TopoNode::ShellDef& aShellDef = aDefs.Shell(BRepGraph_ShellId(aShellDefIdx));
      aSub.myShellDefIds.Append(BRepGraph_ShellId(aShellDefIdx));

      for (int aFaceIter = 0; aFaceIter < aShellDef.FaceRefs.Length(); ++aFaceIter)
      {
        const BRepGraph_FaceId aFaceDefId = aShellDef.FaceRefs.Value(aFaceIter).FaceDefId;
        aSub.myFaceDefIds.Append(aFaceDefId);
        collectFaceChildren(aSub, aFaceDefId);
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

      const BRepGraph_FaceId aFaceDefId(aFaceDefIdx);
      aSub.myFaceDefIds.Append(aFaceDefId);
      collectFaceChildren(aSub, aFaceDefId);

      aResult.Append(aSub);
    }
  }

  return aResult;
}

//==================================================================================================

double BRepGraph_Analyze::EdgeEndpointPairScore(const BRepGraph&       theGraph,
                                                const BRepGraph_EdgeId theEdgeA,
                                                const BRepGraph_EdgeId theEdgeB)
{
  const BRepGraph::DefsView          aDefs  = theGraph.Defs();
  const BRepGraph_TopoNode::EdgeDef& aEdgeA = aDefs.Edge(theEdgeA);
  const BRepGraph_TopoNode::EdgeDef& aEdgeB = aDefs.Edge(theEdgeB);

  const gp_Pnt aStartA = BRepGraph_Tool::Vertex::Pnt(theGraph, aEdgeA.StartVertex);
  const gp_Pnt aEndA   = BRepGraph_Tool::Vertex::Pnt(theGraph, aEdgeA.EndVertex);
  const gp_Pnt aStartB = BRepGraph_Tool::Vertex::Pnt(theGraph, aEdgeB.StartVertex);
  const gp_Pnt aEndB   = BRepGraph_Tool::Vertex::Pnt(theGraph, aEdgeB.EndVertex);

  return std::min(aStartA.Distance(aStartB) + aEndA.Distance(aEndB),
                  aStartA.Distance(aEndB) + aEndA.Distance(aStartB));
}

//==================================================================================================

bool BRepGraph_Analyze::AreEdgesCompatibleSampled(const BRepGraph&       theGraph,
                                                  const BRepGraph_EdgeId theEdgeA,
                                                  const BRepGraph_EdgeId theEdgeB,
                                                  const double           theTolerance,
                                                  const int              theNbSamples,
                                                  const double           theMaxChordRatio,
                                                  const double           theHighConfidenceRatio)
{
  const BRepGraph::DefsView          aDefs  = theGraph.Defs();
  const BRepGraph_TopoNode::EdgeDef& aNodeA = aDefs.Edge(theEdgeA);
  const BRepGraph_TopoNode::EdgeDef& aNodeB = aDefs.Edge(theEdgeB);

  if (aNodeA.IsDegenerate || aNodeB.IsDegenerate)
  {
    return false;
  }
  if (!aNodeA.Curve3DRepId.IsValid() || !aNodeB.Curve3DRepId.IsValid())
  {
    return false;
  }

  const gp_Pnt aStartA = BRepGraph_Tool::Vertex::Pnt(theGraph, aNodeA.StartVertex);
  const gp_Pnt aEndA   = BRepGraph_Tool::Vertex::Pnt(theGraph, aNodeA.EndVertex);
  const gp_Pnt aStartB = BRepGraph_Tool::Vertex::Pnt(theGraph, aNodeB.StartVertex);
  const gp_Pnt aEndB   = BRepGraph_Tool::Vertex::Pnt(theGraph, aNodeB.EndVertex);

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

  GeomAdaptor_TransformedCurve aCurveA = BRepGraph_Tool::Edge::CurveAdaptor(theGraph, theEdgeA);
  GeomAdaptor_TransformedCurve aCurveB = BRepGraph_Tool::Edge::CurveAdaptor(theGraph, theEdgeB);

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

bool BRepGraph_Analyze::AreEdgesCompatibleSampled(const BRepGraph&                  theGraph,
                                                  const BRepGraph_EdgeId            theEdgeA,
                                                  const BRepGraph_EdgeId            theEdgeB,
                                                  const NCollection_Array1<gp_Pnt>& theSamplePtsA,
                                                  const ExtremaPC_Curve&            theExtPCRevA,
                                                  const double                      theChordA,
                                                  const double                      theTolerance,
                                                  const int                         theNbSamples,
                                                  const double theMaxChordRatio,
                                                  const double theHighConfidenceRatio)
{
  const BRepGraph::DefsView          aDefs  = theGraph.Defs();
  const BRepGraph_TopoNode::EdgeDef& aNodeA = aDefs.Edge(theEdgeA);
  const BRepGraph_TopoNode::EdgeDef& aNodeB = aDefs.Edge(theEdgeB);

  if (aNodeA.IsDegenerate || aNodeB.IsDegenerate)
  {
    return false;
  }

  const gp_Pnt aStartA = BRepGraph_Tool::Vertex::Pnt(theGraph, aNodeA.StartVertex);
  const gp_Pnt aEndA   = BRepGraph_Tool::Vertex::Pnt(theGraph, aNodeA.EndVertex);
  const gp_Pnt aStartB = BRepGraph_Tool::Vertex::Pnt(theGraph, aNodeB.StartVertex);
  const gp_Pnt aEndB   = BRepGraph_Tool::Vertex::Pnt(theGraph, aNodeB.EndVertex);

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

  GeomAdaptor_TransformedCurve aCurveB = BRepGraph_Tool::Edge::CurveAdaptor(theGraph, theEdgeB);
  ExtremaPC_Curve              anExtPCB(aCurveB);

  const double aTolSq        = theTolerance * theTolerance;
  double       aMaxFwdDistSq = 0.0;
  for (int aSampleIter = theSamplePtsA.Lower(); aSampleIter <= theSamplePtsA.Upper(); ++aSampleIter)
  {
    const gp_Pnt&            aPntA = theSamplePtsA.Value(aSampleIter);
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
    const gp_Pnt             aPntB = aCurveB.EvalD0(aSamplerB.Parameter(aSampleIter));
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
  const BRepGraph&                                   theGraph,
  const BRepGraph_SubGraph&                          theSub,
  const std::function<void(const BRepGraph_FaceId)>& theLambda)
{
  (void)theGraph;
  const NCollection_Vector<BRepGraph_FaceId>& aIndices = theSub.FaceDefIds();
  OSD_Parallel::For(
    0,
    aIndices.Length(),
    [&](int anIdx) { theLambda(aIndices.Value(anIdx)); },
    false);
}

//==================================================================================================

void BRepGraph_Analyze::ParallelForEachEdge(
  const BRepGraph&                                   theGraph,
  const BRepGraph_SubGraph&                          theSub,
  const std::function<void(const BRepGraph_EdgeId)>& theLambda)
{
  (void)theGraph;
  const NCollection_Vector<BRepGraph_EdgeId>& aIndices = theSub.EdgeDefIds();
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

NCollection_Vector<BRepGraph_EdgeId> BRepGraph::AnalyzeView::FreeEdges() const
{
  return BRepGraph_Analyze::FreeEdges(*myGraph);
}

//=================================================================================================

NCollection_Vector<std::pair<BRepGraph_EdgeId, BRepGraph_FaceId>> BRepGraph::AnalyzeView::
  MissingPCurves() const
{
  return BRepGraph_Analyze::MissingPCurves(*myGraph);
}

//=================================================================================================

NCollection_Vector<BRepGraph_EdgeId> BRepGraph::AnalyzeView::ToleranceConflicts(
  const double theThreshold) const
{
  return BRepGraph_Analyze::ToleranceConflicts(*myGraph, theThreshold);
}

//=================================================================================================

NCollection_Vector<BRepGraph_WireId> BRepGraph::AnalyzeView::DegenerateWires() const
{
  return BRepGraph_Analyze::DegenerateWires(*myGraph);
}

//=================================================================================================

NCollection_Vector<BRepGraph_SubGraph> BRepGraph::AnalyzeView::Decompose() const
{
  return BRepGraph_Analyze::Decompose(*myGraph);
}

//=================================================================================================

void BRepGraph::AnalyzeView::ParallelForEachFace(
  const BRepGraph_SubGraph&                          theSub,
  const std::function<void(const BRepGraph_FaceId)>& theLambda) const
{
  BRepGraph_Analyze::ParallelForEachFace(*myGraph, theSub, theLambda);
}

//=================================================================================================

void BRepGraph::AnalyzeView::ParallelForEachEdge(
  const BRepGraph_SubGraph&                          theSub,
  const std::function<void(const BRepGraph_EdgeId)>& theLambda) const
{
  BRepGraph_Analyze::ParallelForEachEdge(*myGraph, theSub, theLambda);
}
