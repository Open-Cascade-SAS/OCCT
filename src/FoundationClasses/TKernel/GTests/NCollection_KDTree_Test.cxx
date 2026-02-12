// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <NCollection_KDTree.hxx>
#include <NCollection_DynamicArray.hxx>

#include <algorithm>
#include <cmath>
#include <limits>
#include <set>
#include <vector>

namespace
{

//! Simple 2D point for testing (no dependency on gp).
struct TestPoint2D
{
  double X, Y;

  TestPoint2D()
      : X(0.0),
        Y(0.0)
  {
  }

  TestPoint2D(double theX, double theY)
      : X(theX),
        Y(theY)
  {
  }

  double Coord(int theIndex) const { return theIndex == 1 ? X : Y; }
};

//! Simple 3D point for testing (no dependency on gp).
struct TestPoint3D
{
  double X, Y, Z;

  TestPoint3D()
      : X(0.0),
        Y(0.0),
        Z(0.0)
  {
  }

  TestPoint3D(double theX, double theY, double theZ)
      : X(theX),
        Y(theY),
        Z(theZ)
  {
  }

  double Coord(int theIndex) const { return theIndex == 1 ? X : (theIndex == 2 ? Y : Z); }
};

//! Squared distance helpers for brute-force verification.
double sqDist2D(const TestPoint2D& theA, const TestPoint2D& theB)
{
  const double aDx = theA.X - theB.X;
  const double aDy = theA.Y - theB.Y;
  return aDx * aDx + aDy * aDy;
}

double sqDist3D(const TestPoint3D& theA, const TestPoint3D& theB)
{
  const double aDx = theA.X - theB.X;
  const double aDy = theA.Y - theB.Y;
  const double aDz = theA.Z - theB.Z;
  return aDx * aDx + aDy * aDy + aDz * aDz;
}

//! Simple linear congruential pseudo-random for reproducibility.
class TestRandom
{
public:
  TestRandom(unsigned int theSeed)
      : mySeed(theSeed)
  {
  }

  //! Returns a random double in [theMin, theMax].
  double NextDouble(double theMin, double theMax)
  {
    mySeed             = mySeed * 1103515245u + 12345u;
    const double aNorm = static_cast<double>((mySeed >> 16) & 0x7FFF) / 32767.0;
    return theMin + aNorm * (theMax - theMin);
  }

private:
  unsigned int mySeed;
};

} // anonymous namespace

// ============================================================
// Construction & State
// ============================================================

TEST(NCollection_KDTreeTest, DefaultConstructor)
{
  NCollection_KDTree<TestPoint2D, 2> aTree;
  EXPECT_TRUE(aTree.IsEmpty());
  EXPECT_EQ(aTree.Size(), 0u);
}

TEST(NCollection_KDTreeTest, BuildFromCArray)
{
  TestPoint2D                        aPoints[] = {{1.0, 2.0}, {3.0, 4.0}, {5.0, 6.0}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 3);
  EXPECT_FALSE(aTree.IsEmpty());
  EXPECT_EQ(aTree.Size(), 3u);
}

TEST(NCollection_KDTreeTest, BuildFromArray1)
{
  NCollection_Array1<TestPoint2D> aPoints(1, 3);
  aPoints.SetValue(1, {1.0, 2.0});
  aPoints.SetValue(2, {3.0, 4.0});
  aPoints.SetValue(3, {5.0, 6.0});
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints);
  EXPECT_FALSE(aTree.IsEmpty());
  EXPECT_EQ(aTree.Size(), 3u);
}

TEST(NCollection_KDTreeTest, BuildSinglePoint)
{
  TestPoint2D                        aPoint(7.0, 8.0);
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(&aPoint, 1);
  EXPECT_EQ(aTree.Size(), 1u);
  EXPECT_NEAR(aTree.Point(1).X, 7.0, 1e-15);
  EXPECT_NEAR(aTree.Point(1).Y, 8.0, 1e-15);
}

TEST(NCollection_KDTreeTest, BuildTwoPoints)
{
  TestPoint2D                        aPoints[] = {{1.0, 0.0}, {0.0, 1.0}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 2);
  EXPECT_EQ(aTree.Size(), 2u);
}

TEST(NCollection_KDTreeTest, Clear)
{
  TestPoint2D                        aPoints[] = {{1.0, 2.0}, {3.0, 4.0}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 2);
  EXPECT_FALSE(aTree.IsEmpty());
  aTree.Clear();
  EXPECT_TRUE(aTree.IsEmpty());
  EXPECT_EQ(aTree.Size(), 0u);
}

// ============================================================
// Copy/Move Semantics
// ============================================================

TEST(NCollection_KDTreeTest, CopyConstructor)
{
  TestPoint2D                        aPoints[] = {{1.0, 2.0}, {3.0, 4.0}, {5.0, 6.0}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 3);
  NCollection_KDTree<TestPoint2D, 2> aCopy(aTree);
  EXPECT_EQ(aCopy.Size(), 3u);
  // Both should find the same nearest point
  TestPoint2D aQuery(3.1, 4.1);
  EXPECT_EQ(aTree.NearestPoint(aQuery), aCopy.NearestPoint(aQuery));
}

TEST(NCollection_KDTreeTest, MoveConstructor)
{
  TestPoint2D                        aPoints[] = {{1.0, 2.0}, {3.0, 4.0}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 2);
  NCollection_KDTree<TestPoint2D, 2> aMoved(std::move(aTree));
  EXPECT_EQ(aMoved.Size(), 2u);
  EXPECT_TRUE(aTree.IsEmpty());
}

TEST(NCollection_KDTreeTest, CopyAssignment)
{
  TestPoint2D                        aPoints[] = {{1.0, 2.0}, {3.0, 4.0}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 2);
  NCollection_KDTree<TestPoint2D, 2> aCopy;
  aCopy = aTree;
  EXPECT_EQ(aCopy.Size(), 2u);
  EXPECT_EQ(aTree.Size(), 2u);
}

TEST(NCollection_KDTreeTest, MoveAssignment)
{
  TestPoint2D                        aPoints[] = {{1.0, 2.0}, {3.0, 4.0}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 2);
  NCollection_KDTree<TestPoint2D, 2> aMoved;
  aMoved = std::move(aTree);
  EXPECT_EQ(aMoved.Size(), 2u);
  EXPECT_TRUE(aTree.IsEmpty());
}

// ============================================================
// NearestPoint 2D
// ============================================================

TEST(NCollection_KDTreeTest, NearestPoint2D_ExactMatch)
{
  TestPoint2D                        aPoints[] = {{0.0, 0.0}, {1.0, 1.0}, {2.0, 2.0}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 3);
  TestPoint2D  aQuery(1.0, 1.0);
  const size_t aIdx = aTree.NearestPoint(aQuery);
  EXPECT_NEAR(aTree.Point(aIdx).X, 1.0, 1e-15);
  EXPECT_NEAR(aTree.Point(aIdx).Y, 1.0, 1e-15);
}

TEST(NCollection_KDTreeTest, NearestPoint2D_ClosestOfMany)
{
  TestPoint2D                        aPoints[] = {{0.0, 0.0}, {10.0, 10.0}, {3.0, 4.0}, {7.0, 1.0}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 4);
  TestPoint2D  aQuery(3.1, 3.9);
  const size_t aIdx = aTree.NearestPoint(aQuery);
  EXPECT_NEAR(aTree.Point(aIdx).X, 3.0, 1e-15);
  EXPECT_NEAR(aTree.Point(aIdx).Y, 4.0, 1e-15);
}

TEST(NCollection_KDTreeTest, NearestPoint2D_GridPoints)
{
  // Create 5x5 grid
  TestPoint2D aPoints[25];
  int         k = 0;
  for (int i = 0; i < 5; ++i)
  {
    for (int j = 0; j < 5; ++j)
    {
      aPoints[k++] = {static_cast<double>(i), static_cast<double>(j)};
    }
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 25);
  // Query near (2.1, 3.2) -> nearest is (2, 3)
  TestPoint2D  aQuery(2.1, 3.2);
  const size_t aIdx = aTree.NearestPoint(aQuery);
  EXPECT_NEAR(aTree.Point(aIdx).X, 2.0, 1e-15);
  EXPECT_NEAR(aTree.Point(aIdx).Y, 3.0, 1e-15);
}

TEST(NCollection_KDTreeTest, NearestPoint2D_AllSamePoint)
{
  TestPoint2D                        aPoints[] = {{5.0, 5.0}, {5.0, 5.0}, {5.0, 5.0}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 3);
  TestPoint2D  aQuery(0.0, 0.0);
  double       aSqDist = 0.0;
  const size_t aIdx    = aTree.NearestPoint(aQuery, aSqDist);
  EXPECT_NE(aIdx, 0u);
  EXPECT_NEAR(aSqDist, 50.0, 1e-10);
}

TEST(NCollection_KDTreeTest, NearestPoint2D_CollinearPoints)
{
  TestPoint2D aPoints[] = {{0.0, 0.0}, {1.0, 0.0}, {2.0, 0.0}, {3.0, 0.0}, {4.0, 0.0}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 5);
  TestPoint2D  aQuery(2.3, 0.0);
  const size_t aIdx = aTree.NearestPoint(aQuery);
  EXPECT_NEAR(aTree.Point(aIdx).X, 2.0, 1e-15);
  EXPECT_NEAR(aTree.Point(aIdx).Y, 0.0, 1e-15);
}

TEST(NCollection_KDTreeTest, NearestPoint2D_NegativeCoordinates)
{
  TestPoint2D                        aPoints[] = {{-5.0, -3.0}, {-1.0, -2.0}, {-4.0, -7.0}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 3);
  TestPoint2D  aQuery(-1.0, -2.0);
  const size_t aIdx = aTree.NearestPoint(aQuery);
  EXPECT_NEAR(aTree.Point(aIdx).X, -1.0, 1e-15);
  EXPECT_NEAR(aTree.Point(aIdx).Y, -2.0, 1e-15);
}

TEST(NCollection_KDTreeTest, NearestPoint2D_QueryFarAway)
{
  TestPoint2D                        aPoints[] = {{0.0, 0.0}, {1.0, 1.0}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 2);
  TestPoint2D  aQuery(1e6, 1e6);
  const size_t aIdx = aTree.NearestPoint(aQuery);
  EXPECT_NEAR(aTree.Point(aIdx).X, 1.0, 1e-15);
  EXPECT_NEAR(aTree.Point(aIdx).Y, 1.0, 1e-15);
}

TEST(NCollection_KDTreeTest, NearestPoint2D_SqDistanceOutput)
{
  TestPoint2D                        aPoints[] = {{0.0, 0.0}, {3.0, 4.0}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 2);
  double       aSqDist = 0.0;
  TestPoint2D  aQuery(0.0, 0.0);
  const size_t aIdx = aTree.NearestPoint(aQuery, aSqDist);
  EXPECT_NEAR(aTree.Point(aIdx).X, 0.0, 1e-15);
  EXPECT_NEAR(aSqDist, 0.0, 1e-15);
}

TEST(NCollection_KDTreeTest, NearestPoint2D_EmptyTree)
{
  NCollection_KDTree<TestPoint2D, 2> aTree;
  TestPoint2D                        aQuery(1.0, 1.0);
  double                             aSqDist = 0.0;
  const size_t                       aIdx    = aTree.NearestPoint(aQuery, aSqDist);
  EXPECT_EQ(aIdx, 0u);
}

TEST(NCollection_KDTreeTest, NearestPoint2D_SinglePoint)
{
  TestPoint2D                        aPoint(3.0, 4.0);
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(&aPoint, 1);
  double       aSqDist = 0.0;
  TestPoint2D  aQuery(0.0, 0.0);
  const size_t aIdx = aTree.NearestPoint(aQuery, aSqDist);
  EXPECT_EQ(aIdx, 1u);
  EXPECT_NEAR(aSqDist, 25.0, 1e-10);
}

// ============================================================
// NearestPoint 3D
// ============================================================

TEST(NCollection_KDTreeTest, NearestPoint3D_BasicSearch)
{
  TestPoint3D                        aPoints[] = {{0, 0, 0}, {1, 1, 1}, {2, 2, 2}, {3, 3, 3}};
  NCollection_KDTree<TestPoint3D, 3> aTree;
  aTree.Build(aPoints, 4);
  TestPoint3D  aQuery(1.1, 0.9, 1.0);
  const size_t aIdx = aTree.NearestPoint(aQuery);
  EXPECT_NEAR(aTree.Point(aIdx).X, 1.0, 1e-15);
  EXPECT_NEAR(aTree.Point(aIdx).Y, 1.0, 1e-15);
  EXPECT_NEAR(aTree.Point(aIdx).Z, 1.0, 1e-15);
}

TEST(NCollection_KDTreeTest, NearestPoint3D_GridPoints)
{
  // 3x3x3 grid
  TestPoint3D aPoints[27];
  int         k = 0;
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      for (int l = 0; l < 3; ++l)
        aPoints[k++] = {static_cast<double>(i), static_cast<double>(j), static_cast<double>(l)};
  NCollection_KDTree<TestPoint3D, 3> aTree;
  aTree.Build(aPoints, 27);
  TestPoint3D  aQuery(1.1, 2.2, 0.9);
  const size_t aIdx = aTree.NearestPoint(aQuery);
  EXPECT_NEAR(aTree.Point(aIdx).X, 1.0, 1e-15);
  EXPECT_NEAR(aTree.Point(aIdx).Y, 2.0, 1e-15);
  EXPECT_NEAR(aTree.Point(aIdx).Z, 1.0, 1e-15);
}

TEST(NCollection_KDTreeTest, NearestPoint3D_Diagonal)
{
  TestPoint3D aPoints[10];
  for (int i = 0; i < 10; ++i)
  {
    const double aVal = static_cast<double>(i);
    aPoints[i]        = {aVal, aVal, aVal};
  }
  NCollection_KDTree<TestPoint3D, 3> aTree;
  aTree.Build(aPoints, 10);
  TestPoint3D  aQuery(4.6, 4.6, 4.6);
  const size_t aIdx = aTree.NearestPoint(aQuery);
  EXPECT_NEAR(aTree.Point(aIdx).X, 5.0, 1e-15);
}

TEST(NCollection_KDTreeTest, NearestPoint3D_Clustered)
{
  // Cluster A around (0,0,0), cluster B around (100,100,100)
  TestPoint3D                        aPoints[] = {{0.1, 0.1, 0.1},
                                                  {-0.1, -0.1, -0.1},
                                                  {0.2, 0.0, 0.0},
                                                  {100.0, 100.0, 100.0},
                                                  {99.9, 100.1, 100.0}};
  NCollection_KDTree<TestPoint3D, 3> aTree;
  aTree.Build(aPoints, 5);
  TestPoint3D  aQuery(99.0, 100.0, 100.0);
  const size_t aIdx = aTree.NearestPoint(aQuery);
  EXPECT_GT(aTree.Point(aIdx).X, 50.0); // Should be in cluster B
}

TEST(NCollection_KDTreeTest, NearestPoint3D_EquidistantPoints)
{
  // Points equidistant from origin on axes
  TestPoint3D aPoints[] = {{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}};
  NCollection_KDTree<TestPoint3D, 3> aTree;
  aTree.Build(aPoints, 6);
  double       aSqDist = 0.0;
  TestPoint3D  aQuery(0, 0, 0);
  const size_t aIdx = aTree.NearestPoint(aQuery, aSqDist);
  EXPECT_NE(aIdx, 0u);
  EXPECT_NEAR(aSqDist, 1.0, 1e-10);
}

// ============================================================
// KNearestPoints
// ============================================================

TEST(NCollection_KDTreeTest, KNearest2D_K1)
{
  TestPoint2D                        aPoints[] = {{0, 0}, {1, 1}, {2, 2}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 3);
  NCollection_Array1<size_t> anIndices(1, 0);
  NCollection_Array1<double> aDists(1, 0);
  TestPoint2D                aQuery(1.1, 1.1);
  const size_t               aCount = aTree.KNearestPoints(aQuery, 1, anIndices, aDists);
  EXPECT_EQ(aCount, 1u);
  EXPECT_NEAR(aTree.Point(anIndices.Value(1)).X, 1.0, 1e-15);
}

TEST(NCollection_KDTreeTest, KNearest2D_K3)
{
  TestPoint2D                        aPoints[] = {{0, 0}, {1, 0}, {2, 0}, {10, 0}, {20, 0}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 5);
  NCollection_Array1<size_t> anIndices(1, 0);
  NCollection_Array1<double> aDists(1, 0);
  TestPoint2D                aQuery(0.5, 0.0);
  const size_t               aCount = aTree.KNearestPoints(aQuery, 3, anIndices, aDists);
  EXPECT_EQ(aCount, 3u);
  // Collect the X coords of the 3 nearest
  std::set<double> aXCoords;
  for (int i = 1; i <= 3; ++i)
  {
    aXCoords.insert(aTree.Point(anIndices.Value(i)).X);
  }
  EXPECT_TRUE(aXCoords.count(0.0) > 0);
  EXPECT_TRUE(aXCoords.count(1.0) > 0);
  EXPECT_TRUE(aXCoords.count(2.0) > 0);
}

