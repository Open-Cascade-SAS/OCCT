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

#include <BRepGraph_Validate.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>

#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepGraph_WireExplorer.hxx>

#include <NCollection_DataMap.hxx>
#include <NCollection_Map.hxx>

namespace
{

//! Check that a NodeId refers to a valid index within graph bounds.
bool isValidNodeId(const BRepGraph& theGraph, const BRepGraph_NodeId theId)
{
  if (!theId.IsValid())
    return false;

  switch (theId.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Solid:
      return theId.IsValid(theGraph.Topo().Solids().Nb());
    case BRepGraph_NodeId::Kind::Shell:
      return theId.IsValid(theGraph.Topo().Shells().Nb());
    case BRepGraph_NodeId::Kind::Face:
      return theId.IsValid(theGraph.Topo().Faces().Nb());
    case BRepGraph_NodeId::Kind::Wire:
      return theId.IsValid(theGraph.Topo().Wires().Nb());
    case BRepGraph_NodeId::Kind::CoEdge:
      return theId.IsValid(theGraph.Topo().CoEdges().Nb());
    case BRepGraph_NodeId::Kind::Edge:
      return theId.IsValid(theGraph.Topo().Edges().Nb());
    case BRepGraph_NodeId::Kind::Vertex:
      return theId.IsValid(theGraph.Topo().Vertices().Nb());
    case BRepGraph_NodeId::Kind::Compound:
      return theId.IsValid(theGraph.Topo().Compounds().Nb());
    case BRepGraph_NodeId::Kind::CompSolid:
      return theId.IsValid(theGraph.Topo().CompSolids().Nb());
    case BRepGraph_NodeId::Kind::Product:
      return theId.IsValid(theGraph.Topo().Products().Nb());
    case BRepGraph_NodeId::Kind::Occurrence:
      return theId.IsValid(theGraph.Topo().Occurrences().Nb());
  }
  return false;
}

//! Check if a topology def is removed.
bool isEntityRemoved(const BRepGraph& theGraph, BRepGraph_NodeId theId)
{
  const BRepGraphInc::BaseDef* aDef = theGraph.Topo().Gen().TopoEntity(theId);
  return aDef != nullptr && aDef->IsRemoved;
}

//! Convert mutator boundary issues to validator issues.
//! @param[in]     theBoundaryIssues boundary issues reported by mutator
//! @param[in,out] theIssues         destination validator issue vector
void appendMutationBoundaryIssues(
  const NCollection_Vector<BRepGraph::BuilderView::BoundaryIssue>& theBoundaryIssues,
  NCollection_Vector<BRepGraph_Validate::Issue>&                   theIssues)
{
  using Issue    = BRepGraph_Validate::Issue;
  using Severity = BRepGraph_Validate::Severity;
  for (int anIdx = 0; anIdx < theBoundaryIssues.Length(); ++anIdx)
  {
    const BRepGraph::BuilderView::BoundaryIssue& aBoundaryIssue = theBoundaryIssues.Value(anIdx);
    theIssues.Append(Issue{Severity::Error, aBoundaryIssue.NodeId, aBoundaryIssue.Description});
  }
}

//! Validate that all cross-reference indices (vertex, edge, wire, face, shell,
//! coedge, compound child refs) point to valid, in-bounds entity slots.
//! @param[in]     theGraph  source graph
//! @param[in,out] theIssues collection to append diagnostic issues
void checkCrossReferenceBounds(const BRepGraph&                               theGraph,
                               NCollection_Vector<BRepGraph_Validate::Issue>& theIssues)
{
  using Issue    = BRepGraph_Validate::Issue;
  using Severity = BRepGraph_Validate::Severity;

  // Check EdgeDef references.
  for (BRepGraph_Iterator<BRepGraphInc::EdgeDef> anEdgeIt(theGraph); anEdgeIt.More();
       anEdgeIt.Next())
  {
    const BRepGraphInc::EdgeDef& anEdge = anEdgeIt.Current();

    // Resolve vertex def ids through vertex ref entries.
    if (anEdge.StartVertexRefId.IsValid())
    {
      const BRepGraph_NodeId aStartVtxId =
        theGraph.Refs().Vertices().Entry(anEdge.StartVertexRefId).VertexDefId;
      if (aStartVtxId.IsValid() && !isValidNodeId(theGraph, aStartVtxId))
      {
        theIssues.Append(Issue{Severity::Error,
                               anEdge.Id,
                               "EdgeDef.StartVertexRefId resolves to out-of-bounds VertexDefId"});
      }
    }
    if (anEdge.EndVertexRefId.IsValid())
    {
      const BRepGraph_NodeId anEndVtxId =
        theGraph.Refs().Vertices().Entry(anEdge.EndVertexRefId).VertexDefId;
      if (anEndVtxId.IsValid() && !isValidNodeId(theGraph, anEndVtxId))
      {
        theIssues.Append(Issue{Severity::Error,
                               anEdge.Id,
                               "EdgeDef.EndVertexRefId resolves to out-of-bounds VertexDefId"});
      }
    }
  }

  // Check CoEdgeDef references.
  for (BRepGraph_Iterator<BRepGraphInc::CoEdgeDef> aCoEdgeIt(theGraph); aCoEdgeIt.More();
       aCoEdgeIt.Next())
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge   = aCoEdgeIt.Current();
    const BRepGraph_CoEdgeId       aCoEdgeId = aCoEdgeIt.CurrentId();

    if (aCoEdge.FaceDefId.IsValid() && !isValidNodeId(theGraph, aCoEdge.FaceDefId))
    {
      theIssues.Append(Issue{Severity::Error, aCoEdge.Id, "CoEdgeDef.FaceDefId out of bounds"});
    }
    const BRepGraph_NodeId anEdgeId = aCoEdge.EdgeDefId;
    if (anEdgeId.IsValid() && !isValidNodeId(theGraph, anEdgeId))
    {
      theIssues.Append(Issue{Severity::Error, aCoEdge.Id, "CoEdgeDef.EdgeDefId out of bounds"});
    }
    // SeamPairId consistency.
    if (aCoEdge.SeamPairId.IsValid())
    {
      if (!aCoEdge.SeamPairId.IsValid(theGraph.Topo().CoEdges().Nb()))
      {
        theIssues.Append(Issue{Severity::Error, aCoEdge.Id, "CoEdgeDef.SeamPairId out of bounds"});
      }
      else if (aCoEdge.SeamPairId.Index == aCoEdgeId.Index)
      {
        theIssues.Append(
          Issue{Severity::Error, aCoEdge.Id, "CoEdgeDef.SeamPairId is self-reference"});
      }
      else
      {
        const BRepGraphInc::CoEdgeDef& aPair =
          theGraph.Topo().CoEdges().Definition(aCoEdge.SeamPairId);
        if (aPair.SeamPairId.Index != aCoEdgeId.Index)
        {
          theIssues.Append(
            Issue{Severity::Error, aCoEdge.Id, "CoEdgeDef.SeamPairId not bidirectional"});
        }
        if (aPair.FaceDefId != aCoEdge.FaceDefId)
        {
          theIssues.Append(
            Issue{Severity::Error, aCoEdge.Id, "CoEdgeDef seam pair has different FaceDefId"});
        }
      }
    }
    // Rep index bounds.
    if (aCoEdge.Curve2DRepId.IsValid()
        && !aCoEdge.Curve2DRepId.IsValid(theGraph.Topo().Geometry().NbCurves2D()))
    {
      theIssues.Append(Issue{Severity::Error, aCoEdge.Id, "CoEdgeDef.Curve2DRepId out of bounds"});
    }
  }

