// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <NCollection_LinearVector.hxx>
#include <TCollection_AsciiString.hxx>

#include <gtest/gtest.h>
#include <algorithm>
#include <string>

// Helper struct for testing non-trivial types with move semantics.
struct MoveOnly
{
  int Value  = 0;
  MoveOnly() = default;

  explicit MoveOnly(const int theVal)
      : Value(theVal)
  {
  }

  MoveOnly(const MoveOnly&)            = delete;
  MoveOnly& operator=(const MoveOnly&) = delete;

  MoveOnly(MoveOnly&& theOther) noexcept
      : Value(theOther.Value)
  {
    theOther.Value = -1;
  }

  MoveOnly& operator=(MoveOnly&& theOther) noexcept
  {
    Value          = theOther.Value;
    theOther.Value = -1;
    return *this;
  }
};

// Helper struct for testing multi-argument emplace construction.
struct MultiArg
{
  int    A;
  double B;

  MultiArg(const int theA, const double theB)
      : A(theA),
        B(theB)
  {
  }
};

TEST(NCollection_LinearVectorTest, DefaultConstructor)
{
  NCollection_LinearVector<int> aVec;
  EXPECT_EQ(0, aVec.Size());
  EXPECT_TRUE(aVec.IsEmpty());
  EXPECT_EQ(0, aVec.Capacity());
}

TEST(NCollection_LinearVectorTest, ReserveConstructor)
{
  NCollection_LinearVector<int> aVec(100);
  EXPECT_EQ(0, aVec.Size());
  EXPECT_TRUE(aVec.IsEmpty());
  EXPECT_GE(aVec.Capacity(), 100);
}

TEST(NCollection_LinearVectorTest, Append)
{
  NCollection_LinearVector<int> aVec;
  aVec.Append(10);
  aVec.Append(20);
  aVec.Append(30);

  EXPECT_EQ(3, aVec.Size());
  EXPECT_FALSE(aVec.IsEmpty());
  EXPECT_EQ(10, aVec(0));
  EXPECT_EQ(20, aVec(1));
  EXPECT_EQ(30, aVec(2));
}

TEST(NCollection_LinearVectorTest, AppendMove)
{
  NCollection_LinearVector<std::string> aVec;
  std::string                           aStr = "hello";
  aVec.Append(std::move(aStr));

  EXPECT_EQ(1, aVec.Size());
  EXPECT_EQ("hello", aVec(0));
  EXPECT_TRUE(aStr.empty());
}

TEST(NCollection_LinearVectorTest, Appended)
{
  NCollection_LinearVector<int> aVec;
  int&                          aRef = aVec.Appended();
  aRef                               = 42;

  EXPECT_EQ(1, aVec.Size());
  EXPECT_EQ(42, aVec(0));
}

TEST(NCollection_LinearVectorTest, EmplaceAppend)
{
  NCollection_LinearVector<MultiArg> aVec;
  MultiArg&                          aRef = aVec.EmplaceAppend(5, 3.14);

  EXPECT_EQ(1, aVec.Size());
  EXPECT_EQ(5, aRef.A);
  EXPECT_NEAR(3.14, aRef.B, 1e-10);
}

TEST(NCollection_LinearVectorTest, SetValue)
{
  NCollection_LinearVector<int> aVec;
  aVec.Append(10);
  aVec.Append(20);

  // Overwrite existing
  aVec.SetValue(0, 99);
  EXPECT_EQ(99, aVec(0));

  // Auto-extend
  aVec.SetValue(5, 55);
  EXPECT_EQ(6, aVec.Size());
  EXPECT_EQ(55, aVec(5));
  // Intermediate elements should be default-constructed (0 for int)
  EXPECT_EQ(0, aVec(3));
}

