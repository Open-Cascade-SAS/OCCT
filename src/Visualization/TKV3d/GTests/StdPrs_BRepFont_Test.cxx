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
#include <Prs3d_DimensionAspect.hxx>
#include <PrsDim_AngleDimension.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <Font_TextFormatter.hxx>
#include <Font_FTFont.hxx>
#include <Font_FontMgr.hxx>
#include <Font_BRepTextBuilder.hxx>
#include <Font_BRepFont.hxx>

#include <BRepCheck_Analyzer.hxx>
#include <NCollection_LinearVector.hxx>
#include <StdPrs_BRepFontCache.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Iterator.hxx>

#include <atomic>
#include <cmath>
#include <limits>
#include <thread>

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

TEST(StdPrs_BRepFontTest, RenderGlyphReturnsIndependentTopology)
{
  const occ::handle<StdPrs_BRepFont> aFont = createFont();
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "FreeType and usable fonts are unavailable";
  }

  const TopoDS_Shape aDefaultShape = aFont->RenderGlyph(U'B');
  ASSERT_FALSE(aDefaultShape.IsNull());
  const TopoDS_Shape aSecondDefaultShape = aFont->RenderGlyph(U'B');
  ASSERT_FALSE(aSecondDefaultShape.IsNull());
  EXPECT_FALSE(aDefaultShape.IsSame(aSecondDefaultShape));

  StdPrs_BRepFont::GlyphOptions anOptions;
  anOptions.ToConcatenateContours    = true;
  const TopoDS_Shape aCompositeShape = aFont->RenderGlyph(U'B', anOptions);
  ASSERT_FALSE(aCompositeShape.IsNull());
  const TopoDS_Shape aSecondCompositeShape = aFont->RenderGlyph(U'B', anOptions);
  ASSERT_FALSE(aSecondCompositeShape.IsNull());
  EXPECT_FALSE(aCompositeShape.IsSame(aSecondCompositeShape));
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

TEST(StdPrs_BRepFontTest, RenderTextSharesOnlyPerCallGlyphTopology)
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

  TopoDS_Shape aFirstOccurrence;
  size_t       aNbOccurrences = 0;
  for (TopoDS_Iterator anIterator(aText); anIterator.More(); anIterator.Next())
  {
    EXPECT_FALSE(aGlyph.IsPartner(anIterator.Value()));
    if (aFirstOccurrence.IsNull())
    {
      aFirstOccurrence = anIterator.Value();
    }
    else
    {
      EXPECT_TRUE(aFirstOccurrence.IsPartner(anIterator.Value()));
    }
    ++aNbOccurrences;
  }
  EXPECT_EQ(2u, aNbOccurrences);
}

TEST(StdPrs_BRepFontTest, TextPlanStoresBoundsAndRegions)
{
  const occ::handle<StdPrs_BRepFont> aFont = createFont();
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "FreeType and usable fonts are unavailable";
  }

  std::optional<BRepFont_Builder::TextPlan> aPlan =
    aFont->PlanText("ABBA", Graphic3d_HTA_CENTER, Graphic3d_VTA_CENTER);
  ASSERT_TRUE(aPlan.has_value());
  EXPECT_EQ(2u, aPlan->NbRegions());
  EXPECT_EQ(4u, aPlan->Items().Size());
  EXPECT_GT(aPlan->Width(), 0.0);
  EXPECT_GT(aPlan->Height(), 0.0);
  EXPECT_LT(aPlan->LowerLeft().X(), 0.0);
  EXPECT_GT(aPlan->UpperRight().X(), 0.0);

  const TopoDS_Shape aText = aFont->RenderText(*aPlan);
  ASSERT_FALSE(aText.IsNull());
  EXPECT_EQ(4, aText.NbChildren());
}

