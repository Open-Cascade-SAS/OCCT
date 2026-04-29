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

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepTools_ReShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Vertex.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

namespace
{
TopoDS_Vertex MakeVertex(const double theX, const double theY, const double theZ)
{
  return BRepBuilderAPI_MakeVertex(gp_Pnt(theX, theY, theZ));
}

TopoDS_Edge MakeEdge(const TopoDS_Vertex& theV1, const TopoDS_Vertex& theV2)
{
  return BRepBuilderAPI_MakeEdge(theV1, theV2);
}
} // namespace

// Value() returns the direct replacement only (one hop); ValueLeaf() follows the
// chain through intermediate replacements to its terminal shape.
TEST(BRepTools_ReShapeTest, ValueLeaf_FollowsChainToLeaf)
{
  const TopoDS_Vertex aA = MakeVertex(0, 0, 0);
  const TopoDS_Vertex aB = MakeVertex(1, 0, 0);
  const TopoDS_Vertex aC = MakeVertex(2, 0, 0);

  BRepTools_ReShape aReShape;
  aReShape.Replace(aA, aB);
  aReShape.Replace(aB, aC);

  EXPECT_TRUE(aReShape.Value(aA).IsSame(aB)) << "Value() must yield only the direct replacement";
  EXPECT_TRUE(aReShape.ValueLeaf(aA).IsSame(aC)) << "ValueLeaf() must walk the full chain";
  EXPECT_TRUE(aReShape.ValueLeaf(aB).IsSame(aC));
  EXPECT_TRUE(aReShape.ValueLeaf(aC).IsSame(aC)) << "Terminal element is a fixpoint";
}

// ValueLeaf() must terminate on an unrecorded shape without touching the map.
TEST(BRepTools_ReShapeTest, ValueLeaf_UnrecordedShapeIsIdentity)
{
  const TopoDS_Vertex aA = MakeVertex(0, 0, 0);

  BRepTools_ReShape aReShape;
  EXPECT_TRUE(aReShape.ValueLeaf(aA).IsSame(aA));
}

// A chain ending in a removal (null replacement) must surface as a null shape.
TEST(BRepTools_ReShapeTest, ValueLeaf_ChainEndingInRemoveReturnsNull)
{
  const TopoDS_Vertex aA = MakeVertex(0, 0, 0);
  const TopoDS_Vertex aB = MakeVertex(1, 0, 0);

  BRepTools_ReShape aReShape;
  aReShape.Replace(aA, aB);
  aReShape.Remove(aB);

  EXPECT_TRUE(aReShape.ValueLeaf(aA).IsNull());
}

// A direct A->B then B->A would close a cycle in the replacement map.
// The second Replace() must be rejected; the first stays intact.
TEST(BRepTools_ReShapeTest, Replace_RejectsDirectCycle)
{
  const TopoDS_Vertex aA = MakeVertex(0, 0, 0);
  const TopoDS_Vertex aB = MakeVertex(1, 0, 0);

  BRepTools_ReShape aReShape;
  aReShape.Replace(aA, aB);
  aReShape.Replace(aB, aA); // would create A -> B -> A

  EXPECT_TRUE(aReShape.Value(aA).IsSame(aB)) << "First replacement must remain in effect";
  EXPECT_TRUE(aReShape.Value(aB).IsSame(aB)) << "Cyclic second replacement must have been rejected";
  EXPECT_TRUE(aReShape.ValueLeaf(aA).IsSame(aB)) << "Chain must terminate, not loop";
}

// A longer cycle A -> B -> C -> A must also be rejected at the closing edge.
TEST(BRepTools_ReShapeTest, Replace_RejectsLongerCycle)
{
  const TopoDS_Vertex aA = MakeVertex(0, 0, 0);
  const TopoDS_Vertex aB = MakeVertex(1, 0, 0);
  const TopoDS_Vertex aC = MakeVertex(2, 0, 0);

  BRepTools_ReShape aReShape;
  aReShape.Replace(aA, aB);
  aReShape.Replace(aB, aC);
  aReShape.Replace(aC, aA); // would create A -> B -> C -> A

  EXPECT_TRUE(aReShape.ValueLeaf(aA).IsSame(aC)) << "Chain terminates at C; no cycle formed";
  EXPECT_TRUE(aReShape.Value(aC).IsSame(aC)) << "Closing replacement was rejected";
}

// Apply() must not stack-overflow when a shape's replacement is a compound that
// transitively contains the original shape as a sub-shape. This is the pattern
// that crashed in shape healing on non-orientable / shared-edge inputs.
TEST(BRepTools_ReShapeTest, Apply_HandlesStructuralContainmentWithoutCrash)
{
  const TopoDS_Vertex aV1 = MakeVertex(0, 0, 0);
  const TopoDS_Vertex aV2 = MakeVertex(1, 0, 0);
  const TopoDS_Vertex aV3 = MakeVertex(2, 0, 0);
  const TopoDS_Edge   anE = MakeEdge(aV1, aV2);

  // Build a compound that contains the edge (plus another shape for flavour).
  TopoDS_Compound aContainer;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aContainer);
  aBuilder.Add(aContainer, anE);
  aBuilder.Add(aContainer, aV3);

  BRepTools_ReShape aReShape;
  aReShape.Replace(anE, aContainer); // e -> compound{e, v3}

  TopoDS_Shape aResult;
  ASSERT_NO_FATAL_FAILURE(aResult = aReShape.Apply(anE));
  EXPECT_FALSE(aResult.IsNull());
}

