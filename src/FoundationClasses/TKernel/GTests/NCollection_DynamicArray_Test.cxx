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

#include <NCollection_BaseAllocator.hxx>
#include <NCollection_DynamicArray.hxx>
#include <Standard_Integer.hxx>

#include <gtest/gtest.h>
#include <algorithm>
#include <random>
#include <vector>

TEST(NCollection_DynamicArrayTest, DefaultConstructor)
{
  // Default constructor should create an empty vector
  NCollection_DynamicArray<int> aVector;

  EXPECT_EQ(0, aVector.Length());
  EXPECT_TRUE(aVector.IsEmpty());
}

TEST(NCollection_DynamicArrayTest, ResizeConstructor)
{
  // Test constructor with initial size
  const int                     initialSize  = 10;
  const int                     initialValue = 42;
  NCollection_DynamicArray<int> aVector(initialSize);

  // Initialize all elements to the same value
  for (int i = 0; i < initialSize; i++)
  {
    aVector.SetValue(i, initialValue);
  }

  EXPECT_EQ(initialSize, aVector.Length());
  EXPECT_FALSE(aVector.IsEmpty());

  // Check all values are initialized
  for (int i = 0; i < initialSize; i++)
  {
    EXPECT_EQ(initialValue, aVector(i));
  }
}

TEST(NCollection_DynamicArrayTest, Append)
{
  NCollection_DynamicArray<int> aVector;

  // Test Append method
  aVector.Append(10);
  aVector.Append(20);
  aVector.Append(30);

  EXPECT_EQ(3, aVector.Length());
  EXPECT_EQ(10, aVector(0));
  EXPECT_EQ(20, aVector(1));
  EXPECT_EQ(30, aVector(2));
}

TEST(NCollection_DynamicArrayTest, SetValue)
{
  NCollection_DynamicArray<int> aVector(5, nullptr);

  // Test SetValue method
  aVector.SetValue(2, 42);

  EXPECT_EQ(42, aVector(2));
  EXPECT_EQ(0, aVector(0));
  EXPECT_EQ(0, aVector(1));

  // Test operator()
  aVector(3) = 99;
  EXPECT_EQ(99, aVector(3));
}

TEST(NCollection_DynamicArrayTest, Value)
{
  NCollection_DynamicArray<int> aVector;
  aVector.Append(10);
  aVector.Append(20);

  // Test Value and operator()
  EXPECT_EQ(10, aVector.Value(0));
  EXPECT_EQ(20, aVector.Value(1));

  EXPECT_EQ(aVector.Value(0), aVector(0));
  EXPECT_EQ(aVector.Value(1), aVector(1));
}

TEST(NCollection_DynamicArrayTest, ChangeValue)
{
  NCollection_DynamicArray<int> aVector;
  aVector.Append(10);
  aVector.Append(20);

  // Test ChangeValue
  aVector.ChangeValue(1) = 25;
  EXPECT_EQ(25, aVector(1));

  // Equivalent using operator()
  aVector(0) = 15;
  EXPECT_EQ(15, aVector(0));
}

TEST(NCollection_DynamicArrayTest, FirstLast)
{
  NCollection_DynamicArray<int> aVector;
  aVector.Append(10);
  aVector.Append(20);
  aVector.Append(30);

  // Test First and Last
  EXPECT_EQ(10, aVector.First());
  EXPECT_EQ(30, aVector.Last());

  // Test ChangeFirst and ChangeLast
  aVector.ChangeFirst() = 15;
  aVector.ChangeLast()  = 35;

  EXPECT_EQ(15, aVector.First());
  EXPECT_EQ(35, aVector.Last());
}

