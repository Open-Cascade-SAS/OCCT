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

#include <LProp_CIType.hxx>
#include <LProp_CurAndInf.hxx>

#include <gtest/gtest.h>

TEST(LProp_CurAndInfTest, DefaultConstructor_IsEmpty)
{
  LProp_CurAndInf aResult;
  EXPECT_TRUE(aResult.IsEmpty());
  EXPECT_EQ(aResult.NbPoints(), 0);
}

TEST(LProp_CurAndInfTest, AddInflection)
{
  LProp_CurAndInf aResult;
  aResult.AddInflection(1.5);

  EXPECT_FALSE(aResult.IsEmpty());
  EXPECT_EQ(aResult.NbPoints(), 1);
  EXPECT_DOUBLE_EQ(aResult.Parameter(1), 1.5);
  EXPECT_EQ(aResult.Type(1), LProp_Inflection);
}

TEST(LProp_CurAndInfTest, AddExtCur_Minimum)
{
  LProp_CurAndInf aResult;
  aResult.AddExtCur(2.0, true);

  EXPECT_EQ(aResult.NbPoints(), 1);
  EXPECT_DOUBLE_EQ(aResult.Parameter(1), 2.0);
  EXPECT_EQ(aResult.Type(1), LProp_MinCur);
}

TEST(LProp_CurAndInfTest, AddExtCur_Maximum)
{
  LProp_CurAndInf aResult;
  aResult.AddExtCur(3.0, false);

  EXPECT_EQ(aResult.NbPoints(), 1);
  EXPECT_DOUBLE_EQ(aResult.Parameter(1), 3.0);
  EXPECT_EQ(aResult.Type(1), LProp_MaxCur);
}

TEST(LProp_CurAndInfTest, MultiplePoints_SortedByParameter)
{
  LProp_CurAndInf aResult;
  // Add in non-sorted order
  aResult.AddInflection(5.0);
  aResult.AddExtCur(1.0, true);
  aResult.AddExtCur(3.0, false);
  aResult.AddInflection(7.0);

  EXPECT_EQ(aResult.NbPoints(), 4);

  // Verify sorted order
  for (int i = 1; i < aResult.NbPoints(); ++i)
  {
    EXPECT_LE(aResult.Parameter(i), aResult.Parameter(i + 1));
  }
}

TEST(LProp_CurAndInfTest, MultiplePoints_CorrectTypes)
{
  LProp_CurAndInf aResult;
  aResult.AddExtCur(1.0, true);  // MinCur
  aResult.AddInflection(3.0);    // Inflection
  aResult.AddExtCur(5.0, false); // MaxCur

  EXPECT_EQ(aResult.NbPoints(), 3);
  EXPECT_EQ(aResult.Type(1), LProp_MinCur);
  EXPECT_EQ(aResult.Type(2), LProp_Inflection);
  EXPECT_EQ(aResult.Type(3), LProp_MaxCur);
}

TEST(LProp_CurAndInfTest, Clear)
{
  LProp_CurAndInf aResult;
  aResult.AddInflection(1.0);
  aResult.AddExtCur(2.0, true);

  EXPECT_EQ(aResult.NbPoints(), 2);

  aResult.Clear();
  EXPECT_TRUE(aResult.IsEmpty());
  EXPECT_EQ(aResult.NbPoints(), 0);
}

TEST(LProp_CurAndInfTest, Clear_ThenRefill)
{
  LProp_CurAndInf aResult;
  aResult.AddInflection(1.0);
  aResult.Clear();

  aResult.AddExtCur(5.0, false);
  EXPECT_EQ(aResult.NbPoints(), 1);
  EXPECT_DOUBLE_EQ(aResult.Parameter(1), 5.0);
  EXPECT_EQ(aResult.Type(1), LProp_MaxCur);
}

TEST(LProp_CurAndInfTest, Parameter_OutOfRange_Throws)
{
  LProp_CurAndInf aResult;
  aResult.AddInflection(1.0);

  EXPECT_THROW(aResult.Parameter(0), Standard_OutOfRange);
  EXPECT_THROW(aResult.Parameter(2), Standard_OutOfRange);
}

TEST(LProp_CurAndInfTest, Type_OutOfRange_Throws)
{
  LProp_CurAndInf aResult;
  aResult.AddInflection(1.0);

  EXPECT_THROW(aResult.Type(0), Standard_OutOfRange);
  EXPECT_THROW(aResult.Type(2), Standard_OutOfRange);
}
