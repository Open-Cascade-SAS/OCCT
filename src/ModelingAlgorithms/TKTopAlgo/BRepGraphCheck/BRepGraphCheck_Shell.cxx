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
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>

#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Tool.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_Vector.hxx>

namespace
{

//=================================================================================================

static NCollection_Vector<BRepGraphInc::FaceRef> collectShellFaceRefs(
  const BRepGraph::TopoView& theDefs,
  const BRepGraph::RefsView& theRefs,
  const BRepGraph_NodeId     theShellNodeId)
{
  NCollection_Vector<BRepGraphInc::FaceRef> aResult;
  const BRepGraphInc::ShellDef&             aShellDef =
    theDefs.Shells().Definition(BRepGraph_ShellId(theShellNodeId.Index));
  for (int aRefIter = 0; aRefIter < aShellDef.FaceRefIds.Length(); ++aRefIter)
  {
    const BRepGraph_FaceRefId    aRefId = aShellDef.FaceRefIds.Value(aRefIter);
    const BRepGraphInc::FaceRef& aRef   = theRefs.Faces().Entry(aRefId);
    if (aRef.IsRemoved || !aRef.FaceDefId.IsValid(theDefs.Faces().Nb()))
    {
      continue;
    }
    aResult.Append(aRef);
  }
  return aResult;
}

//=================================================================================================

static NCollection_Vector<BRepGraphInc::WireRef> collectFaceWireRefs(
  const BRepGraph::TopoView& theDefs,
  const BRepGraph::RefsView& theRefs,
  const BRepGraph_NodeId     theFaceNodeId)
{
  NCollection_Vector<BRepGraphInc::WireRef> aResult;
  const BRepGraphInc::FaceDef&              aFaceDef =
    theDefs.Faces().Definition(BRepGraph_FaceId(theFaceNodeId.Index));
  for (int aRefIter = 0; aRefIter < aFaceDef.WireRefIds.Length(); ++aRefIter)
  {
    const BRepGraph_WireRefId    aRefId = aFaceDef.WireRefIds.Value(aRefIter);
    const BRepGraphInc::WireRef& aRef   = theRefs.Wires().Entry(aRefId);
    if (aRef.IsRemoved || !aRef.WireDefId.IsValid(theDefs.Wires().Nb()))
    {
      continue;
    }
    aResult.Append(aRef);
  }
  return aResult;
}

//=================================================================================================

static NCollection_Vector<BRepGraphInc::CoEdgeUsage> collectWireCoEdgeRefs(
  const BRepGraph::TopoView& theDefs,
  const BRepGraph::RefsView& theRefs,
  const BRepGraph_NodeId     theWireNodeId)
{
  NCollection_Vector<BRepGraphInc::CoEdgeUsage> aResult;
  const BRepGraphInc::WireDef&                  aWireDef =
    theDefs.Wires().Definition(BRepGraph_WireId(theWireNodeId.Index));
  for (int aRefIter = 0; aRefIter < aWireDef.CoEdgeRefIds.Length(); ++aRefIter)
  {
    const BRepGraph_CoEdgeRefId    aRefId = aWireDef.CoEdgeRefIds.Value(aRefIter);
    const BRepGraphInc::CoEdgeRef& aRef   = theRefs.CoEdges().Entry(aRefId);
    if (aRef.IsRemoved || !aRef.CoEdgeDefId.IsValid(theDefs.CoEdges().Nb()))
    {
      continue;
    }

    BRepGraphInc::CoEdgeUsage aCoEdgeRef;
    aCoEdgeRef.DefId    = aRef.CoEdgeDefId;
    aCoEdgeRef.Location = aRef.LocalLocation;
    aResult.Append(aCoEdgeRef);
  }
  return aResult;
}

} // namespace

//=================================================================================================

