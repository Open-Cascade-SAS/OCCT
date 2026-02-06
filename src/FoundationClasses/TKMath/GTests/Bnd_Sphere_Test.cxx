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

#include <Bnd_Sphere.hxx>
#include <gp_XYZ.hxx>

#include <gtest/gtest.h>

TEST(Bnd_SphereTest, DefaultConstructor)
{
  Bnd_Sphere aSphere;
  EXPECT_DOUBLE_EQ(aSphere.Center().X(), 0.0);
  EXPECT_DOUBLE_EQ(aSphere.Center().Y(), 0.0);
  EXPECT_DOUBLE_EQ(aSphere.Center().Z(), 0.0);
  EXPECT_DOUBLE_EQ(aSphere.Radius(), 0.0);
  EXPECT_FALSE(aSphere.IsValid());
  EXPECT_EQ(aSphere.U(), 0);
  EXPECT_EQ(aSphere.V(), 0);
}

TEST(Bnd_SphereTest, ParameterizedConstructor)
{
  const gp_XYZ aCenter(1.0, 2.0, 3.0);
  Bnd_Sphere   aSphere(aCenter, 5.0, 10, 20);
  EXPECT_DOUBLE_EQ(aSphere.Center().X(), 1.0);
  EXPECT_DOUBLE_EQ(aSphere.Center().Y(), 2.0);
  EXPECT_DOUBLE_EQ(aSphere.Center().Z(), 3.0);
  EXPECT_DOUBLE_EQ(aSphere.Radius(), 5.0);
  EXPECT_EQ(aSphere.U(), 10);
  EXPECT_EQ(aSphere.V(), 20);
}

TEST(Bnd_SphereTest, Validity)
{
  Bnd_Sphere aSphere;
  EXPECT_FALSE(aSphere.IsValid());
  aSphere.SetValid(true);
  EXPECT_TRUE(aSphere.IsValid());
  aSphere.SetValid(false);
  EXPECT_FALSE(aSphere.IsValid());
}

TEST(Bnd_SphereTest, Distance)
{
  const gp_XYZ aCenter(0.0, 0.0, 0.0);
  Bnd_Sphere   aSphere(aCenter, 1.0, 0, 0);
  const gp_XYZ aPoint(3.0, 4.0, 0.0);
  const double aDist = aSphere.Distance(aPoint);
  EXPECT_DOUBLE_EQ(aDist, 5.0);
}

TEST(Bnd_SphereTest, SquareDistance)
{
  const gp_XYZ aCenter(0.0, 0.0, 0.0);
  Bnd_Sphere   aSphere(aCenter, 1.0, 0, 0);
  const gp_XYZ aPoint(3.0, 4.0, 0.0);
  EXPECT_DOUBLE_EQ(aSphere.SquareDistance(aPoint), 25.0);
}

TEST(Bnd_SphereTest, Distances_PointOutside)
{
  const gp_XYZ aCenter(0.0, 0.0, 0.0);
  Bnd_Sphere   aSphere(aCenter, 2.0, 0, 0);
  const gp_XYZ aPoint(5.0, 0.0, 0.0);
  double       aMin = 0.0, aMax = 0.0;
  aSphere.Distances(aPoint, aMin, aMax);
  EXPECT_DOUBLE_EQ(aMin, 3.0);
  EXPECT_DOUBLE_EQ(aMax, 7.0);
}

TEST(Bnd_SphereTest, Distances_PointInside)
{
  const gp_XYZ aCenter(0.0, 0.0, 0.0);
  Bnd_Sphere   aSphere(aCenter, 5.0, 0, 0);
  const gp_XYZ aPoint(1.0, 0.0, 0.0);
  double       aMin = 0.0, aMax = 0.0;
  aSphere.Distances(aPoint, aMin, aMax);
  EXPECT_DOUBLE_EQ(aMin, 0.0);
  EXPECT_DOUBLE_EQ(aMax, 6.0);
}

TEST(Bnd_SphereTest, SquareDistances_PointOutside)
{
  const gp_XYZ aCenter(0.0, 0.0, 0.0);
  Bnd_Sphere   aSphere(aCenter, 2.0, 0, 0);
  const gp_XYZ aPoint(5.0, 0.0, 0.0);
  double       aMin = 0.0, aMax = 0.0;
  aSphere.SquareDistances(aPoint, aMin, aMax);
  // d² = 25, r² = 4
  // min = d² - r² = 21, max = d² + r² = 29
  EXPECT_DOUBLE_EQ(aMin, 21.0);
  EXPECT_DOUBLE_EQ(aMax, 29.0);
}

TEST(Bnd_SphereTest, SquareDistances_PointInside)
{
  const gp_XYZ aCenter(0.0, 0.0, 0.0);
  Bnd_Sphere   aSphere(aCenter, 5.0, 0, 0);
  const gp_XYZ aPoint(1.0, 0.0, 0.0);
  double       aMin = 0.0, aMax = 0.0;
  aSphere.SquareDistances(aPoint, aMin, aMax);
  // d² = 1, r² = 25
  // d² < r² → min = 0, max = d² + r² = 26
  EXPECT_DOUBLE_EQ(aMin, 0.0);
  EXPECT_DOUBLE_EQ(aMax, 26.0);
}

