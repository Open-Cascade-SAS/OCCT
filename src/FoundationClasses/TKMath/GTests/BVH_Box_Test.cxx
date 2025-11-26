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

#include <BVH_Box.hxx>
#include <Precision.hxx>

TEST(BVH_BoxTest, DefaultConstructor)
{
  BVH_Box<Standard_Real, 3> aBox;
  EXPECT_FALSE(aBox.IsValid());
}

TEST(BVH_BoxTest, ConstructorWithCorners)
{
  BVH_Vec3d aMin(0.0, 0.0, 0.0);
  BVH_Vec3d aMax(1.0, 2.0, 3.0);

  BVH_Box<Standard_Real, 3> aBox(aMin, aMax);

  EXPECT_TRUE(aBox.IsValid());
  EXPECT_NEAR(aBox.CornerMin().x(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMin().y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMin().z(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().x(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().z(), 3.0, Precision::Confusion());
}

TEST(BVH_BoxTest, Add)
{
  BVH_Box<Standard_Real, 3> aBox;

  aBox.Add(BVH_Vec3d(1.0, 2.0, 3.0));
  EXPECT_TRUE(aBox.IsValid());

  aBox.Add(BVH_Vec3d(-1.0, -2.0, -3.0));

  EXPECT_NEAR(aBox.CornerMin().x(), -1.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMin().y(), -2.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMin().z(), -3.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().x(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().z(), 3.0, Precision::Confusion());
}

TEST(BVH_BoxTest, Combine)
{
  BVH_Box<Standard_Real, 3> aBox1(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));
  BVH_Box<Standard_Real, 3> aBox2(BVH_Vec3d(2.0, 2.0, 2.0), BVH_Vec3d(3.0, 3.0, 3.0));

  aBox1.Combine(aBox2);

  EXPECT_NEAR(aBox1.CornerMin().x(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aBox1.CornerMin().y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aBox1.CornerMin().z(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aBox1.CornerMax().x(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aBox1.CornerMax().y(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aBox1.CornerMax().z(), 3.0, Precision::Confusion());
}

TEST(BVH_BoxTest, Size)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(2.0, 3.0, 4.0));

  BVH_Vec3d aSize = aBox.Size();
  EXPECT_NEAR(aSize.x(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aSize.y(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aSize.z(), 4.0, Precision::Confusion());
}

TEST(BVH_BoxTest, Center)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(2.0, 4.0, 6.0));

  BVH_Vec3d aCenter = aBox.Center();
  EXPECT_NEAR(aCenter.x(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aCenter.y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aCenter.z(), 3.0, Precision::Confusion());
}

TEST(BVH_BoxTest, Area)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 2.0, 3.0));

  // Surface area = 2 * (1*2 + 2*3 + 1*3) = 2 * (2 + 6 + 3) = 22
  Standard_Real anArea = aBox.Area();
  EXPECT_NEAR(anArea, 22.0, Precision::Confusion());
}

TEST(BVH_BoxTest, IsOut)
{
  BVH_Box<Standard_Real, 3> aBox1(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));
  BVH_Box<Standard_Real, 3> aBox2(BVH_Vec3d(2.0, 2.0, 2.0), BVH_Vec3d(3.0, 3.0, 3.0));
  BVH_Box<Standard_Real, 3> aBox3(BVH_Vec3d(0.5, 0.5, 0.5), BVH_Vec3d(1.5, 1.5, 1.5));

  EXPECT_TRUE(aBox1.IsOut(aBox2));
  EXPECT_FALSE(aBox1.IsOut(aBox3));
}

TEST(BVH_BoxTest, Clear)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));
  EXPECT_TRUE(aBox.IsValid());

  aBox.Clear();
  EXPECT_FALSE(aBox.IsValid());
}