TEST(NCollection_LinearVectorTest, ValueAccess)
{
  NCollection_LinearVector<int> aVec;
  aVec.Append(10);
  aVec.Append(20);
  aVec.Append(30);

  // Const access
  EXPECT_EQ(10, aVec.Value(0));
  EXPECT_EQ(20, aVec.Value(1));
  EXPECT_EQ(30, aVec.Value(2));

  // Mutable access
  aVec.ChangeValue(1) = 99;
  EXPECT_EQ(99, aVec(1));

  // operator() and operator[] consistency
  EXPECT_EQ(aVec(0), aVec[0]);
  EXPECT_EQ(aVec(1), aVec[1]);
  EXPECT_EQ(aVec(2), aVec[2]);

  // Mutable operator[]
  aVec[2] = 77;
  EXPECT_EQ(77, aVec.Value(2));
}

TEST(NCollection_LinearVectorTest, FirstLast)
{
  NCollection_LinearVector<int> aVec;
  aVec.Append(10);
  aVec.Append(20);
  aVec.Append(30);

  EXPECT_EQ(10, aVec.First());
  EXPECT_EQ(30, aVec.Last());

  aVec.ChangeFirst() = 99;
  aVec.ChangeLast()  = 88;
  EXPECT_EQ(99, aVec(0));
  EXPECT_EQ(88, aVec(2));
}

TEST(NCollection_LinearVectorTest, CopyConstructor)
{
  NCollection_LinearVector<int> aVec;
  aVec.Append(1);
  aVec.Append(2);
  aVec.Append(3);

  NCollection_LinearVector<int> aCopy(aVec);
  EXPECT_EQ(3, aCopy.Size());
  EXPECT_EQ(1, aCopy(0));
  EXPECT_EQ(2, aCopy(1));
  EXPECT_EQ(3, aCopy(2));

  // Verify independence
  aCopy(0) = 99;
  EXPECT_EQ(1, aVec(0));
}

TEST(NCollection_LinearVectorTest, CopyAssignment)
{
  NCollection_LinearVector<int> aVec;
  aVec.Append(1);
  aVec.Append(2);

  NCollection_LinearVector<int> aCopy;
  aCopy.Append(100);
  aCopy = aVec;

  EXPECT_EQ(2, aCopy.Size());
  EXPECT_EQ(1, aCopy(0));
  EXPECT_EQ(2, aCopy(1));

  // Verify independence
  aCopy(0) = 99;
  EXPECT_EQ(1, aVec(0));
}

TEST(NCollection_LinearVectorTest, MoveConstructor)
{
  NCollection_LinearVector<int> aVec;
  aVec.Append(1);
  aVec.Append(2);

  NCollection_LinearVector<int> aMoved(std::move(aVec));
  EXPECT_EQ(2, aMoved.Size());
  EXPECT_EQ(1, aMoved(0));
  EXPECT_EQ(2, aMoved(1));

  // Source should be empty
  EXPECT_EQ(0, aVec.Size());
  EXPECT_TRUE(aVec.IsEmpty());
}

TEST(NCollection_LinearVectorTest, MoveAssignment)
{
  NCollection_LinearVector<int> aVec;
  aVec.Append(1);
  aVec.Append(2);

  NCollection_LinearVector<int> aMoved;
  aMoved.Append(100);
  aMoved = std::move(aVec);

  EXPECT_EQ(2, aMoved.Size());
  EXPECT_EQ(1, aMoved(0));
  EXPECT_EQ(2, aMoved(1));

  // Source should be empty
  EXPECT_EQ(0, aVec.Size());
  EXPECT_TRUE(aVec.IsEmpty());
}

TEST(NCollection_LinearVectorTest, Reserve)
{
  NCollection_LinearVector<int> aVec;
  aVec.Append(1);
  aVec.Append(2);

  aVec.Reserve(100);
  EXPECT_EQ(2, aVec.Size());
  EXPECT_GE(aVec.Capacity(), 100);
  EXPECT_EQ(1, aVec(0));
  EXPECT_EQ(2, aVec(1));

  // Reserve smaller should be a no-op
  aVec.Reserve(5);
  EXPECT_GE(aVec.Capacity(), 100);
}

