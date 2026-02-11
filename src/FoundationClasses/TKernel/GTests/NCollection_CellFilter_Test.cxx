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

#include <NCollection_CellFilter.hxx>

#include <gtest/gtest.h>
#include <vector>

namespace
{

//! Simple 2D point for testing
struct TestPoint2D
{
  double X;
  double Y;

  TestPoint2D(double theX = 0.0, double theY = 0.0)
      : X(theX),
        Y(theY)
  {
  }
};

//! Simple 2D inspector for CellFilter tests
class TestInspector2D
{
public:
  typedef int         Target;
  typedef TestPoint2D Point;

  enum
  {
    Dimension = 2
  };

  static double Coord(int i, const Point& thePnt) { return i == 0 ? thePnt.X : thePnt.Y; }

  static bool IsEqual(const Target& theT1, const Target& theT2) { return theT1 == theT2; }

  TestInspector2D()
      : myPurgeValue(-1)
  {
  }

  void SetPurgeValue(int theVal) { myPurgeValue = theVal; }

  NCollection_CellFilter_Action Inspect(const Target& theTarget)
  {
    if (theTarget == myPurgeValue)
      return CellFilter_Purge;
    myFound.push_back(theTarget);
    return CellFilter_Keep;
  }

  const std::vector<int>& Found() const { return myFound; }

  void Reset() { myFound.clear(); }

private:
  std::vector<int> myFound;
  int              myPurgeValue;
};

//! Simple 3D point for testing
struct TestPoint3D
{
  double X;
  double Y;
  double Z;

  TestPoint3D(double theX = 0.0, double theY = 0.0, double theZ = 0.0)
      : X(theX),
        Y(theY),
        Z(theZ)
  {
  }
};

//! Simple 3D inspector for CellFilter tests
class TestInspector3D
{
public:
  typedef int         Target;
  typedef TestPoint3D Point;

  enum
  {
    Dimension = 3
  };

  static double Coord(int i, const Point& thePnt)
  {
    return i == 0 ? thePnt.X : (i == 1 ? thePnt.Y : thePnt.Z);
  }

  static bool IsEqual(const Target& theT1, const Target& theT2) { return theT1 == theT2; }

  NCollection_CellFilter_Action Inspect(const Target& theTarget)
  {
    myFound.push_back(theTarget);
    return CellFilter_Keep;
  }

  const std::vector<int>& Found() const { return myFound; }

  void Reset() { myFound.clear(); }

private:
  std::vector<int> myFound;
};

} // namespace

TEST(NCollection_CellFilterTest, AddInspect2D_SinglePoint)
{
  NCollection_CellFilter<TestInspector2D> aFilter(1.0);

  // Add targets at different points
  aFilter.Add(10, TestPoint2D(0.5, 0.5));
  aFilter.Add(20, TestPoint2D(1.5, 1.5));
  aFilter.Add(30, TestPoint2D(0.3, 0.3));

  // Inspect cell containing (0.5, 0.5) -- should find 10 and 30 (same cell)
  TestInspector2D anInspector;
  aFilter.Inspect(TestPoint2D(0.5, 0.5), anInspector);

  // Both 10 and 30 are in cell (0,0)
  EXPECT_EQ(2u, anInspector.Found().size());
}

TEST(NCollection_CellFilterTest, AddInspect2D_Range)
{
  NCollection_CellFilter<TestInspector2D> aFilter(1.0);

  aFilter.Add(1, TestPoint2D(0.5, 0.5));
  aFilter.Add(2, TestPoint2D(1.5, 0.5));
  aFilter.Add(3, TestPoint2D(2.5, 0.5));

  // Inspect range covering cells (0,0) and (1,0)
  TestInspector2D anInspector;
  aFilter.Inspect(TestPoint2D(0.0, 0.0), TestPoint2D(1.5, 0.5), anInspector);

  // Should find targets 1 and 2
  EXPECT_EQ(2u, anInspector.Found().size());
}

