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

#include <NCollection_KDTree.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Vector.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>

#include <gtest/gtest.h>
#include <algorithm>
#include <cmath>
#include <random>

//! Custom accessor for gp_Pnt (1-based coordinate access).
struct GpPntAccessor
{
  static double Coord(const gp_Pnt& thePnt, int theAxis)
  {
    return thePnt.Coord(theAxis + 1); // gp_Pnt uses 1-based indexing
  }
};

//! Simple 3D point structure with direct coordinate access for testing.
struct Point3D
{
  double X, Y, Z;

  Point3D(double theX = 0.0, double theY = 0.0, double theZ = 0.0)
      : X(theX),
        Y(theY),
        Z(theZ)
  {
  }

  double operator[](int theAxis) const
  {
    switch (theAxis)
    {
      case 0:
        return X;
      case 1:
        return Y;
      case 2:
        return Z;
      default:
        return 0.0;
    }
  }

  double Distance(const Point3D& theOther) const
  {
    return std::sqrt(SquareDistance(theOther));
  }

  double SquareDistance(const Point3D& theOther) const
  {
    const double aDx = X - theOther.X;
    const double aDy = Y - theOther.Y;
    const double aDz = Z - theOther.Z;
    return aDx * aDx + aDy * aDy + aDz * aDz;
  }
};

//==================================================================================================
// Basic Functionality Tests
//==================================================================================================

TEST(NCollection_KDTreeTest, DefaultConstructor)
{
  NCollection_KDTree<Point3D> aTree;

  EXPECT_FALSE(aTree.IsBuilt());
  EXPECT_TRUE(aTree.IsEmpty());
  EXPECT_EQ(0, aTree.Size());
}

TEST(NCollection_KDTreeTest, BuildEmpty)
{
  NCollection_Array1<Point3D> anEmptyArray;
  NCollection_KDTree<Point3D> aTree;

  aTree.Build(anEmptyArray);

  EXPECT_FALSE(aTree.IsBuilt()); // Empty array doesn't build a valid tree
  EXPECT_TRUE(aTree.IsEmpty());
}

TEST(NCollection_KDTreeTest, BuildSinglePoint)
{
  NCollection_Array1<Point3D> aPoints(1, 1);
  aPoints.SetValue(1, Point3D(1.0, 2.0, 3.0));

  NCollection_KDTree<Point3D> aTree;
  aTree.Build(aPoints);

  EXPECT_TRUE(aTree.IsBuilt());
  EXPECT_EQ(1, aTree.Size());
  EXPECT_EQ(1, aTree.NbNodes());
}

TEST(NCollection_KDTreeTest, BuildMultiplePoints)
{
  NCollection_Array1<Point3D> aPoints(1, 5);
  aPoints.SetValue(1, Point3D(0.0, 0.0, 0.0));
  aPoints.SetValue(2, Point3D(1.0, 0.0, 0.0));
  aPoints.SetValue(3, Point3D(0.0, 1.0, 0.0));
  aPoints.SetValue(4, Point3D(0.0, 0.0, 1.0));
  aPoints.SetValue(5, Point3D(1.0, 1.0, 1.0));

  NCollection_KDTree<Point3D> aTree;
  aTree.Build(aPoints);

  EXPECT_TRUE(aTree.IsBuilt());
  EXPECT_EQ(5, aTree.Size());
  EXPECT_EQ(5, aTree.NbNodes()); // All points become nodes
}

TEST(NCollection_KDTreeTest, ConstructorWithBuild)
{
  NCollection_Array1<Point3D> aPoints(1, 3);
  aPoints.SetValue(1, Point3D(0.0, 0.0, 0.0));
  aPoints.SetValue(2, Point3D(1.0, 0.0, 0.0));
  aPoints.SetValue(3, Point3D(0.0, 1.0, 0.0));

  NCollection_KDTree<Point3D> aTree(aPoints);

  EXPECT_TRUE(aTree.IsBuilt());
  EXPECT_EQ(3, aTree.Size());
}

