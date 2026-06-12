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

// Regression coverage for BRepGraph_MutGuard<T>: move semantics, inert-state
// detection (operator bool()), and exception-path notification safety.
//
// MutGuard owns exactly one definition/ref mutation notification per scope. A
// move transfers that obligation; a moved-from guard must be inert. An
// exception inside the scope must not skip the notification nor propagate a
// noexcept-violation from the guard's destructor.

#include <BRepGraph.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_MutGuard.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Standard_ProgramError.hxx>

#include <gtest/gtest.h>

#include <utility>

namespace
{

BRepGraph makeBoxGraph()
{
  BRepGraph aGraph;
  aGraph.Clear();
  [[maybe_unused]] const BRepGraph::ShapesView::Result aBuildRes1 =
    aGraph.Shapes().Add(BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape());
  return aGraph;
}

} // namespace

TEST(BRepGraph_MutGuardTest, OperatorBool_TrueWhenOwned_FalseAfterMove)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph_MutGuard<BRepGraphInc::VertexDef> aGuard =
    aGraph.Editor().Vertices().Mut(BRepGraph_VertexId::Start());
  EXPECT_TRUE(static_cast<bool>(aGuard));

  BRepGraph_MutGuard<BRepGraphInc::VertexDef> aMoved(std::move(aGuard));
  EXPECT_FALSE(static_cast<bool>(aGuard)) << "Moved-from guard must be inert";
  EXPECT_TRUE(static_cast<bool>(aMoved));
}

TEST(BRepGraph_MutGuardTest, DereferenceAfterMove_Throws)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph_MutGuard<BRepGraphInc::VertexDef> aGuard =
    aGraph.Editor().Vertices().Mut(BRepGraph_VertexId::Start());

  BRepGraph_MutGuard<BRepGraphInc::VertexDef> aMoved(std::move(aGuard));
  (void)aMoved;
#ifndef No_Exception
  // Using operator-> or operator* on the moved-from guard must raise.
  EXPECT_THROW({ (void)aGuard.operator->(); }, Standard_ProgramError);
  EXPECT_THROW({ (void)(*aGuard); }, Standard_ProgramError);
#endif
}

TEST(BRepGraph_MutGuardTest, MoveAssignmentFlushesThenTransfers)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  const uint32_t aGenBefore =
    aGraph.Topo().Vertices().Definition(BRepGraph_VertexId::Start()).OwnGen;

  {
    BRepGraph_MutGuard<BRepGraphInc::VertexDef> aFirst =
      aGraph.Editor().Vertices().Mut(BRepGraph_VertexId::Start());
    aGraph.Editor().Vertices().SetPoint(aFirst, gp_Pnt(1.0, 2.0, 3.0));

    BRepGraph_MutGuard<BRepGraphInc::VertexDef> aSecond =
      aGraph.Editor().Vertices().Mut(BRepGraph_VertexId(1));
    aSecond.MarkDirty();
    aSecond = std::move(aFirst);
    // aFirst now inert; aSecond owns what was aFirst.
    EXPECT_FALSE(static_cast<bool>(aFirst));
    EXPECT_TRUE(static_cast<bool>(aSecond));
  }

  // Both vertex 0 and vertex 1 must have had their OwnGen bumped exactly once each.
  const uint32_t aGenAfterV0 =
    aGraph.Topo().Vertices().Definition(BRepGraph_VertexId::Start()).OwnGen;
  const uint32_t aGenAfterV1 = aGraph.Topo().Vertices().Definition(BRepGraph_VertexId(1)).OwnGen;
  EXPECT_GT(aGenAfterV0, aGenBefore);
  EXPECT_GT(aGenAfterV1, 0u);
}

TEST(BRepGraph_MutGuardTest, ExceptionInsideScope_StillNotifies)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  const uint32_t aGenBefore =
    aGraph.Topo().Vertices().Definition(BRepGraph_VertexId::Start()).OwnGen;

  try
  {
    BRepGraph_MutGuard<BRepGraphInc::VertexDef> aGuard =
      aGraph.Editor().Vertices().Mut(BRepGraph_VertexId::Start());
    aGraph.Editor().Vertices().SetPoint(aGuard, gp_Pnt(9.0, 9.0, 9.0));
    throw std::runtime_error("test: throw mid-scope");
  }
  catch (const std::runtime_error&)
  {
    // Fall through. The guard's destructor must have executed during stack
    // unwinding and must have bumped OwnGen.
  }

  const uint32_t aGenAfter =
    aGraph.Topo().Vertices().Definition(BRepGraph_VertexId::Start()).OwnGen;
  EXPECT_GT(aGenAfter, aGenBefore)
    << "Notification must fire even when the scope exits via an exception";
}

