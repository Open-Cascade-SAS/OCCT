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
#include <BRepGraphInc_Definition.hxx>

#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_Layer.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_Mutator.hxx>
#include <BRepGraph_MutRef.hxx>
#include <BRepGraph_UID.hxx>

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
      // Product/Occurrence kinds are not compacted - they reference topology
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

//! Iterate active coedge reference entries for a wire in append order.
template <typename FuncT>
void forWireCoEdgeRefEntries(const BRepGraph& theGraph, const BRepGraph_WireId theWireId, FuncT&& theFunc)
{
  const BRepGraphInc::WireDef& aWireEnt = theGraph.Topo().Wire(theWireId);
  const BRepGraph::RefsView&         aRefs    = theGraph.Refs();
  for (int i = 0; i < aWireEnt.CoEdgeRefIds.Length(); ++i)
  {
    const BRepGraph_CoEdgeRefId         aRefId = aWireEnt.CoEdgeRefIds.Value(i);
    const BRepGraphInc::CoEdgeRefEntry& aCR    = aRefs.CoEdge(aRefId);
    if (aCR.IsRemoved || !aCR.CoEdgeDefId.IsValid(theGraph.Topo().NbCoEdges()))
    {
      continue;
    }
    theFunc(aCR);
  }
}

//! Iterate active wire reference entries for a face in append order.
template <typename FuncT>
void forFaceWireRefEntries(const BRepGraph& theGraph, const BRepGraph_FaceId theFaceId, FuncT&& theFunc)
{
  const BRepGraphInc::FaceDef& aFaceEnt = theGraph.Topo().Face(theFaceId);
  const BRepGraph::RefsView&         aRefs    = theGraph.Refs();
  for (int i = 0; i < aFaceEnt.WireRefIds.Length(); ++i)
  {
    const BRepGraph_WireRefId         aRefId = aFaceEnt.WireRefIds.Value(i);
    const BRepGraphInc::WireRefEntry& aWR    = aRefs.Wire(aRefId);
    if (aWR.IsRemoved || !aWR.WireDefId.IsValid(theGraph.Topo().NbWires()))
    {
      continue;
    }
    theFunc(aWR);
  }
}

//! Iterate active face reference entries for a shell in append order.
template <typename FuncT>
void forShellFaceRefEntries(const BRepGraph& theGraph, const BRepGraph_ShellId theShellId, FuncT&& theFunc)
{
  const BRepGraphInc::ShellDef& aShellEnt = theGraph.Topo().Shell(theShellId);
  const BRepGraph::RefsView&          aRefs     = theGraph.Refs();
  for (int i = 0; i < aShellEnt.FaceRefIds.Length(); ++i)
  {
    const BRepGraph_FaceRefId         aRefId = aShellEnt.FaceRefIds.Value(i);
    const BRepGraphInc::FaceRefEntry& aFR    = aRefs.Face(aRefId);
    if (aFR.IsRemoved || !aFR.FaceDefId.IsValid(theGraph.Topo().NbFaces()))
    {
      continue;
    }
    theFunc(aFR);
  }
}

//! Iterate active shell reference entries for a solid in append order.
template <typename FuncT>
void forSolidShellRefEntries(const BRepGraph& theGraph, const BRepGraph_SolidId theSolidId, FuncT&& theFunc)
{
  const BRepGraphInc::SolidDef& aSolidEnt = theGraph.Topo().Solid(theSolidId);
  const BRepGraph::RefsView&          aRefs     = theGraph.Refs();
  for (int i = 0; i < aSolidEnt.ShellRefIds.Length(); ++i)
  {
    const BRepGraph_ShellRefId         aRefId = aSolidEnt.ShellRefIds.Value(i);
    const BRepGraphInc::ShellRefEntry& aSR    = aRefs.Shell(aRefId);
    if (aSR.IsRemoved || !aSR.ShellDefId.IsValid(theGraph.Topo().NbShells()))
    {
      continue;
    }
    theFunc(aSR);
  }
}

