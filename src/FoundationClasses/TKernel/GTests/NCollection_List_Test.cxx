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
  NCollection_List<int> aList;
  EXPECT_TRUE(aList.IsEmpty());
  EXPECT_EQ(0, aList.Size());
  EXPECT_EQ(0, aList.Extent());
}

TEST_F(NCollection_ListTest, Append)
{
  NCollection_List<int> aList;

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
  NCollection_List<int> aList;

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
  NCollection_List<int> aList;
  aList.Append(10);
  aList.Append(20);
  aList.Append(30);

  // Test iteration using OCCT iterator
  NCollection_List<int>::Iterator it(aList);
  int                             expectedValues[] = {10, 20, 30};
  int                             index            = 0;

  for (; it.More(); it.Next(), index++)
  {
    EXPECT_EQ(expectedValues[index], it.Value());
  }
  EXPECT_EQ(3, index); // Ensure we iterated through all elements
}

TEST_F(NCollection_ListTest, STLIterators)
{
  NCollection_List<int> aList;
  aList.Append(10);
  aList.Append(20);
  aList.Append(30);

  // Test STL-compatible iterators
  int expectedValues[] = {10, 20, 30};
  int index            = 0;

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
  NCollection_List<int> aList;
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
  NCollection_List<int> aList;
  aList.Append(10);
  aList.Append(20);
  aList.Append(30);

  // Test Remove with iterator
  NCollection_List<int>::Iterator it(aList);
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
  NCollection_List<int> aList;
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
  NCollection_List<int>::Iterator it(aList);
  EXPECT_EQ(20, it.Value());
  it.Next();
  EXPECT_EQ(30, it.Value());
}

TEST_F(NCollection_ListTest, Clear)
{
  NCollection_List<int> aList;
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
  NCollection_List<int> aList1;
  aList1.Append(10);
  aList1.Append(20);
  aList1.Append(30);

  // Test assignment operator
  NCollection_List<int> aList2;
  aList2 = aList1;

  // Check both lists have the same content
  EXPECT_EQ(aList1.Size(), aList2.Size());

  NCollection_List<int>::Iterator it1(aList1);
  NCollection_List<int>::Iterator it2(aList2);

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
  NCollection_List<int> aList1;
  aList1.Append(10);
  aList1.Append(20);
  aList1.Append(30);

  // Test Assign method
  NCollection_List<int> aList2;
  aList2.Append(40);     // Add some initial content
  aList2.Assign(aList1); // This should replace aList2's content

  EXPECT_EQ(aList1.Size(), aList2.Size());

  // Check values
  NCollection_List<int>::Iterator it2(aList2);
  EXPECT_EQ(10, it2.Value());
  it2.Next();
  EXPECT_EQ(20, it2.Value());
  it2.Next();
  EXPECT_EQ(30, it2.Value());
}

TEST_F(NCollection_ListTest, AppendList)
{
  NCollection_List<int> aList1;
  aList1.Append(10);
  aList1.Append(20);

  NCollection_List<int> aList2;
  aList2.Append(30);
  aList2.Append(40);

  // Test Append(List)
  aList1.Append(aList2);

  EXPECT_EQ(4, aList1.Size());
  EXPECT_TRUE(aList2.IsEmpty()); // aList2 should be cleared

  // Check values in aList1
  NCollection_List<int>::Iterator it(aList1);
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
  NCollection_List<int> aList1;
  aList1.Append(30);
  aList1.Append(40);

  NCollection_List<int> aList2;
  aList2.Append(10);
  aList2.Append(20);

  // Test Prepend(List)
  aList1.Prepend(aList2);

  EXPECT_EQ(4, aList1.Size());
  EXPECT_TRUE(aList2.IsEmpty()); // aList2 should be cleared

  // Check values in aList1
  NCollection_List<int>::Iterator it(aList1);
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
  NCollection_List<int> aList;
  aList.Append(10);
  aList.Append(30);

  // Get iterator to second element
  NCollection_List<int>::Iterator it(aList);
  it.Next();

  // Insert before the second element
  EXPECT_EQ(20, aList.InsertBefore(20, it));

  // Check the list
  EXPECT_EQ(3, aList.Size());

  NCollection_List<int>::Iterator checkIt(aList);
  EXPECT_EQ(10, checkIt.Value());
  checkIt.Next();
  EXPECT_EQ(20, checkIt.Value());
  checkIt.Next();
  EXPECT_EQ(30, checkIt.Value());
}

