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

#include <Geom2d_Transformation.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_TrsfForm.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

#include <cmath>

TEST(Geom2d_TransformationTest, DefaultConstructor_Identity)
{
  occ::handle<Geom2d_Transformation> aTrsf = new Geom2d_Transformation();

  EXPECT_EQ(aTrsf->Form(), gp_Identity);
  EXPECT_NEAR(aTrsf->ScaleFactor(), 1.0, Precision::Confusion());
  EXPECT_FALSE(aTrsf->IsNegative());
}

TEST(Geom2d_TransformationTest, ConstructFromTrsf2d)
{
  gp_Trsf2d aGpTrsf;
  aGpTrsf.SetTranslation(gp_Vec2d(3.0, 4.0));

  occ::handle<Geom2d_Transformation> aTrsf = new Geom2d_Transformation(aGpTrsf);
  EXPECT_EQ(aTrsf->Form(), gp_Translation);
}

TEST(Geom2d_TransformationTest, SetTranslation_Vector)
{
  occ::handle<Geom2d_Transformation> aTrsf = new Geom2d_Transformation();
  aTrsf->SetTranslation(gp_Vec2d(5.0, 10.0));

  EXPECT_EQ(aTrsf->Form(), gp_Translation);

  double aX = 1.0, aY = 2.0;
  aTrsf->Transforms(aX, aY);
  EXPECT_NEAR(aX, 6.0, Precision::Confusion());
  EXPECT_NEAR(aY, 12.0, Precision::Confusion());
}

TEST(Geom2d_TransformationTest, SetTranslation_TwoPoints)
{
  occ::handle<Geom2d_Transformation> aTrsf = new Geom2d_Transformation();
  aTrsf->SetTranslation(gp_Pnt2d(1.0, 1.0), gp_Pnt2d(4.0, 5.0));

  double aX = 0.0, aY = 0.0;
  aTrsf->Transforms(aX, aY);
  EXPECT_NEAR(aX, 3.0, Precision::Confusion());
  EXPECT_NEAR(aY, 4.0, Precision::Confusion());
}

TEST(Geom2d_TransformationTest, SetRotation)
{
  occ::handle<Geom2d_Transformation> aTrsf = new Geom2d_Transformation();
  aTrsf->SetRotation(gp_Pnt2d(0.0, 0.0), M_PI / 2.0);

  EXPECT_EQ(aTrsf->Form(), gp_Rotation);

  double aX = 1.0, aY = 0.0;
  aTrsf->Transforms(aX, aY);
  EXPECT_NEAR(aX, 0.0, Precision::Confusion());
  EXPECT_NEAR(aY, 1.0, Precision::Confusion());
}

TEST(Geom2d_TransformationTest, SetRotation_180Degrees)
{
  occ::handle<Geom2d_Transformation> aTrsf = new Geom2d_Transformation();
  aTrsf->SetRotation(gp_Pnt2d(0.0, 0.0), M_PI);

  double aX = 3.0, aY = 4.0;
  aTrsf->Transforms(aX, aY);
  EXPECT_NEAR(aX, -3.0, Precision::Confusion());
  EXPECT_NEAR(aY, -4.0, Precision::Confusion());
}

TEST(Geom2d_TransformationTest, SetScale)
{
  occ::handle<Geom2d_Transformation> aTrsf = new Geom2d_Transformation();
  aTrsf->SetScale(gp_Pnt2d(0.0, 0.0), 2.0);

  EXPECT_EQ(aTrsf->Form(), gp_Scale);
  EXPECT_NEAR(aTrsf->ScaleFactor(), 2.0, Precision::Confusion());

  double aX = 3.0, aY = 4.0;
  aTrsf->Transforms(aX, aY);
  EXPECT_NEAR(aX, 6.0, Precision::Confusion());
  EXPECT_NEAR(aY, 8.0, Precision::Confusion());
}