TEST(NCollection_CellFilterTest, Remove2D)
{
  NCollection_CellFilter<TestInspector2D> aFilter(1.0);

  aFilter.Add(10, TestPoint2D(0.5, 0.5));
  aFilter.Add(20, TestPoint2D(0.5, 0.5));

  // Remove target 10
  aFilter.Remove(10, TestPoint2D(0.5, 0.5));

  // Inspect -- should only find 20
  TestInspector2D anInspector;
  aFilter.Inspect(TestPoint2D(0.5, 0.5), anInspector);

  ASSERT_EQ(1u, anInspector.Found().size());
  EXPECT_EQ(20, anInspector.Found()[0]);
}

TEST(NCollection_CellFilterTest, EmptyCellCleanupAfterRemove)
{
  NCollection_CellFilter<TestInspector2D> aFilter(1.0);

  aFilter.Add(10, TestPoint2D(0.5, 0.5));

  // Remove the only target -- cell should be cleaned up
  aFilter.Remove(10, TestPoint2D(0.5, 0.5));

  // Inspect -- should find nothing
  TestInspector2D anInspector;
  aFilter.Inspect(TestPoint2D(0.5, 0.5), anInspector);
  EXPECT_TRUE(anInspector.Found().empty());
}

TEST(NCollection_CellFilterTest, EmptyCellCleanupAfterPurge)
{
  NCollection_CellFilter<TestInspector2D> aFilter(1.0);

  aFilter.Add(10, TestPoint2D(0.5, 0.5));

  // Inspect with purge action for target 10
  TestInspector2D anInspector;
  anInspector.SetPurgeValue(10);
  aFilter.Inspect(TestPoint2D(0.5, 0.5), anInspector);

  // Found should be empty (purged, not kept)
  EXPECT_TRUE(anInspector.Found().empty());

  // Second inspect -- cell should have been cleaned up, nothing found
  TestInspector2D anInspector2;
  aFilter.Inspect(TestPoint2D(0.5, 0.5), anInspector2);
  EXPECT_TRUE(anInspector2.Found().empty());
}

TEST(NCollection_CellFilterTest, AddInspect3D)
{
  NCollection_CellFilter<TestInspector3D> aFilter(1.0);

  aFilter.Add(1, TestPoint3D(0.5, 0.5, 0.5));
  aFilter.Add(2, TestPoint3D(1.5, 1.5, 1.5));
  aFilter.Add(3, TestPoint3D(0.3, 0.3, 0.3));

  // Inspect cell containing (0.5, 0.5, 0.5)
  TestInspector3D anInspector;
  aFilter.Inspect(TestPoint3D(0.5, 0.5, 0.5), anInspector);

  // Both 1 and 3 are in cell (0,0,0)
  EXPECT_EQ(2u, anInspector.Found().size());
}

TEST(NCollection_CellFilterTest, Reset)
{
  NCollection_CellFilter<TestInspector2D> aFilter(1.0);

  aFilter.Add(10, TestPoint2D(0.5, 0.5));

  // Reset with new cell size
  aFilter.Reset(2.0);

  // After reset, all data should be cleared
  TestInspector2D anInspector;
  aFilter.Inspect(TestPoint2D(0.5, 0.5), anInspector);
  EXPECT_TRUE(anInspector.Found().empty());
}

TEST(NCollection_CellFilterTest, InspectWithPurge)
{
  NCollection_CellFilter<TestInspector2D> aFilter(1.0);

  aFilter.Add(10, TestPoint2D(0.5, 0.5));
  aFilter.Add(20, TestPoint2D(0.5, 0.5));
  aFilter.Add(30, TestPoint2D(0.5, 0.5));

  // Inspect with purge for target 20
  TestInspector2D anInspector;
  anInspector.SetPurgeValue(20);
  aFilter.Inspect(TestPoint2D(0.5, 0.5), anInspector);

  // Should find 10 and 30 (20 was purged)
  EXPECT_EQ(2u, anInspector.Found().size());

  // Re-inspect -- 20 should be gone
  TestInspector2D anInspector2;
  aFilter.Inspect(TestPoint2D(0.5, 0.5), anInspector2);
  EXPECT_EQ(2u, anInspector2.Found().size());
}
