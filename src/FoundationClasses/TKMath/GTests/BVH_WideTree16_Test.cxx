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

//! Walks every leaf of a BVH16 (W=16 wide BVH) and collects the primitive index range.
void CollectLeafPrims16(const opencascade::handle<BVH_Tree<double, 3, BVH_WideTree<16>>>& theTree,
                        std::set<int>&                                                    thePrims)
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

TEST(BVH_WideTree16Test, EmptyTreeProducesEmptyWideTree)
{
  opencascade::handle<BVH_LinearBuilder<double, 3>> aBuilder = new BVH_LinearBuilder<double, 3>();
  BVH_BoxSet<double, 3>                             aBoxSet(aBuilder);
  aBoxSet.Build();

  const auto&                                                aBVH = aBoxSet.BVH();
  opencascade::handle<BVH_Tree<double, 3, BVH_WideTree<16>>> aWBVH(aBVH->CollapseToWide<16>());

  EXPECT_EQ(aWBVH->Length(), 0);
}

TEST(BVH_WideTree16Test, SingleElementBecomesSingleLeaf)
{
  opencascade::handle<BVH_LinearBuilder<double, 3>> aBuilder = new BVH_LinearBuilder<double, 3>();
  BVH_BoxSet<double, 3>                             aBoxSet(aBuilder);
  aBoxSet.Add(0, BVH_Box<double, 3>(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0)));
  aBoxSet.Build();

  const auto&                                                aBVH = aBoxSet.BVH();
  opencascade::handle<BVH_Tree<double, 3, BVH_WideTree<16>>> aWBVH(aBVH->CollapseToWide<16>());

  EXPECT_EQ(aWBVH->Length(), 1);
  EXPECT_TRUE(aWBVH->IsOuter(0));
  EXPECT_EQ(aWBVH->BegPrimitive(0), 0);
  EXPECT_EQ(aWBVH->EndPrimitive(0), 0);
}

TEST(BVH_WideTree16Test, AllPrimitivesPreservedAfterCollapse)
{
  // 256 primitives -> deep enough binary tree that 4-level collapse
  // (2^4 = 16) yields multiple BVH16 inner nodes.
  constexpr int kNumPrims = 256;
  auto          aBoxSet   = MakeAxisAlignedBoxSet(kNumPrims);

  const auto&                                                aBVH = aBoxSet->BVH();
  opencascade::handle<BVH_Tree<double, 3, BVH_WideTree<16>>> aWBVH(aBVH->CollapseToWide<16>());

  std::set<int> aBinaryPrims, aWidePrims;
  CollectLeafPrimsBinary(aBVH, aBinaryPrims);
  CollectLeafPrims16(aWBVH, aWidePrims);

  EXPECT_EQ(aBinaryPrims.size(), static_cast<size_t>(kNumPrims));
  EXPECT_EQ(aWidePrims, aBinaryPrims);
}

TEST(BVH_WideTree16Test, NodeCountReduced)
{
  constexpr int kNumPrims = 256;
  auto          aBoxSet   = MakeAxisAlignedBoxSet(kNumPrims);

  const auto&                                                aBVH = aBoxSet->BVH();
  opencascade::handle<BVH_Tree<double, 3, BVH_WideTree<16>>> aWBVH(aBVH->CollapseToWide<16>());

  // Collapsing 4 binary levels into a single BVH16 inner node should leave
  // strictly fewer total nodes (in the limit, ~1/4 of the binary count).
  EXPECT_LT(aWBVH->Length(), aBVH->Length());
}

TEST(BVH_WideTree16Test, InnerNodeChildCountInRange1To16)
{
  constexpr int kNumPrims = 256;
  auto          aBoxSet   = MakeAxisAlignedBoxSet(kNumPrims);

  const auto&                                                aBVH = aBoxSet->BVH();
  opencascade::handle<BVH_Tree<double, 3, BVH_WideTree<16>>> aWBVH(aBVH->CollapseToWide<16>());

  bool anySawSixteen = false;
  for (int aIdx = 0; aIdx < aWBVH->Length(); ++aIdx)
  {
    if (aWBVH->IsOuter(aIdx))
    {
      continue;
    }
    const int aNumChildren = aWBVH->NodeInfoBuffer()[aIdx].z() + 1;
    EXPECT_GE(aNumChildren, 1) << "node " << aIdx;
    EXPECT_LE(aNumChildren, 16) << "node " << aIdx;
    if (aNumChildren == 16)
    {
      anySawSixteen = true;
    }
  }
  EXPECT_TRUE(anySawSixteen) << "expected at least one fully populated BVH16 node";
}