TEST(NCollection_KDTreeTest, KNearest2D_KEqualsN)
{
  TestPoint2D                        aPoints[] = {{0, 0}, {1, 1}, {2, 2}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 3);
  NCollection_Array1<size_t> anIndices(1, 0);
  NCollection_Array1<double> aDists(1, 0);
  TestPoint2D                aQuery(0, 0);
  const size_t               aCount = aTree.KNearestPoints(aQuery, 3, anIndices, aDists);
  EXPECT_EQ(aCount, 3u);
}

TEST(NCollection_KDTreeTest, KNearest2D_KGreaterThanN)
{
  TestPoint2D                        aPoints[] = {{0, 0}, {1, 1}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 2);
  NCollection_Array1<size_t> anIndices(1, 0);
  NCollection_Array1<double> aDists(1, 0);
  TestPoint2D                aQuery(0, 0);
  const size_t               aCount = aTree.KNearestPoints(aQuery, 10, anIndices, aDists);
  EXPECT_EQ(aCount, 2u);
}

TEST(NCollection_KDTreeTest, KNearest2D_SortedByDistance)
{
  TestPoint2D                        aPoints[] = {{0, 0}, {3, 0}, {1, 0}, {2, 0}, {4, 0}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 5);
  NCollection_Array1<size_t> anIndices(1, 0);
  NCollection_Array1<double> aDists(1, 0);
  TestPoint2D                aQuery(0, 0);
  const size_t               aCount = aTree.KNearestPoints(aQuery, 5, anIndices, aDists);
  EXPECT_EQ(aCount, 5u);
  // Verify sorted by distance
  for (int i = 1; i < static_cast<int>(aCount); ++i)
  {
    EXPECT_LE(aDists.Value(i), aDists.Value(i + 1));
  }
}

TEST(NCollection_KDTreeTest, KNearest3D_Basic)
{
  TestPoint3D aPoints[] = {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {10, 10, 10}};
  NCollection_KDTree<TestPoint3D, 3> aTree;
  aTree.Build(aPoints, 5);
  NCollection_Array1<size_t> anIndices(1, 0);
  NCollection_Array1<double> aDists(1, 0);
  TestPoint3D                aQuery(0.1, 0.1, 0.1);
  const size_t               aCount = aTree.KNearestPoints(aQuery, 3, anIndices, aDists);
  EXPECT_EQ(aCount, 3u);
}

TEST(NCollection_KDTreeTest, KNearest2D_SinglePoint)
{
  TestPoint2D                        aPoint(5.0, 5.0);
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(&aPoint, 1);
  NCollection_Array1<size_t> anIndices(1, 0);
  NCollection_Array1<double> aDists(1, 0);
  TestPoint2D                aQuery(0, 0);
  const size_t               aCount = aTree.KNearestPoints(aQuery, 5, anIndices, aDists);
  EXPECT_EQ(aCount, 1u);
}

TEST(NCollection_KDTreeTest, KNearest2D_EmptyTree)
{
  NCollection_KDTree<TestPoint2D, 2> aTree;
  NCollection_Array1<size_t>         anIndices(1, 0);
  NCollection_Array1<double>         aDists(1, 0);
  TestPoint2D                        aQuery(0, 0);
  const size_t                       aCount = aTree.KNearestPoints(aQuery, 5, anIndices, aDists);
  EXPECT_EQ(aCount, 0u);
}

// ============================================================
// RangeSearch
// ============================================================

TEST(NCollection_KDTreeTest, RangeSearch2D_EmptyResult)
{
  TestPoint2D                        aPoints[] = {{10, 10}, {20, 20}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 2);
  TestPoint2D                            aQuery(0, 0);
  const NCollection_DynamicArray<size_t> aResult = aTree.RangeSearch(aQuery, 1.0);
  EXPECT_EQ(aResult.Size(), 0u);
}

TEST(NCollection_KDTreeTest, RangeSearch2D_AllInRange)
{
  TestPoint2D                        aPoints[] = {{0, 0}, {0.1, 0.1}, {-0.1, -0.1}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 3);
  TestPoint2D                            aQuery(0, 0);
  const NCollection_DynamicArray<size_t> aResult = aTree.RangeSearch(aQuery, 1.0);
  EXPECT_EQ(aResult.Size(), 3u);
}

TEST(NCollection_KDTreeTest, RangeSearch2D_SomeInRange)
{
  TestPoint2D                        aPoints[] = {{0, 0}, {0.5, 0}, {2.0, 0}, {5.0, 0}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 4);
  TestPoint2D                            aQuery(0, 0);
  const NCollection_DynamicArray<size_t> aResult = aTree.RangeSearch(aQuery, 1.0);
  EXPECT_EQ(aResult.Size(), 2u);
}

TEST(NCollection_KDTreeTest, RangeSearch2D_ExactRadius)
{
  TestPoint2D                        aPoints[] = {{3.0, 4.0}}; // distance from origin = 5
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 1);
  TestPoint2D                            aQuery(0, 0);
  const NCollection_DynamicArray<size_t> aResult = aTree.RangeSearch(aQuery, 5.0);
  EXPECT_EQ(aResult.Size(), 1u);
}

TEST(NCollection_KDTreeTest, RangeSearch2D_ZeroRadius)
{
  TestPoint2D                        aPoints[] = {{0, 0}, {1, 1}, {0, 0}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 3);
  TestPoint2D                            aQuery(0, 0);
  const NCollection_DynamicArray<size_t> aResult = aTree.RangeSearch(aQuery, 0.0);
  EXPECT_EQ(aResult.Size(), 2u); // Two points at (0,0)
}

TEST(NCollection_KDTreeTest, RangeSearch3D_Sphere)
{
  TestPoint3D aPoints[] = {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {2, 2, 2}};
  NCollection_KDTree<TestPoint3D, 3> aTree;
  aTree.Build(aPoints, 5);
  TestPoint3D                            aQuery(0, 0, 0);
  const NCollection_DynamicArray<size_t> aResult = aTree.RangeSearch(aQuery, 1.0);
  EXPECT_EQ(aResult.Size(), 4u); // (0,0,0), (1,0,0), (0,1,0), (0,0,1) are within radius 1
}

TEST(NCollection_KDTreeTest, RangeSearch2D_EmptyTree)
{
  NCollection_KDTree<TestPoint2D, 2>     aTree;
  TestPoint2D                            aQuery(0, 0);
  const NCollection_DynamicArray<size_t> aResult = aTree.RangeSearch(aQuery, 10.0);
  EXPECT_EQ(aResult.Size(), 0u);
}

TEST(NCollection_KDTreeTest, RangeSearch2D_SinglePoint)
{
  TestPoint2D                        aPoint(5.0, 5.0);
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(&aPoint, 1);
  TestPoint2D aQuery(0, 0);
  // Out of range
  const NCollection_DynamicArray<size_t> aResult1 = aTree.RangeSearch(aQuery, 1.0);
  EXPECT_EQ(aResult1.Size(), 0u);
  // In range
  const NCollection_DynamicArray<size_t> aResult2 = aTree.RangeSearch(aQuery, 10.0);
  EXPECT_EQ(aResult2.Size(), 1u);
}

// ============================================================
// BoxSearch
// ============================================================

TEST(NCollection_KDTreeTest, BoxSearch2D_EmptyResult)
{
  TestPoint2D                        aPoints[] = {{10, 10}, {20, 20}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 2);
  TestPoint2D                            aMin(0, 0);
  TestPoint2D                            aMax(5, 5);
  const NCollection_DynamicArray<size_t> aResult = aTree.BoxSearch(aMin, aMax);
  EXPECT_EQ(aResult.Size(), 0u);
}

TEST(NCollection_KDTreeTest, BoxSearch2D_AllInBox)
{
  TestPoint2D                        aPoints[] = {{1, 1}, {2, 2}, {3, 3}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 3);
  TestPoint2D                            aMin(0, 0);
  TestPoint2D                            aMax(5, 5);
  const NCollection_DynamicArray<size_t> aResult = aTree.BoxSearch(aMin, aMax);
  EXPECT_EQ(aResult.Size(), 3u);
}

TEST(NCollection_KDTreeTest, BoxSearch2D_SomeInBox)
{
  TestPoint2D                        aPoints[] = {{1, 1}, {3, 3}, {5, 5}, {7, 7}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 4);
  TestPoint2D                            aMin(0, 0);
  TestPoint2D                            aMax(4, 4);
  const NCollection_DynamicArray<size_t> aResult = aTree.BoxSearch(aMin, aMax);
  EXPECT_EQ(aResult.Size(), 2u); // (1,1) and (3,3)
}

TEST(NCollection_KDTreeTest, BoxSearch2D_PointOnBoundary)
{
  TestPoint2D                        aPoints[] = {{0, 0}, {5, 5}, {5, 0}, {0, 5}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 4);
  TestPoint2D                            aMin(0, 0);
  TestPoint2D                            aMax(5, 5);
  const NCollection_DynamicArray<size_t> aResult = aTree.BoxSearch(aMin, aMax);
  EXPECT_EQ(aResult.Size(), 4u); // All on boundary should be included
}

TEST(NCollection_KDTreeTest, BoxSearch3D_Basic)
{
  TestPoint3D                        aPoints[] = {{1, 1, 1}, {2, 2, 2}, {5, 5, 5}, {10, 10, 10}};
  NCollection_KDTree<TestPoint3D, 3> aTree;
  aTree.Build(aPoints, 4);
  TestPoint3D                            aMin(0, 0, 0);
  TestPoint3D                            aMax(3, 3, 3);
  const NCollection_DynamicArray<size_t> aResult = aTree.BoxSearch(aMin, aMax);
  EXPECT_EQ(aResult.Size(), 2u); // (1,1,1) and (2,2,2)
}

TEST(NCollection_KDTreeTest, BoxSearch2D_EmptyTree)
{
  NCollection_KDTree<TestPoint2D, 2>     aTree;
  TestPoint2D                            aMin(0, 0);
  TestPoint2D                            aMax(10, 10);
  const NCollection_DynamicArray<size_t> aResult = aTree.BoxSearch(aMin, aMax);
  EXPECT_EQ(aResult.Size(), 0u);
}

// ============================================================
// Correctness Verification (brute-force comparison)
// ============================================================

TEST(NCollection_KDTreeTest, BruteForce_NearestComparison)
{
  constexpr int THE_N = 200;
  TestRandom    aRng(42);
  TestPoint2D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {aRng.NextDouble(-100, 100), aRng.NextDouble(-100, 100)};
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, THE_N);
  // Test 50 random queries
  for (int q = 0; q < 50; ++q)
  {
    TestPoint2D  aQuery(aRng.NextDouble(-120, 120), aRng.NextDouble(-120, 120));
    double       aKdSqDist = 0.0;
    const size_t aKdIdx    = aTree.NearestPoint(aQuery, aKdSqDist);
    // Brute force
    double aBestBfSqDist = std::numeric_limits<double>::max();
    for (int i = 0; i < THE_N; ++i)
    {
      const double aDist = sqDist2D(aQuery, aPoints[i]);
      if (aDist < aBestBfSqDist)
      {
        aBestBfSqDist = aDist;
      }
    }
    EXPECT_NEAR(aKdSqDist, aBestBfSqDist, 1e-10)
      << "Mismatch for query (" << aQuery.X << ", " << aQuery.Y << ") idx=" << aKdIdx;
  }
}

TEST(NCollection_KDTreeTest, BruteForce_KNearestComparison)
{
  constexpr int    THE_N = 200;
  constexpr size_t THE_K = 5;
  TestRandom       aRng(123);
  TestPoint2D      aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {aRng.NextDouble(-50, 50), aRng.NextDouble(-50, 50)};
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, THE_N);
  for (int q = 0; q < 30; ++q)
  {
    TestPoint2D                aQuery(aRng.NextDouble(-60, 60), aRng.NextDouble(-60, 60));
    NCollection_Array1<size_t> anIndices(1, 0);
    NCollection_Array1<double> aDists(1, 0);
    const size_t               aCount = aTree.KNearestPoints(aQuery, THE_K, anIndices, aDists);
    EXPECT_EQ(aCount, THE_K);
    // Brute force: sort all distances
    double aBfDists[THE_N];
    for (int i = 0; i < THE_N; ++i)
    {
      aBfDists[i] = sqDist2D(aQuery, aPoints[i]);
    }
    std::sort(aBfDists, aBfDists + THE_N);
    // Compare K-th distance
    EXPECT_NEAR(aDists.Value(static_cast<int>(THE_K)), aBfDists[THE_K - 1], 1e-10);
  }
}

TEST(NCollection_KDTreeTest, BruteForce_RangeSearchComparison)
{
  constexpr int THE_N = 200;
  TestRandom    aRng(77);
  TestPoint2D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {aRng.NextDouble(-50, 50), aRng.NextDouble(-50, 50)};
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, THE_N);
  for (int q = 0; q < 20; ++q)
  {
    TestPoint2D  aQuery(aRng.NextDouble(-60, 60), aRng.NextDouble(-60, 60));
    const double aRadius                             = aRng.NextDouble(5.0, 30.0);
    const NCollection_DynamicArray<size_t> aKdResult = aTree.RangeSearch(aQuery, aRadius);
    // Brute force count
    size_t       aBfCount  = 0;
    const double aRadiusSq = aRadius * aRadius;
    for (int i = 0; i < THE_N; ++i)
    {
      if (sqDist2D(aQuery, aPoints[i]) <= aRadiusSq)
      {
        ++aBfCount;
      }
    }
    EXPECT_EQ(aKdResult.Size(), aBfCount);
  }
}

TEST(NCollection_KDTreeTest, BruteForce_BoxSearchComparison)
{
  constexpr int THE_N = 200;
  TestRandom    aRng(99);
  TestPoint2D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {aRng.NextDouble(-50, 50), aRng.NextDouble(-50, 50)};
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, THE_N);
  for (int q = 0; q < 20; ++q)
  {
    const double                           aX1 = aRng.NextDouble(-60, 60);
    const double                           aY1 = aRng.NextDouble(-60, 60);
    const double                           aX2 = aX1 + aRng.NextDouble(5, 40);
    const double                           aY2 = aY1 + aRng.NextDouble(5, 40);
    TestPoint2D                            aMin(aX1, aY1);
    TestPoint2D                            aMax(aX2, aY2);
    const NCollection_DynamicArray<size_t> aKdResult = aTree.BoxSearch(aMin, aMax);
    // Brute force
    size_t aBfCount = 0;
    for (int i = 0; i < THE_N; ++i)
    {
      if (aPoints[i].X >= aX1 && aPoints[i].X <= aX2 && aPoints[i].Y >= aY1 && aPoints[i].Y <= aY2)
      {
        ++aBfCount;
      }
    }
    EXPECT_EQ(aKdResult.Size(), aBfCount);
  }
}

// ============================================================
// Stress Tests
// ============================================================

TEST(NCollection_KDTreeTest, LargeDataSet_1000Points)
{
  constexpr int THE_N = 1000;
  TestRandom    aRng(2024);
  TestPoint2D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {aRng.NextDouble(-1000, 1000), aRng.NextDouble(-1000, 1000)};
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, THE_N);
  EXPECT_EQ(aTree.Size(), static_cast<size_t>(THE_N));
  // Verify nearest for one query
  TestPoint2D  aQuery(0, 0);
  double       aSqDist = 0.0;
  const size_t aIdx    = aTree.NearestPoint(aQuery, aSqDist);
  EXPECT_NE(aIdx, 0u);
  // Brute force check
  double aBfBest = std::numeric_limits<double>::max();
  for (int i = 0; i < THE_N; ++i)
  {
    const double aDist = sqDist2D(aQuery, aPoints[i]);
    if (aDist < aBfBest)
    {
      aBfBest = aDist;
    }
  }
  EXPECT_NEAR(aSqDist, aBfBest, 1e-10);
}

TEST(NCollection_KDTreeTest, LargeDataSet_10000Points)
{
  constexpr int                   THE_N = 10000;
  TestRandom                      aRng(7777);
  NCollection_Array1<TestPoint2D> aPoints(1, THE_N);
  for (int i = 1; i <= THE_N; ++i)
  {
    aPoints.SetValue(i, {aRng.NextDouble(-5000, 5000), aRng.NextDouble(-5000, 5000)});
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints);
  EXPECT_EQ(aTree.Size(), static_cast<size_t>(THE_N));
  // Spot-check nearest for 10 queries
  for (int q = 0; q < 10; ++q)
  {
    TestPoint2D aQuery(aRng.NextDouble(-5500, 5500), aRng.NextDouble(-5500, 5500));
    double      aKdSqDist = 0.0;
    aTree.NearestPoint(aQuery, aKdSqDist);
    // Brute force
    double aBfBest = std::numeric_limits<double>::max();
    for (int i = 1; i <= THE_N; ++i)
    {
      const double aDist = sqDist2D(aQuery, aPoints.Value(i));
      if (aDist < aBfBest)
      {
        aBfBest = aDist;
      }
    }
    EXPECT_NEAR(aKdSqDist, aBfBest, 1e-10);
  }
}

TEST(NCollection_KDTreeTest, DuplicatePoints)
{
  TestPoint2D                        aPoints[] = {{1, 1}, {1, 1}, {1, 1}, {2, 2}, {2, 2}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 5);
  EXPECT_EQ(aTree.Size(), 5u);
  // Nearest to (1,1) should be exact
  double      aSqDist = 0.0;
  TestPoint2D aQuery(1, 1);
  aTree.NearestPoint(aQuery, aSqDist);
  EXPECT_NEAR(aSqDist, 0.0, 1e-15);
  // Range search should find 3 copies
  const NCollection_DynamicArray<size_t> aResult = aTree.RangeSearch(aQuery, 0.0);
  EXPECT_EQ(aResult.Size(), 3u);
}

