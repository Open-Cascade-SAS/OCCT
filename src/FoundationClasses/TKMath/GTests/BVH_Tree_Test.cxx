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

#include <BVH_BinaryTree.hxx>
#include <BVH_Box.hxx>
#include <Precision.hxx>

TEST(BVH_TreeTest, DefaultConstructor)
{
  BVH_Tree<Standard_Real, 3, BVH_BinaryTree> aTree;

  EXPECT_EQ(aTree.Length(), 0);
  EXPECT_EQ(aTree.Depth(), 0);
}

TEST(BVH_TreeTest, AddLeafNode)
{
  BVH_Tree<Standard_Real, 3, BVH_BinaryTree> aTree;

  BVH_Vec3d aMin(0.0, 0.0, 0.0);
  BVH_Vec3d aMax(1.0, 1.0, 1.0);

  int aNodeIndex = aTree.AddLeafNode(aMin, aMax, 0, 10);

  EXPECT_EQ(aNodeIndex, 0);
  EXPECT_EQ(aTree.Length(), 1);
  EXPECT_TRUE(aTree.IsOuter(0));
  EXPECT_EQ(aTree.BegPrimitive(0), 0);
  EXPECT_EQ(aTree.EndPrimitive(0), 10);
  EXPECT_EQ(aTree.NbPrimitives(0), 11);
}

TEST(BVH_TreeTest, AddInnerNode)
{
  BVH_Tree<Standard_Real, 3, BVH_BinaryTree> aTree;

  // Add two leaf nodes first
  BVH_Vec3d aMin1(0.0, 0.0, 0.0);
  BVH_Vec3d aMax1(1.0, 1.0, 1.0);
  int       aLeaf1 = aTree.AddLeafNode(aMin1, aMax1, 0, 5);

  BVH_Vec3d aMin2(2.0, 0.0, 0.0);
  BVH_Vec3d aMax2(3.0, 1.0, 1.0);
  int       aLeaf2 = aTree.AddLeafNode(aMin2, aMax2, 6, 10);

  // Add inner node
  BVH_Vec3d aMinRoot(0.0, 0.0, 0.0);
  BVH_Vec3d aMaxRoot(3.0, 1.0, 1.0);
  int       aRoot = aTree.AddInnerNode(aMinRoot, aMaxRoot, aLeaf1, aLeaf2);

  EXPECT_EQ(aTree.Length(), 3);
  EXPECT_FALSE(aTree.IsOuter(aRoot));
  EXPECT_EQ(aTree.template Child<0>(aRoot), aLeaf1);
  EXPECT_EQ(aTree.template Child<1>(aRoot), aLeaf2);
}

TEST(BVH_TreeTest, MinMaxPoints)
{
  BVH_Tree<Standard_Real, 3, BVH_BinaryTree> aTree;

  BVH_Vec3d aMin(1.0, 2.0, 3.0);
  BVH_Vec3d aMax(4.0, 5.0, 6.0);

  aTree.AddLeafNode(aMin, aMax, 0, 0);

  EXPECT_NEAR(aTree.MinPoint(0).x(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aTree.MinPoint(0).y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aTree.MinPoint(0).z(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aTree.MaxPoint(0).x(), 4.0, Precision::Confusion());
  EXPECT_NEAR(aTree.MaxPoint(0).y(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aTree.MaxPoint(0).z(), 6.0, Precision::Confusion());
}

TEST(BVH_TreeTest, Clear)
{
  BVH_Tree<Standard_Real, 3, BVH_BinaryTree> aTree;

  aTree.AddLeafNode(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0), 0, 0);
  aTree.AddLeafNode(BVH_Vec3d(1.0, 0.0, 0.0), BVH_Vec3d(2.0, 1.0, 1.0), 1, 1);

  EXPECT_EQ(aTree.Length(), 2);

  aTree.Clear();

  EXPECT_EQ(aTree.Length(), 0);
  EXPECT_EQ(aTree.Depth(), 0);
}

TEST(BVH_TreeTest, Reserve)
{
  BVH_Tree<Standard_Real, 3, BVH_BinaryTree> aTree;

  // Reserve should not throw
  aTree.Reserve(100);

  // Can still add nodes after reserve
  aTree.AddLeafNode(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0), 0, 0);
  EXPECT_EQ(aTree.Length(), 1);
}

TEST(BVH_TreeTest, SetOuterInner)
{
  BVH_Tree<Standard_Real, 3, BVH_BinaryTree> aTree;

  // Add leaf and change to inner
  aTree.AddLeafNode(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0), 0, 0);

  EXPECT_TRUE(aTree.IsOuter(0));

  aTree.SetInner(0);
  EXPECT_FALSE(aTree.IsOuter(0));

  aTree.SetOuter(0);
  EXPECT_TRUE(aTree.IsOuter(0));
}