TEST(StdPrs_BRepFontTest, RightAlignedMultilineTextBuildsShape)
{
  const occ::handle<StdPrs_BRepFont> aFont = StdPrs_BRepFont::FindAndCreate("Times-Roman",
                                                                            Font_FontAspect_Regular,
                                                                            50.0,
                                                                            Font_StrictLevel_Any);
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "FreeType and usable fonts are unavailable";
  }

  const std::optional<BRepFont_Builder::TextPlan> aPlan =
    aFont->PlanText("Top-Right\nFirst line\nLion The Second\n3rd",
                    Graphic3d_HTA_RIGHT,
                    Graphic3d_VTA_TOP);
  ASSERT_TRUE(aPlan.has_value());
  ASSERT_GT(aPlan->NbRegions(), 0u);
  ASSERT_FALSE(aPlan->Items().IsEmpty());
  EXPECT_LT(aPlan->LowerLeft().X(), aPlan->UpperRight().X());
  EXPECT_LT(aPlan->LowerLeft().Y(), aPlan->UpperRight().Y());
  EXPECT_FALSE(aFont->RenderText(*aPlan).IsNull());
}

TEST(StdPrs_BRepFontTest, TextPlanRemainsValidAfterFontChanges)
{
  const occ::handle<StdPrs_BRepFont> aFont       = createFont();
  const occ::handle<StdPrs_BRepFont> anOtherFont = createFont();
  if (aFont.IsNull() || anOtherFont.IsNull())
  {
    GTEST_SKIP() << "FreeType and usable fonts are unavailable";
  }

  const std::optional<BRepFont_Builder::TextPlan> aPlan = aFont->PlanText("B");
  ASSERT_TRUE(aPlan.has_value());
  const TopoDS_Shape anOriginal = aFont->RenderText(*aPlan);
  ASSERT_FALSE(anOriginal.IsNull());
  EXPECT_GT(countSubShapes(anOriginal, TopAbs_FACE), 0);
  EXPECT_FALSE(anOtherFont->RenderText(*aPlan).IsNull());

  ASSERT_TRUE(aFont->SetWidthScaling(1.25f));
  const TopoDS_Shape aTextFromOldPlan = aFont->RenderText(*aPlan);
  ASSERT_FALSE(aTextFromOldPlan.IsNull());
  EXPECT_GT(countSubShapes(aTextFromOldPlan, TopAbs_FACE), 0);
  const std::optional<BRepFont_Builder::TextPlan> aScaledPlan = aFont->PlanText("B");
  ASSERT_TRUE(aScaledPlan.has_value());
  EXPECT_FALSE(aFont->RenderText(*aScaledPlan).IsNull());

  aFont->SetSingleStrokeFont(true);
  const TopoDS_Shape aTextFromFilledPlan = aFont->RenderText(*aScaledPlan);
  ASSERT_FALSE(aTextFromFilledPlan.IsNull());
  EXPECT_GT(countSubShapes(aTextFromFilledPlan, TopAbs_FACE), 0);

  const std::optional<BRepFont_Builder::TextPlan> aStrokePlan = aFont->PlanText("B");
  ASSERT_TRUE(aStrokePlan.has_value());
  const TopoDS_Shape aStrokeText = aFont->RenderText(*aStrokePlan);
  ASSERT_FALSE(aStrokeText.IsNull());
  EXPECT_EQ(0, countSubShapes(aStrokeText, TopAbs_FACE));
  EXPECT_GT(countSubShapes(aStrokeText, TopAbs_WIRE), 0);
}

TEST(StdPrs_BRepFontTest, InvisibleTextRetainsLayoutWithoutBuildingTopology)
{
  const occ::handle<StdPrs_BRepFont> aFont = createFont();
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "FreeType and usable fonts are unavailable";
  }

  const std::optional<BRepFont_Builder::TextPlan> aPlan = aFont->PlanText("  ");
  ASSERT_TRUE(aPlan.has_value());
  EXPECT_TRUE(aPlan->Items().IsEmpty());
  EXPECT_EQ(0u, aPlan->NbRegions());
  EXPECT_GT(aPlan->Width(), 0.0);
  EXPECT_TRUE(aFont->RenderText(*aPlan).IsNull());
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
  EXPECT_FALSE(aFont->RenderGlyph(U'I').IsNull());

  ASSERT_TRUE(aFont->SetWidthScaling(1.25f));
  const TopoDS_Shape anAfter = aFont->RenderGlyph(U'I');
  EXPECT_FALSE(anAfter.IsNull());
  EXPECT_FALSE(aBefore.IsSame(anAfter));
}

