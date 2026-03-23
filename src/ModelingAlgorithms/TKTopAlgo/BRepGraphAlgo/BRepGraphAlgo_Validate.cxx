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
#include <BRepGraph_GeomView.hxx>
#include <BRepGraph_RelEdgesView.hxx>
#include <BRepGraph_UsagesView.hxx>

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
    case BRepGraph_NodeId::Kind::Solid:     return theId.Index < theGraph.Defs().NbSolids();
    case BRepGraph_NodeId::Kind::Shell:     return theId.Index < theGraph.Defs().NbShells();
    case BRepGraph_NodeId::Kind::Face:      return theId.Index < theGraph.Defs().NbFaces();
    case BRepGraph_NodeId::Kind::Wire:      return theId.Index < theGraph.Defs().NbWires();
    case BRepGraph_NodeId::Kind::Edge:      return theId.Index < theGraph.Defs().NbEdges();
    case BRepGraph_NodeId::Kind::Vertex:    return theId.Index < theGraph.Defs().NbVertices();
    case BRepGraph_NodeId::Kind::Compound:  return theId.Index < theGraph.Defs().NbCompounds();
    case BRepGraph_NodeId::Kind::CompSolid: return theId.Index < theGraph.Defs().NbCompSolids();
    case BRepGraph_NodeId::Kind::Surface:   return theId.Index < theGraph.Geom().NbSurfaces();
    case BRepGraph_NodeId::Kind::Curve:     return theId.Index < theGraph.Geom().NbCurves();
    case BRepGraph_NodeId::Kind::PCurve:    return theId.Index < theGraph.Geom().NbPCurves();
  }
  return false;
}

