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

#include <BRepGraphAlgo_Validate.hxx>

#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_RelEdgesView.hxx>
#include <BRepGraphInc_IncidenceRef.hxx>
#include <BRepGraphInc_WireExplorer.hxx>

#include <NCollection_Map.hxx>

namespace
{

//! Check that a NodeId refers to a valid index within graph bounds.
bool isValidNodeId(const BRepGraph& theGraph, BRepGraph_NodeId theId)
{
  if (!theId.IsValid())
    return false;

  switch (theId.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Solid:      return theId.Index < theGraph.Defs().NbSolids();
    case BRepGraph_NodeId::Kind::Shell:      return theId.Index < theGraph.Defs().NbShells();
    case BRepGraph_NodeId::Kind::Face:       return theId.Index < theGraph.Defs().NbFaces();
    case BRepGraph_NodeId::Kind::Wire:       return theId.Index < theGraph.Defs().NbWires();
    case BRepGraph_NodeId::Kind::CoEdge:     return theId.Index < theGraph.Defs().NbCoEdges();
    case BRepGraph_NodeId::Kind::Edge:       return theId.Index < theGraph.Defs().NbEdges();
    case BRepGraph_NodeId::Kind::Vertex:     return theId.Index < theGraph.Defs().NbVertices();
    case BRepGraph_NodeId::Kind::Compound:   return theId.Index < theGraph.Defs().NbCompounds();
    case BRepGraph_NodeId::Kind::CompSolid:  return theId.Index < theGraph.Defs().NbCompSolids();
    case BRepGraph_NodeId::Kind::Product:    return theId.Index < theGraph.Defs().NbProducts();
    case BRepGraph_NodeId::Kind::Occurrence: return theId.Index < theGraph.Defs().NbOccurrences();
  }
  return false;
}

//! Check if a topology def is removed.
bool isDefRemoved(const BRepGraph& theGraph, BRepGraph_NodeId theId)
{
  const BRepGraph_TopoNode::BaseDef* aDef = theGraph.Defs().TopoDef(theId);
  return aDef != nullptr && aDef->IsRemoved;
}

// -----------------------------------------------------------------------
// Pass 1: Cross-reference bounds
// -----------------------------------------------------------------------
void checkCrossReferenceBounds(const BRepGraph&                                      theGraph,
                               NCollection_Vector<BRepGraphAlgo_Validate::Issue>& theIssues)
{
  using Issue    = BRepGraphAlgo_Validate::Issue;
  using Severity = BRepGraphAlgo_Validate::Severity;

  // Check EdgeDef references.
  for (int anEdgeIdx = 0; anEdgeIdx < theGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = theGraph.Defs().Edge(anEdgeIdx);
    if (anEdge.IsRemoved)
      continue;

    if (anEdge.StartVertexDefId().IsValid() && !isValidNodeId(theGraph, anEdge.StartVertexDefId()))
    {
      theIssues.Append(Issue{Severity::Error,
                             anEdge.Id,
                             "EdgeDef.StartVertexDefId out of bounds"});
    }
    if (anEdge.EndVertexDefId().IsValid() && !isValidNodeId(theGraph, anEdge.EndVertexDefId()))
    {
      theIssues.Append(Issue{Severity::Error,
                             anEdge.Id,
                             "EdgeDef.EndVertexDefId out of bounds"});
    }
    for (int aPCIdx = 0; aPCIdx < anEdge.PCurves.Length(); ++aPCIdx)
    {
      const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry = anEdge.PCurves.Value(aPCIdx);
      if (aPCEntry.FaceDefId.IsValid() && !isValidNodeId(theGraph, aPCEntry.FaceDefId))
      {
        theIssues.Append(Issue{Severity::Error,
                               anEdge.Id,
                               "EdgeDef.PCurveEntry.FaceDefId out of bounds"});
      }
    }
  }

  // Check FaceDef references.
  for (int aFaceIdx = 0; aFaceIdx < theGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFace = theGraph.Defs().Face(aFaceIdx);
    if (aFace.IsRemoved)
      continue;

    // Surface handle is stored directly on FaceDef; no cross-reference to validate.
  }

  // Check WireDef CoEdgeRefs references.
  for (int aWireIdx = 0; aWireIdx < theGraph.Defs().NbWires(); ++aWireIdx)
  {
    const BRepGraph_TopoNode::WireDef& aWire = theGraph.Defs().Wire(aWireIdx);
    if (aWire.IsRemoved)
      continue;

    for (int aCoEdgeEntryIdx = 0; aCoEdgeEntryIdx < aWire.CoEdgeRefs.Length(); ++aCoEdgeEntryIdx)
    {
      const BRepGraphInc::CoEdgeRef& aCR = aWire.CoEdgeRefs.Value(aCoEdgeEntryIdx);
      const BRepGraph_NodeId aCoEdgeDefId = BRepGraph_NodeId::CoEdge(aCR.CoEdgeIdx);
      if (aCoEdgeDefId.IsValid() && !isValidNodeId(theGraph, aCoEdgeDefId))
      {
        theIssues.Append(Issue{Severity::Error,
                               aWire.Id,
                               "WireDef.CoEdgeRef CoEdgeIdx out of bounds"});
      }
      const BRepGraph_TopoNode::CoEdgeDef& aCoEdge = theGraph.Defs().CoEdge(aCR.CoEdgeIdx);
      const BRepGraph_NodeId anEdgeDefId = BRepGraph_NodeId::Edge(aCoEdge.EdgeIdx);
      if (anEdgeDefId.IsValid() && !isValidNodeId(theGraph, anEdgeDefId))
      {
        theIssues.Append(Issue{Severity::Error,
                               aWire.Id,
                               "WireDef.CoEdgeRef EdgeIdx out of bounds"});
      }
    }
  }

  // Check CompoundDef references.
  for (int aCompIdx = 0; aCompIdx < theGraph.Defs().NbCompounds(); ++aCompIdx)
  {
    const BRepGraph_TopoNode::CompoundDef& aComp = theGraph.Defs().Compound(aCompIdx);
    if (aComp.IsRemoved)
      continue;

    for (int aChildIdx = 0; aChildIdx < aComp.ChildRefs.Length(); ++aChildIdx)
    {
      const BRepGraphInc::ChildRef& aCR = aComp.ChildRefs.Value(aChildIdx);
      const BRepGraph_NodeId aChildId(static_cast<BRepGraph_NodeId::Kind>(aCR.Kind), aCR.ChildIdx);
      if (aChildId.IsValid() && !isValidNodeId(theGraph, aChildId))
      {
        theIssues.Append(Issue{Severity::Error,
                               aComp.Id,
                               "CompoundDef.ChildDefId out of bounds"});
      }
    }
  }

  // Check CompSolidDef references.
  for (int aCSIdx = 0; aCSIdx < theGraph.Defs().NbCompSolids(); ++aCSIdx)
  {
    const BRepGraph_TopoNode::CompSolidDef& aCS = theGraph.Defs().CompSolid(aCSIdx);
    if (aCS.IsRemoved)
      continue;

    for (int aSolidIdx = 0; aSolidIdx < aCS.SolidRefs.Length(); ++aSolidIdx)
    {
      const BRepGraph_NodeId aSolidId = BRepGraph_NodeId::Solid(aCS.SolidRefs.Value(aSolidIdx).SolidIdx);
      if (aSolidId.IsValid() && !isValidNodeId(theGraph, aSolidId))
      {
        theIssues.Append(Issue{Severity::Error,
                               aCS.Id,
                               "CompSolidDef.SolidDefId out of bounds"});
      }
    }
  }

  // Check ShellDef FaceRefs references.
  for (int aShellIdx = 0; aShellIdx < theGraph.Defs().NbShells(); ++aShellIdx)
  {
    const BRepGraph_TopoNode::ShellDef& aShell = theGraph.Defs().Shell(aShellIdx);
    if (aShell.IsRemoved)
      continue;

    for (int aFaceRefIdx = 0; aFaceRefIdx < aShell.FaceRefs.Length(); ++aFaceRefIdx)
    {
      const BRepGraphInc::FaceRef& aFR = aShell.FaceRefs.Value(aFaceRefIdx);
      const BRepGraph_NodeId aFaceDefId = BRepGraph_NodeId::Face(aFR.FaceIdx);
      if (aFaceDefId.IsValid() && !isValidNodeId(theGraph, aFaceDefId))
      {
        theIssues.Append(Issue{Severity::Error,
                               aShell.Id,
                               "ShellDef.FaceRef FaceIdx out of bounds"});
      }
    }
  }

  // Check SolidDef ShellRefs references.
  for (int aSolidIdx = 0; aSolidIdx < theGraph.Defs().NbSolids(); ++aSolidIdx)
  {
    const BRepGraph_TopoNode::SolidDef& aSolid = theGraph.Defs().Solid(aSolidIdx);
    if (aSolid.IsRemoved)
      continue;

    for (int aShellRefIdx = 0; aShellRefIdx < aSolid.ShellRefs.Length(); ++aShellRefIdx)
    {
      const BRepGraphInc::ShellRef& aSR = aSolid.ShellRefs.Value(aShellRefIdx);
      const BRepGraph_NodeId aShellDefId = BRepGraph_NodeId::Shell(aSR.ShellIdx);
      if (aShellDefId.IsValid() && !isValidNodeId(theGraph, aShellDefId))
      {
        theIssues.Append(Issue{Severity::Error,
                               aSolid.Id,
                               "SolidDef.ShellRef ShellIdx out of bounds"});
      }
    }
  }
}

// -----------------------------------------------------------------------
// Pass 2: Reverse index consistency (edge-to-wires)
// -----------------------------------------------------------------------
void checkReverseIndexConsistency(const BRepGraph&                                      theGraph,
                                  NCollection_Vector<BRepGraphAlgo_Validate::Issue>& theIssues)
{
  using Issue    = BRepGraphAlgo_Validate::Issue;
  using Severity = BRepGraphAlgo_Validate::Severity;

  // Build expected edge->wires mapping from CoEdgeRefs.
  NCollection_DataMap<int, NCollection_Map<int>> anExpected;
  for (int aWireIdx = 0; aWireIdx < theGraph.Defs().NbWires(); ++aWireIdx)
  {
    const BRepGraph_TopoNode::WireDef& aWire = theGraph.Defs().Wire(aWireIdx);
    if (aWire.IsRemoved)
      continue;

    for (int aCoEdgeEntryIdx = 0; aCoEdgeEntryIdx < aWire.CoEdgeRefs.Length(); ++aCoEdgeEntryIdx)
    {
      const BRepGraphInc::CoEdgeRef& aCR = aWire.CoEdgeRefs.Value(aCoEdgeEntryIdx);
      const BRepGraph_TopoNode::CoEdgeDef& aCoEdge = theGraph.Defs().CoEdge(aCR.CoEdgeIdx);
      if (aCoEdge.EdgeIdx < 0)
        continue;

      if (!anExpected.IsBound(aCoEdge.EdgeIdx))
        anExpected.Bind(aCoEdge.EdgeIdx, NCollection_Map<int>());
      anExpected.ChangeFind(aCoEdge.EdgeIdx).Add(aWireIdx);
    }
  }

  // Check that RelEdges WiresOfEdge matches expected.
  for (int anEdgeIdx = 0; anEdgeIdx < theGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = theGraph.Defs().Edge(anEdgeIdx);
    if (anEdge.IsRemoved)
      continue;

    const NCollection_Vector<int>& aActualWires = theGraph.RelEdges().WiresOfEdge(anEdgeIdx);
    const NCollection_Map<int>* anExpectedWires  = anExpected.Seek(anEdgeIdx);

    const int anExpectedCount = (anExpectedWires != nullptr) ? anExpectedWires->Extent() : 0;

    // Build a set from actual wires for comparison.
    NCollection_Map<int> anActualSet;
    for (int aWIdx = 0; aWIdx < aActualWires.Length(); ++aWIdx)
      anActualSet.Add(aActualWires.Value(aWIdx));

    if (anActualSet.Extent() != anExpectedCount)
    {
      theIssues.Append(Issue{Severity::Error,
                             anEdge.Id,
                             "Reverse index ReverseIdx.WiresOfEdge size mismatch"});
      continue;
    }

    if (anExpectedWires != nullptr)
    {
      for (NCollection_Map<int>::Iterator anIt(*anExpectedWires); anIt.More(); anIt.Next())
      {
        if (!anActualSet.Contains(anIt.Value()))
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

// -----------------------------------------------------------------------
// Pass 3: Incidence ref consistency (shell->face, solid->shell, etc.)
// -----------------------------------------------------------------------
void checkIncidenceRefConsistency(const BRepGraph&                                      theGraph,
                                  NCollection_Vector<BRepGraphAlgo_Validate::Issue>& theIssues)
{
  using Issue    = BRepGraphAlgo_Validate::Issue;
  using Severity = BRepGraphAlgo_Validate::Severity;

  // Check face->wire incidence refs.
  for (int aFaceIdx = 0; aFaceIdx < theGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFace = theGraph.Defs().Face(aFaceIdx);
    if (aFace.IsRemoved)
      continue;

    for (int aWireRefIdx = 0; aWireRefIdx < aFace.WireRefs.Length(); ++aWireRefIdx)
    {
      const BRepGraphInc::WireRef& aWR = aFace.WireRefs.Value(aWireRefIdx);
      const BRepGraph_NodeId aWireId = BRepGraph_NodeId::Wire(aWR.WireIdx);
      if (aWireId.IsValid() && !isValidNodeId(theGraph, aWireId))
      {
        theIssues.Append(Issue{Severity::Error,
                               aFace.Id,
                               "FaceDef.WireRef WireIdx out of bounds"});
      }
    }
  }

  // Check shell->face incidence refs.
  for (int aShellIdx = 0; aShellIdx < theGraph.Defs().NbShells(); ++aShellIdx)
  {
    const BRepGraph_TopoNode::ShellDef& aShell = theGraph.Defs().Shell(aShellIdx);
    if (aShell.IsRemoved)
      continue;

    for (int aFaceRefIdx = 0; aFaceRefIdx < aShell.FaceRefs.Length(); ++aFaceRefIdx)
    {
      const BRepGraphInc::FaceRef& aFR = aShell.FaceRefs.Value(aFaceRefIdx);
      const BRepGraph_NodeId aFaceId = BRepGraph_NodeId::Face(aFR.FaceIdx);
      if (aFaceId.IsValid() && isDefRemoved(theGraph, aFaceId))
      {
        theIssues.Append(Issue{Severity::Error,
                               aShell.Id,
                               "ShellDef references removed FaceDef"});
      }
    }
  }

  // Check solid->shell incidence refs.
  for (int aSolidIdx = 0; aSolidIdx < theGraph.Defs().NbSolids(); ++aSolidIdx)
  {
    const BRepGraph_TopoNode::SolidDef& aSolid = theGraph.Defs().Solid(aSolidIdx);
    if (aSolid.IsRemoved)
      continue;

    for (int aShellRefIdx = 0; aShellRefIdx < aSolid.ShellRefs.Length(); ++aShellRefIdx)
    {
      const BRepGraphInc::ShellRef& aSR = aSolid.ShellRefs.Value(aShellRefIdx);
      const BRepGraph_NodeId aShellId = BRepGraph_NodeId::Shell(aSR.ShellIdx);
      if (aShellId.IsValid() && isDefRemoved(theGraph, aShellId))
      {
        theIssues.Append(Issue{Severity::Error,
                               aSolid.Id,
                               "SolidDef references removed ShellDef"});
      }
    }
  }
}

// -----------------------------------------------------------------------
// Pass 4: Geometry reference validity
// -----------------------------------------------------------------------
void checkGeometryReferences(const BRepGraph&                                      theGraph,
                             NCollection_Vector<BRepGraphAlgo_Validate::Issue>& theIssues)
{
  using Issue    = BRepGraphAlgo_Validate::Issue;
  using Severity = BRepGraphAlgo_Validate::Severity;

  // Check edge->curve references (handles stored directly on EdgeDef).
  for (int anEdgeIdx = 0; anEdgeIdx < theGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = theGraph.Defs().Edge(anEdgeIdx);
    if (anEdge.IsRemoved)
      continue;

    if (!anEdge.IsDegenerate && anEdge.Curve3d.IsNull())
    {
      theIssues.Append(Issue{Severity::Error,
                             anEdge.Id,
                             "Non-degenerate EdgeDef has null Curve3d handle"});
    }

    // Check inline PCurve data.
    for (int aPCIdx = 0; aPCIdx < anEdge.PCurves.Length(); ++aPCIdx)
    {
      const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry = anEdge.PCurves.Value(aPCIdx);
      if (aPCEntry.Curve2d.IsNull())
      {
        theIssues.Append(Issue{Severity::Error,
                               anEdge.Id,
                               "EdgeDef.PCurveEntry has null Curve2d handle"});
      }
    }
  }
}

// -----------------------------------------------------------------------
// Pass 5: Removed node isolation
// -----------------------------------------------------------------------
void checkRemovedNodeIsolation(const BRepGraph&                                      theGraph,
                               NCollection_Vector<BRepGraphAlgo_Validate::Issue>& theIssues)
{
  using Issue    = BRepGraphAlgo_Validate::Issue;
  using Severity = BRepGraphAlgo_Validate::Severity;

  // Non-removed edges must not reference removed vertices.
  for (int anEdgeIdx = 0; anEdgeIdx < theGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = theGraph.Defs().Edge(anEdgeIdx);
    if (anEdge.IsRemoved)
      continue;

    if (anEdge.StartVertexDefId().IsValid() && isDefRemoved(theGraph, anEdge.StartVertexDefId()))
    {
      theIssues.Append(Issue{Severity::Error,
                             anEdge.Id,
                             "Non-removed EdgeDef references removed StartVertexDef"});
    }
    if (anEdge.EndVertexDefId().IsValid() && isDefRemoved(theGraph, anEdge.EndVertexDefId()))
    {
      theIssues.Append(Issue{Severity::Error,
                             anEdge.Id,
                             "Non-removed EdgeDef references removed EndVertexDef"});
    }
  }

  // Non-removed wires must not reference removed edges.
  for (int aWireIdx = 0; aWireIdx < theGraph.Defs().NbWires(); ++aWireIdx)
  {
    const BRepGraph_TopoNode::WireDef& aWire = theGraph.Defs().Wire(aWireIdx);
    if (aWire.IsRemoved)
      continue;

    for (int aCoEdgeEntryIdx = 0; aCoEdgeEntryIdx < aWire.CoEdgeRefs.Length(); ++aCoEdgeEntryIdx)
    {
      const BRepGraphInc::CoEdgeRef& aCR = aWire.CoEdgeRefs.Value(aCoEdgeEntryIdx);
      const BRepGraph_TopoNode::CoEdgeDef& aCoEdge = theGraph.Defs().CoEdge(aCR.CoEdgeIdx);
      const BRepGraph_NodeId anEdgeId = BRepGraph_NodeId::Edge(aCoEdge.EdgeIdx);
      if (anEdgeId.IsValid() && isDefRemoved(theGraph, anEdgeId))
      {
        theIssues.Append(Issue{Severity::Error,
                               aWire.Id,
                               "Non-removed WireDef references removed EdgeDef"});
        break;
      }
    }
  }
}

// -----------------------------------------------------------------------
// Pass 6: Wire connectivity (order-independent via WireExplorer)
// -----------------------------------------------------------------------
void checkWireConnectivity(const BRepGraph&                                   theGraph,
                           NCollection_Vector<BRepGraphAlgo_Validate::Issue>& theIssues)
{
  using Issue    = BRepGraphAlgo_Validate::Issue;
  using Severity = BRepGraphAlgo_Validate::Severity;

  auto edgeLookup = [&theGraph](int theIdx) -> const BRepGraphInc::EdgeEntity& {
    return theGraph.Defs().Edge(theIdx);
  };

  for (int aWireIdx = 0; aWireIdx < theGraph.Defs().NbWires(); ++aWireIdx)
  {
    const BRepGraph_TopoNode::WireDef& aWire = theGraph.Defs().Wire(aWireIdx);
    if (aWire.IsRemoved)
      continue;

    const int aNbCoEdges = aWire.CoEdgeRefs.Length();
    if (aNbCoEdges < 2)
      continue;

    // Validate all edge indices first.
    bool aAllValid = true;
    for (int anIdx = 0; anIdx < aNbCoEdges; ++anIdx)
    {
      const BRepGraphInc::CoEdgeRef& aCR = aWire.CoEdgeRefs.Value(anIdx);
      const BRepGraph_TopoNode::CoEdgeDef& aCoEdge = theGraph.Defs().CoEdge(aCR.CoEdgeIdx);
      const BRepGraph_NodeId anEdgeId = BRepGraph_NodeId::Edge(aCoEdge.EdgeIdx);
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
    auto coedgeLookup = [&theGraph](int theIdx) -> const BRepGraphInc::CoEdgeEntity& {
      return theGraph.Defs().CoEdge(theIdx);
    };
    BRepGraphInc_WireExplorer anExp(aWire.CoEdgeRefs, coedgeLookup, edgeLookup);
    const NCollection_Vector<BRepGraphInc::CoEdgeRef>& anOrdered = anExp.OrderedRefs();

    for (int anIdx = 0; anIdx < anOrdered.Length() - 1; ++anIdx)
    {
      const BRepGraphInc::CoEdgeRef& aCurrCR = anOrdered.Value(anIdx);
      const BRepGraphInc::CoEdgeRef& aNextCR = anOrdered.Value(anIdx + 1);

      const BRepGraph_TopoNode::CoEdgeDef& aCurrCoEdge = theGraph.Defs().CoEdge(aCurrCR.CoEdgeIdx);
      const BRepGraph_TopoNode::CoEdgeDef& aNextCoEdge = theGraph.Defs().CoEdge(aNextCR.CoEdgeIdx);

      const BRepGraph_TopoNode::EdgeDef& aCurrEdge = theGraph.Defs().Edge(aCurrCoEdge.EdgeIdx);
      const BRepGraph_TopoNode::EdgeDef& aNextEdge = theGraph.Defs().Edge(aNextCoEdge.EdgeIdx);

      const BRepGraph_NodeId aCurrEnd   = aCurrEdge.OrientedEndVertex(aCurrCoEdge.Sense);
      const BRepGraph_NodeId aNextStart = aNextEdge.OrientedStartVertex(aNextCoEdge.Sense);

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

// ---------------------------------------------------------------------------
// Pass 7: Validate entity IDs match their vector position.
// After Build() or Compact(), each entity's Id must equal (Kind, VectorIndex).
// ---------------------------------------------------------------------------

void checkEntityIds(const BRepGraph& theGraph,
                    NCollection_Vector<BRepGraphAlgo_Validate::Issue>& theIssues)
{
  using Severity = BRepGraphAlgo_Validate::Severity;
  using Issue    = BRepGraphAlgo_Validate::Issue;
  const BRepGraph::DefsView aDefs = theGraph.Defs();

  auto checkKind = [&](BRepGraph_NodeId::Kind theKind, int theNb) {
    for (int anIdx = 0; anIdx < theNb; ++anIdx)
    {
      const BRepGraph_NodeId anExpected(theKind, anIdx);
      const BRepGraph_TopoNode::BaseDef* aDef = aDefs.TopoDef(anExpected);
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

  checkKind(BRepGraph_NodeId::Kind::Vertex,    aDefs.NbVertices());
  checkKind(BRepGraph_NodeId::Kind::Edge,      aDefs.NbEdges());
  checkKind(BRepGraph_NodeId::Kind::Wire,      aDefs.NbWires());
  checkKind(BRepGraph_NodeId::Kind::Face,      aDefs.NbFaces());
  checkKind(BRepGraph_NodeId::Kind::Shell,     aDefs.NbShells());
  checkKind(BRepGraph_NodeId::Kind::Solid,     aDefs.NbSolids());
  checkKind(BRepGraph_NodeId::Kind::Compound,  aDefs.NbCompounds());
  checkKind(BRepGraph_NodeId::Kind::CompSolid, aDefs.NbCompSolids());
}

// ---------------------------------------------------------------------------
// Pass 8: Validate active counts match non-removed entity counts.
// ---------------------------------------------------------------------------

void checkActiveCounts(const BRepGraph& theGraph,
                       NCollection_Vector<BRepGraphAlgo_Validate::Issue>& theIssues)
{
  using Severity = BRepGraphAlgo_Validate::Severity;
  using Issue    = BRepGraphAlgo_Validate::Issue;
  const BRepGraph::DefsView aDefs = theGraph.Defs();

  auto countActive = [&](BRepGraph_NodeId::Kind theKind, int theNb) -> int {
    int aCount = 0;
    for (int anIdx = 0; anIdx < theNb; ++anIdx)
    {
      const BRepGraph_NodeId anId(theKind, anIdx);
      if (!isDefRemoved(theGraph, anId))
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

  verify("Vertices",   aDefs.NbActiveVertices(),   countActive(BRepGraph_NodeId::Kind::Vertex,    aDefs.NbVertices()));
  verify("Edges",      aDefs.NbActiveEdges(),      countActive(BRepGraph_NodeId::Kind::Edge,      aDefs.NbEdges()));
  verify("Wires",      aDefs.NbActiveWires(),      countActive(BRepGraph_NodeId::Kind::Wire,      aDefs.NbWires()));
  verify("Faces",      aDefs.NbActiveFaces(),      countActive(BRepGraph_NodeId::Kind::Face,      aDefs.NbFaces()));
  verify("Shells",     aDefs.NbActiveShells(),      countActive(BRepGraph_NodeId::Kind::Shell,     aDefs.NbShells()));
  verify("Solids",     aDefs.NbActiveSolids(),      countActive(BRepGraph_NodeId::Kind::Solid,     aDefs.NbSolids()));
  verify("Compounds",  aDefs.NbActiveCompounds(),   countActive(BRepGraph_NodeId::Kind::Compound,  aDefs.NbCompounds()));
  verify("CompSolids", aDefs.NbActiveCompSolids(),  countActive(BRepGraph_NodeId::Kind::CompSolid, aDefs.NbCompSolids()));
}

} // namespace

//=================================================================================================

BRepGraphAlgo_Validate::Result BRepGraphAlgo_Validate::Perform(const BRepGraph& theGraph)
{
  Result aResult;
  if (!theGraph.IsDone())
  {
    return aResult;
  }

  checkCrossReferenceBounds(theGraph, aResult.Issues);
  checkReverseIndexConsistency(theGraph, aResult.Issues);
  checkIncidenceRefConsistency(theGraph, aResult.Issues);
  checkGeometryReferences(theGraph, aResult.Issues);
  checkRemovedNodeIsolation(theGraph, aResult.Issues);
  checkWireConnectivity(theGraph, aResult.Issues);
  checkEntityIds(theGraph, aResult.Issues);
  checkActiveCounts(theGraph, aResult.Issues);

  return aResult;
}
