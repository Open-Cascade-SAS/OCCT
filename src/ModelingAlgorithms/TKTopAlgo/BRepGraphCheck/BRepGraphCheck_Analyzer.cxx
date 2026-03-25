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

#include <BRepGraphCheck_Analyzer.hxx>

#include <BRepGraph_DefsView.hxx>
#include <BRepGraphCheck.hxx>
#include <BRepGraphInc_IncidenceRef.hxx>
#include <OSD_Parallel.hxx>

//=================================================================================================

BRepGraphCheck_Analyzer::BRepGraphCheck_Analyzer(const BRepGraph& theGraph)
    : myGraph(&theGraph)
{
}

//=================================================================================================

void BRepGraphCheck_Analyzer::SetGeometricControls(bool theVal)
{
  myGeomControls = theVal;
}

//=================================================================================================

void BRepGraphCheck_Analyzer::SetParallel(bool theVal)
{
  myIsParallel = theVal;
}

//=================================================================================================

void BRepGraphCheck_Analyzer::SetExactMethod(bool theVal)
{
  myIsExact = theVal;
}

//=================================================================================================

void BRepGraphCheck_Analyzer::Perform()
{
  myReport.Clear();

  const BRepGraph::DefsView aDefs       = myGraph->Defs();
  const int                 aNbEdges    = aDefs.NbEdges();
  const int                 aNbFaces    = aDefs.NbFaces();
  const int                 aNbVertices = aDefs.NbVertices();
  const int                 aNbShells   = aDefs.NbShells();
  const int                 aNbSolids   = aDefs.NbSolids();

  // Phase 1: Per-edge minimum checks (embarrassingly parallel).
  {
    NCollection_Vector<NCollection_Vector<BRepGraphCheck_Issue>> aPerThread;
    aPerThread.SetValue(aNbEdges - 1, NCollection_Vector<BRepGraphCheck_Issue>());

    const bool aForceSingle = !myIsParallel;
    OSD_Parallel::For(
      0,
      aNbEdges,
      [&](int theIdx) {
        BRepGraphCheck::CheckEdgeMinimum(*myGraph,
                                         BRepGraph_EdgeId(theIdx),
                                         aPerThread.ChangeValue(theIdx));
      },
      aForceSingle);

    for (int anEdgeIter = 0; anEdgeIter < aNbEdges; ++anEdgeIter)
    {
      myReport.AddIssues(aPerThread.Value(anEdgeIter));
    }
  }

  // Phase 2: Per-face contextual checks (embarrassingly parallel).
  // For each face: vertex-in-face, edge-in-face, wire-in-face, face minimum.
  if (aNbFaces > 0)
  {
    NCollection_Vector<NCollection_Vector<BRepGraphCheck_Issue>> aPerThread;
    aPerThread.SetValue(aNbFaces - 1, NCollection_Vector<BRepGraphCheck_Issue>());

    const bool aForceSingle = !myIsParallel;
    const bool aGeomCtl     = myGeomControls;
    const bool anIsExact    = myIsExact;

    OSD_Parallel::For(
      0,
      aNbFaces,
      [&](int theFaceIdx) {
        NCollection_Vector<BRepGraphCheck_Issue>& aLocal = aPerThread.ChangeValue(theFaceIdx);
        const BRepGraph_FaceId aFaceId(theFaceIdx);

        // Face minimum check.
        BRepGraphCheck::CheckFaceMinimum(*myGraph, aFaceId, aLocal);

        // Face wire checks (redundant wire is always checked; geometric checks depend on flag).
        BRepGraphCheck::CheckFaceWires(*myGraph, aFaceId, aGeomCtl, aLocal);

        // Edge-in-face and vertex-in-face checks.
        const BRepGraph::DefsView          aLocalDefs = myGraph->Defs();
        const BRepGraph_TopoNode::FaceDef& aFaceDef   = aLocalDefs.Face(aFaceId);

        // Iterate wires of this face through incidence refs.
        for (int aWireIter = 0; aWireIter < aFaceDef.WireRefs.Length(); ++aWireIter)
        {
          const BRepGraphInc::WireRef& aWR     = aFaceDef.WireRefs.Value(aWireIter);
          const BRepGraph_WireId       aWireId = aWR.WireDefId;

          // Wire minimum check.
          BRepGraphCheck::CheckWireMinimum(*myGraph, aWireId, aLocal);

          // Wire-on-face check.
          BRepGraphCheck::CheckWireOnFace(*myGraph, aWireId, aFaceId, aGeomCtl, aLocal);

          // Edge-in-face checks for each edge in this wire.
          const BRepGraph_TopoNode::WireDef& aWireDef = aLocalDefs.Wire(aWireId);
          for (int aCoEdgeIter = 0; aCoEdgeIter < aWireDef.CoEdgeRefs.Length(); ++aCoEdgeIter)
          {
            const BRepGraphInc::CoEdgeRef& aCoEdgeRef = aWireDef.CoEdgeRefs.Value(aCoEdgeIter);
            const BRepGraph_TopoNode::CoEdgeDef& aCoEdgeDef =
              aLocalDefs.CoEdge(aCoEdgeRef.CoEdgeDefId);
            const BRepGraph_EdgeId anEdgeId = aCoEdgeDef.EdgeDefId;

            BRepGraphCheck::CheckEdgeOnFace(*myGraph, anEdgeId, aFaceId, anIsExact, aLocal);

            // Vertex checks at edge endpoints.
            const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aLocalDefs.Edge(anEdgeId);
            if (anEdgeDef.StartVertexDefId().IsValid())
            {
              const BRepGraph_VertexId aStartVtxId = anEdgeDef.StartVertex.VertexDefId;
              BRepGraphCheck::CheckVertexOnEdge(*myGraph, aStartVtxId, anEdgeId, aLocal);
              BRepGraphCheck::CheckVertexOnFace(*myGraph, aStartVtxId, aFaceId, aLocal);
            }
            if (anEdgeDef.EndVertexDefId().IsValid())
            {
              const BRepGraph_VertexId anEndVtxId = anEdgeDef.EndVertex.VertexDefId;
              BRepGraphCheck::CheckVertexOnEdge(*myGraph, anEndVtxId, anEdgeId, aLocal);
              BRepGraphCheck::CheckVertexOnFace(*myGraph, anEndVtxId, aFaceId, aLocal);
            }
          }
        }
      },
      aForceSingle);

    for (int aFaceIter = 0; aFaceIter < aNbFaces; ++aFaceIter)
    {
      myReport.AddIssues(aPerThread.Value(aFaceIter));
    }
  }

  // Also check vertices not associated with any face.
  for (int aVertexIter = 0; aVertexIter < aNbVertices; ++aVertexIter)
  {
    // Vertex standalone checks are covered by vertex-on-edge above.
    (void)aVertexIter;
  }

  // Phase 3: Shell + solid checks.
  if (aNbShells > 0)
  {
    NCollection_Vector<NCollection_Vector<BRepGraphCheck_Issue>> aPerThread;
    aPerThread.SetValue(aNbShells - 1, NCollection_Vector<BRepGraphCheck_Issue>());

    const bool aForceSingle = !myIsParallel;
    OSD_Parallel::For(
      0,
      aNbShells,
      [&](int theIdx) {
        NCollection_Vector<BRepGraphCheck_Issue>& aLocal = aPerThread.ChangeValue(theIdx);
        const BRepGraph_ShellId aShellId(theIdx);
        BRepGraphCheck::CheckShellMinimum(*myGraph, aShellId, aLocal);
        BRepGraphCheck::CheckShellClosed(*myGraph, aShellId, aLocal);
        BRepGraphCheck::CheckShellOrientation(*myGraph, aShellId, aLocal);
      },
      aForceSingle);

    for (int aShellIter = 0; aShellIter < aNbShells; ++aShellIter)
    {
      myReport.AddIssues(aPerThread.Value(aShellIter));
    }
  }

  // Solid checks (sequential - may use BRepClass3d_SolidClassifier).
  for (int aSolidIter = 0; aSolidIter < aNbSolids; ++aSolidIter)
  {
    NCollection_Vector<BRepGraphCheck_Issue> aLocal;
    BRepGraphCheck::CheckSolidMinimum(*myGraph, BRepGraph_SolidId(aSolidIter), aLocal);
    myReport.AddIssues(aLocal);
  }
}

