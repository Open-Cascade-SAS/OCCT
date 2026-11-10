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

#include <OpenGl_Text.hxx>
#include <Graphic3d_Text.hxx>
#include <NCollection_Vec3.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Macro.hxx>
#include <gtest/gtest.h>

// This test exercises the deprecated text conversion entry point.
Standard_DISABLE_DEPRECATION_WARNINGS

  TEST(OpenGl_TextTest, Init_InvalidUtf8_ReportsFailureWithoutChangingText)
{
  OpenGl_Text aText;
  aText.Text()->SetText("unchanged");
  aText.Set2D(true);
  const NCollection_Vec3<float> aPoint(1.0f, 2.0f, 3.0f);

  EXPECT_THROW(aText.Init(nullptr, "\xC0\xAF", aPoint), Standard_DomainError);
  EXPECT_EQ(aText.Text()->Text().View(), "unchanged");
  EXPECT_TRUE(aText.Is2D());

  aText.Init(nullptr, "\xC3\xA9", aPoint);
  EXPECT_EQ(aText.Text()->Text().View(), "\xC3\xA9");
  EXPECT_FALSE(aText.Is2D());
  EXPECT_DOUBLE_EQ(aText.Text()->Position().X(), 1.0);
  EXPECT_DOUBLE_EQ(aText.Text()->Position().Y(), 2.0);
  EXPECT_DOUBLE_EQ(aText.Text()->Position().Z(), 3.0);

  aText.Init(nullptr, nullptr, aPoint);
  EXPECT_TRUE(aText.Text()->Text().IsEmpty());
}

Standard_ENABLE_DEPRECATION_WARNINGS
