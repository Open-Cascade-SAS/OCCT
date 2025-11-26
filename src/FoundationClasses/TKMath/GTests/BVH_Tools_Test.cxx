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
#include <BVH_Tools.hxx>
#include <Precision.hxx>

TEST(BVH_ToolsTest, PointBoxSquareDistance)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));

  // Point inside box
  Standard_Real aDist1 =
    BVH_Tools<Standard_Real, 3>::PointBoxSquareDistance(BVH_Vec3d(0.5, 0.5, 0.5), aBox);
  EXPECT_NEAR(aDist1, 0.0, Precision::Confusion());

  // Point on face
  Standard_Real aDist2 =
    BVH_Tools<Standard_Real, 3>::PointBoxSquareDistance(BVH_Vec3d(0.5, 0.5, 1.0), aBox);
  EXPECT_NEAR(aDist2, 0.0, Precision::Confusion());

  // Point outside box (distance = 1)
  Standard_Real aDist3 =
    BVH_Tools<Standard_Real, 3>::PointBoxSquareDistance(BVH_Vec3d(2.0, 0.5, 0.5), aBox);
  EXPECT_NEAR(aDist3, 1.0, Precision::Confusion());

  // Point at corner outside (distance^2 = 1 + 1 + 1 = 3)
  Standard_Real aDist4 =
    BVH_Tools<Standard_Real, 3>::PointBoxSquareDistance(BVH_Vec3d(2.0, 2.0, 2.0), aBox);
  EXPECT_NEAR(aDist4, 3.0, Precision::Confusion());
}

TEST(BVH_ToolsTest, BoxBoxSquareDistance)
{
  BVH_Box<Standard_Real, 3> aBox1(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));
  BVH_Box<Standard_Real, 3> aBox2(BVH_Vec3d(2.0, 0.0, 0.0), BVH_Vec3d(3.0, 1.0, 1.0));
  BVH_Box<Standard_Real, 3> aBox3(BVH_Vec3d(0.5, 0.5, 0.5), BVH_Vec3d(1.5, 1.5, 1.5));

  // Separated boxes
  Standard_Real aDist1 = BVH_Tools<Standard_Real, 3>::BoxBoxSquareDistance(aBox1, aBox2);
  EXPECT_NEAR(aDist1, 1.0, Precision::Confusion());

  // Overlapping boxes
  Standard_Real aDist2 = BVH_Tools<Standard_Real, 3>::BoxBoxSquareDistance(aBox1, aBox3);
  EXPECT_NEAR(aDist2, 0.0, Precision::Confusion());
}

TEST(BVH_ToolsTest, PointBoxProjection)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));

  // Point inside - should return same point
  BVH_Vec3d aProj1 =
    BVH_Tools<Standard_Real, 3>::PointBoxProjection(BVH_Vec3d(0.5, 0.5, 0.5), aBox);
  EXPECT_NEAR(aProj1.x(), 0.5, Precision::Confusion());
  EXPECT_NEAR(aProj1.y(), 0.5, Precision::Confusion());
  EXPECT_NEAR(aProj1.z(), 0.5, Precision::Confusion());

  // Point outside - should clamp to box
  BVH_Vec3d aProj2 =
    BVH_Tools<Standard_Real, 3>::PointBoxProjection(BVH_Vec3d(2.0, 0.5, 0.5), aBox);
  EXPECT_NEAR(aProj2.x(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aProj2.y(), 0.5, Precision::Confusion());
  EXPECT_NEAR(aProj2.z(), 0.5, Precision::Confusion());
}

TEST(BVH_ToolsTest, RayBoxIntersection)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));

  Standard_Real aTimeEnter, aTimeLeave;

  // Ray hitting the box
  Standard_Boolean aHit1 =
    BVH_Tools<Standard_Real, 3>::RayBoxIntersection(BVH_Vec3d(-1.0, 0.5, 0.5),
                                                    BVH_Vec3d(1.0, 0.0, 0.0),
                                                    aBox,
                                                    aTimeEnter,
                                                    aTimeLeave);
  EXPECT_TRUE(aHit1);
  EXPECT_NEAR(aTimeEnter, 1.0, Precision::Confusion());
  EXPECT_NEAR(aTimeLeave, 2.0, Precision::Confusion());

  // Ray missing the box
  Standard_Boolean aHit2 =
    BVH_Tools<Standard_Real, 3>::RayBoxIntersection(BVH_Vec3d(-1.0, 5.0, 0.5),
                                                    BVH_Vec3d(1.0, 0.0, 0.0),
                                                    aBox,
                                                    aTimeEnter,
                                                    aTimeLeave);
  EXPECT_FALSE(aHit2);

  // Ray starting inside box
  Standard_Boolean aHit3 = BVH_Tools<Standard_Real, 3>::RayBoxIntersection(BVH_Vec3d(0.5, 0.5, 0.5),
                                                                           BVH_Vec3d(1.0, 0.0, 0.0),
                                                                           aBox,
                                                                           aTimeEnter,
                                                                           aTimeLeave);
  EXPECT_TRUE(aHit3);
  EXPECT_LE(aTimeEnter, 0.0);
  EXPECT_NEAR(aTimeLeave, 0.5, Precision::Confusion());
}

