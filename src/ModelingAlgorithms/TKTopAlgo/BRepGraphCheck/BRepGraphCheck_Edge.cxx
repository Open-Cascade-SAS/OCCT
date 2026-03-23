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

#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_TopoNode.hxx>
#include <BRepLib_ValidateEdge.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_Curve.hxx>
#include <Precision.hxx>
#include <TopAbs_Orientation.hxx>

#include <cmath>

//=================================================================================================

void BRepGraphCheck::CheckEdgeMinimum(
  const BRepGraph&                         theGraph,
  int                                      theEdgeDefIdx,
  NCollection_Vector<BRepGraphCheck_Issue>& theIssues)
{
  const BRepGraph::DefsView aDefs = theGraph.Defs();
  const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aDefs.Edge(theEdgeDefIdx);

  const bool anIsDegenerate = BRepGraph_Tool::Edge::Degenerated(theGraph, theEdgeDefIdx);
  const bool aHasCurve      = BRepGraph_Tool::Edge::HasCurve(theGraph, theEdgeDefIdx);

  // Check No3DCurve: non-degenerate edge must have a 3D curve.
  if (!anIsDegenerate && !aHasCurve)
  {
    BRepGraphCheck_Issue anIssue;
    anIssue.NodeId        = anEdgeDef.Id;
    anIssue.Status        = BRepCheck_No3DCurve;
    anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
    theIssues.Append(anIssue);
  }

  // Check degenerate edge flag consistency:
  // A degenerate edge should have no 3D curve.
  if (anIsDegenerate && aHasCurve)
  {
    BRepGraphCheck_Issue anIssue;
    anIssue.NodeId        = anEdgeDef.Id;
    anIssue.Status        = BRepCheck_InvalidDegeneratedFlag;
    anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
    theIssues.Append(anIssue);
  }

  // Check parameter range validity.
  if (aHasCurve)
  {
    const auto [aFirst, aLast] = BRepGraph_Tool::Edge::Range(theGraph, theEdgeDefIdx);

    // Parameters should be ordered (first < last for non-periodic,
    // or at least different for periodic curves).
    if (std::abs(aFirst - aLast) < Precision::PConfusion())
    {
      BRepGraphCheck_Issue anIssue;
      anIssue.NodeId        = anEdgeDef.Id;
      anIssue.Status        = BRepCheck_InvalidRange;
      anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
      theIssues.Append(anIssue);
    }
    else
    {
      const GeomAdaptor_TransformedCurve aCurveAdaptor =
        BRepGraph_Tool::Edge::CurveAdaptor(theGraph, theEdgeDefIdx);
      if (!aCurveAdaptor.IsPeriodic() && aFirst > aLast)
      {
        BRepGraphCheck_Issue anIssue;
        anIssue.NodeId        = anEdgeDef.Id;
        anIssue.Status        = BRepCheck_InvalidRange;
        anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
        theIssues.Append(anIssue);
      }
    }
  }

  // Check tolerance value.
  if (BRepGraph_Tool::Edge::Tolerance(theGraph, theEdgeDefIdx) < 0.0)
  {
    BRepGraphCheck_Issue anIssue;
    anIssue.NodeId        = anEdgeDef.Id;
    anIssue.Status        = BRepCheck_InvalidToleranceValue;
    anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
    theIssues.Append(anIssue);
  }

  // SameRange=false but SameParameter=true is invalid.
  if (BRepGraph_Tool::Edge::SameParameter(theGraph, theEdgeDefIdx)
      && !BRepGraph_Tool::Edge::SameRange(theGraph, theEdgeDefIdx))
  {
    BRepGraphCheck_Issue anIssue;
    anIssue.NodeId        = anEdgeDef.Id;
    anIssue.Status        = BRepCheck_InvalidSameParameterFlag;
    anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
    theIssues.Append(anIssue);
  }

  // Vertex tolerances should not exceed edge tolerance unreasonably.
  if (anEdgeDef.StartVertexDefId().IsValid())
  {
    const int aStartVtxIdx = anEdgeDef.StartVertex.VertexIdx;
    if (BRepGraph_Tool::Vertex::Tolerance(theGraph, aStartVtxIdx) < Precision::Confusion())
    {
      BRepGraphCheck_Issue anIssue;
      anIssue.NodeId        = aDefs.Vertex(aStartVtxIdx).Id;
      anIssue.Status        = BRepCheck_InvalidToleranceValue;
      anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Warning;
      theIssues.Append(anIssue);
    }
  }
  if (anEdgeDef.EndVertexDefId().IsValid())
  {
    const int anEndVtxIdx = anEdgeDef.EndVertex.VertexIdx;
    if (BRepGraph_Tool::Vertex::Tolerance(theGraph, anEndVtxIdx) < Precision::Confusion())
    {
      BRepGraphCheck_Issue anIssue;
      anIssue.NodeId        = aDefs.Vertex(anEndVtxIdx).Id;
      anIssue.Status        = BRepCheck_InvalidToleranceValue;
      anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Warning;
      theIssues.Append(anIssue);
    }
  }
}

