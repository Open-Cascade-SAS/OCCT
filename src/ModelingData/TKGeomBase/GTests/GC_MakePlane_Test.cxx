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

#include <GC_MakePlane.hxx>
#include <Precision.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

TEST(GC_MakePlaneTest, FromPlaneAndDist_Done)
{
  const gp_Pln aBase(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));

  GC_MakePlane aMaker(aBase, 5.0);
  ASSERT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);
  EXPECT_NEAR(aMaker.Value()->Pln().Location().Z(), 5.0, Precision::Confusion());
}

TEST(GC_MakePlaneTest, FromPlaneAndPoint_Done)
{
  const gp_Pln aBase(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  const gp_Pnt aPoint(2.0, -3.0, 7.0);

  GC_MakePlane aMaker(aBase, aPoint);
  ASSERT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);
  EXPECT_TRUE(aMaker.Value()->Pln().Location().IsEqual(aPoint, Precision::Confusion()));
}

TEST(GC_MakePlaneTest, FromAxis_Done)
{
  const gp_Ax1 aAxis(gp_Pnt(1.0, 2.0, 3.0), gp_Dir(0.0, 1.0, 0.0));

  GC_MakePlane aMaker(aAxis);
  ASSERT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);
  EXPECT_TRUE(
    aMaker.Value()->Pln().Axis().Direction().IsParallel(aAxis.Direction(), Precision::Angular()));
}