TEST(NCollection_LinearVectorTest, Resize)
{
  NCollection_LinearVector<int> aVec;
  aVec.Append(1);
  aVec.Append(2);
  aVec.Append(3);

  // Grow
  aVec.Resize(5);
  EXPECT_EQ(5, aVec.Size());
  EXPECT_EQ(1, aVec(0));
  EXPECT_EQ(2, aVec(1));
  EXPECT_EQ(3, aVec(2));
  EXPECT_EQ(0, aVec(3));
  EXPECT_EQ(0, aVec(4));

  // Shrink
  aVec.Resize(2);
  EXPECT_EQ(2, aVec.Size());
  EXPECT_EQ(1, aVec(0));
  EXPECT_EQ(2, aVec(1));
}

TEST(NCollection_LinearVectorTest, EraseLast)
{
  NCollection_LinearVector<int> aVec;
  aVec.Append(1);
  aVec.Append(2);
  aVec.Append(3);

  aVec.EraseLast();
  EXPECT_EQ(2, aVec.Size());
  EXPECT_EQ(1, aVec(0));
  EXPECT_EQ(2, aVec(1));
}

TEST(NCollection_LinearVectorTest, EraseSingle)
{
  NCollection_LinearVector<int> aVec;
  aVec.Append(10);
  aVec.Append(20);
  aVec.Append(30);
  aVec.Append(40);

  // Erase middle element
  aVec.Erase(1);
  EXPECT_EQ(3, aVec.Size());
  EXPECT_EQ(10, aVec(0));
  EXPECT_EQ(30, aVec(1));
  EXPECT_EQ(40, aVec(2));

  // Erase first
  aVec.Erase(0);
  EXPECT_EQ(2, aVec.Size());
  EXPECT_EQ(30, aVec(0));
  EXPECT_EQ(40, aVec(1));
}

TEST(NCollection_LinearVectorTest, EraseRange)
{
  NCollection_LinearVector<int> aVec;
  for (int i = 0; i < 6; ++i)
  {
    aVec.Append(i * 10);
  }
  // [0, 10, 20, 30, 40, 50]

  // Erase [1, 4) => remove 10, 20, 30
  aVec.Erase(1, 4);
  EXPECT_EQ(3, aVec.Size());
  EXPECT_EQ(0, aVec(0));
  EXPECT_EQ(40, aVec(1));
  EXPECT_EQ(50, aVec(2));
}

// Regression: non-tail Erase must not destroy the target slot before shifting.
// TCollection_AsciiString's move-assignment reads the current buffer (to decide
// whether to free it), so assigning into an already-destructed slot would call
// free() on garbage and crash the process.
TEST(NCollection_LinearVectorTest, EraseSingle_NonTrivial_HeapBuffers)
{
  NCollection_LinearVector<TCollection_AsciiString> aVec;
  aVec.Append(TCollection_AsciiString("alpha-string-forces-heap-allocation"));
  aVec.Append(TCollection_AsciiString("beta-string-forces-heap-allocation"));
  aVec.Append(TCollection_AsciiString("gamma-string-forces-heap-allocation"));
  aVec.Append(TCollection_AsciiString("delta-string-forces-heap-allocation"));

  aVec.Erase(1);
  ASSERT_EQ(3u, aVec.Size());
  EXPECT_STREQ("alpha-string-forces-heap-allocation", aVec(0).ToCString());
  EXPECT_STREQ("gamma-string-forces-heap-allocation", aVec(1).ToCString());
  EXPECT_STREQ("delta-string-forces-heap-allocation", aVec(2).ToCString());

  aVec.Erase(0);
  ASSERT_EQ(2u, aVec.Size());
  EXPECT_STREQ("gamma-string-forces-heap-allocation", aVec(0).ToCString());
  EXPECT_STREQ("delta-string-forces-heap-allocation", aVec(1).ToCString());
}