TEST(NCollection_DynamicArrayTest, CopyConstructor)
{
  NCollection_DynamicArray<int> aVector1;
  aVector1.Append(10);
  aVector1.Append(20);
  aVector1.Append(30);

  // Test copy constructor
  NCollection_DynamicArray<int> aVector2(aVector1);

  EXPECT_EQ(aVector1.Length(), aVector2.Length());

  for (int i = 0; i < aVector1.Length(); i++)
  {
    EXPECT_EQ(aVector1(i), aVector2(i));
  }

  // Modify original to ensure deep copy
  aVector1(1) = 25;
  EXPECT_EQ(20, aVector2(1));
}

TEST(NCollection_DynamicArrayTest, AssignmentOperator)
{
  NCollection_DynamicArray<int> aVector1;
  aVector1.Append(10);
  aVector1.Append(20);
  aVector1.Append(30);

  // Test assignment operator
  NCollection_DynamicArray<int> aVector2;
  aVector2 = aVector1;

  EXPECT_EQ(aVector1.Length(), aVector2.Length());

  for (int i = 0; i < aVector1.Length(); i++)
  {
    EXPECT_EQ(aVector1(i), aVector2(i));
  }

  // Modify original to ensure deep copy
  aVector1(1) = 25;
  EXPECT_EQ(20, aVector2(1));
}

TEST(NCollection_DynamicArrayTest, Clear)
{
  NCollection_DynamicArray<int> aVector;
  aVector.Append(10);
  aVector.Append(20);

  // Test Clear
  aVector.Clear();

  EXPECT_EQ(0, aVector.Length());
  EXPECT_TRUE(aVector.IsEmpty());
}

TEST(NCollection_DynamicArrayTest, Iterator)
{
  NCollection_DynamicArray<int> aVector;
  aVector.Append(10);
  aVector.Append(20);
  aVector.Append(30);

  // Test iterator
  int sum = 0;
  for (NCollection_DynamicArray<int>::Iterator it(aVector); it.More(); it.Next())
  {
    sum += it.Value();
  }

  EXPECT_EQ(60, sum);

  // Test modifying values through iterator
  for (NCollection_DynamicArray<int>::Iterator it(aVector); it.More(); it.Next())
  {
    it.ChangeValue() *= 2;
  }

  EXPECT_EQ(20, aVector(0));
  EXPECT_EQ(40, aVector(1));
  EXPECT_EQ(60, aVector(2));
}

TEST(NCollection_DynamicArrayTest, STLIterators)
{
  NCollection_DynamicArray<int> aVector;
  aVector.Append(10);
  aVector.Append(20);
  aVector.Append(30);

  // Test C++11 range-based for loop with STL-style iterators
  int sum = 0;
  for (const auto& val : aVector)
  {
    sum += val;
  }

  EXPECT_EQ(60, sum);

  // Test modification through iterator
  sum = 0;
  for (auto& val : aVector)
  {
    val *= 2; // Double each value
    sum += val;
  }

  EXPECT_EQ(120, sum);

  // Verify the modifications
  EXPECT_EQ(20, aVector(0));
  EXPECT_EQ(40, aVector(1));
  EXPECT_EQ(60, aVector(2));
}

TEST(NCollection_DynamicArrayTest, Grow)
{
  NCollection_DynamicArray<int> aVector;

  // Test automatic resize through many appends
  for (int i = 0; i < 1000; i++)
  {
    aVector.Append(i);
  }

  EXPECT_EQ(1000, aVector.Length());

  for (int i = 0; i < 1000; i++)
  {
    EXPECT_EQ(i, aVector(i));
  }
}

TEST(NCollection_DynamicArrayTest, Move)
{
  NCollection_DynamicArray<int> aVector1;
  aVector1.Append(10);
  aVector1.Append(20);

  // Test Move constructor
  NCollection_DynamicArray<int> aVector2 = std::move(aVector1);

  EXPECT_EQ(0, aVector1.Length()); // aVector1 should be empty after move
  EXPECT_EQ(2, aVector2.Length());
  EXPECT_EQ(10, aVector2(0));
  EXPECT_EQ(20, aVector2(1));

  // Test Move assignment
  NCollection_DynamicArray<int> aVector3;
  aVector3.Append(30);
  aVector3 = std::move(aVector2);

  EXPECT_EQ(0, aVector2.Length()); // aVector2 should be empty after move
  EXPECT_EQ(2, aVector3.Length());
  EXPECT_EQ(10, aVector3(0));
  EXPECT_EQ(20, aVector3(1));
}

