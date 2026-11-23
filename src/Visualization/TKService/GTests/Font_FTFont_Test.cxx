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
#include <Font_FTLibrary.hxx>
#include <Font_FontMgr.hxx>
#include <Font_GlyphOutline.hxx>
#include <Font_SystemFont.hxx>
#include <NCollection_List.hxx>
#include <Message_Printer.hxx>
#include <Message_Messenger.hxx>
#include <Message.hxx>

#include <limits>
#include <optional>
#include <type_traits>

#include <gtest/gtest.h>

#ifdef HAVE_FREETYPE
  #include "../Font/Font_FTFontBitmap.pxx"
#endif

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

namespace
{
struct FontTest_Printer : public Message_Printer
{
public:
  mutable int Failures = 0; //!< Number of failure messages received.

protected:
  void send(const TCollection_AsciiString& theText, Message_Gravity theGravity) const override;
};

struct FontTest_Environment
{
public:
  FontTest_Environment();
  ~FontTest_Environment();

  occ::handle<FontTest_Printer> Printer; //!< Temporary failure-message collector.

private:
  occ::handle<Font_FontMgr> myManager; //!< Font database restored on destruction.
  NCollection_List<occ::handle<Font_SystemFont>> myFonts;       //!< Original registered faces.
  bool                                           myPrintErrors; //!< Original diagnostic setting.
};

//==================================================================================================

void FontTest_Printer::send(const TCollection_AsciiString&, const Message_Gravity theGravity) const
{
  if (theGravity == Message_Fail)
  {
    ++Failures;
  }
}

//==================================================================================================

FontTest_Environment::FontTest_Environment()
    : Printer(new FontTest_Printer()),
      myManager(Font_FontMgr::GetInstance()),
      myFonts(myManager->GetAvailableFonts()),
      myPrintErrors(myManager->ToPrintErrors())
{
  myManager->ClearFontDataBase();
  myManager->SetPrintErrors(true);
  Message::DefaultMessenger()->AddPrinter(Printer);
}

//==================================================================================================

FontTest_Environment::~FontTest_Environment()
{
  Message::DefaultMessenger()->RemovePrinter(Printer);
  myManager->ClearFontDataBase();
  for (const occ::handle<Font_SystemFont>& aFont : myFonts)
  {
    myManager->RegisterFont(aFont, false);
  }
  myManager->SetPrintErrors(myPrintErrors);
}
} // namespace

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

TEST(Font_FTFontTest, MissingSystemFontUsesEmbeddedFallbackWithoutFailureMessage)
{
  if (Font_FontMgr::EmbedFallbackFont().IsNull())
  {
    GTEST_SKIP() << "FreeType or the embedded fallback font is unavailable";
  }

  FontTest_Environment           anEnvironment;
  const occ::handle<Font_FTFont> aFont =
    Font_FTFont::FindAndCreate("OCCT-Definitely-Missing-Font-Family-4A72C3",
                               Font_FontAspect_Regular,
                               Font_FTFont::Params{12, 72},
                               Font_StrictLevel_Any);

  ASSERT_FALSE(aFont.IsNull());
  EXPECT_TRUE(aFont->IsValid());
  EXPECT_EQ(0, anEnvironment.Printer->Failures);
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

Standard_DISABLE_DEPRECATION_WARNINGS

  TEST(Font_FTFontTest, CompatibilityParametersAndBorrowedOutline)
{
  Font_FTFontParams aDefaultParams;
  EXPECT_FALSE(aDefaultParams.IsValid());
  Font_FTFontParams aParams(12, 72);
  EXPECT_TRUE(aParams.IsValid());
  const occ::handle<Font_FTFont> aFont = new Font_FTFont();
  if (!aFont->Init(Font_FontMgr::EmbedFallbackFont(), "Embedded fallback", aParams))
  {
    GTEST_SKIP() << "FreeType or the embedded fallback font is unavailable";
  }
  const float anAdvance = aFont->AdvanceX(U'A', U'V');
  ASSERT_TRUE(aFont->RenderGlyph(U'A'));
  EXPECT_NE(nullptr, aFont->renderGlyphOutline(U'A'));
  EXPECT_FLOAT_EQ(anAdvance, aFont->AdvanceX(U'V'));
  EXPECT_TRUE(aFont->GlyphImage().IsEmpty());
  EXPECT_EQ(nullptr, aFont->renderGlyphOutline(0));
  EXPECT_FLOAT_EQ(0.0f, aFont->AdvanceX(U'V'));
}

Standard_ENABLE_DEPRECATION_WARNINGS

  TEST(Font_FTFontTest, OutlineExtractionPreservesPixelAdvances)
{
  const occ::handle<Font_FTFont> aFont = Font_FTFont_Test::createEmbeddedFont(12, 72);
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "FreeType or the embedded fallback font is unavailable";
  }
  const float anExpectedX = aFont->AdvanceX(U'A', U'V');
  const float anExpectedY = aFont->AdvanceY(U'A', U'V');
  ASSERT_TRUE(aFont->LoadGlyphOutline(U'B').has_value());
  EXPECT_FLOAT_EQ(anExpectedX, aFont->AdvanceX(U'V'));
  EXPECT_FLOAT_EQ(anExpectedY, aFont->AdvanceY(U'V'));
  // Reusing the same metric character must not read the outline slot in design units.
  EXPECT_FLOAT_EQ(anExpectedX, aFont->AdvanceX(U'A', U'V'));
  ASSERT_TRUE(aFont->RenderGlyph(U'A'));
  ASSERT_TRUE(aFont->LoadGlyphOutline(U'B').has_value());
  EXPECT_FLOAT_EQ(anExpectedX, aFont->AdvanceX(U'V'));
  EXPECT_TRUE(aFont->GlyphImage().IsEmpty());
}