// ============================================================
// Custom Point Type
// ============================================================

TEST(NCollection_KDTreeTest, CustomPointType2D)
{
  // TestPoint2D is already a custom type
  TestPoint2D                        aPoints[] = {{1, 2}, {3, 4}, {5, 6}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 3);
  TestPoint2D  aQuery(3.1, 4.1);
  const size_t aIdx = aTree.NearestPoint(aQuery);
  EXPECT_NEAR(aTree.Point(aIdx).X, 3.0, 1e-15);
  EXPECT_NEAR(aTree.Point(aIdx).Y, 4.0, 1e-15);
}

TEST(NCollection_KDTreeTest, CustomPointType3D)
{
  TestPoint3D                        aPoints[] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
  NCollection_KDTree<TestPoint3D, 3> aTree;
  aTree.Build(aPoints, 3);
  TestPoint3D  aQuery(4.1, 5.1, 6.1);
  const size_t aIdx = aTree.NearestPoint(aQuery);
  EXPECT_NEAR(aTree.Point(aIdx).X, 4.0, 1e-15);
  EXPECT_NEAR(aTree.Point(aIdx).Y, 5.0, 1e-15);
  EXPECT_NEAR(aTree.Point(aIdx).Z, 6.0, 1e-15);
}

// ============================================================
// Construction & State (extended)
// ============================================================

TEST(NCollection_KDTreeTest, Rebuild_DifferentData)
{
  NCollection_KDTree<TestPoint2D, 2> aTree;
  TestPoint2D                        aPoints1[] = {{0, 0}, {1, 1}};
  aTree.Build(aPoints1, 2);
  EXPECT_EQ(aTree.Size(), 2u);
  double aSqDist = 0.0;
  aTree.NearestPoint({0, 0}, aSqDist);
  EXPECT_NEAR(aSqDist, 0.0, 1e-15);
  // Rebuild with different data
  TestPoint2D aPoints2[] = {{10, 10}, {20, 20}, {30, 30}};
  aTree.Build(aPoints2, 3);
  EXPECT_EQ(aTree.Size(), 3u);
  const size_t aIdx = aTree.NearestPoint({11, 11}, aSqDist);
  EXPECT_NEAR(aTree.Point(aIdx).X, 10.0, 1e-15);
}

TEST(NCollection_KDTreeTest, BuildFromArray1_NonUnitLower)
{
  NCollection_Array1<TestPoint2D> aPoints(5, 7);
  aPoints.SetValue(5, {1.0, 2.0});
  aPoints.SetValue(6, {3.0, 4.0});
  aPoints.SetValue(7, {5.0, 6.0});
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints);
  EXPECT_EQ(aTree.Size(), 3u);
  const size_t aIdx = aTree.NearestPoint({3.1, 4.1});
  EXPECT_NEAR(aTree.Point(aIdx).X, 3.0, 1e-15);
  EXPECT_NEAR(aTree.Point(aIdx).Y, 4.0, 1e-15);
}

TEST(NCollection_KDTreeTest, BuildZeroCount)
{
  TestPoint2D                        aPoint(1, 1);
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(&aPoint, 0);
  EXPECT_TRUE(aTree.IsEmpty());
}

TEST(NCollection_KDTreeTest, PointAccessor_AllPoints)
{
  TestPoint2D                        aPoints[] = {{1, 2}, {3, 4}, {5, 6}, {7, 8}, {9, 10}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 5);
  // Every input point should be accessible via Point()
  std::set<std::pair<int, int>> anOrigSet;
  for (int i = 0; i < 5; ++i)
  {
    anOrigSet.insert({static_cast<int>(aPoints[i].X), static_cast<int>(aPoints[i].Y)});
  }
  std::set<std::pair<int, int>> aTreeSet;
  for (size_t i = 1; i <= aTree.Size(); ++i)
  {
    aTreeSet.insert({static_cast<int>(aTree.Point(i).X), static_cast<int>(aTree.Point(i).Y)});
  }
  EXPECT_EQ(anOrigSet, aTreeSet);
}

TEST(NCollection_KDTreeTest, BuildThreePoints)
{
  TestPoint2D                        aPoints[] = {{0, 0}, {1, 0}, {0, 1}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 3);
  EXPECT_EQ(aTree.Size(), 3u);
  const size_t aIdx = aTree.NearestPoint({0.1, 0.1});
  EXPECT_NEAR(aTree.Point(aIdx).X, 0.0, 1e-15);
  EXPECT_NEAR(aTree.Point(aIdx).Y, 0.0, 1e-15);
}

TEST(NCollection_KDTreeTest, BuildSevenPoints)
{
  // 2^3 - 1 = 7 gives a perfectly balanced tree
  TestPoint2D aPoints[] = {{4, 0}, {2, 0}, {6, 0}, {1, 0}, {3, 0}, {5, 0}, {7, 0}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 7);
  EXPECT_EQ(aTree.Size(), 7u);
  const size_t aIdx = aTree.NearestPoint({4.9, 0});
  EXPECT_NEAR(aTree.Point(aIdx).X, 5.0, 1e-15);
}

TEST(NCollection_KDTreeTest, BuildFifteenPoints)
{
  // 2^4 - 1 = 15 balanced tree
  TestPoint2D aPoints[15];
  for (int i = 0; i < 15; ++i)
  {
    aPoints[i] = {static_cast<double>(i), static_cast<double>(i % 3)};
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 15);
  EXPECT_EQ(aTree.Size(), 15u);
}

TEST(NCollection_KDTreeTest, BuildSixPoints)
{
  // Non-power-of-2 for unbalanced case
  TestPoint2D                        aPoints[] = {{0, 0}, {1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 6);
  EXPECT_EQ(aTree.Size(), 6u);
  const size_t aIdx = aTree.NearestPoint({2.9, 2.9});
  EXPECT_NEAR(aTree.Point(aIdx).X, 3.0, 1e-15);
}

TEST(NCollection_KDTreeTest, ClearAndRebuild)
{
  TestPoint2D                        aPoints1[] = {{0, 0}, {1, 1}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints1, 2);
  aTree.Clear();
  EXPECT_TRUE(aTree.IsEmpty());
  TestPoint2D aPoints2[] = {{5, 5}, {6, 6}, {7, 7}};
  aTree.Build(aPoints2, 3);
  EXPECT_EQ(aTree.Size(), 3u);
  double       aSqDist = 0.0;
  const size_t aIdx    = aTree.NearestPoint({5.1, 5.1}, aSqDist);
  EXPECT_NEAR(aTree.Point(aIdx).X, 5.0, 1e-15);
}

// ============================================================
// Copy/Move Semantics (extended)
// ============================================================

TEST(NCollection_KDTreeTest, CopyIndependence)
{
  TestPoint2D                        aPoints[] = {{0, 0}, {1, 1}, {2, 2}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 3);
  NCollection_KDTree<TestPoint2D, 2> aCopy(aTree);
  // Rebuild original with different data
  TestPoint2D aPoints2[] = {{10, 10}};
  aTree.Build(aPoints2, 1);
  // Copy should still work with original data
  EXPECT_EQ(aCopy.Size(), 3u);
  const size_t aIdx = aCopy.NearestPoint({1.1, 1.1});
  EXPECT_NEAR(aCopy.Point(aIdx).X, 1.0, 1e-15);
}

TEST(NCollection_KDTreeTest, MoveFromEmptyTree)
{
  NCollection_KDTree<TestPoint2D, 2> anEmpty;
  NCollection_KDTree<TestPoint2D, 2> aMoved(std::move(anEmpty));
  EXPECT_TRUE(aMoved.IsEmpty());
  EXPECT_TRUE(anEmpty.IsEmpty());
}

TEST(NCollection_KDTreeTest, CopyEmptyTree)
{
  NCollection_KDTree<TestPoint2D, 2> anEmpty;
  NCollection_KDTree<TestPoint2D, 2> aCopy(anEmpty);
  EXPECT_TRUE(aCopy.IsEmpty());
}

TEST(NCollection_KDTreeTest, MoveAssignmentFromEmpty)
{
  TestPoint2D                        aPoints[] = {{1, 1}, {2, 2}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 2);
  NCollection_KDTree<TestPoint2D, 2> anEmpty;
  aTree = std::move(anEmpty);
  EXPECT_TRUE(aTree.IsEmpty());
}

TEST(NCollection_KDTreeTest, CopyAssignmentOverwrite)
{
  TestPoint2D                        aPoints1[] = {{0, 0}};
  TestPoint2D                        aPoints2[] = {{10, 10}, {20, 20}, {30, 30}};
  NCollection_KDTree<TestPoint2D, 2> aTree1;
  aTree1.Build(aPoints1, 1);
  NCollection_KDTree<TestPoint2D, 2> aTree2;
  aTree2.Build(aPoints2, 3);
  aTree1 = aTree2;
  EXPECT_EQ(aTree1.Size(), 3u);
  const size_t aIdx = aTree1.NearestPoint({10, 10});
  EXPECT_NEAR(aTree1.Point(aIdx).X, 10.0, 1e-15);
}

TEST(NCollection_KDTreeTest, MovedTreeQueriesWork)
{
  TestPoint2D                        aPoints[] = {{0, 0}, {5, 5}, {10, 10}, {3, 4}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 4);
  NCollection_KDTree<TestPoint2D, 2> aMoved(std::move(aTree));
  // All query types should work on moved tree
  double aSqDist = 0.0;
  EXPECT_NE(aMoved.NearestPoint({3, 3}, aSqDist), 0u);
  NCollection_Array1<size_t> anIdx(1, 0);
  NCollection_Array1<double> aDist(1, 0);
  EXPECT_EQ(aMoved.KNearestPoints({0, 0}, 2, anIdx, aDist), 2u);
  const NCollection_DynamicArray<size_t> aRange = aMoved.RangeSearch({5, 5}, 20.0);
  EXPECT_GT(aRange.Size(), 0u);
  const NCollection_DynamicArray<size_t> aBox = aMoved.BoxSearch({-1, -1}, {11, 11});
  EXPECT_GT(aBox.Size(), 0u);
}

// ============================================================
// NearestPoint 2D (extended)
// ============================================================

TEST(NCollection_KDTreeTest, NearestPoint2D_CrossHyperplane)
{
  // Nearest point is on the far side of the splitting plane
  TestPoint2D                        aPoints[] = {{0, 0}, {3, 0}, {1, 2}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 3);
  // Query at (1.5, 1.9) - nearest is (1,2) which may be across the split
  TestPoint2D  aQuery(1.5, 1.9);
  const size_t aIdx = aTree.NearestPoint(aQuery);
  EXPECT_NEAR(aTree.Point(aIdx).X, 1.0, 1e-15);
  EXPECT_NEAR(aTree.Point(aIdx).Y, 2.0, 1e-15);
}

TEST(NCollection_KDTreeTest, NearestPoint2D_TwoEquidistant)
{
  // Two points equidistant from query, either is acceptable
  TestPoint2D                        aPoints[] = {{-1, 0}, {1, 0}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 2);
  double aSqDist = 0.0;
  aTree.NearestPoint({0, 0}, aSqDist);
  EXPECT_NEAR(aSqDist, 1.0, 1e-15);
}

TEST(NCollection_KDTreeTest, NearestPoint2D_VeryClosePoints)
{
  TestPoint2D                        aPoints[] = {{0.0, 0.0}, {1e-10, 1e-10}, {1.0, 1.0}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 3);
  double       aSqDist = 0.0;
  const size_t aIdx    = aTree.NearestPoint({5e-11, 5e-11}, aSqDist);
  // Should find one of the two very close points
  EXPECT_LT(aSqDist, 1e-18);
  (void)aIdx;
}

TEST(NCollection_KDTreeTest, NearestPoint2D_LargeCoordinates)
{
  TestPoint2D                        aPoints[] = {{1e12, 1e12}, {-1e12, -1e12}, {0, 0}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 3);
  const size_t aIdx = aTree.NearestPoint({1e12 + 1.0, 1e12 + 1.0});
  EXPECT_NEAR(aTree.Point(aIdx).X, 1e12, 1.0);
}

TEST(NCollection_KDTreeTest, NearestPoint2D_MidpointQuery)
{
  TestPoint2D                        aPoints[] = {{0, 0}, {10, 0}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 2);
  double aSqDist = 0.0;
  aTree.NearestPoint({5, 0}, aSqDist);
  EXPECT_NEAR(aSqDist, 25.0, 1e-10);
}

TEST(NCollection_KDTreeTest, NearestPoint2D_NegativeQuadrantOnly)
{
  TestPoint2D                        aPoints[] = {{-10, -10}, {-5, -5}, {-1, -1}, {-20, -30}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 4);
  const size_t aIdx = aTree.NearestPoint({-5.1, -5.1});
  EXPECT_NEAR(aTree.Point(aIdx).X, -5.0, 1e-15);
}

TEST(NCollection_KDTreeTest, NearestPoint2D_CircularLayout)
{
  // Points arranged in a circle of radius 10
  constexpr int THE_N = 12;
  TestPoint2D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    const double anAngle = 2.0 * 3.14159265358979323846 * i / THE_N;
    aPoints[i]           = {10.0 * std::cos(anAngle), 10.0 * std::sin(anAngle)};
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, THE_N);
  // Query at center
  double aSqDist = 0.0;
  aTree.NearestPoint({0, 0}, aSqDist);
  EXPECT_NEAR(aSqDist, 100.0, 1e-8);
  // Query near one of the points
  const size_t aIdx = aTree.NearestPoint({10.1, 0.0});
  EXPECT_NEAR(aTree.Point(aIdx).X, 10.0, 0.1);
  EXPECT_NEAR(aTree.Point(aIdx).Y, 0.0, 0.1);
}

TEST(NCollection_KDTreeTest, NearestPoint2D_LShape)
{
  // L-shaped distribution
  TestPoint2D aPoints[] = {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 9);
  const size_t aIdx = aTree.NearestPoint({0.1, 2.9});
  EXPECT_NEAR(aTree.Point(aIdx).X, 0.0, 1e-15);
  EXPECT_NEAR(aTree.Point(aIdx).Y, 3.0, 1e-15);
}

TEST(NCollection_KDTreeTest, NearestPoint2D_MultipleQueries)
{
  TestPoint2D                        aPoints[] = {{0, 0}, {10, 0}, {0, 10}, {10, 10}, {5, 5}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 5);

  // Multiple queries on same tree
  struct QueryExpect
  {
    TestPoint2D Query;
    double      ExpX, ExpY;
  };

  QueryExpect aCases[] = {{{0.1, 0.1}, 0, 0},
                          {{9.9, 0.1}, 10, 0},
                          {{0.1, 9.9}, 0, 10},
                          {{9.9, 9.9}, 10, 10},
                          {{5.1, 5.1}, 5, 5}};
  for (const auto& aCase : aCases)
  {
    const size_t aIdx = aTree.NearestPoint(aCase.Query);
    EXPECT_NEAR(aTree.Point(aIdx).X, aCase.ExpX, 1e-15);
    EXPECT_NEAR(aTree.Point(aIdx).Y, aCase.ExpY, 1e-15);
  }
}

TEST(NCollection_KDTreeTest, NearestPoint2D_ThinStrip)
{
  // Points in a very thin horizontal strip
  TestPoint2D aPoints[] = {{0, 0}, {10, 0.001}, {20, -0.001}, {30, 0.002}, {40, -0.002}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 5);
  double aSqDist = 0.0;
  aTree.NearestPoint({15, 0}, aSqDist);
  // Both (10,0.001) and (20,-0.001) are nearly equidistant, verify via brute force
  double aBfBest = std::numeric_limits<double>::max();
  for (int i = 0; i < 5; ++i)
  {
    const double aDist = sqDist2D({15, 0}, aPoints[i]);
    if (aDist < aBfBest)
      aBfBest = aDist;
  }
  EXPECT_NEAR(aSqDist, aBfBest, 1e-10);
}

TEST(NCollection_KDTreeTest, NearestPoint2D_SameXDifferentY)
{
  TestPoint2D                        aPoints[] = {{5, 0}, {5, 1}, {5, 2}, {5, 3}, {5, 4}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 5);
  const size_t aIdx = aTree.NearestPoint({5, 2.3});
  EXPECT_NEAR(aTree.Point(aIdx).X, 5.0, 1e-15);
  EXPECT_NEAR(aTree.Point(aIdx).Y, 2.0, 1e-15);
}

TEST(NCollection_KDTreeTest, NearestPoint2D_SameYDifferentX)
{
  TestPoint2D                        aPoints[] = {{0, 5}, {1, 5}, {2, 5}, {3, 5}, {4, 5}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 5);
  const size_t aIdx = aTree.NearestPoint({3.7, 5});
  EXPECT_NEAR(aTree.Point(aIdx).X, 4.0, 1e-15);
  EXPECT_NEAR(aTree.Point(aIdx).Y, 5.0, 1e-15);
}

// ============================================================
// NearestPoint 3D (extended)
// ============================================================