TEST_F(NCollection_ListTest, InsertAfter)
{
  NCollection_List<int> aList;
  aList.Append(10);
  aList.Append(30);

  // Get iterator to first element
  NCollection_List<int>::Iterator it(aList);

  // Insert after the first element
  EXPECT_EQ(20, aList.InsertAfter(20, it));

  // Check the list
  EXPECT_EQ(3, aList.Size());

  NCollection_List<int>::Iterator checkIt(aList);
  EXPECT_EQ(10, checkIt.Value());
  checkIt.Next();
  EXPECT_EQ(20, checkIt.Value());
  checkIt.Next();
  EXPECT_EQ(30, checkIt.Value());
}

TEST_F(NCollection_ListTest, InsertList)
{
  NCollection_List<int> aList1;
  aList1.Append(10);
  aList1.Append(40);

  NCollection_List<int> aList2;
  aList2.Append(20);
  aList2.Append(30);

  // Get iterator to the second element in aList1
  NCollection_List<int>::Iterator it(aList1);
  it.Next();

  // Insert aList2 before the second element in aList1
  aList1.InsertBefore(aList2, it);

  EXPECT_EQ(4, aList1.Size());
  EXPECT_TRUE(aList2.IsEmpty());

  // Check the resulting list
  NCollection_List<int>::Iterator checkIt(aList1);
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
  NCollection_List<int> aList;
  aList.Append(10);
  aList.Append(20);
  aList.Append(30);

  // Test Reverse
  aList.Reverse();

  // Check the reversed list
  EXPECT_EQ(30, aList.First());
  EXPECT_EQ(10, aList.Last());

  NCollection_List<int>::Iterator it(aList);
  EXPECT_EQ(30, it.Value());
  it.Next();
  EXPECT_EQ(20, it.Value());
  it.Next();
  EXPECT_EQ(10, it.Value());
}

