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

#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

TEST(gp_LinTest, ConstructorFromAx1)
{
  gp_Ax1 anAx1(gp_Pnt(1.0, 2.0, 3.0), gp_Dir(1.0, 0.0, 0.0));
  gp_Lin aLin(anAx1);
  EXPECT_TRUE(aLin.Location().IsEqual(gp_Pnt(1.0, 2.0, 3.0), Precision::Confusion()));
  EXPECT_TRUE(aLin.Direction().IsEqual(gp_Dir(1.0, 0.0, 0.0), Precision::Angular()));
}

TEST(gp_LinTest, ConstructorFromPointAndDir)
{
  gp_Lin aLin(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  EXPECT_TRUE(aLin.Location().IsEqual(gp_Pnt(0.0, 0.0, 0.0), Precision::Confusion()));
  EXPECT_TRUE(aLin.Direction().IsEqual(gp_Dir(0.0, 0.0, 1.0), Precision::Angular()));
}

TEST(gp_LinTest, DistanceToPoint)
{
  gp_Lin aLin(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  gp_Pnt aPnt(5.0, 3.0, 4.0);
  // Distance from point to X axis: sqrt(3^2 + 4^2) = 5
  EXPECT_NEAR(aLin.Distance(aPnt), 5.0, Precision::Confusion());
}

TEST(gp_LinTest, DistanceToLine_Parallel)
{
  gp_Lin aL1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  gp_Lin aL2(gp_Pnt(0.0, 3.0, 4.0), gp_Dir(1.0, 0.0, 0.0));
  EXPECT_NEAR(aL1.Distance(aL2), 5.0, Precision::Confusion());
}

TEST(gp_LinTest, DistanceToLine_Intersecting)
{
  gp_Lin aL1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  gp_Lin aL2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0));
  EXPECT_NEAR(aL1.Distance(aL2), 0.0, Precision::Confusion());
}

TEST(gp_LinTest, Contains)
{
  gp_Lin aLin(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  gp_Pnt aOnLine(5.0, 0.0, 0.0);
  gp_Pnt aOffLine(5.0, 1.0, 0.0);
  EXPECT_TRUE(aLin.Contains(aOnLine, Precision::Confusion()));
  EXPECT_FALSE(aLin.Contains(aOffLine, Precision::Confusion()));
}

TEST(gp_LinTest, Angle)
{
  gp_Lin aL1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  gp_Lin aL2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0));
  EXPECT_NEAR(aL1.Angle(aL2), M_PI_2, Precision::Angular());
}

TEST(gp_LinTest, Translate)
{
  gp_Lin aLin(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  gp_Vec aVec(0.0, 5.0, 0.0);
  gp_Lin aTranslated = aLin.Translated(aVec);
  EXPECT_NEAR(aTranslated.Location().Y(), 5.0, Precision::Confusion());
}

TEST(gp_LinTest, Rotate)
{
  gp_Lin aLin(gp_Pnt(1.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  gp_Ax1 anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Lin aRotated = aLin.Rotated(anAxis, M_PI_2);
  EXPECT_NEAR(aRotated.Location().X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aRotated.Location().Y(), 1.0, Precision::Confusion());
  EXPECT_TRUE(aRotated.Direction().IsEqual(gp_Dir(0.0, 1.0, 0.0), Precision::Angular()));
}

TEST(gp_LinTest, Mirror)
{
  gp_Lin aLin(gp_Pnt(1.0, 1.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  gp_Ax1 anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  gp_Lin aMirrored = aLin.Mirrored(anAxis);
  EXPECT_NEAR(aMirrored.Location().Y(), -1.0, Precision::Confusion());
}

TEST(gp_LinTest, Transform)
{
  gp_Lin  aLin(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(0.0, 0.0, 10.0));
  gp_Lin aTransformed = aLin.Transformed(aTrsf);
  EXPECT_NEAR(aTransformed.Location().Z(), 10.0, Precision::Confusion());
}