TEST(BVH_ToolsTest, RayBoxIntersectionParallelRay)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));

  Standard_Real aTimeEnter, aTimeLeave;

  // Ray parallel to X-axis, passing through box
  Standard_Boolean aHit1 =
    BVH_Tools<Standard_Real, 3>::RayBoxIntersection(BVH_Vec3d(-1.0, 0.5, 0.5),
                                                    BVH_Vec3d(1.0, 0.0, 0.0),
                                                    aBox,
                                                    aTimeEnter,
                                                    aTimeLeave);
  EXPECT_TRUE(aHit1);

  // Ray parallel to X-axis, missing box (Y out of range)
  Standard_Boolean aHit2 =
    BVH_Tools<Standard_Real, 3>::RayBoxIntersection(BVH_Vec3d(-1.0, 2.0, 0.5),
                                                    BVH_Vec3d(1.0, 0.0, 0.0),
                                                    aBox,
                                                    aTimeEnter,
                                                    aTimeLeave);
  EXPECT_FALSE(aHit2);
}

TEST(BVH_ToolsTest, PointTriangleProjection)
{
  BVH_Vec3d aNode0(0.0, 0.0, 0.0);
  BVH_Vec3d aNode1(1.0, 0.0, 0.0);
  BVH_Vec3d aNode2(0.0, 1.0, 0.0);

  // Point projects to vertex
  BVH_Vec3d aProj1 =
    BVH_Tools<Standard_Real, 3>::PointTriangleProjection(BVH_Vec3d(-1.0, -1.0, 0.0),
                                                         aNode0,
                                                         aNode1,
                                                         aNode2);
  EXPECT_NEAR(aProj1.x(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aProj1.y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aProj1.z(), 0.0, Precision::Confusion());

  // Point projects to edge
  BVH_Vec3d aProj2 = BVH_Tools<Standard_Real, 3>::PointTriangleProjection(BVH_Vec3d(0.5, -1.0, 0.0),
                                                                          aNode0,
                                                                          aNode1,
                                                                          aNode2);
  EXPECT_NEAR(aProj2.x(), 0.5, Precision::Confusion());
  EXPECT_NEAR(aProj2.y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aProj2.z(), 0.0, Precision::Confusion());

  // Point projects inside triangle
  BVH_Vec3d aProj3 =
    BVH_Tools<Standard_Real, 3>::PointTriangleProjection(BVH_Vec3d(0.25, 0.25, 1.0),
                                                         aNode0,
                                                         aNode1,
                                                         aNode2);
  EXPECT_NEAR(aProj3.x(), 0.25, Precision::Confusion());
  EXPECT_NEAR(aProj3.y(), 0.25, Precision::Confusion());
  EXPECT_NEAR(aProj3.z(), 0.0, Precision::Confusion());
}

TEST(BVH_ToolsTest, PointBoxSquareDistance2D)
{
  BVH_Box<Standard_Real, 2> aBox(BVH_Vec2d(0.0, 0.0), BVH_Vec2d(1.0, 1.0));

  // Point inside box
  Standard_Real aDist1 =
    BVH_Tools<Standard_Real, 2>::PointBoxSquareDistance(BVH_Vec2d(0.5, 0.5), aBox);
  EXPECT_NEAR(aDist1, 0.0, Precision::Confusion());

  // Point outside box
  Standard_Real aDist2 =
    BVH_Tools<Standard_Real, 2>::PointBoxSquareDistance(BVH_Vec2d(2.0, 0.5), aBox);
  EXPECT_NEAR(aDist2, 1.0, Precision::Confusion());
}

TEST(BVH_ToolsTest, RayBoxIntersectionDiagonalRay)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));

  Standard_Real aTimeEnter, aTimeLeave;

  // Diagonal ray through box center
  BVH_Vec3d     aDir(1.0, 1.0, 1.0);
  Standard_Real aNorm = std::sqrt(3.0);
  aDir                = BVH_Vec3d(aDir.x() / aNorm, aDir.y() / aNorm, aDir.z() / aNorm);

  Standard_Boolean aHit =
    BVH_Tools<Standard_Real, 3>::RayBoxIntersection(BVH_Vec3d(-1.0, -1.0, -1.0),
                                                    aDir,
                                                    aBox,
                                                    aTimeEnter,
                                                    aTimeLeave);
  EXPECT_TRUE(aHit);
  EXPECT_GT(aTimeEnter, 0.0);
  EXPECT_GT(aTimeLeave, aTimeEnter);
}

