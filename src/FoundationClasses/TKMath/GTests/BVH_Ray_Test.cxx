// Created on: 2025-01-20
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

#include <BVH_Ray.hxx>

#include <gtest/gtest.h>

// =======================================================================================
// Tests for BVH_Ray
// =======================================================================================

TEST(BVH_RayTest, ConstructorBasic)
{
  BVH_Vec3d aOrigin(1.0, 2.0, 3.0);
  BVH_Vec3d aDirection(0.0, 1.0, 0.0);

  BVH_Ray<Standard_Real, 3> aRay(aOrigin, aDirection);

  EXPECT_EQ(aRay.Origin.x(), 1.0);
  EXPECT_EQ(aRay.Origin.y(), 2.0);
  EXPECT_EQ(aRay.Origin.z(), 3.0);

  EXPECT_EQ(aRay.Direct.x(), 0.0);
  EXPECT_EQ(aRay.Direct.y(), 1.0);
  EXPECT_EQ(aRay.Direct.z(), 0.0);
}

TEST(BVH_RayTest, DirectionStorage)
{
  BVH_Vec3d aOrigin(0.0, 0.0, 0.0);
  BVH_Vec3d aDirection(2.0, 4.0, 8.0);

  BVH_Ray<Standard_Real, 3> aRay(aOrigin, aDirection);

  // Verify direction is stored correctly
  EXPECT_EQ(aRay.Direct.x(), 2.0);
  EXPECT_EQ(aRay.Direct.y(), 4.0);
  EXPECT_EQ(aRay.Direct.z(), 8.0);
}

TEST(BVH_RayTest, NegativeDirection)
{
  BVH_Vec3d aOrigin(0.0, 0.0, 0.0);
  BVH_Vec3d aDirection(-2.0, -4.0, -1.0);

  BVH_Ray<Standard_Real, 3> aRay(aOrigin, aDirection);

  EXPECT_EQ(aRay.Direct.x(), -2.0);
  EXPECT_EQ(aRay.Direct.y(), -4.0);
  EXPECT_EQ(aRay.Direct.z(), -1.0);
}

TEST(BVH_RayTest, ZeroComponent)
{
  BVH_Vec3d aOrigin(0.0, 0.0, 0.0);
  BVH_Vec3d aDirection(1.0, 0.0, 1.0); // Zero Y component

  BVH_Ray<Standard_Real, 3> aRay(aOrigin, aDirection);

  EXPECT_EQ(aRay.Direct.x(), 1.0);
  EXPECT_EQ(aRay.Direct.y(), 0.0);
  EXPECT_EQ(aRay.Direct.z(), 1.0);
}

TEST(BVH_RayTest, AllZeroDirection)
{
  BVH_Vec3d aOrigin(0.0, 0.0, 0.0);
  BVH_Vec3d aDirection(0.0, 0.0, 0.0);

  BVH_Ray<Standard_Real, 3> aRay(aOrigin, aDirection);

  // All components should be zero
  EXPECT_EQ(aRay.Direct.x(), 0.0);
  EXPECT_EQ(aRay.Direct.y(), 0.0);
  EXPECT_EQ(aRay.Direct.z(), 0.0);
}

TEST(BVH_RayTest, DefaultConstructor)
{
  BVH_Ray<Standard_Real, 3> aRay;

  // Default constructed ray should have zero origin and direction
  EXPECT_EQ(aRay.Origin.x(), 0.0);
  EXPECT_EQ(aRay.Origin.y(), 0.0);
  EXPECT_EQ(aRay.Origin.z(), 0.0);

  EXPECT_EQ(aRay.Direct.x(), 0.0);
  EXPECT_EQ(aRay.Direct.y(), 0.0);
  EXPECT_EQ(aRay.Direct.z(), 0.0);
}

TEST(BVH_RayTest, Ray2D)
{
  BVH_Vec2d aOrigin(1.0, 2.0);
  BVH_Vec2d aDirection(3.0, 4.0);

  BVH_Ray<Standard_Real, 2> aRay(aOrigin, aDirection);

  EXPECT_EQ(aRay.Origin.x(), 1.0);
  EXPECT_EQ(aRay.Origin.y(), 2.0);

  EXPECT_EQ(aRay.Direct.x(), 3.0);
  EXPECT_EQ(aRay.Direct.y(), 4.0);
}

TEST(BVH_RayTest, NormalizedDirection)
{
  BVH_Vec3d aOrigin(0.0, 0.0, 0.0);
  BVH_Vec3d aDirection(1.0, 0.0, 0.0); // Unit vector along X

  BVH_Ray<Standard_Real, 3> aRay(aOrigin, aDirection);

  EXPECT_EQ(aRay.Direct.x(), 1.0);
  EXPECT_EQ(aRay.Direct.y(), 0.0);
  EXPECT_EQ(aRay.Direct.z(), 0.0);
}

TEST(BVH_RayTest, VerySmallDirection)
{
  BVH_Vec3d aOrigin(0.0, 0.0, 0.0);
  BVH_Vec3d aDirection(1e-20, 1e-20, 1e-20);

  BVH_Ray<Standard_Real, 3> aRay(aOrigin, aDirection);

  // Should handle very small values correctly
  EXPECT_EQ(aRay.Direct.x(), 1e-20);
  EXPECT_EQ(aRay.Direct.y(), 1e-20);
  EXPECT_EQ(aRay.Direct.z(), 1e-20);
}

