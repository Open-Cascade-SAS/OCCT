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

#include <BVH_BinnedBuilder.hxx>
#include <BVH_Traverse.hxx>
#include <BVH_Triangulation.hxx>

#include <vector>

// =======================================================================================
// Test implementations of BVH_Traverse
// =======================================================================================

//! Simple traverse implementation that counts all elements
class BVH_CountAllElements : public BVH_Traverse<Standard_Real, 3, void, Standard_Real>
{
public:
  BVH_CountAllElements()
      : myAcceptedCount(0)
  {
  }

  virtual Standard_Boolean RejectNode(const BVH_VecNt&,
                                      const BVH_VecNt&,
                                      Standard_Real& theMetric) const Standard_OVERRIDE
  {
    theMetric = 0.0;       // All nodes have same metric
    return Standard_False; // Never reject
  }

  virtual Standard_Boolean Accept(const Standard_Integer, const Standard_Real&) Standard_OVERRIDE
  {
    ++myAcceptedCount;
    return Standard_True;
  }

  Standard_Integer AcceptedCount() const { return myAcceptedCount; }

  void Reset() { myAcceptedCount = 0; }

private:
  mutable Standard_Integer myAcceptedCount;
};

//! Traverse that rejects nodes outside a bounding box
class BVH_BoxSelector : public BVH_Traverse<Standard_Real, 3, void, Standard_Real>
{
public:
  BVH_BoxSelector(const BVH_Box<Standard_Real, 3>& theBox)
      : myBox(theBox),
        myAcceptedCount(0)
  {
  }

  virtual Standard_Boolean RejectNode(const BVH_VecNt& theMin,
                                      const BVH_VecNt& theMax,
                                      Standard_Real&   theMetric) const Standard_OVERRIDE
  {
    // Reject if box doesn't intersect with selection box
    theMetric = 0.0;
    return myBox.IsOut(theMin, theMax);
  }

  virtual Standard_Boolean Accept(const Standard_Integer, const Standard_Real&) Standard_OVERRIDE
  {
    ++myAcceptedCount;
    return Standard_True;
  }

  Standard_Integer AcceptedCount() const { return myAcceptedCount; }

  void Reset() { myAcceptedCount = 0; }

private:
  BVH_Box<Standard_Real, 3> myBox;
  mutable Standard_Integer  myAcceptedCount;
};

//! Traverse with distance-based metric and early termination
class BVH_DistanceSelector : public BVH_Traverse<Standard_Real, 3, void, Standard_Real>
{
public:
  BVH_DistanceSelector(const BVH_Vec3d& thePoint, Standard_Real theMaxDist)
      : myPoint(thePoint),
        myMaxDistSq(theMaxDist * theMaxDist),
        myMinDistSq(std::numeric_limits<Standard_Real>::max()),
        myAcceptedCount(0),
        myClosestIndex(-1)
  {
  }

  virtual Standard_Boolean RejectNode(const BVH_VecNt& theMin,
                                      const BVH_VecNt& theMax,
                                      Standard_Real&   theMetric) const Standard_OVERRIDE
  {
    // Compute squared distance from point to box
    theMetric = PointBoxSquareDistance(myPoint, theMin, theMax);
    return theMetric > myMaxDistSq;
  }

  virtual Standard_Boolean Accept(const Standard_Integer theIndex,
                                  const Standard_Real&   theMetric) Standard_OVERRIDE
  {
    ++myAcceptedCount;
    if (theMetric < myMinDistSq)
    {
      myMinDistSq    = theMetric;
      myClosestIndex = theIndex;
    }
    return Standard_True;
  }

  virtual Standard_Boolean IsMetricBetter(const Standard_Real& theLeft,
                                          const Standard_Real& theRight) const Standard_OVERRIDE
  {
    return theLeft < theRight; // Closer is better
  }

  virtual Standard_Boolean RejectMetric(const Standard_Real& theMetric) const Standard_OVERRIDE
  {
    return theMetric > myMaxDistSq;
  }

