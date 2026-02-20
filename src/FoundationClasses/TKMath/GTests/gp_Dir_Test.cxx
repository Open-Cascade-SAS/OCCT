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
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>

#include <gtest/gtest.h>

TEST(gp_DirTest, CoordinateConstructor)
{
  gp_Dir aDir(0.0, 0.0, 1.0);
  EXPECT_NEAR(aDir.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aDir.Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aDir.Z(), 1.0, Precision::Confusion());
}

TEST(gp_DirTest, XYZConstructor)
{
  gp_XYZ aXYZ(0.0, 3.0, 4.0);
  gp_Dir aDir(aXYZ);
  EXPECT_NEAR(aDir.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aDir.Y(), 0.6, Precision::Confusion());
  EXPECT_NEAR(aDir.Z(), 0.8, Precision::Confusion());
}

TEST(gp_DirTest, VecConstructor)
{
  gp_Vec aVec(3.0, 0.0, 0.0);
  gp_Dir aDir(aVec);
  EXPECT_NEAR(aDir.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aDir.Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aDir.Z(), 0.0, Precision::Confusion());
}

TEST(gp_DirTest, PredefinedDirections)
{
  EXPECT_TRUE(gp_Dir(1.0, 0.0, 0.0).IsEqual(gp_Dir(gp_Dir::D::X), Precision::Angular()));
  EXPECT_TRUE(gp_Dir(0.0, 1.0, 0.0).IsEqual(gp_Dir(gp_Dir::D::Y), Precision::Angular()));
  EXPECT_TRUE(gp_Dir(0.0, 0.0, 1.0).IsEqual(gp_Dir(gp_Dir::D::Z), Precision::Angular()));
}

TEST(gp_DirTest, Angle)
{
  gp_Dir aD1(1.0, 0.0, 0.0);
  gp_Dir aD2(0.0, 1.0, 0.0);
  EXPECT_NEAR(aD1.Angle(aD2), M_PI_2, Precision::Angular());
}

TEST(gp_DirTest, AngleWithRef)
{
  gp_Dir aD1(1.0, 0.0, 0.0);
  gp_Dir aD2(0.0, 1.0, 0.0);
  gp_Dir aRef(0.0, 0.0, 1.0);
  double anAngle = aD1.AngleWithRef(aD2, aRef);
  EXPECT_NEAR(anAngle, M_PI_2, Precision::Angular());

  // Reverse reference direction should negate the angle
  double anAngleReversed = aD1.AngleWithRef(aD2, gp_Dir(0.0, 0.0, -1.0));
  EXPECT_NEAR(anAngleReversed, -M_PI_2, Precision::Angular());
}

TEST(gp_DirTest, IsEqual)
{
  gp_Dir aD1(1.0, 0.0, 0.0);
  gp_Dir aD2(1.0, 0.0, 0.0);
  EXPECT_TRUE(aD1.IsEqual(aD2, Precision::Angular()));
}

TEST(gp_DirTest, IsNormal)
{
  gp_Dir aD1(1.0, 0.0, 0.0);
  gp_Dir aD2(0.0, 1.0, 0.0);
  EXPECT_TRUE(aD1.IsNormal(aD2, Precision::Angular()));
}

TEST(gp_DirTest, IsOpposite)
{
  gp_Dir aD1(1.0, 0.0, 0.0);
  gp_Dir aD2(-1.0, 0.0, 0.0);
  EXPECT_TRUE(aD1.IsOpposite(aD2, Precision::Angular()));
}

TEST(gp_DirTest, IsParallel)
{
  gp_Dir aD1(1.0, 0.0, 0.0);
  gp_Dir aD2(-1.0, 0.0, 0.0);
  EXPECT_TRUE(aD1.IsParallel(aD2, Precision::Angular()));
}

TEST(gp_DirTest, Cross)
{
  gp_Dir aD1(1.0, 0.0, 0.0);
  gp_Dir aD2(0.0, 1.0, 0.0);
  gp_Dir aCross = aD1.Crossed(aD2);
  EXPECT_NEAR(aCross.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aCross.Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aCross.Z(), 1.0, Precision::Confusion());
}

TEST(gp_DirTest, CrossCross)
{
  gp_Dir aA(1.0, 0.0, 0.0);
  gp_Dir aB(1.0, 1.0, 0.0);
  gp_Dir aC(0.0, 0.0, 1.0);
  gp_Dir aCrossCross = aA.CrossCrossed(aB, aC);
  // Result should be a valid normalized direction
  double aMag = std::sqrt(aCrossCross.X() * aCrossCross.X()
                          + aCrossCross.Y() * aCrossCross.Y()
                          + aCrossCross.Z() * aCrossCross.Z());
  EXPECT_NEAR(aMag, 1.0, Precision::Confusion());
}

TEST(gp_DirTest, Dot)
{
  gp_Dir aD1(1.0, 0.0, 0.0);
  gp_Dir aD2(0.0, 1.0, 0.0);
  EXPECT_NEAR(aD1.Dot(aD2), 0.0, Precision::Confusion());

  gp_Dir aD3(1.0, 0.0, 0.0);
  EXPECT_NEAR(aD1.Dot(aD3), 1.0, Precision::Confusion());
}

TEST(gp_DirTest, Mirror_Point)
{
  gp_Dir aDir(1.0, 0.0, 0.0);
  gp_Ax1 anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0));
  gp_Dir aMirrored = aDir.Mirrored(anAxis);
  // Mirror X direction through Y axis: result should be (-1,0,0)...
  // Actually mirroring direction (1,0,0) through axis Y means reflecting about the Y axis line
  // The component along Y stays, the perpendicular component reverses: result is (-1,0,0)
  // But for axis mirror of a direction, it reflects in the axis:
  // d_mirror = 2*(d.axis)*axis - d
  // d.axis = (1,0,0).(0,1,0) = 0
  // result = 2*0*(0,1,0) - (1,0,0) = (-1,0,0)
  EXPECT_NEAR(aMirrored.X(), -1.0, Precision::Confusion());
  EXPECT_NEAR(aMirrored.Y(), 0.0, Precision::Confusion());
}

TEST(gp_DirTest, Rotate)
{
  gp_Dir aDir(1.0, 0.0, 0.0);
  gp_Ax1 anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Dir aRotated = aDir.Rotated(anAxis, M_PI_2);
  EXPECT_NEAR(aRotated.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aRotated.Y(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aRotated.Z(), 0.0, Precision::Confusion());
}

TEST(gp_DirTest, Transform)
{
  gp_Dir  aDir(1.0, 0.0, 0.0);
  gp_Trsf aTrsf;
  aTrsf.SetRotation(gp_Ax1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), M_PI);
  gp_Dir aTransformed = aDir.Transformed(aTrsf);
  EXPECT_NEAR(aTransformed.X(), -1.0, Precision::Confusion());
  EXPECT_NEAR(aTransformed.Y(), 0.0, Precision::Confusion());
}

TEST(gp_DirTest, ZeroMagnitudeInput_ThrowsException)
{
  EXPECT_THROW(gp_Dir aDir(0.0, 0.0, 0.0), Standard_ConstructionError);
}