TEST_F(NCollection_ListTest, STLAlgorithmCompatibility_MinMax)
{
  NCollection_List<int> aList;
  std::list<int>        aStdList;

  std::mt19937                       aGenerator(1); // Fixed seed for reproducible tests
  std::uniform_int_distribution<int> aDistribution(0, RAND_MAX);
  for (int anIdx = 0; anIdx < 100; ++anIdx)
  {
    int aVal = aDistribution(aGenerator);
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
  NCollection_List<int> aList;
  std::list<int>        aStdList;

  std::mt19937                       aGenerator(1); // Fixed seed for reproducible tests
  std::uniform_int_distribution<int> aDistribution(0, RAND_MAX);
  for (int anIdx = 0; anIdx < 100; ++anIdx)
  {
    int aVal = aDistribution(aGenerator);
    aList.Append(aVal);
    aStdList.push_back(aVal);
  }

  int aTargetValue = aStdList.back();
  int aNewValue    = -1;

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

  occ::handle<NCollection_IncAllocator> anAlloc1 = new NCollection_IncAllocator();
  NCollection_List<int>                 aList1(anAlloc1);

  // Perform multiple assign operations with different source lists,
  // each having their own allocator
  for (int i = 0; i < 10; i++)
  {
    occ::handle<NCollection_IncAllocator> anAlloc2 = new NCollection_IncAllocator();
    NCollection_List<int>                 aList2(anAlloc2);
    aList2.Append(i);

    // Store the allocator before Assign
    occ::handle<NCollection_BaseAllocator> anAllocBefore = aList1.Allocator();

    // Assign aList2 to aList1
    aList1.Assign(aList2);

    // Verify that the allocator of aList1 hasn't changed
    occ::handle<NCollection_BaseAllocator> anAllocAfter = aList1.Allocator();
    EXPECT_EQ(anAllocBefore, anAllocAfter) << "Assign() should not change the target's allocator";

    // Verify the content was copied correctly
    EXPECT_EQ(1, aList1.Size());
    EXPECT_EQ(i, aList1.First());
  }
}

// Helper struct for testing in-place construction with multiple arguments
struct MultiArgType
{
  int    myA;
  double myB;

  MultiArgType(int theA, double theB)
      : myA(theA),
        myB(theB)
  {
  }
};

// Helper struct for testing move-only types
struct MoveOnlyType
{
  int myValue;

  explicit MoveOnlyType(int theValue)
      : myValue(theValue)
  {
  }

  MoveOnlyType(MoveOnlyType&& theOther) noexcept
      : myValue(theOther.myValue)
  {
    theOther.myValue = 0;
  }

  MoveOnlyType& operator=(MoveOnlyType&& theOther) noexcept
  {
    myValue          = theOther.myValue;
    theOther.myValue = 0;
    return *this;
  }

  MoveOnlyType(const MoveOnlyType&)            = delete;
  MoveOnlyType& operator=(const MoveOnlyType&) = delete;
};

TEST_F(NCollection_ListTest, EmplaceAppend)
{
  NCollection_List<MultiArgType> aList;

  // Test EmplaceAppend with multiple constructor arguments
  MultiArgType& aRef1 = aList.EmplaceAppend(42, 3.14);
  EXPECT_EQ(42, aRef1.myA);
  EXPECT_NEAR(3.14, aRef1.myB, 1e-10);
  EXPECT_EQ(1, aList.Size());

  MultiArgType& aRef2 = aList.EmplaceAppend(100, 2.71);
  EXPECT_EQ(100, aRef2.myA);
  EXPECT_NEAR(2.71, aRef2.myB, 1e-10);
  EXPECT_EQ(2, aList.Size());

  // Verify the order
  EXPECT_EQ(42, aList.First().myA);
  EXPECT_EQ(100, aList.Last().myA);
}

TEST_F(NCollection_ListTest, EmplacePrepend)
{
  NCollection_List<MultiArgType> aList;

  // Test EmplacePrepend with multiple constructor arguments
  MultiArgType& aRef1 = aList.EmplacePrepend(42, 3.14);
  EXPECT_EQ(42, aRef1.myA);
  EXPECT_NEAR(3.14, aRef1.myB, 1e-10);
  EXPECT_EQ(1, aList.Size());

  MultiArgType& aRef2 = aList.EmplacePrepend(100, 2.71);
  EXPECT_EQ(100, aRef2.myA);
  EXPECT_NEAR(2.71, aRef2.myB, 1e-10);
  EXPECT_EQ(2, aList.Size());

  // Verify the order (prepended items should be at the front)
  EXPECT_EQ(100, aList.First().myA);
  EXPECT_EQ(42, aList.Last().myA);
}

TEST_F(NCollection_ListTest, EmplaceBefore)
{
  NCollection_List<MultiArgType> aList;
  aList.EmplaceAppend(10, 1.0);
  aList.EmplaceAppend(30, 3.0);

  // Get iterator to second element
  NCollection_List<MultiArgType>::Iterator anIter(aList);
  anIter.Next();

  // Emplace before the second element
  MultiArgType& aRef = aList.EmplaceBefore(anIter, 20, 2.0);
  EXPECT_EQ(20, aRef.myA);
  EXPECT_NEAR(2.0, aRef.myB, 1e-10);

  // Verify the order
  EXPECT_EQ(3, aList.Size());
  NCollection_List<MultiArgType>::Iterator aCheckIter(aList);
  EXPECT_EQ(10, aCheckIter.Value().myA);
  aCheckIter.Next();
  EXPECT_EQ(20, aCheckIter.Value().myA);
  aCheckIter.Next();
  EXPECT_EQ(30, aCheckIter.Value().myA);
}

TEST_F(NCollection_ListTest, EmplaceAfter)
{
  NCollection_List<MultiArgType> aList;
  aList.EmplaceAppend(10, 1.0);
  aList.EmplaceAppend(30, 3.0);

  // Get iterator to first element
  NCollection_List<MultiArgType>::Iterator anIter(aList);

  // Emplace after the first element
  MultiArgType& aRef = aList.EmplaceAfter(anIter, 20, 2.0);
  EXPECT_EQ(20, aRef.myA);
  EXPECT_NEAR(2.0, aRef.myB, 1e-10);

  // Verify the order
  EXPECT_EQ(3, aList.Size());
  NCollection_List<MultiArgType>::Iterator aCheckIter(aList);
  EXPECT_EQ(10, aCheckIter.Value().myA);
  aCheckIter.Next();
  EXPECT_EQ(20, aCheckIter.Value().myA);
  aCheckIter.Next();
  EXPECT_EQ(30, aCheckIter.Value().myA);
}

TEST_F(NCollection_ListTest, EmplaceWithMoveOnlyType)
{
  NCollection_List<MoveOnlyType> aList;

  // Test EmplaceAppend with move-only type
  MoveOnlyType& aRef1 = aList.EmplaceAppend(42);
  EXPECT_EQ(42, aRef1.myValue);

  MoveOnlyType& aRef2 = aList.EmplacePrepend(100);
  EXPECT_EQ(100, aRef2.myValue);

  EXPECT_EQ(2, aList.Size());
  EXPECT_EQ(100, aList.First().myValue);
  EXPECT_EQ(42, aList.Last().myValue);
}

TEST_F(NCollection_ListTest, InitializerListConstructor)
{
  // Test initializer list constructor
  NCollection_List<int> aList = {10, 20, 30, 40, 50};

  EXPECT_EQ(5, aList.Size());
  EXPECT_EQ(10, aList.First());
  EXPECT_EQ(50, aList.Last());

  // Verify all elements
  int                             anExpected[] = {10, 20, 30, 40, 50};
  NCollection_List<int>::Iterator anIter(aList);
  for (int anIdx = 0; anIter.More(); anIter.Next(), ++anIdx)
  {
    EXPECT_EQ(anExpected[anIdx], anIter.Value());
  }
}

TEST_F(NCollection_ListTest, InitializerListConstructorEmpty)
{
  // Test empty initializer list
  NCollection_List<int> aList = {};

  EXPECT_TRUE(aList.IsEmpty());
  EXPECT_EQ(0, aList.Size());
}

TEST_F(NCollection_ListTest, Exchange)
{
  NCollection_List<int> aList1;
  aList1.Append(10);
  aList1.Append(20);

  NCollection_List<int> aList2;
  aList2.Append(30);
  aList2.Append(40);
  aList2.Append(50);

  // Exchange the lists
  aList1.Exchange(aList2);

  // Verify aList1 now has aList2's content
  EXPECT_EQ(3, aList1.Size());
  EXPECT_EQ(30, aList1.First());
  EXPECT_EQ(50, aList1.Last());

  // Verify aList2 now has aList1's content
  EXPECT_EQ(2, aList2.Size());
  EXPECT_EQ(10, aList2.First());
  EXPECT_EQ(20, aList2.Last());
}

TEST_F(NCollection_ListTest, ExchangeWithEmpty)
{
  NCollection_List<int> aList1;
  aList1.Append(10);
  aList1.Append(20);

  NCollection_List<int> aList2;

  // Exchange with empty list
  aList1.Exchange(aList2);

  EXPECT_TRUE(aList1.IsEmpty());
  EXPECT_EQ(2, aList2.Size());
  EXPECT_EQ(10, aList2.First());
  EXPECT_EQ(20, aList2.Last());
}

TEST_F(NCollection_ListTest, MoveConstructor)
{
  NCollection_List<int> aList1;
  aList1.Append(10);
  aList1.Append(20);
  aList1.Append(30);

  // Move construct
  NCollection_List<int> aList2(std::move(aList1));

  // Verify aList2 has the content
  EXPECT_EQ(3, aList2.Size());
  EXPECT_EQ(10, aList2.First());
  EXPECT_EQ(30, aList2.Last());

  // Verify aList1 is empty after move
  EXPECT_TRUE(aList1.IsEmpty());
  EXPECT_EQ(0, aList1.Size());
}

TEST_F(NCollection_ListTest, MoveAssignment)
{
  NCollection_List<int> aList1;
  aList1.Append(10);
  aList1.Append(20);

  NCollection_List<int> aList2;
  aList2.Append(100); // Some initial content

  // Move assign
  aList2 = std::move(aList1);

  // Verify aList2 has aList1's content
  EXPECT_EQ(2, aList2.Size());
  EXPECT_EQ(10, aList2.First());
  EXPECT_EQ(20, aList2.Last());

  // Verify aList1 is empty after move
  EXPECT_TRUE(aList1.IsEmpty());
}

TEST_F(NCollection_ListTest, ConstIteratorFromConstList)
{
  NCollection_List<int> aList;
  aList.Append(10);
  aList.Append(20);
  aList.Append(30);

  // Get const reference to the list
  const NCollection_List<int>& aConstList = aList;

  // Iterate using const iterators
  int anExpected[] = {10, 20, 30};
  int anIdx        = 0;
  for (auto anIt = aConstList.begin(); anIt != aConstList.end(); ++anIt, ++anIdx)
  {
    EXPECT_EQ(anExpected[anIdx], *anIt);
  }
  EXPECT_EQ(3, anIdx);

  // Also test range-based for on const list
  anIdx = 0;
  for (const auto& aValue : aConstList)
  {
    EXPECT_EQ(anExpected[anIdx++], aValue);
  }
  EXPECT_EQ(3, anIdx);
}