void BRepGraphCheck::CheckShellMinimum(const BRepGraph&                          theGraph,
                                       const BRepGraph_ShellId                   theShell,
                                       NCollection_Vector<BRepGraphCheck_Issue>& theIssues)
{
  const BRepGraph::TopoView&                      aDefs     = theGraph.Topo();
  const BRepGraph::RefsView&                      aRefs     = theGraph.Refs();
  const BRepGraphInc::ShellDef&                   aShellDef = aDefs.Shells().Definition(theShell);
  const BRepGraph_NodeId                          aShellNodeId = aShellDef.Id;
  const NCollection_Vector<BRepGraphInc::FaceRef> aShellFaceRefs =
    collectShellFaceRefs(aDefs, aRefs, aShellNodeId);

  // Shell must have at least one face.
  if (aShellFaceRefs.IsEmpty())
  {
    BRepGraphCheck_Issue anIssue;
    anIssue.NodeId        = aShellDef.Id;
    anIssue.Status        = BRepCheck_EmptyShell;
    anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
    theIssues.Append(anIssue);
    return;
  }

  // Connectivity check via BFS using face adjacency.
  NCollection_Map<int> aShellFaces;
  for (int aFaceIter = 0; aFaceIter < aShellFaceRefs.Length(); ++aFaceIter)
  {
    const BRepGraphInc::FaceRef& aFR = aShellFaceRefs.Value(aFaceIter);
    aShellFaces.Add(aFR.FaceDefId.Index);
  }

  if (aShellFaces.Size() <= 1)
    return;

  // BFS from the first face.
  const BRepGraph::TopoView& aSpatial = theGraph.Topo();
  NCollection_Map<int>       aVisited;
  NCollection_Vector<int>    aQueue;

  const int aStartFace = NCollection_Map<int>::Iterator(aShellFaces).Value();
  aVisited.Add(aStartFace);
  aQueue.Append(aStartFace);

  int aFront = 0;
  while (aFront < aQueue.Length())
  {
    const int                                  aCurrFaceIdx = aQueue.Value(aFront++);
    const BRepGraph_FaceId                     aCurrFaceId(aCurrFaceIdx);
    const NCollection_Vector<BRepGraph_FaceId> anAdjFaces =
      aSpatial.Faces().Adjacent(aCurrFaceId, theGraph.Allocator());

    for (int anAdjIter = 0; anAdjIter < anAdjFaces.Length(); ++anAdjIter)
    {
      const int anAdjIdx = anAdjFaces.Value(anAdjIter).Index;
      if (aShellFaces.Contains(anAdjIdx) && aVisited.Add(anAdjIdx))
      {
        aQueue.Append(anAdjIdx);
      }
    }
  }

  if (aVisited.Size() < aShellFaces.Size())
  {
    BRepGraphCheck_Issue anIssue;
    anIssue.NodeId        = aDefs.Shells().Definition(theShell).Id;
    anIssue.Status        = BRepCheck_NotConnected;
    anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
    theIssues.Append(anIssue);
  }
}

//=================================================================================================