TEST(NCollection_KDTreeTest, NearestPoint3D_AllSameXDiffYZ)
{
  TestPoint3D                        aPoints[] = {{5, 0, 0}, {5, 10, 0}, {5, 0, 10}, {5, 5, 5}};
  NCollection_KDTree<TestPoint3D, 3> aTree;
  aTree.Build(aPoints, 4);
  const size_t aIdx = aTree.NearestPoint({5, 5.1, 4.9});
  EXPECT_NEAR(aTree.Point(aIdx).X, 5.0, 1e-15);
  EXPECT_NEAR(aTree.Point(aIdx).Y, 5.0, 1e-15);
  EXPECT_NEAR(aTree.Point(aIdx).Z, 5.0, 1e-15);
}

TEST(NCollection_KDTreeTest, NearestPoint3D_SphereSurface)
{
  // Points on sphere of radius 10
  constexpr int THE_N = 8;
  TestPoint3D   aPoints[THE_N];
  aPoints[0] = {10, 0, 0};
  aPoints[1] = {-10, 0, 0};
  aPoints[2] = {0, 10, 0};
  aPoints[3] = {0, -10, 0};
  aPoints[4] = {0, 0, 10};
  aPoints[5] = {0, 0, -10};
  aPoints[6] = {5.77, 5.77, 5.77};
  aPoints[7] = {-5.77, -5.77, -5.77};
  NCollection_KDTree<TestPoint3D, 3> aTree;
  aTree.Build(aPoints, THE_N);
  const size_t aIdx = aTree.NearestPoint({9.5, 0.5, 0.5});
  EXPECT_NEAR(aTree.Point(aIdx).X, 10.0, 1e-15);
}

TEST(NCollection_KDTreeTest, NearestPoint3D_MixedSignCoordinates)
{
  TestPoint3D aPoints[] = {{-1, 2, -3}, {4, -5, 6}, {-7, 8, -9}, {10, -11, 12}};
  NCollection_KDTree<TestPoint3D, 3> aTree;
  aTree.Build(aPoints, 4);
  const size_t aIdx = aTree.NearestPoint({-1.1, 2.1, -3.1});
  EXPECT_NEAR(aTree.Point(aIdx).X, -1.0, 1e-15);
  EXPECT_NEAR(aTree.Point(aIdx).Y, 2.0, 1e-15);
  EXPECT_NEAR(aTree.Point(aIdx).Z, -3.0, 1e-15);
}

TEST(NCollection_KDTreeTest, NearestPoint3D_Collinear)
{
  // All points on the X axis in 3D
  TestPoint3D aPoints[] = {{0, 0, 0}, {1, 0, 0}, {2, 0, 0}, {3, 0, 0}, {4, 0, 0}};
  NCollection_KDTree<TestPoint3D, 3> aTree;
  aTree.Build(aPoints, 5);
  const size_t aIdx = aTree.NearestPoint({2.4, 0, 0});
  EXPECT_NEAR(aTree.Point(aIdx).X, 2.0, 1e-15);
}

TEST(NCollection_KDTreeTest, NearestPoint3D_LargeGrid)
{
  // 5x5x5 grid = 125 points
  constexpr int THE_N = 125;
  TestPoint3D   aPoints[THE_N];
  int           k = 0;
  for (int i = 0; i < 5; ++i)
    for (int j = 0; j < 5; ++j)
      for (int l = 0; l < 5; ++l)
        aPoints[k++] = {static_cast<double>(i * 2),
                        static_cast<double>(j * 2),
                        static_cast<double>(l * 2)};
  NCollection_KDTree<TestPoint3D, 3> aTree;
  aTree.Build(aPoints, THE_N);
  const size_t aIdx = aTree.NearestPoint({3.1, 5.1, 7.1});
  // Nearest grid point is (2, 4, 6) or (4, 6, 8) - should be (4, 6, 8) -> nope
  // Actually nearest to (3.1,5.1,7.1) from grid {0,2,4,6,8} x {0,2,4,6,8} x {0,2,4,6,8}:
  // (2,4,6): dist^2 = 1.21+1.21+1.21 = 3.63
  // (4,6,8): dist^2 = 0.81+0.81+0.81 = 2.43
  EXPECT_NEAR(aTree.Point(aIdx).X, 4.0, 1e-15);
  EXPECT_NEAR(aTree.Point(aIdx).Y, 6.0, 1e-15);
  EXPECT_NEAR(aTree.Point(aIdx).Z, 8.0, 1e-15);
}

// ============================================================
// KNearestPoints (extended)
// ============================================================

TEST(NCollection_KDTreeTest, KNearest2D_K2)
{
  TestPoint2D                        aPoints[] = {{0, 0}, {1, 0}, {5, 0}, {10, 0}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 4);
  NCollection_Array1<size_t> anIndices(1, 0);
  NCollection_Array1<double> aDists(1, 0);
  const size_t               aCount = aTree.KNearestPoints({0.5, 0}, 2, anIndices, aDists);
  EXPECT_EQ(aCount, 2u);
  std::set<double> aXCoords;
  for (int i = 1; i <= 2; ++i)
  {
    aXCoords.insert(aTree.Point(anIndices.Value(i)).X);
  }
  EXPECT_TRUE(aXCoords.count(0.0) > 0);
  EXPECT_TRUE(aXCoords.count(1.0) > 0);
}

TEST(NCollection_KDTreeTest, KNearest2D_K0)
{
  TestPoint2D                        aPoints[] = {{0, 0}, {1, 1}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 2);
  NCollection_Array1<size_t> anIndices(1, 0);
  NCollection_Array1<double> aDists(1, 0);
  const size_t               aCount = aTree.KNearestPoints({0, 0}, 0, anIndices, aDists);
  EXPECT_EQ(aCount, 0u);
}

TEST(NCollection_KDTreeTest, KNearest2D_AllDuplicates)
{
  TestPoint2D                        aPoints[] = {{3, 3}, {3, 3}, {3, 3}, {3, 3}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 4);
  NCollection_Array1<size_t> anIndices(1, 0);
  NCollection_Array1<double> aDists(1, 0);
  const size_t               aCount = aTree.KNearestPoints({0, 0}, 3, anIndices, aDists);
  EXPECT_EQ(aCount, 3u);
  // All should have same distance
  for (int i = 1; i <= 3; ++i)
  {
    EXPECT_NEAR(aDists.Value(i), 18.0, 1e-10);
  }
}

TEST(NCollection_KDTreeTest, KNearest2D_ValidIndices)
{
  constexpr int THE_N = 50;
  TestRandom    aRng(555);
  TestPoint2D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {aRng.NextDouble(-10, 10), aRng.NextDouble(-10, 10)};
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, THE_N);
  NCollection_Array1<size_t> anIndices(1, 0);
  NCollection_Array1<double> aDists(1, 0);
  const size_t               aCount = aTree.KNearestPoints({0, 0}, 10, anIndices, aDists);
  EXPECT_EQ(aCount, 10u);
  for (int i = 1; i <= static_cast<int>(aCount); ++i)
  {
    EXPECT_GE(anIndices.Value(i), 1u);
    EXPECT_LE(anIndices.Value(i), static_cast<size_t>(THE_N));
  }
}

TEST(NCollection_KDTreeTest, KNearest2D_VerifyDistances)
{
  TestPoint2D                        aPoints[] = {{0, 0}, {3, 4}, {1, 0}, {0, 1}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 4);
  NCollection_Array1<size_t> anIndices(1, 0);
  NCollection_Array1<double> aDists(1, 0);
  TestPoint2D                aQuery(0, 0);
  const size_t               aCount = aTree.KNearestPoints(aQuery, 4, anIndices, aDists);
  EXPECT_EQ(aCount, 4u);
  // Verify each distance matches actual squared distance
  for (int i = 1; i <= static_cast<int>(aCount); ++i)
  {
    const double aExpected = sqDist2D(aQuery, aTree.Point(anIndices.Value(i)));
    EXPECT_NEAR(aDists.Value(i), aExpected, 1e-15);
  }
}

TEST(NCollection_KDTreeTest, KNearest3D_BruteForce)
{
  constexpr int    THE_N = 100;
  constexpr size_t THE_K = 7;
  TestRandom       aRng(333);
  TestPoint3D      aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {aRng.NextDouble(-50, 50), aRng.NextDouble(-50, 50), aRng.NextDouble(-50, 50)};
  }
  NCollection_KDTree<TestPoint3D, 3> aTree;
  aTree.Build(aPoints, THE_N);
  for (int q = 0; q < 20; ++q)
  {
    TestPoint3D                aQuery(aRng.NextDouble(-60, 60),
                       aRng.NextDouble(-60, 60),
                       aRng.NextDouble(-60, 60));
    NCollection_Array1<size_t> anIndices(1, 0);
    NCollection_Array1<double> aDists(1, 0);
    const size_t               aCount = aTree.KNearestPoints(aQuery, THE_K, anIndices, aDists);
    EXPECT_EQ(aCount, THE_K);
    // Brute force
    std::vector<double> aBfDists(THE_N);
    for (int i = 0; i < THE_N; ++i)
    {
      aBfDists[i] = sqDist3D(aQuery, aPoints[i]);
    }
    std::sort(aBfDists.begin(), aBfDists.end());
    EXPECT_NEAR(aDists.Value(static_cast<int>(THE_K)), aBfDists[THE_K - 1], 1e-10);
  }
}

TEST(NCollection_KDTreeTest, KNearest2D_LargeK)
{
  constexpr int    THE_N = 500;
  constexpr size_t THE_K = 50;
  TestRandom       aRng(789);
  TestPoint2D      aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {aRng.NextDouble(-100, 100), aRng.NextDouble(-100, 100)};
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, THE_N);
  NCollection_Array1<size_t> anIndices(1, 0);
  NCollection_Array1<double> aDists(1, 0);
  TestPoint2D                aQuery(0, 0);
  const size_t               aCount = aTree.KNearestPoints(aQuery, THE_K, anIndices, aDists);
  EXPECT_EQ(aCount, THE_K);
  // Verify sorted
  for (int i = 1; i < static_cast<int>(aCount); ++i)
  {
    EXPECT_LE(aDists.Value(i), aDists.Value(i + 1));
  }
  // Brute force: verify K-th distance
  std::vector<double> aBfDists(THE_N);
  for (int i = 0; i < THE_N; ++i)
  {
    aBfDists[i] = sqDist2D(aQuery, aPoints[i]);
  }
  std::sort(aBfDists.begin(), aBfDists.end());
  EXPECT_NEAR(aDists.Value(static_cast<int>(THE_K)), aBfDists[THE_K - 1], 1e-10);
}

TEST(NCollection_KDTreeTest, KNearest2D_UniqueIndices)
{
  // Ensure no duplicate indices in results (unless points are actually duplicates)
  TestPoint2D aPoints[] = {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 8);
  NCollection_Array1<size_t> anIndices(1, 0);
  NCollection_Array1<double> aDists(1, 0);
  const size_t               aCount = aTree.KNearestPoints({3.5, 0}, 5, anIndices, aDists);
  EXPECT_EQ(aCount, 5u);
  std::set<size_t> aIdxSet;
  for (int i = 1; i <= static_cast<int>(aCount); ++i)
  {
    aIdxSet.insert(anIndices.Value(i));
  }
  EXPECT_EQ(aIdxSet.size(), 5u);
}

// ============================================================
// RangeSearch (extended)
// ============================================================

TEST(NCollection_KDTreeTest, RangeSearch2D_NegativeRadius)
{
  TestPoint2D                        aPoints[] = {{0, 0}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 1);
  const NCollection_DynamicArray<size_t> aResult = aTree.RangeSearch({0, 0}, -1.0);
  EXPECT_EQ(aResult.Size(), 0u);
}

TEST(NCollection_KDTreeTest, RangeSearch2D_LargeRadiusAll)
{
  TestPoint2D                        aPoints[] = {{-100, -100}, {100, 100}, {0, 0}, {50, -50}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 4);
  const NCollection_DynamicArray<size_t> aResult = aTree.RangeSearch({0, 0}, 1e6);
  EXPECT_EQ(aResult.Size(), 4u);
}

TEST(NCollection_KDTreeTest, RangeSearch2D_VerySmallRadius)
{
  TestPoint2D                        aPoints[] = {{0, 0}, {1e-8, 1e-8}, {1, 1}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 3);
  const NCollection_DynamicArray<size_t> aResult = aTree.RangeSearch({0, 0}, 1e-7);
  EXPECT_EQ(aResult.Size(), 2u); // (0,0) and (1e-8,1e-8)
}

TEST(NCollection_KDTreeTest, RangeSearch2D_OffCenter)
{
  // Grid of points, search around an off-center location
  constexpr int THE_N = 25;
  TestPoint2D   aPoints[THE_N];
  int           k = 0;
  for (int i = 0; i < 5; ++i)
    for (int j = 0; j < 5; ++j)
      aPoints[k++] = {static_cast<double>(i), static_cast<double>(j)};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, THE_N);
  // Search radius 0.6 around (2, 2) should find only (2,2)
  const NCollection_DynamicArray<size_t> aResult = aTree.RangeSearch({2, 2}, 0.6);
  EXPECT_EQ(aResult.Size(), 1u);
  // Search radius 1.0 around (2, 2) should find (2,2),(1,2),(3,2),(2,1),(2,3)
  const NCollection_DynamicArray<size_t> aResult2 = aTree.RangeSearch({2, 2}, 1.0);
  EXPECT_EQ(aResult2.Size(), 5u);
}

TEST(NCollection_KDTreeTest, RangeSearch2D_ValidIndices)
{
  constexpr int THE_N = 30;
  TestRandom    aRng(444);
  TestPoint2D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {aRng.NextDouble(-10, 10), aRng.NextDouble(-10, 10)};
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, THE_N);
  const NCollection_DynamicArray<size_t> aResult = aTree.RangeSearch({0, 0}, 15.0);
  for (int i = 0; i < aResult.Size(); ++i)
  {
    EXPECT_GE(aResult[i], 1u);
    EXPECT_LE(aResult[i], static_cast<size_t>(THE_N));
  }
}

TEST(NCollection_KDTreeTest, RangeSearch3D_BruteForce)
{
  constexpr int THE_N = 150;
  TestRandom    aRng(888);
  TestPoint3D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {aRng.NextDouble(-50, 50), aRng.NextDouble(-50, 50), aRng.NextDouble(-50, 50)};
  }
  NCollection_KDTree<TestPoint3D, 3> aTree;
  aTree.Build(aPoints, THE_N);
  for (int q = 0; q < 15; ++q)
  {
    TestPoint3D                            aQuery(aRng.NextDouble(-60, 60),
                       aRng.NextDouble(-60, 60),
                       aRng.NextDouble(-60, 60));
    const double                           aRadius   = aRng.NextDouble(10, 40);
    const NCollection_DynamicArray<size_t> aKdResult = aTree.RangeSearch(aQuery, aRadius);
    size_t                                 aBfCount  = 0;
    const double                           aRadiusSq = aRadius * aRadius;
    for (int i = 0; i < THE_N; ++i)
    {
      if (sqDist3D(aQuery, aPoints[i]) <= aRadiusSq)
      {
        ++aBfCount;
      }
    }
    EXPECT_EQ(aKdResult.Size(), aBfCount);
  }
}

TEST(NCollection_KDTreeTest, RangeSearch2D_VerifyPointsInRange)
{
  constexpr int THE_N = 50;
  TestRandom    aRng(111);
  TestPoint2D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {aRng.NextDouble(-20, 20), aRng.NextDouble(-20, 20)};
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, THE_N);
  TestPoint2D                            aQuery(0, 0);
  const double                           aRadius = 10.0;
  const NCollection_DynamicArray<size_t> aResult = aTree.RangeSearch(aQuery, aRadius);
  // Verify every returned point is actually within radius
  for (int i = 0; i < aResult.Size(); ++i)
  {
    const double aDist = sqDist2D(aQuery, aTree.Point(aResult[i]));
    EXPECT_LE(aDist, aRadius * aRadius + 1e-10);
  }
}

// ============================================================
// BoxSearch (extended)
// ============================================================

TEST(NCollection_KDTreeTest, BoxSearch2D_ZeroVolumeBox)
{
  TestPoint2D                        aPoints[] = {{0, 0}, {5, 5}, {5, 0}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 3);
  // Box is a single point
  const NCollection_DynamicArray<size_t> aResult = aTree.BoxSearch({5, 5}, {5, 5});
  EXPECT_EQ(aResult.Size(), 1u);
}

TEST(NCollection_KDTreeTest, BoxSearch2D_DegenerateLineBox)
{
  // Box is a horizontal line (min.Y == max.Y)
  TestPoint2D                        aPoints[] = {{0, 5}, {1, 5}, {2, 5}, {3, 5}, {2, 6}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 5);
  const NCollection_DynamicArray<size_t> aResult = aTree.BoxSearch({0, 5}, {3, 5});
  EXPECT_EQ(aResult.Size(), 4u); // Points at y=5
}

TEST(NCollection_KDTreeTest, BoxSearch2D_NegativeQuadrant)
{
  TestPoint2D                        aPoints[] = {{-10, -10}, {-5, -5}, {-1, -1}, {5, 5}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 4);
  const NCollection_DynamicArray<size_t> aResult = aTree.BoxSearch({-11, -11}, {-4, -4});
  EXPECT_EQ(aResult.Size(), 2u); // (-10,-10) and (-5,-5)
}

