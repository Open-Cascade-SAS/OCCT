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

#include <gtest/gtest.h>

#include <BVH_Ray.hxx>
#include <BVH_TraverseWide.hxx>
#include <BVH_Tree.hxx>
#include <BVH_WideTree.hxx>

#include <set>

namespace
{

//! Builds a flat BVH16 with one inner root and sixteen leaf children.
opencascade::handle<BVH_Tree<float, 3, BVH_WideTree<16>>> MakeFlatWBVH16(const BVH_Vec3f mins[16],
                                                                         const BVH_Vec3f maxs[16])
{
  auto aTree = new BVH_Tree<float, 3, BVH_WideTree<16>>;

  BVH_Vec3f aRootMin = mins[0];
  BVH_Vec3f aRootMax = maxs[0];
  for (int i = 1; i < 16; ++i)
  {
    aRootMin.x() = std::min(aRootMin.x(), mins[i].x());
    aRootMin.y() = std::min(aRootMin.y(), mins[i].y());
    aRootMin.z() = std::min(aRootMin.z(), mins[i].z());
    aRootMax.x() = std::max(aRootMax.x(), maxs[i].x());
    aRootMax.y() = std::max(aRootMax.y(), maxs[i].y());
    aRootMax.z() = std::max(aRootMax.z(), maxs[i].z());
  }

  // Node 0: inner root, base_child = 1, num_children-1 = 15.
  aTree->MinPointBuffer().push_back(aRootMin);
  aTree->MaxPointBuffer().push_back(aRootMax);
  aTree->NodeInfoBuffer().push_back(BVH_Vec4i(0, 1, 15, 0));

  // Nodes 1..16: leaves, each holds one primitive.
  for (int i = 0; i < 16; ++i)
  {
    aTree->MinPointBuffer().push_back(mins[i]);
    aTree->MaxPointBuffer().push_back(maxs[i]);
    aTree->NodeInfoBuffer().push_back(BVH_Vec4i(1, i, i, 1));
  }
  return aTree;
}

struct CollectingAcceptor
{
  std::set<int>* hits;

  void operator()(int thePrim, float /*tEnter*/) const { hits->insert(thePrim); }
};

} // namespace

TEST(BVH_TraverseWide16Test, EmptyTree_NoHits)
{
  opencascade::handle<BVH_Tree<float, 3, BVH_WideTree<16>>> aTree;
  BVH_Ray<float, 3>  aRay(BVH_Vec3f(0, 0, 0), BVH_Vec3f(1, 0, 0));
  std::set<int>      aHits;
  CollectingAcceptor aAcc{&aHits};
  BVH::SIMD::TraverseWide<16>(aTree, aRay, aAcc);
  EXPECT_TRUE(aHits.empty());
}

TEST(BVH_TraverseWide16Test, FlatRoot_AllSixteenHit)
{
  // 16 unit boxes along +X. Ray on Y=0.5, Z=0.5 hits every one.
  BVH_Vec3f aMins[16], aMaxs[16];
  for (int i = 0; i < 16; ++i)
  {
    aMins[i] = BVH_Vec3f(i * 2.0f, 0.0f, 0.0f);
    aMaxs[i] = BVH_Vec3f(i * 2.0f + 1.0f, 1.0f, 1.0f);
  }
  auto aTree = MakeFlatWBVH16(aMins, aMaxs);

  BVH_Ray<float, 3>  aRay(BVH_Vec3f(-1, 0.5f, 0.5f), BVH_Vec3f(1, 0, 0));
  std::set<int>      aHits;
  CollectingAcceptor aAcc{&aHits};
  BVH::SIMD::TraverseWide<16>(aTree, aRay, aAcc);
  std::set<int> aExpected;
  for (int i = 0; i < 16; ++i)
    aExpected.insert(i);
  EXPECT_EQ(aHits, aExpected);
}

TEST(BVH_TraverseWide16Test, FlatRoot_AllMiss)
{
  BVH_Vec3f aMins[16], aMaxs[16];
  for (int i = 0; i < 16; ++i)
  {
    aMins[i] = BVH_Vec3f(i * 2.0f, 0.0f, 0.0f);
    aMaxs[i] = BVH_Vec3f(i * 2.0f + 1.0f, 1.0f, 1.0f);
  }
  auto aTree = MakeFlatWBVH16(aMins, aMaxs);

  BVH_Ray<float, 3>  aRay(BVH_Vec3f(-1, 5.0f, 0.5f), BVH_Vec3f(1, 0, 0));
  std::set<int>      aHits;
  CollectingAcceptor aAcc{&aHits};
  BVH::SIMD::TraverseWide<16>(aTree, aRay, aAcc);
  EXPECT_TRUE(aHits.empty());
}

TEST(BVH_TraverseWide16Test, FlatRoot_16BitMosaic)
{
  // Even-index lanes hit, odd lanes are offset out of plane.
  BVH_Vec3f aMins[16], aMaxs[16];
  for (int i = 0; i < 16; ++i)
  {
    const float dy = (i & 1) ? 5.0f : 0.0f;
    aMins[i]       = BVH_Vec3f(i * 2.0f, dy, 0.0f);
    aMaxs[i]       = BVH_Vec3f(i * 2.0f + 1.0f, dy + 1.0f, 1.0f);
  }
  auto aTree = MakeFlatWBVH16(aMins, aMaxs);

  BVH_Ray<float, 3>  aRay(BVH_Vec3f(-1, 0.5f, 0.5f), BVH_Vec3f(1, 0, 0));
  std::set<int>      aHits;
  CollectingAcceptor aAcc{&aHits};
  BVH::SIMD::TraverseWide<16>(aTree, aRay, aAcc);
  EXPECT_EQ(aHits, std::set<int>({0, 2, 4, 6, 8, 10, 12, 14}));
}

TEST(BVH_TraverseWide16Test, FlatRoot_RayBehindAllBoxes)
{
  BVH_Vec3f aMins[16], aMaxs[16];
  for (int i = 0; i < 16; ++i)
  {
    aMins[i] = BVH_Vec3f(i * 2.0f, 0.0f, 0.0f);
    aMaxs[i] = BVH_Vec3f(i * 2.0f + 1.0f, 1.0f, 1.0f);
  }
  auto aTree = MakeFlatWBVH16(aMins, aMaxs);

  // Origin past every box, ray towards +X => tLeave < 0 for every lane.
  BVH_Ray<float, 3>  aRay(BVH_Vec3f(40, 0.5f, 0.5f), BVH_Vec3f(1, 0, 0));
  std::set<int>      aHits;
  CollectingAcceptor aAcc{&aHits};
  BVH::SIMD::TraverseWide<16>(aTree, aRay, aAcc);
  EXPECT_TRUE(aHits.empty());
}
