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

#include <BVH_Triangulation.hxx>

// =============================================================================
// BVH_Triangulation Basic Tests
// =============================================================================

TEST(BVH_TriangulationTest, DefaultConstructor)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  EXPECT_EQ(aTriangulation.Size(), 0);
  EXPECT_EQ((BVH::Array<Standard_Real, 3>::Size(aTriangulation.Vertices)), 0);
  EXPECT_EQ((BVH::Array<Standard_Integer, 4>::Size(aTriangulation.Elements)), 0);
}

TEST(BVH_TriangulationTest, AddSingleTriangle)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, 0.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(1.0, 0.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, 1.0, 0.0));
  BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements, BVH_Vec4i(0, 1, 2, 0));

  EXPECT_EQ(aTriangulation.Size(), 1);
  EXPECT_EQ((BVH::Array<Standard_Real, 3>::Size(aTriangulation.Vertices)), 3);
}

TEST(BVH_TriangulationTest, AddMultipleTriangles)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  for (int i = 0; i < 5; ++i)
  {
    Standard_Real x = static_cast<Standard_Real>(i);
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 1.0, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 0.5, 1.0, 0.0));
    BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  EXPECT_EQ(aTriangulation.Size(), 5);
  EXPECT_EQ((BVH::Array<Standard_Real, 3>::Size(aTriangulation.Vertices)), 15);
}

// =============================================================================
// BVH_Triangulation Box Tests
// =============================================================================

TEST(BVH_TriangulationTest, BoxForSingleTriangle)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(1.0, 2.0, 3.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(4.0, 1.0, 2.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(2.0, 3.0, 1.0));
  BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements, BVH_Vec4i(0, 1, 2, 0));

  BVH_Box<Standard_Real, 3> aBox = aTriangulation.Box(0);

  // Min point should be componentwise min of all vertices
  EXPECT_NEAR(aBox.CornerMin().x(), 1.0, 1e-10);
  EXPECT_NEAR(aBox.CornerMin().y(), 1.0, 1e-10);
  EXPECT_NEAR(aBox.CornerMin().z(), 1.0, 1e-10);

  // Max point should be componentwise max of all vertices
  EXPECT_NEAR(aBox.CornerMax().x(), 4.0, 1e-10);
  EXPECT_NEAR(aBox.CornerMax().y(), 3.0, 1e-10);
  EXPECT_NEAR(aBox.CornerMax().z(), 3.0, 1e-10);
}

TEST(BVH_TriangulationTest, BoxForDegenerateTriangle)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  // Triangle with all vertices at same point (degenerate)
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(1.0, 1.0, 1.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(1.0, 1.0, 1.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(1.0, 1.0, 1.0));
  BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements, BVH_Vec4i(0, 1, 2, 0));

  BVH_Box<Standard_Real, 3> aBox = aTriangulation.Box(0);

  EXPECT_NEAR(aBox.CornerMin().x(), 1.0, 1e-10);
  EXPECT_NEAR(aBox.CornerMin().y(), 1.0, 1e-10);
  EXPECT_NEAR(aBox.CornerMin().z(), 1.0, 1e-10);
  EXPECT_NEAR(aBox.CornerMax().x(), 1.0, 1e-10);
  EXPECT_NEAR(aBox.CornerMax().y(), 1.0, 1e-10);
  EXPECT_NEAR(aBox.CornerMax().z(), 1.0, 1e-10);
}

TEST(BVH_TriangulationTest, BoxForFlatTriangle)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  // Triangle flat in XY plane (Z = 0)
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, 0.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(2.0, 0.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(1.0, 3.0, 0.0));
  BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements, BVH_Vec4i(0, 1, 2, 0));

  BVH_Box<Standard_Real, 3> aBox = aTriangulation.Box(0);

  EXPECT_NEAR(aBox.CornerMin().z(), 0.0, 1e-10);
  EXPECT_NEAR(aBox.CornerMax().z(), 0.0, 1e-10);
  EXPECT_NEAR(aBox.CornerMax().x(), 2.0, 1e-10);
  EXPECT_NEAR(aBox.CornerMax().y(), 3.0, 1e-10);
}

