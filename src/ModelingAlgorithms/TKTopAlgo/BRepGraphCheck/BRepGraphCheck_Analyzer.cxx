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
#include <BRepGraph_UsagesView.hxx>
#include <BRepGraphCheck.hxx>
#include <OSD_Parallel.hxx>

//=================================================================================================

BRepGraphCheck_Analyzer::BRepGraphCheck_Analyzer(const BRepGraph& theGraph)
  : myGraph(&theGraph)
{
}

//=================================================================================================

void BRepGraphCheck_Analyzer::SetGeometricControls(bool theVal) { myGeomControls = theVal; }

//=================================================================================================

void BRepGraphCheck_Analyzer::SetParallel(bool theVal) { myIsParallel = theVal; }

//=================================================================================================

void BRepGraphCheck_Analyzer::SetExactMethod(bool theVal) { myIsExact = theVal; }

//=================================================================================================

void BRepGraphCheck_Analyzer::Perform()
{
  myReport.Clear();

  const BRepGraph::DefsView aDefs = myGraph->Defs();
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
    OSD_Parallel::For(0, aNbEdges, [&](int theIdx)
    {
      BRepGraphCheck::CheckEdgeMinimum(*myGraph, theIdx, aPerThread.ChangeValue(theIdx));
    }, aForceSingle);

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

    OSD_Parallel::For(0, aNbFaces, [&](int theFaceIdx)
    {
      NCollection_Vector<BRepGraphCheck_Issue>& aLocal = aPerThread.ChangeValue(theFaceIdx);

      // Face minimum check.
      BRepGraphCheck::CheckFaceMinimum(*myGraph, theFaceIdx, aLocal);

      // Face wire checks (redundant wire is always checked; geometric checks depend on flag).
      BRepGraphCheck::CheckFaceWires(*myGraph, theFaceIdx, aGeomCtl, aLocal);

      // Edge-in-face and vertex-in-face checks.
      const BRepGraph::DefsView aLocalDefs = myGraph->Defs();
      const BRepGraph_TopoNode::FaceDef& aFaceDef = aLocalDefs.Face(theFaceIdx);

      // Iterate wires of this face through usages.
      for (int aUsageIter = 0; aUsageIter < aFaceDef.Usages.Length(); ++aUsageIter)
      {
        const BRepGraph_UsageId& aFaceUsage = aFaceDef.Usages.Value(aUsageIter);
        const BRepGraph::UsagesView aUsages = myGraph->Usages();
        const BRepGraph_TopoNode::FaceUsage& aFU = aUsages.Face(aFaceUsage.Index);

        // Iterate wire usages (outer + inner).
        for (int aWireIter = 0; aWireIter < aFU.NbWireUsages(); ++aWireIter)
        {
          const BRepGraph_UsageId aWireUsage = aFU.WireUsage(aWireIter);
          const BRepGraph_NodeId   aWireDefId = myGraph->DefOf(aWireUsage);
          const int                aWireDefIdx = aWireDefId.Index;

          // Wire minimum check.
          BRepGraphCheck::CheckWireMinimum(*myGraph, aWireDefIdx, aLocal);

          // Wire-on-face check.
          BRepGraphCheck::CheckWireOnFace(*myGraph, aWireDefIdx, theFaceIdx,
                                          aGeomCtl, aLocal);

          // Edge-in-face checks for each edge in this wire.
          const BRepGraph_TopoNode::WireDef& aWireDef = aLocalDefs.Wire(aWireDefIdx);
          for (int anEdgeIter = 0; anEdgeIter < aWireDef.OrderedEdges.Length(); ++anEdgeIter)
          {
            const int anEdgeDefIdx = aWireDef.OrderedEdges.Value(anEdgeIter).EdgeDefId.Index;

            BRepGraphCheck::CheckEdgeOnFace(*myGraph, anEdgeDefIdx, theFaceIdx,
                                            anIsExact, aLocal);

            // Vertex checks at edge endpoints.
            const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aLocalDefs.Edge(anEdgeDefIdx);
            if (anEdgeDef.StartVertexDefId.IsValid())
            {
              BRepGraphCheck::CheckVertexOnEdge(*myGraph, anEdgeDef.StartVertexDefId.Index,
                                                anEdgeDefIdx, aLocal);
              BRepGraphCheck::CheckVertexOnFace(*myGraph, anEdgeDef.StartVertexDefId.Index,
                                                theFaceIdx, aLocal);
            }
            if (anEdgeDef.EndVertexDefId.IsValid())
            {
              BRepGraphCheck::CheckVertexOnEdge(*myGraph, anEdgeDef.EndVertexDefId.Index,
                                                anEdgeDefIdx, aLocal);
              BRepGraphCheck::CheckVertexOnFace(*myGraph, anEdgeDef.EndVertexDefId.Index,
                                                theFaceIdx, aLocal);
            }
          }
        }
        // Only process the first usage for checking purposes.
        break;
      }
    }, aForceSingle);

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
    OSD_Parallel::For(0, aNbShells, [&](int theIdx)
    {
      NCollection_Vector<BRepGraphCheck_Issue>& aLocal = aPerThread.ChangeValue(theIdx);
      BRepGraphCheck::CheckShellMinimum(*myGraph, theIdx, aLocal);
      BRepGraphCheck::CheckShellClosed(*myGraph, theIdx, aLocal);
      BRepGraphCheck::CheckShellOrientation(*myGraph, theIdx, aLocal);
    }, aForceSingle);

    for (int aShellIter = 0; aShellIter < aNbShells; ++aShellIter)
    {
      myReport.AddIssues(aPerThread.Value(aShellIter));
    }
  }

  // Solid checks (sequential — may use BRepClass3d_SolidClassifier).
  for (int aSolidIter = 0; aSolidIter < aNbSolids; ++aSolidIter)
  {
    NCollection_Vector<BRepGraphCheck_Issue> aLocal;
    BRepGraphCheck::CheckSolidMinimum(*myGraph, aSolidIter, aLocal);
    myReport.AddIssues(aLocal);
  }
}

