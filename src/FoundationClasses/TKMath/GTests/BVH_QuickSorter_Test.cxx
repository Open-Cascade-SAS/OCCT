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

#include <BVH_QuickSorter.hxx>
#include <BVH_Triangulation.hxx>

// =============================================================================
// BVH_QuickSorter Basic Tests
// =============================================================================

TEST(BVH_QuickSorterTest, Constructor)
{
  BVH_QuickSorter<Standard_Real, 3> aSorterX(0);
  BVH_QuickSorter<Standard_Real, 3> aSorterY(1);
  BVH_QuickSorter<Standard_Real, 3> aSorterZ(2);

  // Constructor should not crash
  EXPECT_TRUE(true);
}

TEST(BVH_QuickSorterTest, SortEmptySet)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;
  BVH_QuickSorter<Standard_Real, 3>   aSorter(0);

  // Sorting empty set should not crash
  aSorter.Perform(&aTriangulation);
  EXPECT_EQ(aTriangulation.Size(), 0);
}

TEST(BVH_QuickSorterTest, SortSingleElement)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, 0.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(1.0, 0.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, 1.0, 0.0));
  BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements, BVH_Vec4i(0, 1, 2, 0));

  BVH_QuickSorter<Standard_Real, 3> aSorter(0);
  aSorter.Perform(&aTriangulation);

  // Single element should remain unchanged
  EXPECT_EQ(aTriangulation.Size(), 1);
}

// =============================================================================
// Sorting Correctness Tests
// =============================================================================

TEST(BVH_QuickSorterTest, SortAlongXAxis)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  // Create triangles with increasing X centroids: 5, 1, 3
  // Triangle 0: centroid X = 5
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(4.0, 0.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(5.0, 0.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(6.0, 1.0, 0.0));
  BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements, BVH_Vec4i(0, 1, 2, 0));

  // Triangle 1: centroid X = 1
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, 0.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(1.0, 0.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(2.0, 1.0, 0.0));
  BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements, BVH_Vec4i(3, 4, 5, 0));

  // Triangle 2: centroid X = 3
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(2.0, 0.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(3.0, 0.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(4.0, 1.0, 0.0));
  BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements, BVH_Vec4i(6, 7, 8, 0));

  BVH_QuickSorter<Standard_Real, 3> aSorter(0); // Sort along X
  aSorter.Perform(&aTriangulation);

  // After sorting: should be ordered 1, 3, 5 (indices 1, 2, 0)
  EXPECT_LT(aTriangulation.Center(0, 0), aTriangulation.Center(1, 0));
  EXPECT_LT(aTriangulation.Center(1, 0), aTriangulation.Center(2, 0));

  // Verify actual values
  EXPECT_NEAR(aTriangulation.Center(0, 0), 1.0, 1e-10);
  EXPECT_NEAR(aTriangulation.Center(1, 0), 3.0, 1e-10);
  EXPECT_NEAR(aTriangulation.Center(2, 0), 5.0, 1e-10);
}

TEST(BVH_QuickSorterTest, SortAlongYAxis)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  // Create triangles with Y centroids: 3, 1, 2
  for (int i = 0; i < 3; ++i)
  {
    Standard_Real y = (i == 0 ? 3.0 : (i == 1 ? 1.0 : 2.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, y - 0.5, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(1.0, y, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.5, y + 0.5, 0.0));
    BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  BVH_QuickSorter<Standard_Real, 3> aSorter(1); // Sort along Y
  aSorter.Perform(&aTriangulation);

  // Should be ordered by Y centroid
  EXPECT_LT(aTriangulation.Center(0, 1), aTriangulation.Center(1, 1));
  EXPECT_LT(aTriangulation.Center(1, 1), aTriangulation.Center(2, 1));
}

TEST(BVH_QuickSorterTest, SortAlongZAxis)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  // Create triangles with Z centroids: 2, 0, 1
  for (int i = 0; i < 3; ++i)
  {
    Standard_Real z = (i == 0 ? 2.0 : (i == 1 ? 0.0 : 1.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, 0.0, z));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(1.0, 0.0, z));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.5, 1.0, z));
    BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  BVH_QuickSorter<Standard_Real, 3> aSorter(2); // Sort along Z
  aSorter.Perform(&aTriangulation);

  // Should be ordered by Z centroid
  EXPECT_LT(aTriangulation.Center(0, 2), aTriangulation.Center(1, 2));
  EXPECT_LT(aTriangulation.Center(1, 2), aTriangulation.Center(2, 2));
}

