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
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>

#include <BRepGraph_PathView.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_UIDsView.hxx>
#include <BRepGraph_WireExplorer.hxx>

#include <NCollection_DataMap.hxx>
#include <NCollection_Map.hxx>

namespace
{

//! Iterate active coedge reference entries for a wire in append order.
template <typename FuncT>
void forWireCoEdgeRefEntries(const BRepGraph&       theGraph,
                             const BRepGraph_WireId theWireId,
                             FuncT&&                theFunc)
{
  const BRepGraphInc::WireDef& aWireEnt = theGraph.Topo().Wire(theWireId);
  const BRepGraph::RefsView&   aRefs    = theGraph.Refs();
  for (int i = 0; i < aWireEnt.CoEdgeRefIds.Length(); ++i)
  {
    const BRepGraph_CoEdgeRefId    aRefId = aWireEnt.CoEdgeRefIds.Value(i);
    const BRepGraphInc::CoEdgeRef& aCR    = aRefs.CoEdge(aRefId);
    if (aCR.IsRemoved || !aCR.CoEdgeDefId.IsValid(theGraph.Topo().NbCoEdges()))
    {
      continue;
    }
    theFunc(aCR);
  }
}

//! Iterate active wire reference entries for a face in append order.
template <typename FuncT>
void forFaceWireRefEntries(const BRepGraph&       theGraph,
                           const BRepGraph_FaceId theFaceId,
                           FuncT&&                theFunc)
{
  const BRepGraphInc::FaceDef& aFaceEnt = theGraph.Topo().Face(theFaceId);
  const BRepGraph::RefsView&   aRefs    = theGraph.Refs();
  for (int i = 0; i < aFaceEnt.WireRefIds.Length(); ++i)
  {
    const BRepGraph_WireRefId    aRefId = aFaceEnt.WireRefIds.Value(i);
    const BRepGraphInc::WireRef& aWR    = aRefs.Wire(aRefId);
    if (aWR.IsRemoved || !aWR.WireDefId.IsValid(theGraph.Topo().NbWires()))
    {
      continue;
    }
    theFunc(aWR);
  }
}

//! Iterate active face reference entries for a shell in append order.
template <typename FuncT>
void forShellFaceRefEntries(const BRepGraph&        theGraph,
                            const BRepGraph_ShellId theShellId,
                            FuncT&&                 theFunc)
{
  const BRepGraphInc::ShellDef& aShellEnt = theGraph.Topo().Shell(theShellId);
  const BRepGraph::RefsView&    aRefs     = theGraph.Refs();
  for (int i = 0; i < aShellEnt.FaceRefIds.Length(); ++i)
  {
    const BRepGraph_FaceRefId    aRefId = aShellEnt.FaceRefIds.Value(i);
    const BRepGraphInc::FaceRef& aFR    = aRefs.Face(aRefId);
    if (aFR.IsRemoved || !aFR.FaceDefId.IsValid(theGraph.Topo().NbFaces()))
    {
      continue;
    }
    theFunc(aFR);
  }
}

//! Iterate active shell reference entries for a solid in append order.
template <typename FuncT>
void forSolidShellRefEntries(const BRepGraph&        theGraph,
                             const BRepGraph_SolidId theSolidId,
                             FuncT&&                 theFunc)
{
  const BRepGraphInc::SolidDef& aSolidEnt = theGraph.Topo().Solid(theSolidId);
  const BRepGraph::RefsView&    aRefs     = theGraph.Refs();
  for (int i = 0; i < aSolidEnt.ShellRefIds.Length(); ++i)
  {
    const BRepGraph_ShellRefId    aRefId = aSolidEnt.ShellRefIds.Value(i);
    const BRepGraphInc::ShellRef& aSR    = aRefs.Shell(aRefId);
    if (aSR.IsRemoved || !aSR.ShellDefId.IsValid(theGraph.Topo().NbShells()))
    {
      continue;
    }
    theFunc(aSR);
  }
}

//! Iterate active child reference entries for a compound in append order.
template <typename FuncT>
void forCompoundChildRefEntries(const BRepGraph&           theGraph,
                                const BRepGraph_CompoundId theCompId,
                                FuncT&&                    theFunc)
{
  const BRepGraphInc::CompoundDef& aCompEnt = theGraph.Topo().Compound(theCompId);
  const BRepGraph::RefsView&       aRefs    = theGraph.Refs();
  for (int i = 0; i < aCompEnt.ChildRefIds.Length(); ++i)
  {
    const BRepGraph_ChildRefId    aRefId = aCompEnt.ChildRefIds.Value(i);
    const BRepGraphInc::ChildRef& aCR    = aRefs.Child(aRefId);
    if (aCR.IsRemoved || !aCR.ChildDefId.IsValid())
    {
      continue;
    }
    theFunc(aCR);
  }
}

//! Iterate active solid reference entries for a compsolid in append order.
template <typename FuncT>
void forCompSolidSolidRefEntries(const BRepGraph&            theGraph,
                                 const BRepGraph_CompSolidId theCompSolidId,
                                 FuncT&&                     theFunc)
{
  const BRepGraphInc::CompSolidDef& aCSEnt = theGraph.Topo().CompSolid(theCompSolidId);
  const BRepGraph::RefsView&        aRefs  = theGraph.Refs();
  for (int i = 0; i < aCSEnt.SolidRefIds.Length(); ++i)
  {
    const BRepGraph_SolidRefId    aRefId = aCSEnt.SolidRefIds.Value(i);
    const BRepGraphInc::SolidRef& aSR    = aRefs.Solid(aRefId);
    if (aSR.IsRemoved || !aSR.SolidDefId.IsValid(theGraph.Topo().NbSolids()))
    {
      continue;
    }
    theFunc(aSR);
  }
}

//! Check that a NodeId refers to a valid index within graph bounds.
bool isValidNodeId(const BRepGraph& theGraph, const BRepGraph_NodeId theId)
{
  if (!theId.IsValid())
    return false;

  switch (theId.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Solid:
      return theId.IsValid(theGraph.Topo().NbSolids());
    case BRepGraph_NodeId::Kind::Shell:
      return theId.IsValid(theGraph.Topo().NbShells());
    case BRepGraph_NodeId::Kind::Face:
      return theId.IsValid(theGraph.Topo().NbFaces());
    case BRepGraph_NodeId::Kind::Wire:
      return theId.IsValid(theGraph.Topo().NbWires());
    case BRepGraph_NodeId::Kind::CoEdge:
      return theId.IsValid(theGraph.Topo().NbCoEdges());
    case BRepGraph_NodeId::Kind::Edge:
      return theId.IsValid(theGraph.Topo().NbEdges());
    case BRepGraph_NodeId::Kind::Vertex:
      return theId.IsValid(theGraph.Topo().NbVertices());
    case BRepGraph_NodeId::Kind::Compound:
      return theId.IsValid(theGraph.Topo().NbCompounds());
    case BRepGraph_NodeId::Kind::CompSolid:
      return theId.IsValid(theGraph.Topo().NbCompSolids());
    case BRepGraph_NodeId::Kind::Product:
      return theId.IsValid(theGraph.Topo().NbProducts());
    case BRepGraph_NodeId::Kind::Occurrence:
      return theId.IsValid(theGraph.Topo().NbOccurrences());
  }
  return false;
}

//! Check if a topology def is removed.
bool isEntityRemoved(const BRepGraph& theGraph, BRepGraph_NodeId theId)
{
  const BRepGraphInc::BaseDef* aDef = theGraph.Topo().TopoEntity(theId);
  return aDef != nullptr && aDef->IsRemoved;
}

//! Convert mutator boundary issues to validator issues.
//! @param[in]     theBoundaryIssues boundary issues reported by mutator
//! @param[in,out] theIssues         destination validator issue vector
void appendMutationBoundaryIssues(
  const NCollection_Vector<BRepGraph::BuilderView::BoundaryIssue>& theBoundaryIssues,
  NCollection_Vector<BRepGraphAlgo_Validate::Issue>&               theIssues)
{
  using Issue    = BRepGraphAlgo_Validate::Issue;
  using Severity = BRepGraphAlgo_Validate::Severity;
  for (int anIdx = 0; anIdx < theBoundaryIssues.Length(); ++anIdx)
  {
    const BRepGraph::BuilderView::BoundaryIssue& aBoundaryIssue =
      theBoundaryIssues.Value(anIdx);
    theIssues.Append(Issue{Severity::Error, aBoundaryIssue.NodeId, aBoundaryIssue.Description});
  }
}

//! Validate that all cross-reference indices (vertex, edge, wire, face, shell,
//! coedge, compound child refs) point to valid, in-bounds entity slots.
//! @param[in]     theGraph  source graph
//! @param[in,out] theIssues collection to append diagnostic issues
void checkCrossReferenceBounds(const BRepGraph&                                   theGraph,
                               NCollection_Vector<BRepGraphAlgo_Validate::Issue>& theIssues)
{
  using Issue    = BRepGraphAlgo_Validate::Issue;
  using Severity = BRepGraphAlgo_Validate::Severity;

  // Check EdgeDef references.
  for (int anEdgeIdx = 0; anEdgeIdx < theGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraphInc::EdgeDef& anEdge = theGraph.Topo().Edge(BRepGraph_EdgeId(anEdgeIdx));
    if (anEdge.IsRemoved)
      continue;

    // Resolve vertex def ids through vertex ref entries.
    if (anEdge.StartVertexRefId.IsValid())
    {
      const BRepGraph_NodeId aStartVtxId =
        theGraph.Refs().Vertex(anEdge.StartVertexRefId).VertexDefId;
      if (aStartVtxId.IsValid() && !isValidNodeId(theGraph, aStartVtxId))
      {
        theIssues.Append(Issue{Severity::Error,
                               anEdge.Id,
                               "EdgeDef.StartVertexRefId resolves to out-of-bounds VertexDefId"});
      }
    }
    if (anEdge.EndVertexRefId.IsValid())
    {
      const BRepGraph_NodeId anEndVtxId = theGraph.Refs().Vertex(anEdge.EndVertexRefId).VertexDefId;
      if (anEndVtxId.IsValid() && !isValidNodeId(theGraph, anEndVtxId))
      {
        theIssues.Append(Issue{Severity::Error,
                               anEdge.Id,
                               "EdgeDef.EndVertexRefId resolves to out-of-bounds VertexDefId"});
      }
    }
  }

  // Check CoEdgeDef references.
  for (int aCoEdgeIdx = 0; aCoEdgeIdx < theGraph.Topo().NbCoEdges(); ++aCoEdgeIdx)
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdge(BRepGraph_CoEdgeId(aCoEdgeIdx));
    if (aCoEdge.IsRemoved)
      continue;

    if (aCoEdge.FaceDefId.IsValid() && !isValidNodeId(theGraph, aCoEdge.FaceDefId))
    {
      theIssues.Append(Issue{Severity::Error, aCoEdge.Id, "CoEdgeDef.FaceDefId out of bounds"});
    }
    const BRepGraph_NodeId anEdgeId = aCoEdge.EdgeDefId;
    if (anEdgeId.IsValid() && !isValidNodeId(theGraph, anEdgeId))
    {
      theIssues.Append(Issue{Severity::Error, aCoEdge.Id, "CoEdgeDef.EdgeDefId out of bounds"});
    }
    // SeamPairId consistency.
    if (aCoEdge.SeamPairId.IsValid())
    {
      if (!aCoEdge.SeamPairId.IsValid(theGraph.Topo().NbCoEdges()))
      {
        theIssues.Append(Issue{Severity::Error, aCoEdge.Id, "CoEdgeDef.SeamPairId out of bounds"});
      }
      else if (aCoEdge.SeamPairId.Index == aCoEdgeIdx)
      {
        theIssues.Append(
          Issue{Severity::Error, aCoEdge.Id, "CoEdgeDef.SeamPairId is self-reference"});
      }
      else
      {
        const BRepGraphInc::CoEdgeDef& aPair = theGraph.Topo().CoEdge(aCoEdge.SeamPairId);
        if (aPair.SeamPairId.Index != aCoEdgeIdx)
        {
          theIssues.Append(
            Issue{Severity::Error, aCoEdge.Id, "CoEdgeDef.SeamPairId not bidirectional"});
        }
        if (aPair.FaceDefId != aCoEdge.FaceDefId)
        {
          theIssues.Append(
            Issue{Severity::Error, aCoEdge.Id, "CoEdgeDef seam pair has different FaceDefId"});
        }
      }
    }
    // Rep index bounds.
    if (aCoEdge.Curve2DRepId.IsValid()
        && !aCoEdge.Curve2DRepId.IsValid(theGraph.Topo().NbCurves2D()))
    {
      theIssues.Append(Issue{Severity::Error, aCoEdge.Id, "CoEdgeDef.Curve2DRepId out of bounds"});
    }
  }

