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

#include <GCE2d_MakeSegment.hxx>
#include <gp.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>

#include <gtest/gtest.h>

TEST(GCE2d_MakeSegmentTest, FromPoints_BelowResolution_ConfusedPoints)
{
  const gp_Pnt2d aP1(0.0, 0.0);
  const gp_Pnt2d aP2(0.5 * gp::Resolution(), 0.0);

  GCE2d_MakeSegment aMaker(aP1, aP2);
  EXPECT_FALSE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_ConfusedPoints);
}

TEST(GCE2d_MakeSegmentTest, FromPoints_AboveResolution_Done)
{
  const gp_Pnt2d aP1(0.0, 0.0);
  const gp_Pnt2d aP2(2.0 * gp::Resolution(), 0.0);

  GCE2d_MakeSegment aMaker(aP1, aP2);
  EXPECT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);
}

TEST(GCE2d_MakeSegmentTest, FromPoints_AtResolution_ConfusedPoints)
{
  const gp_Pnt2d aP1(0.0, 0.0);
  const gp_Pnt2d aP2(gp::Resolution(), 0.0);

  GCE2d_MakeSegment aMaker(aP1, aP2);
  EXPECT_FALSE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_ConfusedPoints);
}

TEST(GCE2d_MakeSegmentTest, FromPointDir_BelowResolutionParameter_ConfusedPoints)
{
  const gp_Pnt2d aP1(0.0, 0.0);
  const gp_Dir2d aDir(1.0, 0.0);
  const gp_Pnt2d aP2(0.5 * gp::Resolution(), 3.0);

  GCE2d_MakeSegment aMaker(aP1, aDir, aP2);
  EXPECT_FALSE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_ConfusedPoints);
}

TEST(GCE2d_MakeSegmentTest, FromPointDir_AboveResolutionParameter_Done)
{
  const gp_Pnt2d aP1(0.0, 0.0);
  const gp_Dir2d aDir(1.0, 0.0);
  const gp_Pnt2d aP2(2.0 * gp::Resolution(), 3.0);

  GCE2d_MakeSegment aMaker(aP1, aDir, aP2);
  EXPECT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);
}

TEST(GCE2d_MakeSegmentTest, FromPointDir_AtResolutionParameter_ConfusedPoints)
{
  const gp_Pnt2d aP1(0.0, 0.0);
  const gp_Dir2d aDir(1.0, 0.0);
  const gp_Pnt2d aP2(gp::Resolution(), 5.0);

  GCE2d_MakeSegment aMaker(aP1, aDir, aP2);
  EXPECT_FALSE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_ConfusedPoints);
}
