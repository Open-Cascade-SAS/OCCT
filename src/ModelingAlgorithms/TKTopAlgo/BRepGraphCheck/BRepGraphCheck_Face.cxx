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

#include <BRepGraphInc_WireExplorer.hxx>
#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_TopoNode.hxx>
#include <BRepGraphAlgo_FClass2d.hxx>
#include <BRepGraphInc_IncidenceRef.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <NCollection_Map.hxx>
#include <Precision.hxx>

#include <cmath>

//! Helper: compute 2D signed area of a wire from its pcurves using the shoelace formula
//! on sampled points. Positive = CCW, negative = CW.
static double computeWireSignedArea(const BRepGraph&                           theGraph,
                                    const BRepGraph_TopoNode::WireDef&         theWireDef,
                                    const BRepGraph_NodeId&                    theFaceNodeId)
{
  const BRepGraph::DefsView aDefs = theGraph.Defs();

  constexpr int THE_NB_SAMPLES = 20;
  double        anArea = 0.0;
  gp_Pnt2d      aPrevPnt;
  bool          aHasPrev = false;

  auto edgeLookup = [&aDefs](int theIdx) -> const BRepGraphInc::EdgeEntity& {
    return aDefs.Edge(theIdx);
  };
  auto coedgeLookup = [&aDefs](int theIdx) -> const BRepGraphInc::CoEdgeEntity& {
    return aDefs.CoEdge(theIdx);
  };
  for (BRepGraphInc_WireExplorer anExp(theWireDef.CoEdgeRefs, coedgeLookup, edgeLookup);
       anExp.More(); anExp.Next())
  {
    const BRepGraphInc::CoEdgeRef& aCoEdgeRef = anExp.CurrentRef();
    const BRepGraph_TopoNode::CoEdgeDef& aCoEdgeDef = aDefs.CoEdge(aCoEdgeRef.CoEdgeIdx);

    const BRepGraphInc::CoEdgeEntity* aPCurve =
      BRepGraph_Tool::Edge::FindPCurve(theGraph, aCoEdgeDef.EdgeIdx,
                                 theFaceNodeId.Index);
    if (aPCurve == nullptr)
      continue;

    if (aPCurve->Curve2DRepIdx < 0)
      continue;

    const occ::handle<Geom2d_Curve>& aPCurve2d =
      BRepGraph_Tool::CoEdge::PCurve(theGraph, *aPCurve);
    if (aPCurve2d.IsNull())
      continue;

    const double aFirst = aPCurve->ParamFirst;
    const double aLast  = aPCurve->ParamLast;
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
    Bnd_Box2d                   Box;
    BRepGraph_NodeId            StartVtxId;
    BRepGraph_NodeId            EndVtxId;
  };
  NCollection_Vector<EdgeData> Edges;
};

static void collectWirePCurves(const BRepGraph&                   theGraph,
                               const BRepGraph_TopoNode::WireDef& theWireDef,
                               const BRepGraph_NodeId&            theFaceNodeId,
                               WirePCurveSet&                     theResult)
{
  const BRepGraph::DefsView aDefs = theGraph.Defs();

  auto edgeLookup = [&aDefs](int theIdx) -> const BRepGraphInc::EdgeEntity& {
    return aDefs.Edge(theIdx);
  };
  auto coedgeLookup = [&aDefs](int theIdx) -> const BRepGraphInc::CoEdgeEntity& {
    return aDefs.CoEdge(theIdx);
  };
  for (BRepGraphInc_WireExplorer anExp(theWireDef.CoEdgeRefs, coedgeLookup, edgeLookup);
       anExp.More(); anExp.Next())
  {
    const BRepGraphInc::CoEdgeRef& aCoEdgeRef = anExp.CurrentRef();
    const BRepGraph_TopoNode::CoEdgeDef& aCoEdgeDef = aDefs.CoEdge(aCoEdgeRef.CoEdgeIdx);
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aDefs.Edge(aCoEdgeDef.EdgeIdx);

    if (BRepGraph_Tool::Edge::Degenerated(theGraph, aCoEdgeDef.EdgeIdx))
    {
      theResult.Edges.Append(WirePCurveSet::EdgeData());
      continue;
    }

    const BRepGraphInc::CoEdgeEntity* aPCurve =
      BRepGraph_Tool::Edge::FindPCurve(theGraph, aCoEdgeDef.EdgeIdx,
                                 theFaceNodeId.Index);
    if (aPCurve == nullptr)
    {
      theResult.Edges.Append(WirePCurveSet::EdgeData());
      continue;
    }

    if (aPCurve->Curve2DRepIdx < 0)
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
    aData.StartVtxId = anEdgeDef.OrientedStartVertex(aCoEdgeDef.Sense);
    aData.EndVtxId   = anEdgeDef.OrientedEndVertex(aCoEdgeDef.Sense);

    aData.Adaptor = new Geom2dAdaptor_Curve(aCollectedPC2d, aPCurve->ParamFirst, aPCurve->ParamLast);
    BndLib_Add2dCurve::Add(*aData.Adaptor, Precision::PConfusion(), aData.Box);

    theResult.Edges.Append(aData);
  }
}

