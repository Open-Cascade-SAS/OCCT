// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <BVH_BinnedBuilder.hxx>
#include <BVH_BoxSet.hxx>
#include <Precision.hxx>

TEST(BVH_BinnedBuilderTest, DefaultConstructor)
{
  BVH_BinnedBuilder<Standard_Real, 3> aBuilder;

  EXPECT_GT(aBuilder.LeafNodeSize(), 0);
  EXPECT_GT(aBuilder.MaxTreeDepth(), 0);
}

TEST(BVH_BinnedBuilderTest, CustomParameters)
{
  BVH_BinnedBuilder<Standard_Real, 3> aBuilder(5, 20);

  EXPECT_EQ(aBuilder.LeafNodeSize(), 5);
  EXPECT_EQ(aBuilder.MaxTreeDepth(), 20);
}

TEST(BVH_BinnedBuilderTest, BuildEmptySet)
{
  opencascade::handle<BVH_BinnedBuilder<Standard_Real, 3>> aBuilder =
    new BVH_BinnedBuilder<Standard_Real, 3>();
  BVH_BoxSet<Standard_Real, 3> aBoxSet(aBuilder);

  aBoxSet.Build();

  const opencascade::handle<BVH_Tree<Standard_Real, 3>>& aBVH = aBoxSet.BVH();
  EXPECT_EQ(aBVH->Length(), 0);
}

TEST(BVH_BinnedBuilderTest, BuildSingleElement)
{
  opencascade::handle<BVH_BinnedBuilder<Standard_Real, 3>> aBuilder =
    new BVH_BinnedBuilder<Standard_Real, 3>();
  BVH_BoxSet<Standard_Real, 3> aBoxSet(aBuilder);

  aBoxSet.Add(0, BVH_Box<Standard_Real, 3>(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0)));
  aBoxSet.Build();

  const opencascade::handle<BVH_Tree<Standard_Real, 3>>& aBVH = aBoxSet.BVH();
  EXPECT_EQ(aBVH->Length(), 1);
  EXPECT_TRUE(aBVH->IsOuter(0));
}

TEST(BVH_BinnedBuilderTest, BuildMultipleElements)
{
  opencascade::handle<BVH_BinnedBuilder<Standard_Real, 3>> aBuilder =
    new BVH_BinnedBuilder<Standard_Real, 3>(1, 32);
  BVH_BoxSet<Standard_Real, 3> aBoxSet(aBuilder);

  // Add boxes along X axis
  for (int i = 0; i < 10; ++i)
  {
    BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(i * 2.0, 0.0, 0.0),
                                   BVH_Vec3d(i * 2.0 + 1.0, 1.0, 1.0));
    aBoxSet.Add(i, aBox);
  }

  aBoxSet.Build();

  const opencascade::handle<BVH_Tree<Standard_Real, 3>>& aBVH = aBoxSet.BVH();
  EXPECT_GT(aBVH->Length(), 1);
  EXPECT_GT(aBVH->Depth(), 0);
}

TEST(BVH_BinnedBuilderTest, SAHOptimization)
{
  opencascade::handle<BVH_BinnedBuilder<Standard_Real, 3>> aBuilder =
    new BVH_BinnedBuilder<Standard_Real, 3>(1, 32);
  BVH_BoxSet<Standard_Real, 3> aBoxSet(aBuilder);

  // Add boxes in a pattern where SAH should matter
  // Two clusters: one near origin, one far away
  for (int i = 0; i < 5; ++i)
  {
    BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(i * 0.1, 0.0, 0.0),
                                   BVH_Vec3d(i * 0.1 + 0.1, 0.1, 0.1));
    aBoxSet.Add(i, aBox);
  }

  for (int i = 0; i < 5; ++i)
  {
    BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(100.0 + i * 0.1, 0.0, 0.0),
                                   BVH_Vec3d(100.0 + i * 0.1 + 0.1, 0.1, 0.1));
    aBoxSet.Add(i + 5, aBox);
  }

  aBoxSet.Build();

  const opencascade::handle<BVH_Tree<Standard_Real, 3>>& aBVH = aBoxSet.BVH();

  // SAH should produce a reasonable tree
  Standard_Real aSAH = aBVH->EstimateSAH();
  EXPECT_GT(aSAH, 0.0);
}

