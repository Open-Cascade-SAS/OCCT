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

#include <BVH_SpatialMedianBuilder.hxx>
#include <BVH_Triangulation.hxx>

// Helper function to compute bounding box for a set
template <class T, int N>
BVH_Box<T, N> ComputeSetBox(BVH_Set<T, N>* theSet)
{
  BVH_Box<T, N>          aBox;
  const int aSize = theSet->Size();
  for (int i = 0; i < aSize; ++i)
  {
    aBox.Combine(theSet->Box(i));
  }
  return aBox;
}

// =============================================================================
// BVH_SpatialMedianBuilder Basic Tests
// =============================================================================

TEST(BVH_SpatialMedianBuilderTest, DefaultConstructor)
{
  BVH_SpatialMedianBuilder<double, 3> aBuilder;

  EXPECT_EQ(aBuilder.LeafNodeSize(), 5);
  EXPECT_EQ(aBuilder.MaxTreeDepth(), 32);
}

TEST(BVH_SpatialMedianBuilderTest, CustomParameters)
{
  BVH_SpatialMedianBuilder<double, 3> aBuilder(10, 20, true);

  EXPECT_EQ(aBuilder.LeafNodeSize(), 10);
  EXPECT_EQ(aBuilder.MaxTreeDepth(), 20);
}

TEST(BVH_SpatialMedianBuilderTest, BuildEmptySet)
{
  BVH_Triangulation<double, 3>        aTriangulation;
  BVH_Tree<double, 3>                 aTree;
  BVH_SpatialMedianBuilder<double, 3> aBuilder;

  BVH_Box<double, 3> aBox = ComputeSetBox(&aTriangulation);
  aBuilder.Build(&aTriangulation, &aTree, aBox);

  EXPECT_EQ(aTree.Length(), 0);
}

TEST(BVH_SpatialMedianBuilderTest, BuildSingleTriangle)
{
  BVH_Triangulation<double, 3> aTriangulation;

  BVH::Array<double, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, 0.0, 0.0));
  BVH::Array<double, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(1.0, 0.0, 0.0));
  BVH::Array<double, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, 1.0, 0.0));
  BVH::Array<int, 4>::Append(aTriangulation.Elements, BVH_Vec4i(0, 1, 2, 0));

  BVH_Tree<double, 3>                 aTree;
  BVH_SpatialMedianBuilder<double, 3> aBuilder;

  BVH_Box<double, 3> aBox = ComputeSetBox(&aTriangulation);
  aBuilder.Build(&aTriangulation, &aTree, aBox);

  EXPECT_EQ(aTree.Length(), 1);
  EXPECT_TRUE(aTree.IsOuter(0));
  EXPECT_EQ(aTree.NbPrimitives(0), 1);
}

TEST(BVH_SpatialMedianBuilderTest, BuildMultipleTriangles)
{
  BVH_Triangulation<double, 3> aTriangulation;
  const int                           aNumTriangles = 10;

  for (int i = 0; i < aNumTriangles; ++i)
  {
    double x = static_cast<double>(i);

    BVH::Array<double, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x, 0.0, 0.0));
    BVH::Array<double, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 0.5, 1.0, 0.0));
    BVH::Array<double, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 1.0, 0.0, 0.0));
    BVH::Array<int, 4>::Append(aTriangulation.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  BVH_Tree<double, 3>                 aTree;
  BVH_SpatialMedianBuilder<double, 3> aBuilder;

  BVH_Box<double, 3> aBox = ComputeSetBox(&aTriangulation);
  aBuilder.Build(&aTriangulation, &aTree, aBox);

  EXPECT_GT(aTree.Length(), 0);
  EXPECT_GE(aTree.Depth(), 1);
}

