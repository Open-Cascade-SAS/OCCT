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
#include <BRepGraph_RelEdgesView.hxx>
#include <BRepGraph_SpatialView.hxx>
#include <BRepGraph_TopoNode.hxx>
#include <BRepGraph_UsagesView.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_Vector.hxx>

//=================================================================================================

void BRepGraphCheck::CheckShellMinimum(
  const BRepGraph&                         theGraph,
  int                                      theShellDefIdx,
  NCollection_Vector<BRepGraphCheck_Issue>& theIssues)
{
  const BRepGraph::DefsView aDefs = theGraph.Defs();
  const BRepGraph_TopoNode::ShellDef& aShellDef = aDefs.Shell(theShellDefIdx);

  // Shell must have at least one face (checked through usages).
  if (aShellDef.Usages.IsEmpty())
  {
    BRepGraphCheck_Issue anIssue;
    anIssue.NodeId        = aShellDef.Id;
    anIssue.Status        = BRepCheck_EmptyShell;
    anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
    theIssues.Append(anIssue);
    return;
  }

  // Check that the shell has faces through its first usage.
  const BRepGraph::UsagesView aUsages = theGraph.Usages();
  const BRepGraph_TopoNode::ShellUsage& aSU = aUsages.Shell(aShellDef.Usages.Value(0).Index);
  if (aSU.FaceUsages.IsEmpty())
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
  for (int aFaceIter = 0; aFaceIter < aSU.FaceUsages.Length(); ++aFaceIter)
  {
    const BRepGraph_NodeId aFaceDefId = theGraph.DefOf(aSU.FaceUsages.Value(aFaceIter));
    if (aFaceDefId.IsValid())
      aShellFaces.Add(aFaceDefId.Index);
  }

  if (aShellFaces.Size() <= 1)
    return;

  // BFS from the first face.
  const BRepGraph::SpatialView aSpatial = theGraph.Spatial();
  NCollection_Map<int> aVisited;
  NCollection_Vector<int> aQueue;

  const int aStartFace = NCollection_Map<int>::Iterator(aShellFaces).Value();
  aVisited.Add(aStartFace);
  aQueue.Append(aStartFace);

  int aFront = 0;
  while (aFront < aQueue.Length())
  {
    const int aCurrFaceIdx = aQueue.Value(aFront++);
    const BRepGraph_NodeId aCurrFaceNodeId = BRepGraph_NodeId::Face(aCurrFaceIdx);
    const NCollection_Vector<BRepGraph_NodeId> anAdjFaces = aSpatial.AdjacentFaces(aCurrFaceNodeId);

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
    anIssue.NodeId        = aDefs.Shell(theShellDefIdx).Id;
    anIssue.Status        = BRepCheck_NotConnected;
    anIssue.IssueSeverity = BRepGraphCheck_Issue::Severity::Error;
    theIssues.Append(anIssue);
  }
}

//=================================================================================================