TEST(NCollection_DynamicArrayTest, EraseLast)
{
  NCollection_DynamicArray<int> aVector;
  aVector.Append(10);
  aVector.Append(20);
  aVector.Append(30);

  EXPECT_EQ(3, aVector.Length());

  // Test EraseLast method
  aVector.EraseLast();
  EXPECT_EQ(2, aVector.Length());
  EXPECT_EQ(10, aVector(0));
  EXPECT_EQ(20, aVector(1));

  // Remove another element
  aVector.EraseLast();
  EXPECT_EQ(1, aVector.Length());
  EXPECT_EQ(10, aVector(0));

  // Remove the last element
  aVector.EraseLast();
  EXPECT_EQ(0, aVector.Length());
  EXPECT_TRUE(aVector.IsEmpty());

  // Calling EraseLast on an empty vector should not cause errors
  aVector.EraseLast();
  EXPECT_EQ(0, aVector.Length());
}

TEST(NCollection_DynamicArrayTest, Appended)
{
  NCollection_DynamicArray<int> aVector;

  // Test Appended method - returns reference to the appended element
  int& ref1 = aVector.Appended();
  ref1      = 10;

  int& ref2 = aVector.Appended();
  ref2      = 20;

  EXPECT_EQ(2, aVector.Length());
  EXPECT_EQ(10, aVector(0));
  EXPECT_EQ(20, aVector(1));

  // Modify through the reference
  ref1 = 15;
  EXPECT_EQ(15, aVector(0));
}

TEST(NCollection_DynamicArrayTest, CustomAllocator)
{
  // Test with custom allocator
  occ::handle<NCollection_BaseAllocator> anAlloc = NCollection_BaseAllocator::CommonBaseAllocator();
  NCollection_DynamicArray<int>          aVector(256, anAlloc);

  // Verify vector works with custom allocator
  aVector.Append(10);
  aVector.Append(20);
  aVector.Append(30);

  EXPECT_EQ(3, aVector.Length());
  EXPECT_EQ(10, aVector(0));
  EXPECT_EQ(20, aVector(1));
  EXPECT_EQ(30, aVector(2));

  // Test clear with custom allocator
  aVector.Clear();
  EXPECT_EQ(0, aVector.Length());
}

TEST(NCollection_DynamicArrayTest, SetIncrement)
{
  NCollection_DynamicArray<int> aVector;

  // SetIncrement only works on empty vectors
  aVector.SetIncrement(512);

  // Fill the vector to test the custom increment size
  for (int i = 0; i < 1000; i++)
  {
    aVector.Append(i);
  }

  EXPECT_EQ(1000, aVector.Length());

  // Verify data integrity with the custom increment
  for (int i = 0; i < 1000; i++)
  {
    EXPECT_EQ(i, aVector(i));
  }
}

TEST(NCollection_DynamicArrayTest, STLAlgorithmCompatibility_MinMax)
{
  NCollection_DynamicArray<int> aVector;
  std::vector<int>              aStdVector;

  std::mt19937                       aGenerator(1); // Fixed seed for reproducible tests
  std::uniform_int_distribution<int> aDistribution(0, RAND_MAX);
  for (int anIdx = 0; anIdx < 100; ++anIdx)
  {
    int aVal = aDistribution(aGenerator);
    aVector.Append(aVal);
    aStdVector.push_back(aVal);
  }

  auto aMinOCCT = std::min_element(aVector.begin(), aVector.end());
  auto aMinStd  = std::min_element(aStdVector.begin(), aStdVector.end());

  auto aMaxOCCT = std::max_element(aVector.begin(), aVector.end());
  auto aMaxStd  = std::max_element(aStdVector.begin(), aStdVector.end());

  EXPECT_EQ(*aMinOCCT, *aMinStd);
  EXPECT_EQ(*aMaxOCCT, *aMaxStd);
}

