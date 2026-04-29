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

#include <BRepGraph.hxx>
#include <BRepGraph_Builder.hxx>
#include <BRepGraph_Copy.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Transform.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <BRep_Builder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Geom_Line.hxx>
#include <NCollection_DynamicArray.hxx>
#include <TopoDS_Compound.hxx>
#include <gp_Trsf.hxx>

#include <gtest/gtest.h>

namespace
{

BRepGraph makeBoxGraph()
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph_Builder::Result aRes =
    BRepGraph_Builder::Add(aGraph, BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  return aGraph;
}

BRepGraph_EdgeId makeSelfLoopEdge(BRepGraph& theGraph)
{
  const BRepGraph_VertexId     aV = theGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.0e-7);
  const occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  return theGraph.Editor().Edges().Add(aV, aV, aLine, 0.0, 1.0, 1.0e-7);
}

} // namespace

// B1: RemoveRef on a Product->Occurrence ref must unbind the reverse-index entry
// keyed by the referenced product (OccurrenceDef::ChildDefId), not by the parent.
TEST(BRepGraph_PermissionUpdateTest, RemoveRef_OccurrenceRef_PreservesProductToOccurrencesIndex)
{
  // Targeted regression for the wrong-key Unbind in GenOps::RemoveRef. Walks the
  // reverse index directly because BRepGraphInc_ReverseIndex::Validate currently
  // covers only topology references.
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_TRUE(aGraph.IsDone());
  BRepGraph::EditorView::ProductOps& aProds = aGraph.Editor().Products();

  const BRepGraph_ProductId aParent = aProds.CreateEmptyProduct();
  const BRepGraph_ProductId aChild  = aProds.CreateEmptyProduct();
  ASSERT_TRUE(aParent.IsValid());
  ASSERT_TRUE(aChild.IsValid());

  BRepGraph_OccurrenceRefId    aOccRefId;
  const BRepGraph_OccurrenceId aOccId =
    aProds.LinkProducts(aParent, aChild, TopLoc_Location(), BRepGraph_OccurrenceId(), &aOccRefId);
  ASSERT_TRUE(aOccId.IsValid());
  ASSERT_TRUE(aOccRefId.IsValid());

  bool aFoundBefore = false;
  for (const BRepGraph_OccurrenceId& anId : aGraph.Topo().Products().Instances(aChild))
  {
    if (anId == aOccId)
    {
      aFoundBefore = true;
    }
  }
  EXPECT_TRUE(aFoundBefore) << "Occurrence must be indexed under referenced product before remove";

  EXPECT_TRUE(aGraph.Editor().Gen().RemoveRef(BRepGraph_RefId(aOccRefId)));

  for (const BRepGraph_OccurrenceId& anId : aGraph.Topo().Products().Instances(aChild))
  {
    EXPECT_NE(anId, aOccId) << "RemoveRef must drop the referenced-product entry";
  }

  // The wrong-key bug would also leave a stale entry under the parent product id.
  for (const BRepGraph_OccurrenceId& anId : aGraph.Topo().Products().Instances(aParent))
  {
    EXPECT_NE(anId, aOccId) << "Bug: Unbind happened against parent product key";
  }
}