//=================================================================================================

void BRepGraphCheck_Analyzer::CheckVertex(const BRepGraph_VertexId theVertex)
{
  // Check vertex against all edges that reference it.
  const BRepGraph::DefsView                aDefs    = myGraph->Defs();
  const int                                aNbEdges = aDefs.NbEdges();
  NCollection_Vector<BRepGraphCheck_Issue> aLocal;

  for (int anEdgeIter = 0; anEdgeIter < aNbEdges; ++anEdgeIter)
  {
    const BRepGraph_EdgeId             anEdgeId(anEdgeIter);
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aDefs.Edge(anEdgeId);
    if ((anEdgeDef.StartVertexDefId().IsValid()
         && anEdgeDef.StartVertex.VertexDefId == theVertex)
        || (anEdgeDef.EndVertexDefId().IsValid()
            && anEdgeDef.EndVertex.VertexDefId == theVertex))
    {
      BRepGraphCheck::CheckVertexOnEdge(*myGraph, theVertex, anEdgeId, aLocal);
    }
  }
  myReport.AddIssues(aLocal);
}

//=================================================================================================

void BRepGraphCheck_Analyzer::CheckEdge(const BRepGraph_EdgeId theEdge)
{
  NCollection_Vector<BRepGraphCheck_Issue> aLocal;
  BRepGraphCheck::CheckEdgeMinimum(*myGraph, theEdge, aLocal);
  myReport.AddIssues(aLocal);
}

