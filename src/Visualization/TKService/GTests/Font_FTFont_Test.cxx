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

#include <Font_FTFont.hxx>
#include <Font_FontMgr.hxx>
#include <Font_GlyphOutline.hxx>

#include <limits>
#include <optional>
#include <type_traits>

#include <gtest/gtest.h>

namespace Font_FTFont_Test
{
static_assert(std::is_aggregate_v<Font_FTFont::Params>);

occ::handle<Font_FTFont> createEmbeddedFont(const uint32_t thePointSize  = 72,
                                            const uint32_t theResolution = 4800)
{
  occ::handle<Font_FTFont>  aFont = new Font_FTFont();
  const Font_FTFont::Params aParams{thePointSize, theResolution};
  return aFont->Init(Font_FontMgr::EmbedFallbackFont(), "Embedded fallback", aParams, 0)
           ? aFont
           : occ::handle<Font_FTFont>();
}
} // namespace Font_FTFont_Test

TEST(Font_FTFontTest, UninitializedAccessorsReturnEmptyValues)
{
  const occ::handle<Font_FTFont> aFont = new Font_FTFont();
  EXPECT_FALSE(aFont->IsValid());
  EXPECT_FALSE(aFont->HasSymbol(U'A'));
  EXPECT_FALSE(aFont->RenderGlyph(U'A'));
  EXPECT_EQ(0u, aFont->GlyphMaxSizeX());
  EXPECT_EQ(0u, aFont->GlyphMaxSizeY());
  EXPECT_FLOAT_EQ(0.0f, aFont->Ascender());
  EXPECT_FLOAT_EQ(0.0f, aFont->Descender());
  EXPECT_FLOAT_EQ(0.0f, aFont->LineSpacing());
  EXPECT_FLOAT_EQ(0.0f, aFont->AdvanceX(U'A', U'B'));
  EXPECT_FLOAT_EQ(0.0f, aFont->AdvanceY(U'A', U'B'));
  EXPECT_EQ(0, aFont->GlyphsNumber());

  Font_Rect aRect{1.0f, 2.0f, 3.0f, 4.0f};
  aFont->GlyphRect(aRect);
  EXPECT_FLOAT_EQ(0.0f, aRect.Left);
  EXPECT_FLOAT_EQ(0.0f, aRect.Right);
  EXPECT_FLOAT_EQ(0.0f, aRect.Top);
  EXPECT_FLOAT_EQ(0.0f, aRect.Bottom);
}

TEST(Font_FTFontTest, LoadsResolvedGlyphOutline)
{
  const occ::handle<Font_FTFont> aFont = Font_FTFont_Test::createEmbeddedFont();
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "FreeType or the embedded fallback font is unavailable";
  }

  const std::optional<Font_GlyphOutline::Glyph> aGlyph = aFont->ResolveGlyph(U'A');
  ASSERT_TRUE(aGlyph.has_value());
  EXPECT_TRUE(aFont->IsGlyphValid(*aGlyph));

  const std::optional<Font_GlyphOutline> anOutline = aFont->LoadGlyphOutline(*aGlyph);
  ASSERT_TRUE(anOutline.has_value());
  EXPECT_GT(anOutline->UnitsPerEm(), 0.0);
  EXPECT_FALSE(anOutline->IsEmpty());
  EXPECT_FALSE(anOutline->Segments().IsEmpty());
  EXPECT_FALSE(anOutline->Contours().IsEmpty());
}

TEST(Font_FTFontTest, WidthScalingInvalidatesResolvedGlyph)
{
  const occ::handle<Font_FTFont> aFont = Font_FTFont_Test::createEmbeddedFont();
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "FreeType or the embedded fallback font is unavailable";
  }

  const std::optional<Font_GlyphOutline::Glyph> anOldGlyph = aFont->ResolveGlyph(U'I');
  ASSERT_TRUE(anOldGlyph.has_value());

  ASSERT_TRUE(aFont->SetWidthScaling(1.25f));
  EXPECT_FALSE(aFont->IsGlyphValid(*anOldGlyph));
  EXPECT_FALSE(aFont->LoadGlyphOutline(*anOldGlyph).has_value());

  const std::optional<Font_GlyphOutline::Glyph> aNewGlyph = aFont->ResolveGlyph(U'I');
  ASSERT_TRUE(aNewGlyph.has_value());
  EXPECT_NE(*anOldGlyph, *aNewGlyph);
  EXPECT_TRUE(aFont->IsGlyphValid(*aNewGlyph));
  EXPECT_TRUE(aFont->LoadGlyphOutline(*aNewGlyph).has_value());
}