TEST(NCollection_KDTreeTest, Clear)
{
  NCollection_Array1<Point3D> aPoints(1, 3);
  aPoints.SetValue(1, Point3D(0.0, 0.0, 0.0));
  aPoints.SetValue(2, Point3D(1.0, 0.0, 0.0));
  aPoints.SetValue(3, Point3D(0.0, 1.0, 0.0));

  NCollection_KDTree<Point3D> aTree(aPoints);
  EXPECT_TRUE(aTree.IsBuilt());

  aTree.Clear();

  EXPECT_FALSE(aTree.IsBuilt());
  EXPECT_TRUE(aTree.IsEmpty());
  EXPECT_EQ(0, aTree.Size());
}

//==================================================================================================
// Nearest Neighbor Tests
//==================================================================================================

TEST(NCollection_KDTreeTest, FindNearestSinglePoint)
{
  NCollection_Array1<Point3D> aPoints(1, 1);
  aPoints.SetValue(1, Point3D(1.0, 2.0, 3.0));

  NCollection_KDTree<Point3D> aTree(aPoints);

  int    anIndex;
  double aSqDist;
  bool   isFound = aTree.FindNearest(Point3D(0.0, 0.0, 0.0), anIndex, aSqDist);

  EXPECT_TRUE(isFound);
  EXPECT_EQ(1, anIndex); // Index in original array (1-based)
  EXPECT_NEAR(14.0, aSqDist, 1e-10); // 1^2 + 2^2 + 3^2 = 14
}

TEST(NCollection_KDTreeTest, FindNearestEmptyTree)
{
  NCollection_KDTree<Point3D> aTree;

  int    anIndex;
  double aSqDist;
  bool   isFound = aTree.FindNearest(Point3D(0.0, 0.0, 0.0), anIndex, aSqDist);

  EXPECT_FALSE(isFound);
  EXPECT_EQ(-1, anIndex);
}

TEST(NCollection_KDTreeTest, FindNearestMultiplePoints)
{
  NCollection_Array1<Point3D> aPoints(1, 5);
  aPoints.SetValue(1, Point3D(0.0, 0.0, 0.0));
  aPoints.SetValue(2, Point3D(10.0, 0.0, 0.0));
  aPoints.SetValue(3, Point3D(0.0, 10.0, 0.0));
  aPoints.SetValue(4, Point3D(0.0, 0.0, 10.0));
  aPoints.SetValue(5, Point3D(5.0, 5.0, 5.0));

  NCollection_KDTree<Point3D> aTree(aPoints);

  // Query near origin
  {
    int    anIndex;
    double aSqDist;
    bool   isFound = aTree.FindNearest(Point3D(0.1, 0.1, 0.1), anIndex, aSqDist);

    EXPECT_TRUE(isFound);
    EXPECT_EQ(1, anIndex); // Point at origin is closest
    EXPECT_NEAR(0.03, aSqDist, 1e-10);
  }

  // Query near point 5 (5,5,5)
  {
    int    anIndex;
    double aSqDist;
    bool   isFound = aTree.FindNearest(Point3D(5.0, 5.0, 5.1), anIndex, aSqDist);

    EXPECT_TRUE(isFound);
    EXPECT_EQ(5, anIndex);
    EXPECT_NEAR(0.01, aSqDist, 1e-10);
  }
}

TEST(NCollection_KDTreeTest, FindNearestExactMatch)
{
  NCollection_Array1<Point3D> aPoints(1, 3);
  aPoints.SetValue(1, Point3D(1.0, 2.0, 3.0));
  aPoints.SetValue(2, Point3D(4.0, 5.0, 6.0));
  aPoints.SetValue(3, Point3D(7.0, 8.0, 9.0));

  NCollection_KDTree<Point3D> aTree(aPoints);

  int    anIndex;
  double aSqDist;
  bool   isFound = aTree.FindNearest(Point3D(4.0, 5.0, 6.0), anIndex, aSqDist);

  EXPECT_TRUE(isFound);
  EXPECT_EQ(2, anIndex);
  EXPECT_NEAR(0.0, aSqDist, 1e-10);
}

//==================================================================================================
// K-Nearest Neighbors Tests
//==================================================================================================

