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
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>

#include <gtest/gtest.h>

#include <cmath>

class Geom2d_VectorWithMagnitudeTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    myVec1 = new Geom2d_VectorWithMagnitude(3.0, 4.0);
    myVec2 = new Geom2d_VectorWithMagnitude(1.0, 0.0);
  }

  occ::handle<Geom2d_VectorWithMagnitude> myVec1;
  occ::handle<Geom2d_VectorWithMagnitude> myVec2;
};

TEST_F(Geom2d_VectorWithMagnitudeTest, ConstructFromCoords)
{
  EXPECT_NEAR(myVec1->X(), 3.0, Precision::Confusion());
  EXPECT_NEAR(myVec1->Y(), 4.0, Precision::Confusion());
}

TEST_F(Geom2d_VectorWithMagnitudeTest, ConstructFromVec2d)
{
  occ::handle<Geom2d_VectorWithMagnitude> aVec =
    new Geom2d_VectorWithMagnitude(gp_Vec2d(5.0, 12.0));
  EXPECT_NEAR(aVec->Magnitude(), 13.0, Precision::Confusion());
}

TEST_F(Geom2d_VectorWithMagnitudeTest, ConstructFromTwoPoints)
{
  occ::handle<Geom2d_VectorWithMagnitude> aVec =
    new Geom2d_VectorWithMagnitude(gp_Pnt2d(1.0, 2.0), gp_Pnt2d(4.0, 6.0));
  EXPECT_NEAR(aVec->X(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aVec->Y(), 4.0, Precision::Confusion());
  EXPECT_NEAR(aVec->Magnitude(), 5.0, Precision::Confusion());
}

TEST_F(Geom2d_VectorWithMagnitudeTest, Magnitude)
{
  EXPECT_NEAR(myVec1->Magnitude(), 5.0, Precision::Confusion());
  EXPECT_NEAR(myVec2->Magnitude(), 1.0, Precision::Confusion());
}

TEST_F(Geom2d_VectorWithMagnitudeTest, SquareMagnitude)
{
  EXPECT_NEAR(myVec1->SquareMagnitude(), 25.0, Precision::Confusion());
}

TEST_F(Geom2d_VectorWithMagnitudeTest, Added)
{
  occ::handle<Geom2d_VectorWithMagnitude> aSum = myVec1->Added(myVec2);
  EXPECT_NEAR(aSum->X(), 4.0, Precision::Confusion());
  EXPECT_NEAR(aSum->Y(), 4.0, Precision::Confusion());
}

TEST_F(Geom2d_VectorWithMagnitudeTest, Subtracted)
{
  occ::handle<Geom2d_VectorWithMagnitude> aDiff = myVec1->Subtracted(myVec2);
  EXPECT_NEAR(aDiff->X(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aDiff->Y(), 4.0, Precision::Confusion());
}

TEST_F(Geom2d_VectorWithMagnitudeTest, Multiplied)
{
  occ::handle<Geom2d_VectorWithMagnitude> aScaled = myVec1->Multiplied(2.0);
  EXPECT_NEAR(aScaled->X(), 6.0, Precision::Confusion());
  EXPECT_NEAR(aScaled->Y(), 8.0, Precision::Confusion());
  EXPECT_NEAR(aScaled->Magnitude(), 10.0, Precision::Confusion());
}

TEST_F(Geom2d_VectorWithMagnitudeTest, Divided)
{
  occ::handle<Geom2d_VectorWithMagnitude> aScaled = myVec1->Divided(5.0);
  EXPECT_NEAR(aScaled->X(), 0.6, Precision::Confusion());
  EXPECT_NEAR(aScaled->Y(), 0.8, Precision::Confusion());
}

TEST_F(Geom2d_VectorWithMagnitudeTest, Normalize)
{
  occ::handle<Geom2d_VectorWithMagnitude> aVec = new Geom2d_VectorWithMagnitude(3.0, 4.0);
  aVec->Normalize();
  EXPECT_NEAR(aVec->Magnitude(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aVec->X(), 0.6, Precision::Confusion());
  EXPECT_NEAR(aVec->Y(), 0.8, Precision::Confusion());
}

TEST_F(Geom2d_VectorWithMagnitudeTest, Normalized)
{
  occ::handle<Geom2d_VectorWithMagnitude> aNorm = myVec1->Normalized();
  EXPECT_NEAR(aNorm->Magnitude(), 1.0, Precision::Confusion());
  // Original unchanged
  EXPECT_NEAR(myVec1->Magnitude(), 5.0, Precision::Confusion());
}

TEST_F(Geom2d_VectorWithMagnitudeTest, Crossed)
{
  // Cross product in 2D is scalar: (3,4) x (1,0) = 3*0 - 4*1 = -4
  const double aCross = myVec1->Crossed(myVec2);
  EXPECT_NEAR(aCross, -4.0, Precision::Confusion());
}

TEST_F(Geom2d_VectorWithMagnitudeTest, Dot)
{
  // (3,4) . (1,0) = 3
  const double aDot = myVec1->Dot(myVec2);
  EXPECT_NEAR(aDot, 3.0, Precision::Confusion());
}

TEST_F(Geom2d_VectorWithMagnitudeTest, Angle)
{
  // Angle from (1,0) to (0,1) = PI/2
  occ::handle<Geom2d_VectorWithMagnitude> aVecY   = new Geom2d_VectorWithMagnitude(0.0, 1.0);
  const double                            anAngle = myVec2->Angle(aVecY);
  EXPECT_NEAR(anAngle, M_PI / 2.0, Precision::Confusion());
}

TEST_F(Geom2d_VectorWithMagnitudeTest, Angle_Negative)
{
  // Angle from (0,1) to (1,0) = -PI/2
  occ::handle<Geom2d_VectorWithMagnitude> aVecY   = new Geom2d_VectorWithMagnitude(0.0, 1.0);
  const double                            anAngle = aVecY->Angle(myVec2);
  EXPECT_NEAR(anAngle, -M_PI / 2.0, Precision::Confusion());
}

TEST_F(Geom2d_VectorWithMagnitudeTest, Reverse)
{
  occ::handle<Geom2d_VectorWithMagnitude> aVec = new Geom2d_VectorWithMagnitude(3.0, 4.0);
  aVec->Reverse();
  EXPECT_NEAR(aVec->X(), -3.0, Precision::Confusion());
  EXPECT_NEAR(aVec->Y(), -4.0, Precision::Confusion());
}

TEST_F(Geom2d_VectorWithMagnitudeTest, SetCoord)
{
  myVec1->SetCoord(7.0, 8.0);
  EXPECT_NEAR(myVec1->X(), 7.0, Precision::Confusion());
  EXPECT_NEAR(myVec1->Y(), 8.0, Precision::Confusion());
}

TEST_F(Geom2d_VectorWithMagnitudeTest, Vec2d)
{
  const gp_Vec2d aVec = myVec1->Vec2d();
  EXPECT_NEAR(aVec.X(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aVec.Y(), 4.0, Precision::Confusion());
}

TEST_F(Geom2d_VectorWithMagnitudeTest, Copy)
{
  occ::handle<Geom2d_Geometry>            aCopyGeom = myVec1->Copy();
  occ::handle<Geom2d_VectorWithMagnitude> aCopy =
    occ::down_cast<Geom2d_VectorWithMagnitude>(aCopyGeom);

  ASSERT_FALSE(aCopy.IsNull());
  EXPECT_NEAR(aCopy->X(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aCopy->Y(), 4.0, Precision::Confusion());

  aCopy->SetCoord(0.0, 0.0);
  EXPECT_NEAR(myVec1->X(), 3.0, Precision::Confusion());
}

TEST_F(Geom2d_VectorWithMagnitudeTest, Add_InPlace)
{
  myVec1->Add(myVec2);
  EXPECT_NEAR(myVec1->X(), 4.0, Precision::Confusion());
  EXPECT_NEAR(myVec1->Y(), 4.0, Precision::Confusion());
}

TEST_F(Geom2d_VectorWithMagnitudeTest, Subtract_InPlace)
{
  myVec1->Subtract(myVec2);
  EXPECT_NEAR(myVec1->X(), 2.0, Precision::Confusion());
  EXPECT_NEAR(myVec1->Y(), 4.0, Precision::Confusion());
}

TEST_F(Geom2d_VectorWithMagnitudeTest, Multiply_InPlace)
{
  myVec1->Multiply(3.0);
  EXPECT_NEAR(myVec1->X(), 9.0, Precision::Confusion());
  EXPECT_NEAR(myVec1->Y(), 12.0, Precision::Confusion());
}

TEST_F(Geom2d_VectorWithMagnitudeTest, Divide_InPlace)
{
  myVec1->Divide(5.0);
  EXPECT_NEAR(myVec1->X(), 0.6, Precision::Confusion());
  EXPECT_NEAR(myVec1->Y(), 0.8, Precision::Confusion());
}

TEST_F(Geom2d_VectorWithMagnitudeTest, Transform_Rotation)
{
  gp_Trsf2d aTrsf;
  aTrsf.SetRotation(gp_Pnt2d(0.0, 0.0), M_PI / 2.0);

  occ::handle<Geom2d_VectorWithMagnitude> aVec = new Geom2d_VectorWithMagnitude(1.0, 0.0);
  aVec->Transform(aTrsf);

  EXPECT_NEAR(aVec->X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aVec->Y(), 1.0, Precision::Confusion());
}