TEST(BVH_ToolsTest, RayBoxIntersectionNegativeDirection)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));

  Standard_Real aTimeEnter, aTimeLeave;

  // Ray going in negative X direction
  Standard_Boolean aHit = BVH_Tools<Standard_Real, 3>::RayBoxIntersection(BVH_Vec3d(2.0, 0.5, 0.5),
                                                                          BVH_Vec3d(-1.0, 0.0, 0.0),
                                                                          aBox,
                                                                          aTimeEnter,
                                                                          aTimeLeave);
  EXPECT_TRUE(aHit);
  EXPECT_NEAR(aTimeEnter, 1.0, Precision::Confusion());
  EXPECT_NEAR(aTimeLeave, 2.0, Precision::Confusion());
}

TEST(BVH_ToolsTest, RayBoxIntersectionTouchingEdge)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));

  Standard_Real aTimeEnter, aTimeLeave;

  // Ray touching edge of box
  Standard_Boolean aHit = BVH_Tools<Standard_Real, 3>::RayBoxIntersection(BVH_Vec3d(-1.0, 0.0, 0.0),
                                                                          BVH_Vec3d(1.0, 0.0, 0.0),
                                                                          aBox,
                                                                          aTimeEnter,
                                                                          aTimeLeave);
  EXPECT_TRUE(aHit);
}

TEST(BVH_ToolsTest, RayBoxIntersectionTouchingCorner)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));

  Standard_Real aTimeEnter, aTimeLeave;

  // Ray through corner
  BVH_Vec3d     aDir(1.0, 1.0, 1.0);
  Standard_Real aNorm = std::sqrt(3.0);
  aDir                = BVH_Vec3d(aDir.x() / aNorm, aDir.y() / aNorm, aDir.z() / aNorm);

  Standard_Boolean aHit =
    BVH_Tools<Standard_Real, 3>::RayBoxIntersection(BVH_Vec3d(-1.0, -1.0, -1.0),
                                                    aDir,
                                                    aBox,
                                                    aTimeEnter,
                                                    aTimeLeave);
  EXPECT_TRUE(aHit);
}

TEST(BVH_ToolsTest, BoxBoxSquareDistanceTouching)
{
  BVH_Box<Standard_Real, 3> aBox1(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));
  BVH_Box<Standard_Real, 3> aBox2(BVH_Vec3d(1.0, 0.0, 0.0), BVH_Vec3d(2.0, 1.0, 1.0));

  // Touching boxes (sharing a face)
  Standard_Real aDist = BVH_Tools<Standard_Real, 3>::BoxBoxSquareDistance(aBox1, aBox2);
  EXPECT_NEAR(aDist, 0.0, Precision::Confusion());
}

TEST(BVH_ToolsTest, BoxBoxSquareDistanceOneInsideOther)
{
  BVH_Box<Standard_Real, 3> aBox1(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(10.0, 10.0, 10.0));
  BVH_Box<Standard_Real, 3> aBox2(BVH_Vec3d(2.0, 2.0, 2.0), BVH_Vec3d(3.0, 3.0, 3.0));

  // Small box inside large box
  Standard_Real aDist = BVH_Tools<Standard_Real, 3>::BoxBoxSquareDistance(aBox1, aBox2);
  EXPECT_NEAR(aDist, 0.0, Precision::Confusion());
}

TEST(BVH_ToolsTest, BoxBoxSquareDistanceCornerToCorner)
{
  BVH_Box<Standard_Real, 3> aBox1(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));
  BVH_Box<Standard_Real, 3> aBox2(BVH_Vec3d(2.0, 2.0, 2.0), BVH_Vec3d(3.0, 3.0, 3.0));

  // Distance from corner to corner: sqrt(1^2 + 1^2 + 1^2) = sqrt(3), squared = 3
  Standard_Real aDist = BVH_Tools<Standard_Real, 3>::BoxBoxSquareDistance(aBox1, aBox2);
  EXPECT_NEAR(aDist, 3.0, Precision::Confusion());
}

TEST(BVH_ToolsTest, PointBoxProjectionNegativeCoords)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(-1.0, -1.0, -1.0), BVH_Vec3d(1.0, 1.0, 1.0));

  // Point far outside in negative direction
  BVH_Vec3d aProj =
    BVH_Tools<Standard_Real, 3>::PointBoxProjection(BVH_Vec3d(-5.0, -5.0, -5.0), aBox);
  EXPECT_NEAR(aProj.x(), -1.0, Precision::Confusion());
  EXPECT_NEAR(aProj.y(), -1.0, Precision::Confusion());
  EXPECT_NEAR(aProj.z(), -1.0, Precision::Confusion());
}