// B2: removing one CoEdgeRef must leave the wire's other usages of the same edge
// reflected in the deduplicated Edge->Wire reverse index.
//
// Constructs a wire with two CoEdgeRefs whose CoEdges share the same edge so the
// dedup-aware unbind path is exercised; removing one ref must keep the
// (Edge -> Wire) reverse entry alive because the sibling still references it.
TEST(BRepGraph_PermissionUpdateTest, RemoveRef_CoEdge_PreservesEdgeToWireWhenSiblingPresent)
{
  BRepGraph aGraph;
  aGraph.Clear();

  const BRepGraph_VertexId     aV1 = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.0e-7);
  const BRepGraph_VertexId     aV2 = aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.0e-7);
  const occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  const BRepGraph_EdgeId aEdge = aGraph.Editor().Edges().Add(aV1, aV2, aLine, 0.0, 1.0, 1.0e-7);
  ASSERT_TRUE(aEdge.IsValid());

  NCollection_DynamicArray<std::pair<BRepGraph_EdgeId, TopAbs_Orientation>> anEdges;
  anEdges.Append(std::make_pair(aEdge, TopAbs_FORWARD));
  anEdges.Append(std::make_pair(aEdge, TopAbs_REVERSED));
  const BRepGraph_WireId aWireId = aGraph.Editor().Wires().Add(anEdges);
  ASSERT_TRUE(aWireId.IsValid());
  EXPECT_TRUE(aGraph.ValidateReverseIndex());

  const BRepGraphInc::WireDef& aWireDef = aGraph.Topo().Wires().Definition(aWireId);
  ASSERT_EQ(aWireDef.CoEdgeRefIds.Size(), 2u);
  const BRepGraph_CoEdgeRefId aFirstRef = aWireDef.CoEdgeRefIds.First();
  ASSERT_TRUE(aFirstRef.IsValid());

  auto containsWire = [&](const BRepGraph_EdgeId theE) -> bool {
    for (const BRepGraph_WireId& aW : aGraph.Topo().Edges().Wires(theE))
    {
      if (aW == aWireId)
      {
        return true;
      }
    }
    return false;
  };
  ASSERT_TRUE(containsWire(aEdge)) << "Edge->Wire reverse must be present before removal";

  EXPECT_TRUE(aGraph.Editor().Gen().RemoveRef(BRepGraph_RefId(aFirstRef)));
  EXPECT_TRUE(aGraph.ValidateReverseIndex());
  EXPECT_TRUE(containsWire(aEdge))
    << "Sibling CoEdgeRef still references the edge: Edge->Wire entry must survive";
}

// B3: SetRefVertexDefId on a self-loop edge (start == end == V_old) must keep
// the V_old->edge entry while the sibling slot still references it.
TEST(BRepGraph_PermissionUpdateTest, SetRefVertexDefId_SelfLoopSibling_KeepsRevIndexValid)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_TRUE(aGraph.IsDone());
  const BRepGraph_EdgeId aEdge = makeSelfLoopEdge(aGraph);
  ASSERT_TRUE(aEdge.IsValid());
  EXPECT_TRUE(aGraph.ValidateReverseIndex());

  const BRepGraphInc::EdgeDef& aDef        = aGraph.Topo().Edges().Definition(aEdge);
  const BRepGraph_VertexRefId  aStartRefId = aDef.StartVertexRefId;
  ASSERT_TRUE(aStartRefId.IsValid());

  const BRepGraph_VertexId aNewV = aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.0e-7);

  aGraph.Editor().Vertices().SetRefVertexDefId(aStartRefId, aNewV);
  EXPECT_TRUE(aGraph.ValidateReverseIndex());

  // EndVertexRef still references the original vertex, so the (Vold -> aEdge)
  // reverse entry must survive the start-side rebind.
  const BRepGraph_VertexId aOldV =
    aGraph.Topo().Edges().Definition(aEdge).EndVertexRefId.IsValid()
      ? aGraph.Refs()
          .Vertices()
          .Entry(aGraph.Topo().Edges().Definition(aEdge).EndVertexRefId)
          .VertexDefId
      : BRepGraph_VertexId();
  ASSERT_TRUE(aOldV.IsValid());
  bool aOldStillIndexed = false;
  for (const BRepGraph_EdgeId& aE : aGraph.Topo().Vertices().Edges(aOldV))
  {
    if (aE == aEdge)
    {
      aOldStillIndexed = true;
    }
  }
  EXPECT_TRUE(aOldStillIndexed)
    << "Vold must remain in EdgesOfVertex(Vold) after start-side rebind";
}