  // Check FaceDef references.
  for (int aFaceIdx = 0; aFaceIdx < theGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    const BRepGraphInc::FaceDef& aFace = theGraph.Topo().Face(BRepGraph_FaceId(aFaceIdx));
    if (aFace.IsRemoved)
      continue;

    // Surface handle is stored directly on FaceDef; no cross-reference to validate.
  }

  // Check WireDef CoEdgeRef references.
  for (int aWireIdx = 0; aWireIdx < theGraph.Topo().NbWires(); ++aWireIdx)
  {
    const BRepGraphInc::WireDef& aWire = theGraph.Topo().Wire(BRepGraph_WireId(aWireIdx));
    if (aWire.IsRemoved)
      continue;

    forWireCoEdgeRefEntries(
      theGraph,
      BRepGraph_WireId(aWireIdx),
      [&](const BRepGraphInc::CoEdgeRef& aCR) {
        const BRepGraph_NodeId aCoEdgeDefId = aCR.CoEdgeDefId;
        if (aCoEdgeDefId.IsValid() && !isValidNodeId(theGraph, aCoEdgeDefId))
        {
          theIssues.Append(
            Issue{Severity::Error, aWire.Id, "WireDef.CoEdgeUsage CoEdgeIdx out of bounds"});
        }
        const BRepGraphInc::CoEdgeDef& aCoEdge     = theGraph.Topo().CoEdge(aCR.CoEdgeDefId);
        const BRepGraph_NodeId         anEdgeDefId = aCoEdge.EdgeDefId;
        if (anEdgeDefId.IsValid() && !isValidNodeId(theGraph, anEdgeDefId))
        {
          theIssues.Append(
            Issue{Severity::Error, aWire.Id, "WireDef.CoEdgeUsage EdgeIdx out of bounds"});
        }
      });
  }