TEST(BVH_BinnedBuilderTest, LeafNodeSizeRespected)
{
  const int                                                aLeafSize = 3;
  opencascade::handle<BVH_BinnedBuilder<Standard_Real, 3>> aBuilder =
    new BVH_BinnedBuilder<Standard_Real, 3>(aLeafSize, 32);
  BVH_BoxSet<Standard_Real, 3> aBoxSet(aBuilder);

  for (int i = 0; i < 10; ++i)
  {
    BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(i * 2.0, 0.0, 0.0),
                                   BVH_Vec3d(i * 2.0 + 1.0, 1.0, 1.0));
    aBoxSet.Add(i, aBox);
  }

  aBoxSet.Build();

  const opencascade::handle<BVH_Tree<Standard_Real, 3>>& aBVH = aBoxSet.BVH();

  // Check that leaf nodes don't exceed leaf size
  for (int i = 0; i < aBVH->Length(); ++i)
  {
    if (aBVH->IsOuter(i))
    {
      int aNbPrims = aBVH->NbPrimitives(i);
      EXPECT_LE(aNbPrims, aLeafSize);
    }
  }
}

TEST(BVH_BinnedBuilderTest, BuildWithDifferentBinCounts)
{
  // Test with different number of bins (template parameter)
  opencascade::handle<BVH_BinnedBuilder<Standard_Real, 3, 16>> aBuilder16 =
    new BVH_BinnedBuilder<Standard_Real, 3, 16>(1, 32);
  BVH_BoxSet<Standard_Real, 3> aBoxSet(aBuilder16);

  for (int i = 0; i < 20; ++i)
  {
    BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(i * 1.0, 0.0, 0.0),
                                   BVH_Vec3d(i * 1.0 + 0.5, 0.5, 0.5));
    aBoxSet.Add(i, aBox);
  }

  aBoxSet.Build();

  const opencascade::handle<BVH_Tree<Standard_Real, 3>>& aBVH = aBoxSet.BVH();
  EXPECT_GT(aBVH->Length(), 1);
}

TEST(BVH_BinnedBuilderTest, Build2D)
{
  opencascade::handle<BVH_BinnedBuilder<Standard_Real, 2>> aBuilder =
    new BVH_BinnedBuilder<Standard_Real, 2>(1, 32);
  BVH_BoxSet<Standard_Real, 2> aBoxSet(aBuilder);

  for (int i = 0; i < 10; ++i)
  {
    BVH_Box<Standard_Real, 2> aBox(BVH_Vec2d(i * 2.0, 0.0), BVH_Vec2d(i * 2.0 + 1.0, 1.0));
    aBoxSet.Add(i, aBox);
  }

  aBoxSet.Build();

  const opencascade::handle<BVH_Tree<Standard_Real, 2>>& aBVH = aBoxSet.BVH();
  EXPECT_GT(aBVH->Length(), 1);
}

// Note: Float tests skipped due to BVH_BoxSet::Center return type issue

TEST(BVH_BinnedBuilderTest, RandomDistribution)
{
  opencascade::handle<BVH_BinnedBuilder<Standard_Real, 3>> aBuilder =
    new BVH_BinnedBuilder<Standard_Real, 3>(1, 32);
  BVH_BoxSet<Standard_Real, 3> aBoxSet(aBuilder);

  // Add boxes in a 3D grid pattern
  int aCount = 0;
  for (int x = 0; x < 5; ++x)
  {
    for (int y = 0; y < 5; ++y)
    {
      for (int z = 0; z < 5; ++z)
      {
        BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(x * 2.0, y * 2.0, z * 2.0),
                                       BVH_Vec3d(x * 2.0 + 1.0, y * 2.0 + 1.0, z * 2.0 + 1.0));
        aBoxSet.Add(aCount++, aBox);
      }
    }
  }

  aBoxSet.Build();

  const opencascade::handle<BVH_Tree<Standard_Real, 3>>& aBVH = aBoxSet.BVH();
  EXPECT_GT(aBVH->Length(), 1);

  // Verify tree covers all primitives
  int aTotalPrims = 0;
  for (int i = 0; i < aBVH->Length(); ++i)
  {
    if (aBVH->IsOuter(i))
    {
      aTotalPrims += aBVH->NbPrimitives(i);
    }
  }
  EXPECT_EQ(aTotalPrims, 125);
}

