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

#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_TopoNode.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt.hxx>

//=================================================================================================

void BRepGraphCheck::CheckVertexOnEdge(
  const BRepGraph&                         theGraph,
  int                                      theVertexDefIdx,
  int                                      theEdgeDefIdx,
  NCollection_Vector<BRepGraphCheck_Issue>& theIssues)
{
  const BRepGraph::DefsView aDefs = theGraph.Defs();
  const BRepGraph_TopoNode::VertexDef& aVtxDef   = aDefs.Vertex(theVertexDefIdx);
  const BRepGraph_TopoNode::EdgeDef&   anEdgeDef = aDefs.Edge(theEdgeDefIdx);

  // Skip degenerate edges.
  if (BRepGraph_Tool::Edge::Degenerated(theGraph, theEdgeDefIdx))
    return;

  // No 3D curve — cannot check vertex position.
  if (!BRepGraph_Tool::Edge::HasCurve(theGraph, theEdgeDefIdx))
    return;

  // Determine parameter for this vertex on the curve.
  double aParam = 0.0;
  if (anEdgeDef.StartVertexDefId().IsValid()
      && anEdgeDef.StartVertex.VertexIdx == theVertexDefIdx)
  {
    aParam = anEdgeDef.ParamFirst;
  }
  else if (anEdgeDef.EndVertexDefId().IsValid()
           && anEdgeDef.EndVertex.VertexIdx == theVertexDefIdx)
  {
    aParam = anEdgeDef.ParamLast;
  }
  else
  {
    return; // Vertex not at endpoint of this edge.
  }

  // Evaluate curve at parameter (geometry is stored at identity).
  const GeomAdaptor_TransformedCurve aCurveAdaptor =
    BRepGraph_Tool::Edge::CurveAdaptor(theGraph, theEdgeDefIdx);
  const gp_Pnt aCurvePnt = aCurveAdaptor.Value(aParam);

  const gp_Pnt aVtxPnt = BRepGraph_Tool::Vertex::Pnt(theGraph, theVertexDefIdx);
  const double aDist   = aVtxPnt.Distance(aCurvePnt);
  if (aDist > BRepGraph_Tool::Edge::Tolerance(theGraph, theEdgeDefIdx))
  {
    BRepGraphCheck_Issue anIssue;
    anIssue.NodeId        = aVtxDef.Id;
    anIssue.ContextNodeId = anEdgeDef.Id;
    anIssue.Status        = BRepCheck_InvalidPointOnCurve;
    anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
    theIssues.Append(anIssue);
  }
}

//=================================================================================================

void BRepGraphCheck::CheckVertexOnFace(
  const BRepGraph&                         theGraph,
  int                                      theVertexDefIdx,
  int                                      theFaceDefIdx,
  NCollection_Vector<BRepGraphCheck_Issue>& theIssues)
{
  const BRepGraph::DefsView aDefs = theGraph.Defs();
  const BRepGraph_TopoNode::VertexDef& aVtxDef  = aDefs.Vertex(theVertexDefIdx);

  if (!BRepGraph_Tool::Face::HasSurface(theGraph, theFaceDefIdx))
    return;

  // Check vertex through PCurve evaluation: find an edge of this face
  // that has this vertex as endpoint.
  const int aNbEdges = aDefs.NbEdges();
  for (int anEdgeIter = 0; anEdgeIter < aNbEdges; ++anEdgeIter)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aDefs.Edge(anEdgeIter);

    bool aIsEndpoint = false;
    double aParam = 0.0;

    if (anEdgeDef.StartVertexDefId().IsValid()
        && anEdgeDef.StartVertex.VertexIdx == theVertexDefIdx)
    {
      aIsEndpoint = true;
      aParam = anEdgeDef.ParamFirst;
    }
    else if (anEdgeDef.EndVertexDefId().IsValid()
             && anEdgeDef.EndVertex.VertexIdx == theVertexDefIdx)
    {
      aIsEndpoint = true;
      aParam = anEdgeDef.ParamLast;
    }

    if (!aIsEndpoint)
      continue;

    // Look for PCurve of this edge on this face.
    const BRepGraphInc::CoEdgeEntity* aPCurve =
      BRepGraph_Tool::Edge::FindPCurve(theGraph, anEdgeIter, theFaceDefIdx);
    if (aPCurve == nullptr)
      continue;

    if (aPCurve->Curve2DRepIdx < 0)
      continue;

    const occ::handle<Geom2d_Curve>& aVtxPCurve =
      BRepGraph_Tool::CoEdge::PCurve(theGraph, *aPCurve);
    // Evaluate PCurve at parameter to get UV, then evaluate surface (geometry at identity).
    const gp_Pnt2d aUV = aVtxPCurve->Value(aParam);
    const GeomAdaptor_TransformedSurface aSurfAdaptor =
      BRepGraph_Tool::Face::SurfaceAdaptor(theGraph, theFaceDefIdx);
    const gp_Pnt aSurfPnt = aSurfAdaptor.Value(aUV.X(), aUV.Y());

    const gp_Pnt  aVtxPnt = BRepGraph_Tool::Vertex::Pnt(theGraph, theVertexDefIdx);
    const double  aDist   = aVtxPnt.Distance(aSurfPnt);
    const double  aVtxTol = BRepGraph_Tool::Vertex::Tolerance(theGraph, theVertexDefIdx);
    const double  aEdgTol = BRepGraph_Tool::Edge::Tolerance(theGraph, anEdgeIter);
    if (aDist > aVtxTol + aEdgTol)
    {
      BRepGraphCheck_Issue anIssue;
      anIssue.NodeId        = aVtxDef.Id;
      anIssue.ContextNodeId = BRepGraph_NodeId::Face(theFaceDefIdx);
      anIssue.Status        = BRepCheck_InvalidPointOnCurveOnSurface;
      anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
      theIssues.Append(anIssue);
    }

    // Only check through the first matching edge.
    return;
  }
}
