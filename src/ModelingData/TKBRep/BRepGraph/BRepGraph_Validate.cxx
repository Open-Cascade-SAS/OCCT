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

#include <BRepGraph_MeshCache.hxx>
#include <BRepGraph_MeshView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_EditorView.hxx>
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
  {
    return false;
  }

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
  const NCollection_DynamicArray<BRepGraph::EditorView::BoundaryIssue>& theBoundaryIssues,
  NCollection_DynamicArray<BRepGraph_Validate::Issue>&                  theIssues)
{
  using Issue    = BRepGraph_Validate::Issue;
  using Severity = BRepGraph_Validate::Severity;
  for (const BRepGraph::EditorView::BoundaryIssue& aBoundaryIssue : theBoundaryIssues)
  {
    theIssues.Append(Issue{Severity::Error, aBoundaryIssue.NodeId, aBoundaryIssue.Description});
  }
}

//! Validate that all cross-reference indices (vertex, edge, wire, face, shell,
//! coedge, compound child refs) point to valid, in-bounds entity slots.
//! @param[in]     theGraph  source graph
//! @param[in,out] theIssues collection to append diagnostic issues
void checkCrossReferenceBounds(const BRepGraph&                                     theGraph,
                               NCollection_DynamicArray<BRepGraph_Validate::Issue>& theIssues)
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
                               anEdgeIt.CurrentId(),
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
                               anEdgeIt.CurrentId(),
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
      theIssues.Append(Issue{Severity::Error, aCoEdgeId, "CoEdgeDef.FaceDefId out of bounds"});
    }
    const BRepGraph_NodeId anEdgeId = aCoEdge.EdgeDefId;
    if (anEdgeId.IsValid() && !isValidNodeId(theGraph, anEdgeId))
    {
      theIssues.Append(Issue{Severity::Error, aCoEdgeId, "CoEdgeDef.EdgeDefId out of bounds"});
    }
    // SeamPairId consistency.
    if (aCoEdge.SeamPairId.IsValid())
    {
      if (!aCoEdge.SeamPairId.IsValid(theGraph.Topo().CoEdges().Nb()))
      {
        theIssues.Append(Issue{Severity::Error, aCoEdgeId, "CoEdgeDef.SeamPairId out of bounds"});
      }
      else if (aCoEdge.SeamPairId == aCoEdgeId)
      {
        theIssues.Append(
          Issue{Severity::Error, aCoEdgeId, "CoEdgeDef.SeamPairId is self-reference"});
      }
      else
      {
        const BRepGraphInc::CoEdgeDef& aPair =
          theGraph.Topo().CoEdges().Definition(aCoEdge.SeamPairId);
        if (aPair.SeamPairId != aCoEdgeId)
        {
          theIssues.Append(
            Issue{Severity::Error, aCoEdgeId, "CoEdgeDef.SeamPairId not bidirectional"});
        }
        if (aPair.FaceDefId != aCoEdge.FaceDefId)
        {
          theIssues.Append(
            Issue{Severity::Error, aCoEdgeId, "CoEdgeDef seam pair has different FaceDefId"});
        }
      }
    }
    // Rep index bounds.
    if (aCoEdge.Curve2DRepId.IsValid()
        && !aCoEdge.Curve2DRepId.IsValid(theGraph.Topo().Geometry().NbCurves2D()))
    {
      theIssues.Append(Issue{Severity::Error, aCoEdgeId, "CoEdgeDef.Curve2DRepId out of bounds"});
    }
  }

  // Surface handles are stored directly on FaceDef; no cross-reference to validate.

  // Check WireDef CoEdgeRef references.
  for (BRepGraph_Iterator<BRepGraphInc::WireDef> aWireIt(theGraph); aWireIt.More(); aWireIt.Next())
  {
    const BRepGraph_WireId aWireId = aWireIt.CurrentId();

    for (BRepGraph_RefsCoEdgeOfWire anIt(theGraph, aWireId); anIt.More(); anIt.Next())
    {
      const BRepGraphInc::CoEdgeRef& aCR = theGraph.Refs().CoEdges().Entry(anIt.CurrentId());
      const BRepGraph_NodeId         aCoEdgeDefId = aCR.CoEdgeDefId;
      if (aCoEdgeDefId.IsValid() && !isValidNodeId(theGraph, aCoEdgeDefId))
      {
        theIssues.Append(
          Issue{Severity::Error, aWireId, "WireDef.CoEdgeUsage CoEdgeIdx out of bounds"});
      }
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        theGraph.Topo().CoEdges().Definition(aCR.CoEdgeDefId);
      const BRepGraph_NodeId anEdgeDefId = aCoEdge.EdgeDefId;
      if (anEdgeDefId.IsValid() && !isValidNodeId(theGraph, anEdgeDefId))
      {
        theIssues.Append(
          Issue{Severity::Error, aWireId, "WireDef.CoEdgeUsage EdgeIdx out of bounds"});
      }
    }
  }

  // Check CompoundDef ChildRef references.
  for (BRepGraph_Iterator<BRepGraphInc::CompoundDef> aCompIt(theGraph); aCompIt.More();
       aCompIt.Next())
  {
    const BRepGraph_CompoundId aCompoundId = aCompIt.CurrentId();

    for (BRepGraph_RefsChildOfCompound anIt(theGraph, aCompoundId); anIt.More(); anIt.Next())
    {
      const BRepGraphInc::ChildRef& aCR      = theGraph.Refs().Children().Entry(anIt.CurrentId());
      const BRepGraph_NodeId        aChildId = aCR.ChildDefId;
      if (aChildId.IsValid() && !isValidNodeId(theGraph, aChildId))
      {
        theIssues.Append(
          Issue{Severity::Error, aCompoundId, "CompoundDef.ChildDefId out of bounds"});
      }
    }
  }

  // Check CompSolidDef SolidRef references.
  for (BRepGraph_Iterator<BRepGraphInc::CompSolidDef> aCSIt(theGraph); aCSIt.More(); aCSIt.Next())
  {
    const BRepGraph_CompSolidId aCompSolidId = aCSIt.CurrentId();

    for (BRepGraph_RefsSolidOfCompSolid anIt(theGraph, aCompSolidId); anIt.More(); anIt.Next())
    {
      const BRepGraphInc::SolidRef& aSR      = theGraph.Refs().Solids().Entry(anIt.CurrentId());
      const BRepGraph_NodeId        aSolidId = aSR.SolidDefId;
      if (aSolidId.IsValid() && !isValidNodeId(theGraph, aSolidId))
      {
        theIssues.Append(
          Issue{Severity::Error, aCompSolidId, "CompSolidDef.SolidDefId out of bounds"});
      }
    }
  }

  // Check ShellDef FaceRef references.
  for (BRepGraph_Iterator<BRepGraphInc::ShellDef> aShellIt(theGraph); aShellIt.More();
       aShellIt.Next())
  {
    const BRepGraph_ShellId aShellId = aShellIt.CurrentId();

    for (BRepGraph_RefsFaceOfShell anIt(theGraph, aShellId); anIt.More(); anIt.Next())
    {
      const BRepGraphInc::FaceRef& aFR        = theGraph.Refs().Faces().Entry(anIt.CurrentId());
      const BRepGraph_NodeId       aFaceDefId = aFR.FaceDefId;
      if (aFaceDefId.IsValid() && !isValidNodeId(theGraph, aFaceDefId))
      {
        theIssues.Append(
          Issue{Severity::Error, aShellId, "ShellDef.FaceUsage FaceIdx out of bounds"});
      }
    }
  }

  // Check SolidDef ShellRef references.
  for (BRepGraph_Iterator<BRepGraphInc::SolidDef> aSolidIt(theGraph); aSolidIt.More();
       aSolidIt.Next())
  {
    const BRepGraph_SolidId aSolidId = aSolidIt.CurrentId();

    for (BRepGraph_RefsShellOfSolid anIt(theGraph, aSolidId); anIt.More(); anIt.Next())
    {
      const BRepGraphInc::ShellRef& aSR         = theGraph.Refs().Shells().Entry(anIt.CurrentId());
      const BRepGraph_NodeId        aShellDefId = aSR.ShellDefId;
      if (aShellDefId.IsValid() && !isValidNodeId(theGraph, aShellDefId))
      {
        theIssues.Append(
          Issue{Severity::Error, aSolidId, "SolidDef.ShellUsage ShellIdx out of bounds"});
      }
    }
  }

  // Check ProductDef references.
  for (BRepGraph_Iterator<BRepGraphInc::ProductDef> aProductIt(theGraph); aProductIt.More();
       aProductIt.Next())
  {
    const BRepGraph_ProductId aProdId = aProductIt.CurrentId();

    for (BRepGraph_RefsOccurrenceOfProduct anOccIt(theGraph, aProdId); anOccIt.More();
         anOccIt.Next())
    {
      const BRepGraph_OccurrenceId anOccId =
        theGraph.Refs().Occurrences().Entry(anOccIt.CurrentId()).OccurrenceDefId;
      if (anOccId.IsValid() && !isValidNodeId(theGraph, anOccId))
      {
        theIssues.Append(
          Issue{Severity::Error, aProdId, "ProductDef.OccurrenceUsage out of bounds"});
      }
    }
  }

  // Check OccurrenceDef references.
  for (BRepGraph_Iterator<BRepGraphInc::OccurrenceDef> anOccIt(theGraph); anOccIt.More();
       anOccIt.Next())
  {
    const BRepGraphInc::OccurrenceDef& anOcc    = anOccIt.Current();
    const BRepGraph_OccurrenceId       anOccId  = anOccIt.CurrentId();
    const BRepGraph_NodeId             aChildId = anOcc.ChildDefId;
    const BRepGraph_NodeId::Kind       aKind    = aChildId.NodeKind;

    if (!aChildId.IsValid())
    {
      theIssues.Append(Issue{Severity::Error, anOccId, "OccurrenceDef.ChildDefId invalid"});
      continue;
    }

    const bool isKindAllowed =
      aKind == BRepGraph_NodeId::Kind::Product || BRepGraph_NodeId::IsTopologyKind(aKind);
    if (!isKindAllowed)
    {
      theIssues.Append(
        Issue{Severity::Error,
              anOccId,
              aKind == BRepGraph_NodeId::Kind::Occurrence
                ? "OccurrenceDef.ChildDefId cannot reference an Occurrence"
                : "OccurrenceDef.ChildDefId kind is not Product or a topology kind"});
      continue;
    }

    if (!isValidNodeId(theGraph, aChildId) || isEntityRemoved(theGraph, aChildId))
    {
      theIssues.Append(Issue{Severity::Error, anOccId, "OccurrenceDef.ChildDefId invalid"});
    }
  }
}

