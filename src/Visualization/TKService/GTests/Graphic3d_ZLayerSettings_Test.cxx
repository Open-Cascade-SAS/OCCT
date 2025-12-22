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

#include <gtest/gtest.h>

#include <Graphic3d_ZLayerSettings.hxx>

#include <sstream>

//==================================================================================================
// Default Constructor Tests
//==================================================================================================

TEST(Graphic3d_ZLayerSettingsTest, DefaultConstructor_IsVisibleTrue)
{
  Graphic3d_ZLayerSettings aSettings;
  EXPECT_TRUE(aSettings.IsVisible()) << "Default visibility should be TRUE";
}

TEST(Graphic3d_ZLayerSettingsTest, DefaultConstructor_IsImmediateFalse)
{
  Graphic3d_ZLayerSettings aSettings;
  EXPECT_FALSE(aSettings.IsImmediate()) << "Default IsImmediate should be FALSE";
}

TEST(Graphic3d_ZLayerSettingsTest, DefaultConstructor_IsRaytracableTrue)
{
  Graphic3d_ZLayerSettings aSettings;
  EXPECT_TRUE(aSettings.IsRaytracable()) << "Default IsRaytracable should be TRUE";
}

TEST(Graphic3d_ZLayerSettingsTest, DefaultConstructor_DepthTestEnabled)
{
  Graphic3d_ZLayerSettings aSettings;
  EXPECT_TRUE(aSettings.ToEnableDepthTest()) << "Default depth test should be enabled";
}

TEST(Graphic3d_ZLayerSettingsTest, DefaultConstructor_DepthWriteEnabled)
{
  Graphic3d_ZLayerSettings aSettings;
  EXPECT_TRUE(aSettings.ToEnableDepthWrite()) << "Default depth write should be enabled";
}

//==================================================================================================
// Visibility Tests
//==================================================================================================

TEST(Graphic3d_ZLayerSettingsTest, SetVisible_False)
{
  Graphic3d_ZLayerSettings aSettings;
  aSettings.SetVisible(Standard_False);
  EXPECT_FALSE(aSettings.IsVisible()) << "Visibility should be FALSE after SetVisible(false)";
}

TEST(Graphic3d_ZLayerSettingsTest, SetVisible_True)
{
  Graphic3d_ZLayerSettings aSettings;
  aSettings.SetVisible(Standard_False);
  aSettings.SetVisible(Standard_True);
  EXPECT_TRUE(aSettings.IsVisible()) << "Visibility should be TRUE after SetVisible(true)";
}

TEST(Graphic3d_ZLayerSettingsTest, SetVisible_Toggle)
{
  Graphic3d_ZLayerSettings aSettings;

  // Initially visible
  EXPECT_TRUE(aSettings.IsVisible());

  // Hide
  aSettings.SetVisible(Standard_False);
  EXPECT_FALSE(aSettings.IsVisible());

  // Show again
  aSettings.SetVisible(Standard_True);
  EXPECT_TRUE(aSettings.IsVisible());

  // Hide again
  aSettings.SetVisible(Standard_False);
  EXPECT_FALSE(aSettings.IsVisible());
}

//==================================================================================================
// Other Property Tests
//==================================================================================================

TEST(Graphic3d_ZLayerSettingsTest, SetName)
{
  Graphic3d_ZLayerSettings aSettings;
  aSettings.SetName("TestLayer");
  EXPECT_STREQ("TestLayer", aSettings.Name().ToCString()) << "Name should match set value";
}

TEST(Graphic3d_ZLayerSettingsTest, SetImmediate)
{
  Graphic3d_ZLayerSettings aSettings;
  aSettings.SetImmediate(Standard_True);
  EXPECT_TRUE(aSettings.IsImmediate()) << "IsImmediate should be TRUE after SetImmediate(true)";
}

TEST(Graphic3d_ZLayerSettingsTest, SetRaytracable)
{
  Graphic3d_ZLayerSettings aSettings;
  aSettings.SetRaytracable(Standard_False);
  EXPECT_FALSE(aSettings.IsRaytracable())
    << "IsRaytracable should be FALSE after SetRaytracable(false)";
}

TEST(Graphic3d_ZLayerSettingsTest, SetEnableDepthTest)
{
  Graphic3d_ZLayerSettings aSettings;
  aSettings.SetEnableDepthTest(Standard_False);
  EXPECT_FALSE(aSettings.ToEnableDepthTest())
    << "Depth test should be disabled after SetEnableDepthTest(false)";
}

TEST(Graphic3d_ZLayerSettingsTest, SetEnableDepthWrite)
{
  Graphic3d_ZLayerSettings aSettings;
  aSettings.SetEnableDepthWrite(Standard_False);
  EXPECT_FALSE(aSettings.ToEnableDepthWrite())
    << "Depth write should be disabled after SetEnableDepthWrite(false)";
}

//==================================================================================================
// DumpJson Tests
//==================================================================================================

TEST(Graphic3d_ZLayerSettingsTest, DumpJson_ContainsVisibility)
{
  Graphic3d_ZLayerSettings aSettings;
  aSettings.SetVisible(Standard_True);

  std::ostringstream aStream;
  aSettings.DumpJson(aStream);

  const std::string aJsonStr = aStream.str();
  // Note: DumpFieldToName strips the "my" prefix, so "myVisible" becomes "Visible"
  EXPECT_NE(std::string::npos, aJsonStr.find("Visible")) << "DumpJson should contain Visible field";
}

TEST(Graphic3d_ZLayerSettingsTest, DumpJson_VisibilityValueTrue)
{
  Graphic3d_ZLayerSettings aSettings;
  aSettings.SetVisible(Standard_True);

  std::ostringstream aStream;
  aSettings.DumpJson(aStream);

  const std::string aJsonStr = aStream.str();
  // Check that Visible appears with value 1 (true)
  // Note: DumpFieldToName strips the "my" prefix
  EXPECT_NE(std::string::npos, aJsonStr.find("\"Visible\": 1"))
    << "DumpJson should contain Visible: 1 when visible";
}

TEST(Graphic3d_ZLayerSettingsTest, DumpJson_VisibilityValueFalse)
{
  Graphic3d_ZLayerSettings aSettings;
  aSettings.SetVisible(Standard_False);

  std::ostringstream aStream;
  aSettings.DumpJson(aStream);

  const std::string aJsonStr = aStream.str();
  // Note: DumpFieldToName strips the "my" prefix
  EXPECT_NE(std::string::npos, aJsonStr.find("\"Visible\": 0"))
    << "DumpJson should contain Visible: 0 when hidden";
}
