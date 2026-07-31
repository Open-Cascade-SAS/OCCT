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
#include <BRepGraph_ReverseIterator.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <BRepGraph_CacheMesh.hxx>
#include <BRepGraph_MeshView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepGraph_LayerTopoSupplement.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_FlatMap.hxx>
#include <NCollection_LinearVector.hxx>
#include <NCollection_Map.hxx>
#include <Standard_Failure.hxx>

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
  return aDef != nullptr && theId.IsRemoved(theGraph);
}

//! Convert mutator boundary issues to validator issues.
//! @param[in]     theBoundaryIssues boundary issues reported by mutator
//! @param[in,out] theIssues         destination validator issue vector
void appendMutationBoundaryIssues(
  const NCollection_LinearVector<BRepGraph::EditorView::BoundaryIssue>& theBoundaryIssues,
  NCollection_LinearVector<BRepGraph_Validate::Issue>&                  theIssues)
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
                               NCollection_LinearVector<BRepGraph_Validate::Issue>& theIssues)
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
        theGraph.Refs().Vertices().Entry(anEdge.StartVertexRefId).ChildVertexId;
      if (aStartVtxId.IsValid() && !isValidNodeId(theGraph, aStartVtxId))
      {
        theIssues.Append(Issue{Severity::Error,
                               anEdgeIt.CurrentId(),
                               "EdgeDef.StartVertexRefId resolves to out-of-bounds ChildVertexId"});
      }
    }
    if (anEdge.EndVertexRefId.IsValid())
    {
      const BRepGraph_NodeId anEndVtxId =
        theGraph.Refs().Vertices().Entry(anEdge.EndVertexRefId).ChildVertexId;
      if (anEndVtxId.IsValid() && !isValidNodeId(theGraph, anEndVtxId))
      {
        theIssues.Append(Issue{Severity::Error,
                               anEdgeIt.CurrentId(),
                               "EdgeDef.EndVertexRefId resolves to out-of-bounds ChildVertexId"});
      }
    }
  }

  // Check CoEdgeDef references.
  for (BRepGraph_Iterator<BRepGraphInc::CoEdgeDef> aCoEdgeIt(theGraph); aCoEdgeIt.More();
       aCoEdgeIt.Next())
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge   = aCoEdgeIt.Current();
    const BRepGraph_CoEdgeId       aCoEdgeId = aCoEdgeIt.CurrentId();

    if (aCoEdge.FaceId.IsValid() && !isValidNodeId(theGraph, aCoEdge.FaceId))
    {
      theIssues.Append(Issue{Severity::Error, aCoEdgeId, "CoEdgeDef.FaceId out of bounds"});
    }
    const BRepGraph_NodeId anEdgeId = aCoEdge.ChildEdgeId;
    if (anEdgeId.IsValid() && !isValidNodeId(theGraph, anEdgeId))
    {
      theIssues.Append(Issue{Severity::Error, aCoEdgeId, "CoEdgeDef.ChildEdgeId out of bounds"});
    }
    // Seam consistency: for every (ChildEdgeId, FaceId) pair there are at most
    // 2 CoEdges; if 2, they must have opposite Orientation.
    if (aCoEdge.ChildEdgeId.IsValid() && aCoEdge.FaceId.IsValid())
    {
      uint32_t aSameFaceCount         = 0;
      bool     aHasOppositeOnSameFace = false;
      for (BRepGraph_CoEdgesOfEdge anIt(theGraph,
                                        theGraph.Topo().Edges().CoEdges(aCoEdge.ChildEdgeId));
           anIt.More();
           anIt.Next())
      {
        const BRepGraph_CoEdgeId       aOtherId = anIt.CurrentId();
        const BRepGraphInc::CoEdgeDef& aOther   = anIt.Definition();
        if (aOther.FaceId != aCoEdge.FaceId)
        {
          continue;
        }
        ++aSameFaceCount;
        if (aOtherId != aCoEdgeId && aOther.Orientation != aCoEdge.Orientation)
        {
          aHasOppositeOnSameFace = true;
        }
      }
      if (aSameFaceCount > 2)
      {
        theIssues.Append(Issue{Severity::Error,
                               aCoEdgeId,
                               "More than 2 CoEdges share the same (ChildEdgeId, FaceId)"});
      }
      else if (aSameFaceCount == 2 && !aHasOppositeOnSameFace)
      {
        theIssues.Append(
          Issue{Severity::Error, aCoEdgeId, "Seam pair CoEdges have the same Orientation"});
      }
    }
    // Rep index bounds.
    if (aCoEdge.Curve2DRepId.IsValid()
        && !aCoEdge.Curve2DRepId.IsValid(theGraph.Topo().Geometry().NbCoEdgeCurves2D()))
    {
      theIssues.Append(Issue{Severity::Error, aCoEdgeId, "CoEdgeDef.Curve2DRepId out of bounds"});
    }
  }

  // Surface handles are stored directly on FaceDef; no cross-reference to validate.

  // Check WireDef coedge sequence.
  for (BRepGraph_Iterator<BRepGraphInc::WireDef> aWireIt(theGraph); aWireIt.More(); aWireIt.Next())
  {
    const BRepGraph_WireId aWireId = aWireIt.CurrentId();

    for (BRepGraph_CoEdgesOfWire anIt(theGraph, aWireId); anIt.More(); anIt.Next())
    {
      const BRepGraph_NodeId aCoChildEdgeId = anIt.CurrentId();
      if (aCoChildEdgeId.IsValid() && !isValidNodeId(theGraph, aCoChildEdgeId))
      {
        theIssues.Append(
          Issue{Severity::Error, aWireId, "WireDef.CoEdgeUsage CoEdgeIdx out of bounds"});
      }
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        theGraph.Topo().CoEdges().Definition(anIt.CurrentId());
      const BRepGraph_NodeId anChildEdgeId = aCoEdge.ChildEdgeId;
      if (anChildEdgeId.IsValid() && !isValidNodeId(theGraph, anChildEdgeId))
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
      const BRepGraph_NodeId        aChildId = aCR.ChildNodeId;
      if (aChildId.IsValid() && !isValidNodeId(theGraph, aChildId))
      {
        theIssues.Append(
          Issue{Severity::Error, aCompoundId, "CompoundDef.ChildNodeId out of bounds"});
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
      const BRepGraph_NodeId        aSolidId = aSR.ChildSolidId;
      if (aSolidId.IsValid() && !isValidNodeId(theGraph, aSolidId))
      {
        theIssues.Append(
          Issue{Severity::Error, aCompSolidId, "CompSolidDef.ChildSolidId out of bounds"});
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
      const BRepGraphInc::FaceRef& aFR     = theGraph.Refs().Faces().Entry(anIt.CurrentId());
      const BRepGraph_NodeId       aFaceId = aFR.ChildFaceId;
      if (aFaceId.IsValid() && !isValidNodeId(theGraph, aFaceId))
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
      const BRepGraphInc::ShellRef& aSR = theGraph.Refs().Shells().Entry(anIt.CurrentId());
      const BRepGraph_NodeId        aChildShellId = aSR.ChildShellId;
      if (aChildShellId.IsValid() && !isValidNodeId(theGraph, aChildShellId))
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
        theGraph.Refs().Occurrences().Entry(anOccIt.CurrentId()).ChildOccurrenceId;
      if (anOccId.IsValid() && !isValidNodeId(theGraph, anOccId))
      {
        theIssues.Append(
          Issue{Severity::Error, aProdId, "ProductDef.OccurrenceRefId out of bounds"});
      }
    }
  }

  // Check OccurrenceDef references.
  for (BRepGraph_Iterator<BRepGraphInc::OccurrenceDef> anOccIt(theGraph); anOccIt.More();
       anOccIt.Next())
  {
    const BRepGraphInc::OccurrenceDef& anOcc    = anOccIt.Current();
    const BRepGraph_OccurrenceId       anOccId  = anOccIt.CurrentId();
    const BRepGraph_NodeId             aChildId = anOcc.ChildNodeId;
    const BRepGraph_NodeId::Kind       aKind    = aChildId.NodeKind;

    if (!aChildId.IsValid())
    {
      theIssues.Append(Issue{Severity::Error, anOccId, "OccurrenceDef.ChildNodeId invalid"});
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
                ? "OccurrenceDef.ChildNodeId cannot reference an Occurrence"
                : "OccurrenceDef.ChildNodeId kind is not Product or a topology kind"});
      continue;
    }

    if (!isValidNodeId(theGraph, aChildId) || isEntityRemoved(theGraph, aChildId))
    {
      theIssues.Append(Issue{Severity::Error, anOccId, "OccurrenceDef.ChildNodeId invalid"});
    }
  }
}