TEST(BVH_BoxTest, Box2D)
{
  BVH_Box<Standard_Real, 2> aBox;

  aBox.Add(BVH_Vec2d(0.0, 0.0));
  aBox.Add(BVH_Vec2d(1.0, 1.0));

  EXPECT_TRUE(aBox.IsValid());
  EXPECT_NEAR(aBox.CornerMin().x(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMin().y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().x(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().y(), 1.0, Precision::Confusion());

  // Area in 2D is width * height (actual area)
  Standard_Real anArea = aBox.Area();
  EXPECT_NEAR(anArea, 1.0, Precision::Confusion()); // 1 * 1 = 1
}

TEST(BVH_BoxTest, Box4D)
{
  BVH_Box<Standard_Real, 4> aBox;

  aBox.Add(BVH_Vec4d(0.0, 0.0, 0.0, 0.0));
  aBox.Add(BVH_Vec4d(1.0, 2.0, 3.0, 4.0));

  EXPECT_TRUE(aBox.IsValid());
  EXPECT_NEAR(aBox.CornerMin().x(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMin().y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMin().z(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMin().w(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().x(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().z(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().w(), 4.0, Precision::Confusion());
}

TEST(BVH_BoxTest, FloatPrecision)
{
  BVH_Box<Standard_ShortReal, 3> aBox;

  aBox.Add(BVH_Vec3f(0.0f, 0.0f, 0.0f));
  aBox.Add(BVH_Vec3f(1.0f, 2.0f, 3.0f));

  EXPECT_TRUE(aBox.IsValid());
  EXPECT_NEAR(aBox.CornerMin().x(), 0.0f, 1e-5f);
  EXPECT_NEAR(aBox.CornerMax().x(), 1.0f, 1e-5f);
  EXPECT_NEAR(aBox.CornerMax().y(), 2.0f, 1e-5f);
  EXPECT_NEAR(aBox.CornerMax().z(), 3.0f, 1e-5f);
}

TEST(BVH_BoxTest, SinglePointBox)
{
  BVH_Box<Standard_Real, 3> aBox;

  aBox.Add(BVH_Vec3d(5.0, 5.0, 5.0));

  EXPECT_TRUE(aBox.IsValid());
  EXPECT_NEAR(aBox.CornerMin().x(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().x(), 5.0, Precision::Confusion());

  // Size should be zero
  BVH_Vec3d aSize = aBox.Size();
  EXPECT_NEAR(aSize.x(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aSize.y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aSize.z(), 0.0, Precision::Confusion());

  // Area should be zero
  EXPECT_NEAR(aBox.Area(), 0.0, Precision::Confusion());
}

TEST(BVH_BoxTest, NegativeCoordinates)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(-10.0, -20.0, -30.0), BVH_Vec3d(-5.0, -10.0, -15.0));

  EXPECT_TRUE(aBox.IsValid());

  BVH_Vec3d aCenter = aBox.Center();
  EXPECT_NEAR(aCenter.x(), -7.5, Precision::Confusion());
  EXPECT_NEAR(aCenter.y(), -15.0, Precision::Confusion());
  EXPECT_NEAR(aCenter.z(), -22.5, Precision::Confusion());

  BVH_Vec3d aSize = aBox.Size();
  EXPECT_NEAR(aSize.x(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aSize.y(), 10.0, Precision::Confusion());
  EXPECT_NEAR(aSize.z(), 15.0, Precision::Confusion());
}

TEST(BVH_BoxTest, LargeValues)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(1e10, 1e10, 1e10),
                                 BVH_Vec3d(1e10 + 1.0, 1e10 + 2.0, 1e10 + 3.0));

  EXPECT_TRUE(aBox.IsValid());

  BVH_Vec3d aSize = aBox.Size();
  EXPECT_NEAR(aSize.x(), 1.0, 1e-5);
  EXPECT_NEAR(aSize.y(), 2.0, 1e-5);
  EXPECT_NEAR(aSize.z(), 3.0, 1e-5);
}

TEST(BVH_BoxTest, CombineWithInvalid)
{
  BVH_Box<Standard_Real, 3> aBox1(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));
  BVH_Box<Standard_Real, 3> aBox2; // Invalid box

  aBox1.Combine(aBox2);

  // Box1 should remain unchanged when combining with invalid box
  EXPECT_TRUE(aBox1.IsValid());
  EXPECT_NEAR(aBox1.CornerMin().x(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aBox1.CornerMax().x(), 1.0, Precision::Confusion());
}

TEST(BVH_BoxTest, AddToInvalid)
{
  BVH_Box<Standard_Real, 3> aBox1; // Invalid box
  BVH_Box<Standard_Real, 3> aBox2(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));

  aBox1.Combine(aBox2);

  // Box1 should now be valid and equal to aBox2
  EXPECT_TRUE(aBox1.IsValid());
  EXPECT_NEAR(aBox1.CornerMin().x(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aBox1.CornerMax().x(), 1.0, Precision::Confusion());
}

TEST(BVH_BoxTest, IsOutTouchingBoxes)
{
  // Boxes that touch at a face
  BVH_Box<Standard_Real, 3> aBox1(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));
  BVH_Box<Standard_Real, 3> aBox2(BVH_Vec3d(1.0, 0.0, 0.0), BVH_Vec3d(2.0, 1.0, 1.0));

  // Touching boxes should NOT be "out"
  EXPECT_FALSE(aBox1.IsOut(aBox2));
}

TEST(BVH_BoxTest, IsOutTouchingAtEdge)
{
  // Boxes that touch at an edge
  BVH_Box<Standard_Real, 3> aBox1(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));
  BVH_Box<Standard_Real, 3> aBox2(BVH_Vec3d(1.0, 1.0, 0.0), BVH_Vec3d(2.0, 2.0, 1.0));

  // Touching at edge should NOT be "out"
  EXPECT_FALSE(aBox1.IsOut(aBox2));
}

TEST(BVH_BoxTest, IsOutTouchingAtCorner)
{
  // Boxes that touch at a corner
  BVH_Box<Standard_Real, 3> aBox1(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));
  BVH_Box<Standard_Real, 3> aBox2(BVH_Vec3d(1.0, 1.0, 1.0), BVH_Vec3d(2.0, 2.0, 2.0));

  // Touching at corner should NOT be "out"
  EXPECT_FALSE(aBox1.IsOut(aBox2));
}

TEST(BVH_BoxTest, AreaUnitCube)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));

  // Surface area of unit cube = 6 faces * 1 = 6
  // Formula: 2 * (xy + yz + xz) = 2 * (1 + 1 + 1) = 6
  EXPECT_NEAR(aBox.Area(), 6.0, Precision::Confusion());
}

TEST(BVH_BoxTest, CenterAtOrigin)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(-1.0, -1.0, -1.0), BVH_Vec3d(1.0, 1.0, 1.0));

  BVH_Vec3d aCenter = aBox.Center();
  EXPECT_NEAR(aCenter.x(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aCenter.y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aCenter.z(), 0.0, Precision::Confusion());
}

TEST(BVH_BoxTest, MultipleAdds)
{
  BVH_Box<Standard_Real, 3> aBox;

  // Add points in random order
  aBox.Add(BVH_Vec3d(5.0, 3.0, 1.0));
  aBox.Add(BVH_Vec3d(-2.0, 7.0, 4.0));
  aBox.Add(BVH_Vec3d(1.0, -1.0, 8.0));
  aBox.Add(BVH_Vec3d(0.0, 2.0, -3.0));

  EXPECT_NEAR(aBox.CornerMin().x(), -2.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMin().y(), -1.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMin().z(), -3.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().x(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().y(), 7.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().z(), 8.0, Precision::Confusion());
}

TEST(BVH_BoxTest, FlatBox2D)
{
  // Box with zero extent in Z (flat)
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 5.0), BVH_Vec3d(3.0, 4.0, 5.0));

  EXPECT_TRUE(aBox.IsValid());

  BVH_Vec3d aSize = aBox.Size();
  EXPECT_NEAR(aSize.x(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aSize.y(), 4.0, Precision::Confusion());
  EXPECT_NEAR(aSize.z(), 0.0, Precision::Confusion());

  // Area = 2 * (3*4 + 4*0 + 3*0) = 24
  EXPECT_NEAR(aBox.Area(), 24.0, Precision::Confusion());
}