TEST(StdPrs_BRepFontTest, CacheDistinguishesFontParameters)
{
  occ::handle<StdPrs_BRepFontCache>  aCache = new StdPrs_BRepFontCache(4);
  const occ::handle<StdPrs_BRepFont> aFont =
    aCache->FindFont("OCCT-Embedded-Fallback-Request-4A72C3", Font_FontAspect_Regular, 12.0);
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "FreeType and usable fonts are unavailable";
  }

  const occ::handle<StdPrs_BRepFont> aSameFont =
    aCache->FindFont("OCCT-Embedded-Fallback-Request-4A72C3", Font_FontAspect_Regular, 12.0);
  ASSERT_FALSE(aSameFont.IsNull());
  EXPECT_NE(aFont, aSameFont);
  const TopoDS_Shape aGlyph     = aFont->RenderGlyph(U'A');
  const TopoDS_Shape aSameGlyph = aSameFont->RenderGlyph(U'A');
  ASSERT_FALSE(aGlyph.IsNull());
  ASSERT_FALSE(aSameGlyph.IsNull());
  EXPECT_FALSE(aGlyph.IsPartner(aSameGlyph));
  const std::optional<BRepFont_Builder::TextPlan> aPlan     = aFont->PlanText("A");
  const std::optional<BRepFont_Builder::TextPlan> aSamePlan = aSameFont->PlanText("A");
  ASSERT_TRUE(aPlan.has_value());
  ASSERT_TRUE(aSamePlan.has_value());
  ASSERT_EQ(1u, aPlan->NbRegions());
  ASSERT_EQ(1u, aSamePlan->NbRegions());
  EXPECT_EQ(aPlan->RegionHandle(0).get(), aSamePlan->RegionHandle(0).get());
  const double                       aNextSize = std::nextafter(12.0, 13.0);
  const occ::handle<StdPrs_BRepFont> aNextFont =
    aCache->FindFont("OCCT-Embedded-Fallback-Request-4A72C3", Font_FontAspect_Regular, aNextSize);
  ASSERT_FALSE(aNextFont.IsNull());
  EXPECT_NE(aFont, aNextFont);
  EXPECT_EQ(2u, aCache->Size());

  ASSERT_TRUE(aFont->SetWidthScaling(1.25f));
  const occ::handle<StdPrs_BRepFont> aCached =
    aCache->FindFont("OCCT-Embedded-Fallback-Request-4A72C3", Font_FontAspect_Regular, 12.0);
  ASSERT_FALSE(aCached.IsNull());
  EXPECT_NE(aFont, aCached);
  const std::optional<BRepFont_Builder::TextPlan> aCachedPlan  = aCached->PlanText("AB");
  const std::optional<BRepFont_Builder::TextPlan> aChangedPlan = aFont->PlanText("AB");
  ASSERT_TRUE(aCachedPlan.has_value());
  ASSERT_TRUE(aChangedPlan.has_value());
  EXPECT_LT(aCachedPlan->Width(), aChangedPlan->Width());
  EXPECT_EQ(2u, aCache->Size());
}

