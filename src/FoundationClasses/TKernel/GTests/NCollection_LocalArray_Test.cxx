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
  // Content is undefined -- just verify it doesn't crash
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

// ============ Non-trivially-copyable type tests ============

// Tracker for construction/destruction call counts.
static int THE_CTOR_COUNT = 0;
static int THE_DTOR_COUNT = 0;
static int THE_MOVE_COUNT = 0;

struct NCollection_LocalArray_Tracked
{
  int Value = 0;

  NCollection_LocalArray_Tracked() { ++THE_CTOR_COUNT; }

  ~NCollection_LocalArray_Tracked() { ++THE_DTOR_COUNT; }

  NCollection_LocalArray_Tracked(const NCollection_LocalArray_Tracked& theOther)
      : Value(theOther.Value)
  {
    ++THE_CTOR_COUNT;
  }

  NCollection_LocalArray_Tracked(NCollection_LocalArray_Tracked&& theOther) noexcept
      : Value(theOther.Value)
  {
    theOther.Value = -1;
    ++THE_MOVE_COUNT;
  }

  NCollection_LocalArray_Tracked& operator=(NCollection_LocalArray_Tracked&& theOther) noexcept
  {
    Value          = theOther.Value;
    theOther.Value = -1;
    ++THE_MOVE_COUNT;
    return *this;
  }

  NCollection_LocalArray_Tracked& operator=(const NCollection_LocalArray_Tracked&) = default;
};

static void resetTrackedCounters()
{
  THE_CTOR_COUNT = 0;
  THE_DTOR_COUNT = 0;
  THE_MOVE_COUNT = 0;
}

static_assert(!std::is_trivially_copyable_v<NCollection_LocalArray_Tracked>,
              "Tracked type must be non-trivially copyable for these tests");

TEST(NCollection_LocalArrayTest, NonTrivial_SmallAllocation)
{
  resetTrackedCounters();
  {
    NCollection_LocalArray<NCollection_LocalArray_Tracked, 8> anArr(4);
    EXPECT_EQ(4u, anArr.Size());
    EXPECT_EQ(4, THE_CTOR_COUNT);

    anArr[0].Value = 10;
    anArr[1].Value = 20;
    anArr[2].Value = 30;
    anArr[3].Value = 40;
    EXPECT_EQ(30, anArr[2].Value);
  }
  // All 4 elements destroyed.
  EXPECT_EQ(4, THE_DTOR_COUNT);
}

TEST(NCollection_LocalArrayTest, NonTrivial_HeapAllocation)
{
  resetTrackedCounters();
  {
    NCollection_LocalArray<NCollection_LocalArray_Tracked, 4> anArr(8);
    EXPECT_EQ(8u, anArr.Size());

    for (size_t i = 0; i < 8; ++i)
      anArr[i].Value = static_cast<int>(i * 100);

    for (size_t i = 0; i < 8; ++i)
      EXPECT_EQ(static_cast<int>(i * 100), anArr[i].Value);
  }
  EXPECT_EQ(THE_CTOR_COUNT + THE_MOVE_COUNT, THE_DTOR_COUNT);
}

TEST(NCollection_LocalArrayTest, NonTrivial_ReallocateWithCopy_InlineToInline)
{
  NCollection_LocalArray<NCollection_LocalArray_Tracked, 16> anArr(4);
  for (size_t i = 0; i < 4; ++i)
    anArr[i].Value = static_cast<int>(i + 1);

  anArr.Reallocate(8, true);
  EXPECT_EQ(8u, anArr.Size());

  // Original elements preserved.
  for (size_t i = 0; i < 4; ++i)
    EXPECT_EQ(static_cast<int>(i + 1), anArr[i].Value);
}

TEST(NCollection_LocalArrayTest, NonTrivial_ReallocateWithCopy_InlineToHeap)
{
  NCollection_LocalArray<NCollection_LocalArray_Tracked, 4> anArr(4);
  for (size_t i = 0; i < 4; ++i)
    anArr[i].Value = static_cast<int>(i * 10);

  anArr.Reallocate(16, true);
  EXPECT_EQ(16u, anArr.Size());

  for (size_t i = 0; i < 4; ++i)
    EXPECT_EQ(static_cast<int>(i * 10), anArr[i].Value);
}