TEST(NCollection_LinearVectorTest, EraseRange_NonTrivial_HeapBuffers)
{
  NCollection_LinearVector<TCollection_AsciiString> aVec;
  for (int i = 0; i < 6; ++i)
  {
    TCollection_AsciiString aEntry("payload-with-heap-allocation-");
    aEntry += TCollection_AsciiString(i);
    aVec.Append(std::move(aEntry));
  }

  aVec.Erase(1, 4);
  ASSERT_EQ(3u, aVec.Size());
  EXPECT_STREQ("payload-with-heap-allocation-0", aVec(0).ToCString());
  EXPECT_STREQ("payload-with-heap-allocation-4", aVec(1).ToCString());
  EXPECT_STREQ("payload-with-heap-allocation-5", aVec(2).ToCString());
}

TEST(NCollection_LinearVectorTest, InsertBefore)
{
  NCollection_LinearVector<int> aVec;
  aVec.Append(10);
  aVec.Append(30);

  // Insert at beginning
  aVec.InsertBefore(0, 5);
  EXPECT_EQ(3, aVec.Size());
  EXPECT_EQ(5, aVec(0));
  EXPECT_EQ(10, aVec(1));
  EXPECT_EQ(30, aVec(2));

  // Insert in middle
  aVec.InsertBefore(2, 20);
  EXPECT_EQ(4, aVec.Size());
  EXPECT_EQ(5, aVec(0));
  EXPECT_EQ(10, aVec(1));
  EXPECT_EQ(20, aVec(2));
  EXPECT_EQ(30, aVec(3));

  // Insert at end (valid: theIndex == mySize)
  aVec.InsertBefore(4, 40);
  EXPECT_EQ(5, aVec.Size());
  EXPECT_EQ(40, aVec(4));
}

TEST(NCollection_LinearVectorTest, InsertAfter)
{
  NCollection_LinearVector<int> aVec;
  aVec.Append(10);
  aVec.Append(30);

  aVec.InsertAfter(0, 20);
  EXPECT_EQ(3, aVec.Size());
  EXPECT_EQ(10, aVec(0));
  EXPECT_EQ(20, aVec(1));
  EXPECT_EQ(30, aVec(2));
}

TEST(NCollection_LinearVectorTest, Clear)
{
  NCollection_LinearVector<int> aVec;
  aVec.Append(1);
  aVec.Append(2);
  aVec.Append(3);
  const size_t aCapBefore = aVec.Capacity();

  // Clear without releasing memory
  aVec.Clear(false);
  EXPECT_EQ(0, aVec.Size());
  EXPECT_TRUE(aVec.IsEmpty());
  EXPECT_EQ(aCapBefore, aVec.Capacity());

  // Re-populate and clear with release
  aVec.Append(10);
  aVec.Clear(true);
  EXPECT_EQ(0, aVec.Size());
  EXPECT_EQ(0, aVec.Capacity());
}

TEST(NCollection_LinearVectorTest, IteratorRangeFor)
{
  NCollection_LinearVector<int> aVec;
  aVec.Append(1);
  aVec.Append(2);
  aVec.Append(3);

  int aSum = 0;
  for (const int aVal : aVec)
  {
    aSum += aVal;
  }
  EXPECT_EQ(6, aSum);

  // Mutable range-for
  for (int& aVal : aVec)
  {
    aVal *= 10;
  }
  EXPECT_EQ(10, aVec(0));
  EXPECT_EQ(20, aVec(1));
  EXPECT_EQ(30, aVec(2));
}