TEST(BVH_TreeTest, Level)
{
  BVH_Tree<Standard_Real, 3, BVH_BinaryTree> aTree;

  aTree.AddLeafNode(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0), 0, 0);

  // Default level is 0
  EXPECT_EQ(aTree.Level(0), 0);

  // Change level
  aTree.Level(0) = 5;
  EXPECT_EQ(aTree.Level(0), 5);
}

TEST(BVH_TreeTest, AddLeafNodeWithBox)
{
  BVH_Tree<Standard_Real, 3, BVH_BinaryTree> aTree;

  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));

  int aNodeIndex = aTree.AddLeafNode(aBox, 0, 5);

  EXPECT_EQ(aNodeIndex, 0);
  EXPECT_TRUE(aTree.IsOuter(0));
  EXPECT_EQ(aTree.BegPrimitive(0), 0);
  EXPECT_EQ(aTree.EndPrimitive(0), 5);
}

TEST(BVH_TreeTest, AddInnerNodeWithBox)
{
  BVH_Tree<Standard_Real, 3, BVH_BinaryTree> aTree;

  BVH_Box<Standard_Real, 3> aBox1(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));
  BVH_Box<Standard_Real, 3> aBox2(BVH_Vec3d(2.0, 0.0, 0.0), BVH_Vec3d(3.0, 1.0, 1.0));
  BVH_Box<Standard_Real, 3> aRootBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(3.0, 1.0, 1.0));

  int aLeaf1 = aTree.AddLeafNode(aBox1, 0, 5);
  int aLeaf2 = aTree.AddLeafNode(aBox2, 6, 10);
  int aRoot  = aTree.AddInnerNode(aRootBox, aLeaf1, aLeaf2);

  EXPECT_FALSE(aTree.IsOuter(aRoot));
  EXPECT_EQ(aTree.template Child<0>(aRoot), aLeaf1);
  EXPECT_EQ(aTree.template Child<1>(aRoot), aLeaf2);
}

TEST(BVH_TreeTest, EstimateSAH)
{
  BVH_Tree<Standard_Real, 3, BVH_BinaryTree> aTree;

  // Create a simple tree with root and two leaves
  BVH_Vec3d aMin1(0.0, 0.0, 0.0);
  BVH_Vec3d aMax1(1.0, 1.0, 1.0);
  int       aLeaf1 = aTree.AddLeafNode(aMin1, aMax1, 0, 0);

  BVH_Vec3d aMin2(2.0, 0.0, 0.0);
  BVH_Vec3d aMax2(3.0, 1.0, 1.0);
  int       aLeaf2 = aTree.AddLeafNode(aMin2, aMax2, 1, 1);

  BVH_Vec3d aMinRoot(0.0, 0.0, 0.0);
  BVH_Vec3d aMaxRoot(3.0, 1.0, 1.0);
  aTree.AddInnerNode(aMinRoot, aMaxRoot, aLeaf1, aLeaf2);

  Standard_Real aSAH = aTree.EstimateSAH();

  // SAH should be positive
  EXPECT_GT(aSAH, 0.0);
}

TEST(BVH_TreeTest, NbPrimitives)
{
  BVH_Tree<Standard_Real, 3, BVH_BinaryTree> aTree;

  aTree.AddLeafNode(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0), 5, 15);

  // NbPrimitives = EndPrimitive - BegPrimitive + 1 = 15 - 5 + 1 = 11
  EXPECT_EQ(aTree.NbPrimitives(0), 11);
}

TEST(BVH_TreeTest, SinglePrimitive)
{
  BVH_Tree<Standard_Real, 3, BVH_BinaryTree> aTree;

  aTree.AddLeafNode(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0), 7, 7);

  EXPECT_EQ(aTree.NbPrimitives(0), 1);
  EXPECT_EQ(aTree.BegPrimitive(0), 7);
  EXPECT_EQ(aTree.EndPrimitive(0), 7);
}