  // Surface handles are stored directly on FaceDef; no cross-reference to validate.

  // Check WireDef CoEdgeRef references.
  for (BRepGraph_Iterator<BRepGraphInc::WireDef> aWireIt(theGraph); aWireIt.More(); aWireIt.Next())
  {
    const BRepGraphInc::WireDef& aWire   = aWireIt.Current();
    const BRepGraph_WireId       aWireId = aWireIt.CurrentId();

    for (BRepGraph_RefsCoEdgeOfWire anIt(theGraph, aWireId); anIt.More(); anIt.Next())
    {
      const BRepGraphInc::CoEdgeRef& aCR = theGraph.Refs().CoEdges().Entry(anIt.CurrentId());
      const BRepGraph_NodeId         aCoEdgeDefId = aCR.CoEdgeDefId;
      if (aCoEdgeDefId.IsValid() && !isValidNodeId(theGraph, aCoEdgeDefId))
      {
        theIssues.Append(
          Issue{Severity::Error, aWire.Id, "WireDef.CoEdgeUsage CoEdgeIdx out of bounds"});
      }
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        theGraph.Topo().CoEdges().Definition(aCR.CoEdgeDefId);
      const BRepGraph_NodeId anEdgeDefId = aCoEdge.EdgeDefId;
      if (anEdgeDefId.IsValid() && !isValidNodeId(theGraph, anEdgeDefId))
      {
        theIssues.Append(
          Issue{Severity::Error, aWire.Id, "WireDef.CoEdgeUsage EdgeIdx out of bounds"});
      }
    }
  }

  // Check CompoundDef ChildRef references.
  for (BRepGraph_Iterator<BRepGraphInc::CompoundDef> aCompIt(theGraph); aCompIt.More();
       aCompIt.Next())
  {
    const BRepGraphInc::CompoundDef& aComp       = aCompIt.Current();
    const BRepGraph_CompoundId       aCompoundId = aCompIt.CurrentId();

    for (BRepGraph_RefsChildOfCompound anIt(theGraph, aCompoundId); anIt.More(); anIt.Next())
    {
      const BRepGraphInc::ChildRef& aCR      = theGraph.Refs().Children().Entry(anIt.CurrentId());
      const BRepGraph_NodeId        aChildId = aCR.ChildDefId;
      if (aChildId.IsValid() && !isValidNodeId(theGraph, aChildId))
      {
        theIssues.Append(Issue{Severity::Error, aComp.Id, "CompoundDef.ChildDefId out of bounds"});
      }
    }
  }

  // Check CompSolidDef SolidRef references.
  for (BRepGraph_Iterator<BRepGraphInc::CompSolidDef> aCSIt(theGraph); aCSIt.More(); aCSIt.Next())
  {
    const BRepGraphInc::CompSolidDef& aCS          = aCSIt.Current();
    const BRepGraph_CompSolidId       aCompSolidId = aCSIt.CurrentId();

    for (BRepGraph_RefsSolidOfCompSolid anIt(theGraph, aCompSolidId); anIt.More(); anIt.Next())
    {
      const BRepGraphInc::SolidRef& aSR      = theGraph.Refs().Solids().Entry(anIt.CurrentId());
      const BRepGraph_NodeId        aSolidId = aSR.SolidDefId;
      if (aSolidId.IsValid() && !isValidNodeId(theGraph, aSolidId))
      {
        theIssues.Append(Issue{Severity::Error, aCS.Id, "CompSolidDef.SolidDefId out of bounds"});
      }
    }
  }

  // Check ShellDef FaceRef references.
  for (BRepGraph_Iterator<BRepGraphInc::ShellDef> aShellIt(theGraph); aShellIt.More();
       aShellIt.Next())
  {
    const BRepGraphInc::ShellDef& aShell   = aShellIt.Current();
    const BRepGraph_ShellId       aShellId = aShellIt.CurrentId();

    for (BRepGraph_RefsFaceOfShell anIt(theGraph, aShellId); anIt.More(); anIt.Next())
    {
      const BRepGraphInc::FaceRef& aFR        = theGraph.Refs().Faces().Entry(anIt.CurrentId());
      const BRepGraph_NodeId       aFaceDefId = aFR.FaceDefId;
      if (aFaceDefId.IsValid() && !isValidNodeId(theGraph, aFaceDefId))
      {
        theIssues.Append(
          Issue{Severity::Error, aShell.Id, "ShellDef.FaceUsage FaceIdx out of bounds"});
      }
    }
  }

  // Check SolidDef ShellRef references.
  for (BRepGraph_Iterator<BRepGraphInc::SolidDef> aSolidIt(theGraph); aSolidIt.More();
       aSolidIt.Next())
  {
    const BRepGraphInc::SolidDef& aSolid   = aSolidIt.Current();
    const BRepGraph_SolidId       aSolidId = aSolidIt.CurrentId();

    for (BRepGraph_RefsShellOfSolid anIt(theGraph, aSolidId); anIt.More(); anIt.Next())
    {
      const BRepGraphInc::ShellRef& aSR         = theGraph.Refs().Shells().Entry(anIt.CurrentId());
      const BRepGraph_NodeId        aShellDefId = aSR.ShellDefId;
      if (aShellDefId.IsValid() && !isValidNodeId(theGraph, aShellDefId))
      {
        theIssues.Append(
          Issue{Severity::Error, aSolid.Id, "SolidDef.ShellUsage ShellIdx out of bounds"});
      }
    }
  }