#ifdef HAVE_FREETYPE
TEST(Font_FTFontTest, MonochromeBitmapUsesPaddedRows)
{
  const Font_FTLibrary aLibrary;
  uint8_t              aData[] = {0x80, 0xFF, 0x40, 0xFF, 0x20, 0xFF};
  FT_Bitmap            aBitmap{};
  aBitmap.width      = 8;
  aBitmap.rows       = 3;
  aBitmap.pitch      = 2;
  aBitmap.buffer     = aData;
  aBitmap.pixel_mode = FT_PIXEL_MODE_MONO;
  Image_PixMap anImage;
  ASSERT_TRUE(Font_FTFontBitmap::initGlyphBitmap(aLibrary.Instance(), aBitmap, anImage));
  for (size_t aRow = 0; aRow < 3; ++aRow)
  {
    for (size_t aCol = 0; aCol < 8; ++aCol)
    {
      EXPECT_EQ(aRow == aCol ? 255 : 0, anImage.Row(aRow)[aCol]);
    }
  }
  uint8_t aPartialData[] = {0x80, 0x80, 0xFF, 0x40, 0x00, 0xFF};
  aBitmap.width          = 9;
  aBitmap.rows           = 2;
  aBitmap.pitch          = 3;
  aBitmap.buffer         = aPartialData;
  ASSERT_TRUE(Font_FTFontBitmap::initGlyphBitmap(aLibrary.Instance(), aBitmap, anImage));
  for (size_t aRow = 0; aRow < 2; ++aRow)
  {
    for (size_t aCol = 0; aCol < 9; ++aCol)
    {
      EXPECT_EQ(aCol == aRow || (aRow == 0 && aCol == 8) ? 255 : 0, anImage.Row(aRow)[aCol]);
    }
  }
}

TEST(Font_FTFontTest, GrayscaleBitmapIsReadyForTightlyPackedTextureUpload)
{
  const Font_FTLibrary aLibrary;
  uint8_t              aData[] = {10, 20, 0xFF, 30, 40, 0xFF};
  FT_Bitmap            aBitmap{};
  aBitmap.width      = 2;
  aBitmap.rows       = 2;
  aBitmap.pitch      = 3;
  aBitmap.buffer     = aData;
  aBitmap.pixel_mode = FT_PIXEL_MODE_GRAY;
  aBitmap.num_grays  = 256;
  Image_PixMap anImage;
  ASSERT_TRUE(Font_FTFontBitmap::initGlyphBitmap(aLibrary.Instance(), aBitmap, anImage));
  EXPECT_EQ(2u, anImage.SizeRowBytes());
  EXPECT_EQ(10, anImage.Data()[0]);
  EXPECT_EQ(20, anImage.Data()[1]);
  EXPECT_EQ(30, anImage.Data()[2]);
  EXPECT_EQ(40, anImage.Data()[3]);
}

