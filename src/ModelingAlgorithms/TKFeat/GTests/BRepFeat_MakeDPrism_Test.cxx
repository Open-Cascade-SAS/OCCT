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

#include <BRepFeat_MakeDPrism.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gtest/gtest.h>
#include <cmath>

namespace
{
double volume(const TopoDS_Shape& theShape)
{
  GProp_GProps aProps;
  BRepGProp::VolumeProperties(theShape, aProps);
  return aProps.Mass();
}

TopoDS_Face planeAt(const double theZ)
{
  return BRepBuilderAPI_MakeFace(gp_Pln(gp_Pnt(0, 0, theZ), gp_Dir(0, 0, 1))).Face();
}

// Signed integral over normal height; the expanding side has rounded corners.
double draftVolume(const double theHeight, const double theAngle)
{
  const double aSlope        = std::tan(theAngle);
  const double aCornerFactor = theHeight < 0.0 ? M_PI : 4.0;
  return 36.0 * theHeight - 12.0 * aSlope * theHeight * theHeight
         + aCornerFactor / 3.0 * aSlope * aSlope * theHeight * theHeight * theHeight;
}
} // namespace

TEST(BRepFeat_MakeDPrism, SeparateFeatureTerminationModes)
{
  for (int aTermination = 0; aTermination < 5; ++aTermination)
  {
    SCOPED_TRACE(aTermination);
    const TopoDS_Shape aStock = BRepPrimAPI_MakeBox(20, 20, 10).Shape();
    const TopoDS_Face  aProfile =
      BRepBuilderAPI_MakeFace(gp_Pln(gp_Pnt(0, 0, 10), gp_Dir(0, 0, 1)), 3, 9, 3, 9).Face();
    BRepFeat_MakeDPrism aFeature(aStock, aProfile, TopoDS_Face(), 0.05, 2, false);
    double              anExpectedVolume = draftVolume(5.0, 0.05);
    switch (aTermination)
    {
      case 0:
        aFeature.Perform(5.0);
        break;
      case 1:
        aFeature.Perform(planeAt(15.0));
        break;
      case 2:
        aFeature.Perform(planeAt(11.0), planeAt(15.0));
        anExpectedVolume -= draftVolume(1.0, 0.05);
        break;
      case 3:
        aFeature.PerformUntilHeight(planeAt(15.0), 10.0);
        break;
      case 4:
        aFeature.PerformFromEnd(planeAt(15.0));
        anExpectedVolume -= draftVolume(-10.0, 0.05);
        break;
    }
    ASSERT_TRUE(aFeature.IsDone());
    ASSERT_FALSE(aFeature.Shape().IsNull());
    EXPECT_TRUE(BRepCheck_Analyzer(aFeature.Shape()).IsValid());
    EXPECT_NEAR(volume(aFeature.Shape()), anExpectedVolume, 1.e-6);
    EXPECT_NEAR(volume(aStock), 4000.0, 1.e-6);
    EXPECT_TRUE(BRepCheck_Analyzer(aStock).IsValid());
    EXPECT_TRUE(BRepCheck_Analyzer(aProfile).IsValid());
  }
}

TEST(BRepFeat_MakeDPrism, FuseAndCutModes)
{
  for (const int aMode : {0, 1})
  {
    SCOPED_TRACE(aMode);
    const TopoDS_Shape aStock = BRepPrimAPI_MakeBox(20, 20, 10).Shape();
    const double       aZ     = aMode == 0 ? 0.0 : 10.0;
    const TopoDS_Face  aProfile =
      BRepBuilderAPI_MakeFace(gp_Pln(gp_Pnt(0, 0, aZ), gp_Dir(0, 0, 1)), 3, 9, 3, 9).Face();
    BRepFeat_MakeDPrism aFeature(aStock, aProfile, TopoDS_Face(), 0.05, aMode, false);
    aFeature.Perform(5.0);
    ASSERT_TRUE(aFeature.IsDone());
    EXPECT_TRUE(BRepCheck_Analyzer(aFeature.Shape()).IsValid());
    EXPECT_NEAR(volume(aFeature.Shape()),
                4000.0 + (aMode == 0 ? -1.0 : 1.0) * draftVolume(5.0, 0.05),
                1.e-6);
    EXPECT_NEAR(volume(aStock), 4000.0, 1.e-6);
  }
}

TEST(BRepFeat_MakeDPrism, OutwardSeparateFeatureHasRoundedCornerVolume)
{
  const TopoDS_Shape aStock = BRepPrimAPI_MakeBox(20, 20, 10).Shape();
  const TopoDS_Face  aProfile =
    BRepBuilderAPI_MakeFace(gp_Pln(gp_Pnt(0, 0, 10), gp_Dir(0, 0, 1)), 3, 9, 3, 9).Face();
  BRepFeat_MakeDPrism aFeature(aStock, aProfile, TopoDS_Face(), -0.05, 2, false);
  const double        aHeight = 5.0;
  const double        aSlope  = std::tan(0.05);
  // The outward offset has four rounded corners, whose combined area is a disk.
  const double anExpectedVolume = 36.0 * aHeight + 12.0 * aSlope * aHeight * aHeight
                                  + M_PI / 3.0 * aSlope * aSlope * aHeight * aHeight * aHeight;
  aFeature.Perform(aHeight);
  ASSERT_TRUE(aFeature.IsDone());
  EXPECT_TRUE(BRepCheck_Analyzer(aFeature.Shape()).IsValid());
  EXPECT_NEAR(volume(aFeature.Shape()), anExpectedVolume, 1.e-6);
  EXPECT_NEAR(volume(aStock), 4000.0, 1.e-6);
}

TEST(BRepFeat_MakeDPrism, UntilHeightDelegatesOptionalLimits)
{
  for (const bool hasLimit : {false, true})
  {
    for (const int aMode : {0, 1, 2})
    {
      SCOPED_TRACE(hasLimit);
      SCOPED_TRACE(aMode);
      const TopoDS_Shape aStock = BRepPrimAPI_MakeBox(20, 20, 10).Shape();
      const double       aZ     = aMode == 0 ? 0.0 : 10.0;
      const TopoDS_Face  aProfile =
        BRepBuilderAPI_MakeFace(gp_Pln(gp_Pnt(0, 0, aZ), gp_Dir(0, 0, 1)), 3, 9, 3, 9).Face();
      BRepFeat_MakeDPrism aFeature(aStock, aProfile, TopoDS_Face(), 0.05, aMode, false);
      ASSERT_NO_THROW(aFeature.PerformUntilHeight(hasLimit ? planeAt(aZ + 5.0) : TopoDS_Shape(),
                                                  hasLimit ? 0.0 : 5.0));
      ASSERT_TRUE(aFeature.IsDone());
      EXPECT_TRUE(BRepCheck_Analyzer(aFeature.Shape()).IsValid());
      const double aFeatureVolume = draftVolume(5.0, 0.05);
      const double anExpectedVolume =
        aMode == 2 ? aFeatureVolume : 4000.0 + (aMode == 0 ? -aFeatureVolume : aFeatureVolume);
      EXPECT_NEAR(volume(aFeature.Shape()), anExpectedVolume, 1.e-6);
      EXPECT_NEAR(volume(aStock), 4000.0, 1.e-6);
    }
  }
}