TEST(Geom2d_TransformationTest, SetScale_Negative)
{
  occ::handle<Geom2d_Transformation> aTrsf = new Geom2d_Transformation();
  aTrsf->SetScale(gp_Pnt2d(0.0, 0.0), -1.0);

  // In 2D, IsNegative checks determinant sign; scale(-1) has det = (-1)*(-1) = 1 > 0
  EXPECT_FALSE(aTrsf->IsNegative());
  EXPECT_NEAR(aTrsf->ScaleFactor(), -1.0, Precision::Confusion());

  double aX = 3.0, aY = 4.0;
  aTrsf->Transforms(aX, aY);
  EXPECT_NEAR(aX, -3.0, Precision::Confusion());
  EXPECT_NEAR(aY, -4.0, Precision::Confusion());
}

TEST(Geom2d_TransformationTest, SetMirror_Point)
{
  occ::handle<Geom2d_Transformation> aTrsf = new Geom2d_Transformation();
  aTrsf->SetMirror(gp_Pnt2d(0.0, 0.0));

  EXPECT_EQ(aTrsf->Form(), gp_PntMirror);
  // In 2D, point mirror has det = (-1)*(-1) = 1 > 0, so IsNegative is false
  EXPECT_FALSE(aTrsf->IsNegative());

  double aX = 3.0, aY = 4.0;
  aTrsf->Transforms(aX, aY);
  EXPECT_NEAR(aX, -3.0, Precision::Confusion());
  EXPECT_NEAR(aY, -4.0, Precision::Confusion());
}

TEST(Geom2d_TransformationTest, SetMirror_Axis)
{
  // Mirror about X axis
  occ::handle<Geom2d_Transformation> aTrsf = new Geom2d_Transformation();
  aTrsf->SetMirror(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)));

  EXPECT_EQ(aTrsf->Form(), gp_Ax1Mirror);

  double aX = 3.0, aY = 4.0;
  aTrsf->Transforms(aX, aY);
  EXPECT_NEAR(aX, 3.0, Precision::Confusion());
  EXPECT_NEAR(aY, -4.0, Precision::Confusion());
}

TEST(Geom2d_TransformationTest, Inverted)
{
  occ::handle<Geom2d_Transformation> aTrsf = new Geom2d_Transformation();
  aTrsf->SetTranslation(gp_Vec2d(5.0, 10.0));

  occ::handle<Geom2d_Transformation> anInv = aTrsf->Inverted();

  // Applying both should be identity
  double aX = 7.0, aY = 3.0;
  aTrsf->Transforms(aX, aY);
  anInv->Transforms(aX, aY);
  EXPECT_NEAR(aX, 7.0, Precision::Confusion());
  EXPECT_NEAR(aY, 3.0, Precision::Confusion());
}

TEST(Geom2d_TransformationTest, Multiplied)
{
  // Translation + Rotation
  occ::handle<Geom2d_Transformation> aTrans = new Geom2d_Transformation();
  aTrans->SetTranslation(gp_Vec2d(1.0, 0.0));

  occ::handle<Geom2d_Transformation> aRot = new Geom2d_Transformation();
  aRot->SetRotation(gp_Pnt2d(0.0, 0.0), M_PI / 2.0);

  // aRot * aTrans: first translate, then rotate
  occ::handle<Geom2d_Transformation> aComposed = aRot->Multiplied(aTrans);

  double aX = 0.0, aY = 0.0;
  aComposed->Transforms(aX, aY);
  // (0,0) -> translate to (1,0) -> rotate 90deg to (0,1)
  EXPECT_NEAR(aX, 0.0, Precision::Confusion());
  EXPECT_NEAR(aY, 1.0, Precision::Confusion());
}

TEST(Geom2d_TransformationTest, Power_Zero_IsIdentity)
{
  occ::handle<Geom2d_Transformation> aTrsf = new Geom2d_Transformation();
  aTrsf->SetRotation(gp_Pnt2d(0.0, 0.0), M_PI / 4.0);
  aTrsf->Power(0);

  EXPECT_EQ(aTrsf->Form(), gp_Identity);
}

TEST(Geom2d_TransformationTest, Power_Positive)
{
  occ::handle<Geom2d_Transformation> aTrsf = new Geom2d_Transformation();
  aTrsf->SetRotation(gp_Pnt2d(0.0, 0.0), M_PI / 2.0);
  aTrsf->Power(2); // Two 90-degree rotations = 180 degrees

  double aX = 1.0, aY = 0.0;
  aTrsf->Transforms(aX, aY);
  EXPECT_NEAR(aX, -1.0, Precision::Confusion());
  EXPECT_NEAR(aY, 0.0, Precision::Confusion());
}