TEST(NCollection_LinearVectorTest, STLAlgorithms)
{
  NCollection_LinearVector<int> aVec;
  aVec.Append(30);
  aVec.Append(10);
  aVec.Append(20);

  // std::sort
  std::sort(aVec.begin(), aVec.end());
  EXPECT_EQ(10, aVec(0));
  EXPECT_EQ(20, aVec(1));
  EXPECT_EQ(30, aVec(2));

  // std::find
  int* aFound = std::find(aVec.begin(), aVec.end(), 20);
  EXPECT_NE(aVec.end(), aFound);
  EXPECT_EQ(20, *aFound);

  // std::reverse
  std::reverse(aVec.begin(), aVec.end());
  EXPECT_EQ(30, aVec(0));
  EXPECT_EQ(20, aVec(1));
  EXPECT_EQ(10, aVec(2));
}

TEST(NCollection_LinearVectorTest, TrivialTypeGrowth)
{
  NCollection_LinearVector<double> aVec;
  for (int i = 0; i < 100; ++i)
  {
    aVec.Append(static_cast<double>(i));
  }
  EXPECT_EQ(100, aVec.Size());
  for (int i = 0; i < 100; ++i)
  {
    EXPECT_NEAR(static_cast<double>(i), aVec(i), 1e-15);
  }
}

TEST(NCollection_LinearVectorTest, NonTrivialType)
{
  NCollection_LinearVector<std::string> aVec;
  aVec.Append("hello");
  aVec.Append("world");
  aVec.Append("test");

  EXPECT_EQ(3, aVec.Size());
  EXPECT_EQ("hello", aVec(0));
  EXPECT_EQ("world", aVec(1));
  EXPECT_EQ("test", aVec(2));

  // Copy
  NCollection_LinearVector<std::string> aCopy(aVec);
  EXPECT_EQ(3, aCopy.Size());
  EXPECT_EQ("hello", aCopy(0));

  // Move
  NCollection_LinearVector<std::string> aMoved(std::move(aCopy));
  EXPECT_EQ(3, aMoved.Size());
  EXPECT_EQ("hello", aMoved(0));
  EXPECT_TRUE(aCopy.IsEmpty());

  // Erase middle
  aMoved.Erase(1);
  EXPECT_EQ(2, aMoved.Size());
  EXPECT_EQ("hello", aMoved(0));
  EXPECT_EQ("test", aMoved(1));

  // InsertBefore
  aMoved.InsertBefore(1, "inserted");
  EXPECT_EQ(3, aMoved.Size());
  EXPECT_EQ("inserted", aMoved(1));
}

TEST(NCollection_LinearVectorTest, EmptyOperations)
{
  NCollection_LinearVector<int> aVec;

  // EraseLast on empty should be safe
  aVec.EraseLast();
  EXPECT_EQ(0, aVec.Size());

  // Clear on empty
  aVec.Clear(false);
  EXPECT_EQ(0, aVec.Size());
  aVec.Clear(true);
  EXPECT_EQ(0, aVec.Size());
  EXPECT_EQ(0, aVec.Capacity());
}

TEST(NCollection_LinearVectorTest, LargeAppend)
{
  NCollection_LinearVector<int> aVec;
  const int                     aCount = 10000;
  for (int i = 0; i < aCount; ++i)
  {
    aVec.Append(i);
  }
  EXPECT_EQ(aCount, aVec.Size());
  for (int i = 0; i < aCount; ++i)
  {
    EXPECT_EQ(i, aVec(i));
  }
}

TEST(NCollection_LinearVectorTest, MoveOnlyType)
{
  NCollection_LinearVector<MoveOnly> aVec;
  aVec.Append(MoveOnly(1));
  aVec.Append(MoveOnly(2));
  aVec.Append(MoveOnly(3));

  EXPECT_EQ(3, aVec.Size());
  EXPECT_EQ(1, aVec(0).Value);
  EXPECT_EQ(2, aVec(1).Value);
  EXPECT_EQ(3, aVec(2).Value);

  // Move construct
  NCollection_LinearVector<MoveOnly> aMoved(std::move(aVec));
  EXPECT_EQ(3, aMoved.Size());
  EXPECT_EQ(1, aMoved(0).Value);
  EXPECT_TRUE(aVec.IsEmpty());
}