//=================================================================================================

void BRepGraphCheck::CheckEdgeOnFace(
  const BRepGraph&                         theGraph,
  int                                      theEdgeDefIdx,
  int                                      theFaceDefIdx,
  bool                                     theIsExact,
  NCollection_Vector<BRepGraphCheck_Issue>& theIssues)
{
  const BRepGraph::DefsView aDefs = theGraph.Defs();
  const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aDefs.Edge(theEdgeDefIdx);

  // Skip degenerate edges for PCurve/SameParameter checks.
  if (BRepGraph_Tool::Edge::Degenerated(theGraph, theEdgeDefIdx))
    return;

  if (!BRepGraph_Tool::Face::HasSurface(theGraph, theFaceDefIdx))
    return;

  const BRepGraph_NodeId aFaceNodeId = BRepGraph_NodeId::Face(theFaceDefIdx);

  // Check PCurve existence.
  const BRepGraphInc::CoEdgeEntity* aPCurve =
    BRepGraph_Tool::Edge::FindPCurve(theGraph, theEdgeDefIdx, theFaceDefIdx);
  if (aPCurve == nullptr)
  {
    BRepGraphCheck_Issue anIssue;
    anIssue.NodeId        = anEdgeDef.Id;
    anIssue.ContextNodeId = aFaceNodeId;
    anIssue.Status        = BRepCheck_NoCurveOnSurface;
    anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
    theIssues.Append(anIssue);
    return;
  }

  // Check SameRange flag: PCurve range should match edge parameter range.
  const auto [anEdgeFirst, anEdgeLast] = BRepGraph_Tool::Edge::Range(theGraph, theEdgeDefIdx);
  if (aPCurve->Curve2DRepIdx >= 0 && BRepGraph_Tool::Edge::SameRange(theGraph, theEdgeDefIdx))
  {
    const double aPCFirst = aPCurve->ParamFirst;
    const double aPCLast  = aPCurve->ParamLast;
    if (std::abs(aPCFirst - anEdgeFirst) > Precision::PConfusion()
        || std::abs(aPCLast - anEdgeLast) > Precision::PConfusion())
    {
      BRepGraphCheck_Issue anIssue;
      anIssue.NodeId        = anEdgeDef.Id;
      anIssue.ContextNodeId = aFaceNodeId;
      anIssue.Status        = BRepCheck_InvalidSameRangeFlag;
      anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
      theIssues.Append(anIssue);
    }
  }

  // Seam edge check: for edges on closed surfaces, validate both PCurves.
  // A seam edge has two PCurves on the same face (FORWARD and REVERSED).
  {
    const BRepGraphInc::CoEdgeEntity* aPCurveF =
      BRepGraph_Tool::Edge::FindPCurve(theGraph, theEdgeDefIdx, theFaceDefIdx, TopAbs_FORWARD);
    const BRepGraphInc::CoEdgeEntity* aPCurveR =
      BRepGraph_Tool::Edge::FindPCurve(theGraph, theEdgeDefIdx, theFaceDefIdx, TopAbs_REVERSED);
    if (aPCurveF != nullptr && aPCurveR != nullptr && aPCurveF != aPCurveR)
    {
      // Both PCurves exist — this is a seam edge. Validate the second PCurve.
      if (aPCurveR->Curve2DRepIdx < 0)
      {
        BRepGraphCheck_Issue anIssue;
        anIssue.NodeId        = anEdgeDef.Id;
        anIssue.ContextNodeId = aFaceNodeId;
        anIssue.Status        = BRepCheck_InvalidCurveOnClosedSurface;
        anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
        theIssues.Append(anIssue);
      }
      else if (BRepGraph_Tool::Edge::SameRange(theGraph, theEdgeDefIdx))
      {
        // Validate SameRange for the second PCurve as well.
        const double aPCFirst2 = aPCurveR->ParamFirst;
        const double aPCLast2  = aPCurveR->ParamLast;
        if (std::abs(aPCFirst2 - anEdgeFirst) > Precision::PConfusion()
            || std::abs(aPCLast2 - anEdgeLast) > Precision::PConfusion())
        {
          BRepGraphCheck_Issue anIssue;
          anIssue.NodeId        = anEdgeDef.Id;
          anIssue.ContextNodeId = aFaceNodeId;
          anIssue.Status        = BRepCheck_InvalidSameRangeFlag;
          anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
          theIssues.Append(anIssue);
        }
      }
    }
  }

  // SameParameter check via BRepLib_ValidateEdge.
  if (!BRepGraph_Tool::Edge::HasCurve(theGraph, theEdgeDefIdx))
    return;

  if (!BRepGraph_Tool::Edge::SameParameter(theGraph, theEdgeDefIdx))
    return; // Cannot validate if not flagged as SameParameter.

  if (aPCurve->Curve2DRepIdx < 0)
    return;

  const occ::handle<Geom2d_Curve>& aPCurve2d =
    BRepGraph_Tool::CoEdge::PCurve(theGraph, *aPCurve);
  if (aPCurve2d.IsNull())
    return;

  // Build 3D curve adaptor (raw handle needed for BRepLib_ValidateEdge).
  const occ::handle<Geom_Curve>& aEdgeCurve3d =
    BRepGraph_Tool::Edge::Curve(theGraph, theEdgeDefIdx);
  occ::handle<GeomAdaptor_Curve> aRefCurve = new GeomAdaptor_Curve(
    aEdgeCurve3d, anEdgeFirst, anEdgeLast);

  // Build curve-on-surface adaptor from PCurve + Surface (raw handles needed).
  occ::handle<Geom2dAdaptor_Curve> aPC2d = new Geom2dAdaptor_Curve(
    aPCurve2d, anEdgeFirst, anEdgeLast);
  const occ::handle<Geom_Surface>& aFaceSurface =
    BRepGraph_Tool::Face::Surface(theGraph, theFaceDefIdx);
  occ::handle<GeomAdaptor_Surface> aHS = new GeomAdaptor_Surface(aFaceSurface);
  occ::handle<Adaptor3d_CurveOnSurface> aCOS = new Adaptor3d_CurveOnSurface(aPC2d, aHS);

  const bool   anIsSameParam = BRepGraph_Tool::Edge::SameParameter(theGraph, theEdgeDefIdx);
  const double anEdgeTol     = BRepGraph_Tool::Edge::Tolerance(theGraph, theEdgeDefIdx);
  BRepLib_ValidateEdge aValidator(aRefCurve, aCOS, anIsSameParam);
  aValidator.SetExitIfToleranceExceeded(anEdgeTol);
  aValidator.SetExactMethod(theIsExact);
  aValidator.Process();

  if (aValidator.IsDone() && !aValidator.CheckTolerance(anEdgeTol))
  {
    BRepGraphCheck_Issue anIssue;
    anIssue.NodeId        = anEdgeDef.Id;
    anIssue.ContextNodeId = aFaceNodeId;
    anIssue.Status        = BRepCheck_InvalidCurveOnSurface;
    anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
    theIssues.Append(anIssue);
  }
}