TEST(NCollection_DynamicArrayTest, STLAlgorithmCompatibility_Replace)
{
  NCollection_DynamicArray<int> aVector;
  std::vector<int>              aStdVector;

  std::mt19937                       aGenerator(1); // Fixed seed for reproducible tests
  std::uniform_int_distribution<int> aDistribution(0, RAND_MAX);
  for (int anIdx = 0; anIdx < 100; ++anIdx)
  {
    int aVal = aDistribution(aGenerator);
    aVector.Append(aVal);
    aStdVector.push_back(aVal);
  }

  int aTargetValue = aStdVector.back();
  int aNewValue    = -1;

  std::replace(aVector.begin(), aVector.end(), aTargetValue, aNewValue);
  std::replace(aStdVector.begin(), aStdVector.end(), aTargetValue, aNewValue);

  EXPECT_TRUE(std::equal(aVector.begin(), aVector.end(), aStdVector.begin()));
}

TEST(NCollection_DynamicArrayTest, STLAlgorithmCompatibility_Reverse)
{
  NCollection_DynamicArray<int> aVector;
  std::vector<int>              aStdVector;

  for (int anIdx = 0; anIdx < 100; ++anIdx)
  {
    aVector.Append(anIdx);
    aStdVector.push_back(anIdx);
  }

  std::reverse(aVector.begin(), aVector.end());
  std::reverse(aStdVector.begin(), aStdVector.end());

  EXPECT_TRUE(std::equal(aVector.begin(), aVector.end(), aStdVector.begin()));
}

TEST(NCollection_DynamicArrayTest, STLAlgorithmCompatibility_Sort)
{
  NCollection_DynamicArray<int> aVector;
  std::vector<int>              aStdVector;

  std::mt19937                       aGenerator(1); // Fixed seed for reproducible tests
  std::uniform_int_distribution<int> aDistribution(0, RAND_MAX);
  for (int anIdx = 0; anIdx < 100; ++anIdx)
  {
    int aVal = aDistribution(aGenerator);
    aVector.Append(aVal);
    aStdVector.push_back(aVal);
  }

  std::sort(aVector.begin(), aVector.end());
  std::sort(aStdVector.begin(), aStdVector.end());

  EXPECT_TRUE(std::equal(aVector.begin(), aVector.end(), aStdVector.begin()));
}

// Helper struct for testing in-place construction with multiple arguments
struct VecMultiArgType
{
  int    myA;
  double myB;

  VecMultiArgType()
      : myA(0),
        myB(0.0)
  {
  }

  VecMultiArgType(int theA, double theB)
      : myA(theA),
        myB(theB)
  {
  }
};

// Helper struct for testing move-only types
struct VecMoveOnlyType
{
  int myValue;

  VecMoveOnlyType()
      : myValue(0)
  {
  }

  explicit VecMoveOnlyType(int theValue)
      : myValue(theValue)
  {
  }

  VecMoveOnlyType(VecMoveOnlyType&& theOther) noexcept
      : myValue(theOther.myValue)
  {
    theOther.myValue = 0;
  }

  VecMoveOnlyType& operator=(VecMoveOnlyType&& theOther) noexcept
  {
    myValue          = theOther.myValue;
    theOther.myValue = 0;
    return *this;
  }

  VecMoveOnlyType(const VecMoveOnlyType&)            = delete;
  VecMoveOnlyType& operator=(const VecMoveOnlyType&) = delete;
};

