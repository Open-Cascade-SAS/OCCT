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
#include <BRepGraphInc_IncidenceRef.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_List.hxx>
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
  if (aWireDef.CoEdgeRefs.IsEmpty())
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
  for (int aCoEdgeIter = 0; aCoEdgeIter < aWireDef.CoEdgeRefs.Length(); ++aCoEdgeIter)
  {
    const BRepGraphInc::CoEdgeRef& aCR = aWireDef.CoEdgeRefs.Value(aCoEdgeIter);
    const BRepGraph_TopoNode::CoEdgeDef& aCoEdgeDef = aDefs.CoEdge(aCR.CoEdgeIdx);
    const int anEdgeIdx = aCoEdgeDef.EdgeIdx;
    if (!anEdgeCounts.IsBound(anEdgeIdx))
    {
      anEdgeCounts.Bind(anEdgeIdx, 1);
      anEdgeForwardCounts.Bind(anEdgeIdx, 0);
    }
    else
    {
      anEdgeCounts.ChangeFind(anEdgeIdx)++;
    }
    if (aCoEdgeDef.Sense == TopAbs_FORWARD)
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

  if (aWireDef.CoEdgeRefs.IsEmpty())
    return;

  // Connectivity check: verify all edges are reachable via vertex adjacency.
  // Uses the same approach as BRepCheck_Wire::Closed() — build a vertex-to-edge
  // adjacency map, then propagate from the first edge to check all are reachable.
  // This works regardless of CoEdgeRefs ordering.
  const int aNbEdges = aWireDef.CoEdgeRefs.Length();
  {
    // Build vertex → edge-indices map (using oriented vertices).
    NCollection_DataMap<int, NCollection_Vector<int>> aVtxToEdgeIndices;
    for (int anEdgeIter = 0; anEdgeIter < aNbEdges; ++anEdgeIter)
    {
      const BRepGraphInc::CoEdgeRef& aCR = aWireDef.CoEdgeRefs.Value(anEdgeIter);
      const BRepGraph_TopoNode::CoEdgeDef& aCoEdgeDef = aDefs.CoEdge(aCR.CoEdgeIdx);
      const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aDefs.Edge(aCoEdgeDef.EdgeIdx);
      const BRepGraph_NodeId aStartVtx = anEdgeDef.OrientedStartVertex(aCoEdgeDef.Sense);
      const BRepGraph_NodeId anEndVtx  = anEdgeDef.OrientedEndVertex(aCoEdgeDef.Sense);
      if (aStartVtx.IsValid())
      {
        if (!aVtxToEdgeIndices.IsBound(aStartVtx.Index))
          aVtxToEdgeIndices.Bind(aStartVtx.Index, NCollection_Vector<int>());
        aVtxToEdgeIndices.ChangeFind(aStartVtx.Index).Append(anEdgeIter);
      }
      if (anEndVtx.IsValid())
      {
        if (!aVtxToEdgeIndices.IsBound(anEndVtx.Index))
          aVtxToEdgeIndices.Bind(anEndVtx.Index, NCollection_Vector<int>());
        aVtxToEdgeIndices.ChangeFind(anEndVtx.Index).Append(anEdgeIter);
      }
    }

    // Propagate from edge 0: mark all reachable edges via shared vertices.
    NCollection_Vector<bool> aVisited(aNbEdges);
    for (int i = 0; i < aNbEdges; ++i)
      aVisited.SetValue(i, false);

    NCollection_List<int> aQueue;
    aQueue.Append(0);
    aVisited.SetValue(0, true);
    int aNbVisited = 1;

    while (!aQueue.IsEmpty())
    {
      const int aCurrIdx = aQueue.First();
      aQueue.RemoveFirst();

      const BRepGraphInc::CoEdgeRef& aCurrCR = aWireDef.CoEdgeRefs.Value(aCurrIdx);
      const BRepGraph_TopoNode::CoEdgeDef& aCurrCoEdge = aDefs.CoEdge(aCurrCR.CoEdgeIdx);
      const BRepGraph_TopoNode::EdgeDef& aCurrEdge = aDefs.Edge(aCurrCoEdge.EdgeIdx);
      const BRepGraph_NodeId aStartVtx = aCurrEdge.OrientedStartVertex(aCurrCoEdge.Sense);
      const BRepGraph_NodeId anEndVtx  = aCurrEdge.OrientedEndVertex(aCurrCoEdge.Sense);

      // Visit neighbors via both vertices.
      for (int aVtxPass = 0; aVtxPass < 2; ++aVtxPass)
      {
        const int aVtxIdx = (aVtxPass == 0 && aStartVtx.IsValid()) ? aStartVtx.Index
                          : (aVtxPass == 1 && anEndVtx.IsValid())   ? anEndVtx.Index
                                                                     : -1;
        if (aVtxIdx < 0)
          continue;
        const NCollection_Vector<int>* aNeighbors = aVtxToEdgeIndices.Seek(aVtxIdx);
        if (aNeighbors == nullptr)
          continue;
        for (int i = 0; i < aNeighbors->Length(); ++i)
        {
          const int aNeighIdx = aNeighbors->Value(i);
          if (!aVisited.Value(aNeighIdx))
          {
            aVisited.SetValue(aNeighIdx, true);
            ++aNbVisited;
            aQueue.Append(aNeighIdx);
          }
        }
      }
    }

    if (aNbVisited < aNbEdges)
    {
      BRepGraphCheck_Issue anIssue;
      anIssue.NodeId        = aWireNodeId;
      anIssue.ContextNodeId = aFaceNodeId;
      anIssue.Status        = BRepCheck_NotConnected;
      anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
      theIssues.Append(anIssue);
    }

    // Closure check: each vertex must appear exactly twice (once as start, once as end).
    // For closed wires, every vertex should have even degree.
    if (aWireDef.IsClosed && aNbEdges > 1)
    {
      bool aIsClosed = true;
      for (NCollection_DataMap<int, NCollection_Vector<int>>::Iterator aVtxIt(aVtxToEdgeIndices);
           aVtxIt.More(); aVtxIt.Next())
      {
        if (aVtxIt.Value().Length() % 2 != 0)
        {
          aIsClosed = false;
          break;
        }
      }
      if (!aIsClosed)
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

  // Geometric controls: 2D parametric closure and self-intersection detection.
  if (!theGeomControls)
    return;

  // 2D parametric closure check: verify UV endpoints of first and last edges meet.
  // Skip for wires containing seam edges (coedges with a seam pair on the same face),
  // because their UV closure works through periodic surface wrapping and the
  // vertex-degree closure check above is sufficient for such wires.
  bool aHasSeamEdge = false;
  for (int anEdgeIter = 0; anEdgeIter < aNbEdges && !aHasSeamEdge; ++anEdgeIter)
  {
    const BRepGraphInc::CoEdgeRef& aCR = aWireDef.CoEdgeRefs.Value(anEdgeIter);
    const BRepGraph_TopoNode::CoEdgeDef& aCoEdgeDef = aDefs.CoEdge(aCR.CoEdgeIdx);
    if (aCoEdgeDef.SeamPairIdx >= 0)
      aHasSeamEdge = true;
  }
  if (aWireDef.IsClosed && aNbEdges > 1
      && BRepGraph_Tool::HasSurface(theGraph, theFaceDefIdx) && !aHasSeamEdge)
  {
    auto edgeLookup = [&aDefs](int theIdx) -> const BRepGraphInc::EdgeEntity& {
      return aDefs.Edge(theIdx);
    };
    auto coedgeLookup = [&aDefs](int theIdx) -> const BRepGraphInc::CoEdgeEntity& {
      return aDefs.CoEdge(theIdx);
    };
    BRepGraphInc_WireExplorer aWireExp(aWireDef.CoEdgeRefs, coedgeLookup, edgeLookup);
    BRepGraphInc::CoEdgeRef aFirstCR = aWireExp.CurrentRef();
    BRepGraphInc::CoEdgeRef aLastCR  = aFirstCR;
    for (; aWireExp.More(); aWireExp.Next())
      aLastCR = aWireExp.CurrentRef();

    const BRepGraph_TopoNode::CoEdgeDef& aFirstCoEdge = aDefs.CoEdge(aFirstCR.CoEdgeIdx);
    const BRepGraph_TopoNode::CoEdgeDef& aLastCoEdge  = aDefs.CoEdge(aLastCR.CoEdgeIdx);

    // Get PCurves for first and last edges (orientation-specific for seam edges).
    const BRepGraphInc::CoEdgeEntity* aFirstPC =
      BRepGraph_Tool::FindPCurve(theGraph, aFirstCoEdge.EdgeIdx,
                                 theFaceDefIdx, aFirstCoEdge.Sense);
    const BRepGraphInc::CoEdgeEntity* aLastPC =
      BRepGraph_Tool::FindPCurve(theGraph, aLastCoEdge.EdgeIdx,
                                 theFaceDefIdx, aLastCoEdge.Sense);

    if (aFirstPC != nullptr && aLastPC != nullptr)
    {
      if (aFirstPC->Curve2DRepIdx >= 0 && aLastPC->Curve2DRepIdx >= 0)
      {
        const occ::handle<Geom2d_Curve>& aFirstPCCurve =
          BRepGraph_Tool::PCurve(theGraph, *aFirstPC);
        const occ::handle<Geom2d_Curve>& aLastPCCurve =
          BRepGraph_Tool::PCurve(theGraph, *aLastPC);
        // Evaluate UV at wire-start of first edge and wire-end of last edge.
        const double aFirstParam = (aFirstCoEdge.Sense == TopAbs_FORWARD)
                                     ? aFirstPC->ParamFirst
                                     : aFirstPC->ParamLast;
        const double aLastParam = (aLastCoEdge.Sense == TopAbs_FORWARD)
                                    ? aLastPC->ParamLast
                                    : aLastPC->ParamFirst;

        const gp_Pnt2d aFirstUV = aFirstPCCurve->Value(aFirstParam);
        const gp_Pnt2d aLastUV  = aLastPCCurve->Value(aLastParam);

        // Convert 3D tolerance to UV tolerance using surface adaptor.
        {
          const double aTol3d = Precision::Confusion();
          const GeomAdaptor_TransformedSurface aSurfAdaptor =
            BRepGraph_Tool::SurfaceAdaptor(theGraph, theFaceDefIdx);
          const double aUTol = aSurfAdaptor.UResolution(aTol3d);
          const double aVTol = aSurfAdaptor.VResolution(aTol3d);

          double aDeltaU = std::abs(aFirstUV.X() - aLastUV.X());
          double aDeltaV = std::abs(aFirstUV.Y() - aLastUV.Y());

          // Account for period wrapping on periodic surfaces.
          if (aSurfAdaptor.IsUPeriodic())
          {
            const double aUPeriod = aSurfAdaptor.UPeriod();
            if (aDeltaU > aUPeriod * 0.5)
              aDeltaU = aUPeriod - aDeltaU;
          }
          if (aSurfAdaptor.IsVPeriodic())
          {
            const double aVPeriod = aSurfAdaptor.VPeriod();
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

  if (!BRepGraph_Tool::HasSurface(theGraph, theFaceDefIdx))
    return;

  // Collect PCurve adaptors and bounding boxes for each edge in the wire.
  struct EdgePCurveData
  {
    occ::handle<Geom2dAdaptor_Curve> Adaptor;
    Bnd_Box2d                   Box;
    int                         EdgeDefIdx;
    BRepGraph_NodeId            StartVtxId;
    BRepGraph_NodeId            EndVtxId;
  };
  NCollection_Vector<EdgePCurveData> aPCurveData;

  for (int anEdgeIter = 0; anEdgeIter < aNbEdges; ++anEdgeIter)
  {
    const BRepGraphInc::CoEdgeRef& aCR = aWireDef.CoEdgeRefs.Value(anEdgeIter);
    const BRepGraph_TopoNode::CoEdgeDef& aCoEdgeDef = aDefs.CoEdge(aCR.CoEdgeIdx);
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aDefs.Edge(aCoEdgeDef.EdgeIdx);

    // Skip degenerate edges.
    if (BRepGraph_Tool::Degenerated(theGraph, aCoEdgeDef.EdgeIdx))
    {
      aPCurveData.Append(EdgePCurveData());
      continue;
    }

    const BRepGraphInc::CoEdgeEntity* aPCurve =
      BRepGraph_Tool::FindPCurve(theGraph, aCoEdgeDef.EdgeIdx, theFaceDefIdx);
    if (aPCurve == nullptr)
    {
      aPCurveData.Append(EdgePCurveData());
      continue;
    }

    if (aPCurve->Curve2DRepIdx < 0)
    {
      aPCurveData.Append(EdgePCurveData());
      continue;
    }

    const occ::handle<Geom2d_Curve>& aWirePC2d =
      BRepGraph_Tool::PCurve(theGraph, *aPCurve);
    if (aWirePC2d.IsNull())
    {
      aPCurveData.Append(EdgePCurveData());
      continue;
    }

    EdgePCurveData aData;
    aData.EdgeDefIdx = aCoEdgeDef.EdgeIdx;

    aData.StartVtxId = anEdgeDef.OrientedStartVertex(aCoEdgeDef.Sense);
    aData.EndVtxId   = anEdgeDef.OrientedEndVertex(aCoEdgeDef.Sense);

    const double aFirst = aPCurve->ParamFirst;
    const double aLast  = aPCurve->ParamLast;
    aData.Adaptor = new Geom2dAdaptor_Curve(aWirePC2d, aFirst, aLast);

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