TEST(Bnd_SphereTest, SquareDistances_PointAtCenter)
{
  const gp_XYZ aCenter(0.0, 0.0, 0.0);
  Bnd_Sphere   aSphere(aCenter, 3.0, 0, 0);
  const gp_XYZ aPoint(0.0, 0.0, 0.0);
  double       aMin = 0.0, aMax = 0.0;
  aSphere.SquareDistances(aPoint, aMin, aMax);
  // d² = 0, r² = 9
  // d² < r² → min = 0, max = 0 + 9 = 9
  EXPECT_DOUBLE_EQ(aMin, 0.0);
  EXPECT_DOUBLE_EQ(aMax, 9.0);
}

TEST(Bnd_SphereTest, SquareDistances_PointOnSurface)
{
  const gp_XYZ aCenter(0.0, 0.0, 0.0);
  Bnd_Sphere   aSphere(aCenter, 3.0, 0, 0);
  const gp_XYZ aPoint(3.0, 0.0, 0.0);
  double       aMin = 0.0, aMax = 0.0;
  aSphere.SquareDistances(aPoint, aMin, aMax);
  // d² = 9, r² = 9
  // d² < r² is false (equal), so min = d² - r² = 0, max = d² + r² = 18
  EXPECT_DOUBLE_EQ(aMin, 0.0);
  EXPECT_DOUBLE_EQ(aMax, 18.0);
}

TEST(Bnd_SphereTest, Project)
{
  const gp_XYZ aCenter(1.0, 2.0, 3.0);
  Bnd_Sphere   aSphere(aCenter, 5.0, 0, 0);
  const gp_XYZ aNode(10.0, 20.0, 30.0);
  gp_XYZ       aProjNode;
  double       aDist    = 0.0;
  bool         anInside = false;
  const bool   isOk     = aSphere.Project(aNode, aProjNode, aDist, anInside);
  EXPECT_TRUE(isOk);
  EXPECT_TRUE(anInside);
  EXPECT_DOUBLE_EQ(aProjNode.X(), aCenter.X());
  EXPECT_DOUBLE_EQ(aProjNode.Y(), aCenter.Y());
  EXPECT_DOUBLE_EQ(aProjNode.Z(), aCenter.Z());
}

TEST(Bnd_SphereTest, Add_EnclosingSphere)
{
  Bnd_Sphere aSphere1(gp_XYZ(0.0, 0.0, 0.0), 10.0, 0, 0);
  Bnd_Sphere aSphere2(gp_XYZ(1.0, 0.0, 0.0), 2.0, 0, 0);
  aSphere1.Add(aSphere2);
  // aSphere1 already encloses aSphere2, should remain unchanged
  EXPECT_DOUBLE_EQ(aSphere1.Radius(), 10.0);
}

TEST(Bnd_SphereTest, Add_EnclosedBySphere)
{
  Bnd_Sphere aSphere1(gp_XYZ(0.0, 0.0, 0.0), 2.0, 0, 0);
  Bnd_Sphere aSphere2(gp_XYZ(0.0, 0.0, 0.0), 10.0, 0, 0);
  aSphere1.Add(aSphere2);
  // aSphere2 encloses aSphere1, should take aSphere2
  EXPECT_DOUBLE_EQ(aSphere1.Radius(), 10.0);
}

TEST(Bnd_SphereTest, Add_PartialOverlap)
{
  Bnd_Sphere aSphere1(gp_XYZ(0.0, 0.0, 0.0), 3.0, 0, 0);
  Bnd_Sphere aSphere2(gp_XYZ(5.0, 0.0, 0.0), 3.0, 0, 0);
  aSphere1.Add(aSphere2);
  // Combined radius should be (5 + 3 + 3) / 2 = 5.5
  EXPECT_DOUBLE_EQ(aSphere1.Radius(), 5.5);
}

TEST(Bnd_SphereTest, IsOut_Separated)
{
  Bnd_Sphere aSphere1(gp_XYZ(0.0, 0.0, 0.0), 1.0, 0, 0);
  Bnd_Sphere aSphere2(gp_XYZ(10.0, 0.0, 0.0), 1.0, 0, 0);
  EXPECT_TRUE(aSphere1.IsOut(aSphere2));
}

TEST(Bnd_SphereTest, IsOut_Overlapping)
{
  Bnd_Sphere aSphere1(gp_XYZ(0.0, 0.0, 0.0), 3.0, 0, 0);
  Bnd_Sphere aSphere2(gp_XYZ(4.0, 0.0, 0.0), 3.0, 0, 0);
  EXPECT_FALSE(aSphere1.IsOut(aSphere2));
}

TEST(Bnd_SphereTest, IsOut_PointWithMaxDist)
{
  Bnd_Sphere   aSphere(gp_XYZ(0.0, 0.0, 0.0), 5.0, 0, 0);
  const gp_XYZ aPoint(20.0, 0.0, 0.0);
  double       aMaxDist = 100.0;
  aSphere.SetValid(true);
  EXPECT_FALSE(aSphere.IsOut(aPoint, aMaxDist));
  // aMaxDist should be updated to aCurMaxDist = 20 + 5 = 25
  EXPECT_DOUBLE_EQ(aMaxDist, 25.0);
}

TEST(Bnd_SphereTest, IsOut_PointTooFar)
{
  Bnd_Sphere   aSphere(gp_XYZ(0.0, 0.0, 0.0), 1.0, 0, 0);
  const gp_XYZ aPoint(20.0, 0.0, 0.0);
  double       aMaxDist = 10.0;
  EXPECT_TRUE(aSphere.IsOut(aPoint, aMaxDist));
}

TEST(Bnd_SphereTest, SquareExtent)
{
  Bnd_Sphere aSphere(gp_XYZ(0.0, 0.0, 0.0), 3.0, 0, 0);
  EXPECT_DOUBLE_EQ(aSphere.SquareExtent(), 36.0);
}
