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
#include <BRepGraph_Data.hxx>
#include <NCollection_Map.hxx>
#include <OSD_Parallel.hxx>

#include <functional>

//==================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph_Analyze::FreeEdges(const BRepGraph& theGraph)
{
  NCollection_Vector<BRepGraph_NodeId> aResult;

  for (int anEdgeIdx = 0; anEdgeIdx < theGraph.NbEdgeDefs(); ++anEdgeIdx)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = theGraph.EdgeDefinition(anEdgeIdx);
    if (anEdge.IsDegenerate)
      continue;

    if (theGraph.FaceCountForEdge(anEdgeIdx) == 1)
      aResult.Append(anEdge.Id);
  }
  return aResult;
}

//==================================================================================================

NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>>
  BRepGraph_Analyze::MissingPCurves(const BRepGraph& theGraph)
{
  NCollection_Vector<std::pair<BRepGraph_NodeId, BRepGraph_NodeId>> aResult;

  for (int aFaceUsageIdx = 0; aFaceUsageIdx < theGraph.NbFaceUsages(); ++aFaceUsageIdx)
  {
    const BRepGraph_TopoNode::FaceUsage& aFaceUsage = theGraph.FaceUsageNode(aFaceUsageIdx);
    const BRepGraph_NodeId               aFaceDefId = aFaceUsage.DefId;

    auto checkWire = [&](BRepGraph_UsageId theWireUsageId) {
      if (!theWireUsageId.IsValid())
        return;
      const BRepGraph_TopoNode::WireUsage& aWireUsage = theGraph.WireUsageNode(theWireUsageId.Index);
      const BRepGraph_TopoNode::WireDef&   aWireDef   = theGraph.WireDefinition(aWireUsage.DefId.Index);
      for (int anEdgeIdx = 0; anEdgeIdx < aWireDef.OrderedEdges.Length(); ++anEdgeIdx)
      {
        const BRepGraph_NodeId               anEdgeDefId = aWireDef.OrderedEdges.Value(anEdgeIdx).EdgeDefId;
        const BRepGraph_TopoNode::EdgeDef&   anEdge      = theGraph.EdgeDefinition(anEdgeDefId.Index);

        if (anEdge.IsDegenerate)
          continue;

        const BRepGraph_NodeId aPCurveId = theGraph.PCurveOf(anEdgeDefId, aFaceDefId);
        if (!aPCurveId.IsValid())
          aResult.Append(std::make_pair(anEdgeDefId, aFaceDefId));
      }
    };

    checkWire(aFaceUsage.OuterWireUsage);
    for (int aWireIdx = 0; aWireIdx < aFaceUsage.InnerWireUsages.Length(); ++aWireIdx)
      checkWire(aFaceUsage.InnerWireUsages.Value(aWireIdx));
  }

  return aResult;
}