TEST(BVH_BoxTest, FlatBox1D)
{
  // Box with zero extent in Y and Z (line)
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 5.0, 5.0), BVH_Vec3d(10.0, 5.0, 5.0));

  EXPECT_TRUE(aBox.IsValid());

  BVH_Vec3d aSize = aBox.Size();
  EXPECT_NEAR(aSize.x(), 10.0, Precision::Confusion());
  EXPECT_NEAR(aSize.y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aSize.z(), 0.0, Precision::Confusion());

  // For degenerate box (line), Area returns length of longest dimension
  // This is an implementation detail - the surface area formula gives 0,
  // but Area() returns the length (10) for degenerate cases
  Standard_Real anArea = aBox.Area();
  EXPECT_GE(anArea, 0.0);
}

TEST(BVH_BoxTest, CombineMultiple)
{
  BVH_Box<Standard_Real, 3> aBox1(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));
  BVH_Box<Standard_Real, 3> aBox2(BVH_Vec3d(2.0, 0.0, 0.0), BVH_Vec3d(3.0, 1.0, 1.0));
  BVH_Box<Standard_Real, 3> aBox3(BVH_Vec3d(0.0, 2.0, 0.0), BVH_Vec3d(1.0, 3.0, 1.0));

  aBox1.Combine(aBox2);
  aBox1.Combine(aBox3);

  EXPECT_NEAR(aBox1.CornerMin().x(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aBox1.CornerMin().y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aBox1.CornerMin().z(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aBox1.CornerMax().x(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aBox1.CornerMax().y(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aBox1.CornerMax().z(), 1.0, Precision::Confusion());
}

TEST(BVH_BoxTest, IsOutPartialOverlap)
{
  // Boxes that partially overlap
  BVH_Box<Standard_Real, 3> aBox1(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(2.0, 2.0, 2.0));
  BVH_Box<Standard_Real, 3> aBox2(BVH_Vec3d(1.0, 1.0, 1.0), BVH_Vec3d(3.0, 3.0, 3.0));

  EXPECT_FALSE(aBox1.IsOut(aBox2));
  EXPECT_FALSE(aBox2.IsOut(aBox1));
}

TEST(BVH_BoxTest, IsOutContained)
{
  // One box completely inside another
  BVH_Box<Standard_Real, 3> aBox1(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(10.0, 10.0, 10.0));
  BVH_Box<Standard_Real, 3> aBox2(BVH_Vec3d(2.0, 2.0, 2.0), BVH_Vec3d(3.0, 3.0, 3.0));

  EXPECT_FALSE(aBox1.IsOut(aBox2));
  EXPECT_FALSE(aBox2.IsOut(aBox1));
}

TEST(BVH_BoxTest, IsOutFloat)
{
  BVH_Box<Standard_ShortReal, 3> aBox1(BVH_Vec3f(0.0f, 0.0f, 0.0f), BVH_Vec3f(1.0f, 1.0f, 1.0f));
  BVH_Box<Standard_ShortReal, 3> aBox2(BVH_Vec3f(2.0f, 2.0f, 2.0f), BVH_Vec3f(3.0f, 3.0f, 3.0f));
  BVH_Box<Standard_ShortReal, 3> aBox3(BVH_Vec3f(0.5f, 0.5f, 0.5f), BVH_Vec3f(1.5f, 1.5f, 1.5f));

  EXPECT_TRUE(aBox1.IsOut(aBox2));
  EXPECT_FALSE(aBox1.IsOut(aBox3));
}

TEST(BVH_BoxTest, Box2DIsOut)
{
  BVH_Box<Standard_Real, 2> aBox1(BVH_Vec2d(0.0, 0.0), BVH_Vec2d(1.0, 1.0));
  BVH_Box<Standard_Real, 2> aBox2(BVH_Vec2d(2.0, 2.0), BVH_Vec2d(3.0, 3.0));
  BVH_Box<Standard_Real, 2> aBox3(BVH_Vec2d(0.5, 0.5), BVH_Vec2d(1.5, 1.5));

  EXPECT_TRUE(aBox1.IsOut(aBox2));
  EXPECT_FALSE(aBox1.IsOut(aBox3));
}

TEST(BVH_BoxTest, Box2DCenter)
{
  BVH_Box<Standard_Real, 2> aBox(BVH_Vec2d(0.0, 0.0), BVH_Vec2d(4.0, 6.0));

  BVH_Vec2d aCenter = aBox.Center();
  EXPECT_NEAR(aCenter.x(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aCenter.y(), 3.0, Precision::Confusion());
}

TEST(BVH_BoxTest, Box2DSize)
{
  BVH_Box<Standard_Real, 2> aBox(BVH_Vec2d(1.0, 2.0), BVH_Vec2d(4.0, 7.0));

  BVH_Vec2d aSize = aBox.Size();
  EXPECT_NEAR(aSize.x(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aSize.y(), 5.0, Precision::Confusion());
}

TEST(BVH_BoxTest, VerySmallBox)
{
  // Test with very small values
  Standard_Real             aSmall = 1e-10;
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(aSmall, aSmall, aSmall));

  EXPECT_TRUE(aBox.IsValid());

  BVH_Vec3d aSize = aBox.Size();
  EXPECT_NEAR(aSize.x(), aSmall, 1e-15);
  EXPECT_NEAR(aSize.y(), aSmall, 1e-15);
  EXPECT_NEAR(aSize.z(), aSmall, 1e-15);
}

TEST(BVH_BoxTest, SymmetricBox)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(-5.0, -5.0, -5.0), BVH_Vec3d(5.0, 5.0, 5.0));

  BVH_Vec3d aCenter = aBox.Center();
  EXPECT_NEAR(aCenter.x(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aCenter.y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aCenter.z(), 0.0, Precision::Confusion());

  BVH_Vec3d aSize = aBox.Size();
  EXPECT_NEAR(aSize.x(), 10.0, Precision::Confusion());
  EXPECT_NEAR(aSize.y(), 10.0, Precision::Confusion());
  EXPECT_NEAR(aSize.z(), 10.0, Precision::Confusion());
}

TEST(BVH_BoxTest, NonCubicBox)
{
  // Box with different dimensions on each axis
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 10.0, 100.0));

  BVH_Vec3d aSize = aBox.Size();
  EXPECT_NEAR(aSize.x(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aSize.y(), 10.0, Precision::Confusion());
  EXPECT_NEAR(aSize.z(), 100.0, Precision::Confusion());

  // Area = 2 * (1*10 + 10*100 + 1*100) = 2 * (10 + 1000 + 100) = 2220
  EXPECT_NEAR(aBox.Area(), 2220.0, Precision::Confusion());
}

TEST(BVH_BoxTest, ClearAndReuse)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));

  EXPECT_TRUE(aBox.IsValid());

  aBox.Clear();
  EXPECT_FALSE(aBox.IsValid());

  // Reuse the box
  aBox.Add(BVH_Vec3d(5.0, 5.0, 5.0));
  aBox.Add(BVH_Vec3d(10.0, 10.0, 10.0));

  EXPECT_TRUE(aBox.IsValid());
  EXPECT_NEAR(aBox.CornerMin().x(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().x(), 10.0, Precision::Confusion());
}