// Legitimate diamond sharing (the same sub-shape reached via different parent
// paths) must continue to process normally; the DFS guard must not mistake it
// for a cycle. Two edges sharing a vertex form the minimal diamond.
TEST(BRepTools_ReShapeTest, Apply_DiamondSharingIsProcessedCorrectly)
{
  const TopoDS_Vertex aV1     = MakeVertex(0, 0, 0);
  const TopoDS_Vertex aV2     = MakeVertex(1, 0, 0);
  const TopoDS_Vertex aV3     = MakeVertex(2, 0, 0);
  const TopoDS_Vertex aV2Repl = MakeVertex(1.5, 0, 0);

  // aE1 and aE2 both reference aV2 as an endpoint - that's the diamond.
  const TopoDS_Edge aE1 = MakeEdge(aV1, aV2);
  const TopoDS_Edge aE2 = MakeEdge(aV2, aV3);

  TopoDS_Compound aPair;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aPair);
  aBuilder.Add(aPair, aE1);
  aBuilder.Add(aPair, aE2);

  BRepTools_ReShape aReShape;
  aReShape.Replace(aV2, aV2Repl);

  const TopoDS_Shape aResult = aReShape.Apply(aPair);
  ASSERT_FALSE(aResult.IsNull());

  // aV2 must have been substituted everywhere it appeared; aV2Repl must appear;
  // the original aV2 TShape must no longer be present in the result.
  int aNbReplVertices = 0;
  for (TopExp_Explorer anExp(aResult, TopAbs_VERTEX); anExp.More(); anExp.Next())
  {
    EXPECT_FALSE(anExp.Current().TShape() == aV2.TShape())
      << "Original v2 must have been replaced on every diamond arm";
    if (anExp.Current().TShape() == aV2Repl.TShape())
    {
      ++aNbReplVertices;
    }
  }
  EXPECT_GE(aNbReplVertices, 2) << "Replacement must appear on both arms of the diamond";
}

// Clear() must drop every binding and reset the map to identity behaviour.
TEST(BRepTools_ReShapeTest, Clear_DropsAllBindings)
{
  const TopoDS_Vertex aA = MakeVertex(0, 0, 0);
  const TopoDS_Vertex aB = MakeVertex(1, 0, 0);

  BRepTools_ReShape aReShape;
  aReShape.Replace(aA, aB);
  ASSERT_TRUE(aReShape.Value(aA).IsSame(aB));

  aReShape.Clear();
  EXPECT_TRUE(aReShape.Value(aA).IsSame(aA));
  EXPECT_TRUE(aReShape.ValueLeaf(aA).IsSame(aA));
}

// Re-replacing the same shape must overwrite the previous binding.
// This is the documented behaviour and must not be blocked by the cycle guard.
TEST(BRepTools_ReShapeTest, Replace_LastWins)
{
  const TopoDS_Vertex aA = MakeVertex(0, 0, 0);
  const TopoDS_Vertex aB = MakeVertex(1, 0, 0);
  const TopoDS_Vertex aC = MakeVertex(2, 0, 0);

  BRepTools_ReShape aReShape;
  aReShape.Replace(aA, aB);
  aReShape.Replace(aA, aC); // overwrite

  EXPECT_TRUE(aReShape.Value(aA).IsSame(aC));
  EXPECT_TRUE(aReShape.ValueLeaf(aA).IsSame(aC));
}

// Replace(X, X) is a no-op - the shape equals itself, no binding should be stored.
TEST(BRepTools_ReShapeTest, Replace_SelfIsNoOp)
{
  const TopoDS_Vertex aA = MakeVertex(0, 0, 0);

  BRepTools_ReShape aReShape;
  aReShape.Replace(aA, aA);

  EXPECT_FALSE(aReShape.IsRecorded(aA));
  EXPECT_TRUE(aReShape.Value(aA).IsSame(aA));
}

// A Remove request must propagate through Apply() so that the sub-shape is
// dropped from the rebuilt parent.
TEST(BRepTools_ReShapeTest, Apply_RemoveDropsSubShapeFromParent)
{
  const TopoDS_Vertex aV1 = MakeVertex(0, 0, 0);
  const TopoDS_Vertex aV2 = MakeVertex(1, 0, 0);
  const TopoDS_Vertex aV3 = MakeVertex(2, 0, 0);

  TopoDS_Compound aParent;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aParent);
  aBuilder.Add(aParent, aV1);
  aBuilder.Add(aParent, aV2);
  aBuilder.Add(aParent, aV3);

  BRepTools_ReShape aReShape;
  aReShape.Remove(aV2);

  const TopoDS_Shape aResult = aReShape.Apply(aParent);
  ASSERT_FALSE(aResult.IsNull());

  int aNbVerts = 0;
  for (TopoDS_Iterator anIt(aResult); anIt.More(); anIt.Next())
  {
    EXPECT_FALSE(anIt.Value().TShape() == aV2.TShape()) << "v2 should have been removed";
    ++aNbVerts;
  }
  EXPECT_EQ(aNbVerts, 2);
}

