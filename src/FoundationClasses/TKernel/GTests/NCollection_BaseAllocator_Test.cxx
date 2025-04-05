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

#include <gtest/gtest.h>

// Simple struct to test allocations
struct TestStruct
{
  Standard_Integer   myValue1;
  Standard_Real      myValue2;
  Standard_Character myChar;

  TestStruct(Standard_Integer   theVal1 = 0,
             Standard_Real      theVal2 = 0.0,
             Standard_Character theChar = 'A')
      : myValue1(theVal1),
        myValue2(theVal2),
        myChar(theChar)
  {
  }

  bool operator==(const TestStruct& other) const
  {
    return myValue1 == other.myValue1 && fabs(myValue2 - other.myValue2) < 1e-10
           && myChar == other.myChar;
  }
};

TEST(NCollection_BaseAllocatorTest, DefaultInstance)
{
  // Get default allocator
  Handle(NCollection_BaseAllocator) aDefaultAlloc =
    NCollection_BaseAllocator::CommonBaseAllocator();

  // Ensure it's not null
  EXPECT_FALSE(aDefaultAlloc.IsNull());

  // Test that we get the same instance when requesting default allocator again
  Handle(NCollection_BaseAllocator) anotherDefaultAlloc =
    NCollection_BaseAllocator::CommonBaseAllocator();
  EXPECT_EQ(aDefaultAlloc, anotherDefaultAlloc);
}

TEST(NCollection_BaseAllocatorTest, Allocate)
{
  Handle(NCollection_BaseAllocator) anAlloc = NCollection_BaseAllocator::CommonBaseAllocator();

  // Test allocation of different sizes
  void* ptr1 = anAlloc->Allocate(10);
  EXPECT_NE(ptr1, nullptr);

  void* ptr2 = anAlloc->Allocate(100);
  EXPECT_NE(ptr2, nullptr);

  void* ptr3 = anAlloc->Allocate(1000);
  EXPECT_NE(ptr3, nullptr);

  // Allocations should return different pointers
  EXPECT_NE(ptr1, ptr2);
  EXPECT_NE(ptr1, ptr3);
  EXPECT_NE(ptr2, ptr3);

  // Free the allocated memory
  anAlloc->Free(ptr1);
  anAlloc->Free(ptr2);
  anAlloc->Free(ptr3);
}

TEST(NCollection_BaseAllocatorTest, AllocateStruct)
{
  Handle(NCollection_BaseAllocator) anAlloc = NCollection_BaseAllocator::CommonBaseAllocator();

  // Allocate and construct test struct
  TestStruct* pStruct = static_cast<TestStruct*>(anAlloc->Allocate(sizeof(TestStruct)));
  EXPECT_NE(pStruct, nullptr);

  // Use placement new to construct object at allocated memory
  new (pStruct) TestStruct(42, 3.14159, 'Z');

  // Verify object values
  EXPECT_EQ(pStruct->myValue1, 42);
  EXPECT_DOUBLE_EQ(pStruct->myValue2, 3.14159);
  EXPECT_EQ(pStruct->myChar, 'Z');

  // Destruct the object and free memory
  pStruct->~TestStruct();
  anAlloc->Free(pStruct);
}

TEST(NCollection_BaseAllocatorTest, AllocateArray)
{
  Handle(NCollection_BaseAllocator) anAlloc = NCollection_BaseAllocator::CommonBaseAllocator();

  const Standard_Integer arraySize = 5;

  // Allocate memory for an array of test structs
  TestStruct* pArray = static_cast<TestStruct*>(anAlloc->Allocate(arraySize * sizeof(TestStruct)));
  EXPECT_NE(pArray, nullptr);

  // Construct objects at allocated memory
  for (Standard_Integer i = 0; i < arraySize; ++i)
  {
    new (&pArray[i]) TestStruct(i, i * 1.5, static_cast<Standard_Character>('A' + i));
  }

  // Verify object values
  for (Standard_Integer i = 0; i < arraySize; ++i)
  {
    EXPECT_EQ(pArray[i].myValue1, i);
    EXPECT_DOUBLE_EQ(pArray[i].myValue2, i * 1.5);
    EXPECT_EQ(pArray[i].myChar, static_cast<Standard_Character>('A' + i));
  }

  // Destruct objects and free memory
  for (Standard_Integer i = 0; i < arraySize; ++i)
  {
    pArray[i].~TestStruct();
  }
  anAlloc->Free(pArray);
}