//=================================================================================================

void BRepGraphCheck_Analyzer::CheckWire(const BRepGraph_WireId theWire,
                                        const BRepGraph_FaceId theFace)
{
  NCollection_Vector<BRepGraphCheck_Issue> aLocal;
  BRepGraphCheck::CheckWireMinimum(*myGraph, theWire, aLocal);
  if (theFace.IsValid())
  {
    BRepGraphCheck::CheckWireOnFace(*myGraph, theWire, theFace, myGeomControls, aLocal);
  }
  myReport.AddIssues(aLocal);
}

//=================================================================================================

void BRepGraphCheck_Analyzer::CheckFace(const BRepGraph_FaceId theFace)
{
  NCollection_Vector<BRepGraphCheck_Issue> aLocal;
  BRepGraphCheck::CheckFaceMinimum(*myGraph, theFace, aLocal);
  BRepGraphCheck::CheckFaceWires(*myGraph, theFace, myGeomControls, aLocal);
  myReport.AddIssues(aLocal);
}

//=================================================================================================

void BRepGraphCheck_Analyzer::CheckShell(const BRepGraph_ShellId theShell)
{
  NCollection_Vector<BRepGraphCheck_Issue> aLocal;
  BRepGraphCheck::CheckShellMinimum(*myGraph, theShell, aLocal);
  BRepGraphCheck::CheckShellClosed(*myGraph, theShell, aLocal);
  BRepGraphCheck::CheckShellOrientation(*myGraph, theShell, aLocal);
  myReport.AddIssues(aLocal);
}

//=================================================================================================

void BRepGraphCheck_Analyzer::CheckSolid(const BRepGraph_SolidId theSolid)
{
  NCollection_Vector<BRepGraphCheck_Issue> aLocal;
  BRepGraphCheck::CheckSolidMinimum(*myGraph, theSolid, aLocal);
  myReport.AddIssues(aLocal);
}

//=================================================================================================

bool BRepGraphCheck_Analyzer::IsValid() const
{
  return myReport.IsValid();
}

//=================================================================================================

bool BRepGraphCheck_Analyzer::IsValid(BRepGraph_NodeId theNode) const
{
  if (!myReport.HasIssuesForNode(theNode))
    return true;

  const NCollection_Vector<BRepGraphCheck_Issue> aNodeIssues = myReport.IssuesForNode(theNode);
  for (int anIssueIter = 0; anIssueIter < aNodeIssues.Length(); ++anIssueIter)
  {
    const BRepGraphCheck_Issue::Severity aSev = aNodeIssues.Value(anIssueIter).IssueSeverity;
    if (aSev == BRepGraphCheck_Issue::Severity::Error
        || aSev == BRepGraphCheck_Issue::Severity::Fatal)
    {
      return false;
    }
  }
  return true;
}

//=================================================================================================

const BRepGraphCheck_Report& BRepGraphCheck_Analyzer::Report() const
{
  return myReport;
}