  // Check CompoundDef ChildRef references.
  for (int aCompIdx = 0; aCompIdx < theGraph.Topo().NbCompounds(); ++aCompIdx)
  {
    const BRepGraphInc::CompoundDef& aComp =
      theGraph.Topo().Compound(BRepGraph_CompoundId(aCompIdx));
    if (aComp.IsRemoved)
      continue;

    forCompoundChildRefEntries(theGraph,
                               BRepGraph_CompoundId(aCompIdx),
                               [&](const BRepGraphInc::ChildRef& aCR) {
                                 const BRepGraph_NodeId aChildId = aCR.ChildDefId;
                                 if (aChildId.IsValid() && !isValidNodeId(theGraph, aChildId))
                                 {
                                   theIssues.Append(Issue{Severity::Error,
                                                          aComp.Id,
                                                          "CompoundDef.ChildDefId out of bounds"});
                                 }
                               });
  }

  // Check CompSolidDef SolidRef references.
  for (int aCSIdx = 0; aCSIdx < theGraph.Topo().NbCompSolids(); ++aCSIdx)
  {
    const BRepGraphInc::CompSolidDef& aCS =
      theGraph.Topo().CompSolid(BRepGraph_CompSolidId(aCSIdx));
    if (aCS.IsRemoved)
      continue;

    forCompSolidSolidRefEntries(
      theGraph,
      BRepGraph_CompSolidId(aCSIdx),
      [&](const BRepGraphInc::SolidRef& aSR) {
        const BRepGraph_NodeId aSolidId = aSR.SolidDefId;
        if (aSolidId.IsValid() && !isValidNodeId(theGraph, aSolidId))
        {
          theIssues.Append(Issue{Severity::Error, aCS.Id, "CompSolidDef.SolidDefId out of bounds"});
        }
      });
  }

  // Check ShellDef FaceRef references.
  for (int aShellIdx = 0; aShellIdx < theGraph.Topo().NbShells(); ++aShellIdx)
  {
    const BRepGraphInc::ShellDef& aShell = theGraph.Topo().Shell(BRepGraph_ShellId(aShellIdx));
    if (aShell.IsRemoved)
      continue;

    forShellFaceRefEntries(theGraph,
                           BRepGraph_ShellId(aShellIdx),
                           [&](const BRepGraphInc::FaceRef& aFR) {
                             const BRepGraph_NodeId aFaceDefId = aFR.FaceDefId;
                             if (aFaceDefId.IsValid() && !isValidNodeId(theGraph, aFaceDefId))
                             {
                               theIssues.Append(Issue{Severity::Error,
                                                      aShell.Id,
                                                      "ShellDef.FaceUsage FaceIdx out of bounds"});
                             }
                           });
  }

  // Check SolidDef ShellRef references.
  for (int aSolidIdx = 0; aSolidIdx < theGraph.Topo().NbSolids(); ++aSolidIdx)
  {
    const BRepGraphInc::SolidDef& aSolid = theGraph.Topo().Solid(BRepGraph_SolidId(aSolidIdx));
    if (aSolid.IsRemoved)
      continue;

    forSolidShellRefEntries(
      theGraph,
      BRepGraph_SolidId(aSolidIdx),
      [&](const BRepGraphInc::ShellRef& aSR) {
        const BRepGraph_NodeId aShellDefId = aSR.ShellDefId;
        if (aShellDefId.IsValid() && !isValidNodeId(theGraph, aShellDefId))
        {
          theIssues.Append(
            Issue{Severity::Error, aSolid.Id, "SolidDef.ShellUsage ShellIdx out of bounds"});
        }
      });
  }

  // Check ProductDef references.
  for (int aProductIdx = 0; aProductIdx < theGraph.Topo().NbProducts(); ++aProductIdx)
  {
    const BRepGraphInc::ProductDef& aProduct =
      theGraph.Topo().Product(BRepGraph_ProductId(aProductIdx));
    if (aProduct.IsRemoved)
      continue;

    if (aProduct.ShapeRootId.IsValid())
    {
      if (!isValidNodeId(theGraph, aProduct.ShapeRootId))
      {
        theIssues.Append(
          Issue{Severity::Error, aProduct.Id, "ProductDef.ShapeRootId out of bounds"});
      }
      else if (aProduct.ShapeRootId.NodeKind == BRepGraph_NodeId::Kind::CoEdge
               || BRepGraph_NodeId::IsAssemblyKind(aProduct.ShapeRootId.NodeKind))
      {
        theIssues.Append(
          Issue{Severity::Error, aProduct.Id, "ProductDef.ShapeRootId has invalid node kind"});
      }
    }

    const BRepGraph_ProductId aProdId(aProductIdx);
    const int                 aNbComponents = theGraph.Topo().NbComponents(aProdId);
    for (int anOccRefIdx = 0; anOccRefIdx < aNbComponents; ++anOccRefIdx)
    {
      const BRepGraph_NodeId anOccId = theGraph.Topo().Component(aProdId, anOccRefIdx);
      if (anOccId.IsValid() && !isValidNodeId(theGraph, anOccId))
      {
        theIssues.Append(
          Issue{Severity::Error, aProduct.Id, "ProductDef.OccurrenceUsage out of bounds"});
      }
    }
  }

