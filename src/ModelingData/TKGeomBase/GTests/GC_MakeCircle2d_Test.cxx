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

#include <GC_MakeCircle2d.hxx>
#include <Precision.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>

#include <gtest/gtest.h>

TEST(GC_MakeCircle2dTest, FromCircAndPoint_Done)
{
  const gp_Circ2d aBase(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);
  const gp_Pnt2d  aPoint(0.0, 10.0);

  GC_MakeCircle2d aMaker(aBase, aPoint);
  ASSERT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);
  EXPECT_NEAR(aMaker.Value()->Circ2d().Radius(), 10.0, Precision::Confusion());
}

TEST(GC_MakeCircle2dTest, FromCircAndDist_Done)
{
  const gp_Circ2d aBase(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);

  GC_MakeCircle2d aMaker(aBase, -2.0);
  ASSERT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);
  EXPECT_NEAR(aMaker.Value()->Circ2d().Radius(), 3.0, Precision::Confusion());
}

TEST(GC_MakeCircle2dTest, FromCircAndPoint_AtCenter_DoneZeroRadius)
{
  const gp_Circ2d aBase(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 5.0);

  GC_MakeCircle2d aMaker(aBase, gp_Pnt2d(0.0, 0.0));
  ASSERT_TRUE(aMaker.IsDone());
  EXPECT_EQ(aMaker.Status(), gce_Done);
  ASSERT_FALSE(aMaker.Value().IsNull());
  EXPECT_NEAR(aMaker.Value()->Circ2d().Radius(), 0.0, Precision::Confusion());
}