TEST(NCollection_DynamicArrayTest, EmplaceAppend)
{
  NCollection_DynamicArray<VecMultiArgType> aVector;

  // Test EmplaceAppend with multiple constructor arguments
  VecMultiArgType& aRef1 = aVector.EmplaceAppend(42, 3.14);
  EXPECT_EQ(42, aRef1.myA);
  EXPECT_NEAR(3.14, aRef1.myB, 1e-10);
  EXPECT_EQ(1, aVector.Length());

  VecMultiArgType& aRef2 = aVector.EmplaceAppend(100, 2.71);
  EXPECT_EQ(100, aRef2.myA);
  EXPECT_NEAR(2.71, aRef2.myB, 1e-10);
  EXPECT_EQ(2, aVector.Length());

  // Verify the order (0-based indexing)
  EXPECT_EQ(42, aVector(0).myA);
  EXPECT_EQ(100, aVector(1).myA);
}

TEST(NCollection_DynamicArrayTest, EmplaceValue)
{
  NCollection_DynamicArray<VecMultiArgType> aVector;

  // Test EmplaceValue at index 0
  VecMultiArgType& aRef1 = aVector.EmplaceValue(0, 10, 1.0);
  EXPECT_EQ(10, aRef1.myA);
  EXPECT_NEAR(1.0, aRef1.myB, 1e-10);
  EXPECT_EQ(1, aVector.Length());

  // Test EmplaceValue at index beyond current size (should fill with default values)
  VecMultiArgType& aRef2 = aVector.EmplaceValue(3, 40, 4.0);
  EXPECT_EQ(40, aRef2.myA);
  EXPECT_NEAR(4.0, aRef2.myB, 1e-10);
  EXPECT_EQ(4, aVector.Length());

  // Check that intermediate elements were default-constructed
  EXPECT_EQ(0, aVector(1).myA);
  EXPECT_NEAR(0.0, aVector(1).myB, 1e-10);
  EXPECT_EQ(0, aVector(2).myA);
  EXPECT_NEAR(0.0, aVector(2).myB, 1e-10);
}

TEST(NCollection_DynamicArrayTest, EmplaceValue_ReplacesExisting)
{
  NCollection_DynamicArray<VecMultiArgType> aVector;

  // Set initial values
  aVector.EmplaceValue(0, 10, 1.0);
  aVector.EmplaceValue(1, 20, 2.0);
  aVector.EmplaceValue(2, 30, 3.0);

  EXPECT_EQ(3, aVector.Length());

  // Replace value at index 1 (existing element)
  VecMultiArgType& aRef = aVector.EmplaceValue(1, 200, 20.0);
  EXPECT_EQ(200, aRef.myA);
  EXPECT_NEAR(20.0, aRef.myB, 1e-10);

  // Verify other values unchanged and size unchanged
  EXPECT_EQ(3, aVector.Length());
  EXPECT_EQ(10, aVector(0).myA);
  EXPECT_EQ(200, aVector(1).myA);
  EXPECT_EQ(30, aVector(2).myA);
}

TEST(NCollection_DynamicArrayTest, EmplaceWithMoveOnlyType)
{
  NCollection_DynamicArray<VecMoveOnlyType> aVector;

  // Test EmplaceAppend with move-only type
  VecMoveOnlyType& aRef1 = aVector.EmplaceAppend(42);
  EXPECT_EQ(42, aRef1.myValue);

  VecMoveOnlyType& aRef2 = aVector.EmplaceAppend(100);
  EXPECT_EQ(100, aRef2.myValue);

  EXPECT_EQ(2, aVector.Length());
  EXPECT_EQ(42, aVector(0).myValue);
  EXPECT_EQ(100, aVector(1).myValue);
}