TEST(Font_FTFontTest, GrayscaleBitmapKeepsOrdinaryCoverageWithoutCopy)
{
  const Font_FTLibrary aLibrary;
  uint8_t              aData[] = {0, 64, 128, 255};
  FT_Bitmap            aBitmap{};
  aBitmap.width      = 2;
  aBitmap.rows       = 2;
  aBitmap.pitch      = 2;
  aBitmap.buffer     = aData;
  aBitmap.pixel_mode = FT_PIXEL_MODE_GRAY;
  aBitmap.num_grays  = 256;
  Image_PixMap anImage;
  ASSERT_TRUE(Font_FTFontBitmap::initGlyphBitmap(aLibrary.Instance(), aBitmap, anImage));
  EXPECT_EQ(aData, anImage.Data());
  EXPECT_EQ(Image_Format_Alpha, anImage.Format());
  EXPECT_TRUE(anImage.IsTopDown());
}

TEST(Font_FTFontTest, BitmapConvertsBottomUpRowsForTextureUpload)
{
  const Font_FTLibrary aLibrary;
  uint8_t              aGrayData[] = {30, 40, 0xFF, 10, 20, 0xFF};
  FT_Bitmap            aBitmap{};
  aBitmap.width      = 2;
  aBitmap.rows       = 2;
  aBitmap.pitch      = -3;
  aBitmap.buffer     = aGrayData;
  aBitmap.pixel_mode = FT_PIXEL_MODE_GRAY;
  aBitmap.num_grays  = 256;
  Image_PixMap anImage;
  ASSERT_TRUE(Font_FTFontBitmap::initGlyphBitmap(aLibrary.Instance(), aBitmap, anImage));
  EXPECT_TRUE(anImage.IsTopDown());
  EXPECT_EQ(10, anImage.Data()[0]);
  EXPECT_EQ(20, anImage.Data()[1]);
  EXPECT_EQ(30, anImage.Data()[2]);
  EXPECT_EQ(40, anImage.Data()[3]);

  uint8_t aMonoData[] = {0x40, 0xFF, 0x80, 0xFF};
  aBitmap.buffer      = aMonoData;
  aBitmap.pitch       = -2;
  aBitmap.pixel_mode  = FT_PIXEL_MODE_MONO;
  ASSERT_TRUE(Font_FTFontBitmap::initGlyphBitmap(aLibrary.Instance(), aBitmap, anImage));
  EXPECT_TRUE(anImage.IsTopDown());
  EXPECT_EQ(255, anImage.Data()[0]);
  EXPECT_EQ(0, anImage.Data()[1]);
  EXPECT_EQ(0, anImage.Data()[2]);
  EXPECT_EQ(255, anImage.Data()[3]);
}

TEST(Font_FTFontTest, BitmapExpandsPackedCoverageToFullAlphaRange)
{
  const Font_FTLibrary aLibrary;
  uint8_t              aGray2Data[] = {0x1B, 0xFF, 0xE4, 0xFF};
  FT_Bitmap            aBitmap{};
  aBitmap.width      = 4;
  aBitmap.rows       = 2;
  aBitmap.pitch      = 2;
  aBitmap.buffer     = aGray2Data;
  aBitmap.pixel_mode = FT_PIXEL_MODE_GRAY2;
  Image_PixMap anImage;
  ASSERT_TRUE(Font_FTFontBitmap::initGlyphBitmap(aLibrary.Instance(), aBitmap, anImage));
  for (size_t aCol = 0; aCol < 4; ++aCol)
  {
    EXPECT_EQ(aCol * 85, anImage.Row(0)[aCol]);
    EXPECT_EQ((3 - aCol) * 85, anImage.Row(1)[aCol]);
  }

  uint8_t aGray4Data[] = {0x05, 0xAF, 0xFF};
  aBitmap.rows         = 1;
  aBitmap.pitch        = 3;
  aBitmap.buffer       = aGray4Data;
  aBitmap.pixel_mode   = FT_PIXEL_MODE_GRAY4;
  ASSERT_TRUE(Font_FTFontBitmap::initGlyphBitmap(aLibrary.Instance(), aBitmap, anImage));
  for (size_t aCol = 0; aCol < 4; ++aCol)
  {
    EXPECT_EQ(aCol * 85, anImage.Row(0)[aCol]);
  }

  uint8_t aGrayData[] = {0, 5, 10, 15};
  aBitmap.pitch       = 4;
  aBitmap.buffer      = aGrayData;
  aBitmap.pixel_mode  = FT_PIXEL_MODE_GRAY;
  aBitmap.num_grays   = 16;
  ASSERT_TRUE(Font_FTFontBitmap::initGlyphBitmap(aLibrary.Instance(), aBitmap, anImage));
  for (size_t aCol = 0; aCol < 4; ++aCol)
  {
    EXPECT_EQ(aCol * 85, anImage.Row(0)[aCol]);
  }
}

