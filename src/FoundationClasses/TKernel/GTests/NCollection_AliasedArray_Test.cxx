// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <NCollection_AliasedArray.hxx>

#include <gtest/gtest.h>

TEST(NCollection_AliasedArrayTest, ResizeEmptyWithCopy)
{
  NCollection_AliasedArray<> anArray(sizeof(int));

  anArray.Resize(2, true);

  EXPECT_EQ(size_t{2}, anArray.Size());
  EXPECT_EQ(2, anArray.Length());
  anArray.ChangeValue<int>(0) = 10;
  anArray.ChangeLast<int>()   = 20;
  EXPECT_EQ(10, anArray.Value<int>(0));
  EXPECT_EQ(20, anArray.Value<int>(1));
}

TEST(NCollection_AliasedArrayTest, ResizeWithoutCopy)
{
  NCollection_AliasedArray<> anArray(sizeof(int), 2);
  EXPECT_EQ(sizeof(int), anArray.Stride());
  EXPECT_TRUE(anArray.IsDeletable());
  anArray.ChangeValue<int>(0) = 10;
  anArray.ChangeValue<int>(1) = 20;

  anArray.Resize(size_t{4}, false);

  EXPECT_EQ(size_t{4}, anArray.Size());
  EXPECT_EQ(4, anArray.Length());
  anArray.ChangeLast<int>() = 40;
  EXPECT_EQ(40, anArray.Value<int>(3));
}

TEST(NCollection_AliasedArrayTest, WrapExternalArray)
{
  int                        anExternalArray[] = {10, 20};
  NCollection_AliasedArray<> anArray(anExternalArray, size_t{2});

  EXPECT_EQ(sizeof(int), anArray.Stride());
  EXPECT_FALSE(anArray.IsDeletable());
  EXPECT_EQ(10, anArray.Value<int>(0));
  anArray.ChangeValue<int>(size_t{1}) = 30;
  EXPECT_EQ(30, anExternalArray[1]);
}

TEST(NCollection_AliasedArrayTest, TypedDataAndInit)
{
  NCollection_AliasedArray<> anArray(sizeof(int), size_t{3});

  anArray.Init<int>(7);
  const int* aData = anArray.Data<int>();
  EXPECT_EQ(7, aData[0]);
  EXPECT_EQ(7, aData[1]);
  EXPECT_EQ(7, aData[2]);

  int* aChangeData = anArray.ChangeData<int>();
  aChangeData[1]   = 9;
  EXPECT_EQ(9, anArray.Value<int>(size_t{1}));
}
