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
#include <NCollection_Vector.hxx>
#include <Standard_Integer.hxx>

#include <gtest/gtest.h>
#include <algorithm>
#include <random>
#include <vector>

TEST(NCollection_VectorTest, DefaultConstructor)
{
  // Default constructor should create an empty vector
  NCollection_Vector<Standard_Integer> aVector;

  EXPECT_EQ(0, aVector.Length());
  EXPECT_TRUE(aVector.IsEmpty());
}

TEST(NCollection_VectorTest, ResizeConstructor)
{
  // Test constructor with initial size
  const Standard_Integer               initialSize  = 10;
  const Standard_Integer               initialValue = 42;
  NCollection_Vector<Standard_Integer> aVector(initialSize);

  // Initialize all elements to the same value
  for (Standard_Integer i = 0; i < initialSize; i++)
  {
    aVector.SetValue(i, initialValue);
  }

  EXPECT_EQ(initialSize, aVector.Length());
  EXPECT_FALSE(aVector.IsEmpty());

  // Check all values are initialized
  for (Standard_Integer i = 0; i < initialSize; i++)
  {
    EXPECT_EQ(initialValue, aVector(i));
  }
}

TEST(NCollection_VectorTest, Append)
{
  NCollection_Vector<Standard_Integer> aVector;

  // Test Append method
  aVector.Append(10);
  aVector.Append(20);
  aVector.Append(30);

  EXPECT_EQ(3, aVector.Length());
  EXPECT_EQ(10, aVector(0));
  EXPECT_EQ(20, aVector(1));
  EXPECT_EQ(30, aVector(2));
}

TEST(NCollection_VectorTest, SetValue)
{
  NCollection_Vector<Standard_Integer> aVector(5, 0);

  // Test SetValue method
  aVector.SetValue(2, 42);

  EXPECT_EQ(42, aVector(2));
  EXPECT_EQ(0, aVector(0));
  EXPECT_EQ(0, aVector(1));

  // Test operator()
  aVector(3) = 99;
  EXPECT_EQ(99, aVector(3));
}

TEST(NCollection_VectorTest, Value)
{
  NCollection_Vector<Standard_Integer> aVector;
  aVector.Append(10);
  aVector.Append(20);

  // Test Value and operator()
  EXPECT_EQ(10, aVector.Value(0));
  EXPECT_EQ(20, aVector.Value(1));

  EXPECT_EQ(aVector.Value(0), aVector(0));
  EXPECT_EQ(aVector.Value(1), aVector(1));
}

TEST(NCollection_VectorTest, ChangeValue)
{
  NCollection_Vector<Standard_Integer> aVector;
  aVector.Append(10);
  aVector.Append(20);

  // Test ChangeValue
  aVector.ChangeValue(1) = 25;
  EXPECT_EQ(25, aVector(1));

  // Equivalent using operator()
  aVector(0) = 15;
  EXPECT_EQ(15, aVector(0));
}

