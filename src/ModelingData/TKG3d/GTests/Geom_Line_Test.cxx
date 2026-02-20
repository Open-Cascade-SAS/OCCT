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

#include <Geom_Line.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

TEST(Geom_LineTest, ConstructFromAx1)
{
  gp_Ax1                 anAx1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  occ::handle<Geom_Line> aLine = new Geom_Line(anAx1);
  ASSERT_FALSE(aLine.IsNull());

  gp_Pnt aLoc = aLine->Lin().Location();
  EXPECT_TRUE(aLoc.IsEqual(gp_Pnt(0.0, 0.0, 0.0), Precision::Confusion()));
}

TEST(Geom_LineTest, ConstructFromPointAndDir)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(1.0, 2.0, 3.0), gp_Dir(0.0, 0.0, 1.0));
  ASSERT_FALSE(aLine.IsNull());

  gp_Pnt aLoc = aLine->Lin().Location();
  EXPECT_TRUE(aLoc.IsEqual(gp_Pnt(1.0, 2.0, 3.0), Precision::Confusion()));
  EXPECT_TRUE(aLine->Lin().Direction().IsEqual(gp_Dir(0.0, 0.0, 1.0), Precision::Angular()));
}

TEST(Geom_LineTest, D0Evaluation)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  gp_Pnt                 aPnt;
  aLine->D0(5.0, aPnt);
  EXPECT_NEAR(aPnt.X(), 5.0, Precision::Confusion());
  EXPECT_NEAR(aPnt.Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aPnt.Z(), 0.0, Precision::Confusion());
}

TEST(Geom_LineTest, D1Evaluation)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0));
  gp_Pnt                 aPnt;
  gp_Vec                 aV1;
  aLine->D1(3.0, aPnt, aV1);
  EXPECT_NEAR(aPnt.Y(), 3.0, Precision::Confusion());
  // First derivative is the direction vector (constant for a line)
  EXPECT_NEAR(aV1.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aV1.Y(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aV1.Z(), 0.0, Precision::Confusion());
}

TEST(Geom_LineTest, D2Evaluation)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  gp_Pnt                 aPnt;
  gp_Vec                 aV1, aV2;
  aLine->D2(1.0, aPnt, aV1, aV2);
  // Second derivative of a line is zero
  EXPECT_NEAR(aV2.Magnitude(), 0.0, Precision::Confusion());
}

TEST(Geom_LineTest, InfiniteParameters)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  EXPECT_EQ(aLine->FirstParameter(), -Precision::Infinite());
  EXPECT_EQ(aLine->LastParameter(), Precision::Infinite());
}

TEST(Geom_LineTest, Reverse)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  aLine->Reverse();
  EXPECT_TRUE(aLine->Lin().Direction().IsEqual(gp_Dir(-1.0, 0.0, 0.0), Precision::Angular()));
}

TEST(Geom_LineTest, Reversed)
{
  occ::handle<Geom_Line>  aLine     = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  occ::handle<Geom_Curve> aReversed = aLine->Reversed();
  ASSERT_FALSE(aReversed.IsNull());
  occ::handle<Geom_Line> aRevLine = occ::down_cast<Geom_Line>(aReversed);
  ASSERT_FALSE(aRevLine.IsNull());
  EXPECT_TRUE(aRevLine->Lin().Direction().IsEqual(gp_Dir(-1.0, 0.0, 0.0), Precision::Angular()));
}

TEST(Geom_LineTest, Transform)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  gp_Trsf                aTrsf;
  aTrsf.SetTranslation(gp_Vec(0.0, 0.0, 5.0));
  aLine->Transform(aTrsf);
  EXPECT_NEAR(aLine->Lin().Location().Z(), 5.0, Precision::Confusion());
}

TEST(Geom_LineTest, Copy)
{
  occ::handle<Geom_Line>     aLine = new Geom_Line(gp_Pnt(1.0, 2.0, 3.0), gp_Dir(0.0, 0.0, 1.0));
  occ::handle<Geom_Geometry> aCopy = aLine->Copy();
  ASSERT_FALSE(aCopy.IsNull());
  occ::handle<Geom_Line> aCopyLine = occ::down_cast<Geom_Line>(aCopy);
  ASSERT_FALSE(aCopyLine.IsNull());
  EXPECT_TRUE(aCopyLine->Lin().Location().IsEqual(gp_Pnt(1.0, 2.0, 3.0), Precision::Confusion()));
}
