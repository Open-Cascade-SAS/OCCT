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

#include <Bnd_Range.hxx>
#include <NCollection_List.hxx>
#include <Standard_ConstructionError.hxx>

#include <gtest/gtest.h>

TEST(Bnd_RangeTest, DefaultConstructor_IsVoid)
{
  Bnd_Range aRange;
  EXPECT_TRUE(aRange.IsVoid());
  EXPECT_LT(aRange.Delta(), 0.0);
}

TEST(Bnd_RangeTest, ParameterizedConstructor)
{
  Bnd_Range aRange(3.0, 15.0);
  EXPECT_FALSE(aRange.IsVoid());
  EXPECT_DOUBLE_EQ(aRange.Delta(), 12.0);
}

TEST(Bnd_RangeTest, ParameterizedConstructor_InvalidRange)
{
  EXPECT_THROW(Bnd_Range(10.0, 5.0), Standard_ConstructionError);
}

TEST(Bnd_RangeTest, ParameterizedConstructor_PointRange)
{
  Bnd_Range aRange(5.0, 5.0);
  EXPECT_FALSE(aRange.IsVoid());
  EXPECT_DOUBLE_EQ(aRange.Delta(), 0.0);
}

TEST(Bnd_RangeTest, GetMin_GetMax_GetBounds)
{
  Bnd_Range aRange(2.0, 8.0);
  double    aMin = 0.0, aMax = 0.0;
  EXPECT_TRUE(aRange.GetMin(aMin));
  EXPECT_DOUBLE_EQ(aMin, 2.0);
  EXPECT_TRUE(aRange.GetMax(aMax));
  EXPECT_DOUBLE_EQ(aMax, 8.0);

  double aFirst = 0.0, aLast = 0.0;
  EXPECT_TRUE(aRange.GetBounds(aFirst, aLast));
  EXPECT_DOUBLE_EQ(aFirst, 2.0);
  EXPECT_DOUBLE_EQ(aLast, 8.0);
}

TEST(Bnd_RangeTest, GetMin_GetMax_Void)
{
  Bnd_Range aRange;
  double    aVal = 0.0;
  EXPECT_FALSE(aRange.GetMin(aVal));
  EXPECT_FALSE(aRange.GetMax(aVal));

  double aFirst = 0.0, aLast = 0.0;
  EXPECT_FALSE(aRange.GetBounds(aFirst, aLast));
}

TEST(Bnd_RangeTest, GetIntermediatePoint)
{
  Bnd_Range aRange(10.0, 20.0);
  double    aPar = 0.0;
  EXPECT_TRUE(aRange.GetIntermediatePoint(0.0, aPar));
  EXPECT_DOUBLE_EQ(aPar, 10.0);
  EXPECT_TRUE(aRange.GetIntermediatePoint(0.5, aPar));
  EXPECT_DOUBLE_EQ(aPar, 15.0);
  EXPECT_TRUE(aRange.GetIntermediatePoint(1.0, aPar));
  EXPECT_DOUBLE_EQ(aPar, 20.0);
}

TEST(Bnd_RangeTest, GetIntermediatePoint_Void)
{
  Bnd_Range aRange;
  double    aPar = 0.0;
  EXPECT_FALSE(aRange.GetIntermediatePoint(0.5, aPar));
}

TEST(Bnd_RangeTest, SetVoid)
{
  Bnd_Range aRange(1.0, 5.0);
  EXPECT_FALSE(aRange.IsVoid());
  aRange.SetVoid();
  EXPECT_TRUE(aRange.IsVoid());
}

TEST(Bnd_RangeTest, Add_Double_ToVoid)
{
  Bnd_Range aRange;
  aRange.Add(5.0);
  EXPECT_FALSE(aRange.IsVoid());
  double aMin = 0.0, aMax = 0.0;
  EXPECT_TRUE(aRange.GetBounds(aMin, aMax));
  EXPECT_DOUBLE_EQ(aMin, 5.0);
  EXPECT_DOUBLE_EQ(aMax, 5.0);
}

TEST(Bnd_RangeTest, Add_Double_Extends)
{
  Bnd_Range aRange(3.0, 7.0);
  aRange.Add(1.0);
  aRange.Add(10.0);
  double aMin = 0.0, aMax = 0.0;
  EXPECT_TRUE(aRange.GetBounds(aMin, aMax));
  EXPECT_DOUBLE_EQ(aMin, 1.0);
  EXPECT_DOUBLE_EQ(aMax, 10.0);
}