// CopyNode must not loop infinitely on a self-referencing compound.
TEST(BRepGraph_PermissionUpdateTest, CopyNode_SelfReferencingCompound_Terminates)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_TRUE(aGraph.IsDone());
  const BRepGraph_VertexId aV = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.0e-7);
  NCollection_DynamicArray<BRepGraph_NodeId> aChildren;
  aChildren.Append(BRepGraph_NodeId(aV));
  const BRepGraph_CompoundId aRoot = aGraph.Editor().Compounds().Add(aChildren);
  ASSERT_TRUE(aRoot.IsValid());

  // Splice the compound into itself by rewriting its first child ref.
  const BRepGraphInc::CompoundDef& aDef = aGraph.Topo().Compounds().Definition(aRoot);
  ASSERT_FALSE(aDef.ChildRefIds.IsEmpty());
  const BRepGraph_ChildRefId aChildRefId = aDef.ChildRefIds.First();
  aGraph.Editor().Gen().SetChildRefChildDefId(aChildRefId, BRepGraph_NodeId(aRoot));

  const BRepGraph aCopy = BRepGraph_Copy::CopyNode(aGraph,
                                                   BRepGraph_NodeId(aRoot),
                                                   /*copyGeom*/ true,
                                                   /*copyMesh*/ false,
                                                   /*reserveCache*/ false);
  EXPECT_TRUE(aCopy.IsDone());
  EXPECT_TRUE(aCopy.ValidateReverseIndex());
}

// TransformNode with copyGeom on an assembly node must reject (returns invalid graph)
// rather than silently produce inconsistent geometry/location state.
TEST(BRepGraph_PermissionUpdateTest, TransformNode_AssemblyWithCopyGeom_Rejected)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_TRUE(aGraph.IsDone());
  const BRepGraph_ProductId aProd = aGraph.Editor().Products().CreateEmptyProduct();
  ASSERT_TRUE(aProd.IsValid());

  gp_Trsf aT;
  aT.SetTranslation(gp_Vec(1.0, 0.0, 0.0));
  const BRepGraph aResult =
    BRepGraph_Transform::TransformNode(aGraph, BRepGraph_NodeId(aProd), aT, true, false);
  EXPECT_FALSE(aResult.IsDone());
}

// Companion to the above: Occurrence node with copyGeom must be rejected too.
TEST(BRepGraph_PermissionUpdateTest, TransformNode_OccurrenceWithCopyGeom_Rejected)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_TRUE(aGraph.IsDone());
  BRepGraph::EditorView::ProductOps& aProds  = aGraph.Editor().Products();
  const BRepGraph_ProductId          aParent = aProds.CreateEmptyProduct();
  const BRepGraph_ProductId          aChild  = aProds.CreateEmptyProduct();
  ASSERT_TRUE(aParent.IsValid());
  ASSERT_TRUE(aChild.IsValid());
  const BRepGraph_OccurrenceId aOccId = aProds.LinkProducts(aParent, aChild, TopLoc_Location());
  ASSERT_TRUE(aOccId.IsValid());

  gp_Trsf aT;
  aT.SetTranslation(gp_Vec(1.0, 0.0, 0.0));
  const BRepGraph aResult =
    BRepGraph_Transform::TransformNode(aGraph, BRepGraph_NodeId(aOccId), aT, true, false);
  EXPECT_FALSE(aResult.IsDone());
}

// LinkProducts with the unified signature returns the freshly inserted OccurrenceRef
// id through the optional out pointer.
TEST(BRepGraph_PermissionUpdateTest, LinkProducts_OutOccurrenceRefId_Populated)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_TRUE(aGraph.IsDone());
  BRepGraph::EditorView::ProductOps& aProds  = aGraph.Editor().Products();
  const BRepGraph_ProductId          aParent = aProds.CreateEmptyProduct();
  const BRepGraph_ProductId          aChild  = aProds.CreateEmptyProduct();
  ASSERT_TRUE(aParent.IsValid());
  ASSERT_TRUE(aChild.IsValid());

  BRepGraph_OccurrenceRefId    aOccRefId;
  const BRepGraph_OccurrenceId aOccId =
    aProds.LinkProducts(aParent, aChild, TopLoc_Location(), BRepGraph_OccurrenceId(), &aOccRefId);
  ASSERT_TRUE(aOccId.IsValid());
  ASSERT_TRUE(aOccRefId.IsValid());
  EXPECT_EQ(aGraph.Refs().Occurrences().Entry(aOccRefId).OccurrenceDefId, aOccId);

  // Default out-pointer = nullptr is also legal.
  const BRepGraph_OccurrenceId aOccId2 = aProds.LinkProducts(aParent, aChild, TopLoc_Location());
  EXPECT_TRUE(aOccId2.IsValid());
}

