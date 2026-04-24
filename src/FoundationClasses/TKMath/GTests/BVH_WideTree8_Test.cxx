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
#include <BVH_WideTree.hxx>

#include <set>

namespace
{

//! Walks every leaf of a BVH8 (W=8 wide BVH) and collects the primitive index range.
void CollectLeafPrims(const opencascade::handle<BVH_Tree<double, 3, BVH_WideTree<8>>>& theTree,
                      std::set<int>&                                                   thePrims)
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
    BVH_Box<double, 3> aBox(BVH_Vec3d(i * 2.0, 0.0, 0.0), BVH_Vec3d(i * 2.0 + 1.0, 1.0, 1.0));
    aBoxSet->Add(i, aBox);
  }
  aBoxSet->Build();
  return aBoxSet;
}

} // namespace

TEST(BVH_WideTree8Test, EmptyTreeProducesEmptyWideTree)
{
  opencascade::handle<BVH_LinearBuilder<double, 3>> aBuilder = new BVH_LinearBuilder<double, 3>();
  BVH_BoxSet<double, 3>                             aBoxSet(aBuilder);
  aBoxSet.Build();

  const auto&                                               aBVH = aBoxSet.BVH();
  opencascade::handle<BVH_Tree<double, 3, BVH_WideTree<8>>> aWBVH(aBVH->CollapseToWide<8>());

  EXPECT_EQ(aWBVH->Length(), 0);
}

TEST(BVH_WideTree8Test, SingleElementBecomesSingleLeaf)
{
  opencascade::handle<BVH_LinearBuilder<double, 3>> aBuilder = new BVH_LinearBuilder<double, 3>();
  BVH_BoxSet<double, 3>                             aBoxSet(aBuilder);
  aBoxSet.Add(0, BVH_Box<double, 3>(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0)));
  aBoxSet.Build();

  const auto&                                               aBVH = aBoxSet.BVH();
  opencascade::handle<BVH_Tree<double, 3, BVH_WideTree<8>>> aWBVH(aBVH->CollapseToWide<8>());

  EXPECT_EQ(aWBVH->Length(), 1);
  EXPECT_TRUE(aWBVH->IsOuter(0));
  EXPECT_EQ(aWBVH->BegPrimitive(0), 0);
  EXPECT_EQ(aWBVH->EndPrimitive(0), 0);
}

TEST(BVH_WideTree8Test, AllPrimitivesPreservedAfterCollapse)
{
  // 64 primitives -> deep enough binary tree that 3-level collapse yields
  // multiple BVH8 inner nodes.
  constexpr int kNumPrims = 64;
  auto          aBoxSet   = MakeAxisAlignedBoxSet(kNumPrims);

  const auto&                                               aBVH = aBoxSet->BVH();
  opencascade::handle<BVH_Tree<double, 3, BVH_WideTree<8>>> aWBVH(aBVH->CollapseToWide<8>());

  std::set<int> aBinaryPrims, aWidePrims;
  CollectLeafPrimsBinary(aBVH, aBinaryPrims);
  CollectLeafPrims(aWBVH, aWidePrims);

  EXPECT_EQ(aBinaryPrims.size(), static_cast<size_t>(kNumPrims));
  EXPECT_EQ(aWidePrims, aBinaryPrims);
}

TEST(BVH_WideTree8Test, NodeCountReduced)
{
  constexpr int kNumPrims = 64;
  auto          aBoxSet   = MakeAxisAlignedBoxSet(kNumPrims);

  const auto&                                               aBVH = aBoxSet->BVH();
  opencascade::handle<BVH_Tree<double, 3, BVH_WideTree<8>>> aWBVH(aBVH->CollapseToWide<8>());

  // Collapsing 3 binary levels into a single BVH8 inner node should leave
  // strictly fewer total nodes (in the limit, ~1/3 of the binary count).
  EXPECT_LT(aWBVH->Length(), aBVH->Length());
}

TEST(BVH_WideTree8Test, InnerNodeChildCountInRange1To8)
{
  constexpr int kNumPrims = 64;
  auto          aBoxSet   = MakeAxisAlignedBoxSet(kNumPrims);

  const auto&                                               aBVH = aBoxSet->BVH();
  opencascade::handle<BVH_Tree<double, 3, BVH_WideTree<8>>> aWBVH(aBVH->CollapseToWide<8>());

  // Walk every node and assert inner ones report 1..8 children. At least
  // one node should saturate at 8 for a balanced 64-leaf tree.
  bool anySawEight = false;
  for (int aIdx = 0; aIdx < aWBVH->Length(); ++aIdx)
  {
    if (aWBVH->IsOuter(aIdx))
    {
      continue;
    }
    const int aNumChildren = aWBVH->NodeInfoBuffer()[aIdx].z() + 1;
    EXPECT_GE(aNumChildren, 1) << "node " << aIdx;
    EXPECT_LE(aNumChildren, 8) << "node " << aIdx;
    if (aNumChildren == 8)
    {
      anySawEight = true;
    }
  }
  EXPECT_TRUE(anySawEight) << "expected at least one fully populated BVH8 node";
}
