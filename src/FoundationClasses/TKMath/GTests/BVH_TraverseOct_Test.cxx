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

#include <BVH_OctTree.hxx>
#include <BVH_Ray.hxx>
#include <BVH_TraverseOct.hxx>
#include <BVH_Tree.hxx>

#include <set>

namespace
{

//! Builds a flat BVH8 with one inner root and eight leaf children.
opencascade::handle<BVH_Tree<float, 3, BVH_OctTree>> MakeFlatOBVH(const BVH_Vec3f mins[8],
                                                                  const BVH_Vec3f maxs[8])
{
  auto aTree = new BVH_Tree<float, 3, BVH_OctTree>;

  BVH_Vec3f aRootMin = mins[0];
  BVH_Vec3f aRootMax = maxs[0];
  for (int i = 1; i < 8; ++i)
  {
    aRootMin.x() = std::min(aRootMin.x(), mins[i].x());
    aRootMin.y() = std::min(aRootMin.y(), mins[i].y());
    aRootMin.z() = std::min(aRootMin.z(), mins[i].z());
    aRootMax.x() = std::max(aRootMax.x(), maxs[i].x());
    aRootMax.y() = std::max(aRootMax.y(), maxs[i].y());
    aRootMax.z() = std::max(aRootMax.z(), maxs[i].z());
  }

  // Node 0: inner root, base_child = 1, num_children-1 = 7.
  aTree->MinPointBuffer().push_back(aRootMin);
  aTree->MaxPointBuffer().push_back(aRootMax);
  aTree->NodeInfoBuffer().push_back(BVH_Vec4i(0, 1, 7, 0));

  // Nodes 1..8: leaves, each holds one primitive.
  for (int i = 0; i < 8; ++i)
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
  void           operator()(int thePrim, float /*tEnter*/) const { hits->insert(thePrim); }
};

} // namespace

TEST(BVH_TraverseOctTest, EmptyTree_NoHits)
{
  opencascade::handle<BVH_Tree<float, 3, BVH_OctTree>> aTree;
  BVH_Ray<float, 3>                                    aRay(BVH_Vec3f(0, 0, 0), BVH_Vec3f(1, 0, 0));
  std::set<int>                                        aHits;
  CollectingAcceptor                                   aAcc{&aHits};
  BVH::SIMD::TraverseOct(aTree, aRay, aAcc);
  EXPECT_TRUE(aHits.empty());
}

TEST(BVH_TraverseOctTest, FlatRoot_AllEightHit)
{
  // 8 unit boxes along +X. Ray on Y=0.5, Z=0.5 hits every one.
  BVH_Vec3f aMins[8], aMaxs[8];
  for (int i = 0; i < 8; ++i)
  {
    aMins[i] = BVH_Vec3f(i * 2.0f, 0.0f, 0.0f);
    aMaxs[i] = BVH_Vec3f(i * 2.0f + 1.0f, 1.0f, 1.0f);
  }
  auto aTree = MakeFlatOBVH(aMins, aMaxs);

  BVH_Ray<float, 3>  aRay(BVH_Vec3f(-1, 0.5f, 0.5f), BVH_Vec3f(1, 0, 0));
  std::set<int>      aHits;
  CollectingAcceptor aAcc{&aHits};
  BVH::SIMD::TraverseOct(aTree, aRay, aAcc);
  EXPECT_EQ(aHits, std::set<int>({0, 1, 2, 3, 4, 5, 6, 7}));
}

TEST(BVH_TraverseOctTest, FlatRoot_AllMiss)
{
  BVH_Vec3f aMins[8], aMaxs[8];
  for (int i = 0; i < 8; ++i)
  {
    aMins[i] = BVH_Vec3f(i * 2.0f, 0.0f, 0.0f);
    aMaxs[i] = BVH_Vec3f(i * 2.0f + 1.0f, 1.0f, 1.0f);
  }
  auto aTree = MakeFlatOBVH(aMins, aMaxs);

  BVH_Ray<float, 3>  aRay(BVH_Vec3f(-1, 5.0f, 0.5f), BVH_Vec3f(1, 0, 0));
  std::set<int>      aHits;
  CollectingAcceptor aAcc{&aHits};
  BVH::SIMD::TraverseOct(aTree, aRay, aAcc);
  EXPECT_TRUE(aHits.empty());
}

TEST(BVH_TraverseOctTest, FlatRoot_8BitMosaic)
{
  // Even-index lanes hit, odd lanes are offset out of plane.
  BVH_Vec3f aMins[8], aMaxs[8];
  for (int i = 0; i < 8; ++i)
  {
    const float dy = (i & 1) ? 5.0f : 0.0f;
    aMins[i] = BVH_Vec3f(i * 2.0f, dy, 0.0f);
    aMaxs[i] = BVH_Vec3f(i * 2.0f + 1.0f, dy + 1.0f, 1.0f);
  }
  auto aTree = MakeFlatOBVH(aMins, aMaxs);

  BVH_Ray<float, 3>  aRay(BVH_Vec3f(-1, 0.5f, 0.5f), BVH_Vec3f(1, 0, 0));
  std::set<int>      aHits;
  CollectingAcceptor aAcc{&aHits};
  BVH::SIMD::TraverseOct(aTree, aRay, aAcc);
  EXPECT_EQ(aHits, std::set<int>({0, 2, 4, 6}));
}

TEST(BVH_TraverseOctTest, FlatRoot_RayBehindAllBoxes)
{
  BVH_Vec3f aMins[8], aMaxs[8];
  for (int i = 0; i < 8; ++i)
  {
    aMins[i] = BVH_Vec3f(i * 2.0f, 0.0f, 0.0f);
    aMaxs[i] = BVH_Vec3f(i * 2.0f + 1.0f, 1.0f, 1.0f);
  }
  auto aTree = MakeFlatOBVH(aMins, aMaxs);

  // Origin past every box, ray towards +X => tLeave < 0 for every lane.
  BVH_Ray<float, 3>  aRay(BVH_Vec3f(20, 0.5f, 0.5f), BVH_Vec3f(1, 0, 0));
  std::set<int>      aHits;
  CollectingAcceptor aAcc{&aHits};
  BVH::SIMD::TraverseOct(aTree, aRay, aAcc);
  EXPECT_TRUE(aHits.empty());
}
