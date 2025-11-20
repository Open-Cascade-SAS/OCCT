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

#include <BVH_SweepPlaneBuilder.hxx>
#include <BVH_Triangulation.hxx>

// Helper to compute bounding box for BVH_Set (avoids cached empty box issue)
template <class T, int N>
BVH_Box<T, N> ComputeSetBox(BVH_Set<T, N>* theSet)
{
  BVH_Box<T, N>          aBox;
  const Standard_Integer aSize = theSet->Size();
  for (Standard_Integer i = 0; i < aSize; ++i)
  {
    aBox.Combine(theSet->Box(i));
  }
  return aBox;
}

// =============================================================================
// BVH_SweepPlaneBuilder Construction Tests
// =============================================================================

TEST(BVH_SweepPlaneBuilderTest, DefaultConstructor)
{
  BVH_SweepPlaneBuilder<Standard_Real, 3> aBuilder;
  EXPECT_EQ(aBuilder.LeafNodeSize(), BVH_Constants_LeafNodeSizeDefault);
  EXPECT_EQ(aBuilder.MaxTreeDepth(), BVH_Constants_MaxTreeDepth);
}

TEST(BVH_SweepPlaneBuilderTest, CustomParameters)
{
  BVH_SweepPlaneBuilder<Standard_Real, 3> aBuilder(5, 20, 1);
  EXPECT_EQ(aBuilder.LeafNodeSize(), 5);
  EXPECT_EQ(aBuilder.MaxTreeDepth(), 20);
}

// =============================================================================
// BVH Building Tests
// =============================================================================

TEST(BVH_SweepPlaneBuilderTest, BuildEmptyTriangulation)
{
  BVH_Triangulation<Standard_Real, 3>     aTriangulation;
  BVH_SweepPlaneBuilder<Standard_Real, 3> aBuilder;

  BVH_Tree<Standard_Real, 3> aBVH;
  BVH_Box<Standard_Real, 3>  aBox;

  aBuilder.Build(&aTriangulation, &aBVH, aBox);

  EXPECT_EQ(aBVH.Length(), 0);
}

TEST(BVH_SweepPlaneBuilderTest, BuildSingleTriangle)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  // Add vertices
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, 0.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(1.0, 0.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, 1.0, 0.0));

  // Add triangle
  BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements, BVH_Vec4i(0, 1, 2, 0));

  BVH_SweepPlaneBuilder<Standard_Real, 3> aBuilder(1, 32, 1);
  BVH_Tree<Standard_Real, 3>              aBVH;
  BVH_Box<Standard_Real, 3>               aBox = ComputeSetBox(&aTriangulation);

  aBuilder.Build(&aTriangulation, &aBVH, aBox);

  // Single triangle should create one leaf node
  EXPECT_EQ(aBVH.Length(), 1);
  EXPECT_TRUE(aBVH.IsOuter(0));
  EXPECT_EQ(aBVH.BegPrimitive(0), 0);
  EXPECT_EQ(aBVH.EndPrimitive(0), 0);
}

TEST(BVH_SweepPlaneBuilderTest, BuildTwoTriangles)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  // Create two triangles forming a quad
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, 0.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(1.0, 0.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(1.0, 1.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, 1.0, 0.0));

  BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements, BVH_Vec4i(0, 1, 2, 0));
  BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements, BVH_Vec4i(0, 2, 3, 0));

  BVH_SweepPlaneBuilder<Standard_Real, 3> aBuilder(1, 32, 1);
  BVH_Tree<Standard_Real, 3>              aBVH;
  BVH_Box<Standard_Real, 3>               aBox = ComputeSetBox(&aTriangulation);

  aBuilder.Build(&aTriangulation, &aBVH, aBox);

  // Two triangles with leaf size 1 should create internal nodes
  EXPECT_GT(aBVH.Length(), 2);
  EXPECT_GE(aBVH.Depth(), 1);
}

TEST(BVH_SweepPlaneBuilderTest, BuildMultipleTriangles_Grid)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  // Create a 2x2 grid of quads (8 triangles)
  const int aGridSize = 3;
  for (int i = 0; i < aGridSize; ++i)
  {
    for (int j = 0; j < aGridSize; ++j)
    {
      BVH::Array<Standard_Real, 3>::Append(
        aTriangulation.Vertices,
        BVH_Vec3d(static_cast<Standard_Real>(i), static_cast<Standard_Real>(j), 0.0));
    }
  }

  // Add triangles for the grid
  for (int i = 0; i < aGridSize - 1; ++i)
  {
    for (int j = 0; j < aGridSize - 1; ++j)
    {
      int aV0 = i * aGridSize + j;
      int aV1 = i * aGridSize + (j + 1);
      int aV2 = (i + 1) * aGridSize + (j + 1);
      int aV3 = (i + 1) * aGridSize + j;

      BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements, BVH_Vec4i(aV0, aV1, aV2, 0));
      BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements, BVH_Vec4i(aV0, aV2, aV3, 0));
    }
  }

  BVH_SweepPlaneBuilder<Standard_Real, 3> aBuilder(2, 32, 1);
  BVH_Tree<Standard_Real, 3>              aBVH;
  BVH_Box<Standard_Real, 3>               aBox = ComputeSetBox(&aTriangulation);

  aBuilder.Build(&aTriangulation, &aBVH, aBox);

  int aTotalTriangles = (aGridSize - 1) * (aGridSize - 1) * 2;
  EXPECT_GT(aBVH.Length(), 0);
  EXPECT_GE(aBVH.Depth(), 1);

  // Verify tree bounds contain all triangles
  BVH_Box<Standard_Real, 3> aRootBox(aBVH.MinPoint(0), aBVH.MaxPoint(0));
  for (int i = 0; i < aTotalTriangles; ++i)
  {
    BVH_Box<Standard_Real, 3> aTriBox = aTriangulation.Box(i);
    EXPECT_FALSE(aRootBox.IsOut(aTriBox));
  }
}