TEST(StdPrs_BRepFontTest, CachesAreIndependentAndRespectCapacity)
{
  occ::handle<StdPrs_BRepFontCache> aFirstCache  = new StdPrs_BRepFontCache(2);
  occ::handle<StdPrs_BRepFontCache> aSecondCache = new StdPrs_BRepFontCache(2);

  const occ::handle<StdPrs_BRepFont> aFirst =
    aFirstCache->FindFont("OCCT-Embedded-Fallback-Request-4A72C3", Font_FontAspect_Regular, 12.0);
  if (aFirst.IsNull())
  {
    GTEST_SKIP() << "FreeType and usable fonts are unavailable";
  }
  const occ::handle<StdPrs_BRepFont> aFirstAgain =
    aFirstCache->FindFont("OCCT-Embedded-Fallback-Request-4A72C3", Font_FontAspect_Regular, 12.0);
  ASSERT_FALSE(aFirstAgain.IsNull());
  EXPECT_NE(aFirst, aFirstAgain);
  const occ::handle<StdPrs_BRepFont> anOtherSize =
    aFirstCache->FindFont("OCCT-Embedded-Fallback-Request-4A72C3", Font_FontAspect_Regular, 13.0);
  ASSERT_FALSE(anOtherSize.IsNull());
  EXPECT_NE(aFirst, anOtherSize);
  EXPECT_EQ(2u, aFirstCache->Size());

  const occ::handle<StdPrs_BRepFont> aThird =
    aFirstCache->FindFont("OCCT-Embedded-Fallback-Request-4A72C3", Font_FontAspect_Regular, 14.0);
  ASSERT_FALSE(aThird.IsNull());
  EXPECT_EQ(2u, aFirstCache->Size());
  const occ::handle<StdPrs_BRepFont> anEvicted =
    aFirstCache->FindFont("OCCT-Embedded-Fallback-Request-4A72C3", Font_FontAspect_Regular, 12.0);
  ASSERT_FALSE(anEvicted.IsNull());
  EXPECT_NE(aFirst, anEvicted);

  const occ::handle<StdPrs_BRepFont> anIndependent =
    aSecondCache->FindFont("OCCT-Embedded-Fallback-Request-4A72C3", Font_FontAspect_Regular, 12.0);
  ASSERT_FALSE(anIndependent.IsNull());
  EXPECT_NE(aFirst, anIndependent);

  aFirstCache->Clear();
  EXPECT_EQ(0u, aFirstCache->Size());
  EXPECT_TRUE(aFirst->IsValid());
  const occ::handle<StdPrs_BRepFont> aRecreated =
    aFirstCache->FindFont("OCCT-Embedded-Fallback-Request-4A72C3", Font_FontAspect_Regular, 12.0);
  ASSERT_FALSE(aRecreated.IsNull());
  EXPECT_NE(aFirst, aRecreated);

  aRecreated->Release();
  const occ::handle<StdPrs_BRepFont> aStillCached =
    aFirstCache->FindFont("OCCT-Embedded-Fallback-Request-4A72C3", Font_FontAspect_Regular, 12.0);
  ASSERT_FALSE(aStillCached.IsNull());
  EXPECT_TRUE(aStillCached->IsValid());
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
  EXPECT_FALSE(aFont->RenderGlyph(U'I').IsNull());
}

TEST(StdPrs_BRepFontTest, ReinitializationPreservesWidthScaling)
{
  const occ::handle<StdPrs_BRepFont> aFont = createFont();
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "FreeType and usable fonts are unavailable";
  }

  const std::optional<BRepFont_Builder::TextPlan> anInitialPlan = aFont->PlanText("AB");
  ASSERT_TRUE(anInitialPlan.has_value());
  ASSERT_TRUE(aFont->SetWidthScaling(1.25f));
  const std::optional<BRepFont_Builder::TextPlan> aScaledPlan = aFont->PlanText("AB");
  ASSERT_TRUE(aScaledPlan.has_value());
  EXPECT_GT(aScaledPlan->Width(), anInitialPlan->Width());

  aFont->Release();
  ASSERT_TRUE(aFont->Init("OCCT-Embedded-Fallback-Request-4A72C3", Font_FontAspect_Regular, 12.0));
  const std::optional<BRepFont_Builder::TextPlan> aReinitializedPlan = aFont->PlanText("AB");
  ASSERT_TRUE(aReinitializedPlan.has_value());
  EXPECT_DOUBLE_EQ(aScaledPlan->Width(), aReinitializedPlan->Width());
}