TEST(Font_FTFontTest, IndependentFontsRejectForeignGlyph)
{
  const occ::handle<Font_FTFont> aFirstFont  = Font_FTFont_Test::createEmbeddedFont();
  const occ::handle<Font_FTFont> aSecondFont = Font_FTFont_Test::createEmbeddedFont();
  if (aFirstFont.IsNull() || aSecondFont.IsNull())
  {
    GTEST_SKIP() << "FreeType or the embedded fallback font is unavailable";
  }

  const std::optional<Font_GlyphOutline::Glyph> aFirstGlyph  = aFirstFont->ResolveGlyph(U'I');
  const std::optional<Font_GlyphOutline::Glyph> aSecondGlyph = aSecondFont->ResolveGlyph(U'I');
  ASSERT_TRUE(aFirstGlyph.has_value());
  ASSERT_TRUE(aSecondGlyph.has_value());
  EXPECT_NE(aFirstGlyph->Owner(), aSecondGlyph->Owner());
  EXPECT_FALSE(aSecondFont->IsGlyphValid(*aFirstGlyph));
  EXPECT_FALSE(aSecondFont->LoadGlyphOutline(*aFirstGlyph).has_value());
  EXPECT_TRUE(aSecondFont->IsGlyphValid(*aSecondGlyph));
}

TEST(Font_FTFontTest, InvalidWidthScalingPreservesResolvedGlyph)
{
  const occ::handle<Font_FTFont> aFont = Font_FTFont_Test::createEmbeddedFont();
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "FreeType or the embedded fallback font is unavailable";
  }

  const std::optional<Font_GlyphOutline::Glyph> aGlyph = aFont->ResolveGlyph(U'I');
  ASSERT_TRUE(aGlyph.has_value());
  EXPECT_FALSE(aFont->SetWidthScaling(0.0f));
  EXPECT_FALSE(aFont->SetWidthScaling(-1.0f));
  EXPECT_FLOAT_EQ(1.0f, aFont->WidthScaling());
  EXPECT_TRUE(aFont->IsGlyphValid(*aGlyph));
}

TEST(Font_FTFontTest, SingleStrokeModeInvalidatesResolvedGlyph)
{
  const occ::handle<Font_FTFont> aFont = Font_FTFont_Test::createEmbeddedFont();
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "FreeType or the embedded fallback font is unavailable";
  }

  const std::optional<Font_GlyphOutline::Glyph> anOldGlyph = aFont->ResolveGlyph(U'I');
  ASSERT_TRUE(anOldGlyph.has_value());
  aFont->SetSingleStrokeFont(true);
  EXPECT_FALSE(aFont->IsGlyphValid(*anOldGlyph));

  const std::optional<Font_GlyphOutline::Glyph> aNewGlyph = aFont->ResolveGlyph(U'I');
  ASSERT_TRUE(aNewGlyph.has_value());
  const std::optional<Font_GlyphOutline> anOutline = aFont->LoadGlyphOutline(*aNewGlyph);
  ASSERT_TRUE(anOutline.has_value());
  EXPECT_TRUE(anOutline->IsSingleStroke());
}

TEST(Font_FTFontTest, FindAndInitPreservesExplicitSingleStrokeMode)
{
  const occ::handle<Font_FontMgr> aManager = Font_FontMgr::GetInstance();
  ASSERT_FALSE(aManager.IsNull());
  Font_FontAspect                    anAspect = Font_FontAspect_Regular;
  const occ::handle<Font_SystemFont> aSystemFont =
    aManager->FindFont("", Font_StrictLevel_Any, anAspect, false);
  if (aSystemFont.IsNull() || aSystemFont->IsSingleStrokeFont())
  {
    GTEST_SKIP() << "A regular system font is unavailable";
  }

  Font_FTFont::Params aParams{72, 4800};
  aParams.IsSingleStrokeFont           = true;
  const occ::handle<Font_FTFont> aFont = new Font_FTFont();
  ASSERT_TRUE(aFont->FindAndInit("", anAspect, aParams, Font_StrictLevel_Any));
  EXPECT_TRUE(aFont->IsSingleStrokeFont());

  const std::optional<Font_GlyphOutline> anOutline = aFont->LoadGlyphOutline(U'A');
  ASSERT_TRUE(anOutline.has_value());
  EXPECT_TRUE(anOutline->IsSingleStroke());
}

TEST(Font_FTFontTest, FallbackPolicyInvalidatesResolvedGlyph)
{
  const occ::handle<Font_FTFont> aFont = Font_FTFont_Test::createEmbeddedFont();
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "FreeType or the embedded fallback font is unavailable";
  }

  const std::optional<Font_GlyphOutline::Glyph> anOldGlyph = aFont->ResolveGlyph(U'A');
  ASSERT_TRUE(anOldGlyph.has_value());
  aFont->SetUseUnicodeSubsetFallback(!aFont->ToUseUnicodeSubsetFallback());
  EXPECT_FALSE(aFont->IsGlyphValid(*anOldGlyph));

  const std::optional<Font_GlyphOutline::Glyph> aNewGlyph = aFont->ResolveGlyph(U'A');
  ASSERT_TRUE(aNewGlyph.has_value());
  EXPECT_NE(*anOldGlyph, *aNewGlyph);
  EXPECT_TRUE(aFont->IsGlyphValid(*aNewGlyph));
}

