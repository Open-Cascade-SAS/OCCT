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

#include <BRepGraphCheck.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>

#include <BRepGraph_WireExplorer.hxx>
#include <BRepGraph_RefsView.hxx>
#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraphAlgo_FClass2d.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <NCollection_Map.hxx>
#include <Precision.hxx>

#include <cmath>

namespace
{

//=================================================================================================

static NCollection_Vector<BRepGraphInc::CoEdgeUsage> collectWireCoEdgeRefs(
  const BRepGraph::TopoView& theDefs,
  const BRepGraph::RefsView& theRefs,
  const BRepGraph_NodeId     theWireNodeId)
{
  NCollection_Vector<BRepGraphInc::CoEdgeUsage> aResult;
  const BRepGraphInc::WireDef&                  aWireDef =
    theDefs.Wires().Definition(BRepGraph_WireId(theWireNodeId.Index));
  for (int aRefIter = 0; aRefIter < aWireDef.CoEdgeRefIds.Length(); ++aRefIter)
  {
    const BRepGraph_CoEdgeRefId    aRefId = aWireDef.CoEdgeRefIds.Value(aRefIter);
    const BRepGraphInc::CoEdgeRef& aRef   = theRefs.CoEdges().Entry(aRefId);
    if (aRef.IsRemoved || !aRef.CoEdgeDefId.IsValid(theDefs.CoEdges().Nb()))
    {
      continue;
    }

    BRepGraphInc::CoEdgeUsage aCoEdgeRef;
    aCoEdgeRef.DefId    = aRef.CoEdgeDefId;
    aCoEdgeRef.Location = aRef.LocalLocation;
    aResult.Append(aCoEdgeRef);
  }
  return aResult;
}

//=================================================================================================

static NCollection_Vector<BRepGraphInc::WireRef> collectFaceWireRefs(
  const BRepGraph::TopoView& theDefs,
  const BRepGraph::RefsView& theRefs,
  const BRepGraph_NodeId     theFaceNodeId)
{
  NCollection_Vector<BRepGraphInc::WireRef> aResult;
  const BRepGraphInc::FaceDef&              aFaceDef =
    theDefs.Faces().Definition(BRepGraph_FaceId(theFaceNodeId.Index));
  for (int aRefIter = 0; aRefIter < aFaceDef.WireRefIds.Length(); ++aRefIter)
  {
    const BRepGraph_WireRefId    aRefId = aFaceDef.WireRefIds.Value(aRefIter);
    const BRepGraphInc::WireRef& aRef   = theRefs.Wires().Entry(aRefId);
    if (aRef.IsRemoved || !aRef.WireDefId.IsValid(theDefs.Wires().Nb()))
    {
      continue;
    }
    aResult.Append(aRef);
  }
  return aResult;
}

} // namespace

//! Helper: compute 2D signed area of a wire from its pcurves using the shoelace formula
//! on sampled points. Positive = CCW, negative = CW.
static double computeWireSignedArea(
  const BRepGraph&                                     theGraph,
  const NCollection_Vector<BRepGraphInc::CoEdgeUsage>& theCoEdgeRefs,
  const BRepGraph_FaceId                               theFaceId)
{
  const BRepGraph::TopoView& aDefs = theGraph.Topo();

  constexpr int THE_NB_SAMPLES = 20;
  double        anArea         = 0.0;
  gp_Pnt2d      aPrevPnt;
  bool          aHasPrev = false;

  auto edgeLookup = [&aDefs](int theIdx) -> const BRepGraphInc::EdgeDef& {
    return aDefs.Edges().Definition(BRepGraph_EdgeId(theIdx));
  };
  auto coedgeLookup = [&aDefs](int theIdx) -> const BRepGraphInc::CoEdgeDef& {
    return aDefs.CoEdges().Definition(BRepGraph_CoEdgeId(theIdx));
  };
  auto vtxRefLookup = [&theGraph](const BRepGraph_VertexRefId theRefId) -> BRepGraph_VertexId {
    return theGraph.Refs().Vertices().Entry(theRefId).VertexDefId;
  };
  for (BRepGraph_WireExplorer anExp(theCoEdgeRefs, coedgeLookup, edgeLookup, vtxRefLookup);
       anExp.More();
       anExp.Next())
  {
    const BRepGraphInc::CoEdgeUsage& aCoEdgeRef = anExp.CurrentRef();
    const BRepGraphInc::CoEdgeDef& aCoEdgeDef = aDefs.CoEdges().Definition(aCoEdgeRef.DefId);

    const BRepGraphInc::CoEdgeDef* aPCurve =
      BRepGraph_Tool::Edge::FindPCurve(theGraph, aCoEdgeDef.EdgeDefId, theFaceId);
    if (aPCurve == nullptr)
      continue;

    if (!aPCurve->Curve2DRepId.IsValid())
      continue;

    const occ::handle<Geom2d_Curve>& aPCurve2d = BRepGraph_Tool::CoEdge::PCurve(theGraph, *aPCurve);
    if (aPCurve2d.IsNull())
      continue;

    const double aFirst       = aPCurve->ParamFirst;
    const double aLast        = aPCurve->ParamLast;
    const bool   anIsReversed = (aCoEdgeDef.Sense == TopAbs_REVERSED);

    for (int aSampleIter = 0; aSampleIter <= THE_NB_SAMPLES; ++aSampleIter)
    {
      double aT;
      if (anIsReversed)
        aT = aLast - (aLast - aFirst) * aSampleIter / THE_NB_SAMPLES;
      else
        aT = aFirst + (aLast - aFirst) * aSampleIter / THE_NB_SAMPLES;

      const gp_Pnt2d aCurrPnt = aPCurve2d->Value(aT);
      if (aHasPrev)
      {
        anArea += (aPrevPnt.X() * aCurrPnt.Y() - aCurrPnt.X() * aPrevPnt.Y());
      }
      aPrevPnt = aCurrPnt;
      aHasPrev = true;
    }
  }

  return anArea * 0.5;
}