TEST(BVH_SpatialMedianBuilderTest, LeafNodeSizeRespected)
{
  BVH_Triangulation<double, 3> aTriangulation;
  const int                           aNumTriangles = 20;

  for (int i = 0; i < aNumTriangles; ++i)
  {
    double x = static_cast<double>(i);

    BVH::Array<double, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x, 0.0, 0.0));
    BVH::Array<double, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 0.5, 1.0, 0.0));
    BVH::Array<double, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 1.0, 0.0, 0.0));
    BVH::Array<int, 4>::Append(aTriangulation.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  const int                     aLeafSize = 3;
  BVH_Tree<double, 3>                 aTree;
  BVH_SpatialMedianBuilder<double, 3> aBuilder(aLeafSize, 32);

  BVH_Box<double, 3> aBox = ComputeSetBox(&aTriangulation);
  aBuilder.Build(&aTriangulation, &aTree, aBox);

  // Check all leaf nodes respect the leaf size
  for (int i = 0; i < aTree.Length(); ++i)
  {
    if (aTree.IsOuter(i))
    {
      EXPECT_LE(aTree.NbPrimitives(i), aLeafSize);
    }
  }
}

TEST(BVH_SpatialMedianBuilderTest, MaxDepthRespected)
{
  BVH_Triangulation<double, 3> aTriangulation;
  const int                           aNumTriangles = 100;

  for (int i = 0; i < aNumTriangles; ++i)
  {
    double x = static_cast<double>(i);

    BVH::Array<double, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x, 0.0, 0.0));
    BVH::Array<double, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 0.5, 1.0, 0.0));
    BVH::Array<double, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 1.0, 0.0, 0.0));
    BVH::Array<int, 4>::Append(aTriangulation.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  const int                     aMaxDepth = 5;
  BVH_Tree<double, 3>                 aTree;
  BVH_SpatialMedianBuilder<double, 3> aBuilder(1, aMaxDepth);

  BVH_Box<double, 3> aBox = ComputeSetBox(&aTriangulation);
  aBuilder.Build(&aTriangulation, &aTree, aBox);

  EXPECT_LE(aTree.Depth(), aMaxDepth);
}

TEST(BVH_SpatialMedianBuilderTest, SplitsAtSpatialMedian)
{
  BVH_Triangulation<double, 3> aTriangulation;
  const int                           aNumTriangles = 8;

  for (int i = 0; i < aNumTriangles; ++i)
  {
    double x = static_cast<double>(i);

    BVH::Array<double, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x, 0.0, 0.0));
    BVH::Array<double, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 0.5, 1.0, 0.0));
    BVH::Array<double, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 1.0, 0.0, 0.0));
    BVH::Array<int, 4>::Append(aTriangulation.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  BVH_Tree<double, 3>                 aTree;
  BVH_SpatialMedianBuilder<double, 3> aBuilder(2, 32);

  BVH_Box<double, 3> aBox = ComputeSetBox(&aTriangulation);
  aBuilder.Build(&aTriangulation, &aTree, aBox);

  // For uniformly distributed data, spatial median should create balanced splits
  // Root node should have roughly equal children
  if (aTree.Length() > 0 && !aTree.IsOuter(0))
  {
    int aLeftChild  = aTree.Child<0>(0);
    int aRightChild = aTree.Child<1>(0);

    if (aLeftChild != -1 && aRightChild != -1)
    {
      int aLeftPrims  = aTree.NbPrimitives(aLeftChild);
      int aRightPrims = aTree.NbPrimitives(aRightChild);

      // For 8 triangles, expect roughly 4-4 split (allowing some tolerance)
      EXPECT_GE(aLeftPrims, 2);
      EXPECT_LE(aLeftPrims, 6);
      EXPECT_GE(aRightPrims, 2);
      EXPECT_LE(aRightPrims, 6);
    }
  }
}