TEST(NCollection_DynamicArrayTest, EmplaceAppendMany)
{
  NCollection_DynamicArray<VecMultiArgType> aVector;

  // Test EmplaceAppend with many elements to trigger internal array expansion
  for (int i = 0; i < 1000; i++)
  {
    VecMultiArgType& aRef = aVector.EmplaceAppend(i, static_cast<double>(i) * 0.1);
    EXPECT_EQ(i, aRef.myA);
    EXPECT_NEAR(static_cast<double>(i) * 0.1, aRef.myB, 1e-10);
  }

  EXPECT_EQ(1000, aVector.Length());

  // Verify all values
  for (int i = 0; i < 1000; i++)
  {
    EXPECT_EQ(i, aVector(i).myA);
    EXPECT_NEAR(static_cast<double>(i) * 0.1, aVector(i).myB, 1e-10);
  }
}

TEST(NCollection_DynamicArrayTest, SetValue_ReplacesExisting)
{
  NCollection_DynamicArray<VecMultiArgType> aVector;

  // Set initial values
  aVector.SetValue(0, VecMultiArgType(10, 1.0));
  aVector.SetValue(1, VecMultiArgType(20, 2.0));
  aVector.SetValue(2, VecMultiArgType(30, 3.0));

  EXPECT_EQ(3, aVector.Length());

  // Replace value at index 1 (existing element)
  VecMultiArgType& aRef = aVector.SetValue(1, VecMultiArgType(200, 20.0));
  EXPECT_EQ(200, aRef.myA);
  EXPECT_NEAR(20.0, aRef.myB, 1e-10);

  // Verify other values unchanged and size unchanged
  EXPECT_EQ(3, aVector.Length());
  EXPECT_EQ(10, aVector(0).myA);
  EXPECT_EQ(200, aVector(1).myA);
  EXPECT_EQ(30, aVector(2).myA);
}

TEST(NCollection_DynamicArrayTest, InsertAfter_Middle)
{
  NCollection_DynamicArray<int> aVector;
  aVector.Append(10);
  aVector.Append(20);
  aVector.Append(30);
  aVector.Append(40);

  // Insert 99 after index 1 (after 20): [10, 20, 99, 30, 40]
  aVector.InsertAfter(1, 99);

  EXPECT_EQ(5, aVector.Length());
  EXPECT_EQ(10, aVector(0));
  EXPECT_EQ(20, aVector(1));
  EXPECT_EQ(99, aVector(2));
  EXPECT_EQ(30, aVector(3));
  EXPECT_EQ(40, aVector(4));
}

TEST(NCollection_DynamicArrayTest, InsertAfter_First)
{
  NCollection_DynamicArray<int> aVector;
  aVector.Append(10);
  aVector.Append(20);
  aVector.Append(30);

  // Insert 99 after index 0 (after 10): [10, 99, 20, 30]
  aVector.InsertAfter(0, 99);

  EXPECT_EQ(4, aVector.Length());
  EXPECT_EQ(10, aVector(0));
  EXPECT_EQ(99, aVector(1));
  EXPECT_EQ(20, aVector(2));
  EXPECT_EQ(30, aVector(3));
}

TEST(NCollection_DynamicArrayTest, InsertAfter_Last)
{
  NCollection_DynamicArray<int> aVector;
  aVector.Append(10);
  aVector.Append(20);
  aVector.Append(30);

  // Insert 99 after last index (after 30): [10, 20, 30, 99]
  aVector.InsertAfter(2, 99);

  EXPECT_EQ(4, aVector.Length());
  EXPECT_EQ(10, aVector(0));
  EXPECT_EQ(20, aVector(1));
  EXPECT_EQ(30, aVector(2));
  EXPECT_EQ(99, aVector(3));
}

TEST(NCollection_DynamicArrayTest, InsertAfter_SingleElement)
{
  NCollection_DynamicArray<int> aVector;
  aVector.Append(10);

  // Insert 99 after index 0: [10, 99]
  aVector.InsertAfter(0, 99);

  EXPECT_EQ(2, aVector.Length());
  EXPECT_EQ(10, aVector(0));
  EXPECT_EQ(99, aVector(1));
}

