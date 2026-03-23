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
#include <BRepGraph_GeomNode.hxx>
#include <BRepGraph_GeomView.hxx>
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

  // Check No3DCurve: non-degenerate edge must have a 3D curve.
  if (!anEdgeDef.IsDegenerate && !anEdgeDef.CurveNodeId.IsValid())
  {
    BRepGraphCheck_Issue anIssue;
    anIssue.NodeId        = anEdgeDef.Id;
    anIssue.Status        = BRepCheck_No3DCurve;
    anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
    theIssues.Append(anIssue);
  }

  // Check degenerate edge flag consistency:
  // A degenerate edge should have no 3D curve.
  if (anEdgeDef.IsDegenerate && anEdgeDef.CurveNodeId.IsValid())
  {
    const BRepGraph::GeomView aGeom = theGraph.Geom();
    const BRepGraph_GeomNode::Curve& aCurveNode = aGeom.Curve(anEdgeDef.CurveNodeId.Index);
    if (!aCurveNode.CurveGeom.IsNull())
    {
      BRepGraphCheck_Issue anIssue;
      anIssue.NodeId        = anEdgeDef.Id;
      anIssue.Status        = BRepCheck_InvalidDegeneratedFlag;
      anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
      theIssues.Append(anIssue);
    }
  }

  // Check parameter range validity.
  if (anEdgeDef.CurveNodeId.IsValid())
  {
    const BRepGraph::GeomView aGeom = theGraph.Geom();
    const BRepGraph_GeomNode::Curve& aCurveNode = aGeom.Curve(anEdgeDef.CurveNodeId.Index);
    if (!aCurveNode.CurveGeom.IsNull())
    {
      const double aFirst = anEdgeDef.ParamFirst;
      const double aLast  = anEdgeDef.ParamLast;

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
      else if (!aCurveNode.CurveGeom->IsPeriodic() && aFirst > aLast)
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
  if (anEdgeDef.Tolerance < 0.0)
  {
    BRepGraphCheck_Issue anIssue;
    anIssue.NodeId        = anEdgeDef.Id;
    anIssue.Status        = BRepCheck_InvalidToleranceValue;
    anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
    theIssues.Append(anIssue);
  }

  // SameRange=false but SameParameter=true is invalid.
  if (anEdgeDef.SameParameter && !anEdgeDef.SameRange)
  {
    BRepGraphCheck_Issue anIssue;
    anIssue.NodeId        = anEdgeDef.Id;
    anIssue.Status        = BRepCheck_InvalidSameParameterFlag;
    anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
    theIssues.Append(anIssue);
  }

  // Vertex tolerances should not exceed edge tolerance unreasonably.
  if (anEdgeDef.StartVertexDefId.IsValid())
  {
    const BRepGraph_TopoNode::VertexDef& aStartVtx = aDefs.Vertex(anEdgeDef.StartVertexDefId.Index);
    if (aStartVtx.Tolerance < Precision::Confusion())
    {
      BRepGraphCheck_Issue anIssue;
      anIssue.NodeId        = aStartVtx.Id;
      anIssue.Status        = BRepCheck_InvalidToleranceValue;
      anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Warning;
      theIssues.Append(anIssue);
    }
  }
  if (anEdgeDef.EndVertexDefId.IsValid())
  {
    const BRepGraph_TopoNode::VertexDef& anEndVtx = aDefs.Vertex(anEdgeDef.EndVertexDefId.Index);
    if (anEndVtx.Tolerance < Precision::Confusion())
    {
      BRepGraphCheck_Issue anIssue;
      anIssue.NodeId        = anEndVtx.Id;
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
  const BRepGraph_TopoNode::FaceDef& aFaceDef  = aDefs.Face(theFaceDefIdx);

  // Skip degenerate edges for PCurve/SameParameter checks.
  if (anEdgeDef.IsDegenerate)
    return;

  if (!aFaceDef.SurfNodeId.IsValid())
    return;

  const BRepGraph::GeomView aGeom = theGraph.Geom();
  const BRepGraph_NodeId    aEdgeNodeId = BRepGraph_NodeId::Edge(theEdgeDefIdx);
  const BRepGraph_NodeId    aFaceNodeId = BRepGraph_NodeId::Face(theFaceDefIdx);

  // Check PCurve existence.
  const BRepGraph_TopoNode::EdgeDef::PCurveEntry* aPCurve =
    aGeom.FindPCurve(aEdgeNodeId, aFaceNodeId);
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
  if (!aPCurve->Curve2d.IsNull() && anEdgeDef.SameRange)
  {
    const double aPCFirst = aPCurve->ParamFirst;
    const double aPCLast  = aPCurve->ParamLast;
    if (std::abs(aPCFirst - anEdgeDef.ParamFirst) > Precision::PConfusion()
        || std::abs(aPCLast - anEdgeDef.ParamLast) > Precision::PConfusion())
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
    const BRepGraph_TopoNode::EdgeDef::PCurveEntry* aPCurveF =
      aGeom.FindPCurve(aEdgeNodeId, aFaceNodeId, TopAbs_FORWARD);
    const BRepGraph_TopoNode::EdgeDef::PCurveEntry* aPCurveR =
      aGeom.FindPCurve(aEdgeNodeId, aFaceNodeId, TopAbs_REVERSED);
    if (aPCurveF != nullptr && aPCurveR != nullptr && aPCurveF != aPCurveR)
    {
      // Both PCurves exist — this is a seam edge. Validate the second PCurve.
      if (aPCurveR->Curve2d.IsNull())
      {
        BRepGraphCheck_Issue anIssue;
        anIssue.NodeId        = anEdgeDef.Id;
        anIssue.ContextNodeId = aFaceNodeId;
        anIssue.Status        = BRepCheck_InvalidCurveOnClosedSurface;
        anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
        theIssues.Append(anIssue);
      }
      else if (anEdgeDef.SameRange)
      {
        // Validate SameRange for the second PCurve as well.
        const double aPCFirst2 = aPCurveR->ParamFirst;
        const double aPCLast2  = aPCurveR->ParamLast;
        if (std::abs(aPCFirst2 - anEdgeDef.ParamFirst) > Precision::PConfusion()
            || std::abs(aPCLast2 - anEdgeDef.ParamLast) > Precision::PConfusion())
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
  if (!anEdgeDef.CurveNodeId.IsValid())
    return;

  if (!anEdgeDef.SameParameter)
    return; // Cannot validate if not flagged as SameParameter.

  const BRepGraph_GeomNode::Curve& aCurveNode = aGeom.Curve(anEdgeDef.CurveNodeId.Index);
  if (aCurveNode.CurveGeom.IsNull())
    return;

  if (aPCurve->Curve2d.IsNull())
    return;

  const BRepGraph_GeomNode::Surf& aSurfNode = aGeom.Surface(aFaceDef.SurfNodeId.Index);
  if (aSurfNode.Surface.IsNull())
    return;

  // Build 3D curve adaptor.
  Handle(GeomAdaptor_Curve) aRefCurve = new GeomAdaptor_Curve(
    aCurveNode.CurveGeom, anEdgeDef.ParamFirst, anEdgeDef.ParamLast);

  // Build curve-on-surface adaptor from PCurve + Surface.
  Handle(Geom2dAdaptor_Curve) aPC2d = new Geom2dAdaptor_Curve(
    aPCurve->Curve2d, anEdgeDef.ParamFirst, anEdgeDef.ParamLast);
  Handle(GeomAdaptor_Surface) aHS = new GeomAdaptor_Surface(aSurfNode.Surface);
  Handle(Adaptor3d_CurveOnSurface) aCOS = new Adaptor3d_CurveOnSurface(aPC2d, aHS);

  BRepLib_ValidateEdge aValidator(aRefCurve, aCOS, anEdgeDef.SameParameter);
  aValidator.SetExitIfToleranceExceeded(anEdgeDef.Tolerance);
  aValidator.SetExactMethod(theIsExact);
  aValidator.Process();

  if (aValidator.IsDone() && !aValidator.CheckTolerance(anEdgeDef.Tolerance))
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
  if (anEdgeDef.IsDegenerate)
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