TEST(Bnd_RangeTest, Add_Range_ToVoid)
{
  Bnd_Range aRange;
  Bnd_Range anOther(3.0, 7.0);
  aRange.Add(anOther);
  double aMin = 0.0, aMax = 0.0;
  EXPECT_TRUE(aRange.GetBounds(aMin, aMax));
  EXPECT_DOUBLE_EQ(aMin, 3.0);
  EXPECT_DOUBLE_EQ(aMax, 7.0);
}

TEST(Bnd_RangeTest, Add_VoidRange)
{
  Bnd_Range aRange(3.0, 7.0);
  Bnd_Range aVoid;
  aRange.Add(aVoid);
  // Should remain unchanged
  double aMin = 0.0, aMax = 0.0;
  EXPECT_TRUE(aRange.GetBounds(aMin, aMax));
  EXPECT_DOUBLE_EQ(aMin, 3.0);
  EXPECT_DOUBLE_EQ(aMax, 7.0);
}

TEST(Bnd_RangeTest, Add_Range_VoidToVoid)
{
  Bnd_Range aRange;
  Bnd_Range aVoid;
  aRange.Add(aVoid);
  EXPECT_TRUE(aRange.IsVoid());
}

TEST(Bnd_RangeTest, Add_Range_Extends)
{
  Bnd_Range aRange(3.0, 7.0);
  Bnd_Range anOther(1.0, 10.0);
  aRange.Add(anOther);
  double aMin = 0.0, aMax = 0.0;
  EXPECT_TRUE(aRange.GetBounds(aMin, aMax));
  EXPECT_DOUBLE_EQ(aMin, 1.0);
  EXPECT_DOUBLE_EQ(aMax, 10.0);
}

TEST(Bnd_RangeTest, Common_Overlapping)
{
  Bnd_Range aRange(1.0, 10.0);
  Bnd_Range anOther(5.0, 15.0);
  aRange.Common(anOther);
  double aMin = 0.0, aMax = 0.0;
  EXPECT_TRUE(aRange.GetBounds(aMin, aMax));
  EXPECT_DOUBLE_EQ(aMin, 5.0);
  EXPECT_DOUBLE_EQ(aMax, 10.0);
}

TEST(Bnd_RangeTest, Common_NoOverlap)
{
  Bnd_Range aRange(1.0, 5.0);
  Bnd_Range anOther(7.0, 10.0);
  aRange.Common(anOther);
  EXPECT_TRUE(aRange.IsVoid());
}

TEST(Bnd_RangeTest, Common_WithVoid)
{
  Bnd_Range aRange(1.0, 5.0);
  Bnd_Range aVoid;
  aRange.Common(aVoid);
  EXPECT_TRUE(aRange.IsVoid());
}

TEST(Bnd_RangeTest, Union_Overlapping)
{
  Bnd_Range aRange(1.0, 7.0);
  Bnd_Range anOther(5.0, 15.0);
  EXPECT_TRUE(aRange.Union(anOther));
  double aMin = 0.0, aMax = 0.0;
  EXPECT_TRUE(aRange.GetBounds(aMin, aMax));
  EXPECT_DOUBLE_EQ(aMin, 1.0);
  EXPECT_DOUBLE_EQ(aMax, 15.0);
}

TEST(Bnd_RangeTest, Union_Separated)
{
  Bnd_Range aRange(1.0, 5.0);
  Bnd_Range anOther(7.0, 10.0);
  EXPECT_FALSE(aRange.Union(anOther));
  // aRange should remain unchanged
  double aMin = 0.0, aMax = 0.0;
  EXPECT_TRUE(aRange.GetBounds(aMin, aMax));
  EXPECT_DOUBLE_EQ(aMin, 1.0);
  EXPECT_DOUBLE_EQ(aMax, 5.0);
}

TEST(Bnd_RangeTest, Union_WithVoid)
{
  Bnd_Range aRange(1.0, 5.0);
  Bnd_Range aVoid;
  EXPECT_FALSE(aRange.Union(aVoid));
}

