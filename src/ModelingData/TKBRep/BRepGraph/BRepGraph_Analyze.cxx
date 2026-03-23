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

#include <BRepGraph_Analyze.hxx>

#include <BRepGraph.hxx>
#include <NCollection_Map.hxx>
#include <OSD_Parallel.hxx>

#include <functional>

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph_Analyze::FreeEdges(const BRepGraph& theGraph)
{
  NCollection_Vector<BRepGraph_NodeId> aResult;

  NCollection_Map<int> aFaceSet;
  for (int anEdgeIdx = 0; anEdgeIdx < theGraph.myEdges.Length(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::Edge& anEdge = theGraph.myEdges.Value(anEdgeIdx);
    if (anEdge.IsDegenerate)
      continue;

    // Count distinct faces via edge-to-wire reverse index.
    const NCollection_Vector<int>& aWires = theGraph.WiresOfEdge(anEdgeIdx);
    aFaceSet.Clear();
    for (int aWIdx = 0; aWIdx < aWires.Length(); ++aWIdx)
    {
      const BRepGraph_TopoNode::Wire& aWire = theGraph.myWires.Value(aWires.Value(aWIdx));
      if (aWire.OwnerFaceId.IsValid())
        aFaceSet.Add(aWire.OwnerFaceId.Index);
    }
    if (aFaceSet.Extent() == 1)
      aResult.Append(anEdge.Id);
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>>
  BRepGraph_Analyze::MissingPCurves(const BRepGraph& theGraph)
{
  NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>> aResult;

  for (int aFaceIdx = 0; aFaceIdx < theGraph.myFaces.Length(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::Face& aFace = theGraph.myFaces.Value(aFaceIdx);
    const BRepGraph_NodeId          aFaceId(BRepGraph_NodeKind::Face, aFaceIdx);

    auto checkWire = [&](BRepGraph_NodeId theWireId) {
      if (!theWireId.IsValid())
        return;
      const BRepGraph_TopoNode::Wire& aWire = theGraph.myWires.Value(theWireId.Index);
      for (int anEdgeIdx = 0; anEdgeIdx < aWire.OrderedEdges.Length(); ++anEdgeIdx)
      {
        const BRepGraph_NodeId          anEdgeId = aWire.OrderedEdges.Value(anEdgeIdx).EdgeId;
        const BRepGraph_TopoNode::Edge& anEdge   = theGraph.myEdges.Value(anEdgeId.Index);

        if (anEdge.IsDegenerate)
          continue;

        const BRepGraph_NodeId aPCurveId = theGraph.PCurveOf(anEdgeId, aFaceId);
        if (!aPCurveId.IsValid())
          aResult.Append(std::make_pair(anEdgeId, aFaceId));
      }
    };

    checkWire(aFace.OuterWireId);
    for (int aWireIdx = 0; aWireIdx < aFace.InnerWireIds.Length(); ++aWireIdx)
      checkWire(aFace.InnerWireIds.Value(aWireIdx));
  }

  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph_Analyze::ToleranceConflicts(
  const BRepGraph& theGraph,
  double           theThreshold)
{
  NCollection_Vector<BRepGraph_NodeId> aResult;

  for (int aCurveIdx = 0; aCurveIdx < theGraph.myCurves.Length(); ++aCurveIdx)
  {
    const BRepGraph_GeomNode::Curve& aCurve = theGraph.myCurves.Value(aCurveIdx);
    if (aCurve.EdgeUsers.Length() <= 1)
      continue;

    double aMinTol = 1.0e100;
    double aMaxTol = -1.0;
    for (int anIdx = 0; anIdx < aCurve.EdgeUsers.Length(); ++anIdx)
    {
      const double aTol = theGraph.myEdges.Value(aCurve.EdgeUsers.Value(anIdx).Index).Tolerance;
      if (aTol < aMinTol)
        aMinTol = aTol;
      if (aTol > aMaxTol)
        aMaxTol = aTol;
    }

    if (aMaxTol - aMinTol > theThreshold)
    {
      for (int anIdx = 0; anIdx < aCurve.EdgeUsers.Length(); ++anIdx)
        aResult.Append(aCurve.EdgeUsers.Value(anIdx));
    }
  }

  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph_Analyze::DegenerateWires(const BRepGraph& theGraph)
{
  NCollection_Vector<BRepGraph_NodeId> aResult;

  for (int aWireIdx = 0; aWireIdx < theGraph.myWires.Length(); ++aWireIdx)
  {
    const BRepGraph_TopoNode::Wire& aWire = theGraph.myWires.Value(aWireIdx);

    // Wire with < 2 edges.
    if (aWire.OrderedEdges.Length() < 2)
    {
      aResult.Append(aWire.Id);
      continue;
    }

    // Outer wire that is not closed.
    if (aWire.OwnerFaceId.IsValid())
    {
      const BRepGraph_TopoNode::Face& aFace = theGraph.myFaces.Value(aWire.OwnerFaceId.Index);
      if (aFace.OuterWireId == aWire.Id && !aWire.IsClosed)
        aResult.Append(aWire.Id);
    }
  }

  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_SubGraph> BRepGraph_Analyze::Decompose(const BRepGraph& theGraph)
{
  NCollection_Vector<BRepGraph_SubGraph> aResult;

  // Shared lambda: collect wire, edge and vertex indices from a face into a SubGraph.
  auto collectWire = [&](BRepGraph_SubGraph& theSub, BRepGraph_NodeId theWireId) {
    if (!theWireId.IsValid())
      return;
    theSub.myWireIndices.Append(theWireId.Index);
    const BRepGraph_TopoNode::Wire& aWire = theGraph.myWires.Value(theWireId.Index);
    for (int anEdgeIdx = 0; anEdgeIdx < aWire.OrderedEdges.Length(); ++anEdgeIdx)
    {
      theSub.myEdgeIndices.Append(aWire.OrderedEdges.Value(anEdgeIdx).EdgeId.Index);
      const BRepGraph_TopoNode::Edge& anEdge =
        theGraph.myEdges.Value(aWire.OrderedEdges.Value(anEdgeIdx).EdgeId.Index);
      if (anEdge.StartVertexId.IsValid())
        theSub.myVertexIndices.Append(anEdge.StartVertexId.Index);
      if (anEdge.EndVertexId.IsValid())
        theSub.myVertexIndices.Append(anEdge.EndVertexId.Index);
    }
  };

  // Shared lambda: collect all wire/edge/vertex data for a face.
  auto collectFaceChildren = [&](BRepGraph_SubGraph& theSub, int theFaceIdx) {
    const BRepGraph_TopoNode::Face& aFace = theGraph.myFaces.Value(theFaceIdx);
    collectWire(theSub, aFace.OuterWireId);
    for (int aWireIdx = 0; aWireIdx < aFace.InnerWireIds.Length(); ++aWireIdx)
      collectWire(theSub, aFace.InnerWireIds.Value(aWireIdx));
  };

  if (theGraph.mySolids.Length() > 0)
  {
    // Each solid -> one SubGraph.
    for (int aSolidIdx = 0; aSolidIdx < theGraph.mySolids.Length(); ++aSolidIdx)
    {
      BRepGraph_SubGraph aSub;
      aSub.myParent = &theGraph;
      aSub.mySolidIndices.Append(aSolidIdx);

      const BRepGraph_TopoNode::Solid& aSolid = theGraph.mySolids.Value(aSolidIdx);
      for (int aShellIter = 0; aShellIter < aSolid.Shells.Length(); ++aShellIter)
      {
        const int aShellIdx = aSolid.Shells.Value(aShellIter).Index;
        aSub.myShellIndices.Append(aShellIdx);

        const BRepGraph_TopoNode::Shell& aShell = theGraph.myShells.Value(aShellIdx);
        for (int aFaceIter = 0; aFaceIter < aShell.Faces.Length(); ++aFaceIter)
        {
          const int aFaceIdx = aShell.Faces.Value(aFaceIter).Index;
          aSub.myFaceIndices.Append(aFaceIdx);
          collectFaceChildren(aSub, aFaceIdx);
        }
      }

      aResult.Append(aSub);
    }
  }
  else if (theGraph.myShells.Length() > 0)
  {
    // No solids but shells exist: each shell -> one SubGraph.
    for (int aShellIdx = 0; aShellIdx < theGraph.myShells.Length(); ++aShellIdx)
    {
      BRepGraph_SubGraph aSub;
      aSub.myParent = &theGraph;
      aSub.myShellIndices.Append(aShellIdx);

      const BRepGraph_TopoNode::Shell& aShell = theGraph.myShells.Value(aShellIdx);
      for (int aFaceIter = 0; aFaceIter < aShell.Faces.Length(); ++aFaceIter)
      {
        const int aFaceIdx = aShell.Faces.Value(aFaceIter).Index;
        aSub.myFaceIndices.Append(aFaceIdx);
        collectFaceChildren(aSub, aFaceIdx);
      }

      aResult.Append(aSub);
    }
  }
  else
  {
    // No solids, no shells: each face -> one SubGraph.
    for (int aFaceIdx = 0; aFaceIdx < theGraph.myFaces.Length(); ++aFaceIdx)
    {
      BRepGraph_SubGraph aSub;
      aSub.myParent = &theGraph;
      aSub.myFaceIndices.Append(aFaceIdx);
      collectFaceChildren(aSub, aFaceIdx);

      aResult.Append(aSub);
    }
  }

  return aResult;
}

//=================================================================================================

void BRepGraph_Analyze::ParallelForEachFace(
  const BRepGraph&                        theGraph,
  const BRepGraph_SubGraph&               theSub,
  const std::function<void(int faceIdx)>& theLambda)
{
  (void)theGraph;
  const NCollection_Vector<int>& aIndices = theSub.FaceIndices();
  OSD_Parallel::For(
    0,
    aIndices.Length(),
    [&](int anIdx) { theLambda(aIndices.Value(anIdx)); },
    false);
}

//=================================================================================================

void BRepGraph_Analyze::ParallelForEachEdge(
  const BRepGraph&                        theGraph,
  const BRepGraph_SubGraph&               theSub,
  const std::function<void(int edgeIdx)>& theLambda)
{
  (void)theGraph;
  const NCollection_Vector<int>& aIndices = theSub.EdgeIndices();
  OSD_Parallel::For(
    0,
    aIndices.Length(),
    [&](int anIdx) { theLambda(aIndices.Value(anIdx)); },
    false);
}