//! Verify that every forward incidence ref has a matching reverse-index entry
//! (edge->wires, edge->faces, vertex->edges, wire->faces, face->shells, shell->solids).
//! @param[in]     theGraph  source graph
//! @param[in,out] theIssues collection to append diagnostic issues
void checkReverseIndexConsistency(const BRepGraph&                                     theGraph,
                                  NCollection_DynamicArray<BRepGraph_Validate::Issue>& theIssues)
{
  using Issue    = BRepGraph_Validate::Issue;
  using Severity = BRepGraph_Validate::Severity;

  // Build expected edge->wires mapping from CoEdgeRef scans.
  NCollection_DataMap<BRepGraph_EdgeId, NCollection_Map<BRepGraph_WireId>> anExpected;
  for (BRepGraph_Iterator<BRepGraphInc::WireDef> aWireIt(theGraph); aWireIt.More(); aWireIt.Next())
  {
    const BRepGraph_WireId aWireId = aWireIt.CurrentId();

    for (BRepGraph_RefsCoEdgeOfWire anIt(theGraph, aWireId); anIt.More(); anIt.Next())
    {
      const BRepGraphInc::CoEdgeRef& aCR = theGraph.Refs().CoEdges().Entry(anIt.CurrentId());
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        theGraph.Topo().CoEdges().Definition(aCR.CoEdgeDefId);
      if (!aCoEdge.EdgeDefId.IsValid())
      {
        continue;
      }

      if (!anExpected.IsBound(aCoEdge.EdgeDefId))
      {
        anExpected.Bind(aCoEdge.EdgeDefId, NCollection_Map<BRepGraph_WireId>());
      }
      anExpected.ChangeFind(aCoEdge.EdgeDefId).Add(aWireId);
    }
  }

  // Check that Topo WiresOfEdge matches expected.
  for (BRepGraph_Iterator<BRepGraphInc::EdgeDef> anEdgeIt(theGraph); anEdgeIt.More();
       anEdgeIt.Next())
  {
    const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();

    const NCollection_DynamicArray<BRepGraph_WireId>& aActualWires =
      theGraph.Topo().Edges().Wires(anEdgeId);
    const NCollection_Map<BRepGraph_WireId>* anExpectedWires = anExpected.Seek(anEdgeId);

    const int anExpectedCount = (anExpectedWires != nullptr) ? anExpectedWires->Extent() : 0;

    // Build a set from actual wires for comparison.
    NCollection_Map<BRepGraph_WireId> anActualSet;
    for (const BRepGraph_WireId& aWireId : aActualWires)
    {
      anActualSet.Add(aWireId);
    }

    if (anActualSet.Extent() != anExpectedCount)
    {
      theIssues.Append(Issue{Severity::Error,
                             anEdgeIt.CurrentId(),
                             "Reverse index ReverseIdx.WiresOfEdge size mismatch"});
      continue;
    }

    if (anExpectedWires != nullptr)
    {
      for (const BRepGraph_WireId& aWireId : *anExpectedWires)
      {
        if (!anActualSet.Contains(aWireId))
        {
          theIssues.Append(Issue{Severity::Error,
                                 anEdgeIt.CurrentId(),
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
void checkReverseIndexFaceCountCache(const BRepGraph&                                     theGraph,
                                     NCollection_DynamicArray<BRepGraph_Validate::Issue>& theIssues)
{
  using Issue                      = BRepGraph_Validate::Issue;
  using Severity                   = BRepGraph_Validate::Severity;
  const BRepGraph::TopoView& aDefs = theGraph.Topo();

  for (BRepGraph_Iterator<BRepGraphInc::EdgeDef> anEdgeIt(theGraph); anEdgeIt.More();
       anEdgeIt.Next())
  {
    const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();

    NCollection_Map<BRepGraph_FaceId>                   aUniqueFaces;
    const NCollection_DynamicArray<BRepGraph_CoEdgeId>& aCoEdges = aDefs.Edges().CoEdges(anEdgeId);
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdges)
    {
      const BRepGraphInc::CoEdgeDef& aCoEdge = aDefs.CoEdges().Definition(aCoEdgeId);
      if (aCoEdge.IsRemoved || !aCoEdge.FaceDefId.IsValid())
      {
        continue;
      }
      aUniqueFaces.Add(aCoEdge.FaceDefId);
    }

    const uint32_t aCachedCount  = aDefs.Edges().NbFaces(anEdgeId);
    const int      anActualCount = aUniqueFaces.Extent();
    if (static_cast<int>(aCachedCount) != anActualCount)
    {
      TCollection_AsciiString aDesc("Reverse index face-count cache mismatch: cached=");
      aDesc += TCollection_AsciiString(static_cast<int>(aCachedCount));
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
void checkIncidenceRefConsistency(const BRepGraph&                                     theGraph,
                                  NCollection_DynamicArray<BRepGraph_Validate::Issue>& theIssues)
{
  using Issue    = BRepGraph_Validate::Issue;
  using Severity = BRepGraph_Validate::Severity;

  // Check face->wire incidence refs.
  for (BRepGraph_Iterator<BRepGraphInc::FaceDef> aFaceIt(theGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();

    for (BRepGraph_RefsWireOfFace anIt(theGraph, aFaceId); anIt.More(); anIt.Next())
    {
      const BRepGraphInc::WireRef& aWR     = theGraph.Refs().Wires().Entry(anIt.CurrentId());
      const BRepGraph_NodeId       aWireId = aWR.WireDefId;
      if (aWireId.IsValid() && !isValidNodeId(theGraph, aWireId))
      {
        theIssues.Append(
          Issue{Severity::Error, aFaceIt.CurrentId(), "FaceDef.WireUsage WireIdx out of bounds"});
      }
    }
  }

  // Check shell->face incidence refs.
  for (BRepGraph_Iterator<BRepGraphInc::ShellDef> aShellIt(theGraph); aShellIt.More();
       aShellIt.Next())
  {
    const BRepGraph_ShellId aShellId = aShellIt.CurrentId();

    for (BRepGraph_RefsFaceOfShell anIt(theGraph, aShellId); anIt.More(); anIt.Next())
    {
      const BRepGraphInc::FaceRef& aFR     = theGraph.Refs().Faces().Entry(anIt.CurrentId());
      const BRepGraph_NodeId       aFaceId = aFR.FaceDefId;
      if (aFaceId.IsValid() && isEntityRemoved(theGraph, aFaceId))
      {
        theIssues.Append(Issue{Severity::Error, aShellId, "ShellDef references removed FaceDef"});
      }
    }
  }

  // Check solid->shell incidence refs.
  for (BRepGraph_Iterator<BRepGraphInc::SolidDef> aSolidIt(theGraph); aSolidIt.More();
       aSolidIt.Next())
  {
    const BRepGraph_SolidId aSolidId = aSolidIt.CurrentId();

    for (BRepGraph_RefsShellOfSolid anIt(theGraph, aSolidId); anIt.More(); anIt.Next())
    {
      const BRepGraphInc::ShellRef& aSR      = theGraph.Refs().Shells().Entry(anIt.CurrentId());
      const BRepGraph_NodeId        aShellId = aSR.ShellDefId;
      if (aShellId.IsValid() && isEntityRemoved(theGraph, aShellId))
      {
        theIssues.Append(Issue{Severity::Error, aSolidId, "SolidDef references removed ShellDef"});
      }
    }
  }
}

//! Validate that geometry representation ids (SurfaceRepId, Curve3DRepId,
//! Curve2DRepId, TriangulationRepIds, Polygon3DRepId) are in bounds and
//! reference non-null geometry handles.
//! @param[in]     theGraph  source graph
//! @param[in,out] theIssues collection to append diagnostic issues
void checkGeometryReferences(const BRepGraph&                                     theGraph,
                             NCollection_DynamicArray<BRepGraph_Validate::Issue>& theIssues)
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
      theIssues.Append(Issue{Severity::Error,
                             anEdgeIt.CurrentId(),
                             "Non-degenerate EdgeDef has no Curve3D representation"});
    }
    if (anEdge.Curve3DRepId.IsValid()
        && !anEdge.Curve3DRepId.IsValid(theGraph.Topo().Geometry().NbCurves3D()))
    {
      theIssues.Append(
        Issue{Severity::Error, anEdgeIt.CurrentId(), "EdgeDef.Curve3DRepId out of bounds"});
    }
    if (anEdge.Polygon3DRepId.IsValid()
        && !anEdge.Polygon3DRepId.IsValid(theGraph.Mesh().Poly().NbPolygons3D()))
    {
      theIssues.Append(
        Issue{Severity::Error, anEdgeIt.CurrentId(), "EdgeDef.Polygon3DRepId out of bounds"});
    }
  }

  // Check face rep indices.
  for (BRepGraph_Iterator<BRepGraphInc::FaceDef> aFaceIt(theGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraphInc::FaceDef& aFace = aFaceIt.Current();
    if (aFace.SurfaceRepId.IsValid()
        && !aFace.SurfaceRepId.IsValid(theGraph.Topo().Geometry().NbSurfaces()))
    {
      theIssues.Append(
        Issue{Severity::Error, aFaceIt.CurrentId(), "FaceDef.SurfaceRepId out of bounds"});
    }
    if (aFace.TriangulationRepId.IsValid()
        && !aFace.TriangulationRepId.IsValid(theGraph.Mesh().Poly().NbTriangulations()))
    {
      theIssues.Append(
        Issue{Severity::Error, aFaceIt.CurrentId(), "FaceDef.TriangulationRepId out of bounds"});
    }
    // Validate cached mesh entry bounds.
    const BRepGraph_MeshCache::FaceMeshEntry* aCachedFace =
      theGraph.Mesh().Faces().CachedMesh(aFaceIt.CurrentId());
    if (aCachedFace != nullptr)
    {
      for (const BRepGraph_TriangulationRepId& aCTriRepId : aCachedFace->TriangulationRepIds)
      {
        if (!aCTriRepId.IsValid(theGraph.Mesh().Poly().NbTriangulations()))
        {
          theIssues.Append(Issue{Severity::Error,
                                 aFaceIt.CurrentId(),
                                 "MeshCache.FaceMesh.TriangulationRepId out of bounds"});
          break;
        }
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
        Issue{Severity::Error, aCoEdgeId, "CoEdgeDef has no Curve2D representation"});
    }
  }

  // Orphan-rep detection: an active def that forward-references a rep which
  // has been soft-removed leaves the rep id pointing at dead geometry. The
  // rep should have been cleared (or the def removed) as part of the same
  // mutation; surface the asymmetry.
  const BRepGraph::TopoView::GeometryOps& aGeom = theGraph.Topo().Geometry();
  const BRepGraph::MeshView::PolyOps&     aPoly = theGraph.Mesh().Poly();
  for (BRepGraph_Iterator<BRepGraphInc::EdgeDef> anEdgeIt(theGraph); anEdgeIt.More();
       anEdgeIt.Next())
  {
    const BRepGraphInc::EdgeDef& anEdge = anEdgeIt.Current();
    if (anEdge.Curve3DRepId.IsValid() && anEdge.Curve3DRepId.IsValid(aGeom.NbCurves3D())
        && aGeom.Curve3DRep(anEdge.Curve3DRepId).IsRemoved)
    {
      theIssues.Append(Issue{Severity::Error,
                             anEdgeIt.CurrentId(),
                             "EdgeDef.Curve3DRepId points to a removed Curve3DRep"});
    }
    if (anEdge.Polygon3DRepId.IsValid() && anEdge.Polygon3DRepId.IsValid(aPoly.NbPolygons3D())
        && aPoly.Polygon3DRep(anEdge.Polygon3DRepId).IsRemoved)
    {
      theIssues.Append(Issue{Severity::Error,
                             anEdgeIt.CurrentId(),
                             "EdgeDef.Polygon3DRepId points to a removed Polygon3DRep"});
    }
  }
  for (BRepGraph_Iterator<BRepGraphInc::FaceDef> aFaceIt(theGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraphInc::FaceDef& aFace = aFaceIt.Current();
    if (aFace.SurfaceRepId.IsValid() && aFace.SurfaceRepId.IsValid(aGeom.NbSurfaces())
        && aGeom.SurfaceRep(aFace.SurfaceRepId).IsRemoved)
    {
      theIssues.Append(Issue{Severity::Error,
                             aFaceIt.CurrentId(),
                             "FaceDef.SurfaceRepId points to a removed SurfaceRep"});
    }
    if (aFace.TriangulationRepId.IsValid()
        && aFace.TriangulationRepId.IsValid(aPoly.NbTriangulations())
        && aPoly.TriangulationRep(aFace.TriangulationRepId).IsRemoved)
    {
      theIssues.Append(Issue{Severity::Error,
                             aFaceIt.CurrentId(),
                             "FaceDef.TriangulationRepId points to a removed TriangulationRep"});
    }
  }
  for (BRepGraph_Iterator<BRepGraphInc::CoEdgeDef> aCoEdgeIt(theGraph); aCoEdgeIt.More();
       aCoEdgeIt.Next())
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = aCoEdgeIt.Current();
    if (aCoEdge.Curve2DRepId.IsValid() && aCoEdge.Curve2DRepId.IsValid(aGeom.NbCurves2D())
        && aGeom.Curve2DRep(aCoEdge.Curve2DRepId).IsRemoved)
    {
      theIssues.Append(Issue{Severity::Error,
                             aCoEdgeIt.CurrentId(),
                             "CoEdgeDef.Curve2DRepId points to a removed Curve2DRep"});
    }
  }
}

//! Verify that removed entities are not referenced by any active (non-removed)
//! parent entity through forward incidence refs.
//! @param[in]     theGraph  source graph
//! @param[in,out] theIssues collection to append diagnostic issues
void checkRemovedNodeIsolation(const BRepGraph&                                     theGraph,
                               NCollection_DynamicArray<BRepGraph_Validate::Issue>& theIssues)
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
                               anEdgeIt.CurrentId(),
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
                               anEdgeIt.CurrentId(),
                               "Non-removed EdgeDef references removed EndVertexEntity"});
      }
    }
  }

  // Non-removed wires must not reference removed edges.
  for (BRepGraph_Iterator<BRepGraphInc::WireDef> aWireIt(theGraph); aWireIt.More(); aWireIt.Next())
  {
    const BRepGraph_WireId aWireId = aWireIt.CurrentId();

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
        Issue{Severity::Error, aWireId, "Non-removed WireDef references removed EdgeDef"});
    }
  }

  // Non-removed coedges must not reference removed faces.
  for (BRepGraph_Iterator<BRepGraphInc::CoEdgeDef> aCoEdgeIt(theGraph); aCoEdgeIt.More();
       aCoEdgeIt.Next())
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = aCoEdgeIt.Current();
    const BRepGraph_NodeId         aFaceId = aCoEdge.FaceDefId;
    if (aFaceId.IsValid() && isEntityRemoved(theGraph, aFaceId))
    {
      theIssues.Append(Issue{Severity::Error,
                             aCoEdgeIt.CurrentId(),
                             "Non-removed CoEdgeDef.FaceDefId references removed FaceDef"});
    }
  }
}

