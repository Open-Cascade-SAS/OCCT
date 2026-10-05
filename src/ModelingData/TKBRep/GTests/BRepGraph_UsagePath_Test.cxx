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

#include <BRepGraph_UsagePath.hxx>

#include <gtest/gtest.h>

#include <functional>

namespace
{
BRepGraph_UsagePath::Step makeStep(const BRepGraph_NodeId theNode,
                                   const BRepGraph_RefId  theRef,
                                   const uint32_t         theStepIndex)
{
  return BRepGraph_UsagePath::Step{theNode, theRef, theStepIndex};
}
} // namespace

TEST(BRepGraph_UsagePathTest, AppendInsertAndClearMaintainStepOrder)
{
  BRepGraph_UsagePath aPath(3);
  EXPECT_TRUE(aPath.IsEmpty());

  const BRepGraph_UsagePath::Step aRoot =
    makeStep(BRepGraph_CompoundId::Start(),
             BRepGraph_RefId(),
             BRepGraph_UsagePath::Step::THE_INVALID_STEP_INDEX);
  const BRepGraph_UsagePath::Step aFace =
    makeStep(BRepGraph_FaceId::Start(), BRepGraph_FaceRefId::Start(), 2u);
  const BRepGraph_UsagePath::Step aWire =
    makeStep(BRepGraph_WireId::Start(), BRepGraph_WireRefId::Start(), 0u);

  aPath.Append(aRoot);
  aPath.Append(aWire);
  aPath.InsertBefore(1, aFace);

  ASSERT_EQ(aPath.Size(), 3u);
  EXPECT_EQ(aPath.First(), aRoot);
  EXPECT_EQ(aPath.Value(1), aFace);
  EXPECT_EQ(aPath.Last(), aWire);

  aPath.Clear();
  EXPECT_TRUE(aPath.IsEmpty());
}

TEST(BRepGraph_UsagePathTest, EqualityUsesEveryStep)
{
  const BRepGraph_UsagePath::Step aFirst =
    makeStep(BRepGraph_ProductId::Start(), BRepGraph_RefId(), 0u);
  const BRepGraph_UsagePath::Step aMiddle =
    makeStep(BRepGraph_OccurrenceId::Start(), BRepGraph_OccurrenceRefId::Start(), 1u);
  const BRepGraph_UsagePath::Step aDifferentMiddle =
    makeStep(BRepGraph_OccurrenceId(3), BRepGraph_OccurrenceRefId(5), 1u);
  const BRepGraph_UsagePath::Step aLast =
    makeStep(BRepGraph_FaceId::Start(), BRepGraph_FaceRefId::Start(), 2u);

  BRepGraph_UsagePath aLeft;
  aLeft.Append(aFirst);
  aLeft.Append(aMiddle);
  aLeft.Append(aLast);

  BRepGraph_UsagePath aSame;
  aSame.Append(aFirst);
  aSame.Append(aMiddle);
  aSame.Append(aLast);

  BRepGraph_UsagePath aDifferent;
  aDifferent.Append(aFirst);
  aDifferent.Append(aDifferentMiddle);
  aDifferent.Append(aLast);

  EXPECT_EQ(aLeft, aSame);
  EXPECT_TRUE(aLeft.IsEqual(aSame));
  EXPECT_FALSE(aLeft == aDifferent);
  EXPECT_EQ(aLeft.HashCode(), aSame.HashCode());
  EXPECT_NE(aLeft.HashCode(), aDifferent.HashCode());
}

TEST(BRepGraph_UsagePathTest, CopiesPreserveIndependentValues)
{
  const BRepGraph_UsagePath::Step aRoot =
    makeStep(BRepGraph_ProductId::Start(), BRepGraph_RefId(), 0u);
  const BRepGraph_UsagePath::Step aChild =
    makeStep(BRepGraph_FaceId::Start(), BRepGraph_FaceRefId::Start(), 1u);

  BRepGraph_UsagePath anOriginal;
  anOriginal.Append(aRoot);

  BRepGraph_UsagePath anAppended = anOriginal;
  anAppended.Append(aChild);
  ASSERT_EQ(anOriginal.Size(), 1u);
  ASSERT_EQ(anAppended.Size(), 2u);
  EXPECT_EQ(anOriginal.Last(), aRoot);
  EXPECT_EQ(anAppended.Last(), aChild);

  BRepGraph_UsagePath anInserted = anOriginal;
  anInserted.InsertBefore(0, aChild);
  ASSERT_EQ(anOriginal.Size(), 1u);
  ASSERT_EQ(anInserted.Size(), 2u);
  EXPECT_EQ(anOriginal.First(), aRoot);
  EXPECT_EQ(anInserted.First(), aChild);
  BRepGraph_UsagePath anExpectedInserted;
  anExpectedInserted.Append(aChild);
  anExpectedInserted.Append(aRoot);
  EXPECT_EQ(anInserted.HashCode(), anExpectedInserted.HashCode());

  BRepGraph_UsagePath aCleared = anOriginal;
  aCleared.Clear();
  EXPECT_TRUE(aCleared.IsEmpty());
  EXPECT_EQ(anOriginal.Size(), 1u);
  EXPECT_EQ(aCleared.HashCode(), BRepGraph_UsagePath().HashCode());
}

TEST(BRepGraph_UsagePathTest, HashMatchesForEqualPathsAndEmptyPathIsHashable)
{
  const std::hash<BRepGraph_UsagePath> aHasher;

  BRepGraph_UsagePath anEmpty;
  EXPECT_EQ(aHasher(anEmpty), anEmpty.HashCode());

  BRepGraph_UsagePath aLeft;
  aLeft.Append(makeStep(BRepGraph_EdgeId::Start(), BRepGraph_RefId(), 0u));
  aLeft.Append(makeStep(BRepGraph_VertexId::Start(), BRepGraph_VertexRefId::Start(), 1u));

  BRepGraph_UsagePath aSame;
  aSame.Append(makeStep(BRepGraph_EdgeId::Start(), BRepGraph_RefId(), 0u));
  aSame.Append(makeStep(BRepGraph_VertexId::Start(), BRepGraph_VertexRefId::Start(), 1u));

  EXPECT_EQ(aHasher(aLeft), aHasher(aSame));
  EXPECT_EQ(aLeft.HashCode(), aSame.HashCode());
}

TEST(BRepGraph_UsagePathTest, HashUsesFirstMiddleAndLastSteps)
{
  BRepGraph_UsagePath aFirst;
  BRepGraph_UsagePath aSecond;
  for (uint32_t anIndex = 0; anIndex < 5; ++anIndex)
  {
    aFirst.Append(makeStep(BRepGraph_FaceId(anIndex), BRepGraph_FaceRefId(anIndex), anIndex));
    aSecond.Append(makeStep(BRepGraph_FaceId(anIndex),
                            BRepGraph_FaceRefId(anIndex),
                            anIndex == 2 ? 42u : anIndex));
  }

  EXPECT_NE(aFirst, aSecond);
  EXPECT_NE(aFirst.HashCode(), aSecond.HashCode());
}