  Standard_Integer AcceptedCount() const { return myAcceptedCount; }

  Standard_Integer ClosestIndex() const { return myClosestIndex; }

  Standard_Real MinDistance() const { return std::sqrt(myMinDistSq); }

private:
  static Standard_Real PointBoxSquareDistance(const BVH_Vec3d& thePoint,
                                              const BVH_Vec3d& theMin,
                                              const BVH_Vec3d& theMax)
  {
    Standard_Real aDist = 0.0;
    for (int i = 0; i < 3; ++i)
    {
      if (thePoint[i] < theMin[i])
      {
        Standard_Real d = theMin[i] - thePoint[i];
        aDist += d * d;
      }
      else if (thePoint[i] > theMax[i])
      {
        Standard_Real d = thePoint[i] - theMax[i];
        aDist += d * d;
      }
    }
    return aDist;
  }

  BVH_Vec3d        myPoint;
  Standard_Real    myMaxDistSq;
  Standard_Real    myMinDistSq;
  Standard_Integer myAcceptedCount;
  Standard_Integer myClosestIndex;
};

//! Traverse with early stopping after finding N elements
class BVH_LimitedSelector : public BVH_Traverse<Standard_Real, 3, void, Standard_Real>
{
public:
  BVH_LimitedSelector(Standard_Integer theMaxCount)
      : myMaxCount(theMaxCount),
        myAcceptedCount(0)
  {
  }

  virtual Standard_Boolean RejectNode(const BVH_VecNt&,
                                      const BVH_VecNt&,
                                      Standard_Real& theMetric) const Standard_OVERRIDE
  {
    theMetric = 0.0;
    return Standard_False;
  }

  virtual Standard_Boolean Accept(const Standard_Integer, const Standard_Real&) Standard_OVERRIDE
  {
    ++myAcceptedCount;
    return Standard_True;
  }

  virtual Standard_Boolean Stop() const Standard_OVERRIDE { return myAcceptedCount >= myMaxCount; }

  Standard_Integer AcceptedCount() const { return myAcceptedCount; }

private:
  Standard_Integer myMaxCount;
  Standard_Integer myAcceptedCount;
};

// =======================================================================================
// Test implementations of BVH_PairTraverse
// =======================================================================================

//! Simple pair traverse that counts all pairs
class BVH_CountAllPairs : public BVH_PairTraverse<Standard_Real, 3, void, Standard_Real>
{
public:
  BVH_CountAllPairs()
      : myAcceptedCount(0)
  {
  }

  virtual Standard_Boolean RejectNode(const BVH_VecNt&,
                                      const BVH_VecNt&,
                                      const BVH_VecNt&,
                                      const BVH_VecNt&,
                                      Standard_Real& theMetric) const Standard_OVERRIDE
  {
    theMetric = 0.0;
    return Standard_False; // Never reject
  }

  virtual Standard_Boolean Accept(const Standard_Integer, const Standard_Integer) Standard_OVERRIDE
  {
    ++myAcceptedCount;
    return Standard_True;
  }

  Standard_Integer AcceptedCount() const { return myAcceptedCount; }

private:
  mutable Standard_Integer myAcceptedCount;
};

//! Pair traverse that only accepts overlapping boxes
class BVH_OverlapDetector : public BVH_PairTraverse<Standard_Real, 3, void, Standard_Real>
{
public:
  BVH_OverlapDetector()
      : myOverlapCount(0),
        myRejectCount(0)
  {
  }

  virtual Standard_Boolean RejectNode(const BVH_VecNt& theMin1,
                                      const BVH_VecNt& theMax1,
                                      const BVH_VecNt& theMin2,
                                      const BVH_VecNt& theMax2,
                                      Standard_Real&   theMetric) const Standard_OVERRIDE
  {
    ++myRejectCount;
    theMetric = 0.0;
    // Reject if boxes don't overlap
    BVH_Box<Standard_Real, 3> aBox1(theMin1, theMax1);
    Standard_Boolean          isOut = aBox1.IsOut(theMin2, theMax2);
    return isOut;
  }

