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
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_Copy.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ReverseIterator.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Transform.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <BRep_Builder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Geom_Line.hxx>
#include <NCollection_LinearVector.hxx>
#include <TopoDS_Compound.hxx>
#include <gp_Trsf.hxx>

#include <tuple>

#include <gtest/gtest.h>

namespace
{

BRepGraph makeBoxGraph()
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aRes =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  return aGraph;
}

BRepGraph_EdgeId makeSelfLoopEdge(BRepGraph& theGraph)
{
  const BRepGraph_VertexId     aV = theGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.0e-7);
  const occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  return theGraph.Editor().Edges().Add(aV, aV, aLine, 0.0, 1.0, 1.0e-7);
}

template <typename IdT>
bool containsId(const NCollection_LinearVector<IdT>& theIds, const IdT theId)
{
  for (const IdT& anId : theIds)
  {
    if (anId == theId)
    {
      return true;
    }
  }
  return false;
}

template <typename IteratorT, typename IdT>
bool containsCurrentId(IteratorT theIterator, const IdT theId)
{
  for (; theIterator.More(); theIterator.Next())
  {
    if (theIterator.CurrentId() == theId)
    {
      return true;
    }
  }
  return false;
}

} // namespace

TEST(BRepGraph_PermissionUpdateTest, RemoveRef_FaceRef_InvalidatesOwningShellBeforeUnbind)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph_RefsFaceOfShell aFaceRefs(aGraph, BRepGraph_ShellId::Start());
  ASSERT_TRUE(aFaceRefs.More());
  const BRepGraph_FaceRefId aFaceRefId = aFaceRefs.CurrentId();

  const uint32_t aShellSubtreeGenBefore =
    aGraph.Topo().Shells().Definition(BRepGraph_ShellId::Start()).SubtreeGen;
  ASSERT_TRUE(aGraph.Editor().Gen().RemoveRef(aFaceRefId));

  EXPECT_TRUE(aFaceRefId.IsRemoved(aGraph));
  EXPECT_GT(aGraph.Topo().Shells().Definition(BRepGraph_ShellId::Start()).SubtreeGen,
            aShellSubtreeGenBefore);
  EXPECT_TRUE(aGraph.ValidateRelations());
}

TEST(BRepGraph_PermissionUpdateTest, RemoveShell_SameSolidSiblingRef_PreservesRelations)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  const BRepGraph_SolidId    aSolidId = BRepGraph_SolidId::Start();
  BRepGraph_RefsShellOfSolid aShellRefs(aGraph, aSolidId);
  ASSERT_TRUE(aShellRefs.More());
  const BRepGraph_ShellRefId aFirstRefId = aShellRefs.CurrentId();
  const BRepGraph_ShellId    aShellId    = aGraph.Refs().Shells().Entry(aFirstRefId).ChildShellId;

  const BRepGraph_ShellRefId aSecondRefId =
    aGraph.Editor().Solids().Append(aSolidId, aShellId, TopAbs_REVERSED);
  ASSERT_TRUE(aSecondRefId.IsValid());
  ASSERT_NE(aSecondRefId, aFirstRefId);
  ASSERT_TRUE(containsCurrentId(
    BRepGraph_SolidsOfShell(aGraph, aGraph.Topo().Shells().Relations(aShellId).ParentShellRefIds),
    aSolidId));

  ASSERT_TRUE(aGraph.Editor().Solids().RemoveShell(aSolidId, aFirstRefId));

  EXPECT_TRUE(aFirstRefId.IsRemoved(aGraph));
  EXPECT_FALSE(aSecondRefId.IsRemoved(aGraph));
  EXPECT_TRUE(containsCurrentId(
    BRepGraph_SolidsOfShell(aGraph, aGraph.Topo().Shells().Relations(aShellId).ParentShellRefIds),
    aSolidId))
    << "A sibling ShellRef still connects the same Solid to the same Shell";
  EXPECT_TRUE(aGraph.ValidateRelations());
}