TEST(NCollection_LinearVectorTest, ResizeNonTrivial)
{
  NCollection_LinearVector<std::string> aVec;
  aVec.Append("a");
  aVec.Append("b");

  // Grow - new elements should be default-constructed (empty strings)
  aVec.Resize(4);
  EXPECT_EQ(4, aVec.Size());
  EXPECT_EQ("a", aVec(0));
  EXPECT_EQ("b", aVec(1));
  EXPECT_TRUE(aVec(2).empty());
  EXPECT_TRUE(aVec(3).empty());

  // Shrink - excess elements should be destroyed
  aVec.Resize(1);
  EXPECT_EQ(1, aVec.Size());
  EXPECT_EQ("a", aVec(0));
}

TEST(NCollection_LinearVectorTest, BoundsAccess)
{
  NCollection_LinearVector<int> aVec;
  aVec.Append(10);
  aVec.Append(20);
  aVec.Append(30);

  EXPECT_EQ(3, aVec.Size());
  EXPECT_EQ(10, aVec(0));
  EXPECT_EQ(30, aVec(aVec.Size() - 1));
}

TEST(NCollection_LinearVectorTest, ReserveZero_NoEffect)
{
  NCollection_LinearVector<int> aVec;
  aVec.Append(1);
  const size_t aCapBefore = aVec.Capacity();
  aVec.Reserve(0);
  EXPECT_EQ(aCapBefore, aVec.Capacity());
  EXPECT_EQ(1, aVec.Size());
  EXPECT_EQ(1, aVec(0));
}

TEST(NCollection_LinearVectorTest, ReserveDecreasing_NoShrink)
{
  NCollection_LinearVector<int> aVec;
  aVec.Reserve(100);
  EXPECT_GE(aVec.Capacity(), 100);
  const size_t aCapBefore = aVec.Capacity();
  aVec.Reserve(10);
  EXPECT_EQ(aCapBefore, aVec.Capacity());
}

TEST(NCollection_LinearVectorTest, InsertBeforeAtEnd_EquivalentToAppend)
{
  NCollection_LinearVector<int> aVec;
  aVec.Append(1);
  aVec.Append(2);
  aVec.InsertBefore(aVec.Size(), 3);
  EXPECT_EQ(3, aVec.Size());
  EXPECT_EQ(1, aVec(0));
  EXPECT_EQ(2, aVec(1));
  EXPECT_EQ(3, aVec(2));
}

TEST(NCollection_LinearVectorTest, SetValueMove)
{
  NCollection_LinearVector<std::string> aVec;
  std::string                           aSource = "abc";

  aVec.SetValue(0, std::move(aSource));

  EXPECT_EQ(1, aVec.Size());
  EXPECT_EQ("abc", aVec(0));
}

TEST(NCollection_LinearVectorTest, InsertBeforeZero_InsertsAtFront)
{
  NCollection_LinearVector<int> aVec;
  aVec.Append(2);
  aVec.Append(3);

  aVec.InsertBefore(0, 1);

  EXPECT_EQ(3, aVec.Size());
  EXPECT_EQ(1, aVec(0));
  EXPECT_EQ(2, aVec(1));
  EXPECT_EQ(3, aVec(2));
}

TEST(NCollection_LinearVectorTest, OutOfRangeIndicesThrow)
{
  NCollection_LinearVector<int> aVec;
  aVec.Append(1);

#ifndef No_Exception
  const size_t anOutIndex = 5;
  EXPECT_ANY_THROW(aVec.Value(anOutIndex));
  EXPECT_ANY_THROW(aVec.ChangeValue(anOutIndex));
  EXPECT_ANY_THROW(aVec.InsertAfter(anOutIndex, 1));
  EXPECT_ANY_THROW(aVec.Erase(anOutIndex));
#endif
}
