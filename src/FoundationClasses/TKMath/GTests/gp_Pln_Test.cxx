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
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

TEST(gp_PlnTest, ConstructorFromAx3)
{
  gp_Ax3 anAx3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Pln aPln(anAx3);
  EXPECT_TRUE(aPln.Location().IsEqual(gp_Pnt(0.0, 0.0, 0.0), Precision::Confusion()));
  EXPECT_TRUE(aPln.Axis().Direction().IsEqual(gp_Dir(0.0, 0.0, 1.0), Precision::Angular()));
}

TEST(gp_PlnTest, ConstructorFromPointAndDir)
{
  gp_Pnt aLoc(1.0, 2.0, 3.0);
  gp_Dir aNorm(0.0, 0.0, 1.0);
  gp_Pln aPln(aLoc, aNorm);
  EXPECT_TRUE(aPln.Location().IsEqual(aLoc, Precision::Confusion()));
  EXPECT_TRUE(aPln.Axis().Direction().IsEqual(aNorm, Precision::Angular()));
}

TEST(gp_PlnTest, ConstructorFromCoefficients)
{
  // Plane Z = 5 => 0*X + 0*Y + 1*Z - 5 = 0
  gp_Pln aPln(0.0, 0.0, 1.0, -5.0);
  double anA, aB, aC, aD;
  aPln.Coefficients(anA, aB, aC, aD);
  EXPECT_NEAR(anA, 0.0, Precision::Confusion());
  EXPECT_NEAR(aB, 0.0, Precision::Confusion());
  EXPECT_NEAR(aC, 1.0, Precision::Confusion());
  EXPECT_NEAR(aD, -5.0, Precision::Confusion());
}

TEST(gp_PlnTest, DistanceToPoint)
{
  gp_Pln aPln(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Pnt aPnt(3.0, 4.0, 5.0);
  EXPECT_NEAR(aPln.Distance(aPnt), 5.0, Precision::Confusion());
}

TEST(gp_PlnTest, Contains)
{
  gp_Pln aPln(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Pnt aOnPlane(3.0, 4.0, 0.0);
  gp_Pnt aOffPlane(3.0, 4.0, 1.0);
  EXPECT_TRUE(aPln.Contains(aOnPlane, Precision::Confusion()));
  EXPECT_FALSE(aPln.Contains(aOffPlane, Precision::Confusion()));
}

TEST(gp_PlnTest, Coefficients)
{
  gp_Pln aPln(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  double anA, aB, aC, aD;
  aPln.Coefficients(anA, aB, aC, aD);
  EXPECT_NEAR(anA, 0.0, Precision::Confusion());
  EXPECT_NEAR(aB, 0.0, Precision::Confusion());
  EXPECT_NEAR(aC, 1.0, Precision::Confusion());
  EXPECT_NEAR(aD, 0.0, Precision::Confusion());
}

TEST(gp_PlnTest, Mirror)
{
  gp_Pln aPln(gp_Pnt(0.0, 0.0, 5.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Ax2 aMirrorPlane(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Pln aMirrored = aPln.Mirrored(aMirrorPlane);
  EXPECT_NEAR(aMirrored.Location().Z(), -5.0, Precision::Confusion());
}

TEST(gp_PlnTest, Rotate)
{
  gp_Pln aPln(gp_Pnt(1.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Ax1 anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Pln aRotated = aPln.Rotated(anAxis, M_PI_2);
  EXPECT_NEAR(aRotated.Location().X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aRotated.Location().Y(), 1.0, Precision::Confusion());
}

TEST(gp_PlnTest, Scale)
{
  gp_Pln aPln(gp_Pnt(1.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Pnt aCenter(0.0, 0.0, 0.0);
  gp_Pln aScaled = aPln.Scaled(aCenter, 3.0);
  EXPECT_NEAR(aScaled.Location().X(), 3.0, Precision::Confusion());
}

TEST(gp_PlnTest, Translate)
{
  gp_Pln aPln(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Vec aVec(1.0, 2.0, 3.0);
  gp_Pln aTranslated = aPln.Translated(aVec);
  EXPECT_NEAR(aTranslated.Location().X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aTranslated.Location().Y(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aTranslated.Location().Z(), 3.0, Precision::Confusion());
}

TEST(gp_PlnTest, Transform)
{
  gp_Pln  aPln(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(0.0, 0.0, 10.0));
  gp_Pln aTransformed = aPln.Transformed(aTrsf);
  EXPECT_NEAR(aTransformed.Location().Z(), 10.0, Precision::Confusion());
}
