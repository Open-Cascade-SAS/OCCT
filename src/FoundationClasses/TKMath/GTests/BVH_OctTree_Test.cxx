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

#include <BVH_BinaryTree.hxx>
#include <BVH_BoxSet.hxx>
#include <BVH_LinearBuilder.hxx>
#include <BVH_OctTree.hxx>

#include <set>

namespace
{

//! Walks every leaf of an OBVH and collects the primitive index range.
void CollectLeafPrims(const opencascade::handle<BVH_Tree<double, 3, BVH_OctTree>>& theTree,
                      std::set<int>& thePrims)
{
  if (theTree.IsNull() || theTree->Length() == 0)
  {
    return;
  }
  std::vector<int> aStack;
  aStack.push_back(0);
  while (!aStack.empty())
  {
    const int aNode = aStack.back();
    aStack.pop_back();
    if (theTree->IsOuter(aNode))
    {
      for (int aPrim = theTree->BegPrimitive(aNode); aPrim <= theTree->EndPrimitive(aNode); ++aPrim)
      {
        thePrims.insert(aPrim);
      }
      continue;
    }
    const auto& aInfo        = theTree->NodeInfoBuffer()[aNode];
    const int   aBaseIdx     = aInfo.y();
    const int   aNumChildren = aInfo.z() + 1;
    for (int aK = 0; aK < aNumChildren; ++aK)
    {
      aStack.push_back(aBaseIdx + aK);
    }
  }
}

//! Walks every leaf of a binary tree and collects the primitive index range.
void CollectLeafPrimsBinary(const opencascade::handle<BVH_Tree<double, 3, BVH_BinaryTree>>& theTree,
                            std::set<int>& thePrims)
{
  if (theTree.IsNull() || theTree->Length() == 0)
  {
    return;
  }
  std::vector<int> aStack;
  aStack.push_back(0);
  while (!aStack.empty())
  {
    const int aNode = aStack.back();
    aStack.pop_back();
    if (theTree->IsOuter(aNode))
    {
      for (int aPrim = theTree->BegPrimitive(aNode); aPrim <= theTree->EndPrimitive(aNode); ++aPrim)
      {
        thePrims.insert(aPrim);
      }
      continue;
    }
    aStack.push_back(theTree->Child<0>(aNode));
    aStack.push_back(theTree->Child<1>(aNode));
  }
}

//! Builds a BVH from N boxes laid out along the X axis.
opencascade::handle<BVH_BoxSet<double, 3>> MakeAxisAlignedBoxSet(int theNum)
{
  opencascade::handle<BVH_LinearBuilder<double, 3>> aBuilder =
    new BVH_LinearBuilder<double, 3>(1 /* leaf size */, 32 /* max depth */);
  opencascade::handle<BVH_BoxSet<double, 3>> aBoxSet = new BVH_BoxSet<double, 3>(aBuilder);
  for (int i = 0; i < theNum; ++i)
  {
    BVH_Box<double, 3> aBox(BVH_Vec3d(i * 2.0, 0.0, 0.0),
                           BVH_Vec3d(i * 2.0 + 1.0, 1.0, 1.0));
    aBoxSet->Add(i, aBox);
  }
  aBoxSet->Build();
  return aBoxSet;
}

} // namespace

TEST(BVH_OctTreeTest, EmptyTreeProducesEmptyOctTree)
{
  opencascade::handle<BVH_LinearBuilder<double, 3>> aBuilder = new BVH_LinearBuilder<double, 3>();
  BVH_BoxSet<double, 3>                             aBoxSet(aBuilder);
  aBoxSet.Build();

  const auto& aBVH = aBoxSet.BVH();
  opencascade::handle<BVH_Tree<double, 3, BVH_OctTree>> aOBVH(aBVH->CollapseToOctTree());

  EXPECT_EQ(aOBVH->Length(), 0);
}

TEST(BVH_OctTreeTest, SingleElementBecomesSingleLeaf)
{
  opencascade::handle<BVH_LinearBuilder<double, 3>> aBuilder = new BVH_LinearBuilder<double, 3>();
  BVH_BoxSet<double, 3>                             aBoxSet(aBuilder);
  aBoxSet.Add(0, BVH_Box<double, 3>(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0)));
  aBoxSet.Build();

  const auto& aBVH = aBoxSet.BVH();
  opencascade::handle<BVH_Tree<double, 3, BVH_OctTree>> aOBVH(aBVH->CollapseToOctTree());

  EXPECT_EQ(aOBVH->Length(), 1);
  EXPECT_TRUE(aOBVH->IsOuter(0));
  EXPECT_EQ(aOBVH->BegPrimitive(0), 0);
  EXPECT_EQ(aOBVH->EndPrimitive(0), 0);
}

TEST(BVH_OctTreeTest, AllPrimitivesPreservedAfterCollapse)
{
  // 64 primitives -> deep enough binary tree that 3-level collapse yields
  // multiple BVH8 inner nodes.
  constexpr int kNumPrims = 64;
  auto          aBoxSet   = MakeAxisAlignedBoxSet(kNumPrims);

  const auto& aBVH = aBoxSet->BVH();
  opencascade::handle<BVH_Tree<double, 3, BVH_OctTree>> aOBVH(aBVH->CollapseToOctTree());

  std::set<int> aBinaryPrims, aOctPrims;
  CollectLeafPrimsBinary(aBVH, aBinaryPrims);
  CollectLeafPrims(aOBVH, aOctPrims);

  EXPECT_EQ(aBinaryPrims.size(), static_cast<size_t>(kNumPrims));
  EXPECT_EQ(aOctPrims, aBinaryPrims);
}

TEST(BVH_OctTreeTest, NodeCountReduced)
{
  constexpr int kNumPrims = 64;
  auto          aBoxSet   = MakeAxisAlignedBoxSet(kNumPrims);

  const auto& aBVH = aBoxSet->BVH();
  opencascade::handle<BVH_Tree<double, 3, BVH_OctTree>> aOBVH(aBVH->CollapseToOctTree());

  // Collapsing 3 binary levels into a single BVH8 inner node should leave
  // strictly fewer total nodes (in the limit, ~1/3 of the binary count).
  EXPECT_LT(aOBVH->Length(), aBVH->Length());
}

TEST(BVH_OctTreeTest, InnerNodeChildCountInRange1To8)
{
  constexpr int kNumPrims = 64;
  auto          aBoxSet   = MakeAxisAlignedBoxSet(kNumPrims);

  const auto& aBVH = aBoxSet->BVH();
  opencascade::handle<BVH_Tree<double, 3, BVH_OctTree>> aOBVH(aBVH->CollapseToOctTree());

  // Walk every node and assert inner ones report 1..8 children. At least
  // one node should saturate at 8 for a balanced 64-leaf tree.
  bool anySawEight = false;
  for (int aIdx = 0; aIdx < aOBVH->Length(); ++aIdx)
  {
    if (aOBVH->IsOuter(aIdx))
    {
      continue;
    }
    const int aNumChildren = aOBVH->NodeInfoBuffer()[aIdx].z() + 1;
    EXPECT_GE(aNumChildren, 1) << "node " << aIdx;
    EXPECT_LE(aNumChildren, 8) << "node " << aIdx;
    if (aNumChildren == 8)
    {
      anySawEight = true;
    }
  }
  EXPECT_TRUE(anySawEight) << "expected at least one fully populated BVH8 node";
}