//=================================================================================================

void BRepGraphCheck::CheckFaceMinimum(
  const BRepGraph&                         theGraph,
  int                                      theFaceDefIdx,
  NCollection_Vector<BRepGraphCheck_Issue>& theIssues)
{
  const BRepGraph::DefsView aDefs = theGraph.Defs();
  const BRepGraph_TopoNode::FaceDef& aFaceDef = aDefs.Face(theFaceDefIdx);

  // Face must have a surface.
  if (!BRepGraph_Tool::Face::HasSurface(theGraph, theFaceDefIdx))
  {
    BRepGraphCheck_Issue anIssue;
    anIssue.NodeId        = aFaceDef.Id;
    anIssue.Status        = BRepCheck_NoSurface;
    anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
    theIssues.Append(anIssue);
    return;
  }

  // Check tolerance value.
  if (BRepGraph_Tool::Face::Tolerance(theGraph, theFaceDefIdx) < 0.0)
  {
    BRepGraphCheck_Issue anIssue;
    anIssue.NodeId        = aFaceDef.Id;
    anIssue.Status        = BRepCheck_InvalidToleranceValue;
    anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
    theIssues.Append(anIssue);
  }
}

//=================================================================================================

void BRepGraphCheck::CheckFaceWires(
  const BRepGraph&                         theGraph,
  int                                      theFaceDefIdx,
  bool                                     theGeomControls,
  NCollection_Vector<BRepGraphCheck_Issue>& theIssues)
{
  const BRepGraph::DefsView aDefs = theGraph.Defs();
  const BRepGraph_TopoNode::FaceDef& aFaceDef = aDefs.Face(theFaceDefIdx);
  const BRepGraph_NodeId aFaceNodeId = aFaceDef.Id;

  // Check for redundant wires (same wire appearing twice in face).
  if (aFaceDef.WireRefs.IsEmpty())
    return;

  NCollection_Map<int> aWireDefSet;
  for (int aWireIter = 0; aWireIter < aFaceDef.WireRefs.Length(); ++aWireIter)
  {
    const BRepGraphInc::WireRef& aWR = aFaceDef.WireRefs.Value(aWireIter);
    const BRepGraph_NodeId aWireDefId = BRepGraph_NodeId::Wire(aWR.WireIdx);
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
  NCollection_Vector<int> aWireDefIndices;
  NCollection_Vector<WirePCurveSet> aWirePCurveSets;
  bool aHasOuterWire = false;
  int  aOuterWireRefIdx = -1;

  // First pass: find outer wire and put it first.
  for (int aWireIter = 0; aWireIter < aFaceDef.WireRefs.Length(); ++aWireIter)
  {
    const BRepGraphInc::WireRef& aWR = aFaceDef.WireRefs.Value(aWireIter);
    if (aWR.IsOuter)
    {
      aOuterWireRefIdx = aWireIter;
      aWireDefIndices.Append(aWR.WireIdx);
      WirePCurveSet aSet;
      collectWirePCurves(theGraph, aDefs.Wire(aWR.WireIdx), aFaceNodeId, aSet);
      aWirePCurveSets.Append(aSet);
      aHasOuterWire = true;
      break;
    }
  }

  // Second pass: add inner wires.
  for (int aWireIter = 0; aWireIter < aFaceDef.WireRefs.Length(); ++aWireIter)
  {
    if (aWireIter == aOuterWireRefIdx)
      continue;
    const BRepGraphInc::WireRef& aWR = aFaceDef.WireRefs.Value(aWireIter);
    aWireDefIndices.Append(aWR.WireIdx);
    WirePCurveSet aSet;
    collectWirePCurves(theGraph, aDefs.Wire(aWR.WireIdx), aFaceNodeId, aSet);
    aWirePCurveSets.Append(aSet);
  }

  // Wire-wire intersection check: for each pair of wires, check all edge PCurve pairs.
  for (int aWireI = 0; aWireI < aWirePCurveSets.Length(); ++aWireI)
  {
    const WirePCurveSet& aSetI = aWirePCurveSets.Value(aWireI);
    for (int aWireJ = aWireI + 1; aWireJ < aWirePCurveSets.Length(); ++aWireJ)
    {
      const WirePCurveSet& aSetJ = aWirePCurveSets.Value(aWireJ);
      bool aHasIntersection = false;

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
            (aEdgeI.StartVtxId.IsValid() && (aEdgeI.StartVtxId == aEdgeJ.StartVtxId
                                              || aEdgeI.StartVtxId == aEdgeJ.EndVtxId))
            || (aEdgeI.EndVtxId.IsValid() && (aEdgeI.EndVtxId == aEdgeJ.StartVtxId
                                               || aEdgeI.EndVtxId == aEdgeJ.EndVtxId));

          Geom2dInt_GInter anInter(*aEdgeI.Adaptor, *aEdgeJ.Adaptor,
                                   Precision::PConfusion(), Precision::PConfusion());
          if (!anInter.IsDone())
            continue;

          for (int aPntIter = 1; aPntIter <= anInter.NbPoints(); ++aPntIter)
          {
            const IntRes2d_IntersectionPoint& anIntPnt = anInter.Point(aPntIter);
            if (anIsSharing)
            {
              const double aParamI = anIntPnt.ParamOnFirst();
              const double aParamJ = anIntPnt.ParamOnSecond();
              const bool anIsAtEndpointI =
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
    BRepGraphAlgo_FClass2d aClassifier(theGraph, theFaceDefIdx,
                                      BRepGraph_Tool::Face::Tolerance(theGraph, theFaceDefIdx));

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
          anIssue.NodeId        = BRepGraph_NodeId::Wire(aWireDefIndices.Value(anInnerIter));
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
    const int aOuterWireIdx = aWireDefIndices.Value(0);
    const BRepGraph_NodeId aOuterDefId = BRepGraph_NodeId::Wire(aOuterWireIdx);

    // Determine expected orientation: face definition is in its natural (FORWARD) orientation.
    // The face usage orientation (from parent shell) would flip this, but at the definition
    // level the face is always FORWARD.
    const TopAbs_Orientation aFaceOri = TopAbs_FORWARD;
    const double anArea = computeWireSignedArea(theGraph, aDefs.Wire(aOuterWireIdx),
                                                aFaceNodeId);
    // For FORWARD face: outer wire should be CCW (positive area).
    // For REVERSED face: outer wire should be CW (negative area).
    const bool anExpectPositive = (aFaceOri != TopAbs_REVERSED);
    if ((anExpectPositive && anArea < -Precision::PConfusion())
        || (!anExpectPositive && anArea > Precision::PConfusion()))
    {
      BRepGraphCheck_Issue anIssue;
      anIssue.NodeId        = aOuterDefId;
      anIssue.ContextNodeId = aFaceNodeId;
      anIssue.Status        = BRepCheck_BadOrientationOfSubshape;
      anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Warning;
      theIssues.Append(anIssue);
    }
  }
}