TEST(NCollection_KDTreeTest, FindKNearest_K1)
{
  NCollection_Array1<Point3D> aPoints(1, 5);
  aPoints.SetValue(1, Point3D(0.0, 0.0, 0.0));
  aPoints.SetValue(2, Point3D(1.0, 0.0, 0.0));
  aPoints.SetValue(3, Point3D(2.0, 0.0, 0.0));
  aPoints.SetValue(4, Point3D(3.0, 0.0, 0.0));
  aPoints.SetValue(5, Point3D(4.0, 0.0, 0.0));

  NCollection_KDTree<Point3D> aTree(aPoints);

  NCollection_Vector<int>    anIndices;
  NCollection_Vector<double> aDistances;
  aTree.FindKNearest(Point3D(1.1, 0.0, 0.0), 1, anIndices, aDistances);

  EXPECT_EQ(1, anIndices.Length());
  EXPECT_EQ(2, anIndices.First()); // Point at (1,0,0) is closest
  EXPECT_NEAR(0.01, aDistances.First(), 1e-10);
}

TEST(NCollection_KDTreeTest, FindKNearest_K3)
{
  NCollection_Array1<Point3D> aPoints(1, 5);
  aPoints.SetValue(1, Point3D(0.0, 0.0, 0.0));
  aPoints.SetValue(2, Point3D(1.0, 0.0, 0.0));
  aPoints.SetValue(3, Point3D(2.0, 0.0, 0.0));
  aPoints.SetValue(4, Point3D(3.0, 0.0, 0.0));
  aPoints.SetValue(5, Point3D(4.0, 0.0, 0.0));

  NCollection_KDTree<Point3D> aTree(aPoints);

  NCollection_Vector<int>    anIndices;
  NCollection_Vector<double> aDistances;
  aTree.FindKNearest(Point3D(1.5, 0.0, 0.0), 3, anIndices, aDistances);

  EXPECT_EQ(3, anIndices.Length());

  // Results should be sorted by distance
  EXPECT_TRUE(aDistances.Value(0) <= aDistances.Value(1));
  EXPECT_TRUE(aDistances.Value(1) <= aDistances.Value(2));

  // Closest should be either index 2 or 3 (distance 0.5)
  EXPECT_NEAR(0.25, aDistances.First(), 1e-10); // 0.5^2
}

TEST(NCollection_KDTreeTest, FindKNearest_KGreaterThanSize)
{
  NCollection_Array1<Point3D> aPoints(1, 3);
  aPoints.SetValue(1, Point3D(0.0, 0.0, 0.0));
  aPoints.SetValue(2, Point3D(1.0, 0.0, 0.0));
  aPoints.SetValue(3, Point3D(2.0, 0.0, 0.0));

  NCollection_KDTree<Point3D> aTree(aPoints);

  NCollection_Vector<int>    anIndices;
  NCollection_Vector<double> aDistances;
  aTree.FindKNearest(Point3D(0.0, 0.0, 0.0), 10, anIndices, aDistances);

  EXPECT_EQ(3, anIndices.Length()); // Should return all 3 points
}

TEST(NCollection_KDTreeTest, FindKNearest_K0)
{
  NCollection_Array1<Point3D> aPoints(1, 3);
  aPoints.SetValue(1, Point3D(0.0, 0.0, 0.0));
  aPoints.SetValue(2, Point3D(1.0, 0.0, 0.0));
  aPoints.SetValue(3, Point3D(2.0, 0.0, 0.0));

  NCollection_KDTree<Point3D> aTree(aPoints);

  NCollection_Vector<int>    anIndices;
  NCollection_Vector<double> aDistances;
  aTree.FindKNearest(Point3D(0.0, 0.0, 0.0), 0, anIndices, aDistances);

  EXPECT_EQ(0, anIndices.Length());
}

//==================================================================================================
// Radius Search Tests
//==================================================================================================

TEST(NCollection_KDTreeTest, FindInRadius_NoResults)
{
  NCollection_Array1<Point3D> aPoints(1, 3);
  aPoints.SetValue(1, Point3D(10.0, 0.0, 0.0));
  aPoints.SetValue(2, Point3D(20.0, 0.0, 0.0));
  aPoints.SetValue(3, Point3D(30.0, 0.0, 0.0));

  NCollection_KDTree<Point3D> aTree(aPoints);

  NCollection_Vector<int> anIndices;
  aTree.FindInRadius(Point3D(0.0, 0.0, 0.0), 1.0, anIndices);

  EXPECT_EQ(0, anIndices.Length());
}