// B1: RemoveRef on a Product->Occurrence ref must unbind the relation-table entry
// keyed by the referenced product (OccurrenceDef::ChildNodeId), not by the parent.
TEST(BRepGraph_PermissionUpdateTest, RemoveRef_OccurrenceRef_PreservesProductToOccurrencesIndex)
{
  // Targeted regression for the wrong-key Unbind in GenOps::RemoveRef. Walks the
  // relation tables directly because BRepGraphInc_Relations::Validate currently
  // covers only topology references.
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());
  BRepGraph::EditorView::ProductOps& aProds = aGraph.Editor().Products();

  const BRepGraph_ProductId aParent = aProds.Add();
  aProds.AppendDocumentRoot(aParent);
  const BRepGraph_ProductId aChild = aProds.Add();
  aProds.AppendDocumentRoot(aChild);
  ASSERT_TRUE(aParent.IsValid());
  ASSERT_TRUE(aChild.IsValid());

  BRepGraph_OccurrenceRefId    aOccRefId;
  const BRepGraph_OccurrenceId aOccId =
    aProds.Append(aParent, aChild, TopLoc_Location(), BRepGraph_OccurrenceId(), &aOccRefId);
  ASSERT_TRUE(aOccId.IsValid());
  ASSERT_TRUE(aOccRefId.IsValid());

  bool aFoundBefore = false;
  for (const BRepGraph_OccurrenceId& anId :
       BRepGraph_OccurrencesOfChild(aGraph,
                                    aGraph.Topo().Gen().OccurrenceRefIds(BRepGraph_NodeId(aChild))))
  {
    if (anId == aOccId)
    {
      aFoundBefore = true;
    }
  }
  EXPECT_TRUE(aFoundBefore) << "Occurrence must be indexed under referenced product before remove";

  EXPECT_TRUE(aGraph.Editor().Gen().RemoveRef(BRepGraph_RefId(aOccRefId)));

  for (const BRepGraph_OccurrenceId& anId :
       BRepGraph_OccurrencesOfChild(aGraph,
                                    aGraph.Topo().Gen().OccurrenceRefIds(BRepGraph_NodeId(aChild))))
  {
    EXPECT_NE(anId, aOccId) << "RemoveRef must drop the referenced-product entry";
  }

  // The wrong-key bug would also leave a stale entry under the parent product id.
  for (const BRepGraph_OccurrenceId& anId : BRepGraph_OccurrencesOfChild(
         aGraph,
         aGraph.Topo().Gen().OccurrenceRefIds(BRepGraph_NodeId(aParent))))
  {
    EXPECT_NE(anId, aOccId) << "Bug: Unbind happened against parent product key";
  }
}

// B2: removing one coedge usage must leave the wire's other usages of the same edge
// reflected in the deduplicated Edge->Wire relation tables.
//
// Constructs a wire with two coedge usages whose CoEdges share the same edge so the
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

  NCollection_LinearVector<BRepGraph_CoEdgeId> aCoEdgeIds;
  aCoEdgeIds.Append(aGraph.Editor().CoEdges().Add(aEdge, TopAbs_FORWARD));
  aCoEdgeIds.Append(aGraph.Editor().CoEdges().Add(aEdge, TopAbs_REVERSED));
  const BRepGraph_WireId aWireId = aGraph.Editor().Wires().Add(aCoEdgeIds.ToArray1());
  ASSERT_TRUE(aWireId.IsValid());
  EXPECT_TRUE(aGraph.ValidateRelations());

  const BRepGraphInc::WireRelations& aWireRelations = aGraph.Topo().Wires().Relations(aWireId);
  ASSERT_EQ(aWireRelations.CoEdgeIds.Size(), 2u);
  const BRepGraph_CoEdgeId aFirstCoEdge = aWireRelations.CoEdgeIds.First();
  ASSERT_TRUE(aFirstCoEdge.IsValid());

  auto containsWire = [&](const BRepGraph_EdgeId theE) -> bool {
    for (const BRepGraph_WireId& aW : aGraph.Topo().Edges().WiresOf(theE))
    {
      if (aW == aWireId)
      {
        return true;
      }
    }
    return false;
  };
  ASSERT_TRUE(containsWire(aEdge)) << "Edge->Wire reverse must be present before removal";

  EXPECT_TRUE(aGraph.Editor().Wires().RemoveCoEdge(aWireId, aFirstCoEdge));
  EXPECT_TRUE(aGraph.ValidateRelations());
  EXPECT_TRUE(containsWire(aEdge))
    << "Sibling coedge still references the edge: Edge->Wire entry must survive";
}