TEST(Bnd_RangeTest, IsIntersected_Void)
{
  Bnd_Range aRange;
  EXPECT_EQ(aRange.IsIntersected(5.0), Bnd_Range::IntersectStatus_Out);
}

TEST(Bnd_RangeTest, IsIntersected_Contains)
{
  Bnd_Range aRange(3.0, 15.0);
  EXPECT_EQ(aRange.IsIntersected(5.0), Bnd_Range::IntersectStatus_In);
}

TEST(Bnd_RangeTest, IsIntersected_Outside)
{
  Bnd_Range aRange(3.0, 15.0);
  EXPECT_EQ(aRange.IsIntersected(20.0), Bnd_Range::IntersectStatus_Out);
  EXPECT_EQ(aRange.IsIntersected(1.0), Bnd_Range::IntersectStatus_Out);
}

TEST(Bnd_RangeTest, IsIntersected_OnBoundary)
{
  Bnd_Range aRange(3.0, 15.0);
  EXPECT_EQ(aRange.IsIntersected(3.0), Bnd_Range::IntersectStatus_Boundary);
  EXPECT_EQ(aRange.IsIntersected(15.0), Bnd_Range::IntersectStatus_Boundary);
}

TEST(Bnd_RangeTest, IsIntersected_PointRange)
{
  // When myFirst == myLast, only Out or Boundary possible
  Bnd_Range aRange(5.0, 5.0);
  EXPECT_EQ(aRange.IsIntersected(5.0), Bnd_Range::IntersectStatus_Boundary);
  EXPECT_EQ(aRange.IsIntersected(3.0), Bnd_Range::IntersectStatus_Out);
}

TEST(Bnd_RangeTest, IsIntersected_Periodic_Contains)
{
  // Range [3, 15], period 4, val 1
  // Check: val + k*period in [3, 15]
  // k=1: 1+4=5, inside [3,15] → In
  Bnd_Range aRange(3.0, 15.0);
  EXPECT_EQ(aRange.IsIntersected(1.0, 4.0), Bnd_Range::IntersectStatus_In);
}

TEST(Bnd_RangeTest, IsIntersected_Periodic_OnBoundary)
{
  // Range [3, 15], period 4, val 3 → val + 0*4 = 3 exactly
  Bnd_Range aRange(3.0, 15.0);
  EXPECT_EQ(aRange.IsIntersected(3.0, 4.0), Bnd_Range::IntersectStatus_Boundary);
}

TEST(Bnd_RangeTest, IsIntersected_Periodic_Outside)
{
  // Range [3.5, 3.9], period 4, val 0
  // Check: 0 + k*4 in [3.5, 3.9] → k=1 gives 4.0, not in range
  Bnd_Range aRange(3.5, 3.9);
  EXPECT_EQ(aRange.IsIntersected(0.0, 4.0), Bnd_Range::IntersectStatus_Out);
}

TEST(Bnd_RangeTest, Split_NoIntersection)
{
  Bnd_Range                   aRange(3.0, 15.0);
  NCollection_List<Bnd_Range> aList;
  aRange.Split(20.0, aList);
  EXPECT_EQ(aList.Size(), 1);
  double aMin = 0.0, aMax = 0.0;
  EXPECT_TRUE(aList.First().GetBounds(aMin, aMax));
  EXPECT_DOUBLE_EQ(aMin, 3.0);
  EXPECT_DOUBLE_EQ(aMax, 15.0);
}

TEST(Bnd_RangeTest, Split_AtInteriorPoint)
{
  Bnd_Range                   aRange(3.0, 15.0);
  NCollection_List<Bnd_Range> aList;
  aRange.Split(5.0, aList);
  EXPECT_EQ(aList.Size(), 2);
  double aMin1 = 0.0, aMax1 = 0.0;
  EXPECT_TRUE(aList.First().GetBounds(aMin1, aMax1));
  EXPECT_DOUBLE_EQ(aMin1, 3.0);
  EXPECT_DOUBLE_EQ(aMax1, 5.0);
  double aMin2 = 0.0, aMax2 = 0.0;
  EXPECT_TRUE(aList.Last().GetBounds(aMin2, aMax2));
  EXPECT_DOUBLE_EQ(aMin2, 5.0);
  EXPECT_DOUBLE_EQ(aMax2, 15.0);
}