//! Verify that every forward incidence ref has a matching relation entry
//! (edge->wires, edge->faces, vertex->edges, wire->faces, face->shells, shell->solids).
//! @param[in]     theGraph  source graph
//! @param[in,out] theIssues collection to append diagnostic issues
void checkRelationConsistency(const BRepGraph&                                     theGraph,
                              NCollection_LinearVector<BRepGraph_Validate::Issue>& theIssues)
{
  using Issue    = BRepGraph_Validate::Issue;
  using Severity = BRepGraph_Validate::Severity;

  // Build expected edge->wires mapping from wire-owned coedge scans.
  NCollection_DataMap<BRepGraph_EdgeId, NCollection_FlatMap<BRepGraph_WireId>> anExpected;
  for (BRepGraph_Iterator<BRepGraphInc::WireDef> aWireIt(theGraph); aWireIt.More(); aWireIt.Next())
  {
    const BRepGraph_WireId aWireId = aWireIt.CurrentId();

    for (BRepGraph_CoEdgesOfWire anIt(theGraph, aWireId); anIt.More(); anIt.Next())
    {
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        theGraph.Topo().CoEdges().Definition(anIt.CurrentId());
      if (!aCoEdge.ChildEdgeId.IsValid())
      {
        continue;
      }

      if (!anExpected.IsBound(aCoEdge.ChildEdgeId))
      {
        anExpected.Bind(aCoEdge.ChildEdgeId, NCollection_FlatMap<BRepGraph_WireId>());
      }
      anExpected.ChangeFind(aCoEdge.ChildEdgeId).Add(aWireId);
    }
  }

  // Check that Topo WiresOfEdge matches expected.
  for (BRepGraph_Iterator<BRepGraphInc::EdgeDef> anEdgeIt(theGraph); anEdgeIt.More();
       anEdgeIt.Next())
  {
    const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();

    const NCollection_FlatMap<BRepGraph_WireId>* anExpectedWires = anExpected.Seek(anEdgeId);

    const uint32_t anExpectedCount =
      (anExpectedWires != nullptr) ? static_cast<uint32_t>(anExpectedWires->Size()) : 0u;

    // Build a set from actual wires for comparison.
    NCollection_FlatMap<BRepGraph_WireId> anActualSet;
    for (BRepGraph_WiresOfEdge aWireIt = theGraph.Topo().Edges().WiresOf(anEdgeId); aWireIt.More();
         aWireIt.Next())
    {
      anActualSet.Add(aWireIt.CurrentId());
    }

    if (static_cast<uint32_t>(anActualSet.Size()) != anExpectedCount)
    {
      TCollection_AsciiString aDesc("Relation Edges.Wires size mismatch: expected=");
      aDesc += TCollection_AsciiString(static_cast<int>(anExpectedCount));
      aDesc += ", actual=";
      aDesc += TCollection_AsciiString(static_cast<int>(anActualSet.Size()));
      aDesc += ", edge=";
      aDesc += TCollection_AsciiString(static_cast<int>(anEdgeId.Index));
      aDesc += ", actualWires=[";
      bool isFirstActual = true;
      for (BRepGraph_WiresOfEdge aWireIt = theGraph.Topo().Edges().WiresOf(anEdgeId);
           aWireIt.More();
           aWireIt.Next())
      {
        if (!isFirstActual)
        {
          aDesc += ",";
        }
        isFirstActual                  = false;
        const BRepGraph_WireId aWireId = aWireIt.CurrentId();
        aDesc += TCollection_AsciiString(static_cast<int>(aWireId.Index));
        if (!aWireId.IsValid(theGraph.Topo().Wires().Nb()) || aWireId.IsRemoved(theGraph))
        {
          aDesc += " removed";
        }
      }
      aDesc += "]";
      theIssues.Append(Issue{Severity::Error, anEdgeIt.CurrentId(), aDesc});
      continue;
    }

    if (anExpectedWires != nullptr)
    {
      for (NCollection_FlatMap<BRepGraph_WireId>::Iterator aWireIt(*anExpectedWires);
           aWireIt.More();
           aWireIt.Next())
      {
        const BRepGraph_WireId aWireId = aWireIt.Value();
        if (!anActualSet.Contains(aWireId))
        {
          theIssues.Append(Issue{Severity::Error,
                                 anEdgeIt.CurrentId(),
                                 "Relation Edges.Wires missing wire entry"});
          break;
        }
      }
    }
  }
}

//! Validate consistency of cached edge-face counts in relation storage.
//! @param[in]     theGraph  source graph
//! @param[in,out] theIssues collection to append diagnostic issues
void checkRelationFaceCountCache(const BRepGraph&                                     theGraph,
                                 NCollection_LinearVector<BRepGraph_Validate::Issue>& theIssues)
{
  using Issue                      = BRepGraph_Validate::Issue;
  using Severity                   = BRepGraph_Validate::Severity;
  const BRepGraph::TopoView& aDefs = theGraph.Topo();

  for (BRepGraph_Iterator<BRepGraphInc::EdgeDef> anEdgeIt(theGraph); anEdgeIt.More();
       anEdgeIt.Next())
  {
    const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();

    NCollection_FlatMap<BRepGraph_FaceId>               aUniqueFaces;
    const NCollection_LinearVector<BRepGraph_CoEdgeId>& aCoEdges = aDefs.Edges().CoEdges(anEdgeId);
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdges)
    {
      const BRepGraphInc::CoEdgeDef& aCoEdge = aDefs.CoEdges().Definition(aCoEdgeId);
      if (BRepGraph_NodeId(aCoEdgeId).IsRemoved(theGraph) || !aCoEdge.FaceId.IsValid())
      {
        continue;
      }
      aUniqueFaces.Add(aCoEdge.FaceId);
    }

    const uint32_t aCachedCount  = aDefs.Edges().NbFaces(anEdgeId);
    const uint32_t anActualCount = static_cast<uint32_t>(aUniqueFaces.Size());
    if (aCachedCount != anActualCount)
    {
      TCollection_AsciiString aDesc("Relation face-count cache mismatch: cached=");
      aDesc += TCollection_AsciiString(static_cast<int>(aCachedCount));
      aDesc += " actual=";
      aDesc += TCollection_AsciiString(static_cast<int>(anActualCount));
      aDesc += " edge=";
      aDesc += TCollection_AsciiString(static_cast<int>(anEdgeId.Index));
      aDesc += " faces=[";
      bool isFirstFace = true;
      for (BRepGraph_FacesOfEdge aFaceIt = theGraph.Topo().Edges().FacesOf(anEdgeId);
           aFaceIt.More();
           aFaceIt.Next())
      {
        if (!isFirstFace)
        {
          aDesc += ",";
        }
        isFirstFace                    = false;
        const BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();
        aDesc += TCollection_AsciiString(static_cast<int>(aFaceId.Index));
      }
      aDesc += "]";
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
                                  NCollection_LinearVector<BRepGraph_Validate::Issue>& theIssues)
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
      const BRepGraph_NodeId       aWireId = aWR.ChildWireId;
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
      const BRepGraph_NodeId       aFaceId = aFR.ChildFaceId;
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
      const BRepGraph_NodeId        aShellId = aSR.ChildShellId;
      if (aShellId.IsValid() && isEntityRemoved(theGraph, aShellId))
      {
        theIssues.Append(Issue{Severity::Error, aSolidId, "SolidDef references removed ShellDef"});
      }
    }
  }
}