// B3: SetRefChildVertexId on a self-loop edge (start == end == V_old) must keep
// the V_old->edge entry while the sibling slot still references it.
TEST(BRepGraph_PermissionUpdateTest, SetRefChildVertexId_SelfLoopSibling_KeepsRevIndexValid)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());
  const BRepGraph_EdgeId aEdge = makeSelfLoopEdge(aGraph);
  ASSERT_TRUE(aEdge.IsValid());
  EXPECT_TRUE(aGraph.ValidateRelations());

  const BRepGraphInc::EdgeDef& aDef        = aGraph.Topo().Edges().Definition(aEdge);
  const BRepGraph_VertexRefId  aStartRefId = aDef.StartVertexRefId;
  ASSERT_TRUE(aStartRefId.IsValid());

  const BRepGraph_VertexId aNewV = aGraph.Editor().Vertices().Add(gp_Pnt(1.0, 0.0, 0.0), 1.0e-7);

  aGraph.Editor().Vertices().SetRefChildVertexId(aStartRefId, aNewV);
  EXPECT_TRUE(aGraph.ValidateRelations());

  // EndVertexRef still references the original vertex, so the (Vold -> aEdge)
  // reverse entry must survive the start-side rebind.
  const BRepGraph_VertexId aOldV =
    aGraph.Topo().Edges().Definition(aEdge).EndVertexRefId.IsValid()
      ? aGraph.Refs()
          .Vertices()
          .Entry(aGraph.Topo().Edges().Definition(aEdge).EndVertexRefId)
          .ChildVertexId
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
  ASSERT_FALSE(aGraph.IsEmpty());
  const BRepGraph_VertexId aV = aGraph.Editor().Vertices().Add(gp_Pnt(0.0, 0.0, 0.0), 1.0e-7);
  NCollection_LinearVector<BRepGraph_NodeId> aChildren;
  aChildren.Append(BRepGraph_NodeId(aV));
  const BRepGraph_CompoundId aRoot = aGraph.Editor().Compounds().Add(aChildren.ToArray1());
  ASSERT_TRUE(aRoot.IsValid());

  // Splice the compound into itself by rewriting its first child ref.
  const BRepGraphInc::CompoundRelations& aRelations = aGraph.Topo().Compounds().Relations(aRoot);
  ASSERT_FALSE(aRelations.ChildRefIds.IsEmpty());
  const BRepGraph_ChildRefId aChildRefId = aRelations.ChildRefIds.First();
  aGraph.Editor().Gen().SetChildRefChildNodeId(aChildRefId, BRepGraph_NodeId(aRoot));

  BRepGraph              aCopy;
  const BRepGraph_NodeId aRootId = BRepGraph_Copy::CopyNode(aGraph,
                                                            aCopy,
                                                            BRepGraph_NodeId(aRoot),
                                                            BRepGraph_Copy::GeomPolicy::Copy,
                                                            BRepGraph_Copy::MeshPolicy::Drop);
  EXPECT_TRUE(aRootId.IsValid());
  EXPECT_FALSE(aCopy.IsEmpty());
  EXPECT_TRUE(aCopy.ValidateRelations());
}

// TransformNode with copyGeom on an assembly node must reject (returns invalid graph)
// rather than silently produce inconsistent geometry/location state.
TEST(BRepGraph_PermissionUpdateTest, TransformNode_AssemblyWithCopyGeom_Rejected)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());
  const BRepGraph_ProductId aProd = aGraph.Editor().Products().Add();
  aGraph.Editor().Products().AppendDocumentRoot(aProd);
  ASSERT_TRUE(aProd.IsValid());

  gp_Trsf aT;
  aT.SetTranslation(gp_Vec(1.0, 0.0, 0.0));
  BRepGraph              aResult;
  const BRepGraph_NodeId aRootId =
    BRepGraph_Transform::TransformNode(aGraph,
                                       aResult,
                                       BRepGraph_NodeId(aProd),
                                       aT,
                                       BRepGraph_Copy::GeomPolicy::Copy,
                                       BRepGraph_Copy::MeshPolicy::Drop);
  EXPECT_FALSE(aRootId.IsValid());
}

// Companion to the above: Occurrence node with copyGeom must be rejected too.
TEST(BRepGraph_PermissionUpdateTest, TransformNode_OccurrenceWithCopyGeom_Rejected)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());
  BRepGraph::EditorView::ProductOps& aProds  = aGraph.Editor().Products();
  const BRepGraph_ProductId          aParent = aProds.Add();
  aProds.AppendDocumentRoot(aParent);
  const BRepGraph_ProductId aChild = aProds.Add();
  aProds.AppendDocumentRoot(aChild);
  ASSERT_TRUE(aParent.IsValid());
  ASSERT_TRUE(aChild.IsValid());
  const BRepGraph_OccurrenceId aOccId = aProds.Append(aParent, aChild, TopLoc_Location());
  ASSERT_TRUE(aOccId.IsValid());

  gp_Trsf aT;
  aT.SetTranslation(gp_Vec(1.0, 0.0, 0.0));
  BRepGraph              aResult;
  const BRepGraph_NodeId aRootId =
    BRepGraph_Transform::TransformNode(aGraph,
                                       aResult,
                                       BRepGraph_NodeId(aOccId),
                                       aT,
                                       BRepGraph_Copy::GeomPolicy::Copy,
                                       BRepGraph_Copy::MeshPolicy::Drop);
  EXPECT_FALSE(aRootId.IsValid());
}