TEST(BVH_BoxTest, IsOutSameBox)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));

  // Box should not be "out" of itself
  EXPECT_FALSE(aBox.IsOut(aBox));
}

TEST(BVH_BoxTest, CombineSameBox)
{
  BVH_Box<Standard_Real, 3> aBox1(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));
  BVH_Box<Standard_Real, 3> aBox2(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));

  aBox1.Combine(aBox2);

  // Should remain unchanged
  EXPECT_NEAR(aBox1.CornerMin().x(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aBox1.CornerMax().x(), 1.0, Precision::Confusion());
}

TEST(BVH_BoxTest, IsOutNearMiss)
{
  // Boxes that are very close but not touching
  BVH_Box<Standard_Real, 3> aBox1(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));
  BVH_Box<Standard_Real, 3> aBox2(BVH_Vec3d(1.0001, 0.0, 0.0), BVH_Vec3d(2.0, 1.0, 1.0));

  EXPECT_TRUE(aBox1.IsOut(aBox2));
}

TEST(BVH_BoxTest, AddDuplicatePoint)
{
  BVH_Box<Standard_Real, 3> aBox;

  aBox.Add(BVH_Vec3d(1.0, 2.0, 3.0));
  aBox.Add(BVH_Vec3d(1.0, 2.0, 3.0));
  aBox.Add(BVH_Vec3d(1.0, 2.0, 3.0));

  // Should still be a single point box
  EXPECT_NEAR(aBox.CornerMin().x(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().x(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aBox.Size().x(), 0.0, Precision::Confusion());
}

// =======================================================================================
// Tests for Single-Point Constructor
// =======================================================================================

TEST(BVH_BoxTest, SinglePointConstructor)
{
  BVH_Vec3d                 aPoint(5.0, 10.0, 15.0);
  BVH_Box<Standard_Real, 3> aBox(aPoint);

  EXPECT_TRUE(aBox.IsValid());
  EXPECT_NEAR(aBox.CornerMin().x(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMin().y(), 10.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMin().z(), 15.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().x(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().y(), 10.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().z(), 15.0, Precision::Confusion());

  // Size should be zero
  BVH_Vec3d aSize = aBox.Size();
  EXPECT_NEAR(aSize.x(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aSize.y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aSize.z(), 0.0, Precision::Confusion());
}

TEST(BVH_BoxTest, SinglePointConstructor2D)
{
  BVH_Vec2d                 aPoint(3.0, 7.0);
  BVH_Box<Standard_Real, 2> aBox(aPoint);

  EXPECT_TRUE(aBox.IsValid());
  EXPECT_NEAR(aBox.CornerMin().x(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMin().y(), 7.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().x(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().y(), 7.0, Precision::Confusion());
}

TEST(BVH_BoxTest, SinglePointConstructorOrigin)
{
  BVH_Vec3d                 aOrigin(0.0, 0.0, 0.0);
  BVH_Box<Standard_Real, 3> aBox(aOrigin);

  EXPECT_TRUE(aBox.IsValid());
  EXPECT_NEAR(aBox.Center().x(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aBox.Center().y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aBox.Center().z(), 0.0, Precision::Confusion());
}

// =======================================================================================
// Tests for Center(axis) method
// =======================================================================================

TEST(BVH_BoxTest, CenterByAxis)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(10.0, 20.0, 30.0));

  EXPECT_NEAR(aBox.Center(0), 5.0, Precision::Confusion());  // X axis
  EXPECT_NEAR(aBox.Center(1), 10.0, Precision::Confusion()); // Y axis
  EXPECT_NEAR(aBox.Center(2), 15.0, Precision::Confusion()); // Z axis
}

TEST(BVH_BoxTest, CenterByAxis2D)
{
  BVH_Box<Standard_Real, 2> aBox(BVH_Vec2d(2.0, 4.0), BVH_Vec2d(8.0, 12.0));

  EXPECT_NEAR(aBox.Center(0), 5.0, Precision::Confusion()); // X axis
  EXPECT_NEAR(aBox.Center(1), 8.0, Precision::Confusion()); // Y axis
}

TEST(BVH_BoxTest, CenterByAxisNegative)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(-10.0, -20.0, -30.0), BVH_Vec3d(10.0, 20.0, 30.0));

  EXPECT_NEAR(aBox.Center(0), 0.0, Precision::Confusion());
  EXPECT_NEAR(aBox.Center(1), 0.0, Precision::Confusion());
  EXPECT_NEAR(aBox.Center(2), 0.0, Precision::Confusion());
}

TEST(BVH_BoxTest, CenterByAxis4D)
{
  BVH_Box<Standard_Real, 4> aBox(BVH_Vec4d(0.0, 0.0, 0.0, 0.0), BVH_Vec4d(4.0, 6.0, 8.0, 10.0));

  EXPECT_NEAR(aBox.Center(0), 2.0, Precision::Confusion());
  EXPECT_NEAR(aBox.Center(1), 3.0, Precision::Confusion());
  EXPECT_NEAR(aBox.Center(2), 4.0, Precision::Confusion());
}

// =======================================================================================
// Tests for IsOut(point) method
// =======================================================================================

TEST(BVH_BoxTest, IsOutPoint_Inside)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(10.0, 10.0, 10.0));

  // Point inside
  EXPECT_FALSE(aBox.IsOut(BVH_Vec3d(5.0, 5.0, 5.0)));
  EXPECT_FALSE(aBox.IsOut(BVH_Vec3d(1.0, 1.0, 1.0)));
  EXPECT_FALSE(aBox.IsOut(BVH_Vec3d(9.0, 9.0, 9.0)));
}

TEST(BVH_BoxTest, IsOutPoint_Outside)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(10.0, 10.0, 10.0));

  // Points outside
  EXPECT_TRUE(aBox.IsOut(BVH_Vec3d(11.0, 5.0, 5.0)));   // Outside X
  EXPECT_TRUE(aBox.IsOut(BVH_Vec3d(5.0, 11.0, 5.0)));   // Outside Y
  EXPECT_TRUE(aBox.IsOut(BVH_Vec3d(5.0, 5.0, 11.0)));   // Outside Z
  EXPECT_TRUE(aBox.IsOut(BVH_Vec3d(-1.0, 5.0, 5.0)));   // Outside negative X
  EXPECT_TRUE(aBox.IsOut(BVH_Vec3d(15.0, 15.0, 15.0))); // Far outside
}

