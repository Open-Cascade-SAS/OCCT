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

#include <BVH_QuadTree.hxx>
#include <BVH_Ray.hxx>
#include <BVH_TraverseQuad.hxx>
#include <BVH_Tree.hxx>

#include <set>

namespace
{

//! Builds a flat QBVH with one inner root and four leaf children, each
//! holding a single primitive. Caller passes 4 AABBs (one per leaf).
opencascade::handle<BVH_Tree<float, 3, BVH_QuadTree>> MakeFlatQBVH(const BVH_Vec3f& theMin0,
                                                                   const BVH_Vec3f& theMax0,
                                                                   const BVH_Vec3f& theMin1,
                                                                   const BVH_Vec3f& theMax1,
                                                                   const BVH_Vec3f& theMin2,
                                                                   const BVH_Vec3f& theMax2,
                                                                   const BVH_Vec3f& theMin3,
                                                                   const BVH_Vec3f& theMax3)
{
  auto aTree = new BVH_Tree<float, 3, BVH_QuadTree>;

  // Root bbox = union of children.
  BVH_Vec3f aRootMin(std::min({theMin0.x(), theMin1.x(), theMin2.x(), theMin3.x()}),
                     std::min({theMin0.y(), theMin1.y(), theMin2.y(), theMin3.y()}),
                     std::min({theMin0.z(), theMin1.z(), theMin2.z(), theMin3.z()}));
  BVH_Vec3f aRootMax(std::max({theMax0.x(), theMax1.x(), theMax2.x(), theMax3.x()}),
                     std::max({theMax0.y(), theMax1.y(), theMax2.y(), theMax3.y()}),
                     std::max({theMax0.z(), theMax1.z(), theMax2.z(), theMax3.z()}));

  // Node 0: inner root. base_child = 1, num_children-1 = 3.
  aTree->MinPointBuffer().push_back(aRootMin);
  aTree->MaxPointBuffer().push_back(aRootMax);
  aTree->NodeInfoBuffer().push_back(BVH_Vec4i(0, 1, 3, 0));

  // Nodes 1..4: leaves, each holds one primitive.
  const BVH_Vec3f aMins[4] = {theMin0, theMin1, theMin2, theMin3};
  const BVH_Vec3f aMaxs[4] = {theMax0, theMax1, theMax2, theMax3};
  for (int i = 0; i < 4; ++i)
  {
    aTree->MinPointBuffer().push_back(aMins[i]);
    aTree->MaxPointBuffer().push_back(aMaxs[i]);
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

TEST(BVH_TraverseQuadTest, EmptyTree_NoHits)
{
  opencascade::handle<BVH_Tree<float, 3, BVH_QuadTree>> aTree;
  BVH_Ray<float, 3>  aRay(BVH_Vec3f(0, 0, 0), BVH_Vec3f(1, 0, 0));
  std::set<int>      aHits;
  CollectingAcceptor aAcc{&aHits};
  BVH::SIMD::TraverseQuad(aTree, aRay, aAcc);
  EXPECT_TRUE(aHits.empty());
}

TEST(BVH_TraverseQuadTest, FlatRoot_RayHitsAllFour)
{
  // 4 unit boxes along +X. Ray on Y=0.5, Z=0.5 hits all of them.
  auto               aTree = MakeFlatQBVH(BVH_Vec3f(0, 0, 0),
                            BVH_Vec3f(1, 1, 1),
                            BVH_Vec3f(2, 0, 0),
                            BVH_Vec3f(3, 1, 1),
                            BVH_Vec3f(4, 0, 0),
                            BVH_Vec3f(5, 1, 1),
                            BVH_Vec3f(6, 0, 0),
                            BVH_Vec3f(7, 1, 1));
  BVH_Ray<float, 3>  aRay(BVH_Vec3f(-1, 0.5f, 0.5f), BVH_Vec3f(1, 0, 0));
  std::set<int>      aHits;
  CollectingAcceptor aAcc{&aHits};
  BVH::SIMD::TraverseQuad(aTree, aRay, aAcc);
  EXPECT_EQ(aHits, std::set<int>({0, 1, 2, 3}));
}

TEST(BVH_TraverseQuadTest, FlatRoot_RayMissesAll)
{
  auto aTree = MakeFlatQBVH(BVH_Vec3f(0, 0, 0),
                            BVH_Vec3f(1, 1, 1),
                            BVH_Vec3f(2, 0, 0),
                            BVH_Vec3f(3, 1, 1),
                            BVH_Vec3f(4, 0, 0),
                            BVH_Vec3f(5, 1, 1),
                            BVH_Vec3f(6, 0, 0),
                            BVH_Vec3f(7, 1, 1));
  // Y too high to intersect any box.
  BVH_Ray<float, 3>  aRay(BVH_Vec3f(-1, 5.0f, 0.5f), BVH_Vec3f(1, 0, 0));
  std::set<int>      aHits;
  CollectingAcceptor aAcc{&aHits};
  BVH::SIMD::TraverseQuad(aTree, aRay, aAcc);
  EXPECT_TRUE(aHits.empty());
}

TEST(BVH_TraverseQuadTest, FlatRoot_RayHitsMosaic)
{
  // Lanes 0 and 2 along Y=0.5 are hit; lanes 1 and 3 are offset in Y.
  auto               aTree = MakeFlatQBVH(BVH_Vec3f(0, 0, 0),
                            BVH_Vec3f(1, 1, 1), // hit
                            BVH_Vec3f(2, 5, 0),
                            BVH_Vec3f(3, 6, 1), // miss
                            BVH_Vec3f(4, 0, 0),
                            BVH_Vec3f(5, 1, 1), // hit
                            BVH_Vec3f(6, 5, 0),
                            BVH_Vec3f(7, 6, 1)); // miss
  BVH_Ray<float, 3>  aRay(BVH_Vec3f(-1, 0.5f, 0.5f), BVH_Vec3f(1, 0, 0));
  std::set<int>      aHits;
  CollectingAcceptor aAcc{&aHits};
  BVH::SIMD::TraverseQuad(aTree, aRay, aAcc);
  EXPECT_EQ(aHits, std::set<int>({0, 2}));
}

TEST(BVH_TraverseQuadTest, FlatRoot_RayBehindAllBoxes)
{
  auto aTree = MakeFlatQBVH(BVH_Vec3f(0, 0, 0),
                            BVH_Vec3f(1, 1, 1),
                            BVH_Vec3f(2, 0, 0),
                            BVH_Vec3f(3, 1, 1),
                            BVH_Vec3f(4, 0, 0),
                            BVH_Vec3f(5, 1, 1),
                            BVH_Vec3f(6, 0, 0),
                            BVH_Vec3f(7, 1, 1));
  // Ray origin past every box, direction +X => tLeave < 0 for every lane.
  BVH_Ray<float, 3>  aRay(BVH_Vec3f(10, 0.5f, 0.5f), BVH_Vec3f(1, 0, 0));
  std::set<int>      aHits;
  CollectingAcceptor aAcc{&aHits};
  BVH::SIMD::TraverseQuad(aTree, aRay, aAcc);
  EXPECT_TRUE(aHits.empty());
}