TEST(BVH_RayTest, MixedZeroNonZero)
{
  BVH_Vec3d aOrigin(1.0, 2.0, 3.0);
  BVH_Vec3d aDirection(0.0, 2.0, 0.0);

  BVH_Ray<Standard_Real, 3> aRay(aOrigin, aDirection);

  EXPECT_EQ(aRay.Direct.x(), 0.0);
  EXPECT_EQ(aRay.Direct.y(), 2.0);
  EXPECT_EQ(aRay.Direct.z(), 0.0);
}

TEST(BVH_RayTest, FloatPrecision)
{
  BVH_Vec3f aOrigin(1.0f, 2.0f, 3.0f);
  BVH_Vec3f aDirection(2.0f, 4.0f, 8.0f);

  BVH_Ray<Standard_ShortReal, 3> aRay(aOrigin, aDirection);

  EXPECT_EQ(aRay.Direct.x(), 2.0f);
  EXPECT_EQ(aRay.Direct.y(), 4.0f);
  EXPECT_EQ(aRay.Direct.z(), 8.0f);
}

TEST(BVH_RayTest, ConstexprConstructor)
{
  // This test verifies that the constructor is truly constexpr
  constexpr BVH_Vec3d                 aOrigin(1.0, 2.0, 3.0);
  constexpr BVH_Vec3d                 aDirection(1.0, 1.0, 1.0);
  constexpr BVH_Ray<Standard_Real, 3> aRay(aOrigin, aDirection);

  // If this compiles, constexpr works
  EXPECT_EQ(aRay.Origin.x(), 1.0);
}

TEST(BVH_RayTest, DiagonalRay)
{
  BVH_Vec3d aOrigin(0.0, 0.0, 0.0);
  BVH_Vec3d aDirection(1.0, 1.0, 1.0);

  BVH_Ray<Standard_Real, 3> aRay(aOrigin, aDirection);

  EXPECT_EQ(aRay.Direct.x(), 1.0);
  EXPECT_EQ(aRay.Direct.y(), 1.0);
  EXPECT_EQ(aRay.Direct.z(), 1.0);
}

TEST(BVH_RayTest, NegativeOrigin)
{
  BVH_Vec3d aOrigin(-10.0, -20.0, -30.0);
  BVH_Vec3d aDirection(1.0, 2.0, 3.0);

  BVH_Ray<Standard_Real, 3> aRay(aOrigin, aDirection);

  EXPECT_EQ(aRay.Origin.x(), -10.0);
  EXPECT_EQ(aRay.Origin.y(), -20.0);
  EXPECT_EQ(aRay.Origin.z(), -30.0);

  EXPECT_EQ(aRay.Direct.x(), 1.0);
  EXPECT_EQ(aRay.Direct.y(), 2.0);
  EXPECT_EQ(aRay.Direct.z(), 3.0);
}

TEST(BVH_RayTest, AxisAlignedX)
{
  BVH_Vec3d aOrigin(0.0, 0.0, 0.0);
  BVH_Vec3d aDirection(1.0, 0.0, 0.0);

  BVH_Ray<Standard_Real, 3> aRay(aOrigin, aDirection);

  EXPECT_EQ(aRay.Direct.x(), 1.0);
  EXPECT_EQ(aRay.Direct.y(), 0.0);
  EXPECT_EQ(aRay.Direct.z(), 0.0);
}

TEST(BVH_RayTest, AxisAlignedY)
{
  BVH_Vec3d aOrigin(0.0, 0.0, 0.0);
  BVH_Vec3d aDirection(0.0, 1.0, 0.0);

  BVH_Ray<Standard_Real, 3> aRay(aOrigin, aDirection);

  EXPECT_EQ(aRay.Direct.x(), 0.0);
  EXPECT_EQ(aRay.Direct.y(), 1.0);
  EXPECT_EQ(aRay.Direct.z(), 0.0);
}

TEST(BVH_RayTest, AxisAlignedZ)
{
  BVH_Vec3d aOrigin(0.0, 0.0, 0.0);
  BVH_Vec3d aDirection(0.0, 0.0, 1.0);

  BVH_Ray<Standard_Real, 3> aRay(aOrigin, aDirection);

  EXPECT_EQ(aRay.Direct.x(), 0.0);
  EXPECT_EQ(aRay.Direct.y(), 0.0);
  EXPECT_EQ(aRay.Direct.z(), 1.0);
}

TEST(BVH_RayTest, LargeValues)
{
  BVH_Vec3d aOrigin(1e6, 2e6, 3e6);
  BVH_Vec3d aDirection(1e3, 2e3, 4e3);

  BVH_Ray<Standard_Real, 3> aRay(aOrigin, aDirection);

  EXPECT_EQ(aRay.Direct.x(), 1e3);
  EXPECT_EQ(aRay.Direct.y(), 2e3);
  EXPECT_EQ(aRay.Direct.z(), 4e3);
}

TEST(BVH_RayTest, OriginAndDirectionPreservation)
{
  BVH_Vec3d aOrigin(1.0, 2.0, 3.0);
  BVH_Vec3d aDirection(2.0, 4.0, 8.0);

  BVH_Ray<Standard_Real, 3> aRay(aOrigin, aDirection);

  // Verify that both origin and direction are preserved exactly
  EXPECT_EQ(aRay.Origin.x(), aOrigin.x());
  EXPECT_EQ(aRay.Origin.y(), aOrigin.y());
  EXPECT_EQ(aRay.Origin.z(), aOrigin.z());

  EXPECT_EQ(aRay.Direct.x(), aDirection.x());
  EXPECT_EQ(aRay.Direct.y(), aDirection.y());
  EXPECT_EQ(aRay.Direct.z(), aDirection.z());
}