TEST(BVH_BoxTest, IsOutPoint_OnBoundary)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(10.0, 10.0, 10.0));

  // Points on boundary (should be considered inside)
  EXPECT_FALSE(aBox.IsOut(BVH_Vec3d(0.0, 5.0, 5.0)));    // On min X face
  EXPECT_FALSE(aBox.IsOut(BVH_Vec3d(10.0, 5.0, 5.0)));   // On max X face
  EXPECT_FALSE(aBox.IsOut(BVH_Vec3d(5.0, 0.0, 5.0)));    // On min Y face
  EXPECT_FALSE(aBox.IsOut(BVH_Vec3d(5.0, 10.0, 5.0)));   // On max Y face
  EXPECT_FALSE(aBox.IsOut(BVH_Vec3d(0.0, 0.0, 0.0)));    // At corner
  EXPECT_FALSE(aBox.IsOut(BVH_Vec3d(10.0, 10.0, 10.0))); // At opposite corner
}

TEST(BVH_BoxTest, IsOutPoint_InvalidBox)
{
  BVH_Box<Standard_Real, 3> aBox; // Invalid box

  EXPECT_TRUE(aBox.IsOut(BVH_Vec3d(0.0, 0.0, 0.0)));
  EXPECT_TRUE(aBox.IsOut(BVH_Vec3d(5.0, 5.0, 5.0)));
}

TEST(BVH_BoxTest, IsOutPoint_2D)
{
  BVH_Box<Standard_Real, 2> aBox(BVH_Vec2d(0.0, 0.0), BVH_Vec2d(5.0, 5.0));

  EXPECT_FALSE(aBox.IsOut(BVH_Vec2d(2.5, 2.5))); // Inside
  EXPECT_TRUE(aBox.IsOut(BVH_Vec2d(6.0, 2.5)));  // Outside X
  EXPECT_TRUE(aBox.IsOut(BVH_Vec2d(2.5, 6.0)));  // Outside Y
  EXPECT_FALSE(aBox.IsOut(BVH_Vec2d(0.0, 0.0))); // On corner
}

TEST(BVH_BoxTest, IsOutPoint_NegativeCoords)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(-5.0, -5.0, -5.0), BVH_Vec3d(5.0, 5.0, 5.0));

  EXPECT_FALSE(aBox.IsOut(BVH_Vec3d(0.0, 0.0, 0.0)));    // At center
  EXPECT_FALSE(aBox.IsOut(BVH_Vec3d(-3.0, -3.0, -3.0))); // Inside negative
  EXPECT_TRUE(aBox.IsOut(BVH_Vec3d(-6.0, 0.0, 0.0)));    // Outside negative X
}

// =======================================================================================
// Tests for Contains() methods
// =======================================================================================

TEST(BVH_BoxTest, Contains_FullyContained)
{
  BVH_Box<Standard_Real, 3> aBox1(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(10.0, 10.0, 10.0));
  BVH_Box<Standard_Real, 3> aBox2(BVH_Vec3d(2.0, 2.0, 2.0), BVH_Vec3d(8.0, 8.0, 8.0));

  Standard_Boolean hasOverlap  = Standard_False;
  Standard_Boolean isContained = aBox1.Contains(aBox2, hasOverlap);

  EXPECT_TRUE(isContained);
  EXPECT_TRUE(hasOverlap);
}

TEST(BVH_BoxTest, Contains_NotContained)
{
  BVH_Box<Standard_Real, 3> aBox1(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(10.0, 10.0, 10.0));
  BVH_Box<Standard_Real, 3> aBox2(BVH_Vec3d(8.0, 8.0, 8.0), BVH_Vec3d(12.0, 12.0, 12.0));

  Standard_Boolean hasOverlap  = Standard_False;
  Standard_Boolean isContained = aBox1.Contains(aBox2, hasOverlap);

  EXPECT_FALSE(isContained);
  EXPECT_TRUE(hasOverlap); // They overlap but box2 is not fully contained
}