// LinkProducts with the unified signature returns the freshly inserted OccurrenceRef
// id through the optional out pointer.
TEST(BRepGraph_PermissionUpdateTest, LinkProducts_OutOccurrenceRefId_Populated)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());
  BRepGraph::EditorView::ProductOps& aProds  = aGraph.Editor().Products();
  const BRepGraph_ProductId          aParent = aProds.Add();
  aProds.AppendDocumentRoot(aParent);
  const BRepGraph_ProductId aChild = aProds.Add();
  aProds.AppendDocumentRoot(aChild);
  ASSERT_TRUE(aParent.IsValid());
  ASSERT_TRUE(aChild.IsValid());

  BRepGraph_OccurrenceRefId    aOccRefId;
  const BRepGraph_OccurrenceId aOccId =
    aProds.Append(aParent, aChild, TopLoc_Location(), BRepGraph_OccurrenceId(), &aOccRefId);
  ASSERT_TRUE(aOccId.IsValid());
  ASSERT_TRUE(aOccRefId.IsValid());
  EXPECT_EQ(aGraph.Refs().Occurrences().Entry(aOccRefId).ChildOccurrenceId, aOccId);

  // Default out-pointer = nullptr is also legal.
  const BRepGraph_OccurrenceId aOccId2 = aProds.Append(aParent, aChild, TopLoc_Location());
  EXPECT_TRUE(aOccId2.IsValid());
}

TEST(BRepGraph_PermissionUpdateTest, SetOccurrenceChildNodeId_RejectsOccurrenceChild)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());
  BRepGraph::EditorView::ProductOps& aProds  = aGraph.Editor().Products();
  const BRepGraph_ProductId          aParent = aProds.Add();
  aProds.AppendDocumentRoot(aParent);
  const BRepGraph_ProductId aChild = aProds.Add();
  aProds.AppendDocumentRoot(aChild);
  ASSERT_TRUE(aParent.IsValid());
  ASSERT_TRUE(aChild.IsValid());

  const BRepGraph_OccurrenceId aOccId = aProds.Append(aParent, aChild, TopLoc_Location());
  ASSERT_TRUE(aOccId.IsValid());
  const BRepGraph_NodeId anOldChild = aGraph.Topo().Occurrences().Definition(aOccId).ChildNodeId;

  aGraph.Editor().Occurrences().SetChildNodeId(aOccId, BRepGraph_NodeId(aOccId));

  EXPECT_EQ(aGraph.Topo().Occurrences().Definition(aOccId).ChildNodeId, anOldChild);
  EXPECT_TRUE(containsCurrentId(
    BRepGraph_OccurrencesOfChild(aGraph, aGraph.Topo().Gen().OccurrenceRefIds(anOldChild)),
    aOccId));
  EXPECT_FALSE(containsCurrentId(
    BRepGraph_OccurrencesOfChild(aGraph,
                                 aGraph.Topo().Gen().OccurrenceRefIds(BRepGraph_NodeId(aOccId))),
    aOccId));
  EXPECT_TRUE(aGraph.ValidateRelations());
}

TEST(BRepGraph_PermissionUpdateTest, SetOccurrenceChildNodeId_InvalidOccurrenceNoOp)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());
  BRepGraph::EditorView::ProductOps& aProds = aGraph.Editor().Products();
  const BRepGraph_ProductId          aChild = aProds.Add();
  aProds.AppendDocumentRoot(aChild);
  ASSERT_TRUE(aChild.IsValid());

  EXPECT_NO_THROW(aGraph.Editor().Occurrences().SetChildNodeId(BRepGraph_OccurrenceId(100000),
                                                               BRepGraph_NodeId(aChild)));
  EXPECT_TRUE(aGraph.ValidateRelations());
}