TEST(StdPrs_BRepFontTest, FamilyNameInitOverloadForwardsToFontLookup)
{
  StdPrs_BRepFont aFont;
  if (!aFont.Init("OCCT-Embedded-Fallback-Request-4A72C3", Font_FontAspect_Regular, 12.0))
  {
    GTEST_SKIP() << "FreeType and usable fonts are unavailable";
  }
  EXPECT_TRUE(aFont.IsValid());
  EXPECT_DOUBLE_EQ(12.0, aFont.Size());
  EXPECT_FALSE(aFont.RenderGlyph(U'A').IsNull());
}

TEST(StdPrs_BRepFontTest, ConcurrentTextRenderingRemainsUsable)
{
  const occ::handle<StdPrs_BRepFont> aFont = createFont();
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "FreeType and usable fonts are unavailable";
  }
  ASSERT_FALSE(aFont->RenderText("ABBA").IsNull());

  std::atomic<bool>                     isValid{true};
  NCollection_LinearVector<std::thread> aThreads;
  for (int aThread = 0; aThread < 4; ++aThread)
  {
    aThreads.EmplaceAppend([&aFont, &isValid]() {
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

namespace
{
class FontTest_Dimension : public PrsDim_AngleDimension
{
public:
  using PrsDim_AngleDimension::DrawArcWithText;
  using PrsDim_AngleDimension::FitTextAlignment;
  using PrsDim_AngleDimension::PrsDim_AngleDimension;
  using PrsDim_Dimension::DrawExtension;
  using PrsDim_Dimension::drawText;
  using PrsDim_Dimension::FitTextAlignmentForLinear;
  using PrsDim_Dimension::GetValueString;
};
} // namespace

Standard_DISABLE_DEPRECATION_WARNINGS

  TEST(StdPrs_BRepFontTest, CompatibilityNamesAndCompositeMode)
{
  const occ::handle<Font_BRepFont> aFont = createFont();
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "FreeType and usable fonts are unavailable";
  }
  {
    std::lock_guard<std::mutex> aLock(aFont->Mutex());
    EXPECT_TRUE(aFont->FTFont()->IsValid());
  }
  const TopoDS_Shape aDefault = aFont->RenderGlyph(U'B');
  aFont->SetCompositeCurveMode(true);
  const TopoDS_Shape aComposite = aFont->RenderGlyph(U'B');
  ASSERT_FALSE(aComposite.IsNull());
  EXPECT_LT(countSubShapes(aComposite, TopAbs_EDGE), countSubShapes(aDefault, TopAbs_EDGE));
  EXPECT_EQ(countSubShapes(aDefault, TopAbs_EDGE),
            countSubShapes(aFont->RenderGlyph(U'B', StdPrs_BRepFont::GlyphOptions{}), TopAbs_EDGE));

  Font_BRepTextBuilder aBuilder;
  const gp_Ax3         aPen(gp_Pnt(4.0, 5.0, 6.0), gp_Dir(0.0, 1.0, 0.0));
  const TopoDS_Shape   aText = aBuilder.Perform(*aFont, "BB", aPen);
  ASSERT_FALSE(aText.IsNull());
  EXPECT_TRUE(BRepCheck_Analyzer(aText).IsValid());
  EXPECT_EQ(2 * countSubShapes(aComposite, TopAbs_EDGE), countSubShapes(aText, TopAbs_EDGE));
  aFont->SetCompositeCurveMode(false);
  EXPECT_EQ(countSubShapes(aDefault, TopAbs_EDGE),
            countSubShapes(aFont->RenderGlyph(U'B'), TopAbs_EDGE));
}

TEST(StdPrs_BRepFontTest, CompatibilityBuilderPreservesFormattedLayout)
{
  const occ::handle<StdPrs_BRepFont> aFont = createFont();
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "FreeType and usable fonts are unavailable";
  }
  const occ::handle<Font_TextFormatter> aFormatter = new Font_TextFormatter();
  aFormatter->SetupAlignment(Graphic3d_HTA_CENTER, Graphic3d_VTA_CENTER);
  aFormatter->Append("AB\nBA", *aFont->FTFont());
  aFormatter->Format();
  StdPrs_BRepTextBuilder aBuilder;
  const gp_Ax3           aPen(gp_Pnt(4.0, 5.0, 6.0), gp_Dir(0.0, 1.0, 0.0));
  const TopoDS_Shape     aFormatted = aBuilder.Perform(*aFont, aFormatter, aPen);
  const TopoDS_Shape     aDirect =
    aBuilder.Perform(*aFont, "AB\nBA", aPen, Graphic3d_HTA_CENTER, Graphic3d_VTA_CENTER);
  ASSERT_FALSE(aFormatted.IsNull());
  ASSERT_FALSE(aDirect.IsNull());
  Bnd_Box aFormattedBox;
  Bnd_Box aDirectBox;
  BRepBndLib::Add(aFormatted, aFormattedBox);
  BRepBndLib::Add(aDirect, aDirectBox);
  const Bnd_Box::Limits aFormattedLimits = aFormattedBox.Get();
  const Bnd_Box::Limits aDirectLimits    = aDirectBox.Get();
  EXPECT_DOUBLE_EQ(aFormattedLimits.Xmin, aDirectLimits.Xmin);
  EXPECT_DOUBLE_EQ(aFormattedLimits.Xmax, aDirectLimits.Xmax);
  EXPECT_DOUBLE_EQ(aFormattedLimits.Ymin, aDirectLimits.Ymin);
  EXPECT_DOUBLE_EQ(aFormattedLimits.Ymax, aDirectLimits.Ymax);
  EXPECT_DOUBLE_EQ(aFormattedLimits.Zmin, aDirectLimits.Zmin);
  EXPECT_DOUBLE_EQ(aFormattedLimits.Zmax, aDirectLimits.Zmax);
  EXPECT_TRUE(aBuilder.Perform(*aFont, occ::handle<Font_TextFormatter>()).IsNull());
}

TEST(StdPrs_BRepFontTest, CompatibilityBackendMutationIsolatesCacheAndPreservesPlans)
{
  StdPrs_BRepFontCache               aCache;
  const occ::handle<StdPrs_BRepFont> aFont = aCache.FindFont("", Font_FontAspect_Regular, 12.0);
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "FreeType and usable fonts are unavailable";
  }
  const occ::handle<Font_FTFont>                  aBackend  = aFont->FTFont();
  const std::optional<BRepFont_Builder::TextPlan> anOldPlan = aFont->PlanText("AB");
  ASSERT_TRUE(anOldPlan.has_value());
  ASSERT_TRUE(aBackend->SetWidthScaling(1.5f));
  const std::optional<BRepFont_Builder::TextPlan> aNewPlan = aFont->PlanText("AB");
  ASSERT_TRUE(aNewPlan.has_value());
  EXPECT_GT(aNewPlan->Width(), anOldPlan->Width());
  EXPECT_FALSE(aFont->RenderText(*anOldPlan).IsNull());
  const occ::handle<StdPrs_BRepFont> aCached = aCache.FindFont("", Font_FontAspect_Regular, 12.0);
  ASSERT_FALSE(aCached.IsNull());
  const std::optional<BRepFont_Builder::TextPlan> aCachedPlan = aCached->PlanText("AB");
  ASSERT_TRUE(aCachedPlan.has_value());
  EXPECT_DOUBLE_EQ(anOldPlan->Width(), aCachedPlan->Width());
}