  virtual Standard_Boolean Accept(const Standard_Integer, const Standard_Integer) Standard_OVERRIDE
  {
    // For this test, if we reach Accept, it means the bounding boxes overlap.
    // In a real implementation, you would check actual triangle-triangle intersection here.
    // For testing purposes, we count all pairs whose bounding boxes overlap.
    ++myOverlapCount;
    return Standard_True;
  }

  Standard_Integer OverlapCount() const { return myOverlapCount; }

  Standard_Integer RejectCount() const { return myRejectCount; }

private:
  mutable Standard_Integer myOverlapCount;
  mutable Standard_Integer myRejectCount;
};

// =======================================================================================
// Helper functions
// =======================================================================================

//! Creates a simple triangulation for testing
opencascade::handle<BVH_Tree<Standard_Real, 3>> CreateSimpleTriangulationBVH(
  Standard_Integer theNumTriangles)
{
  BVH_Triangulation<Standard_Real, 3> aTriangulation;

  for (Standard_Integer i = 0; i < theNumTriangles; ++i)
  {
    Standard_Real x = static_cast<Standard_Real>(i * 2);
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 1.0, 1.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTriangulation.Vertices, BVH_Vec3d(x + 2.0, 0.0, 0.0));
    BVH::Array<Standard_Integer, 4>::Append(aTriangulation.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  opencascade::handle<BVH_Tree<Standard_Real, 3>> aBVH = new BVH_Tree<Standard_Real, 3>;
  BVH_BinnedBuilder<Standard_Real, 3>             aBuilder;
  aBuilder.Build(&aTriangulation, aBVH.get(), aTriangulation.Box());

  return aBVH;
}

// =======================================================================================
// Tests for BVH_Traverse
// =======================================================================================

TEST(BVH_TraverseTest, CountAllElements)
{
  opencascade::handle<BVH_Tree<Standard_Real, 3>> aBVH = CreateSimpleTriangulationBVH(10);

  BVH_CountAllElements aSelector;
  Standard_Integer     aCount = aSelector.Select(aBVH);

  EXPECT_EQ(aCount, 10);
  EXPECT_EQ(aSelector.AcceptedCount(), 10);
}

TEST(BVH_TraverseTest, EmptyTree)
{
  opencascade::handle<BVH_Tree<Standard_Real, 3>> aBVH = new BVH_Tree<Standard_Real, 3>;

  BVH_CountAllElements aSelector;
  Standard_Integer     aCount = aSelector.Select(aBVH);

  EXPECT_EQ(aCount, 0);
}

TEST(BVH_TraverseTest, NullTree)
{
  opencascade::handle<BVH_Tree<Standard_Real, 3>> aBVH;

  BVH_CountAllElements aSelector;
  Standard_Integer     aCount = aSelector.Select(aBVH);

  EXPECT_EQ(aCount, 0);
}

TEST(BVH_TraverseTest, BoxSelection)
{
  opencascade::handle<BVH_Tree<Standard_Real, 3>> aBVH = CreateSimpleTriangulationBVH(10);

  // Select elements in the first half
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(0.0, 0.0, -1.0), BVH_Vec3d(10.0, 2.0, 1.0));
  BVH_BoxSelector           aSelector(aBox);
  Standard_Integer          aCount = aSelector.Select(aBVH);

  // Should select approximately half of the elements
  EXPECT_GT(aCount, 0);
  EXPECT_LT(aCount, 10);
}

TEST(BVH_TraverseTest, EmptyBoxSelection)
{
  opencascade::handle<BVH_Tree<Standard_Real, 3>> aBVH = CreateSimpleTriangulationBVH(10);

  // Select with box that doesn't intersect any elements
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(100.0, 100.0, 100.0), BVH_Vec3d(200.0, 200.0, 200.0));
  BVH_BoxSelector           aSelector(aBox);
  Standard_Integer          aCount = aSelector.Select(aBVH);

  EXPECT_EQ(aCount, 0);
}

