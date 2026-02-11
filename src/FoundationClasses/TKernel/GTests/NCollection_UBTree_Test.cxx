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

#include <NCollection_UBTree.hxx>
#include <NCollection_EBTree.hxx>

#include <gtest/gtest.h>
#include <vector>
#include <algorithm>

namespace
{

//! Simple 1D interval bounding type for testing.
//! Satisfies the UBTree bounding type interface: Add, IsOut, SquareExtent.
struct TestBnd1D
{
  double Min;
  double Max;

  TestBnd1D()
      : Min(0.0),
        Max(0.0)
  {
  }

  TestBnd1D(double theMin, double theMax)
      : Min(theMin),
        Max(theMax)
  {
  }

  void Add(const TestBnd1D& theOther)
  {
    if (theOther.Min < Min)
      Min = theOther.Min;
    if (theOther.Max > Max)
      Max = theOther.Max;
  }

  bool IsOut(const TestBnd1D& theOther) const { return theOther.Min > Max || theOther.Max < Min; }

  double SquareExtent() const
  {
    double d = Max - Min;
    return d * d;
  }
};

//! Selector that collects all objects overlapping a given query interval.
class TestSelector1D : public NCollection_UBTree<int, TestBnd1D>::Selector
{
public:
  TestSelector1D(const TestBnd1D& theQuery)
      : myQuery(theQuery)
  {
  }

  bool Reject(const TestBnd1D& theBnd) const override { return myQuery.IsOut(theBnd); }

  bool Accept(const int& theObj) override
  {
    myResults.push_back(theObj);
    return true;
  }

  const std::vector<int>& Results() const { return myResults; }

private:
  TestBnd1D        myQuery;
  std::vector<int> myResults;
};

//! Selector that stops after accepting a given number of objects.
class TestStopSelector1D : public NCollection_UBTree<int, TestBnd1D>::Selector
{
public:
  TestStopSelector1D(const TestBnd1D& theQuery, int theMaxAccept)
      : myQuery(theQuery),
        myMaxAccept(theMaxAccept),
        myAccepted(0)
  {
  }

  bool Reject(const TestBnd1D& theBnd) const override { return myQuery.IsOut(theBnd); }

  bool Accept(const int& theObj) override
  {
    myResults.push_back(theObj);
    myAccepted++;
    if (myAccepted >= myMaxAccept)
      myStop = true;
    return true;
  }

  const std::vector<int>& Results() const { return myResults; }

private:
  TestBnd1D        myQuery;
  int              myMaxAccept;
  int              myAccepted;
  std::vector<int> myResults;
};

} // namespace

// ======================= UBTree Tests =======================

TEST(NCollection_UBTreeTest, EmptyTree)
{
  NCollection_UBTree<int, TestBnd1D> aTree;

  EXPECT_TRUE(aTree.IsEmpty());

  TestSelector1D aSelector(TestBnd1D(0.0, 10.0));
  int            nSel = aTree.Select(aSelector);
  EXPECT_EQ(0, nSel);
}

TEST(NCollection_UBTreeTest, AddAndSelect_SingleObject)
{
  NCollection_UBTree<int, TestBnd1D> aTree;

  aTree.Add(1, TestBnd1D(0.0, 5.0));
  EXPECT_FALSE(aTree.IsEmpty());

  // Query overlapping
  TestSelector1D aSelector(TestBnd1D(3.0, 7.0));
  int            nSel = aTree.Select(aSelector);
  EXPECT_EQ(1, nSel);
  ASSERT_EQ(1u, aSelector.Results().size());
  EXPECT_EQ(1, aSelector.Results()[0]);

  // Query not overlapping
  TestSelector1D aSelector2(TestBnd1D(6.0, 10.0));
  int            nSel2 = aTree.Select(aSelector2);
  EXPECT_EQ(0, nSel2);
}

TEST(NCollection_UBTreeTest, AddAndSelect_MultipleObjects)
{
  NCollection_UBTree<int, TestBnd1D> aTree;

  // Add intervals: [0,2], [3,5], [4,6], [8,10]
  aTree.Add(1, TestBnd1D(0.0, 2.0));
  aTree.Add(2, TestBnd1D(3.0, 5.0));
  aTree.Add(3, TestBnd1D(4.0, 6.0));
  aTree.Add(4, TestBnd1D(8.0, 10.0));

  // Query [3.5, 5.5] should overlap with objects 2 and 3
  TestSelector1D aSelector(TestBnd1D(3.5, 5.5));
  int            nSel = aTree.Select(aSelector);
  EXPECT_EQ(2, nSel);

  std::vector<int> aResults = aSelector.Results();
  std::sort(aResults.begin(), aResults.end());
  ASSERT_EQ(2u, aResults.size());
  EXPECT_EQ(2, aResults[0]);
  EXPECT_EQ(3, aResults[1]);

  // Query [0, 10] should overlap all
  TestSelector1D aSelector2(TestBnd1D(0.0, 10.0));
  int            nSel2 = aTree.Select(aSelector2);
  EXPECT_EQ(4, nSel2);
}