TEST(StdPrs_BRepFontTest, DimensionSubclassCompatibilityOverloads)
{
  FontTest_Dimension aDimension(gp_Pnt(10.0, 0.0, 0.0), gp_Pnt(), gp_Pnt(0.0, 10.0, 0.0));
  const occ::handle<Prs3d_DimensionAspect> anAspect = new Prs3d_DimensionAspect();
  anAspect->MakeText3d(true);
  aDimension.SetDimensionAspect(anAspect);
  aDimension.SetCustomValue(TCollection_ExtendedString("AB"));
  double                                    aLegacyWidth = 0.0;
  double                                    aWidth       = 0.0;
  occ::handle<StdPrs_BRepFont>              aFont;
  std::optional<BRepFont_Builder::TextPlan> aPlan;
  const TCollection_ExtendedString          aLegacyText = aDimension.GetValueString(aLegacyWidth);
  const TCollection_ExtendedString          aText = aDimension.GetValueString(aWidth, aFont, aPlan);
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "FreeType and usable fonts are unavailable";
  }
  EXPECT_EQ(aLegacyText, aText);
  EXPECT_DOUBLE_EQ(aLegacyWidth, aWidth);
  const double aWidthWithMargins       = aWidth + 0.2 * anAspect->TextAspect()->Height();
  int          aLegacyPosition         = 0;
  int          aPosition               = 0;
  bool         areLegacyArrowsExternal = false;
  bool         areArrowsExternal       = false;
  aDimension.FitTextAlignment(Prs3d_DTHP_Fit, aLegacyPosition, areLegacyArrowsExternal);
  aDimension.FitTextAlignment(Prs3d_DTHP_Fit, aWidthWithMargins, aPosition, areArrowsExternal);
  EXPECT_EQ(aLegacyPosition, aPosition);
  EXPECT_EQ(areLegacyArrowsExternal, areArrowsExternal);
  aLegacyPosition = aPosition = 0;
  aDimension.FitTextAlignmentForLinear(gp_Pnt(),
                                       gp_Pnt(10.0, 0.0, 0.0),
                                       false,
                                       Prs3d_DTHP_Fit,
                                       aLegacyPosition,
                                       areLegacyArrowsExternal);
  aDimension.FitTextAlignmentForLinear(gp_Pnt(),
                                       gp_Pnt(10.0, 0.0, 0.0),
                                       false,
                                       Prs3d_DTHP_Fit,
                                       aWidthWithMargins,
                                       aPosition,
                                       areArrowsExternal);
  EXPECT_EQ(aLegacyPosition, aPosition);
  EXPECT_EQ(areLegacyArrowsExternal, areArrowsExternal);

  // Check the old drawing signatures and exported symbols without a graphics context.
  using DrawText      = void (PrsDim_Dimension::*)(const occ::handle<Prs3d_Presentation>&,
                                              const gp_Pnt&,
                                              const gp_Dir&,
                                              const TCollection_ExtendedString&,
                                              int);
  using DrawExtension = void (PrsDim_Dimension::*)(const occ::handle<Prs3d_Presentation>&,
                                                   double,
                                                   const gp_Pnt&,
                                                   const gp_Dir&,
                                                   const TCollection_ExtendedString&,
                                                   double,
                                                   int,
                                                   int);
  using DrawArc       = void (PrsDim_AngleDimension::*)(const occ::handle<Prs3d_Presentation>&,
                                                  const gp_Pnt&,
                                                  const gp_Pnt&,
                                                  const gp_Pnt&,
                                                  const TCollection_ExtendedString&,
                                                  double,
                                                  int,
                                                  int);
  EXPECT_NE(nullptr, static_cast<DrawText>(&FontTest_Dimension::drawText));
  EXPECT_NE(nullptr, static_cast<DrawExtension>(&FontTest_Dimension::DrawExtension));
  EXPECT_NE(nullptr, static_cast<DrawArc>(&FontTest_Dimension::DrawArcWithText));
}

