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

#include <NCollection_LocalArray.hxx>

#include <gtest/gtest.h>

// Test default constructor and initial state
TEST(NCollection_LocalArrayTest, DefaultConstructor)
{
  NCollection_LocalArray<int> array;
  EXPECT_EQ(0, array.Size());
}

// Test constructor with size
TEST(NCollection_LocalArrayTest, ConstructorWithSize)
{
  const size_t                size = 100;
  NCollection_LocalArray<int> array(size);
  EXPECT_EQ(size, array.Size());
}

// Test allocation with small size (using buffer)
TEST(NCollection_LocalArrayTest, SmallSizeAllocation)
{
  const size_t                      size = 10; // Less than MAX_ARRAY_SIZE
  NCollection_LocalArray<int, 1024> array(size);
  EXPECT_EQ(size, array.Size());

  // Populate array
  for (size_t i = 0; i < size; ++i)
  {
    array[i] = static_cast<int>(i * 10);
  }

  // Verify array contents
  for (size_t i = 0; i < size; ++i)
  {
    EXPECT_EQ(static_cast<int>(i * 10), array[i]);
  }
}

// Test allocation with large size (heap allocation)
TEST(NCollection_LocalArrayTest, LargeSizeAllocation)
{
  const size_t                size = 2000; // Greater than default MAX_ARRAY_SIZE of 1024
  NCollection_LocalArray<int> array(size);
  EXPECT_EQ(size, array.Size());

  // Populate array
  for (size_t i = 0; i < size; ++i)
  {
    array[i] = static_cast<int>(i * 10);
  }

  // Verify array contents
  for (size_t i = 0; i < size; ++i)
  {
    EXPECT_EQ(static_cast<int>(i * 10), array[i]);
  }
}

// Test reallocation
TEST(NCollection_LocalArrayTest, Reallocation)
{
  NCollection_LocalArray<int> array(10);
  EXPECT_EQ(10, array.Size());

  // Populate array
  for (size_t i = 0; i < 10; ++i)
  {
    array[i] = static_cast<int>(i);
  }

  // Reallocate to larger size
  array.Allocate(50);
  EXPECT_EQ(50, array.Size());

  // Populate new elements
  for (size_t i = 0; i < 50; ++i)
  {
    array[i] = static_cast<int>(i * 2);
  }

  // Verify array contents
  for (size_t i = 0; i < 50; ++i)
  {
    EXPECT_EQ(static_cast<int>(i * 2), array[i]);
  }

  // Reallocate to smaller size
  array.Allocate(5);
  EXPECT_EQ(5, array.Size());

  // Verify array contents
  for (size_t i = 0; i < 5; ++i)
  {
    EXPECT_EQ(static_cast<int>(i * 2), array[i]);
  }
}

// Test with custom MAX_ARRAY_SIZE
TEST(NCollection_LocalArrayTest, CustomMaxArraySize)
{
  const size_t customMaxSize = 50;

  // Test with size less than custom max
  NCollection_LocalArray<int, customMaxSize> smallArray(20);
  EXPECT_EQ(20, smallArray.Size());

  // Test with size greater than custom max
  NCollection_LocalArray<int, customMaxSize> largeArray(100);
  EXPECT_EQ(100, largeArray.Size());
}

// Test with custom type
struct NCollection_LocalArray_TestStruct
{
  int    a;
  double b;

  bool operator==(const NCollection_LocalArray_TestStruct& other) const
  {
    return a == other.a && b == other.b;
  }
};

TEST(NCollection_LocalArrayTest, CustomType)
{
  NCollection_LocalArray<NCollection_LocalArray_TestStruct> array(5);
  EXPECT_EQ(5, array.Size());

  NCollection_LocalArray_TestStruct ts{10, 3.14};

  // Set and retrieve values
  array[0] = ts;
  EXPECT_TRUE(ts == array[0]);
}

// Test transition from stack to heap allocation and back
TEST(NCollection_LocalArrayTest, TransitionStackToHeap)
{
  const int                            maxSize = 10;
  NCollection_LocalArray<int, maxSize> array;

  // Initially allocate in stack buffer
  array.Allocate(5);
  for (size_t i = 0; i < 5; ++i)
  {
    array[i] = static_cast<int>(i);
  }

  // Verify array contents
  for (size_t i = 0; i < 5; ++i)
  {
    EXPECT_EQ(static_cast<int>(i), array[i]);
  }

  // Now allocate larger size, forcing heap allocation
  array.Allocate(maxSize + 10);
  for (size_t i = 0; i < maxSize + 10; ++i)
  {
    array[i] = static_cast<int>(i * 3);
  }

  // Verify heap allocation
  for (size_t i = 0; i < maxSize + 10; ++i)
  {
    EXPECT_EQ(static_cast<int>(i * 3), array[i]);
  }

  // Now go back to stack allocation
  array.Allocate(maxSize - 5);
  for (size_t i = 0; i < maxSize - 5; ++i)
  {
    array[i] = static_cast<int>(i * 5);
  }

  // Verify stack allocation again
  for (size_t i = 0; i < maxSize - 5; ++i)
  {
    EXPECT_EQ(static_cast<int>(i * 5), array[i]);
  }
}