TEST(NCollection_BaseAllocatorTest, UsageWithVector)
{
  // Create a collection using the default allocator
  NCollection_Vector<TestStruct> aVector;

  // Add elements
  aVector.Append(TestStruct(10, 1.0, 'X'));
  aVector.Append(TestStruct(20, 2.0, 'Y'));
  aVector.Append(TestStruct(30, 3.0, 'Z'));

  // Verify elements were stored correctly
  EXPECT_EQ(aVector.Length(), 3);
  EXPECT_EQ(aVector(0).myValue1, 10);
  EXPECT_DOUBLE_EQ(aVector(0).myValue2, 1.0);
  EXPECT_EQ(aVector(0).myChar, 'X');

  EXPECT_EQ(aVector(1).myValue1, 20);
  EXPECT_DOUBLE_EQ(aVector(1).myValue2, 2.0);
  EXPECT_EQ(aVector(1).myChar, 'Y');

  EXPECT_EQ(aVector(2).myValue1, 30);
  EXPECT_DOUBLE_EQ(aVector(2).myValue2, 3.0);
  EXPECT_EQ(aVector(2).myChar, 'Z');

  // Create a custom allocator
  Handle(NCollection_BaseAllocator) aCustomAlloc = NCollection_BaseAllocator::CommonBaseAllocator();

  // Create a collection using custom allocator
  NCollection_Vector<TestStruct> aVectorWithCustomAlloc(5, aCustomAlloc);

  // Add elements
  aVectorWithCustomAlloc.Append(TestStruct(40, 4.0, 'P'));
  aVectorWithCustomAlloc.Append(TestStruct(50, 5.0, 'Q'));

  // Verify elements were stored correctly
  EXPECT_EQ(aVectorWithCustomAlloc.Length(), 2);
  EXPECT_EQ(aVectorWithCustomAlloc(0).myValue1, 40);
  EXPECT_DOUBLE_EQ(aVectorWithCustomAlloc(0).myValue2, 4.0);
  EXPECT_EQ(aVectorWithCustomAlloc(0).myChar, 'P');

  EXPECT_EQ(aVectorWithCustomAlloc(1).myValue1, 50);
  EXPECT_DOUBLE_EQ(aVectorWithCustomAlloc(1).myValue2, 5.0);
  EXPECT_EQ(aVectorWithCustomAlloc(1).myChar, 'Q');
}

TEST(NCollection_BaseAllocatorTest, CopyAndMove)
{
  Handle(NCollection_BaseAllocator) anAlloc1 = NCollection_BaseAllocator::CommonBaseAllocator();

  // Create a collection with allocator
  NCollection_Vector<TestStruct> aVector1(5, anAlloc1);
  aVector1.Append(TestStruct(10, 1.0, 'A'));
  aVector1.Append(TestStruct(20, 2.0, 'B'));

  // Copy constructor should preserve the allocator
  NCollection_Vector<TestStruct> aVector2(aVector1);
  EXPECT_EQ(aVector2.Length(), 2);
  EXPECT_EQ(aVector2(0), TestStruct(10, 1.0, 'A'));
  EXPECT_EQ(aVector2(1), TestStruct(20, 2.0, 'B'));

  // Create a new collection with new allocator
  Handle(NCollection_BaseAllocator) anAlloc2 = NCollection_BaseAllocator::CommonBaseAllocator();
  NCollection_Vector<TestStruct>    aVector3(5, anAlloc2);

  // Assignment operator should preserve the destination's allocator
  aVector3 = aVector1;
  EXPECT_EQ(aVector3.Length(), 2);
  EXPECT_EQ(aVector3(0), TestStruct(10, 1.0, 'A'));
  EXPECT_EQ(aVector3(1), TestStruct(20, 2.0, 'B'));
}

TEST(NCollection_BaseAllocatorTest, BigAllocation)
{
  Handle(NCollection_BaseAllocator) anAlloc = NCollection_BaseAllocator::CommonBaseAllocator();

  // Test a large allocation
  const size_t largeSize = 1024 * 1024; // 1MB
  void*        pLarge    = anAlloc->Allocate(largeSize);
  EXPECT_NE(pLarge, nullptr);

  // Write to the allocated memory to verify it's usable
  memset(pLarge, 0xAB, largeSize);

  // Free the allocated memory
  anAlloc->Free(pLarge);
}