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
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <NCollection_Map.hxx>
#include <Precision.hxx>

#include <cmath>

//=================================================================================================

void BRepGraphCheck::CheckWireMinimum(
  const BRepGraph&                         theGraph,
  int                                      theWireDefIdx,
  NCollection_Vector<BRepGraphCheck_Issue>& theIssues)
{
  const BRepGraph::DefsView aDefs = theGraph.Defs();
  const BRepGraph_TopoNode::WireDef& aWireDef = aDefs.Wire(theWireDefIdx);

  // Empty wire check.
  if (aWireDef.OrderedEdges.IsEmpty())
  {
    BRepGraphCheck_Issue anIssue;
    anIssue.NodeId        = aWireDef.Id;
    anIssue.Status        = BRepCheck_EmptyWire;
    anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
    theIssues.Append(anIssue);
    return;
  }

  // Redundant edge check: same edge appearing more than once
  // without forming a valid FORWARD/REVERSED pair.
  NCollection_DataMap<int, int> anEdgeCounts;
  NCollection_DataMap<int, int> anEdgeForwardCounts;
  for (int anEdgeIter = 0; anEdgeIter < aWireDef.OrderedEdges.Length(); ++anEdgeIter)
  {
    const BRepGraph_TopoNode::WireDef::EdgeEntry& anEntry =
      aWireDef.OrderedEdges.Value(anEdgeIter);
    const int anEdgeIdx = anEntry.EdgeDefId.Index;
    if (!anEdgeCounts.IsBound(anEdgeIdx))
    {
      anEdgeCounts.Bind(anEdgeIdx, 1);
      anEdgeForwardCounts.Bind(anEdgeIdx, 0);
    }
    else
    {
      anEdgeCounts.ChangeFind(anEdgeIdx)++;
    }
    if (anEntry.OrientationInWire == TopAbs_FORWARD)
    {
      anEdgeForwardCounts.ChangeFind(anEdgeIdx)++;
    }
  }

  for (NCollection_DataMap<int, int>::Iterator anIter(anEdgeCounts); anIter.More(); anIter.Next())
  {
    const int aCount = anIter.Value();
    if (aCount > 2)
    {
      // More than 2 occurrences is always redundant.
      BRepGraphCheck_Issue anIssue;
      anIssue.NodeId        = BRepGraph_NodeId::Edge(anIter.Key());
      anIssue.ContextNodeId = aWireDef.Id;
      anIssue.Status        = BRepCheck_RedundantEdge;
      anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
      theIssues.Append(anIssue);
    }
    else if (aCount == 2)
    {
      // Two occurrences with same orientation is redundant.
      const int aFwdCount = anEdgeForwardCounts.Find(anIter.Key());
      if (aFwdCount == 2 || aFwdCount == 0)
      {
        BRepGraphCheck_Issue anIssue;
        anIssue.NodeId        = BRepGraph_NodeId::Edge(anIter.Key());
        anIssue.ContextNodeId = aWireDef.Id;
        anIssue.Status        = BRepCheck_RedundantEdge;
        anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
        theIssues.Append(anIssue);
      }
    }
  }
}

//=================================================================================================