TEST(NCollection_UBTreeTest, StopSelector)
{
  NCollection_UBTree<int, TestBnd1D> aTree;

  for (int i = 0; i < 10; ++i)
  {
    aTree.Add(i, TestBnd1D(double(i), double(i + 1)));
  }

  // Query overlaps all, but stop after 3
  TestStopSelector1D aSelector(TestBnd1D(0.0, 11.0), 3);
  int                nSel = aTree.Select(aSelector);
  EXPECT_EQ(3, nSel);
  EXPECT_EQ(3u, aSelector.Results().size());
}

TEST(NCollection_UBTreeTest, Clear)
{
  NCollection_UBTree<int, TestBnd1D> aTree;

  aTree.Add(1, TestBnd1D(0.0, 5.0));
  aTree.Add(2, TestBnd1D(3.0, 7.0));

  EXPECT_FALSE(aTree.IsEmpty());

  aTree.Clear();
  EXPECT_TRUE(aTree.IsEmpty());

  TestSelector1D aSelector(TestBnd1D(0.0, 10.0));
  EXPECT_EQ(0, aTree.Select(aSelector));
}

TEST(NCollection_UBTreeTest, ManyObjects)
{
  NCollection_UBTree<int, TestBnd1D> aTree;

  const int N = 100;
  for (int i = 0; i < N; ++i)
  {
    aTree.Add(i, TestBnd1D(double(i), double(i + 1)));
  }

  // Query that overlaps all
  TestSelector1D aSelector(TestBnd1D(0.0, double(N + 1)));
  int            nSel = aTree.Select(aSelector);
  EXPECT_EQ(N, nSel);
}

TEST(NCollection_UBTreeTest, DeepUnbalancedTree)
{
  // Insert sorted non-overlapping intervals to create a deeply unbalanced tree.
  // Each new interval is "out" of all previous bounds, forcing the tree
  // to grow on one side — producing depth ~ N.
  NCollection_UBTree<int, TestBnd1D> aTree;

  const int N = 200; // well over the initial stack size of 64
  for (int i = 0; i < N; ++i)
  {
    double aLo = double(i) * 10.0;
    aTree.Add(i, TestBnd1D(aLo, aLo + 1.0));
  }

  // Select all — should find all N objects despite deep recursion
  TestSelector1D aSelector(TestBnd1D(0.0, double(N) * 10.0));
  int            nSel = aTree.Select(aSelector);
  EXPECT_EQ(N, nSel);
}

TEST(NCollection_UBTreeTest, MoveConstructor)
{
  NCollection_UBTree<int, TestBnd1D> aSrc;
  aSrc.Add(1, TestBnd1D(0.0, 2.0));
  aSrc.Add(2, TestBnd1D(3.0, 5.0));
  aSrc.Add(3, TestBnd1D(4.0, 6.0));

  NCollection_UBTree<int, TestBnd1D> aDst(std::move(aSrc));

  // Source is empty
  EXPECT_TRUE(aSrc.IsEmpty());

  // Destination has all objects
  EXPECT_FALSE(aDst.IsEmpty());
  TestSelector1D aSelector(TestBnd1D(0.0, 10.0));
  int            nSel = aDst.Select(aSelector);
  EXPECT_EQ(3, nSel);
}

TEST(NCollection_UBTreeTest, MoveAssignment)
{
  NCollection_UBTree<int, TestBnd1D> aSrc;
  aSrc.Add(1, TestBnd1D(0.0, 2.0));
  aSrc.Add(2, TestBnd1D(3.0, 5.0));

  NCollection_UBTree<int, TestBnd1D> aDst;
  aDst.Add(10, TestBnd1D(100.0, 200.0));

  aDst = std::move(aSrc);

  EXPECT_TRUE(aSrc.IsEmpty());
  EXPECT_FALSE(aDst.IsEmpty());

  TestSelector1D aSelector(TestBnd1D(0.0, 10.0));
  int            nSel = aDst.Select(aSelector);
  EXPECT_EQ(2, nSel);
}

// ======================= EBTree Tests =======================