TEST(BVH_QuickSorterTest, SortRangeInSet)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  // Create 5 triangles with X centroids: 0, 1, 2, 3, 4
  for (int i = 0; i < 5; ++i)
  {
    Standard_Real x = static_cast<Standard_Real>(i);
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 1.0, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 0.5, 1.0, 0.0));
    BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  // Reverse the middle 3 elements (indices 1, 2, 3) manually
  aTriangulation.Swap(1, 3);

  // Now we have centroids: 0, 3, 2, 1, 4
  EXPECT_NEAR(aTriangulation.Center(1, 0), 3.5, 1e-10);
  EXPECT_NEAR(aTriangulation.Center(2, 0), 2.5, 1e-10);
  EXPECT_NEAR(aTriangulation.Center(3, 0), 1.5, 1e-10);

  // Sort only range [1, 3] (middle 3 elements)
  BVH_QuickSorter<Standard_Real, 3> aSorter(0);
  aSorter.Perform(&aTriangulation, 1, 3);

  // Elements 0 and 4 should remain unchanged
  EXPECT_NEAR(aTriangulation.Center(0, 0), 0.5, 1e-10);
  EXPECT_NEAR(aTriangulation.Center(4, 0), 4.5, 1e-10);

  // Elements 1-3 should be sorted: 1, 2, 3
  EXPECT_NEAR(aTriangulation.Center(1, 0), 1.5, 1e-10);
  EXPECT_NEAR(aTriangulation.Center(2, 0), 2.5, 1e-10);
  EXPECT_NEAR(aTriangulation.Center(3, 0), 3.5, 1e-10);
}

// =============================================================================
// Edge Cases and Stability Tests
// =============================================================================

TEST(BVH_QuickSorterTest, AlreadySorted)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  // Create already sorted triangles
  for (int i = 0; i < 10; ++i)
  {
    Standard_Real x = static_cast<Standard_Real>(i);
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 0.9, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 0.5, 0.9, 0.0));
    BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  BVH_QuickSorter<Standard_Real, 3> aSorter(0);
  aSorter.Perform(&aTriangulation);

  // Should remain sorted
  for (int i = 0; i < 9; ++i)
  {
    EXPECT_LT(aTriangulation.Center(i, 0), aTriangulation.Center(i + 1, 0));
  }
}

TEST(BVH_QuickSorterTest, ReverseSorted)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  // Create reverse sorted triangles
  for (int i = 0; i < 10; ++i)
  {
    Standard_Real x = static_cast<Standard_Real>(9 - i);
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 0.9, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 0.5, 0.9, 0.0));
    BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  BVH_QuickSorter<Standard_Real, 3> aSorter(0);
  aSorter.Perform(&aTriangulation);

  // Should become sorted
  for (int i = 0; i < 9; ++i)
  {
    EXPECT_LT(aTriangulation.Center(i, 0), aTriangulation.Center(i + 1, 0));
  }
}

TEST(BVH_QuickSorterTest, DuplicateValues)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  // Create triangles with duplicate centroids: 1, 2, 2, 2, 3
  for (int i = 0; i < 5; ++i)
  {
    Standard_Real x = (i == 0 ? 1.0 : (i <= 3 ? 2.0 : 3.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 0.9, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 0.5, 0.9, 0.0));
    BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  BVH_QuickSorter<Standard_Real, 3> aSorter(0);
  aSorter.Perform(&aTriangulation);

  // Should be non-decreasing
  for (int i = 0; i < 4; ++i)
  {
    EXPECT_LE(aTriangulation.Center(i, 0), aTriangulation.Center(i + 1, 0));
  }
}

TEST(BVH_QuickSorterTest, AllSameValue)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  // All triangles at same position
  for (int i = 0; i < 5; ++i)
  {
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(1.0, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(1.9, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(1.5, 0.9, 0.0));
    BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  BVH_QuickSorter<Standard_Real, 3> aSorter(0);
  aSorter.Perform(&aTriangulation);

  // All should have same centroid
  for (int i = 0; i < 5; ++i)
  {
    EXPECT_NEAR(aTriangulation.Center(i, 0), 1.467, 1e-2);
  }
}

TEST(BVH_QuickSorterTest, LargeDataSet)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  // Create 1000 triangles with random-ish order
  for (int i = 0; i < 1000; ++i)
  {
    // Use simple pseudo-random pattern
    Standard_Real x = static_cast<Standard_Real>((i * 37) % 1000);
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 0.9, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 0.5, 0.9, 0.0));
    BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  BVH_QuickSorter<Standard_Real, 3> aSorter(0);
  aSorter.Perform(&aTriangulation);

  // Verify sorted
  for (int i = 0; i < 999; ++i)
  {
    EXPECT_LE(aTriangulation.Center(i, 0), aTriangulation.Center(i + 1, 0));
  }
}

TEST(BVH_QuickSorterTest, TwoElements)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  // Two elements out of order
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(2.0, 0.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(3.0, 0.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(2.5, 1.0, 0.0));
  BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements, BVH_Vec4i(0, 1, 2, 0));

  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, 0.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(1.0, 0.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.5, 1.0, 0.0));
  BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements, BVH_Vec4i(3, 4, 5, 0));

  BVH_QuickSorter<Standard_Real, 3> aSorter(0);
  aSorter.Perform(&aTriangulation);

  EXPECT_LT(aTriangulation.Center(0, 0), aTriangulation.Center(1, 0));
}