TEST(BVH_SpatialMedianBuilderTest, Build2D)
{
  BVH_Triangulation<double, 2> aTriangulation;
  const int                           aNumTriangles = 10;

  for (int i = 0; i < aNumTriangles; ++i)
  {
    double x = static_cast<double>(i);

    BVH::Array<double, 2>::Append(aTriangulation.Vertices, BVH_Vec2d(x, 0.0));
    BVH::Array<double, 2>::Append(aTriangulation.Vertices, BVH_Vec2d(x + 0.5, 1.0));
    BVH::Array<double, 2>::Append(aTriangulation.Vertices, BVH_Vec2d(x + 1.0, 0.0));
    BVH::Array<int, 4>::Append(aTriangulation.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  BVH_Tree<double, 2>                 aTree;
  BVH_SpatialMedianBuilder<double, 2> aBuilder;

  BVH_Box<double, 2> aBox = ComputeSetBox(&aTriangulation);
  aBuilder.Build(&aTriangulation, &aTree, aBox);

  EXPECT_GT(aTree.Length(), 0);
  EXPECT_GE(aTree.Depth(), 1);
}

TEST(BVH_SpatialMedianBuilderTest, UseMainAxisFlag)
{
  BVH_Triangulation<double, 3> aTriangulation;
  const int                           aNumTriangles = 20;

  // Create elongated distribution along X axis
  for (int i = 0; i < aNumTriangles; ++i)
  {
    double x = static_cast<double>(i * 10);

    BVH::Array<double, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x, 0.0, 0.0));
    BVH::Array<double, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 0.5, 1.0, 0.0));
    BVH::Array<double, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 1.0, 0.0, 0.0));
    BVH::Array<int, 4>::Append(aTriangulation.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  BVH_Tree<double, 3>                 aTreeMainAxis;
  BVH_SpatialMedianBuilder<double, 3> aBuilderMainAxis(5, 32, true);

  BVH_Box<double, 3> aBox = ComputeSetBox(&aTriangulation);
  aBuilderMainAxis.Build(&aTriangulation, &aTreeMainAxis, aBox);

  EXPECT_GT(aTreeMainAxis.Length(), 0);
}

TEST(BVH_SpatialMedianBuilderTest, CompareWithBinnedBuilder)
{
  BVH_Triangulation<double, 3> aTriangulation;
  const int                           aNumTriangles = 50;

  for (int i = 0; i < aNumTriangles; ++i)
  {
    double x = static_cast<double>(i);

    BVH::Array<double, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x, 0.0, 0.0));
    BVH::Array<double, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 0.5, 1.0, 0.0));
    BVH::Array<double, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 1.0, 0.0, 0.0));
    BVH::Array<int, 4>::Append(aTriangulation.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  BVH_Box<double, 3> aBox = ComputeSetBox(&aTriangulation);

  // Build with SpatialMedianBuilder (uses 2 bins)
  BVH_Tree<double, 3>                 aTreeMedian;
  BVH_SpatialMedianBuilder<double, 3> aBuilderMedian;
  aBuilderMedian.Build(&aTriangulation, &aTreeMedian, aBox);

  // Build with BinnedBuilder with 2 bins (should be identical)
  BVH_Tree<double, 3>             aTreeBinned;
  BVH_BinnedBuilder<double, 3, 2> aBuilderBinned;
  aBuilderBinned.Build(&aTriangulation, &aTreeBinned, aBox);

  // Both should produce trees (exact structure may vary but both should be valid)
  EXPECT_EQ(aTreeMedian.Length(), aTreeBinned.Length());
  EXPECT_EQ(aTreeMedian.Depth(), aTreeBinned.Depth());
}

TEST(BVH_SpatialMedianBuilderTest, ClusteredData)
{
  BVH_Triangulation<double, 3> aTriangulation;
  const int                           aNumTriangles = 30;

  // Create two clusters: 15 triangles at x=0, 15 at x=100
  for (int i = 0; i < aNumTriangles; ++i)
  {
    double x = (i < 15) ? 0.0 : 100.0;
    double y = static_cast<double>(i % 15);

    BVH::Array<double, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x, y, 0.0));
    BVH::Array<double, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 0.5, y + 1.0, 0.0));
    BVH::Array<double, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 1.0, y, 0.0));
    BVH::Array<int, 4>::Append(aTriangulation.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  BVH_Tree<double, 3>                 aTree;
  BVH_SpatialMedianBuilder<double, 3> aBuilder;

  BVH_Box<double, 3> aBox = ComputeSetBox(&aTriangulation);
  aBuilder.Build(&aTriangulation, &aTree, aBox);

  // Spatial median should split the two clusters
  EXPECT_GT(aTree.Length(), 0);
  EXPECT_GE(aTree.Depth(), 1);
}