TEST(BVH_ToolsTest, PointTriangleProjectionOnEdge01)
{
  BVH_Vec3d aNode0(0.0, 0.0, 0.0);
  BVH_Vec3d aNode1(2.0, 0.0, 0.0);
  BVH_Vec3d aNode2(0.0, 2.0, 0.0);

  // Point projects onto edge between Node0 and Node1
  BVH_Vec3d aProj = BVH_Tools<Standard_Real, 3>::PointTriangleProjection(BVH_Vec3d(1.0, -1.0, 0.0),
                                                                         aNode0,
                                                                         aNode1,
                                                                         aNode2);
  EXPECT_NEAR(aProj.x(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aProj.y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aProj.z(), 0.0, Precision::Confusion());
}

TEST(BVH_ToolsTest, PointTriangleProjectionOnEdge12)
{
  BVH_Vec3d aNode0(0.0, 0.0, 0.0);
  BVH_Vec3d aNode1(2.0, 0.0, 0.0);
  BVH_Vec3d aNode2(0.0, 2.0, 0.0);

  // Point projects onto edge between Node1 and Node2
  BVH_Vec3d aProj = BVH_Tools<Standard_Real, 3>::PointTriangleProjection(BVH_Vec3d(2.0, 2.0, 0.0),
                                                                         aNode0,
                                                                         aNode1,
                                                                         aNode2);
  EXPECT_NEAR(aProj.x(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aProj.y(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aProj.z(), 0.0, Precision::Confusion());
}

TEST(BVH_ToolsTest, PointTriangleProjectionOnEdge20)
{
  BVH_Vec3d aNode0(0.0, 0.0, 0.0);
  BVH_Vec3d aNode1(2.0, 0.0, 0.0);
  BVH_Vec3d aNode2(0.0, 2.0, 0.0);

  // Point projects onto edge between Node2 and Node0
  BVH_Vec3d aProj = BVH_Tools<Standard_Real, 3>::PointTriangleProjection(BVH_Vec3d(-1.0, 1.0, 0.0),
                                                                         aNode0,
                                                                         aNode1,
                                                                         aNode2);
  EXPECT_NEAR(aProj.x(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aProj.y(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aProj.z(), 0.0, Precision::Confusion());
}

TEST(BVH_ToolsTest, PointTriangleProjectionVertex1)
{
  BVH_Vec3d aNode0(0.0, 0.0, 0.0);
  BVH_Vec3d aNode1(2.0, 0.0, 0.0);
  BVH_Vec3d aNode2(0.0, 2.0, 0.0);

  // Point projects to Node1
  BVH_Vec3d aProj = BVH_Tools<Standard_Real, 3>::PointTriangleProjection(BVH_Vec3d(3.0, -1.0, 0.0),
                                                                         aNode0,
                                                                         aNode1,
                                                                         aNode2);
  EXPECT_NEAR(aProj.x(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aProj.y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aProj.z(), 0.0, Precision::Confusion());
}

TEST(BVH_ToolsTest, PointTriangleProjectionVertex2)
{
  BVH_Vec3d aNode0(0.0, 0.0, 0.0);
  BVH_Vec3d aNode1(2.0, 0.0, 0.0);
  BVH_Vec3d aNode2(0.0, 2.0, 0.0);

  // Point projects to Node2
  BVH_Vec3d aProj = BVH_Tools<Standard_Real, 3>::PointTriangleProjection(BVH_Vec3d(-1.0, 3.0, 0.0),
                                                                         aNode0,
                                                                         aNode1,
                                                                         aNode2);
  EXPECT_NEAR(aProj.x(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aProj.y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aProj.z(), 0.0, Precision::Confusion());
}

TEST(BVH_ToolsTest, PointTriangleProjection3D)
{
  // Triangle in 3D space (not in XY plane)
  BVH_Vec3d aNode0(0.0, 0.0, 0.0);
  BVH_Vec3d aNode1(1.0, 0.0, 0.0);
  BVH_Vec3d aNode2(0.0, 1.0, 1.0);

  // Point above triangle center
  BVH_Vec3d aProj = BVH_Tools<Standard_Real, 3>::PointTriangleProjection(BVH_Vec3d(0.3, 0.3, 0.8),
                                                                         aNode0,
                                                                         aNode1,
                                                                         aNode2);

  // Should project inside triangle
  EXPECT_GE(aProj.x(), 0.0);
  EXPECT_GE(aProj.y(), 0.0);
  EXPECT_LE(aProj.x() + aProj.y(), 1.0 + Precision::Confusion());
}

TEST(BVH_ToolsTest, BoxBoxSquareDistance2D)
{
  BVH_Box<Standard_Real, 2> aBox1(BVH_Vec2d(0.0, 0.0), BVH_Vec2d(1.0, 1.0));
  BVH_Box<Standard_Real, 2> aBox2(BVH_Vec2d(3.0, 0.0), BVH_Vec2d(4.0, 1.0));

  // Distance = 2
  Standard_Real aDist = BVH_Tools<Standard_Real, 2>::BoxBoxSquareDistance(aBox1, aBox2);
  EXPECT_NEAR(aDist, 4.0, Precision::Confusion());
}

TEST(BVH_ToolsTest, PointBoxProjection2D)
{
  BVH_Box<Standard_Real, 2> aBox(BVH_Vec2d(0.0, 0.0), BVH_Vec2d(1.0, 1.0));

  // Point outside
  BVH_Vec2d aProj = BVH_Tools<Standard_Real, 2>::PointBoxProjection(BVH_Vec2d(2.0, 2.0), aBox);
  EXPECT_NEAR(aProj.x(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aProj.y(), 1.0, Precision::Confusion());
}

TEST(BVH_ToolsTest, PointBoxSquareDistanceAtVertex)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));

  // Point on vertex
  Standard_Real aDist =
    BVH_Tools<Standard_Real, 3>::PointBoxSquareDistance(BVH_Vec3d(0.0, 0.0, 0.0), aBox);
  EXPECT_NEAR(aDist, 0.0, Precision::Confusion());
}

TEST(BVH_ToolsTest, PointBoxSquareDistanceAtEdge)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));

  // Point on edge
  Standard_Real aDist =
    BVH_Tools<Standard_Real, 3>::PointBoxSquareDistance(BVH_Vec3d(0.5, 0.0, 0.0), aBox);
  EXPECT_NEAR(aDist, 0.0, Precision::Confusion());
}

TEST(BVH_ToolsTest, PointBoxSquareDistanceFloat)
{
  BVH_Box<Standard_ShortReal, 3> aBox(BVH_Vec3f(0.0f, 0.0f, 0.0f), BVH_Vec3f(1.0f, 1.0f, 1.0f));

  // Point inside box
  Standard_ShortReal aDist1 =
    BVH_Tools<Standard_ShortReal, 3>::PointBoxSquareDistance(BVH_Vec3f(0.5f, 0.5f, 0.5f), aBox);
  EXPECT_NEAR(aDist1, 0.0f, 1e-5f);

  // Point outside box
  Standard_ShortReal aDist2 =
    BVH_Tools<Standard_ShortReal, 3>::PointBoxSquareDistance(BVH_Vec3f(2.0f, 0.5f, 0.5f), aBox);
  EXPECT_NEAR(aDist2, 1.0f, 1e-5f);
}

TEST(BVH_ToolsTest, RayBoxIntersectionFloat)
{
  BVH_Box<Standard_ShortReal, 3> aBox(BVH_Vec3f(0.0f, 0.0f, 0.0f), BVH_Vec3f(1.0f, 1.0f, 1.0f));

  Standard_ShortReal aTimeEnter, aTimeLeave;

  // Ray hitting the box
  Standard_Boolean aHit =
    BVH_Tools<Standard_ShortReal, 3>::RayBoxIntersection(BVH_Vec3f(-1.0f, 0.5f, 0.5f),
                                                         BVH_Vec3f(1.0f, 0.0f, 0.0f),
                                                         aBox,
                                                         aTimeEnter,
                                                         aTimeLeave);
  EXPECT_TRUE(aHit);
  EXPECT_NEAR(aTimeEnter, 1.0f, 1e-5f);
  EXPECT_NEAR(aTimeLeave, 2.0f, 1e-5f);
}

TEST(BVH_ToolsTest, RayBoxIntersectionBehindRay)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));

  Standard_Real aTimeEnter, aTimeLeave;

  // Ray pointing away from box
  Standard_Boolean aHit = BVH_Tools<Standard_Real, 3>::RayBoxIntersection(BVH_Vec3d(2.0, 0.5, 0.5),
                                                                          BVH_Vec3d(1.0, 0.0, 0.0),
                                                                          aBox,
                                                                          aTimeEnter,
                                                                          aTimeLeave);
  // The box is behind the ray origin
  EXPECT_TRUE(aTimeLeave < 0.0 || !aHit);
}