TEST(BVH_TraverseTest, FullBoxSelection)
{
  opencascade::handle<BVH_Tree<Standard_Real, 3>> aBVH = CreateSimpleTriangulationBVH(10);

  // Select with box that contains all elements
  BVH_Box<Standard_Real, 3> aBox(BVH_Vec3d(-100.0, -100.0, -100.0), BVH_Vec3d(100.0, 100.0, 100.0));
  BVH_BoxSelector           aSelector(aBox);
  Standard_Integer          aCount = aSelector.Select(aBVH);

  EXPECT_EQ(aCount, 10);
}

TEST(BVH_TraverseTest, DistanceBasedSelection)
{
  opencascade::handle<BVH_Tree<Standard_Real, 3>> aBVH = CreateSimpleTriangulationBVH(10);

  // Find elements near a point
  BVH_Vec3d            aPoint(5.0, 0.5, 0.0);
  Standard_Real        aMaxDist = 5.0;
  BVH_DistanceSelector aSelector(aPoint, aMaxDist);
  Standard_Integer     aCount = aSelector.Select(aBVH);

  EXPECT_GT(aCount, 0);
  EXPECT_LE(aCount, 10);
  EXPECT_GE(aSelector.ClosestIndex(), 0);
  EXPECT_LT(aSelector.ClosestIndex(), 10);
}

TEST(BVH_TraverseTest, EarlyTermination)
{
  opencascade::handle<BVH_Tree<Standard_Real, 3>> aBVH = CreateSimpleTriangulationBVH(100);

  // Stop after finding 5 elements
  BVH_LimitedSelector aSelector(5);
  Standard_Integer    aCount = aSelector.Select(aBVH);

  EXPECT_EQ(aCount, 5);
  EXPECT_EQ(aSelector.AcceptedCount(), 5);
}

TEST(BVH_TraverseTest, LargeDataSet)
{
  opencascade::handle<BVH_Tree<Standard_Real, 3>> aBVH = CreateSimpleTriangulationBVH(1000);

  BVH_CountAllElements aSelector;
  Standard_Integer     aCount = aSelector.Select(aBVH);

  EXPECT_EQ(aCount, 1000);
}

TEST(BVH_TraverseTest, MetricBasedPruning)
{
  opencascade::handle<BVH_Tree<Standard_Real, 3>> aBVH = CreateSimpleTriangulationBVH(50);

  // Very restrictive distance should result in few acceptances
  BVH_Vec3d            aPoint(1000.0, 1000.0, 1000.0); // Far away
  Standard_Real        aMaxDist = 1.0;                 // Small radius
  BVH_DistanceSelector aSelector(aPoint, aMaxDist);
  Standard_Integer     aCount = aSelector.Select(aBVH);

  EXPECT_EQ(aCount, 0); // Nothing should be within range
}

// =======================================================================================
// Tests for BVH_PairTraverse
// =======================================================================================

TEST(BVH_PairTraverseTest, IsOutVerification)
{
  // Verify that IsOut works correctly for non-overlapping boxes
  BVH_Box<Standard_Real, 3> aBox1(BVH_Vec3d(0.0, 0.0, 0.0), BVH_Vec3d(3.0, 1.0, 0.0));
  BVH_Box<Standard_Real, 3> aBox2(BVH_Vec3d(100.0, 0.0, 0.0), BVH_Vec3d(103.0, 1.0, 0.0));

  // These boxes are far apart and should not overlap
  EXPECT_TRUE(aBox1.IsOut(aBox2.CornerMin(), aBox2.CornerMax()));
  EXPECT_TRUE(aBox2.IsOut(aBox1.CornerMin(), aBox1.CornerMax()));
}