TEST(Font_FTFontTest, OutlineLoadingClearsBitmapState)
{
  const occ::handle<Font_FTFont> aFont = Font_FTFont_Test::createEmbeddedFont(12, 72);
  if (aFont.IsNull() || !aFont->RenderGlyph(U'A'))
  {
    GTEST_SKIP() << "FreeType or the embedded fallback bitmap glyph is unavailable";
  }

  Font_Rect aRenderedRect;
  aFont->GlyphRect(aRenderedRect);
  EXPECT_GT(aRenderedRect.Right - aRenderedRect.Left, 0.0f);
  EXPECT_GT(aRenderedRect.Top - aRenderedRect.Bottom, 0.0f);

  ASSERT_TRUE(aFont->LoadGlyphOutline(U'A').has_value());
  Font_Rect anOutlineRect{1.0f, 2.0f, 3.0f, 4.0f};
  aFont->GlyphRect(anOutlineRect);
  EXPECT_FLOAT_EQ(0.0f, anOutlineRect.Left);
  EXPECT_FLOAT_EQ(0.0f, anOutlineRect.Right);
  EXPECT_FLOAT_EQ(0.0f, anOutlineRect.Top);
  EXPECT_FLOAT_EQ(0.0f, anOutlineRect.Bottom);
}

TEST(Font_FTFontTest, ReleaseInvalidatesResolvedGlyph)
{
  const occ::handle<Font_FTFont> aFont = Font_FTFont_Test::createEmbeddedFont();
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "FreeType or the embedded fallback font is unavailable";
  }

  const std::optional<Font_GlyphOutline::Glyph> aGlyph = aFont->ResolveGlyph(U'A');
  ASSERT_TRUE(aGlyph.has_value());
  aFont->Release();
  EXPECT_FALSE(aFont->IsGlyphValid(*aGlyph));
  EXPECT_FALSE(aFont->IsValid());
  EXPECT_FALSE(aFont->ResolveGlyph(U'A').has_value());
}

TEST(Font_FTFontTest, FailedReinitializationPreservesLoadedFace)
{
  const occ::handle<Font_FTFont> aFont = Font_FTFont_Test::createEmbeddedFont();
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "FreeType or the embedded fallback font is unavailable";
  }

  const std::optional<Font_GlyphOutline::Glyph> aGlyph = aFont->ResolveGlyph(U'A');
  ASSERT_TRUE(aGlyph.has_value());

  const Font_FTFont::Params aValidParams{72, 4800};
  EXPECT_FALSE(aFont->Init("OCCT-Missing-Font-4F9E2C.ttf", aValidParams));
  EXPECT_TRUE(aFont->IsValid());
  EXPECT_TRUE(aFont->IsGlyphValid(*aGlyph));

  const Font_FTFont::Params anInvalidParams{0, 4800};
  EXPECT_FALSE(
    aFont->Init(Font_FontMgr::EmbedFallbackFont(), "Embedded fallback", anInvalidParams, 0));
  EXPECT_TRUE(aFont->IsValid());
  EXPECT_TRUE(aFont->IsGlyphValid(*aGlyph));

  Font_FTFont::Params aConflictingParams{72, 4800};
  aConflictingParams.FontHinting = Font_Hinting(Font_Hinting_Light | Font_Hinting_Normal);
  EXPECT_FALSE(
    aFont->Init(Font_FontMgr::EmbedFallbackFont(), "Embedded fallback", aConflictingParams, 0));
  EXPECT_TRUE(aFont->IsValid());
  EXPECT_TRUE(aFont->IsGlyphValid(*aGlyph));
  EXPECT_TRUE(aFont->LoadGlyphOutline(*aGlyph).has_value());
}

TEST(Font_FTFontTest, ZeroCharacterHasNoResolvedGlyph)
{
  const occ::handle<Font_FTFont> aFont = Font_FTFont_Test::createEmbeddedFont();
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "FreeType or the embedded fallback font is unavailable";
  }
  EXPECT_FALSE(aFont->ResolveGlyph(char32_t(0)).has_value());
}

TEST(Font_FTFontTest, ParamsValidationIsBackendIndependent)
{
  Font_FTFont::Params aParams;
  aParams.PointSize = std::numeric_limits<uint32_t>::max();
  EXPECT_TRUE(aParams.IsValid());
}
