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

  EXPECT_EQ(2, anArray.Size());
  anArray.ChangeValue<int>(0) = 10;
  anArray.ChangeLast<int>()    = 20;
  EXPECT_EQ(10, anArray.Value<int>(0));
  EXPECT_EQ(20, anArray.Value<int>(1));
}

TEST(NCollection_AliasedArrayTest, ResizeWithoutCopy)
{
  NCollection_AliasedArray<> anArray(sizeof(int), 2);
  anArray.ChangeValue<int>(0) = 10;
  anArray.ChangeValue<int>(1) = 20;

  anArray.Resize(4, false);

  EXPECT_EQ(4, anArray.Size());
  anArray.ChangeLast<int>() = 40;
  EXPECT_EQ(40, anArray.Value<int>(3));
}