TEST(NCollection_KDTreeTest, FindInRadius_SingleResult)
{
  NCollection_Array1<Point3D> aPoints(1, 3);
  aPoints.SetValue(1, Point3D(0.5, 0.0, 0.0));
  aPoints.SetValue(2, Point3D(10.0, 0.0, 0.0));
  aPoints.SetValue(3, Point3D(20.0, 0.0, 0.0));

  NCollection_KDTree<Point3D> aTree(aPoints);

  NCollection_Vector<int> anIndices;
  aTree.FindInRadius(Point3D(0.0, 0.0, 0.0), 1.0, anIndices);

  EXPECT_EQ(1, anIndices.Length());
  EXPECT_EQ(1, anIndices.First());
}

TEST(NCollection_KDTreeTest, FindInRadius_MultipleResults)
{
  NCollection_Array1<Point3D> aPoints(1, 5);
  aPoints.SetValue(1, Point3D(0.0, 0.0, 0.0));
  aPoints.SetValue(2, Point3D(0.5, 0.0, 0.0));
  aPoints.SetValue(3, Point3D(0.0, 0.5, 0.0));
  aPoints.SetValue(4, Point3D(10.0, 0.0, 0.0));
  aPoints.SetValue(5, Point3D(20.0, 0.0, 0.0));

  NCollection_KDTree<Point3D> aTree(aPoints);

  NCollection_Vector<int> anIndices;
  aTree.FindInRadius(Point3D(0.0, 0.0, 0.0), 1.0, anIndices);

  EXPECT_EQ(3, anIndices.Length());
}

TEST(NCollection_KDTreeTest, FindInRadius_WithDistances)
{
  NCollection_Array1<Point3D> aPoints(1, 3);
  aPoints.SetValue(1, Point3D(0.3, 0.0, 0.0));
  aPoints.SetValue(2, Point3D(0.5, 0.0, 0.0));
  aPoints.SetValue(3, Point3D(10.0, 0.0, 0.0));

  NCollection_KDTree<Point3D> aTree(aPoints);

  NCollection_Vector<int>    anIndices;
  NCollection_Vector<double> aDistances;
  aTree.FindInRadius(Point3D(0.0, 0.0, 0.0), 1.0, anIndices, &aDistances);

  EXPECT_EQ(2, anIndices.Length());
  EXPECT_EQ(2, aDistances.Length());

  // Check distances are correct
  for (int i = 0; i < anIndices.Length(); ++i)
  {
    const int      anIdx   = anIndices.Value(i);
    const Point3D& aPt     = aPoints.Value(anIdx);
    const double   anExpSq = aPt.SquareDistance(Point3D(0.0, 0.0, 0.0));
    EXPECT_NEAR(anExpSq, aDistances.Value(i), 1e-10);
  }
}

//==================================================================================================
// Rebuild Tests
//==================================================================================================

TEST(NCollection_KDTreeTest, Rebuild)
{
  NCollection_Array1<Point3D> aPoints(1, 3);
  aPoints.SetValue(1, Point3D(0.0, 0.0, 0.0));
  aPoints.SetValue(2, Point3D(1.0, 0.0, 0.0));
  aPoints.SetValue(3, Point3D(2.0, 0.0, 0.0));

  NCollection_KDTree<Point3D> aTree(aPoints);

  // Verify initial state
  int    anIndex;
  double aSqDist;
  aTree.FindNearest(Point3D(1.0, 0.0, 0.0), anIndex, aSqDist);
  EXPECT_EQ(2, anIndex);

  // Modify point externally
  aPoints.ChangeValue(2) = Point3D(100.0, 0.0, 0.0);

  // Rebuild tree
  aTree.Rebuild();

  // Query again - now point 1 or 3 should be closest
  aTree.FindNearest(Point3D(1.0, 0.0, 0.0), anIndex, aSqDist);
  EXPECT_TRUE(anIndex == 1 || anIndex == 3);
}

//==================================================================================================
// Large Dataset Tests
//==================================================================================================