void BRepGraphCheck::CheckWireOnFace(
  const BRepGraph&                         theGraph,
  int                                      theWireDefIdx,
  int                                      theFaceDefIdx,
  bool                                     theGeomControls,
  NCollection_Vector<BRepGraphCheck_Issue>& theIssues)
{
  const BRepGraph::DefsView aDefs = theGraph.Defs();
  const BRepGraph_TopoNode::WireDef& aWireDef = aDefs.Wire(theWireDefIdx);
  const BRepGraph_NodeId aWireNodeId = aWireDef.Id;
  const BRepGraph_NodeId aFaceNodeId = BRepGraph_NodeId::Face(theFaceDefIdx);

  if (aWireDef.OrderedEdges.IsEmpty())
    return;

  // Connectivity check: adjacent edges must share a vertex.
  const int aNbEdges = aWireDef.OrderedEdges.Length();
  for (int anEdgeIter = 0; anEdgeIter < aNbEdges - 1; ++anEdgeIter)
  {
    const BRepGraph_TopoNode::WireDef::EdgeEntry& aCurrEntry =
      aWireDef.OrderedEdges.Value(anEdgeIter);
    const BRepGraph_TopoNode::WireDef::EdgeEntry& aNextEntry =
      aWireDef.OrderedEdges.Value(anEdgeIter + 1);

    const BRepGraph_TopoNode::EdgeDef& aCurrEdge = aDefs.Edge(aCurrEntry.EdgeDefId.Index);
    const BRepGraph_TopoNode::EdgeDef& aNextEdge = aDefs.Edge(aNextEntry.EdgeDefId.Index);

    // Determine the end vertex of current edge and start vertex of next edge
    // based on orientation in the wire.
    const BRepGraph_NodeId aCurrEndVtx   = aCurrEdge.OrientedEndVertex(aCurrEntry.OrientationInWire);
    const BRepGraph_NodeId aNextStartVtx = aNextEdge.OrientedStartVertex(aNextEntry.OrientationInWire);

    if (aCurrEndVtx.IsValid() && aNextStartVtx.IsValid() && aCurrEndVtx != aNextStartVtx)
    {
      BRepGraphCheck_Issue anIssue;
      anIssue.NodeId        = aWireNodeId;
      anIssue.ContextNodeId = aFaceNodeId;
      anIssue.Status        = BRepCheck_NotConnected;
      anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
      theIssues.Append(anIssue);
      break; // Report once per wire.
    }
  }

  // Closure check for closed wires: first start == last end.
  if (aNbEdges > 1)
  {
    const BRepGraph_TopoNode::WireDef::EdgeEntry& aFirstEntry =
      aWireDef.OrderedEdges.Value(0);
    const BRepGraph_TopoNode::WireDef::EdgeEntry& aLastEntry =
      aWireDef.OrderedEdges.Value(aNbEdges - 1);

    const BRepGraph_TopoNode::EdgeDef& aFirstEdge = aDefs.Edge(aFirstEntry.EdgeDefId.Index);
    const BRepGraph_TopoNode::EdgeDef& aLastEdge  = aDefs.Edge(aLastEntry.EdgeDefId.Index);

    const BRepGraph_NodeId aFirstStartVtx = aFirstEdge.OrientedStartVertex(aFirstEntry.OrientationInWire);
    const BRepGraph_NodeId aLastEndVtx    = aLastEdge.OrientedEndVertex(aLastEntry.OrientationInWire);

    // If the wire is expected to be closed but vertices don't match.
    if (aWireDef.IsClosed && aFirstStartVtx.IsValid() && aLastEndVtx.IsValid()
        && aFirstStartVtx != aLastEndVtx)
    {
      BRepGraphCheck_Issue anIssue;
      anIssue.NodeId        = aWireNodeId;
      anIssue.ContextNodeId = aFaceNodeId;
      anIssue.Status        = BRepCheck_NotClosed;
      anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
      theIssues.Append(anIssue);
    }
  }

  // Geometric controls: 2D parametric closure and self-intersection detection.
  if (!theGeomControls)
    return;

  const BRepGraph::GeomView aGeom = theGraph.Geom();
  const BRepGraph_TopoNode::FaceDef& aFaceDef = aDefs.Face(theFaceDefIdx);

  // 2D parametric closure check: verify UV endpoints of first and last edges meet.
  if (aWireDef.IsClosed && aNbEdges > 1 && aFaceDef.SurfNodeId.IsValid())
  {
    const BRepGraph_TopoNode::WireDef::EdgeEntry& aFirstEntry =
      aWireDef.OrderedEdges.Value(0);
    const BRepGraph_TopoNode::WireDef::EdgeEntry& aLastEntry =
      aWireDef.OrderedEdges.Value(aNbEdges - 1);

    const BRepGraph_TopoNode::EdgeDef& aFirstEdge = aDefs.Edge(aFirstEntry.EdgeDefId.Index);
    const BRepGraph_TopoNode::EdgeDef& aLastEdge  = aDefs.Edge(aLastEntry.EdgeDefId.Index);

    // Get PCurves for first and last edges.
    const BRepGraph_TopoNode::EdgeDef::PCurveEntry* aFirstPC =
      aGeom.FindPCurve(aFirstEdge.Id, aFaceNodeId);
    const BRepGraph_TopoNode::EdgeDef::PCurveEntry* aLastPC =
      aGeom.FindPCurve(aLastEdge.Id, aFaceNodeId);

    if (aFirstPC != nullptr && aLastPC != nullptr)
    {
      if (!aFirstPC->Curve2d.IsNull() && !aLastPC->Curve2d.IsNull())
      {
        // Evaluate UV at wire-start of first edge and wire-end of last edge.
        const double aFirstParam = (aFirstEntry.OrientationInWire == TopAbs_FORWARD)
                                     ? aFirstPC->ParamFirst
                                     : aFirstPC->ParamLast;
        const double aLastParam = (aLastEntry.OrientationInWire == TopAbs_FORWARD)
                                    ? aLastPC->ParamLast
                                    : aLastPC->ParamFirst;

        const gp_Pnt2d aFirstUV = aFirstPC->Curve2d->Value(aFirstParam);
        const gp_Pnt2d aLastUV  = aLastPC->Curve2d->Value(aLastParam);

        // Convert 3D tolerance to UV tolerance using surface adaptor.
        const BRepGraph_GeomNode::Surf& aSurf = aGeom.Surface(aFaceDef.SurfNodeId.Index);
        if (!aSurf.Surface.IsNull())
        {
          const double aTol3d = Precision::Confusion();
          GeomAdaptor_Surface aSurfAdaptor(aSurf.Surface);
          const double aUTol = aSurfAdaptor.UResolution(aTol3d);
          const double aVTol = aSurfAdaptor.VResolution(aTol3d);

          double aDeltaU = std::abs(aFirstUV.X() - aLastUV.X());
          double aDeltaV = std::abs(aFirstUV.Y() - aLastUV.Y());

          // Account for period wrapping on periodic surfaces.
          if (aSurf.Surface->IsUPeriodic())
          {
            const double aUPeriod = aSurf.Surface->UPeriod();
            if (aDeltaU > aUPeriod * 0.5)
              aDeltaU = aUPeriod - aDeltaU;
          }
          if (aSurf.Surface->IsVPeriodic())
          {
            const double aVPeriod = aSurf.Surface->VPeriod();
            if (aDeltaV > aVPeriod * 0.5)
              aDeltaV = aVPeriod - aDeltaV;
          }

          if (aDeltaU > aUTol || aDeltaV > aVTol)
          {
            BRepGraphCheck_Issue anIssue;
            anIssue.NodeId        = aWireNodeId;
            anIssue.ContextNodeId = aFaceNodeId;
            anIssue.Status        = BRepCheck_NotClosed;
            anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
            theIssues.Append(anIssue);
          }
        }
      }
    }
  }

  if (!aFaceDef.SurfNodeId.IsValid())
    return;

  // Collect PCurve adaptors and bounding boxes for each edge in the wire.
  struct EdgePCurveData
  {
    Handle(Geom2dAdaptor_Curve) Adaptor;
    Bnd_Box2d                   Box;
    int                         EdgeDefIdx;
    BRepGraph_NodeId            StartVtxId;
    BRepGraph_NodeId            EndVtxId;
  };
  NCollection_Vector<EdgePCurveData> aPCurveData;

  for (int anEdgeIter = 0; anEdgeIter < aNbEdges; ++anEdgeIter)
  {
    const BRepGraph_TopoNode::WireDef::EdgeEntry& anEntry =
      aWireDef.OrderedEdges.Value(anEdgeIter);
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aDefs.Edge(anEntry.EdgeDefId.Index);

    // Skip degenerate edges.
    if (anEdgeDef.IsDegenerate)
    {
      aPCurveData.Append(EdgePCurveData());
      continue;
    }

    const BRepGraph_TopoNode::EdgeDef::PCurveEntry* aPCurve =
      aGeom.FindPCurve(anEdgeDef.Id, aFaceNodeId);
    if (aPCurve == nullptr)
    {
      aPCurveData.Append(EdgePCurveData());
      continue;
    }

    if (aPCurve->Curve2d.IsNull())
    {
      aPCurveData.Append(EdgePCurveData());
      continue;
    }

    EdgePCurveData aData;
    aData.EdgeDefIdx = anEntry.EdgeDefId.Index;

    aData.StartVtxId = anEdgeDef.OrientedStartVertex(anEntry.OrientationInWire);
    aData.EndVtxId   = anEdgeDef.OrientedEndVertex(anEntry.OrientationInWire);

    const double aFirst = aPCurve->ParamFirst;
    const double aLast  = aPCurve->ParamLast;
    aData.Adaptor = new Geom2dAdaptor_Curve(aPCurve->Curve2d, aFirst, aLast);

    BndLib_Add2dCurve::Add(*aData.Adaptor, Precision::PConfusion(), aData.Box);

    aPCurveData.Append(aData);
  }

  // Self-intersection check: pairwise intersection of non-adjacent edges.
  for (int anI = 0; anI < aPCurveData.Length(); ++anI)
  {
    const EdgePCurveData& aDataI = aPCurveData.Value(anI);
    if (aDataI.Adaptor.IsNull())
      continue;

    for (int aJ = anI + 1; aJ < aPCurveData.Length(); ++aJ)
    {
      const EdgePCurveData& aDataJ = aPCurveData.Value(aJ);
      if (aDataJ.Adaptor.IsNull())
        continue;

      // Bounding box pruning.
      if (aDataI.Box.IsOut(aDataJ.Box))
        continue;

      // Check if edges are adjacent (share a vertex).
      const bool anIsAdjacent =
        (aDataI.EndVtxId.IsValid() && aDataI.EndVtxId == aDataJ.StartVtxId)
        || (aDataJ.EndVtxId.IsValid() && aDataJ.EndVtxId == aDataI.StartVtxId)
        || (aDataI.StartVtxId.IsValid() && aDataI.StartVtxId == aDataJ.EndVtxId)
        || (aDataJ.StartVtxId.IsValid() && aDataJ.StartVtxId == aDataI.EndVtxId);

      Geom2dInt_GInter anInter(*aDataI.Adaptor, *aDataJ.Adaptor,
                               Precision::PConfusion(), Precision::PConfusion());
      if (!anInter.IsDone())
        continue;

      // Check intersection points, filtering shared vertex intersections.
      for (int aPntIter = 1; aPntIter <= anInter.NbPoints(); ++aPntIter)
      {
        const IntRes2d_IntersectionPoint& anIntPnt = anInter.Point(aPntIter);
        const double aParamI = anIntPnt.ParamOnFirst();
        const double aParamJ = anIntPnt.ParamOnSecond();

        if (anIsAdjacent)
        {
          // Filter intersections at endpoints (shared vertices).
          const bool anIsAtEndpointI =
            std::abs(aParamI - aDataI.Adaptor->FirstParameter()) < Precision::PConfusion()
            || std::abs(aParamI - aDataI.Adaptor->LastParameter()) < Precision::PConfusion();
          const bool anIsAtEndpointJ =
            std::abs(aParamJ - aDataJ.Adaptor->FirstParameter()) < Precision::PConfusion()
            || std::abs(aParamJ - aDataJ.Adaptor->LastParameter()) < Precision::PConfusion();
          if (anIsAtEndpointI && anIsAtEndpointJ)
            continue;
        }

        // Non-vertex intersection found.
        BRepGraphCheck_Issue anIssue;
        anIssue.NodeId        = aWireNodeId;
        anIssue.ContextNodeId = aFaceNodeId;
        anIssue.Status        = BRepCheck_SelfIntersectingWire;
        anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
        theIssues.Append(anIssue);
        return; // Report once per wire.
      }

      // Check intersection segments.
      if (anInter.NbSegments() > 0)
      {
        if (!anIsAdjacent)
        {
          BRepGraphCheck_Issue anIssue;
          anIssue.NodeId        = aWireNodeId;
          anIssue.ContextNodeId = aFaceNodeId;
          anIssue.Status        = BRepCheck_SelfIntersectingWire;
          anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
          theIssues.Append(anIssue);
          return;
        }
      }
    }
  }
}