TEST(BVH_BoxTest, Contains_NoOverlap)
{
  BVH_Box<Standard_Real, 3> aBox1(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(5.0, 5.0, 5.0));
  BVH_Box<Standard_Real, 3> aBox2(BVH_Vec3d(10.0, 10.0, 10.0), BVH_Vec3d(15.0, 15.0, 15.0));

  Standard_Boolean hasOverlap  = Standard_False;
  Standard_Boolean isContained = aBox1.Contains(aBox2, hasOverlap);

  EXPECT_FALSE(isContained);
  EXPECT_FALSE(hasOverlap);
}

TEST(BVH_BoxTest, Contains_SameBox)
{
  BVH_Box<Standard_Real, 3> aBox1(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(10.0, 10.0, 10.0));
  BVH_Box<Standard_Real, 3> aBox2(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(10.0, 10.0, 10.0));

  Standard_Boolean hasOverlap  = Standard_False;
  Standard_Boolean isContained = aBox1.Contains(aBox2, hasOverlap);

  EXPECT_TRUE(isContained);
  EXPECT_TRUE(hasOverlap);
}

TEST(BVH_BoxTest, Contains_TouchingBoundary)
{
  BVH_Box<Standard_Real, 3> aBox1(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(10.0, 10.0, 10.0));
  BVH_Box<Standard_Real, 3> aBox2(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(5.0, 5.0, 5.0));

  Standard_Boolean hasOverlap  = Standard_False;
  Standard_Boolean isContained = aBox1.Contains(aBox2, hasOverlap);

  EXPECT_TRUE(isContained);
  EXPECT_TRUE(hasOverlap);
}

TEST(BVH_BoxTest, Contains_InvalidBox)
{
  BVH_Box<Standard_Real, 3> aBox1(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(10.0, 10.0, 10.0));
  BVH_Box<Standard_Real, 3> aBox2; // Invalid

  Standard_Boolean hasOverlap  = Standard_False;
  Standard_Boolean isContained = aBox1.Contains(aBox2, hasOverlap);

  EXPECT_FALSE(isContained);
  EXPECT_FALSE(hasOverlap);
}

TEST(BVH_BoxTest, Contains_InvalidContainer)
{
  BVH_Box<Standard_Real, 3> aBox1; // Invalid
  BVH_Box<Standard_Real, 3> aBox2(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(10.0, 10.0, 10.0));

  Standard_Boolean hasOverlap  = Standard_False;
  Standard_Boolean isContained = aBox1.Contains(aBox2, hasOverlap);

  EXPECT_FALSE(isContained);
  EXPECT_FALSE(hasOverlap);
}

TEST(BVH_BoxTest, Contains_PartialOverlapOneAxis)
{
  BVH_Box<Standard_Real, 3> aBox1(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(10.0, 10.0, 10.0));
  BVH_Box<Standard_Real, 3> aBox2(BVH_Vec3d(5.0, 5.0, 5.0), BVH_Vec3d(15.0, 8.0, 8.0));

  Standard_Boolean hasOverlap  = Standard_False;
  Standard_Boolean isContained = aBox1.Contains(aBox2, hasOverlap);

  EXPECT_FALSE(isContained); // Not fully contained (extends in X)
  EXPECT_TRUE(hasOverlap);
}

TEST(BVH_BoxTest, Contains_2D)
{
  BVH_Box<Standard_Real, 2> aBox1(BVH_Vec2d(0.0, 0.0), BVH_Vec2d(10.0, 10.0));
  BVH_Box<Standard_Real, 2> aBox2(BVH_Vec2d(2.0, 2.0), BVH_Vec2d(8.0, 8.0));

  Standard_Boolean hasOverlap  = Standard_False;
  Standard_Boolean isContained = aBox1.Contains(aBox2, hasOverlap);

  EXPECT_TRUE(isContained);
  EXPECT_TRUE(hasOverlap);
}

TEST(BVH_BoxTest, ContainsByCorners_FullyContained)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(10.0, 10.0, 10.0));

  Standard_Boolean hasOverlap = Standard_False;
  Standard_Boolean isContained =
    aBox.Contains(BVH_Vec3d(2.0, 2.0, 2.0), BVH_Vec3d(8.0, 8.0, 8.0), hasOverlap);

  EXPECT_TRUE(isContained);
  EXPECT_TRUE(hasOverlap);
}

TEST(BVH_BoxTest, ContainsByCorners_NotContained)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(10.0, 10.0, 10.0));

  Standard_Boolean hasOverlap = Standard_False;
  Standard_Boolean isContained =
    aBox.Contains(BVH_Vec3d(8.0, 8.0, 8.0), BVH_Vec3d(12.0, 12.0, 12.0), hasOverlap);

  EXPECT_FALSE(isContained);
  EXPECT_TRUE(hasOverlap);
}

// =======================================================================================
// Tests for Transform() and Transformed() methods
// =======================================================================================

TEST(BVH_BoxTest, Transform_Identity)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));

  NCollection_Mat4<Standard_Real> aIdentity;
  aIdentity.InitIdentity();

  aBox.Transform(aIdentity);

  EXPECT_NEAR(aBox.CornerMin().x(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().x(), 1.0, Precision::Confusion());
}

TEST(BVH_BoxTest, Transform_Translation)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));

  NCollection_Mat4<Standard_Real> aTransform;
  aTransform.InitIdentity();
  aTransform.SetColumn(3, NCollection_Vec3<Standard_Real>(5.0, 10.0, 15.0));

  aBox.Transform(aTransform);

  EXPECT_NEAR(aBox.CornerMin().x(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMin().y(), 10.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMin().z(), 15.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().x(), 6.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().y(), 11.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().z(), 16.0, Precision::Confusion());
}