  // Check OccurrenceDef references.
  for (int anOccIdx = 0; anOccIdx < theGraph.Topo().NbOccurrences(); ++anOccIdx)
  {
    const BRepGraphInc::OccurrenceDef& anOcc =
      theGraph.Topo().Occurrence(BRepGraph_OccurrenceId(anOccIdx));
    if (anOcc.IsRemoved)
      continue;

    if (!anOcc.ProductDefId.IsValid() || !isValidNodeId(theGraph, anOcc.ProductDefId))
    {
      theIssues.Append(Issue{Severity::Error, anOcc.Id, "OccurrenceDef.ProductDefId invalid"});
    }
    if (!anOcc.ParentProductDefId.IsValid() || !isValidNodeId(theGraph, anOcc.ParentProductDefId))
    {
      theIssues.Append(
        Issue{Severity::Error, anOcc.Id, "OccurrenceDef.ParentProductDefId invalid"});
    }

    if (anOcc.ParentOccurrenceDefId.IsValid())
    {
      if (!isValidNodeId(theGraph, anOcc.ParentOccurrenceDefId))
      {
        theIssues.Append(
          Issue{Severity::Error, anOcc.Id, "OccurrenceDef.ParentOccurrenceDefId invalid"});
      }
      else if (anOcc.ParentOccurrenceDefId.Index == anOccIdx)
      {
        theIssues.Append(
          Issue{Severity::Error, anOcc.Id, "OccurrenceDef.ParentOccurrenceDefId self-reference"});
      }
    }
  }
}

