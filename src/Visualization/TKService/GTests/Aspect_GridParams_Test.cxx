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

#include <Aspect_GridParams.hxx>
#include <Precision.hxx>
#include <Quantity_Color.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

TEST(Aspect_GridParamsTest, Defaults_AreReasonable)
{
  Aspect_GridParams aParams;
  EXPECT_EQ(Quantity_NOC_GRAY70, aParams.Color().Name()) << "default color is grey";
  EXPECT_DOUBLE_EQ(0.01, aParams.Scale());
  EXPECT_DOUBLE_EQ(0.0, aParams.ScaleY()) << "default ScaleY is 0 (isotropic sentinel)";
  EXPECT_DOUBLE_EQ(0.01, aParams.EffectiveScaleY()) << "isotropic: EffectiveScaleY mirrors Scale";
  EXPECT_DOUBLE_EQ(0.01, aParams.LineThickness());
  EXPECT_DOUBLE_EQ(0.0, aParams.RotationAngle());
  EXPECT_EQ(0, aParams.AngularDivisions()) << "default grid is rectangular";
  EXPECT_FALSE(aParams.IsCircular());
  EXPECT_EQ(Aspect_GDM_Lines, aParams.DrawMode());
  EXPECT_FALSE(aParams.IsBackground());
  EXPECT_TRUE(aParams.IsDrawAxis());
  EXPECT_FALSE(aParams.IsInfinity());
  EXPECT_NEAR(0.0, aParams.Origin().X(), Precision::Confusion());
  EXPECT_NEAR(0.0, aParams.Origin().Y(), Precision::Confusion());
  EXPECT_NEAR(0.0, aParams.Origin().Z(), Precision::Confusion());
}

TEST(Aspect_GridParamsTest, DrawMode_RoundTrip)
{
  Aspect_GridParams aParams;
  aParams.SetDrawMode(Aspect_GDM_Points);
  EXPECT_EQ(Aspect_GDM_Points, aParams.DrawMode());
  aParams.SetDrawMode(Aspect_GDM_None);
  EXPECT_EQ(Aspect_GDM_None, aParams.DrawMode());
  aParams.SetDrawMode(Aspect_GDM_Lines);
  EXPECT_EQ(Aspect_GDM_Lines, aParams.DrawMode());
}

TEST(Aspect_GridParamsTest, AngularDivisions_ToggleCircular)
{
  Aspect_GridParams aParams;
  EXPECT_FALSE(aParams.IsCircular());

  aParams.SetAngularDivisions(8);
  EXPECT_EQ(8, aParams.AngularDivisions());
  EXPECT_TRUE(aParams.IsCircular());

  aParams.SetAngularDivisions(0);
  EXPECT_FALSE(aParams.IsCircular()) << "zero divisions falls back to rectangular";
}

TEST(Aspect_GridParamsTest, EffectiveScaleY_FollowsScaleWhenZero)
{
  Aspect_GridParams aParams;
  aParams.SetScale(0.25);
  EXPECT_DOUBLE_EQ(0.25, aParams.EffectiveScaleY());
  aParams.SetScaleY(0.5);
  EXPECT_DOUBLE_EQ(0.5, aParams.EffectiveScaleY());
  aParams.SetScaleY(0.0);
  EXPECT_DOUBLE_EQ(0.25, aParams.EffectiveScaleY()) << "clearing ScaleY restores isotropy";
}

TEST(Aspect_GridParamsTest, RotationAngle_RoundTrip)
{
  Aspect_GridParams aParams;
  aParams.SetRotationAngle(0.5);
  EXPECT_DOUBLE_EQ(0.5, aParams.RotationAngle());
  aParams.SetRotationAngle(-1.25);
  EXPECT_DOUBLE_EQ(-1.25, aParams.RotationAngle());
}

TEST(Aspect_GridParamsTest, Setters_RoundTrip)
{
  Aspect_GridParams aParams;

  const Quantity_Color aBlue(Quantity_NOC_BLUE1);
  aParams.SetColor(aBlue);
  EXPECT_EQ(aBlue, aParams.Color());

  const gp_Pnt aOrigin(1.0, 2.0, 3.0);
  aParams.SetOrigin(aOrigin);
  EXPECT_NEAR(1.0, aParams.Origin().X(), Precision::Confusion());
  EXPECT_NEAR(2.0, aParams.Origin().Y(), Precision::Confusion());
  EXPECT_NEAR(3.0, aParams.Origin().Z(), Precision::Confusion());

  aParams.SetScale(0.5);
  EXPECT_DOUBLE_EQ(0.5, aParams.Scale());

  aParams.SetScaleY(0.75);
  EXPECT_DOUBLE_EQ(0.75, aParams.ScaleY());
  EXPECT_DOUBLE_EQ(0.75, aParams.EffectiveScaleY());

  aParams.SetLineThickness(0.02);
  EXPECT_DOUBLE_EQ(0.02, aParams.LineThickness());

  aParams.SetRotationAngle(0.123);
  EXPECT_DOUBLE_EQ(0.123, aParams.RotationAngle());

  aParams.SetAngularDivisions(12);
  EXPECT_EQ(12, aParams.AngularDivisions());
  EXPECT_TRUE(aParams.IsCircular());

  aParams.SetIsBackground(true);
  EXPECT_TRUE(aParams.IsBackground());

  aParams.SetIsDrawAxis(false);
  EXPECT_FALSE(aParams.IsDrawAxis());

  aParams.SetIsInfinity(true);
  EXPECT_TRUE(aParams.IsInfinity());
}

TEST(Aspect_GridParamsTest, Copy_PreservesFields)
{
  Aspect_GridParams aSrc;
  aSrc.SetColor(Quantity_Color(0.1, 0.2, 0.3, Quantity_TOC_RGB));
  aSrc.SetOrigin(gp_Pnt(4.0, 5.0, 6.0));
  aSrc.SetScale(0.25);
  aSrc.SetScaleY(0.125);
  aSrc.SetLineThickness(0.04);
  aSrc.SetRotationAngle(-0.5);
  aSrc.SetAngularDivisions(16);
  aSrc.SetIsBackground(true);
  aSrc.SetIsDrawAxis(false);
  aSrc.SetIsInfinity(true);

  const Aspect_GridParams aCopy = aSrc;
  EXPECT_NEAR(0.1, aCopy.Color().Red(), Precision::Confusion());
  EXPECT_NEAR(0.2, aCopy.Color().Green(), Precision::Confusion());
  EXPECT_NEAR(0.3, aCopy.Color().Blue(), Precision::Confusion());
  EXPECT_NEAR(4.0, aCopy.Origin().X(), Precision::Confusion());
  EXPECT_NEAR(5.0, aCopy.Origin().Y(), Precision::Confusion());
  EXPECT_NEAR(6.0, aCopy.Origin().Z(), Precision::Confusion());
  EXPECT_DOUBLE_EQ(0.25, aCopy.Scale());
  EXPECT_DOUBLE_EQ(0.125, aCopy.ScaleY());
  EXPECT_DOUBLE_EQ(0.125, aCopy.EffectiveScaleY());
  EXPECT_DOUBLE_EQ(0.04, aCopy.LineThickness());
  EXPECT_DOUBLE_EQ(-0.5, aCopy.RotationAngle());
  EXPECT_EQ(16, aCopy.AngularDivisions());
  EXPECT_TRUE(aCopy.IsCircular());
  EXPECT_TRUE(aCopy.IsBackground());
  EXPECT_FALSE(aCopy.IsDrawAxis());
  EXPECT_TRUE(aCopy.IsInfinity());
}