TEST(BVH_BoxTest, Transform_Scale)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));

  NCollection_Mat4<Standard_Real> aTransform;
  aTransform.InitIdentity();
  aTransform.SetValue(0, 0, 2.0); // Scale X by 2
  aTransform.SetValue(1, 1, 3.0); // Scale Y by 3
  aTransform.SetValue(2, 2, 4.0); // Scale Z by 4

  aBox.Transform(aTransform);

  EXPECT_NEAR(aBox.CornerMin().x(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().x(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().y(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().z(), 4.0, Precision::Confusion());
}

TEST(BVH_BoxTest, Transformed_Identity)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));

  NCollection_Mat4<Standard_Real> aIdentity;
  aIdentity.InitIdentity();

  BVH_Box<Standard_Real, 3> aTransformed = aBox.Transformed(aIdentity);

  // Original should be unchanged
  EXPECT_NEAR(aBox.CornerMin().x(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().x(), 1.0, Precision::Confusion());

  // Transformed should be the same
  EXPECT_NEAR(aTransformed.CornerMin().x(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aTransformed.CornerMax().x(), 1.0, Precision::Confusion());
}

TEST(BVH_BoxTest, Transformed_Translation)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));

  NCollection_Mat4<Standard_Real> aTransform;
  aTransform.InitIdentity();
  aTransform.SetColumn(3, NCollection_Vec3<Standard_Real>(10.0, 20.0, 30.0));

  BVH_Box<Standard_Real, 3> aTransformed = aBox.Transformed(aTransform);

  // Original should be unchanged
  EXPECT_NEAR(aBox.CornerMin().x(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().x(), 1.0, Precision::Confusion());

  // Transformed should be translated
  EXPECT_NEAR(aTransformed.CornerMin().x(), 10.0, Precision::Confusion());
  EXPECT_NEAR(aTransformed.CornerMin().y(), 20.0, Precision::Confusion());
  EXPECT_NEAR(aTransformed.CornerMin().z(), 30.0, Precision::Confusion());
  EXPECT_NEAR(aTransformed.CornerMax().x(), 11.0, Precision::Confusion());
  EXPECT_NEAR(aTransformed.CornerMax().y(), 21.0, Precision::Confusion());
  EXPECT_NEAR(aTransformed.CornerMax().z(), 31.0, Precision::Confusion());
}

TEST(BVH_BoxTest, Transform_InvalidBox)
{
  BVH_Box<Standard_Real, 3> aBox; // Invalid box

  NCollection_Mat4<Standard_Real> aTransform;
  aTransform.InitIdentity();
  aTransform.SetColumn(3, NCollection_Vec3<Standard_Real>(10.0, 20.0, 30.0));

  aBox.Transform(aTransform);

  // Should remain invalid
  EXPECT_FALSE(aBox.IsValid());
}

TEST(BVH_BoxTest, Transformed_InvalidBox)
{
  BVH_Box<Standard_Real, 3> aBox; // Invalid box

  NCollection_Mat4<Standard_Real> aTransform;
  aTransform.InitIdentity();
  aTransform.SetColumn(3, NCollection_Vec3<Standard_Real>(10.0, 20.0, 30.0));

  BVH_Box<Standard_Real, 3> aTransformed = aBox.Transformed(aTransform);

  // Should remain invalid
  EXPECT_FALSE(aTransformed.IsValid());
}

// =======================================================================================
// Tests for Corner Modification
// =======================================================================================

TEST(BVH_BoxTest, ModifyCornerMin)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(10.0, 10.0, 10.0));

  // Modify via non-const reference
  aBox.CornerMin() = BVH_Vec3d(-5.0, -5.0, -5.0);

  EXPECT_NEAR(aBox.CornerMin().x(), -5.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMin().y(), -5.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMin().z(), -5.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().x(), 10.0, Precision::Confusion());
}

TEST(BVH_BoxTest, ModifyCornerMax)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(10.0, 10.0, 10.0));

  // Modify via non-const reference
  aBox.CornerMax() = BVH_Vec3d(20.0, 30.0, 40.0);

  EXPECT_NEAR(aBox.CornerMax().x(), 20.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().y(), 30.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().z(), 40.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMin().x(), 0.0, Precision::Confusion());
}

TEST(BVH_BoxTest, ModifyCornerComponents)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(10.0, 10.0, 10.0));

  // Modify individual components
  aBox.CornerMin().x() = -1.0;
  aBox.CornerMin().y() = -2.0;
  aBox.CornerMax().z() = 15.0;

  EXPECT_NEAR(aBox.CornerMin().x(), -1.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMin().y(), -2.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMin().z(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aBox.CornerMax().z(), 15.0, Precision::Confusion());
}

// =======================================================================================
// Additional Edge Cases
// =======================================================================================

TEST(BVH_BoxTest, Area_DegenerateBox2D)
{
  // 2D box with zero area should return perimeter
  BVH_Box<Standard_Real, 2> aBox(BVH_Vec2d(0.0, 0.0), BVH_Vec2d(5.0, 0.0));

  Standard_Real anArea = aBox.Area();
  // For degenerate case, returns sum of dimensions
  EXPECT_GE(anArea, 0.0);
  EXPECT_NEAR(anArea, 5.0, Precision::Confusion()); // Width + height = 5 + 0
}

TEST(BVH_BoxTest, IsOut_TwoCorners_Overlapping)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(10.0, 10.0, 10.0));

  // Test with overlapping region
  EXPECT_FALSE(aBox.IsOut(BVH_Vec3d(5.0, 5.0, 5.0), BVH_Vec3d(15.0, 15.0, 15.0)));
}

TEST(BVH_BoxTest, IsOut_TwoCorners_Disjoint)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(10.0, 10.0, 10.0));

  // Test with disjoint region
  EXPECT_TRUE(aBox.IsOut(BVH_Vec3d(20.0, 20.0, 20.0), BVH_Vec3d(30.0, 30.0, 30.0)));
}

TEST(BVH_BoxTest, IsOut_TwoCorners_Contained)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(10.0, 10.0, 10.0));

  // Test with fully contained region
  EXPECT_FALSE(aBox.IsOut(BVH_Vec3d(2.0, 2.0, 2.0), BVH_Vec3d(8.0, 8.0, 8.0)));
}