void BRepGraphCheck::CheckShellClosed(
  const BRepGraph&                         theGraph,
  int                                      theShellDefIdx,
  NCollection_Vector<BRepGraphCheck_Issue>& theIssues)
{
  const BRepGraph::DefsView   aDefs   = theGraph.Defs();
  const BRepGraph::UsagesView aUsages = theGraph.Usages();
  const BRepGraph_TopoNode::ShellDef& aShellDef = aDefs.Shell(theShellDefIdx);
  const BRepGraph_NodeId aShellNodeId = aShellDef.Id;

  if (aShellDef.Usages.IsEmpty())
    return;

  const BRepGraph_TopoNode::ShellUsage& aSU = aUsages.Shell(aShellDef.Usages.Value(0).Index);
  if (aSU.FaceUsages.IsEmpty())
    return;

  // Collect all face definition indices belonging to this shell.
  NCollection_Map<int> aShellFaces;
  for (int aFaceIter = 0; aFaceIter < aSU.FaceUsages.Length(); ++aFaceIter)
  {
    const BRepGraph_NodeId aFaceDefId = theGraph.DefOf(aSU.FaceUsages.Value(aFaceIter));
    if (aFaceDefId.IsValid())
    {
      if (!aShellFaces.Add(aFaceDefId.Index))
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
  }

  // For each edge in the shell, count how many faces of this shell reference it.
  // An edge shared by exactly 2 faces is manifold; free edges indicate open shell.
  NCollection_DataMap<int, int> anEdgeFaceCounts;

  for (NCollection_Map<int>::Iterator aFaceIter(aShellFaces); aFaceIter.More(); aFaceIter.Next())
  {
    const int aFaceIdx = aFaceIter.Value();
    const BRepGraph_TopoNode::FaceDef& aFaceDef = aDefs.Face(aFaceIdx);

    if (aFaceDef.Usages.IsEmpty())
      continue;

    const BRepGraph_TopoNode::FaceUsage& aFU = aUsages.Face(aFaceDef.Usages.Value(0).Index);

    // Collect all edges from all wires of this face.
    for (int aWireIter = 0; aWireIter < aFU.NbWireUsages(); ++aWireIter)
    {
      const BRepGraph_NodeId aWireDefId = theGraph.DefOf(aFU.WireUsage(aWireIter));
      if (!aWireDefId.IsValid())
        continue;

      const BRepGraph_TopoNode::WireDef& aWireDef = aDefs.Wire(aWireDefId.Index);
      if (aWireDef.Usages.IsEmpty())
        continue;
      const BRepGraph_TopoNode::WireUsage& aWireUsage =
        aUsages.Wire(aWireDef.Usages.Value(0).Index);
      for (int anEdgeIter = 0; anEdgeIter < aWireUsage.EdgeUsages.Length(); ++anEdgeIter)
      {
        const BRepGraph_TopoNode::EdgeUsage& anEdgeUsage =
          aUsages.Edge(aWireUsage.EdgeUsages.Value(anEdgeIter).Index);
        const int anEdgeIdx = anEdgeUsage.DefId.Index;
        const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aDefs.Edge(anEdgeIdx);

        // Skip degenerate edges.
        if (anEdgeDef.IsDegenerate)
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
  for (NCollection_DataMap<int, int>::Iterator anEdgeIter(anEdgeFaceCounts);
       anEdgeIter.More(); anEdgeIter.Next())
  {
    BRepGraphCheck::CheckEdgeInShell(theGraph, anEdgeIter.Key(), theShellDefIdx,
                                     anEdgeIter.Value(), theIssues);
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

void BRepGraphCheck::CheckShellOrientation(
  const BRepGraph&                         theGraph,
  int                                      theShellDefIdx,
  NCollection_Vector<BRepGraphCheck_Issue>& theIssues)
{
  const BRepGraph::DefsView   aDefs   = theGraph.Defs();
  const BRepGraph::UsagesView aUsages = theGraph.Usages();
  const BRepGraph_TopoNode::ShellDef& aShellDef = aDefs.Shell(theShellDefIdx);
  const BRepGraph_NodeId aShellNodeId = aShellDef.Id;

  if (aShellDef.Usages.IsEmpty())
    return;

  const BRepGraph_TopoNode::ShellUsage& aSU = aUsages.Shell(aShellDef.Usages.Value(0).Index);
  if (aSU.FaceUsages.Length() < 2)
    return;

  // Build a map: edge def index -> list of (face def index, orientation in wire).
  struct EdgeInFace
  {
    int              FaceDefIdx;
    TopAbs_Orientation OrientationInWire;
  };
  NCollection_DataMap<int, NCollection_Vector<EdgeInFace>> anEdgeUsageMap;

  for (int aFaceIter = 0; aFaceIter < aSU.FaceUsages.Length(); ++aFaceIter)
  {
    const BRepGraph_NodeId aFaceDefId = theGraph.DefOf(aSU.FaceUsages.Value(aFaceIter));
    if (!aFaceDefId.IsValid())
      continue;

    const int aFaceIdx = aFaceDefId.Index;
    const BRepGraph_TopoNode::FaceDef& aFaceDef = aDefs.Face(aFaceIdx);
    if (aFaceDef.Usages.IsEmpty())
      continue;

    const BRepGraph_TopoNode::FaceUsage& aFU = aUsages.Face(aFaceDef.Usages.Value(0).Index);
    const TopAbs_Orientation aFaceOri = aFU.Orientation;

    for (int aWireIter = 0; aWireIter < aFU.NbWireUsages(); ++aWireIter)
    {
      const BRepGraph_NodeId aWireDefId = theGraph.DefOf(aFU.WireUsage(aWireIter));
      if (!aWireDefId.IsValid())
        continue;

      const BRepGraph_TopoNode::WireDef& aWireDef = aDefs.Wire(aWireDefId.Index);
      if (aWireDef.Usages.IsEmpty())
        continue;
      const BRepGraph_TopoNode::WireUsage& aWireUsage2 =
        aUsages.Wire(aWireDef.Usages.Value(0).Index);
      for (int anEdgeIter = 0; anEdgeIter < aWireUsage2.EdgeUsages.Length(); ++anEdgeIter)
      {
        const BRepGraph_TopoNode::EdgeUsage& anEdgeUsage =
          aUsages.Edge(aWireUsage2.EdgeUsages.Value(anEdgeIter).Index);
        const int anEdgeIdx = anEdgeUsage.DefId.Index;

        // Compose edge orientation: wire orientation XOR face orientation.
        TopAbs_Orientation aCompOri = anEdgeUsage.Orientation;
        if (aFaceOri == TopAbs_REVERSED)
        {
          if (aCompOri == TopAbs_FORWARD)
            aCompOri = TopAbs_REVERSED;
          else if (aCompOri == TopAbs_REVERSED)
            aCompOri = TopAbs_FORWARD;
        }

        EdgeInFace anEIF;
        anEIF.FaceDefIdx = aFaceIdx;
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
       anIter.More(); anIter.Next())
  {
    const NCollection_Vector<EdgeInFace>& aEntries = anIter.Value();
    if (aEntries.Length() != 2)
      continue;

    // Skip degenerate edges.
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aDefs.Edge(anIter.Key());
    if (anEdgeDef.IsDegenerate)
      continue;

    const TopAbs_Orientation anOri0 = aEntries.Value(0).OrientationInWire;
    const TopAbs_Orientation anOri1 = aEntries.Value(1).OrientationInWire;

    // Both FORWARD or both REVERSED means inconsistent orientation.
    if (anOri0 == anOri1
        && (anOri0 == TopAbs_FORWARD || anOri0 == TopAbs_REVERSED))
    {
      aBadOriCount++;
      BRepGraphCheck_Issue anIssue;
      anIssue.NodeId        = aShellNodeId;
      anIssue.ContextNodeId = BRepGraph_NodeId::Edge(anIter.Key());
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
    for (int aFaceIter = 0; aFaceIter < aSU.FaceUsages.Length(); ++aFaceIter)
    {
      const BRepGraph_NodeId aFaceDefId = theGraph.DefOf(aSU.FaceUsages.Value(aFaceIter));
      if (aFaceDefId.IsValid())
        aShellFaces.Add(aFaceDefId.Index);
    }

    // BFS with orientation assignment: +1 or -1.
    NCollection_DataMap<int, int> aFaceOriMap; // Face def idx -> assigned orientation sign
    NCollection_Vector<int> aQueue;
    bool anIsUnorientable = false;

    const int aStartFace = NCollection_Map<int>::Iterator(aShellFaces).Value();
    aFaceOriMap.Bind(aStartFace, +1);
    aQueue.Append(aStartFace);

    int aFront = 0;
    while (aFront < aQueue.Length() && !anIsUnorientable)
    {
      const int aCurrFace = aQueue.Value(aFront++);
      const int aCurrSign = aFaceOriMap.Find(aCurrFace);

      // Find all manifold-shared edges (those with exactly 2 face references).
      const BRepGraph::SpatialView aSpatial = theGraph.Spatial();
      const BRepGraph_NodeId aCurrFaceNodeId(BRepGraph_NodeId::Kind::Face, aCurrFace);
      const NCollection_Vector<BRepGraph_NodeId> anAdjFaces =
        aSpatial.AdjacentFaces(aCurrFaceNodeId);

      for (int anAdjIter = 0; anAdjIter < anAdjFaces.Length(); ++anAdjIter)
      {
        const int anAdjFaceIdx = anAdjFaces.Value(anAdjIter).Index;
        if (!aShellFaces.Contains(anAdjFaceIdx))
          continue;

        // Find the shared edge and its orientations in both faces.
        const NCollection_Vector<BRepGraph_NodeId> aSharedEdges =
          aSpatial.SharedEdges(aCurrFaceNodeId, anAdjFaces.Value(anAdjIter));

        for (int anEdgeIter = 0; anEdgeIter < aSharedEdges.Length(); ++anEdgeIter)
        {
          const int anEdgeIdx = aSharedEdges.Value(anEdgeIter).Index;
          if (!anEdgeUsageMap.IsBound(anEdgeIdx))
            continue;

          const NCollection_Vector<EdgeInFace>& aEntries = anEdgeUsageMap.Find(anEdgeIdx);
          if (aEntries.Length() != 2)
            continue;

          // Determine required sign for the adjacent face.
          const bool anIsSameOri = (aEntries.Value(0).OrientationInWire
                                    == aEntries.Value(1).OrientationInWire);
          // If same orientation, adjacent face needs opposite sign to fix it.
          const int aRequiredSign = anIsSameOri ? -aCurrSign : aCurrSign;

          if (!aFaceOriMap.IsBound(anAdjFaceIdx))
          {
            aFaceOriMap.Bind(anAdjFaceIdx, aRequiredSign);
            aQueue.Append(anAdjFaceIdx);
          }
          else
          {
            // Already assigned — check consistency.
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