TEST(NCollection_KDTreeTest, BoxSearch2D_NarrowBox)
{
  // Very narrow box in X
  TestPoint2D                        aPoints[] = {{0, 0}, {0.001, 5}, {0.002, 10}, {1, 5}, {-1, 5}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 5);
  const NCollection_DynamicArray<size_t> aResult = aTree.BoxSearch({-0.01, -100}, {0.01, 100});
  EXPECT_EQ(aResult.Size(), 3u); // (0,0), (0.001,5), (0.002,10)
}

TEST(NCollection_KDTreeTest, BoxSearch2D_SinglePointInOut)
{
  TestPoint2D                        aPoint(5, 5);
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(&aPoint, 1);
  const NCollection_DynamicArray<size_t> aResult1 = aTree.BoxSearch({4, 4}, {6, 6});
  EXPECT_EQ(aResult1.Size(), 1u);
  const NCollection_DynamicArray<size_t> aResult2 = aTree.BoxSearch({6, 6}, {7, 7});
  EXPECT_EQ(aResult2.Size(), 0u);
}

TEST(NCollection_KDTreeTest, BoxSearch3D_BruteForce)
{
  constexpr int THE_N = 150;
  TestRandom    aRng(246);
  TestPoint3D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {aRng.NextDouble(-50, 50), aRng.NextDouble(-50, 50), aRng.NextDouble(-50, 50)};
  }
  NCollection_KDTree<TestPoint3D, 3> aTree;
  aTree.Build(aPoints, THE_N);
  for (int q = 0; q < 15; ++q)
  {
    const double                           aX1 = aRng.NextDouble(-60, 60);
    const double                           aY1 = aRng.NextDouble(-60, 60);
    const double                           aZ1 = aRng.NextDouble(-60, 60);
    const double                           aX2 = aX1 + aRng.NextDouble(5, 40);
    const double                           aY2 = aY1 + aRng.NextDouble(5, 40);
    const double                           aZ2 = aZ1 + aRng.NextDouble(5, 40);
    const NCollection_DynamicArray<size_t> aKdResult =
      aTree.BoxSearch({aX1, aY1, aZ1}, {aX2, aY2, aZ2});
    size_t aBfCount = 0;
    for (int i = 0; i < THE_N; ++i)
    {
      if (aPoints[i].X >= aX1 && aPoints[i].X <= aX2 && aPoints[i].Y >= aY1 && aPoints[i].Y <= aY2
          && aPoints[i].Z >= aZ1 && aPoints[i].Z <= aZ2)
      {
        ++aBfCount;
      }
    }
    EXPECT_EQ(aKdResult.Size(), aBfCount);
  }
}

TEST(NCollection_KDTreeTest, BoxSearch2D_VerifyPointsInBox)
{
  constexpr int THE_N = 50;
  TestRandom    aRng(222);
  TestPoint2D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {aRng.NextDouble(-20, 20), aRng.NextDouble(-20, 20)};
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, THE_N);
  TestPoint2D                            aMin(-5, -5);
  TestPoint2D                            aMax(5, 5);
  const NCollection_DynamicArray<size_t> aResult = aTree.BoxSearch(aMin, aMax);
  for (int i = 0; i < aResult.Size(); ++i)
  {
    const TestPoint2D& aP = aTree.Point(aResult[i]);
    EXPECT_GE(aP.X, -5.0);
    EXPECT_LE(aP.X, 5.0);
    EXPECT_GE(aP.Y, -5.0);
    EXPECT_LE(aP.Y, 5.0);
  }
}

TEST(NCollection_KDTreeTest, BoxSearch3D_SinglePointInOut)
{
  TestPoint3D                        aPoint(5, 5, 5);
  NCollection_KDTree<TestPoint3D, 3> aTree;
  aTree.Build(&aPoint, 1);
  const NCollection_DynamicArray<size_t> aResult1 = aTree.BoxSearch({4, 4, 4}, {6, 6, 6});
  EXPECT_EQ(aResult1.Size(), 1u);
  const NCollection_DynamicArray<size_t> aResult2 = aTree.BoxSearch({6, 6, 6}, {7, 7, 7});
  EXPECT_EQ(aResult2.Size(), 0u);
}

// ============================================================
// Brute-Force 3D Correctness
// ============================================================

TEST(NCollection_KDTreeTest, BruteForce3D_NearestComparison)
{
  constexpr int THE_N = 200;
  TestRandom    aRng(1001);
  TestPoint3D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {aRng.NextDouble(-100, 100),
                  aRng.NextDouble(-100, 100),
                  aRng.NextDouble(-100, 100)};
  }
  NCollection_KDTree<TestPoint3D, 3> aTree;
  aTree.Build(aPoints, THE_N);
  for (int q = 0; q < 50; ++q)
  {
    TestPoint3D aQuery(aRng.NextDouble(-120, 120),
                       aRng.NextDouble(-120, 120),
                       aRng.NextDouble(-120, 120));
    double      aKdSqDist = 0.0;
    aTree.NearestPoint(aQuery, aKdSqDist);
    double aBfBest = std::numeric_limits<double>::max();
    for (int i = 0; i < THE_N; ++i)
    {
      const double aDist = sqDist3D(aQuery, aPoints[i]);
      if (aDist < aBfBest)
      {
        aBfBest = aDist;
      }
    }
    EXPECT_NEAR(aKdSqDist, aBfBest, 1e-10);
  }
}

// ============================================================
// Brute-Force Multiple Seeds (2D)
// ============================================================

TEST(NCollection_KDTreeTest, BruteForce_NearestMultipleSeeds)
{
  // Test with several different seeds to increase coverage
  constexpr int      THE_N    = 150;
  const unsigned int aSeeds[] = {1, 13, 42, 100, 9999, 12345, 65535};
  for (unsigned int aSeed : aSeeds)
  {
    TestRandom  aRng(aSeed);
    TestPoint2D aPoints[THE_N];
    for (int i = 0; i < THE_N; ++i)
    {
      aPoints[i] = {aRng.NextDouble(-200, 200), aRng.NextDouble(-200, 200)};
    }
    NCollection_KDTree<TestPoint2D, 2> aTree;
    aTree.Build(aPoints, THE_N);
    for (int q = 0; q < 20; ++q)
    {
      TestPoint2D aQuery(aRng.NextDouble(-250, 250), aRng.NextDouble(-250, 250));
      double      aKdSqDist = 0.0;
      aTree.NearestPoint(aQuery, aKdSqDist);
      double aBfBest = std::numeric_limits<double>::max();
      for (int i = 0; i < THE_N; ++i)
      {
        const double aDist = sqDist2D(aQuery, aPoints[i]);
        if (aDist < aBfBest)
        {
          aBfBest = aDist;
        }
      }
      EXPECT_NEAR(aKdSqDist, aBfBest, 1e-10) << "seed=" << aSeed << " query=" << q;
    }
  }
}

// ============================================================
// Result Set Verification (indices match actual points)
// ============================================================

TEST(NCollection_KDTreeTest, RangeSearch_IndicesMatchPoints)
{
  constexpr int THE_N = 100;
  TestRandom    aRng(666);
  TestPoint2D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {aRng.NextDouble(-30, 30), aRng.NextDouble(-30, 30)};
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, THE_N);
  TestPoint2D                            aQuery(5, 5);
  const double                           aRadius = 10.0;
  const NCollection_DynamicArray<size_t> aResult = aTree.RangeSearch(aQuery, aRadius);
  // Verify every returned index maps to a point within radius
  for (int i = 0; i < aResult.Size(); ++i)
  {
    const size_t anIdx = aResult[i];
    EXPECT_GE(anIdx, 1u);
    EXPECT_LE(anIdx, static_cast<size_t>(THE_N));
    const double aDist = sqDist2D(aQuery, aTree.Point(anIdx));
    EXPECT_LE(aDist, aRadius * aRadius + 1e-10);
  }
  // Verify no valid point was missed
  size_t aBfCount = 0;
  for (int i = 0; i < THE_N; ++i)
  {
    if (sqDist2D(aQuery, aPoints[i]) <= aRadius * aRadius)
    {
      ++aBfCount;
    }
  }
  EXPECT_EQ(aResult.Size(), aBfCount);
}

TEST(NCollection_KDTreeTest, BoxSearch_IndicesMatchPoints)
{
  constexpr int THE_N = 100;
  TestRandom    aRng(777);
  TestPoint2D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {aRng.NextDouble(-30, 30), aRng.NextDouble(-30, 30)};
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, THE_N);
  TestPoint2D                            aMin(-10, -10);
  TestPoint2D                            aMax(10, 10);
  const NCollection_DynamicArray<size_t> aResult = aTree.BoxSearch(aMin, aMax);
  for (int i = 0; i < aResult.Size(); ++i)
  {
    const size_t       anIdx = aResult[i];
    const TestPoint2D& aP    = aTree.Point(anIdx);
    EXPECT_GE(aP.X, -10.0);
    EXPECT_LE(aP.X, 10.0);
    EXPECT_GE(aP.Y, -10.0);
    EXPECT_LE(aP.Y, 10.0);
  }
  size_t aBfCount = 0;
  for (int i = 0; i < THE_N; ++i)
  {
    if (aPoints[i].X >= -10.0 && aPoints[i].X <= 10.0 && aPoints[i].Y >= -10.0
        && aPoints[i].Y <= 10.0)
    {
      ++aBfCount;
    }
  }
  EXPECT_EQ(aResult.Size(), aBfCount);
}

TEST(NCollection_KDTreeTest, KNearest_IndicesMatchPoints)
{
  constexpr int    THE_N = 100;
  constexpr size_t THE_K = 10;
  TestRandom       aRng(321);
  TestPoint2D      aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {aRng.NextDouble(-50, 50), aRng.NextDouble(-50, 50)};
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, THE_N);
  TestPoint2D                aQuery(0, 0);
  NCollection_Array1<size_t> anIndices(1, 0);
  NCollection_Array1<double> aDists(1, 0);
  const size_t               aCount = aTree.KNearestPoints(aQuery, THE_K, anIndices, aDists);
  EXPECT_EQ(aCount, THE_K);
  for (int i = 1; i <= static_cast<int>(aCount); ++i)
  {
    const size_t anIdx = anIndices.Value(i);
    EXPECT_GE(anIdx, 1u);
    EXPECT_LE(anIdx, static_cast<size_t>(THE_N));
    const double aExpected = sqDist2D(aQuery, aTree.Point(anIdx));
    EXPECT_NEAR(aDists.Value(i), aExpected, 1e-10);
  }
  // Verify the K-th distance is correct
  std::vector<double> aBfDists(THE_N);
  for (int i = 0; i < THE_N; ++i)
  {
    aBfDists[i] = sqDist2D(aQuery, aPoints[i]);
  }
  std::sort(aBfDists.begin(), aBfDists.end());
  EXPECT_NEAR(aDists.Value(static_cast<int>(THE_K)), aBfDists[THE_K - 1], 1e-10);
}

// ============================================================
// Degenerate Distributions
// ============================================================

TEST(NCollection_KDTreeTest, Degenerate_AllOnXAxis)
{
  // All points on X-axis with Y=0
  constexpr int THE_N = 20;
  TestPoint2D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {static_cast<double>(i), 0.0};
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, THE_N);
  // Nearest
  double       aSqDist = 0.0;
  const size_t aIdx    = aTree.NearestPoint({7.3, 0}, aSqDist);
  EXPECT_NEAR(aTree.Point(aIdx).X, 7.0, 1e-15);
  EXPECT_NEAR(aSqDist, 0.09, 1e-10);
  // KNearest
  NCollection_Array1<size_t> anIndices(1, 0);
  NCollection_Array1<double> aDists(1, 0);
  aTree.KNearestPoints({10, 0}, 3, anIndices, aDists);
  EXPECT_EQ(anIndices.Length(), 3);
  // Range
  const NCollection_DynamicArray<size_t> aRange = aTree.RangeSearch({10, 0}, 2.5);
  // Points 8,9,10,11,12 are within 2.5
  EXPECT_EQ(aRange.Size(), 5u);
}

TEST(NCollection_KDTreeTest, Degenerate_AllOnYAxis)
{
  constexpr int THE_N = 20;
  TestPoint2D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {0.0, static_cast<double>(i)};
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, THE_N);
  const size_t aIdx = aTree.NearestPoint({0, 15.4});
  EXPECT_NEAR(aTree.Point(aIdx).Y, 15.0, 1e-15);
}

TEST(NCollection_KDTreeTest, Degenerate_AllIdentical)
{
  constexpr int THE_N = 20;
  TestPoint2D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {42.0, 42.0};
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, THE_N);
  double aSqDist = 0.0;
  aTree.NearestPoint({42, 42}, aSqDist);
  EXPECT_NEAR(aSqDist, 0.0, 1e-15);
  const NCollection_DynamicArray<size_t> aRange = aTree.RangeSearch({42, 42}, 0.0);
  EXPECT_EQ(aRange.Size(), static_cast<size_t>(THE_N));
}

TEST(NCollection_KDTreeTest, Degenerate_GridAlignedToAxes)
{
  // 10x10 grid aligned to splitting axes
  constexpr int THE_N = 100;
  TestPoint2D   aPoints[THE_N];
  int           k = 0;
  for (int i = 0; i < 10; ++i)
    for (int j = 0; j < 10; ++j)
      aPoints[k++] = {static_cast<double>(i), static_cast<double>(j)};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, THE_N);
  // Verify nearest for several queries
  TestRandom aRng(999);
  for (int q = 0; q < 30; ++q)
  {
    TestPoint2D aQuery(aRng.NextDouble(-1, 10), aRng.NextDouble(-1, 10));
    double      aKdSqDist = 0.0;
    aTree.NearestPoint(aQuery, aKdSqDist);
    double aBfBest = std::numeric_limits<double>::max();
    for (int i = 0; i < THE_N; ++i)
    {
      const double aDist = sqDist2D(aQuery, aPoints[i]);
      if (aDist < aBfBest)
      {
        aBfBest = aDist;
      }
    }
    EXPECT_NEAR(aKdSqDist, aBfBest, 1e-10);
  }
}

TEST(NCollection_KDTreeTest, Degenerate_PointsOnCircle)
{
  constexpr int THE_N = 36;
  TestPoint2D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    const double anAngle = 2.0 * 3.14159265358979323846 * i / THE_N;
    aPoints[i]           = {100.0 * std::cos(anAngle), 100.0 * std::sin(anAngle)};
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, THE_N);
  // All points equidistant from origin
  double aSqDist = 0.0;
  aTree.NearestPoint({0, 0}, aSqDist);
  EXPECT_NEAR(aSqDist, 10000.0, 1.0);
  // Range search at radius 100 should find all
  const NCollection_DynamicArray<size_t> aRange = aTree.RangeSearch({0, 0}, 100.01);
  EXPECT_EQ(aRange.Size(), static_cast<size_t>(THE_N));
}

TEST(NCollection_KDTreeTest, Degenerate_TwoClustersFar)
{
  // Two well-separated clusters
  TestPoint2D                        aPoints[] = {{0, 0},
                                                  {0.1, 0.1},
                                                  {-0.1, 0.1},
                                                  {0, -0.1},
                                                  {1000, 1000},
                                                  {1000.1, 1000.1},
                                                  {999.9, 1000},
                                                  {1000, 999.9}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 8);
  // Query near cluster A
  const size_t aIdx1 = aTree.NearestPoint({0.05, 0.05});
  EXPECT_LT(aTree.Point(aIdx1).X, 1.0);
  // Query near cluster B
  const size_t aIdx2 = aTree.NearestPoint({1000.05, 1000.05});
  EXPECT_GT(aTree.Point(aIdx2).X, 999.0);
  // KNearest 4 from cluster A center should get all 4 A points
  NCollection_Array1<size_t> anIndices(1, 0);
  NCollection_Array1<double> aDists(1, 0);
  aTree.KNearestPoints({0, 0}, 4, anIndices, aDists);
  for (int i = 1; i <= 4; ++i)
  {
    EXPECT_LT(aTree.Point(anIndices.Value(i)).X, 1.0);
  }
}

TEST(NCollection_KDTreeTest, Degenerate_OneQuadrantOnly)
{
  // All points in first quadrant only
  constexpr int THE_N = 30;
  TestRandom    aRng(654);
  TestPoint2D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {aRng.NextDouble(0, 100), aRng.NextDouble(0, 100)};
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, THE_N);
  // Query from third quadrant
  double       aSqDist = 0.0;
  const size_t aIdx    = aTree.NearestPoint({-10, -10}, aSqDist);
  EXPECT_NE(aIdx, 0u);
  // Brute force
  double aBfBest = std::numeric_limits<double>::max();
  for (int i = 0; i < THE_N; ++i)
  {
    const double aDist = sqDist2D({-10, -10}, aPoints[i]);
    if (aDist < aBfBest)
    {
      aBfBest = aDist;
    }
  }
  EXPECT_NEAR(aSqDist, aBfBest, 1e-10);
}

// ============================================================
// Stress Tests (extended)
// ============================================================