//! Check that a UsageId refers to a valid index within graph bounds.
bool isValidUsageId(const BRepGraph& theGraph, BRepGraph_UsageId theId)
{
  if (!theId.IsValid())
    return false;

  switch (theId.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Solid:     return theId.Index < theGraph.Usages().NbSolids();
    case BRepGraph_NodeId::Kind::Shell:     return theId.Index < theGraph.Usages().NbShells();
    case BRepGraph_NodeId::Kind::Face:      return theId.Index < theGraph.Usages().NbFaces();
    case BRepGraph_NodeId::Kind::Wire:      return theId.Index < theGraph.Usages().NbWires();
    case BRepGraph_NodeId::Kind::Edge:      return theId.Index < theGraph.Usages().NbEdges();
    case BRepGraph_NodeId::Kind::Vertex:    return theId.Index < theGraph.Usages().NbVertices();
    case BRepGraph_NodeId::Kind::Compound:  return theId.Index < theGraph.Usages().NbCompounds();
    case BRepGraph_NodeId::Kind::CompSolid: return theId.Index < theGraph.Usages().NbCompSolids();
    default: break;
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

    if (anEdge.CurveNodeId.IsValid() && !isValidNodeId(theGraph, anEdge.CurveNodeId))
    {
      theIssues.Append(Issue{Severity::Error,
                             anEdge.Id,
                             "EdgeDef.CurveNodeId out of bounds"});
    }
    if (anEdge.StartVertexDefId.IsValid() && !isValidNodeId(theGraph, anEdge.StartVertexDefId))
    {
      theIssues.Append(Issue{Severity::Error,
                             anEdge.Id,
                             "EdgeDef.StartVertexDefId out of bounds"});
    }
    if (anEdge.EndVertexDefId.IsValid() && !isValidNodeId(theGraph, anEdge.EndVertexDefId))
    {
      theIssues.Append(Issue{Severity::Error,
                             anEdge.Id,
                             "EdgeDef.EndVertexDefId out of bounds"});
    }
    for (int aPCIdx = 0; aPCIdx < anEdge.PCurves.Length(); ++aPCIdx)
    {
      const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry = anEdge.PCurves.Value(aPCIdx);
      if (aPCEntry.PCurveNodeId.IsValid() && !isValidNodeId(theGraph, aPCEntry.PCurveNodeId))
      {
        theIssues.Append(Issue{Severity::Error,
                               anEdge.Id,
                               "EdgeDef.PCurveEntry.PCurveNodeId out of bounds"});
      }
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

    if (aFace.SurfNodeId.IsValid() && !isValidNodeId(theGraph, aFace.SurfNodeId))
    {
      theIssues.Append(Issue{Severity::Error,
                             aFace.Id,
                             "FaceDef.SurfNodeId out of bounds"});
    }
  }

  // Check WireDef references.
  for (int aWireIdx = 0; aWireIdx < theGraph.Defs().NbWires(); ++aWireIdx)
  {
    const BRepGraph_TopoNode::WireDef& aWire = theGraph.Defs().Wire(aWireIdx);
    if (aWire.IsRemoved)
      continue;

    for (int anEdgeEntryIdx = 0; anEdgeEntryIdx < aWire.OrderedEdges.Length(); ++anEdgeEntryIdx)
    {
      const BRepGraph_TopoNode::WireDef::EdgeEntry& anEntry =
        aWire.OrderedEdges.Value(anEdgeEntryIdx);
      if (anEntry.EdgeDefId.IsValid() && !isValidNodeId(theGraph, anEntry.EdgeDefId))
      {
        theIssues.Append(Issue{Severity::Error,
                               aWire.Id,
                               "WireDef.OrderedEdges entry EdgeDefId out of bounds"});
      }
    }
  }

  // Check CompoundDef references.
  for (int aCompIdx = 0; aCompIdx < theGraph.Defs().NbCompounds(); ++aCompIdx)
  {
    const BRepGraph_TopoNode::CompoundDef& aComp = theGraph.Defs().Compound(aCompIdx);
    if (aComp.IsRemoved)
      continue;

    for (int aChildIdx = 0; aChildIdx < aComp.ChildDefIds.Length(); ++aChildIdx)
    {
      const BRepGraph_NodeId& aChildId = aComp.ChildDefIds.Value(aChildIdx);
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

    for (int aSolidIdx = 0; aSolidIdx < aCS.SolidDefIds.Length(); ++aSolidIdx)
    {
      const BRepGraph_NodeId& aSolidId = aCS.SolidDefIds.Value(aSolidIdx);
      if (aSolidId.IsValid() && !isValidNodeId(theGraph, aSolidId))
      {
        theIssues.Append(Issue{Severity::Error,
                               aCS.Id,
                               "CompSolidDef.SolidDefId out of bounds"});
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

  // Build expected edge->wires mapping from WireDefs.
  NCollection_DataMap<int, NCollection_Map<int>> anExpected;
  for (int aWireIdx = 0; aWireIdx < theGraph.Defs().NbWires(); ++aWireIdx)
  {
    const BRepGraph_TopoNode::WireDef& aWire = theGraph.Defs().Wire(aWireIdx);
    if (aWire.IsRemoved)
      continue;

    for (int anEntryIdx = 0; anEntryIdx < aWire.OrderedEdges.Length(); ++anEntryIdx)
    {
      const BRepGraph_NodeId& anEdgeId = aWire.OrderedEdges.Value(anEntryIdx).EdgeDefId;
      if (!anEdgeId.IsValid())
        continue;

      if (!anExpected.IsBound(anEdgeId.Index))
        anExpected.Bind(anEdgeId.Index, NCollection_Map<int>());
      anExpected.ChangeFind(anEdgeId.Index).Add(aWireIdx);
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
                             "Reverse index myEdgeToWires size mismatch"});
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
                                 "Reverse index myEdgeToWires missing wire entry"});
          break;
        }
      }
    }
  }
}