//! Validate that geometry use ids (SurfaceRepId, Curve3DRepId,
//! Curve2DRepId, TriangulationRepId, Polygon3DRepId) are in bounds and
//! reference non-null geometry handles.
//! @param[in]     theGraph  source graph
//! @param[in,out] theIssues collection to append diagnostic issues
void checkGeometryReferences(const BRepGraph&                                     theGraph,
                             NCollection_LinearVector<BRepGraph_Validate::Issue>& theIssues)
{
  using Issue    = BRepGraph_Validate::Issue;
  using Severity = BRepGraph_Validate::Severity;

  const auto hasActiveSurface = [&](const BRepGraph_FaceId theFaceId) -> bool {
    if (!isValidNodeId(theGraph, theFaceId) || isEntityRemoved(theGraph, theFaceId))
    {
      return false;
    }
    const BRepGraph_FaceSurfaceRepId aSurfaceRepId =
      theGraph.Topo().Faces().Definition(theFaceId).SurfaceRepId;
    return aSurfaceRepId.IsValid(theGraph.Topo().Geometry().NbFaceSurfaces())
           && !aSurfaceRepId.IsRemoved(theGraph);
  };

  // Check edge->curve references (handles stored directly on EdgeDef).
  for (BRepGraph_Iterator<BRepGraphInc::EdgeDef> anEdgeIt(theGraph); anEdgeIt.More();
       anEdgeIt.Next())
  {
    const BRepGraph_EdgeId       anEdgeId = anEdgeIt.CurrentId();
    const BRepGraphInc::EdgeDef& anEdge   = anEdgeIt.Current();

    bool                  needsCurve3D = true;
    BRepGraph_FacesOfEdge aFaceIt      = theGraph.Topo().Edges().FacesOf(anEdgeId);
    if (aFaceIt.More())
    {
      needsCurve3D = false;
      for (; aFaceIt.More(); aFaceIt.Next())
      {
        if (hasActiveSurface(aFaceIt.CurrentId()))
        {
          needsCurve3D = true;
          break;
        }
      }
    }

    if (needsCurve3D && !BRepGraph_Tool::Edge::Degenerated(theGraph, anEdgeId)
        && !BRepGraph_Tool::Edge::HasCurve(theGraph, anEdgeId))
    {
      if (anEdge.Curve3DRepId.IsValid() && anEdge.Curve3DRepId.IsRemoved(theGraph))
      {
        theIssues.Append(Issue{Severity::Error,
                               anEdgeIt.CurrentId(),
                               "Non-degenerate EdgeDef references a removed Curve3DRep"});
      }
      else
      {
        theIssues.Append(Issue{Severity::Error,
                               anEdgeIt.CurrentId(),
                               "Non-degenerate EdgeDef has no Curve3D representation"});
      }
    }
    if (anEdge.Curve3DRepId.IsValid()
        && !anEdge.Curve3DRepId.IsValid(theGraph.Topo().Geometry().NbEdgeCurves3D()))
    {
      theIssues.Append(
        Issue{Severity::Error, anEdgeIt.CurrentId(), "EdgeDef.Curve3DRepId out of bounds"});
    }
    if (anEdge.Polygon3DRepId.IsValid()
        && !anEdge.Polygon3DRepId.IsValid(theGraph.Mesh().Poly().NbEdgePolygons3D()))
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
        && !aFace.SurfaceRepId.IsValid(theGraph.Topo().Geometry().NbFaceSurfaces()))
    {
      theIssues.Append(
        Issue{Severity::Error, aFaceIt.CurrentId(), "FaceDef.SurfaceRepId out of bounds"});
    }
    if (aFace.TriangulationRepId.IsValid()
        && !aFace.TriangulationRepId.IsValid(theGraph.Mesh().Poly().NbFaceTriangulations()))
    {
      theIssues.Append(
        Issue{Severity::Error, aFaceIt.CurrentId(), "FaceDef.TriangulationRepId out of bounds"});
    }
    // Validate cached mesh entry bounds.
    const BRepGraph_CacheMesh::FaceMeshEntry* aCachedFace =
      theGraph.Mesh().Cache().Faces().Entry(aFaceIt.CurrentId());
    if (aCachedFace != nullptr)
    {
      if (aCachedFace->Triangulation.IsNull())
      {
        theIssues.Append(
          Issue{Severity::Error, aFaceIt.CurrentId(), "CacheMesh.FaceMesh.Triangulation is null"});
      }
    }
  }

  // A surfaced face without wire refs is allowed for intentionally unbounded
  // natural faces. Keep it diagnostic so callers can decide whether to accept it.
  for (BRepGraph_Iterator<BRepGraphInc::FaceDef> aFaceIt(theGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraphInc::FaceDef& aFace = aFaceIt.Current();
    if (aFace.SurfaceRepId.IsValid(theGraph.Topo().Geometry().NbFaceSurfaces())
        && !aFace.SurfaceRepId.IsRemoved(theGraph))
    {
      const bool hasWire = BRepGraph_RefsWireOfFace(theGraph, aFaceIt.CurrentId()).More();
      if (!hasWire)
      {
        theIssues.Append(
          Issue{Severity::Warning, aFaceIt.CurrentId(), "Surfaced face has no wire refs"});
      }
    }
  }

  // Check CoEdge PCurve data.
  for (BRepGraph_Iterator<BRepGraphInc::CoEdgeDef> aCoEdgeIt(theGraph); aCoEdgeIt.More();
       aCoEdgeIt.Next())
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge   = aCoEdgeIt.Current();
    const BRepGraph_CoEdgeId       aCoEdgeId = aCoEdgeIt.CurrentId();

    const bool needsPCurve = hasActiveSurface(aCoEdge.FaceId);
    if (needsPCurve && !BRepGraph_Tool::CoEdge::HasPCurve(theGraph, aCoEdgeId))
    {
      theIssues.Append(
        Issue{Severity::Error, aCoEdgeId, "CoEdgeDef has no Curve2D representation"});
    }
  }
}

} // namespace

//=================================================================================================

bool BRepGraph_Validate::Result::IsValid() const
{
  for (const Issue& anIssue : Issues)
  {
    if (anIssue.Sev == Severity::Error)
    {
      return false;
    }
  }
  return true;
}

//=================================================================================================

int BRepGraph_Validate::Result::NbIssues(const Severity theSev) const
{
  int aCount = 0;
  for (const Issue& anIssue : Issues)
  {
    if (anIssue.Sev == theSev)
    {
      ++aCount;
    }
  }
  return aCount;
}

//=================================================================================================