// =============================================================================
// BVH_Triangulation Center Tests
// =============================================================================

TEST(BVH_TriangulationTest, CenterComputation)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  // Triangle with vertices at (0,0,0), (3,0,0), (0,3,0)
  // Centroid should be at (1,1,0)
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, 0.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(3.0, 0.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, 3.0, 0.0));
  BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements, BVH_Vec4i(0, 1, 2, 0));

  EXPECT_NEAR(aTriangulation.Center(0, 0), 1.0, 1e-10); // X centroid
  EXPECT_NEAR(aTriangulation.Center(0, 1), 1.0, 1e-10); // Y centroid
  EXPECT_NEAR(aTriangulation.Center(0, 2), 0.0, 1e-10); // Z centroid
}

TEST(BVH_TriangulationTest, CenterAlongXAxis)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(1.0, 0.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(2.0, 0.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(3.0, 0.0, 0.0));
  BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements, BVH_Vec4i(0, 1, 2, 0));

  // Centroid X = (1 + 2 + 3) / 3 = 2.0
  EXPECT_NEAR(aTriangulation.Center(0, 0), 2.0, 1e-10);
}

TEST(BVH_TriangulationTest, CenterAlongYAxis)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, 2.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, 4.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, 6.0, 0.0));
  BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements, BVH_Vec4i(0, 1, 2, 0));

  // Centroid Y = (2 + 4 + 6) / 3 = 4.0
  EXPECT_NEAR(aTriangulation.Center(0, 1), 4.0, 1e-10);
}

TEST(BVH_TriangulationTest, CenterAlongZAxis)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, 0.0, 1.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, 0.0, 4.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, 0.0, 7.0));
  BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements, BVH_Vec4i(0, 1, 2, 0));

  // Centroid Z = (1 + 4 + 7) / 3 = 4.0
  EXPECT_NEAR(aTriangulation.Center(0, 2), 4.0, 1e-10);
}

// =============================================================================
// BVH_Triangulation Swap Tests
// =============================================================================

TEST(BVH_TriangulationTest, SwapTwoTriangles)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  // Triangle 0
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, 0.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(1.0, 0.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, 1.0, 0.0));
  BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements, BVH_Vec4i(0, 1, 2, 0));

  // Triangle 1
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(5.0, 5.0, 5.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(6.0, 5.0, 5.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(5.0, 6.0, 5.0));
  BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements, BVH_Vec4i(3, 4, 5, 0));

  Standard_Real aCentroid0Before = aTriangulation.Center(0, 0);
  Standard_Real aCentroid1Before = aTriangulation.Center(1, 0);

  aTriangulation.Swap(0, 1);

  // After swap, centroids should be swapped
  EXPECT_NEAR(aTriangulation.Center(0, 0), aCentroid1Before, 1e-10);
  EXPECT_NEAR(aTriangulation.Center(1, 0), aCentroid0Before, 1e-10);
}

TEST(BVH_TriangulationTest, SwapPreservesVertices)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  // Add vertices and triangles
  for (int i = 0; i < 3; ++i)
  {
    Standard_Real x = static_cast<Standard_Real>(i) * 10.0;
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 1.0, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 0.5, 1.0, 0.0));
    BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  Standard_Integer aVertexCount = BVH::Array<Standard_Real, 3>::Size(aTriangulation.Vertices);

  aTriangulation.Swap(0, 2);

  // Vertex count should not change
  EXPECT_EQ((BVH::Array<Standard_Real, 3>::Size(aTriangulation.Vertices)), aVertexCount);
}

// =============================================================================
// BVH_Triangulation 2D Tests
// =============================================================================

