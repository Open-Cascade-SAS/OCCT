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

#include <Font_FontMgr.hxx>

#include <gtest/gtest.h>

TEST(Font_FontMgrTest, EmbeddedFallbackDataIsAvailable)
{
  const occ::handle<NCollection_Buffer> aBuffer = Font_FontMgr::EmbedFallbackFont();
  ASSERT_FALSE(aBuffer.IsNull());
  EXPECT_GT(aBuffer->Size(), 0u);
}

TEST(Font_FontMgrTest, StrictMissingFamilyIsNotResolved)
{
  const occ::handle<Font_FontMgr> aManager = Font_FontMgr::GetInstance();
  ASSERT_FALSE(aManager.IsNull());
  Font_FontAspect anAspect = Font_FontAspect_Regular;
  EXPECT_TRUE(aManager
                ->FindFont("OCCT-Definitely-Missing-Font-Family-4A72C3",
                           Font_StrictLevel_Strict,
                           anAspect,
                           false)
                .IsNull());
}

TEST(Font_FontMgrTest, ResolvedFontProvidesUsablePath)
{
  const occ::handle<Font_FontMgr> aManager = Font_FontMgr::GetInstance();
  ASSERT_FALSE(aManager.IsNull());
  Font_FontAspect                    anAspect = Font_FontAspect_Regular;
  const occ::handle<Font_SystemFont> aFont =
    aManager->FindFont("", Font_StrictLevel_Any, anAspect, false);
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "No system font is registered";
  }

  bool                          toSynthesizeItalic = false;
  int                           aFaceId            = 0;
  const TCollection_AsciiString aPath = aFont->FontPathAny(anAspect, toSynthesizeItalic, aFaceId);
  EXPECT_FALSE(aPath.IsEmpty());
}