TEST(NCollection_VectorTest, FirstLast)
{
  NCollection_Vector<Standard_Integer> aVector;
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

TEST(NCollection_VectorTest, CopyConstructor)
{
  NCollection_Vector<Standard_Integer> aVector1;
  aVector1.Append(10);
  aVector1.Append(20);
  aVector1.Append(30);

  // Test copy constructor
  NCollection_Vector<Standard_Integer> aVector2(aVector1);

  EXPECT_EQ(aVector1.Length(), aVector2.Length());

  for (Standard_Integer i = 0; i < aVector1.Length(); i++)
  {
    EXPECT_EQ(aVector1(i), aVector2(i));
  }

  // Modify original to ensure deep copy
  aVector1(1) = 25;
  EXPECT_EQ(20, aVector2(1));
}

TEST(NCollection_VectorTest, AssignmentOperator)
{
  NCollection_Vector<Standard_Integer> aVector1;
  aVector1.Append(10);
  aVector1.Append(20);
  aVector1.Append(30);

  // Test assignment operator
  NCollection_Vector<Standard_Integer> aVector2;
  aVector2 = aVector1;

  EXPECT_EQ(aVector1.Length(), aVector2.Length());

  for (Standard_Integer i = 0; i < aVector1.Length(); i++)
  {
    EXPECT_EQ(aVector1(i), aVector2(i));
  }

  // Modify original to ensure deep copy
  aVector1(1) = 25;
  EXPECT_EQ(20, aVector2(1));
}

TEST(NCollection_VectorTest, Clear)
{
  NCollection_Vector<Standard_Integer> aVector;
  aVector.Append(10);
  aVector.Append(20);

  // Test Clear
  aVector.Clear();

  EXPECT_EQ(0, aVector.Length());
  EXPECT_TRUE(aVector.IsEmpty());
}

TEST(NCollection_VectorTest, Iterator)
{
  NCollection_Vector<Standard_Integer> aVector;
  aVector.Append(10);
  aVector.Append(20);
  aVector.Append(30);

  // Test iterator
  Standard_Integer sum = 0;
  for (NCollection_Vector<Standard_Integer>::Iterator it(aVector); it.More(); it.Next())
  {
    sum += it.Value();
  }

  EXPECT_EQ(60, sum);

  // Test modifying values through iterator
  for (NCollection_Vector<Standard_Integer>::Iterator it(aVector); it.More(); it.Next())
  {
    it.ChangeValue() *= 2;
  }

  EXPECT_EQ(20, aVector(0));
  EXPECT_EQ(40, aVector(1));
  EXPECT_EQ(60, aVector(2));
}

TEST(NCollection_VectorTest, STLIterators)
{
  NCollection_Vector<Standard_Integer> aVector;
  aVector.Append(10);
  aVector.Append(20);
  aVector.Append(30);

  // Test C++11 range-based for loop with STL-style iterators
  Standard_Integer sum = 0;
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

TEST(NCollection_VectorTest, Grow)
{
  NCollection_Vector<Standard_Integer> aVector;

  // Test automatic resize through many appends
  for (Standard_Integer i = 0; i < 1000; i++)
  {
    aVector.Append(i);
  }

  EXPECT_EQ(1000, aVector.Length());

  for (Standard_Integer i = 0; i < 1000; i++)
  {
    EXPECT_EQ(i, aVector(i));
  }
}

TEST(NCollection_VectorTest, Move)
{
  NCollection_Vector<Standard_Integer> aVector1;
  aVector1.Append(10);
  aVector1.Append(20);

  // Test Move constructor
  NCollection_Vector<Standard_Integer> aVector2 = std::move(aVector1);

  EXPECT_EQ(0, aVector1.Length()); // aVector1 should be empty after move
  EXPECT_EQ(2, aVector2.Length());
  EXPECT_EQ(10, aVector2(0));
  EXPECT_EQ(20, aVector2(1));

  // Test Move assignment
  NCollection_Vector<Standard_Integer> aVector3;
  aVector3.Append(30);
  aVector3 = std::move(aVector2);

  EXPECT_EQ(0, aVector2.Length()); // aVector2 should be empty after move
  EXPECT_EQ(2, aVector3.Length());
  EXPECT_EQ(10, aVector3(0));
  EXPECT_EQ(20, aVector3(1));
}

TEST(NCollection_VectorTest, EraseLast)
{
  NCollection_Vector<Standard_Integer> aVector;
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

TEST(NCollection_VectorTest, Appended)
{
  NCollection_Vector<Standard_Integer> aVector;

  // Test Appended method - returns reference to the appended element
  Standard_Integer& ref1 = aVector.Appended();
  ref1                   = 10;

  Standard_Integer& ref2 = aVector.Appended();
  ref2                   = 20;

  EXPECT_EQ(2, aVector.Length());
  EXPECT_EQ(10, aVector(0));
  EXPECT_EQ(20, aVector(1));

  // Modify through the reference
  ref1 = 15;
  EXPECT_EQ(15, aVector(0));
}

TEST(NCollection_VectorTest, CustomAllocator)
{
  // Test with custom allocator
  Handle(NCollection_BaseAllocator)    anAlloc = NCollection_BaseAllocator::CommonBaseAllocator();
  NCollection_Vector<Standard_Integer> aVector(256, anAlloc);

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

TEST(NCollection_VectorTest, SetIncrement)
{
  NCollection_Vector<Standard_Integer> aVector;

  // SetIncrement only works on empty vectors
  aVector.SetIncrement(512);

  // Fill the vector to test the custom increment size
  for (Standard_Integer i = 0; i < 1000; i++)
  {
    aVector.Append(i);
  }

  EXPECT_EQ(1000, aVector.Length());

  // Verify data integrity with the custom increment
  for (Standard_Integer i = 0; i < 1000; i++)
  {
    EXPECT_EQ(i, aVector(i));
  }
}

TEST(NCollection_VectorTest, STLAlgorithmCompatibility_MinMax)
{
  NCollection_Vector<Standard_Integer> aVector;
  std::vector<Standard_Integer>        aStdVector;

  std::mt19937 aGenerator(1); // Fixed seed for reproducible tests
  std::uniform_int_distribution<Standard_Integer> aDistribution(0, RAND_MAX);
  for (Standard_Integer anIdx = 0; anIdx < 100; ++anIdx)
  {
    Standard_Integer aVal = aDistribution(aGenerator);
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

TEST(NCollection_VectorTest, STLAlgorithmCompatibility_Replace)
{
  NCollection_Vector<Standard_Integer> aVector;
  std::vector<Standard_Integer>        aStdVector;

  std::mt19937 aGenerator(1); // Fixed seed for reproducible tests
  std::uniform_int_distribution<Standard_Integer> aDistribution(0, RAND_MAX);
  for (Standard_Integer anIdx = 0; anIdx < 100; ++anIdx)
  {
    Standard_Integer aVal = aDistribution(aGenerator);
    aVector.Append(aVal);
    aStdVector.push_back(aVal);
  }

  Standard_Integer aTargetValue = aStdVector.back();
  Standard_Integer aNewValue    = -1;

  std::replace(aVector.begin(), aVector.end(), aTargetValue, aNewValue);
  std::replace(aStdVector.begin(), aStdVector.end(), aTargetValue, aNewValue);

  EXPECT_TRUE(std::equal(aVector.begin(), aVector.end(), aStdVector.begin()));
}

TEST(NCollection_VectorTest, STLAlgorithmCompatibility_Reverse)
{
  NCollection_Vector<Standard_Integer> aVector;
  std::vector<Standard_Integer>        aStdVector;

  for (Standard_Integer anIdx = 0; anIdx < 100; ++anIdx)
  {
    aVector.Append(anIdx);
    aStdVector.push_back(anIdx);
  }

  std::reverse(aVector.begin(), aVector.end());
  std::reverse(aStdVector.begin(), aStdVector.end());

  EXPECT_TRUE(std::equal(aVector.begin(), aVector.end(), aStdVector.begin()));
}

TEST(NCollection_VectorTest, STLAlgorithmCompatibility_Sort)
{
  NCollection_Vector<Standard_Integer> aVector;
  std::vector<Standard_Integer>        aStdVector;

  std::mt19937 aGenerator(1); // Fixed seed for reproducible tests
  std::uniform_int_distribution<Standard_Integer> aDistribution(0, RAND_MAX);
  for (Standard_Integer anIdx = 0; anIdx < 100; ++anIdx)
  {
    Standard_Integer aVal = aDistribution(aGenerator);
    aVector.Append(aVal);
    aStdVector.push_back(aVal);
  }

  std::sort(aVector.begin(), aVector.end());
  std::sort(aStdVector.begin(), aStdVector.end());

  EXPECT_TRUE(std::equal(aVector.begin(), aVector.end(), aStdVector.begin()));
}