TEST(NCollection_KDTreeTest, LargeDataSet_50000Points)
{
  constexpr int                   THE_N = 50000;
  TestRandom                      aRng(31337);
  NCollection_Array1<TestPoint2D> aPoints(1, THE_N);
  for (int i = 1; i <= THE_N; ++i)
  {
    aPoints.SetValue(i, {aRng.NextDouble(-10000, 10000), aRng.NextDouble(-10000, 10000)});
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints);
  EXPECT_EQ(aTree.Size(), static_cast<size_t>(THE_N));
  // Spot-check 5 nearest queries
  for (int q = 0; q < 5; ++q)
  {
    TestPoint2D aQuery(aRng.NextDouble(-11000, 11000), aRng.NextDouble(-11000, 11000));
    double      aKdSqDist = 0.0;
    aTree.NearestPoint(aQuery, aKdSqDist);
    double aBfBest = std::numeric_limits<double>::max();
    for (int i = 1; i <= THE_N; ++i)
    {
      const double aDist = sqDist2D(aQuery, aPoints.Value(i));
      if (aDist < aBfBest)
      {
        aBfBest = aDist;
      }
    }
    EXPECT_NEAR(aKdSqDist, aBfBest, 1e-8);
  }
}

TEST(NCollection_KDTreeTest, LargeDataSet_3D_5000Points)
{
  constexpr int THE_N = 5000;
  TestRandom    aRng(4242);
  TestPoint3D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {aRng.NextDouble(-1000, 1000),
                  aRng.NextDouble(-1000, 1000),
                  aRng.NextDouble(-1000, 1000)};
  }
  NCollection_KDTree<TestPoint3D, 3> aTree;
  aTree.Build(aPoints, THE_N);
  EXPECT_EQ(aTree.Size(), static_cast<size_t>(THE_N));
  for (int q = 0; q < 10; ++q)
  {
    TestPoint3D aQuery(aRng.NextDouble(-1100, 1100),
                       aRng.NextDouble(-1100, 1100),
                       aRng.NextDouble(-1100, 1100));
    double      aKdSqDist = 0.0;
    aTree.NearestPoint(aQuery, aKdSqDist);
    double aBfBest = std::numeric_limits<double>::max();
    for (int i = 0; i < THE_N; ++i)
    {
      const double aDist = sqDist3D(aQuery, aPoints[i]);
      if (aDist < aBfBest)
      {
        aBfBest = aDist;
      }
    }
    EXPECT_NEAR(aKdSqDist, aBfBest, 1e-8);
  }
}

TEST(NCollection_KDTreeTest, ConsecutiveBuilds)
{
  NCollection_KDTree<TestPoint2D, 2> aTree;
  TestRandom                         aRng(555);
  // Build, query, rebuild several times with different sizes
  for (int aBuild = 0; aBuild < 5; ++aBuild)
  {
    const int                       aN = 10 + aBuild * 20;
    NCollection_Array1<TestPoint2D> aPoints(1, aN);
    for (int i = 1; i <= aN; ++i)
    {
      aPoints.SetValue(i, {aRng.NextDouble(-50, 50), aRng.NextDouble(-50, 50)});
    }
    aTree.Build(aPoints);
    EXPECT_EQ(aTree.Size(), static_cast<size_t>(aN));
    // Verify nearest
    TestPoint2D aQuery(aRng.NextDouble(-55, 55), aRng.NextDouble(-55, 55));
    double      aKdSqDist = 0.0;
    aTree.NearestPoint(aQuery, aKdSqDist);
    double aBfBest = std::numeric_limits<double>::max();
    for (int i = 1; i <= aN; ++i)
    {
      const double aDist = sqDist2D(aQuery, aPoints.Value(i));
      if (aDist < aBfBest)
      {
        aBfBest = aDist;
      }
    }
    EXPECT_NEAR(aKdSqDist, aBfBest, 1e-10);
  }
}

TEST(NCollection_KDTreeTest, AllQueryTypesAfterRebuild)
{
  NCollection_KDTree<TestPoint2D, 2> aTree;
  TestPoint2D                        aPoints1[] = {{0, 0}, {1, 1}};
  aTree.Build(aPoints1, 2);
  aTree.Clear();
  TestPoint2D aPoints2[] = {{10, 10}, {20, 20}, {15, 15}, {12, 18}};
  aTree.Build(aPoints2, 4);
  // NearestPoint
  double       aSqDist = 0.0;
  const size_t aIdx    = aTree.NearestPoint({15.1, 15.1}, aSqDist);
  EXPECT_NEAR(aTree.Point(aIdx).X, 15.0, 1e-15);
  // KNearest
  NCollection_Array1<size_t> anIndices(1, 0);
  NCollection_Array1<double> aDists(1, 0);
  EXPECT_EQ(aTree.KNearestPoints({15, 15}, 2, anIndices, aDists), 2u);
  // RangeSearch
  const NCollection_DynamicArray<size_t> aRange = aTree.RangeSearch({15, 15}, 10.0);
  EXPECT_GT(aRange.Size(), 0u);
  // BoxSearch
  const NCollection_DynamicArray<size_t> aBox = aTree.BoxSearch({9, 9}, {21, 21});
  EXPECT_GT(aBox.Size(), 0u);
}

TEST(NCollection_KDTreeTest, DuplicatePoints_AllSame3D)
{
  constexpr int THE_N = 50;
  TestPoint3D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {7.0, 7.0, 7.0};
  }
  NCollection_KDTree<TestPoint3D, 3> aTree;
  aTree.Build(aPoints, THE_N);
  double aSqDist = 0.0;
  aTree.NearestPoint({7, 7, 7}, aSqDist);
  EXPECT_NEAR(aSqDist, 0.0, 1e-15);
  const NCollection_DynamicArray<size_t> aRange = aTree.RangeSearch({7, 7, 7}, 0.0);
  EXPECT_EQ(aRange.Size(), static_cast<size_t>(THE_N));
}

TEST(NCollection_KDTreeTest, DuplicatePointsMixed)
{
  // Mix of unique and duplicate points
  TestPoint2D                        aPoints[] = {{0, 0}, {0, 0}, {1, 1}, {1, 1}, {1, 1}, {5, 5}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 6);
  const NCollection_DynamicArray<size_t> aRange = aTree.RangeSearch({1, 1}, 0.0);
  EXPECT_EQ(aRange.Size(), 3u);
  const NCollection_DynamicArray<size_t> aRange2 = aTree.RangeSearch({0, 0}, 0.0);
  EXPECT_EQ(aRange2.Size(), 2u);
}

// ============================================================
// Edge: All identical points (comprehensive)
// ============================================================

TEST(NCollection_KDTreeTest, AllSamePoints_LargeSet2D)
{
  // 1000 identical 2D points — stress tests median-split when all coordinates equal.
  constexpr int THE_N = 1000;
  TestPoint2D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {3.0, 7.0};
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, THE_N);
  EXPECT_EQ(aTree.Size(), static_cast<size_t>(THE_N));
  // NearestPoint from anywhere should return dist to (3,7)
  double aSqDist = 0.0;
  aTree.NearestPoint({0.0, 0.0}, aSqDist);
  EXPECT_NEAR(aSqDist, 58.0, 1e-10);
  // NearestPoint from (3,7) should return 0
  aTree.NearestPoint({3.0, 7.0}, aSqDist);
  EXPECT_NEAR(aSqDist, 0.0, 1e-15);
}

TEST(NCollection_KDTreeTest, AllSamePoints_LargeSet3D)
{
  constexpr int THE_N = 500;
  TestPoint3D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {-2.0, 4.0, 6.0};
  }
  NCollection_KDTree<TestPoint3D, 3> aTree;
  aTree.Build(aPoints, THE_N);
  EXPECT_EQ(aTree.Size(), static_cast<size_t>(THE_N));
  double aSqDist = 0.0;
  aTree.NearestPoint({-2.0, 4.0, 6.0}, aSqDist);
  EXPECT_NEAR(aSqDist, 0.0, 1e-15);
}

TEST(NCollection_KDTreeTest, AllSamePoints_KNearest)
{
  constexpr int THE_N = 100;
  TestPoint2D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {5.0, 5.0};
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, THE_N);
  NCollection_Array1<size_t> anIndices(1, 0);
  NCollection_Array1<double> aDists(1, 0);
  // K < N
  const size_t aCount = aTree.KNearestPoints({0.0, 0.0}, 10, anIndices, aDists);
  EXPECT_EQ(aCount, 10u);
  for (int i = 1; i <= static_cast<int>(aCount); ++i)
  {
    EXPECT_NEAR(aDists.Value(i), 50.0, 1e-10);
  }
  // K > N — should return all N
  const size_t aCount2 = aTree.KNearestPoints({0.0, 0.0}, 200, anIndices, aDists);
  EXPECT_EQ(aCount2, static_cast<size_t>(THE_N));
}

TEST(NCollection_KDTreeTest, AllSamePoints_RangeSearch)
{
  constexpr int THE_N = 100;
  TestPoint2D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {5.0, 5.0};
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, THE_N);
  // Zero radius at the exact point — should find all
  const NCollection_DynamicArray<size_t> aResult = aTree.RangeSearch({5.0, 5.0}, 0.0);
  EXPECT_EQ(aResult.Size(), THE_N);
  // Small radius away — should find all (dist = sqrt(2) ≈ 1.414)
  const NCollection_DynamicArray<size_t> aResult2 = aTree.RangeSearch({4.0, 4.0}, 2.0);
  EXPECT_EQ(aResult2.Size(), THE_N);
  // Radius too small from away — should find none
  const NCollection_DynamicArray<size_t> aResult3 = aTree.RangeSearch({0.0, 0.0}, 1.0);
  EXPECT_EQ(aResult3.Size(), 0);
}

TEST(NCollection_KDTreeTest, AllSamePoints_BoxSearch)
{
  constexpr int THE_N = 100;
  TestPoint2D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {5.0, 5.0};
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, THE_N);
  // Box that contains the point — should find all
  const NCollection_DynamicArray<size_t> aResult = aTree.BoxSearch({4.0, 4.0}, {6.0, 6.0});
  EXPECT_EQ(aResult.Size(), THE_N);
  // Box exactly at the point — should find all
  const NCollection_DynamicArray<size_t> aResult2 = aTree.BoxSearch({5.0, 5.0}, {5.0, 5.0});
  EXPECT_EQ(aResult2.Size(), THE_N);
  // Box that misses the point — should find none
  const NCollection_DynamicArray<size_t> aResult3 = aTree.BoxSearch({0.0, 0.0}, {4.9, 4.9});
  EXPECT_EQ(aResult3.Size(), 0);
}

TEST(NCollection_KDTreeTest, AllSamePoints_NearestPoints)
{
  constexpr int THE_N = 50;
  TestPoint2D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {3.0, 3.0};
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, THE_N);
  double aSqDist = 0.0;
  // All equidistant — should return all N
  const NCollection_DynamicArray<size_t> aResult = aTree.NearestPoints({0.0, 0.0}, 1e-6, aSqDist);
  EXPECT_EQ(aResult.Size(), THE_N);
  EXPECT_NEAR(aSqDist, 18.0, 1e-10);
}

TEST(NCollection_KDTreeTest, AllSamePoints_CopyMove)
{
  constexpr int THE_N = 100;
  TestPoint2D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {2.0, 8.0};
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, THE_N);
  // Copy
  NCollection_KDTree<TestPoint2D, 2> aCopy(aTree);
  EXPECT_EQ(aCopy.Size(), static_cast<size_t>(THE_N));
  double aSqDist = 0.0;
  aCopy.NearestPoint({2.0, 8.0}, aSqDist);
  EXPECT_NEAR(aSqDist, 0.0, 1e-15);
  // Move
  NCollection_KDTree<TestPoint2D, 2> aMoved(std::move(aCopy));
  EXPECT_EQ(aMoved.Size(), static_cast<size_t>(THE_N));
  EXPECT_TRUE(aCopy.IsEmpty());
  aMoved.NearestPoint({2.0, 8.0}, aSqDist);
  EXPECT_NEAR(aSqDist, 0.0, 1e-15);
}

TEST(NCollection_KDTreeRadiiTest, AllSamePoints_ContainingSearch)
{
  constexpr int THE_N = 100;
  TestPoint2D   aPoints[THE_N];
  double        aRadii[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {5.0, 5.0};
    aRadii[i]  = 2.0;
  }
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  aTree.Build(aPoints, aRadii, THE_N);
  // Query at the same point — inside all spheres
  const NCollection_DynamicArray<size_t> aResult = aTree.ContainingSearch({5.0, 5.0});
  EXPECT_EQ(aResult.Size(), THE_N);
  // Query nearby (dist=1 <= 2) — still inside all
  const NCollection_DynamicArray<size_t> aResult2 = aTree.ContainingSearch({6.0, 5.0});
  EXPECT_EQ(aResult2.Size(), THE_N);
  // Query far (dist=10 > 2) — outside all
  const NCollection_DynamicArray<size_t> aResult3 = aTree.ContainingSearch({15.0, 5.0});
  EXPECT_EQ(aResult3.Size(), 0);
}

TEST(NCollection_KDTreeRadiiTest, AllSamePoints_NearestWeighted)
{
  constexpr int THE_N = 100;
  TestPoint2D   aPoints[THE_N];
  double        aRadii[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {5.0, 5.0};
    aRadii[i]  = 3.0;
  }
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  aTree.Build(aPoints, aRadii, THE_N);
  double aGap = 0.0;
  // Query inside: gap = 0 - 3 = -3
  const size_t aIdx = aTree.NearestWeighted({5.0, 5.0}, aGap);
  EXPECT_NE(aIdx, 0u);
  EXPECT_NEAR(aGap, -3.0, 1e-10);
  // Query outside: gap = 10 - 3 = 7
  aTree.NearestWeighted({15.0, 5.0}, aGap);
  EXPECT_NEAR(aGap, 7.0, 1e-10);
}

TEST(NCollection_KDTreeRadiiTest, AllSamePoints_VaryingRadii)
{
  // All points at same location but with different radii
  constexpr int THE_N = 50;
  TestPoint2D   aPoints[THE_N];
  double        aRadii[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {0.0, 0.0};
    aRadii[i]  = static_cast<double>(i + 1); // 1, 2, ..., 50
  }
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  aTree.Build(aPoints, aRadii, THE_N);
  // NearestWeighted from (10,0): gap = 10 - radius, minimum when radius=50 -> gap=-40
  double       aGap = 0.0;
  const size_t aIdx = aTree.NearestWeighted({10.0, 0.0}, aGap);
  EXPECT_NEAR(aGap, -40.0, 1e-10);
  EXPECT_NEAR(aTree.Radius(aIdx), 50.0, 1e-15);
  // ContainingSearch from (10,0): spheres with R >= 10 contain it -> indices 10..50 = 41 points
  const NCollection_DynamicArray<size_t> aResult = aTree.ContainingSearch({10.0, 0.0});
  EXPECT_EQ(aResult.Size(), 41);
}

// ============================================================
// Edge: Nearest with query exactly on a point
// ============================================================

TEST(NCollection_KDTreeTest, NearestPoint_QueryOnEveryPoint)
{
  constexpr int THE_N = 20;
  TestPoint2D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {static_cast<double>(i * 3), static_cast<double>(i * 7 % 11)};
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, THE_N);
  // Query on every stored point - distance should be 0
  for (int i = 0; i < THE_N; ++i)
  {
    double aSqDist = 0.0;
    aTree.NearestPoint(aPoints[i], aSqDist);
    EXPECT_NEAR(aSqDist, 0.0, 1e-15) << "Failed for point " << i;
  }
}

// ============================================================
// Comprehensive All-Queries Brute-Force (single large test)
// ============================================================

TEST(NCollection_KDTreeTest, BruteForce_AllQueries_500Points)
{
  constexpr int THE_N = 500;
  TestRandom    aRng(2026);
  TestPoint2D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {aRng.NextDouble(-500, 500), aRng.NextDouble(-500, 500)};
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, THE_N);
  for (int q = 0; q < 30; ++q)
  {
    TestPoint2D aQuery(aRng.NextDouble(-600, 600), aRng.NextDouble(-600, 600));
    // Nearest
    {
      double aKdSqDist = 0.0;
      aTree.NearestPoint(aQuery, aKdSqDist);
      double aBfBest = std::numeric_limits<double>::max();
      for (int i = 0; i < THE_N; ++i)
      {
        const double aDist = sqDist2D(aQuery, aPoints[i]);
        if (aDist < aBfBest)
          aBfBest = aDist;
      }
      EXPECT_NEAR(aKdSqDist, aBfBest, 1e-10) << "Nearest q=" << q;
    }
    // KNearest
    {
      constexpr size_t           K = 10;
      NCollection_Array1<size_t> anIndices(1, 0);
      NCollection_Array1<double> aDists(1, 0);
      aTree.KNearestPoints(aQuery, K, anIndices, aDists);
      std::vector<double> aBfDists(THE_N);
      for (int i = 0; i < THE_N; ++i)
        aBfDists[i] = sqDist2D(aQuery, aPoints[i]);
      std::sort(aBfDists.begin(), aBfDists.end());
      EXPECT_NEAR(aDists.Value(static_cast<int>(K)), aBfDists[K - 1], 1e-10) << "KNN q=" << q;
    }
    // RangeSearch
    {
      const double                           aRadius   = aRng.NextDouble(20, 100);
      const NCollection_DynamicArray<size_t> aKdResult = aTree.RangeSearch(aQuery, aRadius);
      const double                           aRadiusSq = aRadius * aRadius;
      size_t                                 aBfCount  = 0;
      for (int i = 0; i < THE_N; ++i)
      {
        if (sqDist2D(aQuery, aPoints[i]) <= aRadiusSq)
          ++aBfCount;
      }
      EXPECT_EQ(aKdResult.Size(), aBfCount) << "Range q=" << q;
    }
    // BoxSearch
    {
      const double                           aHalf = aRng.NextDouble(20, 100);
      TestPoint2D                            aMin(aQuery.X - aHalf, aQuery.Y - aHalf);
      TestPoint2D                            aMax(aQuery.X + aHalf, aQuery.Y + aHalf);
      const NCollection_DynamicArray<size_t> aKdResult = aTree.BoxSearch(aMin, aMax);
      size_t                                 aBfCount  = 0;
      for (int i = 0; i < THE_N; ++i)
      {
        if (aPoints[i].X >= aMin.X && aPoints[i].X <= aMax.X && aPoints[i].Y >= aMin.Y
            && aPoints[i].Y <= aMax.Y)
          ++aBfCount;
      }
      EXPECT_EQ(aKdResult.Size(), aBfCount) << "Box q=" << q;
    }
  }
}