TEST(NCollection_KDTreeTest, LargeDataset_Build)
{
  const int                   aNbPoints = 10000;
  NCollection_Array1<Point3D> aPoints(1, aNbPoints);

  std::mt19937                          aGen(42);
  std::uniform_real_distribution<double> aDist(-100.0, 100.0);

  for (int i = 1; i <= aNbPoints; ++i)
  {
    aPoints.SetValue(i, Point3D(aDist(aGen), aDist(aGen), aDist(aGen)));
  }

  NCollection_KDTree<Point3D> aTree(aPoints);

  EXPECT_TRUE(aTree.IsBuilt());
  EXPECT_EQ(aNbPoints, aTree.Size());
}

TEST(NCollection_KDTreeTest, LargeDataset_FindNearest_Correctness)
{
  const int                   aNbPoints = 1000;
  NCollection_Array1<Point3D> aPoints(1, aNbPoints);

  std::mt19937                          aGen(42);
  std::uniform_real_distribution<double> aDist(-100.0, 100.0);

  for (int i = 1; i <= aNbPoints; ++i)
  {
    aPoints.SetValue(i, Point3D(aDist(aGen), aDist(aGen), aDist(aGen)));
  }

  NCollection_KDTree<Point3D> aTree(aPoints);

  // Test multiple query points
  for (int q = 0; q < 100; ++q)
  {
    Point3D aQuery(aDist(aGen), aDist(aGen), aDist(aGen));

    // Find nearest using KD-Tree
    int    aTreeIdx;
    double aTreeSqDist;
    aTree.FindNearest(aQuery, aTreeIdx, aTreeSqDist);

    // Find nearest using brute force
    double aBruteSqDist = aPoints.Value(1).SquareDistance(aQuery);
    for (int i = 2; i <= aNbPoints; ++i)
    {
      const double aSqDist = aPoints.Value(i).SquareDistance(aQuery);
      if (aSqDist < aBruteSqDist)
      {
        aBruteSqDist = aSqDist;
      }
    }

    // Results should match (distances should be equal, indices may differ for equidistant points)
    EXPECT_NEAR(aBruteSqDist, aTreeSqDist, 1e-10)
      << "Query " << q << ": brute=" << aBruteSqDist << ", tree=" << aTreeSqDist;
  }
}

TEST(NCollection_KDTreeTest, LargeDataset_FindKNearest_Correctness)
{
  const int                   aNbPoints = 500;
  const int                   aK        = 10;
  NCollection_Array1<Point3D> aPoints(1, aNbPoints);

  std::mt19937                          aGen(42);
  std::uniform_real_distribution<double> aDist(-50.0, 50.0);

  for (int i = 1; i <= aNbPoints; ++i)
  {
    aPoints.SetValue(i, Point3D(aDist(aGen), aDist(aGen), aDist(aGen)));
  }

  NCollection_KDTree<Point3D> aTree(aPoints);

  // Test a few query points
  for (int q = 0; q < 20; ++q)
  {
    Point3D aQuery(aDist(aGen), aDist(aGen), aDist(aGen));

    // Find K nearest using KD-Tree
    NCollection_Vector<int>    aTreeIndices;
    NCollection_Vector<double> aTreeDistances;
    aTree.FindKNearest(aQuery, aK, aTreeIndices, aTreeDistances);

    // Verify K results returned
    EXPECT_EQ(aK, aTreeIndices.Length());

    // Verify sorted order
    for (int i = 1; i < aTreeDistances.Length(); ++i)
    {
      EXPECT_LE(aTreeDistances.Value(i - 1), aTreeDistances.Value(i));
    }

    // Verify distances are correct
    for (int i = 0; i < aTreeIndices.Length(); ++i)
    {
      const int      anIdx   = aTreeIndices.Value(i);
      const double   anExpSq = aPoints.Value(anIdx).SquareDistance(aQuery);
      EXPECT_NEAR(anExpSq, aTreeDistances.Value(i), 1e-10);
    }
  }
}

//==================================================================================================
// Edge Cases
//==================================================================================================