TEST(Font_FTFontTest, BitmapRejectsInvalidLayoutAndUnsupportedModes)
{
  const Font_FTLibrary aLibrary;
  uint8_t              aData[] = {0, 255};
  FT_Bitmap            aBitmap{};
  aBitmap.width      = 2;
  aBitmap.rows       = 1;
  aBitmap.pitch      = 2;
  aBitmap.buffer     = aData;
  aBitmap.pixel_mode = FT_PIXEL_MODE_GRAY;
  aBitmap.num_grays  = 256;
  Image_PixMap anImage;
  ASSERT_TRUE(Font_FTFontBitmap::initGlyphBitmap(aLibrary.Instance(), aBitmap, anImage));
  aBitmap.pitch = 1;
  EXPECT_FALSE(Font_FTFontBitmap::initGlyphBitmap(aLibrary.Instance(), aBitmap, anImage));
  EXPECT_TRUE(anImage.IsEmpty());
  aBitmap.pitch     = 2;
  aBitmap.num_grays = 1;
  EXPECT_FALSE(Font_FTFontBitmap::initGlyphBitmap(aLibrary.Instance(), aBitmap, anImage));
  aBitmap.num_grays  = 256;
  aBitmap.pixel_mode = FT_PIXEL_MODE_LCD;
  EXPECT_FALSE(Font_FTFontBitmap::initGlyphBitmap(aLibrary.Instance(), aBitmap, anImage));
  aBitmap.pixel_mode = FT_PIXEL_MODE_GRAY;
  aBitmap.width      = 0;
  EXPECT_FALSE(Font_FTFontBitmap::initGlyphBitmap(aLibrary.Instance(), aBitmap, anImage));
  aBitmap.width  = 2;
  aBitmap.buffer = nullptr;
  EXPECT_FALSE(Font_FTFontBitmap::initGlyphBitmap(aLibrary.Instance(), aBitmap, anImage));
}

TEST(Font_FTFontTest, BitmapCoverageAndMetricsSurviveRepeatedOutlineAndRenderCalls)
{
  const occ::handle<Font_FTFont> aFont = Font_FTFont_Test::createEmbeddedFont(12, 72);
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "FreeType or the embedded fallback font is unavailable";
  }
  ASSERT_TRUE(aFont->RenderGlyph(U'A'));
  Image_PixMap anExpected;
  ASSERT_TRUE(anExpected.InitCopy(aFont->GlyphImage()));
  anExpected.SetTopDown(aFont->GlyphImage().IsTopDown());
  Font_Rect anExpectedRect;
  aFont->GlyphRect(anExpectedRect);
  const float anExpectedAdvance   = aFont->AdvanceX(U'V');
  bool        aHasPartialCoverage = false;
  for (size_t aRow = 0; aRow < anExpected.SizeY(); ++aRow)
  {
    for (size_t aCol = 0; aCol < anExpected.SizeX(); ++aCol)
    {
      const uint8_t aValue = anExpected.Row(aRow)[aCol];
      aHasPartialCoverage  = aHasPartialCoverage || (aValue != 0 && aValue != 255);
    }
  }
  EXPECT_TRUE(aHasPartialCoverage);
  ASSERT_TRUE(aFont->LoadGlyphOutline(U'B').has_value());
  ASSERT_TRUE(aFont->RenderGlyph(U'A'));
  const Image_PixMap& anActual = aFont->GlyphImage();
  ASSERT_EQ(anExpected.SizeX(), anActual.SizeX());
  ASSERT_EQ(anExpected.SizeY(), anActual.SizeY());
  EXPECT_TRUE(anActual.IsTopDown());
  EXPECT_EQ(anActual.SizeX(), anActual.SizeRowBytes());
  for (size_t aRow = 0; aRow < anExpected.SizeY(); ++aRow)
  {
    for (size_t aCol = 0; aCol < anExpected.SizeX(); ++aCol)
    {
      EXPECT_EQ(anExpected.Row(aRow)[aCol], anActual.Row(aRow)[aCol]);
    }
  }
  Font_Rect anActualRect;
  aFont->GlyphRect(anActualRect);
  EXPECT_FLOAT_EQ(anExpectedRect.Left, anActualRect.Left);
  EXPECT_FLOAT_EQ(anExpectedRect.Right, anActualRect.Right);
  EXPECT_FLOAT_EQ(anExpectedRect.Top, anActualRect.Top);
  EXPECT_FLOAT_EQ(anExpectedRect.Bottom, anActualRect.Bottom);
  EXPECT_FLOAT_EQ(anExpectedAdvance, aFont->AdvanceX(U'V'));
}
#endif