TEST(BVH_ToolsTest, RayBoxIntersectionYAxis)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));

  Standard_Real aTimeEnter, aTimeLeave;

  // Ray along Y axis
  Standard_Boolean aHit = BVH_Tools<Standard_Real, 3>::RayBoxIntersection(BVH_Vec3d(0.5, -1.0, 0.5),
                                                                          BVH_Vec3d(0.0, 1.0, 0.0),
                                                                          aBox,
                                                                          aTimeEnter,
                                                                          aTimeLeave);
  EXPECT_TRUE(aHit);
  EXPECT_NEAR(aTimeEnter, 1.0, Precision::Confusion());
  EXPECT_NEAR(aTimeLeave, 2.0, Precision::Confusion());
}

TEST(BVH_ToolsTest, RayBoxIntersectionZAxis)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));

  Standard_Real aTimeEnter, aTimeLeave;

  // Ray along Z axis
  Standard_Boolean aHit = BVH_Tools<Standard_Real, 3>::RayBoxIntersection(BVH_Vec3d(0.5, 0.5, -1.0),
                                                                          BVH_Vec3d(0.0, 0.0, 1.0),
                                                                          aBox,
                                                                          aTimeEnter,
                                                                          aTimeLeave);
  EXPECT_TRUE(aHit);
  EXPECT_NEAR(aTimeEnter, 1.0, Precision::Confusion());
  EXPECT_NEAR(aTimeLeave, 2.0, Precision::Confusion());
}