//! Iterate active child reference entries for a compound in append order.
template <typename FuncT>
void forCompoundChildRefEntries(const BRepGraph&          theGraph,
                                const BRepGraph_CompoundId theCompId,
                                FuncT&&                   theFunc)
{
  const BRepGraphInc::CompoundDef& aCompEnt = theGraph.Topo().Compound(theCompId);
  const BRepGraph::RefsView&             aRefs    = theGraph.Refs();
  for (int i = 0; i < aCompEnt.ChildRefIds.Length(); ++i)
  {
    const BRepGraph_ChildRefId         aRefId = aCompEnt.ChildRefIds.Value(i);
    const BRepGraphInc::ChildRefEntry& aCR    = aRefs.Child(aRefId);
    if (aCR.IsRemoved || !aCR.ChildDefId.IsValid())
    {
      continue;
    }
    theFunc(aCR);
  }
}

//! Iterate active solid reference entries for a compsolid in append order.
template <typename FuncT>
void forCompSolidSolidRefEntries(const BRepGraph&           theGraph,
                                 const BRepGraph_CompSolidId theCompSolidId,
                                 FuncT&&                    theFunc)
{
  const BRepGraphInc::CompSolidDef& aCSEnt = theGraph.Topo().CompSolid(theCompSolidId);
  const BRepGraph::RefsView&              aRefs  = theGraph.Refs();
  for (int i = 0; i < aCSEnt.SolidRefIds.Length(); ++i)
  {
    const BRepGraph_SolidRefId         aRefId = aCSEnt.SolidRefIds.Value(i);
    const BRepGraphInc::SolidRefEntry& aSR    = aRefs.Solid(aRefId);
    if (aSR.IsRemoved || !aSR.SolidDefId.IsValid(theGraph.Topo().NbSolids()))
    {
      continue;
    }
    theFunc(aSR);
  }
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
  for (int anIdx = 0; anIdx < theGraph.Topo().NbVertices(); ++anIdx)
    if (theGraph.Topo().Vertex(BRepGraph_VertexId(anIdx)).IsRemoved)
      ++aResult.NbRemovedVertices;
  for (int anIdx = 0; anIdx < theGraph.Topo().NbEdges(); ++anIdx)
    if (theGraph.Topo().Edge(BRepGraph_EdgeId(anIdx)).IsRemoved)
      ++aResult.NbRemovedEdges;
  for (int anIdx = 0; anIdx < theGraph.Topo().NbWires(); ++anIdx)
    if (theGraph.Topo().Wire(BRepGraph_WireId(anIdx)).IsRemoved)
      ++aResult.NbRemovedWires;
  for (int anIdx = 0; anIdx < theGraph.Topo().NbFaces(); ++anIdx)
    if (theGraph.Topo().Face(BRepGraph_FaceId(anIdx)).IsRemoved)
      ++aResult.NbRemovedFaces;
  for (int anIdx = 0; anIdx < theGraph.Topo().NbShells(); ++anIdx)
    if (theGraph.Topo().Shell(BRepGraph_ShellId(anIdx)).IsRemoved)
      ++aResult.NbRemovedShells;
  for (int anIdx = 0; anIdx < theGraph.Topo().NbSolids(); ++anIdx)
    if (theGraph.Topo().Solid(BRepGraph_SolidId(anIdx)).IsRemoved)
      ++aResult.NbRemovedSolids;
  for (int anIdx = 0; anIdx < theGraph.Topo().NbCompounds(); ++anIdx)
    if (theGraph.Topo().Compound(BRepGraph_CompoundId(anIdx)).IsRemoved)
      ++aResult.NbRemovedCompounds;
  for (int anIdx = 0; anIdx < theGraph.Topo().NbCompSolids(); ++anIdx)
    if (theGraph.Topo().CompSolid(BRepGraph_CompSolidId(anIdx)).IsRemoved)
      ++aResult.NbRemovedCompSolids;

  const int aTotalRemoved = aResult.NbRemovedVertices + aResult.NbRemovedEdges
                            + aResult.NbRemovedWires + aResult.NbRemovedFaces
                            + aResult.NbRemovedShells + aResult.NbRemovedSolids
                            + aResult.NbRemovedCompounds + aResult.NbRemovedCompSolids;

  aResult.NbNodesBefore = static_cast<int>(theGraph.Topo().NbNodes());

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
  for (int anIdx = 0; anIdx < theGraph.Topo().NbVertices(); ++anIdx)
    if (!theGraph.Topo().Vertex(BRepGraph_VertexId(anIdx)).IsRemoved)
      aVertexMap.Bind(anIdx, aNewIdx++);
  aNewIdx = 0;
  for (int anIdx = 0; anIdx < theGraph.Topo().NbEdges(); ++anIdx)
    if (!theGraph.Topo().Edge(BRepGraph_EdgeId(anIdx)).IsRemoved)
      anEdgeMap.Bind(anIdx, aNewIdx++);
  aNewIdx = 0;
  for (int anIdx = 0; anIdx < theGraph.Topo().NbWires(); ++anIdx)
    if (!theGraph.Topo().Wire(BRepGraph_WireId(anIdx)).IsRemoved)
      aWireMap.Bind(anIdx, aNewIdx++);
  aNewIdx = 0;
  for (int anIdx = 0; anIdx < theGraph.Topo().NbFaces(); ++anIdx)
    if (!theGraph.Topo().Face(BRepGraph_FaceId(anIdx)).IsRemoved)
      aFaceMap.Bind(anIdx, aNewIdx++);
  aNewIdx = 0;
  for (int anIdx = 0; anIdx < theGraph.Topo().NbShells(); ++anIdx)
    if (!theGraph.Topo().Shell(BRepGraph_ShellId(anIdx)).IsRemoved)
      aShellMap.Bind(anIdx, aNewIdx++);
  aNewIdx = 0;
  for (int anIdx = 0; anIdx < theGraph.Topo().NbSolids(); ++anIdx)
    if (!theGraph.Topo().Solid(BRepGraph_SolidId(anIdx)).IsRemoved)
      aSolidMap.Bind(anIdx, aNewIdx++);
  aNewIdx = 0;
  for (int anIdx = 0; anIdx < theGraph.Topo().NbCompounds(); ++anIdx)
    if (!theGraph.Topo().Compound(BRepGraph_CompoundId(anIdx)).IsRemoved)
      aCompoundMap.Bind(anIdx, aNewIdx++);
  aNewIdx = 0;
  for (int anIdx = 0; anIdx < theGraph.Topo().NbCompSolids(); ++anIdx)
    if (!theGraph.Topo().CompSolid(BRepGraph_CompSolidId(anIdx)).IsRemoved)
      aCompSolidMap.Bind(anIdx, aNewIdx++);

  const bool wasHistoryEnabled = theGraph.History().IsEnabled();
  theGraph.History().SetEnabled(theOptions.HistoryMode);
  const uint32_t aGeneration = theGraph.myData->myGeneration.load(std::memory_order_relaxed);

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
  // Geometry nodes (Surface, Curve) are automatically created through AddFace/AddEdge.
  BRepGraph aNewGraph;
  aNewGraph.myData->myGeneration.store(aGeneration, std::memory_order_relaxed);

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
  for (int anIdx = 0; anIdx < theGraph.Topo().NbVertices(); ++anIdx)
  {
    if (!aVertexMap.IsBound(anIdx))
      continue;
    const BRepGraphInc::VertexDef& anOldVtx =
      theGraph.Topo().Vertex(BRepGraph_VertexId(anIdx));
    (void)aNewGraph.Builder().AddVertex(anOldVtx.Point, anOldVtx.Tolerance);
  }

  // Edges.
  for (int anIdx = 0; anIdx < theGraph.Topo().NbEdges(); ++anIdx)
  {
    if (!anEdgeMap.IsBound(anIdx))
      continue;
    const BRepGraphInc::EdgeDef& anOldEdge = theGraph.Topo().Edge(BRepGraph_EdgeId(anIdx));

    const BRepGraph_NodeId aNewStart =
      anOldEdge.StartVertexRefId.IsValid()
        ? remapId(BRepGraph_Tool::Edge::StartVertex(theGraph, BRepGraph_EdgeId(anIdx)).VertexDefId)
        : BRepGraph_NodeId();
    const BRepGraph_NodeId aNewEnd =
      anOldEdge.EndVertexRefId.IsValid()
        ? remapId(BRepGraph_Tool::Edge::EndVertex(theGraph, BRepGraph_EdgeId(anIdx)).VertexDefId)
        : BRepGraph_NodeId();

    // Get the curve handle if available.
    const occ::handle<Geom_Curve>& aCurve =
      BRepGraph_Tool::Edge::Curve(theGraph, BRepGraph_EdgeId(anIdx));

    BRepGraph_NodeId aNewEdgeId = aNewGraph.Builder().AddEdge(aNewStart,
                                                                 aNewEnd,
                                                                 aCurve,
                                                                 anOldEdge.ParamFirst,
                                                                 anOldEdge.ParamLast,
                                                                 anOldEdge.Tolerance);

    // Copy edge properties.
    BRepGraph_MutRef<BRepGraphInc::EdgeDef> aNewEdge =
      aNewGraph.Builder().MutEdge(BRepGraph_EdgeId(aNewEdgeId.Index));
    aNewEdge->IsDegenerate  = anOldEdge.IsDegenerate;
    aNewEdge->SameParameter = anOldEdge.SameParameter;
    aNewEdge->SameRange     = anOldEdge.SameRange;
  }

  // PCurves (added after edges and faces are known).
  // We need faces first, so do PCurves after face creation.

  // Wires.
  for (int anIdx = 0; anIdx < theGraph.Topo().NbWires(); ++anIdx)
  {
    if (!aWireMap.IsBound(anIdx))
      continue;

    NCollection_Vector<std::pair<BRepGraph_NodeId, TopAbs_Orientation>> aNewEntries;
    forWireCoEdgeRefEntries(theGraph, BRepGraph_WireId(anIdx), [&](const BRepGraphInc::CoEdgeRefEntry& aCR) {
      const BRepGraphInc::CoEdgeDef& aCoEdge       = theGraph.Topo().CoEdge(aCR.CoEdgeDefId);
      const BRepGraph_NodeId               aNewEdgeDefId = remapId(aCoEdge.EdgeDefId);
      if (aNewEdgeDefId.IsValid())
        aNewEntries.Append(std::make_pair(aNewEdgeDefId, aCoEdge.Sense));
    });
    (void)aNewGraph.Builder().AddWire(aNewEntries);
  }

  // Faces.
  for (int anIdx = 0; anIdx < theGraph.Topo().NbFaces(); ++anIdx)
  {
    if (!aFaceMap.IsBound(anIdx))
      continue;
    const BRepGraphInc::FaceDef& anOldFace = theGraph.Topo().Face(BRepGraph_FaceId(anIdx));

    const occ::handle<Geom_Surface>& aSurf =
      BRepGraph_Tool::Face::Surface(theGraph, BRepGraph_FaceId(anIdx));

    // Find outer wire from transitional incidence entries.
    BRepGraph_NodeId                     aNewOuterWire;
    NCollection_Vector<BRepGraph_NodeId> aNewInnerWires;

    forFaceWireRefEntries(theGraph, BRepGraph_FaceId(anIdx), [&](const BRepGraphInc::WireRefEntry& aWR) {
      const BRepGraph_NodeId       aRemapped = remapId(aWR.WireDefId);
      if (!aRemapped.IsValid())
        return;
      if (aWR.IsOuter)
      {
        aNewOuterWire = aRemapped;
      }
      else
      {
        aNewInnerWires.Append(aRemapped);
      }
    });

    (void)aNewGraph.Builder().AddFace(aSurf, aNewOuterWire, aNewInnerWires, anOldFace.Tolerance);

    // Copy triangulations from old FaceDef to new FaceDef.
    BRepGraph_MutRef<BRepGraphInc::FaceDef> aNewFace =
      aNewGraph.Builder().MutFace(BRepGraph_FaceId(aFaceMap.Find(anIdx)));
    aNewFace->TriangulationRepIds      = anOldFace.TriangulationRepIds;
    aNewFace->ActiveTriangulationIndex = anOldFace.ActiveTriangulationIndex;
  }

  // Add PCurves to edges in the new graph via CoEdge data.
  for (int anIdx = 0; anIdx < theGraph.Topo().NbEdges(); ++anIdx)
  {
    if (!anEdgeMap.IsBound(anIdx))
      continue;
    const int aNewEdgeIdx = anEdgeMap.Find(anIdx);

    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIds =
      theGraph.Topo().CoEdgesOfEdge(BRepGraph_EdgeId(anIdx));
    for (int aCEIter = 0; aCEIter < aCoEdgeIds.Length(); ++aCEIter)
    {
      const BRepGraph_CoEdgeId             aCoEdgeId = aCoEdgeIds.Value(aCEIter);
      const BRepGraphInc::CoEdgeDef& aCoEdge   = theGraph.Topo().CoEdge(aCoEdgeId);
      if (!aCoEdge.Curve2DRepId.IsValid())
        continue;

      const BRepGraph_NodeId aNewFaceId = remapId(aCoEdge.FaceDefId);
      if (!aNewFaceId.IsValid())
        continue;

      const occ::handle<Geom2d_Curve>& aCompactPCurve =
        BRepGraph_Tool::CoEdge::PCurve(theGraph, aCoEdgeId);
      aNewGraph.Builder().AddPCurveToEdge(BRepGraph_NodeId::Edge(aNewEdgeIdx),
                                          aNewFaceId,
                                          aCompactPCurve,
                                          aCoEdge.ParamFirst,
                                          aCoEdge.ParamLast,
                                          aCoEdge.Sense);
    }
  }

  // Shells.
  for (int anIdx = 0; anIdx < theGraph.Topo().NbShells(); ++anIdx)
  {
    if (!aShellMap.IsBound(anIdx))
      continue;

    BRepGraph_NodeId aNewShellId = aNewGraph.Builder().AddShell();

    // Add faces to shell via transitional incidence entries.
    forShellFaceRefEntries(theGraph, BRepGraph_ShellId(anIdx), [&](const BRepGraphInc::FaceRefEntry& aFR) {
      const BRepGraph_NodeId       aNewFace = remapId(aFR.FaceDefId);
      if (aNewFace.IsValid())
        aNewGraph.Builder().AddFaceToShell(aNewShellId, aNewFace, aFR.Orientation);
    });
  }

  // Solids.
  for (int anIdx = 0; anIdx < theGraph.Topo().NbSolids(); ++anIdx)
  {
    if (!aSolidMap.IsBound(anIdx))
      continue;

    BRepGraph_NodeId aNewSolidId = aNewGraph.Builder().AddSolid();

    forSolidShellRefEntries(theGraph, BRepGraph_SolidId(anIdx), [&](const BRepGraphInc::ShellRefEntry& aSR) {
      const BRepGraph_NodeId        aNewShell = remapId(aSR.ShellDefId);
      if (aNewShell.IsValid())
        aNewGraph.Builder().AddShellToSolid(aNewSolidId, aNewShell, aSR.Orientation);
    });
  }

  // Compounds.
  for (int anIdx = 0; anIdx < theGraph.Topo().NbCompounds(); ++anIdx)
  {
    if (!aCompoundMap.IsBound(anIdx))
      continue;

    NCollection_Vector<BRepGraph_NodeId> aNewChildren;
    forCompoundChildRefEntries(theGraph, BRepGraph_CompoundId(anIdx), [&](const BRepGraphInc::ChildRefEntry& aCR) {
      const BRepGraph_NodeId        aNewChild = remapId(aCR.ChildDefId);
      if (aNewChild.IsValid())
        aNewChildren.Append(aNewChild);
    });
    (void)aNewGraph.Builder().AddCompound(aNewChildren);
  }

  // CompSolids.
  for (int anIdx = 0; anIdx < theGraph.Topo().NbCompSolids(); ++anIdx)
  {
    if (!aCompSolidMap.IsBound(anIdx))
      continue;

    NCollection_Vector<BRepGraph_NodeId> aNewSolids;
    forCompSolidSolidRefEntries(theGraph,
                                BRepGraph_CompSolidId(anIdx),
                                [&](const BRepGraphInc::SolidRefEntry& aSR) {
      const BRepGraph_NodeId aNewSolid = remapId(aSR.SolidDefId);
      if (aNewSolid.IsValid())
        aNewSolids.Append(aNewSolid);
    });
    (void)aNewGraph.Builder().AddCompSolid(aNewSolids);
  }

  // Rebuild reverse index from final forward incidence to guarantee compact output consistency.
  aNewGraph.myData->myIncStorage.BuildReverseIndex();

  aResult.NbNodesAfter = static_cast<int>(aNewGraph.Topo().NbNodes());

  // Transfer per-kind UID vectors from old graph to new graph using index remap maps.
  // Each new graph's UID vector[newIdx] = old graph's UID vector[oldIdx].
  auto transferUIDs = [&](const NCollection_DataMap<int, int>&     theMap,
                          const NCollection_Vector<BRepGraph_UID>& theOldVec,
                          NCollection_Vector<BRepGraph_UID>&       theNewVec) {
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

  transferUIDs(aVertexMap,
               theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Vertex),
               aNewGraph.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Vertex));
  transferUIDs(anEdgeMap,
               theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Edge),
               aNewGraph.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Edge));
  transferUIDs(aWireMap,
               theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Wire),
               aNewGraph.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Wire));
  transferUIDs(aFaceMap,
               theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Face),
               aNewGraph.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Face));
  transferUIDs(aShellMap,
               theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Shell),
               aNewGraph.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Shell));
  transferUIDs(aSolidMap,
               theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Solid),
               aNewGraph.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Solid));
  transferUIDs(aCompoundMap,
               theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::Compound),
               aNewGraph.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::Compound));
  transferUIDs(aCompSolidMap,
               theGraph.myData->myIncStorage.UIDs(BRepGraph_NodeId::Kind::CompSolid),
               aNewGraph.myData->myIncStorage.ChangeUIDs(BRepGraph_NodeId::Kind::CompSolid));

  aNewGraph.myData->myNextUIDCounter.store(
    theGraph.myData->myNextUIDCounter.load(std::memory_order_relaxed),
    std::memory_order_relaxed);
  aNewGraph.myData->myGeneration.store(
    theGraph.myData->myGeneration.load(std::memory_order_relaxed),
    std::memory_order_relaxed);
  aNewGraph.myData->myIsDone = true;

  // Save layers before swap (default move would transfer empty layers from aNewGraph).
  NCollection_DataMap<TCollection_AsciiString, occ::handle<BRepGraph_Layer>> aSavedLayers;
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

  for (NCollection_DataMap<TCollection_AsciiString, occ::handle<BRepGraph_Layer>>::Iterator anIter(
         theGraph.myLayers);
       anIter.More();
       anIter.Next())
  {
    anIter.Value()->OnCompact(aRemapMap);
  }

  BRepGraph_Mutator::CommitMutation(theGraph);
  theGraph.History().SetEnabled(wasHistoryEnabled);
  return aResult;
}