TEST(BVH_PairTraverseTest, TriangulationBoxVerification)
{
  // Create two triangulations and verify their bounding boxes don't overlap
  BVH_Triangulation<Standard_Real, 3> aTri1, aTri2;

  for (Standard_Integer i = 0; i < 3; ++i)
  {
    Standard_Real x = static_cast<Standard_Real>(i);
    BVH::Array<Standard_Real, 3>::Append(aTri1.Vertices, BVH_Vec3d(x, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTri1.Vertices, BVH_Vec3d(x + 0.5, 1.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTri1.Vertices, BVH_Vec3d(x + 1.0, 0.0, 0.0));
    BVH::Array<Standard_Integer, 4>::Append(aTri1.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  for (Standard_Integer i = 0; i < 3; ++i)
  {
    Standard_Real x = 100.0 + static_cast<Standard_Real>(i);
    BVH::Array<Standard_Real, 3>::Append(aTri2.Vertices, BVH_Vec3d(x, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTri2.Vertices, BVH_Vec3d(x + 0.5, 1.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTri2.Vertices, BVH_Vec3d(x + 1.0, 0.0, 0.0));
    BVH::Array<Standard_Integer, 4>::Append(aTri2.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  // Mark as dirty to force bounding box computation
  aTri1.MarkDirty();
  aTri2.MarkDirty();

  BVH_Box<Standard_Real, 3> aBox1 = aTri1.Box();
  BVH_Box<Standard_Real, 3> aBox2 = aTri2.Box();

  // Verify the boxes are where we expect them
  EXPECT_TRUE(aBox1.IsValid());
  EXPECT_TRUE(aBox2.IsValid());

  // Box1 should be roughly [0, 0, 0] to [3, 1, 0]
  EXPECT_NEAR(aBox1.CornerMin().x(), 0.0, 0.01);
  EXPECT_NEAR(aBox1.CornerMax().x(), 3.0, 0.01);

  // Box2 should be roughly [100, 0, 0] to [103, 1, 0]
  EXPECT_NEAR(aBox2.CornerMin().x(), 100.0, 0.01);
  EXPECT_NEAR(aBox2.CornerMax().x(), 103.0, 0.01);

  // The boxes should not overlap
  EXPECT_TRUE(aBox1.IsOut(aBox2));
  EXPECT_TRUE(aBox2.IsOut(aBox1));
}

TEST(BVH_PairTraverseTest, BVHRootBoxVerification)
{
  // Create triangulations and verify their BVH root boxes
  BVH_Triangulation<Standard_Real, 3> aTri1, aTri2;

  for (Standard_Integer i = 0; i < 3; ++i)
  {
    Standard_Real x = static_cast<Standard_Real>(i);
    BVH::Array<Standard_Real, 3>::Append(aTri1.Vertices, BVH_Vec3d(x, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTri1.Vertices, BVH_Vec3d(x + 0.5, 1.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTri1.Vertices, BVH_Vec3d(x + 1.0, 0.0, 0.0));
    BVH::Array<Standard_Integer, 4>::Append(aTri1.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  for (Standard_Integer i = 0; i < 3; ++i)
  {
    Standard_Real x = 100.0 + static_cast<Standard_Real>(i);
    BVH::Array<Standard_Real, 3>::Append(aTri2.Vertices, BVH_Vec3d(x, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTri2.Vertices, BVH_Vec3d(x + 0.5, 1.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTri2.Vertices, BVH_Vec3d(x + 1.0, 0.0, 0.0));
    BVH::Array<Standard_Integer, 4>::Append(aTri2.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  aTri1.MarkDirty();
  aTri2.MarkDirty();

  opencascade::handle<BVH_Tree<Standard_Real, 3>> aBVH1 = new BVH_Tree<Standard_Real, 3>;
  opencascade::handle<BVH_Tree<Standard_Real, 3>> aBVH2 = new BVH_Tree<Standard_Real, 3>;

  BVH_BinnedBuilder<Standard_Real, 3> aBuilder;
  aBuilder.Build(&aTri1, aBVH1.get(), aTri1.Box());
  aBuilder.Build(&aTri2, aBVH2.get(), aTri2.Box());

  // Check the root node bounding boxes
  BVH_Vec3d aMin1 = aBVH1->MinPoint(0);
  BVH_Vec3d aMax1 = aBVH1->MaxPoint(0);
  BVH_Vec3d aMin2 = aBVH2->MinPoint(0);
  BVH_Vec3d aMax2 = aBVH2->MaxPoint(0);

  // Verify root boxes are separated
  EXPECT_NEAR(aMin1.x(), 0.0, 0.01);
  EXPECT_NEAR(aMax1.x(), 3.0, 0.01);
  EXPECT_NEAR(aMin2.x(), 100.0, 0.01);
  EXPECT_NEAR(aMax2.x(), 103.0, 0.01);

  // The root boxes should not overlap
  BVH_Box<Standard_Real, 3> aBox1(aMin1, aMax1);
  EXPECT_TRUE(aBox1.IsOut(aMin2, aMax2));

  // Check if the nodes are inner or leaf nodes
  const BVH_Vec4i& aData1 = aBVH1->NodeInfoBuffer()[0];
  const BVH_Vec4i& aData2 = aBVH2->NodeInfoBuffer()[0];

  // aData.x() == 0 means inner node, != 0 means leaf node
  // With 3 triangles, the tree might be a single leaf node
  EXPECT_EQ(aData1.x(), 1) << "Root of BVH1 should be a leaf node (3 elements)";
  EXPECT_EQ(aData2.x(), 1) << "Root of BVH2 should be a leaf node (3 elements)";
}

TEST(BVH_PairTraverseTest, CountAllPairs)
{
  opencascade::handle<BVH_Tree<Standard_Real, 3>> aBVH1 = CreateSimpleTriangulationBVH(5);
  opencascade::handle<BVH_Tree<Standard_Real, 3>> aBVH2 = CreateSimpleTriangulationBVH(5);

  BVH_CountAllPairs aSelector;
  Standard_Integer  aCount = aSelector.Select(aBVH1, aBVH2);

  EXPECT_EQ(aCount, 25); // 5 x 5 pairs
}

TEST(BVH_PairTraverseTest, EmptyFirstTree)
{
  opencascade::handle<BVH_Tree<Standard_Real, 3>> aBVH1 = new BVH_Tree<Standard_Real, 3>;
  opencascade::handle<BVH_Tree<Standard_Real, 3>> aBVH2 = CreateSimpleTriangulationBVH(5);

  BVH_CountAllPairs aSelector;
  Standard_Integer  aCount = aSelector.Select(aBVH1, aBVH2);

  EXPECT_EQ(aCount, 0);
}

TEST(BVH_PairTraverseTest, EmptySecondTree)
{
  opencascade::handle<BVH_Tree<Standard_Real, 3>> aBVH1 = CreateSimpleTriangulationBVH(5);
  opencascade::handle<BVH_Tree<Standard_Real, 3>> aBVH2 = new BVH_Tree<Standard_Real, 3>;

  BVH_CountAllPairs aSelector;
  Standard_Integer  aCount = aSelector.Select(aBVH1, aBVH2);

  EXPECT_EQ(aCount, 0);
}

TEST(BVH_PairTraverseTest, NullTrees)
{
  opencascade::handle<BVH_Tree<Standard_Real, 3>> aBVH1;
  opencascade::handle<BVH_Tree<Standard_Real, 3>> aBVH2;

  BVH_CountAllPairs aSelector;
  Standard_Integer  aCount = aSelector.Select(aBVH1, aBVH2);

  EXPECT_EQ(aCount, 0);
}

TEST(BVH_PairTraverseTest, OverlapDetection_SameTrees)
{
  opencascade::handle<BVH_Tree<Standard_Real, 3>> aBVH = CreateSimpleTriangulationBVH(10);

  BVH_OverlapDetector aSelector;
  Standard_Integer    aCount = aSelector.Select(aBVH, aBVH);

  // Self-overlap: all 10 elements overlap with themselves
  EXPECT_GE(aCount, 10);
}

TEST(BVH_PairTraverseTest, OverlapDetection_NonOverlapping)
{
  // Create two triangulations in different regions
  BVH_Triangulation<Standard_Real, 3> aTri1, aTri2;

  // First triangulation at x=0..5
  for (Standard_Integer i = 0; i < 3; ++i)
  {
    Standard_Real x = static_cast<Standard_Real>(i);
    BVH::Array<Standard_Real, 3>::Append(aTri1.Vertices, BVH_Vec3d(x, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTri1.Vertices, BVH_Vec3d(x + 0.5, 1.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTri1.Vertices, BVH_Vec3d(x + 1.0, 0.0, 0.0));
    BVH::Array<Standard_Integer, 4>::Append(aTri1.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  // Second triangulation at x=100..105 (far away)
  for (Standard_Integer i = 0; i < 3; ++i)
  {
    Standard_Real x = 100.0 + static_cast<Standard_Real>(i);
    BVH::Array<Standard_Real, 3>::Append(aTri2.Vertices, BVH_Vec3d(x, 0.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTri2.Vertices, BVH_Vec3d(x + 0.5, 1.0, 0.0));
    BVH::Array<Standard_Real, 3>::Append(aTri2.Vertices, BVH_Vec3d(x + 1.0, 0.0, 0.0));
    BVH::Array<Standard_Integer, 4>::Append(aTri2.Elements,
                                            BVH_Vec4i(i * 3, i * 3 + 1, i * 3 + 2, 0));
  }

  // Mark as dirty to force bounding box computation
  aTri1.MarkDirty();
  aTri2.MarkDirty();

  opencascade::handle<BVH_Tree<Standard_Real, 3>> aBVH1 = new BVH_Tree<Standard_Real, 3>;
  opencascade::handle<BVH_Tree<Standard_Real, 3>> aBVH2 = new BVH_Tree<Standard_Real, 3>;

  BVH_BinnedBuilder<Standard_Real, 3> aBuilder;
  aBuilder.Build(&aTri1, aBVH1.get(), aTri1.Box());
  aBuilder.Build(&aTri2, aBVH2.get(), aTri2.Box());

  BVH_OverlapDetector aSelector;
  Standard_Integer    aCount = aSelector.Select(aBVH1, aBVH2);

  // Debug: Check how many times RejectNode was called
  // If it's 0, RejectNode is not being called at all
  // If it's > 0 but overlaps are still found, then IsOut is broken
  EXPECT_GT(aSelector.RejectCount(), 0) << "RejectNode should be called at least once";

  // No overlaps expected
  EXPECT_EQ(aCount, 0) << "Found " << aCount << " overlaps (RejectNode called "
                       << aSelector.RejectCount() << " times)";
}

TEST(BVH_PairTraverseTest, AsymmetricPairs)
{
  opencascade::handle<BVH_Tree<Standard_Real, 3>> aBVH1 = CreateSimpleTriangulationBVH(3);
  opencascade::handle<BVH_Tree<Standard_Real, 3>> aBVH2 = CreateSimpleTriangulationBVH(7);

  BVH_CountAllPairs aSelector;
  Standard_Integer  aCount = aSelector.Select(aBVH1, aBVH2);

  EXPECT_EQ(aCount, 21); // 3 x 7 pairs
}

TEST(BVH_PairTraverseTest, LargeDataSets)
{
  opencascade::handle<BVH_Tree<Standard_Real, 3>> aBVH1 = CreateSimpleTriangulationBVH(50);
  opencascade::handle<BVH_Tree<Standard_Real, 3>> aBVH2 = CreateSimpleTriangulationBVH(50);

  BVH_CountAllPairs aSelector;
  Standard_Integer  aCount = aSelector.Select(aBVH1, aBVH2);

  EXPECT_EQ(aCount, 2500); // 50 x 50 pairs
}
