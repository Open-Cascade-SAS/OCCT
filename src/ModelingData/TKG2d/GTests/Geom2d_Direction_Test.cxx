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

#include <Geom2d_Direction.hxx>
#include <Geom2d_VectorWithMagnitude.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Trsf2d.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>

#include <gtest/gtest.h>

#include <cmath>

TEST(Geom2d_DirectionTest, ConstructFromCoords)
{
  occ::handle<Geom2d_Direction> aDir = new Geom2d_Direction(3.0, 4.0);
  // Should be normalized
  EXPECT_NEAR(aDir->Magnitude(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aDir->X(), 0.6, Precision::Confusion());
  EXPECT_NEAR(aDir->Y(), 0.8, Precision::Confusion());
}

TEST(Geom2d_DirectionTest, ConstructFromDir2d)
{
  occ::handle<Geom2d_Direction> aDir = new Geom2d_Direction(gp_Dir2d(0.0, 1.0));
  EXPECT_NEAR(aDir->X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aDir->Y(), 1.0, Precision::Confusion());
}

TEST(Geom2d_DirectionTest, ConstructFromZero_Throws)
{
#ifndef No_Exception
  EXPECT_THROW(new Geom2d_Direction(0.0, 0.0), Standard_ConstructionError);
#else
  GTEST_SKIP() << "No_Exception is defined; exception behavior is disabled in this build.";
#endif
}

TEST(Geom2d_DirectionTest, MagnitudeAlwaysOne)
{
  occ::handle<Geom2d_Direction> aDir = new Geom2d_Direction(100.0, 200.0);
  EXPECT_NEAR(aDir->Magnitude(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aDir->SquareMagnitude(), 1.0, Precision::Confusion());
}

TEST(Geom2d_DirectionTest, SetCoord)
{
  occ::handle<Geom2d_Direction> aDir = new Geom2d_Direction(1.0, 0.0);
  aDir->SetCoord(0.0, 5.0);
  EXPECT_NEAR(aDir->X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aDir->Y(), 1.0, Precision::Confusion());
}

TEST(Geom2d_DirectionTest, SetCoord_Zero_Throws)
{
#ifndef No_Exception
  occ::handle<Geom2d_Direction> aDir = new Geom2d_Direction(1.0, 0.0);
  EXPECT_THROW(aDir->SetCoord(0.0, 0.0), Standard_ConstructionError);
#else
  GTEST_SKIP() << "No_Exception is defined; exception behavior is disabled in this build.";
#endif
}

TEST(Geom2d_DirectionTest, Dir2d)
{
  occ::handle<Geom2d_Direction> aDir   = new Geom2d_Direction(1.0, 0.0);
  const gp_Dir2d                aGpDir = aDir->Dir2d();
  EXPECT_NEAR(aGpDir.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aGpDir.Y(), 0.0, Precision::Confusion());
}

TEST(Geom2d_DirectionTest, Crossed)
{
  occ::handle<Geom2d_Direction> aDirX = new Geom2d_Direction(1.0, 0.0);
  occ::handle<Geom2d_Direction> aDirY = new Geom2d_Direction(0.0, 1.0);

  // (1,0) x (0,1) = 1
  EXPECT_NEAR(aDirX->Crossed(aDirY), 1.0, Precision::Confusion());
  // (0,1) x (1,0) = -1
  EXPECT_NEAR(aDirY->Crossed(aDirX), -1.0, Precision::Confusion());
}

TEST(Geom2d_DirectionTest, Angle)
{
  occ::handle<Geom2d_Direction> aDirX = new Geom2d_Direction(1.0, 0.0);
  occ::handle<Geom2d_Direction> aDirY = new Geom2d_Direction(0.0, 1.0);

  EXPECT_NEAR(aDirX->Angle(aDirY), M_PI / 2.0, Precision::Confusion());
}

TEST(Geom2d_DirectionTest, Angle_OppositeDirections)
{
  occ::handle<Geom2d_Direction> aDirPos = new Geom2d_Direction(1.0, 0.0);
  occ::handle<Geom2d_Direction> aDirNeg = new Geom2d_Direction(-1.0, 0.0);

  EXPECT_NEAR(std::abs(aDirPos->Angle(aDirNeg)), M_PI, Precision::Confusion());
}

TEST(Geom2d_DirectionTest, Dot)
{
  occ::handle<Geom2d_Direction> aDirX = new Geom2d_Direction(1.0, 0.0);
  occ::handle<Geom2d_Direction> aDirY = new Geom2d_Direction(0.0, 1.0);

  EXPECT_NEAR(aDirX->Dot(aDirY), 0.0, Precision::Confusion());
  EXPECT_NEAR(aDirX->Dot(aDirX), 1.0, Precision::Confusion());
}

TEST(Geom2d_DirectionTest, Reverse)
{
  occ::handle<Geom2d_Direction> aDir = new Geom2d_Direction(1.0, 0.0);
  aDir->Reverse();
  EXPECT_NEAR(aDir->X(), -1.0, Precision::Confusion());
  EXPECT_NEAR(aDir->Y(), 0.0, Precision::Confusion());
}

TEST(Geom2d_DirectionTest, CrossedWithVectorWithMagnitude)
{
  occ::handle<Geom2d_Direction>           aDir = new Geom2d_Direction(1.0, 0.0);
  occ::handle<Geom2d_VectorWithMagnitude> aVec = new Geom2d_VectorWithMagnitude(0.0, 5.0);

  EXPECT_NEAR(aDir->Crossed(aVec), 5.0, Precision::Confusion());
}

TEST(Geom2d_DirectionTest, Copy)
{
  occ::handle<Geom2d_Direction> aDir      = new Geom2d_Direction(1.0, 0.0);
  occ::handle<Geom2d_Geometry>  aCopyGeom = aDir->Copy();
  occ::handle<Geom2d_Direction> aCopy     = occ::down_cast<Geom2d_Direction>(aCopyGeom);

  ASSERT_FALSE(aCopy.IsNull());
  EXPECT_NEAR(aCopy->X(), 1.0, Precision::Confusion());

  aCopy->SetCoord(0.0, 1.0);
  EXPECT_NEAR(aDir->X(), 1.0, Precision::Confusion());
}

TEST(Geom2d_DirectionTest, Transform_Rotation)
{
  gp_Trsf2d aTrsf;
  aTrsf.SetRotation(gp_Pnt2d(0.0, 0.0), M_PI / 2.0);

  occ::handle<Geom2d_Direction> aDir = new Geom2d_Direction(1.0, 0.0);
  aDir->Transform(aTrsf);

  EXPECT_NEAR(aDir->X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aDir->Y(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aDir->Magnitude(), 1.0, Precision::Confusion());
}