TEST(NCollection_DynamicArrayTest, InsertBefore_Middle)
{
  NCollection_DynamicArray<int> aVector;
  aVector.Append(10);
  aVector.Append(20);
  aVector.Append(30);
  aVector.Append(40);

  // Insert 99 before index 2 (before 30): [10, 20, 99, 30, 40]
  aVector.InsertBefore(2, 99);

  EXPECT_EQ(5, aVector.Length());
  EXPECT_EQ(10, aVector(0));
  EXPECT_EQ(20, aVector(1));
  EXPECT_EQ(99, aVector(2));
  EXPECT_EQ(30, aVector(3));
  EXPECT_EQ(40, aVector(4));
}

TEST(NCollection_DynamicArrayTest, InsertBefore_First)
{
  NCollection_DynamicArray<int> aVector;
  aVector.Append(10);
  aVector.Append(20);
  aVector.Append(30);

  // Insert 99 before index 0: [99, 10, 20, 30]
  aVector.InsertBefore(0, 99);

  EXPECT_EQ(4, aVector.Length());
  EXPECT_EQ(99, aVector(0));
  EXPECT_EQ(10, aVector(1));
  EXPECT_EQ(20, aVector(2));
  EXPECT_EQ(30, aVector(3));
}

TEST(NCollection_DynamicArrayTest, InsertBefore_Last)
{
  NCollection_DynamicArray<int> aVector;
  aVector.Append(10);
  aVector.Append(20);
  aVector.Append(30);

  // Insert 99 before last index (before 30): [10, 20, 99, 30]
  aVector.InsertBefore(2, 99);

  EXPECT_EQ(4, aVector.Length());
  EXPECT_EQ(10, aVector(0));
  EXPECT_EQ(20, aVector(1));
  EXPECT_EQ(99, aVector(2));
  EXPECT_EQ(30, aVector(3));
}

TEST(NCollection_DynamicArrayTest, InsertAfter_MoveSemantics)
{
  NCollection_DynamicArray<VecMoveOnlyType> aVector;
  aVector.EmplaceAppend(10);
  aVector.EmplaceAppend(20);
  aVector.EmplaceAppend(30);

  // Move-insert 99 after index 1: [10, 20, 99, 30]
  aVector.InsertAfter(1, VecMoveOnlyType(99));

  EXPECT_EQ(4, aVector.Length());
  EXPECT_EQ(10, aVector(0).myValue);
  EXPECT_EQ(20, aVector(1).myValue);
  EXPECT_EQ(99, aVector(2).myValue);
  EXPECT_EQ(30, aVector(3).myValue);
}

TEST(NCollection_DynamicArrayTest, InsertAfter_MultipleInserts)
{
  NCollection_DynamicArray<int> aVector;
  aVector.Append(10);
  aVector.Append(40);

  // Insert 20 after 10: [10, 20, 40]
  aVector.InsertAfter(0, 20);
  // Insert 30 after 20: [10, 20, 30, 40]
  aVector.InsertAfter(1, 30);

  EXPECT_EQ(4, aVector.Length());
  EXPECT_EQ(10, aVector(0));
  EXPECT_EQ(20, aVector(1));
  EXPECT_EQ(30, aVector(2));
  EXPECT_EQ(40, aVector(3));
}

TEST(NCollection_DynamicArrayTest, SizeReturnsSizeT)
{
  NCollection_DynamicArray<int> aVec;
  static_assert(std::is_same_v<decltype(aVec.Size()), size_t>, "Size() must return size_t");
  EXPECT_EQ(size_t(0), aVec.Size());

  aVec.Append(42);
  aVec.Append(43);
  EXPECT_EQ(size_t(2), aVec.Size());
}

TEST(NCollection_DynamicArrayTest, LengthStillReturnsInt)
{
  NCollection_DynamicArray<int> aVec;
  static_assert(std::is_same_v<decltype(aVec.Length()), int>,
                "Length() must stay int for backward compatibility");
  EXPECT_EQ(0, aVec.Length());
}