  // Check ProductDef references.
  for (BRepGraph_Iterator<BRepGraphInc::ProductDef> aProductIt(theGraph); aProductIt.More();
       aProductIt.Next())
  {
    const BRepGraphInc::ProductDef& aProduct = aProductIt.Current();
    const BRepGraph_ProductId       aProdId  = aProductIt.CurrentId();

    if (aProduct.ShapeRootId.IsValid())
    {
      if (!isValidNodeId(theGraph, aProduct.ShapeRootId))
      {
        theIssues.Append(
          Issue{Severity::Error, aProduct.Id, "ProductDef.ShapeRootId out of bounds"});
      }
      else if (aProduct.ShapeRootId.NodeKind == BRepGraph_NodeId::Kind::CoEdge
               || BRepGraph_NodeId::IsAssemblyKind(aProduct.ShapeRootId.NodeKind))
      {
        theIssues.Append(
          Issue{Severity::Error, aProduct.Id, "ProductDef.ShapeRootId has invalid node kind"});
      }
    }

    for (BRepGraph_RefsOccurrenceOfProduct anOccIt(theGraph, aProdId); anOccIt.More();
         anOccIt.Next())
    {
      const BRepGraph_OccurrenceId anOccId =
        theGraph.Refs().Occurrences().Entry(anOccIt.CurrentId()).OccurrenceDefId;
      if (anOccId.IsValid() && !isValidNodeId(theGraph, anOccId))
      {
        theIssues.Append(
          Issue{Severity::Error, aProduct.Id, "ProductDef.OccurrenceUsage out of bounds"});
      }
    }
  }

  // Check OccurrenceDef references.
  for (BRepGraph_Iterator<BRepGraphInc::OccurrenceDef> anOccIt(theGraph); anOccIt.More();
       anOccIt.Next())
  {
    const BRepGraphInc::OccurrenceDef& anOcc   = anOccIt.Current();
    const BRepGraph_OccurrenceId       anOccId = anOccIt.CurrentId();

    if (!anOcc.ProductDefId.IsValid() || !isValidNodeId(theGraph, anOcc.ProductDefId))
    {
      theIssues.Append(Issue{Severity::Error, anOcc.Id, "OccurrenceDef.ProductDefId invalid"});
    }
    if (!anOcc.ParentProductDefId.IsValid() || !isValidNodeId(theGraph, anOcc.ParentProductDefId))
    {
      theIssues.Append(
        Issue{Severity::Error, anOcc.Id, "OccurrenceDef.ParentProductDefId invalid"});
    }

    if (anOcc.ParentOccurrenceDefId.IsValid())
    {
      if (!isValidNodeId(theGraph, anOcc.ParentOccurrenceDefId))
      {
        theIssues.Append(
          Issue{Severity::Error, anOcc.Id, "OccurrenceDef.ParentOccurrenceDefId invalid"});
      }
      else if (anOcc.ParentOccurrenceDefId.Index == anOccId.Index)
      {
        theIssues.Append(
          Issue{Severity::Error, anOcc.Id, "OccurrenceDef.ParentOccurrenceDefId self-reference"});
      }
    }
  }
}

//! Verify that every forward incidence ref has a matching reverse-index entry
//! (edge->wires, edge->faces, vertex->edges, wire->faces, face->shells, shell->solids).
//! @param[in]     theGraph  source graph
//! @param[in,out] theIssues collection to append diagnostic issues
void checkReverseIndexConsistency(const BRepGraph&                               theGraph,
                                  NCollection_Vector<BRepGraph_Validate::Issue>& theIssues)
{
  using Issue    = BRepGraph_Validate::Issue;
  using Severity = BRepGraph_Validate::Severity;

  // Build expected edge->wires mapping from CoEdgeRef scans.
  NCollection_DataMap<int, NCollection_Map<int>> anExpected;
  for (BRepGraph_Iterator<BRepGraphInc::WireDef> aWireIt(theGraph); aWireIt.More(); aWireIt.Next())
  {
    const BRepGraph_WireId aWireId = aWireIt.CurrentId();

    for (BRepGraph_RefsCoEdgeOfWire anIt(theGraph, aWireId); anIt.More(); anIt.Next())
    {
      const BRepGraphInc::CoEdgeRef& aCR = theGraph.Refs().CoEdges().Entry(anIt.CurrentId());
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        theGraph.Topo().CoEdges().Definition(aCR.CoEdgeDefId);
      if (!aCoEdge.EdgeDefId.IsValid())
        continue;

      if (!anExpected.IsBound(aCoEdge.EdgeDefId.Index))
        anExpected.Bind(aCoEdge.EdgeDefId.Index, NCollection_Map<int>());
      anExpected.ChangeFind(aCoEdge.EdgeDefId.Index).Add(aWireId.Index);
    }
  }

  // Check that Topo WiresOfEdge matches expected.
  for (BRepGraph_Iterator<BRepGraphInc::EdgeDef> anEdgeIt(theGraph); anEdgeIt.More();
       anEdgeIt.Next())
  {
    const BRepGraph_EdgeId       anEdgeId = anEdgeIt.CurrentId();
    const BRepGraphInc::EdgeDef& anEdge   = anEdgeIt.Current();

    const NCollection_Vector<BRepGraph_WireId>& aActualWires =
      theGraph.Topo().Edges().Wires(anEdgeId);
    const NCollection_Map<int>* anExpectedWires = anExpected.Seek(anEdgeId.Index);

    const int anExpectedCount = (anExpectedWires != nullptr) ? anExpectedWires->Extent() : 0;

    // Build a set from actual wires for comparison.
    NCollection_Map<int> anActualSet;
    for (int aWIdx = 0; aWIdx < aActualWires.Length(); ++aWIdx)
      anActualSet.Add(aActualWires.Value(aWIdx).Index);

    if (anActualSet.Extent() != anExpectedCount)
    {
      theIssues.Append(
        Issue{Severity::Error, anEdge.Id, "Reverse index ReverseIdx.WiresOfEdge size mismatch"});
      continue;
    }

    if (anExpectedWires != nullptr)
    {
      for (NCollection_Map<int>::Iterator anMapIt(*anExpectedWires); anMapIt.More(); anMapIt.Next())
      {
        if (!anActualSet.Contains(anMapIt.Value()))
        {
          theIssues.Append(Issue{Severity::Error,
                                 anEdge.Id,
                                 "Reverse index ReverseIdx.WiresOfEdge missing wire entry"});
          break;
        }
      }
    }
  }
}