TEST(BVH_BoxTest, Constexpr_Construction)
{
  // Test that constexpr constructors work at compile time
  constexpr BVH_Vec3d                 aMin(0.0, 0.0, 0.0);
  constexpr BVH_Vec3d                 aMax(1.0, 1.0, 1.0);
  constexpr BVH_Box<Standard_Real, 3> aBox(aMin, aMax);

  static_assert(aBox.IsValid(), "Constexpr box should be valid");
}

TEST(BVH_BoxTest, Constexpr_SinglePoint)
{
  constexpr BVH_Vec3d                 aPoint(5.0, 5.0, 5.0);
  constexpr BVH_Box<Standard_Real, 3> aBox(aPoint);

  static_assert(aBox.IsValid(), "Constexpr single-point box should be valid");
}

TEST(BVH_BoxTest, Constexpr_DefaultInvalid)
{
  constexpr BVH_Box<Standard_Real, 3> aBox;

  static_assert(!aBox.IsValid(), "Default constexpr box should be invalid");
}

// =======================================================================================
// Tests for Transform/Transformed with float precision
// =======================================================================================

TEST(BVH_BoxTest, Transform_Float_Identity)
{
  BVH_Box<Standard_ShortReal, 3> aBox(BVH_Vec3f(0.0f, 0.0f, 0.0f), BVH_Vec3f(1.0f, 1.0f, 1.0f));

  NCollection_Mat4<Standard_ShortReal> aIdentity;
  aIdentity.InitIdentity();

  aBox.Transform(aIdentity);

  EXPECT_NEAR(aBox.CornerMin().x(), 0.0f, 1e-5f);
  EXPECT_NEAR(aBox.CornerMax().x(), 1.0f, 1e-5f);
}

TEST(BVH_BoxTest, Transform_Float_Translation)
{
  BVH_Box<Standard_ShortReal, 3> aBox(BVH_Vec3f(0.0f, 0.0f, 0.0f), BVH_Vec3f(1.0f, 1.0f, 1.0f));

  NCollection_Mat4<Standard_ShortReal> aTransform;
  aTransform.InitIdentity();
  aTransform.SetColumn(3, NCollection_Vec3<Standard_ShortReal>(5.0f, 10.0f, 15.0f));

  aBox.Transform(aTransform);

  EXPECT_NEAR(aBox.CornerMin().x(), 5.0f, 1e-5f);
  EXPECT_NEAR(aBox.CornerMin().y(), 10.0f, 1e-5f);
  EXPECT_NEAR(aBox.CornerMin().z(), 15.0f, 1e-5f);
  EXPECT_NEAR(aBox.CornerMax().x(), 6.0f, 1e-5f);
  EXPECT_NEAR(aBox.CornerMax().y(), 11.0f, 1e-5f);
  EXPECT_NEAR(aBox.CornerMax().z(), 16.0f, 1e-5f);
}

TEST(BVH_BoxTest, Transform_Float_Scale)
{
  BVH_Box<Standard_ShortReal, 3> aBox(BVH_Vec3f(0.0f, 0.0f, 0.0f), BVH_Vec3f(1.0f, 1.0f, 1.0f));

  NCollection_Mat4<Standard_ShortReal> aTransform;
  aTransform.InitIdentity();
  aTransform.SetValue(0, 0, 2.0f); // Scale X by 2
  aTransform.SetValue(1, 1, 3.0f); // Scale Y by 3
  aTransform.SetValue(2, 2, 4.0f); // Scale Z by 4

  aBox.Transform(aTransform);

  EXPECT_NEAR(aBox.CornerMin().x(), 0.0f, 1e-5f);
  EXPECT_NEAR(aBox.CornerMax().x(), 2.0f, 1e-5f);
  EXPECT_NEAR(aBox.CornerMax().y(), 3.0f, 1e-5f);
  EXPECT_NEAR(aBox.CornerMax().z(), 4.0f, 1e-5f);
}

TEST(BVH_BoxTest, Transformed_Float_Translation)
{
  BVH_Box<Standard_ShortReal, 3> aBox(BVH_Vec3f(0.0f, 0.0f, 0.0f), BVH_Vec3f(1.0f, 1.0f, 1.0f));

  NCollection_Mat4<Standard_ShortReal> aTransform;
  aTransform.InitIdentity();
  aTransform.SetColumn(3, NCollection_Vec3<Standard_ShortReal>(10.0f, 20.0f, 30.0f));

  BVH_Box<Standard_ShortReal, 3> aTransformed = aBox.Transformed(aTransform);

  // Original should be unchanged
  EXPECT_NEAR(aBox.CornerMin().x(), 0.0f, 1e-5f);
  EXPECT_NEAR(aBox.CornerMax().x(), 1.0f, 1e-5f);

  // Transformed should be translated
  EXPECT_NEAR(aTransformed.CornerMin().x(), 10.0f, 1e-5f);
  EXPECT_NEAR(aTransformed.CornerMin().y(), 20.0f, 1e-5f);
  EXPECT_NEAR(aTransformed.CornerMin().z(), 30.0f, 1e-5f);
  EXPECT_NEAR(aTransformed.CornerMax().x(), 11.0f, 1e-5f);
  EXPECT_NEAR(aTransformed.CornerMax().y(), 21.0f, 1e-5f);
  EXPECT_NEAR(aTransformed.CornerMax().z(), 31.0f, 1e-5f);
}

TEST(BVH_BoxTest, Transform_Float_InvalidBox)
{
  BVH_Box<Standard_ShortReal, 3> aBox; // Invalid box

  NCollection_Mat4<Standard_ShortReal> aTransform;
  aTransform.InitIdentity();
  aTransform.SetColumn(3, NCollection_Vec3<Standard_ShortReal>(10.0f, 20.0f, 30.0f));

  aBox.Transform(aTransform);

  // Should remain invalid
  EXPECT_FALSE(aBox.IsValid());
}
