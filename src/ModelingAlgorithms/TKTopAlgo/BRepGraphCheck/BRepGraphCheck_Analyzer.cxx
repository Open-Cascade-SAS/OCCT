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
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>

#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraphCheck.hxx>
#include <NCollection_Array1.hxx>
#include <OSD_Parallel.hxx>

//=================================================================================================

BRepGraphCheck_Analyzer::BRepGraphCheck_Analyzer(const BRepGraph& theGraph)
    : myGraph(&theGraph)
{
}

//=================================================================================================

void BRepGraphCheck_Analyzer::SetGeometricControls(const bool theVal)
{
  myGeomControls = theVal;
}

//=================================================================================================

void BRepGraphCheck_Analyzer::SetParallel(const bool theVal)
{
  myIsParallel = theVal;
}

//=================================================================================================

void BRepGraphCheck_Analyzer::SetExactMethod(const bool theVal)
{
  myIsExact = theVal;
}

//=================================================================================================

void BRepGraphCheck_Analyzer::Perform()
{
  myReport.Clear();

  const BRepGraph::TopoView& aDefs       = myGraph->Topo();
  const int                  aNbEdges    = aDefs.Edges().Nb();
  const int                  aNbFaces    = aDefs.Faces().Nb();
  const int                  aNbVertices = aDefs.Vertices().Nb();
  const int                  aNbShells   = aDefs.Shells().Nb();
  const int                  aNbSolids   = aDefs.Solids().Nb();

  // Phase 1: Per-edge minimum checks (embarrassingly parallel).
  if (aNbEdges > 0)
  {
    NCollection_Array1<NCollection_Vector<BRepGraphCheck_Issue>> aPerThread(0, aNbEdges - 1);

    const bool aForceSingle = !myIsParallel;
    OSD_Parallel::For(
      0,
      aNbEdges,
      [&](int theIdx) {
        BRepGraphCheck::CheckEdgeMinimum(*myGraph, BRepGraph_EdgeId(theIdx), aPerThread(theIdx));
      },
      aForceSingle);

    for (int anEdgeIter = 0; anEdgeIter < aNbEdges; ++anEdgeIter)
    {
      myReport.AddIssues(aPerThread(anEdgeIter));
    }
  }

  // Phase 2: Per-face contextual checks (embarrassingly parallel).
  // For each face: vertex-in-face, edge-in-face, wire-in-face, face minimum.
  if (aNbFaces > 0)
  {
    NCollection_Array1<NCollection_Vector<BRepGraphCheck_Issue>> aPerThread(0, aNbFaces - 1);

    const bool aForceSingle = !myIsParallel;
    const bool aGeomCtl     = myGeomControls;
    const bool anIsExact    = myIsExact;

    OSD_Parallel::For(
      0,
      aNbFaces,
      [&](int theFaceIdx) {
        NCollection_Vector<BRepGraphCheck_Issue>& aLocal = aPerThread(theFaceIdx);
        const BRepGraph_FaceId                    aFaceId(theFaceIdx);

        // Face minimum check.
        BRepGraphCheck::CheckFaceMinimum(*myGraph, aFaceId, aLocal);

        // Face wire checks (redundant wire is always checked; geometric checks depend on flag).
        BRepGraphCheck::CheckFaceWires(*myGraph, aFaceId, aGeomCtl, aLocal);

        // Edge-in-face and vertex-in-face checks.
        const BRepGraph::TopoView&   aLocalDefs = myGraph->Topo();
        const BRepGraph::RefsView&   aLocalRefs = myGraph->Refs();
        const BRepGraphInc::FaceDef& aFaceDef   = aLocalDefs.Faces().Definition(aFaceId);

        for (int aWireRefIter = 0; aWireRefIter < aFaceDef.WireRefIds.Length(); ++aWireRefIter)
        {
          const BRepGraph_WireRefId    aWireRefId = aFaceDef.WireRefIds.Value(aWireRefIter);
          const BRepGraphInc::WireRef& aWireRef   = aLocalRefs.Wires().Entry(aWireRefId);
          if (aWireRef.IsRemoved || !aWireRef.WireDefId.IsValid(aLocalDefs.Wires().Nb()))
          {
            continue;
          }
          const BRepGraph_WireId aWireId = aWireRef.WireDefId;

          // Wire minimum check.
          BRepGraphCheck::CheckWireMinimum(*myGraph, aWireId, aLocal);

          // Wire-on-face check.
          BRepGraphCheck::CheckWireOnFace(*myGraph, aWireId, aFaceId, aGeomCtl, aLocal);

          const BRepGraphInc::WireDef& aWireDef = aLocalDefs.Wires().Definition(aWireId);
          for (int aCoEdgeRefIter = 0; aCoEdgeRefIter < aWireDef.CoEdgeRefIds.Length();
               ++aCoEdgeRefIter)
          {
            const BRepGraph_CoEdgeRefId aCoEdgeRefId  = aWireDef.CoEdgeRefIds.Value(aCoEdgeRefIter);
            const BRepGraphInc::CoEdgeRef& aCoEdgeRef = aLocalRefs.CoEdges().Entry(aCoEdgeRefId);
            if (aCoEdgeRef.IsRemoved || !aCoEdgeRef.CoEdgeDefId.IsValid(aLocalDefs.CoEdges().Nb()))
            {
              continue;
            }
            const BRepGraphInc::CoEdgeDef& aCoEdgeDef =
              aLocalDefs.CoEdges().Definition(aCoEdgeRef.CoEdgeDefId);
            const BRepGraph_EdgeId anEdgeId = aCoEdgeDef.EdgeDefId;

            BRepGraphCheck::CheckEdgeOnFace(*myGraph, anEdgeId, aFaceId, anIsExact, aLocal);

            // Vertex checks at edge endpoints.
            const BRepGraphInc::EdgeDef& anEdgeDef = aLocalDefs.Edges().Definition(anEdgeId);
            if (anEdgeDef.StartVertexRefId.IsValid())
            {
              const BRepGraph_VertexId aStartVtxId =
                BRepGraph_Tool::Edge::StartVertex(*myGraph, anEdgeId).VertexDefId;
              if (aStartVtxId.IsValid())
              {
                BRepGraphCheck::CheckVertexOnEdge(*myGraph, aStartVtxId, anEdgeId, aLocal);
                BRepGraphCheck::CheckVertexOnFace(*myGraph, aStartVtxId, aFaceId, aLocal);
              }
            }
            if (anEdgeDef.EndVertexRefId.IsValid())
            {
              const BRepGraph_VertexId anEndVtxId =
                BRepGraph_Tool::Edge::EndVertex(*myGraph, anEdgeId).VertexDefId;
              if (anEndVtxId.IsValid())
              {
                BRepGraphCheck::CheckVertexOnEdge(*myGraph, anEndVtxId, anEdgeId, aLocal);
                BRepGraphCheck::CheckVertexOnFace(*myGraph, anEndVtxId, aFaceId, aLocal);
              }
            }
          }
        }
      },
      aForceSingle);

    for (int aFaceIter = 0; aFaceIter < aNbFaces; ++aFaceIter)
    {
      myReport.AddIssues(aPerThread(aFaceIter));
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
    NCollection_Array1<NCollection_Vector<BRepGraphCheck_Issue>> aPerThread(0, aNbShells - 1);

    const bool aForceSingle = !myIsParallel;
    OSD_Parallel::For(
      0,
      aNbShells,
      [&](int theIdx) {
        NCollection_Vector<BRepGraphCheck_Issue>& aLocal = aPerThread(theIdx);
        const BRepGraph_ShellId                   aShellId(theIdx);
        BRepGraphCheck::CheckShellMinimum(*myGraph, aShellId, aLocal);
        BRepGraphCheck::CheckShellClosed(*myGraph, aShellId, aLocal);
        BRepGraphCheck::CheckShellOrientation(*myGraph, aShellId, aLocal);
      },
      aForceSingle);

    for (int aShellIter = 0; aShellIter < aNbShells; ++aShellIter)
    {
      myReport.AddIssues(aPerThread(aShellIter));
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
  const BRepGraph::TopoView&               aDefs    = myGraph->Topo();
  const int                                aNbEdges = aDefs.Edges().Nb();
  NCollection_Vector<BRepGraphCheck_Issue> aLocal;

  for (int anEdgeIter = 0; anEdgeIter < aNbEdges; ++anEdgeIter)
  {
    const BRepGraph_EdgeId       anEdgeId(anEdgeIter);
    const BRepGraphInc::EdgeDef& anEdgeDef = aDefs.Edges().Definition(anEdgeId);
    const BRepGraph_VertexId     aStartVtxId =
      anEdgeDef.StartVertexRefId.IsValid()
            ? BRepGraph_Tool::Edge::StartVertex(*myGraph, anEdgeId).VertexDefId
            : BRepGraph_VertexId();
    const BRepGraph_VertexId anEndVtxId =
      anEdgeDef.EndVertexRefId.IsValid()
        ? BRepGraph_Tool::Edge::EndVertex(*myGraph, anEdgeId).VertexDefId
        : BRepGraph_VertexId();
    if ((aStartVtxId.IsValid() && aStartVtxId == theVertex)
        || (anEndVtxId.IsValid() && anEndVtxId == theVertex))
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

bool BRepGraphCheck_Analyzer::IsValid(const BRepGraph_NodeId theNode) const
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