// The fuller Moebius-style pattern: an edge appears in two different parents
// (simulated here by two compounds), each parent is rebuilt through Apply,
// and the edge replacement must propagate to both without triggering the
// DFS cycle guard. Regression for the original "shared sub-shape" failure.
TEST(BRepTools_ReShapeTest, Apply_SharedEdgeAcrossTwoParents)
{
  const TopoDS_Vertex aV1    = MakeVertex(0, 0, 0);
  const TopoDS_Vertex aV2    = MakeVertex(1, 0, 0);
  const TopoDS_Vertex aV3    = MakeVertex(2, 0, 0);
  const TopoDS_Edge   aE1    = MakeEdge(aV1, aV2);
  const TopoDS_Edge   aE2    = MakeEdge(aV2, aV3);
  const TopoDS_Edge   aE1New = MakeEdge(MakeVertex(0.5, 0, 0), aV2);

  // Build two parents both referencing aE1: parentA{aE1, aV3}, parentB{aE1, aE2}.
  TopoDS_Compound aParentA, aParentB;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aParentA);
  aBuilder.Add(aParentA, aE1);
  aBuilder.Add(aParentA, aV3);
  aBuilder.MakeCompound(aParentB);
  aBuilder.Add(aParentB, aE1);
  aBuilder.Add(aParentB, aE2);

  BRepTools_ReShape aReShape;
  aReShape.Replace(aE1, aE1New);

  const TopoDS_Shape aResultA = aReShape.Apply(aParentA);
  const TopoDS_Shape aResultB = aReShape.Apply(aParentB);
  ASSERT_FALSE(aResultA.IsNull());
  ASSERT_FALSE(aResultB.IsNull());

  bool aFoundNewInA = false, aFoundNewInB = false;
  for (TopExp_Explorer anExp(aResultA, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    EXPECT_FALSE(anExp.Current().TShape() == aE1.TShape());
    if (anExp.Current().TShape() == aE1New.TShape())
    {
      aFoundNewInA = true;
    }
  }
  for (TopExp_Explorer anExp(aResultB, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    EXPECT_FALSE(anExp.Current().TShape() == aE1.TShape());
    if (anExp.Current().TShape() == aE1New.TShape())
    {
      aFoundNewInB = true;
    }
  }
  EXPECT_TRUE(aFoundNewInA);
  EXPECT_TRUE(aFoundNewInB);
}

// Idempotence: Apply() on a shape that has no bindings must return
// the shape unchanged. Guards against accidental rebuild-on-no-change.
TEST(BRepTools_ReShapeTest, Apply_NoBindingsIsIdentity)
{
  const TopoDS_Vertex aV1 = MakeVertex(0, 0, 0);
  const TopoDS_Vertex aV2 = MakeVertex(1, 0, 0);
  const TopoDS_Edge   anE = MakeEdge(aV1, aV2);

  BRepTools_ReShape  aReShape;
  const TopoDS_Shape aResult = aReShape.Apply(anE);
  EXPECT_TRUE(aResult.IsSame(anE));
}

// Deep structural containment: a replacement compound contains a nested
// compound that in turn contains the original shape. DFS guard must still
// break the cycle regardless of depth.
TEST(BRepTools_ReShapeTest, Apply_DeepStructuralContainmentWithoutCrash)
{
  const TopoDS_Vertex aV1     = MakeVertex(0, 0, 0);
  const TopoDS_Vertex aV2     = MakeVertex(1, 0, 0);
  const TopoDS_Vertex anExtra = MakeVertex(3, 0, 0);
  const TopoDS_Edge   anE     = MakeEdge(aV1, aV2);

  // inner{e}, outer{inner, extra}: e is reachable two levels down.
  TopoDS_Compound anInner, anOuter;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(anInner);
  aBuilder.Add(anInner, anE);
  aBuilder.MakeCompound(anOuter);
  aBuilder.Add(anOuter, anInner);
  aBuilder.Add(anOuter, anExtra);

  BRepTools_ReShape aReShape;
  aReShape.Replace(anE, anOuter); // e -> outer{inner{e}, extra}

  TopoDS_Shape aResult;
  ASSERT_NO_FATAL_FAILURE(aResult = aReShape.Apply(anE));
  EXPECT_FALSE(aResult.IsNull());
}

// Locations must be tracked when ModeConsiderLocation is enabled.
TEST(BRepTools_ReShapeTest, ValueLeaf_ConsidersLocationMode)
{
  const TopoDS_Vertex aA = MakeVertex(0, 0, 0);
  const TopoDS_Vertex aB = MakeVertex(1, 0, 0);

  BRepTools_ReShape aReShape;
  aReShape.ModeConsiderLocation() = true;
  aReShape.Replace(aA, aB);

  EXPECT_TRUE(aReShape.ValueLeaf(aA).IsSame(aB));
}
