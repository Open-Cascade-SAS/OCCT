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

#include <NCollection_List.hxx>
#include <NCollection_IncAllocator.hxx>
#include <Standard_Integer.hxx>

#include <gtest/gtest.h>
#include <algorithm>
#include <list>
#include <random>

// Test fixture for NCollection_List tests
class NCollection_ListTest : public testing::Test
{
protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(NCollection_ListTest, DefaultConstructor)
{
  // Default constructor should create an empty list
  NCollection_List<Standard_Integer> aList;
  EXPECT_TRUE(aList.IsEmpty());
  EXPECT_EQ(0, aList.Size());
  EXPECT_EQ(0, aList.Extent());
}

TEST_F(NCollection_ListTest, Append)
{
  NCollection_List<Standard_Integer> aList;

  // Test Append method
  EXPECT_EQ(10, aList.Append(10));
  EXPECT_EQ(20, aList.Append(20));
  EXPECT_EQ(30, aList.Append(30));

  EXPECT_FALSE(aList.IsEmpty());
  EXPECT_EQ(3, aList.Size());

  // Test First and Last access
  EXPECT_EQ(10, aList.First());
  EXPECT_EQ(30, aList.Last());
}

TEST_F(NCollection_ListTest, Prepend)
{
  NCollection_List<Standard_Integer> aList;

  // Test Prepend method
  EXPECT_EQ(30, aList.Prepend(30));
  EXPECT_EQ(20, aList.Prepend(20));
  EXPECT_EQ(10, aList.Prepend(10));

  EXPECT_FALSE(aList.IsEmpty());
  EXPECT_EQ(3, aList.Size());

  // Test First and Last access
  EXPECT_EQ(10, aList.First());
  EXPECT_EQ(30, aList.Last());
}

TEST_F(NCollection_ListTest, IteratorAccess)
{
  NCollection_List<Standard_Integer> aList;
  aList.Append(10);
  aList.Append(20);
  aList.Append(30);

  // Test iteration using OCCT iterator
  NCollection_List<Standard_Integer>::Iterator it(aList);
  Standard_Integer                             expectedValues[] = {10, 20, 30};
  Standard_Integer                             index            = 0;

  for (; it.More(); it.Next(), index++)
  {
    EXPECT_EQ(expectedValues[index], it.Value());
  }
  EXPECT_EQ(3, index); // Ensure we iterated through all elements
}

TEST_F(NCollection_ListTest, STLIterators)
{
  NCollection_List<Standard_Integer> aList;
  aList.Append(10);
  aList.Append(20);
  aList.Append(30);

  // Test STL-compatible iterators
  Standard_Integer expectedValues[] = {10, 20, 30};
  Standard_Integer index            = 0;

  for (auto it = aList.begin(); it != aList.end(); ++it, ++index)
  {
    EXPECT_EQ(expectedValues[index], *it);
  }
  EXPECT_EQ(3, index);

  // Test range-based for loop
  index = 0;
  for (const auto& value : aList)
  {
    EXPECT_EQ(expectedValues[index++], value);
  }
  EXPECT_EQ(3, index);
}

TEST_F(NCollection_ListTest, RemoveFirst)
{
  NCollection_List<Standard_Integer> aList;
  aList.Append(10);
  aList.Append(20);
  aList.Append(30);

  // Test RemoveFirst
  aList.RemoveFirst();
  EXPECT_EQ(2, aList.Size());
  EXPECT_EQ(20, aList.First());

  aList.RemoveFirst();
  EXPECT_EQ(1, aList.Size());
  EXPECT_EQ(30, aList.First());

  aList.RemoveFirst();
  EXPECT_TRUE(aList.IsEmpty());
}

TEST_F(NCollection_ListTest, Remove)
{
  NCollection_List<Standard_Integer> aList;
  aList.Append(10);
  aList.Append(20);
  aList.Append(30);

  // Test Remove with iterator
  NCollection_List<Standard_Integer>::Iterator it(aList);
  it.Next();        // Point to second element (20)
  aList.Remove(it); // Remove 20, iterator now points to 30

  // Check the list after removal
  EXPECT_EQ(2, aList.Size());
  EXPECT_EQ(10, aList.First());
  EXPECT_EQ(30, aList.Last());
  EXPECT_EQ(30, it.Value()); // Iterator should now point to 30
}

TEST_F(NCollection_ListTest, RemoveByValue)
{
  NCollection_List<Standard_Integer> aList;
  aList.Append(10);
  aList.Append(20);
  aList.Append(10); // Add duplicate
  aList.Append(30);

  // Test removing by value - should remove the first occurrence only
  bool removed = aList.Remove(10);
  EXPECT_TRUE(removed);
  EXPECT_EQ(3, aList.Size());
  EXPECT_EQ(20, aList.First());

  // Try to remove a non-existent value
  removed = aList.Remove(50);
  EXPECT_FALSE(removed);
  EXPECT_EQ(3, aList.Size());

  // Remove the second occurrence of 10
  removed = aList.Remove(10);
  EXPECT_TRUE(removed);
  EXPECT_EQ(2, aList.Size());

  // Check final list state
  NCollection_List<Standard_Integer>::Iterator it(aList);
  EXPECT_EQ(20, it.Value());
  it.Next();
  EXPECT_EQ(30, it.Value());
}

TEST_F(NCollection_ListTest, Clear)
{
  NCollection_List<Standard_Integer> aList;
  aList.Append(10);
  aList.Append(20);
  aList.Append(30);

  // Test Clear
  aList.Clear();
  EXPECT_TRUE(aList.IsEmpty());
  EXPECT_EQ(0, aList.Size());
}

TEST_F(NCollection_ListTest, Assignment)
{
  NCollection_List<Standard_Integer> aList1;
  aList1.Append(10);
  aList1.Append(20);
  aList1.Append(30);

  // Test assignment operator
  NCollection_List<Standard_Integer> aList2;
  aList2 = aList1;

  // Check both lists have the same content
  EXPECT_EQ(aList1.Size(), aList2.Size());

  NCollection_List<Standard_Integer>::Iterator it1(aList1);
  NCollection_List<Standard_Integer>::Iterator it2(aList2);

  for (; it1.More() && it2.More(); it1.Next(), it2.Next())
  {
    EXPECT_EQ(it1.Value(), it2.Value());
  }

  // Modify original to ensure deep copy
  aList1.First() = 100;
  EXPECT_EQ(100, aList1.First());
  EXPECT_EQ(10, aList2.First());
}

TEST_F(NCollection_ListTest, AssignMethod)
{
  NCollection_List<Standard_Integer> aList1;
  aList1.Append(10);
  aList1.Append(20);
  aList1.Append(30);

  // Test Assign method
  NCollection_List<Standard_Integer> aList2;
  aList2.Append(40);     // Add some initial content
  aList2.Assign(aList1); // This should replace aList2's content

  EXPECT_EQ(aList1.Size(), aList2.Size());

  // Check values
  NCollection_List<Standard_Integer>::Iterator it2(aList2);
  EXPECT_EQ(10, it2.Value());
  it2.Next();
  EXPECT_EQ(20, it2.Value());
  it2.Next();
  EXPECT_EQ(30, it2.Value());
}

TEST_F(NCollection_ListTest, AppendList)
{
  NCollection_List<Standard_Integer> aList1;
  aList1.Append(10);
  aList1.Append(20);

  NCollection_List<Standard_Integer> aList2;
  aList2.Append(30);
  aList2.Append(40);

  // Test Append(List)
  aList1.Append(aList2);

  EXPECT_EQ(4, aList1.Size());
  EXPECT_TRUE(aList2.IsEmpty()); // aList2 should be cleared

  // Check values in aList1
  NCollection_List<Standard_Integer>::Iterator it(aList1);
  EXPECT_EQ(10, it.Value());
  it.Next();
  EXPECT_EQ(20, it.Value());
  it.Next();
  EXPECT_EQ(30, it.Value());
  it.Next();
  EXPECT_EQ(40, it.Value());
}

TEST_F(NCollection_ListTest, PrependList)
{
  NCollection_List<Standard_Integer> aList1;
  aList1.Append(30);
  aList1.Append(40);

  NCollection_List<Standard_Integer> aList2;
  aList2.Append(10);
  aList2.Append(20);

  // Test Prepend(List)
  aList1.Prepend(aList2);

  EXPECT_EQ(4, aList1.Size());
  EXPECT_TRUE(aList2.IsEmpty()); // aList2 should be cleared

  // Check values in aList1
  NCollection_List<Standard_Integer>::Iterator it(aList1);
  EXPECT_EQ(10, it.Value());
  it.Next();
  EXPECT_EQ(20, it.Value());
  it.Next();
  EXPECT_EQ(30, it.Value());
  it.Next();
  EXPECT_EQ(40, it.Value());
}

TEST_F(NCollection_ListTest, InsertBefore)
{
  NCollection_List<Standard_Integer> aList;
  aList.Append(10);
  aList.Append(30);

  // Get iterator to second element
  NCollection_List<Standard_Integer>::Iterator it(aList);
  it.Next();

  // Insert before the second element
  EXPECT_EQ(20, aList.InsertBefore(20, it));

  // Check the list
  EXPECT_EQ(3, aList.Size());

  NCollection_List<Standard_Integer>::Iterator checkIt(aList);
  EXPECT_EQ(10, checkIt.Value());
  checkIt.Next();
  EXPECT_EQ(20, checkIt.Value());
  checkIt.Next();
  EXPECT_EQ(30, checkIt.Value());
}

TEST_F(NCollection_ListTest, InsertAfter)
{
  NCollection_List<Standard_Integer> aList;
  aList.Append(10);
  aList.Append(30);

  // Get iterator to first element
  NCollection_List<Standard_Integer>::Iterator it(aList);

  // Insert after the first element
  EXPECT_EQ(20, aList.InsertAfter(20, it));

  // Check the list
  EXPECT_EQ(3, aList.Size());

  NCollection_List<Standard_Integer>::Iterator checkIt(aList);
  EXPECT_EQ(10, checkIt.Value());
  checkIt.Next();
  EXPECT_EQ(20, checkIt.Value());
  checkIt.Next();
  EXPECT_EQ(30, checkIt.Value());
}

TEST_F(NCollection_ListTest, InsertList)
{
  NCollection_List<Standard_Integer> aList1;
  aList1.Append(10);
  aList1.Append(40);

  NCollection_List<Standard_Integer> aList2;
  aList2.Append(20);
  aList2.Append(30);

  // Get iterator to the second element in aList1
  NCollection_List<Standard_Integer>::Iterator it(aList1);
  it.Next();

  // Insert aList2 before the second element in aList1
  aList1.InsertBefore(aList2, it);

  EXPECT_EQ(4, aList1.Size());
  EXPECT_TRUE(aList2.IsEmpty());

  // Check the resulting list
  NCollection_List<Standard_Integer>::Iterator checkIt(aList1);
  EXPECT_EQ(10, checkIt.Value());
  checkIt.Next();
  EXPECT_EQ(20, checkIt.Value());
  checkIt.Next();
  EXPECT_EQ(30, checkIt.Value());
  checkIt.Next();
  EXPECT_EQ(40, checkIt.Value());
}

TEST_F(NCollection_ListTest, Reverse)
{
  NCollection_List<Standard_Integer> aList;
  aList.Append(10);
  aList.Append(20);
  aList.Append(30);

  // Test Reverse
  aList.Reverse();

  // Check the reversed list
  EXPECT_EQ(30, aList.First());
  EXPECT_EQ(10, aList.Last());

  NCollection_List<Standard_Integer>::Iterator it(aList);
  EXPECT_EQ(30, it.Value());
  it.Next();
  EXPECT_EQ(20, it.Value());
  it.Next();
  EXPECT_EQ(10, it.Value());
}

TEST_F(NCollection_ListTest, STLAlgorithmCompatibility_MinMax)
{
  NCollection_List<Standard_Integer> aList;
  std::list<Standard_Integer>        aStdList;

  std::mt19937 aGenerator(1); // Fixed seed for reproducible tests
  std::uniform_int_distribution<Standard_Integer> aDistribution(0, RAND_MAX);
  for (Standard_Integer anIdx = 0; anIdx < 100; ++anIdx)
  {
    Standard_Integer aVal = aDistribution(aGenerator);
    aList.Append(aVal);
    aStdList.push_back(aVal);
  }

  auto aMinOCCT = std::min_element(aList.begin(), aList.end());
  auto aMinStd  = std::min_element(aStdList.begin(), aStdList.end());

  auto aMaxOCCT = std::max_element(aList.begin(), aList.end());
  auto aMaxStd  = std::max_element(aStdList.begin(), aStdList.end());

  EXPECT_EQ(*aMinOCCT, *aMinStd);
  EXPECT_EQ(*aMaxOCCT, *aMaxStd);
}

TEST_F(NCollection_ListTest, STLAlgorithmCompatibility_Replace)
{
  NCollection_List<Standard_Integer> aList;
  std::list<Standard_Integer>        aStdList;

  std::mt19937 aGenerator(1); // Fixed seed for reproducible tests
  std::uniform_int_distribution<Standard_Integer> aDistribution(0, RAND_MAX);
  for (Standard_Integer anIdx = 0; anIdx < 100; ++anIdx)
  {
    Standard_Integer aVal = aDistribution(aGenerator);
    aList.Append(aVal);
    aStdList.push_back(aVal);
  }

  Standard_Integer aTargetValue = aStdList.back();
  Standard_Integer aNewValue    = -1;

  std::replace(aList.begin(), aList.end(), aTargetValue, aNewValue);
  std::replace(aStdList.begin(), aStdList.end(), aTargetValue, aNewValue);

  EXPECT_TRUE(std::equal(aList.begin(), aList.end(), aStdList.begin()));
}

TEST_F(NCollection_ListTest, OCC25348_AssignDoesNotChangeAllocator)
{
  // Bug OCC25348: Method Assign of NCollection containers must not change
  // own allocator of the target
  // This test verifies that calling Assign() doesn't change the allocator
  // of the target list

  Handle(NCollection_IncAllocator) anAlloc1 = new NCollection_IncAllocator();
  NCollection_List<int>            aList1(anAlloc1);

  // Perform multiple assign operations with different source lists,
  // each having their own allocator
  for (int i = 0; i < 10; i++)
  {
    Handle(NCollection_IncAllocator) anAlloc2 = new NCollection_IncAllocator();
    NCollection_List<int>            aList2(anAlloc2);
    aList2.Append(i);

    // Store the allocator before Assign
    Handle(NCollection_BaseAllocator) anAllocBefore = aList1.Allocator();

    // Assign aList2 to aList1
    aList1.Assign(aList2);

    // Verify that the allocator of aList1 hasn't changed
    Handle(NCollection_BaseAllocator) anAllocAfter = aList1.Allocator();
    EXPECT_EQ(anAllocBefore, anAllocAfter) << "Assign() should not change the target's allocator";

    // Verify the content was copied correctly
    EXPECT_EQ(1, aList1.Size());
    EXPECT_EQ(i, aList1.First());
  }
}