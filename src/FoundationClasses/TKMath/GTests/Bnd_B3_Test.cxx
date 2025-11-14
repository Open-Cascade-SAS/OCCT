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

#include <Bnd_B3.hxx>
#include <gp_XYZ.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>
#include <Precision.hxx>

TEST(Bnd_B3dTest, DefaultConstructor)
{
  Bnd_B3d aBox;
  EXPECT_TRUE(aBox.IsVoid());
}

TEST(Bnd_B3dTest, ConstructorWithCenterAndHSize)
{
  gp_XYZ aCenter(5.0, 10.0, 15.0);
  gp_XYZ aHSize(2.0, 3.0, 4.0);

  Bnd_B3d aBox(aCenter, aHSize);

  EXPECT_FALSE(aBox.IsVoid());

  gp_XYZ aMin = aBox.CornerMin();
  gp_XYZ aMax = aBox.CornerMax();

  EXPECT_NEAR(aMin.X(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aMin.Y(), 7.0, Precision::Confusion());
  EXPECT_NEAR(aMin.Z(), 11.0, Precision::Confusion());
  EXPECT_NEAR(aMax.X(), 7.0, Precision::Confusion());
  EXPECT_NEAR(aMax.Y(), 13.0, Precision::Confusion());
  EXPECT_NEAR(aMax.Z(), 19.0, Precision::Confusion());
}

TEST(Bnd_B3dTest, Clear)
{
  Bnd_B3d aBox(gp_XYZ(0.0, 0.0, 0.0), gp_XYZ(1.0, 1.0, 1.0));
  EXPECT_FALSE(aBox.IsVoid());

  aBox.Clear();
  EXPECT_TRUE(aBox.IsVoid());
}

TEST(Bnd_B3dTest, AddPoint)
{
  Bnd_B3d aBox;

  aBox.Add(gp_XYZ(1.0, 2.0, 3.0));
  EXPECT_FALSE(aBox.IsVoid());

  gp_XYZ aMin = aBox.CornerMin();
  gp_XYZ aMax = aBox.CornerMax();
  EXPECT_NEAR(aMin.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aMin.Y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aMin.Z(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aMax.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aMax.Y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aMax.Z(), 3.0, Precision::Confusion());

  aBox.Add(gp_XYZ(4.0, 5.0, 6.0));
  aMin = aBox.CornerMin();
  aMax = aBox.CornerMax();
  EXPECT_NEAR(aMin.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aMin.Y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aMin.Z(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aMax.X(), 4.0, Precision::Confusion());
  EXPECT_NEAR(aMax.Y(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aMax.Z(), 6.0, Precision::Confusion());
}

TEST(Bnd_B3dTest, AddPnt)
{
  Bnd_B3d aBox;
  aBox.Add(gp_Pnt(1.0, 2.0, 3.0));

  EXPECT_FALSE(aBox.IsVoid());
  gp_XYZ aMin = aBox.CornerMin();
  EXPECT_NEAR(aMin.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aMin.Y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aMin.Z(), 3.0, Precision::Confusion());
}

TEST(Bnd_B3dTest, AddBox)
{
  Bnd_B3d aBox1(gp_XYZ(0.0, 0.0, 0.0), gp_XYZ(1.0, 1.0, 1.0));
  Bnd_B3d aBox2(gp_XYZ(3.0, 3.0, 3.0), gp_XYZ(1.0, 1.0, 1.0));

  aBox1.Add(aBox2);

  gp_XYZ aMin = aBox1.CornerMin();
  gp_XYZ aMax = aBox1.CornerMax();
  EXPECT_NEAR(aMin.X(), -1.0, Precision::Confusion());
  EXPECT_NEAR(aMin.Y(), -1.0, Precision::Confusion());
  EXPECT_NEAR(aMin.Z(), -1.0, Precision::Confusion());
  EXPECT_NEAR(aMax.X(), 4.0, Precision::Confusion());
  EXPECT_NEAR(aMax.Y(), 4.0, Precision::Confusion());
  EXPECT_NEAR(aMax.Z(), 4.0, Precision::Confusion());
}

TEST(Bnd_B3dTest, SquareExtent)
{
  Bnd_B3d       aBox(gp_XYZ(0.0, 0.0, 0.0), gp_XYZ(3.0, 4.0, 5.0));
  Standard_Real aSqExtent = aBox.SquareExtent();

  // Square diagonal = 4 * (3^2 + 4^2 + 5^2) = 4 * 50 = 200
  EXPECT_NEAR(aSqExtent, 200.0, Precision::Confusion());
}

TEST(Bnd_B3dTest, Enlarge)
{
  Bnd_B3d aBox(gp_XYZ(0.0, 0.0, 0.0), gp_XYZ(1.0, 1.0, 1.0));
  aBox.Enlarge(0.5);

  gp_XYZ aMin = aBox.CornerMin();
  gp_XYZ aMax = aBox.CornerMax();
  EXPECT_NEAR(aMin.X(), -1.5, Precision::Confusion());
  EXPECT_NEAR(aMin.Y(), -1.5, Precision::Confusion());
  EXPECT_NEAR(aMin.Z(), -1.5, Precision::Confusion());
  EXPECT_NEAR(aMax.X(), 1.5, Precision::Confusion());
  EXPECT_NEAR(aMax.Y(), 1.5, Precision::Confusion());
  EXPECT_NEAR(aMax.Z(), 1.5, Precision::Confusion());
}

TEST(Bnd_B3dTest, Limit)
{
  // Test limiting a large box by a smaller box inside it
  // aBox1: (-5, -5, -5) to (5, 5, 5)
  // aBox2: (0, 0, 0) to (4, 4, 4)
  Bnd_B3d aBox1(gp_XYZ(0.0, 0.0, 0.0), gp_XYZ(5.0, 5.0, 5.0));
  Bnd_B3d aBox2(gp_XYZ(2.0, 2.0, 2.0), gp_XYZ(2.0, 2.0, 2.0));

  Standard_Boolean isLimited = aBox1.Limit(aBox2);
  EXPECT_TRUE(isLimited);

  // After limiting, aBox1's min corner should align with aBox2's min corner
  gp_XYZ aMin = aBox1.CornerMin();
  gp_XYZ aMax = aBox1.CornerMax();
  EXPECT_NEAR(aMin.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aMin.Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aMin.Z(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aMax.X(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aMax.Y(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aMax.Z(), 5.0, Precision::Confusion());

  // Test with non-intersecting boxes
  Bnd_B3d          aBox3(gp_XYZ(0.0, 0.0, 0.0), gp_XYZ(1.0, 1.0, 1.0));
  Bnd_B3d          aBox4(gp_XYZ(10.0, 10.0, 10.0), gp_XYZ(1.0, 1.0, 1.0));
  Standard_Boolean isLimited2 = aBox3.Limit(aBox4);
  EXPECT_FALSE(isLimited2);
}

TEST(Bnd_B3dTest, IsOutPoint)
{
  Bnd_B3d aBox(gp_XYZ(0.0, 0.0, 0.0), gp_XYZ(1.0, 1.0, 1.0));

  EXPECT_FALSE(aBox.IsOut(gp_XYZ(0.0, 0.0, 0.0)));
  EXPECT_FALSE(aBox.IsOut(gp_XYZ(0.5, 0.5, 0.5)));
  EXPECT_TRUE(aBox.IsOut(gp_XYZ(2.0, 0.0, 0.0)));
  EXPECT_TRUE(aBox.IsOut(gp_XYZ(0.0, 2.0, 0.0)));
  EXPECT_TRUE(aBox.IsOut(gp_XYZ(0.0, 0.0, 2.0)));
}

TEST(Bnd_B3dTest, IsOutSphere)
{
  Bnd_B3d aBox(gp_XYZ(0.0, 0.0, 0.0), gp_XYZ(1.0, 1.0, 1.0));

  // Sphere at (0, 0, 0) with small radius - should intersect
  EXPECT_FALSE(aBox.IsOut(gp_XYZ(0.0, 0.0, 0.0), 0.5));

  // Sphere far away - should not intersect
  EXPECT_TRUE(aBox.IsOut(gp_XYZ(10.0, 10.0, 10.0), 1.0));
}

TEST(Bnd_B3dTest, IsOutBox)
{
  Bnd_B3d aBox1(gp_XYZ(0.0, 0.0, 0.0), gp_XYZ(1.0, 1.0, 1.0));
  Bnd_B3d aBox2(gp_XYZ(0.5, 0.5, 0.5), gp_XYZ(1.0, 1.0, 1.0));
  Bnd_B3d aBox3(gp_XYZ(5.0, 5.0, 5.0), gp_XYZ(1.0, 1.0, 1.0));

  EXPECT_FALSE(aBox1.IsOut(aBox2));
  EXPECT_TRUE(aBox1.IsOut(aBox3));
}

TEST(Bnd_B3dTest, IsOutLine)
{
  Bnd_B3d aBox(gp_XYZ(0.0, 0.0, 0.0), gp_XYZ(1.0, 1.0, 1.0));

  // Line passing through box
  gp_Ax1 aLine1(gp_Pnt(-2.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  EXPECT_FALSE(aBox.IsOut(aLine1));

  // Line not intersecting box
  gp_Ax1 aLine2(gp_Pnt(-2.0, 5.0, 5.0), gp_Dir(1.0, 0.0, 0.0));
  EXPECT_TRUE(aBox.IsOut(aLine2));
}

TEST(Bnd_B3dTest, IsOutPlane)
{
  Bnd_B3d aBox(gp_XYZ(0.0, 0.0, 0.0), gp_XYZ(1.0, 1.0, 1.0));

  // Plane passing through box
  gp_Ax3 aPlane1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  EXPECT_FALSE(aBox.IsOut(aPlane1));

  // Plane not intersecting box
  gp_Ax3 aPlane2(gp_Pnt(0.0, 0.0, 5.0), gp_Dir(0.0, 0.0, 1.0));
  EXPECT_TRUE(aBox.IsOut(aPlane2));
}

TEST(Bnd_B3dTest, IsInBox)
{
  Bnd_B3d aBox1(gp_XYZ(0.0, 0.0, 0.0), gp_XYZ(0.5, 0.5, 0.5));
  Bnd_B3d aBox2(gp_XYZ(0.0, 0.0, 0.0), gp_XYZ(2.0, 2.0, 2.0));

  EXPECT_TRUE(aBox1.IsIn(aBox2));
  EXPECT_FALSE(aBox2.IsIn(aBox1));
}

TEST(Bnd_B3dTest, Transformed)
{
  Bnd_B3d aBox(gp_XYZ(1.0, 1.0, 1.0), gp_XYZ(1.0, 1.0, 1.0));

  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(2.0, 3.0, 4.0));

  Bnd_B3d aTransformedBox = aBox.Transformed(aTrsf);

  gp_XYZ aMin = aTransformedBox.CornerMin();
  gp_XYZ aMax = aTransformedBox.CornerMax();

  EXPECT_NEAR(aMin.X(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aMin.Y(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aMin.Z(), 4.0, Precision::Confusion());
  EXPECT_NEAR(aMax.X(), 4.0, Precision::Confusion());
  EXPECT_NEAR(aMax.Y(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aMax.Z(), 6.0, Precision::Confusion());
}

TEST(Bnd_B3dTest, TransformedWithRotation)
{
  Bnd_B3d aBox(gp_XYZ(1.0, 0.0, 0.0), gp_XYZ(0.5, 0.5, 0.5));

  gp_Trsf aTrsf;
  aTrsf.SetRotation(gp_Ax1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), M_PI / 2.0);

  Bnd_B3d aTransformedBox = aBox.Transformed(aTrsf);

  // After 90 degree rotation around Z-axis, the center should be at (0, 1, 0)
  gp_XYZ aMin = aTransformedBox.CornerMin();
  gp_XYZ aMax = aTransformedBox.CornerMax();

  // The rotated box should have its center near (0, 1, 0)
  // and half-size should remain approximately the same (with some expansion for rotation)
  EXPECT_NEAR((aMin.X() + aMax.X()) / 2.0, 0.0, 1e-10);
  EXPECT_NEAR((aMin.Y() + aMax.Y()) / 2.0, 1.0, 1e-10);
  EXPECT_NEAR((aMin.Z() + aMax.Z()) / 2.0, 0.0, 1e-10);
}

TEST(Bnd_B3dTest, SetCenterAndHSize)
{
  Bnd_B3d aBox;

  aBox.SetCenter(gp_XYZ(5.0, 10.0, 15.0));
  aBox.SetHSize(gp_XYZ(2.0, 3.0, 4.0));

  gp_XYZ aMin = aBox.CornerMin();
  gp_XYZ aMax = aBox.CornerMax();

  EXPECT_NEAR(aMin.X(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aMin.Y(), 7.0, Precision::Confusion());
  EXPECT_NEAR(aMin.Z(), 11.0, Precision::Confusion());
  EXPECT_NEAR(aMax.X(), 7.0, Precision::Confusion());
  EXPECT_NEAR(aMax.Y(), 13.0, Precision::Confusion());
  EXPECT_NEAR(aMax.Z(), 19.0, Precision::Confusion());
}

TEST(Bnd_B3fTest, FloatPrecision)
{
  Bnd_B3f aBox(gp_XYZ(1.0f, 2.0f, 3.0f), gp_XYZ(0.5f, 0.5f, 0.5f));

  EXPECT_FALSE(aBox.IsVoid());

  gp_XYZ aMin = aBox.CornerMin();
  gp_XYZ aMax = aBox.CornerMax();

  EXPECT_NEAR(aMin.X(), 0.5, 1e-5);
  EXPECT_NEAR(aMin.Y(), 1.5, 1e-5);
  EXPECT_NEAR(aMin.Z(), 2.5, 1e-5);
  EXPECT_NEAR(aMax.X(), 1.5, 1e-5);
  EXPECT_NEAR(aMax.Y(), 2.5, 1e-5);
  EXPECT_NEAR(aMax.Z(), 3.5, 1e-5);
}

TEST(Bnd_B3dTest, IsOutRay)
{
  Bnd_B3d aBox(gp_XYZ(1.0, 1.0, 1.0), gp_XYZ(1.0, 1.0, 1.0));

  // Ray starting before the box and pointing towards it
  gp_Ax1 aRay1(gp_Pnt(-1.0, 1.0, 1.0), gp_Dir(1.0, 0.0, 0.0));
  EXPECT_FALSE(aBox.IsOut(aRay1, Standard_True));

  // Ray starting inside the box
  gp_Ax1 aRay2(gp_Pnt(1.0, 1.0, 1.0), gp_Dir(1.0, 0.0, 0.0));
  EXPECT_FALSE(aBox.IsOut(aRay2, Standard_True));

  // Ray pointing away from the box
  gp_Ax1 aRay3(gp_Pnt(-1.0, 1.0, 1.0), gp_Dir(-1.0, 0.0, 0.0));
  EXPECT_TRUE(aBox.IsOut(aRay3, Standard_True));
}

TEST(Bnd_B3dTest, IsOutLineWithOverthickness)
{
  Bnd_B3d aBox(gp_XYZ(0.0, 0.0, 0.0), gp_XYZ(1.0, 1.0, 1.0));

  // Line passing just outside the box, but within overthickness
  gp_Ax1 aLine(gp_Pnt(-2.0, 1.2, 0.0), gp_Dir(1.0, 0.0, 0.0));

  // Without overthickness, should be out
  EXPECT_TRUE(aBox.IsOut(aLine, Standard_False, 0.0));

  // With sufficient overthickness, should intersect
  EXPECT_FALSE(aBox.IsOut(aLine, Standard_False, 0.3));
}