TEST(BVH_ToolsTest, BoxBoxSquareDistanceFloat)
{
  BVH_Box<Standard_ShortReal, 3> aBox1(BVH_Vec3f(0.0f, 0.0f, 0.0f), BVH_Vec3f(1.0f, 1.0f, 1.0f));
  BVH_Box<Standard_ShortReal, 3> aBox2(BVH_Vec3f(2.0f, 0.0f, 0.0f), BVH_Vec3f(3.0f, 1.0f, 1.0f));

  Standard_ShortReal aDist = BVH_Tools<Standard_ShortReal, 3>::BoxBoxSquareDistance(aBox1, aBox2);
  EXPECT_NEAR(aDist, 1.0f, 1e-5f);
}

TEST(BVH_ToolsTest, PointBoxProjectionFloat)
{
  BVH_Box<Standard_ShortReal, 3> aBox(BVH_Vec3f(0.0f, 0.0f, 0.0f), BVH_Vec3f(1.0f, 1.0f, 1.0f));

  BVH_Vec3f aProj =
    BVH_Tools<Standard_ShortReal, 3>::PointBoxProjection(BVH_Vec3f(2.0f, 0.5f, 0.5f), aBox);
  EXPECT_NEAR(aProj.x(), 1.0f, 1e-5f);
  EXPECT_NEAR(aProj.y(), 0.5f, 1e-5f);
  EXPECT_NEAR(aProj.z(), 0.5f, 1e-5f);
}

TEST(BVH_ToolsTest, PointBoxSquareDistanceNegativeBox)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(-2.0, -2.0, -2.0), BVH_Vec3d(-1.0, -1.0, -1.0));

  // Point at origin
  Standard_Real aDist =
    BVH_Tools<Standard_Real, 3>::PointBoxSquareDistance(BVH_Vec3d(0.0, 0.0, 0.0), aBox);
  // Distance = sqrt(1^2 + 1^2 + 1^2) = sqrt(3), squared = 3
  EXPECT_NEAR(aDist, 3.0, Precision::Confusion());
}

TEST(BVH_ToolsTest, BoxBoxSquareDistanceEdgeToEdge)
{
  BVH_Box<Standard_Real, 3> aBox1(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));
  BVH_Box<Standard_Real, 3> aBox2(BVH_Vec3d(2.0, 2.0, 0.0), BVH_Vec3d(3.0, 3.0, 1.0));

  // Closest points are on edges, distance = sqrt(1^2 + 1^2) = sqrt(2), squared = 2
  Standard_Real aDist = BVH_Tools<Standard_Real, 3>::BoxBoxSquareDistance(aBox1, aBox2);
  EXPECT_NEAR(aDist, 2.0, Precision::Confusion());
}

TEST(BVH_ToolsTest, PointTriangleProjectionFloat)
{
  BVH_Vec3f aNode0(0.0f, 0.0f, 0.0f);
  BVH_Vec3f aNode1(1.0f, 0.0f, 0.0f);
  BVH_Vec3f aNode2(0.0f, 1.0f, 0.0f);

  // Point projects inside triangle
  BVH_Vec3f aProj =
    BVH_Tools<Standard_ShortReal, 3>::PointTriangleProjection(BVH_Vec3f(0.25f, 0.25f, 1.0f),
                                                              aNode0,
                                                              aNode1,
                                                              aNode2);
  EXPECT_NEAR(aProj.x(), 0.25f, 1e-5f);
  EXPECT_NEAR(aProj.y(), 0.25f, 1e-5f);
  EXPECT_NEAR(aProj.z(), 0.0f, 1e-5f);
}

TEST(BVH_ToolsTest, PointBoxSquareDistanceLargeBox)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1000.0, 1000.0, 1000.0));

  // Point inside
  Standard_Real aDist1 =
    BVH_Tools<Standard_Real, 3>::PointBoxSquareDistance(BVH_Vec3d(500.0, 500.0, 500.0), aBox);
  EXPECT_NEAR(aDist1, 0.0, Precision::Confusion());

  // Point outside
  Standard_Real aDist2 =
    BVH_Tools<Standard_Real, 3>::PointBoxSquareDistance(BVH_Vec3d(1001.0, 500.0, 500.0), aBox);
  EXPECT_NEAR(aDist2, 1.0, Precision::Confusion());
}