TEST(BVH_TriangulationTest, Triangulation2D)
{
  BVH_Triangulation<Standard_Real, 2> aTriangulation2D;

  BVH::Array<Standard_Real, 2>::Append(aTriangulation2D.Vertices, BVH_Vec2d(0.0, 0.0));
  BVH::Array<Standard_Real, 2>::Append(aTriangulation2D.Vertices, BVH_Vec2d(1.0, 0.0));
  BVH::Array<Standard_Real, 2>::Append(aTriangulation2D.Vertices, BVH_Vec2d(0.5, 1.0));
  BVH::Array<Standard_Integer, 4>::Append(aTriangulation2D.Elements, BVH_Vec4i(0, 1, 2, 0));

  EXPECT_EQ(aTriangulation2D.Size(), 1);

  BVH_Box<Standard_Real, 2> aBox = aTriangulation2D.Box(0);
  EXPECT_NEAR(aBox.CornerMin().x(), 0.0, 1e-10);
  EXPECT_NEAR(aBox.CornerMin().y(), 0.0, 1e-10);
  EXPECT_NEAR(aBox.CornerMax().x(), 1.0, 1e-10);
  EXPECT_NEAR(aBox.CornerMax().y(), 1.0, 1e-10);
}

// =============================================================================
// BVH_Triangulation Shared Vertices Tests
// =============================================================================

TEST(BVH_TriangulationTest, SharedVertices)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  // Create quad with 4 vertices, 2 triangles sharing edge
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, 0.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(1.0, 0.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(1.0, 1.0, 0.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, 1.0, 0.0));

  // Triangle 0: (0, 1, 2)
  BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements, BVH_Vec4i(0, 1, 2, 0));
  // Triangle 1: (0, 2, 3) - shares vertices 0 and 2 with triangle 0
  BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements, BVH_Vec4i(0, 2, 3, 0));

  EXPECT_EQ(aTriangulation.Size(), 2);
  EXPECT_EQ((BVH::Array<Standard_Real, 3>::Size(aTriangulation.Vertices)), 4);

  // Both triangles should have valid boxes
  BVH_Box<Standard_Real, 3> aBox0 = aTriangulation.Box(0);
  BVH_Box<Standard_Real, 3> aBox1 = aTriangulation.Box(1);

  EXPECT_FALSE(aBox0.IsOut(aBox1)); // Should overlap
}

// =============================================================================
// BVH_Triangulation Float Precision Tests
// =============================================================================

TEST(BVH_TriangulationTest, FloatPrecision)
{
  BVH_Triangulation<Standard_ShortReal, 3> aTriangulation;

  BVH::Array<Standard_ShortReal, 3>::Append(aTriangulation.Vertices, BVH_Vec3f(0.0f, 0.0f, 0.0f));
  BVH::Array<Standard_ShortReal, 3>::Append(aTriangulation.Vertices, BVH_Vec3f(1.0f, 0.0f, 0.0f));
  BVH::Array<Standard_ShortReal, 3>::Append(aTriangulation.Vertices, BVH_Vec3f(0.0f, 1.0f, 0.0f));
  BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements, BVH_Vec4i(0, 1, 2, 0));

  EXPECT_EQ(aTriangulation.Size(), 1);

  BVH_Box<Standard_ShortReal, 3> aBox = aTriangulation.Box(0);
  EXPECT_NEAR(aBox.CornerMax().x(), 1.0f, 1e-6f);
  EXPECT_NEAR(aBox.CornerMax().y(), 1.0f, 1e-6f);
}

// =============================================================================
// BVH_Triangulation Negative Coordinates Tests
// =============================================================================

TEST(BVH_TriangulationTest, NegativeCoordinates)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(-1.0, -2.0, -3.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(1.0, -1.0, -2.0));
  BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(0.0, 0.0, -1.0));
  BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements, BVH_Vec4i(0, 1, 2, 0));

  BVH_Box<Standard_Real, 3> aBox = aTriangulation.Box(0);

  EXPECT_NEAR(aBox.CornerMin().x(), -1.0, 1e-10);
  EXPECT_NEAR(aBox.CornerMin().y(), -2.0, 1e-10);
  EXPECT_NEAR(aBox.CornerMin().z(), -3.0, 1e-10);
  EXPECT_NEAR(aBox.CornerMax().x(), 1.0, 1e-10);
  EXPECT_NEAR(aBox.CornerMax().y(), 0.0, 1e-10);
  EXPECT_NEAR(aBox.CornerMax().z(), -1.0, 1e-10);
}