TEST(BVH_TreeTest, Float3DTree)
{
  BVH_Tree<Standard_ShortReal, 3, BVH_BinaryTree> aTree;

  BVH_Vec3f aMin(0.0f, 0.0f, 0.0f);
  BVH_Vec3f aMax(1.0f, 1.0f, 1.0f);

  aTree.AddLeafNode(aMin, aMax, 0, 5);

  EXPECT_EQ(aTree.Length(), 1);
  EXPECT_NEAR(aTree.MinPoint(0).x(), 0.0f, 1e-5f);
  EXPECT_NEAR(aTree.MaxPoint(0).x(), 1.0f, 1e-5f);
}

TEST(BVH_TreeTest, Tree2D)
{
  BVH_Tree<Standard_Real, 2, BVH_BinaryTree> aTree;

  BVH_Vec2d aMin(0.0, 0.0);
  BVH_Vec2d aMax(1.0, 1.0);

  aTree.AddLeafNode(aMin, aMax, 0, 5);

  EXPECT_EQ(aTree.Length(), 1);
  EXPECT_NEAR(aTree.MinPoint(0).x(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aTree.MinPoint(0).y(), 0.0, Precision::Confusion());
}

TEST(BVH_TreeTest, Tree4D)
{
  BVH_Tree<Standard_Real, 4, BVH_BinaryTree> aTree;

  BVH_Vec4d aMin(0.0, 0.0, 0.0, 0.0);
  BVH_Vec4d aMax(1.0, 1.0, 1.0, 1.0);

  aTree.AddLeafNode(aMin, aMax, 0, 5);

  EXPECT_EQ(aTree.Length(), 1);
  EXPECT_NEAR(aTree.MinPoint(0).x(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aTree.MinPoint(0).w(), 0.0, Precision::Confusion());
}

TEST(BVH_TreeTest, MultipleLeaves)
{
  BVH_Tree<Standard_Real, 3, BVH_BinaryTree> aTree;

  for (int i = 0; i < 10; ++i)
  {
    BVH_Vec3d aMin(i * 1.0, 0.0, 0.0);
    BVH_Vec3d aMax(i * 1.0 + 1.0, 1.0, 1.0);
    aTree.AddLeafNode(aMin, aMax, i, i);
  }

  EXPECT_EQ(aTree.Length(), 10);

  for (int i = 0; i < 10; ++i)
  {
    EXPECT_TRUE(aTree.IsOuter(i));
    EXPECT_EQ(aTree.BegPrimitive(i), i);
    EXPECT_EQ(aTree.EndPrimitive(i), i);
  }
}

TEST(BVH_TreeTest, DeepTree)
{
  BVH_Tree<Standard_Real, 3, BVH_BinaryTree> aTree;

  // Create leaves
  BVH_Vec3d aMin1(0.0, 0.0, 0.0);
  BVH_Vec3d aMax1(1.0, 1.0, 1.0);
  int       aLeaf1 = aTree.AddLeafNode(aMin1, aMax1, 0, 0);

  BVH_Vec3d aMin2(1.0, 0.0, 0.0);
  BVH_Vec3d aMax2(2.0, 1.0, 1.0);
  int       aLeaf2 = aTree.AddLeafNode(aMin2, aMax2, 1, 1);

  BVH_Vec3d aMin3(2.0, 0.0, 0.0);
  BVH_Vec3d aMax3(3.0, 1.0, 1.0);
  int       aLeaf3 = aTree.AddLeafNode(aMin3, aMax3, 2, 2);

  BVH_Vec3d aMin4(3.0, 0.0, 0.0);
  BVH_Vec3d aMax4(4.0, 1.0, 1.0);
  int       aLeaf4 = aTree.AddLeafNode(aMin4, aMax4, 3, 3);

  // Create intermediate nodes
  BVH_Vec3d aMinI1(0.0, 0.0, 0.0);
  BVH_Vec3d aMaxI1(2.0, 1.0, 1.0);
  int       aInner1 = aTree.AddInnerNode(aMinI1, aMaxI1, aLeaf1, aLeaf2);

  BVH_Vec3d aMinI2(2.0, 0.0, 0.0);
  BVH_Vec3d aMaxI2(4.0, 1.0, 1.0);
  int       aInner2 = aTree.AddInnerNode(aMinI2, aMaxI2, aLeaf3, aLeaf4);

  // Create root
  BVH_Vec3d aMinRoot(0.0, 0.0, 0.0);
  BVH_Vec3d aMaxRoot(4.0, 1.0, 1.0);
  int       aRoot = aTree.AddInnerNode(aMinRoot, aMaxRoot, aInner1, aInner2);

  EXPECT_EQ(aTree.Length(), 7);
  EXPECT_FALSE(aTree.IsOuter(aRoot));
  EXPECT_FALSE(aTree.IsOuter(aInner1));
  EXPECT_FALSE(aTree.IsOuter(aInner2));
  EXPECT_TRUE(aTree.IsOuter(aLeaf1));
  EXPECT_TRUE(aTree.IsOuter(aLeaf4));
}

TEST(BVH_TreeTest, ModifyPrimitiveIndices)
{
  BVH_Tree<Standard_Real, 3, BVH_BinaryTree> aTree;

  aTree.AddLeafNode(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0), 0, 5);

  // Modify primitive indices
  aTree.BegPrimitive(0) = 10;
  aTree.EndPrimitive(0) = 20;

  EXPECT_EQ(aTree.BegPrimitive(0), 10);
  EXPECT_EQ(aTree.EndPrimitive(0), 20);
  EXPECT_EQ(aTree.NbPrimitives(0), 11);
}

TEST(BVH_TreeTest, ModifyMinMaxPoints)
{
  BVH_Tree<Standard_Real, 3, BVH_BinaryTree> aTree;

  aTree.AddLeafNode(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0), 0, 0);

  // Modify min/max points
  aTree.MinPoint(0) = BVH_Vec3d(-1.0, -1.0, -1.0);
  aTree.MaxPoint(0) = BVH_Vec3d(2.0, 2.0, 2.0);

  EXPECT_NEAR(aTree.MinPoint(0).x(), -1.0, Precision::Confusion());
  EXPECT_NEAR(aTree.MaxPoint(0).x(), 2.0, Precision::Confusion());
}