TEST(BVH_SpatialMedianBuilderTest, FloatPrecision)
{
  BVH_Triangulation<float, 3> aTriangulation;
  const int                                aNumTriangles = 10;

  for (int i = 0; i < aNumTriangles; ++i)
  {
    float x = static_cast<float>(i);

    BVH::Array<float, 3>::Append(aTriangulation.Vertices,
                                              NCollection_Vec3<float>(x, 0.0f, 0.0f));
    BVH::Array<float, 3>::Append(
      aTriangulation.Vertices,
      NCollection_Vec3<float>(x + 0.5f, 1.0f, 0.0f));
    BVH::Array<float, 3>::Append(
      aTriangulation.Vertices,
      NCollection_Vec3<float>(x + 1.0f, 0.0f, 0.0f));
    BVH::Array<int, 4>::Append(aTriangulation.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  BVH_Tree<float, 3>                 aTree;
  BVH_SpatialMedianBuilder<float, 3> aBuilder;

  BVH_Box<float, 3> aBox = ComputeSetBox(&aTriangulation);
  aBuilder.Build(&aTriangulation, &aTree, aBox);

  EXPECT_GT(aTree.Length(), 0);
}

TEST(BVH_SpatialMedianBuilderTest, IdenticalBoxes)
{
  BVH_Triangulation<double, 3> aTriangulation;
  const int                           aNumTriangles = 10;

  // All triangles at the same position
  for (int i = 0; i < aNumTriangles; ++i)
  {
    BVH::Array<double, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, 0.0, 0.0));
    BVH::Array<double, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(1.0, 0.0, 0.0));
    BVH::Array<double, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, 1.0, 0.0));
    BVH::Array<int, 4>::Append(aTriangulation.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  BVH_Tree<double, 3>                 aTree;
  BVH_SpatialMedianBuilder<double, 3> aBuilder;

  BVH_Box<double, 3> aBox = ComputeSetBox(&aTriangulation);
  aBuilder.Build(&aTriangulation, &aTree, aBox);

  // Should create a single leaf containing all triangles
  EXPECT_GT(aTree.Length(), 0);
}

TEST(BVH_SpatialMedianBuilderTest, LargeDataSet)
{
  BVH_Triangulation<double, 3> aTriangulation;
  const int                           aNumTriangles = 500;

  // Random-ish distribution
  for (int i = 0; i < aNumTriangles; ++i)
  {
    double x = static_cast<double>((i * 7) % 100);
    double y = static_cast<double>((i * 11) % 100);
    double z = static_cast<double>((i * 13) % 100);

    BVH::Array<double, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x, y, z));
    BVH::Array<double, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 0.5, y + 1.0, z));
    BVH::Array<double, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 1.0, y, z + 1.0));
    BVH::Array<int, 4>::Append(aTriangulation.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  BVH_Tree<double, 3>                 aTree;
  BVH_SpatialMedianBuilder<double, 3> aBuilder;

  BVH_Box<double, 3> aBox = ComputeSetBox(&aTriangulation);
  aBuilder.Build(&aTriangulation, &aTree, aBox);

  EXPECT_GT(aTree.Length(), 0);
  EXPECT_GT(aTree.Depth(), 5); // Large dataset should produce deeper tree

  // Estimate SAH quality
  double aSAH = aTree.EstimateSAH();
  EXPECT_GT(aSAH, 0.0);
}