void BRepGraphCheck::CheckShellClosed(const BRepGraph&                          theGraph,
                                      const BRepGraph_ShellId                   theShell,
                                      NCollection_Vector<BRepGraphCheck_Issue>& theIssues)
{
  const BRepGraph::TopoView&                      aDefs     = theGraph.Topo();
  const BRepGraph::RefsView&                      aRefs     = theGraph.Refs();
  const BRepGraphInc::ShellDef&                   aShellDef = aDefs.Shells().Definition(theShell);
  const BRepGraph_NodeId                          aShellNodeId = aShellDef.Id;
  const NCollection_Vector<BRepGraphInc::FaceRef> aShellFaceRefs =
    collectShellFaceRefs(aDefs, aRefs, aShellNodeId);

  if (aShellFaceRefs.IsEmpty())
    return;

  // Collect all face definition indices belonging to this shell.
  NCollection_Map<int> aShellFaces;
  for (int aFaceIter = 0; aFaceIter < aShellFaceRefs.Length(); ++aFaceIter)
  {
    const BRepGraphInc::FaceRef& aFR        = aShellFaceRefs.Value(aFaceIter);
    const BRepGraph_NodeId       aFaceDefId = aFR.FaceDefId;
    if (!aShellFaces.Add(aFR.FaceDefId.Index))
    {
      // Redundant face: same face appears twice in the shell.
      BRepGraphCheck_Issue anIssue;
      anIssue.NodeId        = aFaceDefId;
      anIssue.ContextNodeId = aShellNodeId;
      anIssue.Status        = BRepCheck_RedundantFace;
      anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
      theIssues.Append(anIssue);
    }
  }

  // For each edge in the shell, count how many faces of this shell reference it.
  // An edge shared by exactly 2 faces is manifold; free edges indicate open shell.
  NCollection_DataMap<int, int> anEdgeFaceCounts;

  for (NCollection_Map<int>::Iterator aFaceIter(aShellFaces); aFaceIter.More(); aFaceIter.Next())
  {
    const int              aFaceIdx = aFaceIter.Value();
    const BRepGraph_NodeId aFaceNodeId(BRepGraph_NodeId::Kind::Face, aFaceIdx);
    const NCollection_Vector<BRepGraphInc::WireRef> aFaceWireRefs =
      collectFaceWireRefs(aDefs, aRefs, aFaceNodeId);

    // Collect all edges from all wires of this face.
    for (int aWireIter = 0; aWireIter < aFaceWireRefs.Length(); ++aWireIter)
    {
      const BRepGraphInc::WireRef& aWR         = aFaceWireRefs.Value(aWireIter);
      const BRepGraph_NodeId       aWireNodeId = BRepGraph_WireId(aWR.WireDefId.Index);
      const NCollection_Vector<BRepGraphInc::CoEdgeUsage> aWireCoEdgeRefs =
        collectWireCoEdgeRefs(aDefs, aRefs, aWireNodeId);

      for (int aCoEdgeIter = 0; aCoEdgeIter < aWireCoEdgeRefs.Length(); ++aCoEdgeIter)
      {
        const BRepGraphInc::CoEdgeUsage& aCR        = aWireCoEdgeRefs.Value(aCoEdgeIter);
        const BRepGraphInc::CoEdgeDef&   aCoEdgeDef = aDefs.CoEdges().Definition(aCR.DefId);
        const int                        anEdgeIdx  = aCoEdgeDef.EdgeDefId.Index;

        // Skip degenerate edges.
        if (BRepGraph_Tool::Edge::Degenerated(theGraph, BRepGraph_EdgeId(anEdgeIdx)))
          continue;

        if (!anEdgeFaceCounts.IsBound(anEdgeIdx))
          anEdgeFaceCounts.Bind(anEdgeIdx, 1);
        else
          anEdgeFaceCounts.ChangeFind(anEdgeIdx)++;
      }
    }
  }

  // Report free edges (referenced by only 1 face in this shell) and multi-connexity.
  bool aHasFreeEdge = false;
  for (NCollection_DataMap<int, int>::Iterator anEdgeIter(anEdgeFaceCounts); anEdgeIter.More();
       anEdgeIter.Next())
  {
    BRepGraphCheck::CheckEdgeInShell(theGraph,
                                     BRepGraph_EdgeId(anEdgeIter.Key()),
                                     theShell,
                                     anEdgeIter.Value(),
                                     theIssues);
    if (anEdgeIter.Value() == 1)
    {
      aHasFreeEdge = true;
    }
  }

  if (aHasFreeEdge)
  {
    BRepGraphCheck_Issue anIssue;
    anIssue.NodeId        = aShellNodeId;
    anIssue.Status        = BRepCheck_NotClosed;
    anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Warning;
    theIssues.Append(anIssue);
  }
}

//=================================================================================================