TEST(NCollection_LocalArrayTest, NonTrivial_ReallocateWithCopy_HeapToHeap)
{
  NCollection_LocalArray<NCollection_LocalArray_Tracked, 4> anArr(8);
  for (size_t i = 0; i < 8; ++i)
    anArr[i].Value = static_cast<int>(i * 5);

  anArr.Reallocate(16, true);
  EXPECT_EQ(16u, anArr.Size());

  for (size_t i = 0; i < 8; ++i)
    EXPECT_EQ(static_cast<int>(i * 5), anArr[i].Value);
}

TEST(NCollection_LocalArrayTest, NonTrivial_Shrink_DestroysExcess)
{
  resetTrackedCounters();
  {
    NCollection_LocalArray<NCollection_LocalArray_Tracked, 8> anArr(8);
    EXPECT_EQ(8, THE_CTOR_COUNT);

    anArr.Reallocate(3, true);
    EXPECT_EQ(3u, anArr.Size());
    // 5 excess elements destroyed.
    EXPECT_EQ(5, THE_DTOR_COUNT);
  }
  // Remaining 3 destroyed in destructor.
  EXPECT_EQ(8, THE_DTOR_COUNT);
}

TEST(NCollection_LocalArrayTest, NonTrivial_MoveConstructor_FromInline)
{
  NCollection_LocalArray<NCollection_LocalArray_Tracked, 8> aSrc(4);
  for (size_t i = 0; i < 4; ++i)
    aSrc[i].Value = static_cast<int>(i * 7);

  NCollection_LocalArray<NCollection_LocalArray_Tracked, 8> aDst(std::move(aSrc));

  EXPECT_EQ(4u, aDst.Size());
  for (size_t i = 0; i < 4; ++i)
    EXPECT_EQ(static_cast<int>(i * 7), aDst[i].Value);

  EXPECT_EQ(0u, aSrc.Size());
}

TEST(NCollection_LocalArrayTest, NonTrivial_MoveConstructor_FromHeap)
{
  NCollection_LocalArray<NCollection_LocalArray_Tracked, 4> aSrc(8);
  for (size_t i = 0; i < 8; ++i)
    aSrc[i].Value = static_cast<int>(i * 3);

  NCollection_LocalArray<NCollection_LocalArray_Tracked, 4> aDst(std::move(aSrc));

  EXPECT_EQ(8u, aDst.Size());
  for (size_t i = 0; i < 8; ++i)
    EXPECT_EQ(static_cast<int>(i * 3), aDst[i].Value);

  EXPECT_EQ(0u, aSrc.Size());
}

TEST(NCollection_LocalArrayTest, NonTrivial_MoveAssignment)
{
  NCollection_LocalArray<NCollection_LocalArray_Tracked, 8> aSrc(4);
  for (size_t i = 0; i < 4; ++i)
    aSrc[i].Value = static_cast<int>(i + 100);

  NCollection_LocalArray<NCollection_LocalArray_Tracked, 8> aDst(2);
  aDst = std::move(aSrc);

  EXPECT_EQ(4u, aDst.Size());
  for (size_t i = 0; i < 4; ++i)
    EXPECT_EQ(static_cast<int>(i + 100), aDst[i].Value);

  EXPECT_EQ(0u, aSrc.Size());
}

TEST(NCollection_LocalArrayTest, NonTrivial_DestructorCallBalance)
{
  resetTrackedCounters();
  {
    NCollection_LocalArray<NCollection_LocalArray_Tracked, 4> anArr(4);
    anArr.Reallocate(8, true);  // inline -> heap: 4 moves + 4 new + 4 destroy old
    anArr.Reallocate(2, true);  // shrink: 6 destroyed
    anArr.Reallocate(16, true); // heap -> bigger heap: 2 moves + 14 new + 2 destroy old
  }
  // Total constructions (ctor + move) must equal total destructions.
  EXPECT_EQ(THE_CTOR_COUNT + THE_MOVE_COUNT, THE_DTOR_COUNT);
}