TEST(BVH_BinnedBuilderTest, CompareTreeQuality)
{
  // Build tree with small leaf size vs large leaf size
  opencascade::handle<BVH_BinnedBuilder<Standard_Real, 3>> aBuilder1 =
    new BVH_BinnedBuilder<Standard_Real, 3>(1, 32);
  opencascade::handle<BVH_BinnedBuilder<Standard_Real, 3>> aBuilder4 =
    new BVH_BinnedBuilder<Standard_Real, 3>(4, 32);

  BVH_BoxSet<Standard_Real, 3> aBoxSet1(aBuilder1);
  BVH_BoxSet<Standard_Real, 3> aBoxSet4(aBuilder4);

  for (int i = 0; i < 50; ++i)
  {
    BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(i * 2.0, 0.0, 0.0),
                                   BVH_Vec3d(i * 2.0 + 1.0, 1.0, 1.0));
    aBoxSet1.Add(i, aBox);
    aBoxSet4.Add(i, aBox);
  }

  aBoxSet1.Build();
  aBoxSet4.Build();

  const opencascade::handle<BVH_Tree<Standard_Real, 3>>& aBVH1 = aBoxSet1.BVH();
  const opencascade::handle<BVH_Tree<Standard_Real, 3>>& aBVH4 = aBoxSet4.BVH();

  // Tree with smaller leaf size should be deeper
  EXPECT_GE(aBVH1->Depth(), aBVH4->Depth());
}

TEST(BVH_BinnedBuilderTest, MaxDepthRespected)
{
  const int                                                aMaxDepth = 5;
  opencascade::handle<BVH_BinnedBuilder<Standard_Real, 3>> aBuilder =
    new BVH_BinnedBuilder<Standard_Real, 3>(1, aMaxDepth);
  BVH_BoxSet<Standard_Real, 3> aBoxSet(aBuilder);

  for (int i = 0; i < 100; ++i)
  {
    BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(i * 2.0, 0.0, 0.0),
                                   BVH_Vec3d(i * 2.0 + 1.0, 1.0, 1.0));
    aBoxSet.Add(i, aBox);
  }

  aBoxSet.Build();

  const opencascade::handle<BVH_Tree<Standard_Real, 3>>& aBVH = aBoxSet.BVH();
  EXPECT_LE(aBVH->Depth(), aMaxDepth);
}

TEST(BVH_BinnedBuilderTest, OverlappingBoxes)
{
  opencascade::handle<BVH_BinnedBuilder<Standard_Real, 3>> aBuilder =
    new BVH_BinnedBuilder<Standard_Real, 3>(1, 32);
  BVH_BoxSet<Standard_Real, 3> aBoxSet(aBuilder);

  // Add overlapping boxes
  for (int i = 0; i < 10; ++i)
  {
    BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(i * 0.5, 0.0, 0.0),
                                   BVH_Vec3d(i * 0.5 + 2.0, 1.0, 1.0));
    aBoxSet.Add(i, aBox);
  }

  aBoxSet.Build();

  const opencascade::handle<BVH_Tree<Standard_Real, 3>>& aBVH = aBoxSet.BVH();
  EXPECT_GT(aBVH->Length(), 1);
}

TEST(BVH_BinnedBuilderTest, IdenticalBoxes)
{
  opencascade::handle<BVH_BinnedBuilder<Standard_Real, 3>> aBuilder =
    new BVH_BinnedBuilder<Standard_Real, 3>(1, 32);
  BVH_BoxSet<Standard_Real, 3> aBoxSet(aBuilder);

  // Add identical boxes
  for (int i = 0; i < 10; ++i)
  {
    BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));
    aBoxSet.Add(i, aBox);
  }

  aBoxSet.Build();

  const opencascade::handle<BVH_Tree<Standard_Real, 3>>& aBVH = aBoxSet.BVH();
  EXPECT_GE(aBVH->Length(), 1);
}
