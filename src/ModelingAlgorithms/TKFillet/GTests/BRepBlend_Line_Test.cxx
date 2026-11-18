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

#include <BRepBlend_Line.hxx>
#include <Blend_Point.hxx>
#include <gp_Pnt.hxx>
#include <gtest/gtest.h>

namespace
{
//==================================================================================================

Blend_Point makePoint(double theParameter)
{
  return Blend_Point(gp_Pnt(theParameter, 0.0, 0.0),
                     gp_Pnt(theParameter, 1.0, 0.0),
                     theParameter,
                     theParameter,
                     0.0,
                     theParameter,
                     1.0);
}
} // namespace

//==================================================================================================

TEST(BRepBlend_LineTest, InsertAndRemovePreserveOrder)
{
  BRepBlend_Line aLine;
  aLine.Prepend(makePoint(0.0));
  aLine.Append(makePoint(2.0));
  aLine.InsertBefore(2, makePoint(1.0));
  aLine.InsertBefore(aLine.NbPoints() + 1, makePoint(3.0));
  ASSERT_EQ(aLine.NbPoints(), 4);
  for (int anIndex = 1; anIndex <= aLine.NbPoints(); ++anIndex)
  {
    EXPECT_DOUBLE_EQ(aLine.Point(anIndex).Parameter(), anIndex - 1.0);
  }

  aLine.Remove(2, 3);
  ASSERT_EQ(aLine.NbPoints(), 2);
  EXPECT_DOUBLE_EQ(aLine.Point(1).Parameter(), 0.0);
  EXPECT_DOUBLE_EQ(aLine.Point(2).Parameter(), 3.0);
  aLine.Remove(1, aLine.NbPoints());
  EXPECT_EQ(aLine.NbPoints(), 0);
  aLine.InsertBefore(1, makePoint(4.0));
  EXPECT_DOUBLE_EQ(aLine.Point(1).Parameter(), 4.0);
}

//==================================================================================================

TEST(BRepBlend_LineTest, InsertionCopiesAliasedPoint)
{
  BRepBlend_Line aLine;
  aLine.Append(makePoint(0.0));
  aLine.Append(makePoint(1.0));
  aLine.Append(makePoint(2.0));
  const Blend_Point& aFirstPoint = aLine.Point(1);
  aLine.InsertBefore(1, aLine.Point(3));
  ASSERT_EQ(aLine.NbPoints(), 4);
  EXPECT_DOUBLE_EQ(aLine.Point(1).Parameter(), 2.0);
  EXPECT_DOUBLE_EQ(aLine.Point(2).Parameter(), 0.0);
  EXPECT_DOUBLE_EQ(aLine.Point(3).Parameter(), 1.0);
  EXPECT_DOUBLE_EQ(aLine.Point(4).Parameter(), 2.0);
  EXPECT_DOUBLE_EQ(aFirstPoint.Parameter(), 0.0);
}

//==================================================================================================

TEST(BRepBlend_LineTest, AppendPreservesReferencesAndCopiesAreIndependent)
{
  BRepBlend_Line aLine;
  aLine.Append(makePoint(0.0));
  const Blend_Point& aFirstPoint = aLine.Point(1);
  constexpr int      aNbPoints   = 1025;
  for (int anIndex = 1; anIndex < aNbPoints; ++anIndex)
  {
    aLine.Append(makePoint(anIndex));
  }
  EXPECT_DOUBLE_EQ(aFirstPoint.Parameter(), 0.0);
  EXPECT_DOUBLE_EQ(aLine.Point(aNbPoints).Parameter(), aNbPoints - 1.0);

  BRepBlend_Line aCopy(aLine);
  BRepBlend_Line aAssigned;
  aAssigned = aLine;
  aLine.Clear();
  ASSERT_EQ(aCopy.NbPoints(), aNbPoints);
  ASSERT_EQ(aAssigned.NbPoints(), aNbPoints);
  EXPECT_DOUBLE_EQ(aCopy.Point(aNbPoints).Parameter(), aNbPoints - 1.0);
  EXPECT_DOUBLE_EQ(aAssigned.Point(aNbPoints).Parameter(), aNbPoints - 1.0);
}