TEST(BVH_ToolsTest, RayBoxIntersectionLargeBox)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1000.0, 1000.0, 1000.0));

  Standard_Real aTimeEnter, aTimeLeave;

  Standard_Boolean aHit =
    BVH_Tools<Standard_Real, 3>::RayBoxIntersection(BVH_Vec3d(-100.0, 500.0, 500.0),
                                                    BVH_Vec3d(1.0, 0.0, 0.0),
                                                    aBox,
                                                    aTimeEnter,
                                                    aTimeLeave);
  EXPECT_TRUE(aHit);
  EXPECT_NEAR(aTimeEnter, 100.0, Precision::Confusion());
  EXPECT_NEAR(aTimeLeave, 1100.0, Precision::Confusion());
}

TEST(BVH_ToolsTest, PointTriangleProjectionCentroid)
{
  BVH_Vec3d aNode0(0.0, 0.0, 0.0);
  BVH_Vec3d aNode1(3.0, 0.0, 0.0);
  BVH_Vec3d aNode2(0.0, 3.0, 0.0);

  // Point directly above centroid
  BVH_Vec3d aProj = BVH_Tools<Standard_Real, 3>::PointTriangleProjection(BVH_Vec3d(1.0, 1.0, 5.0),
                                                                         aNode0,
                                                                         aNode1,
                                                                         aNode2);
  EXPECT_NEAR(aProj.x(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aProj.y(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aProj.z(), 0.0, Precision::Confusion());
}

TEST(BVH_ToolsTest, BoxBoxSquareDistanceSameBox)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));

  // Distance to itself should be 0
  Standard_Real aDist = BVH_Tools<Standard_Real, 3>::BoxBoxSquareDistance(aBox, aBox);
  EXPECT_NEAR(aDist, 0.0, Precision::Confusion());
}

TEST(BVH_ToolsTest, PointBoxProjectionAllCorners)
{
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));

  // Test projection from each octant
  BVH_Vec3d aProj1 =
    BVH_Tools<Standard_Real, 3>::PointBoxProjection(BVH_Vec3d(-1.0, -1.0, -1.0), aBox);
  EXPECT_NEAR(aProj1.x(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aProj1.y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aProj1.z(), 0.0, Precision::Confusion());

  BVH_Vec3d aProj2 =
    BVH_Tools<Standard_Real, 3>::PointBoxProjection(BVH_Vec3d(2.0, 2.0, 2.0), aBox);
  EXPECT_NEAR(aProj2.x(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aProj2.y(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aProj2.z(), 1.0, Precision::Confusion());
}

// =======================================================================================
// Tests for improved RayBoxIntersection (dimension-independent, early exit)
// =======================================================================================

TEST(BVH_ToolsTest, RayBoxIntersection2D)
{
  // Test 2D ray-box intersection (old code was hardcoded for 3D)
  BVH_Box<Standard_Real, 2> aBox(BVH_Vec2d(0.0, 0.0), BVH_Vec2d(1.0, 1.0));

  Standard_Real aTimeEnter, aTimeLeave;

  // Ray hitting the box
  Standard_Boolean aHit1 = BVH_Tools<Standard_Real, 2>::RayBoxIntersection(BVH_Vec2d(-1.0, 0.5),
                                                                           BVH_Vec2d(1.0, 0.0),
                                                                           aBox,
                                                                           aTimeEnter,
                                                                           aTimeLeave);
  EXPECT_TRUE(aHit1);
  EXPECT_NEAR(aTimeEnter, 1.0, Precision::Confusion());
  EXPECT_NEAR(aTimeLeave, 2.0, Precision::Confusion());

  // Ray missing the box
  Standard_Boolean aHit2 = BVH_Tools<Standard_Real, 2>::RayBoxIntersection(BVH_Vec2d(-1.0, 2.0),
                                                                           BVH_Vec2d(1.0, 0.0),
                                                                           aBox,
                                                                           aTimeEnter,
                                                                           aTimeLeave);
  EXPECT_FALSE(aHit2);

  // Ray parallel to X axis, inside Y bounds
  Standard_Boolean aHit3 = BVH_Tools<Standard_Real, 2>::RayBoxIntersection(BVH_Vec2d(-1.0, 0.5),
                                                                           BVH_Vec2d(1.0, 0.0),
                                                                           aBox,
                                                                           aTimeEnter,
                                                                           aTimeLeave);
  EXPECT_TRUE(aHit3);

  // Ray parallel to Y axis, inside X bounds
  Standard_Boolean aHit4 = BVH_Tools<Standard_Real, 2>::RayBoxIntersection(BVH_Vec2d(0.5, -1.0),
                                                                           BVH_Vec2d(0.0, 1.0),
                                                                           aBox,
                                                                           aTimeEnter,
                                                                           aTimeLeave);
  EXPECT_TRUE(aHit4);
  EXPECT_NEAR(aTimeEnter, 1.0, Precision::Confusion());
  EXPECT_NEAR(aTimeLeave, 2.0, Precision::Confusion());
}

TEST(BVH_ToolsTest, RayBoxIntersectionEarlyExit)
{
  // Test early exit optimization when ray misses on first axis
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));
  Standard_Real             aTimeEnter, aTimeLeave;

  // Ray clearly misses in X direction - should exit immediately
  Standard_Boolean aHit = BVH_Tools<Standard_Real, 3>::RayBoxIntersection(BVH_Vec3d(-2.0, 0.5, 0.5),
                                                                          BVH_Vec3d(-1.0, 0.0, 0.0),
                                                                          aBox,
                                                                          aTimeEnter,
                                                                          aTimeLeave);
  EXPECT_FALSE(aHit);

  // Ray with early mismatch in Y direction
  Standard_Boolean aHit2 =
    BVH_Tools<Standard_Real, 3>::RayBoxIntersection(BVH_Vec3d(0.5, -2.0, 0.5),
                                                    BVH_Vec3d(0.0, -1.0, 0.0),
                                                    aBox,
                                                    aTimeEnter,
                                                    aTimeLeave);
  EXPECT_FALSE(aHit2);
}