TEST(Geom2d_TransformationTest, Value_Matrix)
{
  occ::handle<Geom2d_Transformation> aTrsf = new Geom2d_Transformation();
  // Identity matrix values
  EXPECT_NEAR(aTrsf->Value(1, 1), 1.0, Precision::Confusion());
  EXPECT_NEAR(aTrsf->Value(1, 2), 0.0, Precision::Confusion());
  EXPECT_NEAR(aTrsf->Value(2, 1), 0.0, Precision::Confusion());
  EXPECT_NEAR(aTrsf->Value(2, 2), 1.0, Precision::Confusion());
}

TEST(Geom2d_TransformationTest, Trsf2d_RoundTrip)
{
  gp_Trsf2d aGpTrsf;
  aGpTrsf.SetRotation(gp_Pnt2d(1.0, 2.0), M_PI / 3.0);

  occ::handle<Geom2d_Transformation> aTrsf = new Geom2d_Transformation(aGpTrsf);
  gp_Trsf2d                          aBack = aTrsf->Trsf2d();

  // Verify the round-trip preserves the transformation
  double aX1 = 5.0, aY1 = 7.0;
  double aX2 = 5.0, aY2 = 7.0;
  aGpTrsf.Transforms(aX1, aY1);
  aBack.Transforms(aX2, aY2);
  EXPECT_NEAR(aX1, aX2, Precision::Confusion());
  EXPECT_NEAR(aY1, aY2, Precision::Confusion());
}

TEST(Geom2d_TransformationTest, Copy)
{
  occ::handle<Geom2d_Transformation> aTrsf = new Geom2d_Transformation();
  aTrsf->SetScale(gp_Pnt2d(0.0, 0.0), 3.0);

  occ::handle<Geom2d_Transformation> aCopy = aTrsf->Copy();
  EXPECT_NEAR(aCopy->ScaleFactor(), 3.0, Precision::Confusion());

  // Verify independence
  aCopy->SetScale(gp_Pnt2d(0.0, 0.0), 5.0);
  EXPECT_NEAR(aTrsf->ScaleFactor(), 3.0, Precision::Confusion());
}

TEST(Geom2d_TransformationTest, SetScale_WithCenter)
{
  // Scaling with center at (2, 3) by factor 2
  occ::handle<Geom2d_Transformation> aTrsf = new Geom2d_Transformation();
  aTrsf->SetScale(gp_Pnt2d(2.0, 3.0), 2.0);

  // The center should remain fixed
  double aX = 2.0, aY = 3.0;
  aTrsf->Transforms(aX, aY);
  EXPECT_NEAR(aX, 2.0, Precision::Confusion());
  EXPECT_NEAR(aY, 3.0, Precision::Confusion());

  // A different point should scale from the center
  aX = 4.0;
  aY = 3.0;
  aTrsf->Transforms(aX, aY);
  EXPECT_NEAR(aX, 6.0, Precision::Confusion()); // 2 + 2*(4-2) = 6
  EXPECT_NEAR(aY, 3.0, Precision::Confusion()); // 3 + 2*(3-3) = 3
}

TEST(Geom2d_TransformationTest, SetRotation_WithCenter)
{
  // 90-degree rotation about (1, 1)
  occ::handle<Geom2d_Transformation> aTrsf = new Geom2d_Transformation();
  aTrsf->SetRotation(gp_Pnt2d(1.0, 1.0), M_PI / 2.0);

  // Center stays fixed
  double aX = 1.0, aY = 1.0;
  aTrsf->Transforms(aX, aY);
  EXPECT_NEAR(aX, 1.0, Precision::Confusion());
  EXPECT_NEAR(aY, 1.0, Precision::Confusion());

  // (2, 1) relative to center is (1, 0), rotated 90 deg is (0, 1), so result is (1, 2)
  aX = 2.0;
  aY = 1.0;
  aTrsf->Transforms(aX, aY);
  EXPECT_NEAR(aX, 1.0, Precision::Confusion());
  EXPECT_NEAR(aY, 2.0, Precision::Confusion());
}
