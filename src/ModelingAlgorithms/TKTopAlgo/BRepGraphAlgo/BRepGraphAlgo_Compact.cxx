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

#include <BRepGraphAlgo_Compact.hxx>

#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_Layer.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_Mutator.hxx>
#include <BRepGraph_MutRef.hxx>
#include <BRepGraph_MutView.hxx>
#include <BRepGraph_RelEdgesView.hxx>
#include <BRepGraph_UID.hxx>
#include <BRepGraphInc_IncidenceRef.hxx>

#include <NCollection_DataMap.hxx>

#include <utility>

namespace
{

//! Remap a NodeId through old->new index maps. Returns invalid NodeId if not found.
BRepGraph_NodeId remapNodeId(const BRepGraph_NodeId&              theId,
                             const NCollection_DataMap<int, int>& theVertexMap,
                             const NCollection_DataMap<int, int>& theEdgeMap,
                             const NCollection_DataMap<int, int>& theWireMap,
                             const NCollection_DataMap<int, int>& theFaceMap,
                             const NCollection_DataMap<int, int>& theShellMap,
                             const NCollection_DataMap<int, int>& theSolidMap,
                             const NCollection_DataMap<int, int>& theCompoundMap,
                             const NCollection_DataMap<int, int>& theCompSolidMap)
{
  if (!theId.IsValid())
    return BRepGraph_NodeId();

  const NCollection_DataMap<int, int>* aMap = nullptr;
  switch (theId.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      aMap = &theVertexMap;
      break;
    case BRepGraph_NodeId::Kind::Edge:
      aMap = &theEdgeMap;
      break;
    case BRepGraph_NodeId::Kind::Wire:
      aMap = &theWireMap;
      break;
    case BRepGraph_NodeId::Kind::Face:
      aMap = &theFaceMap;
      break;
    case BRepGraph_NodeId::Kind::Shell:
      aMap = &theShellMap;
      break;
    case BRepGraph_NodeId::Kind::Solid:
      aMap = &theSolidMap;
      break;
    case BRepGraph_NodeId::Kind::Compound:
      aMap = &theCompoundMap;
      break;
    case BRepGraph_NodeId::Kind::CompSolid:
      aMap = &theCompSolidMap;
      break;
    default:
      // Product/Occurrence kinds are not compacted — they reference topology
      // nodes which are remapped independently.
      break;
  }

  if (aMap == nullptr)
    return BRepGraph_NodeId();

  const int* aNewIdx = aMap->Seek(theId.Index);
  if (aNewIdx == nullptr)
    return BRepGraph_NodeId();

  return BRepGraph_NodeId(theId.NodeKind, *aNewIdx);
}

} // namespace

//=================================================================================================

BRepGraphAlgo_Compact::Result BRepGraphAlgo_Compact::Perform(BRepGraph& theGraph)
{
  return Perform(theGraph, Options());
}

//=================================================================================================

