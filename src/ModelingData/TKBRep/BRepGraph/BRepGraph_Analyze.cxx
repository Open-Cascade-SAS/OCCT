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
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>

#include <Geom_Curve.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_DataMap.hxx>
#include <OSD_Parallel.hxx>

#include <functional>

//==================================================================================================

NCollection_Vector<BRepGraph_EdgeId> BRepGraph_Analyze::FreeEdges(const BRepGraph& theGraph)
{
  NCollection_Vector<BRepGraph_EdgeId> aResult;

  const BRepGraph::TopoView& aDefs = theGraph.Topo();

  for (int anEdgeIdx = 0; anEdgeIdx < aDefs.NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_EdgeId       anEdgeId(anEdgeIdx);
    const BRepGraphInc::EdgeDef& anEdge = aDefs.Edge(anEdgeId);
    if (anEdge.IsRemoved || anEdge.IsDegenerate)
      continue;

    if (aDefs.NbFacesOfEdge(anEdgeId) == 1)
      aResult.Append(anEdgeId);
  }
  return aResult;
}

//==================================================================================================

NCollection_Vector<std::pair<BRepGraph_EdgeId, BRepGraph_FaceId>> BRepGraph_Analyze::MissingPCurves(
  const BRepGraph& theGraph)
{
  NCollection_Vector<std::pair<BRepGraph_EdgeId, BRepGraph_FaceId>> aResult;

  const BRepGraph::TopoView& aDefs = theGraph.Topo();

  for (int aFaceDefIdx = 0; aFaceDefIdx < aDefs.NbFaces(); ++aFaceDefIdx)
  {
    const BRepGraph_FaceId aFaceId(aFaceDefIdx);
    const BRepGraph_NodeId aFaceDefId = BRepGraph_NodeId::Face(aFaceDefIdx);

    const NCollection_Vector<BRepGraph_NodeId> anEdges = aDefs.EdgesOfFace(aFaceDefId);
    for (int anEdgeIdx = 0; anEdgeIdx < anEdges.Length(); ++anEdgeIdx)
    {
      const BRepGraph_NodeId anEdgeNode = anEdges.Value(anEdgeIdx);
      if (anEdgeNode.NodeKind != BRepGraph_NodeId::Kind::Edge)
        continue;
      const BRepGraph_EdgeId       anEdgeDefId(anEdgeNode.Index);
      const BRepGraphInc::EdgeDef& anEdge = aDefs.Edge(anEdgeDefId);
      if (anEdge.IsDegenerate)
        continue;

      const BRepGraphInc::CoEdgeDef* aPCurve = aDefs.FindPCurve(anEdge.Id, aFaceDefId);
      if (aPCurve == nullptr)
        aResult.Append(std::make_pair(anEdgeDefId, aFaceId));
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
    const BRepGraph_EdgeId       anEdgeId(anEdgeIdx);
    const BRepGraphInc::EdgeDef& anEdgeDef = theGraph.myData->myIncStorage.Edge(anEdgeId);
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

  const BRepGraph::TopoView&  aDefs    = theGraph.Topo();
  const BRepGraphInc_Storage& aStorage = theGraph.myData->myIncStorage;

  const int aNbWires = aDefs.NbWires();
  if (aNbWires == 0)
    return aResult;

  NCollection_Array1<int> aNbCoEdgesPerWire(0, aNbWires - 1);
  NCollection_Array1<int> anIsOuterWire(0, aNbWires - 1);
  aNbCoEdgesPerWire.Init(0);
  anIsOuterWire.Init(0);

  for (int aWireDefIdx = 0; aWireDefIdx < aNbWires; ++aWireDefIdx)
  {
    const BRepGraphInc::WireDef& aWireEnt = aStorage.Wire(BRepGraph_WireId(aWireDefIdx));
    if (aWireEnt.IsRemoved)
      continue;
    for (int i = 0; i < aWireEnt.CoEdgeRefIds.Length(); ++i)
    {
      const BRepGraphInc::CoEdgeRef& aCoEdgeRef =
        aStorage.CoEdgeRef(aWireEnt.CoEdgeRefIds.Value(i));
      if (aCoEdgeRef.IsRemoved || !aCoEdgeRef.CoEdgeDefId.IsValid(aDefs.NbCoEdges()))
        continue;
      const BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.CoEdge(aCoEdgeRef.CoEdgeDefId);
      if (aCoEdge.IsRemoved || !aCoEdge.EdgeDefId.IsValid(aDefs.NbEdges()))
        continue;
      aNbCoEdgesPerWire(aWireDefIdx) += 1;
    }
  }

  for (int aFaceDefIdx = 0; aFaceDefIdx < aDefs.NbFaces(); ++aFaceDefIdx)
  {
    const BRepGraphInc::FaceDef& aFaceEnt = aStorage.Face(BRepGraph_FaceId(aFaceDefIdx));
    if (aFaceEnt.IsRemoved)
      continue;
    for (int i = 0; i < aFaceEnt.WireRefIds.Length(); ++i)
    {
      const BRepGraphInc::WireRef& aWireRef = aStorage.WireRef(aFaceEnt.WireRefIds.Value(i));
      if (aWireRef.IsRemoved || !aWireRef.IsOuter || !aWireRef.WireDefId.IsValid(aDefs.NbWires()))
        continue;
      anIsOuterWire(aWireRef.WireDefId.Index) = 1;
    }
  }

  for (int aWireDefIdx = 0; aWireDefIdx < aNbWires; ++aWireDefIdx)
  {
    const BRepGraph_WireId aWireId(aWireDefIdx);

    if (aNbCoEdgesPerWire(aWireDefIdx) < 2)
    {
      aResult.Append(aWireId);
      continue;
    }

    // Outer wire that is not closed.
    const BRepGraphInc::WireDef& aWire = aDefs.Wire(aWireId);
    if (!aWire.IsClosed && anIsOuterWire(aWireDefIdx) != 0)
      aResult.Append(aWireId);
  }

  return aResult;
}

//==================================================================================================

NCollection_Vector<BRepGraph_SubGraph> BRepGraph_Analyze::Decompose(const BRepGraph& theGraph)
{
  NCollection_Vector<BRepGraph_SubGraph> aResult;

  const BRepGraph::TopoView&  aDefs    = theGraph.Topo();
  const BRepGraphInc_Storage& aStorage = theGraph.myData->myIncStorage;

  // Collect wire, edge and vertex children from a face def into a SubGraph.
  const auto aCollectFaceChildren = [&](BRepGraph_SubGraph&    theSub,
                                        const BRepGraph_FaceId theFaceDefId) {
    const BRepGraphInc::FaceDef& aFaceEnt = aStorage.Face(theFaceDefId);
    for (int aWireRefIdx = 0; aWireRefIdx < aFaceEnt.WireRefIds.Length(); ++aWireRefIdx)
    {
      const BRepGraphInc::WireRef& aWireRef =
        aStorage.WireRef(aFaceEnt.WireRefIds.Value(aWireRefIdx));
      if (aWireRef.IsRemoved || !aWireRef.WireDefId.IsValid(aDefs.NbWires()))
        continue;

      const BRepGraph_WireId aWireDefId = aWireRef.WireDefId;
      theSub.myWireDefIds.Append(aWireDefId);

      const BRepGraphInc::WireDef& aWireEnt = aStorage.Wire(aWireDefId);
      for (int aCoEdgeRefIdx = 0; aCoEdgeRefIdx < aWireEnt.CoEdgeRefIds.Length(); ++aCoEdgeRefIdx)
      {
        const BRepGraphInc::CoEdgeRef& aCoEdgeRef =
          aStorage.CoEdgeRef(aWireEnt.CoEdgeRefIds.Value(aCoEdgeRefIdx));
        if (aCoEdgeRef.IsRemoved || !aCoEdgeRef.CoEdgeDefId.IsValid(aDefs.NbCoEdges()))
          continue;

        const BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.CoEdge(aCoEdgeRef.CoEdgeDefId);
        if (aCoEdge.IsRemoved || !aCoEdge.EdgeDefId.IsValid(aDefs.NbEdges()))
          continue;
        const BRepGraph_EdgeId anEdgeDefId = aCoEdge.EdgeDefId;
        theSub.myEdgeDefIds.Append(anEdgeDefId);

        const BRepGraphInc::EdgeDef& anEdgeDef = aDefs.Edge(anEdgeDefId);
        if (anEdgeDef.StartVertexRefId.IsValid())
        {
          const BRepGraph_VertexId aStartVtxId =
            aStorage.VertexRef(anEdgeDef.StartVertexRefId).VertexDefId;
          if (aStartVtxId.IsValid(aDefs.NbVertices()))
            theSub.myVertexDefIds.Append(aStartVtxId);
        }
        if (anEdgeDef.EndVertexRefId.IsValid())
        {
          const BRepGraph_VertexId anEndVtxId =
            aStorage.VertexRef(anEdgeDef.EndVertexRefId).VertexDefId;
          if (anEndVtxId.IsValid(aDefs.NbVertices()))
            theSub.myVertexDefIds.Append(anEndVtxId);
        }
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

      const BRepGraph_SolidId aSolidDefId(aSolidDefIdx);
      aSub.mySolidDefIds.Append(aSolidDefId);

      const BRepGraphInc::SolidDef& aSolidEnt = aStorage.Solid(aSolidDefId);
      for (int aShellRefIdx = 0; aShellRefIdx < aSolidEnt.ShellRefIds.Length(); ++aShellRefIdx)
      {
        const BRepGraphInc::ShellRef& aShellRef =
          aStorage.ShellRef(aSolidEnt.ShellRefIds.Value(aShellRefIdx));
        if (aShellRef.IsRemoved || !aShellRef.ShellDefId.IsValid(aDefs.NbShells()))
          continue;

        const BRepGraph_ShellId aShellDefId = aShellRef.ShellDefId;
        aSub.myShellDefIds.Append(aShellDefId);

        const BRepGraphInc::ShellDef& aShellEnt = aStorage.Shell(aShellDefId);
        for (int aFaceRefIdx = 0; aFaceRefIdx < aShellEnt.FaceRefIds.Length(); ++aFaceRefIdx)
        {
          const BRepGraphInc::FaceRef& aFaceRef =
            aStorage.FaceRef(aShellEnt.FaceRefIds.Value(aFaceRefIdx));
          if (aFaceRef.IsRemoved || !aFaceRef.FaceDefId.IsValid(aDefs.NbFaces()))
            continue;

          const BRepGraph_FaceId aFaceDefId = aFaceRef.FaceDefId;
          aSub.myFaceDefIds.Append(aFaceDefId);
          aCollectFaceChildren(aSub, aFaceDefId);
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

      const BRepGraph_ShellId aShellDefId(aShellDefIdx);
      aSub.myShellDefIds.Append(aShellDefId);

      const BRepGraphInc::ShellDef& aShellEnt = aStorage.Shell(aShellDefId);
      for (int aFaceRefIdx = 0; aFaceRefIdx < aShellEnt.FaceRefIds.Length(); ++aFaceRefIdx)
      {
        const BRepGraphInc::FaceRef& aFaceRef =
          aStorage.FaceRef(aShellEnt.FaceRefIds.Value(aFaceRefIdx));
        if (aFaceRef.IsRemoved || !aFaceRef.FaceDefId.IsValid(aDefs.NbFaces()))
          continue;

        const BRepGraph_FaceId aFaceDefId = aFaceRef.FaceDefId;
        aSub.myFaceDefIds.Append(aFaceDefId);
        aCollectFaceChildren(aSub, aFaceDefId);
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
      aCollectFaceChildren(aSub, aFaceDefId);

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