// MoveRef: a pure rotation/translation must succeed; a scaled trsf must fail with no
// state change.
TEST(BRepGraph_PermissionUpdateTest, MoveRef_ScaledTrsf_RejectedWithoutMutation)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_TRUE(aGraph.IsDone());

  // Find any face ref to test against.
  BRepGraph_FaceRefId aFaceRef;
  for (BRepGraph_FullFaceRefIterator aIt(aGraph); aIt.More(); aIt.Next())
  {
    aFaceRef = aIt.CurrentId();
    break;
  }
  ASSERT_TRUE(aFaceRef.IsValid());

  const TopLoc_Location aBefore = aGraph.Refs().Faces().Entry(aFaceRef).LocalLocation;

  gp_Trsf aScaled;
  aScaled.SetScaleFactor(2.0);
  EXPECT_FALSE(BRepGraph_Transform::MoveRef(aGraph, BRepGraph_RefId(aFaceRef), aScaled));
  EXPECT_TRUE(aGraph.Refs().Faces().Entry(aFaceRef).LocalLocation == aBefore);

  gp_Trsf aTrans;
  aTrans.SetTranslation(gp_Vec(1.0, 2.0, 3.0));
  EXPECT_TRUE(BRepGraph_Transform::MoveRef(aGraph, BRepGraph_RefId(aFaceRef), aTrans));
  EXPECT_FALSE(aGraph.Refs().Faces().Entry(aFaceRef).LocalLocation == aBefore);
}

// RemoveSubgraph cascade rebuilds the reverse index exactly once at the outermost
// scope; nested calls during recursion must not trigger intermediate rebuilds, and
// the final state must validate.
TEST(BRepGraph_PermissionUpdateTest, RemoveSubgraph_NestedCascade_FinalStateValid)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_TRUE(aGraph.IsDone());
  EXPECT_TRUE(aGraph.ValidateReverseIndex());

  BRepGraph_SolidId aSolidId;
  for (BRepGraph_SolidIterator aIt(aGraph); aIt.More(); aIt.Next())
  {
    aSolidId = aIt.CurrentId();
    break;
  }
  ASSERT_TRUE(aSolidId.IsValid());

  aGraph.Editor().Gen().RemoveSubgraph(BRepGraph_NodeId(aSolidId));
  EXPECT_TRUE(aGraph.ValidateReverseIndex());
}

// MutGuard: a guard that observes but never writes must not bump OwnGen on destruction;
// MarkDirty() must force the bump even when no setter was called.
TEST(BRepGraph_PermissionUpdateTest, MutGuard_DirtyFlag_RespectsExplicitMarkAndCleanScope)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraph_EdgeId aEdgeId;
  for (BRepGraph_EdgeIterator anIt(aGraph); anIt.More(); anIt.Next())
  {
    aEdgeId = anIt.CurrentId();
    break;
  }
  ASSERT_TRUE(aEdgeId.IsValid());

  const uint32_t aOwnGenBefore = aGraph.Topo().Edges().Definition(aEdgeId).OwnGen;
  {
    BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aGuard = aGraph.Editor().Edges().Mut(aEdgeId);
    (void)aGuard->IsClosed; // read-only access only
  }
  EXPECT_EQ(aOwnGenBefore, aGraph.Topo().Edges().Definition(aEdgeId).OwnGen)
    << "Read-only guard scope must not bump OwnGen";

  {
    BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aGuard = aGraph.Editor().Edges().Mut(aEdgeId);
    aGuard.MarkDirty();
  }
  EXPECT_GT(aGraph.Topo().Edges().Definition(aEdgeId).OwnGen, aOwnGenBefore)
    << "MarkDirty() must bump OwnGen on guard destruction";
}