//=================================================================================================

void BRepGraphCheck::CheckEdgeInShell(
  const BRepGraph&                         theGraph,
  int                                      theEdgeDefIdx,
  int                                      theShellDefIdx,
  int                                      theEdgeFaceCount,
  NCollection_Vector<BRepGraphCheck_Issue>& theIssues)
{
  const BRepGraph::DefsView aDefs = theGraph.Defs();
  const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aDefs.Edge(theEdgeDefIdx);
  const BRepGraph_NodeId aShellNodeId = BRepGraph_NodeId::Shell(theShellDefIdx);

  // Skip degenerate edges.
  if (BRepGraph_Tool::Edge::Degenerated(theGraph, theEdgeDefIdx))
    return;

  if (theEdgeFaceCount == 1)
  {
    BRepGraphCheck_Issue anIssue;
    anIssue.NodeId        = anEdgeDef.Id;
    anIssue.ContextNodeId = aShellNodeId;
    anIssue.Status        = BRepCheck_FreeEdge;
    anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Warning;
    theIssues.Append(anIssue);
  }
  else if (theEdgeFaceCount > 2)
  {
    BRepGraphCheck_Issue anIssue;
    anIssue.NodeId        = anEdgeDef.Id;
    anIssue.ContextNodeId = aShellNodeId;
    anIssue.Status        = BRepCheck_InvalidMultiConnexity;
    anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
    theIssues.Append(anIssue);
  }
}