//==================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph_Analyze::ToleranceConflicts(
  const BRepGraph& theGraph,
  double           theThreshold)
{
  NCollection_Vector<BRepGraph_NodeId> aResult;

  for (int aCurveIdx = 0; aCurveIdx < theGraph.myData->myCurves.Length(); ++aCurveIdx)
  {
    const BRepGraph_GeomNode::Curve& aCurve = theGraph.myData->myCurves.Value(aCurveIdx);
    if (aCurve.EdgeDefUsers.Length() <= 1)
      continue;

    double aMinTol = 1.0e100;
    double aMaxTol = -1.0;
    for (int anIdx = 0; anIdx < aCurve.EdgeDefUsers.Length(); ++anIdx)
    {
      const double aTol = theGraph.myData->myEdgeDefs.Value(aCurve.EdgeDefUsers.Value(anIdx).Index).Tolerance;
      if (aTol < aMinTol)
        aMinTol = aTol;
      if (aTol > aMaxTol)
        aMaxTol = aTol;
    }

    if (aMaxTol - aMinTol > theThreshold)
    {
      for (int anIdx = 0; anIdx < aCurve.EdgeDefUsers.Length(); ++anIdx)
        aResult.Append(aCurve.EdgeDefUsers.Value(anIdx));
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph_Analyze::DegenerateWires(const BRepGraph& theGraph)
{
  NCollection_Vector<BRepGraph_NodeId> aResult;

  for (int aWireDefIdx = 0; aWireDefIdx < theGraph.NbWireDefs(); ++aWireDefIdx)
  {
    const BRepGraph_TopoNode::WireDef& aWire = theGraph.WireDefinition(aWireDefIdx);

    // Wire with < 2 edges.
    if (aWire.OrderedEdges.Length() < 2)
    {
      aResult.Append(aWire.Id);
      continue;
    }

    // Outer wire that is not closed.
    if (!aWire.IsClosed)
    {
      // Check if any usage of this wire is the outer wire of its face.
      for (int aUsageIdx = 0; aUsageIdx < aWire.Usages.Length(); ++aUsageIdx)
      {
        const BRepGraph_UsageId& aWireUsageId = aWire.Usages.Value(aUsageIdx);
        const BRepGraph_TopoNode::WireUsage& aWireUsage =
          theGraph.WireUsageNode(aWireUsageId.Index);
        if (aWireUsage.OwnerFaceUsage.IsValid())
        {
          const BRepGraph_TopoNode::FaceUsage& aFaceUsage =
            theGraph.FaceUsageNode(aWireUsage.OwnerFaceUsage.Index);
          if (aFaceUsage.OuterWireUsage == aWireUsageId)
          {
            aResult.Append(aWire.Id);
            break;
          }
        }
      }
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Vector<BRepGraph_SubGraph> BRepGraph_Analyze::Decompose(const BRepGraph& theGraph)
{
  NCollection_Vector<BRepGraph_SubGraph> aResult;

  // Collect wire, edge and vertex children from a face usage into a SubGraph.
  auto collectFaceChildren = [&](BRepGraph_SubGraph& theSub, int theFaceUsageIdx) {
    const BRepGraph_TopoNode::FaceUsage& aFaceUsage = theGraph.FaceUsageNode(theFaceUsageIdx);

    auto collectWire = [&](BRepGraph_UsageId theWireUsageId) {
      if (!theWireUsageId.IsValid())
        return;
      const BRepGraph_TopoNode::WireUsage& aWireUsage =
        theGraph.WireUsageNode(theWireUsageId.Index);
      theSub.myWireDefIndices.Append(aWireUsage.DefId.Index);
      theSub.myWireUsageIndices.Append(theWireUsageId.Index);
      const BRepGraph_TopoNode::WireDef& aWireDef =
        theGraph.WireDefinition(aWireUsage.DefId.Index);
      for (int anEdgeIdx = 0; anEdgeIdx < aWireDef.OrderedEdges.Length(); ++anEdgeIdx)
      {
        const int anEdgeDefIdx = aWireDef.OrderedEdges.Value(anEdgeIdx).EdgeDefId.Index;
        theSub.myEdgeDefIndices.Append(anEdgeDefIdx);

        // Also add edge usage indices from WireUsage.EdgeUsages if available.
        if (anEdgeIdx < aWireUsage.EdgeUsages.Length())
        {
          const BRepGraph_UsageId& anEdgeUsageId = aWireUsage.EdgeUsages.Value(anEdgeIdx);
          theSub.myEdgeUsageIndices.Append(anEdgeUsageId.Index);

          // Get vertex usages from edge usage.
          const BRepGraph_TopoNode::EdgeUsage& anEdgeUsage =
            theGraph.EdgeUsageNode(anEdgeUsageId.Index);
          if (anEdgeUsage.StartVertexUsage.IsValid())
            theSub.myVertexUsageIndices.Append(anEdgeUsage.StartVertexUsage.Index);
          if (anEdgeUsage.EndVertexUsage.IsValid())
            theSub.myVertexUsageIndices.Append(anEdgeUsage.EndVertexUsage.Index);
        }

        const BRepGraph_TopoNode::EdgeDef& anEdgeDef = theGraph.EdgeDefinition(anEdgeDefIdx);
        if (anEdgeDef.StartVertexDefId.IsValid())
          theSub.myVertexDefIndices.Append(anEdgeDef.StartVertexDefId.Index);
        if (anEdgeDef.EndVertexDefId.IsValid())
          theSub.myVertexDefIndices.Append(anEdgeDef.EndVertexDefId.Index);
      }
    };

    collectWire(aFaceUsage.OuterWireUsage);
    for (int aWireIdx = 0; aWireIdx < aFaceUsage.InnerWireUsages.Length(); ++aWireIdx)
      collectWire(aFaceUsage.InnerWireUsages.Value(aWireIdx));
  };

  if (theGraph.NbSolidUsages() > 0)
  {
    // Each solid usage -> one SubGraph.
    for (int aSolidUsageIdx = 0; aSolidUsageIdx < theGraph.NbSolidUsages(); ++aSolidUsageIdx)
    {
      BRepGraph_SubGraph aSub;
      aSub.myParent = &theGraph;

      const BRepGraph_TopoNode::SolidUsage& aSolidUsage = theGraph.SolidUsageNode(aSolidUsageIdx);
      aSub.mySolidDefIndices.Append(aSolidUsage.DefId.Index);
      aSub.mySolidUsageIndices.Append(aSolidUsageIdx);

      for (int aShellIter = 0; aShellIter < aSolidUsage.ShellUsages.Length(); ++aShellIter)
      {
        const BRepGraph_UsageId& aShellUsageId = aSolidUsage.ShellUsages.Value(aShellIter);
        const BRepGraph_TopoNode::ShellUsage& aShellUsage =
          theGraph.ShellUsageNode(aShellUsageId.Index);
        aSub.myShellDefIndices.Append(aShellUsage.DefId.Index);
        aSub.myShellUsageIndices.Append(aShellUsageId.Index);

        for (int aFaceIter = 0; aFaceIter < aShellUsage.FaceUsages.Length(); ++aFaceIter)
        {
          const BRepGraph_UsageId& aFaceUsageId = aShellUsage.FaceUsages.Value(aFaceIter);
          const BRepGraph_TopoNode::FaceUsage& aFaceUsage =
            theGraph.FaceUsageNode(aFaceUsageId.Index);
          aSub.myFaceDefIndices.Append(aFaceUsage.DefId.Index);
          aSub.myFaceUsageIndices.Append(aFaceUsageId.Index);
          collectFaceChildren(aSub, aFaceUsageId.Index);
        }
      }

      aResult.Append(aSub);
    }
  }
  else if (theGraph.NbShellUsages() > 0)
  {
    // No solids but shells exist: each shell usage -> one SubGraph.
    for (int aShellUsageIdx = 0; aShellUsageIdx < theGraph.NbShellUsages(); ++aShellUsageIdx)
    {
      BRepGraph_SubGraph aSub;
      aSub.myParent = &theGraph;

      const BRepGraph_TopoNode::ShellUsage& aShellUsage = theGraph.ShellUsageNode(aShellUsageIdx);
      aSub.myShellDefIndices.Append(aShellUsage.DefId.Index);
      aSub.myShellUsageIndices.Append(aShellUsageIdx);

      for (int aFaceIter = 0; aFaceIter < aShellUsage.FaceUsages.Length(); ++aFaceIter)
      {
        const BRepGraph_UsageId& aFaceUsageId = aShellUsage.FaceUsages.Value(aFaceIter);
        const BRepGraph_TopoNode::FaceUsage& aFaceUsage =
          theGraph.FaceUsageNode(aFaceUsageId.Index);
        aSub.myFaceDefIndices.Append(aFaceUsage.DefId.Index);
        aSub.myFaceUsageIndices.Append(aFaceUsageId.Index);
        collectFaceChildren(aSub, aFaceUsageId.Index);
      }

      aResult.Append(aSub);
    }
  }
  else
  {
    // No solids, no shells: each face usage -> one SubGraph.
    for (int aFaceUsageIdx = 0; aFaceUsageIdx < theGraph.NbFaceUsages(); ++aFaceUsageIdx)
    {
      BRepGraph_SubGraph aSub;
      aSub.myParent = &theGraph;

      const BRepGraph_TopoNode::FaceUsage& aFaceUsage = theGraph.FaceUsageNode(aFaceUsageIdx);
      aSub.myFaceDefIndices.Append(aFaceUsage.DefId.Index);
      aSub.myFaceUsageIndices.Append(aFaceUsageIdx);
      collectFaceChildren(aSub, aFaceUsageIdx);

      aResult.Append(aSub);
    }
  }

  return aResult;
}

//==================================================================================================

void BRepGraph_Analyze::ParallelForEachFace(
  const BRepGraph&                        theGraph,
  const BRepGraph_SubGraph&               theSub,
  const std::function<void(int faceIdx)>& theLambda)
{
  (void)theGraph;
  const NCollection_Vector<int>& aIndices = theSub.FaceDefIndices();
  OSD_Parallel::For(
    0,
    aIndices.Length(),
    [&](int anIdx) { theLambda(aIndices.Value(anIdx)); },
    false);
}

//==================================================================================================

void BRepGraph_Analyze::ParallelForEachEdge(
  const BRepGraph&                        theGraph,
  const BRepGraph_SubGraph&               theSub,
  const std::function<void(int edgeIdx)>& theLambda)
{
  (void)theGraph;
  const NCollection_Vector<int>& aIndices = theSub.EdgeDefIndices();
  OSD_Parallel::For(
    0,
    aIndices.Length(),
    [&](int anIdx) { theLambda(aIndices.Value(anIdx)); },
    false);
}