TEST(BVH_SweepPlaneBuilderTest, SplitAlongDifferentAxes)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  // Create triangles spread along X axis (should prefer X-axis split)
  for (int i = 0; i < 10; ++i)
  {
    Standard_Real x = static_cast<Standard_Real>(i) * 2.0;
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 1.0, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 0.5, 1.0, 0.0));

    BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  BVH_SweepPlaneBuilder<Standard_Real, 3> aBuilder(1, 32, 1);
  BVH_Tree<Standard_Real, 3>              aBVH;
  BVH_Box<Standard_Real, 3>               aBox = ComputeSetBox(&aTriangulation);

  aBuilder.Build(&aTriangulation, &aBVH, aBox);

  // Should create splits along X axis
  EXPECT_GT(aBVH.Length(), 1);
  EXPECT_GE(aBVH.Depth(), 1);
}

TEST(BVH_SweepPlaneBuilderTest, DegenerateCase_AllSamePosition)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  // All triangles at same position (degenerate case)
  for (int i = 0; i < 5; ++i)
  {
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.1, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, 0.1, 0.0));

    BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  BVH_SweepPlaneBuilder<Standard_Real, 3> aBuilder(1, 32, 1);
  BVH_Tree<Standard_Real, 3>              aBVH;
  BVH_Box<Standard_Real, 3>               aBox = ComputeSetBox(&aTriangulation);

  aBuilder.Build(&aTriangulation, &aBVH, aBox);

  // Should still build valid tree even with degenerate geometry
  EXPECT_GT(aBVH.Length(), 0);
}

// =============================================================================
// SAH Quality Tests
// =============================================================================

TEST(BVH_SweepPlaneBuilderTest, SAHQuality_VerifyBetter)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  // Create well-separated triangle groups (should result in good SAH)
  // Group 1: left side
  for (int i = 0; i < 5; ++i)
  {
    Standard_Real x = static_cast<Standard_Real>(i) * 0.5;
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 0.4, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 0.2, 0.4, 0.0));
    BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  // Group 2: right side (spatially separated)
  for (int i = 0; i < 5; ++i)
  {
    Standard_Real x = 10.0 + static_cast<Standard_Real>(i) * 0.5;
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 0.4, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 0.2, 0.4, 0.0));
    BVH::Array<Standard_Integer, 4>::Append(
      aTriangulation.Elements,
      BVH_Vec4i((5 + i) * 3, (5 + i) * 3 + 1, (5 + i) * 3 + 2, 0));
  }

  BVH_SweepPlaneBuilder<Standard_Real, 3> aBuilder(2, 32, 1);
  BVH_Tree<Standard_Real, 3>              aBVH;
  BVH_Box<Standard_Real, 3>               aBox = ComputeSetBox(&aTriangulation);

  aBuilder.Build(&aTriangulation, &aBVH, aBox);

  // Spatially separated groups should create clear split
  EXPECT_GE(aBVH.Depth(), 1);

  // Root should have two children covering separate regions
  if (!aBVH.IsOuter(0))
  {
    int aLeftChild  = aBVH.template Child<0>(0);
    int aRightChild = aBVH.template Child<1>(0);

    BVH_Box<Standard_Real, 3> aLeftBox(aBVH.MinPoint(aLeftChild), aBVH.MaxPoint(aLeftChild));
    BVH_Box<Standard_Real, 3> aRightBox(aBVH.MinPoint(aRightChild), aBVH.MaxPoint(aRightChild));

    // Boxes should not significantly overlap for well-separated geometry
    Standard_Real aOverlap = 0.0;
    for (int i = 0; i < 3; ++i)
    {
      Standard_Real aMin = std::max(aLeftBox.CornerMin()[i], aRightBox.CornerMin()[i]);
      Standard_Real aMax = std::min(aLeftBox.CornerMax()[i], aRightBox.CornerMax()[i]);
      if (aMax > aMin)
      {
        aOverlap += (aMax - aMin);
      }
    }

    // Overlap should be minimal for well-separated groups
    Standard_Real aTotalSize = (aBox.CornerMax()[0] - aBox.CornerMin()[0]);
    EXPECT_LT(aOverlap, aTotalSize * 0.1); // Less than 10% overlap
  }
}

// =============================================================================
// Leaf Size Tests
// =============================================================================

TEST(BVH_SweepPlaneBuilderTest, LeafSize_RespectMaxSize)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  // Create 20 triangles
  for (int i = 0; i < 20; ++i)
  {
    Standard_Real x = static_cast<Standard_Real>(i);
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 0.9, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 0.5, 0.9, 0.0));
    BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  BVH_SweepPlaneBuilder<Standard_Real, 3> aBuilder(5, 32, 1); // Leaf size = 5
  BVH_Tree<Standard_Real, 3>              aBVH;
  BVH_Box<Standard_Real, 3>               aBox = ComputeSetBox(&aTriangulation);

  aBuilder.Build(&aTriangulation, &aBVH, aBox);

  // Verify leaf nodes don't exceed the specified size
  for (int i = 0; i < aBVH.Length(); ++i)
  {
    if (aBVH.IsOuter(i))
    {
      int aLeafSize = aBVH.EndPrimitive(i) - aBVH.BegPrimitive(i) + 1;
      EXPECT_LE(aLeafSize, 5);
    }
  }
}