Standard_ENABLE_DEPRECATION_WARNINGS

  TEST(StdPrs_BRepFontTest, CachedWrappersKeepIndependentCurrentCharacters)
{
  StdPrs_BRepFontCache               aCache;
  const occ::handle<StdPrs_BRepFont> aFirst =
    aCache.FindFont("OCCT-Embedded-Fallback-Request-4A72C3", Font_FontAspect_Regular, 12.0);
  const occ::handle<StdPrs_BRepFont> aSecond =
    aCache.FindFont("OCCT-Embedded-Fallback-Request-4A72C3", Font_FontAspect_Regular, 12.0);
  if (aFirst.IsNull() || aSecond.IsNull())
  {
    GTEST_SKIP() << "FreeType and usable fonts are unavailable";
  }
  const double anExpectedA = aFirst->AdvanceX(U'A', U'V');
  const double anExpectedB = aSecond->AdvanceX(U'W', U'V');
  ASSERT_NE(anExpectedA, anExpectedB);
  ASSERT_FALSE(aFirst->RenderGlyph(U'A').IsNull());
  ASSERT_FALSE(aSecond->RenderGlyph(U'W').IsNull());
  EXPECT_DOUBLE_EQ(anExpectedA, aFirst->AdvanceX(U'V'));
  EXPECT_DOUBLE_EQ(anExpectedB, aSecond->AdvanceX(U'V'));
  EXPECT_FALSE(aSecond->RenderText("BA").IsNull());
  EXPECT_DOUBLE_EQ(anExpectedA, aFirst->AdvanceX(U'V'));
}