//! Validate consistency of cached edge-face counts in reverse index.
//! @param[in]     theGraph  source graph
//! @param[in,out] theIssues collection to append diagnostic issues
void checkReverseIndexFaceCountCache(const BRepGraph&                               theGraph,
                                     NCollection_Vector<BRepGraph_Validate::Issue>& theIssues)
{
  using Issue                      = BRepGraph_Validate::Issue;
  using Severity                   = BRepGraph_Validate::Severity;
  const BRepGraph::TopoView& aDefs = theGraph.Topo();

  for (BRepGraph_Iterator<BRepGraphInc::EdgeDef> anEdgeIt(theGraph); anEdgeIt.More();
       anEdgeIt.Next())
  {
    const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();

    NCollection_Map<int>                          aUniqueFaces;
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdges = aDefs.Edges().CoEdges(anEdgeId);
    for (int aCoEdgeIdx = 0; aCoEdgeIdx < aCoEdges.Length(); ++aCoEdgeIdx)
    {
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        aDefs.CoEdges().Definition(aCoEdges.Value(aCoEdgeIdx));
      if (aCoEdge.IsRemoved || !aCoEdge.FaceDefId.IsValid())
      {
        continue;
      }
      aUniqueFaces.Add(aCoEdge.FaceDefId.Index);
    }

    const int aCachedCount  = aDefs.Edges().NbFaces(anEdgeId);
    const int anActualCount = aUniqueFaces.Extent();
    if (aCachedCount != anActualCount)
    {
      TCollection_AsciiString aDesc("Reverse index face-count cache mismatch: cached=");
      aDesc += TCollection_AsciiString(aCachedCount);
      aDesc += " actual=";
      aDesc += TCollection_AsciiString(anActualCount);
      theIssues.Append(Issue{Severity::Error, anEdgeId, aDesc});
    }
  }
}