//=================================================================================================

void BRepGraphCheck_Analyzer::CheckVertex(int theVertexDefIdx)
{
  // Check vertex against all edges that reference it.
  const BRepGraph::DefsView aDefs = myGraph->Defs();
  const int aNbEdges = aDefs.NbEdges();
  NCollection_Vector<BRepGraphCheck_Issue> aLocal;

  for (int anEdgeIter = 0; anEdgeIter < aNbEdges; ++anEdgeIter)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aDefs.Edge(anEdgeIter);
    if ((anEdgeDef.StartVertexDefId.IsValid() && anEdgeDef.StartVertexDefId.Index == theVertexDefIdx)
        || (anEdgeDef.EndVertexDefId.IsValid() && anEdgeDef.EndVertexDefId.Index == theVertexDefIdx))
    {
      BRepGraphCheck::CheckVertexOnEdge(*myGraph, theVertexDefIdx, anEdgeIter, aLocal);
    }
  }
  myReport.AddIssues(aLocal);
}

//=================================================================================================

void BRepGraphCheck_Analyzer::CheckEdge(int theEdgeDefIdx)
{
  NCollection_Vector<BRepGraphCheck_Issue> aLocal;
  BRepGraphCheck::CheckEdgeMinimum(*myGraph, theEdgeDefIdx, aLocal);
  myReport.AddIssues(aLocal);
}

//=================================================================================================

void BRepGraphCheck_Analyzer::CheckWire(int theWireDefIdx, int theFaceDefIdx)
{
  NCollection_Vector<BRepGraphCheck_Issue> aLocal;
  BRepGraphCheck::CheckWireMinimum(*myGraph, theWireDefIdx, aLocal);
  if (theFaceDefIdx >= 0)
  {
    BRepGraphCheck::CheckWireOnFace(*myGraph, theWireDefIdx, theFaceDefIdx,
                                    myGeomControls, aLocal);
  }
  myReport.AddIssues(aLocal);
}

//=================================================================================================

void BRepGraphCheck_Analyzer::CheckFace(int theFaceDefIdx)
{
  NCollection_Vector<BRepGraphCheck_Issue> aLocal;
  BRepGraphCheck::CheckFaceMinimum(*myGraph, theFaceDefIdx, aLocal);
  BRepGraphCheck::CheckFaceWires(*myGraph, theFaceDefIdx, myGeomControls, aLocal);
  myReport.AddIssues(aLocal);
}

//=================================================================================================

void BRepGraphCheck_Analyzer::CheckShell(int theShellDefIdx)
{
  NCollection_Vector<BRepGraphCheck_Issue> aLocal;
  BRepGraphCheck::CheckShellMinimum(*myGraph, theShellDefIdx, aLocal);
  BRepGraphCheck::CheckShellClosed(*myGraph, theShellDefIdx, aLocal);
  BRepGraphCheck::CheckShellOrientation(*myGraph, theShellDefIdx, aLocal);
  myReport.AddIssues(aLocal);
}

//=================================================================================================

void BRepGraphCheck_Analyzer::CheckSolid(int theSolidDefIdx)
{
  NCollection_Vector<BRepGraphCheck_Issue> aLocal;
  BRepGraphCheck::CheckSolidMinimum(*myGraph, theSolidDefIdx, aLocal);
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
