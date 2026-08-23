// Copyright (c) 2013-2026 OPEN CASCADE SAS
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

#include <StdPrs_BRepFont.hxx>

#include <BRepCheck_Analyzer.hxx>
#include <Font_FontMgr.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Iterator.hxx>

#include <atomic>
#include <limits>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

namespace
{
occ::handle<StdPrs_BRepFont> createFont()
{
  return StdPrs_BRepFont::FindAndCreate("OCCT-Embedded-Fallback-Request-4A72C3",
                                        Font_FontAspect_Regular,
                                        12.0,
                                        Font_StrictLevel_Any);
}

int countSubShapes(const TopoDS_Shape& theShape, const TopAbs_ShapeEnum theType)
{
  int aCount = 0;
  for (TopExp_Explorer anExplorer(theShape, theType); anExplorer.More(); anExplorer.Next())
  {
    ++aCount;
  }
  return aCount;
}
} // namespace

TEST(StdPrs_BRepFontTest, InvalidAndReleasedFontReturnEmptyResults)
{
  StdPrs_BRepFont aFont;
  EXPECT_FALSE(aFont.IsValid());
  EXPECT_DOUBLE_EQ(0.0, aFont.Size());
  EXPECT_DOUBLE_EQ(0.0, aFont.Ascender());
  EXPECT_TRUE(aFont.RenderGlyph(U'A').IsNull());
  EXPECT_TRUE(aFont.RenderText("A").IsNull());

  const occ::handle<StdPrs_BRepFont> anInitialized = createFont();
  if (anInitialized.IsNull())
  {
    GTEST_SKIP() << "FreeType and usable fonts are unavailable";
  }
  anInitialized->Release();
  EXPECT_FALSE(anInitialized->IsValid());
  EXPECT_TRUE(anInitialized->RenderGlyph(U'A').IsNull());
}

TEST(StdPrs_BRepFontTest, GlyphOptionsConcatenateContoursPerCall)
{
  const occ::handle<StdPrs_BRepFont> aFont = createFont();
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "FreeType and usable fonts are unavailable";
  }

  const TopoDS_Shape aDefaultShape = aFont->RenderGlyph(U'B');
  ASSERT_FALSE(aDefaultShape.IsNull());

  StdPrs_BRepFont::GlyphOptions anOptions;
  anOptions.ToConcatenateContours    = true;
  const TopoDS_Shape aCompositeShape = aFont->RenderGlyph(U'B', anOptions);
  ASSERT_FALSE(aCompositeShape.IsNull());
  EXPECT_TRUE(BRepCheck_Analyzer(aCompositeShape).IsValid());
  EXPECT_LT(countSubShapes(aCompositeShape, TopAbs_EDGE),
            countSubShapes(aDefaultShape, TopAbs_EDGE));
  EXPECT_EQ(countSubShapes(aDefaultShape, TopAbs_FACE),
            countSubShapes(aCompositeShape, TopAbs_FACE));
}

TEST(StdPrs_BRepFontTest, ReusesBuiltGlyphForEachContourMode)
{
  const occ::handle<StdPrs_BRepFont> aFont = createFont();
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "FreeType and usable fonts are unavailable";
  }

  const TopoDS_Shape aDefaultShape = aFont->RenderGlyph(U'B');
  ASSERT_FALSE(aDefaultShape.IsNull());
  EXPECT_TRUE(aDefaultShape.IsSame(aFont->RenderGlyph(U'B')));

  StdPrs_BRepFont::GlyphOptions anOptions;
  anOptions.ToConcatenateContours    = true;
  const TopoDS_Shape aCompositeShape = aFont->RenderGlyph(U'B', anOptions);
  ASSERT_FALSE(aCompositeShape.IsNull());
  EXPECT_TRUE(aCompositeShape.IsSame(aFont->RenderGlyph(U'B', anOptions)));
  EXPECT_FALSE(aDefaultShape.IsSame(aCompositeShape));
}

TEST(StdPrs_BRepFontTest, RenderTextBuildsPlacedCompound)
{
  const occ::handle<StdPrs_BRepFont> aFont = createFont();
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "FreeType and usable fonts are unavailable";
  }

  const TopoDS_Shape aDefaultText = aFont->RenderText("ABBA");
  ASSERT_FALSE(aDefaultText.IsNull());
  EXPECT_TRUE(BRepCheck_Analyzer(aDefaultText).IsValid());

  StdPrs_BRepFont::TextOptions anOptions;
  anOptions.HorizontalAlignment      = Graphic3d_HTA_CENTER;
  anOptions.VerticalAlignment        = Graphic3d_VTA_CENTER;
  anOptions.ToConcatenateContours    = true;
  const TopoDS_Shape aConfiguredText = aFont->RenderText("ABBA", anOptions);
  ASSERT_FALSE(aConfiguredText.IsNull());
  EXPECT_TRUE(BRepCheck_Analyzer(aConfiguredText).IsValid());
}