TEST(NCollection_KDTreeTest, ColinearPoints)
{
  // All points on a line (degenerate case)
  NCollection_Array1<Point3D> aPoints(1, 5);
  for (int i = 1; i <= 5; ++i)
  {
    aPoints.SetValue(i, Point3D(static_cast<double>(i), 0.0, 0.0));
  }

  NCollection_KDTree<Point3D> aTree(aPoints);

  int    anIndex;
  double aSqDist;
  aTree.FindNearest(Point3D(2.5, 0.0, 0.0), anIndex, aSqDist);

  EXPECT_TRUE(anIndex == 2 || anIndex == 3);
  EXPECT_NEAR(0.25, aSqDist, 1e-10);
}

TEST(NCollection_KDTreeTest, CoplanarPoints)
{
  // All points in XY plane
  NCollection_Array1<Point3D> aPoints(1, 9);
  int                         anIdx = 1;
  for (int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 3; ++j)
    {
      aPoints.SetValue(anIdx++, Point3D(static_cast<double>(i), static_cast<double>(j), 0.0));
    }
  }

  NCollection_KDTree<Point3D> aTree(aPoints);

  int    anIndex;
  double aSqDist;
  aTree.FindNearest(Point3D(1.0, 1.0, 0.1), anIndex, aSqDist);

  EXPECT_EQ(5, anIndex); // Center point (1,1,0)
  EXPECT_NEAR(0.01, aSqDist, 1e-10);
}

TEST(NCollection_KDTreeTest, DuplicatePoints)
{
  NCollection_Array1<Point3D> aPoints(1, 5);
  for (int i = 1; i <= 5; ++i)
  {
    aPoints.SetValue(i, Point3D(1.0, 2.0, 3.0)); // All same point
  }

  NCollection_KDTree<Point3D> aTree(aPoints);

  int    anIndex;
  double aSqDist;
  aTree.FindNearest(Point3D(1.0, 2.0, 3.0), anIndex, aSqDist);

  EXPECT_TRUE(anIndex >= 1 && anIndex <= 5);
  EXPECT_NEAR(0.0, aSqDist, 1e-10);
}

//==================================================================================================
// Integration with gp_Pnt
//==================================================================================================

//! Specialized accessor for gp_Pnt used by the KD-Tree.
template <>
struct NCollection_KDTree<gp_Pnt, 3>::DefaultAccessor
{
  static double Coord(const gp_Pnt& thePnt, int theAxis)
  {
    return thePnt.Coord(theAxis + 1); // gp_Pnt uses 1-based indexing
  }
};

TEST(NCollection_KDTreeTest, GpPntIntegration)
{
  NCollection_Array1<gp_Pnt> aPoints(1, 4);
  aPoints.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoints.SetValue(2, gp_Pnt(1.0, 0.0, 0.0));
  aPoints.SetValue(3, gp_Pnt(0.0, 1.0, 0.0));
  aPoints.SetValue(4, gp_Pnt(0.0, 0.0, 1.0));

  NCollection_KDTree<gp_Pnt, 3> aTree(aPoints);

  EXPECT_TRUE(aTree.IsBuilt());
  EXPECT_EQ(4, aTree.Size());

  int    anIndex;
  double aSqDist;
  aTree.FindNearest(gp_Pnt(0.1, 0.0, 0.0), anIndex, aSqDist);

  EXPECT_EQ(1, anIndex); // Origin is closest
  EXPECT_NEAR(0.01, aSqDist, 1e-10);
}

TEST(NCollection_KDTreeTest, GpPntKNearest)
{
  NCollection_Array1<gp_Pnt> aPoints(1, 5);
  aPoints.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoints.SetValue(2, gp_Pnt(1.0, 0.0, 0.0));
  aPoints.SetValue(3, gp_Pnt(2.0, 0.0, 0.0));
  aPoints.SetValue(4, gp_Pnt(3.0, 0.0, 0.0));
  aPoints.SetValue(5, gp_Pnt(4.0, 0.0, 0.0));

  NCollection_KDTree<gp_Pnt, 3> aTree(aPoints);

  NCollection_Vector<int>    anIndices;
  NCollection_Vector<double> aDistances;
  aTree.FindKNearest(gp_Pnt(1.0, 0.0, 0.0), 2, anIndices, aDistances);

  EXPECT_EQ(2, anIndices.Length());
  EXPECT_EQ(2, anIndices.Value(0)); // Exact match at index 2
  EXPECT_NEAR(0.0, aDistances.Value(0), 1e-10);
}