// -----------------------------------------------------------------------
// Pass 3: Usage <-> Def consistency
// -----------------------------------------------------------------------
void checkUsageDefConsistency(const BRepGraph&                                      theGraph,
                              NCollection_Vector<BRepGraphAlgo_Validate::Issue>& theIssues)
{
  using Issue    = BRepGraphAlgo_Validate::Issue;
  using Severity = BRepGraphAlgo_Validate::Severity;

  // For each face usage, check DefId points back correctly.
  for (int aFaceUsIdx = 0; aFaceUsIdx < theGraph.Usages().NbFaces(); ++aFaceUsIdx)
  {
    const BRepGraph_TopoNode::FaceUsage& aFaceUs = theGraph.Usages().Face(aFaceUsIdx);
    if (!aFaceUs.DefId.IsValid() || !isValidNodeId(theGraph, aFaceUs.DefId))
    {
      theIssues.Append(Issue{Severity::Error,
                             BRepGraph_NodeId::Face(aFaceUsIdx),
                             "FaceUsage.DefId invalid or out of bounds"});
      continue;
    }

    const BRepGraph_TopoNode::FaceDef& aFaceDef =
      theGraph.Defs().Face(aFaceUs.DefId.Index);

    bool aFound = false;
    for (int anUsIdx = 0; anUsIdx < aFaceDef.Usages.Length(); ++anUsIdx)
    {
      if (aFaceDef.Usages.Value(anUsIdx) == aFaceUs.UsageId)
      {
        aFound = true;
        break;
      }
    }
    if (!aFound)
    {
      theIssues.Append(Issue{Severity::Error,
                             aFaceDef.Id,
                             "FaceDef.Usages does not contain its FaceUsage"});
    }
  }

  // For each edge usage.
  for (int anEdgeUsIdx = 0; anEdgeUsIdx < theGraph.Usages().NbEdges(); ++anEdgeUsIdx)
  {
    const BRepGraph_TopoNode::EdgeUsage& anEdgeUs = theGraph.Usages().Edge(anEdgeUsIdx);
    if (!anEdgeUs.DefId.IsValid() || !isValidNodeId(theGraph, anEdgeUs.DefId))
    {
      theIssues.Append(Issue{Severity::Error,
                             BRepGraph_NodeId::Edge(anEdgeUsIdx),
                             "EdgeUsage.DefId invalid or out of bounds"});
      continue;
    }

    const BRepGraph_TopoNode::EdgeDef& anEdgeDef =
      theGraph.Defs().Edge(anEdgeUs.DefId.Index);

    bool aFound = false;
    for (int anUsIdx = 0; anUsIdx < anEdgeDef.Usages.Length(); ++anUsIdx)
    {
      if (anEdgeDef.Usages.Value(anUsIdx) == anEdgeUs.UsageId)
      {
        aFound = true;
        break;
      }
    }
    if (!aFound)
    {
      theIssues.Append(Issue{Severity::Error,
                             anEdgeDef.Id,
                             "EdgeDef.Usages does not contain its EdgeUsage"});
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

  // Check face->surface references.
  for (int aFaceIdx = 0; aFaceIdx < theGraph.Defs().NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aFace = theGraph.Defs().Face(aFaceIdx);
    if (aFace.IsRemoved)
      continue;

    if (aFace.SurfNodeId.IsValid())
    {
      if (!isValidNodeId(theGraph, aFace.SurfNodeId))
      {
        theIssues.Append(Issue{Severity::Error,
                               aFace.Id,
                               "FaceDef.SurfNodeId index out of bounds"});
      }
      else if (theGraph.Geom().Surface(aFace.SurfNodeId.Index).Surface.IsNull())
      {
        theIssues.Append(Issue{Severity::Error,
                               aFace.Id,
                               "FaceDef references null Surface handle"});
      }
    }
  }

  // Check edge->curve references.
  for (int anEdgeIdx = 0; anEdgeIdx < theGraph.Defs().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = theGraph.Defs().Edge(anEdgeIdx);
    if (anEdge.IsRemoved)
      continue;

    if (anEdge.CurveNodeId.IsValid())
    {
      if (!isValidNodeId(theGraph, anEdge.CurveNodeId))
      {
        theIssues.Append(Issue{Severity::Error,
                               anEdge.Id,
                               "EdgeDef.CurveNodeId index out of bounds"});
      }
      else if (!anEdge.IsDegenerate
               && theGraph.Geom().Curve(anEdge.CurveNodeId.Index).CurveGeom.IsNull())
      {
        theIssues.Append(Issue{Severity::Error,
                               anEdge.Id,
                               "Non-degenerate EdgeDef references null Curve handle"});
      }
    }

    // Check PCurve references.
    for (int aPCIdx = 0; aPCIdx < anEdge.PCurves.Length(); ++aPCIdx)
    {
      const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry = anEdge.PCurves.Value(aPCIdx);
      if (aPCEntry.PCurveNodeId.IsValid())
      {
        if (!isValidNodeId(theGraph, aPCEntry.PCurveNodeId))
        {
          theIssues.Append(Issue{Severity::Error,
                                 anEdge.Id,
                                 "EdgeDef.PCurveEntry.PCurveNodeId out of bounds"});
        }
        else if (theGraph.Geom().PCurve(aPCEntry.PCurveNodeId.Index).Curve2d.IsNull())
        {
          theIssues.Append(Issue{Severity::Error,
                                 anEdge.Id,
                                 "EdgeDef.PCurveEntry references null Curve2d handle"});
        }
      }
    }
  }

  // Check surface back-references.
  for (int aSurfIdx = 0; aSurfIdx < theGraph.Geom().NbSurfaces(); ++aSurfIdx)
  {
    const BRepGraph_GeomNode::Surf& aSurf = theGraph.Geom().Surface(aSurfIdx);
    for (int anUserIdx = 0; anUserIdx < aSurf.FaceDefUsers.Length(); ++anUserIdx)
    {
      const BRepGraph_NodeId& aFaceId = aSurf.FaceDefUsers.Value(anUserIdx);
      if (!isValidNodeId(theGraph, aFaceId))
      {
        theIssues.Append(Issue{Severity::Error,
                               aSurf.Id,
                               "Surf.FaceDefUsers contains out-of-bounds FaceDefId"});
        continue;
      }
      const BRepGraph_TopoNode::FaceDef& aFace = theGraph.Defs().Face(aFaceId.Index);
      if (aFace.SurfNodeId.Index != aSurfIdx)
      {
        theIssues.Append(Issue{Severity::Error,
                               aSurf.Id,
                               "Surf.FaceDefUsers entry does not reference this surface"});
      }
    }
  }

  // Check curve back-references.
  for (int aCurveIdx = 0; aCurveIdx < theGraph.Geom().NbCurves(); ++aCurveIdx)
  {
    const BRepGraph_GeomNode::Curve& aCurve = theGraph.Geom().Curve(aCurveIdx);
    for (int anUserIdx = 0; anUserIdx < aCurve.EdgeDefUsers.Length(); ++anUserIdx)
    {
      const BRepGraph_NodeId& anEdgeId = aCurve.EdgeDefUsers.Value(anUserIdx);
      if (!isValidNodeId(theGraph, anEdgeId))
      {
        theIssues.Append(Issue{Severity::Error,
                               aCurve.Id,
                               "Curve.EdgeDefUsers contains out-of-bounds EdgeDefId"});
        continue;
      }
      const BRepGraph_TopoNode::EdgeDef& anEdge = theGraph.Defs().Edge(anEdgeId.Index);
      if (anEdge.CurveNodeId.Index != aCurveIdx)
      {
        theIssues.Append(Issue{Severity::Error,
                               aCurve.Id,
                               "Curve.EdgeDefUsers entry does not reference this curve"});
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

    if (anEdge.StartVertexDefId.IsValid() && isDefRemoved(theGraph, anEdge.StartVertexDefId))
    {
      theIssues.Append(Issue{Severity::Error,
                             anEdge.Id,
                             "Non-removed EdgeDef references removed StartVertexDef"});
    }
    if (anEdge.EndVertexDefId.IsValid() && isDefRemoved(theGraph, anEdge.EndVertexDefId))
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

    for (int anEntryIdx = 0; anEntryIdx < aWire.OrderedEdges.Length(); ++anEntryIdx)
    {
      const BRepGraph_NodeId& anEdgeId = aWire.OrderedEdges.Value(anEntryIdx).EdgeDefId;
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
// Pass 6: Wire connectivity
// -----------------------------------------------------------------------
void checkWireConnectivity(const BRepGraph&                                      theGraph,
                           NCollection_Vector<BRepGraphAlgo_Validate::Issue>& theIssues)
{
  using Issue    = BRepGraphAlgo_Validate::Issue;
  using Severity = BRepGraphAlgo_Validate::Severity;

  for (int aWireIdx = 0; aWireIdx < theGraph.Defs().NbWires(); ++aWireIdx)
  {
    const BRepGraph_TopoNode::WireDef& aWire = theGraph.Defs().Wire(aWireIdx);
    if (aWire.IsRemoved || aWire.OrderedEdges.Length() < 2)
      continue;

    for (int anIdx = 0; anIdx < aWire.OrderedEdges.Length() - 1; ++anIdx)
    {
      const BRepGraph_TopoNode::WireDef::EdgeEntry& aCurr = aWire.OrderedEdges.Value(anIdx);
      const BRepGraph_TopoNode::WireDef::EdgeEntry& aNext = aWire.OrderedEdges.Value(anIdx + 1);

      if (!aCurr.EdgeDefId.IsValid() || !aNext.EdgeDefId.IsValid())
        continue;
      if (!isValidNodeId(theGraph, aCurr.EdgeDefId) || !isValidNodeId(theGraph, aNext.EdgeDefId))
        continue;

      const BRepGraph_TopoNode::EdgeDef& aCurrEdge = theGraph.Defs().Edge(aCurr.EdgeDefId.Index);
      const BRepGraph_TopoNode::EdgeDef& aNextEdge = theGraph.Defs().Edge(aNext.EdgeDefId.Index);

      const BRepGraph_NodeId aCurrEnd   = aCurrEdge.OrientedEndVertex(aCurr.OrientationInWire);
      const BRepGraph_NodeId aNextStart = aNextEdge.OrientedStartVertex(aNext.OrientationInWire);

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

// -----------------------------------------------------------------------
// Pass 7: Parent chain acyclicity
// -----------------------------------------------------------------------
void checkParentChainAcyclicity(const BRepGraph&                                      theGraph,
                                NCollection_Vector<BRepGraphAlgo_Validate::Issue>& theIssues)
{
  using Issue    = BRepGraphAlgo_Validate::Issue;
  using Severity = BRepGraphAlgo_Validate::Severity;

  // Check face usages as a representative sample.
  const int aTotalUsages = theGraph.Usages().NbFaces()
                         + theGraph.Usages().NbEdges()
                         + theGraph.Usages().NbVertices()
                         + theGraph.Usages().NbWires()
                         + theGraph.Usages().NbShells()
                         + theGraph.Usages().NbSolids()
                         + theGraph.Usages().NbCompounds()
                         + theGraph.Usages().NbCompSolids();

  // Check face usage parent chains.
  for (int aFaceUsIdx = 0; aFaceUsIdx < theGraph.Usages().NbFaces(); ++aFaceUsIdx)
  {
    BRepGraph_UsageId aCurrent = theGraph.Usages().Face(aFaceUsIdx).ParentUsage;
    int aSteps = 0;
    while (aCurrent.IsValid() && aSteps < aTotalUsages)
    {
      if (!isValidUsageId(theGraph, aCurrent))
        break;
      BRepGraph_UsageId aParent = theGraph.DefOf(aCurrent).IsValid()
                                    ? BRepGraph_UsageId() // simplified: stop at invalid
                                    : BRepGraph_UsageId();
      // Walk up parent usage chain via the usage's own ParentUsage.
      // We need to access the usage's ParentUsage field.
      // Since UsageId can be of any kind, we need kind-based dispatch.
      switch (aCurrent.NodeKind)
      {
        case BRepGraph_NodeId::Kind::Shell:
          aParent = theGraph.Usages().Shell(aCurrent.Index).ParentUsage;
          break;
        case BRepGraph_NodeId::Kind::Solid:
          aParent = theGraph.Usages().Solid(aCurrent.Index).ParentUsage;
          break;
        case BRepGraph_NodeId::Kind::Compound:
          aParent = theGraph.Usages().Compound(aCurrent.Index).ParentUsage;
          break;
        case BRepGraph_NodeId::Kind::CompSolid:
          aParent = theGraph.Usages().CompSolid(aCurrent.Index).ParentUsage;
          break;
        default:
          aParent = BRepGraph_UsageId(); // stop
          break;
      }
      aCurrent = aParent;
      ++aSteps;
    }

    if (aSteps >= aTotalUsages)
    {
      theIssues.Append(Issue{Severity::Error,
                             BRepGraph_NodeId::Face(aFaceUsIdx),
                             "FaceUsage parent chain exceeds total usage count (cycle?)"});
    }
  }
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
  checkUsageDefConsistency(theGraph, aResult.Issues);
  checkGeometryReferences(theGraph, aResult.Issues);
  checkRemovedNodeIsolation(theGraph, aResult.Issues);
  checkWireConnectivity(theGraph, aResult.Issues);
  checkParentChainAcyclicity(theGraph, aResult.Issues);

  return aResult;
}