TEST(Bnd_RangeTest, Split_Periodic)
{
  // Range [3, 15], split at val=5, period=4
  // Split points: 5, 9, 13 → sub-ranges: [3,5], [5,9], [9,13], [13,15]
  Bnd_Range                   aRange(3.0, 15.0);
  NCollection_List<Bnd_Range> aList;
  aRange.Split(5.0, aList, 4.0);
  EXPECT_EQ(aList.Size(), 4);
}

TEST(Bnd_RangeTest, Enlarge)
{
  Bnd_Range aRange(5.0, 10.0);
  aRange.Enlarge(2.0);
  double aMin = 0.0, aMax = 0.0;
  EXPECT_TRUE(aRange.GetBounds(aMin, aMax));
  EXPECT_DOUBLE_EQ(aMin, 3.0);
  EXPECT_DOUBLE_EQ(aMax, 12.0);
}

TEST(Bnd_RangeTest, Enlarge_Void)
{
  Bnd_Range aRange;
  aRange.Enlarge(2.0);
  EXPECT_TRUE(aRange.IsVoid());
}

TEST(Bnd_RangeTest, Shifted)
{
  Bnd_Range aRange(3.0, 7.0);
  Bnd_Range aShifted = aRange.Shifted(10.0);
  double    aMin = 0.0, aMax = 0.0;
  EXPECT_TRUE(aShifted.GetBounds(aMin, aMax));
  EXPECT_DOUBLE_EQ(aMin, 13.0);
  EXPECT_DOUBLE_EQ(aMax, 17.0);
  // Original unchanged
  EXPECT_TRUE(aRange.GetBounds(aMin, aMax));
  EXPECT_DOUBLE_EQ(aMin, 3.0);
  EXPECT_DOUBLE_EQ(aMax, 7.0);
}

TEST(Bnd_RangeTest, Shifted_Void)
{
  Bnd_Range aRange;
  Bnd_Range aShifted = aRange.Shifted(10.0);
  EXPECT_TRUE(aShifted.IsVoid());
}

TEST(Bnd_RangeTest, Shift)
{
  Bnd_Range aRange(3.0, 7.0);
  aRange.Shift(10.0);
  double aMin = 0.0, aMax = 0.0;
  EXPECT_TRUE(aRange.GetBounds(aMin, aMax));
  EXPECT_DOUBLE_EQ(aMin, 13.0);
  EXPECT_DOUBLE_EQ(aMax, 17.0);
}

TEST(Bnd_RangeTest, TrimFrom)
{
  Bnd_Range aRange(3.0, 10.0);
  aRange.TrimFrom(5.0);
  double aMin = 0.0, aMax = 0.0;
  EXPECT_TRUE(aRange.GetBounds(aMin, aMax));
  EXPECT_DOUBLE_EQ(aMin, 5.0);
  EXPECT_DOUBLE_EQ(aMax, 10.0);
}

TEST(Bnd_RangeTest, TrimFrom_MakesVoid)
{
  Bnd_Range aRange(3.0, 10.0);
  aRange.TrimFrom(15.0);
  EXPECT_TRUE(aRange.IsVoid());
}

TEST(Bnd_RangeTest, TrimTo)
{
  Bnd_Range aRange(3.0, 10.0);
  aRange.TrimTo(7.0);
  double aMin = 0.0, aMax = 0.0;
  EXPECT_TRUE(aRange.GetBounds(aMin, aMax));
  EXPECT_DOUBLE_EQ(aMin, 3.0);
  EXPECT_DOUBLE_EQ(aMax, 7.0);
}

TEST(Bnd_RangeTest, TrimTo_MakesVoid)
{
  Bnd_Range aRange(3.0, 10.0);
  aRange.TrimTo(1.0);
  EXPECT_TRUE(aRange.IsVoid());
}

TEST(Bnd_RangeTest, IsOut_Value)
{
  Bnd_Range aRange(3.0, 10.0);
  EXPECT_FALSE(aRange.IsOut(5.0));
  EXPECT_FALSE(aRange.IsOut(3.0));
  EXPECT_FALSE(aRange.IsOut(10.0));
  EXPECT_TRUE(aRange.IsOut(1.0));
  EXPECT_TRUE(aRange.IsOut(15.0));
}