void BRepGraphCheck::CheckShellOrientation(const BRepGraph&                          theGraph,
                                           const BRepGraph_ShellId                   theShell,
                                           NCollection_Vector<BRepGraphCheck_Issue>& theIssues)
{
  const BRepGraph::TopoView&                      aDefs     = theGraph.Topo();
  const BRepGraph::RefsView&                      aRefs     = theGraph.Refs();
  const BRepGraphInc::ShellDef&                   aShellDef = aDefs.Shells().Definition(theShell);
  const BRepGraph_NodeId                          aShellNodeId = aShellDef.Id;
  const NCollection_Vector<BRepGraphInc::FaceRef> aShellFaceRefs =
    collectShellFaceRefs(aDefs, aRefs, aShellNodeId);

  if (aShellFaceRefs.Length() < 2)
    return;

  // Build a map: edge def index -> list of (face def index, orientation in wire).
  struct EdgeInFace
  {
    int                FaceDefIdx;
    TopAbs_Orientation OrientationInWire;
  };

  NCollection_DataMap<int, NCollection_Vector<EdgeInFace>> anEdgeUsageMap;

  for (int aFaceIter = 0; aFaceIter < aShellFaceRefs.Length(); ++aFaceIter)
  {
    const BRepGraphInc::FaceRef& aFR      = aShellFaceRefs.Value(aFaceIter);
    const int                    aFaceIdx = aFR.FaceDefId.Index;
    const TopAbs_Orientation     aFaceOri = aFR.Orientation;
    const BRepGraph_NodeId       aFaceNodeId(BRepGraph_NodeId::Kind::Face, aFaceIdx);
    const NCollection_Vector<BRepGraphInc::WireRef> aFaceWireRefs =
      collectFaceWireRefs(aDefs, aRefs, aFaceNodeId);

    for (int aWireIter = 0; aWireIter < aFaceWireRefs.Length(); ++aWireIter)
    {
      const BRepGraphInc::WireRef& aWR         = aFaceWireRefs.Value(aWireIter);
      const BRepGraph_NodeId       aWireNodeId = BRepGraph_WireId(aWR.WireDefId.Index);
      const NCollection_Vector<BRepGraphInc::CoEdgeUsage> aWireCoEdgeRefs =
        collectWireCoEdgeRefs(aDefs, aRefs, aWireNodeId);

      for (int aCoEdgeIter = 0; aCoEdgeIter < aWireCoEdgeRefs.Length(); ++aCoEdgeIter)
      {
        const BRepGraphInc::CoEdgeUsage& aCR        = aWireCoEdgeRefs.Value(aCoEdgeIter);
        const BRepGraphInc::CoEdgeDef&   aCoEdgeDef = aDefs.CoEdges().Definition(aCR.DefId);
        const int                        anEdgeIdx  = aCoEdgeDef.EdgeDefId.Index;

        // Compose edge orientation: coedge sense x wire x face orientation.
        TopAbs_Orientation aCompOri = aCoEdgeDef.Sense;
        if (aWR.Orientation == TopAbs_REVERSED)
        {
          if (aCompOri == TopAbs_FORWARD)
            aCompOri = TopAbs_REVERSED;
          else if (aCompOri == TopAbs_REVERSED)
            aCompOri = TopAbs_FORWARD;
        }
        if (aFaceOri == TopAbs_REVERSED)
        {
          if (aCompOri == TopAbs_FORWARD)
            aCompOri = TopAbs_REVERSED;
          else if (aCompOri == TopAbs_REVERSED)
            aCompOri = TopAbs_FORWARD;
        }

        EdgeInFace anEIF;
        anEIF.FaceDefIdx        = aFaceIdx;
        anEIF.OrientationInWire = aCompOri;

        if (!anEdgeUsageMap.IsBound(anEdgeIdx))
          anEdgeUsageMap.Bind(anEdgeIdx, NCollection_Vector<EdgeInFace>());
        anEdgeUsageMap.ChangeFind(anEdgeIdx).Append(anEIF);
      }
    }
  }

  // For consistent orientation: each shared edge should appear FORWARD in one
  // face and REVERSED in the other.
  // Also track if a reorientation BFS would cycle (unorientable surface).
  int aBadOriCount = 0;
  for (NCollection_DataMap<int, NCollection_Vector<EdgeInFace>>::Iterator anIter(anEdgeUsageMap);
       anIter.More();
       anIter.Next())
  {
    const NCollection_Vector<EdgeInFace>& aEntries = anIter.Value();
    if (aEntries.Length() != 2)
      continue;

    // Skip degenerate edges.
    if (BRepGraph_Tool::Edge::Degenerated(theGraph, BRepGraph_EdgeId(anIter.Key())))
      continue;

    const TopAbs_Orientation anOri0 = aEntries.Value(0).OrientationInWire;
    const TopAbs_Orientation anOri1 = aEntries.Value(1).OrientationInWire;

    // Both FORWARD or both REVERSED means inconsistent orientation.
    if (anOri0 == anOri1 && (anOri0 == TopAbs_FORWARD || anOri0 == TopAbs_REVERSED))
    {
      aBadOriCount++;
      BRepGraphCheck_Issue anIssue;
      anIssue.NodeId        = aShellNodeId;
      anIssue.ContextNodeId = BRepGraph_EdgeId(anIter.Key());
      anIssue.Status        = BRepCheck_BadOrientation;
      anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
      theIssues.Append(anIssue);
    }
  }

  // Unorientable detection: attempt BFS orientation propagation.
  // If we encounter a conflict (a face needs both orientations), the surface is unorientable.
  if (aBadOriCount > 0)
  {
    // Build face adjacency within this shell.
    NCollection_Map<int> aShellFaces;
    for (int aFaceIter = 0; aFaceIter < aShellFaceRefs.Length(); ++aFaceIter)
    {
      const BRepGraphInc::FaceRef& aFR = aShellFaceRefs.Value(aFaceIter);
      aShellFaces.Add(aFR.FaceDefId.Index);
    }

    // BFS with orientation assignment: +1 or -1.
    NCollection_DataMap<int, int> aFaceOriMap; // Face def idx -> assigned orientation sign
    NCollection_Vector<int>       aQueue;
    bool                          anIsUnorientable = false;

    const int aStartFace = NCollection_Map<int>::Iterator(aShellFaces).Value();
    aFaceOriMap.Bind(aStartFace, +1);
    aQueue.Append(aStartFace);

    int aFront = 0;
    while (aFront < aQueue.Length() && !anIsUnorientable)
    {
      const int aCurrFace = aQueue.Value(aFront++);
      const int aCurrSign = aFaceOriMap.Find(aCurrFace);

      // Find all manifold-shared edges (those with exactly 2 face references).
      const BRepGraph::TopoView&                 aSpatial = theGraph.Topo();
      const BRepGraph_FaceId                     aCurrFaceId(aCurrFace);
      const NCollection_Vector<BRepGraph_FaceId> anAdjFaces =
        aSpatial.Faces().Adjacent(aCurrFaceId, theGraph.Allocator());

      for (int anAdjIter = 0; anAdjIter < anAdjFaces.Length(); ++anAdjIter)
      {
        const int anAdjFaceIdx = anAdjFaces.Value(anAdjIter).Index;
        if (!aShellFaces.Contains(anAdjFaceIdx))
          continue;

        // Find the shared edge and its orientations in both faces.
        const NCollection_Vector<BRepGraph_EdgeId> aSharedEdges =
          aSpatial.Faces().SharedEdges(aCurrFaceId,
                                       anAdjFaces.Value(anAdjIter),
                                       theGraph.Allocator());

        for (int anEdgeIter = 0; anEdgeIter < aSharedEdges.Length(); ++anEdgeIter)
        {
          const int anEdgeIdx = aSharedEdges.Value(anEdgeIter).Index;
          if (!anEdgeUsageMap.IsBound(anEdgeIdx))
            continue;

          const NCollection_Vector<EdgeInFace>& aEntries = anEdgeUsageMap.Find(anEdgeIdx);
          if (aEntries.Length() != 2)
            continue;

          // Determine required sign for the adjacent face.
          const bool anIsSameOri =
            (aEntries.Value(0).OrientationInWire == aEntries.Value(1).OrientationInWire);
          // If same orientation, adjacent face needs opposite sign to fix it.
          const int aRequiredSign = anIsSameOri ? -aCurrSign : aCurrSign;

          if (!aFaceOriMap.IsBound(anAdjFaceIdx))
          {
            aFaceOriMap.Bind(anAdjFaceIdx, aRequiredSign);
            aQueue.Append(anAdjFaceIdx);
          }
          else
          {
            // Already assigned - check consistency.
            if (aFaceOriMap.Find(anAdjFaceIdx) != aRequiredSign)
            {
              anIsUnorientable = true;
              break;
            }
          }
        }
      }
    }

    if (anIsUnorientable)
    {
      BRepGraphCheck_Issue anIssue;
      anIssue.NodeId        = aShellNodeId;
      anIssue.Status        = BRepCheck_UnorientableShape;
      anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
      theIssues.Append(anIssue);
    }
  }
}