//! Helper: collect PCurve adaptors and bounding boxes for a wire.
struct WirePCurveSet
{
  struct EdgeData
  {
    occ::handle<Geom2dAdaptor_Curve> Adaptor;
    Bnd_Box2d                        Box;
    BRepGraph_NodeId                 StartVtxId;
    BRepGraph_NodeId                 EndVtxId;
  };

  NCollection_Vector<EdgeData> Edges;
};

static void collectWirePCurves(const BRepGraph&                                     theGraph,
                               const NCollection_Vector<BRepGraphInc::CoEdgeUsage>& theCoEdgeRefs,
                               const BRepGraph_FaceId                               theFaceId,
                               WirePCurveSet&                                       theResult)
{
  const BRepGraph::TopoView& aDefs = theGraph.Topo();

  auto edgeLookup = [&aDefs](int theIdx) -> const BRepGraphInc::EdgeDef& {
    return aDefs.Edges().Definition(BRepGraph_EdgeId(theIdx));
  };
  auto coedgeLookup = [&aDefs](int theIdx) -> const BRepGraphInc::CoEdgeDef& {
    return aDefs.CoEdges().Definition(BRepGraph_CoEdgeId(theIdx));
  };
  auto vtxRefLookup2 = [&theGraph](const BRepGraph_VertexRefId theRefId) -> BRepGraph_VertexId {
    return theGraph.Refs().Vertices().Entry(theRefId).VertexDefId;
  };
  for (BRepGraph_WireExplorer anExp(theCoEdgeRefs, coedgeLookup, edgeLookup, vtxRefLookup2);
       anExp.More();
       anExp.Next())
  {
    const BRepGraphInc::CoEdgeUsage& aCoEdgeRef = anExp.CurrentRef();
    const BRepGraphInc::CoEdgeDef& aCoEdgeDef = aDefs.CoEdges().Definition(aCoEdgeRef.DefId);
    const BRepGraphInc::EdgeDef&   anEdgeDef  = aDefs.Edges().Definition(aCoEdgeDef.EdgeDefId);

    if (BRepGraph_Tool::Edge::Degenerated(theGraph, aCoEdgeDef.EdgeDefId))
    {
      theResult.Edges.Append(WirePCurveSet::EdgeData());
      continue;
    }

    const BRepGraphInc::CoEdgeDef* aPCurve =
      BRepGraph_Tool::Edge::FindPCurve(theGraph, aCoEdgeDef.EdgeDefId, theFaceId);
    if (aPCurve == nullptr)
    {
      theResult.Edges.Append(WirePCurveSet::EdgeData());
      continue;
    }

    if (!aPCurve->Curve2DRepId.IsValid())
    {
      theResult.Edges.Append(WirePCurveSet::EdgeData());
      continue;
    }

    const occ::handle<Geom2d_Curve>& aCollectedPC2d =
      BRepGraph_Tool::CoEdge::PCurve(theGraph, *aPCurve);
    if (aCollectedPC2d.IsNull())
    {
      theResult.Edges.Append(WirePCurveSet::EdgeData());
      continue;
    }

    WirePCurveSet::EdgeData aData;
    // Resolve oriented vertices through ref entries.
    {
      const BRepGraph_VertexRefId aStartRefId = (aCoEdgeDef.Sense == TopAbs_FORWARD)
                                                  ? anEdgeDef.StartVertexRefId
                                                  : anEdgeDef.EndVertexRefId;
      aData.StartVtxId =
        aStartRefId.IsValid()
          ? BRepGraph_VertexId(theGraph.Refs().Vertices().Entry(aStartRefId).VertexDefId.Index)
          : BRepGraph_NodeId();
      const BRepGraph_VertexRefId anEndRefId = (aCoEdgeDef.Sense == TopAbs_FORWARD)
                                                 ? anEdgeDef.EndVertexRefId
                                                 : anEdgeDef.StartVertexRefId;
      aData.EndVtxId =
        anEndRefId.IsValid()
          ? BRepGraph_VertexId(theGraph.Refs().Vertices().Entry(anEndRefId).VertexDefId.Index)
          : BRepGraph_NodeId();
    }

    aData.Adaptor =
      new Geom2dAdaptor_Curve(aCollectedPC2d, aPCurve->ParamFirst, aPCurve->ParamLast);
    BndLib_Add2dCurve::Add(*aData.Adaptor, Precision::PConfusion(), aData.Box);

    theResult.Edges.Append(aData);
  }
}

