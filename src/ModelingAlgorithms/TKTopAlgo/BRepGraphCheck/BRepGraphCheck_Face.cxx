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

#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_GeomNode.hxx>
#include <BRepGraph_GeomView.hxx>
#include <BRepGraph_TopoNode.hxx>
#include <BRepGraph_UsagesView.hxx>
#include <BRepGraphAlgo_FClass2d.hxx>
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
  const BRepGraph::DefsView  aDefs = theGraph.Defs();
  const BRepGraph::GeomView  aGeom = theGraph.Geom();

  constexpr int THE_NB_SAMPLES = 20;
  double        anArea = 0.0;
  gp_Pnt2d      aPrevPnt;
  bool          aHasPrev = false;

  for (int anEdgeIter = 0; anEdgeIter < theWireDef.OrderedEdges.Length(); ++anEdgeIter)
  {
    const BRepGraph_TopoNode::WireDef::EdgeEntry& anEntry =
      theWireDef.OrderedEdges.Value(anEdgeIter);
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aDefs.Edge(anEntry.EdgeDefId.Index);

    const BRepGraph_NodeId aPCurveNodeId = aGeom.PCurveOf(anEdgeDef.Id, theFaceNodeId);
    if (!aPCurveNodeId.IsValid())
      continue;

    const BRepGraph_GeomNode::PCurve& aPCurve = aGeom.PCurve(aPCurveNodeId.Index);
    if (aPCurve.Curve2d.IsNull())
      continue;

    const double aFirst = aPCurve.ParamFirst;
    const double aLast  = aPCurve.ParamLast;
    const bool   anIsReversed = (anEntry.OrientationInWire == TopAbs_REVERSED);

    for (int aSampleIter = 0; aSampleIter <= THE_NB_SAMPLES; ++aSampleIter)
    {
      double aT;
      if (anIsReversed)
        aT = aLast - (aLast - aFirst) * aSampleIter / THE_NB_SAMPLES;
      else
        aT = aFirst + (aLast - aFirst) * aSampleIter / THE_NB_SAMPLES;

      const gp_Pnt2d aCurrPnt = aPCurve.Curve2d->Value(aT);
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
    Handle(Geom2dAdaptor_Curve) Adaptor;
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
  const BRepGraph::GeomView aGeom = theGraph.Geom();

  for (int anEdgeIter = 0; anEdgeIter < theWireDef.OrderedEdges.Length(); ++anEdgeIter)
  {
    const BRepGraph_TopoNode::WireDef::EdgeEntry& anEntry =
      theWireDef.OrderedEdges.Value(anEdgeIter);
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aDefs.Edge(anEntry.EdgeDefId.Index);

    if (anEdgeDef.IsDegenerate)
    {
      theResult.Edges.Append(WirePCurveSet::EdgeData());
      continue;
    }

    const BRepGraph_NodeId aPCurveNodeId = aGeom.PCurveOf(anEdgeDef.Id, theFaceNodeId);
    if (!aPCurveNodeId.IsValid())
    {
      theResult.Edges.Append(WirePCurveSet::EdgeData());
      continue;
    }

    const BRepGraph_GeomNode::PCurve& aPCurve = aGeom.PCurve(aPCurveNodeId.Index);
    if (aPCurve.Curve2d.IsNull())
    {
      theResult.Edges.Append(WirePCurveSet::EdgeData());
      continue;
    }

    WirePCurveSet::EdgeData aData;
    aData.StartVtxId = anEdgeDef.OrientedStartVertex(anEntry.OrientationInWire);
    aData.EndVtxId   = anEdgeDef.OrientedEndVertex(anEntry.OrientationInWire);

    aData.Adaptor = new Geom2dAdaptor_Curve(aPCurve.Curve2d, aPCurve.ParamFirst, aPCurve.ParamLast);
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
  if (!aFaceDef.SurfNodeId.IsValid())
  {
    BRepGraphCheck_Issue anIssue;
    anIssue.NodeId        = aFaceDef.Id;
    anIssue.Status        = BRepCheck_NoSurface;
    anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
    theIssues.Append(anIssue);
    return;
  }

  // Verify the surface handle is not null.
  const BRepGraph::GeomView aGeom = theGraph.Geom();
  const BRepGraph_GeomNode::Surf& aSurfNode = aGeom.Surface(aFaceDef.SurfNodeId.Index);
  if (aSurfNode.Surface.IsNull())
  {
    BRepGraphCheck_Issue anIssue;
    anIssue.NodeId        = aFaceDef.Id;
    anIssue.Status        = BRepCheck_NoSurface;
    anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
    theIssues.Append(anIssue);
  }

  // Check tolerance value.
  if (aFaceDef.Tolerance < 0.0)
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
  if (aFaceDef.Usages.IsEmpty())
    return;

  const BRepGraph::UsagesView aUsages = theGraph.Usages();
  const BRepGraph_TopoNode::FaceUsage& aFU = aUsages.Face(aFaceDef.Usages.Value(0).Index);

  NCollection_Map<int> aWireDefSet;
  auto checkWireUnique = [&](const BRepGraph_UsageId& theWireUsage)
  {
    const BRepGraph_NodeId aWireDefId = theGraph.DefOf(theWireUsage);
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
  };

  if (aFU.OuterWireUsage.IsValid())
    checkWireUnique(aFU.OuterWireUsage);
  for (int aWireIter = 0; aWireIter < aFU.InnerWireUsages.Length(); ++aWireIter)
    checkWireUnique(aFU.InnerWireUsages.Value(aWireIter));

  if (!theGeomControls)
    return;

  // Collect all wire def indices and their PCurve data.
  NCollection_Vector<int> aWireDefIndices;
  NCollection_Vector<WirePCurveSet> aWirePCurveSets;

  if (aFU.OuterWireUsage.IsValid())
  {
    const BRepGraph_NodeId aOuterDefId = theGraph.DefOf(aFU.OuterWireUsage);
    if (aOuterDefId.IsValid())
    {
      aWireDefIndices.Append(aOuterDefId.Index);
      WirePCurveSet aSet;
      collectWirePCurves(theGraph, aDefs.Wire(aOuterDefId.Index), aFaceNodeId, aSet);
      aWirePCurveSets.Append(aSet);
    }
  }
  for (int aWireIter = 0; aWireIter < aFU.InnerWireUsages.Length(); ++aWireIter)
  {
    const BRepGraph_NodeId aWireDefId = theGraph.DefOf(aFU.InnerWireUsages.Value(aWireIter));
    if (aWireDefId.IsValid())
    {
      aWireDefIndices.Append(aWireDefId.Index);
      WirePCurveSet aSet;
      collectWirePCurves(theGraph, aDefs.Wire(aWireDefId.Index), aFaceNodeId, aSet);
      aWirePCurveSets.Append(aSet);
    }
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
  if (aWirePCurveSets.Length() > 1 && aFU.OuterWireUsage.IsValid())
  {
    BRepGraphAlgo_FClass2d aClassifier(theGraph, theFaceDefIdx, aFaceDef.Tolerance);

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
  if (aWirePCurveSets.Length() > 0 && aFU.OuterWireUsage.IsValid())
  {
    const BRepGraph_NodeId aOuterDefId = theGraph.DefOf(aFU.OuterWireUsage);
    if (aOuterDefId.IsValid())
    {
      // Determine expected orientation from face usage orientation.
      const TopAbs_Orientation aFaceOri = aFU.Orientation;
      const double anArea = computeWireSignedArea(theGraph, aDefs.Wire(aOuterDefId.Index),
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
}