//! Verify that every forward incidence ref has a matching reverse-index entry
//! (edge->wires, edge->faces, vertex->edges, wire->faces, face->shells, shell->solids).
//! @param[in]     theGraph  source graph
//! @param[in,out] theIssues collection to append diagnostic issues
void checkReverseIndexConsistency(const BRepGraph&                                   theGraph,
                                  NCollection_Vector<BRepGraphAlgo_Validate::Issue>& theIssues)
{
  using Issue    = BRepGraphAlgo_Validate::Issue;
  using Severity = BRepGraphAlgo_Validate::Severity;

  // Build expected edge->wires mapping from CoEdgeRef scans.
  NCollection_DataMap<int, NCollection_Map<int>> anExpected;
  for (int aWireIdx = 0; aWireIdx < theGraph.Topo().NbWires(); ++aWireIdx)
  {
    const BRepGraphInc::WireDef& aWire = theGraph.Topo().Wire(BRepGraph_WireId(aWireIdx));
    if (aWire.IsRemoved)
      continue;

    forWireCoEdgeRefEntries(theGraph,
                            BRepGraph_WireId(aWireIdx),
                            [&](const BRepGraphInc::CoEdgeRef& aCR) {
                              const BRepGraphInc::CoEdgeDef& aCoEdge =
                                theGraph.Topo().CoEdge(aCR.CoEdgeDefId);
                              if (!aCoEdge.EdgeDefId.IsValid())
                                return;

                              if (!anExpected.IsBound(aCoEdge.EdgeDefId.Index))
                                anExpected.Bind(aCoEdge.EdgeDefId.Index, NCollection_Map<int>());
                              anExpected.ChangeFind(aCoEdge.EdgeDefId.Index).Add(aWireIdx);
                            });
  }

  // Check that Topo WiresOfEdge matches expected.
  for (int anEdgeIdx = 0; anEdgeIdx < theGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_EdgeId       anEdgeId(anEdgeIdx);
    const BRepGraphInc::EdgeDef& anEdge = theGraph.Topo().Edge(anEdgeId);
    if (anEdge.IsRemoved)
      continue;

    const NCollection_Vector<BRepGraph_WireId>& aActualWires =
      theGraph.Topo().WiresOfEdge(anEdgeId);
    const NCollection_Map<int>* anExpectedWires = anExpected.Seek(anEdgeIdx);

    const int anExpectedCount = (anExpectedWires != nullptr) ? anExpectedWires->Extent() : 0;

    // Build a set from actual wires for comparison.
    NCollection_Map<int> anActualSet;
    for (int aWIdx = 0; aWIdx < aActualWires.Length(); ++aWIdx)
      anActualSet.Add(aActualWires.Value(aWIdx).Index);

    if (anActualSet.Extent() != anExpectedCount)
    {
      theIssues.Append(
        Issue{Severity::Error, anEdge.Id, "Reverse index ReverseIdx.WiresOfEdge size mismatch"});
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

//! Validate consistency of cached edge-face counts in reverse index.
//! @param[in]     theGraph  source graph
//! @param[in,out] theIssues collection to append diagnostic issues
void checkReverseIndexFaceCountCache(const BRepGraph&                                   theGraph,
                                     NCollection_Vector<BRepGraphAlgo_Validate::Issue>& theIssues)
{
  using Issue                      = BRepGraphAlgo_Validate::Issue;
  using Severity                   = BRepGraphAlgo_Validate::Severity;
  const BRepGraph::TopoView& aDefs = theGraph.Topo();

  for (int anEdgeIdx = 0; anEdgeIdx < aDefs.NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_EdgeId       anEdgeId(anEdgeIdx);
    const BRepGraphInc::EdgeDef& anEdge = aDefs.Edge(anEdgeId);
    if (anEdge.IsRemoved)
      continue;

    NCollection_Map<int>                          aUniqueFaces;
    const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdges = aDefs.CoEdgesOfEdge(anEdgeId);
    for (int aCoEdgeIdx = 0; aCoEdgeIdx < aCoEdges.Length(); ++aCoEdgeIdx)
    {
      const BRepGraphInc::CoEdgeDef& aCoEdge = aDefs.CoEdge(aCoEdges.Value(aCoEdgeIdx));
      if (aCoEdge.IsRemoved || !aCoEdge.FaceDefId.IsValid())
      {
        continue;
      }
      aUniqueFaces.Add(aCoEdge.FaceDefId.Index);
    }

    const int aCachedCount  = aDefs.NbFacesOfEdge(anEdgeId);
    const int anActualCount = aUniqueFaces.Extent();
    if (aCachedCount != anActualCount)
    {
      TCollection_AsciiString aDesc("Reverse index face-count cache mismatch: cached=");
      aDesc += TCollection_AsciiString(aCachedCount);
      aDesc += " actual=";
      aDesc += TCollection_AsciiString(anActualCount);
      theIssues.Append(Issue{Severity::Error, anEdge.Id, aDesc});
    }
  }
}

//! Check that parent-child incidence refs (shell->face, solid->shell,
//! compound->child, compsolid->solid) reference non-removed entities
//! of the expected kind.
//! @param[in]     theGraph  source graph
//! @param[in,out] theIssues collection to append diagnostic issues
void checkIncidenceRefConsistency(const BRepGraph&                                   theGraph,
                                  NCollection_Vector<BRepGraphAlgo_Validate::Issue>& theIssues)
{
  using Issue    = BRepGraphAlgo_Validate::Issue;
  using Severity = BRepGraphAlgo_Validate::Severity;

  // Check face->wire incidence refs.
  for (int aFaceIdx = 0; aFaceIdx < theGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    const BRepGraphInc::FaceDef& aFace = theGraph.Topo().Face(BRepGraph_FaceId(aFaceIdx));
    if (aFace.IsRemoved)
      continue;

    forFaceWireRefEntries(theGraph,
                          BRepGraph_FaceId(aFaceIdx),
                          [&](const BRepGraphInc::WireRef& aWR) {
                            const BRepGraph_NodeId aWireId = aWR.WireDefId;
                            if (aWireId.IsValid() && !isValidNodeId(theGraph, aWireId))
                            {
                              theIssues.Append(Issue{Severity::Error,
                                                     aFace.Id,
                                                     "FaceDef.WireUsage WireIdx out of bounds"});
                            }
                          });
  }

  // Check shell->face incidence refs.
  for (int aShellIdx = 0; aShellIdx < theGraph.Topo().NbShells(); ++aShellIdx)
  {
    const BRepGraphInc::ShellDef& aShell = theGraph.Topo().Shell(BRepGraph_ShellId(aShellIdx));
    if (aShell.IsRemoved)
      continue;

    forShellFaceRefEntries(theGraph,
                           BRepGraph_ShellId(aShellIdx),
                           [&](const BRepGraphInc::FaceRef& aFR) {
                             const BRepGraph_NodeId aFaceId = aFR.FaceDefId;
                             if (aFaceId.IsValid() && isEntityRemoved(theGraph, aFaceId))
                             {
                               theIssues.Append(Issue{Severity::Error,
                                                      aShell.Id,
                                                      "ShellDef references removed FaceDef"});
                             }
                           });
  }

  // Check solid->shell incidence refs.
  for (int aSolidIdx = 0; aSolidIdx < theGraph.Topo().NbSolids(); ++aSolidIdx)
  {
    const BRepGraphInc::SolidDef& aSolid = theGraph.Topo().Solid(BRepGraph_SolidId(aSolidIdx));
    if (aSolid.IsRemoved)
      continue;

    forSolidShellRefEntries(theGraph,
                            BRepGraph_SolidId(aSolidIdx),
                            [&](const BRepGraphInc::ShellRef& aSR) {
                              const BRepGraph_NodeId aShellId = aSR.ShellDefId;
                              if (aShellId.IsValid() && isEntityRemoved(theGraph, aShellId))
                              {
                                theIssues.Append(Issue{Severity::Error,
                                                       aSolid.Id,
                                                       "SolidDef references removed ShellDef"});
                              }
                            });
  }
}

//! Validate that geometry representation ids (SurfaceRepId, Curve3DRepId,
//! Curve2DRepId, TriangulationRepIds, Polygon3DRepId) are in bounds and
//! reference non-null geometry handles.
//! @param[in]     theGraph  source graph
//! @param[in,out] theIssues collection to append diagnostic issues
void checkGeometryReferences(const BRepGraph&                                   theGraph,
                             NCollection_Vector<BRepGraphAlgo_Validate::Issue>& theIssues)
{
  using Issue    = BRepGraphAlgo_Validate::Issue;
  using Severity = BRepGraphAlgo_Validate::Severity;

  // Check edge->curve references (handles stored directly on EdgeDef).
  for (int anEdgeIdx = 0; anEdgeIdx < theGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_EdgeId       anEdgeId(anEdgeIdx);
    const BRepGraphInc::EdgeDef& anEdge = theGraph.Topo().Edge(anEdgeId);
    if (anEdge.IsRemoved)
      continue;

    if (!BRepGraph_Tool::Edge::Degenerated(theGraph, anEdgeId)
        && !BRepGraph_Tool::Edge::HasCurve(theGraph, anEdgeId))
    {
      theIssues.Append(
        Issue{Severity::Error, anEdge.Id, "Non-degenerate EdgeDef has no Curve3D representation"});
    }
    if (anEdge.Curve3DRepId.IsValid() && !anEdge.Curve3DRepId.IsValid(theGraph.Topo().NbCurves3D()))
    {
      theIssues.Append(Issue{Severity::Error, anEdge.Id, "EdgeDef.Curve3DRepId out of bounds"});
    }
    if (anEdge.Polygon3DRepId.IsValid()
        && !anEdge.Polygon3DRepId.IsValid(theGraph.Topo().NbPolygons3D()))
    {
      theIssues.Append(Issue{Severity::Error, anEdge.Id, "EdgeDef.Polygon3DRepId out of bounds"});
    }
  }

  // Check face rep indices.
  for (int aFaceIdx = 0; aFaceIdx < theGraph.Topo().NbFaces(); ++aFaceIdx)
  {
    const BRepGraphInc::FaceDef& aFace = theGraph.Topo().Face(BRepGraph_FaceId(aFaceIdx));
    if (aFace.IsRemoved)
      continue;
    if (aFace.SurfaceRepId.IsValid() && !aFace.SurfaceRepId.IsValid(theGraph.Topo().NbSurfaces()))
    {
      theIssues.Append(Issue{Severity::Error, aFace.Id, "FaceDef.SurfaceRepId out of bounds"});
    }
    for (int aTriIdx = 0; aTriIdx < aFace.TriangulationRepIds.Length(); ++aTriIdx)
    {
      if (aFace.TriangulationRepIds.Value(aTriIdx).Index >= theGraph.Topo().NbTriangulations())
      {
        theIssues.Append(
          Issue{Severity::Error, aFace.Id, "FaceDef.TriangulationRepId out of bounds"});
        break;
      }
    }
  }

  // Check CoEdge PCurve data.
  for (int aCoEdgeIdx = 0; aCoEdgeIdx < theGraph.Topo().NbCoEdges(); ++aCoEdgeIdx)
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdge(BRepGraph_CoEdgeId(aCoEdgeIdx));
    if (aCoEdge.IsRemoved)
      continue;

    if (aCoEdge.FaceDefId.IsValid()
        && !BRepGraph_Tool::CoEdge::HasPCurve(theGraph, BRepGraph_CoEdgeId(aCoEdgeIdx)))
    {
      theIssues.Append(
        Issue{Severity::Error, aCoEdge.Id, "CoEdgeDef has no Curve2D representation"});
    }
  }
}

//! Verify that removed entities are not referenced by any active (non-removed)
//! parent entity through forward incidence refs.
//! @param[in]     theGraph  source graph
//! @param[in,out] theIssues collection to append diagnostic issues
void checkRemovedNodeIsolation(const BRepGraph&                                   theGraph,
                               NCollection_Vector<BRepGraphAlgo_Validate::Issue>& theIssues)
{
  using Issue    = BRepGraphAlgo_Validate::Issue;
  using Severity = BRepGraphAlgo_Validate::Severity;

  // Non-removed edges must not reference removed vertices.
  for (int anEdgeIdx = 0; anEdgeIdx < theGraph.Topo().NbEdges(); ++anEdgeIdx)
  {
    const BRepGraphInc::EdgeDef& anEdge = theGraph.Topo().Edge(BRepGraph_EdgeId(anEdgeIdx));
    if (anEdge.IsRemoved)
      continue;

    if (anEdge.StartVertexRefId.IsValid())
    {
      const BRepGraph_NodeId aStartVtxId =
        theGraph.Refs().Vertex(anEdge.StartVertexRefId).VertexDefId;
      if (aStartVtxId.IsValid() && isEntityRemoved(theGraph, aStartVtxId))
      {
        theIssues.Append(Issue{Severity::Error,
                               anEdge.Id,
                               "Non-removed EdgeDef references removed StartVertexEntity"});
      }
    }
    if (anEdge.EndVertexRefId.IsValid())
    {
      const BRepGraph_NodeId anEndVtxId = theGraph.Refs().Vertex(anEdge.EndVertexRefId).VertexDefId;
      if (anEndVtxId.IsValid() && isEntityRemoved(theGraph, anEndVtxId))
      {
        theIssues.Append(Issue{Severity::Error,
                               anEdge.Id,
                               "Non-removed EdgeDef references removed EndVertexEntity"});
      }
    }
  }

  // Non-removed wires must not reference removed edges.
  for (int aWireIdx = 0; aWireIdx < theGraph.Topo().NbWires(); ++aWireIdx)
  {
    const BRepGraphInc::WireDef& aWire = theGraph.Topo().Wire(BRepGraph_WireId(aWireIdx));
    if (aWire.IsRemoved)
      continue;

    bool hasRemovedEdge = false;
    forWireCoEdgeRefEntries(theGraph,
                            BRepGraph_WireId(aWireIdx),
                            [&](const BRepGraphInc::CoEdgeRef& aCR) {
                              if (hasRemovedEdge)
                              {
                                return;
                              }
                              const BRepGraphInc::CoEdgeDef& aCoEdge =
                                theGraph.Topo().CoEdge(aCR.CoEdgeDefId);
                              const BRepGraph_NodeId anEdgeId = aCoEdge.EdgeDefId;
                              if (anEdgeId.IsValid() && isEntityRemoved(theGraph, anEdgeId))
                              {
                                hasRemovedEdge = true;
                              }
                            });
    if (hasRemovedEdge)
    {
      theIssues.Append(
        Issue{Severity::Error, aWire.Id, "Non-removed WireDef references removed EdgeDef"});
    }
  }
}

//! Check wire edge connectivity: each coedge's end vertex must match the
//! next coedge's start vertex. Uses BRepGraph_WireExplorer for
//! order-independent traversal.
//! @param[in]     theGraph  source graph
//! @param[in,out] theIssues collection to append diagnostic issues
void checkWireConnectivity(const BRepGraph&                                   theGraph,
                           NCollection_Vector<BRepGraphAlgo_Validate::Issue>& theIssues)
{
  using Issue    = BRepGraphAlgo_Validate::Issue;
  using Severity = BRepGraphAlgo_Validate::Severity;

  auto edgeLookup = [&theGraph](int theIdx) -> const BRepGraphInc::EdgeDef& {
    return theGraph.Topo().Edge(BRepGraph_EdgeId(theIdx));
  };

  for (int aWireIdx = 0; aWireIdx < theGraph.Topo().NbWires(); ++aWireIdx)
  {
    const BRepGraphInc::WireDef& aWire = theGraph.Topo().Wire(BRepGraph_WireId(aWireIdx));
    if (aWire.IsRemoved)
      continue;

    NCollection_Vector<BRepGraphInc::CoEdgeUsage> aWireCoEdgeRefs;
    forWireCoEdgeRefEntries(theGraph,
                            BRepGraph_WireId(aWireIdx),
                            [&](const BRepGraphInc::CoEdgeRef& aCRE) {
                              BRepGraphInc::CoEdgeUsage aCR;
                              aCR.CoEdgeDefId   = aCRE.CoEdgeDefId;
                              aCR.LocalLocation = aCRE.LocalLocation;
                              aWireCoEdgeRefs.Append(aCR);
                            });

    const int aNbCoEdges = aWireCoEdgeRefs.Length();
    if (aNbCoEdges < 2)
      continue;

    // Validate all edge indices first.
    bool aAllValid = true;
    for (int anIdx = 0; anIdx < aNbCoEdges; ++anIdx)
    {
      const BRepGraphInc::CoEdgeUsage& aCR      = aWireCoEdgeRefs.Value(anIdx);
      const BRepGraphInc::CoEdgeDef&   aCoEdge  = theGraph.Topo().CoEdge(aCR.CoEdgeDefId);
      const BRepGraph_NodeId           anEdgeId = aCoEdge.EdgeDefId;
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
    auto coedgeLookup = [&theGraph](int theIdx) -> const BRepGraphInc::CoEdgeDef& {
      return theGraph.Topo().CoEdge(BRepGraph_CoEdgeId(theIdx));
    };
    auto vtxRefLookup = [&theGraph](const BRepGraph_VertexRefId theRefId) -> BRepGraph_VertexId {
      return theGraph.Refs().Vertex(theRefId).VertexDefId;
    };
    BRepGraph_WireExplorer anExp(aWireCoEdgeRefs, coedgeLookup, edgeLookup, vtxRefLookup);
    const NCollection_Vector<BRepGraphInc::CoEdgeUsage>& anOrdered = anExp.OrderedRefs();

    for (int anIdx = 0; anIdx < anOrdered.Length() - 1; ++anIdx)
    {
      const BRepGraphInc::CoEdgeUsage& aCurrCR = anOrdered.Value(anIdx);
      const BRepGraphInc::CoEdgeUsage& aNextCR = anOrdered.Value(anIdx + 1);

      const BRepGraphInc::CoEdgeDef& aCurrCoEdge = theGraph.Topo().CoEdge(aCurrCR.CoEdgeDefId);
      const BRepGraphInc::CoEdgeDef& aNextCoEdge = theGraph.Topo().CoEdge(aNextCR.CoEdgeDefId);

      const BRepGraphInc::EdgeDef& aCurrEdge = theGraph.Topo().Edge(aCurrCoEdge.EdgeDefId);
      const BRepGraphInc::EdgeDef& aNextEdge = theGraph.Topo().Edge(aNextCoEdge.EdgeDefId);

      // Resolve oriented end vertex of current edge.
      const BRepGraph_VertexRefId aCurrEndRefId = (aCurrCoEdge.Sense == TopAbs_FORWARD)
                                                    ? aCurrEdge.EndVertexRefId
                                                    : aCurrEdge.StartVertexRefId;
      const BRepGraph_NodeId      aCurrEnd =
        aCurrEndRefId.IsValid()
               ? BRepGraph_NodeId::Vertex(theGraph.Refs().Vertex(aCurrEndRefId).VertexDefId.Index)
               : BRepGraph_NodeId();

      // Resolve oriented start vertex of next edge.
      const BRepGraph_VertexRefId aNextStartRefId = (aNextCoEdge.Sense == TopAbs_FORWARD)
                                                      ? aNextEdge.StartVertexRefId
                                                      : aNextEdge.EndVertexRefId;
      const BRepGraph_NodeId      aNextStart =
        aNextStartRefId.IsValid()
               ? BRepGraph_NodeId::Vertex(theGraph.Refs().Vertex(aNextStartRefId).VertexDefId.Index)
               : BRepGraph_NodeId();

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

//! Validate that every entity's Id field matches its actual vector position:
//! Entity[i].Id must equal (Kind, i). Detects corruption from incorrect
//! Compact remapping or manual entity manipulation.
//! @param[in]     theGraph  source graph
//! @param[in,out] theIssues collection to append diagnostic issues
void checkDefIds(const BRepGraph&                                   theGraph,
                 NCollection_Vector<BRepGraphAlgo_Validate::Issue>& theIssues)
{
  using Severity                   = BRepGraphAlgo_Validate::Severity;
  using Issue                      = BRepGraphAlgo_Validate::Issue;
  const BRepGraph::TopoView& aDefs = theGraph.Topo();

  auto checkKind = [&](BRepGraph_NodeId::Kind theKind, int theNb) {
    for (int anIdx = 0; anIdx < theNb; ++anIdx)
    {
      const BRepGraph_NodeId       anExpected(theKind, anIdx);
      const BRepGraphInc::BaseDef* aDef = aDefs.TopoEntity(anExpected);
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

  checkKind(BRepGraph_NodeId::Kind::Vertex, aDefs.NbVertices());
  checkKind(BRepGraph_NodeId::Kind::Edge, aDefs.NbEdges());
  checkKind(BRepGraph_NodeId::Kind::CoEdge, aDefs.NbCoEdges());
  checkKind(BRepGraph_NodeId::Kind::Wire, aDefs.NbWires());
  checkKind(BRepGraph_NodeId::Kind::Face, aDefs.NbFaces());
  checkKind(BRepGraph_NodeId::Kind::Shell, aDefs.NbShells());
  checkKind(BRepGraph_NodeId::Kind::Solid, aDefs.NbSolids());
  checkKind(BRepGraph_NodeId::Kind::Compound, aDefs.NbCompounds());
  checkKind(BRepGraph_NodeId::Kind::CompSolid, aDefs.NbCompSolids());
  const BRepGraph::TopoView& aTopo = theGraph.Topo();
  checkKind(BRepGraph_NodeId::Kind::Product, aTopo.NbProducts());
  checkKind(BRepGraph_NodeId::Kind::Occurrence, aTopo.NbOccurrences());
}

//! Validate that NbActive*() counts match the actual number of non-removed
//! entities in each per-kind vector.
//! @param[in]     theGraph  source graph
//! @param[in,out] theIssues collection to append diagnostic issues
void checkActiveCounts(const BRepGraph&                                   theGraph,
                       NCollection_Vector<BRepGraphAlgo_Validate::Issue>& theIssues)
{
  using Severity                   = BRepGraphAlgo_Validate::Severity;
  using Issue                      = BRepGraphAlgo_Validate::Issue;
  const BRepGraph::TopoView& aDefs = theGraph.Topo();

  auto countActive = [&](BRepGraph_NodeId::Kind theKind, int theNb) -> int {
    int aCount = 0;
    for (int anIdx = 0; anIdx < theNb; ++anIdx)
    {
      const BRepGraph_NodeId anId(theKind, anIdx);
      if (!isEntityRemoved(theGraph, anId))
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

  verify("Vertices",
         aDefs.NbActiveVertices(),
         countActive(BRepGraph_NodeId::Kind::Vertex, aDefs.NbVertices()));
  verify("Edges",
         aDefs.NbActiveEdges(),
         countActive(BRepGraph_NodeId::Kind::Edge, aDefs.NbEdges()));
  verify("CoEdges",
         aDefs.NbActiveCoEdges(),
         countActive(BRepGraph_NodeId::Kind::CoEdge, aDefs.NbCoEdges()));
  verify("Wires",
         aDefs.NbActiveWires(),
         countActive(BRepGraph_NodeId::Kind::Wire, aDefs.NbWires()));
  verify("Faces",
         aDefs.NbActiveFaces(),
         countActive(BRepGraph_NodeId::Kind::Face, aDefs.NbFaces()));
  verify("Shells",
         aDefs.NbActiveShells(),
         countActive(BRepGraph_NodeId::Kind::Shell, aDefs.NbShells()));
  verify("Solids",
         aDefs.NbActiveSolids(),
         countActive(BRepGraph_NodeId::Kind::Solid, aDefs.NbSolids()));
  verify("Compounds",
         aDefs.NbActiveCompounds(),
         countActive(BRepGraph_NodeId::Kind::Compound, aDefs.NbCompounds()));
  verify("CompSolids",
         aDefs.NbActiveCompSolids(),
         countActive(BRepGraph_NodeId::Kind::CompSolid, aDefs.NbCompSolids()));
  const BRepGraph::TopoView& aTopo = theGraph.Topo();
  verify("Products",
         aTopo.NbActiveProducts(),
         countActive(BRepGraph_NodeId::Kind::Product, aTopo.NbProducts()));
  verify("Occurrences",
         aTopo.NbActiveOccurrences(),
         countActive(BRepGraph_NodeId::Kind::Occurrence, aTopo.NbOccurrences()));
}

} // namespace

//=================================================================================================

BRepGraphAlgo_Validate::Result BRepGraphAlgo_Validate::Perform(const BRepGraph& theGraph)
{
  return Perform(theGraph, Options::Lightweight());
}

//=================================================================================================

BRepGraphAlgo_Validate::Result BRepGraphAlgo_Validate::Perform(const BRepGraph& theGraph,
                                                               const Mode       theMode)
{
  Options anOptions;
  anOptions.ValidationMode = theMode;
  return Perform(theGraph, anOptions);
}

//=================================================================================================

BRepGraphAlgo_Validate::Result BRepGraphAlgo_Validate::Perform(const BRepGraph& theGraph,
                                                               const Options&   theOptions)
{
  Result aResult;
  if (!theGraph.IsDone())
  {
    return aResult;
  }

  if (theOptions.ValidationMode == Mode::Lightweight)
  {
    NCollection_Vector<BRepGraph::BuilderView::BoundaryIssue> aBoundaryIssues;
    if (!theGraph.Builder().ValidateMutationBoundary(&aBoundaryIssues))
    {
      appendMutationBoundaryIssues(aBoundaryIssues, aResult.Issues);
    }
    return aResult;
  }

  NCollection_Vector<BRepGraph::BuilderView::BoundaryIssue> aBoundaryIssues;
  if (!theGraph.Builder().ValidateMutationBoundary(&aBoundaryIssues))
  {
    appendMutationBoundaryIssues(aBoundaryIssues, aResult.Issues);
  }

  checkCrossReferenceBounds(theGraph, aResult.Issues);
  checkReverseIndexConsistency(theGraph, aResult.Issues);
  checkReverseIndexFaceCountCache(theGraph, aResult.Issues);
  checkIncidenceRefConsistency(theGraph, aResult.Issues);
  checkGeometryReferences(theGraph, aResult.Issues);
  checkRemovedNodeIsolation(theGraph, aResult.Issues);
  checkWireConnectivity(theGraph, aResult.Issues);

  checkDefIds(theGraph, aResult.Issues);
  checkActiveCounts(theGraph, aResult.Issues);

  // UID integrity checks: all active nodes must have a valid UID that round-trips.
  const BRepGraph::TopoView& aDefs = theGraph.Topo();
  const BRepGraph::UIDsView& aUIDs = theGraph.UIDs();
  auto checkUIDKind                = [&](const BRepGraph_NodeId::Kind theKind, const int theNb) {
    for (int anIdx = 0; anIdx < theNb; ++anIdx)
    {
      const BRepGraph_NodeId       aNode(theKind, anIdx);
      const BRepGraphInc::BaseDef* aDef = aDefs.TopoEntity(aNode);
      if (aDef == nullptr || aDef->IsRemoved)
      {
        continue;
      }

      const BRepGraph_UID aUID = aUIDs.Of(aNode);
      if (!aUID.IsValid())
      {
        aResult.Issues.Append(
          Issue{Severity::Error, aNode, "Node has missing or invalid UID in deep audit"});
        continue;
      }

      if (!aUIDs.Has(aUID))
      {
        aResult.Issues.Append(Issue{Severity::Error,
                                    aNode,
                                    "Node UID is not present in UIDs view for this generation"});
        continue;
      }

      if (aUIDs.NodeIdFrom(aUID) != aNode)
      {
        aResult.Issues.Append(
          Issue{Severity::Error, aNode, "Node UID does not round-trip to originating NodeId"});
      }
    }
  };

  checkUIDKind(BRepGraph_NodeId::Kind::Vertex, aDefs.NbVertices());
  checkUIDKind(BRepGraph_NodeId::Kind::Edge, aDefs.NbEdges());
  checkUIDKind(BRepGraph_NodeId::Kind::CoEdge, aDefs.NbCoEdges());
  checkUIDKind(BRepGraph_NodeId::Kind::Wire, aDefs.NbWires());
  checkUIDKind(BRepGraph_NodeId::Kind::Face, aDefs.NbFaces());
  checkUIDKind(BRepGraph_NodeId::Kind::Shell, aDefs.NbShells());
  checkUIDKind(BRepGraph_NodeId::Kind::Solid, aDefs.NbSolids());
  checkUIDKind(BRepGraph_NodeId::Kind::Compound, aDefs.NbCompounds());
  checkUIDKind(BRepGraph_NodeId::Kind::CompSolid, aDefs.NbCompSolids());
  const BRepGraph::TopoView& aTopo = theGraph.Topo();
  checkUIDKind(BRepGraph_NodeId::Kind::Product, aTopo.NbProducts());
  checkUIDKind(BRepGraph_NodeId::Kind::Occurrence, aTopo.NbOccurrences());

  // Assembly DAG cycle detection: for each product, check if it can
  // reach itself through its occurrence->product chain.
  // Shared references (two occurrences pointing to the same child) are valid;
  // only self-reachability constitutes a cycle.
  for (int aProdIdx = 0; aProdIdx < aTopo.NbProducts(); ++aProdIdx)
  {
    const BRepGraph_ProductId           aProdId(aProdIdx);
    const BRepGraphInc::ProductDef& aProd = aTopo.Product(aProdId);
    if (aProd.IsRemoved)
      continue;

    // BFS from this product's children; skip already-visited to avoid
    // exponential blowup on DAGs. A cycle exists if we re-encounter aProdIdx.
    NCollection_Map<int>                   aVisited;
    NCollection_Vector<BRepGraph_ProductId> aQueue;
    int                                     aHead = 0;

    // Seed with direct children.
    for (int i = 0; i < aProd.OccurrenceRefIds.Length(); ++i)
    {
      const BRepGraphInc::OccurrenceRef& anOccRef =
        theGraph.Refs().Occurrence(aProd.OccurrenceRefIds.Value(i));
      if (anOccRef.IsRemoved || !anOccRef.OccurrenceDefId.IsValid(aTopo.NbOccurrences()))
        continue;
      const BRepGraphInc::OccurrenceDef& anOcc = aTopo.Occurrence(anOccRef.OccurrenceDefId);
      if (anOcc.IsRemoved || !anOcc.ProductDefId.IsValid(aTopo.NbProducts()))
        continue;
      if (anOcc.ProductDefId.Index == aProdIdx)
      {
        aResult.Issues.Append(
          Issue{Severity::Error, aProdId,
                "Assembly cycle: Product directly references itself via occurrence"});
        break;
      }
      if (aVisited.Add(anOcc.ProductDefId.Index))
        aQueue.Append(anOcc.ProductDefId);
    }

    bool aCycleFound = false;
    while (aHead < aQueue.Length() && !aCycleFound)
    {
      const BRepGraph_ProductId         aChildProdId = aQueue.Value(aHead);
      ++aHead;
      const BRepGraphInc::ProductDef& aChildProd = aTopo.Product(aChildProdId);
      if (aChildProd.IsRemoved)
        continue;
      for (int i = 0; i < aChildProd.OccurrenceRefIds.Length(); ++i)
      {
        const BRepGraphInc::OccurrenceRef& aRef =
          theGraph.Refs().Occurrence(aChildProd.OccurrenceRefIds.Value(i));
        if (aRef.IsRemoved || !aRef.OccurrenceDefId.IsValid(aTopo.NbOccurrences()))
          continue;
        const BRepGraphInc::OccurrenceDef& aOcc = aTopo.Occurrence(aRef.OccurrenceDefId);
        if (aOcc.IsRemoved || !aOcc.ProductDefId.IsValid(aTopo.NbProducts()))
          continue;
        if (aOcc.ProductDefId.Index == aProdIdx)
        {
          aResult.Issues.Append(
            Issue{Severity::Error, aProdId,
                  "Assembly cycle: Product reaches itself through occurrence chain"});
          aCycleFound = true;
          break;
        }
        if (aVisited.Add(aOcc.ProductDefId.Index))
          aQueue.Append(aOcc.ProductDefId);
      }
    }
  }

  return aResult;
}