TEST(NCollection_DynamicArrayTest, UpperReturnsMinusOneForEmpty)
{
  NCollection_DynamicArray<int> aVec;
  EXPECT_EQ(0, aVec.Lower());
  EXPECT_EQ(-1, aVec.Upper());

  aVec.Append(10);
  EXPECT_EQ(0, aVec.Upper());
}

TEST(NCollection_DynamicArrayTest, SizeTConstructor)
{
  NCollection_DynamicArray<int> aVec(size_t(128));
  aVec.Append(1);
  EXPECT_EQ(size_t(1), aVec.Size());
}

TEST(NCollection_DynamicArrayTest, SizeTValueAccess)
{
  NCollection_DynamicArray<int> aVec;
  for (int i = 0; i < 10; ++i)
  {
    aVec.Append(i * 7);
  }
  const size_t aTwo = 2;
  EXPECT_EQ(14, aVec.Value(aTwo));
  EXPECT_EQ(14, aVec[aTwo]);
  EXPECT_EQ(14, aVec(aTwo));
}

TEST(NCollection_DynamicArrayTest, SizeTSetValueExtends)
{
  NCollection_DynamicArray<int> aVec;
  aVec.SetValue(size_t(5), 99);
  EXPECT_EQ(size_t(6), aVec.Size());
  EXPECT_EQ(99, aVec.Value(size_t(5)));
}

TEST(NCollection_DynamicArrayTest, SizeTEmplaceValue)
{
  NCollection_DynamicArray<std::pair<int, double>> aVec;
  aVec.EmplaceValue(size_t(3), 1, 2.5);
  EXPECT_EQ(size_t(4), aVec.Size());
  EXPECT_EQ(1, aVec[size_t(3)].first);
  EXPECT_DOUBLE_EQ(2.5, aVec[size_t(3)].second);
}

TEST(NCollection_DynamicArrayTest, SizeTInsertBeforeAfter)
{
  NCollection_DynamicArray<int> aVec;
  for (int i = 0; i < 3; ++i)
  {
    aVec.Append(i); // 0, 1, 2
  }
  aVec.InsertBefore(size_t(1), 99); // 0, 99, 1, 2
  EXPECT_EQ(size_t(4), aVec.Size());
  EXPECT_EQ(99, aVec[size_t(1)]);
  EXPECT_EQ(1, aVec[size_t(2)]);

  aVec.InsertAfter(size_t(0), 42); // 0, 42, 99, 1, 2
  EXPECT_EQ(size_t(5), aVec.Size());
  EXPECT_EQ(42, aVec[size_t(1)]);
}

#ifndef No_Exception
TEST(NCollection_DynamicArrayTest, IntOverloadGuardsNegative)
{
  NCollection_DynamicArray<int> aVec;
  aVec.Append(1);

  EXPECT_THROW((void)aVec.SetValue(-1, 99), Standard_OutOfRange);
  EXPECT_THROW((void)aVec.InsertBefore(-1, 99), Standard_OutOfRange);
  EXPECT_THROW((void)aVec.InsertAfter(-1, 99), Standard_OutOfRange);
  EXPECT_THROW((void)aVec.EmplaceValue(-1, 99), Standard_OutOfRange);
}
#endif

TEST(NCollection_DynamicArrayTest, IntAndSizeTOverloadsAgree)
{
  NCollection_DynamicArray<int> aVecInt;
  NCollection_DynamicArray<int> aVecSize;
  for (int i = 0; i < 20; ++i)
  {
    aVecInt.SetValue(i, i * 3);
    aVecSize.SetValue(static_cast<size_t>(i), i * 3);
  }
  ASSERT_EQ(aVecInt.Size(), aVecSize.Size());
  for (int i = 0; i < 20; ++i)
  {
    EXPECT_EQ(aVecInt.Value(i), aVecSize.Value(static_cast<size_t>(i)));
  }
}