//=================================================================================================

void BRepGraphCheck::CheckFaceMinimum(const BRepGraph&                          theGraph,
                                      const BRepGraph_FaceId                    theFace,
                                      NCollection_Vector<BRepGraphCheck_Issue>& theIssues)
{
  const BRepGraph::TopoView&   aDefs    = theGraph.Topo();
  const BRepGraphInc::FaceDef& aFaceDef = aDefs.Faces().Definition(theFace);

  // Face must have a surface.
  if (!BRepGraph_Tool::Face::HasSurface(theGraph, theFace))
  {
    BRepGraphCheck_Issue anIssue;
    anIssue.NodeId        = aFaceDef.Id;
    anIssue.Status        = BRepCheck_NoSurface;
    anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
    theIssues.Append(anIssue);
    return;
  }

  // Check tolerance value.
  if (BRepGraph_Tool::Face::Tolerance(theGraph, theFace) < 0.0)
  {
    BRepGraphCheck_Issue anIssue;
    anIssue.NodeId        = aFaceDef.Id;
    anIssue.Status        = BRepCheck_InvalidToleranceValue;
    anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
    theIssues.Append(anIssue);
  }
}

//=================================================================================================

void BRepGraphCheck::CheckFaceWires(const BRepGraph&                          theGraph,
                                    const BRepGraph_FaceId                    theFace,
                                    const bool                                theGeomControls,
                                    NCollection_Vector<BRepGraphCheck_Issue>& theIssues)
{
  const BRepGraph::TopoView&                      aDefs       = theGraph.Topo();
  const BRepGraph::RefsView&                      aRefs       = theGraph.Refs();
  const BRepGraphInc::FaceDef&                    aFaceDef    = aDefs.Faces().Definition(theFace);
  const BRepGraph_NodeId                          aFaceNodeId = aFaceDef.Id;
  const NCollection_Vector<BRepGraphInc::WireRef> aWireRefs =
    collectFaceWireRefs(aDefs, aRefs, aFaceNodeId);

  // Check for redundant wires (same wire appearing twice in face).
  if (aWireRefs.IsEmpty())
    return;

  NCollection_Map<int> aWireDefSet;
  for (int aWireIter = 0; aWireIter < aWireRefs.Length(); ++aWireIter)
  {
    const BRepGraphInc::WireRef& aWR        = aWireRefs.Value(aWireIter);
    const BRepGraph_NodeId       aWireDefId = aWR.WireDefId;
    if (aWireDefId.IsValid())
    {
      if (!aWireDefSet.Add(aWireDefId.Index))
      {
        BRepGraphCheck_Issue anIssue;
        anIssue.NodeId        = aWireDefId;
        anIssue.ContextNodeId = aFaceNodeId;
        anIssue.Status        = BRepCheck_RedundantWire;
        anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
        theIssues.Append(anIssue);
      }
    }
  }

  if (!theGeomControls)
    return;

  // Collect all wire def indices and their PCurve data.
  // Outer wire first (if present), then inner wires.
  NCollection_Vector<int>           aWireDefIndices;
  NCollection_Vector<WirePCurveSet> aWirePCurveSets;
  bool                              aHasOuterWire    = false;
  int                               aOuterWireRefIdx = -1;

  // First pass: find outer wire and put it first.
  for (int aWireIter = 0; aWireIter < aWireRefs.Length(); ++aWireIter)
  {
    const BRepGraphInc::WireRef& aWR = aWireRefs.Value(aWireIter);
    if (aWR.IsOuter)
    {
      aOuterWireRefIdx = aWireIter;
      aWireDefIndices.Append(aWR.WireDefId.Index);
      WirePCurveSet                                       aSet;
      const NCollection_Vector<BRepGraphInc::CoEdgeUsage> aWireCoEdgeRefs =
        collectWireCoEdgeRefs(aDefs, aRefs, BRepGraph_WireId(aWR.WireDefId.Index));
      collectWirePCurves(theGraph, aWireCoEdgeRefs, theFace, aSet);
      aWirePCurveSets.Append(aSet);
      aHasOuterWire = true;
      break;
    }
  }

  // Second pass: add inner wires.
  for (int aWireIter = 0; aWireIter < aWireRefs.Length(); ++aWireIter)
  {
    if (aWireIter == aOuterWireRefIdx)
      continue;
    const BRepGraphInc::WireRef& aWR = aWireRefs.Value(aWireIter);
    aWireDefIndices.Append(aWR.WireDefId.Index);
    WirePCurveSet                                       aSet;
    const NCollection_Vector<BRepGraphInc::CoEdgeUsage> aWireCoEdgeRefs =
      collectWireCoEdgeRefs(aDefs, aRefs, BRepGraph_WireId(aWR.WireDefId.Index));
    collectWirePCurves(theGraph, aWireCoEdgeRefs, theFace, aSet);
    aWirePCurveSets.Append(aSet);
  }

  // Wire-wire intersection check: for each pair of wires, check all edge PCurve pairs.
  for (int aWireI = 0; aWireI < aWirePCurveSets.Length(); ++aWireI)
  {
    const WirePCurveSet& aSetI = aWirePCurveSets.Value(aWireI);
    for (int aWireJ = aWireI + 1; aWireJ < aWirePCurveSets.Length(); ++aWireJ)
    {
      const WirePCurveSet& aSetJ            = aWirePCurveSets.Value(aWireJ);
      bool                 aHasIntersection = false;

      for (int anEdgeI = 0; anEdgeI < aSetI.Edges.Length() && !aHasIntersection; ++anEdgeI)
      {
        const WirePCurveSet::EdgeData& aEdgeI = aSetI.Edges.Value(anEdgeI);
        if (aEdgeI.Adaptor.IsNull())
          continue;

        for (int anEdgeJ = 0; anEdgeJ < aSetJ.Edges.Length() && !aHasIntersection; ++anEdgeJ)
        {
          const WirePCurveSet::EdgeData& aEdgeJ = aSetJ.Edges.Value(anEdgeJ);
          if (aEdgeJ.Adaptor.IsNull())
            continue;

          if (aEdgeI.Box.IsOut(aEdgeJ.Box))
            continue;

          // Check if edges share a vertex (wires can meet at vertices).
          const bool anIsSharing =
            (aEdgeI.StartVtxId.IsValid()
             && (aEdgeI.StartVtxId == aEdgeJ.StartVtxId || aEdgeI.StartVtxId == aEdgeJ.EndVtxId))
            || (aEdgeI.EndVtxId.IsValid()
                && (aEdgeI.EndVtxId == aEdgeJ.StartVtxId || aEdgeI.EndVtxId == aEdgeJ.EndVtxId));

          Geom2dInt_GInter anInter(*aEdgeI.Adaptor,
                                   *aEdgeJ.Adaptor,
                                   Precision::PConfusion(),
                                   Precision::PConfusion());
          if (!anInter.IsDone())
            continue;

          for (int aPntIter = 1; aPntIter <= anInter.NbPoints(); ++aPntIter)
          {
            const IntRes2d_IntersectionPoint& anIntPnt = anInter.Point(aPntIter);
            if (anIsSharing)
            {
              const double aParamI = anIntPnt.ParamOnFirst();
              const double aParamJ = anIntPnt.ParamOnSecond();
              const bool   anIsAtEndpointI =
                std::abs(aParamI - aEdgeI.Adaptor->FirstParameter()) < Precision::PConfusion()
                || std::abs(aParamI - aEdgeI.Adaptor->LastParameter()) < Precision::PConfusion();
              const bool anIsAtEndpointJ =
                std::abs(aParamJ - aEdgeJ.Adaptor->FirstParameter()) < Precision::PConfusion()
                || std::abs(aParamJ - aEdgeJ.Adaptor->LastParameter()) < Precision::PConfusion();
              if (anIsAtEndpointI && anIsAtEndpointJ)
                continue;
            }
            aHasIntersection = true;
            break;
          }

          if (!aHasIntersection && anInter.NbSegments() > 0 && !anIsSharing)
          {
            aHasIntersection = true;
          }
        }
      }

      if (aHasIntersection)
      {
        BRepGraphCheck_Issue anIssue;
        anIssue.NodeId        = aFaceNodeId;
        anIssue.Status        = BRepCheck_IntersectingWires;
        anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
        theIssues.Append(anIssue);
        return; // Report once per face.
      }
    }
  }

  // Wire imbrication check: inner wires must be inside the outer wire,
  // and no inner wire should contain another.
  if (aWirePCurveSets.Length() > 1 && aHasOuterWire)
  {
    BRepGraphAlgo_FClass2d aClassifier(theGraph,
                                       theFace,
                                       BRepGraph_Tool::Face::Tolerance(theGraph, theFace));

    // For each inner wire, sample a midpoint on its first non-degenerate edge's PCurve.
    for (int anInnerIter = 1; anInnerIter < aWirePCurveSets.Length(); ++anInnerIter)
    {
      const WirePCurveSet& aInnerSet = aWirePCurveSets.Value(anInnerIter);
      for (int anEdgeIter = 0; anEdgeIter < aInnerSet.Edges.Length(); ++anEdgeIter)
      {
        const WirePCurveSet::EdgeData& anEdgeData = aInnerSet.Edges.Value(anEdgeIter);
        if (anEdgeData.Adaptor.IsNull())
          continue;

        const double aMidParam =
          0.5 * (anEdgeData.Adaptor->FirstParameter() + anEdgeData.Adaptor->LastParameter());
        const gp_Pnt2d aMidPnt = anEdgeData.Adaptor->Value(aMidParam);

        const TopAbs_State aState = aClassifier.Perform(aMidPnt, false);
        if (aState == TopAbs_OUT)
        {
          BRepGraphCheck_Issue anIssue;
          anIssue.NodeId        = BRepGraph_WireId(aWireDefIndices.Value(anInnerIter));
          anIssue.ContextNodeId = aFaceNodeId;
          anIssue.Status        = BRepCheck_InvalidImbricationOfWires;
          anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
          theIssues.Append(anIssue);
        }
        break; // Only need one sample point per inner wire.
      }
    }
  }

  // Wire orientation check: outer wire should have correct signed area.
  if (aWirePCurveSets.Length() > 0 && aHasOuterWire)
  {
    const int              aOuterWireIdx     = aWireDefIndices.Value(0);
    const BRepGraph_NodeId anOuterWireNodeId = BRepGraph_WireId(aOuterWireIdx);

    // Determine expected orientation: face definition is in its natural (FORWARD) orientation.
    // The face usage orientation (from parent shell) would flip this, but at the definition
    // level the face is always FORWARD.
    const TopAbs_Orientation                            aFaceOri = TopAbs_FORWARD;
    const NCollection_Vector<BRepGraphInc::CoEdgeUsage> aOuterCoEdgeRefs =
      collectWireCoEdgeRefs(aDefs, aRefs, BRepGraph_WireId(aOuterWireIdx));
    const double anArea = computeWireSignedArea(theGraph, aOuterCoEdgeRefs, theFace);
    // For FORWARD face: outer wire should be CCW (positive area).
    // For REVERSED face: outer wire should be CW (negative area).
    const bool anExpectPositive = (aFaceOri != TopAbs_REVERSED);
    if ((anExpectPositive && anArea < -Precision::PConfusion())
        || (!anExpectPositive && anArea > Precision::PConfusion()))
    {
      BRepGraphCheck_Issue anIssue;
      anIssue.NodeId        = anOuterWireNodeId;
      anIssue.ContextNodeId = aFaceNodeId;
      anIssue.Status        = BRepCheck_BadOrientationOfSubshape;
      anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Warning;
      theIssues.Append(anIssue);
    }
  }
}