TEST(StdPrs_BRepFontTest, RenderTextReusesGlyphCache)
{
  const occ::handle<StdPrs_BRepFont> aFont = createFont();
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "FreeType and usable fonts are unavailable";
  }

  const TopoDS_Shape aGlyph = aFont->RenderGlyph(U'A');
  ASSERT_FALSE(aGlyph.IsNull());
  const TopoDS_Shape aText = aFont->RenderText("AA");
  ASSERT_FALSE(aText.IsNull());

  size_t aNbOccurrences = 0;
  for (TopoDS_Iterator anIterator(aText); anIterator.More(); anIterator.Next())
  {
    EXPECT_TRUE(aGlyph.IsPartner(anIterator.Value()));
    ++aNbOccurrences;
  }
  EXPECT_EQ(2u, aNbOccurrences);
}

TEST(StdPrs_BRepFontTest, WidthScalingValidationPreservesUsableFont)
{
  const occ::handle<StdPrs_BRepFont> aFont = createFont();
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "FreeType and usable fonts are unavailable";
  }

  const TopoDS_Shape aBefore = aFont->RenderGlyph(U'I');
  ASSERT_FALSE(aBefore.IsNull());
  EXPECT_FALSE(aFont->SetWidthScaling(0.0f));
  EXPECT_FALSE(aFont->SetWidthScaling(-1.0f));
  EXPECT_FALSE(aFont->SetWidthScaling(std::numeric_limits<float>::infinity()));
  EXPECT_TRUE(aBefore.IsSame(aFont->RenderGlyph(U'I')));

  ASSERT_TRUE(aFont->SetWidthScaling(1.25f));
  const TopoDS_Shape anAfter = aFont->RenderGlyph(U'I');
  EXPECT_FALSE(anAfter.IsNull());
  EXPECT_FALSE(aBefore.IsSame(anAfter));
}

TEST(StdPrs_BRepFontTest, SingleStrokeModeRebuildsGlyphAsWires)
{
  const occ::handle<StdPrs_BRepFont> aFont = createFont();
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "FreeType and usable fonts are unavailable";
  }

  const TopoDS_Shape aFilledGlyph = aFont->RenderGlyph(U'B');
  ASSERT_FALSE(aFilledGlyph.IsNull());
  ASSERT_GT(countSubShapes(aFilledGlyph, TopAbs_FACE), 0);

  aFont->SetSingleStrokeFont(true);
  const TopoDS_Shape aStrokeGlyph = aFont->RenderGlyph(U'B');
  ASSERT_FALSE(aStrokeGlyph.IsNull());
  EXPECT_FALSE(aFilledGlyph.IsSame(aStrokeGlyph));
  EXPECT_EQ(0, countSubShapes(aStrokeGlyph, TopAbs_FACE));
  EXPECT_GT(countSubShapes(aStrokeGlyph, TopAbs_WIRE), 0);

  aFont->SetSingleStrokeFont(false);
  const TopoDS_Shape aRestoredGlyph = aFont->RenderGlyph(U'B');
  ASSERT_FALSE(aRestoredGlyph.IsNull());
  EXPECT_GT(countSubShapes(aRestoredGlyph, TopAbs_FACE), 0);
}

TEST(StdPrs_BRepFontTest, FailedReinitializationPreservesCurrentFont)
{
  const occ::handle<StdPrs_BRepFont> aFont = createFont();
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "FreeType and usable fonts are unavailable";
  }

  const double       aSize   = aFont->Size();
  const TopoDS_Shape aBefore = aFont->RenderGlyph(U'I');
  ASSERT_FALSE(aBefore.IsNull());
  EXPECT_FALSE(aFont->Init("", aSize, 0));
  EXPECT_FALSE(aFont->Init("", std::numeric_limits<double>::infinity(), 0));
  EXPECT_TRUE(aFont->IsValid());
  EXPECT_DOUBLE_EQ(aSize, aFont->Size());
  EXPECT_TRUE(aBefore.IsSame(aFont->RenderGlyph(U'I')));
}

TEST(StdPrs_BRepFontTest, ConcurrentTextRenderingRemainsUsable)
{
  const occ::handle<StdPrs_BRepFont> aFont = createFont();
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "FreeType and usable fonts are unavailable";
  }

  std::atomic<bool>        isValid{true};
  std::vector<std::thread> aThreads;
  for (int aThread = 0; aThread < 4; ++aThread)
  {
    aThreads.emplace_back([&aFont, &isValid]() {
      for (int anIteration = 0; anIteration < 8; ++anIteration)
      {
        if (aFont->RenderText("ABBA").IsNull())
        {
          isValid.store(false, std::memory_order_relaxed);
          return;
        }
      }
    });
  }
  for (std::thread& aThread : aThreads)
  {
    aThread.join();
  }
  EXPECT_TRUE(isValid.load(std::memory_order_relaxed));
}