//! Check wire edge connectivity: each coedge's end vertex must match the
//! next coedge's start vertex. Uses BRepGraph_WireExplorer for
//! order-independent traversal.
//! @param[in]     theGraph  source graph
//! @param[in,out] theIssues collection to append diagnostic issues
void checkWireConnectivity(const BRepGraph&                                     theGraph,
                           NCollection_DynamicArray<BRepGraph_Validate::Issue>& theIssues)
{
  using Issue    = BRepGraph_Validate::Issue;
  using Severity = BRepGraph_Validate::Severity;

  for (BRepGraph_Iterator<BRepGraphInc::WireDef> aWireIt(theGraph); aWireIt.More(); aWireIt.Next())
  {
    const BRepGraph_WireId aWireId = aWireIt.CurrentId();

    BRepGraph_WireExplorer anExp(theGraph, aWireId);
    if (anExp.NbEdges() < 2)
    {
      continue;
    }

    // Validate all edge indices first.
    bool aAllValid = true;
    for (; anExp.More(); anExp.Next())
    {
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        theGraph.Topo().CoEdges().Definition(anExp.CurrentCoEdgeId());
      const BRepGraph_NodeId anEdgeId = aCoEdge.EdgeDefId;
      if (!anEdgeId.IsValid() || !isValidNodeId(theGraph, anEdgeId))
      {
        aAllValid = false;
        break;
      }
    }
    if (!aAllValid)
    {
      continue;
    }

    // Check that all consecutive pairs in the ordered sequence are connected.
    anExp.Reset();
    BRepGraph_CoEdgeId aPrevId = anExp.CurrentCoEdgeId();
    anExp.Next();
    int anIdx = 0;
    for (; anExp.More(); anExp.Next(), ++anIdx)
    {
      const BRepGraph_CoEdgeId aCurrId = anExp.CurrentCoEdgeId();

      const BRepGraphInc::CoEdgeDef& aPrevCoEdge = theGraph.Topo().CoEdges().Definition(aPrevId);
      const BRepGraphInc::CoEdgeDef& aCurrCoEdge = theGraph.Topo().CoEdges().Definition(aCurrId);

      const BRepGraphInc::EdgeDef& aPrevEdge =
        theGraph.Topo().Edges().Definition(aPrevCoEdge.EdgeDefId);
      const BRepGraphInc::EdgeDef& aCurrEdge =
        theGraph.Topo().Edges().Definition(aCurrCoEdge.EdgeDefId);

      // Resolve oriented end vertex of previous edge.
      const BRepGraph_VertexRefId aPrevEndRefId = (aPrevCoEdge.Orientation == TopAbs_FORWARD)
                                                    ? aPrevEdge.EndVertexRefId
                                                    : aPrevEdge.StartVertexRefId;
      const BRepGraph_NodeId      aPrevEnd =
        aPrevEndRefId.IsValid()
               ? BRepGraph_VertexId(theGraph.Refs().Vertices().Entry(aPrevEndRefId).VertexDefId)
               : BRepGraph_NodeId();

      // Resolve oriented start vertex of current edge.
      const BRepGraph_VertexRefId aCurrStartRefId = (aCurrCoEdge.Orientation == TopAbs_FORWARD)
                                                      ? aCurrEdge.StartVertexRefId
                                                      : aCurrEdge.EndVertexRefId;
      const BRepGraph_NodeId      aCurrStart =
        aCurrStartRefId.IsValid()
               ? BRepGraph_VertexId(theGraph.Refs().Vertices().Entry(aCurrStartRefId).VertexDefId)
               : BRepGraph_NodeId();

      if (aPrevEnd.IsValid() && aCurrStart.IsValid() && aPrevEnd != aCurrStart)
      {
        TCollection_AsciiString aDesc("Wire edges not connected: edge[");
        aDesc += TCollection_AsciiString(anIdx);
        aDesc += "] end != edge[";
        aDesc += TCollection_AsciiString(anIdx + 1);
        aDesc += "] start";
        theIssues.Append(Issue{Severity::Error, aWireId, aDesc});
      }
      aPrevId = aCurrId;
    }
  }
}

