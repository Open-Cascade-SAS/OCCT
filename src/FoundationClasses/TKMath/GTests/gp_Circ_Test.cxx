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
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

TEST(gp_CircTest, Constructor)
{
  gp_Ax2 anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Circ aCirc(anAxis, 5.0);
  EXPECT_NEAR(aCirc.Radius(), 5.0, Precision::Confusion());
}

TEST(gp_CircTest, Area)
{
  gp_Ax2 anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Circ aCirc(anAxis, 1.0);
  EXPECT_NEAR(aCirc.Area(), M_PI, Precision::Confusion());
}

TEST(gp_CircTest, Length)
{
  gp_Ax2 anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Circ aCirc(anAxis, 1.0);
  EXPECT_NEAR(aCirc.Length(), 2.0 * M_PI, Precision::Confusion());
}

TEST(gp_CircTest, Center)
{
  gp_Pnt aCenter(1.0, 2.0, 3.0);
  gp_Ax2 anAxis(aCenter, gp_Dir(0.0, 0.0, 1.0));
  gp_Circ aCirc(anAxis, 5.0);
  EXPECT_TRUE(aCirc.Location().IsEqual(aCenter, Precision::Confusion()));
}

TEST(gp_CircTest, Translate)
{
  gp_Ax2 anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Circ aCirc(anAxis, 5.0);
  gp_Vec aVec(10.0, 20.0, 30.0);
  gp_Circ aTranslated = aCirc.Translated(aVec);
  EXPECT_NEAR(aTranslated.Location().X(), 10.0, Precision::Confusion());
  EXPECT_NEAR(aTranslated.Location().Y(), 20.0, Precision::Confusion());
  EXPECT_NEAR(aTranslated.Location().Z(), 30.0, Precision::Confusion());
  EXPECT_NEAR(aTranslated.Radius(), 5.0, Precision::Confusion());
}

TEST(gp_CircTest, Rotate)
{
  gp_Ax2 anAxis(gp_Pnt(1.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Circ aCirc(anAxis, 2.0);
  gp_Ax1 aRotAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Circ aRotated = aCirc.Rotated(aRotAxis, M_PI_2);
  EXPECT_NEAR(aRotated.Location().X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aRotated.Location().Y(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aRotated.Radius(), 2.0, Precision::Confusion());
}

TEST(gp_CircTest, Mirror)
{
  gp_Ax2 anAxis(gp_Pnt(1.0, 2.0, 3.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Circ aCirc(anAxis, 5.0);
  gp_Pnt aMirrorPnt(0.0, 0.0, 0.0);
  gp_Circ aMirrored = aCirc.Mirrored(aMirrorPnt);
  EXPECT_NEAR(aMirrored.Location().X(), -1.0, Precision::Confusion());
  EXPECT_NEAR(aMirrored.Location().Y(), -2.0, Precision::Confusion());
  EXPECT_NEAR(aMirrored.Location().Z(), -3.0, Precision::Confusion());
}

TEST(gp_CircTest, Transform)
{
  gp_Ax2 anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Circ aCirc(anAxis, 3.0);
  gp_Trsf aTrsf;
  aTrsf.SetScale(gp_Pnt(0.0, 0.0, 0.0), 2.0);
  gp_Circ aTransformed = aCirc.Transformed(aTrsf);
  EXPECT_NEAR(aTransformed.Radius(), 6.0, Precision::Confusion());
}
