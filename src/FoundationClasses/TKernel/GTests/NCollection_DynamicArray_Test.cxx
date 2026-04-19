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

#include <NCollection_DynamicArray.hxx>
#include <Standard_OutOfRange.hxx>

#include <gtest/gtest.h>
#include <type_traits>

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