//! Validate that NbActive*() counts match the actual number of non-removed
//! entities in each per-kind vector.
//! @param[in]     theGraph  source graph
//! @param[in,out] theIssues collection to append diagnostic issues
void checkActiveCounts(const BRepGraph&                                     theGraph,
                       NCollection_DynamicArray<BRepGraph_Validate::Issue>& theIssues)
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
    NCollection_DynamicArray<BRepGraph::EditorView::BoundaryIssue> aBoundaryIssues;
    if (!theGraph.Editor().ValidateMutationBoundary(&aBoundaryIssues))
    {
      appendMutationBoundaryIssues(aBoundaryIssues, aResult.Issues);
    }
    return aResult;
  }

  NCollection_DynamicArray<BRepGraph::EditorView::BoundaryIssue> aBoundaryIssues;
  if (!theGraph.Editor().ValidateMutationBoundary(&aBoundaryIssues))
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
    NCollection_Map<BRepGraph_ProductId>          aVisited;
    NCollection_DynamicArray<BRepGraph_ProductId> aQueue;
    size_t                                        aHead = 0;

    // Seed with direct children.
    for (BRepGraph_RefsOccurrenceOfProduct anOccIt(theGraph, aProdId); anOccIt.More();
         anOccIt.Next())
    {
      const BRepGraphInc::OccurrenceRef& anOccRef =
        theGraph.Refs().Occurrences().Entry(anOccIt.CurrentId());
      const BRepGraphInc::OccurrenceDef& anOcc =
        aDefs.Occurrences().Definition(anOccRef.OccurrenceDefId);
      if (anOcc.IsRemoved || anOcc.ChildDefId.NodeKind != BRepGraph_NodeId::Kind::Product)
      {
        continue;
      }
      const BRepGraph_ProductId aChildProdId = BRepGraph_ProductId::FromNodeId(anOcc.ChildDefId);
      if (!aChildProdId.IsValid(aDefs.Products().Nb()))
      {
        continue;
      }
      if (aChildProdId == aProdId)
      {
        aResult.Issues.Append(
          Issue{Severity::Error,
                aProdId,
                "Assembly cycle: Product directly references itself via occurrence"});
        break;
      }
      if (aVisited.Add(aChildProdId))
      {
        aQueue.Append(aChildProdId);
      }
    }

    bool aCycleFound = false;
    while (aHead < aQueue.Size() && !aCycleFound)
    {
      const BRepGraph_ProductId aChildProdId = aQueue.Value(aHead);
      ++aHead;
      const BRepGraphInc::ProductDef& aChildProd = aDefs.Products().Definition(aChildProdId);
      if (aChildProd.IsRemoved)
      {
        continue;
      }
      for (BRepGraph_RefsOccurrenceOfProduct aRefIt(theGraph, aChildProdId); aRefIt.More();
           aRefIt.Next())
      {
        const BRepGraphInc::OccurrenceRef& aRef =
          theGraph.Refs().Occurrences().Entry(aRefIt.CurrentId());
        const BRepGraphInc::OccurrenceDef& aOcc =
          aDefs.Occurrences().Definition(aRef.OccurrenceDefId);
        if (aOcc.IsRemoved || aOcc.ChildDefId.NodeKind != BRepGraph_NodeId::Kind::Product)
        {
          continue;
        }
        const BRepGraph_ProductId aDescProdId = BRepGraph_ProductId::FromNodeId(aOcc.ChildDefId);
        if (!aDescProdId.IsValid(aDefs.Products().Nb()))
        {
          continue;
        }
        if (aDescProdId == aProdId)
        {
          aResult.Issues.Append(
            Issue{Severity::Error,
                  aProdId,
                  "Assembly cycle: Product reaches itself through occurrence chain"});
          aCycleFound = true;
          break;
        }
        if (aVisited.Add(aDescProdId))
        {
          aQueue.Append(aDescProdId);
        }
      }
    }
  }

  // Compound structural cycle detection: a compound must not reach itself
  // directly or transitively through ChildRefs whose ChildDefId.NodeKind is
  // Compound. Distinct from Product/Occurrence cycles (those are assembly-level).
  for (BRepGraph_Iterator<BRepGraphInc::CompoundDef> aCompIt(theGraph); aCompIt.More();
       aCompIt.Next())
  {
    const BRepGraph_CompoundId                     aRootCompoundId = aCompIt.CurrentId();
    NCollection_Map<BRepGraph_CompoundId>          aVisited;
    NCollection_DynamicArray<BRepGraph_CompoundId> aQueue;
    size_t                                         aHead = 0;

    for (BRepGraph_RefsChildOfCompound anIt(theGraph, aRootCompoundId); anIt.More(); anIt.Next())
    {
      const BRepGraphInc::ChildRef& aCR = theGraph.Refs().Children().Entry(anIt.CurrentId());
      if (aCR.IsRemoved || aCR.ChildDefId.NodeKind != BRepGraph_NodeId::Kind::Compound
          || !aCR.ChildDefId.IsValid())
      {
        continue;
      }
      const BRepGraph_CompoundId aChildCompoundId =
        BRepGraph_CompoundId::FromNodeId(aCR.ChildDefId);
      if (!aChildCompoundId.IsValidIn(aDefs.Compounds()))
      {
        continue;
      }
      if (aChildCompoundId == aRootCompoundId)
      {
        aResult.Issues.Append(
          Issue{Severity::Error,
                aRootCompoundId,
                "Compound cycle: Compound directly references itself via ChildRef"});
        break;
      }
      if (aVisited.Add(aChildCompoundId))
      {
        aQueue.Append(aChildCompoundId);
      }
    }

    bool aCycleFound = false;
    while (aHead < aQueue.Size() && !aCycleFound)
    {
      const BRepGraph_CompoundId aChildCompoundId = aQueue.Value(aHead);
      ++aHead;
      if (aDefs.Compounds().Definition(aChildCompoundId).IsRemoved)
      {
        continue;
      }
      for (BRepGraph_RefsChildOfCompound aRefIt(theGraph, aChildCompoundId); aRefIt.More();
           aRefIt.Next())
      {
        const BRepGraphInc::ChildRef& aCR = theGraph.Refs().Children().Entry(aRefIt.CurrentId());
        if (aCR.IsRemoved || aCR.ChildDefId.NodeKind != BRepGraph_NodeId::Kind::Compound
            || !aCR.ChildDefId.IsValid())
        {
          continue;
        }
        const BRepGraph_CompoundId aDescCompoundId =
          BRepGraph_CompoundId::FromNodeId(aCR.ChildDefId);
        if (!aDescCompoundId.IsValidIn(aDefs.Compounds()))
        {
          continue;
        }
        if (aDescCompoundId == aRootCompoundId)
        {
          aResult.Issues.Append(
            Issue{Severity::Error,
                  aRootCompoundId,
                  "Compound cycle: Compound reaches itself through ChildRef chain"});
          aCycleFound = true;
          break;
        }
        if (aVisited.Add(aDescCompoundId))
        {
          aQueue.Append(aDescCompoundId);
        }
      }
    }
  }

  // Orphan ref detection: ChildRef/SolidRef entries whose ParentId points
  // to a removed / out-of-range container. These survive node removal and
  // can cause reverse-index queries to return wrong answers.
  {
    const BRepGraph::RefsView& aRefs = theGraph.Refs();
    for (BRepGraph_FullChildRefIterator aChildRefIt(theGraph); aChildRefIt.More();
         aChildRefIt.Next())
    {
      const BRepGraph_ChildRefId    aChildRefId = aChildRefIt.CurrentId();
      const BRepGraphInc::ChildRef& aCR         = aRefs.Children().Entry(aChildRefId);
      if (aCR.IsRemoved)
      {
        continue;
      }
      const BRepGraph_NodeId aParentId = aCR.ParentId;
      if (!aParentId.IsValid() || aParentId.NodeKind != BRepGraph_NodeId::Kind::Compound
          || !BRepGraph_CompoundId(aParentId).IsValidIn(aDefs.Compounds())
          || aDefs.Compounds().Definition(BRepGraph_CompoundId(aParentId)).IsRemoved)
      {
        aResult.Issues.Append(
          Issue{Severity::Error,
                aParentId,
                "Orphan ChildRef: ParentId points to removed or out-of-range Compound"});
      }
    }

    for (BRepGraph_FullSolidRefIterator aSolidRefIt(theGraph); aSolidRefIt.More();
         aSolidRefIt.Next())
    {
      const BRepGraph_SolidRefId    aSolidRefId = aSolidRefIt.CurrentId();
      const BRepGraphInc::SolidRef& aSR         = aRefs.Solids().Entry(aSolidRefId);
      if (aSR.IsRemoved)
      {
        continue;
      }
      if (!aSR.ParentId.IsValid())
      {
        aResult.Issues.Append(
          Issue{Severity::Error, aSR.ParentId, "Orphan SolidRef: ParentId invalid"});
        continue;
      }
      if (aSR.ParentId.NodeKind == BRepGraph_NodeId::Kind::CompSolid)
      {
        if (!BRepGraph_CompSolidId(aSR.ParentId).IsValidIn(aDefs.CompSolids())
            || aDefs.CompSolids().Definition(BRepGraph_CompSolidId(aSR.ParentId)).IsRemoved)
        {
          aResult.Issues.Append(
            Issue{Severity::Error,
                  aSR.ParentId,
                  "Orphan SolidRef: ParentId points to removed or out-of-range CompSolid"});
        }
      }
    }

    // Shell refs: parent must be a live Solid.
    for (BRepGraph_FullShellRefIterator aShellRefIt(theGraph); aShellRefIt.More();
         aShellRefIt.Next())
    {
      const BRepGraph_ShellRefId    aShellRefId = aShellRefIt.CurrentId();
      const BRepGraphInc::ShellRef& aRef        = aRefs.Shells().Entry(aShellRefId);
      if (aRef.IsRemoved)
      {
        continue;
      }
      const BRepGraph_NodeId aP = aRef.ParentId;
      if (!aP.IsValid() || aP.NodeKind != BRepGraph_NodeId::Kind::Solid
          || !BRepGraph_SolidId(aP).IsValidIn(aDefs.Solids())
          || aDefs.Solids().Definition(BRepGraph_SolidId(aP)).IsRemoved)
      {
        aResult.Issues.Append(
          Issue{Severity::Error, aP, "Orphan ShellRef: ParentId is not a live Solid"});
      }
    }

    // Face refs: parent must be a live Shell.
    for (BRepGraph_FullFaceRefIterator aFaceRefIt(theGraph); aFaceRefIt.More(); aFaceRefIt.Next())
    {
      const BRepGraph_FaceRefId    aFaceRefId = aFaceRefIt.CurrentId();
      const BRepGraphInc::FaceRef& aRef       = aRefs.Faces().Entry(aFaceRefId);
      if (aRef.IsRemoved)
      {
        continue;
      }
      const BRepGraph_NodeId aP = aRef.ParentId;
      if (!aP.IsValid() || aP.NodeKind != BRepGraph_NodeId::Kind::Shell
          || !BRepGraph_ShellId(aP).IsValidIn(aDefs.Shells())
          || aDefs.Shells().Definition(BRepGraph_ShellId(aP)).IsRemoved)
      {
        aResult.Issues.Append(
          Issue{Severity::Error, aP, "Orphan FaceRef: ParentId is not a live Shell"});
      }
    }

    // Wire refs: parent must be a live Face.
    for (BRepGraph_FullWireRefIterator aWireRefIt(theGraph); aWireRefIt.More(); aWireRefIt.Next())
    {
      const BRepGraph_WireRefId    aWireRefId = aWireRefIt.CurrentId();
      const BRepGraphInc::WireRef& aRef       = aRefs.Wires().Entry(aWireRefId);
      if (aRef.IsRemoved)
      {
        continue;
      }
      const BRepGraph_NodeId aP = aRef.ParentId;
      if (!aP.IsValid() || aP.NodeKind != BRepGraph_NodeId::Kind::Face
          || !BRepGraph_FaceId(aP).IsValidIn(aDefs.Faces())
          || aDefs.Faces().Definition(BRepGraph_FaceId(aP)).IsRemoved)
      {
        aResult.Issues.Append(
          Issue{Severity::Error, aP, "Orphan WireRef: ParentId is not a live Face"});
      }
    }

    // CoEdge refs: parent must be a live Wire.
    for (BRepGraph_FullCoEdgeRefIterator aCoEdgeRefIt(theGraph); aCoEdgeRefIt.More();
         aCoEdgeRefIt.Next())
    {
      const BRepGraph_CoEdgeRefId    aCoEdgeRefId = aCoEdgeRefIt.CurrentId();
      const BRepGraphInc::CoEdgeRef& aRef         = aRefs.CoEdges().Entry(aCoEdgeRefId);
      if (aRef.IsRemoved)
      {
        continue;
      }
      const BRepGraph_NodeId aP = aRef.ParentId;
      if (!aP.IsValid() || aP.NodeKind != BRepGraph_NodeId::Kind::Wire
          || !BRepGraph_WireId(aP).IsValidIn(aDefs.Wires())
          || aDefs.Wires().Definition(BRepGraph_WireId(aP)).IsRemoved)
      {
        aResult.Issues.Append(
          Issue{Severity::Error, aP, "Orphan CoEdgeRef: ParentId is not a live Wire"});
      }
    }

    // Vertex refs: parent must be a live Edge.
    for (BRepGraph_FullVertexRefIterator aVertexRefIt(theGraph); aVertexRefIt.More();
         aVertexRefIt.Next())
    {
      const BRepGraph_VertexRefId    aVertexRefId = aVertexRefIt.CurrentId();
      const BRepGraphInc::VertexRef& aRef         = aRefs.Vertices().Entry(aVertexRefId);
      if (aRef.IsRemoved)
      {
        continue;
      }
      const BRepGraph_NodeId aP = aRef.ParentId;
      if (!aP.IsValid() || aP.NodeKind != BRepGraph_NodeId::Kind::Edge
          || !BRepGraph_EdgeId(aP).IsValidIn(aDefs.Edges())
          || aDefs.Edges().Definition(BRepGraph_EdgeId(aP)).IsRemoved)
      {
        aResult.Issues.Append(
          Issue{Severity::Error, aP, "Orphan VertexRef: ParentId is not a live Edge"});
      }
    }

    // Occurrence refs: parent must be a live Product.
    for (BRepGraph_FullOccurrenceRefIterator anOccurrenceRefIt(theGraph); anOccurrenceRefIt.More();
         anOccurrenceRefIt.Next())
    {
      const BRepGraph_OccurrenceRefId    aOccurrenceRefId = anOccurrenceRefIt.CurrentId();
      const BRepGraphInc::OccurrenceRef& aRef = aRefs.Occurrences().Entry(aOccurrenceRefId);
      if (aRef.IsRemoved)
      {
        continue;
      }
      const BRepGraph_NodeId aP = aRef.ParentId;
      if (!aP.IsValid() || aP.NodeKind != BRepGraph_NodeId::Kind::Product
          || !BRepGraph_ProductId(aP).IsValidIn(aDefs.Products())
          || aDefs.Products().Definition(BRepGraph_ProductId(aP)).IsRemoved)
      {
        aResult.Issues.Append(
          Issue{Severity::Error, aP, "Orphan OccurrenceRef: ParentId is not a live Product"});
      }
    }
  }

  // Occurrence-to-Product liveness: every active OccurrenceDef whose ChildDefId
  // points to a Product must target a live Product. This catches the case where
  // a Product is soft-removed but dependent Occurrences survived.
  for (BRepGraph_Iterator<BRepGraphInc::OccurrenceDef> anOccIt(theGraph); anOccIt.More();
       anOccIt.Next())
  {
    const BRepGraphInc::OccurrenceDef& anOcc = anOccIt.Current();
    if (anOcc.ChildDefId.NodeKind != BRepGraph_NodeId::Kind::Product)
    {
      continue;
    }
    const BRepGraph_ProductId aChildProductId(anOcc.ChildDefId);
    if (!aChildProductId.IsValidIn(aDefs.Products())
        || aDefs.Products().Definition(aChildProductId).IsRemoved)
    {
      aResult.Issues.Append(
        Issue{Severity::Error,
              anOccIt.CurrentId(),
              "Active OccurrenceDef.ChildDefId points to a removed or out-of-range Product"});
    }
  }

  // Cross-container ownership: a Solid must not be simultaneously referenced by
  // an active Compound ChildRef AND an active CompSolid SolidRef. OCCT models
  // a solid as belonging to either a Compound or a CompSolid, not both.
  {
    const BRepGraph::RefsView&         aRefs = theGraph.Refs();
    NCollection_Map<BRepGraph_SolidId> aCompoundOwnedSolids;
    for (BRepGraph_FullChildRefIterator aChildRefIt(theGraph); aChildRefIt.More();
         aChildRefIt.Next())
    {
      const BRepGraph_ChildRefId    aChildRefId = aChildRefIt.CurrentId();
      const BRepGraphInc::ChildRef& aCR         = aRefs.Children().Entry(aChildRefId);
      if (aCR.IsRemoved)
      {
        continue;
      }
      if (aCR.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Solid && aCR.ChildDefId.IsValid())
      {
        aCompoundOwnedSolids.Add(BRepGraph_SolidId::FromNodeId(aCR.ChildDefId));
      }
    }
    for (BRepGraph_FullSolidRefIterator aSolidRefIt(theGraph); aSolidRefIt.More();
         aSolidRefIt.Next())
    {
      const BRepGraph_SolidRefId    aSolidRefId = aSolidRefIt.CurrentId();
      const BRepGraphInc::SolidRef& aSR         = aRefs.Solids().Entry(aSolidRefId);
      if (aSR.IsRemoved)
      {
        continue;
      }
      if (aSR.SolidDefId.IsValid() && aCompoundOwnedSolids.Contains(aSR.SolidDefId))
      {
        aResult.Issues.Append(
          Issue{Severity::Error,
                aSR.SolidDefId,
                "Cross-container ownership: Solid is referenced by both a Compound (ChildRef) "
                "and a CompSolid (SolidRef)"});
      }
    }
  }

  return aResult;
}