TEST(NCollection_EBTreeTest, AddContainsRemove)
{
  NCollection_EBTree<int, TestBnd1D> aTree;

  EXPECT_TRUE(aTree.IsEmpty());

  aTree.Add(1, TestBnd1D(0.0, 2.0));
  aTree.Add(2, TestBnd1D(3.0, 5.0));
  aTree.Add(3, TestBnd1D(4.0, 6.0));

  EXPECT_TRUE(aTree.Contains(1));
  EXPECT_TRUE(aTree.Contains(2));
  EXPECT_TRUE(aTree.Contains(3));
  EXPECT_FALSE(aTree.Contains(4));

  // Duplicate add should return false
  EXPECT_FALSE(aTree.Add(1, TestBnd1D(0.0, 2.0)));

  // Remove object 2
  EXPECT_TRUE(aTree.Remove(2));
  EXPECT_FALSE(aTree.Contains(2));
  EXPECT_TRUE(aTree.Contains(1));
  EXPECT_TRUE(aTree.Contains(3));

  // Remove non-existent
  EXPECT_FALSE(aTree.Remove(4));
}

TEST(NCollection_EBTreeTest, RemoveRoot)
{
  NCollection_EBTree<int, TestBnd1D> aTree;

  aTree.Add(1, TestBnd1D(0.0, 5.0));
  EXPECT_TRUE(aTree.Contains(1));

  // Removing the only element (root) should clear the tree
  EXPECT_TRUE(aTree.Remove(1));
  EXPECT_TRUE(aTree.IsEmpty());
}

TEST(NCollection_EBTreeTest, SelectAfterRemove)
{
  NCollection_EBTree<int, TestBnd1D> aTree;

  aTree.Add(1, TestBnd1D(0.0, 2.0));
  aTree.Add(2, TestBnd1D(3.0, 5.0));
  aTree.Add(3, TestBnd1D(4.0, 6.0));

  // Remove object 2
  aTree.Remove(2);

  // Select overlapping [3.5, 5.5] — only object 3 should remain
  TestSelector1D aSelector(TestBnd1D(3.5, 5.5));
  int            nSel = aTree.Select(aSelector);
  EXPECT_EQ(1, nSel);
  ASSERT_EQ(1u, aSelector.Results().size());
  EXPECT_EQ(3, aSelector.Results()[0]);
}

TEST(NCollection_EBTreeTest, FindNode)
{
  NCollection_EBTree<int, TestBnd1D> aTree;

  aTree.Add(1, TestBnd1D(0.0, 2.0));
  aTree.Add(2, TestBnd1D(3.0, 5.0));

  const auto& aNode = aTree.FindNode(1);
  EXPECT_TRUE(aNode.IsLeaf());
  EXPECT_EQ(1, aNode.Object());
}

TEST(NCollection_EBTreeTest, RemoveAll)
{
  NCollection_EBTree<int, TestBnd1D> aTree;

  aTree.Add(1, TestBnd1D(0.0, 2.0));
  aTree.Add(2, TestBnd1D(3.0, 5.0));
  aTree.Add(3, TestBnd1D(4.0, 6.0));

  EXPECT_TRUE(aTree.Remove(1));
  EXPECT_TRUE(aTree.Remove(2));
  EXPECT_TRUE(aTree.Remove(3));
  EXPECT_TRUE(aTree.IsEmpty());
}

TEST(NCollection_EBTreeTest, MoveConstructor)
{
  NCollection_EBTree<int, TestBnd1D> aSrc;
  aSrc.Add(1, TestBnd1D(0.0, 2.0));
  aSrc.Add(2, TestBnd1D(3.0, 5.0));
  aSrc.Add(3, TestBnd1D(4.0, 6.0));

  NCollection_EBTree<int, TestBnd1D> aDst(std::move(aSrc));

  // Source is empty
  EXPECT_TRUE(aSrc.IsEmpty());
  EXPECT_FALSE(aSrc.Contains(1));

  // Destination has all objects
  EXPECT_FALSE(aDst.IsEmpty());
  EXPECT_TRUE(aDst.Contains(1));
  EXPECT_TRUE(aDst.Contains(2));
  EXPECT_TRUE(aDst.Contains(3));

  TestSelector1D aSelector(TestBnd1D(0.0, 10.0));
  int            nSel = aDst.Select(aSelector);
  EXPECT_EQ(3, nSel);
}

TEST(NCollection_EBTreeTest, MoveAssignment)
{
  NCollection_EBTree<int, TestBnd1D> aSrc;
  aSrc.Add(1, TestBnd1D(0.0, 2.0));
  aSrc.Add(2, TestBnd1D(3.0, 5.0));

  NCollection_EBTree<int, TestBnd1D> aDst;
  aDst.Add(10, TestBnd1D(100.0, 200.0));

  aDst = std::move(aSrc);

  EXPECT_TRUE(aSrc.IsEmpty());
  EXPECT_FALSE(aDst.IsEmpty());
  EXPECT_TRUE(aDst.Contains(1));
  EXPECT_TRUE(aDst.Contains(2));
  EXPECT_FALSE(aDst.Contains(10));

  TestSelector1D aSelector(TestBnd1D(0.0, 10.0));
  int            nSel = aDst.Select(aSelector);
  EXPECT_EQ(2, nSel);
}