TEST(Bnd_RangeTest, IsOut_Void)
{
  Bnd_Range aRange;
  EXPECT_TRUE(aRange.IsOut(5.0));
}

TEST(Bnd_RangeTest, IsOut_Range)
{
  Bnd_Range aRange(3.0, 10.0);
  EXPECT_FALSE(aRange.IsOut(Bnd_Range(5.0, 8.0)));
  EXPECT_FALSE(aRange.IsOut(Bnd_Range(1.0, 5.0)));
  EXPECT_TRUE(aRange.IsOut(Bnd_Range(11.0, 15.0)));
}

TEST(Bnd_RangeTest, IsOut_Range_Void)
{
  Bnd_Range aRange(3.0, 10.0);
  Bnd_Range aVoid;
  EXPECT_TRUE(aRange.IsOut(aVoid));
  EXPECT_TRUE(aVoid.IsOut(aRange));
}

TEST(Bnd_RangeTest, Equality)
{
  Bnd_Range aRange1(3.0, 10.0);
  Bnd_Range aRange2(3.0, 10.0);
  Bnd_Range aRange3(3.0, 11.0);
  EXPECT_TRUE(aRange1 == aRange2);
  EXPECT_FALSE(aRange1 == aRange3);
}

TEST(Bnd_RangeTest, Contains_Value)
{
  Bnd_Range aRange(3.0, 10.0);
  EXPECT_TRUE(aRange.Contains(5.0));
  EXPECT_TRUE(aRange.Contains(3.0));
  EXPECT_TRUE(aRange.Contains(10.0));
  EXPECT_FALSE(aRange.Contains(2.0));
  EXPECT_FALSE(aRange.Contains(11.0));

  Bnd_Range aVoid;
  EXPECT_FALSE(aVoid.Contains(5.0));
}

TEST(Bnd_RangeTest, Intersects_Range)
{
  Bnd_Range aRange1(3.0, 10.0);
  Bnd_Range aRange2(5.0, 15.0);
  Bnd_Range aRange3(11.0, 20.0);
  EXPECT_TRUE(aRange1.Intersects(aRange2));
  EXPECT_FALSE(aRange1.Intersects(aRange3));

  Bnd_Range aVoid;
  EXPECT_FALSE(aRange1.Intersects(aVoid));
}

TEST(Bnd_RangeTest, Min_Max_DirectAccess)
{
  Bnd_Range aRange(3.0, 10.0);
  ASSERT_TRUE(aRange.Min().has_value());
  ASSERT_TRUE(aRange.Max().has_value());
  EXPECT_DOUBLE_EQ(*aRange.Min(), 3.0);
  EXPECT_DOUBLE_EQ(*aRange.Max(), 10.0);
}

TEST(Bnd_RangeTest, Min_Max_NulloptOnVoid)
{
  Bnd_Range aVoid;
  EXPECT_FALSE(aVoid.Min().has_value());
  EXPECT_FALSE(aVoid.Max().has_value());
}

TEST(Bnd_RangeTest, Get_StructuredBindings)
{
  Bnd_Range aRange(3.0, 10.0);
  auto      aBounds = aRange.Get();
  ASSERT_TRUE(aBounds.has_value());
  const auto [aMin, aMax] = *aBounds;
  EXPECT_DOUBLE_EQ(aMin, 3.0);
  EXPECT_DOUBLE_EQ(aMax, 10.0);
}

TEST(Bnd_RangeTest, Get_NulloptOnVoid)
{
  Bnd_Range aVoid;
  EXPECT_FALSE(aVoid.Get().has_value());
}

TEST(Bnd_RangeTest, Center)
{
  Bnd_Range aRange(2.0, 8.0);
  ASSERT_TRUE(aRange.Center().has_value());
  EXPECT_DOUBLE_EQ(*aRange.Center(), 5.0);

  Bnd_Range aPoint(3.0, 3.0);
  ASSERT_TRUE(aPoint.Center().has_value());
  EXPECT_DOUBLE_EQ(*aPoint.Center(), 3.0);
}

TEST(Bnd_RangeTest, Center_NulloptOnVoid)
{
  Bnd_Range aVoid;
  EXPECT_FALSE(aVoid.Center().has_value());
}
