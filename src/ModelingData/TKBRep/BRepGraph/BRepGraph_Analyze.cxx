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
#include <BRepGraph_TopoView.hxx>
#include <BRepGraphInc_IncidenceRef.hxx>

#include <Geom_Curve.hxx>
#include <NCollection_DataMap.hxx>
#include <OSD_Parallel.hxx>

#include <functional>

//==================================================================================================

NCollection_Vector<BRepGraph_EdgeId> BRepGraph_Analyze::FreeEdges(const BRepGraph& theGraph)
{
  NCollection_Vector<BRepGraph_EdgeId> aResult;

  const BRepGraph::TopoView aDefs = theGraph.Topo();

  for (int anEdgeIdx = 0; anEdgeIdx < aDefs.NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_EdgeId             anEdgeId(anEdgeIdx);
    const BRepGraph_TopoNode::EdgeDef& anEdge = aDefs.Edge(anEdgeId);
    if (anEdge.IsRemoved || anEdge.IsDegenerate)
      continue;

    if (aDefs.FaceCountOfEdge(anEdgeId) == 1)
      aResult.Append(anEdgeId);
  }
  return aResult;
}

//==================================================================================================

NCollection_Vector<std::pair<BRepGraph_EdgeId, BRepGraph_FaceId>> BRepGraph_Analyze::MissingPCurves(
  const BRepGraph& theGraph)
{
  NCollection_Vector<std::pair<BRepGraph_EdgeId, BRepGraph_FaceId>> aResult;

  const BRepGraph::TopoView aDefs = theGraph.Topo();

  for (int aFaceDefIdx = 0; aFaceDefIdx < aDefs.NbFaces(); ++aFaceDefIdx)
  {
    const BRepGraph_FaceId             aFaceId(aFaceDefIdx);
    const BRepGraph_TopoNode::FaceDef& aFaceDef   = aDefs.Face(aFaceId);
    const BRepGraph_NodeId             aFaceDefId = aFaceDef.Id;

    for (int aWireRefIdx = 0; aWireRefIdx < aFaceDef.WireRefs.Length(); ++aWireRefIdx)
    {
      const BRepGraph_WireId aWireDefId           = aFaceDef.WireRefs.Value(aWireRefIdx).WireDefId;
      const BRepGraph_TopoNode::WireDef& aWireDef = aDefs.Wire(aWireDefId);
      for (int aCoEdgeIdx = 0; aCoEdgeIdx < aWireDef.CoEdgeRefs.Length(); ++aCoEdgeIdx)
      {
        const BRepGraphInc::CoEdgeEntity& aCoEdge = theGraph.myData->myIncStorage.CoEdge(
          BRepGraph_CoEdgeId(aWireDef.CoEdgeRefs.Value(aCoEdgeIdx).CoEdgeDefId));
        const BRepGraph_EdgeId             anEdgeDefId = aCoEdge.EdgeDefId;
        const BRepGraph_TopoNode::EdgeDef& anEdge      = aDefs.Edge(anEdgeDefId);

        if (anEdge.IsDegenerate)
          continue;

        const BRepGraphInc::CoEdgeEntity* aPCurve = aDefs.FindPCurve(anEdge.Id, aFaceDefId);
        if (aPCurve == nullptr)
          aResult.Append(std::make_pair(anEdgeDefId, aFaceId));
      }
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Vector<BRepGraph_EdgeId> BRepGraph_Analyze::ToleranceConflicts(
  const BRepGraph& theGraph,
  const double     theThreshold)
{
  NCollection_Vector<BRepGraph_EdgeId> aResult;

  // Build temporary curve pointer -> edges map by scanning edge defs
  NCollection_DataMap<const Geom_Curve*, NCollection_Vector<BRepGraph_EdgeId>> aCurveToEdges;
  for (int anEdgeIdx = 0; anEdgeIdx < theGraph.myData->myIncStorage.NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_EdgeId             anEdgeId(anEdgeIdx);
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = theGraph.myData->myIncStorage.Edge(anEdgeId);
    if (!anEdgeDef.Curve3DRepId.IsValid())
      continue;
    const occ::handle<Geom_Curve>& aCurve3d =
      theGraph.myData->myIncStorage.Curve3DRep(anEdgeDef.Curve3DRepId).Curve;
    if (aCurve3d.IsNull())
      continue;
    const Geom_Curve* aCurveKey = aCurve3d.get();
    aCurveToEdges.TryBind(aCurveKey, NCollection_Vector<BRepGraph_EdgeId>());
    aCurveToEdges.ChangeFind(aCurveKey).Append(anEdgeId);
  }

  for (NCollection_DataMap<const Geom_Curve*, NCollection_Vector<BRepGraph_EdgeId>>::Iterator
         anIter(aCurveToEdges);
       anIter.More();
       anIter.Next())
  {
    const NCollection_Vector<BRepGraph_EdgeId>& anEdges = anIter.Value();
    if (anEdges.Length() <= 1)
      continue;

    double aMinTol = 1.0e100;
    double aMaxTol = -1.0;
    for (int anIdx = 0; anIdx < anEdges.Length(); ++anIdx)
    {
      const BRepGraph_EdgeId anEdgeDefId = anEdges.Value(anIdx);
      const double           aTol = theGraph.myData->myIncStorage.Edge(anEdgeDefId).Tolerance;
      if (aTol < aMinTol)
        aMinTol = aTol;
      if (aTol > aMaxTol)
        aMaxTol = aTol;
    }

    if (aMaxTol - aMinTol > theThreshold)
    {
      for (int anIdx = 0; anIdx < anEdges.Length(); ++anIdx)
        aResult.Append(anEdges.Value(anIdx));
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Vector<BRepGraph_WireId> BRepGraph_Analyze::DegenerateWires(const BRepGraph& theGraph)
{
  NCollection_Vector<BRepGraph_WireId> aResult;

  const BRepGraph::TopoView aDefs = theGraph.Topo();

  for (int aWireDefIdx = 0; aWireDefIdx < aDefs.NbWires(); ++aWireDefIdx)
  {
    const BRepGraph_WireId             aWireId(aWireDefIdx);
    const BRepGraph_TopoNode::WireDef& aWire = aDefs.Wire(aWireId);

    // Wire with < 2 edges (check via incidence CoEdgeRefs).
    const int aNbEdgesInWire = aWire.CoEdgeRefs.Length();
    if (aNbEdgesInWire < 2)
    {
      aResult.Append(aWireId);
      continue;
    }

    // Outer wire that is not closed (check via incidence IsClosed).
    const bool aIsClosed = aWire.IsClosed;
    if (!aIsClosed)
    {
      // Check if this wire is the outer wire of any face (via reverse index).
      auto isOuterWireOfAnyFace = [&]() -> bool {
        const NCollection_Vector<BRepGraph_FaceId>* aFaces =
          theGraph.myData->myIncStorage.ReverseIndex().FacesOfWire(aWireId);
        if (aFaces == nullptr)
          return false;
        for (int aFIdx = 0; aFIdx < aFaces->Length(); ++aFIdx)
        {
          const BRepGraph_TopoNode::FaceDef& aFaceDef = aDefs.Face(aFaces->Value(aFIdx));
          for (int aWRIdx = 0; aWRIdx < aFaceDef.WireRefs.Length(); ++aWRIdx)
          {
            const BRepGraphInc::WireRef& aWR = aFaceDef.WireRefs.Value(aWRIdx);
            if (aWR.WireDefId.Index == aWireDefIdx && aWR.IsOuter)
              return true;
          }
        }
        return false;
      };
      if (isOuterWireOfAnyFace())
        aResult.Append(aWireId);
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Vector<BRepGraph_SubGraph> BRepGraph_Analyze::Decompose(const BRepGraph& theGraph)
{
  NCollection_Vector<BRepGraph_SubGraph> aResult;

  const BRepGraph::TopoView aDefs = theGraph.Topo();

  // Collect wire, edge and vertex children from a face def into a SubGraph.
  auto collectFaceChildren = [&](BRepGraph_SubGraph& theSub, const BRepGraph_FaceId theFaceDefId) {
    const BRepGraph_TopoNode::FaceDef& aFaceDef = aDefs.Face(theFaceDefId);

    for (int aWireRefIdx = 0; aWireRefIdx < aFaceDef.WireRefs.Length(); ++aWireRefIdx)
    {
      const BRepGraph_WireId aWireDefId           = aFaceDef.WireRefs.Value(aWireRefIdx).WireDefId;
      const BRepGraph_TopoNode::WireDef& aWireDef = aDefs.Wire(aWireDefId);
      theSub.myWireDefIds.Append(aWireDefId);

      for (int aCoEdgeIdx = 0; aCoEdgeIdx < aWireDef.CoEdgeRefs.Length(); ++aCoEdgeIdx)
      {
        const BRepGraphInc::CoEdgeEntity& aCoEdge = theGraph.myData->myIncStorage.CoEdge(
          BRepGraph_CoEdgeId(aWireDef.CoEdgeRefs.Value(aCoEdgeIdx).CoEdgeDefId));
        const BRepGraph_EdgeId anEdgeDefId = aCoEdge.EdgeDefId;
        theSub.myEdgeDefIds.Append(anEdgeDefId);

        const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aDefs.Edge(anEdgeDefId);
        if (anEdgeDef.StartVertex.VertexDefId.IsValid())
          theSub.myVertexDefIds.Append(anEdgeDef.StartVertex.VertexDefId);
        if (anEdgeDef.EndVertex.VertexDefId.IsValid())
          theSub.myVertexDefIds.Append(anEdgeDef.EndVertex.VertexDefId);
      }
    }
  };

  if (aDefs.NbSolids() > 0)
  {
    // Each solid def -> one SubGraph.
    for (int aSolidDefIdx = 0; aSolidDefIdx < aDefs.NbSolids(); ++aSolidDefIdx)
    {
      BRepGraph_SubGraph aSub;
      aSub.myParent = &theGraph;

      const BRepGraph_TopoNode::SolidDef& aSolidDef = aDefs.Solid(BRepGraph_SolidId(aSolidDefIdx));
      aSub.mySolidDefIds.Append(BRepGraph_SolidId(aSolidDefIdx));

      for (int aShellIter = 0; aShellIter < aSolidDef.ShellRefs.Length(); ++aShellIter)
      {
        const BRepGraph_ShellId aShellDefId = aSolidDef.ShellRefs.Value(aShellIter).ShellDefId;
        const BRepGraph_TopoNode::ShellDef& aShellDef = aDefs.Shell(aShellDefId);
        aSub.myShellDefIds.Append(aShellDefId);

        for (int aFaceIter = 0; aFaceIter < aShellDef.FaceRefs.Length(); ++aFaceIter)
        {
          const BRepGraph_FaceId aFaceDefId = aShellDef.FaceRefs.Value(aFaceIter).FaceDefId;
          aSub.myFaceDefIds.Append(aFaceDefId);
          collectFaceChildren(aSub, aFaceDefId);
        }
      }

      aResult.Append(aSub);
    }
  }
  else if (aDefs.NbShells() > 0)
  {
    // No solids but shells exist: each shell def -> one SubGraph.
    for (int aShellDefIdx = 0; aShellDefIdx < aDefs.NbShells(); ++aShellDefIdx)
    {
      BRepGraph_SubGraph aSub;
      aSub.myParent = &theGraph;

      const BRepGraph_TopoNode::ShellDef& aShellDef = aDefs.Shell(BRepGraph_ShellId(aShellDefIdx));
      aSub.myShellDefIds.Append(BRepGraph_ShellId(aShellDefIdx));

      for (int aFaceIter = 0; aFaceIter < aShellDef.FaceRefs.Length(); ++aFaceIter)
      {
        const BRepGraph_FaceId aFaceDefId = aShellDef.FaceRefs.Value(aFaceIter).FaceDefId;
        aSub.myFaceDefIds.Append(aFaceDefId);
        collectFaceChildren(aSub, aFaceDefId);
      }

      aResult.Append(aSub);
    }
  }
  else
  {
    // No solids, no shells: each face def -> one SubGraph.
    for (int aFaceDefIdx = 0; aFaceDefIdx < aDefs.NbFaces(); ++aFaceDefIdx)
    {
      BRepGraph_SubGraph aSub;
      aSub.myParent = &theGraph;

      const BRepGraph_FaceId aFaceDefId(aFaceDefIdx);
      aSub.myFaceDefIds.Append(aFaceDefId);
      collectFaceChildren(aSub, aFaceDefId);

      aResult.Append(aSub);
    }
  }

  return aResult;
}

//==================================================================================================

void BRepGraph_Analyze::ParallelForEachFace(
  const BRepGraph&                                   theGraph,
  const BRepGraph_SubGraph&                          theSub,
  const std::function<void(const BRepGraph_FaceId)>& theLambda)
{
  (void)theGraph;
  const NCollection_Vector<BRepGraph_FaceId>& aIndices = theSub.FaceDefIds();
  OSD_Parallel::For(
    0,
    aIndices.Length(),
    [&](int anIdx) { theLambda(aIndices.Value(anIdx)); },
    false);
}

//==================================================================================================

void BRepGraph_Analyze::ParallelForEachEdge(
  const BRepGraph&                                   theGraph,
  const BRepGraph_SubGraph&                          theSub,
  const std::function<void(const BRepGraph_EdgeId)>& theLambda)
{
  (void)theGraph;
  const NCollection_Vector<BRepGraph_EdgeId>& aIndices = theSub.EdgeDefIds();
  OSD_Parallel::For(
    0,
    aIndices.Length(),
    [&](int anIdx) { theLambda(aIndices.Value(anIdx)); },
    false);
}