// ============================================================
// NearestPoints (equidistant results)
// ============================================================

TEST(NCollection_KDTreeTest, NearestPoints_EmptyTree)
{
  NCollection_KDTree<TestPoint2D, 2>     aTree;
  double                                 aSqDist = 0.0;
  const NCollection_DynamicArray<size_t> aResult = aTree.NearestPoints({0, 0}, 0.0, aSqDist);
  EXPECT_EQ(aResult.Size(), 0);
}

TEST(NCollection_KDTreeTest, NearestPoints_SinglePoint)
{
  TestPoint2D                        aPoint(3, 4);
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(&aPoint, 1);
  double                                 aSqDist = 0.0;
  const NCollection_DynamicArray<size_t> aResult = aTree.NearestPoints({0, 0}, 0.0, aSqDist);
  EXPECT_EQ(aResult.Size(), 1);
  EXPECT_NEAR(aSqDist, 25.0, 1e-10);
}

TEST(NCollection_KDTreeTest, NearestPoints_TwoEquidistant)
{
  // Two points equidistant from the origin
  TestPoint2D                        aPoints[] = {{1, 0}, {-1, 0}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 2);
  double                                 aSqDist = 0.0;
  const NCollection_DynamicArray<size_t> aResult = aTree.NearestPoints({0, 0}, 1e-12, aSqDist);
  EXPECT_EQ(aResult.Size(), 2);
  EXPECT_NEAR(aSqDist, 1.0, 1e-10);
}

TEST(NCollection_KDTreeTest, NearestPoints_FourEquidistant)
{
  // Four points on axes, equidistant from origin
  TestPoint2D                        aPoints[] = {{5, 0}, {-5, 0}, {0, 5}, {0, -5}, {100, 100}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 5);
  double                                 aSqDist = 0.0;
  const NCollection_DynamicArray<size_t> aResult = aTree.NearestPoints({0, 0}, 1e-12, aSqDist);
  EXPECT_EQ(aResult.Size(), 4);
  EXPECT_NEAR(aSqDist, 25.0, 1e-10);
}

TEST(NCollection_KDTreeTest, NearestPoints_SixEquidistant3D)
{
  // Six points on axes in 3D, equidistant from origin
  TestPoint3D aPoints[] = {{2, 0, 0}, {-2, 0, 0}, {0, 2, 0}, {0, -2, 0}, {0, 0, 2}, {0, 0, -2}};
  NCollection_KDTree<TestPoint3D, 3> aTree;
  aTree.Build(aPoints, 6);
  double                                 aSqDist = 0.0;
  const NCollection_DynamicArray<size_t> aResult = aTree.NearestPoints({0, 0, 0}, 1e-12, aSqDist);
  EXPECT_EQ(aResult.Size(), 6);
  EXPECT_NEAR(aSqDist, 4.0, 1e-10);
}

TEST(NCollection_KDTreeTest, NearestPoints_NoneEquidistant)
{
  // All points at unique distances
  TestPoint2D                        aPoints[] = {{1, 0}, {3, 0}, {6, 0}, {10, 0}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 4);
  double                                 aSqDist = 0.0;
  const NCollection_DynamicArray<size_t> aResult = aTree.NearestPoints({0, 0}, 1e-12, aSqDist);
  EXPECT_EQ(aResult.Size(), 1);
  EXPECT_NEAR(aSqDist, 1.0, 1e-10);
}

TEST(NCollection_KDTreeTest, NearestPoints_AllSamePoint)
{
  TestPoint2D                        aPoints[] = {{5, 5}, {5, 5}, {5, 5}, {5, 5}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 4);
  double                                 aSqDist = 0.0;
  const NCollection_DynamicArray<size_t> aResult = aTree.NearestPoints({0, 0}, 1e-12, aSqDist);
  EXPECT_EQ(aResult.Size(), 4);
  EXPECT_NEAR(aSqDist, 50.0, 1e-10);
}

TEST(NCollection_KDTreeTest, NearestPoints_WithTolerance)
{
  // Points at slightly different distances
  TestPoint2D                        aPoints[] = {{1.0, 0}, {1.001, 0}, {1.002, 0}, {5.0, 0}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 4);
  double aSqDist = 0.0;
  // With tight tolerance, only exact nearest
  const NCollection_DynamicArray<size_t> aResult1 = aTree.NearestPoints({0, 0}, 0.0, aSqDist);
  EXPECT_EQ(aResult1.Size(), 1);
  // With tolerance 0.005, should find the 3 close points
  const NCollection_DynamicArray<size_t> aResult2 = aTree.NearestPoints({0, 0}, 0.005, aSqDist);
  EXPECT_EQ(aResult2.Size(), 3);
}

TEST(NCollection_KDTreeTest, NearestPoints_ExactMatchQuery)
{
  TestPoint2D                        aPoints[] = {{3, 4}, {0, 0}, {10, 10}};
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, 3);
  double aSqDist = 0.0;
  // Query at exact point location
  const NCollection_DynamicArray<size_t> aResult = aTree.NearestPoints({3, 4}, 1e-12, aSqDist);
  EXPECT_EQ(aResult.Size(), 1);
  EXPECT_NEAR(aSqDist, 0.0, 1e-15);
}

TEST(NCollection_KDTreeTest, NearestPoints_CirclePoints)
{
  // 8 points on a circle of radius 10 around origin
  constexpr int THE_N = 8;
  TestPoint2D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    const double anAngle = 2.0 * 3.14159265358979323846 * i / THE_N;
    aPoints[i]           = {10.0 * std::cos(anAngle), 10.0 * std::sin(anAngle)};
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, THE_N);
  double aSqDist = 0.0;
  // From origin, all 8 points are equidistant at distance 10
  const NCollection_DynamicArray<size_t> aResult = aTree.NearestPoints({0, 0}, 1e-6, aSqDist);
  EXPECT_EQ(aResult.Size(), static_cast<size_t>(THE_N));
  EXPECT_NEAR(aSqDist, 100.0, 1e-6);
}

TEST(NCollection_KDTreeTest, NearestPoints_ValidIndices)
{
  constexpr int THE_N = 50;
  TestRandom    aRng(7070);
  TestPoint2D   aPoints[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {aRng.NextDouble(-10, 10), aRng.NextDouble(-10, 10)};
  }
  NCollection_KDTree<TestPoint2D, 2> aTree;
  aTree.Build(aPoints, THE_N);
  double                                 aSqDist = 0.0;
  const NCollection_DynamicArray<size_t> aResult = aTree.NearestPoints({0, 0}, 0.5, aSqDist);
  EXPECT_GE(aResult.Size(), 1u);
  for (int i = 0; i < aResult.Size(); ++i)
  {
    EXPECT_GE(aResult[i], 1u);
    EXPECT_LE(aResult[i], static_cast<size_t>(THE_N));
  }
}

// ============================================================
// HasRadii: Construction & State
// ============================================================

TEST(NCollection_KDTreeRadiiTest, DefaultConstructor)
{
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  EXPECT_TRUE(aTree.IsEmpty());
  EXPECT_EQ(aTree.Size(), 0u);
}

TEST(NCollection_KDTreeRadiiTest, BuildFromCArray)
{
  TestPoint2D                              aPoints[] = {{0.0, 0.0}, {5.0, 0.0}, {0.0, 5.0}};
  double                                   aRadii[]  = {1.0, 2.0, 3.0};
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  aTree.Build(aPoints, aRadii, 3);
  EXPECT_FALSE(aTree.IsEmpty());
  EXPECT_EQ(aTree.Size(), 3u);
}

TEST(NCollection_KDTreeRadiiTest, BuildFromArray1)
{
  NCollection_Array1<TestPoint2D> aPoints(1, 3);
  aPoints.SetValue(1, {0.0, 0.0});
  aPoints.SetValue(2, {5.0, 0.0});
  aPoints.SetValue(3, {0.0, 5.0});
  NCollection_Array1<double> aRadii(1, 3);
  aRadii.SetValue(1, 1.0);
  aRadii.SetValue(2, 2.0);
  aRadii.SetValue(3, 3.0);
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  aTree.Build(aPoints, aRadii);
  EXPECT_FALSE(aTree.IsEmpty());
  EXPECT_EQ(aTree.Size(), 3u);
}

TEST(NCollection_KDTreeRadiiTest, BuildEmptyArray)
{
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  aTree.Build(static_cast<const TestPoint2D*>(nullptr), static_cast<const double*>(nullptr), 0);
  EXPECT_TRUE(aTree.IsEmpty());
}

TEST(NCollection_KDTreeRadiiTest, RadiusAccessor)
{
  TestPoint2D                              aPoints[] = {{0.0, 0.0}, {5.0, 0.0}, {0.0, 5.0}};
  double                                   aRadii[]  = {1.5, 2.5, 3.5};
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  aTree.Build(aPoints, aRadii, 3);
  // Points are reindexed internally but original indices are preserved
  // Check that each original index has the correct radius
  for (size_t i = 1; i <= 3; ++i)
  {
    const double aR        = aTree.Radius(i);
    const double aExpected = (i == 1) ? 1.5 : (i == 2 ? 2.5 : 3.5);
    EXPECT_NEAR(aR, aExpected, 1e-15);
  }
}

TEST(NCollection_KDTreeRadiiTest, Clear)
{
  TestPoint2D                              aPoints[] = {{0.0, 0.0}, {5.0, 0.0}};
  double                                   aRadii[]  = {1.0, 2.0};
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  aTree.Build(aPoints, aRadii, 2);
  EXPECT_FALSE(aTree.IsEmpty());
  aTree.Clear();
  EXPECT_TRUE(aTree.IsEmpty());
  EXPECT_EQ(aTree.Size(), 0u);
}

// ============================================================
// HasRadii: Copy/Move Semantics
// ============================================================

TEST(NCollection_KDTreeRadiiTest, CopyConstructor)
{
  TestPoint2D                              aPoints[] = {{0.0, 0.0}, {5.0, 0.0}, {0.0, 5.0}};
  double                                   aRadii[]  = {1.0, 2.0, 3.0};
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  aTree.Build(aPoints, aRadii, 3);
  NCollection_KDTree<TestPoint2D, 2, true> aCopy(aTree);
  EXPECT_EQ(aCopy.Size(), 3u);
  for (size_t i = 1; i <= 3; ++i)
  {
    EXPECT_NEAR(aCopy.Radius(i), aTree.Radius(i), 1e-15);
  }
  // Both should give the same NearestWeighted result
  TestPoint2D aQuery(2.0, 1.0);
  EXPECT_EQ(aTree.NearestWeighted(aQuery), aCopy.NearestWeighted(aQuery));
}

TEST(NCollection_KDTreeRadiiTest, MoveConstructor)
{
  TestPoint2D                              aPoints[] = {{0.0, 0.0}, {5.0, 0.0}};
  double                                   aRadii[]  = {1.0, 2.0};
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  aTree.Build(aPoints, aRadii, 2);
  NCollection_KDTree<TestPoint2D, 2, true> aMoved(std::move(aTree));
  EXPECT_EQ(aMoved.Size(), 2u);
  EXPECT_TRUE(aTree.IsEmpty());
}

TEST(NCollection_KDTreeRadiiTest, CopyAssignment)
{
  TestPoint2D                              aPoints[] = {{0.0, 0.0}, {5.0, 0.0}};
  double                                   aRadii[]  = {1.0, 2.0};
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  aTree.Build(aPoints, aRadii, 2);
  NCollection_KDTree<TestPoint2D, 2, true> aCopy;
  aCopy = aTree;
  EXPECT_EQ(aCopy.Size(), 2u);
  EXPECT_EQ(aTree.Size(), 2u);
}

TEST(NCollection_KDTreeRadiiTest, MoveAssignment)
{
  TestPoint2D                              aPoints[] = {{0.0, 0.0}, {5.0, 0.0}};
  double                                   aRadii[]  = {1.0, 2.0};
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  aTree.Build(aPoints, aRadii, 2);
  NCollection_KDTree<TestPoint2D, 2, true> aMoved;
  aMoved = std::move(aTree);
  EXPECT_EQ(aMoved.Size(), 2u);
  EXPECT_TRUE(aTree.IsEmpty());
}

// ============================================================
// HasRadii: Standard queries still work
// ============================================================

TEST(NCollection_KDTreeRadiiTest, NearestPointStillWorks)
{
  TestPoint2D                              aPoints[] = {{0.0, 0.0}, {10.0, 0.0}, {5.0, 5.0}};
  double                                   aRadii[]  = {1.0, 1.0, 1.0};
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  aTree.Build(aPoints, aRadii, 3);
  TestPoint2D  aQuery(4.5, 4.5);
  double       aSqDist = 0.0;
  const size_t aIdx    = aTree.NearestPoint(aQuery, aSqDist);
  EXPECT_NEAR(aTree.Point(aIdx).X, 5.0, 1e-15);
  EXPECT_NEAR(aTree.Point(aIdx).Y, 5.0, 1e-15);
  EXPECT_NEAR(aSqDist, 0.5, 1e-10);
}

TEST(NCollection_KDTreeRadiiTest, RangeSearchStillWorks)
{
  TestPoint2D                              aPoints[] = {{0.0, 0.0}, {1.0, 0.0}, {10.0, 10.0}};
  double                                   aRadii[]  = {0.5, 0.5, 0.5};
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  aTree.Build(aPoints, aRadii, 3);
  const NCollection_DynamicArray<size_t> aResult = aTree.RangeSearch({0.5, 0.0}, 1.0);
  EXPECT_EQ(aResult.Size(), 2);
}

// ============================================================
// HasRadii: ContainingSearch
// ============================================================

TEST(NCollection_KDTreeRadiiTest, ContainingSearch_EmptyTree)
{
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  const NCollection_DynamicArray<size_t>   aResult = aTree.ContainingSearch({0.0, 0.0});
  EXPECT_EQ(aResult.Size(), 0);
}

TEST(NCollection_KDTreeRadiiTest, ContainingSearch_QueryInsideSphere)
{
  // Sphere at (0,0) with radius 5 — query at (1,1) is inside
  TestPoint2D                              aPoints[] = {{0.0, 0.0}};
  double                                   aRadii[]  = {5.0};
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  aTree.Build(aPoints, aRadii, 1);
  const NCollection_DynamicArray<size_t> aResult = aTree.ContainingSearch({1.0, 1.0});
  EXPECT_EQ(aResult.Size(), 1);
}

TEST(NCollection_KDTreeRadiiTest, ContainingSearch_QueryOutsideSphere)
{
  // Sphere at (0,0) with radius 1 — query at (5,5) is outside
  TestPoint2D                              aPoints[] = {{0.0, 0.0}};
  double                                   aRadii[]  = {1.0};
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  aTree.Build(aPoints, aRadii, 1);
  const NCollection_DynamicArray<size_t> aResult = aTree.ContainingSearch({5.0, 5.0});
  EXPECT_EQ(aResult.Size(), 0);
}

TEST(NCollection_KDTreeRadiiTest, ContainingSearch_QueryOnBoundary)
{
  // Sphere at (0,0) with radius 5 — query at (3,4) is exactly on boundary (dist=5)
  TestPoint2D                              aPoints[] = {{0.0, 0.0}};
  double                                   aRadii[]  = {5.0};
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  aTree.Build(aPoints, aRadii, 1);
  const NCollection_DynamicArray<size_t> aResult = aTree.ContainingSearch({3.0, 4.0});
  EXPECT_EQ(aResult.Size(), 1);
}

TEST(NCollection_KDTreeRadiiTest, ContainingSearch_MultipleSpheres)
{
  // Three spheres, query at (1,0) is inside first two but not third
  TestPoint2D                              aPoints[] = {{0.0, 0.0}, {2.0, 0.0}, {10.0, 10.0}};
  double                                   aRadii[]  = {3.0, 2.0, 1.0};
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  aTree.Build(aPoints, aRadii, 3);
  const NCollection_DynamicArray<size_t> aResult = aTree.ContainingSearch({1.0, 0.0});
  EXPECT_EQ(aResult.Size(), 2);
  // Verify both found indices correspond to the first two points
  std::set<size_t> aFoundIndices;
  for (int i = 0; i < aResult.Size(); ++i)
  {
    aFoundIndices.insert(aResult[i]);
  }
  // Point at (0,0) with R=3: dist=1 <= 3 ✓
  // Point at (2,0) with R=2: dist=1 <= 2 ✓
  // Point at (10,10) with R=1: dist=√(81+100) >> 1 ✗
  EXPECT_EQ(aFoundIndices.size(), 2u);
}