//! Check that parent-child incidence refs (shell->face, solid->shell,
//! compound->child, compsolid->solid) reference non-removed entities
//! of the expected kind.
//! @param[in]     theGraph  source graph
//! @param[in,out] theIssues collection to append diagnostic issues
void checkIncidenceRefConsistency(const BRepGraph&                               theGraph,
                                  NCollection_Vector<BRepGraph_Validate::Issue>& theIssues)
{
  using Issue    = BRepGraph_Validate::Issue;
  using Severity = BRepGraph_Validate::Severity;

  // Check face->wire incidence refs.
  for (BRepGraph_Iterator<BRepGraphInc::FaceDef> aFaceIt(theGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraphInc::FaceDef& aFace   = aFaceIt.Current();
    const BRepGraph_FaceId       aFaceId = aFaceIt.CurrentId();

    for (BRepGraph_RefsWireOfFace anIt(theGraph, aFaceId); anIt.More(); anIt.Next())
    {
      const BRepGraphInc::WireRef& aWR     = theGraph.Refs().Wires().Entry(anIt.CurrentId());
      const BRepGraph_NodeId       aWireId = aWR.WireDefId;
      if (aWireId.IsValid() && !isValidNodeId(theGraph, aWireId))
      {
        theIssues.Append(
          Issue{Severity::Error, aFace.Id, "FaceDef.WireUsage WireIdx out of bounds"});
      }
    }
  }

  // Check shell->face incidence refs.
  for (BRepGraph_Iterator<BRepGraphInc::ShellDef> aShellIt(theGraph); aShellIt.More();
       aShellIt.Next())
  {
    const BRepGraphInc::ShellDef& aShell   = aShellIt.Current();
    const BRepGraph_ShellId       aShellId = aShellIt.CurrentId();

    for (BRepGraph_RefsFaceOfShell anIt(theGraph, aShellId); anIt.More(); anIt.Next())
    {
      const BRepGraphInc::FaceRef& aFR     = theGraph.Refs().Faces().Entry(anIt.CurrentId());
      const BRepGraph_NodeId       aFaceId = aFR.FaceDefId;
      if (aFaceId.IsValid() && isEntityRemoved(theGraph, aFaceId))
      {
        theIssues.Append(Issue{Severity::Error, aShell.Id, "ShellDef references removed FaceDef"});
      }
    }
  }

  // Check solid->shell incidence refs.
  for (BRepGraph_Iterator<BRepGraphInc::SolidDef> aSolidIt(theGraph); aSolidIt.More();
       aSolidIt.Next())
  {
    const BRepGraphInc::SolidDef& aSolid   = aSolidIt.Current();
    const BRepGraph_SolidId       aSolidId = aSolidIt.CurrentId();

    for (BRepGraph_RefsShellOfSolid anIt(theGraph, aSolidId); anIt.More(); anIt.Next())
    {
      const BRepGraphInc::ShellRef& aSR      = theGraph.Refs().Shells().Entry(anIt.CurrentId());
      const BRepGraph_NodeId        aShellId = aSR.ShellDefId;
      if (aShellId.IsValid() && isEntityRemoved(theGraph, aShellId))
      {
        theIssues.Append(Issue{Severity::Error, aSolid.Id, "SolidDef references removed ShellDef"});
      }
    }
  }
}

//! Validate that geometry representation ids (SurfaceRepId, Curve3DRepId,
//! Curve2DRepId, TriangulationRepIds, Polygon3DRepId) are in bounds and
//! reference non-null geometry handles.
//! @param[in]     theGraph  source graph
//! @param[in,out] theIssues collection to append diagnostic issues
void checkGeometryReferences(const BRepGraph&                               theGraph,
                             NCollection_Vector<BRepGraph_Validate::Issue>& theIssues)
{
  using Issue    = BRepGraph_Validate::Issue;
  using Severity = BRepGraph_Validate::Severity;

  // Check edge->curve references (handles stored directly on EdgeDef).
  for (BRepGraph_Iterator<BRepGraphInc::EdgeDef> anEdgeIt(theGraph); anEdgeIt.More();
       anEdgeIt.Next())
  {
    const BRepGraph_EdgeId       anEdgeId = anEdgeIt.CurrentId();
    const BRepGraphInc::EdgeDef& anEdge   = anEdgeIt.Current();

    if (!BRepGraph_Tool::Edge::Degenerated(theGraph, anEdgeId)
        && !BRepGraph_Tool::Edge::HasCurve(theGraph, anEdgeId))
    {
      theIssues.Append(
        Issue{Severity::Error, anEdge.Id, "Non-degenerate EdgeDef has no Curve3D representation"});
    }
    if (anEdge.Curve3DRepId.IsValid()
        && !anEdge.Curve3DRepId.IsValid(theGraph.Topo().Geometry().NbCurves3D()))
    {
      theIssues.Append(Issue{Severity::Error, anEdge.Id, "EdgeDef.Curve3DRepId out of bounds"});
    }
    if (anEdge.Polygon3DRepId.IsValid()
        && !anEdge.Polygon3DRepId.IsValid(theGraph.Topo().Poly().NbPolygons3D()))
    {
      theIssues.Append(Issue{Severity::Error, anEdge.Id, "EdgeDef.Polygon3DRepId out of bounds"});
    }
  }

  // Check face rep indices.
  for (BRepGraph_Iterator<BRepGraphInc::FaceDef> aFaceIt(theGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraphInc::FaceDef& aFace = aFaceIt.Current();
    if (aFace.SurfaceRepId.IsValid()
        && !aFace.SurfaceRepId.IsValid(theGraph.Topo().Geometry().NbSurfaces()))
    {
      theIssues.Append(Issue{Severity::Error, aFace.Id, "FaceDef.SurfaceRepId out of bounds"});
    }
    for (int aTriIdx = 0; aTriIdx < aFace.TriangulationRepIds.Length(); ++aTriIdx)
    {
      if (aFace.TriangulationRepIds.Value(aTriIdx).Index
          >= theGraph.Topo().Poly().NbTriangulations())
      {
        theIssues.Append(
          Issue{Severity::Error, aFace.Id, "FaceDef.TriangulationRepId out of bounds"});
        break;
      }
    }
  }

  // Check CoEdge PCurve data.
  for (BRepGraph_Iterator<BRepGraphInc::CoEdgeDef> aCoEdgeIt(theGraph); aCoEdgeIt.More();
       aCoEdgeIt.Next())
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge   = aCoEdgeIt.Current();
    const BRepGraph_CoEdgeId       aCoEdgeId = aCoEdgeIt.CurrentId();

    if (aCoEdge.FaceDefId.IsValid() && !BRepGraph_Tool::CoEdge::HasPCurve(theGraph, aCoEdgeId))
    {
      theIssues.Append(
        Issue{Severity::Error, aCoEdge.Id, "CoEdgeDef has no Curve2D representation"});
    }
  }
}

//! Verify that removed entities are not referenced by any active (non-removed)
//! parent entity through forward incidence refs.
//! @param[in]     theGraph  source graph
//! @param[in,out] theIssues collection to append diagnostic issues
void checkRemovedNodeIsolation(const BRepGraph&                               theGraph,
                               NCollection_Vector<BRepGraph_Validate::Issue>& theIssues)
{
  using Issue    = BRepGraph_Validate::Issue;
  using Severity = BRepGraph_Validate::Severity;

  // Non-removed edges must not reference removed vertices.
  for (BRepGraph_Iterator<BRepGraphInc::EdgeDef> anEdgeIt(theGraph); anEdgeIt.More();
       anEdgeIt.Next())
  {
    const BRepGraphInc::EdgeDef& anEdge = anEdgeIt.Current();

    if (anEdge.StartVertexRefId.IsValid())
    {
      const BRepGraph_NodeId aStartVtxId =
        theGraph.Refs().Vertices().Entry(anEdge.StartVertexRefId).VertexDefId;
      if (aStartVtxId.IsValid() && isEntityRemoved(theGraph, aStartVtxId))
      {
        theIssues.Append(Issue{Severity::Error,
                               anEdge.Id,
                               "Non-removed EdgeDef references removed StartVertexEntity"});
      }
    }
    if (anEdge.EndVertexRefId.IsValid())
    {
      const BRepGraph_NodeId anEndVtxId =
        theGraph.Refs().Vertices().Entry(anEdge.EndVertexRefId).VertexDefId;
      if (anEndVtxId.IsValid() && isEntityRemoved(theGraph, anEndVtxId))
      {
        theIssues.Append(Issue{Severity::Error,
                               anEdge.Id,
                               "Non-removed EdgeDef references removed EndVertexEntity"});
      }
    }
  }

  // Non-removed wires must not reference removed edges.
  for (BRepGraph_Iterator<BRepGraphInc::WireDef> aWireIt(theGraph); aWireIt.More(); aWireIt.Next())
  {
    const BRepGraphInc::WireDef& aWire   = aWireIt.Current();
    const BRepGraph_WireId       aWireId = aWireIt.CurrentId();

    bool hasRemovedEdge = false;
    for (BRepGraph_RefsCoEdgeOfWire anIt(theGraph, aWireId); anIt.More(); anIt.Next())
    {
      const BRepGraphInc::CoEdgeRef& aCR = theGraph.Refs().CoEdges().Entry(anIt.CurrentId());
      if (hasRemovedEdge)
      {
        break;
      }
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        theGraph.Topo().CoEdges().Definition(aCR.CoEdgeDefId);
      const BRepGraph_NodeId anEdgeId = aCoEdge.EdgeDefId;
      if (anEdgeId.IsValid() && isEntityRemoved(theGraph, anEdgeId))
      {
        hasRemovedEdge = true;
      }
    }
    if (hasRemovedEdge)
    {
      theIssues.Append(
        Issue{Severity::Error, aWire.Id, "Non-removed WireDef references removed EdgeDef"});
    }
  }
}

//! Check wire edge connectivity: each coedge's end vertex must match the
//! next coedge's start vertex. Uses BRepGraph_WireExplorer for
//! order-independent traversal.
//! @param[in]     theGraph  source graph
//! @param[in,out] theIssues collection to append diagnostic issues
void checkWireConnectivity(const BRepGraph&                               theGraph,
                           NCollection_Vector<BRepGraph_Validate::Issue>& theIssues)
{
  using Issue    = BRepGraph_Validate::Issue;
  using Severity = BRepGraph_Validate::Severity;

  auto edgeLookup = [&theGraph](int theIdx) -> const BRepGraphInc::EdgeDef& {
    return theGraph.Topo().Edges().Definition(BRepGraph_EdgeId(theIdx));
  };

  for (BRepGraph_Iterator<BRepGraphInc::WireDef> aWireIt(theGraph); aWireIt.More(); aWireIt.Next())
  {
    const BRepGraphInc::WireDef& aWire   = aWireIt.Current();
    const BRepGraph_WireId       aWireId = aWireIt.CurrentId();

    NCollection_Vector<BRepGraphInc::CoEdgeUsage> aWireCoEdgeRefs;
    for (BRepGraph_RefsCoEdgeOfWire anIt(theGraph, aWireId); anIt.More(); anIt.Next())
    {
      const BRepGraphInc::CoEdgeRef& aCRE = theGraph.Refs().CoEdges().Entry(anIt.CurrentId());
      BRepGraphInc::CoEdgeUsage      aCR;
      aCR.CoEdgeDefId   = aCRE.CoEdgeDefId;
      aCR.LocalLocation = aCRE.LocalLocation;
      aWireCoEdgeRefs.Append(aCR);
    }

    const int aNbCoEdges = aWireCoEdgeRefs.Length();
    if (aNbCoEdges < 2)
      continue;

    // Validate all edge indices first.
    bool aAllValid = true;
    for (int anIdx = 0; anIdx < aNbCoEdges; ++anIdx)
    {
      const BRepGraphInc::CoEdgeUsage& aCR = aWireCoEdgeRefs.Value(anIdx);
      const BRepGraphInc::CoEdgeDef&   aCoEdge =
        theGraph.Topo().CoEdges().Definition(aCR.CoEdgeDefId);
      const BRepGraph_NodeId anEdgeId = aCoEdge.EdgeDefId;
      if (!anEdgeId.IsValid() || !isValidNodeId(theGraph, anEdgeId))
      {
        aAllValid = false;
        break;
      }
    }
    if (!aAllValid)
      continue;

    // Build connection-ordered sequence via WireExplorer, then check
    // that all consecutive pairs in the ordered sequence are connected.
    auto coedgeLookup = [&theGraph](int theIdx) -> const BRepGraphInc::CoEdgeDef& {
      return theGraph.Topo().CoEdges().Definition(BRepGraph_CoEdgeId(theIdx));
    };
    auto vtxRefLookup = [&theGraph](const BRepGraph_VertexRefId theRefId) -> BRepGraph_VertexId {
      return theGraph.Refs().Vertices().Entry(theRefId).VertexDefId;
    };
    BRepGraph_WireExplorer anExp(aWireCoEdgeRefs, coedgeLookup, edgeLookup, vtxRefLookup);
    const NCollection_Vector<BRepGraphInc::CoEdgeUsage>& anOrdered = anExp.OrderedRefs();

    for (int anIdx = 0; anIdx < anOrdered.Length() - 1; ++anIdx)
    {
      const BRepGraphInc::CoEdgeUsage& aCurrCR = anOrdered.Value(anIdx);
      const BRepGraphInc::CoEdgeUsage& aNextCR = anOrdered.Value(anIdx + 1);

      const BRepGraphInc::CoEdgeDef& aCurrCoEdge =
        theGraph.Topo().CoEdges().Definition(aCurrCR.CoEdgeDefId);
      const BRepGraphInc::CoEdgeDef& aNextCoEdge =
        theGraph.Topo().CoEdges().Definition(aNextCR.CoEdgeDefId);

      const BRepGraphInc::EdgeDef& aCurrEdge =
        theGraph.Topo().Edges().Definition(aCurrCoEdge.EdgeDefId);
      const BRepGraphInc::EdgeDef& aNextEdge =
        theGraph.Topo().Edges().Definition(aNextCoEdge.EdgeDefId);

      // Resolve oriented end vertex of current edge.
      const BRepGraph_VertexRefId aCurrEndRefId = (aCurrCoEdge.Sense == TopAbs_FORWARD)
                                                    ? aCurrEdge.EndVertexRefId
                                                    : aCurrEdge.StartVertexRefId;
      const BRepGraph_NodeId      aCurrEnd =
        aCurrEndRefId.IsValid()
          ? BRepGraph_VertexId(theGraph.Refs().Vertices().Entry(aCurrEndRefId).VertexDefId.Index)
          : BRepGraph_NodeId();

      // Resolve oriented start vertex of next edge.
      const BRepGraph_VertexRefId aNextStartRefId = (aNextCoEdge.Sense == TopAbs_FORWARD)
                                                      ? aNextEdge.StartVertexRefId
                                                      : aNextEdge.EndVertexRefId;
      const BRepGraph_NodeId      aNextStart =
        aNextStartRefId.IsValid()
          ? BRepGraph_VertexId(theGraph.Refs().Vertices().Entry(aNextStartRefId).VertexDefId.Index)
          : BRepGraph_NodeId();

      if (aCurrEnd.IsValid() && aNextStart.IsValid() && aCurrEnd != aNextStart)
      {
        TCollection_AsciiString aDesc("Wire edges not connected: edge[");
        aDesc += TCollection_AsciiString(anIdx);
        aDesc += "] end != edge[";
        aDesc += TCollection_AsciiString(anIdx + 1);
        aDesc += "] start";
        theIssues.Append(Issue{Severity::Error, aWire.Id, aDesc});
      }
    }
  }
}

//! Validate that every entity's Id field matches its actual vector position:
//! Entity[i].Id must equal (Kind, i). Detects corruption from incorrect
//! Compact remapping or manual entity manipulation.
//! @param[in]     theGraph  source graph
//! @param[in,out] theIssues collection to append diagnostic issues
void checkDefIds(const BRepGraph&                               theGraph,
                 NCollection_Vector<BRepGraph_Validate::Issue>& theIssues)
{
  using Severity                   = BRepGraph_Validate::Severity;
  using Issue                      = BRepGraph_Validate::Issue;
  const BRepGraph::TopoView& aDefs = theGraph.Topo();

  auto checkKind = [&](BRepGraph_NodeId::Kind theKind, int theNb) {
    for (int anIdx = 0; anIdx < theNb; ++anIdx)
    {
      const BRepGraph_NodeId       anExpected(theKind, anIdx);
      const BRepGraphInc::BaseDef* aDef = aDefs.Gen().TopoEntity(anExpected);
      if (aDef == nullptr)
      {
        continue;
      }
      if (aDef->Id != anExpected)
      {
        TCollection_AsciiString aDesc("Entity Id mismatch: expected (");
        aDesc += TCollection_AsciiString(static_cast<int>(theKind));
        aDesc += ",";
        aDesc += TCollection_AsciiString(anIdx);
        aDesc += ") got (";
        aDesc += TCollection_AsciiString(static_cast<int>(aDef->Id.NodeKind));
        aDesc += ",";
        aDesc += TCollection_AsciiString(aDef->Id.Index);
        aDesc += ")";
        theIssues.Append(Issue{Severity::Error, anExpected, aDesc});
      }
    }
  };

  checkKind(BRepGraph_NodeId::Kind::Vertex, aDefs.Vertices().Nb());
  checkKind(BRepGraph_NodeId::Kind::Edge, aDefs.Edges().Nb());
  checkKind(BRepGraph_NodeId::Kind::CoEdge, aDefs.CoEdges().Nb());
  checkKind(BRepGraph_NodeId::Kind::Wire, aDefs.Wires().Nb());
  checkKind(BRepGraph_NodeId::Kind::Face, aDefs.Faces().Nb());
  checkKind(BRepGraph_NodeId::Kind::Shell, aDefs.Shells().Nb());
  checkKind(BRepGraph_NodeId::Kind::Solid, aDefs.Solids().Nb());
  checkKind(BRepGraph_NodeId::Kind::Compound, aDefs.Compounds().Nb());
  checkKind(BRepGraph_NodeId::Kind::CompSolid, aDefs.CompSolids().Nb());
  checkKind(BRepGraph_NodeId::Kind::Product, aDefs.Products().Nb());
  checkKind(BRepGraph_NodeId::Kind::Occurrence, aDefs.Occurrences().Nb());
}

//! Validate that NbActive*() counts match the actual number of non-removed
//! entities in each per-kind vector.
//! @param[in]     theGraph  source graph
//! @param[in,out] theIssues collection to append diagnostic issues
void checkActiveCounts(const BRepGraph&                               theGraph,
                       NCollection_Vector<BRepGraph_Validate::Issue>& theIssues)
{
  using Severity                   = BRepGraph_Validate::Severity;
  using Issue                      = BRepGraph_Validate::Issue;
  const BRepGraph::TopoView& aDefs = theGraph.Topo();

  auto countActive = [&](BRepGraph_NodeId::Kind theKind, int theNb) -> int {
    int aCount = 0;
    for (int anIdx = 0; anIdx < theNb; ++anIdx)
    {
      const BRepGraph_NodeId anId(theKind, anIdx);
      if (!isEntityRemoved(theGraph, anId))
      {
        ++aCount;
      }
    }
    return aCount;
  };

  auto verify = [&](const char* theName, int theActual, int theExpected) {
    if (theActual != theExpected)
    {
      TCollection_AsciiString aDesc("NbActive");
      aDesc += theName;
      aDesc += " mismatch: cached=";
      aDesc += TCollection_AsciiString(theActual);
      aDesc += " actual=";
      aDesc += TCollection_AsciiString(theExpected);
      theIssues.Append(Issue{Severity::Error, BRepGraph_NodeId(), aDesc});
    }
  };

  verify("Vertices",
         aDefs.Vertices().NbActive(),
         countActive(BRepGraph_NodeId::Kind::Vertex, aDefs.Vertices().Nb()));
  verify("Edges",
         aDefs.Edges().NbActive(),
         countActive(BRepGraph_NodeId::Kind::Edge, aDefs.Edges().Nb()));
  verify("CoEdges",
         aDefs.CoEdges().NbActive(),
         countActive(BRepGraph_NodeId::Kind::CoEdge, aDefs.CoEdges().Nb()));
  verify("Wires",
         aDefs.Wires().NbActive(),
         countActive(BRepGraph_NodeId::Kind::Wire, aDefs.Wires().Nb()));
  verify("Faces",
         aDefs.Faces().NbActive(),
         countActive(BRepGraph_NodeId::Kind::Face, aDefs.Faces().Nb()));
  verify("Shells",
         aDefs.Shells().NbActive(),
         countActive(BRepGraph_NodeId::Kind::Shell, aDefs.Shells().Nb()));
  verify("Solids",
         aDefs.Solids().NbActive(),
         countActive(BRepGraph_NodeId::Kind::Solid, aDefs.Solids().Nb()));
  verify("Compounds",
         aDefs.Compounds().NbActive(),
         countActive(BRepGraph_NodeId::Kind::Compound, aDefs.Compounds().Nb()));
  verify("CompSolids",
         aDefs.CompSolids().NbActive(),
         countActive(BRepGraph_NodeId::Kind::CompSolid, aDefs.CompSolids().Nb()));
  verify("Products",
         aDefs.Products().NbActive(),
         countActive(BRepGraph_NodeId::Kind::Product, aDefs.Products().Nb()));
  verify("Occurrences",
         aDefs.Occurrences().NbActive(),
         countActive(BRepGraph_NodeId::Kind::Occurrence, aDefs.Occurrences().Nb()));
}

} // namespace