BRepGraphAlgo_Compact::Result BRepGraphAlgo_Compact::Perform(BRepGraph&     theGraph,
                                                             const Options& theOptions)
{
  Result aResult;
  if (!theGraph.IsDone())
  {
    return aResult;
  }

  // Count removed nodes per kind.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbVertices(); ++anIdx)
    if (theGraph.Defs().Vertex(anIdx).IsRemoved)
      ++aResult.NbRemovedVertices;
  for (int anIdx = 0; anIdx < theGraph.Defs().NbEdges(); ++anIdx)
    if (theGraph.Defs().Edge(anIdx).IsRemoved)
      ++aResult.NbRemovedEdges;
  for (int anIdx = 0; anIdx < theGraph.Defs().NbWires(); ++anIdx)
    if (theGraph.Defs().Wire(anIdx).IsRemoved)
      ++aResult.NbRemovedWires;
  for (int anIdx = 0; anIdx < theGraph.Defs().NbFaces(); ++anIdx)
    if (theGraph.Defs().Face(anIdx).IsRemoved)
      ++aResult.NbRemovedFaces;
  for (int anIdx = 0; anIdx < theGraph.Defs().NbShells(); ++anIdx)
    if (theGraph.Defs().Shell(anIdx).IsRemoved)
      ++aResult.NbRemovedShells;
  for (int anIdx = 0; anIdx < theGraph.Defs().NbSolids(); ++anIdx)
    if (theGraph.Defs().Solid(anIdx).IsRemoved)
      ++aResult.NbRemovedSolids;
  for (int anIdx = 0; anIdx < theGraph.Defs().NbCompounds(); ++anIdx)
    if (theGraph.Defs().Compound(anIdx).IsRemoved)
      ++aResult.NbRemovedCompounds;
  for (int anIdx = 0; anIdx < theGraph.Defs().NbCompSolids(); ++anIdx)
    if (theGraph.Defs().CompSolid(anIdx).IsRemoved)
      ++aResult.NbRemovedCompSolids;

  const int aTotalRemoved =
    aResult.NbRemovedVertices + aResult.NbRemovedEdges + aResult.NbRemovedWires
    + aResult.NbRemovedFaces + aResult.NbRemovedShells + aResult.NbRemovedSolids
    + aResult.NbRemovedCompounds + aResult.NbRemovedCompSolids;

  aResult.NbNodesBefore = static_cast<int>(theGraph.Defs().NbNodes());

  // Short-circuit: nothing to compact.
  if (aTotalRemoved == 0)
  {
    aResult.NbNodesAfter = aResult.NbNodesBefore;
    return aResult;
  }

  // Build old->new index maps for each node kind.
  NCollection_DataMap<int, int> aVertexMap, anEdgeMap, aWireMap, aFaceMap;
  NCollection_DataMap<int, int> aShellMap, aSolidMap, aCompoundMap, aCompSolidMap;

  int aNewIdx = 0;
  for (int anIdx = 0; anIdx < theGraph.Defs().NbVertices(); ++anIdx)
    if (!theGraph.Defs().Vertex(anIdx).IsRemoved)
      aVertexMap.Bind(anIdx, aNewIdx++);
  aNewIdx = 0;
  for (int anIdx = 0; anIdx < theGraph.Defs().NbEdges(); ++anIdx)
    if (!theGraph.Defs().Edge(anIdx).IsRemoved)
      anEdgeMap.Bind(anIdx, aNewIdx++);
  aNewIdx = 0;
  for (int anIdx = 0; anIdx < theGraph.Defs().NbWires(); ++anIdx)
    if (!theGraph.Defs().Wire(anIdx).IsRemoved)
      aWireMap.Bind(anIdx, aNewIdx++);
  aNewIdx = 0;
  for (int anIdx = 0; anIdx < theGraph.Defs().NbFaces(); ++anIdx)
    if (!theGraph.Defs().Face(anIdx).IsRemoved)
      aFaceMap.Bind(anIdx, aNewIdx++);
  aNewIdx = 0;
  for (int anIdx = 0; anIdx < theGraph.Defs().NbShells(); ++anIdx)
    if (!theGraph.Defs().Shell(anIdx).IsRemoved)
      aShellMap.Bind(anIdx, aNewIdx++);
  aNewIdx = 0;
  for (int anIdx = 0; anIdx < theGraph.Defs().NbSolids(); ++anIdx)
    if (!theGraph.Defs().Solid(anIdx).IsRemoved)
      aSolidMap.Bind(anIdx, aNewIdx++);
  aNewIdx = 0;
  for (int anIdx = 0; anIdx < theGraph.Defs().NbCompounds(); ++anIdx)
    if (!theGraph.Defs().Compound(anIdx).IsRemoved)
      aCompoundMap.Bind(anIdx, aNewIdx++);
  aNewIdx = 0;
  for (int anIdx = 0; anIdx < theGraph.Defs().NbCompSolids(); ++anIdx)
    if (!theGraph.Defs().CompSolid(anIdx).IsRemoved)
      aCompSolidMap.Bind(anIdx, aNewIdx++);

  const bool wasHistoryEnabled = theGraph.IsHistoryEnabled();
  theGraph.SetHistoryEnabled(theOptions.HistoryMode);

  // Record history before swap (remap entries for topology defs).
  if (theOptions.HistoryMode)
  {
    for (NCollection_DataMap<int, int>::Iterator anIt(aVertexMap); anIt.More(); anIt.Next())
    {
      if (anIt.Key() != anIt.Value())
      {
        NCollection_Vector<BRepGraph_NodeId> aRepl;
        aRepl.Append(BRepGraph_NodeId::Vertex(anIt.Value()));
        theGraph.History().Record(TCollection_AsciiString("Compact:Remap"),
                                  BRepGraph_NodeId::Vertex(anIt.Key()),
                                  aRepl);
      }
    }
    for (NCollection_DataMap<int, int>::Iterator anIt(anEdgeMap); anIt.More(); anIt.Next())
    {
      if (anIt.Key() != anIt.Value())
      {
        NCollection_Vector<BRepGraph_NodeId> aRepl;
        aRepl.Append(BRepGraph_NodeId::Edge(anIt.Value()));
        theGraph.History().Record(TCollection_AsciiString("Compact:Remap"),
                                  BRepGraph_NodeId::Edge(anIt.Key()),
                                  aRepl);
      }
    }
    for (NCollection_DataMap<int, int>::Iterator anIt(aFaceMap); anIt.More(); anIt.Next())
    {
      if (anIt.Key() != anIt.Value())
      {
        NCollection_Vector<BRepGraph_NodeId> aRepl;
        aRepl.Append(BRepGraph_NodeId::Face(anIt.Value()));
        theGraph.History().Record(TCollection_AsciiString("Compact:Remap"),
                                  BRepGraph_NodeId::Face(anIt.Key()),
                                  aRepl);
      }
    }
  }

  // Construct a fresh graph and rebuild bottom-up.
  // Geometry nodes (Surface, Curve) are automatically created through AddFaceDef/AddEdgeDef.
  BRepGraph aNewGraph;

  // Helper lambda for remapping NodeIds.
  auto remapId = [&](const BRepGraph_NodeId& theId) -> BRepGraph_NodeId {
    return remapNodeId(theId,
                       aVertexMap,
                       anEdgeMap,
                       aWireMap,
                       aFaceMap,
                       aShellMap,
                       aSolidMap,
                       aCompoundMap,
                       aCompSolidMap);
  };

  // Add topology defs bottom-up (Vertex -> Edge -> Wire -> Face -> Shell -> Solid).
  // Vertices.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbVertices(); ++anIdx)
  {
    if (!aVertexMap.IsBound(anIdx))
      continue;
    const BRepGraph_TopoNode::VertexDef& anOldVtx = theGraph.Defs().Vertex(anIdx);
    aNewGraph.Builder().AddVertexDef(anOldVtx.Point, anOldVtx.Tolerance);
  }

  // Edges.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbEdges(); ++anIdx)
  {
    if (!anEdgeMap.IsBound(anIdx))
      continue;
    const BRepGraph_TopoNode::EdgeDef& anOldEdge = theGraph.Defs().Edge(anIdx);

    const BRepGraph_NodeId aNewStart = remapId(anOldEdge.StartVertexDefId());
    const BRepGraph_NodeId aNewEnd   = remapId(anOldEdge.EndVertexDefId());

    // Get the curve handle if available.
    Handle(Geom_Curve) aCurve = anOldEdge.Curve3d;

    BRepGraph_NodeId aNewEdgeId = aNewGraph.Builder().AddEdgeDef(aNewStart,
                                                                 aNewEnd,
                                                                 aCurve,
                                                                 anOldEdge.ParamFirst,
                                                                 anOldEdge.ParamLast,
                                                                 anOldEdge.Tolerance);

    // Copy edge properties.
    BRepGraph_MutRef<BRepGraph_TopoNode::EdgeDef> aNewEdge = aNewGraph.MutEdge(aNewEdgeId.Index);
    aNewEdge->IsDegenerate                 = anOldEdge.IsDegenerate;
    aNewEdge->SameParameter                = anOldEdge.SameParameter;
    aNewEdge->SameRange                    = anOldEdge.SameRange;
  }

  // PCurves (added after edges and faces are known).
  // We need faces first, so do PCurves after face creation.

  // Wires.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbWires(); ++anIdx)
  {
    if (!aWireMap.IsBound(anIdx))
      continue;
    const BRepGraph_TopoNode::WireDef& anOldWire = theGraph.Defs().Wire(anIdx);

    NCollection_Vector<std::pair<BRepGraph_NodeId, TopAbs_Orientation>> aNewEntries;
    for (int anEntryIdx = 0; anEntryIdx < anOldWire.EdgeRefs.Length(); ++anEntryIdx)
    {
      const BRepGraphInc::EdgeRef& aER = anOldWire.EdgeRefs.Value(anEntryIdx);
      const BRepGraph_NodeId aNewEdgeDefId = remapId(BRepGraph_NodeId::Edge(aER.EdgeIdx));
      if (aNewEdgeDefId.IsValid())
        aNewEntries.Append(std::make_pair(aNewEdgeDefId, aER.Orientation));
    }
    aNewGraph.Builder().AddWireDef(aNewEntries);
  }

  // Faces.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbFaces(); ++anIdx)
  {
    if (!aFaceMap.IsBound(anIdx))
      continue;
    const BRepGraph_TopoNode::FaceDef& anOldFace = theGraph.Defs().Face(anIdx);

    Handle(Geom_Surface) aSurf = anOldFace.Surface;

    // Find outer wire from incidence refs.
    BRepGraph_NodeId                     aNewOuterWire;
    NCollection_Vector<BRepGraph_NodeId> aNewInnerWires;

    for (int aWireRefIdx = 0; aWireRefIdx < anOldFace.WireRefs.Length(); ++aWireRefIdx)
    {
      const BRepGraphInc::WireRef& aWR = anOldFace.WireRefs.Value(aWireRefIdx);
      const BRepGraph_NodeId aRemapped = remapId(BRepGraph_NodeId::Wire(aWR.WireIdx));
      if (!aRemapped.IsValid())
        continue;
      if (aWR.IsOuter)
      {
        aNewOuterWire = aRemapped;
      }
      else
      {
        aNewInnerWires.Append(aRemapped);
      }
    }

    aNewGraph.Builder().AddFaceDef(aSurf, aNewOuterWire, aNewInnerWires, anOldFace.Tolerance);

    // Copy triangulations from old FaceDef to new FaceDef.
    BRepGraph_MutRef<BRepGraph_TopoNode::FaceDef> aNewFace = aNewGraph.Mut().FaceDef(aFaceMap.Find(anIdx));
    aNewFace->Triangulations          = anOldFace.Triangulations;
    aNewFace->ActiveTriangulationIndex = anOldFace.ActiveTriangulationIndex;
  }

  // Add PCurves to edges in the new graph.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbEdges(); ++anIdx)
  {
    if (!anEdgeMap.IsBound(anIdx))
      continue;
    const BRepGraph_TopoNode::EdgeDef& anOldEdge   = theGraph.Defs().Edge(anIdx);
    const int                          aNewEdgeIdx = anEdgeMap.Find(anIdx);

    for (int aPCIdx = 0; aPCIdx < anOldEdge.PCurves.Length(); ++aPCIdx)
    {
      const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry = anOldEdge.PCurves.Value(aPCIdx);
      if (aPCEntry.Curve2d.IsNull())
        continue;

      const BRepGraph_NodeId aNewFaceId = remapId(aPCEntry.FaceDefId);
      if (!aNewFaceId.IsValid())
        continue;

      aNewGraph.Mut().AddPCurveToEdge(BRepGraph_NodeId::Edge(aNewEdgeIdx),
                                      aNewFaceId,
                                      aPCEntry.Curve2d,
                                      aPCEntry.ParamFirst,
                                      aPCEntry.ParamLast,
                                      aPCEntry.EdgeOrientation);
    }
  }

  // Shells.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbShells(); ++anIdx)
  {
    if (!aShellMap.IsBound(anIdx))
      continue;

    BRepGraph_NodeId aNewShellId = aNewGraph.Builder().AddShellDef();

    // Add faces to shell via incidence refs.
    const BRepGraph_TopoNode::ShellDef& anOldShell = theGraph.Defs().Shell(anIdx);
    for (int aFaceRefIdx = 0; aFaceRefIdx < anOldShell.FaceRefs.Length(); ++aFaceRefIdx)
    {
      const BRepGraphInc::FaceRef& aFR = anOldShell.FaceRefs.Value(aFaceRefIdx);
      const BRepGraph_NodeId aNewFace = remapId(BRepGraph_NodeId::Face(aFR.FaceIdx));
      if (aNewFace.IsValid())
        aNewGraph.Builder().AddFaceToShell(aNewShellId, aNewFace, aFR.Orientation);
    }
  }

  // Solids.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbSolids(); ++anIdx)
  {
    if (!aSolidMap.IsBound(anIdx))
      continue;

    BRepGraph_NodeId aNewSolidId = aNewGraph.Builder().AddSolidDef();

    const BRepGraph_TopoNode::SolidDef& anOldSolid = theGraph.Defs().Solid(anIdx);
    for (int aShellRefIdx = 0; aShellRefIdx < anOldSolid.ShellRefs.Length(); ++aShellRefIdx)
    {
      const BRepGraphInc::ShellRef& aSR = anOldSolid.ShellRefs.Value(aShellRefIdx);
      const BRepGraph_NodeId aNewShell = remapId(BRepGraph_NodeId::Shell(aSR.ShellIdx));
      if (aNewShell.IsValid())
        aNewGraph.Builder().AddShellToSolid(aNewSolidId, aNewShell, aSR.Orientation);
    }
  }

  // Compounds.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbCompounds(); ++anIdx)
  {
    if (!aCompoundMap.IsBound(anIdx))
      continue;

    const BRepGraph_TopoNode::CompoundDef& anOldComp = theGraph.Defs().Compound(anIdx);
    NCollection_Vector<BRepGraph_NodeId>   aNewChildren;
    for (int aChildIdx = 0; aChildIdx < anOldComp.ChildRefs.Length(); ++aChildIdx)
    {
      const BRepGraphInc::ChildRef& aCR = anOldComp.ChildRefs.Value(aChildIdx);
      const BRepGraph_NodeId aNewChild =
        remapId(BRepGraph_NodeId(static_cast<BRepGraph_NodeId::Kind>(aCR.Kind), aCR.ChildIdx));
      if (aNewChild.IsValid())
        aNewChildren.Append(aNewChild);
    }
    aNewGraph.Builder().AddCompoundDef(aNewChildren);
  }

  // CompSolids.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbCompSolids(); ++anIdx)
  {
    if (!aCompSolidMap.IsBound(anIdx))
      continue;

    const BRepGraph_TopoNode::CompSolidDef& anOldCS = theGraph.Defs().CompSolid(anIdx);
    NCollection_Vector<BRepGraph_NodeId>    aNewSolids;
    for (int aSolidIdx = 0; aSolidIdx < anOldCS.SolidRefs.Length(); ++aSolidIdx)
    {
      const BRepGraph_NodeId aNewSolid =
        remapId(BRepGraph_NodeId::Solid(anOldCS.SolidRefs.Value(aSolidIdx).SolidIdx));
      if (aNewSolid.IsValid())
        aNewSolids.Append(aNewSolid);
    }
    aNewGraph.Builder().AddCompSolidDef(aNewSolids);
  }

  aResult.NbNodesAfter = static_cast<int>(aNewGraph.Defs().NbNodes());

  // Transfer per-kind UID vectors from old graph to new graph using index remap maps.
  // Each new graph's UID vector[newIdx] = old graph's UID vector[oldIdx].
  auto transferUIDs = [&](const NCollection_DataMap<int, int>&      theMap,
                          const NCollection_Vector<BRepGraph_UID>&  theOldVec,
                          NCollection_Vector<BRepGraph_UID>&        theNewVec) {
    // New vector was already populated by BuilderView during reconstruction.
    // Overwrite entries that have a mapping from the old graph.
    for (const auto& [anOldIdx, aNewIdx] : theMap.Items())
    {
      if (anOldIdx < theOldVec.Length() && theOldVec.Value(anOldIdx).IsValid())
      {
        theNewVec.ChangeValue(aNewIdx) = theOldVec.Value(anOldIdx);
      }
    }
  };

  transferUIDs(aVertexMap,    theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Vertex),    aNewGraph.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Vertex));
  transferUIDs(anEdgeMap,     theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Edge),      aNewGraph.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Edge));
  transferUIDs(aWireMap,      theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Wire),      aNewGraph.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Wire));
  transferUIDs(aFaceMap,      theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Face),      aNewGraph.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Face));
  transferUIDs(aShellMap,     theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Shell),     aNewGraph.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Shell));
  transferUIDs(aSolidMap,     theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Solid),     aNewGraph.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Solid));
  transferUIDs(aCompoundMap,  theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Compound),  aNewGraph.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Compound));
  transferUIDs(aCompSolidMap, theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::CompSolid), aNewGraph.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::CompSolid));

  aNewGraph.myData->myNextUIDCounter.store(
    theGraph.myData->myNextUIDCounter.load(std::memory_order_relaxed),
    std::memory_order_relaxed);
  aNewGraph.myData->myGeneration = theGraph.myData->myGeneration;
  aNewGraph.myData->myIsDone = true;

  // Save layers before swap (default move would transfer empty layers from aNewGraph).
  NCollection_DataMap<TCollection_AsciiString, Handle(BRepGraph_Layer)> aSavedLayers;
  aSavedLayers = std::move(theGraph.myLayers);

  // Swap.
  theGraph = std::move(aNewGraph);

  // Restore layers and notify about index remapping.
  theGraph.myLayers = std::move(aSavedLayers);

  // Build unified remap map covering all 8 topology kinds.
  NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId> aRemapMap;
  for (NCollection_DataMap<int, int>::Iterator it(aVertexMap); it.More(); it.Next())
    aRemapMap.Bind(BRepGraph_NodeId::Vertex(it.Key()), BRepGraph_NodeId::Vertex(it.Value()));
  for (NCollection_DataMap<int, int>::Iterator it(anEdgeMap); it.More(); it.Next())
    aRemapMap.Bind(BRepGraph_NodeId::Edge(it.Key()), BRepGraph_NodeId::Edge(it.Value()));
  for (NCollection_DataMap<int, int>::Iterator it(aWireMap); it.More(); it.Next())
    aRemapMap.Bind(BRepGraph_NodeId::Wire(it.Key()), BRepGraph_NodeId::Wire(it.Value()));
  for (NCollection_DataMap<int, int>::Iterator it(aFaceMap); it.More(); it.Next())
    aRemapMap.Bind(BRepGraph_NodeId::Face(it.Key()), BRepGraph_NodeId::Face(it.Value()));
  for (NCollection_DataMap<int, int>::Iterator it(aShellMap); it.More(); it.Next())
    aRemapMap.Bind(BRepGraph_NodeId::Shell(it.Key()), BRepGraph_NodeId::Shell(it.Value()));
  for (NCollection_DataMap<int, int>::Iterator it(aSolidMap); it.More(); it.Next())
    aRemapMap.Bind(BRepGraph_NodeId::Solid(it.Key()), BRepGraph_NodeId::Solid(it.Value()));
  for (NCollection_DataMap<int, int>::Iterator it(aCompoundMap); it.More(); it.Next())
    aRemapMap.Bind(BRepGraph_NodeId::Compound(it.Key()), BRepGraph_NodeId::Compound(it.Value()));
  for (NCollection_DataMap<int, int>::Iterator it(aCompSolidMap); it.More(); it.Next())
    aRemapMap.Bind(BRepGraph_NodeId::CompSolid(it.Key()), BRepGraph_NodeId::CompSolid(it.Value()));

  for (NCollection_DataMap<TCollection_AsciiString, Handle(BRepGraph_Layer)>::Iterator
         anIter(theGraph.myLayers); anIter.More(); anIter.Next())
  {
    anIter.Value()->OnCompact(aRemapMap);
  }

  BRepGraph_Mutator::CommitMutation(theGraph);
  theGraph.SetHistoryEnabled(wasHistoryEnabled);
  return aResult;
}