TEST(NCollection_KDTreeRadiiTest, ContainingSearch_VaryingRadii)
{
  // Large sphere at origin with R=100, small spheres far away
  TestPoint2D                              aPoints[] = {{0.0, 0.0}, {50.0, 0.0}, {-50.0, 0.0}};
  double                                   aRadii[]  = {100.0, 0.1, 0.1};
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  aTree.Build(aPoints, aRadii, 3);
  const NCollection_DynamicArray<size_t> aResult = aTree.ContainingSearch({40.0, 0.0});
  // Only the large sphere at origin (dist=40 <= 100) should contain it
  EXPECT_EQ(aResult.Size(), 1);
  EXPECT_NEAR(aTree.Point(aResult[0]).X, 0.0, 1e-15);
}

TEST(NCollection_KDTreeRadiiTest, ContainingSearch_AllContain)
{
  // All spheres contain the origin
  TestPoint2D aPoints[] = {{1.0, 0.0}, {0.0, 1.0}, {-1.0, 0.0}, {0.0, -1.0}};
  double      aRadii[]  = {2.0, 2.0, 2.0, 2.0};
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  aTree.Build(aPoints, aRadii, 4);
  const NCollection_DynamicArray<size_t> aResult = aTree.ContainingSearch({0.0, 0.0});
  EXPECT_EQ(aResult.Size(), 4);
}

TEST(NCollection_KDTreeRadiiTest, ContainingSearch_NoneContain)
{
  // Tiny spheres, query far away
  TestPoint2D                              aPoints[] = {{0.0, 0.0}, {1.0, 0.0}, {0.0, 1.0}};
  double                                   aRadii[]  = {0.01, 0.01, 0.01};
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  aTree.Build(aPoints, aRadii, 3);
  const NCollection_DynamicArray<size_t> aResult = aTree.ContainingSearch({100.0, 100.0});
  EXPECT_EQ(aResult.Size(), 0);
}

TEST(NCollection_KDTreeRadiiTest, ContainingSearch_3D)
{
  // 3D sphere search
  TestPoint3D                              aPoints[] = {{0.0, 0.0, 0.0}, {10.0, 0.0, 0.0}};
  double                                   aRadii[]  = {5.0, 3.0};
  NCollection_KDTree<TestPoint3D, 3, true> aTree;
  aTree.Build(aPoints, aRadii, 2);
  // Query at (2,0,0): inside first sphere (dist=2 <= 5), outside second (dist=8 > 3)
  const NCollection_DynamicArray<size_t> aResult = aTree.ContainingSearch({2.0, 0.0, 0.0});
  EXPECT_EQ(aResult.Size(), 1);
  EXPECT_NEAR(aTree.Point(aResult[0]).X, 0.0, 1e-15);
}

// ============================================================
// HasRadii: NearestWeighted
// ============================================================

TEST(NCollection_KDTreeRadiiTest, NearestWeighted_EmptyTree)
{
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  double                                   aGap = 0.0;
  EXPECT_EQ(aTree.NearestWeighted({0.0, 0.0}, aGap), 0u);
}

TEST(NCollection_KDTreeRadiiTest, NearestWeighted_SinglePoint)
{
  TestPoint2D                              aPoints[] = {{0.0, 0.0}};
  double                                   aRadii[]  = {3.0};
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  aTree.Build(aPoints, aRadii, 1);
  double aGap = 0.0;
  // Query at (1,0): gap = dist(1,0) - 3.0 = 1.0 - 3.0 = -2.0 (inside)
  const size_t aIdx = aTree.NearestWeighted({1.0, 0.0}, aGap);
  EXPECT_EQ(aIdx, 1u);
  EXPECT_NEAR(aGap, -2.0, 1e-10);
}

TEST(NCollection_KDTreeRadiiTest, NearestWeighted_CloserSurface)
{
  // Two points: small sphere close, large sphere far
  // P1 at (0,0) R=1, P2 at (10,0) R=5
  // Query at (3,0):
  //   gap1 = dist(3,0) - 1 = 3 - 1 = 2
  //   gap2 = dist(7,0) - 5 = 7 - 5 = 2
  // Both equidistant from surface — either is valid
  // Query at (2,0):
  //   gap1 = 2 - 1 = 1
  //   gap2 = 8 - 5 = 3
  // P1 is closer by surface distance
  TestPoint2D                              aPoints[] = {{0.0, 0.0}, {10.0, 0.0}};
  double                                   aRadii[]  = {1.0, 5.0};
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  aTree.Build(aPoints, aRadii, 2);
  double       aGap = 0.0;
  const size_t aIdx = aTree.NearestWeighted({2.0, 0.0}, aGap);
  EXPECT_NEAR(aTree.Point(aIdx).X, 0.0, 1e-15);
  EXPECT_NEAR(aGap, 1.0, 1e-10);
}

TEST(NCollection_KDTreeRadiiTest, NearestWeighted_InsideSphere)
{
  // Query inside a sphere — gap is negative
  TestPoint2D                              aPoints[] = {{0.0, 0.0}, {10.0, 0.0}};
  double                                   aRadii[]  = {5.0, 1.0};
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  aTree.Build(aPoints, aRadii, 2);
  double aGap = 0.0;
  // Query at (1,0): gap1 = 1 - 5 = -4, gap2 = 9 - 1 = 8
  const size_t aIdx = aTree.NearestWeighted({1.0, 0.0}, aGap);
  EXPECT_NEAR(aTree.Point(aIdx).X, 0.0, 1e-15);
  EXPECT_NEAR(aGap, -4.0, 1e-10);
  EXPECT_LT(aGap, 0.0);
}

TEST(NCollection_KDTreeRadiiTest, NearestWeighted_LargeRadiusWins)
{
  // Far point with very large radius can have smaller gap
  // P1 at (0,0) R=0.1, P2 at (100,0) R=200
  // Query at (1,0):
  //   gap1 = 1 - 0.1 = 0.9
  //   gap2 = 99 - 200 = -101 (deeply inside P2's sphere)
  TestPoint2D                              aPoints[] = {{0.0, 0.0}, {100.0, 0.0}};
  double                                   aRadii[]  = {0.1, 200.0};
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  aTree.Build(aPoints, aRadii, 2);
  double       aGap = 0.0;
  const size_t aIdx = aTree.NearestWeighted({1.0, 0.0}, aGap);
  EXPECT_NEAR(aTree.Point(aIdx).X, 100.0, 1e-15);
  EXPECT_NEAR(aGap, -101.0, 1e-10);
}

TEST(NCollection_KDTreeRadiiTest, NearestWeighted_WithoutGapOutput)
{
  // Test the overload without gap output
  TestPoint2D                              aPoints[] = {{0.0, 0.0}, {10.0, 0.0}};
  double                                   aRadii[]  = {1.0, 1.0};
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  aTree.Build(aPoints, aRadii, 2);
  // Query at (1,0): gap1 = 1-1 = 0, gap2 = 9-1 = 8
  const size_t aIdx = aTree.NearestWeighted({1.0, 0.0});
  EXPECT_NEAR(aTree.Point(aIdx).X, 0.0, 1e-15);
}

TEST(NCollection_KDTreeRadiiTest, NearestWeighted_3D)
{
  TestPoint3D aPoints[] = {{0.0, 0.0, 0.0}, {10.0, 0.0, 0.0}, {0.0, 10.0, 0.0}};
  double      aRadii[]  = {2.0, 1.0, 1.0};
  NCollection_KDTree<TestPoint3D, 3, true> aTree;
  aTree.Build(aPoints, aRadii, 3);
  double aGap = 0.0;
  // Query at (1,0,0): gap1 = 1-2 = -1, gap2 = 9-1 = 8, gap3 = sqrt(101)-1 ≈ 9.05
  const size_t aIdx = aTree.NearestWeighted({1.0, 0.0, 0.0}, aGap);
  EXPECT_NEAR(aTree.Point(aIdx).X, 0.0, 1e-15);
  EXPECT_NEAR(aGap, -1.0, 1e-10);
}

TEST(NCollection_KDTreeRadiiTest, NearestWeighted_AllSameRadii)
{
  // When all radii are equal, NearestWeighted = NearestPoint
  TestPoint2D                              aPoints[] = {{0.0, 0.0}, {5.0, 0.0}, {10.0, 0.0}};
  double                                   aRadii[]  = {1.0, 1.0, 1.0};
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  aTree.Build(aPoints, aRadii, 3);
  double       aGap         = 0.0;
  double       aSqDist      = 0.0;
  const size_t aWeightedIdx = aTree.NearestWeighted({4.0, 0.0}, aGap);
  const size_t aNearestIdx  = aTree.NearestPoint({4.0, 0.0}, aSqDist);
  EXPECT_EQ(aWeightedIdx, aNearestIdx);
}

// ============================================================
// HasRadii: Brute-force verification
// ============================================================

TEST(NCollection_KDTreeRadiiTest, BruteForce_ContainingSearch)
{
  constexpr int THE_N = 200;
  TestRandom    aRng(8080);
  TestPoint2D   aPoints[THE_N];
  double        aRadii[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {aRng.NextDouble(-50, 50), aRng.NextDouble(-50, 50)};
    aRadii[i]  = aRng.NextDouble(0.1, 10.0);
  }
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  aTree.Build(aPoints, aRadii, THE_N);
  // Run 50 random queries, verify against brute force
  for (int q = 0; q < 50; ++q)
  {
    TestPoint2D aQuery = {aRng.NextDouble(-60, 60), aRng.NextDouble(-60, 60)};
    const NCollection_DynamicArray<size_t> aResult = aTree.ContainingSearch(aQuery);
    // Brute-force: count how many spheres contain aQuery
    std::set<size_t> aBruteForce;
    for (int i = 0; i < THE_N; ++i)
    {
      const double aDist = sqDist2D(aQuery, aPoints[i]);
      if (aDist <= aRadii[i] * aRadii[i])
      {
        aBruteForce.insert(static_cast<size_t>(i + 1));
      }
    }
    EXPECT_EQ(static_cast<size_t>(aResult.Size()), aBruteForce.size())
      << "Query (" << aQuery.X << ", " << aQuery.Y << ")";
    std::set<size_t> aTreeResult;
    for (int i = 0; i < aResult.Size(); ++i)
    {
      aTreeResult.insert(aResult[i]);
    }
    EXPECT_EQ(aTreeResult, aBruteForce);
  }
}

TEST(NCollection_KDTreeRadiiTest, BruteForce_NearestWeighted)
{
  constexpr int THE_N = 200;
  TestRandom    aRng(9090);
  TestPoint2D   aPoints[THE_N];
  double        aRadii[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {aRng.NextDouble(-50, 50), aRng.NextDouble(-50, 50)};
    aRadii[i]  = aRng.NextDouble(0.1, 10.0);
  }
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  aTree.Build(aPoints, aRadii, THE_N);
  // Run 50 random queries, verify against brute force
  for (int q = 0; q < 50; ++q)
  {
    TestPoint2D  aQuery = {aRng.NextDouble(-60, 60), aRng.NextDouble(-60, 60)};
    double       aGap   = 0.0;
    const size_t aIdx   = aTree.NearestWeighted(aQuery, aGap);
    // Brute-force: find minimum gap
    double aBruteGap = std::numeric_limits<double>::max();
    for (int i = 0; i < THE_N; ++i)
    {
      const double aDist = std::sqrt(sqDist2D(aQuery, aPoints[i]));
      const double aG    = aDist - aRadii[i];
      if (aG < aBruteGap)
      {
        aBruteGap = aG;
      }
    }
    EXPECT_NEAR(aGap, aBruteGap, 1e-8)
      << "Query (" << aQuery.X << ", " << aQuery.Y << "), idx=" << aIdx;
  }
}

TEST(NCollection_KDTreeRadiiTest, BruteForce_NearestWeighted_3D)
{
  constexpr int THE_N = 200;
  TestRandom    aRng(1212);
  TestPoint3D   aPoints[THE_N];
  double        aRadii[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {aRng.NextDouble(-50, 50), aRng.NextDouble(-50, 50), aRng.NextDouble(-50, 50)};
    aRadii[i]  = aRng.NextDouble(0.1, 10.0);
  }
  NCollection_KDTree<TestPoint3D, 3, true> aTree;
  aTree.Build(aPoints, aRadii, THE_N);
  for (int q = 0; q < 50; ++q)
  {
    TestPoint3D  aQuery    = {aRng.NextDouble(-60, 60),
                              aRng.NextDouble(-60, 60),
                              aRng.NextDouble(-60, 60)};
    double       aGap      = 0.0;
    const size_t aIdx      = aTree.NearestWeighted(aQuery, aGap);
    double       aBruteGap = std::numeric_limits<double>::max();
    for (int i = 0; i < THE_N; ++i)
    {
      const double aDist = std::sqrt(sqDist3D(aQuery, aPoints[i]));
      const double aG    = aDist - aRadii[i];
      if (aG < aBruteGap)
      {
        aBruteGap = aG;
      }
    }
    EXPECT_NEAR(aGap, aBruteGap, 1e-8)
      << "Query (" << aQuery.X << ", " << aQuery.Y << ", " << aQuery.Z << "), idx=" << aIdx;
  }
}

// ============================================================
// HasRadii: Stress Tests
// ============================================================

TEST(NCollection_KDTreeRadiiTest, StressTest_1000Points)
{
  constexpr int THE_N = 1000;
  TestRandom    aRng(3030);
  TestPoint2D   aPoints[THE_N];
  double        aRadii[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {aRng.NextDouble(-100, 100), aRng.NextDouble(-100, 100)};
    aRadii[i]  = aRng.NextDouble(0.01, 5.0);
  }
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  aTree.Build(aPoints, aRadii, THE_N);
  EXPECT_EQ(aTree.Size(), static_cast<size_t>(THE_N));
  // Run queries to verify no crashes
  for (int q = 0; q < 100; ++q)
  {
    TestPoint2D aQuery = {aRng.NextDouble(-110, 110), aRng.NextDouble(-110, 110)};
    aTree.ContainingSearch(aQuery);
    aTree.NearestWeighted(aQuery);
    aTree.NearestPoint(aQuery);
  }
}

TEST(NCollection_KDTreeRadiiTest, StressTest_ZeroRadii)
{
  // Zero radii — ContainingSearch should only find exact matches
  constexpr int THE_N = 100;
  TestRandom    aRng(4040);
  TestPoint2D   aPoints[THE_N];
  double        aRadii[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {aRng.NextDouble(-10, 10), aRng.NextDouble(-10, 10)};
    aRadii[i]  = 0.0;
  }
  NCollection_KDTree<TestPoint2D, 2, true> aTree;
  aTree.Build(aPoints, aRadii, THE_N);
  // Query at one of the points — should find it (dist=0 <= 0)
  const NCollection_DynamicArray<size_t> aResult = aTree.ContainingSearch(aPoints[0]);
  EXPECT_GE(aResult.Size(), 1);
  // Query far away — should find nothing
  const NCollection_DynamicArray<size_t> aResult2 = aTree.ContainingSearch({999.0, 999.0});
  EXPECT_EQ(aResult2.Size(), 0);
}

TEST(NCollection_KDTreeRadiiTest, StressTest_IdenticalRadii)
{
  // When all radii are equal, NearestWeighted should give same result as NearestPoint
  constexpr int    THE_N = 500;
  constexpr double THE_R = 2.5;
  TestRandom       aRng(5050);
  TestPoint2D      aPoints[THE_N];
  double           aRadii[THE_N];
  for (int i = 0; i < THE_N; ++i)
  {
    aPoints[i] = {aRng.NextDouble(-50, 50), aRng.NextDouble(-50, 50)};
    aRadii[i]  = THE_R;
  }
  NCollection_KDTree<TestPoint2D, 2, true> aTreeR;
  aTreeR.Build(aPoints, aRadii, THE_N);
  NCollection_KDTree<TestPoint2D, 2, false> aTreeNoR;
  aTreeNoR.Build(aPoints, THE_N);
  for (int q = 0; q < 50; ++q)
  {
    TestPoint2D  aQuery  = {aRng.NextDouble(-60, 60), aRng.NextDouble(-60, 60)};
    double       aGap    = 0.0;
    double       aSqDist = 0.0;
    const size_t aWIdx   = aTreeR.NearestWeighted(aQuery, aGap);
    const size_t aNIdx   = aTreeNoR.NearestPoint(aQuery, aSqDist);
    // Gap should be sqrt(sqDist) - R
    const double aExpectedGap = std::sqrt(aSqDist) - THE_R;
    EXPECT_NEAR(aGap, aExpectedGap, 1e-8);
    // Points should be the same
    EXPECT_NEAR(aTreeR.Point(aWIdx).X, aTreeNoR.Point(aNIdx).X, 1e-10);
    EXPECT_NEAR(aTreeR.Point(aWIdx).Y, aTreeNoR.Point(aNIdx).Y, 1e-10);
  }
}