// Test Reallocate with copy (stack to stack)
TEST(NCollection_LocalArrayTest, ReallocateStackToStack_WithCopy)
{
  NCollection_LocalArray<int, 64> anArray(10);
  for (size_t i = 0; i < 10; ++i)
    anArray[i] = static_cast<int>(i * 100);

  anArray.Reallocate(32, true);
  EXPECT_EQ(32u, anArray.Size());

  // Original elements should be preserved
  for (size_t i = 0; i < 10; ++i)
    EXPECT_EQ(static_cast<int>(i * 100), anArray[i]);
}

// Test Reallocate with copy (stack to heap)
TEST(NCollection_LocalArrayTest, ReallocateStackToHeap_WithCopy)
{
  NCollection_LocalArray<int, 8> anArray(8);
  for (size_t i = 0; i < 8; ++i)
    anArray[i] = static_cast<int>(i + 1);

  // Force heap allocation
  anArray.Reallocate(16, true);
  EXPECT_EQ(16u, anArray.Size());

  // Original elements should be preserved
  for (size_t i = 0; i < 8; ++i)
    EXPECT_EQ(static_cast<int>(i + 1), anArray[i]);
}

// Test Reallocate with copy (heap to larger heap)
TEST(NCollection_LocalArrayTest, ReallocateHeapToHeap_WithCopy)
{
  NCollection_LocalArray<int, 4> anArray(8); // starts on heap
  for (size_t i = 0; i < 8; ++i)
    anArray[i] = static_cast<int>(i * 10);

  anArray.Reallocate(16, true);
  EXPECT_EQ(16u, anArray.Size());

  for (size_t i = 0; i < 8; ++i)
    EXPECT_EQ(static_cast<int>(i * 10), anArray[i]);
}

// Test Reallocate without copy
TEST(NCollection_LocalArrayTest, ReallocateNoCopy)
{
  NCollection_LocalArray<int, 8> anArray(8);
  for (size_t i = 0; i < 8; ++i)
    anArray[i] = static_cast<int>(i + 1);

  anArray.Reallocate(16, false);
  EXPECT_EQ(16u, anArray.Size());
  // Content is undefined — just verify it doesn't crash
}

// Test Reallocate shrink just updates logical size
TEST(NCollection_LocalArrayTest, ReallocateShrink)
{
  NCollection_LocalArray<int, 8> anArray(16); // starts on heap
  for (size_t i = 0; i < 16; ++i)
    anArray[i] = static_cast<int>(i * 5);

  // Shrink - should only update logical size, data preserved
  anArray.Reallocate(4, true);
  EXPECT_EQ(4u, anArray.Size());

  for (size_t i = 0; i < 4; ++i)
    EXPECT_EQ(static_cast<int>(i * 5), anArray[i]);
}

// Test move constructor from stack-allocated source
TEST(NCollection_LocalArrayTest, MoveConstructor_FromStack)
{
  NCollection_LocalArray<int, 64> aSrc(10);
  for (size_t i = 0; i < 10; ++i)
    aSrc[i] = static_cast<int>(i * 7);

  NCollection_LocalArray<int, 64> aDst(std::move(aSrc));

  // Destination has the data
  EXPECT_EQ(10u, aDst.Size());
  for (size_t i = 0; i < 10; ++i)
    EXPECT_EQ(static_cast<int>(i * 7), aDst[i]);

  // Source is empty
  EXPECT_EQ(0u, aSrc.Size());
}

// Test move constructor from heap-allocated source
TEST(NCollection_LocalArrayTest, MoveConstructor_FromHeap)
{
  NCollection_LocalArray<int, 4> aSrc(16); // exceeds stack buffer, goes to heap
  for (size_t i = 0; i < 16; ++i)
    aSrc[i] = static_cast<int>(i * 3);

  NCollection_LocalArray<int, 4> aDst(std::move(aSrc));

  // Destination has the data
  EXPECT_EQ(16u, aDst.Size());
  for (size_t i = 0; i < 16; ++i)
    EXPECT_EQ(static_cast<int>(i * 3), aDst[i]);

  // Source is empty
  EXPECT_EQ(0u, aSrc.Size());
}

// Test move assignment operator
TEST(NCollection_LocalArrayTest, MoveAssignment)
{
  NCollection_LocalArray<int, 8> aSrc(8);
  for (size_t i = 0; i < 8; ++i)
    aSrc[i] = static_cast<int>(i + 100);

  NCollection_LocalArray<int, 8> aDst(4);
  for (size_t i = 0; i < 4; ++i)
    aDst[i] = -1;

  aDst = std::move(aSrc);

  EXPECT_EQ(8u, aDst.Size());
  for (size_t i = 0; i < 8; ++i)
    EXPECT_EQ(static_cast<int>(i + 100), aDst[i]);

  EXPECT_EQ(0u, aSrc.Size());
}

// Test Reallocate used as a growable stack
TEST(NCollection_LocalArrayTest, ReallocateAsGrowableStack)
{
  NCollection_LocalArray<int, 4> aStack(4);
  int                            aTop = 0;

  // Push more than initial capacity
  for (int i = 0; i < 20; ++i)
  {
    if (aTop >= static_cast<int>(aStack.Size()))
      aStack.Reallocate(aStack.Size() * 2, true);
    aStack[aTop++] = i * 3;
  }

  EXPECT_EQ(20, aTop);

  // Pop and verify
  for (int i = 19; i >= 0; --i)
  {
    EXPECT_EQ(i * 3, aStack[--aTop]);
  }
}