TEST(BVH_TreeTest, ChangeChild)
{
  BVH_Tree<Standard_Real, 3, BVH_BinaryTree> aTree;

  int aLeaf1 = aTree.AddLeafNode(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0), 0, 0);
  int aLeaf2 = aTree.AddLeafNode(BVH_Vec3d(1.0, 0.0, 0.0), BVH_Vec3d(2.0, 1.0, 1.0), 1, 1);
  int aLeaf3 = aTree.AddLeafNode(BVH_Vec3d(2.0, 0.0, 0.0), BVH_Vec3d(3.0, 1.0, 1.0), 2, 2);
  int aRoot =
    aTree.AddInnerNode(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(2.0, 1.0, 1.0), aLeaf1, aLeaf2);

  EXPECT_EQ(aTree.template Child<0>(aRoot), aLeaf1);
  EXPECT_EQ(aTree.template Child<1>(aRoot), aLeaf2);

  // Change children
  aTree.template ChangeChild<0>(aRoot) = aLeaf3;

  EXPECT_EQ(aTree.template Child<0>(aRoot), aLeaf3);
  EXPECT_EQ(aTree.template Child<1>(aRoot), aLeaf2);
}

TEST(BVH_TreeTest, NodeInfoBuffer)
{
  BVH_Tree<Standard_Real, 3, BVH_BinaryTree> aTree;

  aTree.AddLeafNode(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0), 5, 10);

  // Access node info buffer
  const BVH_Array4i& aBuffer = aTree.NodeInfoBuffer();
  int                aSize   = BVH::Array<int, 4>::Size(aBuffer);
  EXPECT_EQ(aSize, 1);
}

TEST(BVH_TreeTest, MinMaxPointBuffers)
{
  BVH_Tree<Standard_Real, 3, BVH_BinaryTree> aTree;

  aTree.AddLeafNode(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0), 0, 0);

  // Access point buffers
  const auto& aMinBuffer = aTree.MinPointBuffer();
  const auto& aMaxBuffer = aTree.MaxPointBuffer();

  int aMinSize = BVH::Array<Standard_Real, 3>::Size(aMinBuffer);
  int aMaxSize = BVH::Array<Standard_Real, 3>::Size(aMaxBuffer);
  EXPECT_EQ(aMinSize, 1);
  EXPECT_EQ(aMaxSize, 1);
}