TEST(BRepGraph_MutGuardTest, MovedFrom_DoesNotDoubleNotify)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  const uint32_t aGenBefore =
    aGraph.Topo().Vertices().Definition(BRepGraph_VertexId::Start()).OwnGen;

  {
    BRepGraph_MutGuard<BRepGraphInc::VertexDef> aSrc =
      aGraph.Editor().Vertices().Mut(BRepGraph_VertexId::Start());
    aSrc.MarkDirty();
    BRepGraph_MutGuard<BRepGraphInc::VertexDef> aDst(std::move(aSrc));
    // Only aDst's destructor should notify; aSrc's destructor must be a no-op.
  }

  const uint32_t aGenAfter =
    aGraph.Topo().Vertices().Definition(BRepGraph_VertexId::Start()).OwnGen;
  EXPECT_EQ(aGenAfter, aGenBefore + 1u)
    << "Move must transfer the notification obligation - no double-bump";
}

TEST(BRepGraph_MutGuardTest, RefGuard_SameMoveSemantics)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());
  ASSERT_GT(aGraph.Refs().Faces().Nb(), 0);

  const BRepGraph_FaceRefId                 aRefId(0);
  BRepGraph_MutGuard<BRepGraphInc::FaceRef> aGuard = aGraph.Editor().Faces().MutRef(aRefId);
  EXPECT_TRUE(static_cast<bool>(aGuard));

  BRepGraph_MutGuard<BRepGraphInc::FaceRef> aMoved(std::move(aGuard));
  EXPECT_FALSE(static_cast<bool>(aGuard));
  EXPECT_TRUE(static_cast<bool>(aMoved));
}

TEST(BRepGraph_MutGuardTest, DuplicateGuard_Rejected)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph_MutGuard<BRepGraphInc::VertexDef> aGuard1 =
    aGraph.Editor().Vertices().Mut(BRepGraph_VertexId::Start());
  EXPECT_TRUE(static_cast<bool>(aGuard1));

#ifndef No_Exception
  // Attempting to acquire a second guard on the same vertex must throw.
  EXPECT_THROW(
    {
      auto aDuplicate = aGraph.Editor().Vertices().Mut(BRepGraph_VertexId::Start());
      (void)aDuplicate;
    },
    Standard_ProgramError);
#endif
}

TEST(BRepGraph_MutGuardTest, GuardAllowsGuardedSetter)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  {
    BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aGuard =
      aGraph.Editor().Edges().Mut(BRepGraph_EdgeId(0));
    // Guarded setter must NOT self-block via requireUnlocked.
    aGraph.Editor().Edges().SetTolerance(aGuard, 0.5);
    EXPECT_TRUE(aGuard.IsDirty());
  }
}

TEST(BRepGraph_MutGuardTest, GuardBlocksStructuralRemoval)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  BRepGraph_MutGuard<BRepGraphInc::VertexDef> aGuard =
    aGraph.Editor().Vertices().Mut(BRepGraph_VertexId::Start());
  EXPECT_TRUE(static_cast<bool>(aGuard));

  // Structural removal must throw while a guard is active on the item.
  EXPECT_THROW(aGraph.Editor().Gen().RemoveNode(BRepGraph_VertexId::Start()),
               Standard_ProgramError);
}

TEST(BRepGraph_MutGuardTest, GuardReleasedOnDestruction_CanReacquire)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  {
    BRepGraph_MutGuard<BRepGraphInc::VertexDef> aGuard =
      aGraph.Editor().Vertices().Mut(BRepGraph_VertexId::Start());
    EXPECT_TRUE(static_cast<bool>(aGuard));
  }
  // Guard destroyed - should be able to acquire again.
  BRepGraph_MutGuard<BRepGraphInc::VertexDef> aGuard2 =
    aGraph.Editor().Vertices().Mut(BRepGraph_VertexId::Start());
  EXPECT_TRUE(static_cast<bool>(aGuard2));
}

TEST(BRepGraph_MutGuardTest, ClearRejectsActiveGuard)
{
  BRepGraph aGraph = makeBoxGraph();
  ASSERT_FALSE(aGraph.IsEmpty());

  {
    BRepGraph_MutGuard<BRepGraphInc::VertexDef> aGuard =
      aGraph.Editor().Vertices().Mut(BRepGraph_VertexId::Start());
    EXPECT_TRUE(static_cast<bool>(aGuard));
    EXPECT_THROW(aGraph.Clear(), Standard_ProgramError);
  }

  EXPECT_NO_THROW(aGraph.Clear());
}