//! Validate owned representation-use parent, removal, representation, and uniqueness invariants.
//! @param[in]     theGraph  source graph
//! @param[in,out] theIssues collection to append diagnostic issues
void BRepGraph_Validate::CheckOwnedUseReferences(
  const BRepGraph&                                     theGraph,
  NCollection_LinearVector<BRepGraph_Validate::Issue>& theIssues)
{
  using Issue    = BRepGraph_Validate::Issue;
  using Severity = BRepGraph_Validate::Severity;

  const BRepGraphInc_Storage& aStorage = theGraph.incStorage();

  NCollection_LinearVector<uint32_t>         aSurfaceOwners(aStorage.NbFaceSurfaces(), 0);
  NCollection_LinearVector<BRepGraph_NodeId> aSurfaceOwner(aStorage.NbFaceSurfaces(),
                                                           BRepGraph_NodeId());
  NCollection_LinearVector<uint32_t> aTriangulationOwners(aStorage.NbFaceTriangulations(), 0);
  NCollection_LinearVector<BRepGraph_NodeId> aTriangulationOwner(aStorage.NbFaceTriangulations(),
                                                                 BRepGraph_NodeId());
  NCollection_LinearVector<uint32_t>         aCurve3DOwners(aStorage.NbEdgeCurves3D(), 0);
  NCollection_LinearVector<BRepGraph_NodeId> aCurve3DOwner(aStorage.NbEdgeCurves3D(),
                                                           BRepGraph_NodeId());
  NCollection_LinearVector<uint32_t>         aPolygon3DOwners(aStorage.NbEdgePolygons3D(), 0);
  NCollection_LinearVector<BRepGraph_NodeId> aPolygon3DOwner(aStorage.NbEdgePolygons3D(),
                                                             BRepGraph_NodeId());
  NCollection_LinearVector<uint32_t>         aCurve2DOwners(aStorage.NbCoEdgeCurves2D(), 0);
  NCollection_LinearVector<BRepGraph_NodeId> aCurve2DOwner(aStorage.NbCoEdgeCurves2D(),
                                                           BRepGraph_NodeId());
  NCollection_LinearVector<uint32_t>         aPolygon2DOwners(aStorage.NbCoEdgePolygons2D(), 0);
  NCollection_LinearVector<BRepGraph_NodeId> aPolygon2DOwner(aStorage.NbCoEdgePolygons2D(),
                                                             BRepGraph_NodeId());
  NCollection_LinearVector<uint32_t> aPolygonOnTriOwners(aStorage.NbCoEdgePolygonsOnTri(), 0);
  NCollection_LinearVector<BRepGraph_NodeId> aPolygonOnTriOwner(aStorage.NbCoEdgePolygonsOnTri(),
                                                                BRepGraph_NodeId());

  const auto aRememberOwner = [](NCollection_LinearVector<uint32_t>&         theCounts,
                                 NCollection_LinearVector<BRepGraph_NodeId>& theOwners,
                                 const uint32_t                              theIndex,
                                 const BRepGraph_NodeId                      theOwner) {
    ++theCounts[theIndex];
    if (!theOwners[theIndex].IsValid())
    {
      theOwners[theIndex] = theOwner;
    }
  };

  for (BRepGraph_Iterator<BRepGraphInc::EdgeDef> anEdgeIt(theGraph); anEdgeIt.More();
       anEdgeIt.Next())
  {
    const BRepGraph_EdgeId       anEdgeId = anEdgeIt.CurrentId();
    const BRepGraphInc::EdgeDef& anEdge   = anEdgeIt.Current();
    if (anEdge.Curve3DRepId.IsValid(aStorage.NbEdgeCurves3D()))
    {
      if (!aStorage.IsRemoved(anEdge.Curve3DRepId))
      {
        const BRepGraphInc::EdgeCurve3DRep& aUse = aStorage.EdgeCurve3DRep(anEdge.Curve3DRepId);
        aRememberOwner(aCurve3DOwners, aCurve3DOwner, anEdge.Curve3DRepId.Index, anEdgeId);
        if (aUse.ParentEdgeId != anEdgeId)
        {
          theIssues.Append(
            Issue{Severity::Error, anEdgeId, "EdgeCurve3DRep.ParentEdgeId mismatch"});
        }
        if (aUse.Curve.IsNull())
        {
          theIssues.Append(Issue{Severity::Error, anEdgeId, "EdgeCurve3DRep.Curve is null"});
        }
      }
    }
    if (anEdge.Polygon3DRepId.IsValid(aStorage.NbEdgePolygons3D()))
    {
      if (!aStorage.IsRemoved(anEdge.Polygon3DRepId))
      {
        const BRepGraphInc::EdgePolygon3DRep& aUse =
          aStorage.EdgePolygon3DRep(anEdge.Polygon3DRepId);
        aRememberOwner(aPolygon3DOwners, aPolygon3DOwner, anEdge.Polygon3DRepId.Index, anEdgeId);
        if (aUse.ParentEdgeId != anEdgeId)
        {
          theIssues.Append(
            Issue{Severity::Error, anEdgeId, "EdgePolygon3DRep.ParentEdgeId mismatch"});
        }
        if (aUse.Polygon.IsNull())
        {
          theIssues.Append(Issue{Severity::Error, anEdgeId, "EdgePolygon3DRep.Polygon is null"});
        }
      }
    }
  }

  for (BRepGraph_Iterator<BRepGraphInc::CoEdgeDef> aCoEdgeIt(theGraph); aCoEdgeIt.More();
       aCoEdgeIt.Next())
  {
    const BRepGraph_CoEdgeId       aCoEdgeId = aCoEdgeIt.CurrentId();
    const BRepGraphInc::CoEdgeDef& aCoEdge   = aCoEdgeIt.Current();
    if (aCoEdge.Curve2DRepId.IsValid(aStorage.NbCoEdgeCurves2D()))
    {
      if (!aStorage.IsRemoved(aCoEdge.Curve2DRepId))
      {
        const BRepGraphInc::CoEdgeCurve2DRep& aUse =
          aStorage.CoEdgeCurve2DRep(aCoEdge.Curve2DRepId);
        aRememberOwner(aCurve2DOwners, aCurve2DOwner, aCoEdge.Curve2DRepId.Index, aCoEdgeId);
        if (aUse.ParentCoEdgeId != aCoEdgeId)
        {
          theIssues.Append(
            Issue{Severity::Error, aCoEdgeId, "CoEdgeCurve2DRep.ParentCoEdgeId mismatch"});
        }
        if (aUse.Curve.IsNull())
        {
          theIssues.Append(Issue{Severity::Error, aCoEdgeId, "CoEdgeCurve2DRep.Curve is null"});
        }
      }
    }
    if (aCoEdge.Polygon2DRepId.IsValid(aStorage.NbCoEdgePolygons2D()))
    {
      if (!aStorage.IsRemoved(aCoEdge.Polygon2DRepId))
      {
        const BRepGraphInc::CoEdgePolygon2DRep& aUse =
          aStorage.CoEdgePolygon2DRep(aCoEdge.Polygon2DRepId);
        aRememberOwner(aPolygon2DOwners, aPolygon2DOwner, aCoEdge.Polygon2DRepId.Index, aCoEdgeId);
        if (aUse.ParentCoEdgeId != aCoEdgeId)
        {
          theIssues.Append(
            Issue{Severity::Error, aCoEdgeId, "CoEdgePolygon2DRep.ParentCoEdgeId mismatch"});
        }
        if (aUse.Polygon.IsNull())
        {
          theIssues.Append(Issue{Severity::Error, aCoEdgeId, "CoEdgePolygon2DRep.Polygon is null"});
        }
      }
    }
    if (aCoEdge.PolygonOnTriRepId.IsValid(aStorage.NbCoEdgePolygonsOnTri()))
    {
      if (!aStorage.IsRemoved(aCoEdge.PolygonOnTriRepId))
      {
        const BRepGraphInc::CoEdgePolygonOnTriRep& aUse =
          aStorage.CoEdgePolygonOnTriRep(aCoEdge.PolygonOnTriRepId);
        aRememberOwner(aPolygonOnTriOwners,
                       aPolygonOnTriOwner,
                       aCoEdge.PolygonOnTriRepId.Index,
                       aCoEdgeId);
        if (aUse.ParentCoEdgeId != aCoEdgeId)
        {
          theIssues.Append(
            Issue{Severity::Error, aCoEdgeId, "CoEdgePolygonOnTriRep.ParentCoEdgeId mismatch"});
        }
        if (aUse.Polygon.IsNull())
        {
          theIssues.Append(
            Issue{Severity::Error, aCoEdgeId, "CoEdgePolygonOnTriRep.Polygon is null"});
        }
      }
    }
  }

  for (BRepGraph_Iterator<BRepGraphInc::FaceDef> aFaceIt(theGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId       aFaceId = aFaceIt.CurrentId();
    const BRepGraphInc::FaceDef& aFace   = aFaceIt.Current();
    if (aFace.SurfaceRepId.IsValid(aStorage.NbFaceSurfaces()))
    {
      if (!aStorage.IsRemoved(aFace.SurfaceRepId))
      {
        const BRepGraphInc::FaceSurfaceRep& aUse = aStorage.FaceSurfaceRep(aFace.SurfaceRepId);
        aRememberOwner(aSurfaceOwners, aSurfaceOwner, aFace.SurfaceRepId.Index, aFaceId);
        if (aUse.ParentFaceId != aFaceId)
        {
          theIssues.Append(Issue{Severity::Error, aFaceId, "FaceSurfaceRep.ParentFaceId mismatch"});
        }
        if (aUse.Surface.IsNull())
        {
          theIssues.Append(Issue{Severity::Error, aFaceId, "FaceSurfaceRep.Surface is null"});
        }
      }
    }
    if (aFace.TriangulationRepId.IsValid(aStorage.NbFaceTriangulations()))
    {
      if (!aStorage.IsRemoved(aFace.TriangulationRepId))
      {
        const BRepGraphInc::FaceTriangulationRep& aUse =
          aStorage.FaceTriangulationRep(aFace.TriangulationRepId);
        aRememberOwner(aTriangulationOwners,
                       aTriangulationOwner,
                       aFace.TriangulationRepId.Index,
                       aFaceId);
        if (aUse.ParentFaceId != aFaceId)
        {
          theIssues.Append(
            Issue{Severity::Error, aFaceId, "FaceTriangulationRep.ParentFaceId mismatch"});
        }
        if (aUse.Triangulation.IsNull())
        {
          theIssues.Append(
            Issue{Severity::Error, aFaceId, "FaceTriangulationRep.Triangulation is null"});
        }
      }
    }
  }

  const auto aCheckStore = [&](const char*                                       theName,
                               const uint32_t                                    theCount,
                               const auto                                        theMakeId,
                               const NCollection_LinearVector<uint32_t>&         theOwnerCounts,
                               const NCollection_LinearVector<BRepGraph_NodeId>& theOwners) {
    for (uint32_t anIdx = 0; anIdx < theCount; ++anIdx)
    {
      const auto anId = theMakeId(anIdx);
      if (aStorage.IsRemoved(anId))
      {
        continue;
      }
      if (theOwnerCounts[anIdx] == 0)
      {
        TCollection_AsciiString aDesc("Active ");
        aDesc += theName;
        aDesc += " has no owner";
        theIssues.Append(Issue{Severity::Error, BRepGraph_NodeId(), aDesc});
      }
      else if (theOwnerCounts[anIdx] > 1)
      {
        TCollection_AsciiString aDesc("Active ");
        aDesc += theName;
        aDesc += " has multiple owners";
        theIssues.Append(Issue{Severity::Error, theOwners[anIdx], aDesc});
      }
    }
  };

  aCheckStore(
    "FaceSurfaceRep",
    aStorage.NbFaceSurfaces(),
    [](uint32_t theIdx) { return BRepGraph_FaceSurfaceRepId(theIdx); },
    aSurfaceOwners,
    aSurfaceOwner);
  aCheckStore(
    "FaceTriangulationRep",
    aStorage.NbFaceTriangulations(),
    [](uint32_t theIdx) { return BRepGraph_FaceTriangulationRepId(theIdx); },
    aTriangulationOwners,
    aTriangulationOwner);
  aCheckStore(
    "EdgeCurve3DRep",
    aStorage.NbEdgeCurves3D(),
    [](uint32_t theIdx) { return BRepGraph_EdgeCurve3DRepId(theIdx); },
    aCurve3DOwners,
    aCurve3DOwner);
  aCheckStore(
    "EdgePolygon3DRep",
    aStorage.NbEdgePolygons3D(),
    [](uint32_t theIdx) { return BRepGraph_EdgePolygon3DRepId(theIdx); },
    aPolygon3DOwners,
    aPolygon3DOwner);
  aCheckStore(
    "CoEdgeCurve2DRep",
    aStorage.NbCoEdgeCurves2D(),
    [](uint32_t theIdx) { return BRepGraph_CoEdgeCurve2DRepId(theIdx); },
    aCurve2DOwners,
    aCurve2DOwner);
  aCheckStore(
    "CoEdgePolygon2DRep",
    aStorage.NbCoEdgePolygons2D(),
    [](uint32_t theIdx) { return BRepGraph_CoEdgePolygon2DRepId(theIdx); },
    aPolygon2DOwners,
    aPolygon2DOwner);
  aCheckStore(
    "CoEdgePolygonOnTriRep",
    aStorage.NbCoEdgePolygonsOnTri(),
    [](uint32_t theIdx) { return BRepGraph_CoEdgePolygonOnTriRepId(theIdx); },
    aPolygonOnTriOwners,
    aPolygonOnTriOwner);
}

namespace
{

//! Verify that removed entities are not referenced by any active (non-removed)
//! parent entity through forward incidence refs.
//! @param[in]     theGraph  source graph
//! @param[in,out] theIssues collection to append diagnostic issues
void checkRemovedNodeIsolation(const BRepGraph&                                     theGraph,
                               NCollection_LinearVector<BRepGraph_Validate::Issue>& theIssues)
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
        theGraph.Refs().Vertices().Entry(anEdge.StartVertexRefId).ChildVertexId;
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
        theGraph.Refs().Vertices().Entry(anEdge.EndVertexRefId).ChildVertexId;
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
    for (BRepGraph_CoEdgesOfWire anIt(theGraph, aWireId); anIt.More(); anIt.Next())
    {
      if (hasRemovedEdge)
      {
        break;
      }
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        theGraph.Topo().CoEdges().Definition(anIt.CurrentId());
      const BRepGraph_NodeId anEdgeId = aCoEdge.ChildEdgeId;
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
    const BRepGraph_NodeId         aFaceId = aCoEdge.FaceId;
    if (aFaceId.IsValid() && isEntityRemoved(theGraph, aFaceId))
    {
      theIssues.Append(Issue{Severity::Error,
                             aCoEdgeIt.CurrentId(),
                             "Non-removed CoEdgeDef.FaceId references removed FaceDef"});
    }
  }
}

//! Check wire edge connectivity: each stored coedge's end vertex must match
//! the next stored coedge's start vertex.
//! @param[in]     theGraph  source graph
//! @param[in,out] theIssues collection to append diagnostic issues
void checkWireConnectivity(const BRepGraph&                                     theGraph,
                           NCollection_LinearVector<BRepGraph_Validate::Issue>& theIssues)
{
  using Issue    = BRepGraph_Validate::Issue;
  using Severity = BRepGraph_Validate::Severity;

  for (BRepGraph_Iterator<BRepGraphInc::WireDef> aWireIt(theGraph); aWireIt.More(); aWireIt.Next())
  {
    const BRepGraph_WireId             aWireId  = aWireIt.CurrentId();
    const BRepGraphInc::WireRelations& aWireRel = theGraph.Topo().Wires().Relations(aWireId);
    if (aWireRel.CoEdgeIds.Size() < 2)
    {
      continue;
    }

    BRepGraph_CoEdgeId aPrevId = aWireRel.CoEdgeIds.Value(0);
    for (size_t anIdx = 1; anIdx < aWireRel.CoEdgeIds.Size(); ++anIdx)
    {
      const BRepGraph_CoEdgeId aCurrId = aWireRel.CoEdgeIds.Value(anIdx);
      if (!aPrevId.IsValid(theGraph.Topo().CoEdges().Nb())
          || !aCurrId.IsValid(theGraph.Topo().CoEdges().Nb()))
      {
        aPrevId = aCurrId;
        continue;
      }

      const BRepGraphInc::CoEdgeDef& aPrevCoEdge = theGraph.Topo().CoEdges().Definition(aPrevId);
      const BRepGraphInc::CoEdgeDef& aCurrCoEdge = theGraph.Topo().CoEdges().Definition(aCurrId);
      if (!aPrevCoEdge.ChildEdgeId.IsValid(theGraph.Topo().Edges().Nb())
          || !aCurrCoEdge.ChildEdgeId.IsValid(theGraph.Topo().Edges().Nb()))
      {
        aPrevId = aCurrId;
        continue;
      }

      const BRepGraphInc::EdgeDef& aPrevEdge =
        theGraph.Topo().Edges().Definition(aPrevCoEdge.ChildEdgeId);
      const BRepGraphInc::EdgeDef& aCurrEdge =
        theGraph.Topo().Edges().Definition(aCurrCoEdge.ChildEdgeId);

      // Resolve oriented end vertex of previous edge.
      const BRepGraph_VertexRefId aPrevEndRefId = (aPrevCoEdge.Orientation == TopAbs_FORWARD)
                                                    ? aPrevEdge.EndVertexRefId
                                                    : aPrevEdge.StartVertexRefId;
      const BRepGraph_NodeId      aPrevEnd =
        aPrevEndRefId.IsValid()
               ? BRepGraph_VertexId(theGraph.Refs().Vertices().Entry(aPrevEndRefId).ChildVertexId)
               : BRepGraph_NodeId();

      // Resolve oriented start vertex of current edge.
      const BRepGraph_VertexRefId aCurrStartRefId = (aCurrCoEdge.Orientation == TopAbs_FORWARD)
                                                      ? aCurrEdge.StartVertexRefId
                                                      : aCurrEdge.EndVertexRefId;
      const BRepGraph_NodeId      aCurrStart =
        aCurrStartRefId.IsValid()
               ? BRepGraph_VertexId(theGraph.Refs().Vertices().Entry(aCurrStartRefId).ChildVertexId)
               : BRepGraph_NodeId();

      if (aPrevEnd.IsValid() && aCurrStart.IsValid() && aPrevEnd != aCurrStart)
      {
        TCollection_AsciiString aDesc("Wire edges not connected: edge[");
        aDesc += TCollection_AsciiString(static_cast<int>(anIdx - 1));
        aDesc += "] end != edge[";
        aDesc += TCollection_AsciiString(static_cast<int>(anIdx));
        aDesc += "] start";
        theIssues.Append(Issue{Severity::Warning, aWireId, aDesc});
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
                       NCollection_LinearVector<BRepGraph_Validate::Issue>& theIssues)
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

  const auto countActiveUses = [&](const uint32_t theNb, const auto theMakeId) -> int {
    int aCount = 0;
    for (uint32_t anIdx = 0; anIdx < theNb; ++anIdx)
    {
      if (!theMakeId(anIdx).IsRemoved(theGraph))
      {
        ++aCount;
      }
    }
    return aCount;
  };

  verify("FaceSurfaces",
         aDefs.Geometry().NbActiveFaceSurfaces(),
         countActiveUses(aDefs.Geometry().NbFaceSurfaces(),
                         [](uint32_t theIdx) { return BRepGraph_FaceSurfaceRepId(theIdx); }));
  verify("FaceTriangulations",
         theGraph.Mesh().Poly().NbActiveTriangulations(),
         countActiveUses(theGraph.Mesh().Poly().NbFaceTriangulations(),
                         [](uint32_t theIdx) { return BRepGraph_FaceTriangulationRepId(theIdx); }));
  verify("EdgeCurves3D",
         aDefs.Geometry().NbActiveEdgeCurves3D(),
         countActiveUses(aDefs.Geometry().NbEdgeCurves3D(),
                         [](uint32_t theIdx) { return BRepGraph_EdgeCurve3DRepId(theIdx); }));
  verify("EdgePolygons3D",
         theGraph.Mesh().Poly().NbActivePolygons3D(),
         countActiveUses(theGraph.Mesh().Poly().NbEdgePolygons3D(),
                         [](uint32_t theIdx) { return BRepGraph_EdgePolygon3DRepId(theIdx); }));
  verify("CoEdgeCurves2D",
         aDefs.Geometry().NbActiveCoEdgeCurves2D(),
         countActiveUses(aDefs.Geometry().NbCoEdgeCurves2D(),
                         [](uint32_t theIdx) { return BRepGraph_CoEdgeCurve2DRepId(theIdx); }));
  verify("CoEdgePolygons2D",
         theGraph.Mesh().Poly().NbActivePolygons2D(),
         countActiveUses(theGraph.Mesh().Poly().NbCoEdgePolygons2D(),
                         [](uint32_t theIdx) { return BRepGraph_CoEdgePolygon2DRepId(theIdx); }));
  verify("CoEdgePolygonsOnTri",
         theGraph.Mesh().Poly().NbActivePolygonsOnTri(),
         countActiveUses(theGraph.Mesh().Poly().NbCoEdgePolygonsOnTri(), [](uint32_t theIdx) {
           return BRepGraph_CoEdgePolygonOnTriRepId(theIdx);
         }));
}

//! Validate document roots. A root Product is an entry point into the assembly
//! forest; it must be active, unique, and not referenced by any live Occurrence.
//! @param[in]     theGraph  source graph
//! @param[in,out] theIssues collection to append diagnostic issues
void checkDocumentRootProducts(const BRepGraph&                                     theGraph,
                               NCollection_LinearVector<BRepGraph_Validate::Issue>& theIssues)
{
  using Issue    = BRepGraph_Validate::Issue;
  using Severity = BRepGraph_Validate::Severity;

  const BRepGraph::TopoView&               aDefs = theGraph.Topo();
  NCollection_FlatMap<BRepGraph_ProductId> aSeenRoots;
  for (const BRepGraph_ProductId& aRootProductId : theGraph.RootProductIds())
  {
    if (!aRootProductId.IsValidIn(aDefs.Products()))
    {
      theIssues.Append(
        Issue{Severity::Error, aRootProductId, "Document root Product is out of bounds"});
      continue;
    }

    if (aRootProductId.IsRemoved(theGraph))
    {
      theIssues.Append(Issue{Severity::Error, aRootProductId, "Document root Product is removed"});
      continue;
    }

    if (!aSeenRoots.Add(aRootProductId))
    {
      theIssues.Append(
        Issue{Severity::Error, aRootProductId, "Document root Product is listed more than once"});
    }

    for (const BRepGraph_OccurrenceRefId& anOccRefId :
         theGraph.Refs().Occurrences().IdsReferencing(BRepGraph_NodeId(aRootProductId)))
    {
      if (!anOccRefId.IsValid(theGraph.Refs().Occurrences().Nb()) || anOccRefId.IsRemoved(theGraph))
      {
        continue;
      }
      const BRepGraph_OccurrenceId anOccId =
        theGraph.Refs().Occurrences().Entry(anOccRefId).ChildOccurrenceId;
      if (anOccId.IsValidIn(aDefs.Occurrences()) && !anOccId.IsRemoved(theGraph))
      {
        theIssues.Append(Issue{Severity::Error,
                               aRootProductId,
                               "Document root Product is referenced by an active Occurrence"});
        break;
      }
    }
  }
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
  if (theGraph.IsEmpty())
  {
    return aResult;
  }

  if (theOptions.ValidationMode == Mode::Lightweight)
  {
    NCollection_LinearVector<BRepGraph::EditorView::BoundaryIssue> aBoundaryIssues;
    if (!theGraph.Editor().ValidateMutationBoundary(&aBoundaryIssues))
    {
      appendMutationBoundaryIssues(aBoundaryIssues, aResult.Issues);
    }
    checkRemovedNodeIsolation(theGraph, aResult.Issues);
    checkDocumentRootProducts(theGraph, aResult.Issues);
    return aResult;
  }

  NCollection_LinearVector<BRepGraph::EditorView::BoundaryIssue> aBoundaryIssues;
  if (!theGraph.Editor().ValidateMutationBoundary(&aBoundaryIssues))
  {
    appendMutationBoundaryIssues(aBoundaryIssues, aResult.Issues);
  }

  checkCrossReferenceBounds(theGraph, aResult.Issues);
  checkRelationConsistency(theGraph, aResult.Issues);
  checkRelationFaceCountCache(theGraph, aResult.Issues);
  checkIncidenceRefConsistency(theGraph, aResult.Issues);
  checkGeometryReferences(theGraph, aResult.Issues);
  CheckOwnedUseReferences(theGraph, aResult.Issues);
  checkRemovedNodeIsolation(theGraph, aResult.Issues);
  checkWireConnectivity(theGraph, aResult.Issues);

  checkActiveCounts(theGraph, aResult.Issues);
  checkDocumentRootProducts(theGraph, aResult.Issues);

  if (const occ::handle<BRepGraph_LayerTopoSupplement> aSupplementLayer =
        theGraph.LayerRegistry().FindLayer<BRepGraph_LayerTopoSupplement>();
      !aSupplementLayer.IsNull())
  {
    try
    {
      aSupplementLayer->Validate();
    }
    catch (const Standard_Failure& theFailure)
    {
      aResult.Issues.Append(
        Issue{Severity::Error,
              BRepGraph_NodeId(),
              TCollection_AsciiString("LayerTopoSupplement invalid: ") + theFailure.what()});
    }
  }

  // UID integrity checks: all active nodes must have a valid UID that round-trips.
  const BRepGraph::TopoView& aDefs = theGraph.Topo();
  const BRepGraph::UIDsView& aUIDs = theGraph.UIDs();
  auto checkUIDKind                = [&](const BRepGraph_NodeId::Kind theKind, const int theNb) {
    for (int anIdx = 0; anIdx < theNb; ++anIdx)
    {
      const BRepGraph_NodeId       aNode(theKind, anIdx);
      const BRepGraphInc::BaseDef* aDef = aDefs.Gen().TopoEntity(aNode);
      if (aDef == nullptr || aNode.IsRemoved(theGraph))
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
    NCollection_FlatMap<BRepGraph_ProductId>      aVisited;
    NCollection_LinearVector<BRepGraph_ProductId> aQueue;
    size_t                                        aHead = 0;

    // Seed with direct children.
    for (BRepGraph_RefsOccurrenceOfProduct anOccIt(theGraph, aProdId); anOccIt.More();
         anOccIt.Next())
    {
      const BRepGraphInc::OccurrenceRef& anOccRef =
        theGraph.Refs().Occurrences().Entry(anOccIt.CurrentId());
      const BRepGraphInc::OccurrenceDef& anOcc =
        aDefs.Occurrences().Definition(anOccRef.ChildOccurrenceId);
      if (anOccIt.CurrentId().IsRemoved(theGraph)
          || anOcc.ChildNodeId.NodeKind != BRepGraph_NodeId::Kind::Product)
      {
        continue;
      }
      const BRepGraph_ProductId aChildProdId = BRepGraph_ProductId::FromNodeId(anOcc.ChildNodeId);
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
      if (aChildProdId.IsRemoved(theGraph))
      {
        continue;
      }
      for (BRepGraph_RefsOccurrenceOfProduct aRefIt(theGraph, aChildProdId); aRefIt.More();
           aRefIt.Next())
      {
        const BRepGraphInc::OccurrenceRef& aRef =
          theGraph.Refs().Occurrences().Entry(aRefIt.CurrentId());
        const BRepGraphInc::OccurrenceDef& aOcc =
          aDefs.Occurrences().Definition(aRef.ChildOccurrenceId);
        if (aRefIt.CurrentId().IsRemoved(theGraph)
            || aOcc.ChildNodeId.NodeKind != BRepGraph_NodeId::Kind::Product)
        {
          continue;
        }
        const BRepGraph_ProductId aDescProdId = BRepGraph_ProductId::FromNodeId(aOcc.ChildNodeId);
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
  // directly or transitively through ChildRefs whose ChildNodeId.NodeKind is
  // Compound. Distinct from Product/Occurrence cycles (those are assembly-level).
  for (BRepGraph_Iterator<BRepGraphInc::CompoundDef> aCompIt(theGraph); aCompIt.More();
       aCompIt.Next())
  {
    const BRepGraph_CompoundId                     aRootCompoundId = aCompIt.CurrentId();
    NCollection_FlatMap<BRepGraph_CompoundId>      aVisited;
    NCollection_LinearVector<BRepGraph_CompoundId> aQueue;
    size_t                                         aHead = 0;

    for (BRepGraph_RefsChildOfCompound anIt(theGraph, aRootCompoundId); anIt.More(); anIt.Next())
    {
      const BRepGraphInc::ChildRef& aCR = theGraph.Refs().Children().Entry(anIt.CurrentId());
      if (theGraph.Refs().Gen().IsRemoved(anIt.CurrentId())
          || aCR.ChildNodeId.NodeKind != BRepGraph_NodeId::Kind::Compound
          || !aCR.ChildNodeId.IsValid())
      {
        continue;
      }
      const BRepGraph_CompoundId aChildCompoundId =
        BRepGraph_CompoundId::FromNodeId(aCR.ChildNodeId);
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
      if (aChildCompoundId.IsRemoved(theGraph))
      {
        continue;
      }
      for (BRepGraph_RefsChildOfCompound aRefIt(theGraph, aChildCompoundId); aRefIt.More();
           aRefIt.Next())
      {
        const BRepGraphInc::ChildRef& aCR = theGraph.Refs().Children().Entry(aRefIt.CurrentId());
        if (theGraph.Refs().Gen().IsRemoved(aRefIt.CurrentId())
            || aCR.ChildNodeId.NodeKind != BRepGraph_NodeId::Kind::Compound
            || !aCR.ChildNodeId.IsValid())
        {
          continue;
        }
        const BRepGraph_CompoundId aDescCompoundId =
          BRepGraph_CompoundId::FromNodeId(aCR.ChildNodeId);
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

  // Ref ownership: every active ref is a parent-owned usage record and must be
  // reachable from exactly one live parent slot. Sharing a ref id across two
  // parents, or using it twice in one parent, makes branch context ambiguous.
  {
    const BRepGraph::RefsView& aRefs            = theGraph.Refs();
    auto                       appendOwnerIssue = [&](const BRepGraph_NodeId         theNode,
                                const int                      theOwnerCount,
                                const TCollection_AsciiString& theKind) {
      if (theOwnerCount == 0)
      {
        TCollection_AsciiString aDesc("Orphan ");
        aDesc += theKind;
        aDesc += ": no live parent owns it";
        aResult.Issues.Append(Issue{Severity::Error, theNode, aDesc});
      }
      else if (theOwnerCount > 1)
      {
        TCollection_AsciiString aDesc("Shared ");
        aDesc += theKind;
        aDesc += ": more than one live parent slot owns it";
        aResult.Issues.Append(Issue{Severity::Error, theNode, aDesc});
      }
    };

    for (BRepGraph_FullChildRefIterator aChildRefIt(theGraph); aChildRefIt.More();
         aChildRefIt.Next())
    {
      const BRepGraph_ChildRefId    aChildRefId = aChildRefIt.CurrentId();
      const BRepGraphInc::ChildRef& aCR         = aRefs.Children().Entry(aChildRefId);
      if (theGraph.Refs().Gen().IsRemoved(aChildRefId))
      {
        continue;
      }
      uint32_t anOwnerCount = 0;
      for (BRepGraph_CompoundId aCompoundId = BRepGraph_CompoundId::Start();
           aCompoundId < BRepGraph_CompoundId(aDefs.Compounds().Nb());
           ++aCompoundId)
      {
        if (aCompoundId.IsRemoved(theGraph))
        {
          continue;
        }
        for (const BRepGraph_ChildRefId& aRefId :
             aDefs.Compounds().Relations(aCompoundId).ChildRefIds)
        {
          if (aRefId == aChildRefId)
          {
            ++anOwnerCount;
          }
        }
      }
      appendOwnerIssue(aCR.ChildNodeId, anOwnerCount, "ChildRef");
    }

    for (BRepGraph_FullSolidRefIterator aSolidRefIt(theGraph); aSolidRefIt.More();
         aSolidRefIt.Next())
    {
      const BRepGraph_SolidRefId    aSolidRefId = aSolidRefIt.CurrentId();
      const BRepGraphInc::SolidRef& aSR         = aRefs.Solids().Entry(aSolidRefId);
      if (theGraph.Refs().Gen().IsRemoved(aSolidRefId))
      {
        continue;
      }
      uint32_t anOwnerCount = 0;
      for (BRepGraph_CompSolidId aCompSolidId = BRepGraph_CompSolidId::Start();
           aCompSolidId < BRepGraph_CompSolidId(aDefs.CompSolids().Nb());
           ++aCompSolidId)
      {
        if (aCompSolidId.IsRemoved(theGraph))
        {
          continue;
        }
        for (const BRepGraph_SolidRefId& aRefId :
             aDefs.CompSolids().Relations(aCompSolidId).SolidRefIds)
        {
          if (aRefId == aSolidRefId)
          {
            ++anOwnerCount;
          }
        }
      }
      appendOwnerIssue(aSR.ChildSolidId, anOwnerCount, "SolidRef");
    }

    for (BRepGraph_FullShellRefIterator aShellRefIt(theGraph); aShellRefIt.More();
         aShellRefIt.Next())
    {
      const BRepGraph_ShellRefId    aShellRefId = aShellRefIt.CurrentId();
      const BRepGraphInc::ShellRef& aRef        = aRefs.Shells().Entry(aShellRefId);
      if (theGraph.Refs().Gen().IsRemoved(aShellRefId))
      {
        continue;
      }
      uint32_t anOwnerCount = 0;
      for (BRepGraph_SolidId aSolidId = BRepGraph_SolidId::Start();
           aSolidId < BRepGraph_SolidId(aDefs.Solids().Nb());
           ++aSolidId)
      {
        if (aSolidId.IsRemoved(theGraph))
        {
          continue;
        }
        for (const BRepGraph_ShellRefId& aRefId : aDefs.Solids().Relations(aSolidId).ShellRefIds)
        {
          if (aRefId == aShellRefId)
          {
            ++anOwnerCount;
          }
        }
      }
      appendOwnerIssue(aRef.ChildShellId, anOwnerCount, "ShellRef");
    }

    for (BRepGraph_FullFaceRefIterator aFaceRefIt(theGraph); aFaceRefIt.More(); aFaceRefIt.Next())
    {
      const BRepGraph_FaceRefId    aFaceRefId = aFaceRefIt.CurrentId();
      const BRepGraphInc::FaceRef& aRef       = aRefs.Faces().Entry(aFaceRefId);
      if (theGraph.Refs().Gen().IsRemoved(aFaceRefId))
      {
        continue;
      }
      uint32_t anOwnerCount = 0;
      for (BRepGraph_ShellId aShellId = BRepGraph_ShellId::Start();
           aShellId < BRepGraph_ShellId(aDefs.Shells().Nb());
           ++aShellId)
      {
        if (aShellId.IsRemoved(theGraph))
        {
          continue;
        }
        for (const BRepGraph_FaceRefId& aRefId : aDefs.Shells().Relations(aShellId).FaceRefIds)
        {
          if (aRefId == aFaceRefId)
          {
            ++anOwnerCount;
          }
        }
      }
      appendOwnerIssue(aRef.ChildFaceId, anOwnerCount, "FaceRef");
    }

    for (BRepGraph_FullWireRefIterator aWireRefIt(theGraph); aWireRefIt.More(); aWireRefIt.Next())
    {
      const BRepGraph_WireRefId    aWireRefId = aWireRefIt.CurrentId();
      const BRepGraphInc::WireRef& aRef       = aRefs.Wires().Entry(aWireRefId);
      if (theGraph.Refs().Gen().IsRemoved(aWireRefId))
      {
        continue;
      }
      uint32_t anOwnerCount = 0;
      for (BRepGraph_FaceId aFaceId = BRepGraph_FaceId::Start();
           aFaceId < BRepGraph_FaceId(aDefs.Faces().Nb());
           ++aFaceId)
      {
        if (aFaceId.IsRemoved(theGraph))
        {
          continue;
        }
        for (const BRepGraph_WireRefId& aRefId : aDefs.Faces().Relations(aFaceId).WireRefIds)
        {
          if (aRefId == aWireRefId)
          {
            ++anOwnerCount;
          }
        }
      }
      appendOwnerIssue(aRef.ChildWireId, anOwnerCount, "WireRef");
    }

    // CoEdges: every active coedge must be owned by at least one live wire.
    for (BRepGraph_Iterator<BRepGraphInc::CoEdgeDef> aCoEdgeIt(theGraph); aCoEdgeIt.More();
         aCoEdgeIt.Next())
    {
      const BRepGraph_CoEdgeId aCoEdgeId = aCoEdgeIt.CurrentId();
      if (BRepGraph_NodeId(aCoEdgeId).IsRemoved(theGraph))
      {
        continue;
      }

      const BRepGraph_WireId aWireId = aDefs.CoEdges().Wire(aCoEdgeId);
      const bool hasLiveWire = aWireId.IsValidIn(aDefs.Wires()) && !aWireId.IsRemoved(theGraph);

      if (!hasLiveWire)
      {
        aResult.Issues.Append(
          Issue{Severity::Error, aCoEdgeId, "Orphan CoEdge: not owned by any live Wire"});
      }
    }

    for (BRepGraph_FullVertexRefIterator aVertexRefIt(theGraph); aVertexRefIt.More();
         aVertexRefIt.Next())
    {
      const BRepGraph_VertexRefId    aVertexRefId = aVertexRefIt.CurrentId();
      const BRepGraphInc::VertexRef& aRef         = aRefs.Vertices().Entry(aVertexRefId);
      if (theGraph.Refs().Gen().IsRemoved(aVertexRefId))
      {
        continue;
      }
      uint32_t anOwnerCount = 0;
      for (BRepGraph_EdgeId anEdgeId = BRepGraph_EdgeId::Start();
           anEdgeId < BRepGraph_EdgeId(aDefs.Edges().Nb());
           ++anEdgeId)
      {
        const BRepGraphInc::EdgeDef& anEdge = aDefs.Edges().Definition(anEdgeId);
        if (anEdgeId.IsRemoved(theGraph))
        {
          continue;
        }
        if (anEdge.StartVertexRefId == aVertexRefId)
        {
          ++anOwnerCount;
        }
        if (anEdge.EndVertexRefId == aVertexRefId)
        {
          ++anOwnerCount;
        }
      }
      appendOwnerIssue(aRef.ChildVertexId, anOwnerCount, "VertexRef");
    }

    for (BRepGraph_FullOccurrenceRefIterator anOccurrenceRefIt(theGraph); anOccurrenceRefIt.More();
         anOccurrenceRefIt.Next())
    {
      const BRepGraph_OccurrenceRefId    aOccurrenceRefId = anOccurrenceRefIt.CurrentId();
      const BRepGraphInc::OccurrenceRef& aRef = aRefs.Occurrences().Entry(aOccurrenceRefId);
      if (theGraph.Refs().Gen().IsRemoved(aOccurrenceRefId))
      {
        continue;
      }
      uint32_t anOwnerCount = 0;
      for (BRepGraph_ProductIterator aProductIt(theGraph); aProductIt.More(); aProductIt.Next())
      {
        for (const BRepGraph_OccurrenceRefId& aRefId :
             aDefs.Products().Relations(aProductIt.CurrentId()).OccurrenceRefIds)
        {
          if (aRefId == aOccurrenceRefId)
          {
            ++anOwnerCount;
          }
        }
      }
      appendOwnerIssue(aRef.ChildOccurrenceId, anOwnerCount, "OccurrenceRef");
    }

    for (BRepGraph_Iterator<BRepGraphInc::OccurrenceDef> anOccIt(theGraph); anOccIt.More();
         anOccIt.Next())
    {
      if (anOccIt.CurrentId().IsRemoved(theGraph))
      {
        continue;
      }
      int anOwnerCount = 0;
      for (BRepGraph_FullOccurrenceRefIterator aRefIt(theGraph); aRefIt.More(); aRefIt.Next())
      {
        const BRepGraphInc::OccurrenceRef& aRef = aRefs.Occurrences().Entry(aRefIt.CurrentId());
        if (!theGraph.Refs().Gen().IsRemoved(aRefIt.CurrentId())
            && aRef.ChildOccurrenceId == anOccIt.CurrentId())
        {
          ++anOwnerCount;
        }
      }
      appendOwnerIssue(anOccIt.CurrentId(), anOwnerCount, "OccurrenceDef");
    }
  }

  // Occurrence-to-Product liveness: every active OccurrenceDef whose ChildNodeId
  // points to a Product must target a live Product. This catches the case where
  // a Product is soft-removed but dependent Occurrences survived.
  for (BRepGraph_Iterator<BRepGraphInc::OccurrenceDef> anOccIt(theGraph); anOccIt.More();
       anOccIt.Next())
  {
    const BRepGraphInc::OccurrenceDef& anOcc = anOccIt.Current();
    if (anOcc.ChildNodeId.NodeKind != BRepGraph_NodeId::Kind::Product)
    {
      continue;
    }
    const BRepGraph_ProductId aChildProductId(anOcc.ChildNodeId);
    if (!aChildProductId.IsValidIn(aDefs.Products()) || aChildProductId.IsRemoved(theGraph))
    {
      aResult.Issues.Append(
        Issue{Severity::Error,
              anOccIt.CurrentId(),
              "Active OccurrenceDef.ChildNodeId points to a removed or out-of-range Product"});
    }
  }

  // Cross-container ownership: a Solid must not be simultaneously referenced by
  // an active Compound ChildRef AND an active CompSolid SolidRef. OCCT models
  // a solid as belonging to either a Compound or a CompSolid, not both.
  {
    const BRepGraph::RefsView&             aRefs = theGraph.Refs();
    NCollection_FlatMap<BRepGraph_SolidId> aCompoundOwnedSolids;
    for (BRepGraph_FullChildRefIterator aChildRefIt(theGraph); aChildRefIt.More();
         aChildRefIt.Next())
    {
      const BRepGraph_ChildRefId    aChildRefId = aChildRefIt.CurrentId();
      const BRepGraphInc::ChildRef& aCR         = aRefs.Children().Entry(aChildRefId);
      if (theGraph.Refs().Gen().IsRemoved(aChildRefId))
      {
        continue;
      }
      if (aCR.ChildNodeId.NodeKind == BRepGraph_NodeId::Kind::Solid && aCR.ChildNodeId.IsValid())
      {
        aCompoundOwnedSolids.Add(BRepGraph_SolidId::FromNodeId(aCR.ChildNodeId));
      }
    }
    for (BRepGraph_FullSolidRefIterator aSolidRefIt(theGraph); aSolidRefIt.More();
         aSolidRefIt.Next())
    {
      const BRepGraph_SolidRefId    aSolidRefId = aSolidRefIt.CurrentId();
      const BRepGraphInc::SolidRef& aSR         = aRefs.Solids().Entry(aSolidRefId);
      if (theGraph.Refs().Gen().IsRemoved(aSolidRefId))
      {
        continue;
      }
      if (aSR.ChildSolidId.IsValid() && aCompoundOwnedSolids.Contains(aSR.ChildSolidId))
      {
        aResult.Issues.Append(
          Issue{Severity::Error,
                aSR.ChildSolidId,
                "Cross-container ownership: Solid is referenced by both a Compound (ChildRef) "
                "and a CompSolid (SolidRef)"});
      }
    }
  }

  return aResult;
}
