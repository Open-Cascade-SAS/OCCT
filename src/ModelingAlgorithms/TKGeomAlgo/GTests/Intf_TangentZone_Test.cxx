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

#include <Intf_SectionPoint.hxx>
#include <Intf_TangentZone.hxx>
#include <NCollection_Array1.hxx>

#include <gtest/gtest.h>

namespace
{
Intf_SectionPoint MakePoint(double theX)
{
  return Intf_SectionPoint(gp_Pnt(theX, 0.0, 0.0),
                           Intf_VERTEX,
                           1,
                           0,
                           0.0,
                           Intf_VERTEX,
                           1,
                           0,
                           0.0,
                           0.0);
}
} // namespace

// Points() must return the same points, in the same order, as GetPoint(Index).
TEST(Intf_TangentZone, Points_MatchesGetPoint)
{
  Intf_TangentZone aZone;
  for (int i = 0; i < 5; ++i)
  {
    aZone.Append(MakePoint(static_cast<double>(i)));
  }

  const NCollection_Array1<Intf_SectionPoint>& aPts = aZone.Points();
  ASSERT_EQ(aPts.Length(), aZone.NumberOfPoints());
  for (int i = 1; i <= aZone.NumberOfPoints(); ++i)
  {
    EXPECT_TRUE(aPts(i).IsEqual(aZone.GetPoint(i))) << "index " << i;
  }
}

// The cache must reflect Append() done after the first Points() call.
TEST(Intf_TangentZone, Points_InvalidatedByAppend)
{
  Intf_TangentZone aZone;
  aZone.Append(MakePoint(0.0));
  EXPECT_EQ(aZone.Points().Length(), 1);

  aZone.Append(MakePoint(1.0));
  const NCollection_Array1<Intf_SectionPoint>& aPts = aZone.Points();
  ASSERT_EQ(aPts.Length(), 2);
  EXPECT_TRUE(aPts(2).IsEqual(aZone.GetPoint(2)));
}

// The cache must reflect InsertBefore() done after the first Points() call.
TEST(Intf_TangentZone, Points_InvalidatedByInsertBefore)
{
  Intf_TangentZone aZone;
  aZone.Append(MakePoint(0.0));
  aZone.Append(MakePoint(2.0));
  EXPECT_EQ(aZone.Points().Length(), 2);

  aZone.InsertBefore(2, MakePoint(1.0));
  const NCollection_Array1<Intf_SectionPoint>& aPts = aZone.Points();
  ASSERT_EQ(aPts.Length(), 3);
  for (int i = 1; i <= 3; ++i)
  {
    EXPECT_TRUE(aPts(i).IsEqual(aZone.GetPoint(i))) << "index " << i;
  }
}

// An empty zone has an empty (not stale) Points() array.
TEST(Intf_TangentZone, Points_EmptyZone)
{
  Intf_TangentZone aZone;
  EXPECT_EQ(aZone.Points().Length(), 0);
}