TEST(BVH_ToolsTest, RayBoxIntersectionParallelRayEarlyExit)
{
  // Test parallel ray that misses - should exit immediately
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));
  Standard_Real             aTimeEnter, aTimeLeave;

  // Ray parallel to X axis but Y coordinate outside box - should reject immediately
  Standard_Boolean aHit = BVH_Tools<Standard_Real, 3>::RayBoxIntersection(BVH_Vec3d(-1.0, 2.0, 0.5),
                                                                          BVH_Vec3d(1.0, 0.0, 0.0),
                                                                          aBox,
                                                                          aTimeEnter,
                                                                          aTimeLeave);
  EXPECT_FALSE(aHit);

  // Ray parallel to Y axis but X coordinate outside box
  Standard_Boolean aHit2 =
    BVH_Tools<Standard_Real, 3>::RayBoxIntersection(BVH_Vec3d(2.0, -1.0, 0.5),
                                                    BVH_Vec3d(0.0, 1.0, 0.0),
                                                    aBox,
                                                    aTimeEnter,
                                                    aTimeLeave);
  EXPECT_FALSE(aHit2);

  // Ray parallel to Z axis but X and Y outside
  Standard_Boolean aHit3 =
    BVH_Tools<Standard_Real, 3>::RayBoxIntersection(BVH_Vec3d(-1.0, -1.0, 0.5),
                                                    BVH_Vec3d(0.0, 0.0, 1.0),
                                                    aBox,
                                                    aTimeEnter,
                                                    aTimeLeave);
  EXPECT_FALSE(aHit3);
}

TEST(BVH_ToolsTest, RayBoxIntersection2DParallelBothAxes)
{
  // 2D test with ray parallel to both axes (direction = 0,0)
  BVH_Box<Standard_Real, 2> aBox(BVH_Vec2d(0.0, 0.0), BVH_Vec2d(1.0, 1.0));
  Standard_Real             aTimeEnter, aTimeLeave;

  // Ray origin inside box, direction = 0
  Standard_Boolean aHit1 = BVH_Tools<Standard_Real, 2>::RayBoxIntersection(BVH_Vec2d(0.5, 0.5),
                                                                           BVH_Vec2d(0.0, 0.0),
                                                                           aBox,
                                                                           aTimeEnter,
                                                                           aTimeLeave);
  EXPECT_TRUE(aHit1); // Should still hit because origin is inside

  // Ray origin outside box, direction = 0
  Standard_Boolean aHit2 = BVH_Tools<Standard_Real, 2>::RayBoxIntersection(BVH_Vec2d(2.0, 2.0),
                                                                           BVH_Vec2d(0.0, 0.0),
                                                                           aBox,
                                                                           aTimeEnter,
                                                                           aTimeLeave);
  EXPECT_FALSE(aHit2); // Should miss because origin is outside
}

TEST(BVH_ToolsTest, RayBoxIntersectionNegativeTime)
{
  // Test that ray doesn't report intersection behind the origin
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(1.0, 1.0, 1.0));
  Standard_Real             aTimeEnter, aTimeLeave;

  // Ray origin is past the box, pointing away
  Standard_Boolean aHit = BVH_Tools<Standard_Real, 3>::RayBoxIntersection(BVH_Vec3d(2.0, 0.5, 0.5),
                                                                          BVH_Vec3d(1.0, 0.0, 0.0),
                                                                          aBox,
                                                                          aTimeEnter,
                                                                          aTimeLeave);
  EXPECT_FALSE(aHit);

  // Ray origin inside box, pointing away - should still hit (leave point is in front)
  Standard_Boolean aHit2 = BVH_Tools<Standard_Real, 3>::RayBoxIntersection(BVH_Vec3d(0.5, 0.5, 0.5),
                                                                           BVH_Vec3d(1.0, 0.0, 0.0),
                                                                           aBox,
                                                                           aTimeEnter,
                                                                           aTimeLeave);
  EXPECT_TRUE(aHit2);
  EXPECT_TRUE(aTimeLeave >= 0.0);
}
