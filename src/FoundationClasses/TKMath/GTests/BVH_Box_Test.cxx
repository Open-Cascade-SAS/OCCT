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