TEST(StdPrs_BRepFontTest, RenderGlyphPreservesCurrentCharacterAdvance)
{
  const occ::handle<StdPrs_BRepFont> aFont = createFont();
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "FreeType and usable fonts are unavailable";
  }
  const double anExpected = aFont->AdvanceX(U'A', U'V');
  ASSERT_GT(anExpected, 0.0);
  ASSERT_FALSE(aFont->RenderGlyph(U'A').IsNull());
  EXPECT_DOUBLE_EQ(anExpected, aFont->AdvanceX(U'V'));
  // Repeat after loading another character to cover a cached outline.
  aFont->AdvanceX(U'B', U'V');
  ASSERT_FALSE(aFont->RenderGlyph(U'A').IsNull());
  EXPECT_DOUBLE_EQ(anExpected, aFont->AdvanceX(U'V'));
}

Standard_DISABLE_DEPRECATION_WARNINGS TEST(StdPrs_BRepFontTest,
                                           BackendReinitializationRefreshesMetricsAndPreservesPlans)
{
  const occ::handle<StdPrs_BRepFont> aFont = createFont();
  if (aFont.IsNull())
  {
    GTEST_SKIP() << "FreeType is unavailable";
  }
  const occ::handle<Font_FTFont>                  aBackend  = aFont->FTFont();
  const std::optional<BRepFont_Builder::TextPlan> anOldPlan = aFont->PlanText("AB");
  ASSERT_TRUE(anOldPlan.has_value());
  ASSERT_TRUE(aBackend->Init(Font_FontMgr::EmbedFallbackFont(), "Embedded Latin", {24, 144}));
  EXPECT_DOUBLE_EQ(aFont->Size() / 48.0, aFont->Scale());
  EXPECT_DOUBLE_EQ(aFont->Scale() * aBackend->Ascender(), aFont->Ascender());
  EXPECT_DOUBLE_EQ(aFont->Scale() * aBackend->Descender(), aFont->Descender());
  EXPECT_DOUBLE_EQ(aFont->Scale() * aBackend->LineSpacing(), aFont->LineSpacing());
  EXPECT_DOUBLE_EQ(aFont->Scale() * aBackend->PointSize(), aFont->PointSize());
  ASSERT_TRUE(aFont->PlanText("AB").has_value());
  const TopoDS_Shape anOldText = aFont->RenderText(*anOldPlan);
  ASSERT_FALSE(anOldText.IsNull());
  EXPECT_TRUE(BRepCheck_Analyzer(anOldText).IsValid());
  aBackend->Release();
  EXPECT_FALSE(aFont->IsValid());
  EXPECT_DOUBLE_EQ(0.0, aFont->Ascender());
  EXPECT_FALSE(aFont->PlanText("AB").has_value());
  EXPECT_FALSE(aFont->RenderText(*anOldPlan).IsNull());
}

Standard_ENABLE_DEPRECATION_WARNINGS