TEST(BRepGraph_PermissionUpdateTest, SetOccurrenceChildNodeId_MutGuardRejectsRemovedChild)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());
  BRepGraph::EditorView::ProductOps& aProds  = aGraph.Editor().Products();
  const BRepGraph_ProductId          aParent = aProds.Add();
  aProds.AppendDocumentRoot(aParent);
  const BRepGraph_ProductId aChild = aProds.Add();
  aProds.AppendDocumentRoot(aChild);
  const BRepGraph_ProductId aRemovedChild = aProds.Add();
  aProds.AppendDocumentRoot(aRemovedChild);
  ASSERT_TRUE(aParent.IsValid());
  ASSERT_TRUE(aChild.IsValid());
  ASSERT_TRUE(aRemovedChild.IsValid());

  const BRepGraph_OccurrenceId aOccId = aProds.Append(aParent, aChild, TopLoc_Location());
  ASSERT_TRUE(aOccId.IsValid());
  const BRepGraph_NodeId anOldChild = aGraph.Topo().Occurrences().Definition(aOccId).ChildNodeId;
  aGraph.Editor().Gen().RemoveNode(aRemovedChild);

  {
    BRepGraph_MutGuard<BRepGraphInc::OccurrenceDef> aMut =
      aGraph.Editor().Occurrences().Mut(aOccId);
    aGraph.Editor().Occurrences().SetChildNodeId(aMut, BRepGraph_NodeId(aRemovedChild));
    EXPECT_FALSE(aMut.IsDirty());
  }

  EXPECT_EQ(aGraph.Topo().Occurrences().Definition(aOccId).ChildNodeId, anOldChild);
  EXPECT_TRUE(containsCurrentId(
    BRepGraph_OccurrencesOfChild(aGraph, aGraph.Topo().Gen().OccurrenceRefIds(anOldChild)),
    aOccId));
  EXPECT_FALSE(containsCurrentId(BRepGraph_OccurrencesOfChild(aGraph,
                                                              aGraph.Topo().Gen().OccurrenceRefIds(
                                                                BRepGraph_NodeId(aRemovedChild))),
                                 aOccId));
  EXPECT_TRUE(aGraph.ValidateRelations());
}

// MoveRef: a pure rotation/translation must succeed; a scaled trsf must fail with no
// state change.
TEST(BRepGraph_PermissionUpdateTest, MoveRef_ChildRefScaledTrsf_RejectedWithoutMutation)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  NCollection_LinearVector<BRepGraph_NodeId> aChildren;
  aChildren.Append(BRepGraph_NodeId(BRepGraph_SolidId::Start()));
  const BRepGraph_CompoundId aCompound = aGraph.Editor().Compounds().Add(aChildren.ToArray1());
  ASSERT_TRUE(aCompound.IsValid());
  ASSERT_EQ(aGraph.Refs().Children().IdsOf(aCompound).Size(), 1);
  const BRepGraph_ChildRefId aChildRef = aGraph.Refs().Children().IdsOf(aCompound).First();

  gp_Trsf aScaled;
  aScaled.SetScaleFactor(2.0);
  EXPECT_FALSE(BRepGraph_Transform::MoveRef(aGraph, aChildRef, aScaled));
  EXPECT_TRUE(aGraph.Refs().Children().Entry(aChildRef).LocalLocation.IsIdentity());

  gp_Trsf aTrans;
  aTrans.SetTranslation(gp_Vec(1.0, 2.0, 3.0));
  EXPECT_TRUE(BRepGraph_Transform::MoveRef(aGraph, aChildRef, aTrans));
  EXPECT_FALSE(aGraph.Refs().Children().Entry(aChildRef).LocalLocation.IsIdentity());
}

// RemoveSubgraph cascade rebuilds the relation tables exactly once at the outermost
// scope; nested calls during recursion must not trigger intermediate rebuilds, and
// the final state must validate.
TEST(BRepGraph_PermissionUpdateTest, RemoveSubgraph_NestedCascade_FinalStateValid)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());
  EXPECT_TRUE(aGraph.ValidateRelations());

  BRepGraph_SolidId aSolidId;
  for (BRepGraph_SolidIterator aIt(aGraph); aIt.More(); aIt.Next())
  {
    aSolidId = aIt.CurrentId();
    break;
  }
  ASSERT_TRUE(aSolidId.IsValid());

  aGraph.Editor().Gen().RemoveSubgraph(BRepGraph_NodeId(aSolidId));
  EXPECT_TRUE(aGraph.ValidateRelations());
}

// MutGuard: a guard that observes but never writes must not bump OwnGen on destruction;
// MarkDirty() must force the bump even when no setter was called.
TEST(BRepGraph_PermissionUpdateTest, MutGuard_DirtyFlag_RespectsExplicitMarkAndCleanScope)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

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
    std::ignore                                      = aGuard->Tolerance; // read-only access only
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