//=================================================================================================

BRepGraph_Validate::Result BRepGraph_Validate::Perform(const BRepGraph& theGraph)
{
  return Perform(theGraph, Options::Lightweight());
}

//=================================================================================================

BRepGraph_Validate::Result BRepGraph_Validate::Perform(const BRepGraph& theGraph,
                                                       const Mode       theMode)
{
  Options anOptions;
  anOptions.ValidationMode = theMode;
  return Perform(theGraph, anOptions);
}

//=================================================================================================

BRepGraph_Validate::Result BRepGraph_Validate::Perform(const BRepGraph& theGraph,
                                                       const Options&   theOptions)
{
  Result aResult;
  if (!theGraph.IsDone())
  {
    return aResult;
  }

  if (theOptions.ValidationMode == Mode::Lightweight)
  {
    NCollection_Vector<BRepGraph::BuilderView::BoundaryIssue> aBoundaryIssues;
    if (!theGraph.Builder().ValidateMutationBoundary(&aBoundaryIssues))
    {
      appendMutationBoundaryIssues(aBoundaryIssues, aResult.Issues);
    }
    return aResult;
  }

  NCollection_Vector<BRepGraph::BuilderView::BoundaryIssue> aBoundaryIssues;
  if (!theGraph.Builder().ValidateMutationBoundary(&aBoundaryIssues))
  {
    appendMutationBoundaryIssues(aBoundaryIssues, aResult.Issues);
  }

  checkCrossReferenceBounds(theGraph, aResult.Issues);
  checkReverseIndexConsistency(theGraph, aResult.Issues);
  checkReverseIndexFaceCountCache(theGraph, aResult.Issues);
  checkIncidenceRefConsistency(theGraph, aResult.Issues);
  checkGeometryReferences(theGraph, aResult.Issues);
  checkRemovedNodeIsolation(theGraph, aResult.Issues);
  checkWireConnectivity(theGraph, aResult.Issues);

  checkDefIds(theGraph, aResult.Issues);
  checkActiveCounts(theGraph, aResult.Issues);

  // UID integrity checks: all active nodes must have a valid UID that round-trips.
  const BRepGraph::TopoView& aDefs = theGraph.Topo();
  const BRepGraph::UIDsView& aUIDs = theGraph.UIDs();
  auto checkUIDKind                = [&](const BRepGraph_NodeId::Kind theKind, const int theNb) {
    for (int anIdx = 0; anIdx < theNb; ++anIdx)
    {
      const BRepGraph_NodeId       aNode(theKind, anIdx);
      const BRepGraphInc::BaseDef* aDef = aDefs.Gen().TopoEntity(aNode);
      if (aDef == nullptr || aDef->IsRemoved)
      {
        continue;
      }

      const BRepGraph_UID aUID = aUIDs.Of(aNode);
      if (!aUID.IsValid())
      {
        aResult.Issues.Append(
          Issue{Severity::Error, aNode, "Node has missing or invalid UID in deep audit"});
        continue;
      }

      if (!aUIDs.Has(aUID))
      {
        aResult.Issues.Append(Issue{Severity::Error,
                                    aNode,
                                    "Node UID is not present in UIDs view for this generation"});
        continue;
      }

      if (aUIDs.NodeIdFrom(aUID) != aNode)
      {
        aResult.Issues.Append(
          Issue{Severity::Error, aNode, "Node UID does not round-trip to originating NodeId"});
      }
    }
  };

  checkUIDKind(BRepGraph_NodeId::Kind::Vertex, aDefs.Vertices().Nb());
  checkUIDKind(BRepGraph_NodeId::Kind::Edge, aDefs.Edges().Nb());
  checkUIDKind(BRepGraph_NodeId::Kind::CoEdge, aDefs.CoEdges().Nb());
  checkUIDKind(BRepGraph_NodeId::Kind::Wire, aDefs.Wires().Nb());
  checkUIDKind(BRepGraph_NodeId::Kind::Face, aDefs.Faces().Nb());
  checkUIDKind(BRepGraph_NodeId::Kind::Shell, aDefs.Shells().Nb());
  checkUIDKind(BRepGraph_NodeId::Kind::Solid, aDefs.Solids().Nb());
  checkUIDKind(BRepGraph_NodeId::Kind::Compound, aDefs.Compounds().Nb());
  checkUIDKind(BRepGraph_NodeId::Kind::CompSolid, aDefs.CompSolids().Nb());
  checkUIDKind(BRepGraph_NodeId::Kind::Product, aDefs.Products().Nb());
  checkUIDKind(BRepGraph_NodeId::Kind::Occurrence, aDefs.Occurrences().Nb());

  // Assembly DAG cycle detection: for each product, check if it can
  // reach itself through its occurrence->product chain.
  // Shared references (two occurrences pointing to the same child) are valid;
  // only self-reachability constitutes a cycle.
  for (BRepGraph_Iterator<BRepGraphInc::ProductDef> aProdIt(theGraph); aProdIt.More();
       aProdIt.Next())
  {
    const BRepGraph_ProductId aProdId = aProdIt.CurrentId();

    // BFS from this product's children; skip already-visited to avoid
    // exponential blowup on DAGs. A cycle exists if we re-encounter aProdIdx.
    NCollection_Map<int>                    aVisited;
    NCollection_Vector<BRepGraph_ProductId> aQueue;
    int                                     aHead = 0;

    // Seed with direct children.
    for (BRepGraph_RefsOccurrenceOfProduct anOccIt(theGraph, aProdId); anOccIt.More();
         anOccIt.Next())
    {
      const BRepGraphInc::OccurrenceRef& anOccRef =
        theGraph.Refs().Occurrences().Entry(anOccIt.CurrentId());
      const BRepGraphInc::OccurrenceDef& anOcc =
        aDefs.Occurrences().Definition(anOccRef.OccurrenceDefId);
      if (anOcc.IsRemoved || !anOcc.ProductDefId.IsValid(aDefs.Products().Nb()))
        continue;
      if (anOcc.ProductDefId.Index == aProdId.Index)
      {
        aResult.Issues.Append(
          Issue{Severity::Error,
                aProdId,
                "Assembly cycle: Product directly references itself via occurrence"});
        break;
      }
      if (aVisited.Add(anOcc.ProductDefId.Index))
        aQueue.Append(anOcc.ProductDefId);
    }

    bool aCycleFound = false;
    while (aHead < aQueue.Length() && !aCycleFound)
    {
      const BRepGraph_ProductId aChildProdId = aQueue.Value(aHead);
      ++aHead;
      const BRepGraphInc::ProductDef& aChildProd = aDefs.Products().Definition(aChildProdId);
      if (aChildProd.IsRemoved)
        continue;
      for (BRepGraph_RefsOccurrenceOfProduct aRefIt(theGraph, aChildProdId); aRefIt.More();
           aRefIt.Next())
      {
        const BRepGraphInc::OccurrenceRef& aRef =
          theGraph.Refs().Occurrences().Entry(aRefIt.CurrentId());
        const BRepGraphInc::OccurrenceDef& aOcc =
          aDefs.Occurrences().Definition(aRef.OccurrenceDefId);
        if (aOcc.IsRemoved || !aOcc.ProductDefId.IsValid(aDefs.Products().Nb()))
          continue;
        if (aOcc.ProductDefId.Index == aProdId.Index)
        {
          aResult.Issues.Append(
            Issue{Severity::Error,
                  aProdId,
                  "Assembly cycle: Product reaches itself through occurrence chain"});
          aCycleFound = true;
          break;
        }
        if (aVisited.Add(aOcc.ProductDefId.Index))
          aQueue.Append(aOcc.ProductDefId);
      }
    }
  }

  return aResult;
}
